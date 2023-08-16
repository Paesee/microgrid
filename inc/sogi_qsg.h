#ifndef SOGI_QSG
#define SOGI_QSG

#include <stdint.h>

#define PI 3.14159265359
#define INT_GAIN_1 0.0001
#define INT_GAIN_2 1
#define FLL_GAIN   1

typedef struct{
  int16_t w0;
  int32_t last_alpha_signal;
  int32_t last_beta_signal;
  int32_t last_w_meas;
} SOGIqsg;

void SOGIInit(SOGIqsg *sogi, int32_t frequency);
void setCentralFrequency(SOGIqsg *sogi, int32_t frequency);
void executeSOGI(SOGIqsg *sogi, int32_t meas_signal, int32_t *alpha_signal, int32_t *beta_signal, int32_t *frequency);

#endif