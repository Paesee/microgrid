#include "serial.h"
#include "crc16.h"
#include "projeto_1.h"

// função que lê os registradores de entrada do dispositivo escravo
// ela envia uma solicitação para o dispositivo e espera a resposta.
char * readInputRegister(SerialPort * s, int address, int start_address, int n, char test) {
  char req[8];
  char * resp = 0;

  // monta a requisição para ser enviada
  req[0] = address; // endereço do dispositivo escravo
  req[1] = 0x04; // código da função - 0x04 é para ler registradores de entrada
  req[2] = start_address >> 8; // endereço inicial dos registradores
  req[3] = start_address & 0xFF; // endereço inicial dos registradores
  req[4] = n >> 8; // quantidade de registradores a serem lidos
  req[5] = n & 0xFF; // quantidade de registradores a serem lidos

  // as próximas linhas servem para simular diferentes tipos de erros
  // apenas para fins de teste. não são necessárias para o funcionamento normal da função
  if (test == '3')
    req[0] = 0x01; // muda o endereço do dispositivo escravo
  if (test == '4')
    req[1] = 0x01; // muda o código da função para um valor inválido
  if (test == '5')
    req[2] = 0xFF; // muda o endereço inicial dos registradores para um valor inválido
  if (test == '6')
    req[6] = req[6] + 1; // altera o valor do CRC para introduzir um erro

  // calcula o CRC da mensagem e adiciona ao final do array de bytes
  int crc = CRC16(req, 6);
  req[6] = crc & 0xFF;
  req[7] = crc >> 8;

  // aloca memória para a resposta
  int resp_size = 3 + 2 * n + 2; // tamanho da resposta esperada
  if (resp != NULL)
    free(resp);
  resp = (char * ) malloc(resp_size * sizeof(char));

  // envia a requisição para o dispositivo
  serialPutBytes(s, req, 8);

  // recebe a resposta do dispositivo
  int tamanhoResposta = recebeResposta(s, resp, BAUDRATE);

  // calcula o CRC da resposta recebida
  crc = CRC16(resp, tamanhoResposta);

  // checa alguns erros comuns na resposta recebida
  if (tamanhoResposta == 0) //  se não houver resposta
    return ERR_NO_RESPONSE;

  if (crc != 0) //  se o CRC estiver incorreto
    return ERR_CRC_FAILED;

  if (resp[0] != SLAVE_ADDRESS) //  se o endereço do dispositivo escravo na resposta for diferente do esperado
    return ERR_INVALID_ADDRESS;

  if (resp[1] != 0x04) //  se o código da função na resposta for diferente de 0x04
    return ERR_INVALID_FUNCTION;

  if (tamanhoResposta < 7) { // se tamanhoResposta < 7, então a resp informa um erro
    if (resp[2] == 0x01) // se o código de exceção for 0x01, retorna erro de função inválida
      return ERR_INVALID_FUNCTION;
    if (resp[2] == 0x02) // se o código de exceção for 0x02, retorna erro de registro inválido
      return ERR_INVALID_REG;
  }

  return resp; // retorna a resposta recebida caso todos os testes de integridade tenham passado
}

void displayInfo(char * resp, int resp_size, int n1) {

  if (resp == ERR_NO_RESPONSE) { // verifica se a resposta é ERR_NO_RESPONSE
    printf(" sem resposta do escravo.\n");
  } else if (resp == ERR_CRC_FAILED) { // verifica se a resposta é ERR_CRC_FAILED
    printf(" o CRC esta incorreto.\n");
  } else if (resp == ERR_INVALID_ADDRESS) { // verifica se a resposta é ERR_INVALID_ADDRESS
    printf(" endereco de escravo invalido.\n");
  } else if (resp == ERR_INVALID_FUNCTION) { // verifica se a resposta é ERR_INVALID_FUNCTION
    printf(" codigo de funcao invalido.\n");
  } else if (resp == ERR_INVALID_REG) { // verifica se a resposta é ERR_INVALID_REG
    printf(" registrador invalido.\n");
  } else if (resp == ERR_UNKNOWN) { // verifica se a resposta é ERR_UNKNOWN
    printf(" erro desconhecido.\n");
  } else { // se a resposta for válida, exibe as informações
    for (int i = 3; i < (resp_size - 2); i = i + 2) {
      int aux = (resp[i] << 8) | (0xFF & resp[i + 1]);
      printf("\n A%d: %.2f V", (n1 + (i - 3) / 2), aux / 100.0);
    }
  }
}