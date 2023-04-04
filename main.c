// bibliotecas de funcoes
#include <string.h>
#include "serial.h"
#include "crc16.h"

// configuracao de uma porta COM
#define PORTA_COM "\\\\.\\COM4"		// porta de comunicao
#define BAUDRATE 19200				// taxa em bps
#define PARIDADE NOPARITY			// paridade (NOPARITY, PARITY_EVEN, PARITY_ODD
#define STOPBITS TWOSTOPBITS		// stop bits (ONESTOPBIT, TWOSTOPBITS, ONE5STOPBITS)

// metodo principal
int main(void)
{  
  clock_t t1,t2;  
  SerialPort s;  

  int x = 0;

  if(!serialOpen(&s, PORTA_COM,BAUDRATE,8,PARIDADE,STOPBITS))
  {
      printf("Erro de inicializacao.\n");
  }
  
  x = 10;

  x = 11;

  return 0;
}