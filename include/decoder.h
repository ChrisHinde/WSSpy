struct DecoderData
{
  char data_s[128];
  uint64_t data_b;

  uint8_t length;
};



void config_receiver (uint8_t pin);
DecoderData get_decoder_data(bool reset_has_data = true);
void IRAM_ATTR DecoderTrig (void);

extern volatile bool dec_enable;

extern volatile bool dec_has_data;



enum DecoderState { 
  IDLE, 

  // NEXUX States
  CATCH_SYNC_HIGH, 
  CATCH_SYNC_LOW, 
  READ_BIT_START,
  READ_BIT_END

};
