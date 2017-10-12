#include "sensorslab.h"

volatile signed int _encoder_pos = 0;
bool motor_on = false;

double err_p, err_i, err_d, err_last = 0;
unsigned long lastTick;
int desired;
/*unsigned int current;*/
bool drive_vel, drive_position;
unsigned long last_time;

const double k_p = .26;
const double k_i = 0.1;
const double k_d = 0.6;

long current_time = 0;
long current_encoder_pos;
long last_time = 0;
long last_encoder_pos;



void dcMotorSetup()
{
  pinMode(DC_MOTOR_EN, OUTPUT);

  pinMode(DC_MOTOR_1, OUTPUT);
  pinMode(DC_MOTOR_2, OUTPUT);
  
  pinMode(DC_MOTOR_ENC_A, INPUT);
  pinMode(DC_MOTOR_ENC_B, INPUT);
  
  attachInterrupt(DC_MOTOR_INTERRUPT, encoderInterruptCallBack, CHANGE);
}

void encoderInterruptCallBack()
{
    /*Serial.println("motor moved");*/
  if(digitalRead(DC_MOTOR_ENC_A) == digitalRead(DC_MOTOR_ENC_B))
  {
    _encoder_pos++;
  }
  else
  {
    _encoder_pos--;
  }
}

void dcMotorControl(int desired){
/*Serial.println("pid control loop");*/
unsigned long time = millis();
unsigned long timestep = time-last_time;
err_p = desired - _encoder_pos;
/*Serial.println(err_p);*/
if(abs(err_p)>5){
err_i += err_p;
err_d += (err_p-err_last)/timestep;

double output = err_p*k_p-err_i*k_i+err_d*k_d;

Serial.print("pos: ");
Serial.print(_encoder_pos);
Serial.print(" err_p: ");
Serial.print(err_p*k_p);
Serial.print(" err_i: ");
Serial.print(err_i*k_i);
Serial.print(" err_d: ");
Serial.print(err_d*k_d);
Serial.print(" pid output: ");
rot_dir direction = err_p<abs(err_p)?COUNTERCLOCKWISE:CLOCKWISE;
output=abs(output);
output+=110;
output = min(output, 210);
/*output = output %1000;*/
/*output = map*/
Serial.print((int)output);
/*Serial.print(" pid target: ");*/
/*Serial.println(desired);*/

Serial.print(" direction: ");
Serial.println(direction);
dcMotorDrive((int)output, direction);
err_last = err_p;
last_time = time;
}
else if (motor_on)
{
Serial.print("pos: ");
Serial.println(_encoder_pos);
dcMotorStop();
}


}

void dcMotorDrive(int dc_speed, rot_dir direction)
 { 
  motor_on = true;
  /*clockwise maybe */
  /*Serial.println("Writing to motor");*/
  analogWrite(DC_MOTOR_EN, dc_speed);

  if(direction == CLOCKWISE){
  digitalWrite(DC_MOTOR_1, LOW);
  digitalWrite(DC_MOTOR_2, HIGH);
  }
  else{
  digitalWrite(DC_MOTOR_1, HIGH);
  digitalWrite(DC_MOTOR_2, LOW);
  }

}

void dcMotorStop()
{
    motor_on = false;
    Serial.println("stopping dc motor");
    analogWrite(DC_MOTOR_EN, LOW);
    /*_encoder_pos = 0;*/
    /*_encoder_pos = 0;*/
    err_p = 0;
    err_i = 0;
    err_d = 0;
}

unsigned int getEncoderPos()
{
  return _encoder_pos;
}

void printEncoderPosition()
{
  Serial.print("Encoder Position: ");
  Serial.println(_encoder_pos, DEC);
}

void dcMotorTestFunc()
{
  Serial.println("dc motor test");
  if(_encoder_pos < 1000)
  {
    analogWrite(DC_MOTOR_EN, DC_SPEED_HIGH);
    digitalWrite(DC_MOTOR_1, LOW);
    digitalWrite(DC_MOTOR_2, HIGH);
  }
  else{
    analogWrite(DC_MOTOR_EN, LOW);
    }
  
  printEncoderPosition();
}

void dcSpeedControl(int desired_vel)
{
  current_time = millis();
  current_encoder_pos = _encoder_pos;
  
  timestep = current_time - last_time;
  distance = current_encoder_pos - last_encoder_pos;
  
  double current_vel = (distance * 1000) / timestep;  // degrees/second
  
  last_time = millis();
  last_encoder_pos = _encoder_pos;
  
  int voltage = 140;
  
  if(current_vel - desired_vel > 3)
    voltage--;
  else if(current_vel - desired_vel < 3)
    voltage++;
  
  rot_dir direction_ = (desired_vel<abs(desired_vel))? CLOCKWISE : COUNTERCLOCKWISE;
  dcMotorDrive((int)voltage, direction_);
    
}
