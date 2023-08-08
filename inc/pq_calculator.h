#ifndef PQ_CALCULATOR // diretiva de inclusão condicional para evitar múltiplas inclusões do mesmo arquivo de cabeçalho
#define PQ_CALCULATOR

/*
First Order LPF filter transfer function is:
  LPF(s)  = (w0/k)/(s+(w0/k))
In discrete domain it is:
  LPFd(z) = n2 / (z - d2)
Transforming into difference equation:
  Y[k] = n2 * U[k-1] - (-d2)    * Y[k-1]
  Y[k] = n2 * U[k-1] + G_OUTPUT * Y[k-1]
Where Y stands for OUTPUT and U for INPUT

Y is going to be filtered power
whereas U is the 120 Hz power signal

Please define G_INPUT and G_OUTPUT below, remember it's related to sampling time
*/

#define G_INPUT  0.003763
#define G_OUTPUT 0.9962

#include "circular_buffer.h"
#include <math.h>

typedef struct{
  CircularBuffer voltage_buffer;
  int32_t last_pf;
  int32_t last_qf;
  uint8_t grid_frequency;
  uint8_t execution_rate;
  uint8_t delayed_index;
} ElectricalPower;

int  ElectricalPowerInit(ElectricalPower *ep, uint8_t grid_frequency, uint8_t min_grid_frequency, uint8_t execution_rate);
void calculatePQ(ElectricalPower *ep, int16_t voltage, int16_t current, int32_t *active_power, int32_t *reactive_power);
void calculatePQdroop(ElectricalPower *ep, int16_t voltage, int16_t current, int32_t *active_power, int32_t *reactive_power, uint8_t grid_frequency);

#endif // fecha a diretiva de inclusão condicional