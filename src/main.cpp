#include <Arduino.h>
#include <esp_system.h>
#include "ZennoraCore.h"
#include "ZennoraCoreComms.h"
#include "ZennoraCoreSensor.h"
#include "ZennoraThisSensor.h"
#include <esp_system.h>
#include <esp_task_wdt.h>
#include <Ticker.h>


Ticker tickerStatus;
InitializationMQTTStatus mqttStatus;

// Task timing 
unsigned long previousMillis = 0; //used for the primary functionality 
unsigned long secondPreviousMillis = 0; // used for secondary functionality, like sending device stats but could be used for other things.
//


ZennoraThisSensor thisSensor;


void setup() {

  Serial.begin(115200);
  Wire.begin();

  delay(1000);
  
  Serial.println("Starting Zennora Core");

 
    

  //check for last reset reason
  sensor.printResetReason();
  restartReason = sensor.getResetReasonString();
    // Print and handle reset reason
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.printf("Reset reason: %d\n", reason);
  if (reason == ESP_RST_POWERON) {
      rtcRestartCount = 0;
      Serial.println("Power-on reset, RTC memory cleared.");
  } else {
      rtcRestartCount++;
      Serial.printf("Restart count: %d\n", rtcRestartCount);
  }



  coreStatus = core.begin();

  

  //if the wifi is fully initialized, otherwise fo into AP mode.
  if (coreStatus.wifiInitialized){
  
    mqttStatus = mqtt.begin();
        // Register the device-specific command handler.

    Serial.println(mqttStatus.connectionEstablished ? "MQTT connect successful" : "MQTT connect failed");
    Serial.println(mqttStatus.subscriptionsApplied ? "MQTT subscribe successful" : "MQTT subscribe failed");
    bool sensorStatus = sensor.begin();
    
    // restart reporting block
    if(mqttStatus.subscriptionsApplied){
      DeviceInfo restartInfo = sensor.sendRestartInfo();
      Serial.println("Sending restart info");
      mqtt.sendMessage(restartInfo.topic.c_str(), restartInfo.buffer, restartInfo.qos);
         //init the led indicator
      zedSwitch.begin();
      thisSensor.begin();
      zedSwitch.ledIndicator();
      


      delay(5000);
    }
    //end restart block
    

    if (sensorStatus) {
      Serial.println("Sensor/Time initialization successful");
    } else {
      Serial.println("Sensor/Time initialization failed");
    }
    // Register the device-specific command handler.

    mqtt.setDeviceCommandHandler([](char* topic, byte* payload, unsigned int length) {
    thisSensor.handleDeviceCommand(topic, payload, length);
    });

    tickerStatus.attach_ms(config.loopSpeed, [](){
          zedSwitch.sendStatus();
          //Serial.println("Sending status");
      });
    
    
    
    Serial.println("Initialization complete");
  } else {
    Serial.println("Wifi mode Initialization failed, going into AP mode");
    
  }


}



void loop() {
      // Ensure the MQTT client stays connected
    
    
    // Handle web server requests
    core.server.handleClient();  // Handle client requests
    //Serial.println("Handling client done");
    core.serialListener();       // Listen for core commands
    
    
    unsigned long currentMillis = millis();
    //only try to do donkey work if the wifi is fully initialized.
    if (coreStatus.wifiInitialized && mqttStatus.subscriptionsApplied ){
      //Serial.println("Wifi and MQTT initialized");
      mqtt.loop();
      if (currentMillis - previousMillis >= config.loopSpeed) {  // Adjustable loop speed
          previousMillis = currentMillis;
          //zedSwitch.toggleAll();
          zedSwitch.sendSwitchPackages260(mqtt);



          

      }
  
      if (currentMillis - secondPreviousMillis >= config.loopSpeed*5) { //takes advantage of the assigable loop speed.
          secondPreviousMillis = currentMillis;
          DeviceInfo info = sensor.sendDeviceStats();
          
          mqtt.sendMessage(info.topic.c_str(), info.buffer, info.qos);
          zedSwitch.ledIndicator();
          
      }
    }
      
 
    
}

