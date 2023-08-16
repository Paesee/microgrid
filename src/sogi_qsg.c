#include "sogi_qsg.h"

void SOGIInit(SOGIqsg *sogi, int32_t frequency)
{
  sogi->w0 = 10.0 * 2 * PI * frequency;
  sogi->last_alpha_signal = 0;
  sogi->last_beta_signal  = 0;
  sogi->last_w_meas       = sogi->w0;
}

void setCentralFrequency(SOGIqsg *sogi, int32_t frequency)
{
  sogi->w0 = 10.0 * 2 * PI * frequency;
}

void executeSOGI(SOGIqsg *sogi, int32_t meas_signal, int32_t *alpha_signal, int32_t *beta_signal, int32_t *frequency)
{
  //QSG execution
  int32_t phase_error_1 = meas_signal - sogi->last_alpha_signal;
  int32_t phase_error_2 = sogi->last_w_meas * (phase_error_1 - sogi->last_beta_signal);
  
  int32_t alpha = INT_GAIN_1 * phase_error_2 + INT_GAIN_2 * sogi->last_alpha_signal;
  int32_t beta = sogi->last_w_meas * (INT_GAIN_1 * alpha + INT_GAIN_2 * sogi->last_beta_signal);

  //FLL execution
  int32_t fll_input = -FLL_GAIN * phase_error_1 * beta;
  int32_t w_meas    = 10.0 * (INT_GAIN_1 * fll_input + INT_GAIN_2 * sogi->last_w_meas) + sogi->w0;

  //assign values for the next execution
  sogi->last_alpha_signal = alpha;
  sogi->last_beta_signal  = beta;
  sogi->last_w_meas       = w_meas;

  //output value to the pointers
  *alpha_signal = alpha;
  *beta_signal  = beta;
  *frequency    = 0.15915494309 * w_meas; //from rad/s to Hz
}