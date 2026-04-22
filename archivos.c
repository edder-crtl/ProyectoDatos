/* ============================================================
   archivos.c — Lectura y escritura de archivos TXT
   Formato ESTUDIANTES.TXT : cedula|nombre|programa
   Formato HISTORIAL.TXT   : cedula|codigo|nombre|nota|semestre
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "archivos.h"
#include "estudiantes.h"

/* Tamaño máximo de una línea del archivo */
#define MAX_LINEA 256

/* ─────────────────────────────────────────────────────────────
   Utilidad interna: elimina el '\n' final de una cadena
   ───────────────────────────────────────────────────────────── */
static void quitarSalto(char *s) {
    size_t len = strlen(s);
    if (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
        s[len - 1] = '\0';
    /* Segundo carácter para \r\n en Windows */
    len = strlen(s);
    if (len > 0 && s[len - 1] == '\r')
        s[len - 1] = '\0';
}

/* ─────────────────────────────────────────────────────────────
   cargarEstudiantes
   Lee ESTUDIANTES.TXT y construye la lista enlazada.
   Retorna número de estudiantes cargados, -1 en error.
   ───────────────────────────────────────────────────────────── */
int cargarEstudiantes(Estudiante **lista, const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        printf("[INFO] No se encontró '%s'. Se iniciará vacío.\n", ruta);
        return 0;
    }

    char linea[MAX_LINEA];
    int  cargados = 0;

    while (fgets(linea, sizeof(linea), f)) {
        quitarSalto(linea);
        if (strlen(linea) < 3) continue; /* línea vacía */

        char cedula[MAX_CEDULA]     = "";
        char nombre[MAX_NOMBRE_EST] = "";
        char programa[MAX_PROGRAMA] = "";

        /* Tokenizar por '|' */
        char *tok = strtok(linea, "|");
        if (tok) strncpy(cedula,   tok, MAX_CEDULA   - 1);
        tok = strtok(NULL, "|");
        if (tok) strncpy(nombre,   tok, MAX_NOMBRE_EST - 1);
        tok = strtok(NULL, "|");
        if (tok) strncpy(programa, tok, MAX_PROGRAMA - 1);

        if (strlen(cedula) == 0) continue;

        Estudiante *nuevo = crearEstudiante(cedula, nombre, programa);
        if (nuevo) {
            insertarEstudiante(lista, nuevo);
            cargados++;
        }
    }
    fclose(f);
    printf("[OK] %d estudiante(s) cargado(s) desde '%s'.\n", cargados, ruta);
    return cargados;
}

/* ─────────────────────────────────────────────────────────────
   cargarHistorial
   Lee HISTORIAL.TXT y asocia asignaturas a estudiantes.
   Retorna número de registros cargados, -1 en error.
   ───────────────────────────────────────────────────────────── */
int cargarHistorial(Estudiante *lista, const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        printf("[INFO] No se encontró '%s'. Sin historial previo.\n", ruta);
        return 0;
    }

    char linea[MAX_LINEA];
    int  cargados = 0, omitidos = 0;

    while (fgets(linea, sizeof(linea), f)) {
        quitarSalto(linea);
        if (strlen(linea) < 5) continue;

        char  cedula[MAX_CEDULA]    = "";
        char  codigo[MAX_CODIGO]    = "";
        char  nombre[MAX_NOMBRE_ASIG] = "";
        float nota     = 0.0f;
        int   semestre = 0;

        char *tok = strtok(linea, "|");
        if (tok) strncpy(cedula, tok, MAX_CEDULA    - 1);
        tok = strtok(NULL, "|");
        if (tok) strncpy(codigo, tok, MAX_CODIGO    - 1);
        tok = strtok(NULL, "|");
        if (tok) strncpy(nombre, tok, MAX_NOMBRE_ASIG - 1);
        tok = strtok(NULL, "|");
        if (tok) nota = atof(tok);
        tok = strtok(NULL, "|");
        if (tok) semestre = atoi(tok);

        Estudiante *est = buscarEstudiante(lista, cedula);
        if (!est) {
            fprintf(stderr, "[AVISO] Cédula '%s' en historial sin estudiante. Omitido.\n",
                    cedula);
            omitidos++;
            continue;
        }
        Asignatura *nueva = crearAsignatura(codigo, nombre, nota, semestre);
        if (nueva) {
            agregarAsignatura(est, nueva);
            cargados++;
        }
    }
    fclose(f);
    printf("[OK] %d asignatura(s) cargada(s) desde '%s'.", cargados, ruta);
    if (omitidos) printf(" (%d omitida(s))", omitidos);
    printf("\n");
    return cargados;
}

/* ─────────────────────────────────────────────────────────────
   guardarEstudiantes
   Sobreescribe ESTUDIANTES.TXT con la lista actual.
   Retorna 0 en éxito, -1 en error.
   ───────────────────────────────────────────────────────────── */
int guardarEstudiantes(const Estudiante *lista, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) {
        fprintf(stderr, "[ERROR] No se pudo abrir '%s' para escritura.\n", ruta);
        return -1;
    }
    const Estudiante *e = lista;
    int guardados = 0;
    while (e) {
        fprintf(f, "%s|%s|%s\n", e->cedula, e->nombre, e->programa);
        guardados++;
        e = e->sig;
    }
    fclose(f);
    printf("[OK] %d estudiante(s) guardado(s) en '%s'.\n", guardados, ruta);
    return 0;
}

/* ─────────────────────────────────────────────────────────────
   guardarHistorial
   Sobreescribe HISTORIAL.TXT con todas las asignaturas actuales.
   Retorna 0 en éxito, -1 en error.
   ───────────────────────────────────────────────────────────── */
int guardarHistorial(const Estudiante *lista, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) {
        fprintf(stderr, "[ERROR] No se pudo abrir '%s' para escritura.\n", ruta);
        return -1;
    }
    const Estudiante *e = lista;
    int guardados = 0;
    while (e) {
        const Asignatura *m = e->materias;
        while (m) {
            fprintf(f, "%s|%s|%s|%.2f|%d\n",
                    e->cedula, m->codigo, m->nombre, m->nota, m->semestre);
            guardados++;
            m = m->sig;
        }
        e = e->sig;
    }
    fclose(f);
    printf("[OK] %d registro(s) de historial guardado(s) en '%s'.\n", guardados, ruta);
    return 0;
}
