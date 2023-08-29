#define EXPORT __declspec(dllexport)

#ifndef SOGI_QSG
#define SOGI_QSG

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

EXPORT void SOGIInit(SOGIqsg *sogi, float frequency, float p_gain, float f_gain);
EXPORT void setCentralFrequency(SOGIqsg *sogi, float frequency);
EXPORT void setProportionalGain(SOGIqsg *sogi, float p_gain);
EXPORT void setFLLGain(SOGIqsg *sogi, float f_gain);
EXPORT void executeSOGI(SOGIqsg *sogi, float meas_signal, float *alpha_signal, float *beta_signal, float *frequency);

/* CIRCULAR BUFFER */

typedef struct
{
  float *buff;
  int size;
  int start;
  int end;
  int count;
} CircularBuffer;

EXPORT int circularBufferInit(CircularBuffer *cb, int size);
EXPORT int circularBufferIsEmpty(const CircularBuffer *cb);
EXPORT int circularBufferIsFull(const CircularBuffer *cb);
EXPORT void add2CircularBuffer(CircularBuffer *cb, float data);
EXPORT float getElement(CircularBuffer *cb, int n);
EXPORT void circularBufferFree(CircularBuffer *cb);
EXPORT void plotBufferFromStartToEnd(CircularBuffer *cb);

/* RMS CALCULATOR */

typedef struct
{
  CircularBuffer square_buffer;
  int size;
  float sum;
} RMSCalculator;

EXPORT int RMSCalculatorInit(RMSCalculator *rms, int size);
EXPORT void add2RMSCalculator(RMSCalculator *rms, float data);
EXPORT void calculateRMSvalue(RMSCalculator *rms, float *rms_value);
EXPORT void RMSCalculatorFree(RMSCalculator *rms);

#endif