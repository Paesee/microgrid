#ifndef CIRCULAR_BUFFER // diretiva de inclusão condicional para evitar múltiplas inclusões do mesmo arquivo de cabeçalho
#define CIRCULAR_BUFFER

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>

typedef struct{
  int16_t* buff;
  uint8_t  size;
  uint8_t  start;
  uint8_t  end;  
  uint8_t  count;  
} CircularBuffer;

int16_t getElement(CircularBuffer *vb, int n);
int circularBufferInit(CircularBuffer* vb, uint8_t length);
int circularBufferIsEmpty(const CircularBuffer* vb);
int circularBufferIsFull(const CircularBuffer* vb);
void add2CircularBuffer(CircularBuffer* vb, int16_t voltage);
void circularBufferFree(CircularBuffer* vb);
void plotBufferFromStartToEnd(CircularBuffer *vb);

#endif // fecha a diretiva de inclusão condicional