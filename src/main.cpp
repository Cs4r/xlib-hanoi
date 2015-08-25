/**
*******************************************************
* @file main.cpp
* @brief Implementación del juego de las torres de Hanoi
* por medio de Xlib. 
* @author Cesar Aguilera <cesarap at correo.ugr.es>
* @version 1.0
* @date Noviembre 2011
*
*
********************************************************
**/

#include <iostream> // Biblioteca Entrada/Salida C++
#include <stdio.h> // sprintf
#include "../include/icon.h" // icono xlib-hanoi
#include "../include/constants.h" // constantes xlib-hanoi
#include "../include/utils.h" // procedimientos, funciones, datatypes xlib-hanoi

/* Simplicidad y legilibidad del codigo */
#define HideAlerts XClearArea
#define DrawDiscsBorder XDrawRectangles
#define DrawDiscs XFillRectangles
#define DrawLockers XDrawRectangles

using namespace std;

int main(int argc, char* argv[])
{
  const char progname[] = "xlib-hanoi"; // Nombre del programa

  /* Variables de configuracion del display */
  Display *display;
  unsigned int display_width, display_height;
  int screen_num;
  
  /* Variables de configuracion de la ventana principal */
  Window main_win;
  unsigned int main_width, main_height, main_x, main_y;
  XSizeHints size_hints;
  
  /* Variables de configuracion de la ventana de juego (tablero) */	
  Window table_win;
  unsigned int table_width, table_height, table_x, table_y;
	
  /* Variables de configuracion de la ventana de movimientos */
  Window mov_win;
  unsigned int mov_x, mov_y,  button_borderwidth, button_width, button_height;
	
  /* Variables de configuracion de la ventana salir */
  Window exit_win;
  unsigned int exit_x, exit_y;
  
  /* Variables de configuracion de la ventana deshacer */
  Window undo_win;
  unsigned int undo_x, undo_y;
  
  /* Variables de configuracion de la ventana rehacer */
  Window redo_win;
  unsigned int redo_x, redo_y;

  /* Informe de eventos */
  XEvent event;
  
  /* Contextos graficos: caracteristicas con las que se dibujara texto y graficos */
  GC locker_gc; 
  GC text_gc;
  DiscGC disc_gc;
  
  /* Especificacion de la fuente con la que se escribe texto */
  XFontStruct *font_info;
 
  
  /* Conexion con el display X */
  if ( (display=XOpenDisplay(NULL)) == NULL ){   
    cerr<< progname << ERROR_OPEN_DISPLAY_TEXT <<endl;
    exit(EXIT_FAILURE);	
  }	
  screen_num = DefaultScreen(display);
  display_width = DisplayWidth(display, screen_num);
  display_height = DisplayHeight(display, screen_num);
	
  /* Configuracion de la ventana principal */
  main_x = display_width/6.;
  main_y = display_height/8.;
  main_width = display_width/1.5;
  main_height = display_height/1.5;
  
  /* Configuracion de la ventana de juego */
  table_x = 2*(main_width/100.);
  table_y = 12*(main_height/50.);
  table_width = 96*(main_width/100.);
  table_height = 35*(main_height/50.);
  	
  /* Configuracion de la ventana de movimientos */
  mov_x = mov_y = table_x;
  button_width = 14*(main_width/100.); 
  button_height = 3*(main_height/50.);
  button_borderwidth = 2; 
	
  /* Configuracion de la ventana salir (alto, ancho y borde de ventana de movimientos) */
  exit_x = 83.5*(main_width/100.);
  exit_y = mov_y;
  
  /* Configuracion de la ventana deshacer (alto, ancho y borde de ventana de movimientos) */
  undo_x = (main_width/2.) - button_width - 2*button_borderwidth;
  undo_y = mov_y;
  
  /* Configuracion de la ventana rehacer (alto, ancho y borde de ventana de movimientos) */
  redo_x = (main_width/2.) + 2*button_borderwidth;
  redo_y = mov_y;
    
  /* Numero de discos: se lee por teclado con ReadNumDiscs */
  const unsigned int NUM_DISCS = ReadNumDiscs(display,screen_num, main_x, main_y, main_width, main_height);
  
  /* Estructuras de datos */
  XRectangle locker[NUM_LOCKERS]; // Casilleros
  XRectangle disc[NUM_DISCS]; // Discos
  stack<unsigned int> tower[NUM_LOCKERS]; // Torres
  stack<Movement> undo_mov_stack; // Pila de deshacer movimientos
  stack<Movement> redo_mov_stack; // Pila de rehacer movimientos
  
  
  /* Iniciacializacion del tablero de juego */
  InitHanoiTable(locker, disc, NUM_DISCS, tower, table_width, table_height, (1+NUM_DISCS/5)*10);

  /* Creacion de ventanas */
  main_win = XCreateSimpleWindow(display, RootWindow(display,screen_num), main_x, main_y,
				 main_width, main_height, 0, BlackPixel(display,screen_num),
				 WhitePixel(display,screen_num));
				 	
  table_win = XCreateSimpleWindow(display, main_win, table_x, table_y, table_width,
				  table_height, 0, BlackPixel(display,screen_num), WhitePixel(display,screen_num));
	
  mov_win = XCreateSimpleWindow(display, main_win, mov_x, mov_y, button_width,
				button_height, button_borderwidth, GetColorNum(display, (char *)BUTTON_FG_COLOR), GetColorNum(display, (char *)BUTTON_BG_COLOR));
	
  exit_win = XCreateSimpleWindow(display, main_win, exit_x, exit_y, button_width,
				 button_height, button_borderwidth, GetColorNum(display, (char *)BUTTON_FG_COLOR), GetColorNum(display, (char *)BUTTON_BG_COLOR));

  undo_win = XCreateSimpleWindow(display, main_win,  undo_x,  undo_y,  button_width,
				 button_height, button_borderwidth, GetColorNum(display, (char *)BUTTON_FG_COLOR), GetColorNum(display, (char *)BUTTON_BG_COLOR));

  redo_win = XCreateSimpleWindow(display, main_win,  redo_x,  redo_y,  button_width,
				 button_height, button_borderwidth, GetColorNum(display, (char *)BUTTON_FG_COLOR), GetColorNum(display, (char *)BUTTON_BG_COLOR));

	
  /* Seleccion de eventos a procesar */	
  XSelectInput(display, table_win, ExposureMask | ButtonPressMask);
  XSelectInput(display, mov_win, ExposureMask);
  XSelectInput(display, exit_win, ExposureMask | ButtonPressMask);
  XSelectInput(display, undo_win, ExposureMask | ButtonPressMask);
  XSelectInput(display, redo_win, ExposureMask | ButtonPressMask);
  // XSelectInput(display, main_win, ExposureMask); // No es necesario
  
  /* Preparacion del contexto grafico para el texto */
  if(! PrepareTextGC(display, table_win, text_gc, &font_info)){
    cerr<< progname << ERROR_CARGE_FONT_TEXT <<endl;
    exit(EXIT_FAILURE);		  
  }
  
  /* Preparacion del contexto grafico para los casilleros */
  PrepareLockerGC(display, table_win, locker_gc);
  
  /* Preparacion del contexto grafico para los discos */
  PrepareDiscGC(display, table_win, disc_gc);

  /* Establecimiento de las propiedades de la ventana principal */
  size_hints.flags = PPosition | PSize | PMinSize | PMaxSize; 
  size_hints.x = main_x;
  size_hints.y = main_y;
  size_hints.width =  size_hints.min_width = size_hints.max_width =  main_width;
  size_hints.height = size_hints.min_height = size_hints.max_height =  main_height;
  Pixmap icon_pixmap = XCreateBitmapFromData(display, main_win, icon_bits, icon_width, icon_height);
  XSetStandardProperties(display, main_win, progname, progname, icon_pixmap, argv, argc, &size_hints);
  
  /* Mapeado de ventanas (hace que sean visibles en pantalla) */
  XMapWindow(display, main_win);
  XMapWindow(display, table_win);
  XMapWindow(display, mov_win);
  XMapWindow(display, exit_win);
  XMapWindow(display, undo_win);
  XMapWindow(display, redo_win);
  
  /* Gestionar la finalizacion del programa cuando se pulsa el boton de cerrar ventana */
  Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
  Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, main_win, &wm_delete_window, 1);
  
  /* Variables para controlar el movimiento de discos */
  Movement mov; // Guarda el origen y destino de un movimiento
  mov.orig = mov.dest = TOWER_LEFT;
  bool mov_disc = false; // Indica si el disco se mueve de un casillero a otro
  unsigned int num_mov = 0; // Numero de movimientos
  char num_mov_str[8] = "0"; // String con el numero de movimientos
  bool del_redo_stack = false; //  Indica si hay que borrar la pila de rehacer movimientos
  bool valid_mov = false; // Indica si el ultimo movimiento ha sido valido
     
  /* Bucle de eventos del programa. Es un bucle infinito en el que en cada pasada 
     se lee un evento, se ve de que tipo es y se realiza una u otra accion
     dependiendo del tipo */

  while (true) {
    XNextEvent(display, &event);
    switch(event.type) {
		case Expose:
		  /* Si se ha generado un evento expose en alguna de las ventanas y count a cero se redibujan contenidos */
		  if(event.xexpose.window == exit_win){
			if(event.xexpose.count == 0){
			  ShowText(display, exit_win, text_gc, font_info, button_width, button_height, EXIT_TEXT, false);
			}
		  }else if(event.xexpose.window == mov_win){
			if(event.xexpose.count == 0){
			  ShowText(display, mov_win, text_gc, font_info, button_width, button_height, num_mov_str, false);
			}	
		  }else if(event.xexpose.window == undo_win){
			if(event.xexpose.count == 0){
			  ShowText(display, undo_win, text_gc, font_info, button_width, button_height, UNDO_TEXT, false);
			}	
		  }else if(event.xexpose.window == redo_win){
			if(event.xexpose.count == 0){
			  ShowText(display, redo_win, text_gc, font_info, button_width, button_height, REDO_TEXT, false);
			}	
		  }else if(event.xexpose.window == table_win){
			if(event.xexpose.count == 0){
			  valid_mov = mov_disc = false;
			  DrawLockers(display, table_win, locker_gc, locker, NUM_LOCKERS);
			  DrawDiscs(display, table_win, disc_gc.rectangle, disc, NUM_DISCS);
			  DrawDiscsBorder(display, table_win, disc_gc.border, disc, NUM_DISCS);
			}
		  }
		  break;  
		case ButtonPress:
		  if(event.xbutton.window == exit_win){ 
			if(event.xbutton.button == Button1){
				/* Click derecho en la ventana salir, libera recursos y finalizar el programa */
				XUnloadFont(display, font_info->fid);
				XFreeGC(display, locker_gc);
				XFreeGC(display, disc_gc.rectangle);
				XFreeGC(display, disc_gc.border);
				XFreeGC(display, text_gc);		
				XDestroyWindow(display, main_win);
				XCloseDisplay(display);
				exit(EXIT_SUCCESS);
			}
		  }else if(event.xbutton.window == table_win){
			if(event.xbutton.button == Button1){
			  /* Click derecho en la ventana de juego (tablero) */
			  switch(mov_disc){
				  case false:
				    /* Se ha seleccionado torre de origen */
					if(event.xbutton.x >= locker[TOWER_LEFT].x && event.xbutton.x < (locker[TOWER_LEFT].x + locker[TOWER_LEFT].width)){
					  /* Click en el primer casillero */
					  mov.orig = TOWER_LEFT;
					  SelecDisc(display, table_win, disc_gc, disc, tower, mov.orig);
					  mov_disc = true;
					}else if(event.xbutton.x >= locker[TOWER_CENTER].x && event.xbutton.x < (locker[TOWER_CENTER].x + locker[TOWER_CENTER].width)){
					  /* Click en el segundo casillero */
					  mov.orig = TOWER_CENTER;
					  SelecDisc(display, table_win, disc_gc, disc, tower, mov.orig);
					  mov_disc = true;
					}else if(event.xbutton.x >= locker[TOWER_RIGHT].x && event.xbutton.x < (locker[TOWER_RIGHT].x + locker[TOWER_RIGHT].width)){
					  /* Click en el tercer casillero */
					  mov.orig = TOWER_RIGHT;
					  SelecDisc(display, table_win, disc_gc, disc, tower, mov.orig);
					  mov_disc = true; 
					}
					HideAlerts(display, main_win, 0, 0, main_width, main_height, false);
					break;
													
				  case true: 
				    /* Se ha seleccionado torre de destino */
					if(event.xbutton.x >= locker[TOWER_LEFT].x && event.xbutton.x < (locker[TOWER_LEFT].x + locker[TOWER_LEFT].width)){
					  /* Click en el primer casillero */
					  mov.dest = TOWER_LEFT;
					  valid_mov = MovDisc(display, table_win, disc_gc, locker, disc, tower, mov);
					  mov_disc = false;
					}else if(event.xbutton.x >= locker[TOWER_CENTER].x && event.xbutton.x < (locker[TOWER_CENTER].x + locker[TOWER_CENTER].width)){
					  /* Click en el segundo casillero */
					  mov.dest = TOWER_CENTER;
					  valid_mov = MovDisc(display, table_win, disc_gc, locker, disc, tower, mov);
					  mov_disc = false;
					}else if(event.xbutton.x >= locker[TOWER_RIGHT].x && event.xbutton.x < (locker[TOWER_RIGHT].x + locker[TOWER_RIGHT].width)){
					  /* Click en el tercer casillero */
					  mov.dest = TOWER_RIGHT;
					  valid_mov = MovDisc(display, table_win, disc_gc, locker, disc, tower, mov);
					  mov_disc = false;
					}else{
					  /* Click fuera de un casillero */
					  valid_mov = false;
					}
							
					if(valid_mov && !mov_disc){
					  /* Se ha intentado mover y el movimiento es valido. Incrementar numero de movimientos */ 
					  ++num_mov;
					  sprintf (num_mov_str, "%u", num_mov);
					  ShowText(display, mov_win, text_gc, font_info, button_width, button_height, num_mov_str, true);
					  undo_mov_stack.push(mov);
								
					  if(del_redo_stack){
						/* Se borra la pila de rehacer si despues de un rehacer hay un movimiento valido */
						while(!redo_mov_stack.empty()){
						  redo_mov_stack.pop();
						}
						del_redo_stack = false;
					  }
								
					  if(tower[TOWER_RIGHT].size() == NUM_DISCS){
						/* Se han conseguido llevar todos los discos a la torre derecha */
						HideAlerts(display, main_win, 0, 0, main_width, main_height, false);
						ShowAlertText(display, main_win, text_gc, font_info, table_x+table_width/2.-locker[TOWER_CENTER].width/2., table_y - (table_y -(exit_y+button_height))/2., ENDGAME_TEXT);
					  }
					  
					}else if(!valid_mov && !mov_disc){
					  /* Se ha intentado mover pero el movimiento es invalido */
					  HideAlerts(display, main_win, 0, 0, main_width, main_height, false);
					  ShowAlertText(display, main_win, text_gc, font_info, table_x+table_width/2.-locker[TOWER_CENTER].width/2., table_y - (table_y -(exit_y+button_height))/2., ERROR_MOV_TEXT);
					}
					
					break;
						  
				  default:
					break;
			  }
			}
				
		  }else if(event.xbutton.window == undo_win){
			if(event.xbutton.button == Button1){
				/* Click derecho en la ventana de deshacer */
				if(!undo_mov_stack.empty() && !mov_disc){ 
				  /* La pila de deshacer no esta vacia y no hay disco seleccionado. Se rehace el ultimo movimiento */
				  mov = undo_mov_stack.top();
				  undo_mov_stack.pop();
				  redo_mov_stack.push(mov);
				  unsigned short int aux = mov.orig;
				  mov.orig = mov.dest;
				  mov.dest = aux;
				  mov_disc = valid_mov = false;
				  --num_mov;
				  MovDisc(display, table_win, disc_gc, locker, disc, tower, mov); 
				  HideAlerts(display, main_win, 0, 0, main_width, main_height, false);
				  ShowAlertText(display, main_win, text_gc, font_info, table_x+table_width/2.-locker[TOWER_CENTER].width/2., table_y - (table_y -(exit_y+button_height))/2., UNDO_OK_TEXT);
				  sprintf (num_mov_str, "%u", num_mov);
				  ShowText(display, mov_win, text_gc, font_info, button_width, button_height, num_mov_str, true);
				}else{
				  /* La pila de deshacer esta vacia o hay disco seleccionado */
				  HideAlerts(display, main_win, 0, 0, main_width, main_height, false);
				  ShowAlertText(display, main_win, text_gc, font_info, table_x+table_width/2.-locker[TOWER_CENTER].width/2., table_y - (table_y -(exit_y+button_height))/2., ERROR_UNDO_TEXT);
				}
			}
		  }else if(event.xbutton.window == redo_win){
			if(event.xbutton.button == Button1){ 
				/* Click derecho en la ventana de rehacer */
				if(!redo_mov_stack.empty() && !mov_disc){ 
				  /* La pila de rehacer no esta vacia y no hay disco seleccionado */
				  if(valid_mov){ 
					/* Se ha hecho un movimiento valido antes, no se puede rehacer */
					HideAlerts(display, main_win, 0, 0, main_width, main_height, false);
					ShowAlertText(display, main_win, text_gc, font_info, table_x+table_width/2.-locker[TOWER_CENTER].width/2., table_y - (table_y -(exit_y+button_height))/2., ERROR_REDO_TEXT);  
					if(del_redo_stack){
					  /* Se borra la pila de rehacer si despues de un rehacer hay un movimiento valido */
					  while(!redo_mov_stack.empty()){
						redo_mov_stack.pop();
					  }
					  del_redo_stack = false;
					}  
				  }else{
					/* Se rehace el ultimo movimiento */
					del_redo_stack = true;
					mov = redo_mov_stack.top();
					redo_mov_stack.pop();
					undo_mov_stack.push(mov);
					mov_disc = valid_mov = false;
					++num_mov;
					MovDisc(display, table_win, disc_gc, locker, disc, tower, mov);
					HideAlerts(display, main_win, 0, 0, main_width, main_height, false);
					ShowAlertText(display, main_win, text_gc, font_info, table_x+table_width/2.-locker[TOWER_CENTER].width/2., table_y - (table_y -(exit_y+button_height))/2., REDO_OK_TEXT);
					sprintf (num_mov_str, "%u", num_mov);
					ShowText(display, mov_win, text_gc, font_info, button_width, button_height, num_mov_str, true);
				  }
				}else{
				  /* La pila de rehacer esta vacia o hay disco seleccionado */
				  HideAlerts(display, main_win, 0, 0, main_width, main_height, false);
				  ShowAlertText(display, main_win, text_gc, font_info, table_x+table_width/2.-locker[TOWER_CENTER].width/2., table_y - (table_y -(exit_y+button_height))/2., ERROR_REDO_TEXT);
				}
			}
		  }
		  break;
		case ClientMessage:
		  if (event.xclient.message_type == wm_protocols && event.xclient.data.l[0] == int(wm_delete_window)){
			/* Click en boton de cerrar ventana, libera recursos y finalizar el programa */
			XUnloadFont(display, font_info->fid);
			XFreeGC(display, locker_gc);
			XFreeGC(display, disc_gc.rectangle);
			XFreeGC(display, disc_gc.border);
			XFreeGC(display, text_gc);		
			XDestroyWindow(display, main_win);
			XCloseDisplay(display);
			exit(EXIT_SUCCESS);
		  }
		  break;
		default:
		  valid_mov = mov_disc = false;
		  break;
		}

  }
}
