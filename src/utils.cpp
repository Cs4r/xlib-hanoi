/**
**************************************************************
* @file utils.cpp
* @brief Implementacion de funciones y procedimientos para
* facilitar la programacion y juego de xlib-hanoi.
* @author Cesar Aguilera <cesarap at correo.ugr.es>
* @date Noviembre 2011
**************************************************************
**/

#include "../include/utils.h"
unsigned long int GetColorNum(Display *display, char *name)
{
	static XColor color, temp;
	XAllocNamedColor(display,DefaultColormap(display,DefaultScreen(display)), name, &color, &temp);
	return color.pixel;
}

void PrepareLockerGC(Display *display, Window& win, GC& gc)
{
	gc = XCreateGC(display, win, 0, 0);
	XSetLineAttributes(display, gc, LOCKER_BORDER, LineSolid, CapNotLast, JoinBevel);
}

void PrepareDiscGC(Display *display, Window& win, DiscGC& disc_gc)
{
	disc_gc.rectangle = XCreateGC(display, win, 0, 0);
	XSetForeground(display, disc_gc.rectangle, GetColorNum(display, (char *)DISC_NORMAL_COLOR));	
	
	disc_gc.border = XCreateGC(display, win, 0, 0);
	XSetLineAttributes(display, disc_gc.border, DISC_NORMAL_BORDER, LineSolid, CapNotLast, JoinBevel);
}

bool PrepareTextGC(Display *display, Window& win, GC& gc, XFontStruct **font_info)
{	
	gc = XCreateGC(display, win, 0, 0);
	/* Si no puede cargar la fuente 1, itenta cargar la 2,
	 * si no puede cargar la fuente 2, intenta cargar la 3,
	 * si no puede cargar la fuente 3, intenta cargar la 4
	 * si no puede cargar la fuente 4, devuelve false */
	if ((*font_info = XLoadQueryFont(display,FONT_1)) == NULL)
		if((*font_info = XLoadQueryFont(display,FONT_2)) == NULL)
			if((*font_info = XLoadQueryFont(display,FONT_3)) == NULL)
				if((*font_info = XLoadQueryFont(display,FONT_4)) == NULL)
					return false;

	XSetFont(display, gc, (*font_info)->fid);
	
	/* Se ha podido cargar una fuente */
	return true; 
}

void InitHanoiTable(XRectangle locker[NUM_LOCKERS], XRectangle disc[], unsigned int NUM_DISCS, std::stack<unsigned int> tower[NUM_LOCKERS], unsigned int table_width, unsigned int table_height, unsigned int SEP_V)
{
	     
  locker[TOWER_LEFT].width = locker[TOWER_CENTER].width = locker[TOWER_RIGHT].width =  1*(table_width/3.5);
  locker[TOWER_LEFT].height = locker[TOWER_CENTER].height = locker[TOWER_RIGHT].height =  table_height-2;
  locker[TOWER_LEFT].x = 1;
  locker[TOWER_LEFT].y = 1;
  locker[TOWER_CENTER].x = (table_width - locker[TOWER_RIGHT].width)/2.;
  locker[TOWER_CENTER].y = 1; 
  locker[TOWER_RIGHT].x = table_width-locker[TOWER_CENTER].width-1;
  locker[TOWER_RIGHT].y = 1;
	 
  for(unsigned int i = 0; i<NUM_DISCS; i++){
	/* A mayor indice (i) menor ancho del disco */
    disc[i].width = (SEP_V-(i+1)*2)*(locker[TOWER_LEFT].width*1.0/SEP_V*1.0); 
    disc[i].height = (locker[TOWER_LEFT].height)/(1.0*NUM_DISCS) - 2*SEP_H;
    tower[TOWER_LEFT].push(i);
  }
	 
  disc[0].x = locker[TOWER_LEFT].x + locker[TOWER_LEFT].width/2.0 - disc[0].width/2.0;
  disc[0].y = locker[TOWER_LEFT].height - SEP_H - disc[0].height;
     
  for(unsigned int i = 1; i<NUM_DISCS; ++i){
    disc[i].x = locker[TOWER_LEFT].x + locker[TOWER_LEFT].width/2.0 - disc[i].width/2.0;
    disc[i].y = disc[i-1].y - SEP_H - disc[0].height;
  }	
}

void SelecDisc(Display *display, Window& win, DiscGC& disc_gc, XRectangle disc[], std::stack<unsigned int> tower[], unsigned short int orig)
{
	if(!tower[orig].empty()){
		/* Limpia area ocupada por el disco del tope */
		XClearArea(display, win, disc[tower[orig].top()].x-DISC_NORMAL_BORDER, disc[tower[orig].top()].y-DISC_NORMAL_BORDER, disc[tower[orig].top()].width+DISC_SELECTED_BORDER, disc[tower[orig].top()].height+DISC_SELECTED_BORDER, false);
		
		/* Modifica el contexto grafico y dibuja el disco de diferente color y borde */
		XSetForeground(display, disc_gc.rectangle, GetColorNum(display, (char *) DISC_SELECTED_COLOR));
		XSetLineAttributes(display, disc_gc.border, DISC_SELECTED_BORDER, LineSolid, CapNotLast, JoinBevel);		
		XFillRectangle(display, win, disc_gc.rectangle, disc[tower[orig].top()].x, disc[tower[orig].top()].y, disc[tower[orig].top()].width, disc[tower[orig].top()].height);
		XDrawRectangle(display, win, disc_gc.border, disc[tower[orig].top()].x, disc[tower[orig].top()].y, disc[tower[orig].top()].width, disc[tower[orig].top()].height);
		
		/* Restaura el contexto grafico  */
		XSetForeground(display, disc_gc.rectangle, GetColorNum(display, (char *) DISC_NORMAL_COLOR));
		XSetLineAttributes(display, disc_gc.border, DISC_NORMAL_BORDER, LineSolid, CapNotLast, JoinBevel);
	}
}

bool MovDisc(Display *display, Window& win, DiscGC& disc_gc, XRectangle locker[], XRectangle disc[], std::stack<unsigned int> tower[], Movement mov)
{		
	bool ok; // Indica si el movimiento se ha podido efectuar
	
	if(mov.orig != mov.dest){
		/* Torre de origen y torre de destino distintas */
		if(!tower[mov.orig].empty()){
			/* Torre de origen no vacia */
			if(tower[mov.dest].empty()){
				/* Movimiento correcto.
				 * Torre de destino vacia. 
				 * Se quita el disco de la torre de origen y 
				 * se pone en la de destino. 
				 * Se borra el disco de la torre de origen.
				 * Se dibuja el disco en la torre de destino */
				XClearArea(display, win, disc[tower[mov.orig].top()].x-DISC_NORMAL_BORDER, disc[tower[mov.orig].top()].y-DISC_NORMAL_BORDER, disc[tower[mov.orig].top()].width+DISC_SELECTED_BORDER, disc[tower[mov.orig].top()].height+DISC_SELECTED_BORDER, false);
				disc[tower[mov.orig].top()].x = locker[mov.dest].x + locker[mov.dest].width/2.0 - disc[tower[mov.orig].top()].width/2.0 ;
				disc[tower[mov.orig].top()].y = locker[mov.dest].height - SEP_H - disc[tower[mov.orig].top()].height;
				tower[mov.dest].push(tower[mov.orig].top());
				tower[mov.orig].pop();
				XFillRectangle(display, win, disc_gc.rectangle, disc[tower[mov.dest].top()].x, disc[tower[mov.dest].top()].y, disc[tower[mov.dest].top()].width, disc[tower[mov.dest].top()].height);
				XDrawRectangle(display, win, disc_gc.border, disc[tower[mov.dest].top()].x, disc[tower[mov.dest].top()].y, disc[tower[mov.dest].top()].width, disc[tower[mov.dest].top()].height);
				ok = true;
			}else if(tower[mov.orig].top() > tower[mov.dest].top()){
				/* Movimiento correcto.
				 * Indice del disco de la torre de origen mayor que indice disco de la de destino
				 * (Mayor indice indica menor ancho de disco).
				 * Se quita el disco de la torre de origen y se pone en la de destino. 
				 * Se borra el disco de la torre de origen.
				 * Se dibuja el disco encima de la torre de destino */
				XClearArea(display, win, disc[tower[mov.orig].top()].x-DISC_NORMAL_BORDER, disc[tower[mov.orig].top()].y-DISC_NORMAL_BORDER, disc[tower[mov.orig].top()].width+DISC_SELECTED_BORDER, disc[tower[mov.orig].top()].height+DISC_SELECTED_BORDER, false);
				disc[tower[mov.orig].top()].x = locker[mov.dest].x + locker[mov.dest].width/2.0 - disc[tower[mov.orig].top()].width/2.0 ;
				disc[tower[mov.orig].top()].y = disc[tower[mov.dest].top()].y - SEP_H - disc[tower[mov.dest].top()].height;
				tower[mov.dest].push(tower[mov.orig].top()); 
				tower[mov.orig].pop();
				XFillRectangle(display, win, disc_gc.rectangle, disc[tower[mov.dest].top()].x, disc[tower[mov.dest].top()].y, disc[tower[mov.dest].top()].width, disc[tower[mov.dest].top()].height);
				XDrawRectangle(display, win, disc_gc.border, disc[tower[mov.dest].top()].x, disc[tower[mov.dest].top()].y, disc[tower[mov.dest].top()].width, disc[tower[mov.dest].top()].height);
				ok = true;
			}else{
				/* Movimiento incorrecto.
				 * No se puede colocar un disco encima de uno mas
				 * pequenyo.
				 * Se deselecciona el disco y se vuelve a dibujar */
				XClearArea(display, win, disc[tower[mov.orig].top()].x-DISC_NORMAL_BORDER, disc[tower[mov.orig].top()].y-DISC_NORMAL_BORDER, disc[tower[mov.orig].top()].width+DISC_SELECTED_BORDER, disc[tower[mov.orig].top()].height+DISC_SELECTED_BORDER, false);
				XFillRectangle(display, win, disc_gc.rectangle, disc[tower[mov.orig].top()].x, disc[tower[mov.orig].top()].y, disc[tower[mov.orig].top()].width, disc[tower[mov.orig].top()].height);
				XDrawRectangle(display, win, disc_gc.border, disc[tower[mov.orig].top()].x, disc[tower[mov.orig].top()].y, disc[tower[mov.orig].top()].width, disc[tower[mov.orig].top()].height);
				ok = false;
			}
		}else{
			/* Movimiento incorrecto.
			 * Torre de origen vacia */
			ok = false;
		}
	}else{
		/* Movimiento incorrecto.
		 * Torre de origen y torre de destino son la misma */
		if(!tower[mov.orig].empty()){
			/* Torre de origen no vacia. 
			 * Se deselecciona el disco y se vuelve dibujar */
			XClearArea(display, win, disc[tower[mov.orig].top()].x-DISC_NORMAL_BORDER, disc[tower[mov.orig].top()].y-DISC_NORMAL_BORDER, disc[tower[mov.orig].top()].width+DISC_SELECTED_BORDER, disc[tower[mov.orig].top()].height+DISC_SELECTED_BORDER, false);
			XFillRectangle(display, win, disc_gc.rectangle, disc[tower[mov.dest].top()].x, disc[tower[mov.dest].top()].y, disc[tower[mov.dest].top()].width, disc[tower[mov.dest].top()].height);
			XDrawRectangle(display, win, disc_gc.border, disc[tower[mov.orig].top()].x, disc[tower[mov.orig].top()].y, disc[tower[mov.orig].top()].width, disc[tower[mov.orig].top()].height);
		}
		ok = false;
	}
	return ok;
}

void ShowText(Display *display, Window& win, GC& gc, XFontStruct *font_info, unsigned int win_width, unsigned int win_height, const char text[], bool clear_area)
{
	static int len, width;
	
	len = strlen(text);
	width = XTextWidth(font_info, text, len);

	if(clear_area){
		/* Limpia la ventana */
		XClearArea(display, win, 0, 0, win_width, win_height, false);
	}
	XDrawString(display, win, gc, (win_width - width)/2.0, (win_height)/1.5, text, len);
}

void ShowAlertText(Display *display, Window& win, GC& gc, XFontStruct *font_info, unsigned int x, unsigned int y, const char text[])
{
	static int len;
	len = strlen(text);
	
	/* Modifica el contexto grafico y dibuja el texto */
	XSetForeground(display, gc, GetColorNum(display, (char *)ALERT_COLOR));	
	XDrawString(display, win, gc, x, y, text, len);
	
	/* Restaura el contexto grafico  */
	XSetForeground(display, gc, GetColorNum(display, (char *)BUTTON_FG_COLOR ));	
}

unsigned int ReadNumDiscs(Display* display, int screen_num, unsigned int main_x, unsigned int main_y, unsigned int main_width, unsigned int main_height)
{
	Window pop_win;
	unsigned int pop_width, pop_height, pop_x, pop_y;
	XSizeHints size_hints;
	GC gc;
	XFontStruct *font_info; 
	XEvent event;	
	const unsigned int bufsize = 3;
	char buffer[bufsize];
	KeySym keysym;
	XComposeStatus compose;
	char string[bufsize];
	int length;
	
	string[0] = '3';
	string[1] = '\0';
	
	pop_x = main_x + main_width/4.;
	pop_y = main_y + main_height/4.;
	pop_width = main_width/2.5;
	pop_height = 3*(main_height/50.);
	
	pop_win = XCreateSimpleWindow(display, RootWindow(display,screen_num), pop_x,  pop_y,  pop_width,
				 pop_height, 0, BlackPixel(display,screen_num), WhitePixel(display,screen_num));
	
	size_hints.flags = PPosition | PSize | PMinSize | PMaxSize; 
	size_hints.x = pop_x;
	size_hints.y = pop_y;
	size_hints.width =  size_hints.min_width = size_hints.max_width =  pop_width;
	size_hints.height = size_hints.min_height = size_hints.max_height =  pop_height;
	Pixmap icon_pixmap = XCreateBitmapFromData(display, pop_win, icon_bits, icon_width, icon_height);
	
	XSetStandardProperties(display, pop_win, POPWIN_TITTLE , NULL, icon_pixmap, NULL, 0, &size_hints);
	
	XSelectInput(display, pop_win, ExposureMask | KeyPressMask);
	
	PrepareTextGC(display, pop_win, gc, &font_info);
	
	XMapWindow(display, pop_win);
	
	/* Gestionar que no se finalice el programa cuando se pulsa el boton de cerrar ventana */
	Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
	Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, pop_win, &wm_delete_window, 1);
	
	while (true) {
		XNextEvent(display, &event);
		switch(event.type) {
			case Expose:
				/* Si se ha generado un evento expose y count a cero se redibujan contenidos */
				if(event.xexpose.count == 0){
					ShowText(display, pop_win, gc,font_info, pop_width, pop_height, string, false);
				}
			break;
			case KeyPress:
				/* Lee tecla */
				XLookupString(&(event.xkey), buffer, bufsize, &keysym, &compose);
				if ((keysym == XK_Return) || (keysym == XK_KP_Enter) || (keysym == XK_Linefeed)) {
					/* Confirmacion del numero de discos */
					unsigned int num_discs =  atoi(string);
					if(num_discs >= 3 && num_discs <=30){
						/* Numero de discos correcto */
						XUnloadFont(display, font_info->fid);
						XFreeGC(display, gc);
						XDestroyWindow(display, pop_win);
						return num_discs;
					}else{
						/* Numero de discos incorrecto */
						ShowText(display, pop_win, gc, font_info, pop_width, pop_height, ERROR_NUM_DISCS, true);
						string[0] = '\0';
						break;
					}
				}else if((keysym == XK_BackSpace) || (keysym == XK_Delete)){
					/* Borrar cifra */
					length = strlen(string);
					if (length > 0) {
						string[length-1] = '\0';
					}else{
						XBell(display, 100);
					}
				}else if(buffer[0] >= '0' && buffer[0] <= '9'){
					/* Anyadir cifra */
					length = strlen(string);
					if(length+1 < 3){
						buffer[1] = '\0';
						strcat(string, buffer);
					}else{
						XBell(display, 100);
					}
				}else {
					XBell(display, 100);
				}
				ShowText(display, pop_win, gc,font_info, pop_width, pop_height, string, true);
				buffer[0] = '\0';
				
				break;
				
			case MappingNotify:
				XRefreshKeyboardMapping(&(event.xmapping));
				break;
			case ClientMessage:
				/* Click en boton de cerrar ventana, devuelve que 3 como numero
				 * de discos */
				if (event.xclient.message_type == wm_protocols && event.xclient.data.l[0] == int(wm_delete_window)){
					XUnloadFont(display, font_info->fid);
					XFreeGC(display, gc);
					XDestroyWindow(display, pop_win);
					return 3;
				}
				break;
			default:
			    break;
		}	
	}
}
