#define ENABLE_ERROR_LOGGING  // занимает пин LED_BUILTIN

#include "IRremote.h"

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
    bool setMotorSpinDirection(int pin1, int pin2, int direction)  // set motor spin direction by pins
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
    MotorsHandler(int in1, int in2, int in3, int in4) : in1(in1), in2(in2), in3(in3), in4(in4)
    {
      pinMode(in1, OUTPUT);
      pinMode(in2, OUTPUT);
      pinMode(in3, OUTPUT);
      pinMode(in4, OUTPUT);
      this->stopMotor(LEFT_MOTOR);
      this->stopMotor(RIGHT_MOTOR);
    }

    int getMotorSpinState(int motorId)  // returns last known motor spin state
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

    void setMotorSpinState(int motorId, int state) // motorId: 0 or 1, direction: use MOTOR_SPIN_* constants
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

    void spinBackward(int motorId) // spin motor backward by its id
    {
      this->setMotorSpinState(motorId, MOTOR_SPIN_FORWARD);
    }

    void spinForward(int motorId) // spin motor forward by its id
    {
      this->setMotorSpinState(motorId, MOTOR_SPIN_FORWARD);
    }

    void stopMotor(int motorId) // stop motor by its id
    {
      this->setMotorSpinState(motorId, MOTOR_SPIN_NONE);
    }
};

#define KEY_NONE 0x0
#define KEY_HELD 0xFFFFFFFF
#define KEY_CH_MINUS 0xFFA25D
#define KEY_CH 0xFF629D
#define KEY_CH_PLUS 0xFFE21D
#define KEY_PREV 0xFF22DD
#define KEY_NEXT 0xFF02FD
#define KEY_PLAY_PAUSE 0xFFC23D
#define KEY_VOL_MINUS 0xFFE01F
#define KEY_VOL_PLUS 0xFFA857
#define KEY_EQ 0xFF906F
#define KEY_0 0xFF6897
#define KEY_100_PLUS 0xFF9867
#define KEY_200_PLUS 0xFFB04F
#define KEY_1 0xFF30CF
#define KEY_2 0xFF18E7
#define KEY_3 0xFF7A85
#define KEY_4 0xFF10EF
#define KEY_5 0xFF38C7
#define KEY_6 0xFF5AA5
#define KEY_7 0xFF42BD
#define KEY_8 0xFF4AB5
#define KEY_9 0xFF52AD

class IRHandler
{
  private:
    IRrecv* irrecv;
    decode_results results;
    unsigned long lastKey;
    unsigned long lastReceiveTime;
    unsigned long keyMaxHoldTime = 100;  // in milliseconds
  public:
    IRHandler(int pin)
    {
      irrecv = new IRrecv(pin);
      irrecv->enableIRIn();
      lastReceiveTime = millis();
      lastKey = KEY_NONE;
    }

    unsigned long getLastKey()
    {
      return this->lastKey;
    }

    void update()
    {
      if ( irrecv->decode( &results )) { // если данные пришли
        unsigned long receivedKey = results.value;
        unsigned long currentTime = millis();

        switch (receivedKey)
        {
          case KEY_HELD:
            {
              lastReceiveTime = millis();
              break;
            }
          case KEY_CH_MINUS:
          case KEY_CH:
          case KEY_CH_PLUS:
          case KEY_PREV:
          case KEY_NEXT:
          case KEY_PLAY_PAUSE:
          case KEY_VOL_MINUS:
          case KEY_VOL_PLUS:
          case KEY_EQ:
          case KEY_0:
          case KEY_100_PLUS:
          case KEY_200_PLUS:
          case KEY_1:
          case KEY_2:
          case KEY_3:
          case KEY_4:
          case KEY_5:
          case KEY_6:
          case KEY_7:
          case KEY_8:
          case KEY_9:
            {
              lastKey = receivedKey;
              lastReceiveTime = millis();
              break;
            }
        }

        irrecv->resume(); // принимаем следующую команду
      }
      else if (millis() - lastReceiveTime > keyMaxHoldTime)
      {
        lastKey = KEY_NONE;
      }
      //Serial.println(lastKey, HEX);
    }
};

MotorsHandler* motorHandler;
IRHandler* irHandler;

void setup() {
  Serial.begin(9600);
#ifdef ENABLE_ERROR_LOGGING
  Serial.println("Error logging enabled.");
  pinMode(LED_BUILTIN, OUTPUT);
#endif
  irHandler = new IRHandler(2);
  motorHandler = new MotorsHandler(4, 5, 6, 7);  // 4, 5, 6, 7 - пины управления
  // put your setup code here, to run once:
  motorHandler->stopMotor(LEFT_MOTOR);
  motorHandler->stopMotor(RIGHT_MOTOR); 
}

void loop() {
  irHandler->update();
  unsigned long key = irHandler->getLastKey();

  switch(key)
  {
    case KEY_NONE:
    {
      motorHandler->stopMotor(LEFT_MOTOR);
      motorHandler->stopMotor(RIGHT_MOTOR);
      break;
    }
    case KEY_4:
    {
      motorHandler->stopMotor(RIGHT_MOTOR);
      motorHandler->setMotorSpinState(LEFT_MOTOR, MOTOR_SPIN_FORWARD);
      break;
    }
    case KEY_7:
    {
      motorHandler->stopMotor(RIGHT_MOTOR);
      motorHandler->setMotorSpinState(LEFT_MOTOR, MOTOR_SPIN_BACKWARD);
      break;
    }
    case KEY_5:
    {
      motorHandler->setMotorSpinState(LEFT_MOTOR, MOTOR_SPIN_FORWARD);
      motorHandler->setMotorSpinState(RIGHT_MOTOR, MOTOR_SPIN_FORWARD);
      break;
    }
    case KEY_8:
    {
      motorHandler->setMotorSpinState(LEFT_MOTOR, MOTOR_SPIN_BACKWARD);
      motorHandler->setMotorSpinState(RIGHT_MOTOR, MOTOR_SPIN_BACKWARD);
      break;
    }
    case KEY_6:
    {
      motorHandler->stopMotor(LEFT_MOTOR);
      motorHandler->setMotorSpinState(RIGHT_MOTOR, MOTOR_SPIN_FORWARD);
      break;
    }
    case KEY_9:
    {
      motorHandler->stopMotor(LEFT_MOTOR);
      motorHandler->setMotorSpinState(RIGHT_MOTOR, MOTOR_SPIN_BACKWARD);
      break;
    }

    case KEY_1:
    {
      motorHandler->setMotorSpinState(LEFT_MOTOR, MOTOR_SPIN_BACKWARD);
      motorHandler->setMotorSpinState(RIGHT_MOTOR, MOTOR_SPIN_FORWARD);
      break;
    }
    case KEY_3:
    {
      motorHandler->setMotorSpinState(LEFT_MOTOR, MOTOR_SPIN_FORWARD);
      motorHandler->setMotorSpinState(RIGHT_MOTOR, MOTOR_SPIN_BACKWARD);
      break;
    }
  }
  
  delay(50);
}
