#define ENABLE_ERROR_LOGGING  // занимает пин LED_BUILTIN

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1
#define MOTOR_SPIN_FORWARD 1
#define MOTOR_SPIN_NONE 0
#define MOTOR_SPIN_BACKWARD -1

class MotorsHandler
{
  private:
    const uint8_t in1, in2, in3, in4;  //stores pins which used to control l298n driver
    uint8_t motorStates[2];
    bool MotorsHandler::setMotorSpinDirection(int pin1, int pin2, int direction)  // set motor spin direction by pins 
    {
      switch (direction)
      {
        case MOTOR_SPIN_BACKWARD:
          {
            digitalWrite(pin1, HIGH);
            digitalWrite(pin2, LOW);
            return true;
          }
        case MOTOR_SPIN_NONE:
          {
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, LOW);
            return true;
          }
        case MOTOR_SPIN_FORWARD:
          {
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, HIGH);
            return true;
          }
#ifdef ENABLE_ERROR_LOGGING
        default:
          {
            String message = String("[MotorsHandler::setMotorSpinDirection]Wrong motor direction " + String(direction) + " for pins " + String(pin1) + " and " + String(pin2) + "!");
            Serial.println(message);
            digitalWrite(LED_BUILTIN, HIGH);
            return false;
          }
#endif
          return false;
      }
    }
  public:
    // pins which are connected to their equal input pins IN1, IN2, IN3, IN4 on a driver
    MotorsHandler::MotorsHandler(int in1, int in2, int in3, int in4) : in1(in1), in2(in2), in3(in3), in4(in4)
    {
      pinMode(in1, OUTPUT);
      pinMode(in2, OUTPUT);
      pinMode(in3, OUTPUT);
      pinMode(in4, OUTPUT);
      this->stopMotor(LEFT_MOTOR);
      this->stopMotor(RIGHT_MOTOR);
    }

    int MotorsHandler::getMotorSpinState(int motorId)  // returns last known motor spin state
    {
#ifdef ENABLE_ERROR_LOGGING
      if (motorId < 0 || motorId > 1)
      {
        String message = String("[MotorsHandler::getMotorSpinState]Wrong motor id: " + String(motorId) + "!");
        Serial.println(message);
        digitalWrite(LED_BUILTIN, HIGH);
      }
#endif
      return motorStates[motorId];
    }

    void MotorsHandler::setMotorSpinState(int motorId, int state) // motorId: 0 or 1, direction: use MOTOR_SPIN_* constants
    {
      switch (motorId)
      {
        case LEFT_MOTOR:
          {
            if (this->setMotorSpinDirection(this->in1, this->in2, state))
              motorStates[motorId] = state;
            break;
          }
        case RIGHT_MOTOR:
          {
            if (this->setMotorSpinDirection(this->in3, this->in4, state))
              motorStates[motorId] = state;
            break;
          }
#ifdef ENABLE_ERROR_LOGGING
        default:
          {
            String message = String("[MotorsHandler::setMotorSpinState]Wrong motor id: " + String(motorId) + "!");
            Serial.println(message);
            digitalWrite(LED_BUILTIN, HIGH);
          }
#endif
      }
    }

    void MotorsHandler::spinBackward(int motorId) // spin motor backward by its id
    {
      this->switchMotorState(motorId, MOTOR_SPIN_FORWARD);
    }

    void MotorsHandler::spinForward(int motorId) // spin motor forward by its id
    {
      this->switchMotorState(motorId, MOTOR_SPIN_FORWARD);
    }

    void MotorsHandler::stopMotor(int motorId) // stop motor by its id
    {
      this->switchMotorState(motorId, MOTOR_SPIN_NONE);
    }
};

MotorsHandler* handler;  

void setup() {
#ifdef ENABLE_ERROR_LOGGING
  Serial.begin(9600);
  Serial.println("Error logging enabled.");
  pinMode(LED_BUILTIN, OUTPUT);
#endif
  handler = new MotorsHandler(4, 5, 6, 7);  // 4, 5, 6, 7 - пины управления
  // put your setup code here, to run once:
  handler->stopMotor(LEFT_MOTOR);
  handler->stopMotor(RIGHT_MOTOR);
}

void loop() {
  // put your main code here, to run repeatedly:
  handler->stopMotor(RIGHT_MOTOR);
  //handler->setMotorSpinState(LEFT_MOTOR, MOTOR_SPIN_FORWARD);
  delay(1000);
  //handler->setMotorSpinState(LEFT_MOTOR, MOTOR_SPIN_NONE);
  handler->spinForward(RIGHT_MOTOR);
  delay(1000);
}
