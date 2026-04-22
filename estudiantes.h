#ifndef ESTUDIANTES_H
#define ESTUDIANTES_H

/* ============================================================
   estudiantes.h — Operaciones sobre la lista de estudiantes
   ============================================================ */

#include "tipos.h"

/* ── Gestión de la lista de estudiantes ── */
Estudiante *crearEstudiante(const char *cedula,
                            const char *nombre,
                            const char *programa);

void        insertarEstudiante(Estudiante **lista, Estudiante *nuevo);
Estudiante *buscarEstudiante(Estudiante *lista, const char *cedula);
void        liberarEstudiantes(Estudiante **lista);

/* ── Gestión de asignaturas ── */
Asignatura *crearAsignatura(const char *codigo,
                            const char *nombre,
                            float nota,
                            int semestre);

void        agregarAsignatura(Estudiante *est, Asignatura *nueva);
void        liberarAsignaturas(Asignatura **lista);

/* ── Análisis y consulta ── */
float calcularPromedio(const Estudiante *est);
int   contarMaterias(const Estudiante *est);
int   estaRepetida(const Asignatura *lista,
                   const char *codigo,
                   float notaActual);   /* 1 si ya aprobó con >=NOTA_APROBACION */

void  mostrarHistorialEstudiante(const Estudiante *est);
void  mostrarMateriasRepetidas(const Estudiante *est);
void  mostrarTodosEstudiantes(const Estudiante *lista);
float calcularPorcentajeAprobacion(const Estudiante *est);

/* ── Ordenamiento ── */
void  ordenarMateriasPorNota(Estudiante *est);
void  ordenarMateriasPorSemestre(Estudiante *est);

#endif /* ESTUDIANTES_H */
