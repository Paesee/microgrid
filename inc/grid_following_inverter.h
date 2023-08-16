#ifndef GRID_FOLLOWING_INVERTER
#define GRID_FOLLOWING_INVERTER

#include <stdint.h>
#include <math.h>

/* POWER CONTROLLER */

// Power Controller Struct
typedef struct{
  int32_t p_ref;
  int32_t q_ref;
  int32_t last_p_error;
  int32_t last_p_action;
  int32_t last_q_error;
  int32_t last_q_action;
  int32_t last_pf_meas;
  int32_t last_qf_meas;
} PowerControl;

// Power Controller Gains

/*
Having in mind that the implemented controller is a PI and its difference equation looks like this:
  p_ctrl_action = P_GAIN_1 * p_error + P_GAIN_2 * pc->last_p_error + P_GAIN_3 * pc->last_p_action;
  q_ctrl_action = Q_GAIN_1 * q_error + Q_GAIN_2 * pc->last_q_error + Q_GAIN_3 * pc->last_q_action;
Please, define the gains below.
*/

#define P_GAIN_1 1
#define P_GAIN_2 1
#define P_GAIN_3 1
#define Q_GAIN_1 1
#define Q_GAIN_2 1
#define Q_GAIN_3 1

/*
Having in mind that the implemented power calculation method uses a Low Pass Filter and its difference equation looks like this:
  P = LPF_GAIN_1 * p + LPF_GAIN_2 * ep->last_pf;
  Q = LPF_GAIN_1 * q + LPF_GAIN_2 * ep->last_qf;
Please, define the gains below.
*/

#define LPF_GAIN_1 1
#define LPF_GAIN_2 1

// Power Controller Functions

void PowerControlInit(PowerControl *pc, int32_t p_ref, int32_t q_ref);
void setPQ(PowerControl *pc, int32_t p_ref, int32_t q_ref);
void calculatePQ(PowerControl *pc, int16_t v_alpha, int16_t v_beta, int16_t current, int32_t *active_power, int32_t *reactive_power);
void executePQControl(PowerControl *pc, int32_t *id, int32_t *iq);

/* CURRENT CONTROLLER */

// Current Controller Struct

typedef struct{
  int32_t last_u;
  int32_t sec_last_u;
  int32_t last_error;
  int32_t sec_last_error;
} CurrentControl;

// Current Controller Gains
#define C_GAIN_1 0.012
#define C_GAIN_2 -0.012
#define C_GAIN_3 2
#define C_GAIN_4 -1

// Current Controller Functions
void CurrentControlInit(CurrentControl *cc);
void executeCurrentControl(CurrentControl *cc, int32_t id, int32_t iq, int32_t sin_wt, int32_t cos_wt, int32_t i_meas, int32_t *u);

#endif