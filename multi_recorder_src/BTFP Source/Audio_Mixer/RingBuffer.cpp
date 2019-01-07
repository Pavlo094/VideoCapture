#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuffer.h"

#define MODULE_ID RINGBUFFER


/* call before output thread is active !!!!! */
int
rb_init (struct OutRingBuffer **rb, int size)
{
    struct OutRingBuffer *ring;

    if(rb==NULL || size <= 1024)
    {
        return 0;
    }
    
	
    ring = (OutRingBuffer*)malloc (sizeof (struct OutRingBuffer));
    if(ring == NULL)
    {
        return 0;
    }
    memset (ring, 0, sizeof (struct OutRingBuffer));

    ring->size = 1;
    while(ring->size <= size)
        ring->size <<= 1;


    ring->rd_pointer = 0;
    ring->wr_pointer = 0;
    ring->buffer = (char*)malloc(sizeof(char)*(ring->size));
    
    *rb = ring;



    return 1;
}


int
rb_write (struct OutRingBuffer *rb, unsigned char * buf, int len)
{
    int total;
    int i;

    /* total = len = min(space, len) */
    total = rb_free(rb);
    if(len > total)
        len = total;
    else
        total = len;

    i = rb->wr_pointer;
    if(i + len > rb->size)
    {
        memcpy(rb->buffer + i, buf, rb->size - i);
        buf += rb->size - i;
        len -= rb->size - i;
        i = 0;
    }
    memcpy(rb->buffer + i, buf, len);
    rb->wr_pointer = i + len;
    return total;

        
}

int
rb_free (struct OutRingBuffer *rb)
{
    return (rb->size - 1 - rb_data_size(rb));
}


int
rb_read (struct OutRingBuffer *rb, unsigned char * buf, int max)
{
    int total;
    int i;
    /* total = len = min(used, len) */
    total = rb_data_size(rb);

    if(max > total)
        max = total;
    else
        total = max;

    i = rb->rd_pointer;
    if(i + max > rb->size)
    {
        memcpy(buf, rb->buffer + i, rb->size - i);
        buf += rb->size - i;
        max -= rb->size - i;
        i = 0;
    }
    memcpy(buf, rb->buffer + i, max);
    rb->rd_pointer = i + max;

    return total;

}

int
rb_data_size (struct OutRingBuffer *rb)
{
    return ((rb->wr_pointer - rb->rd_pointer) & (rb->size-1));
}


int
rb_clear (struct OutRingBuffer *rb)
{
    memset(rb->buffer,0,rb->size);
    rb->rd_pointer=0;
    rb->wr_pointer=0;

    return 0;
}