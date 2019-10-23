#define ENABLE_ERROR_LOGGING

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1
#define MOTOR_SPIN_FORWARD 1
#define MOTOR_SPIN_NONE 0
#define MOTOR_SPIN_BACKWARD -1

class MotorsHandler
{
  private:
    const uint8_t in1, in2, in3, in4;
    void setMotorDirection(int pin1, int pin2, int direction)
    {
      switch (direction)
      {
        case MOTOR_SPIN_BACKWARD:
          {
            digitalWrite(pin1, HIGH);
            digitalWrite(pin2, LOW);
            break;
          }
        case MOTOR_SPIN_NONE:
          {
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, LOW);
            break;
          }
        case MOTOR_SPIN_FORWARD:
          {
            digitalWrite(pin1, LOW);
            digitalWrite(pin2, HIGH);
            break;
          }
#ifdef ENABLE_ERROR_LOGGING
        default:
          {
            String message = String("Wrong motor direction " + String(direction) + " for pins " + String(pin1) + " and " + String(pin2) + "!");
            Serial.println(message);
          }
#endif
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
      this->setMotorDirection(in1, in2, MOTOR_SPIN_NONE);
      this->setMotorDirection(in3, in4, MOTOR_SPIN_NONE);
    }

    void MotorsHandler::runMotor(int motorId, int direction) // motorId: 0 or 1, direction: use MOTOR_SPIN_* constants
    {
      switch (motorId)
      {
        case LEFT_MOTOR:
          {
            this->setMotorDirection(this->in1, this->in2, direction);
            break;
          }
        case RIGHT_MOTOR:
          {
            this->setMotorDirection(this->in3, this->in4, direction);
            break;
          }
#ifdef ENABLE_ERROR_LOGGING
        default:
          {
            String message = String("Wrong motor id: " + String(motorId) + "!");
            Serial.println(message);
          }
#endif
      }
    }

    void MotorsHandler::stopMotor(int motorId) // motorId: 0 or 1
    {
      this->runMotor(motorId, MOTOR_SPIN_NONE);
    }
};

MotorsHandler handler(4, 5, 6, 7);  // 4, 5, 6, 7 - пины управления

void setup() {
#ifdef ENABLE_ERROR_LOGGING
  Serial.begin(9600);
  Serial.println("Error logging enabled.");
#endif
  // put your setup code here, to run once:
  handler.stopMotor(LEFT_MOTOR);
  handler.stopMotor(RIGHT_MOTOR);
}

void loop() {
  // put your main code here, to run repeatedly:
  handler.runMotor(RIGHT_MOTOR, MOTOR_SPIN_NONE);
  //handler.runMotor(LEFT_MOTOR, MOTOR_SPIN_FORWARD);
  delay(1000);
  //handler.runMotor(LEFT_MOTOR, MOTOR_SPIN_NONE);
  handler.runMotor(RIGHT_MOTOR, MOTOR_SPIN_FORWARD);
  delay(1000);
}
