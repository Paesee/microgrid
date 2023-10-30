#include "teste.h"

int objectA_sumAB(objectA_t *self)
{
  printf("\n Endereco do Objeto A: 0x%X\n", self);
  self->sum = (self->a + self->b);
  return self->sum;
}

objectA_t *objectA_creator(float a, float b)
{
  objectA_t *newObjectA = (objectA_t *)malloc(sizeof(objectA_t));
  newObjectA->self = newObjectA;
  newObjectA->a = a;
  newObjectA->b = b;
  newObjectA->sum = 0;
  newObjectA->sumAB = objectA_sumAB;
  return newObjectA;
}

void objectA_destructor(objectA_t *self)
{
  free(self);
}

int objectB_productAB(objectB_t *self)
{
  printf("\n Endereco do Objeto B: 0x%X\n", self);
  self->product = (self->a * self->b);
  return self->product;
}

objectB_t *objectB_creator(float a, float b)
{
  objectB_t *newObjectB = (objectB_t *)malloc(sizeof(objectB_t));
  newObjectB->self = newObjectB;
  newObjectB->a = a;
  newObjectB->b = b;
  newObjectB->product = 0;
  newObjectB->productAB = objectB_productAB;
  return newObjectB;
}

void objectB_destructor(objectB_t *self)
{
  free(self);
}

int objectC_averageAB(objectC_t *self)
{
  printf("\n Endereco do Objeto C: 0x%X\n", self);
  int sum = self->objectA->sumAB(self->objectA);
  int product = self->objectB->productAB(self->objectB);
  self->average = (sum + product) / 2.0;
  return self->average;
}

objectC_t *objectC_creator(void)
{
  objectC_t *newObjectC = (objectC_t *)malloc(sizeof(objectC_t));
  newObjectC->self = newObjectC;
  newObjectC->objectA = objectA_creator(5, 10);
  newObjectC->objectB = objectB_creator(5, 5);
  newObjectC->averageAB = objectC_averageAB;
  return newObjectC;
}

void objectC_destructor(objectC_t *self)
{
  objectA_destructor(self->objectA);
  objectB_destructor(self->objectB);
  free(self);
}