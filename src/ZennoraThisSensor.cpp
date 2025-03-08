#include "ZennoraThisSensor.h"


InitializationStatus coreStatus;
int rtcRestartCount = 0;

ZedSwitch zedSwitch;

//CommandMapping commandMappings[20];


ZennoraThisSensor::ZennoraThisSensor(){
    commandMappings[0] = {"toggle-0", []() { toggleOne(0); }};
    commandMappings[1] = {"toggle-1", []() { toggleOne(1); }};
    commandMappings[2] = {"toggle-2", []() { toggleOne(2); }};
    commandMappings[3] = {"toggle-3", []() { toggleOne(3); }};


    commandMappings[4] = {"toggle-all", []() { toggleAll(); }};


    commandMappings[5] = {"on-0", []() { onOff(0,HIGH); }};
    commandMappings[6] = {"on-1", []() { onOff(1,HIGH); }};
    commandMappings[7] = {"on-2", []() { onOff(2,HIGH); }};
    commandMappings[8] = {"on-3", []() { onOff(3,HIGH); }};

    commandMappings[9] = {"off-0", []() { onOff(0,LOW); }};
    commandMappings[10] = {"off-1", []() { onOff(1,LOW); }};
    commandMappings[11] = {"off-2", []() { onOff(2,LOW); }};
    commandMappings[12] = {"off-3", []() { onOff(3,LOW); }};

    commandMappings[13] = {"on-all", []() { onOffAll(HIGH); }};
    commandMappings[14] = {"off-all", []() { onOffAll(LOW); }};

   
    
  

}


InitializationThisSensorStatus ZennoraThisSensor::begin() {
    
    //the wire init is handled in the ZedSwitch class but moving the inits here to make the core functuonalities resuable

    zedSwitch.switchesStatus.switches = switchesInit();
    delay(5000);
    zedSwitch.switchesStatus.inasInit = inaInit();

    //set the uart pins to use
    uartPins();
    //initialize the uart pins
    sensor.coreSensorStatus.uartStatus = uartInit();
   
    // Go through the switches and MOSFETs/relays
    for (int i = 0; i < zedSwitch.switchesStatus.switches.switches.size(); i++) {
        
        //delay(100);]
        Serial.print("Switch: ");
        Serial.println(zedSwitch.switchesStatus.switches.switches[i].name.c_str());
        Serial.print("Pin: ");
        Serial.println(zedSwitch.switchesStatus.switches.switches[i].pin);
        zedSwitch.switchesStatus.mosfets.push_back(zedSwitch.switchesStatus.switches.switches[i].pin);
        pinMode(zedSwitch.switchesStatus.mosfets[i], OUTPUT);

        //delay(5000);
        
    }



    Serial.println("===============================================================");
    Serial.println("Initialization complete");
    Serial.println("===============================================================");


    InitializationThisSensorStatus status;
    return status;

}

bool ZennoraThisSensor::inaInit(){
    // Initialize I2C with custom SDA/SCL pins
    Wire.begin();
    Serial.println("Wire initialized");

    // Set the INA260 I2C address (default 0x40, change if needed)
    zedSwitch.switchesStatus.ina260sensors.push_back(INA260(0x40));

    Serial.println("INA260 setup");
    
    if(!zedSwitch.switchesStatus.ina260sensors[0].begin()){
           Serial.println("Failed to initialize INA219");


        }else{
            Serial.println("INA260 initialized");
        }
    

  
    delay(100);
    return true;
}

Switches ZennoraThisSensor::switchesInit()
{

    /*
    
    Default 0x40 → No soldering needed.
    0x41 → Short A0 pad to GND.
    0x44 → Short A1 pad to GND.
    0x45 → Short both A0 and A1 pads to GND.
    
    
    */

    Switches switches;
    //note that the addresses for a corresponding ina are nit being used for now
    //get the chipset of the esp32
    std::string chip = ESP.getChipModel(); 
    Serial.print("Chip: ");
    Serial.println(chip.c_str());

    //create an array of pin numbers for the mosfets based on the chip
    uint8_t mosfetPins[4];
    if(chip == "ESP32-S3"){
        mosfetPins[0] = 1;
        mosfetPins[1] = 2;
        mosfetPins[2] = 3;
        mosfetPins[3] = 4;
    }else if(chip == "ESP32-C3"){
        mosfetPins[0] = 2;
        mosfetPins[1] = 3;
        mosfetPins[2] = 4;
        mosfetPins[3] = 5;
    }else if(chip == "ESP32-C6"){
        mosfetPins[0] = 0;
        mosfetPins[1] = 1;
        mosfetPins[2] = 2;
        mosfetPins[3] = 21;
    }else{
        mosfetPins[0] = 1;
        mosfetPins[1] = 2;
        mosfetPins[2] = 3;
        mosfetPins[3] = 4;
    }


    /*
    
            uint32_t id; //unique identifier for the switch
            std::string type; //type of switch
            std::string name; //name of the switch
            bool state = LOW; //normallu state of the switch (LOW = off, HIGH = on) unless toggleStateMeaning is true, then filled
            bool toggleStateMeaning = false; //toggle state meaning
            bool status = false; //status of the switch
            uint8_t pin; //pin number of the switch
            uint8_t correspondingI2CPin; //address of the switch (hex)
            uint8_t address; //address of the switch (hex)
            float voltage; //voltage of the switch 
            float current; //current of the switch
            std::string statusMessage = "STARTUP"; //status message of the switch
            int pwm_channel =0; // PWM channel for use with fading
            int pwm_freq = 4095;  // 1kHz PWM frequency
            int pwm_resolution = 12;  // 8-bit resolution (0-255)
    
    */
   bool toggle = true;
   bool initState = false;

    Switch aSwitch {0, "light", "main salon", false, toggle, initState, mosfetPins[0], 4 ,0x40, 0.0f, 0.0f, "STARTUP", 0, 1000, 12}; //unshorted
    Switch aSwitch2 {1, "light", "port books shelf", false, toggle, initState, mosfetPins[1], 3, 0x44, 0.0f, 0.0f, "STARTUP", 0, 1000, 12}; //a1 short
    Switch aSwitch3 {2, "light", "port books reading light", false, toggle, initState, mosfetPins[2], 2, 0x41, 0.0f, 0.0f, "STARTUP", 0, 1000, 12};  //a0 shorted
    Switch aSwitch4 {3, "light", "starboard books shelf", false, toggle, initState,mosfetPins[3], 1, 0x45, 0.0f, 0.0f, "STARTUP", 0, 1000, 12}; //both shorted
    switches.addSwitch(aSwitch);
    switches.addSwitch(aSwitch2);
    switches.addSwitch(aSwitch3);
    switches.addSwitch(aSwitch4);
    return switches;
   



}

void ZennoraThisSensor::uartPins(){
    zedSwitch.switchesStatus.txPin = 43; //16 for Xiao ESP32 C3, 43 for Xiao ESP32 S3 and 21 for Xiao ESP32 C6
    zedSwitch.switchesStatus.rxPin = 44; //17 for Xiao ESP32 C3, 44 for Xiao ESP32 S3 and 20 for Xiao ESP32 C6
}


bool ZennoraThisSensor::uartInit(){
    Serial1.begin(9600, SERIAL_8N1, zedSwitch.switchesStatus.txPin, zedSwitch.switchesStatus.rxPin);
    delay(100); // Allow some time for initialization

    if (Serial1) { 
        Serial.println("UART initialized");
        Serial.println("===============================================================");
        Serial1.println("UART initialized");
    } else {
        Serial.println("Failed to initialize UART");
         Serial.println("===============================================================");
        
        return false;
    }
    return true;
}


// The device-specific handler that will be called from the MQTT library.
void ZennoraThisSensor::handleDeviceCommand(char* topic, byte* payload, unsigned int length) {
    // Copy payload into a local string to prevent buffer overflow
    char commandBuffer[length + 1];
    memcpy(commandBuffer, payload, length);
    commandBuffer[length] = '\0';  // Ensure null termination
    String command = String(commandBuffer);
    
    bool found = false;

    // Iterate over the mapping to find a match for the command.
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (command.equals(commandMappings[i].command)) {
            commandMappings[i].function();  // Execute the associated function.
            found = true;
            break;
        }
    }

    // If no matching command is found, print an error.
    if (!found) {
        Serial.println("Device-specific command not recognized: " + command);
    }
}

void ZennoraThisSensor::toggleAll() {
    Serial.println("Toggle all executed.");
    bool isToggled = zedSwitch.toggleAll();
    std::string message = "toggle-all-ack";
    std::string topic = std::string("commands/") + config.deviceName;
    if(!isToggled){
        message = "No switches toggled";
    }
    
    sendAck(topic, message);

    
}


void ZennoraThisSensor::toggleOne(int switchNumber) {
    Serial.println("toggle executed.");
    zedSwitch.toggle(switchNumber);
    
    // Add your custom functionality here.
}

void ZennoraThisSensor::onOff(int switchNumber, int state) {
    Serial.println("onOff executed.");
    zedSwitch.onOff(switchNumber, state);

    std::string message = "on-" + std::to_string(switchNumber) + "-ack";
    std::string topic = std::string("commands/") + config.deviceName;


    if(!state){
        message = "off-" + std::to_string(switchNumber) + "-ack";
    }
    
    sendAck(topic, message);

    // Add your custom functionality here.
}

//turn all switches on
void ZennoraThisSensor::onOffAll(bool state) {

    Serial.printf("onOffAll executed. State: %d", state);
    std::string topic = std::string("commands/") + config.deviceName;
    std::string message = "on-all-ack";
        
    for (int i = 0; i < zedSwitch.switchesStatus.switches.switches.size(); i++) {
        //zedSwitch.onOff(i, state);
        message = "on-" + std::to_string(i);
        if(!state){
            message = "off-" + std::to_string(i);
        }
        sendAck(topic, message);
        delay(250);


        


    }

    message = "on-all-ack";
    if(!onOff){
        message = "off-off-ack";
    }
    
    sendAck(topic, message);


  
}


//function to send a simple mqtt message "ack" to the broker
void ZennoraThisSensor::sendAck(std::string topic, std::string message) { 


    mqtt.sendMessage(topic, message, 1);


}

