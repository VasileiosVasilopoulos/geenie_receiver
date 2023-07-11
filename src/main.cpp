/*****************************************************************************************
*     Serial Bluetooth communication between 2 ESP32 boards - OK1TK www.ok1tk.com        *
*                   The SERVER (Master) part code v1.32 - 04 May 2023                    *
******************************************************************************************/
#include "BluetoothSerial.h"                                                              // BT: Include the Serial bluetooth library
unsigned long previousMillisReconnect;                                                    // BT: Variable used for comparing millis counter for the reconnection timer
bool SlaveConnected;                                                                      // BT: Variable used to store the current connection state (true=connected/false=disconnected)
int recatt = 0;                                                                           // BT: Variable used to count the reconnection attempts

// BT: Bluetooth availabilty check
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

String myName = "Geenie Receiver";                                                        // BT: Variable used to store the SERVER(Master) bluetooth device name; just for prinitng
String slaveName = "Geenie";                                                      // BT: Variable used to store the CLIENT(Slave) bluetooth device name; just for prinitng; just for printing in this case
String MACadd = "C4:DD:57:9D:08:2A";                                                      // BT: Variable used to store the CLIENT(Slave) bluetooth device Mac address; just for prinitng; just for printing in this case
uint8_t address[6] = { 0xC4, 0xDD, 0x57, 0x9D, 0x08, 0x2A };                              // BT: Variable used to store the CLIENT(Slave) MAC address used for the connection; Use your own andress in the same format

BluetoothSerial SerialBT;                                                                 // BT: Set the Object SerialBT


// BT: Bt_Status callback function
void Bt_Status(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_OPEN_EVT) {                                                        // BT: Checks if the SPP connection is open, the event comes// event == Client connected
    Serial.println ("Client Connected");                                                  // BT: Write to the serial monitor
    SlaveConnected = true;                                                                // BT: Set the variable true = CLIENT is connected to the SERVER
    recatt = 0;                                                                           // BT: Reset the reconnect attempts counter
  }
  else if (event == ESP_SPP_CLOSE_EVT) {                                                  // BT: event == Client disconnected
    Serial.println("Client Disconnected");                                                // BT: Write to the serial monitor
    SlaveConnected = false;                                                               // BT: Set the variable false = CLIENT connection lost
  }
}

void SlaveConnect() {                                                                     // BT: This function connects/reconnects to the CLIENT(Slave)
  Serial.println("Function BT connection executed");                                      // BT: Write to the serial monitor
  Serial.printf("Connecting to slave BT device named \"%s\" and MAC address \"%s\" is started.\n", slaveName.c_str(), MACadd.c_str());  // BT: Write to the serial monitor
  SerialBT.connect(address);                                                              // BT: Establishing the connection with the CLIENT(Slave) with the Mac address stored in the address variable
}

void setup() {
  SlaveConnected = false;                                                                 // BT: Set the variable false = CLIENT is not connected
  Serial.begin(115200);                                                                   // Sets the data rate in bits per second (baud) for serial data transmission
  
  // BT: Define the Bt_Status callback
  SerialBT.register_callback(Bt_Status);
  // BT: Starts the bluetooth device with the name stored in the myName variable as SERVER(Master)
  SerialBT.begin(myName, true);
  Serial.printf("The device \"%s\" started in master mode, make sure slave BT device is on!\n", myName.c_str());
  SlaveConnect();                                                                         // BT: Calls the bluetooth connection function to cnnect to the CLIENT(Slave)
}

void loop() {
  
  if (!SlaveConnected) {                                                                  // BT: Condition to evalute if the connection is established/lost 
    if (millis() - previousMillisReconnect >= 10000) {                                    // BT: Check that 10000ms is passed
      previousMillisReconnect = millis();                                                 // BT: Set previousMillisReconnect to current millis
      recatt++;                                                                           // BT: Increase the the reconnection attempts counter +1 
      Serial.print("Trying to reconnect. Attempt No.: ");                                 // BT: Write to the serial monitor
      Serial.println(recatt);                                                             // BT: Write the attempts count to the serial monitor
      Serial.println("Stopping Bluetooth...");                                            // BT: Write to the serial monitor
      SerialBT.end();                                                                     // BT: Close the bluetooth device
      Serial.println("Bluetooth stopped !");                                              // BT: Write to the serial monitor
      Serial.println("Starting Bluetooth...");                                            // BT: Write to the serial monitor
      SerialBT.begin(myName, true);                                                       // BT: Starts the bluetooth device with the name stored in the myName variable as SERVER(Master)
      Serial.printf("The device \"%s\" started in master mode, make sure slave BT device is on!\n", myName.c_str());
      SlaveConnect();                                                                     // BT: Calls the bluetooth connection function to cnnect to the CLIENT(Slave)
    }
    
  }
  // BT: Data send/receive via bluetooth
  if (Serial.available()) {                                                               // BT: Checks if there are data from the serial monitor available
    SerialBT.write(Serial.read());                                                        // BT: Sends the data via bluetooth
  }
  if (SerialBT.available()) {                                                             // BT: Checks if there are data from the bluetooth available
    Serial.write(SerialBT.read());                                                        // BT: Write the data to the serial monitor
  }
}