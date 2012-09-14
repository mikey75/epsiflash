/*
 * This file is a part of the EpsiFlash Copyright (c) 2000-2007 Michal
 * Szwaczko, WireLabs Systems contact: mikey@wirelabs.net 
 */

/*#include <ctype.h>*/
#include "chips.h"
#include "hardware.h"

extern struct chiptable chips[];
extern unsigned char control;
extern void generic_probe(void);
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


char *filename;

int
main(int argc, char *argv[])
{


    char *mode;

    if (argc == 1) { printf("Syntax: epsiflash erase|read|burn [filename]\n"); exit (1);}

    hardware_init();
    filename = argv[2];
    mode = argv[1];

    probe_chip();
    printf("Chip: %s - %dKB, %s\n", chips[chipindex].name, chips[chipindex].size / 1024, chips[chipindex].adapter);

    if (strcmp(chips[chipindex].name,"UNKNOWN")==0) {
        fprintf(stderr,"Chip is unsupported or not inserted properly\n");
        exit(-1);
    }


    if (strcmp(mode,"burn")==0) {
      program_chip();
      printf("\n");
      exit(1);
    }

    if (strcmp(mode,"read")==0) {
     read_chip(); 
     printf("\n");
     exit(1);
    }
    if (strcmp(mode, "erase")==0) {
     erase_chip();
     printf("\n");
     exit(1);
    }
    return 0;
}

void
msg_error(void)
{
fprintf(stderr, "ERROR ERROR ERROR\n");
}

void
quit(void)
{
    if (buffer != NULL)
	free(buffer);
    power_down();
    exit(1);
}


void
chip_selected(void)
{

    buffersize = chips[chipindex].size;
    pagesize = chips[chipindex].pagesize;

    if (buffer != NULL)
	free(buffer);
    buffer = malloc(buffersize);
    memset(buffer, 0xFF, buffersize);

}




void
file_open(void)
{

    int             fd;
    size_t          fsize;

	fd = open(filename, O_RDONLY);

	if (fd < 0) { printf("Error loading file '%s'\n",  filename);
        exit(-1);
	};

	if (buffer != NULL)
	    free(buffer);
	buffer = malloc(buffersize);
	memset(buffer, 0xFF, buffersize);

	fsize = read(fd, buffer, buffersize);




}



void
file_save(void)
{

    int             fd;
    size_t          f;


	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC,
		  S_IWUSR | S_IRUSR);

	if (fd < 0) {
	printf(			       "Error saving file '%s'\n",filename);
	exit(-1);
	};

	f = write(fd, buffer, buffersize);
}


void
show_progress(long unsigned int adr, const char *mode)
{
      printf("\r");
      printf("[%s] Address: 0x%08lX                    ", mode, adr);
      fflush(stdout); 
}

void
read_chip(void)
{
    int             r;

    power_up();
    r = chips[chipindex].read_func(buffersize, pagesize);
    file_save(); 
    power_down();
}

void
erase_chip(void)
{

    int             r,
                    b;

    power_up();
    r = chips[chipindex].erase_func(buffersize, pagesize);
    b = generic_blank_check(buffersize, pagesize);
    power_down();

    if (r < 0 || b < 0)
	msg_error();

}

void
program_chip(void)
{

    int             r,
                    b,
                    v;
    file_open();
    power_up();
    chips[chipindex].erase_func(buffersize, pagesize);
    b = generic_blank_check(buffersize, pagesize);
    r = chips[chipindex].burn_func(buffersize, pagesize);
    v = generic_verify(buffersize, pagesize);
    power_down();
    if (r < 0 || b < 0 || v < 0)
	msg_error();
}

int
probe_chip (void)
{

  int i;
  power_up ();
  for (i = 0; chips[i].name != NULL; i++)
    {
      chips[i].probe_func();
      if ((chips[i].id1 == id1) && (chips[i].id2 == id2))
	{
          chipindex = i;
          chip_selected ();
	  power_down ();
	  return 1;
	};
    };
  chipindex = i - 1;
  power_down ();
  return -1;
}
