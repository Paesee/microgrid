#include "sogi_qsg.h"

void SOGIqsgInit(SOGIqsg *sogi, int32_t frequency)
{
  sogi->w0 = 10.0 * 2 * PI * frequency;
  sogi->last_alpha_signal = 0;
  sogi->last_beta_signal = 0;
}

void setCentralFrequency(SOGIqsg *sogi, int32_t frequency)
{
  sogi->w0 = 10.0 * 2 * PI * frequency;
}

void calculateAlphaBeta(SOGIqsg *sogi, int32_t meas_signal, int32_t *alpha_signal, int32_t *beta_signal)
{
  int32_t phase_error_1 = meas_signal - sogi->last_alpha_signal;
  int32_t phase_error_2 = sogi->w0 * (phase_error_1 - sogi->last_beta_signal);

  int32_t alpha = INT_GAIN_1 * phase_error_2 + INT_GAIN_2 * sogi->last_alpha_signal;
  int32_t beta = sogi->w0 * (INT_GAIN_1 * alpha + INT_GAIN_2 * sogi->last_beta_signal);

  sogi->last_alpha_signal = alpha;
  sogi->last_beta_signal = beta;
  *alpha_signal = alpha;
  *beta_signal = beta;
}