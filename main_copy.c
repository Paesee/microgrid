#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "circular_buffer.h"

#define F_GRID    60
#define F_CONTROL 20000 

int main() {
  // Calculate buffer size based on the grid and control frequencies
  uint8_t buffer_size = (uint8_t)ceil(0.25 * F_CONTROL / F_GRID);
  printf("aaaa: %d \n", buffer_size);
  // Initialize the VoltageBuffer with the calculated buffer size
  CircularBuffer buffer;
  circularBufferInit(&buffer, buffer_size);

  for (int i = 0; i < 10; i++) {
      add2CircularBuffer(&buffer, (i) * 10);
  }

  plotBufferFromStartToEnd(&buffer);

  printf("Elemento 0 ==> %d\n", getElement(&buffer, 0));
  printf("Elemento 1 ==> %d\n", getElement(&buffer, 1));
  printf("Elemento 2 ==> %d\n", getElement(&buffer, 2));
  printf("Elemento 3 ==> %d\n", getElement(&buffer, 3));
  printf("Elemento 4 ==> %d\n", getElement(&buffer, 4));
  printf("Elemento 5 ==> %d\n", getElement(&buffer, 5));

  for (int i = 0; i < 3; i++) {
    add2CircularBuffer(&buffer, (i + 10) * 10);
  }

  plotBufferFromStartToEnd(&buffer);

  printf("Elemento 0 ==> %d\n", getElement(&buffer, 0));
  printf("Elemento 1 ==> %d\n", getElement(&buffer, 1));
  printf("Elemento 2 ==> %d\n", getElement(&buffer, 2));
  printf("Elemento 3 ==> %d\n", getElement(&buffer, 3));
  printf("Elemento 4 ==> %d\n", getElement(&buffer, 4));
  printf("Elemento 5 ==> %d\n", getElement(&buffer, 5));

  circularBufferFree(&buffer);

  while(!kbhit()){}

  return 0;
}