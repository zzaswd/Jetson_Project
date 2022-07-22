//===========================
// LCD declare
//===========================
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); // (주소,열,행)

int car_number = -1;
int type = -1;
char spot[10] = "";

int Clear_compare= 0; //Lcd Clear 적재적소에 할 수 있도록 기준 설정

volatile byte status = HIGH;

//=============================
//   Servo Motor Declare
//=============================
#include <Servo.h>

Servo blocking_Car;

//=============================
//   Ultra sonic Declare
//=============================

#define TRIG 10 //TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO 9 //ECHO 핀 설정 (초음파 받는 핀)


//=============================
//   Serial Declare
//=============================

#include <SoftwareSerial.h>
#define TX 7
#define RX 8
SoftwareSerial soft_Serial(TX,RX);

void Check_clear();               // Serial 통신으로 값이 넘어 오는 지 체크


void setup() {
  Serial.begin(9600);
//===========================
// LCD setup
//===========================
  lcd.init(); // 초기화 해주고
  lcd.backlight(); // 백라이트 켜주고
  
//=============================
//   Servo Motor Setup
//=============================
  blocking_Car.attach(3);
 
//=============================
//   Ultra sonic Setup
//=============================
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  blocking_Car.write(0);
}
 
//===========================
// LCD Function
//===========================
void lcd_pass(char ** car_info){
      Check_clear();
      lcd.setCursor(0,0); lcd.print("num:"); lcd.print(car_info[0]);      
      lcd.setCursor(0,1);
      if(atoi(car_info[1]) == -1){
        lcd.print("Not Reg Car");
        //Serial.println("주차 가능한 차량이 아닙니다"); 
        delay(2000);
        return;
      }
      
      if(car_info[2]==NULL){
        lcd.print("Don't exist space");
        //Serial.println("주차 공간이 없습니다");
        delay(2000);
        return;
      }
      blocking_Car.write(90);
      lcd.print("Go ");   lcd.print(car_info[2]);
      
      if(atoi(car_info[1]) == 1){
        lcd.print(" Park B");
      }
      else{
        lcd.print(" Park F");
      }
      delay(2000);
}

void lcd_Ready(void){
      Check_clear();
}



void Check_clear(void){
      if(Clear_compare == 0){
        lcd.clear(); 
        Clear_compare = 1;
      }
}


void loop() {
    long duration, distance;
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    duration = pulseIn (ECHO, HIGH);
    
    distance = duration * 17 / 1000;

    Serial.println(distance);
    if(distance <=15){
      if(status == HIGH){
        Clear_compare = 0;

        Serial.println("S");
        char* pToken;
        char *pArray[10] = {0};
        char RxBuffer[40];
        int idx = 0;
        while(1){
          if(Serial.available()>0){
            RxBuffer[idx++] = Serial.read();
            delay(1);
            if(RxBuffer[idx-1] == 'L'){
              RxBuffer[idx-1] ='\0';
              break;
            }
          }
        }
        
        pToken = strtok(RxBuffer,"@");
        idx = 0;

        while(pToken != NULL){
          pArray[idx] =pToken;        //pArray[0] : Car Number
          if(++idx>5) break;          //pArray[1] : type  , pArray[2] : rest_spot
          pToken = strtok(NULL,"@");
        }

        lcd_pass(pArray);

        

        /*
        char car_info[20] = "";
        int idx = 0;
        while(soft_Serial.available()){
          car_info[idx++] = soft_Serial.read();
          delay(1);
        }
        car_info[idx] = '\0';
        car_info[4] = '\0';
        car_info[6] = '\0';
        
        lcd_pass(car_info);
        
        blocking_Car.write(90); // 차단기 올라가기
       */
        status = LOW;
      }
    }
    else if(distance !=0 ){
      if(status == LOW){
        Clear_compare = 0;
        lcd_Ready();
        delay(500);

        blocking_Car.write(0); // 차단기 내려가기
        status = HIGH;
      }
    }

    delay(1000);
}
