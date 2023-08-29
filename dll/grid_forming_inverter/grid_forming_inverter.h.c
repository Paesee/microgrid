#include "grid_forming_inverter.h"

/* DROOP CONTROLLER FUNCTIONS IMPLEMENTATION */

void DroopControlInit(Droop *droop, float p_max, float q_max, float p_ref, float q_ref, float v0, float w0, float delta_v, float delta_w)
{
  droop->p_max = p_max;
  droop->q_max = q_max;
  droop->p_ref = p_ref;
  droop->q_ref = q_ref;
  droop->v0 = v0;
  droop->w0 = w0;
  droop->m = delta_w / (2 * p_max);
  droop->n = delta_v / (2 * q_max);
}

void setPQ(Droop *droop, float p_ref, float q_ref)
{
  droop->p_ref = p_ref;
  droop->q_ref = q_ref;
}

void setV0(Droop *droop, float v0)
{
  droop->v0 = v0;
}

void setW0(Droop *droop, float w0)
{
  droop->w0 = w0;
}

void setM(Droop *droop, float delta_w)
{
  droop->m = delta_w / (2 * droop->p_max);
}

void setN(Droop *droop, float delta_v)
{
  droop->n = delta_v / (2 * droop->q_max);
}

void executeDroopControl(Droop *droop, float p, float q, float p_ref, float q_ref, float *v_ref, float *w_ref)
{
  float v_droop = droop->v0 - droop->n * (q - q_ref);
  float w_droop = droop->w0 - droop->m * (p - p_ref);

  *v_ref = v_droop;
  *w_ref = w_droop;
}

/* VOLTAGE CONTROLLER FUNCTIONS IMPLEMENTATION */

void VoltageControlInit(VoltageControl *vc)
{
  vc->last_u = 0;
  vc->sec_last_u = 0;
  vc->last_error = 0;
  vc->sec_last_error = 0;
}

void executeCurrentControl(VoltageControl *vc, float v_ref, float  v_meas, float *u)
{
  float error = v_ref - v_meas;

  float ctrl_action = V_GAIN_1 * error + V_GAIN_2 * vc->sec_last_error - V_GAIN_3 * vc->last_u - V_GAIN_4 * vc->sec_last_u;

  float aux = (ctrl_action > 500) ? 500 : (ctrl_action < -500) ? -500 : ctrl_action;

  vc->sec_last_error = vc->last_error;
  vc->last_error = error;
  vc->sec_last_u = vc->last_u;
  vc->last_u = aux;
  
  *u = aux * 0.002;
}