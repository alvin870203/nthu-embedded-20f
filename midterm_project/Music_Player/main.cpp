#include "DA7212.h"

#include "accelerometer_handler.h"
#include "config.h"
#include "magic_wand_model_data.h"

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#include "mbed.h"
#include "uLCD_4DGL.h"
#include <cmath>

DA7212 audio;
uLCD_4DGL uLCD(D1, D0, D2);


DigitalIn sw3(SW3);
//DigitalIn sw2(SW2);
InterruptIn sw2(SW2);
DigitalOut green_led(LED2);



#define bufferLength (32)
#define signalLength (84)


Serial pc(USBTX, USBRX);
int16_t waveform[kAudioTxBufferSize];

float signal0[signalLength];
int song[42];
int noteLength[42];
char serialInBuffer[bufferLength];
int serialCount = 0;

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;
int idC = 0;

bool exitModeSelection = false;
int whichSong = 0;
int whichMode = 0;
bool stopSong = false;
int songIndex = 0;

/*
int song[42] = {
  261, 261, 392, 392, 440, 440, 392,
  349, 349, 330, 330, 294, 294, 261,
  392, 392, 349, 349, 330, 330, 294,
  392, 392, 349, 349, 330, 330, 294,
  261, 261, 392, 392, 440, 440, 392,
  349, 349, 330, 330, 294, 294, 261};

int noteLength[42] = {
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2};
*/
///////////////////////////////////////////////////////////////
  // Create an area of memory to use for input, output, and intermediate arrays.
  // The size of this will depend on the model you're using, and may need to be
  // determined by experimentation.
  constexpr int kTensorArenaSize = 60 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];

  // Whether we should clear the buffer next time we fetch data
  bool should_clear_buffer = false;
  bool got_data = false;

  // The gesture index of the prediction
  int gesture_index;

  // Set up logging.
  static tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);


  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroOpResolver<6> micro_op_resolver;


  // Build an interpreter to run the model with
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  tflite::MicroInterpreter* interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors


  // Obtain pointer to the model's input tensor
  TfLiteTensor* model_input = interpreter->input(0);


  int input_length = model_input->bytes / sizeof(float);

  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);



/////////////////////////////////////////////////////////////////





void sw3Fall (bool &exitModeSelection)
{
  if (sw3 == 0) {
    if (!exitModeSelection) {
      exitModeSelection = true;
    } else {
      exitModeSelection = false;
    }
  }
}

/*
void sw2Fall (bool &stopSong)
{
  if (sw2 == 0) {
    stopSong = true;
  }
}
*/

void sw2Fall ()
{
  stopSong = true;
  green_led = !green_led;
}

void loadingSong(int whichLoading)
{
  switch (whichLoading)
  {
  case 0:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.printf("\nLoading...SongA\n"); //Default Green on black text
    break;
  case 1:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.printf("\nLoading...SongB\n"); //Default Green on black text
    break;
  case 2:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.printf("\nLoading...SongC\n"); //Default Green on black text
    break;
  default:
    break;
  }
}

void playingSong(int whichPlaying)
{
  switch (whichPlaying)
  {
  case 0:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.printf("\nPlaying...SongA\n"); //Default Green on black text
    break;
  case 1:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.printf("\nPlaying...SongB\n"); //Default Green on black text
    break;
  case 2:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.printf("\nPlaying...SongC\n"); //Default Green on black text
    break;
  default:
    break;
  }
}

void showSong(int whichSong)
{
  switch (whichSong)
  {
  case 0:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.textbackground_color(RED);
    uLCD.printf("\nSongA\n"); //Default Green on black text
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nSongB\n"); //Default Green on black text
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nSongC\n"); //Default Green on black text
    break;
  case 1:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nSongA\n"); //Default Green on black text
    uLCD.textbackground_color(RED);
    uLCD.printf("\nSongB\n"); //Default Green on black text
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nSongC\n"); //Default Green on black text
    break;
  case 2:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nSongA\n"); //Default Green on black text
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nSongB\n"); //Default Green on black text
    uLCD.textbackground_color(RED);
    uLCD.printf("\nSongC\n"); //Default Green on black text
    break;
  default:
    break;
  }
}

void showMode(int whichMode)
{
  switch (whichMode)
  {
  case 0:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.textbackground_color(RED);
    uLCD.printf("\nNext Song\n"); //Default Green on black text
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nPrevious Song\n"); //Default Green on black text
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nSong Selection\n"); //Default Green on black text
    break;
  case 1:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nNext Song\n"); //Default Green on black text
    uLCD.textbackground_color(RED);
    uLCD.printf("\nPrevious Song\n"); //Default Green on black text
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nSong Selection\n"); //Default Green on black text
    break;
  case 2:
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nNext Song\n"); //Default Green on black text
    uLCD.textbackground_color(BLACK);
    uLCD.printf("\nPrevious Song\n"); //Default Green on black text
    uLCD.textbackground_color(RED);
    uLCD.printf("\nSong Selection\n"); //Default Green on black text
    break;
  default:
    break;
  }
}



// Return the result of the last prediction
int PredictGesture(float* output) {
  // How many times the most recent gesture has been matched in a row
  static int continuous_count = 0;
  // The result of the last prediction
  static int last_predict = -1;

  // Find whichever output has a probability > 0.8 (they sum to 1)
  int this_predict = -1;
  for (int i = 0; i < label_num; i++) {
    if (output[i] > 0.8) this_predict = i;
  }

  // No gesture was detected above the threshold
  if (this_predict == -1) {
    continuous_count = 0;
    last_predict = label_num;
    return label_num;
  }

  if (last_predict == this_predict) {
    continuous_count += 1;
  } else {
    continuous_count = 0;
  }
  last_predict = this_predict;

  // If we haven't yet had enough consecutive matches for this gesture,
  // report a negative result
  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
    return label_num;
  }
  // Otherwise, we've seen a positive result, so clear all our variables
  // and report it
  continuous_count = 0;
  last_predict = -1;

  return this_predict;
}

void loadSignal(int whichSongtoLoad)
{
  switch (whichSongtoLoad)
  {
  case 0:
    printf("SongA\n");
    break;
  case 1:
    printf("SongB\n");
    break;
  case 2:
    printf("SongC\n");
    break;
  default:
    break;
  }
  
  green_led = 0;
  int i = 0;
  serialCount = 0;
  audio.spk.pause();
  while(i < signalLength)
  {
    if(pc.readable())
    {
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if(serialCount == 5)
      {
        serialInBuffer[serialCount] = '\0';
        signal0[i] = (float) atof(serialInBuffer);
        serialCount = 0;
        i++;
      }
    }
  }
  for (int j = 0; j < 42; j++)
  {
    song[j] = (int) (signal0[j] * 1000);
  }
  for (int k = 42; k < 84; k++)
  {
    noteLength[k-42] = (int) (signal0[k] * 10);
  }
  green_led = 1;
}


void playNote(int freq)
{
  for(int i = 0; i < kAudioTxBufferSize; i++)
  {
    waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
  }
  audio.spk.play(waveform, kAudioTxBufferSize);
}


void playSong(int songIndex, bool &stopSong)
{
  switch (songIndex)
  {
  case 0:
    loadingSong(abs(songIndex)%3);
    loadSignal(abs(songIndex)%3);
    playingSong(abs(songIndex)%3);
    for (int i = 0; (i < 42) && (!stopSong); i++) {
      int length = noteLength[i];
      while(length--)
      {
        // the loop below will play the note for the duration of 1s
        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
        {
          queue.call(playNote,song[i]);
        }
        if(length < 1) wait(1.0);
      }
      //sw2Fall(stopSong);
      sw2.fall(sw2Fall);
    }
    audio.spk.pause();
    break;
  case 1:
    loadingSong(abs(songIndex)%3);
    loadSignal(abs(songIndex)%3);
    playingSong(abs(songIndex)%3);
    for (int i = 0; (i < 42) && (!stopSong); i++) {
      int length = noteLength[i];
      while(length--)
      {
        // the loop below will play the note for the duration of 1s
        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
        {
          queue.call(playNote,song[i]);
        }
        if(length < 1) wait(1.0);
      }
      //sw2Fall(stopSong);
      sw2.fall(sw2Fall);
    }
    audio.spk.pause();
    break;
  case 2:
    loadingSong(abs(songIndex)%3);
    loadSignal(abs(songIndex)%3);
    playingSong(abs(songIndex)%3);
    for (int i = 0; (i < 42) && (!stopSong); i++) {
      int length = noteLength[i];
      while(length--)
      {
        // the loop below will play the note for the duration of 1s
        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
        {
          queue.call(playNote,song[i]);
        }
        if(length < 1) wait(1.0);
      }
      //sw2Fall(stopSong);
      sw2.fall(sw2Fall);
    }
    audio.spk.pause();
    break;
  default:
    break;
  }
}


int main(int argc, char* argv[]) {



  t.start(callback(&queue, &EventQueue::dispatch_forever));
  //sw2.fall(queue.event(&toggleLED));

  green_led = 1;

  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return -1;
  }

  micro_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                               tflite::ops::micro::Register_MAX_POOL_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                               tflite::ops::micro::Register_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                               tflite::ops::micro::Register_FULLY_CONNECTED());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                               tflite::ops::micro::Register_SOFTMAX());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                             tflite::ops::micro::Register_RESHAPE(), 1);

  interpreter->AllocateTensors();

  error_reporter->Report("Set up successful...\n");

  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != config.seq_length) ||
      (model_input->dims->data[2] != kChannelNumber) ||
      (model_input->type != kTfLiteFloat32)) {
    error_reporter->Report("Bad input tensor parameters in model");
    return -1;
  }

  if (setup_status != kTfLiteOk) {
    error_reporter->Report("Set up failed\n");
    return -1;
  }


  while (true)
  {
    exitModeSelection = false;
    whichSong = 0;
    whichMode = 0;

    uLCD.background_color(BLACK);
    uLCD.cls();

    showMode(abs(whichMode)%3);
    
    green_led = 1;

    while (!exitModeSelection) {
      sw3Fall(exitModeSelection); 
      // Attempt to read new data from the accelerometer
      got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                  input_length, should_clear_buffer);

      // If there was no new data,
      // don't try to clear the buffer again and wait until next time
      if (!got_data) {
        should_clear_buffer = false;
        continue;
      }

      // Run inference, and report any error
      TfLiteStatus invoke_status = interpreter->Invoke();
      if (invoke_status != kTfLiteOk) {
        error_reporter->Report("Invoke failed on index: %d\n", begin_index);
        continue;
      }

      // Analyze the results to obtain a prediction
      gesture_index = PredictGesture(interpreter->output(0)->data.f);

      // Clear the buffer next time we read data
      should_clear_buffer = gesture_index < label_num;

      // Produce an output
      if (gesture_index < label_num) {
        error_reporter->Report(config.output_message[gesture_index]);
        if (gesture_index == 0) {
          whichMode++;
        } else {
          whichMode--;
        }
        showMode(abs(whichMode)%3);
      }
      
    } // exit mode selection when exitModeSelection=true

    if ((abs(whichMode)%3) == 0) { // Next Song
      stopSong = false;
      songIndex++;
      stopSong = false;
      playSong(abs(songIndex)%3, stopSong);      
      
    } else if ((abs(whichMode)%3) == 1) { // Previous Song
      stopSong = false;
      songIndex--;
      stopSong = false;
      playSong(abs(songIndex)%3, stopSong);
    } else {
      whichSong = 0;
      exitModeSelection = true;
      showSong(abs(whichSong)%3);
      while (exitModeSelection) {
        sw3Fall(exitModeSelection);
        // Attempt to read new data from the accelerometer
        got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                    input_length, should_clear_buffer);

        // If there was no new data,
        // don't try to clear the buffer again and wait until next time
        if (!got_data) {
          should_clear_buffer = false;
          continue;
        }

        // Run inference, and report any error
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk) {
          error_reporter->Report("Invoke failed on index: %d\n", begin_index);
          continue;
        }

        // Analyze the results to obtain a prediction
        gesture_index = PredictGesture(interpreter->output(0)->data.f);

        // Clear the buffer next time we read data
        should_clear_buffer = gesture_index < label_num;

        // Produce an output
        if (gesture_index < label_num) {
          error_reporter->Report(config.output_message[gesture_index]);
          if (gesture_index == 0) {
            whichSong++;
          } else {
            whichSong--;
          }
          showSong(abs(whichSong)%3);
        }
         
      } // exit Song selection when exitModeSelection=false
      songIndex = abs(whichSong)%3;
      stopSong = false;
      playSong(abs(songIndex)%3, stopSong);
      // stop play song when stopSong = true

    }

  }
}