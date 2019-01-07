#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#define RINGBUFFER_MAGIC  0x72627546    /* 'rbuF' */


#define rb_magic_check(var,err)  {if(var->magic!=RINGBUFFER_MAGIC) return err;}



struct OutRingBuffer
{
//    int *vrb_buf;
//    vrb_p vrb_buf;
    char *buffer;
    int wr_pointer;
    int rd_pointer;
    long magic;
    int size;
};


/* ring buffer functions */
int rb_init (struct OutRingBuffer **, int);
int rb_write (struct OutRingBuffer *, unsigned char *, int);
int rb_free (struct OutRingBuffer *);
int rb_read (struct OutRingBuffer *, unsigned char *, int);

int rb_data_size (struct OutRingBuffer *);
int rb_clear (struct OutRingBuffer *);


#endif