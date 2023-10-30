#ifndef GRID_FORMING_INVERTER
#define GRID_FORMING_INVERTER

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* CIRCULAR BUFFER */

typedef struct
{
  float *buff;
  int size;
  int start;
  int end;
  int count;
} ug_CircularBuffer;

int ug_circularBufferInit(ug_CircularBuffer *cb, int size);
int ug_circularBufferIsEmpty(const ug_CircularBuffer *cb);
int ug_circularBufferIsFull(const ug_CircularBuffer *cb);
void ug_add2CircularBuffer(ug_CircularBuffer *cb, float data);
float ug_getElement(ug_CircularBuffer *cb, int n);
void ug_circularBufferFree(ug_CircularBuffer *cb);
void ug_plotBufferFromStartToEnd(ug_CircularBuffer *cb);

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
  float last_pf_meas;
  float last_qf_meas;
  float delta_v;
  float delta_w;
} Droop;

// Droop Controller Functions
void DroopControlInit(Droop *droop, float p_max, float q_max, float v0, float w0, float delta_v, float delta_w);
void calculateDroopPQ(Droop *pc, float v_alpha, float v_beta, float i_alpha, float i_beta, float *active_power, float *reactive_power);
void setDroopPQ(Droop *droop, float p_ref, float q_ref);
void setDroopV0(Droop *droop, float v0);
void setDroopW0(Droop *droop, float w0);
void setDroopDeltaW(Droop *droop, float delta_w);
void setDroopDeltaV(Droop *droop, float delta_v);
void executeDroopControl(Droop *droop, float *v_ref, float *w_ref);

// Droop Controller Gains

/*
Having in mind that the implemented power calculation method uses a Low Pass Filter and its difference equation looks like this:
  P = LPF_GAIN_1 * p + LPF_GAIN_2 * ep->last_pf;
  Q = LPF_GAIN_1 * q + LPF_GAIN_2 * ep->last_qf;
Please, define the gains below.
*/

#define LPF_GAIN_1 0.01867
#define LPF_GAIN_2 -0.9813

/* VOLTAGE CONTROLLER */

// Voltage Controller Struct

typedef struct
{
  float last_u;
  float sec_last_u;
  float last_error;
  float sec_last_error;
  float last_theta;
} VoltageControl;

// Voltage Controller Gains
#define V_GAIN_1 0.15
#define V_GAIN_2 -0.289
#define V_GAIN_3 0.1398
#define V_GAIN_4 -1.994
#define V_GAIN_5 1
// Voltage Controller Definitions
#define TSV 0.0002
#define TWO_PI 6.2831853
#define SQRT2 1.41421356237

// Voltage Controller Functions
void VoltageControlInit(VoltageControl *vc);
void generateReference(VoltageControl *vc, float v_rms, float w, float *sin_wt, float *out_theta);
void executeVoltageControl(VoltageControl *vc, float v_ref, float v_meas, float *u);

/* Current CONTROLLER */

// Current Controller Struct

typedef struct
{
  float last_u;
  float sec_last_u;
  float last_error;
  float sec_last_error;
} GFormCurrentControl;

// Current Controller Gains
#define GFC_GAIN 5.9

// Current Controller Functions
void GFormCurrentControlInit(GFormCurrentControl *cc);
void GFormExecuteCurrentControl(GFormCurrentControl *cc, float i_ref, float i_meas, float *u);

/* Grid Synchronization Controller */

typedef struct {
  // attributes for phase sync
  float w0;
  float w_last_error;
  float w_last_output;
  // attributes for voltage sync
  float u0;
  float u_last_error;
  float u_last_output;
} GridSyncController;

// Grid Synchronization Controller Gains
#define W_GAIN_1 1
#define W_GAIN_2 -0.995
#define W_GAIN_3 -1
#define U_GAIN_1 1
#define U_GAIN_2 -0.995
#define U_GAIN_3 -1
// Limitation to control outputs
#define MAX_RATE_CHANGE_W 0.006283 // dw/dt < 2 Hz/s 
#define MAX_RATE_CHANGE_V 0.005 // dv/dt < 10 V/s 

void GridSyncControllerInit(GridSyncController *self);
void executeThetaSyncControl(GridSyncController *self, float grid_theta, float ugrid_theta, float *output, float *phase_error);
void executeVoltageSyncControl(GridSyncController *self, float grid_voltage, float ugrid_voltage, float *output, float *v_error);

/* MICROGRID CONTROLLER */

#define ISLANDED 1
#define TRANSITION2TIED 2
#define TIED2GRID 3
#define TRANSITION2ISLANDED 4

#define MAX_THETA_DIF 0.5
#define MAX_VOLTAGE_DIF 2

typedef struct
{
  // status of the microgrid according to const defined above
  int ugrid_status;
  // buffer to store last phase errors
  ug_CircularBuffer delta_theta;
  float delta_theta_sum;
  // buffer to store last voltage errors
  ug_CircularBuffer delta_v;
  float delta_v_sum;
  // factor to calculate average, this variable allow to execute only one division 
  float buff_size_factor;
} uGridController;

// initialization functions
void uGridControllerInit(uGridController *self, int size);
// setting request between microgrid modes
void setTransition2Tied(uGridController *self);
void setTransition2Islanded(uGridController *self);
// adding new values to its buffers
void addDeltaTheta(uGridController *self, float delta_theta);
void addDeltaV(uGridController *self, float delta_v);
// tie/untie handlers
int tieHandler(uGridController *self, float delta_theta, float delta_v);
int untieHandler(uGridController *self);

#endif