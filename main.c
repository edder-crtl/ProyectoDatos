/* ============================================================
   main.c — Sistema de Historial Académico Universitario
   Punto de entrada y menú interactivo principal
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"
#include "estudiantes.h"
#include "archivos.h"
#include "reportes.h"

/* ── Prototipo de funciones del menú ── */
static void menuPrincipal(void);
static void menuAgregarEstudiante(Estudiante **lista);
static void menuAgregarAsignatura(Estudiante *lista);
static void menuConsultarEstudiante(Estudiante *lista);
static void menuMateriasRepetidas(Estudiante *lista);
static void menuOrdenar(Estudiante *lista);
static void menuExportar(Estudiante *lista);
static void menuBoletinSemestral(Estudiante *lista);

/* ── Utilidades de entrada ── */
static void leerLinea(char *destino, int max, const char *prompt);
static int  leerEntero(const char *prompt);
static float leerFloat(const char *prompt);
static void limpiarBuffer(void);

/* ============================================================
   FUNCIÓN PRINCIPAL
   ============================================================ */
int main(void) {
    Estudiante *lista = NULL;

    printf("\n╔════════════════════════════════════════════╗\n");
    printf("║  SISTEMA DE HISTORIAL ACADÉMICO            ║\n");
    printf("║  Universidad — Gestión de Notas            ║\n");
    printf("╚════════════════════════════════════════════╝\n\n");

    /* ── Carga inicial desde archivos ── */
    printf("Cargando datos...\n");
    cargarEstudiantes(&lista, ARCHIVO_EST);
    cargarHistorial(lista, ARCHIVO_HIST);
    printf("\n");

    /* ── Ciclo del menú ── */
    menuPrincipal();

    /* Menú principal */
    int opcion;
    do {
        printf("\n╔════════════ MENÚ PRINCIPAL ════════════╗\n");
        printf("║  1. Agregar nuevo estudiante            ║\n");
        printf("║  2. Agregar asignatura a estudiante     ║\n");
        printf("║  3. Consultar historial de estudiante   ║\n");
        printf("║  4. Ver todos los estudiantes           ║\n");
        printf("║  5. Detectar materias repetidas         ║\n");
        printf("║  6. Ordenar materias de un estudiante   ║\n");
        printf("║  7. Exportar reportes                   ║\n");
        printf("║  8. Boletín semestral                   ║\n");
        printf("║  0. Guardar y salir                     ║\n");
        printf("╚════════════════════════════════════════╝\n");

        opcion = leerEntero("Seleccione una opción: ");

        switch (opcion) {
            case 1: menuAgregarEstudiante(&lista);    break;
            case 2: menuAgregarAsignatura(lista);     break;
            case 3: menuConsultarEstudiante(lista);   break;
            case 4:
                printf("\n  Lista de estudiantes registrados:\n");
                mostrarTodosEstudiantes(lista);
                break;
            case 5: menuMateriasRepetidas(lista);     break;
            case 6: menuOrdenar(lista);               break;
            case 7: menuExportar(lista);              break;
            case 8: menuBoletinSemestral(lista);      break;
            case 0:
                printf("\nGuardando datos...\n");
                guardarEstudiantes(lista, ARCHIVO_EST);
                guardarHistorial(lista, ARCHIVO_HIST);
                printf("¡Hasta luego!\n\n");
                break;
            default:
                printf("[!] Opción inválida. Intente de nuevo.\n");
        }
    } while (opcion != 0);

    /* ── Liberar toda la memoria dinámica ── */
    liberarEstudiantes(&lista);
    return 0;
}

/* ============================================================
   IMPLEMENTACIÓN DE SUBMENÚS
   ============================================================ */

static void menuPrincipal(void) {
    /* Función de bienvenida, sin lógica adicional */
    printf("  Sistema listo. Datos cargados correctamente.\n");
}

/* ── 1. Agregar nuevo estudiante ── */
static void menuAgregarEstudiante(Estudiante **lista) {
    char cedula[MAX_CEDULA], nombre[MAX_NOMBRE_EST], programa[MAX_PROGRAMA];

    printf("\n--- Agregar Estudiante ---\n");
    leerLinea(cedula,   MAX_CEDULA,     "  Cédula      : ");
    leerLinea(nombre,   MAX_NOMBRE_EST, "  Nombre      : ");
    leerLinea(programa, MAX_PROGRAMA,   "  Programa    : ");

    if (strlen(cedula) == 0) {
        printf("[!] La cédula no puede estar vacía.\n");
        return;
    }
    Estudiante *nuevo = crearEstudiante(cedula, nombre, programa);
    if (nuevo) {
        insertarEstudiante(lista, nuevo);
        printf("[OK] Estudiante '%s' registrado.\n", nombre);
    }
}

/* ── 2. Agregar asignatura ── */
static void menuAgregarAsignatura(Estudiante *lista) {
    char cedula[MAX_CEDULA];
    printf("\n--- Agregar Asignatura ---\n");
    leerLinea(cedula, MAX_CEDULA, "  Cédula del estudiante: ");

    Estudiante *est = buscarEstudiante(lista, cedula);
    if (!est) {
        printf("[!] No se encontró el estudiante con cédula '%s'.\n", cedula);
        return;
    }
    printf("  Estudiante: %s\n", est->nombre);

    char  codigo[MAX_CODIGO], nombre[MAX_NOMBRE_ASIG];
    leerLinea(codigo, MAX_CODIGO,    "  Código asignatura: ");
    leerLinea(nombre, MAX_NOMBRE_ASIG, "  Nombre asignatura: ");
    float nota     = leerFloat("  Nota (0.0 – 5.0)  : ");
    int   semestre = leerEntero("  Semestre          : ");

    if (semestre < 1 || semestre > 12) {
        printf("[!] Semestre inválido (%d). Debe estar entre 1 y 12.\n", semestre);
        return;
    }

    Asignatura *nueva = crearAsignatura(codigo, nombre, nota, semestre);
    if (nueva) {
        agregarAsignatura(est, nueva);
        printf("[OK] Asignatura '%s' agregada a %s.\n", nombre, est->nombre);
    }
}

/* ── 3. Consultar historial ── */
static void menuConsultarEstudiante(Estudiante *lista) {
    char cedula[MAX_CEDULA];
    printf("\n--- Consultar Historial ---\n");
    leerLinea(cedula, MAX_CEDULA, "  Cédula: ");

    Estudiante *est = buscarEstudiante(lista, cedula);
    mostrarHistorialEstudiante(est);
}

/* ── 5. Materias repetidas ── */
static void menuMateriasRepetidas(Estudiante *lista) {
    char cedula[MAX_CEDULA];
    printf("\n--- Materias Repetidas ---\n");
    printf("  (Dejar vacío para revisar todos los estudiantes)\n");
    leerLinea(cedula, MAX_CEDULA, "  Cédula: ");

    if (strlen(cedula) == 0) {
        /* Revisar todos */
        Estudiante *e = lista;
        while (e) {
            mostrarMateriasRepetidas(e);
            e = e->sig;
        }
    } else {
        Estudiante *est = buscarEstudiante(lista, cedula);
        if (!est) {
            printf("[!] Estudiante no encontrado.\n");
            return;
        }
        mostrarMateriasRepetidas(est);
    }
}

/* ── 6. Ordenar materias ── */
static void menuOrdenar(Estudiante *lista) {
    char cedula[MAX_CEDULA];
    printf("\n--- Ordenar Materias ---\n");
    leerLinea(cedula, MAX_CEDULA, "  Cédula: ");

    Estudiante *est = buscarEstudiante(lista, cedula);
    if (!est) {
        printf("[!] Estudiante no encontrado.\n");
        return;
    }

    printf("  1. Ordenar por nota\n");
    printf("  2. Ordenar por semestre\n");
    int op = leerEntero("  Opción: ");

    if (op == 1) {
        ordenarMateriasPorNota(est);
        printf("[OK] Materias ordenadas por nota.\n");
    } else if (op == 2) {
        ordenarMateriasPorSemestre(est);
        printf("[OK] Materias ordenadas por semestre.\n");
    } else {
        printf("[!] Opción inválida.\n");
    }
    mostrarHistorialEstudiante(est);
}

/* ── 7. Exportar reportes ── */
static void menuExportar(Estudiante *lista) {
    printf("\n--- Exportar Reporte ---\n");
    printf("  1. Reporte individual\n");
    printf("  2. Reporte grupal\n");
    int op = leerEntero("  Opción: ");

    if (op == 1) {
        char cedula[MAX_CEDULA];
        leerLinea(cedula, MAX_CEDULA, "  Cédula: ");
        Estudiante *est = buscarEstudiante(lista, cedula);
        if (!est) {
            printf("[!] Estudiante no encontrado.\n");
            return;
        }
        char ruta[80];
        snprintf(ruta, sizeof(ruta), "REPORTE_%s.TXT", cedula);
        exportarReporteIndividual(est, ruta);
    } else if (op == 2) {
        exportarReporteGrupal(lista, "REPORTE_GRUPAL.TXT");
    } else {
        printf("[!] Opción inválida.\n");
    }
}

/* ── 8. Boletín semestral (extensión) ── */
static void menuBoletinSemestral(Estudiante *lista) {
    char cedula[MAX_CEDULA];
    printf("\n--- Boletín Semestral ---\n");
    leerLinea(cedula, MAX_CEDULA, "  Cédula: ");

    Estudiante *est = buscarEstudiante(lista, cedula);
    if (!est) {
        printf("[!] Estudiante no encontrado.\n");
        return;
    }
    int semestre = leerEntero("  Semestre: ");
    char ruta[80];
    snprintf(ruta, sizeof(ruta), "BOLETIN_%s_SEM%d.TXT", cedula, semestre);
    exportarBoletinSemestral(est, semestre, ruta);
}

/* ============================================================
    UTILIDADES DE ENTRADA SEGURA
   ============================================================ */

/**
 * Lee una línea desde stdin con límite de tamaño.
 * Elimina el salto de línea final.
 */
static void leerLinea(char *destino, int max, const char *prompt) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(destino, max, stdin)) {
        destino[0] = '\0';
        return;
    }
    /* Quitar '\n' */
    size_t len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') destino[len - 1] = '\0';
}

/**
 * Lee un entero validado desde stdin.
 */
static int leerEntero(const char *prompt) {
    char buf[32];
    leerLinea(buf, sizeof(buf), prompt);
    return atoi(buf);
}

/**
 * Lee un float validado desde stdin.
 */
static float leerFloat(const char *prompt) {
    char buf[32];
    leerLinea(buf, sizeof(buf), prompt);
    return (float)atof(buf);
}

/**
 * Descarta caracteres restantes en el buffer de entrada.
 */
static void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
