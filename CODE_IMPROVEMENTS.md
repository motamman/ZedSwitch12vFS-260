# Code Improvement Plan for ZedSwitch12vFS-260

## 1. Error Handling and Safety Mechanisms (HIGH PRIORITY)

### Critical Issues
- No bounds checking for switch indices (could access invalid switches)
- INA260 sensor failures are logged but ignored
- MQTT connection failures aren't handled with reconnection logic
- No validation of GPIO operations before execution

### Proposed Solutions
1. **Add switch bounds validation** (immediate)
   - Validate switchNumber parameter in `toggleOne()` and `onOff()` functions
   - Return error codes instead of void functions
   
2. **Implement MQTT connection monitoring**
   - Add exponential backoff retry logic
   - Queue messages during disconnections
   - Monitor connection state in main loop

3. **Add sensor health checks**
   - Periodic INA260 communication validation
   - Implement fallback modes when sensors fail
   - Validate sensor readings for reasonable ranges

4. **Emergency shutdown procedure**
   - Create safe shutdown command for all circuits
   - Add overcurrent detection with automatic disconnection

### Files Affected
- `src/ZennoraThisSensor.cpp` (toggleOne, onOff functions)
- `src/main.cpp` (MQTT loop handling)

## 2. Code Structure Refactoring (MEDIUM PRIORITY)

### Issues
- Global variables scattered across files
- Monolithic main.cpp with mixed responsibilities
- Hardcoded configuration values
- No dependency injection

### Proposed Improvements
1. **Split main.cpp into focused modules**
   - Create separate files for MQTT handling, sensor management
   - Extract initialization logic into dedicated classes

2. **Remove global variables**
   - Use dependency injection patterns
   - Create proper class hierarchies

3. **Extract hardware configuration**
   - Create config files for different ESP32 variants
   - Move pin assignments to runtime configuration

4. **Create hardware abstractions**
   - Abstract GPIO operations
   - Create interfaces for sensors and communication

### Files to Refactor
- `src/main.cpp` (split into modules)
- `src/ZennoraThisSensor.cpp` (remove globals)
- Create new `config/` directory for hardware profiles

## 3. Marine-Specific Safety Features (HIGH PRIORITY)

### Missing Safety Features
- Voltage monitoring with configurable thresholds
- Load shedding for low battery conditions
- Overcurrent protection per circuit
- Temperature monitoring for thermal protection

### Implementation Plan
1. **Voltage monitoring system**
   - Alert thresholds: 11.5V, 11.0V, 10.5V
   - Automatic load shedding priority system

2. **Overcurrent protection**
   - Per-circuit monitoring using INA260 data
   - Automatic circuit disconnection on overcurrent

3. **Thermal protection**
   - Temperature sensor integration
   - Thermal shutdown protection

4. **Emergency stop functionality**
   - Command to safely power down all circuits
   - Hardware emergency stop integration

### Hardware Considerations
- Expand INA260 monitoring to per-circuit (future hardware revision)
- Add temperature sensors for thermal monitoring
- Consider voltage dividers for battery monitoring

## 4. Memory Management and Performance (MEDIUM PRIORITY)

### Issues
- Dynamic JSON allocation with `DynamicJsonDocument`
- No heap monitoring or fragmentation checks
- Missing cleanup for failed initializations
- No performance metrics

### Solutions
1. **Static memory allocation**
   - Replace `DynamicJsonDocument` with `StaticJsonDocument`
   - Use appropriate buffer sizing

2. **Memory monitoring**
   - Add `heap_caps_get_free_size()` monitoring
   - Track memory fragmentation

3. **Performance metrics**
   - Add timing measurements for switch operations
   - Create memory usage reporting via MQTT

4. **Proper cleanup patterns**
   - Implement cleanup in initialization failure paths
   - Use RAII patterns where applicable

### Files Affected
- `src/ZennoraThisSensor.cpp` (JSON processing)
- `src/main.cpp` (add monitoring)

## 5. Configuration Management System (MEDIUM PRIORITY)

### Issues
- Pin assignments hardcoded for specific ESP32 variants
- Switch names and types embedded in code
- No runtime configuration validation
- Missing calibration storage for sensors
- No way to change settings without recompiling

### Proposed Implementation
1. **Configuration Files**
   - Create JSON/YAML config files for hardware profiles
   - Separate configs for ESP32-C3, ESP32-C6, ESP32-S3 variants
   - Runtime loading and validation of configuration

2. **Hardware Profiles**
   - Pin mapping configurations per board type
   - Switch naming and type definitions
   - UART pin assignments per variant
   - I2C address mappings

3. **Runtime Configuration**
   - Web interface for configuration changes
   - MQTT commands for remote configuration
   - Persistent storage using Preferences library
   - Configuration backup/restore functionality

### Files to Create
- `config/esp32-c3.json`, `config/esp32-c6.json`, `config/esp32-s3.json`
- `src/ConfigManager.h/cpp`
- `web/config.html`

## 6. Structured Logging and Telemetry System (MEDIUM PRIORITY)

### Issues
- No log levels (DEBUG, INFO, WARN, ERROR)
- Cannot filter or control log output
- No remote logging capabilities
- Missing performance metrics
- No system health telemetry

### Proposed Implementation
1. **Structured Logging Framework**
   - Log levels with runtime filtering
   - Formatted log messages with timestamps
   - Component-specific logging (MQTT, Sensors, Switches)
   - Memory-efficient circular log buffer

2. **Remote Logging**
   - MQTT-based log streaming
   - Log level control via MQTT commands
   - Emergency log dump functionality
   - Structured JSON log format

3. **System Telemetry**
   - Heap memory usage monitoring
   - WiFi signal strength tracking
   - Switch operation counters
   - MQTT connection quality metrics
   - Sensor health indicators

### Files to Create
- `src/Logger.h/cpp`
- `src/Telemetry.h/cpp`
- `src/PerformanceMonitor.h/cpp`

## 7. Comprehensive Testing Framework (MEDIUM PRIORITY)

### Missing Test Coverage
- No unit tests for core switching logic
- No integration tests for MQTT communication
- No hardware-in-the-loop testing
- No performance regression testing
- No safety feature validation

### Proposed Implementation
1. **Unit Testing Framework**
   - Use PlatformIO's built-in testing framework
   - Mock hardware interfaces for isolated testing
   - Test switch bounds validation
   - Test configuration loading and validation

2. **Integration Testing**
   - MQTT message processing tests
   - Sensor communication validation
   - End-to-end command processing

3. **Hardware-in-the-Loop (HIL) Testing**
   - Automated switch operation verification
   - Current measurement validation
   - GPIO state verification

### Files to Create
- `test/test_switch_operations/`
- `test/test_mqtt_handling/`
- `test/test_configuration/`
- `test/mocks/`

## 8. Enhanced MQTT Communication (MEDIUM PRIORITY)

### Issues
- Basic MQTT implementation without QoS management
- No message queuing during disconnections
- Limited command structure and validation
- No authentication or encryption
- Missing device discovery and status reporting

### Proposed Enhancements
1. **Robust MQTT Implementation**
   - Implement QoS levels for critical messages
   - Message queuing during network outages
   - Automatic reconnection with exponential backoff
   - Last Will and Testament (LWT) messages

2. **Advanced Command Processing**
   - Structured command validation
   - Command acknowledgment system
   - Batch command processing
   - Command scheduling and timing

3. **Security Features**
   - TLS/SSL encryption support
   - Client certificate authentication
   - Message payload encryption for sensitive commands

### Files to Create
- `src/MQTTManager.h/cpp`
- `src/CommandProcessor.h/cpp`
- `src/SecurityManager.h/cpp`

## 9. Power Management and Battery Monitoring (HIGH PRIORITY)

### Missing Features
- Battery state of charge monitoring
- Intelligent load management
- Sleep mode implementation
- Power consumption optimization
- Battery protection features

### Proposed Implementation
1. **Battery Monitoring System**
   - State of charge (SOC) calculation
   - Battery voltage trend analysis
   - Remaining runtime estimation
   - Battery health monitoring

2. **Intelligent Load Management**
   - Priority-based load shedding
   - Automatic power saving modes
   - Critical vs non-critical load identification
   - Scheduled operation management

3. **Power Optimization**
   - Deep sleep mode implementation
   - CPU frequency scaling
   - Peripheral power management
   - Ultra-low power standby modes

4. **Battery Protection**
   - Over-discharge protection
   - Reverse polarity protection
   - Overcurrent shutdown
   - Thermal protection

### Files to Create
- `src/PowerManager.h/cpp`
- `src/BatteryMonitor.h/cpp`
- `src/LoadManager.h/cpp`
- `src/SleepManager.h/cpp`

## 10. Hardware Abstraction Layer (LONG-TERM PRIORITY)

### Issues
- Direct GPIO manipulation throughout codebase
- Hardcoded I2C and UART implementations
- Platform-specific code mixed with business logic
- Difficult to unit test without hardware
- No support for hardware variants or alternatives

### Proposed Implementation
1. **GPIO Abstraction Layer**
   - Generic pin control interface
   - Platform-independent digital I/O
   - PWM abstraction for dimming
   - Interrupt handling abstraction

2. **Communication Abstractions**
   - Generic I2C interface for sensors
   - UART abstraction for external communication
   - Network communication abstraction

3. **Sensor Interface Layer**
   - Generic current/voltage sensor interface
   - Temperature sensor abstraction
   - Sensor discovery and enumeration

4. **Testing Support**
   - Hardware simulation layer
   - Mock implementations for all interfaces
   - Virtual sensor implementations

### Files to Create
- `src/hal/GPIO.h/cpp`
- `src/hal/I2C.h/cpp`
- `src/hal/UART.h/cpp`
- `src/hal/mocks/`

## Implementation Priority Order

1. **Immediate (Week 1)**
   - Add switch bounds validation
   - Implement basic error return codes
   - Add MQTT connection monitoring

2. **Short Term (Month 1)**
   - Marine safety features (voltage monitoring, emergency stop)
   - Memory management improvements
   - Sensor health checks

3. **Medium Term (Month 2-3)**
   - Code structure refactoring
   - Configuration management
   - Performance optimization

4. **Long Term (Month 3+)**
   - Advanced logging system
   - Unit testing framework
   - Hardware abstraction layer

---
*Generated by Claude Code analysis on 2025-01-06*