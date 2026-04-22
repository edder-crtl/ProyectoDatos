#ifndef TIPOS_H
#define TIPOS_H

/* ============================================================
   tipos.h — Definición de estructuras de datos principales
   Sistema de Historial Académico Universitario
   ============================================================ */

#define MAX_NOMBRE_ASIG  31
#define MAX_CODIGO       11
#define MAX_CEDULA       11
#define MAX_NOMBRE_EST   41
#define MAX_PROGRAMA     31
#define ARCHIVO_EST      "ESTUDIANTES.TXT"
#define ARCHIVO_HIST     "HISTORIAL.TXT"
#define NOTA_APROBACION  3.0f

/* ── Nodo de lista enlazada simple para asignaturas ── */
typedef struct Asignatura {
    char  nombre[MAX_NOMBRE_ASIG]; /* Ej: "Cálculo I"          */
    char  codigo[MAX_CODIGO];      /* Ej: "MAT101"              */
    int   semestre;                /* Semestre en que se cursó  */
    float nota;                    /* Calificación (0.0 – 5.0)  */
    struct Asignatura *sig;        /* Puntero al siguiente nodo */
} Asignatura;

/* ── Nodo de lista enlazada simple para estudiantes ── */
typedef struct Estudiante {
    char       cedula[MAX_CEDULA];    /* Identificación única     */
    char       nombre[MAX_NOMBRE_EST];/* Nombre completo          */
    char       programa[MAX_PROGRAMA];/* Programa académico       */
    Asignatura *materias;             /* Lista de asignaturas     */
    struct Estudiante *sig;           /* Puntero al siguiente     */
} Estudiante;

#endif /* TIPOS_H */
