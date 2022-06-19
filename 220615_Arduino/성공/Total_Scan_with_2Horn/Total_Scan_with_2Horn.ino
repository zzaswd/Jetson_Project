#include <DynamixelWorkbench.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <sstream>

#if defined(__OPENCM904__)
  #define DEVICE_NAME "3" //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP
#elif defined(__OPENCR__)
  #define DEVICE_NAME ""
#endif   


#define BAUDRATE  1000000

DynamixelWorkbench dxl_wb;

uint8_t id_list[2]= {1,2};
uint8_t id_list_2horn[2] = {7,8};
bool result = false;

ros::NodeHandle  nh;
std_msgs::String str_msg;
ros::Publisher Position("Position", &str_msg);

void DLX_Set();
void Horn2_DLX_Set();

void setup() 
{
  Serial.begin(57600);
  while(!Serial); // Wait for Opening Serial Monitor

  nh.initNode();
  nh.advertise(Position);
  DLX_Set();
}

int Degree(int num){
  return (num/360.0)*4092;
}

void move_horn(int idx, int angle, int delay_time,const char **l){
  dxl_wb.jointMode(id_list_2horn[idx], 0, 0, l);
  dxl_wb.goalPosition(id_list_2horn[idx], (int32_t)Degree(angle));
  delay(delay_time);
  dxl_wb.itemWrite(id_list_2horn[idx], "Torque_Enable", 0, l);
}

void Find_hall(){
  const char *log;

//===================
// Defalut 각도 설정
//===================
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(180));
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(180));
 
  move_horn(0,180,1000,&log);
  move_horn(1,300,3000,&log);

//===================
// 차량 왼쪽 scan
//===================
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(240));
  delay(1000);

  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(90));
  delay(1000);

  move_horn(0,240,1000,&log);
  move_horn(1,270,500,&log);
  
  dxl_wb.jointMode(id_list_2horn[1], 0, 0, &log);
  dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(270));
  delay(500);
  for(int i =0 ; i<60; i++){
    dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(270-i));
    delay(100);
  }
  delay(1000);

  dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(300));
  delay(1000);
  dxl_wb.itemWrite(id_list_2horn[1], "Torque_Enable", 0, &log);

  move_horn(0,180,1000,&log);


  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(180));
  delay(500);

//===================
// 차량 오른쪽 scan
//===================  
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(120));
  delay(1000);
  
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(90));
  delay(1000);

  move_horn(0,240,1000,&log);

  dxl_wb.jointMode(id_list_2horn[1], 0, 0, &log);
  dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(270));
  delay(500);
  for(int i =0 ; i<60; i++){
    dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(270-i));
    delay(100);
  }
  delay(1000);
  dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(300));
  delay(1000);
  dxl_wb.itemWrite(id_list_2horn[1], "Torque_Enable", 0, &log);


  move_horn(0,180,1000,&log);
}
void loop() 
{
  Find_hall();
}

void DLX_Set(void){
  const char *log;
  uint16_t model_number = 0;
  
  // 동작 안될 때 확인용
  result = dxl_wb.init(DEVICE_NAME, BAUDRATE, &log);

  if (result == false)
  {
    Serial.println(log);
    Serial.println("Failed to init");
  }
  else
  {
    Serial.print("Succeeded to init : ");
    Serial.println(BAUDRATE);  
  }
  
  for(int idx = 0; idx < sizeof(id_list)/sizeof(char);idx++){
      result = dxl_wb.ping(id_list[idx], &model_number, &log);
      if (result == false)
      {
        Serial.println(log);
        Serial.println("Failed to ping");
      }
      else
      {
        Serial.println("Succeeded to ping");
        Serial.print("id : ");
        Serial.print(id_list[idx]);
        Serial.print(" model_number : ");
        Serial.println(model_number);
      }
  }

  for(int idx = 0; idx < sizeof(id_list_2horn)/sizeof(char);idx++){
      result = dxl_wb.ping(id_list_2horn[idx], &model_number, &log);
      if (result == false)
      {
        Serial.println(log);
        Serial.println("Failed to ping");
      }
      else
      {
        Serial.println("Succeeded to ping");
        Serial.print("id : ");
        Serial.print(id_list_2horn[idx]);
        Serial.print(" model_number : ");
        Serial.println(model_number);
      }
  }
  
  bool result_move[sizeof(id_list)/sizeof(char)];
  for(int idx = 0 ; idx < sizeof(id_list)/sizeof(char) ; idx ++){
    result_move[idx]=dxl_wb.jointMode(id_list[idx], 0, 0, &log);
    if(result_move[idx]==false){
      Serial.println(log);
      Serial.print("Failed to change joint mode : ");
      Serial.println(idx);
    }
  }  
  Serial.println("Succeed to change joint mode");
  Serial.println("Dynamixel is moving...");
}
