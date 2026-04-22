#ifndef REPORTES_H
#define REPORTES_H

/* ============================================================
   reportes.h — Generación de reportes exportables a archivo
   ============================================================ */

#include "tipos.h"

void exportarReporteIndividual(const Estudiante *est, const char *ruta);
void exportarReporteGrupal(const Estudiante *lista, const char *ruta);
void exportarBoletinSemestral(const Estudiante *est,
                               int semestre,
                               const char *ruta);

#endif /* REPORTES_H */
