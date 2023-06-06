#include <Arduino.h>

//#include <Gyver433.h>
 //Gyver433_RX<22, 64> rx;
#define DEBUG_LVL 4
#include "log.h"

#include "helpers.h"
#include "decoder.h"

#define LEDPIN 23
#define RXPIN 22

uint64_t tic = 0;


void IRAM_ATTR isr() {
  DecoderTrig();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(RXPIN, INPUT_PULLUP);
  pinMode(BUILTIN_LED, OUTPUT);
  
  delay(500);
  Serial.println("UP!!");

  config_receiver(RXPIN);

	//config_receiver(RXPIN);

  // Take the interruptions on Change
  attachInterrupt(RXPIN, isr, CHANGE);
}

char payload[180] = "";
char topic[16] = "";

byte a, b, c, d, e, f;
int temp;

unsigned long lastMS = 0;

void loop() {

  if (dec_has_data) {
    digitalWrite(LED_BUILTIN, HIGH);
    DecoderData data = get_decoder_data();
    Serial.println("Got data:");
    Serial.print("Length: "); Serial.println(data.length);
    Serial.print("Data_S: "); Serial.println(data.data_s);
    Serial.print("Data_B: "); Serial.println(data.data_b, BIN);
    Serial.print("Data_B: "); Serial.println(data.data_b, HEX);

    for (int i = 0; i < 56; i+=8) {
      unsigned long b = (data.data_b >> i) & 0xFFUL;//(data.data_b & (0xFF << i)) >> i;
      Serial.print("D: "); Serial.print(i);
      Serial.print(", "); Serial.print(b, HEX); 
      Serial.print(" "); Serial.println(b, DEC);
    }
    a    = data.data_b & 0xFFUL;
    b    = (data.data_b >> 8) & 0xFFUL;
    c    = (data.data_b >> 16) & 0xFFUL;
    d    = (data.data_b >> 32) & 0xFFUL;
    e    = (data.data_b >> 40) & 0xFFUL;
    f    = (data.data_b >> 48) & 0xFFUL;
    temp =  (int8_t)(data.data_b >> 16) & 0xFFL;

    snprintf(payload, 180, "{ \"temp\": %d, \"a\": %u, \"b\": %u, \"c\": %u, \"d\": %u, \"e\": %u, \"f\": %u, \"data\": \"%llx\" }",
             temp,
             a,
             b,
             c,
             d,
             e,
             f,
             data.data_b
      );
      
    for (int i = 55; i >= 0; i--) {
      unsigned long b = (data.data_b >> i) & 1;//(data.data_b & (0xFF << i)) >> i;
      Serial.print(b);
    }
    Serial.println("");
    for (int i = 0; i < 56; i++) {
      unsigned long b = (data.data_b >> i) & 1;//(data.data_b & (0xFF << i)) >> i;
      Serial.print(b);
    }
    Serial.println("");

    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
  }
  //delay(100);
  //Serial.println(tic);
  if (millis() - lastMS > 10000) {
    Serial.print(tic);
    Serial.print(".");
    lastMS = millis();
  }
}