#include "pll.h"

/* SOGI FUNCTIONS IMPLEMENTATION */

void SOGIInit(SOGIqsg *sogi, float frequency, float p_gain, float f_gain)
{
  sogi->w0 = 2 * PI * frequency;
  sogi->last_alpha_signal = 0;
  sogi->last_beta_signal = 0;
  sogi->last_w_meas = 0;
  sogi->last_w = sogi->w0;
  sogi->proportional_gain = p_gain;
  sogi->fll_gain = f_gain;
}

void setCentralFrequency(SOGIqsg *sogi, float frequency)
{
  sogi->w0 = 2 * PI * frequency;
}

void setProportionalGain(SOGIqsg *sogi, float p_gain)
{
  sogi->proportional_gain = p_gain;
}

void setFLLGain(SOGIqsg *sogi, float f_gain)
{
  sogi->fll_gain = f_gain;
}

void executeSOGI(SOGIqsg *sogi, float meas_signal, float *alpha_signal, float *beta_signal, float *frequency)
{
  // QSG execution
  float phase_error_1 = meas_signal - sogi->last_alpha_signal;
  float phase_error_2 = sogi->proportional_gain * phase_error_1 - sogi->last_beta_signal;

  float alpha = INT_GAIN * sogi->last_w * phase_error_2 + sogi->last_alpha_signal;
  float beta = INT_GAIN * sogi->last_w * alpha + sogi->last_beta_signal;

  // FLL execution
  float fll_input = -sogi->fll_gain * phase_error_1 * beta;
  float w_meas = INT_GAIN * fll_input + sogi->last_w_meas;
  float w = w_meas + sogi->w0;

  // assign values for the next execution
  sogi->last_alpha_signal = alpha;
  sogi->last_beta_signal = beta;
  sogi->last_w_meas = w_meas;
  sogi->last_w = w;

  // output value to the pointers
  *alpha_signal = alpha;
  *beta_signal = beta;
  *frequency = 0.15915494309 * w; // from rad/s to Hz
}

/* SRL PLL FUNCTIONS IMPLEMENTATION */

void SRFpllInit(SRFpll *self, float w0){
  self->omega0 = w0;
  self->last_vq = 0;
  self->last_pi_output = 0;
  self->last_sum = 0;
  self->last_theta = 0;
}

void executeSRFpll(SRFpll *self, float x_alpha, float x_beta, float *output){
  float sin_wt = sin(self->last_theta);
  float cos_wt = cos(self->last_theta);
  float vq = -sin_wt * x_alpha + cos_wt * x_beta;
  float pi_output = SRF_GAIN_1 * vq + SRF_GAIN_2 * self->last_vq - SRF_GAIN_3 * self->last_pi_output;
  float sum = self->omega0 + pi_output;
  float theta =  SRF_TS * sum + self->last_theta; 

  self->last_vq = vq;
  self->last_pi_output = pi_output;
  self->last_sum = sum;
  self->last_theta = theta;

  *output = theta;
}

void resetSRFpll(SRFpll *self)
{
  self->last_theta = 0;
}

/* CIRCULAR BUFFER FUNCTIONS IMPLEMENTATION */

int circularBufferInit(CircularBuffer *cb, int size)
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

int circularBufferIsEmpty(const CircularBuffer *cb)
{
  return cb->size == 0;
}

int circularBufferIsFull(const CircularBuffer *cb)
{
  return cb->count == cb->size;
}

void add2CircularBuffer(CircularBuffer *cb, float data)
{
  cb->buff[cb->end] = data;
  cb->end = (cb->end + 1) % cb->size;
  if (cb->count == cb->size)
    cb->start = (cb->start + 1) % cb->size;
  else
    cb->count++;
}

float getElement(CircularBuffer *cb, int n)
{
  if (n >= cb->count || n < 0)
    return 0xFFFF;
  int index = (cb->start + n) % cb->size;
  return cb->buff[index];
}

void circularBufferFree(CircularBuffer *cb)
{
  free(cb->buff);
}

void plotBufferFromStartToEnd(CircularBuffer *cb)
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

/* RMS CALCULATOR FUNCTIONS IMPLEMENTATION */

int RMSCalculatorInit(RMSCalculator *rms, int size)
{
  if (circularBufferInit(&rms->square_buffer, size))
    return -1;
  rms->size = size;
  rms->sum = 0;
}

void add2RMSCalculator(RMSCalculator *rms, float data)
{
  float value = data * data;
  float sub_value = 0;

  if (circularBufferIsFull(&rms->square_buffer))
    sub_value = getElement(&rms->square_buffer, 0);

  add2CircularBuffer(&rms->square_buffer, value);

  rms->sum = rms->sum + value - sub_value;
}

void calculateRMSvalue(RMSCalculator *rms, float *rms_value)
{
  float value = rms->sum / rms->size;
  *rms_value = sqrt(value);
}

void RMSCalculatorFree(RMSCalculator *rms)
{
  circularBufferFree(&rms->square_buffer);
}