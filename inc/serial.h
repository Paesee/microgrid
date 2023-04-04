/*
  Biblioteca para acesso a porta serial no windows
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#ifndef SERIAL_H 
#define SERIAL_H

// definicoes
#define BUF_SIZE    1024  // tamanho dos buffers 
#define TIMEOUT     300   // timeout de recepcao e envio, em ms

// estrutura com variaveis da serial
typedef struct {
  char *serialPortName[20];   // nome da porta      
  HANDLE hCom;                // handler da porta
  BOOL bPortReady;            // indica se a porta esta pronta
  DCB dcb;                    // guarda configuracoes da porta
  COMMTIMEOUTS CommTimeouts;  // timeouts
  DWORD iBytesWritten;        // indica quantos bytes foram escritos na ultima escrita
  DWORD iBytesRead;           // indica quantos bytes foram lidos na ultima recepcao
  COMSTAT comStat;            // status da serial
  DWORD   dwErrors;           // variavel para guardar erros
} SerialPort;

// funcoes da serial
extern int serialOpen(SerialPort *s, char *port, int baudRate, BYTE databits, BYTE parity, BYTE stopbits);      // abre a porta de comunicacao
extern int serialClose(SerialPort *s);                             // fecha a porta
extern int serialPutc(SerialPort *s, char txchar);                 // poe um caractere
extern int serialPutBytes(SerialPort *s,char *buffer, int length); // poe bytes de um buffer
extern int serialPuts(SerialPort *s,char *string);                 // poe uma string
extern int serialBytesReceived(SerialPort *s);                     // ve quantos bytes estao no buffer
extern void serialBufferClear(SerialPort *s);                      // limpa o buffer
extern int serialBytesOut(SerialPort *s);                          // bytes no buffer de saida
extern int serialGetLine(SerialPort *s,char* output);              // pega uma string
extern int serialGetcTimeout(SerialPort *s, char* c);              // pega um caractere, retorna se houve timetout
extern unsigned char serialGetc(SerialPort *s);                    // pega um caractere
extern int recebeResposta(SerialPort *s, char *resp, int baudrate);// recebe uma resposta modbus
extern void exibeDados(unsigned char *data, unsigned short length);

#endif
