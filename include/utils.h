/**
**************************************************************
* @file utils.h
* @brief Definicion de tipos de datos, funciones y procedimientos 
* para facilitar la programacion y juego de xlib-hanoi.
* @author Cesar Aguilera <cesarap at correo.ugr.es>
* @date Noviembre 2011
**************************************************************
**/

#ifndef _XLIB_HANOI_UTILS_H_
#define _XLIB_HANOI_UTILS_H_

#include <X11/Xlib.h> // Xlib
#include <X11/Xutil.h>  // Xlib: sizehints
#include <stdlib.h> // exit
#include <cstring> // macro NULL, strlen
#include <stack> // pila STL
#include "../include/constants.h" // constantes xlib-hanoi
#include "../include/icon.h" // icono xlib-hanoi

/**
**************************************************************
* @brief Estructura para representar un movimiento.
* Se compone de dos enteros cortos sin signo que indican la
* torre de origen y la torre de destino del disco.
**************************************************************
**/

struct Movement
{
	unsigned short int orig;
	unsigned short int dest;
};

/**
**************************************************************
* @brief Estructura para representar el contexto grafico
* de los discos. Se compone del contexto grafico para dibujar
* los rectangulos y el contexto grafico para dibujar el 
* borde o contorno de los rectangulos.
**************************************************************
**/
struct DiscGC
{
	GC rectangle;
	GC border;
};

/**
**************************************************************
* @brief Devuelve el valor de pixel para el nombre de un color
* dado.
* @param display Especifica la conexion con el display X.
* @param name Cadena de caracteres C con el nombre del color.
* @return Valor de pixel que mas se aproxime al color name.
**************************************************************
**/
unsigned long int GetColorNum(Display *display, char *name);

/**
**************************************************************
* @brief Inicializa el contexto grafico de los casilleros.
* @param display Especifica la conexion con el display X.
* @param win Especifica la ventana.
* @param gc Contexto grafico de los casilleros.
**************************************************************
**/
void PrepareLockerGC(Display *display, Window& win, GC& gc);

/**
**************************************************************
* @brief Inicializa el contexto grafico de los discos.
* @param display Especifica la conexion con el display X.
* @param win Especifica la ventana.
* @param disc_gc Contexto grafico de los discos.
**************************************************************
**/
void PrepareDiscGC(Display *display, Window& win, DiscGC& disc_gc);

/**
**************************************************************
* @brief Inicializa el contexto grafico del texto.
* @param display Especifica la conexion con el display X.
* @param win Especifica la ventana.
* @param gc Contexto grafico del texto.
* @param font_info Especifica la fuente del texto.
* @return Devuelve si se ha podido establecer la fuente e
* inicializar correctamente el contexto grafico.
**************************************************************
**/
bool PrepareTextGC(Display *display, Window& win, GC& gc, XFontStruct **font_info);

/**
**************************************************************
* @brief Inicializa el tablero de juego: establece el tamanyo 
* y las coordenadas de los casilleros con respecto a la 
* ventana de juego, inicializa cada una de las pilas que 
* representan el estado de las torres de discos, 
* establece las coordenadas y tamanyo de los discos con 
* respecto al primer casillero.
* @param locker Array de XRectangle que representa los 
* casilleros.
* @param disc Array de XRectangle que representa los discos.
* @param NUM_DISC Numero de discos.
* @param tower Array de pilas de enteros que representa
* el estado de cada una de las torres de discos.
* @param table_width Ancho de la ventana de juego.
* @param table_height Alto de la ventana de juego.
* @param SEP_V Separacion entre las paredes del casillero
* y el lateral de los discos.
**************************************************************
**/
void InitHanoiTable(XRectangle locker[NUM_LOCKERS], XRectangle disc[], unsigned int NUM_DISCS, std::stack<unsigned int> tower[NUM_LOCKERS], unsigned int table_width, unsigned int table_height, unsigned int SEP_V);

/**
**************************************************************
* @brief Selecciona un disco dibujandolo de diferente color
* y con distinto borde.
* @param display Especifica la conexion con el display X.
* @param win Especifica la ventana.
* @param disc_gc Contexto grafico de los discos.
* @param disc Array de XRectangle que representa los discos.
* @param tower Array de pilas de enteros que representa
* el estado de cada una de las torres de discos.
* @param orig Pila en la que se encuentra el disco que 
* se selecciona.
**************************************************************
**/
void SelecDisc(Display *display, Window& win, DiscGC& disc_gc, XRectangle disc[], std::stack<unsigned int> tower[], unsigned short int orig);

/**
**************************************************************
* @brief Efectua un movimiento de disco, de una torre a otra.
* @param display Especifica la conexion con el display X.
* @param win Especifica la ventana.
* @param disc_gc Contexto grafico de los discos.
* @param locker Array de XRectangle que representa los 
* casilleros.
* @param disc Array de XRectangle que representa los discos.
* @param tower Array de pilas de enteros que representa
* el estado de cada una de las torres de discos.
* @param mov Movimiento a efectuar.
* @return Devuelve si ha sido posible efectuar el movimiento.
**************************************************************
**/
bool MovDisc(Display *display, Window& win, DiscGC& disc_gc, XRectangle locker[], XRectangle disc[], std::stack<unsigned int> tower[], Movement mov);

/**
**************************************************************
* @brief Dibuja un texto centrado en una ventana.
* @param display Especifica la conexion con el servidor X.
* @param win Espeficica la ventana.
* @param gc Especifica el contexto grafico del texto.
* @param font_info Especifica la fuente de texto.
* @param win_width Ancho de la ventana.
* @param win_height Alto de la ventana.
* @param text Cadena de caracteres C con el texto 
* que se dibuja.
* @param clear_area Indica si se limpia el contenido de la ventana 
* antes de dibujar el texto.
**************************************************************
**/
void ShowText(Display *display, Window& win, GC& gc, XFontStruct *font_info, unsigned int win_width, unsigned int win_height, const char text[], bool clear_area);

/**
**************************************************************
* @brief Dibuja un texto de alerta en las coordenadas (x,y) de
* una ventana.
* @param display Especifica la conexion con el servidor X.
* @param win Espeficica la ventana.
* @param gc Especifica el contexto grafico del texto.
* @param font_info Especifica la fuente de texto.
* @param x Especifica coordenada x de la ventana.
* @param y Especifica coordenada y de la ventana.
* @param text Cadena de caracteres C con el texto que se 
* dibuja.
**************************************************************
**/
void ShowAlertText(Display *display, Window& win, GC& gc, XFontStruct *font_info, unsigned int x, unsigned int y, const char text[]);

/**
**************************************************************
* @brief Muestra una ventana en la que hay que introducir por
* teclado el numero de discos con los que se desea jugar.
* @param display Especifica la conexion con el servidor X.
* @param screen_num Espeficia numero de pantalla.
* @param main_x Coordenada x de la ventana principal de juego.
* @param main_y Coordenada y de la ventana principal de juego.
* @param main_width Ancho de la ventana principal de juego.
* @param main_height Alto de la ventana principal de juego.
* @return Numero de discos que se ha leido por teclado.
**************************************************************
**/
unsigned int ReadNumDiscs(Display* display, int screen_num, unsigned int main_x, unsigned int main_y, unsigned int main_width, unsigned int main_height);

#endif
