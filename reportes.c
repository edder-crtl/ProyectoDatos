/* ============================================================
   reportes.c — Exportación de informes académicos a archivos
   ============================================================ */

#include <stdio.h>
#include <string.h>
#include "reportes.h"
#include "estudiantes.h"

/* Separador visual reutilizable */
#define SEP "======================================================\n"
#define SEP2 "------------------------------------------------------\n"

/* ─────────────────────────────────────────────────────────────
   exportarReporteIndividual
   Genera un archivo TXT con el historial completo de un estudiante.
   ───────────────────────────────────────────────────────────── */
void exportarReporteIndividual(const Estudiante *est, const char *ruta) {
    if (!est) {
        printf("[ERROR] Estudiante nulo, no se puede exportar.\n");
        return;
    }
    FILE *f = fopen(ruta, "w");
    if (!f) {
        fprintf(stderr, "[ERROR] No se pudo crear '%s'.\n", ruta);
        return;
    }

    fprintf(f, SEP);
    fprintf(f, "  REPORTE ACADÉMICO INDIVIDUAL\n");
    fprintf(f, SEP);
    fprintf(f, "  Cédula   : %s\n", est->cedula);
    fprintf(f, "  Nombre   : %s\n", est->nombre);
    fprintf(f, "  Programa : %s\n", est->programa);
    fprintf(f, "  Promedio : %.2f / 5.00\n", calcularPromedio(est));
    fprintf(f, "  Aprob.   : %.1f%%\n", calcularPorcentajeAprobacion(est));
    fprintf(f, "  Materias : %d\n", contarMaterias(est));
    fprintf(f, SEP2);
    fprintf(f, "  %-10s %-30s %4s  %5s  %s\n",
            "Código", "Asignatura", "Sem.", "Nota", "Estado");
    fprintf(f, "  %-10s %-30s %4s  %5s  %s\n",
            "----------", "------------------------------",
            "----", "-----", "-------");

    const Asignatura *m = est->materias;
    while (m) {
        const char *estado = (m->nota >= NOTA_APROBACION) ? "Aprobó" : "Reprobó";
        fprintf(f, "  %-10s %-30s %4d  %5.2f  %s\n",
                m->codigo, m->nombre, m->semestre, m->nota, estado);
        m = m->sig;
    }
    fprintf(f, SEP);
    fclose(f);
    printf("[OK] Reporte exportado a '%s'.\n", ruta);
}

/* ─────────────────────────────────────────────────────────────
   exportarReporteGrupal
   Genera un archivo TXT con el resumen de todos los estudiantes.
   ───────────────────────────────────────────────────────────── */
void exportarReporteGrupal(const Estudiante *lista, const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) {
        fprintf(stderr, "[ERROR] No se pudo crear '%s'.\n", ruta);
        return;
    }

    fprintf(f, SEP);
    fprintf(f, "  REPORTE ACADÉMICO GRUPAL\n");
    fprintf(f, SEP);
    fprintf(f, "  %-12s %-40s %-28s %7s  %6s\n",
            "Cédula", "Nombre", "Programa", "Promedio", "Aprob.");
    fprintf(f, "  %-12s %-40s %-28s %7s  %6s\n",
            "------------",
            "----------------------------------------",
            "----------------------------",
            "-------", "------");

    float sumaTotal = 0.0f;
    int   total     = 0;
    const Estudiante *e = lista;
    while (e) {
        float prom = calcularPromedio(e);
        fprintf(f, "  %-12s %-40s %-28s %7.2f  %5.1f%%\n",
                e->cedula, e->nombre, e->programa,
                prom, calcularPorcentajeAprobacion(e));
        sumaTotal += prom;
        total++;
        e = e->sig;
    }
    fprintf(f, SEP2);
    if (total > 0)
        fprintf(f, "  Promedio general: %.2f   |   Total estudiantes: %d\n",
                sumaTotal / total, total);
    fprintf(f, SEP);
    fclose(f);
    printf("[OK] Reporte grupal exportado a '%s'.\n", ruta);
}

/* ─────────────────────────────────────────────────────────────
   exportarBoletinSemestral (extensión opcional)
   Exporta sólo las asignaturas de un semestre dado.
   ───────────────────────────────────────────────────────────── */
void exportarBoletinSemestral(const Estudiante *est,int semestre,const char *ruta) {
    if (!est) return;
    FILE *f = fopen(ruta, "w");
    if (!f) {
        fprintf(stderr, "[ERROR] No se pudo crear '%s'.\n", ruta);
        return;
    }

    fprintf(f, SEP);
    fprintf(f, "  BOLETÍN SEMESTRAL — Semestre %d\n", semestre);
    fprintf(f, SEP);
    fprintf(f, "  Estudiante: %s  (%s)\n", est->nombre, est->cedula);
    fprintf(f, SEP2);
    fprintf(f, "  %-10s %-30s  %5s  %s\n",
            "Código", "Asignatura", "Nota", "Estado");
    fprintf(f, "  %-10s %-30s  %5s  %s\n",
            "----------", "------------------------------", "-----", "-------");

    const Asignatura *m = est->materias;
    float suma = 0.0f;
    int   cnt  = 0;
    while (m) {
        if (m->semestre == semestre) {
            const char *estado = (m->nota >= NOTA_APROBACION)
                                  ? "Aprobó" : "Reprobó";
            fprintf(f, "  %-10s %-30s  %5.2f  %s\n",
                    m->codigo, m->nombre, m->nota, estado);
            suma += m->nota;
            cnt++;
        }
        m = m->sig;
    }
    fprintf(f, SEP2);
    if (cnt > 0)
        fprintf(f, "  Promedio del semestre: %.2f   |   Materias: %d\n",
                suma / cnt, cnt);
    else
        fprintf(f, "  Sin materias en semestre %d.\n", semestre);
    fprintf(f, SEP);
    fclose(f);
    printf("[OK] Boletín semestral exportado a '%s'.\n", ruta);
}
