#include <stdint.h> 

typedef struct{
  int16_t* buff;
  uint8_t  size;
  uint8_t  start;
  uint8_t  end;  
  uint8_t  count;  
} CircularBuffer;

int16_t getElement(CircularBuffer *cb, int n);
int circularBufferInit(CircularBuffer* cb, uint8_t size);
int circularBufferIsEmpty(const CircularBuffer* cb);
int circularBufferIsFull(const CircularBuffer* cb);
void add2CircularBuffer(CircularBuffer* cb, int16_t voltage);
void circularBufferFree(CircularBuffer* cb);
void plotBufferFromStartToEnd(CircularBuffer *cb);

typedef struct{
  CircularBuffer voltage_buffer;
  int32_t last_pf;
  int32_t last_qf;
  uint8_t grid_frequency;
  uint8_t execution_rate;
  uint8_t delayed_index;
} ElectricalPower;

#define G_INPUT  0.003763
#define G_OUTPUT 0.9962

int  ElectricalPowerInit(ElectricalPower *ep, uint8_t grid_frequency, uint8_t min_grid_frequency, uint8_t execution_rate);
void calculatePQ(ElectricalPower *ep, int16_t voltage, int16_t current, int32_t *active_power, int32_t *reactive_power);
void calculatePQdroop(ElectricalPower *ep, int16_t voltage, int16_t current, int32_t *active_power, int32_t *reactive_power, uint8_t grid_frequency);