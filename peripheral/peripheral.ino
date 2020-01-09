// ---------------------------------------------------------------------------
// Robot controlado remotamente por App:  - v1.0 - 03/01/2020
//
// AUTOR:
// Creado por Angel Villanueva - @avilmaru
//
// LINKS:
// Blog: http://www.mecatronicalab.es
//
//
// HISTORICO:
// 03/01/2020 v1.0 - Release inicial.
//
// ---------------------------------------------------------------------------

#include <ArduinoBLE.h>
#include <MKRMotorCarrier.h>

const char* deviceServiceUuid = "0000FFE0-0000-1000-8000-00805F9B34FB";
const char* deviceServiceCharacteristicUuid = "0000FFE1-0000-1000-8000-00805F9B34FB";

String command = "";

//Variable to store the battery voltage
static int batteryVoltage;
//Variable to change the motor speed and direction
static int dutyX = 0;
static int dutyY = 0;
static int duty1 = 0;
static int duty2 = 0;

// BLE gesture Service
BLEService movementService(deviceServiceUuid); 

// BLE gesture Switch Characteristic 
BLEStringCharacteristic movementCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite,512);


void setup() {
  
  //Serial.begin(9600);
  //while (!Serial);

  //Establishing the communication with the motor shield
  if (controller.begin()) 
    {
      //Serial.print("MKR Motor Shield connected, firmware version ");
      //Serial.println(controller.getFWVersion());
    } 
  else 
    {
      //Serial.println("Couldn't connect! Is the red led blinking? You may need to update the firmware with FWUpdater sketch");
      while (1);
    }

  // Reboot the motor controller; brings every value back to default
  //Serial.println("Reboot the motor controller");
  controller.reboot();
  delay(500);
  // unused
  M3.setDuty(0);
  M4.setDuty(0);
  
  // begin ble initialization
  if (!BLE.begin()) {
    //Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("MKR RC Robot");
  BLE.setAdvertisedService(movementService);

  // add the characteristic to the service
  movementService.addCharacteristic(movementCharacteristic);

  // add service
  BLE.addService(movementService);

  // set the initial value for the characeristic:
  movementCharacteristic.writeValue("");

  // start advertising
  BLE.advertise();

  //Serial.println("MKR RC Robot Peripheral");
}

void loop() {
  
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    
    //Serial.print("Connected to central: ");
    // print the central's MAC address:
    //Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {

      // if the remote device wrote to the characteristic,
      if (movementCharacteristic.written()) {
         command = movementCharacteristic.value();
         //Serial.print(F("commmand value:  "));
         //Serial.println(command);
         sendInstruction(command);
       }
      
    }

    // when the central disconnects, print it out:
    //Serial.print(F("Disconnected from central: "));
    //Serial.println(central.address());
  }
}

void sendInstruction(String str) {

  if (str.length() == 0)
    return;
    
  //Take the battery status
  float batteryVoltage = (float)battery.getConverted();

  //Reset to the default values if the battery level is lower than 7V
  if (batteryVoltage < 7) 
  {
    //Serial.println(" ");
    //Serial.println("WARNING: LOW BATTERY");
    //Serial.println("ALL SYSTEMS DOWN");
    M1.setDuty(0);
    M2.setDuty(0);

    while (batteryVoltage < 7) 
    {
      batteryVoltage = (float)battery.getConverted();
    }
  }
  else
  {
    if (str == "F")    //  Forward
    {
        duty1 = 80;
        duty2 = 80;
    }
    else if (str == "B") // Backward 
    {
        duty1 = -80;
        duty2 = -80;
    }
    else if (str == "L") // Turn Left  
    {
        duty1 = -40;
        duty2 = 40;
    }
    else if (str == "R") // Turn Right 
    {
        duty1 = 40;
        duty2 = -40;
    }
    else if (str == "S") // Stop 
    {
        duty1 = 0;
        duty2 = 0;
    }
    else
    {
        duty1 = 0;
        duty2 = 0;
    }


    //Serial.print(F("valor en duty1: "));
    //Serial.print(duty1);
    //Serial.print(F("\t valor en duty2: "));
    //Serial.println(duty2);
  
    M1.setDuty(duty1);
    M2.setDuty(duty2); 
       
       
    //Keep active the communication MKR1010 & MKRMotorCarrier
    //Ping the samd11
    controller.ping();
    //wait
    delay(1);
  
  }

}
