/*
 * This file is a part of the EpsiFlash Copyright (c) 2000-2007 Michal
 * Szwaczko, WireLabs Systems contact: mikey@wirelabs.net.
 */

/*
 * this file is based on ideas and programmer interface schema from
 * uniprog by Daniel Quintero <danielqg@infonegocio.com> 
 */


#include "hardware.h"

unsigned char   data = 0x00;
unsigned char   control = 0x0C;

void
hardware_init(void)
{

    struct sched_param param;
    int             d;

    param.sched_priority = 1;

    if ((sched_setscheduler(0, SCHED_RR, &param)) < 0
	|| (ioperm(BASEPORT, 3, 1) < 0)) {
	perror("hardware_init()");
	exit(-1);
    };

    power_up();
    d = detect_hardware();
    power_down();

    if (d < 0) {
	fprintf(stderr,
		"hardware_init(): willem compatible burner not detected\n");
	exit(-1);
    };


}

int
detect_hardware(void)
{

    int             l,
                    r;

    OE_Low;

    clkhi;
    assert_data;
    waitms(10);
    r = einbit;

    clklo;
    assert_data;
    waitms(10);
    l = einbit;

    if (l == 0x80 && r == 0x00)
	return 0;
    else
	return 1;

    return result;
}

void
power_down(void)
{
    hw_read, adrlo, pgrlo, ppoff, powoff, clklo, clalo, assert_ctrl;
    assert_data;
    set_addr(0x000000);

}

void
power_up(void)
{
    set_addr(0x000000);
    hw_read, adrlo, pgrhi, ppoff, powoff, clklo, clalo, assert_ctrl;
    assert_data;
    waitms(10);
    VCC_On;
    waitms(100);
}

void
set_addr(long addr)
{

    int             mask;

    OE_Low;

    for (mask = (1 << 23); mask != 0; mask = mask >> 1) {

	(addr & mask) ? adrhi : adrlo;	/* out bit */
	clklo;
	assert_data;		/* shift clk low */
	clkhi;
	assert_data;		/* shift clk high */
    }
    adrlo;
    clklo;
    assert_data;
}


unsigned char
read_byte_addr(long addr)
{

    int             b,
                    w;

    set_addr(addr);


    OE_Low;

    adrhi;
    assert_data;		/* D = 1, OE=0 shift register parallel
				 * load mode */
    clahi;
    assert_data;		/* cla hi, load parallel clock */
    adrlo;
    assert_data;		/* D = 0, OE=1 Shift register, serial mode 
				 */
    clalo;
    assert_data;		/* cla lo, serial clock low */

    for (w = 0, b = 7; b >= 0; --b) {
	w |= (inbit ? 0 : 1) << b;	/* Get data from sr */
	clahi;
	assert_data;		/* serial clock high */
	clalo;
	assert_data;		/* serial clock low */
    }

    return (unsigned char) w;
}

void
write_byte_addr(long addr, unsigned char val)
{

    set_addr(addr);
    OE_High;
    OUTPORT(BASEPORT, val);
    WE_Low;			/* Write Pulse */
    WE_High;
    OE_Low;
}

void vpp_on(void){
  {ppon; assert_ctrl;};
}

void vpp_off(void) {
 {ppoff;assert_ctrl;};
}