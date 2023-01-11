#include "arduino_secrets.h"
#include <Braccio++.h>              // *** must include before "thingProperties.h" else program terminated
#include "thingProperties.h"
#include "HUSKYLENS.h"

HUSKYLENS huskylens;
// HUSKYLENS green line >> SDA (A4); blue line >> SCL (A5)


// Braccio ++ joints
auto gripper    = Braccio.get(1);
auto wristRoll  = Braccio.get(2);
auto wristPitch = Braccio.get(3);
auto elbow      = Braccio.get(4);
auto shoulder   = Braccio.get(5);
auto base       = Braccio.get(6);
float currentAngle;


void setup() {
  Serial.begin(9600);

  cloud_message = "Starting Braccio.....";
  Braccio.begin();                        // *** Bracio must activate before  cloud connection, else Nano RP2040 chip crashfr equently
  delay(1000);
  Braccio.moveTo(220, 176, 240, 220, 177, 97);
  delay(1000);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  delay(1000); // Waits for the cloud connection
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  //Wait to get cloud connection to init the carrier
  while (ArduinoCloud.connected() != 1) {
    ArduinoCloud.update();
    Serial.println("waiting for cloud connection....");
    delay(500);
  }


  Wire.begin();
  while (!huskylens.begin(Wire))
  {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(500);
  }
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_CLASSIFICATION);
  huskylens.loadModelFromSDCard(1);
  delay(4000); //wait 4s for reading from SDCard;

}

void loop() {
  ArduinoCloud.update();

  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else 
  {
    bool pest = false;
    bool vege = false;
    HUSKYLENSResult result = huskylens.read();
    if (result.command == COMMAND_RETURN_BLOCK) {
      switch (result.ID) {
      case 1:
        move_object("Pest: Diamondback Moths",1);
        break;
      case 2:
        move_object("Pest: Whiteflies",1);
        break;
      case 3:
        move_object("Pest: Flea Beetles",1);
        break;
      case 4:
        move_object("Pest: Awl Snails",1);
        break;
      case 5:
        move_object("Vege: Chinese Spinach",2);
        break;
      case 6:
        move_object("Vege: Kale",2);
        break;
      case 7:
        move_object("Vege: Chai Xin",2);
        break;
      case 8:
        move_object("Vege: Kailan",2);
        break;
      }
    }
  }
  delay(1000);
}

void move_object(String message, int object_type) {

  huskylens.customText(message, 230, 35);   // display object info on huskylens LCD
  cloud_message = message;                  // display object info on cloud daskboard
  if (object_type == 1) {                   // Move object to Pest container   
    Braccio.moveTo(220,176,210,240,150,97);
    delay(1000);
    Braccio.moveTo(195,176,210,240,150,97);          
    delay(1000);
    Braccio.moveTo(195,176,210,240,140,97);          
    delay(1000);
    base.move().to(base.position() - 62);
    Braccio.moveTo(195,176,200,210,180,35);
    delay(500);
    Braccio.moveTo(220,176,200,210,180,35);
    delay(500);
    Braccio.moveTo(220,176,210,240,140,97);          
    delay(500);
  }
  if (object_type == 2) {                   // move object to Vege container  
    Braccio.moveTo(220,176,210,240,150,97);
    delay(1000);
    Braccio.moveTo(195,176,210,240,150,97);          
    delay(1000);
    Braccio.moveTo(195,176,210,240,140,97);          
    delay(1000);
    base.move().to(base.position() - 37);
    Braccio.moveTo(195,176,200,210,180,60);
    delay(500);
    Braccio.moveTo(220,176,200,210,180,60);
    delay(500);
    Braccio.moveTo(220,176,210,240,140,97);          
    delay(500);
  }  
  Braccio.moveTo(220, 176, 240, 220, 177, 97);
  delay(1500);
}
