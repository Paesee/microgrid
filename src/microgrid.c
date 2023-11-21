#include "microgrid.h"

/* CIRCULAR BUFFER FUNCTIONS IMPLEMENTATION */

int ug_circularBufferInit(ug_CircularBuffer *cb, int size)
{
  cb->buff = (float *)malloc(sizeof(float) * size);
  if (sizeof(cb->buff) == 0)
    return -1;
  cb->size = size;
  cb->start = 0;
  cb->end = 0;
  cb->count = 0;
  return 0;
}

int ug_circularBufferIsEmpty(const ug_CircularBuffer *cb)
{
  return cb->size == 0;
}

int ug_circularBufferIsFull(const ug_CircularBuffer *cb)
{
  return cb->count == cb->size;
}

void ug_add2CircularBuffer(ug_CircularBuffer *cb, float data)
{
  cb->buff[cb->end] = data;
  cb->end = (cb->end + 1) % cb->size;
  if (cb->count == cb->size)
    cb->start = (cb->start + 1) % cb->size;
  else
    cb->count++;
}

float ug_getElement(ug_CircularBuffer *cb, int n)
{
  if (n >= cb->count || n < 0)
    return 0xFFFF;
  int index = (cb->start + n) % cb->size;
  return cb->buff[index];
}

void ug_circularBufferFree(ug_CircularBuffer *cb)
{
  free(cb->buff);
}

void ug_plotBufferFromStartToEnd(ug_CircularBuffer *cb)
{
  int i;
  int count = cb->count;
  if (count == 0)
  {
    printf("Buffer is empty.\n");
    return;
  }
  printf("Buffer content:\n");
  for (i = cb->start; count > 0; i = (i + 1) % cb->size)
  {
    printf("%d ", cb->buff[i]);
    count--;
  }
  printf("\n");
}

void ug_clearCircularBuffer(ug_CircularBuffer *cb)
{
  int i = 0;
  cb->start = 0;
  cb->end = 0;
  cb->count = 0;
  if (cb->buff != NULL)
  {
    for (i = 0; i < cb->size; ++i)
    {
      cb->buff[i] = 0;
    }
  }
}

// MICROGRID CONTROLLER FUNCTIONS IMPLEMENTATION

// function to iniatilize uGridController
void uGridControllerInit(uGridController *self, int size)
{
  self->ugrid_status = ISLANDED;
  ug_circularBufferInit(&self->delta_theta, size);
  self->delta_theta_sum = 0;
  ug_circularBufferInit(&self->delta_v, size);
  self->delta_v_sum = 0;
  self->buff_size_factor = (float)1.0 / size;
}

// set transition to grid tied operation mode
void setTransition2Tied(uGridController *self)
{
  if (self->ugrid_status == ISLANDED)
    self->ugrid_status = TRANSITION2TIED;
}

// set transition to islanded operation mode
void setTransition2Islanded(uGridController *self)
{
  if (self->ugrid_status == TIED2GRID)
    self->ugrid_status = TRANSITION2ISLANDED;
}

// add a phase error to its buffer
void addDeltaTheta(uGridController *self, float delta_theta)
{
  float sub_value = 0;
  if (ug_circularBufferIsFull(&self->delta_theta))
    sub_value = ug_getElement(&self->delta_theta, 0);
  ug_add2CircularBuffer(&self->delta_theta, delta_theta);
  self->delta_theta_sum = self->delta_theta_sum + delta_theta - sub_value;
}

// add a voltage error to its buffer
void addDeltaV(uGridController *self, float delta_v)
{
  float sub_value = 0;
  if (ug_circularBufferIsFull(&self->delta_v))
    sub_value = ug_getElement(&self->delta_v, 0);
  ug_add2CircularBuffer(&self->delta_v, delta_v);
  self->delta_v_sum = self->delta_v_sum + delta_v - sub_value;
}

// handle request to tied to the grid
int tieHandler(uGridController *self, float delta_theta, float delta_v)
{
  if (self->ugrid_status == TRANSITION2TIED)
  {
    // add new values to its buffers
    addDeltaTheta(self, delta_theta);
    addDeltaV(self, delta_v);
    // calculate the average of both buffers if they are full
    if (ug_circularBufferIsFull(&self->delta_theta) && ug_circularBufferIsFull(&self->delta_v))
    {
      float average_delta_theta = self->delta_theta_sum * self->buff_size_factor;
      float average_delta_v = self->delta_theta_sum * self->buff_size_factor;
      // if voltage and phase angle are within the limits, the controller allows connection
      if (average_delta_theta < MAX_THETA_DIF && average_delta_v < MAX_VOLTAGE_DIF)
      {
        ug_clearCircularBuffer(&self->delta_theta);
        ug_clearCircularBuffer(&self->delta_v);
        self->ugrid_status = TIED2GRID;
        return 1;
      }
      // return 0 if its no the case above
      return 0;
    }
    // return 0 if buffer is not full, it prevents a connection in the start up of the buffer
    return 0;
  }
  return 0;
}

// handle request to island from grid
int untieHandler(uGridController *self)
{
  if (self->ugrid_status == TRANSITION2ISLANDED)
  {
    self->ugrid_status = ISLANDED;
    return 1;
  }
  return 0;
}