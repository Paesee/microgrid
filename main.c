#include "serial.h"
#include "crc16.h"
#include "projeto_1.h"
#include <string.h>

int main(void) {
  SerialPort s; // declara a porta serial
  char input; // declara a variável para armazenar o input do usuário
  int n1 = 0, n2 = 0, i = 0, aux = 0, resp_size = 0; // declara as variáveis usadas no programa
  char * resp = 0; // declara um ponteiro para a resposta do escravo
  clock_t t1,t2; // variaveis marcadores de tempo
  long millis = 0, sum_millis = 0;

  // inicia a porta serial
  if (!serialOpen(&s, PORTA_COM, BAUDRATE, 8, PARIDADE, STOPBITS)){
    printf("erro de inicializacao.\n");
    system("PAUSE");
    return -1;
  }

  // laço principal do programa
  while (input != '0') {

    // exibe o menu de opções para o usuário
    printf(" (1) - ler entrada analogica             \n");
    printf(" (2) - ler todas entradas analogicas     \n");
    printf(" (3) - enviar quadro com erro            \n");
    printf(" (4) - enviar quadro com funcao invalida \n");
    printf(" (5) - enviar quadro com reg invalido    \n");
    printf(" (6) - enviar quadro com CRC invalido    \n");
    printf(" (7) - calcular tempo de resposta        \n");
    printf(" (0) - encerrar o programa               \n\n");
    printf(" informe o comando: ");
    fflush(stdin);
    scanf("%c", &input);

    // executa a opção escolhida pelo usuário
    switch (input) {
    case '1':

      // envia um quadro requisitando ler de An até An
      printf(" informe a porta analogica: ");
      fflush(stdin);
      scanf("%d", &n1); // define o número da primeira porta analógica
      printf(" informe a quantidade de portas: ");
      fflush(stdin);
      scanf("%d", &n2); // define a quantidade de portas analógicas a serem lidas
      // resp recebe um ponteiro para um vetor que contém os dados ou a mensagem de erro
      resp = readInputRegister(&s, SLAVE_ADDRESS, INPUT_REGISTER_START_ADDRESS + (n1 << 1), n2, input);
      resp_size = 3 + 2 * n2 + 2; // tamanho da resposta esperada
      displayInfo(resp, resp_size, n1); // funcao para exibir os dados
      Sleep(2500);
      break;

    case '2':

      // envia um quadro requisitando a leitura de todas analogicas
      n1 = 0; // define o número da primeira porta analógica
      n2 = 6; // define a quantidade de portas analógicas a serem lidas
      // resp recebe um ponteiro para um vetor que contém os dados ou a mensagem de erro
      resp = readInputRegister(&s, SLAVE_ADDRESS, INPUT_REGISTER_START_ADDRESS + (n1 << 1), n2, input);
      resp_size = 3 + 2 * n2 + 2; // tamanho da resposta esperada
      displayInfo(resp, resp_size, n1); // funcao para exibir os dados
      Sleep(2500);
      break;

    case '3':

      // envia um quadro que contém o endereço errado do escravo
      n1 = 0; // define o número da primeira porta analógica
      n2 = 6; // define a quantidade de portas analógicas a serem lidas
      // resp recebe um ponteiro para um vetor que contém os dados ou a mensagem de erro
      resp = readInputRegister( & s, SLAVE_ADDRESS, INPUT_REGISTER_START_ADDRESS + (n1 << 1), n2, input);
      resp_size = 3 + 2 * n2 + 2; // tamanho da resposta esperada
      displayInfo(resp, resp_size, n1); // funcao para exibir os dados
      Sleep(2500);
      break;

    case '4':

      //envia um quadro com o codigo de funcao invalido
      n1 = 0;
      n2 = 6;
      // resp recebe um ponteiro para um vetor que contém os dados ou a mensagem de erro
      resp = readInputRegister( & s, SLAVE_ADDRESS, INPUT_REGISTER_START_ADDRESS + (n1 << 1), n2, input);
      resp_size = 3 + 2 * n2 + 2; // tamanho da resposta esperada
      displayInfo(resp, resp_size, n1); // funcao para exibir os dados
      Sleep(2500);
      break;

    case '5':

      // envia um quadro que contém o endereço do registrador invalido
      n1 = 0; // define o número da primeira porta analógica
      n2 = 6; // define a quantidade de portas analógicas a serem lidas
      // resp recebe um ponteiro para um vetor que contém os dados ou a mensagem de erro
      resp = readInputRegister( & s, SLAVE_ADDRESS, INPUT_REGISTER_START_ADDRESS + (n1 << 1), n2, input);
      resp_size = 3 + 2 * n2 + 2; // tamanho da resposta esperada
      displayInfo(resp, resp_size, n1); // funcao para exibir os dados
      Sleep(2500);
      break;

    case '6':

      // envia um quadro que contém o crc incorreto
      n1 = 0; // define o número da primeira porta analógica
      n2 = 6; // define a quantidade de portas analógicas a serem lidas
      // resp recebe um ponteiro para um vetor que contém os dados ou a mensagem de erro
      resp = readInputRegister( & s, SLAVE_ADDRESS, INPUT_REGISTER_START_ADDRESS + (n1 << 1), n2, input);
      resp_size = 3 + 2 * n2 + 2; // tamanho da resposta esperada
      displayInfo(resp, resp_size, n1); // funcao para exibir os dados
      Sleep(2500);
      break;

    case '7':
      
      for(i = 0, sum_millis = 0;  i<10; i++){
        t1 = clock(); // tempo da CPU no inicio do processamento
        // envia um quadro requisitando ler todas analogicas
        n1 = 0; // define o número da primeira porta analógica
        n2 = 6; // define a quantidade de portas analógicas a serem lidas
        // resp recebe um ponteiro para um vetor que contém os dados ou a mensagem de erro
        resp = readInputRegister( & s, SLAVE_ADDRESS, INPUT_REGISTER_START_ADDRESS + (n1 << 1), n2, input);
        t2 = clock(); // tempo da CPU no final do recebimento
        millis = (t2 - t1) * (1000.0 / CLOCKS_PER_SEC); // converte para milisegundos
        sum_millis += millis;
      }
      printf(" tempo de resposta: %.2f ms\n", sum_millis/10.0);
      Sleep(2500);
      break;

    case '0':
      printf("encerrando o programa...");
      break;
    }
    
    system("cls");
  }

  system("PAUSE");
  return 0;
}