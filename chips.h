
#ifndef __CHIPS_H__
#define __CHIPS_H__ 1


unsigned int            id1,id2;

int     generic_read (unsigned int , unsigned short int );
int     generic_blank_check (unsigned int , unsigned short int );
int     generic_verify (unsigned int , unsigned short int );
void    generic_probe (void);
int     generic_erase (unsigned int , unsigned short int );

void    DQ6_toggle (unsigned long );
void    check_dq6 (unsigned long );
void    check_dq7 (unsigned long , unsigned char);

int     jedec_page_write (unsigned int , unsigned short int );
int     jedec_byte_write (unsigned int , unsigned short int );

int     intel28f_write (unsigned int , unsigned short int );
int     intel28f_erase (unsigned int , unsigned short int );

int     mx28f_erase (unsigned int , unsigned short int );
int     mx28f_write (unsigned int , unsigned short int );

int     at49f_erase (unsigned int , unsigned short int );
int     at49f_write (unsigned int , unsigned short int );

int     am29f_erase (unsigned int , unsigned short int );
int     am29f_write (unsigned int , unsigned short int );

void    w29_probe(void);
void    mx28f1000_probe(void);
void    i28f001bxt_probe(void);

void    dummy_probe(void);
int    dummy_read(unsigned int , unsigned short int );
int    dummy_write(unsigned int , unsigned short int );
int    dummy_erase(unsigned int , unsigned short int );

struct chiptable {

    const char          *name;
    unsigned int        id1;
    unsigned int        id2;
    unsigned int        size;
    unsigned short int  pagesize;
    const char          *adapter;

    void                (*probe_func)();
    int                 (*read_func) (unsigned int , unsigned short int );
    int                 (*burn_func) (unsigned int, unsigned short int );
    int                 (*erase_func) (unsigned int ,unsigned short int );
};


#endif /* __CHIPS_H__ */