/*
 * This file is a part of the EpsiFlash Copyright (c) 2000-2007 Michal
 * Szwaczko, WireLabs Systems contact: mikey@wirelabs.net 
 */

#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib.h>

#include "chips.h"
#include "hardware.h"

GtkWidget      *mainwindow;
GtkWidget      *chiplabel;
GtkWidget      *chipcombo;
GtkWidget      *statusbar;
GtkWidget      *progressbar;
GtkWidget      *vbox;
GtkWidget      *hbox;

GtkWidget      *helpwindow;
GtkWidget      *jumperimage;

GtkWidget      *bufferwindow;
GtkWidget      *bufferscrolledwindow;
GtkWidget      *adrlabel;
GtkWidget      *hexlabel;
GtkWidget      *asciilabel;
GtkWidget      *bufferclist;


GtkWidget      *fileopenbutton;
GtkWidget      *filesavebutton;
GtkWidget      *readchipbutton;
GtkWidget      *burnchipbutton;
GtkWidget      *viewbufferbutton;
GtkWidget      *showhelpbutton;
GtkWidget      *erasechipbutton;

int             chipindex;
int             bufferloaded = 0;
unsigned char  *buffer = NULL;
unsigned int    buffersize;
unsigned short int pagesize;
char           *fileinthebuffer = NULL;

void            file_open(void);
void            file_save(void);
void            view_buffer(void);
void            show_help(void);
void            msg_ready(void);
void            quit(void);
void            read_chip(void);
void            erase_chip(void);
void            program_chip(void);
void            chip_selected(void);
int             probe_chip(void);
void            show_progress(unsigned long int adr, const char *msg);

typedef struct _viewerline {
    char           *adres;
    char           *hexy;
    char           *ascii;

} viewerline_t;


extern struct chiptable chips[];
extern unsigned char control;
extern void generic_probe(void);
