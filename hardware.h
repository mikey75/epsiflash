#ifndef __HARDWARE_H__
#define __HARDWARE_H__ 1
/*
 * This file is a part of the EpsiFlash Copyright (c) 2000-2007 Michal
 * Szwaczko, WireLabs Systems contact: mikey@wirelabs.net 
 */
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sched.h>

/*
 * you can change this if you have programmer on other port 
 */
#define BASEPORT  0x378

#define EIN 128
#define IN2 64
#define POW 4
#define RW  2
#define PP  1
#define PGM 8
#define ADR 2
#define CLK 1
#define CLA 4

#define CTW 160
#define CTR 161

#define BIC 0x20


#define einbit  (INPORT(BASEPORT+1)&EIN)
#define inbit   (INPORT(BASEPORT+1)&IN2)

#define powoff  (control &= ~POW)
#define powon   (control |= POW)
#define hw_read    (control &= ~RW)
#define hw_write   (control |= RW)
#define pgrhi   (control &= ~PGM)
#define pgrlo   (control |= PGM)
#define ppoff   (control |= PP)
#define ppon    (control &= ~PP)

#define adrlo   (data &= ~ADR)
#define adrhi   (data |= ADR)
#define clklo   (data &= ~CLK)
#define clkhi   (data |= CLK)
#define clalo   (data |=CLA)
#define clahi   (data &=~CLA)

#define assert_ctrl  (OUTPORT(BASEPORT+2,control))
#define assert_data (OUTPORT(BASEPORT,data))

#define WE_High     {pgrhi;assert_ctrl;}	/* WE, code 8 = C17/S4 Pin 
						 * 31 High */
#define WE_Low      {pgrlo;assert_ctrl;}	/* WE, code 8 = C17/S4 Pin 
						 * 31 Low */
#define OE_High     {hw_write;assert_ctrl;}	/* OE, code 2 = S6 high */
#define OE_Low      {hw_read;assert_ctrl;}	        /* OE, code 2 = S6 low */
#define VCC_On      {powon;assert_ctrl;}	/* VCC, code 4 = Vcc 5V */
#define VCC_Off     {powoff;assert_ctrl;}	/* VCC, code 4 = Vcc 0V */


#define OUTPORT(port,val) outb(val, port)
#define INPORT(port)      inb(port)

#define waitms(ms) usleep(ms*1000)	/* miliseconds */


void            hardware_init();
int             detect_hardware();
void            power_down();
void            power_up();
void            set_addr();
void            vpp_on();
void            vpp_off();


#endif 