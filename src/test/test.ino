#include <SPI.h>
#include <SD.h>

const int CHANNELS_PER_MUX = 12;
const int MUX_BITS = 4;
const int MUX_CONTROL_PINS[MUX_BITS] = {2, 3, 4, 5};
const int N_MULIPLEXERS = 2;
const int MUXES[N_MULIPLEXERS] = {A3, A4};

const int TEST = 13;


void set_channel(int ch) {
  for (int i = MUX_BITS-1; i>=0; i--) {
    digitalWrite(MUX_CONTROL_PINS[i], bitRead(ch, i)); // bitRead converts values to binary
  }
  delay(1);
}

int read(int mux) {
  int samples = 3;
  int sum = 0;
  for (int i=0; i<samples; i++) {
    sum = sum + analogRead(mux);
    delay(1);
  }
  // return (int)((float(sum) / float(samples)*10)+0.5);
  return (int)((float(sum) / float(samples))+0.5);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setup started...");

  for (int pin=0; pin<N_MULIPLEXERS; pin++) {
    pinMode(pin, INPUT);
  }

  for (int pin=0; pin<MUX_BITS; pin++) {
    pinMode(pin, OUTPUT);
  }

  pinMode(TEST, OUTPUT);
  digitalWrite(TEST, HIGH);
}

bool on_off = 0;
unsigned long last_time = millis();
int counter = 0;
void loop() {
  unsigned long now = millis();
  unsigned long delta_time = now-last_time;
  // if (delta_time < 500) {return;}
  last_time = now;

  if (on_off) {
    set_channel(11);
  } else {
    set_channel(10);
  }

  on_off = !on_off;
}

