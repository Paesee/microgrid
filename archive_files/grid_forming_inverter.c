#include "grid_forming_inverter.h"

/* CIRCULAR BUFFER FUNCTIONS IMPLEMENTATION */

int ug_circularBufferInit(ug_CircularBuffer *cb, int size)
{
  cb->buff = (float *)malloc(sizeof(float) * size);
  if (sizeof(cb->buff) == 0)
    return -1;
  cb->size = size;
  cb->start = 0;
  cb->end = 0;
  cb->count = 0;
  return 0;
}

int ug_circularBufferIsEmpty(const ug_CircularBuffer *cb)
{
  return cb->size == 0;
}

int ug_circularBufferIsFull(const ug_CircularBuffer *cb)
{
  return cb->count == cb->size;
}

void ug_add2CircularBuffer(ug_CircularBuffer *cb, float data)
{
  cb->buff[cb->end] = data;
  cb->end = (cb->end + 1) % cb->size;
  if (cb->count == cb->size)
    cb->start = (cb->start + 1) % cb->size;
  else
    cb->count++;
}

float ug_getElement(ug_CircularBuffer *cb, int n)
{
  if (n >= cb->count || n < 0)
    return 0xFFFF;
  int index = (cb->start + n) % cb->size;
  return cb->buff[index];
}

void ug_circularBufferFree(ug_CircularBuffer *cb)
{
  free(cb->buff);
}

void ug_plotBufferFromStartToEnd(ug_CircularBuffer *cb)
{
  int i;
  int count = cb->count;
  if (count == 0)
  {
    printf("Buffer is empty.\n");
    return;
  }
  printf("Buffer content:\n");
  for (i = cb->start; count > 0; i = (i + 1) % cb->size)
  {
    printf("%d ", cb->buff[i]);
    count--;
  }
  printf("\n");
}

/* DROOP CONTROLLER FUNCTIONS IMPLEMENTATION */

// Function used to initialize the controller
void DroopControlInit(Droop *droop, float p_max, float q_max, float v0, float w0, float delta_v, float delta_w)
{
  droop->p_max = p_max;
  droop->q_max = q_max;
  droop->p_ref = 0;
  droop->q_ref = 0;
  droop->v0 = v0;
  droop->w0 = w0;
  droop->m = (float)delta_w / (2.0 * p_max);
  droop->n = (float)delta_v / (2.0 * q_max);
  droop->last_pf_meas = 0;
  droop->last_qf_meas = 0;
  droop->delta_v = 0.5*delta_v;
  droop->delta_w = 0.5*delta_w;
}

// Function used to calculate power dispatched from the Converter
void calculateDroopPQ(Droop *droop, float v_alpha, float v_beta, float i_alpha, float i_beta, float *active_power, float *reactive_power)
{
  float p = 0.5 * (v_alpha * i_alpha + v_beta * i_beta);
  float q = 0.5 * (v_beta * i_alpha - v_alpha * i_beta);

  // Implements LOW PASS FILTER using the gains informed by the library user
  p = LPF_GAIN_1 * p - LPF_GAIN_2 * droop->last_pf_meas;
  q = LPF_GAIN_1 * q - LPF_GAIN_2 * droop->last_qf_meas;

  droop->last_pf_meas = p;
  droop->last_qf_meas = q;

  *active_power = p;
  *reactive_power = q;
}

// Set droop P0 and Q0 (only used when tied to grid)
void setDroopPQ(Droop *droop, float p_ref, float q_ref)
{
  droop->p_ref = p_ref;
  droop->q_ref = q_ref;
}

// Set droop nominal RMS voltage
void setDroopV0(Droop *droop, float v0)
{
  droop->v0 = v0;
}

// Set droop nominal frequency (rad/s)
void setDroopW0(Droop *droop, float w0)
{
  droop->w0 = w0;
}

// Set droop max frequency variation
void setDroopDeltaW(Droop *droop, float delta_w)
{
  droop->delta_w = delta_w;
  droop->m = delta_w / (2 * droop->p_max);
}

// Set droop max voltage variation
void setDroopDeltaV(Droop *droop, float delta_v)
{
  droop->delta_v = delta_v;
  droop->n = delta_v / (2 * droop->q_max);
}

// Execute Droop Control its dependent on calling calculateDroopPQ() and setDroopPQ(), these functions will update Droop Struct
void executeDroopControl(Droop *droop, float *v_ref, float *w_ref)
{
  float v_droop = droop->n * (droop->last_qf_meas - droop->q_ref);
  float w_droop = droop->m * (droop->last_pf_meas - droop->p_ref);

  // limitando a maxima variacao de tensao
  if (v_droop > droop->delta_v)
    v_droop = droop->delta_v;
  if (v_droop < -droop->delta_v)
    v_droop = -droop->delta_v;
  // limitando a maxima variacao de frequencia
  if (w_droop > droop->delta_w)
    w_droop = droop->delta_w;
  if (w_droop < -droop->delta_w)
    w_droop = -droop->delta_w;

  v_droop = droop->v0 - v_droop;
  w_droop = droop->w0 - w_droop;

  *v_ref = v_droop;
  *w_ref = w_droop;
}

/* VOLTAGE CONTROLLER FUNCTIONS IMPLEMENTATION */

// Function used to initialize the controller
void VoltageControlInit(VoltageControl *vc)
{
  vc->last_u = 0;
  vc->sec_last_u = 0;
  vc->last_error = 0;
  vc->sec_last_error = 0;
  vc->last_theta = 0;
}

// Function used to generate reference signal [v_ref(t) = sqrt(2) * v_rms * sin(theta)]
void generateReference(VoltageControl *vc, float v_rms, float w, float *sin_wt, float *out_theta)
{
  // used to generate theta for sine function
  float theta = TSV * w + vc->last_theta;
  if (theta >= TWO_PI)
    theta -= TWO_PI;
  vc->last_theta = theta;
  // output voltage reference wave
  *sin_wt = SQRT2 * v_rms * sin(theta);
  *out_theta = theta;
}

// Execute voltage control according to the gains defined in header file
void executeVoltageControl(VoltageControl *vc, float v_ref, float v_meas, float *u)
{
  float error = v_ref - v_meas;

  float ctrl_action = V_GAIN_1 * error + V_GAIN_2 * vc->last_error + V_GAIN_3 * vc->sec_last_error - V_GAIN_4 * vc->last_u - V_GAIN_5 * vc->sec_last_u;

  vc->sec_last_error = vc->last_error;
  vc->last_error = error;
  vc->sec_last_u = vc->last_u;
  vc->last_u = ctrl_action;
  *u = ctrl_action;
}

/* CURRENT CONTROLLER FUNCTIONS IMPLEMENTATION */

// Function used to initialize the controller
void GFormCurrentControlInit(GFormCurrentControl *cc)
{
  cc->last_u = 0;
  cc->sec_last_u = 0;
  cc->last_error = 0;
  cc->sec_last_error = 0;
}

// Execute current control according to the gain defined in header file
void GFormExecuteCurrentControl(GFormCurrentControl *cc, float i_ref, float i_meas, float *u)
{
  float error = i_ref - i_meas;

  float ctrl_action = GFC_GAIN * error;

  //float aux = (ctrl_action > 500) ? 500 : (ctrl_action < -500) ? -500 : ctrl_action;

  cc->sec_last_error = cc->last_error;
  cc->last_error = error;
  cc->sec_last_u = cc->last_u;
  cc->last_u = ctrl_action;

  *u = ctrl_action * 0.002;
}

// GRID SYNCHRONIZATION FUNCTIONS IMPLEMENTATION

void GridSyncControllerInit(GridSyncController *self)
{
  self->w0 = 0;
  self->w_last_error = 0;
  self->w_last_output = 0;
  self->u0 = 0;
  self->u_last_error = 0;
  self->u_last_output = 0;
}

void executeThetaSyncControl(GridSyncController *self, float grid_theta, float ugrid_theta, float *output, float *phase_error)
{
  float error = grid_theta - ugrid_theta;
  float y = W_GAIN_1 * error + W_GAIN_2 * self->w_last_error - W_GAIN_3 * self->w_last_output;

  //float rate_of_change = y - self->w_last_output;
  //if (rate_of_change > MAX_RATE_CHANGE_W)
  //  y = self->w_last_output + MAX_RATE_CHANGE_W;
  //if (rate_of_change < -MAX_RATE_CHANGE_W)
  //  y = self->w_last_output - MAX_RATE_CHANGE_W;

  //if (y > MAX_W_SYNC)
  //  y = MAX_W_SYNC;
  //if (y < -MAX_W_SYNC)
  //  y = -MAX_W_SYNC;

  //if(error > +MAX_PHASE_ERROR)
  //  error = +MAX_PHASE_ERROR;
  //if(error < -MAX_PHASE_ERROR)
  //  error = -MAX_PHASE_ERROR;

  self->w_last_error = error;
  self->w_last_output = y;
  *output = y;
  *phase_error = error;
}

void executeVoltageSyncControl(GridSyncController *self, float grid_voltage, float ugrid_voltage, float *output, float *v_error)
{
  float error = grid_voltage - ugrid_voltage;
  float y = U_GAIN_1 * error + U_GAIN_2 * self->u_last_error - U_GAIN_3 * self->u_last_output;

  // float rate_of_change = y - self->u_last_output;
  // if(rate_of_change > MAX_RATE_CHANGE_V)
  //   y = self->u_last_output + MAX_RATE_CHANGE_V;
  // if(rate_of_change < -MAX_RATE_CHANGE_V)
  //   y = self->u_last_output - MAX_RATE_CHANGE_V;

  self->u_last_error = error;
  self->u_last_output = y;
  *output = y;
  *v_error = error;
}

// MICROGRID CONTROLLER FUNCTIONS IMPLEMENTATION

// function to iniatilize uGridController
void uGridControllerInit(uGridController *self, int size)
{
  self->ugrid_status = ISLANDED;
  ug_circularBufferInit(&self->delta_theta, size);
  self->delta_theta_sum = 0;
  ug_circularBufferInit(&self->delta_v, size);
  self->delta_v_sum = 0;
  self->buff_size_factor = (float)1.0 / size;
}

// set transition to grid tied operation mode
void setTransition2Tied(uGridController *self)
{
  if (self->ugrid_status == ISLANDED)
    self->ugrid_status = TRANSITION2TIED;
}

// set transition to islanded operation mode
void setTransition2Islanded(uGridController *self)
{
  if (self->ugrid_status == TIED2GRID)
    self->ugrid_status = TRANSITION2ISLANDED;
}

// add a phase error to its buffer
void addDeltaTheta(uGridController *self, float delta_theta)
{
  float sub_value = 0;
  if (ug_circularBufferIsFull(&self->delta_theta))
    sub_value = ug_getElement(&self->delta_theta, 0);
  ug_add2CircularBuffer(&self->delta_theta, delta_theta);
  self->delta_theta_sum = self->delta_theta_sum + delta_theta - sub_value;
}

// add a voltage error to its buffer
void addDeltaV(uGridController *self, float delta_v)
{
  float sub_value = 0;
  if (ug_circularBufferIsFull(&self->delta_v))
    sub_value = ug_getElement(&self->delta_v, 0);
  ug_add2CircularBuffer(&self->delta_v, delta_v);
  self->delta_v_sum = self->delta_v_sum + delta_v - sub_value;
}

// handle request to tied to the grid
int tieHandler(uGridController *self, float delta_theta, float delta_v)
{
  if (self->ugrid_status == TRANSITION2TIED)
  {
    // add new values to its buffers
    addDeltaTheta(self, delta_theta);
    addDeltaV(self, delta_v);
    // calculate the average of both buffers if they are full
    if (ug_circularBufferIsFull(&self->delta_theta) && ug_circularBufferIsFull(&self->delta_v))
    {
      float average_delta_theta = self->delta_theta_sum * self->buff_size_factor;
      float average_delta_v = self->delta_theta_sum * self->buff_size_factor;
      // if voltage and phase angle are within the limits, the controller allows connection
      if (average_delta_theta < MAX_THETA_DIF && average_delta_v < MAX_VOLTAGE_DIF)
      {
        self->ugrid_status = TIED2GRID;
        return 1;
      }
      // return 0 if its no the case above
      return 0;
    }
    // return 0 if buffer is not full, it prevents a connection in the start up of the buffer
    return 0;
  }
  return 0;
}

// handle request to island from grid
int untieHandler(uGridController *self)
{
  if (self->ugrid_status == TRANSITION2ISLANDED)
  {
    self->ugrid_status = ISLANDED;
    return 1;
  }
  return 0;
}