#define EXPORT __declspec(dllexport)

#ifndef GRID_FORMING_INVERTER
#define GRID_FORMING_INVERTER

#include <stdint.h>
#include <math.h>

/* DROOP CONTROLLER */

// Droop Controller Struct
typedef struct
{
  float p_max;
  float q_max;
  float p_ref;
  float q_ref;
  float v0;
  float w0;
  float m;
  float n;
} Droop;

// Droop Controller Functions
EXPORT void DroopControlInit(Droop *droop, float p_max, float q_max, float p_ref, float q_ref, float v0, float w0, float delta_v, float delta_w);
EXPORT void setPQ(Droop *droop, float p_ref, float q_ref);
EXPORT void setV0(Droop *droop, float v0);
EXPORT void setW0(Droop *droop, float w0);
EXPORT void setM(Droop *droop, float delta_w);
EXPORT void setN(Droop *droop, float delta_v);
EXPORT void executeDroopControl(Droop *droop, float p, float q, float p_ref, float q_ref, float *v_ref, float *w_ref);

/* VOLTAGE CONTROLLER */

// Voltage Controller Struct

typedef struct
{
  int32_t last_u;
  int32_t sec_last_u;
  int32_t last_error;
  int32_t sec_last_error;
} VoltageControl;

// Current Controller Gains
#define V_GAIN_1 0.012
#define V_GAIN_2 -0.012
#define V_GAIN_3 2
#define V_GAIN_4 -1

// Current Controller Functions
EXPORT void VoltageControlInit(VoltageControl *vc);
EXPORT void executeCurrentControl(VoltageControl *vc, float v_ref, float  v_meas, float *u);

#endif