#	Fichero: Makefile
#	Autor: Cesar Aguilera <cesarap at correo.ugr.es>
#	Construye el ejecutable para el juego xlib-hanoi

#MACROS
CC = g++
BIN = ./bin/
SRC = ./src/
OBJ = ./obj/
INCLUDE = ./include
DOC = ./doc/

#OPCIONES COMPILACION
_C_WALL_I= -c -Wall -I $(INCLUDE)
_I_XLIB = -I/usr/X11R6/include
_L_XLIB = -lX11 -L/usr/X11R6/lib
_OBJECT = -o
_C_WALL = -c -Wall


all: $(BIN)xlib-hanoi
doc: $(INCLUDE)/*.h
	@doxygen $(DOC)doxys/Doxyfile

	
$(OBJ)utils.o: $(SRC)utils.cpp $(INCLUDE)/utils.h $(INCLUDE)/constants.h
	@echo "Compilando ficheros fuente..."
	$(CC) $(_C_WALL_I) $(SRC)utils.cpp $(_OBJECT) $(OBJ)utils.o
	
$(OBJ)main.o: $(SRC)main.cpp $(OBJ)utils.o $(INCLUDE)/icon.h
	$(CC) $(_C_WALL) $(SRC)main.cpp $(_OBJECT) $(OBJ)main.o $(_I_XLIB)
	@echo Hecho.

$(BIN)xlib-hanoi: $(OBJ)main.o
	@echo "Creando ejecutable de xlib-hanoi..."
	$(CC) $(OBJ)main.o $(OBJ)utils.o $(_OBJECT) $(BIN)xlib-hanoi $(_L_XLIB)
	@echo "Hecho." 
	@echo  "Ejecutable xlib-hanoi creado satisfactoriamente en el directorio ./bin/ ."
	@echo "Si desea limpiar la instalacion efectue \"make clean\"."


clean:
	@echo "Limpiando la instalacion..."
	\rm -rf $(OBJ)*.o $(SRC)*~ $(INCLUDE)/*~
	@echo "Hecho."
	@echo "Si desea borrar el ejetuable efectue \"make mrproper\"."

mrproper:
	@echo "Eliminando ejecutale..."
	\rm -rf $(BIN)*
	@echo "Hecho."

