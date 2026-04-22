/* ============================================================
   estudiantes.c — Implementación de operaciones académicas
   Sistema de Historial Académico Universitario
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estudiantes.h"

/* ─────────────────────────────────────────────────────────────
   GESTIÓN DE ESTUDIANTES
   ───────────────────────────────────────────────────────────── */

/**
 * Crea un nuevo nodo Estudiante con memoria dinámica.
 * Retorna puntero al nodo o NULL si falla malloc.
 */
Estudiante *crearEstudiante(const char *cedula,
                             const char *nombre,
                             const char *programa) {
    Estudiante *nuevo = (Estudiante *)malloc(sizeof(Estudiante));
    if (!nuevo) {
        fprintf(stderr, "[ERROR] Sin memoria para crear estudiante.\n");
        return NULL;
    }
    strncpy(nuevo->cedula,   cedula,   MAX_CEDULA   - 1);
    strncpy(nuevo->nombre,   nombre,   MAX_NOMBRE_EST - 1);
    strncpy(nuevo->programa, programa, MAX_PROGRAMA - 1);
    nuevo->cedula[MAX_CEDULA - 1]     = '\0';
    nuevo->nombre[MAX_NOMBRE_EST - 1] = '\0';
    nuevo->programa[MAX_PROGRAMA - 1] = '\0';
    nuevo->materias = NULL;
    nuevo->sig      = NULL;
    return nuevo;
}

/**
 * Inserta un estudiante al final de la lista enlazada.
 * Si ya existe la cédula, no inserta duplicado.
 */
void insertarEstudiante(Estudiante **lista, Estudiante *nuevo) {
    if (!nuevo) return;

    /* Verificar duplicado */
    if (buscarEstudiante(*lista, nuevo->cedula)) {
        printf("[AVISO] Estudiante con cédula %s ya existe.\n", nuevo->cedula);
        free(nuevo);
        return;
    }

    if (!*lista) {
        *lista = nuevo;
        return;
    }
    Estudiante *aux = *lista;
    while (aux->sig) aux = aux->sig;
    aux->sig = nuevo;
}

/**
 * Busca un estudiante por cédula (búsqueda lineal O(n)).
 * Retorna puntero al nodo o NULL si no existe.
 */
Estudiante *buscarEstudiante(Estudiante *lista, const char *cedula) {
    while (lista) {
        if (strcmp(lista->cedula, cedula) == 0) return lista;
        lista = lista->sig;
    }
    return NULL;
}

/**
 * Libera toda la lista de estudiantes y sus asignaturas.
 */
void liberarEstudiantes(Estudiante **lista) {
    Estudiante *actual = *lista;
    while (actual) {
        Estudiante *siguiente = actual->sig;
        liberarAsignaturas(&actual->materias);
        free(actual);
        actual = siguiente;
    }
    *lista = NULL;
}

/* ─────────────────────────────────────────────────────────────
   GESTIÓN DE ASIGNATURAS
   ───────────────────────────────────────────────────────────── */

/**
 * Crea un nuevo nodo Asignatura con memoria dinámica.
 */
Asignatura *crearAsignatura(const char *codigo,
                             const char *nombre,
                             float nota,
                             int semestre) {
    if (nota < 0.0f || nota > 5.0f) {
        printf("[AVISO] Nota %.2f fuera de rango (0.0–5.0). Se ajustará.\n", nota);
        nota = (nota < 0.0f) ? 0.0f : 5.0f;
    }
    Asignatura *nueva = (Asignatura *)malloc(sizeof(Asignatura));
    if (!nueva) {
        fprintf(stderr, "[ERROR] Sin memoria para crear asignatura.\n");
        return NULL;
    }
    strncpy(nueva->codigo, codigo, MAX_CODIGO       - 1);
    strncpy(nueva->nombre, nombre, MAX_NOMBRE_ASIG  - 1);
    nueva->codigo[MAX_CODIGO - 1]      = '\0';
    nueva->nombre[MAX_NOMBRE_ASIG - 1] = '\0';
    nueva->nota      = nota;
    nueva->semestre  = semestre;
    nueva->sig       = NULL;
    return nueva;
}

/**
 * Agrega una asignatura al final de la lista de materias
 * del estudiante dado.
 */
void agregarAsignatura(Estudiante *est, Asignatura *nueva) {
    if (!est || !nueva) return;
    if (!est->materias) {
        est->materias = nueva;
        return;
    }
    Asignatura *aux = est->materias;
    while (aux->sig) aux = aux->sig;
    aux->sig = nueva;
}

/**
 * Libera toda la lista de asignaturas de un estudiante.
 */
void liberarAsignaturas(Asignatura **lista) {
    Asignatura *actual = *lista;
    while (actual) {
        Asignatura *siguiente = actual->sig;
        free(actual);
        actual = siguiente;
    }
    *lista = NULL;
}

/* ─────────────────────────────────────────────────────────────
   ANÁLISIS Y CONSULTA
   ───────────────────────────────────────────────────────────── */

/**
 * Calcula el promedio simple de todas las notas del estudiante.
 * Retorna 0.0 si no tiene materias.
 */
float calcularPromedio(const Estudiante *est) {
    if (!est || !est->materias) return 0.0f;
    float suma  = 0.0f;
    int   total = 0;
    const Asignatura *m = est->materias;
    while (m) {
        suma += m->nota;
        total++;
        m = m->sig;
    }
    return (total > 0) ? suma / total : 0.0f;
}

/**
 * Cuenta el total de registros de asignaturas del estudiante
 * (incluyendo repetidas).
 */
int contarMaterias(const Estudiante *est) {
    int n = 0;
    const Asignatura *m = est ? est->materias : NULL;
    while (m) { n++; m = m->sig; }
    return n;
}

/**
 * Determina si una asignatura (por código) aparece antes en la lista
 * con una nota igual o superior a NOTA_APROBACION.
 * Se usa para detectar repetición innecesaria.
 *
 * Parámetros:
 *   lista      — inicio de la lista de asignaturas
 *   codigo     — código a buscar
 *   notaActual — nota del registro que se está evaluando
 *
 * Retorna 1 si la materia está repetida con aprobación previa, 0 si no.
 */
int estaRepetida(const Asignatura *lista,
                 const char *codigo,
                 float notaActual) {
    int apariciones = 0;
    while (lista) {
        if (strcmp(lista->codigo, codigo) == 0) {
            apariciones++;
            /* Si ya existe un registro aprobado, la repetición es innecesaria */
            if (lista->nota >= NOTA_APROBACION && notaActual >= NOTA_APROBACION)
                return 1;
        }
        lista = lista->sig;
    }
    return (apariciones > 1) ? 1 : 0;
}

/**
 * Muestra el historial completo de un estudiante en consola.
 */
void mostrarHistorialEstudiante(const Estudiante *est) {
    if (!est) {
        printf("  [!] Estudiante no encontrado.\n");
        return;
    }
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  HISTORIAL ACADÉMICO                                 ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    printf("  Cédula  : %s\n", est->cedula);
    printf("  Nombre  : %s\n", est->nombre);
    printf("  Programa: %s\n", est->programa);
    printf("  Promedio: %.2f\n", calcularPromedio(est));
    printf("  Aprob.  : %.1f%%\n", calcularPorcentajeAprobacion(est));
    printf("──────────────────────────────────────────────────────\n");
    printf("  %-10s %-30s %4s  %5s\n",
           "Código", "Asignatura", "Sem.", "Nota");
    printf("  %-10s %-30s %4s  %5s\n",
           "----------", "------------------------------", "----", "-----");

    const Asignatura *m = est->materias;
    if (!m) { printf("  (sin materias registradas)\n"); }
    while (m) {
        char estado = (m->nota >= NOTA_APROBACION) ? ' ' : '*';
        printf("  %-10s %-30s %4d  %4.1f %c\n",
               m->codigo, m->nombre, m->semestre, m->nota, estado);
        m = m->sig;
    }
    printf("  (* = reprobada)\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
}

/**
 * Lista únicamente las materias que el estudiante ha cursado más de una vez.
 */
void mostrarMateriasRepetidas(const Estudiante *est) {
    if (!est) return;
    printf("\n  Materias repetidas de %s:\n", est->nombre);

    int encontradas = 0;
    const Asignatura *i = est->materias;
    while (i) {
        /* Contar cuántas veces aparece este código */
        int veces = 0;
        const Asignatura *j = est->materias;
        while (j) {
            if (strcmp(j->codigo, i->codigo) == 0) veces++;
            j = j->sig;
        }
        if (veces > 1) {
            /* Imprimir sólo la primera vez que se detecta */
            int yaImpreso = 0;
            const Asignatura *k = est->materias;
            while (k != i) {
                if (strcmp(k->codigo, i->codigo) == 0) { yaImpreso = 1; break; }
                k = k->sig;
            }
            if (!yaImpreso) {
                printf("    → %-10s %-30s (%d cursadas)\n",
                       i->codigo, i->nombre, veces);
                encontradas++;
            }
        }
        i = i->sig;
    }
    if (!encontradas) printf("    (ninguna materia repetida)\n");
}

/**
 * Muestra un resumen de todos los estudiantes registrados.
 */
void mostrarTodosEstudiantes(const Estudiante *lista) {
    printf("\n  %-12s %-40s %-30s %7s\n",
           "Cédula", "Nombre", "Programa", "Promedio");
    printf("  %-12s %-40s %-30s %7s\n",
           "------------", "----------------------------------------",
           "------------------------------", "-------");
    int n = 0;
    while (lista) {
        printf("  %-12s %-40s %-30s %7.2f\n",
               lista->cedula, lista->nombre,
               lista->programa, calcularPromedio(lista));
        n++;
        lista = lista->sig;
    }
    printf("  Total estudiantes: %d\n", n);
}

/**
 * Calcula el porcentaje de asignaturas aprobadas sobre el total cursado.
 */
float calcularPorcentajeAprobacion(const Estudiante *est) {
    if (!est || !est->materias) return 0.0f;
    int total = 0, aprobadas = 0;
    const Asignatura *m = est->materias;
    while (m) {
        total++;
        if (m->nota >= NOTA_APROBACION) aprobadas++;
        m = m->sig;
    }
    return (total > 0) ? (aprobadas * 100.0f / total) : 0.0f;
}

/* ─────────────────────────────────────────────────────────────
   ORDENAMIENTO (extensión opcional)
   — Bubble sort sobre lista enlazada intercambiando datos —
   ───────────────────────────────────────────────────────────── */

/** Intercambia los datos (no los punteros) de dos nodos. */
static void intercambiarDatos(Asignatura *a, Asignatura *b) {
    Asignatura tmp = *a;
    /* Preservar punteros originales */
    struct Asignatura *sigA = a->sig;
    struct Asignatura *sigB = b->sig;
    *a = *b;
    *b = tmp;
    a->sig = sigA;
    b->sig = sigB;
}

/**
 * Ordena la lista de materias de un estudiante por nota (ascendente).
 */
void ordenarMateriasPorNota(Estudiante *est) {
    if (!est || !est->materias) return;
    int intercambio;
    do {
        intercambio = 0;
        Asignatura *cur = est->materias;
        while (cur && cur->sig) {
            if (cur->nota > cur->sig->nota) {
                intercambiarDatos(cur, cur->sig);
                intercambio = 1;
            }
            cur = cur->sig;
        }
    } while (intercambio);
}

/**
 * Ordena la lista de materias de un estudiante por semestre (ascendente).
 */
void ordenarMateriasPorSemestre(Estudiante *est) {
    if (!est || !est->materias) return;
    int intercambio;
    do {
        intercambio = 0;
        Asignatura *cur = est->materias;
        while (cur && cur->sig) {
            if (cur->semestre > cur->sig->semestre) {
                intercambiarDatos(cur, cur->sig);
                intercambio = 1;
            }
            cur = cur->sig;
        }
    } while (intercambio);
}
