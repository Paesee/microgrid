#include "pq_calculator.h"

int ElectricalPowerInit(ElectricalPower *ep, uint8_t grid_frequency, uint8_t min_grid_frequency, uint8_t execution_rate)
{
  uint8_t buffer_size   = (uint8_t)ceil(0.25 * execution_rate / min_grid_frequency); //cria um buffer um pouco maior do para a freq nominal
  uint8_t delayed_index = (uint8_t)ceil(0.25 * execution_rate / grid_frequency);     //dá o indice de quantas amostras no passado está -90º 
  if (circularBufferInit(&ep->voltage_buffer, buffer_size) != 0)
    return -1;
  ep->active    = 0;
  ep->reactive  = 0;
  ep->grid_frequency = grid_frequency;
  ep->execution_rate = execution_rate;
  ep->delayed_index  = delayed_index;
  return 0;
}

void calculatePQ(ElectricalPower *ep, float voltage, float current, int16_t *active_power, int16_t *reactive_power){
  
  int16_t quadrature_voltage = getElement(&ep->voltage_buffer, ep->delayed_index);
  int16_t p = voltage*current;
  int16_t q = quadrature_voltage*current;
}