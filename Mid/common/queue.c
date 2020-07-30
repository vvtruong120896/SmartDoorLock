#include "queue.h"
#include <string.h>


void  QUEUE_Init(QUEUEx_t *q,u8* buffer,u8 queue_size, u8 element_size)
{
    q->p=buffer;
    q->count=0;
    q->in=0;
    q->out=0;
    q->element_size=element_size;
    q->queue_size  =queue_size;

}
u8  QUEUE_Push(QUEUEx_t *q,u8* data)
{

    if(QUEUE_Count(q)>=q->queue_size)
    {       
        if(q->out>=((q->element_size)*(q->queue_size)))
        {
            q->out=0;
        }

        q->out+=q->element_size;        
        q->count--;
    }

    if(q->in>=((q->element_size)*(q->queue_size)))
    {
        q->in=0;
    }

    memcpy((u8*)&(q->p[q->in]),(u8*)data,q->element_size);
    q->in +=q->element_size;

    q->count++;

    return 1;
}


u8 QUEUE_Get(QUEUEx_t *q, u8* buffer)
{
    if(QUEUE_Count(q))
    {
        if(q->out>=((q->element_size)*(q->queue_size)))
        {
            q->out=0;
        }

        memcpy((u8*)buffer,(u8*)&(q->p[q->out]),q->element_size);
        q->out+=q->element_size;        
        q->count--;

        return 1;
    }

    return 0;
}

u8  QUEUE_Empty(QUEUEx_t *q)
{
    if(q->count)
    {
        return 0;
    }

    return 1;
}

u8 QUEUE_Count(QUEUEx_t *q)
{
    return q->count;
}
