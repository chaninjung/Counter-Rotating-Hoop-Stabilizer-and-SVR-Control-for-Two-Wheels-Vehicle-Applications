//abs(절대값) 사용하기 위한 라이브러리
#include "Arduino.h"

//통신
#include "I2Cdev.h"

//MPU-9250
#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68

//밀리 단위 실행시간 측정
unsigned long time;

//SVR function
int output;

//MPU-9250
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

void setup() {
  
  //MPU-9250 설정
  Serial.begin(115200);
  Wire.begin();

  if(!myMPU9250.init()){
    Serial.println("MPU9250 does not respond");
  }
  else{
    Serial.println("MPU9250 is connected");
  }
  
  Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(2000);
  myMPU9250.autoOffsets(); //가속도 및 자이로스코프 값을 측정하고 값을 계산, 이때 xy-plane에 평평하게 배치되어야 함
  Serial.println("DONE!");
  //delay(5000); //측정 각도로 기울이도록 허락된 시간(S)
  //Serial.println("DONE!");

  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G); //가속도 측정 범위를 설정, 2-4-6-16으로 설정 가능, 범위가 클수록 분해능이 낮아짐(acc, angle)
  myMPU9250.enableAccDLPF(true); //자이로스코프에 디지털 로우 패스 필터 설정(acc, angle)
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);  //1-8단계 중 선택, 노이즈를 줄이도록 디지털 로우패스 필터 설정, 단계가 높을 수록 가속도 반응 시간이 단축됨, 그러나 단계가 높아야 소음이 낮아짐(acc, angle)
  myMPU9250.setSampleRateDivider(99); // 가속도계 작동 (acc, gyro)
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_250); // 초당 250도 set (gyro)
  myMPU9250.enableGyrDLPF(); //(gyro)
  myMPU9250.setGyrDLPF(MPU9250_DLPF_6); //(gyro)

  //1st 모터 핀 설정
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
 
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  //2nd 모터 핀 설정
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
 
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  //1st 모터 정지
  pinMode(13, OUTPUT);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  
  //2nd 모터 정지
  pinMode(13, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  
}


//1st wheel 동작코드
//모터 축을 눈으로 바라볼 때 (아날로그값 입력시 9번 : 시계방향 / 10번 : 반시계방향)
void Forward_new_1() //시계방향
{
  int output_new=0;
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, 30); //처음에 reaction wheel 30 값으로 PWM 제어
  analogWrite(10, 0);
}

void Reverse_new_1() //반시계방향
{
  int output_new=0;
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, 0);
  analogWrite(10, 255);//overshooting을 위한 5%할증된 pwm 값
  delay(1000); //1.0초 동안 pwm 값 주기
  analogWrite(10, 0);
}

/*void Forward_new_1() //시계방향
{
  int output_new=0;
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, abs(output));
  analogWrite(10, 0);
  delay(2000);
}*/

void Reverse_1() //반시계방향
{
  int output_new=0;
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, 0);
  analogWrite(10, 30);  //30 값으로 PWM 제어
  
}

void stop_1()
{
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, 0);
  analogWrite(10, 0);
}


//2nd wheel 동작코드
//모터 축을 눈으로 바라볼 때 (아날로그값 입력시 6번 : 시계방향 / 5번 : 반시계방향)
void Forward_new_2() //시계방향
{
  int output_new=0;
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  analogWrite(5, 0); 
  analogWrite(6, 30);  //output 뒤에 주는 힘, 100 값으로 PWM 제어
}

void Reverse_new_2() //반시계방향
{
  int output_new=0;
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  analogWrite(5, abs(output));  //SVR function에 의해 output된 예측 pwm값 (overshooting -> correction)
  analogWrite(6, 0);
}

/*
void Forward_new_2() //시계방향
{
  int output_new=0;
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  analogWrite(5, abs(output));
  analogWrite(6, 0);
  delay(2000);
}*/

void Reverse_2() //시계방향
{
  int output_new=0;
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  analogWrite(5, 0);
  analogWrite(6, 30);  //30 값으로 PWM 제어
}

void stop_2()
{
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  analogWrite(5, 0);
  analogWrite(6, 0);
}




void loop() {
  //angle
  xyzFloat gValue = myMPU9250.getGValues(); //로우 가속도 값에 기반으로 하는 g값을 반환
  xyzFloat angle = myMPU9250.getAngles(); //x,y,z축 각각의 g값들의 arcsin 값
  //gyroscope
  xyzFloat gyrRaw = myMPU9250.getGyrRawValues();
  xyzFloat corrGyrRaw = myMPU9250.getCorrectedGyrRawValues();
  xyzFloat gyr = myMPU9250.getGyrValues();
  //acceleration
  xyzFloat accRaw = myMPU9250.getAccRawValues(); //raw data 반환
  xyzFloat accCorrRaw = myMPU9250.getCorrectedAccRawValues(); //raw data를 offset으로 수정한 값
  float resultantG = myMPU9250.getResultantG(gValue); //세가지 g 벡터의 합에 대한 크기로 가속도 계산

  //밀리 단위 실행시간 측정
  Serial.print("Time: ");
  time = millis();
  Serial.println(time); //prints time since program started

  //SVR fucntion (if, angle.y < 0)
  if (angle.y <= -8 && angle.y >= -30)
  output = -0.0003*sq(angle.y)*sq(angle.y)*sq(angle.y)- 0.0214*sq(angle.y)*sq(angle.y)*(angle.y) - 0.6175*sq(angle.y)*sq(angle.y) - 8.579*sq(angle.y)*(angle.y) - 57.894*sq(angle.y) - 169.89*(angle.y);

// 반작용 동작 코드
// 1. 각도값이 양수일 때
// 1st wheel이 overshooting하기 위한 10%할증된 pwm값을 받음
   if (angle.y > 0)
     {
              //기본적으로 각자 시계방향으로 30PWM으로 rotation
              Forward_new_1(); //1st wheel이 시계방향(pwm=30)으로 돈다
              Reverse_2(); //2nd wheel이 보조 바퀴로써 시계방향(pwm=30)으로 돈다
               
              //pwm
              Serial.print("                   |  PWM = ");
              Serial.println(30);
              
              //시리얼 모니터
              //angle_flot
              Serial.print("Angle x  = ");
              Serial.print(angle.x);
              Serial.print("  |  Angle y  = ");
              Serial.print(angle.y);
              Serial.print("  |  Angle z  = ");
              Serial.println(angle.z);
              /*Serial.print("Orientation of the module: ");
              Serial.println(myMPU9250.getOrientationAsString());  //+-방향 반환과 UP DOWN을 출력
              /*gyroscope_flot
              Serial.print("Angular x  = ");
              Serial.print(gyr.x);
              Serial.print(" | Angular y  = ");
              Serial.print(gyr.y);
              Serial.print(" | Angular z  = ");
              Serial.println(gyr.z);*/
              //acceleration
              Serial.print("accel x = ");
              Serial.print(gValue.x);
              Serial.print("   |  Accel y = ");
              Serial.print(gValue.y);
              Serial.print("   |  Accel z = ");
              Serial.println(gValue.z);
              }
            
   if (angle.y >= 29.95 && angle.y <= 30.05)
     {      
            //반시계방향(overshooting을 위한 5% 할증한 pwm값)
            Reverse_new_1();

            //pwm
            Serial.print("                   |  PWM = ");
            Serial.println(255);
            
            //시리얼 모니터
            //angle_flot
            Serial.print("Angle x  = ");
            Serial.print(angle.x);
            Serial.print("  |  Angle y  = ");
            Serial.print(angle.y);
            Serial.print("  |  Angle z  = ");
            Serial.println(angle.z);
            /*Serial.print("Orientation of the module: ");
            Serial.println(myMPU9250.getOrientationAsString());  //+-방향 반환과 UP DOWN을 출력
            /*gyroscope_flot
            Serial.print("Angular x  = ");
            Serial.print(gyr.x);
            Serial.print(" | Angular y  = ");
            Serial.print(gyr.y);
            Serial.print(" | Angular z  = ");
            Serial.println(gyr.z);*/
            //acceleration
            Serial.print("accel x = ");
            Serial.print(gValue.x);
            Serial.print("   |  Accel y = ");
            Serial.print(gValue.y);
            Serial.print("   |  Accel z = ");
            Serial.println(gValue.z);
            delay(100); //test가 끝나도, correction/undershooting/overshooting 확인하기 위한 data값

     }
     
// 2. 각도값이 음수일 때
// 2nd wheel이 overshooting -> correction시킬 pwm값을 SVR function에서 산출해서 받음
   if (angle.y <= 0 && angle.y > -8)
     {
            //기본적으로 각자 시계방향으로 30PWM으로 rotation
            Forward_new_1(); //1st wheel이 시계방향(pwm=30)으로 돈다
            Reverse_2(); //2nd wheel이 보조 바퀴로써 시계방향(pwm=30)으로 돈다
            
            //pwm
            Serial.print("                   |  PWM = ");
            Serial.println(30);
            
            //시리얼 모니터
            //angle_flot
            Serial.print("Angle x  = ");
            Serial.print(angle.x);
            Serial.print("  |  Angle y  = ");
            Serial.print(angle.y);
            Serial.print("  |  Angle z  = ");
            Serial.println(angle.z);
            /*Serial.print("Orientation of the module: ");
            Serial.println(myMPU9250.getOrientationAsString());  //+-방향 반환과 UP DOWN을 출력
            /*gyroscope_flot
            Serial.print("Angular x  = ");
            Serial.print(gyr.x);
            Serial.print(" | Angular y  = ");
            Serial.print(gyr.y);
            Serial.print(" | Angular z  = ");
            Serial.println(gyr.z);*/
            //acceleration
            Serial.print("accel x = ");
            Serial.print(gValue.x);
            Serial.print("   |  Accel y = ");
            Serial.print(gValue.y);
            Serial.print("   |  Accel z = ");
            Serial.println(gValue.z);       
     }

     if (angle.y <= -8 && angle.y >= -30)
     {     
            //function의 output pwm
            Serial.print("                   |  PWM = ");
            Serial.println(abs(output));
            
            //반시계방향(overshooting->correction시키기 위해 SVR function에서 받은 pwm값)
            Reverse_new_2();

            //시리얼 모니터
            //angle_flot
            Serial.print("Angle x  = ");
            Serial.print(angle.x);
            Serial.print("  |  Angle y  = ");
            Serial.print(angle.y);
            Serial.print("  |  Angle z  = ");
            Serial.println(angle.z);
            /*Serial.print("Orientation of the module: ");
            Serial.println(myMPU9250.getOrientationAsString());  //+-방향 반환과 UP DOWN을 출력
            /*gyroscope_flot
            Serial.print("Angular x  = ");
            Serial.print(gyr.x);
            Serial.print(" | Angular y  = ");
            Serial.print(gyr.y);
            Serial.print(" | Angular z  = ");
            Serial.println(gyr.z);*/
            //acceleration
            Serial.print("accel x = ");
            Serial.print(gValue.x);
            Serial.print("   |  Accel y = ");
            Serial.print(gValue.y);
            Serial.print("   |  Accel z = ");
            Serial.println(gValue.z);
            delay(100); //test가 끝나도, correction/undershooting/overshooting 확인하기 위한 data값
     }
  
  Serial.println();
  delay(100);
} 
