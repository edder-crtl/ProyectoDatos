# ============================================================
# Makefile — Sistema de Historial Académico
# Uso: make        → compila
#      make run    → compila y ejecuta
#      make clean  → elimina binarios
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g
TARGET  = historial_academico
SRCS    = main.c estudiantes.c archivos.c reportes.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "✓ Compilado: $(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "✓ Limpieza completada"

.PHONY: all run clean
