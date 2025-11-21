#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// TB6612FNG Motor Driver Pin Definitions
// Two separate TB6612FNG drivers share the same control pins
// Each driver controls 2 motors in series (connected to AO1, AO2 outputs)

// Shared Control Pins (both TB6612FNG drivers use these same pins)
// TB6612FNG Pin Functions:
// - STBY: Standby control (HIGH=active, LOW=standby)
// - PWMA: PWM speed control (0-255 for 8-bit, controls duty cycle)
// - AIN1: Direction control input 1
// - AIN2: Direction control input 2
// - AO1/AO2: Motor output terminals (motors connected here)

#define MOTOR_STBY 27   // Standby/Enable pin (shared by both TB6612FNG drivers)
#define MOTOR_PWMA 25   // PWM speed control pin (PWMA) - shared by both drivers
#define MOTOR_AIN1 26   // Direction control pin 1 (AIN1) - GPIO 26, shared by both drivers
#define MOTOR_AIN2 13   // Direction control pin 2 (AIN2) - shared by both drivers

// Motor Connections:
// Right TB6612FNG: 2 motors in series → connected to AO1 and AO2 outputs
// Left TB6612FNG:  2 motors in series → connected to AO1 and AO2 outputs

// TB6612FNG PWM Configuration
#define MAX_MOTOR_SPEED 255  // Maximum PWM value for 8-bit resolution (0-255)
#define MIN_MOTOR_SPEED 50   // Minimum speed to overcome motor friction (adjust as needed)
const int PWMFreq = 20000;  // 20 KHz - recommended for TB6612FNG (can handle up to 100 KHz)
const int PWMResolution = 8; // 8-bit resolution (0-255)
const int motorPWMSpeedChannel = 4; // PWM channel for speed control

// Acceleration/Deceleration Control
#define ACCELERATION_RATE 5   // Speed change per loop (higher = faster acceleration)
#define DECELERATION_RATE 8   // Speed change per loop when stopping (higher = faster stop)
int currentRightSpeed = 0;    // Current actual motor speed (with acceleration)
int currentLeftSpeed = 0;     // Current actual motor speed (with acceleration)

// Apply acceleration/deceleration smoothing to motor speeds
void applyAcceleration(int targetRightSpeed, int targetLeftSpeed)
{
  // Apply acceleration/deceleration to right motor
  if (targetRightSpeed > currentRightSpeed)
  {
    currentRightSpeed += ACCELERATION_RATE;
    if (currentRightSpeed > targetRightSpeed) currentRightSpeed = targetRightSpeed;
  }
  else if (targetRightSpeed < currentRightSpeed)
  {
    currentRightSpeed -= DECELERATION_RATE;
    if (currentRightSpeed < targetRightSpeed) currentRightSpeed = targetRightSpeed;
  }
  
  // Apply acceleration/deceleration to left motor
  if (targetLeftSpeed > currentLeftSpeed)
  {
    currentLeftSpeed += ACCELERATION_RATE;
    if (currentLeftSpeed > targetLeftSpeed) currentLeftSpeed = targetLeftSpeed;
  }
  else if (targetLeftSpeed < currentLeftSpeed)
  {
    currentLeftSpeed -= DECELERATION_RATE;
    if (currentLeftSpeed < targetLeftSpeed) currentLeftSpeed = targetLeftSpeed;
  }
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  // Apply acceleration/deceleration smoothing
  applyAcceleration(rightMotorSpeed, leftMotorSpeed);
  
  // Both TB6612FNG drivers share the same control pins (AIN1, AIN2, PWMA, STBY)
  // Since they share pins, they move together - use average speed for direction control
  int combinedSpeed = (currentRightSpeed + currentLeftSpeed) / 2;
  
  // TB6612FNG Direction Control Truth Table:
  // AIN1=LOW,  AIN2=LOW  → Stop (short brake)
  // AIN1=LOW,  AIN2=HIGH → CCW (Counter-Clockwise / Reverse)
  // AIN1=HIGH, AIN2=LOW  → CW  (Clockwise / Forward)
  // AIN1=HIGH, AIN2=HIGH → Stop (short brake)
  
  if (combinedSpeed < 0)
  {
    // Reverse direction: AIN1=LOW, AIN2=HIGH
    digitalWrite(MOTOR_AIN1, LOW);
    digitalWrite(MOTOR_AIN2, HIGH);
  }
  else if (combinedSpeed > 0)
  {
    // Forward direction: AIN1=HIGH, AIN2=LOW
    digitalWrite(MOTOR_AIN1, HIGH);
    digitalWrite(MOTOR_AIN2, LOW);
  }
  else
  {
    // Stop (short brake): AIN1=LOW, AIN2=LOW
    digitalWrite(MOTOR_AIN1, LOW);
    digitalWrite(MOTOR_AIN2, LOW);
  }
  
  // Set PWM speed (0-255) - shared PWMA pin controls both TB6612FNG drivers
  // PWMA controls the speed/duty cycle of the motor output
  // Apply minimum speed threshold to overcome motor friction
  int pwmValue = abs(combinedSpeed);
  if (pwmValue > 0 && pwmValue < MIN_MOTOR_SPEED) {
    pwmValue = MIN_MOTOR_SPEED;  // Minimum speed to start motor
  }
  ledcWrite(motorPWMSpeedChannel, pwmValue);
}

void setUpPinModes()
{
  // TB6612FNG Pin Configuration
  // Configure all control pins as outputs
  pinMode(MOTOR_AIN1, OUTPUT);   // Direction control pin 1
  pinMode(MOTOR_AIN2, OUTPUT);   // Direction control pin 2
  pinMode(MOTOR_PWMA, OUTPUT);   // PWM speed control pin
  pinMode(MOTOR_STBY, OUTPUT);   // Standby/Enable pin

  // TB6612FNG STBY Pin Control:
  // STBY = LOW  → Standby mode (all outputs disabled, low power consumption)
  // STBY = HIGH → Active mode (normal operation)
  digitalWrite(MOTOR_STBY, HIGH);  // Enable both TB6612FNG drivers

  // Initialize direction pins to stop position (short brake)
  digitalWrite(MOTOR_AIN1, LOW);
  digitalWrite(MOTOR_AIN2, LOW);

  // Set up ESP32 PWM for speed control
  // TB6612FNG can handle PWM frequencies up to 100 KHz
  // Using 20 KHz for smooth motor operation and reduced audible noise
  ledcSetup(motorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcAttachPin(MOTOR_PWMA, motorPWMSpeedChannel);  // Attach PWMA pin to PWM channel
  
  // Initialize motors to stopped state
  rotateMotor(0, 0); 
}

void setup()
{
  setUpPinModes();
  Dabble.begin("MyBluetoothCar"); 
}

void loop()
{
  int rightMotorSpeed = 0;
  int leftMotorSpeed = 0;
  
  Dabble.processInput();
  
  // Check for joystick/analog control
  // Get joystick X and Y values (returns float, typically -100 to +100)
  float joystickX = GamePad.getXaxisData();  // Left/Right control
  float joystickY = GamePad.getYaxisData();  // Forward/Backward control
  
  // Check if joystick is being used (non-zero values)
  if (abs(joystickX) > 0.1 || abs(joystickY) > 0.1)
  {
    // Convert joystick values to motor speeds (-255 to +255)
    // Y-axis controls forward/backward (throttle)
    // X-axis controls left/right (steering)
    
    int throttle = map((int)joystickY, -100, 100, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
    int steering = map((int)joystickX, -100, 100, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
    
    // Differential drive: throttle + steering for turning
    rightMotorSpeed = throttle - steering;  // Right motor: throttle minus steering
    leftMotorSpeed = throttle + steering;   // Left motor: throttle plus steering
    
    // Limit speeds to valid range
    rightMotorSpeed = constrain(rightMotorSpeed, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
    leftMotorSpeed = constrain(leftMotorSpeed, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
  }
  // Fallback to button control if joystick not being used
  else
  {
    // Variable speed control using buttons with speed increments
    static int speedLevel = 0;  // Current speed level (0-5 for 0%, 20%, 40%, 60%, 80%, 100%)
    int speedIncrement = MAX_MOTOR_SPEED / 5;  // 51 steps per level
    
    // Speed adjustment buttons (if available)
    if (GamePad.isTrianglePressed())  // Increase speed
    {
      speedLevel++;
      if (speedLevel > 5) speedLevel = 5;
    }
    if (GamePad.isCrossPressed())  // Decrease speed
    {
      speedLevel--;
      if (speedLevel < 0) speedLevel = 0;
    }
    
    int currentMaxSpeed = speedLevel * speedIncrement;
    
    // Direction control with variable speed
    if (GamePad.isUpPressed())
    {
      rightMotorSpeed = currentMaxSpeed;
      leftMotorSpeed = currentMaxSpeed;
    }
    else if (GamePad.isDownPressed())
    {
      rightMotorSpeed = -currentMaxSpeed;
      leftMotorSpeed = -currentMaxSpeed;
    }
    else if (GamePad.isLeftPressed())
    {
      rightMotorSpeed = currentMaxSpeed;
      leftMotorSpeed = -currentMaxSpeed;
    }
    else if (GamePad.isRightPressed())
    {
      rightMotorSpeed = -currentMaxSpeed;
      leftMotorSpeed = currentMaxSpeed;
    }
    else
    {
      // No button pressed - stop motors
      rightMotorSpeed = 0;
      leftMotorSpeed = 0;
    }
  }

  // Apply motor control with acceleration
  rotateMotor(rightMotorSpeed, leftMotorSpeed);
  
  // Small delay for smooth operation
  delay(10);
}