#ifndef GRID_FOLLOWING_INVERTER
#define GRID_FOLLOWING_INVERTER

#include <stdint.h>
#include <math.h>

/* POWER CONTROLLER */

// Power Controller Struct
typedef struct{
  int16_t v_poc;
  int32_t p_ref;
  int32_t q_ref;
  int32_t last_p_error;
  int32_t last_p_action;
  int32_t last_q_error;
  int32_t last_q_action;
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

// Power Controller Functions

void PowerControlInit(PowerControl *pc, int16_t v_poc, int32_t p_ref, int32_t q_ref);
void setVpoc(PowerControl *pc, int32_t v_poc);
void setPQ(PowerControl *pc, int32_t p_ref, int32_t q_ref);
void calculatePowerAction(PowerControl *pc, int32_t p_measured, int32_t q_measured, int32_t *control_action);

/* CURRENT CONTROLLER */

// Current Controller Struct

typedef struct{
  int16_t v_poc;
  int32_t p_ref;
  int32_t q_ref;
  int32_t last_p_error;
  int32_t last_p_action;
  int32_t last_q_error;
  int32_t last_q_action;
} CurrentControl;

// Current Controller Gains

// Current Controller Functions

#endif