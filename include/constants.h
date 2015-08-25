/**
**************************************************************
* @file constants.h
* @brief Definicion de constantes utilizadas en el programa.
* @author Cesar Aguilera <cesarap at correo.ugr.es>
* @date Noviembre 2011
**************************************************************
**/
#ifndef _XLIB_HANOI_CONSTANTS_H_
#define _XLIB_HANOI_CONSTANTS_H_

#define NUM_LOCKERS 3 // Numero de casilleros
#define SEP_H 4 // Separacion entre discos

/* Torres */
#define TOWER_LEFT 0 
#define TOWER_CENTER 1
#define TOWER_RIGHT 2 

/* Anchos de borde */
#define LOCKER_BORDER 2
#define DISC_NORMAL_BORDER 1
#define DISC_SELECTED_BORDER 3

/* Colores */
#define DISC_NORMAL_COLOR "LightSkyBlue"
#define DISC_SELECTED_COLOR "DeepSkyBlue"
#define BUTTON_FG_COLOR "Black"
#define BUTTON_BG_COLOR "Gray"
#define ALERT_COLOR "Red"

/* Fuentes de texto */
#define FONT_1 "10x20"
#define FONT_2 "8x13bold"
#define FONT_3 "9x15bold"
#define FONT_4 "9x15"

/* Textos */
#define ERROR_OPEN_DISPLAY_TEXT ": problema al conectar con el  servidor X"
#define ERROR_CARGE_FONT_TEXT ": problema al cargar las fuentes de texto"
#define ERROR_NUM_DISCS "Debe estar entre 3 y 30"
#define ERROR_UNDO_TEXT "No se puede deshacer"
#define ERROR_REDO_TEXT "No se puede rehacer"
#define ERROR_MOV_TEXT "Movimiento incorrecto"
#define EXIT_TEXT "Salir"
#define UNDO_TEXT "Deshacer"
#define REDO_TEXT "Rehacer"
#define POPWIN_TITTLE "Introduce el numero de discos"
#define ENDGAME_TEXT "Juego completado. Enhorabuena!"
#define UNDO_OK_TEXT "Deshecho!"
#define REDO_OK_TEXT "Rehecho!"

#endif
