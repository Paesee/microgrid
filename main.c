#include "teste.h"

int main()
{
  objectA_t *objetoA = objectA_creator(5, 10);
  objectB_t *objetoB = objectB_creator(5, 5);

  printf("Diretamente: \n\n");
  printf("Resultado da soma: %d\n", objetoA->sumAB(objetoA->self));
  printf("Resultado do produto: %d", objetoB->productAB(objetoB->self));

  objectC_t *objetoC = objectC_creator();
  printf("\n Endereco do Objeto C no main: 0x%X\n\n\n", objetoC);

  printf("Encapsulado em objeto C: \n\n");
  printf("Resultado da soma: %d\n", objetoC->objectA->sumAB(objetoC->objectA));
  printf("Resultado do produto: %d\n", objetoC->objectB->productAB(objetoC->objectB));
  printf("Media: %d\n", objetoC->averageAB(objetoC->self));

  printf("\n\nEncapsulado em objeto C: \n\n");
  printf("Resultado da soma: %d\n", objetoC->objectA->sumAB(objetoC->objectA));
  printf("Resultado do produto: %d\n", objetoC->objectB->productAB(objetoC->objectB));
  printf("Media: %d\n", objetoC->averageAB(objetoC->self));
  printf("TESTE: 0x%X\n", objetoC->self);

  while(1);
}