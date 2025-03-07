#pragma once
#include <Arduino.h>
#include "globals.h"
#include "ZennoraCore.h"
#include "ZennoraCoreComms.h"
#include "ZennoraCoreSensor.h"
#include <variant>
#include <vector>
#include <string>
#include "switch.h"
#include "functional"


extern RTC_DATA_ATTR int rtcRestartCount;


extern ZennoraCoreSensor sensor; 
extern ZennoraCore core;
extern ZennoraMQTT mqtt;
extern InitializationStatus coreStatus;
extern ZedSwitch zedSwitch;



struct CommandMapping {
    const char* command;
    std::function<void()> function;
};




// Struct for Initialization Status
struct InitializationThisSensorStatus {
    bool thisBeginRun = false;
    bool thisSomethingElse = false;
    
    // Helper to check overall success
    bool allSucceeded() const {
        return thisBeginRun &&
                thisSomethingElse;
    }

    struct thisBeginRun {
        bool thisSomethingElse = false;
        
    };
};






class ZennoraThisSensor {
public:

    // Define a type alias for command functions.
    typedef std::function<void()> CommandFunction;

    // Structure to map command strings to their handling functions.
    struct CommandMapping {
        const char* command;
        CommandFunction function;
    };
    ZennoraThisSensor();  // Default constructor
    InitializationThisSensorStatus begin(); // Entry point for initialization

        // This is the device-specific command handler that will be
    // registered with the MQTT library.
    // It receives the topic, payload, and length (payload should be null-terminated).
    void handleDeviceCommand(char* topic, byte* payload, unsigned int length);

    // Device-specific command implementations.
    static void toggleAll();
    static void toggleOne(int switchNumber);
    static void onOff(int switchNumber, int state);
    static void onOffAll(bool onOff);

    static void sendAck(std::string topic, std::string message);

    //setups the switches
    Switches switchesInit();
    bool inaInit();
    bool uartInit();





   

private:
  


      // Number of commands available.
    static const int NUM_COMMANDS = 20;

    // An array of command mappings.
    CommandMapping commandMappings[NUM_COMMANDS];
    void uartPins();

};