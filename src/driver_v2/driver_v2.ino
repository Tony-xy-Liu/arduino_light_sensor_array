#include <SPI.h>
#include <SD.h>

const int CHANNELS_PER_MUX = 12;
const int MUX_BITS = 4;
const int MUX_CONTROL_PINS[MUX_BITS] = {5, 4, 3, 2};
const int N_MULIPLEXERS = 2;
const int MUXES[] = {A4, A5};
const int INDICATOR_PIN = 8;
// const long USB_BAUD = 4800;
// const long USB_BAUD = 115200;
const long USB_BAUD = 500000;

void set_channel(int ch) {
  for (int i=0; i<MUX_BITS; i++) {
    bool sig = bitRead(ch, i);
    digitalWrite(MUX_CONTROL_PINS[i], sig);
  }
}

void sort_desc(int *arr) {
  qsort(arr, 5, sizeof(arr[0]), [](const void *a, const void *b){return (*((int *)a) - *((int *)b));});
}

float read(int mux) {
  int samples = 15;
  int values[samples];
  for (int i=0; i<samples; i++) {
    values[i] = analogRead(mux);
  }
  sort_desc(values);
  int sum = 0;
  const int CUTT_START = 1;
  const int CUTT_END = 1;
  for (int i=CUTT_START; i<samples-CUTT_END; i++) { // trim largest and smallest values in sorted array
    sum += values[i];
  }
  return float(sum) / float(samples-CUTT_END-CUTT_END);
}

void read_array(float buffer[]) {
  for (int mux_channel=0; mux_channel<CHANNELS_PER_MUX; mux_channel++) {
    set_channel(mux_channel);
    for (int mux_i=0; mux_i<N_MULIPLEXERS; mux_i++) {
    
    }
    for (int mux_i=0; mux_i<N_MULIPLEXERS; mux_i++) {
      // buffer iterates all channels of one mux before moving on to the next
      int i = mux_channel+(CHANNELS_PER_MUX*mux_i);
      buffer[i] = read(MUXES[mux_i]);
    }
  }
}

void monitor_array(long now, int counter) {
  const int ARRAY_SIZE = CHANNELS_PER_MUX * N_MULIPLEXERS;
  float buffer[ARRAY_SIZE];
  read_array(buffer);
  Serial.print("[");
  Serial.print(now);
  Serial.print(",");
  Serial.print(counter);
  Serial.print(",");
  for (int i=0; i<ARRAY_SIZE; i++) {
    float val = buffer[i];
    Serial.print(val);
    if (i<ARRAY_SIZE-1) Serial.print(",");
  }
  Serial.println("]");
}

// for testing single channel of all muxes
void monitor_one(int ch) {
  set_channel(ch);
  for (int i=0; i<N_MULIPLEXERS; i++) {
    Serial.print(read(MUXES[i]));
    if (i<N_MULIPLEXERS-1) Serial.print(",");
  }
}

void setup() {
  Serial.begin(USB_BAUD);
  Serial.println("Setup started...");

  for (int i=0; i<N_MULIPLEXERS; i++) {
    pinMode(MUXES[i], INPUT);
  }

  for (int i=0; i<MUX_BITS; i++) {
    pinMode(MUX_CONTROL_PINS[i], OUTPUT);
  }

  pinMode(INDICATOR_PIN, OUTPUT);
}

unsigned long last_time = millis();
int counter = 0;
bool led = 0;
void loop() {
  unsigned long now = millis();
  counter = counter + 1;
  led = !led;

  monitor_array(now, counter);
  digitalWrite(INDICATOR_PIN, led);

  // Test single channel
  // monitor_one(3);
  // // Serial.print(",");
  // // monitor_one(0);
  // Serial.println("");
}
