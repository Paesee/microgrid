#ifndef CIRCULAR_BUFFER // diretiva de inclusão condicional para evitar múltiplas inclusões do mesmo arquivo de cabeçalho
#define CIRCULAR_BUFFER

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  int16_t *buff;
  uint8_t size;
  uint8_t start;
  uint8_t end;
  uint8_t count;
} CircularBuffer;

int16_t getElement(CircularBuffer *cb, int n);
int circularBufferInit(CircularBuffer *cb, uint8_t size);
int circularBufferIsEmpty(const CircularBuffer *cb);
int circularBufferIsFull(const CircularBuffer *cb);
void add2CircularBuffer(CircularBuffer *cb, int16_t voltage);
void circularBufferFree(CircularBuffer *cb);
void plotBufferFromStartToEnd(CircularBuffer *cb);

#endif // fecha a diretiva de inclusão condicional