/*
 * This file is a part of the EpsiFlash Copyright (c) 2000-2007 Michal
 * Szwaczko, WireLabs Systems contact: mikey@wirelabs.net 
 */
#include "hardware.h"
#include "chips.h"

#define waitms(ms) usleep(ms*1000)	/* miliseconds */
#define MAX_REFLASH_TRIES 0x5

extern unsigned char    *buffer;
extern void             show_progress(unsigned long, const char *);
extern void             write_byte_addr(long , unsigned char );
extern unsigned char    read_byte_addr(long );

struct chiptable  chips[] = {

{
             .name        =  "Winbond 29C020",
             .id1         =  0xDA,
             .id2         =  0x45,
             .size        =  2048 * 1024 / 8,
             .pagesize    =  0x80, 
             .adapter     =  "Flashboard DIL32",
             .probe_func  =  w29_probe, 
             .read_func   =  generic_read, 
             .burn_func   =  jedec_page_write, 
             .erase_func  =  generic_erase
},{

             .name        =  "Winbond 29EE011", 
             .id1         =  0xDA,
             .id2         =  0xC1,
             .size        =  1024 * 1024 / 8, 
             .pagesize    =  0x80, 
             .adapter     =  "Flashboard DIL32",
             .probe_func  =  w29_probe,
             .read_func   =  generic_read, 
             .burn_func   =  jedec_page_write, 
             .erase_func  =  generic_erase
},{

             .name        =  "SST 29EE020",
             .id1         =  0xbf,
             .id2         =  0x10,
             .size        =  2048 * 1024 / 8, 
             .pagesize    =  0x80, 
             .adapter     =  "Flashboard DIL32",
             .probe_func  =  generic_probe,
             .read_func   =  generic_read, 
             .burn_func   =  jedec_page_write, 
             .erase_func  =  generic_erase

},{

             .name        =  "SST PH29EE010",
             .id1         =  0xbf,
             .id2         =  0x07,
             .size        =  1024 * 1024 / 8,
             .pagesize    =  0x80,
             .adapter     =  "Flashboard DIL32",
             .probe_func  =  generic_probe, 
             .read_func   =  generic_read, 
             .burn_func   =  jedec_page_write, 
             .erase_func  =  generic_erase

},{
             .name        =  "ASD 29F2008", 
             .id1         =  0x10,
             .id2         =  0x10,
             .size        =  2048 * 1024 / 8, 
             .pagesize    =  0x80, 
             .adapter     =  "Flashboard DIL32",
             .probe_func  =  generic_probe, 
             .read_func   =  generic_read, 
             .burn_func   =  jedec_page_write, 
             .erase_func  =  generic_erase

},{

             .name        =  "Maxonic MX28F1000PPC",
             .id1         =  0xc2,
             .id2         =  0x1a,
             .size        =  1024 * 1024 / 8, 
             .pagesize    =  0x80, 
             .adapter     =  "Flashboard DIL32",
             .probe_func  =  mx28f1000_probe, 
             .read_func   =  generic_read, 
             .burn_func   =  mx28f_write, 
             .erase_func  =  mx28f_erase
},{
             .name        =  "Intel 28F001BX-T", 
             .id1         =  0x89,
             .id2         =  0x94,
             .size        =  1024 * 1024 / 8, 
             .pagesize    =  0x100, 
             .adapter     =  "Flashboard DIL32",
             .probe_func  =  i28f001bxt_probe, 
             .read_func   =  generic_read, 
             .burn_func   =  intel28f_write,
             .erase_func  =  intel28f_erase
},{ 

             .name        =  "Atmel AT49F002AN", 
             .id1         =  0x1f,
             .id2         =  0x07,
             .size        =  2048 * 1024 / 8,
             .pagesize    =  0x100,
             .adapter     =  "Flashboard PLCC32",
             .probe_func  =  generic_probe, 
             .read_func   =  generic_read, 
             .burn_func   =  at49f_write, 
             .erase_func  =  at49f_erase
},{ 
             .name        =  "AMD Am29F040B",
             .id1         =  0x01,
             .id2         =  0xa4,
             .size        =  4096 * 1024 /8,
             .pagesize    =  0x100,
             .adapter     =  "Flashboard PLCC32",
             .probe_func  =  generic_probe, 
             .read_func   =  generic_read,
             .burn_func   =  am29f_write,
             .erase_func  =  am29f_erase
},{

             .name        =  "UNKNOWN",
             .id1         =  0x00,
             .id2         =  0x00,
             .size        =  1024 * 1024 /8,
             .pagesize    =  0x100,
             .adapter     =  "NONE",
             .probe_func  =  dummy_probe, 
             .read_func   =  dummy_read,
             .burn_func   =  dummy_write,
             .erase_func  =  dummy_erase
},{


             NULL, 
             0,
             0,
             0, 
             0, 
             NULL,
             NULL,
             NULL,
             NULL
  } 

};

void dummy_probe (void){};
int dummy_read (unsigned int buffer_size, unsigned short int page_size){ return 1;};
int dummy_write (unsigned int buffer_size, unsigned short int page_size){ return 1; };
int dummy_erase (unsigned int buffer_size, unsigned short int page_size){ return 1; };

/* generic functions that work on every chip, if you know its size */
int
generic_read (unsigned int buffer_size, unsigned short int page_size)
{

  int i, z;
  long a = 0x000000;

  for (i = 0; i < buffer_size / page_size; i++)
    {
      show_progress (a, "Read");
      for (z = 0; z < page_size; z++)
	{
	  buffer[(int) a] = read_byte_addr (a);
	  a++;
	}
    }

  return 1;
}

int
generic_blank_check (unsigned int buffer_size, unsigned short int page_size)
{

  int z, i;
  unsigned long a = 0x000000;


  for (i = 0; i < buffer_size / page_size; i++)
    {
      show_progress (a, "Blankcheck");

      for (z = 0; z < page_size; z++)
	{

	  if (read_byte_addr (a) != 0xFF)
	    return -1;
	  a++;
	}

    }
  return 1;
}

int
generic_verify (unsigned int buffer_size, unsigned short int page_size)
{

  long a = 0x000000;
  int z, i;

  for (i = 0; i < buffer_size / page_size; i++)
    {
      show_progress (a, "Verify");
      for (z = 0; z < page_size; z++)
	{
	  if (read_byte_addr (a) != buffer[(int) a])
	    return -1;
	  a++;

	}
    }
  return 1;
}

void 
generic_probe (void) {

          write_byte_addr (0x5555, 0xAA);
          write_byte_addr (0x2AAA, 0x55);
          write_byte_addr (0x5555, 0x90);

          id1 = read_byte_addr(0x0);
          id2 = read_byte_addr(0x1);

          write_byte_addr (0x5555, 0xAA);
          write_byte_addr (0x2AAA, 0x55);
          write_byte_addr (0x5555, 0xf0);

}

void w29_probe (void) {

          write_byte_addr (0x5555, 0xAA);
          write_byte_addr (0x2AAA, 0x55);
          write_byte_addr (0x5555, 0x80);

          write_byte_addr (0x5555, 0xAA);
          write_byte_addr (0x2AAA, 0x55);
          write_byte_addr (0x5555, 0x60);

          waitms(10);
          id1 = read_byte_addr(0x0);
          id2 = read_byte_addr(0x1);

          write_byte_addr (0x5555, 0xAA);
          write_byte_addr (0x2AAA, 0x55);
          write_byte_addr (0x5555, 0xf0);
          waitms(10);
}

void mx28f1000_probe(void) {

          vpp_on();
          write_byte_addr (0, 0x90);
          id1 = read_byte_addr(0x0);
          id2 = read_byte_addr(0x1);
          vpp_off();

}

void i28f001bxt_probe(void){

        write_byte_addr(0,0x90);
        id1 = read_byte_addr(0x0);
        id2 = read_byte_addr(0x1);


}
int
generic_erase (unsigned int buffer_size, unsigned short int page_size)
{
          show_progress (0, "Erase");

          write_byte_addr (0x5555, 0xAA);
          write_byte_addr (0x2AAA, 0x55);
          write_byte_addr (0x5555, 0x80);
          write_byte_addr (0x5555, 0xAA);
          write_byte_addr (0x2AAA, 0x55);
          write_byte_addr (0x5555, 0x10);
          waitms (50);

          DQ6_toggle (0);

          return 1;
}




/*
 * jedec compatible bit6 write phase end check 
 */
void
DQ6_toggle (unsigned long a)
{
  unsigned int i = 0;
  unsigned char b1, b2;

  b1 = (read_byte_addr (a)) & 0x40;

  while (i++ < 0xFFFFFFF)
    {
      b2 = (read_byte_addr (a)) & 0x40;
      if (b1 == b2)
	{
	  break;
	}
      b1 = b2;
    }
}

/*
 * check dq6 ready - based on SST docs 
 */
void
check_dq6 (unsigned long a)
{

  int pd;
  int cd;

  unsigned long t = 0;

  pd = read_byte_addr (a);
  pd = pd & 0x40;

  while (t < 0x07FFFFFF)
    {
      cd = read_byte_addr (a);
      cd = cd & 0x40;

      if (pd == cd)
	{
	  break;
	}
      pd = cd;
      t++;
    }
}

/*
 * check dq7 - data polling - based on SST docs 
 */
void
check_dq7 (unsigned long adr, unsigned char data)
{
  unsigned char cd;
  unsigned long t = 0;

  data = data & 0x80;

  /*while (t < 0x07FFFFFF)*/
  for (;;) 
    {
      /*cd = read_byte_addr (adr);*/
      /*cd = cd & 0x80;
      if (data == cd)*/

      if ((read_byte_addr (adr) & 0x80) == data) 
	break;
      t++;
    }
}

int
jedec_page_write (unsigned int buffer_size, unsigned short int page_size)
{

  int i;
  long a;
  long a2;

  int tries;
  int z;

  a = 0x000000;

  /*
   * jedec_erase(); 
   */

  for (i = 0; i < buffer_size / page_size; i++)
    {

      tries = 0;
      show_progress (a, "Write");
      a2 = a;
    retry:
      write_byte_addr (0x5555, 0xAA);
      write_byte_addr (0x2AAA, 0x55);
      write_byte_addr (0x5555, 0xA0);

      /*
       * burn page 
       */
      for (z = 0; z < page_size; z++)
	{
	  if (buffer[(int) a] != 0xFF)
	    write_byte_addr (a, buffer[(int) a]);
	  a++;
	}

      waitms (10); /* datasheet */
      check_dq6(a - 1);

      /*
       * verify page and reflash in case of errors 
       */
      for (z = 0; z < page_size; z++)
	{

	  if (read_byte_addr (a2 + z) != buffer[(int) a2 + z])
	    {
	      a = a2;
	      tries++;
	      if (tries > MAX_REFLASH_TRIES)
		return -1;
	      else
		goto retry;
	    }

	}

    }


  return 1;
}



int
jedec_byte_write (unsigned int buffer_size, unsigned short int page_size)
{

  int i;
  int z;
  int tries;
  long a;
  tries = 0;
  a = 0x000000;

  vpp_on();

  for (i = 0; i < buffer_size / page_size; i++)
    {

      show_progress (a, "Write");

      for (z = 0; z < page_size; z++)
	{
	retr:
	  write_byte_addr (0x5555, 0xAA);
	  write_byte_addr (0x2AAA, 0x55);
	  write_byte_addr (0x5555, 0xA0);

	  write_byte_addr (a, buffer[(int) a]);
	  waitms (20);
	  DQ6_toggle (a);

	  if (read_byte_addr (a) != buffer[(int) a])
	    {

	      tries++;
	      if (tries > MAX_REFLASH_TRIES)
		{
		  return -1;
		}
	      goto retr;
	    }
	  a++;
	}
    }
  vpp_off();
  return 1;
}


int
intel28f_write (unsigned int buffer_size, unsigned short int page_size)
{

  vpp_on();

  int i, z;
  long a;

  a = 0;

  for (i = 0; i < buffer_size / page_size; i++)
    {

      show_progress (a, "Write");

      for (z = 0; z < page_size; z++)
	{
	  if (buffer[(int) a] != 0xFF)
	    {
	      write_byte_addr (a, 0x40);
	      write_byte_addr (a, buffer[(int) a]);
	    }
	waito:
	  write_byte_addr (a, 0x70);
	  if (read_byte_addr (a & 0x80) == 0)
	    goto waito;
	  a++;
	}

    }
  write_byte_addr (a, 0xff);
  vpp_off();

  return 1;
}


int
intel28f_erase (unsigned int buffer_size, unsigned short int page_size)
{

  vpp_on();
  int i;

  long a;

  long pages[] = { 0x0, 0x1c000, 0x1d000, 0x1e000, 0xff };

  i = 0;

  for (;;)
    {

      a = pages[i];
      if (a == 0xff)
	break;
      show_progress (a, "Erase");
      /*
       * clear status register 
       */
      write_byte_addr (a, 0x50);
      /*
       * erase block command 
       */
      write_byte_addr (a, 0x20);
      write_byte_addr (a, 0xd0);
      /*
       * toggle change of status register 
       */

wait:   write_byte_addr (a, 0x70);
        if (read_byte_addr (a & 0x80) == 0)
	goto wait;
        i++;
    }

  write_byte_addr (a, 0xff);
  vpp_off();
  return 1;
}




int
mx28f_erase (unsigned int buffer_size, unsigned short int page_size)
{

  vpp_on();

      show_progress (0, "Erase");

      write_byte_addr (0, 0x30);
      write_byte_addr (0, 0x30);

      check_dq7(0,0xff);

      write_byte_addr (0, 0xff);


  vpp_off();
  return 1;
}

int
mx28f_write (unsigned int buffer_size, unsigned short int page_size)
{


  vpp_on();

  int i, z;
  long a;

  a = 0;

  for (i = 0; i < buffer_size / page_size; i++)
    {

      show_progress (a, "Write");

      for (z = 0; z < page_size; z++)
	{

	  if (buffer[(int) a] != 0xFF)
	    {
	      write_byte_addr (a, 0x40);
	      write_byte_addr (a, buffer[(int) a]);
	    }
	waito:
	  if (read_byte_addr (a & 0x40) == 0)
	    goto waito;
	  a++;
	}

    }
  write_byte_addr (a, 0xff);
  vpp_off();
  return 1;
}


int
at49f_erase (unsigned int buffer_size, unsigned short int page_size)
{

  show_progress (0, "Erase");
  /*
   * erase block command 
   */
  write_byte_addr (0x555, 0xAA);
  write_byte_addr (0xAAA, 0x55);
  write_byte_addr (0x555, 0x80);
  write_byte_addr (0x555, 0xaa);
  write_byte_addr (0xAAA, 0x55);
  write_byte_addr (0x555, 0x10);	/*a, 0x30); */



wait:
  if (read_byte_addr (0x3ffff & 0x40) == 0)
    goto wait;
  return 1;
}

int
at49f_write (unsigned int buffer_size, unsigned short int page_size)
{


  int i;
  int z;
  int tries;
  long a;
  tries = 0;
  a = 0x000000;


  for (i = 0; i < buffer_size / page_size; i++)
    {

      show_progress (a, "Write");

      for (z = 0; z < page_size; z++)
	{
	retr:
	  if (buffer[(int) a] != 0xff)
	    {
	      write_byte_addr (0x555, 0xAA);
	      write_byte_addr (0xAAA, 0x55);
	      write_byte_addr (0x555, 0xA0);
	      write_byte_addr (a, buffer[(int) a]);

//	    wa:
//	      if (read_byte_addr (a & 0x40) == 0)
//		goto wa;
            check_dq6(a);

	      if (read_byte_addr (a) != buffer[(int) a])
		{

		  tries++;
		  if (tries > MAX_REFLASH_TRIES)
		    {
		      return -1;
		    }
		  goto retr;
		}
	    }
	  a++;
	}
    }
  return 1;
}



int
am29f_erase (unsigned int buffer_size, unsigned short int page_size)
{

    unsigned long a = 0;
      show_progress (a, "Erase");
      /*
       * erase block command 
       */
  
  write_byte_addr (0x555, 0xAA);
  write_byte_addr (0xAAA, 0x55);
  write_byte_addr (0x555, 0x80);

  write_byte_addr (0x555, 0xaa);
  write_byte_addr (0xAAA, 0x55);
  write_byte_addr (0x555, 0x10);
/*  waitms(1500); */
/*  sleep(35); */
/*  check_dq7(0x7FFFF,0xff); */
wa:
  if ((read_byte_addr(0) & 0x80) ==0)
  goto wa;

  
/*  write_byte_addr(0x555, 0xf0);   */


  return 1;
}


int
am29f_write (unsigned int buffer_size, unsigned short int page_size)
{


  int i;
  int z;
  int tries;
  long a;
  tries = 0;
  a = 0x000000;


  for (i = 0; i < buffer_size / page_size; i++)
    {

      show_progress (a, "Write");

      for (z = 0; z < page_size; z++)
	{

	retr:
	  if (buffer[(int) a] != 0xff)
	    {
	      write_byte_addr (0x555, 0xAA);
	      write_byte_addr (0xAAA, 0x55);
	      write_byte_addr (0x555, 0xA0);
	      write_byte_addr (a, buffer[(int) a]);

	    wa:
	      if (read_byte_addr (a & 0x40) == 0)
		goto wa;

	      if (read_byte_addr (a) != buffer[(int) a])
		{

		  tries++;
		  if (tries > MAX_REFLASH_TRIES)
		    {
		      return -1;
		    }
		  goto retr;
		}
	    }
	  a++;
	}
    }
  return 1;
}
