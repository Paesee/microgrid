#include "grid_following_inverter.h"

/* POWER CONTROLLER FUNCTIONS IMPLEMENTATION */

void PowerControlInit(PowerControl *pc, int32_t p_ref, int32_t q_ref)
{
  pc->p_ref = p_ref;
  pc->q_ref = q_ref;
  pc->last_p_error = 0;
  pc->last_p_action = 0;
  pc->last_q_error = 0;
  pc->last_q_action = 0;
  pc->last_pf_meas = 0;
  pc->last_qf_meas = 0;
}

void setPQ(PowerControl *pc, int32_t p_ref, int32_t q_ref)
{
  pc->p_ref = p_ref;
  pc->q_ref = q_ref;
}

void calculatePQ(PowerControl *pc, int16_t v_alpha, int16_t v_beta, int16_t current, int32_t *active_power, int32_t *reactive_power)
{
  int32_t p = 0.01 * v_alpha * current; // recebe as unidades multiplicadas por 10, divide por 100 para ficar em watts
  int32_t q = 0.01 * v_beta * current;
  // Implements LOW PASS FILTER using the gains informed by the library user
  p = LPF_GAIN_1 * p + LPF_GAIN_2 * pc->last_pf_meas;
  q = LPF_GAIN_1 * q + LPF_GAIN_2 * pc->last_qf_meas;
  pc->last_pf_meas = p;
  pc->last_qf_meas = q;
  *active_power = p;
  *reactive_power = q;
}

void executePQControl(PowerControl *pc, int32_t *id, int32_t *iq)
{
  int32_t p_error = pc->p_ref - pc->last_pf_meas;
  int32_t q_error = pc->last_qf_meas - pc->q_ref;

  int32_t p_ctrl_action = P_GAIN_1 * p_error + P_GAIN_2 * pc->last_p_error + P_GAIN_3 * pc->last_p_action;
  int32_t q_ctrl_action = Q_GAIN_1 * q_error + Q_GAIN_2 * pc->last_q_error + Q_GAIN_3 * pc->last_q_action;

  pc->last_p_error = p_error;
  pc->last_p_action = p_ctrl_action;
  pc->last_q_error = q_error;
  pc->last_q_action = q_ctrl_action;

  *id = p_ctrl_action;
  *iq = q_ctrl_action;
}

/* CURRENT CONTROLLER FUNCTIONS IMPLEMENTATION */

void CurrentControlInit(CurrentControl *cc)
{
  cc->last_u = 0;
  cc->sec_last_u = 0;
  cc->last_error = 0;
  cc->sec_last_error = 0;
}

void executeCurrentControl(CurrentControl *cc, int32_t id, int32_t iq, int32_t sin_wt, int32_t cos_wt, int32_t i_meas, int32_t *u)
{
  int32_t i_alpha = cos_wt * id - sin_wt * iq;
  int32_t i_beta = sin_wt * id + cos_wt * iq;

  int32_t error = i_alpha - i_meas;

  int32_t i_ctrl_action = C_GAIN_1 * error + C_GAIN_2 * cc->last_error + C_GAIN_3 * cc->last_u + C_GAIN_4 * cc->sec_last_u;

  cc->last_error = error;
  cc->sec_last_u = cc->last_u;
  cc->last_u = i_ctrl_action;

  *u = i_ctrl_action;
}