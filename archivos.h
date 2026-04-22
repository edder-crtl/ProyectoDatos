#ifndef ARCHIVOS_H
#define ARCHIVOS_H

/* ============================================================
   archivos.h — Lectura y escritura de archivos de texto
   ============================================================ */

#include "tipos.h"

/* ── Carga inicial ── */
int cargarEstudiantes(Estudiante **lista, const char *ruta);
int cargarHistorial(Estudiante *lista, const char *ruta);

/* ── Persistencia al cerrar ── */
int guardarEstudiantes(const Estudiante *lista, const char *ruta);
int guardarHistorial(const Estudiante *lista, const char *ruta);

#endif /* ARCHIVOS_H */
