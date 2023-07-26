#ifndef PQ_CALCULATOR // diretiva de inclusão condicional para evitar múltiplas inclusões do mesmo arquivo de cabeçalho
#define PQ_CALCULATOR

#include "circular_buffer.h"
#include <math.h>

typedef struct{
  CircularBuffer voltage_buffer;
  int16_t active;
  int16_t reactive;
  uint8_t grid_frequency;
  uint8_t execution_rate;
  uint8_t delayed_index;
} ElectricalPower;

int ElectricalPowerInit(ElectricalPower *ep, uint8_t grid_frequency, uint8_t min_grid_frequency, uint8_t execution_rate);

#endif // fecha a diretiva de inclusão condicional