#ifndef teste_h
#define teste_h

#include <stdio.h>
#include <stdlib.h>

typedef struct objectA
{
  struct objectA *self;
  float a;
  float b;
  float sum;
  int (*sumAB)(struct objectA *self);
} objectA_t;

int objectA_sumAB(objectA_t *self);
objectA_t *objectA_creator(float a, float b);
void objectA_destructor(objectA_t *self);


typedef struct objectB
{
  struct objectB *self;
  float a;
  float b;
  float product;
  int (*productAB)(struct objectB *self);
} objectB_t;

int objectB_productAB(objectB_t *self);
objectB_t *objectB_creator(float a, float b);
void objectB_destructor(objectB_t *self);

typedef struct objectC
{
  struct objectA *objectA;
  struct objectB *objectB;
  struct objectC *self;
  float average;
  int (*averageAB)(struct objectC *self);
} objectC_t;

int objectC_averageAB(objectC_t *self);
objectC_t *objectC_creator(void);
void objectC_destructor(objectC_t *self);

#endif