#include <EloquentTinyML.h>  // Version 2.4.4
#include <eloquent_tinyml/tensorflow.h>
#include <Arduino.h>
#include <lx16a-servo.h>

// sine_model.h contains the array you exported from Python with xxd or tinymlgen
#include "ECG.h"

#define N_INPUTS 187
#define N_OUTPUTS 2
// in future projects you may need to tweak this value: it's a trial and error process
#define TENSOR_ARENA_SIZE 5 * 1024

Eloquent::TinyML::TensorFlow::TensorFlow<N_INPUTS, N_OUTPUTS, TENSOR_ARENA_SIZE> tf;

//CLASIFICADOR
float input[1][187];
float output_data = 0;


// Angle Arm Pos
float angles[7][5] = {
  { 10.0, 180.64, 180.64, 89.76, 88.08 },
  { 81.6, 106.32, 113.52, 90.24, 104.88 },
  { 81.6, 62.16, 54.96, 90.0, 80.64 },
  { 90.96, 76.32, 81.12, 90.24, 95.28 },
  { 69.84, 61.92, 69.84, 90.48, 94.56 },
  { 78.0, 46.08, 52.32, 90.48, 94.56 },
  { 10.0, 180.64, 180.64, 89.76, 88.08 }
};

// LEFT:10.0,180.64,180.64,89.76,88.08

// Servo Controll
LX16ABus servoBus;
LX16AServo joint_1(&servoBus, 7);
LX16AServo joint_2(&servoBus, 8);
LX16AServo joint_3(&servoBus, 9);
LX16AServo joint_4(&servoBus, 10);
LX16AServo joint_5(&servoBus, 11);

static float divisor = 4.1;
