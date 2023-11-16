#ifndef MICROGRID_H
#define MICROGRID_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* CIRCULAR BUFFER */

typedef struct
{
  float *buff;
  int size;
  int start;
  int end;
  int count;
} ug_CircularBuffer;

int ug_circularBufferInit(ug_CircularBuffer *cb, int size);
int ug_circularBufferIsEmpty(const ug_CircularBuffer *cb);
int ug_circularBufferIsFull(const ug_CircularBuffer *cb);
void ug_add2CircularBuffer(ug_CircularBuffer *cb, float data);
float ug_getElement(ug_CircularBuffer *cb, int n);
void ug_circularBufferFree(ug_CircularBuffer *cb);
void ug_plotBufferFromStartToEnd(ug_CircularBuffer *cb);

/* MICROGRID CONTROLLER */

#define ISLANDED 1
#define TRANSITION2TIED 2
#define TIED2GRID 3
#define TRANSITION2ISLANDED 4

#define MAX_THETA_DIF 0.5
#define MAX_VOLTAGE_DIF 2

typedef struct
{
  // status of the microgrid according to const defined above
  int ugrid_status;
  // buffer to store last phase errors
  ug_CircularBuffer delta_theta;
  float delta_theta_sum;
  // buffer to store last voltage errors
  ug_CircularBuffer delta_v;
  float delta_v_sum;
  // factor to calculate average, this variable allow to execute only one division 
  float buff_size_factor;
} uGridController;

// initialization functions
void uGridControllerInit(uGridController *self, int size);
// setting request between microgrid modes
void setTransition2Tied(uGridController *self);
void setTransition2Islanded(uGridController *self);
// adding new values to its buffers
void addDeltaTheta(uGridController *self, float delta_theta);
void addDeltaV(uGridController *self, float delta_v);
// tie/untie handlers
int tieHandler(uGridController *self, float delta_theta, float delta_v);
int untieHandler(uGridController *self);

#endif