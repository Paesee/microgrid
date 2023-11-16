#ifndef PLL_H
#define PLL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* SOGI QSG + FLL */

#define PI 3.14159265359
#define INT_GAIN 0.0001

typedef struct {
  float w0;
  float last_alpha_signal;
  float last_beta_signal;
  float last_w_meas;
  float last_w;
  float proportional_gain;
  float fll_gain;
} SOGIqsg;

void SOGIInit(SOGIqsg *sogi, float frequency, float p_gain, float f_gain);
void setCentralFrequency(SOGIqsg *sogi, float frequency);
void setProportionalGain(SOGIqsg *sogi, float p_gain);
void setFLLGain(SOGIqsg *sogi, float f_gain);
void executeSOGI(SOGIqsg *sogi, float meas_signal, float *alpha_signal, float *beta_signal, float *frequency);

/* SRF PLL */

#define SRF_TS     0.0005
#define SRF_GAIN_1 0.6627
#define SRF_GAIN_2 -0.6372
#define SRF_GAIN_3 -1

typedef struct
{
  float omega0;
  float last_vq;
  float last_pi_output;
  float last_sum;
  float last_theta;
} SRFpll;

void SRFpllInit(SRFpll *self, float w0);
void executeSRFpll(SRFpll *self, float x_alpha, float x_beta, float *output);
void resetSRFpll(SRFpll *self);

/* CIRCULAR BUFFER */

typedef struct
{
  float *buff;
  int size;
  int start;
  int end;
  int count;
} CircularBuffer;

int circularBufferInit(CircularBuffer *cb, int size);
int circularBufferIsEmpty(const CircularBuffer *cb);
int circularBufferIsFull(const CircularBuffer *cb);
void add2CircularBuffer(CircularBuffer *cb, float data);
float getElement(CircularBuffer *cb, int n);
void circularBufferFree(CircularBuffer *cb);
void plotBufferFromStartToEnd(CircularBuffer *cb);

/* RMS CALCULATOR */

typedef struct
{
  CircularBuffer square_buffer;
  int size;
  float sum;
} RMSCalculator;

int RMSCalculatorInit(RMSCalculator *rms, int size);
void add2RMSCalculator(RMSCalculator *rms, float data);
void calculateRMSvalue(RMSCalculator *rms, float *rms_value);
void RMSCalculatorFree(RMSCalculator *rms);

#endif