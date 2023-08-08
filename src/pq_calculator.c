#include "pq_calculator.h"

int ElectricalPowerInit(ElectricalPower *ep, uint8_t grid_frequency, uint8_t min_grid_frequency, uint8_t execution_rate)
{
  uint8_t buffer_size   = (uint8_t)ceil(0.25 * execution_rate / min_grid_frequency); //cria um buffer um pouco maior do para a freq nominal
  uint8_t delayed_index = (uint8_t)ceil(0.25 * execution_rate / grid_frequency);     //dá o indice de quantas amostras no passado está -90º 
  if (circularBufferInit(&ep->voltage_buffer, buffer_size) != 0)
    return -1;
  ep->last_pf = 0;
  ep->last_qf = 0;
  ep->grid_frequency = grid_frequency;
  ep->execution_rate = execution_rate;
  ep->delayed_index  = delayed_index;
  return 0;
}

void calculatePQ(ElectricalPower *ep, int16_t voltage, int16_t current, int32_t *active_power, int32_t *reactive_power){
  int16_t quadrature_voltage = getElement(&ep->voltage_buffer, ep->delayed_index);
  int32_t p = voltage*current;
  int32_t q = quadrature_voltage*current;
  //Implements LOW PASS FILTER using the gains informed by the library user
  p = G_INPUT * p + G_OUTPUT * ep->last_pf;
  q = G_INPUT * q + G_OUTPUT * ep->last_qf;
  ep->last_pf     = p;
  ep->last_qf     = q;
  *active_power   = p;
  *reactive_power = q;
}

void calculatePQdroop(ElectricalPower *ep, int16_t voltage, int16_t current, int32_t *active_power, int32_t *reactive_power, uint8_t grid_frequency){
  uint8_t delayed_index = (uint8_t)ceil(0.25 * ep->execution_rate / grid_frequency);
  int16_t quadrature_voltage = getElement(&ep->voltage_buffer, delayed_index);
  int32_t p = 0.01*voltage*current;
  int32_t q = 0.01*quadrature_voltage*current;
  //Implements LOW PASS FILTER using the gains informed by the library user
  p = G_INPUT * p + G_OUTPUT * ep->last_pf;
  q = G_INPUT * q + G_OUTPUT * ep->last_qf;
  ep->last_pf     = p;
  ep->last_qf     = q;
  *active_power   = p;
  *reactive_power = q;
}