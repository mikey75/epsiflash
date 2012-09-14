# This file is a part of the EpsiFlash
# Copyright (c) 2000-2007 Michal Szwaczko, WireLabs Systems
# contact: mikey@wirelabs.net

CC     = gcc 
CFLAGS  = $(shell pkg-config --cflags gtk+-2.0) 
LDFLAGS = $(shell pkg-config --libs gtk+-2.0)

CCDEP  = ${CC} -MM
TARGET_GTK = epsiflash
TARGET_CLI = epsiflash-cli

PROGRAMSRCS_CLI = console.c hardware.c chips.c
PROGRAMOBJS_CLI = ${PROGRAMSRCS_CLI:.c=.o}

PROGRAMSRCS_GTK = main.c hardware.c chips.c
PROGRAMOBJS_GTK = ${PROGRAMSRCS_GTK:.c=.o}

all:  dep_gtk objects_gtk binary_gtk dep_cli objects_cli binary_cli

dep_gtk: 
	${CCDEP} ${PROGRAMSRCS_GTK} > .depends

objects_gtk:${PROGRAMOBJS_GTK} 

binary_gtk: ${PROGRAMSRCS_GTK} 
	${CC} ${LDFLAGS} ${PROGRAMOBJS_GTK} -o ${TARGET_GTK}

dep_cli: 
	${CCDEP} ${PROGRAMSRCS_CLI} > .depends

objects_cli:${PROGRAMOBJS_CLI} 

binary_cli: ${PROGRAMSRCS_CLI} 
	${CC} ${PROGRAMOBJS_CLI} -o ${TARGET_CLI}




clean:
	rm -f ${TARGET_GTK} ${TARGET_CLI}
	rm -f *.[oas] core a.out .depends .depend
	rm -f *.core
	rm -f *.out
	rm -f *~

