#include "serial.h"

int serialOpen(SerialPort *s, char *port, int baudRate, BYTE databits, BYTE parity, BYTE stopbits)
{   
    // gera um arquivo para acesso a porta com
    s->hCom = CreateFile(port,GENERIC_READ|GENERIC_WRITE,
    0,          // exclusive access
    NULL,       // no security
    OPEN_EXISTING,
    0,          // no overlapped I/O
    NULL);      // null template

    // testa se abriu o arquivo    
    if(s->hCom == INVALID_HANDLE_VALUE) return 0;
    
    // configura a porta com os parametros de abertura
    s->bPortReady        = SetupComm(s->hCom, BUF_SIZE, BUF_SIZE); // set buffer sizes read,write
    s->bPortReady        = GetCommState(s->hCom, &s->dcb);
    s->dcb.BaudRate      = baudRate;
    s->dcb.ByteSize      = databits; 
    s->dcb.Parity        = parity;
    s->dcb.StopBits      = stopbits;// ONESTOPBIT;
    s->dcb.fAbortOnError = TRUE;
    
    // ativar controle de fluxo por XON/XOFF
    s->dcb.fOutX = FALSE; // XON/XOFF off for transmit
    s->dcb.fInX  = FALSE; // XON/XOFF off for receive
    // ativar controle de fluxo por RTS/CTS
    s->dcb.fOutxCtsFlow = FALSE; // turn on CTS flow control
    s->dcb.fRtsControl  = FALSE;//RTS_CONTROL_HANDSHAKE; //
    // ativa controle de fluxo por DTR/DSR
    s->dcb.fOutxDsrFlow = FALSE; // turn on DSR flow control
    s->dcb.fDtrControl  = FALSE; //DTR_CONTROL_ENABLE; //
    // dcb.fDtrControl = DTR_CONTROL_DISABLE; //
    // dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; //
    
    // ativa
    s->bPortReady = SetCommState(s->hCom, &s->dcb);
    
    // ativa os timeouts
    s->bPortReady = GetCommTimeouts (s->hCom, &s->CommTimeouts);    
    s->CommTimeouts.ReadIntervalTimeout          = TIMEOUT; 
    s->CommTimeouts.ReadTotalTimeoutConstant     = TIMEOUT;
    s->CommTimeouts.ReadTotalTimeoutMultiplier   = TIMEOUT;
    s->CommTimeouts.WriteTotalTimeoutConstant    = TIMEOUT;
    s->CommTimeouts.WriteTotalTimeoutMultiplier  = TIMEOUT;    
    s->bPortReady = SetCommTimeouts (s->hCom, &s->CommTimeouts);

    // se tudo deu certo, retorna true
    return 1;
}

//recebe um caractere
unsigned char serialGetc(SerialPort *s)                 
{
   unsigned char rxchar;
  	if(!ReadFile(s->hCom, &rxchar, 1, &s->iBytesRead, NULL))
  	{
  		return 0;
  	}
   return rxchar;
}

//le um caractere e armazena em c, retorna 0 se houve timeout
int serialGetcTimeout(SerialPort *s, char* c)                 
{
  	ReadFile(s->hCom, c, 1, &s->iBytesRead, NULL);
   return s->iBytesRead;
}


//envia um caractere
int serialPutc(SerialPort *s,char txchar)    
{
    if(WriteFile(s->hCom, &txchar, 1, &s->iBytesWritten,NULL)) return 1;
    else return 0;
}

//poe uma string
int serialPuts(SerialPort *s, char *string)
{
    if(WriteFile(s->hCom, string, strlen(string), &s->iBytesWritten,NULL)) return 1;
    else return 0;
}

//poe um certo numero de bytes
int serialPutBytes(SerialPort *s, char *buffer, int length)
{
    if(WriteFile(s->hCom, buffer, length, &s->iBytesWritten,NULL)) return 1;
    else return 0;
}

//retorna o numero de bytes no buffer de entrada
int serialBytesReceived(SerialPort *s)
{
  int bytesReceived = 0;
	 if(ClearCommError(s->hCom, &s->dwErrors, &s->comStat)!=0)
	 {
		   bytesReceived = s->comStat.cbInQue;
	 }
	 return bytesReceived;
}

// le ou aguarda uma string do buffer
int serialGetLine(SerialPort *s, char* output){
    int i=0;
    int ready = 0;           // frase pronta
    while(!ready) {
       // testa se leu um caractere
       if(serialGetcTimeout(s, &output[i]))  
       {
          if(output[i]==10||output[i]==13){
             output[i+1] = '\0';
             ready = 1;                         
          }
          // printf("%d\n",i); // indica bytes recebidos
          i++;
       }
    }
    return 0;    
}

// encerra a serial
int serialClose(SerialPort *s)
{
    return CloseHandle(s->hCom);     
}

// limpa o buffer de recepcao
void serialBufferClear(SerialPort *s)
{
	   PurgeComm(s->hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);
}

// recebe uma resposta do escravo
int recebeResposta(SerialPort *s, char *resp, int baudrate)
{   
   int i = 0;
   char temp;
   clock_t millis = clock();

   // limpa o buffer de recepcao
   serialBufferClear(s);           

	// controle de fim de quadro RTU
   int bytesAvailable = 0;   
   int endFrame = 0; 
   int timeout = 0;
        
    // espera chegar um quadro inteiro ou dar TIMEOUT    
   	while(!endFrame) { 
	   
	   // se chegou, comeca a controlar o fim de quadro	 (tempo de 3,5 bytes)    
	   if(serialBytesReceived(s)){ 		
			
		   if(serialBytesReceived(s)!=bytesAvailable) {	   	
		   		bytesAvailable=serialBytesReceived(s);
		   		millis = clock(); 		   		
		   	}
		   else {
				
				if(((clock()-millis)*(1000.0 / CLOCKS_PER_SEC))>(3.5*11*1000)/baudrate) {
					endFrame = 1;
				}		     
		   }
		} else {
			Sleep(1);	// aguarda pelo menos 1 ms (vai incrementando o timeout)
			timeout++;
			if(timeout>TIMEOUT) break;
		}	
		// printf("%d\n",timeout);	
	}
	
	// printf("Bytes recebidos do escravo(hex): ");
	// copia o que foi recebido	 
	 while(serialBytesReceived(s)) {
	 	temp = serialGetc(s);
	 	resp[i] = temp;
	 	i++;
	    // printf("%02x ",temp&0xff);                              
	 }  
	 //printf("\n");
   
   return i;	// o numero de bytes recebidos      
}

// exibe os dados de uma mensagem
void exibeDados(unsigned char *data, unsigned short length)
{
	int i=0;
	for(i=0;i<length;i++){
		printf("%02x ",data[i]&0xff);
	}
	printf("\n");
}