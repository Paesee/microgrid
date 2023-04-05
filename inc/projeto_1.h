#include "serial.h"
#include "crc16.h"
#include <string.h>

#ifndef PROJETO_1 // diretiva de inclusão condicional para evitar múltiplas inclusões do mesmo arquivo de cabeçalho
#define PROJETO_1

#define PORTA_COM "\\\\.\\COM8" // define a porta serial a ser utilizada
#define BAUDRATE 9600 // define a velocidade da comunicação serial em baud rate
#define PARIDADE NOPARITY // define a paridade da comunicação serial como nenhuma
#define STOPBITS TWOSTOPBITS // define o número de bits de parada da comunicação serial como dois
#define INPUT_REGISTER_START_ADDRESS 0x1000 // define o endereço inicial dos registradores de entrada
#define SLAVE_ADDRESS 0x02 // define o endereço do dispositivo escravo

#define ERR_NO_RESPONSE "No response received" // define uma mensagem de erro para quando não houver resposta
#define ERR_CRC_FAILED "CRC check failed" // define uma mensagem de erro para quando a verificação de CRC falhar
#define ERR_INVALID_ADDRESS "Invalid slave address" // define uma mensagem de erro para quando o endereço do escravo for inválido
#define ERR_INVALID_FUNCTION "Invalid function code" // define uma mensagem de erro para quando o código de função for inválido
#define ERR_INVALID_REG "Invalid register" // define uma mensagem de erro para quando o registrador for inválido
#define ERR_UNKNOWN "Unknown error" // define uma mensagem de erro genérica

char * readInputRegister(SerialPort * s, int address, int start_address, int n, char test); // declara a função readInputRegister para leitura de registradores de entrada
void displayInfo(char * resp, int resp_size, int n1); // declara a função displayInfo para exibição de informações

#endif // fecha a diretiva de inclusão condicional