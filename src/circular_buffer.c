#include "circular_buffer.h"

int circularBufferInit(CircularBuffer *cb, uint8_t size)
{
  cb->buff = (int16_t *)malloc(sizeof(int16_t) * size);
  if (sizeof(cb->buff) == 0)
    return -1;
  cb->size = size;
  cb->start = 0;
  cb->end = 0;
  cb->count = 0;
  return 0;
}

int circularBufferIsEmpty(const CircularBuffer *cb)
{
  return cb->size == 0;
}

int circularBufferIsFull(const CircularBuffer *cb)
{
  return cb->count == cb->size;
}

void add2CircularBuffer(CircularBuffer *cb, int16_t data)
{
  cb->buff[cb->end] = data;
  cb->end = (cb->end + 1) % cb->size;
  if (cb->count == cb->size)
    cb->start = (cb->start + 1) % cb->size;
  else
    cb->count++;
}

int16_t getElement(CircularBuffer *cb, int n)
{
  if (n >= cb->count || n < 0)
    return 0xFFFF;
  int index = (cb->start + n) % cb->size;
  return cb->buff[index];
}

void circularBufferFree(CircularBuffer *cb)
{
  free(cb->buff);
}

void plotBufferFromStartToEnd(CircularBuffer *cb)
{
  int i;
  int count = cb->count;
  if (count == 0)
  {
    printf("Buffer is empty.\n");
    return;
  }
  printf("Buffer content:\n");
  for (i = cb->start; count > 0; i = (i + 1) % cb->size)
  {
    printf("%d ", cb->buff[i]);
    count--;
  }
  printf("\n");
}