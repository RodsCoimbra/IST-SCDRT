#include "lumminaire.h"

lumminaire::lumminaire(float _m, float _offset_R_Lux, float _Pmax, unsigned short _desk_number)
  : m{ _m }, offset_R_Lux{ _offset_R_Lux }, Pmax{ _Pmax }, G{ 0 }, desk_number{ _desk_number },
    occupied{ false }, lux_flag{ false }, duty_flag{ false }, ignore_reference{ false }, buffer_full{ false }, idx_buffer{ 0 } {
}

void lumminaire::store_buffer(float lux) {
  last_minute_buffer_d[idx_buffer] = DutyCycle;
  last_minute_buffer_l[idx_buffer] = lux;
  idx_buffer++;
  if (idx_buffer == buffer_size) {
    idx_buffer = 0;
    buffer_full = true;
  }
}
