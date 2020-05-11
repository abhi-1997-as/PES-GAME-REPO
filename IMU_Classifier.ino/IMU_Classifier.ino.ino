#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>
#include <TensorFlowLite.h>
#include <tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h>
#include <tensorflow/lite/experimental/micro/micro_error_reporter.h>
#include <tensorflow/lite/experimental/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

#include "model.h"

#define BLE_UUID_TEST_SERVICE               "19B10010-E8F2-537E-4F6C-D104768A1214"

BLEService testService( BLE_UUID_TEST_SERVICE );
BLEIntCharacteristic Gesture1Char("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic Gesture2Char("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic Gesture3Char("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic Gesture4Char("19B10000-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

const int BLE_LED_PIN = LED_BUILTIN;

const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;

int samplesRead = numSamples;

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::ops::micro::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize];

// array to map gesture index to a name
const char* GESTURES[] = {
  "left",
  "right",
  "jump",
  "duck"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

int left=0;
int right=0;
int jump=0;
int duck=0;
void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  pinMode( BLE_LED_PIN, OUTPUT );
 
  // initialize the IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // print out the samples rates of the IMUs
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");

  Serial.println();

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);

  if( setupBleMode() )
  {
    digitalWrite( BLE_LED_PIN, HIGH );
  }
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;

// poll for BLE events
  BLE.poll();
  
   if(samplesRead == numSamples) {
    if (IMU.accelerationAvailable()) {
      // read the acceleration data
      IMU.readAcceleration(aX, aY, aZ);

      // sum up the absolutes
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // check if it's above the threshold
      if (aSum >= accelerationThreshold) {
        // reset the sample read count
        samplesRead = 0;
        
      }
    }
 }
   

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  if (samplesRead < numSamples) {
    // check if new acceleration AND gyroscope data is available
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      // read the acceleration and gyroscope data
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ); 

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

   samplesRead++;


      if (samplesRead == numSamples) {
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          while (1);
          return;
        }

        // Loop through the output tensor values from the model
          for (int i = 0; i < NUM_GESTURES; i++) 
              {
                  Serial.print(GESTURES[i]);
                  Serial.print(": ");
                  Serial.println(tflOutputTensor->data.f[i], 6);
                    
               }
           if(tflOutputTensor->data.f[0] >0.5)
               {         
                  left=1;
                  Gesture1Char.writeValue(left);
               }
           if(tflOutputTensor->data.f[1] >0.5)
               { 
                  right=1;
                  Gesture2Char.writeValue(right);
               }
           if(tflOutputTensor->data.f[2] >0.5)
               {
                  jump=1;
                  Gesture3Char.writeValue(jump);
               }
           if(tflOutputTensor->data.f[3] >0.5)
               {     
                  duck=1;
                  Gesture4Char.writeValue(duck);
               }
        Serial.println();
      }
    }
  }   
}

bool setupBleMode()
{
  if ( !BLE.begin() )
  { 
    Serial.println("starting BLE failed!");
    return false;
  }

  // set advertised local name and service UUID:
  BLE.setLocalName( "House Run" );
  BLE.setAdvertisedService( testService );

  testService.addCharacteristic(Gesture1Char);
  testService.addCharacteristic(Gesture2Char);
  testService.addCharacteristic(Gesture3Char);
  testService.addCharacteristic(Gesture4Char);
   // add service
  BLE.addService( testService );

  Gesture1Char.writeValue(0);
  Gesture2Char.writeValue(0);
  Gesture3Char.writeValue(0);
  Gesture4Char.writeValue(0);
  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
  return true;
}
