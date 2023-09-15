#include "grid_forming_inverter.h"

/* DROOP CONTROLLER FUNCTIONS IMPLEMENTATION */

//Function used to initialize the controller
void DroopControlInit(Droop *droop, float p_max, float q_max, float v0, float w0, float delta_v, float delta_w)
{
  droop->p_max = p_max;
  droop->q_max = q_max;
  droop->p_ref = 0;
  droop->q_ref = 0;
  droop->v0 = v0;
  droop->w0 = w0;
  droop->m = delta_w / (2 * p_max);
  droop->n = delta_v / (2 * q_max);
  droop->last_pf_meas = 0;
  droop->last_qf_meas = 0;
}

//Function used to calculate power dispatched from the Converter
void calculateDroopPQ(Droop *droop, float v_alpha, float v_beta, float i_alpha, float i_beta, float *active_power, float *reactive_power)
{
  float p = 0.5 * (v_alpha * i_alpha + v_beta * i_beta);
  float q = 0.5 * (v_beta * i_alpha - v_alpha * i_beta);

  // Implements LOW PASS FILTER using the gains informed by the library user
  p = LPF_GAIN_1 * p + LPF_GAIN_2 * droop->last_pf_meas;
  q = LPF_GAIN_1 * q + LPF_GAIN_2 * droop->last_qf_meas;

  droop->last_pf_meas = p;
  droop->last_qf_meas = q;

  *active_power = p;
  *reactive_power = q;
}

//Set droop P0 and Q0 (only used when tied to grid)
void setDroopPQ(Droop *droop, float p_ref, float q_ref)
{
  droop->p_ref = p_ref;
  droop->q_ref = q_ref;
}

//Set droop nominal RMS voltage
void setDroopV0(Droop *droop, float v0)
{
  droop->v0 = v0;
}

//Set droop nominal frequency (rad/s)
void setDroopW0(Droop *droop, float w0)
{
  droop->w0 = w0;
}

//Set droop max frequency variation
void setDroopDeltaW(Droop *droop, float delta_w)
{
  droop->m = delta_w / (2 * droop->p_max);
}

//Set droop max voltage variation
void setDroopDeltaV(Droop *droop, float delta_v)
{
  droop->n = delta_v / (2 * droop->q_max);
}

//Execute Droop Control its dependent on calling calculateDroopPQ() and setDroopPQ(), these functions will update Droop Struct
void executeDroopControl(Droop *droop, float *v_ref, float *w_ref)
{
  float v_droop = droop->v0 - droop->n * (droop->last_pf_meas - droop->q_ref);
  float w_droop = droop->w0 - droop->m * (droop->last_qf_meas - droop->p_ref);

  *v_ref = v_droop;
  *w_ref = w_droop;
}

/* VOLTAGE CONTROLLER FUNCTIONS IMPLEMENTATION */

//Function used to initialize the controller
void VoltageControlInit(VoltageControl *vc)
{
  vc->last_u = 0;
  vc->sec_last_u = 0;
  vc->last_error = 0;
  vc->sec_last_error = 0;
  vc->last_theta = 0;
}

//Function used to generate reference signal [v_ref(t) = sqrt(2) * v_rms * sin(theta)]
void generateReference(VoltageControl *vc, float v_rms, float w, float *sin_wt)
{
  //used to generate theta for sine function
  float theta = TSV * w + vc->last_theta;
  if (theta >= TWO_PI)
    theta -= TWO_PI;
  vc->last_theta = theta;
  //output voltage reference wave
  *sin_wt = SQRT2 * v_rms * sin(theta);
}

//Execute voltage control according to the gains defined in header file
void executeVoltageControl(VoltageControl *vc, float v_ref, float v_meas, float *u)
{
  float error = v_ref - v_meas;

  float ctrl_action = V_GAIN_1 * vc->last_error + V_GAIN_2 * vc->sec_last_error - V_GAIN_3 * vc->last_u - V_GAIN_4 * vc->sec_last_u;

  vc->sec_last_error = vc->last_error;
  vc->last_error = error;
  vc->sec_last_u = vc->last_u;
  vc->last_u = ctrl_action;
  *u = ctrl_action;
}

/* CURRENT CONTROLLER FUNCTIONS IMPLEMENTATION */

//Function used to initialize the controller
void GFormCurrentControlInit(GFormCurrentControl *cc)
{
  cc->last_u = 0;
  cc->sec_last_u = 0;
  cc->last_error = 0;
  cc->sec_last_error = 0;
}

//Execute current control according to the gain defined in header file
void GFormExecuteCurrentControl(GFormCurrentControl *cc, float i_ref, float i_meas, float *u)
{
  float error = i_ref - i_meas;

  float ctrl_action = GFC_GAIN  * error;

  float aux = (ctrl_action > 500) ? 500 : (ctrl_action < -500) ? -500 : ctrl_action;

  cc->sec_last_error = cc->last_error;
  cc->last_error = error;
  cc->sec_last_u = cc->last_u;
  cc->last_u = aux;
  
  *u = aux * 0.002;
}