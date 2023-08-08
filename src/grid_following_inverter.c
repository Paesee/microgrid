#include "grid_following_inverter.h"

void PowerControlInit(PowerControl *pc, int16_t v_poc, int32_t p_ref, int32_t q_ref)
{
  pc->v_poc         = v_poc;
  pc->p_ref         = p_ref;
  pc->q_ref         = q_ref;
  pc->last_p_error  = 0;
  pc->last_p_action = 0;
  pc->last_q_error  = 0;
  pc->last_q_action = 0;
}

void setPQ(PowerControl *pc, int32_t p_ref, int32_t q_ref){
  pc->p_ref = p_ref;
  pc->q_ref = q_ref;
}

void calculatePowerAction(PowerControl *pc, int32_t p_measured, int32_t q_measured, int32_t *control_action){
  int32_t p_error = pc->p_ref - p_measured;
  int32_t q_error = q_measured - pc->q_ref;

  int32_t p_ctrl_action = P_GAIN_1 * p_error + P_GAIN_2 * pc->last_p_error + P_GAIN_3 * pc->last_p_action;
  int32_t q_ctrl_action = Q_GAIN_1 * q_error + Q_GAIN_2 * pc->last_q_error + Q_GAIN_3 * pc->last_q_action;

  pc->last_p_error  = p_error;
  pc->last_p_action = p_ctrl_action;
  pc->last_q_error  = q_error;
  pc->last_q_action = q_ctrl_action;

  *control_action = p_ctrl_action;
  //TODO
}

void calculateCurrentAction(){}