#include "echal_circFIFO64_k.h"
#include <stdint.h>

uint8_t circFIFO_init(circFIFO64k_t *fifo, uint8_t *buffer, uint16_t size)
{
    fifo->head = 0;
    fifo->tail = 0;
    fifo->free = size;
    fifo->size = size;
    fifo->data = buffer;
    return 0;
}
uint16_t circFIFO_push(circFIFO64k_t *fifo, const uint8_t *data,
                       uint16_t length)
{
    uint16_t i;
    for (i = 0; i < length; i++)
    {
        fifo->data[fifo->head++] = data[i];
        if (fifo->head >= fifo->size)
            fifo->head = 0;
        fifo->free--;
    }
    return i;
}
uint16_t circFIFO_pop(circFIFO64k_t *fifo, uint8_t *dest, uint16_t length)
{
    uint16_t i;
    for (i = 0; i < length; i++)
    {
        if (fifo->free == fifo->size)
            break;
        dest[i] = fifo->data[fifo->tail++];
        if (fifo->tail >= fifo->size)
            fifo->tail = 0;
        fifo->free++;
    }
    return i;
}
uint16_t circFIFO_peek(circFIFO64k_t *fifo, uint8_t *dest, uint16_t length)
{
    uint16_t fifoFree = fifo->free;
    uint16_t fifoTail = fifo->tail;
    uint16_t ret_val = circFIFO_pop(fifo, dest, length);
    fifo->free = fifoFree;
    fifo->tail = fifoTail;
    return ret_val;
}
uint16_t circFIFO_size_free(circFIFO64k_t *fifo)
{
    return fifo->free;
}
uint16_t circFIFO_size_used(circFIFO64k_t *fifo)
{
    return fifo->size - fifo->free;
}
uint8_t circFIFO_isEmpty(circFIFO64k_t *fifo)
{
    return fifo->free == fifo->size;
}
uint8_t circFIFO_isFull(circFIFO64k_t *fifo)
{
    return fifo->free == 0;
}
void circFIFO_flush(circFIFO64k_t *fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
    fifo->free = fifo->size;
}
