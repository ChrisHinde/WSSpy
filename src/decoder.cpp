
#include <Arduino.h>
#include "decoder.h"


#define PULSE_LENGTH_FIT(duration, discr, tol) (discr>duration-tol && discr<duration+tol)


#define DEC_PULSE_HIGH 500
#define DEC_PULSE_LOW_SYNC 4000
#define DEC_PULSE_LOW_ONE 2000
#define DEC_PULSE_LOW_ZERO 1000
#define DEC_BIT_TOLERANCE 100

#define DEC_PACKET_BITS_COUNT 56 // data bits in a packet

volatile uint32_t RX_last_change = 0;    // global var to store last signal transition timestamp

// protocol decoder variables
volatile uint64_t RX_Bits = 0;           // received data bit array
volatile uint8_t  RX_Bit_Counter = 0;    // received data bit counter
volatile uint8_t  RX_Pin = 14;           // GPIO pin connected to the receiver - default GPIO14

volatile bool dec_enable = true;
volatile bool dec_has_data = false;

volatile DecoderState state = CATCH_SYNC_HIGH;

char data[64] = "";


void config_receiver (uint8_t pin)
{
  RX_Pin = pin;
  //pinMode(pin, INPUT);
}

void IRAM_ATTR DecoderTrig ()
{
  if (! dec_enable) {
    return;
  }

  uint32_t current_timestamp = micros();
  uint32_t pulse_duration    = current_timestamp - RX_last_change;
  uint8_t  current_level     = digitalRead(RX_Pin);
  RX_last_change = current_timestamp;

  switch (state)
  {
    case CATCH_SYNC_HIGH:
      if (current_level == LOW) {
        if (PULSE_LENGTH_FIT(pulse_duration, DEC_PULSE_HIGH, DEC_BIT_TOLERANCE)) {
          state = CATCH_SYNC_LOW;
          return;
        } else
          break;
      }
      return;

    case CATCH_SYNC_LOW:
      if (current_level == HIGH) {
        if (PULSE_LENGTH_FIT(pulse_duration, DEC_PULSE_LOW_SYNC, DEC_BIT_TOLERANCE)) {
          state = READ_BIT_START;
          RX_Bit_Counter = 0;
          RX_Bits = 0;
          return;
        } else
          break;
      }
      return;

    case READ_BIT_START:
      if (current_level == LOW) {
        state = READ_BIT_END;
      }
      return;

    case READ_BIT_END:
      if (current_level == HIGH) {
        if (PULSE_LENGTH_FIT(pulse_duration, DEC_PULSE_LOW_ZERO, DEC_BIT_TOLERANCE)) {
          data[RX_Bit_Counter] = '0';
        } else if (PULSE_LENGTH_FIT(pulse_duration, DEC_PULSE_LOW_ONE, DEC_BIT_TOLERANCE)) {
          data[RX_Bit_Counter] = '1';
          RX_Bits |= (1ULL << (DEC_PACKET_BITS_COUNT - 1 - RX_Bit_Counter));
        } else {
          break;
        }

        RX_Bit_Counter++;
        state = READ_BIT_START;

        if (RX_Bit_Counter == DEC_PACKET_BITS_COUNT) {
          dec_enable = false;
          dec_has_data = true;
          data[RX_Bit_Counter] = 0;
          break;
        }
      }
      return;

    default:
      return;
  }
  
  // wrong timing or error, start over
  state = CATCH_SYNC_HIGH;
}

DecoderData
get_decoder_data(bool reset_has_data)
{
  DecoderData dec_data;// = new DecoderData();

  dec_data.data_b = RX_Bits;
  dec_data.length = RX_Bit_Counter;

  strncpy(dec_data.data_s, data, RX_Bit_Counter);
  dec_data.data_s[RX_Bit_Counter] = 0;

  if (reset_has_data)
    dec_has_data = false;

  dec_enable = true;

  return dec_data;
}