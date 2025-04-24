#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow.h>
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


// Servo Controll
LX16ABus servoBus;
LX16AServo servo(&servoBus, 1);
LX16AServo servo2(&servoBus, 4);
LX16AServo servo3(&servoBus, 3);
