#include "grid_following_inverter.h"

/* POWER CONTROLLER FUNCTIONS IMPLEMENTATION */

void PowerControlInit(PowerControl *pc, float p_ref, float q_ref)
{
  pc->p_ref = p_ref;
  pc->q_ref = q_ref;
  pc->last_p_error = 0;
  pc->last_i_direct = 0;
  pc->last_q_error = 0;
  pc->last_i_quad = 0;
  pc->last_pf_meas = 0;
  pc->last_qf_meas = 0;
}

void setPQ(PowerControl *pc, float p_ref, float q_ref)
{
  pc->p_ref = p_ref;
  pc->q_ref = q_ref;
}

void calculatePQ(PowerControl *pc, float v_alpha, float v_beta, float i_alpha, float i_beta, float *active_power, float *reactive_power)
{
  float p = 0.5 * (v_alpha * i_alpha + v_beta * i_beta);
  float q = 0.5 * (v_beta * i_alpha - v_alpha * i_beta);

  // Implements LOW PASS FILTER using the gains informed by the library user
  p = LPF_GAIN_1 * p - LPF_GAIN_2 * pc->last_pf_meas;
  q = LPF_GAIN_1 * q - LPF_GAIN_2 * pc->last_qf_meas;

  pc->last_pf_meas = p;
  pc->last_qf_meas = q;

  *active_power = p;
  *reactive_power = q;
}

void executePQControl(PowerControl *pc, float *id, float *iq)
{
  float p_error = pc->p_ref - pc->last_pf_meas;
  float q_error = pc->q_ref - pc->last_qf_meas;

  float i_direct = P_GAIN_1 * p_error + P_GAIN_2 * pc->last_p_error - P_GAIN_3 * pc->last_i_direct;
  float i_quad = Q_GAIN_1 * q_error + Q_GAIN_2 * pc->last_q_error - Q_GAIN_3 * pc->last_i_quad;

  pc->last_p_error = p_error;
  pc->last_i_direct = i_direct;
  pc->last_q_error = q_error;
  pc->last_i_quad = i_quad;

  *id = i_direct;
  *iq = i_quad;
}

/* CURRENT CONTROLLER FUNCTIONS IMPLEMENTATION */

void CurrentControlInit(CurrentControl *cc)
{
  cc->last_u = 0;
  cc->sec_last_u = 0;
  cc->last_error = 0;
  cc->sec_last_error = 0;
}

void executeCurrentControl(CurrentControl *cc, float id, float iq, float sin_wt, float cos_wt, float i_meas, float *u, float *ref_wave)
{
  float i_alpha = id * cos_wt - sin_wt * -iq;

  float error = i_alpha - i_meas;

  float ctrl_action = C_GAIN_1 * error + C_GAIN_2 * cc->last_error + C_GAIN_3 * cc->sec_last_error - C_GAIN_4 * cc->last_u - C_GAIN_5 * cc->sec_last_u;

  float aux = (ctrl_action > 500) ? 500 : (ctrl_action < -500) ? -500 : ctrl_action;

  cc->sec_last_error = cc->last_error;
  cc->last_error = error;
  cc->sec_last_u = cc->last_u;
  cc->last_u = aux;
  
  *u = aux * 0.002;
  *ref_wave = i_alpha;
}