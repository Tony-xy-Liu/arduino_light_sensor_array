#include <SPI.h>
#include <SD.h>

const int CHANNELS_PER_MUX = 3;
const int MUX_BITS = 4;
const int MUX_CONTROL_PINS[MUX_BITS] = {2, 3, 4, 5};
const int N_MULIPLEXERS = 1;
// const int MUXES[N_MULIPLEXERS] = {A0, A1};
const int MUXES[N_MULIPLEXERS] = {A3};

void set_channel(int ch) {
  for (int i = MUX_BITS-1; i>=0; i--) {
    digitalWrite(MUX_CONTROL_PINS[i], bitRead(ch, i)); // bitRead converts values to binary
  }
  delay(5);
}

int read(int mux) {
  int samples = 3;
  int sum = 0;
  for (int i=0; i<samples; i++) {
    sum = sum + analogRead(mux);
    // delay(1);
  }
  // return (int)((float(sum) / float(samples)*10)+0.5);
  return (int)((float(sum) / float(samples))+0.5);
}

void read_array(int buffer[]) {
  for (int mux_channel=0; mux_channel<CHANNELS_PER_MUX; mux_channel++) {
    set_channel(mux_channel);
    for (int mux_i=0; mux_i<N_MULIPLEXERS; mux_i++) {
      // buffer iterates all channels of one mux before moving on to the next
      int i = mux_channel+(CHANNELS_PER_MUX*mux_i);
      buffer[i] = read(MUXES[mux_i]);
    }
  }
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

  set_channel(0);
}

// unsigned long last_time = millis();
// int counter = 0;
void loop() {
  // unsigned long now = millis();
  // unsigned long delta_time = now-last_time;
  // if (delta_time < 500) {return;}
  // last_time = now;

  // Serial.println(">>");
  const int ARRAY_SIZE = CHANNELS_PER_MUX * N_MULIPLEXERS;
  int buffer[ARRAY_SIZE];
  read_array(buffer);
  for (int i=0; i<ARRAY_SIZE; i++) {
    Serial.print(buffer[i]);
    if (i<ARRAY_SIZE-1) Serial.print(",");
  }
  Serial.println("");

}

// scratch

// more complex version of read()
// too slow
// void sort_desc(int *arr) {
//   qsort(arr, 5, sizeof(arr[0]), [](const void *a, const void *b){return (*((int *)a) - *((int *)b));});
// }

// float read(int mux) {
//   int samples = 2;
//   int values[samples];
//   for (int i=0; i<samples; i++) {
//     values[i] = analogRead(mux);
//     // delay(1);
//   }
//   sort_desc(values);
//   int sum = 0;
//   const int CUTT_START = 0;
//   const int CUTT_END = 0;
//   for (int i=CUTT_START; i<samples-CUTT_END; i++) { // trim largest and smallest values in sorted array
//     sum += values[i];
//   }
//   return float(sum) / float(samples-CUTT_END-CUTT_END);
// }