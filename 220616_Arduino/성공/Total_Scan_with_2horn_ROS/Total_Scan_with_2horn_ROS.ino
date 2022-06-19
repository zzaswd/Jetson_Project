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


//int16_t get_data[3][3] = {0};
int flag = 0;

bool result = false;

int receiveData;
ros::NodeHandle  nh;
std_msgs::String echo_msg;
ros::Publisher Echo("echo", &echo_msg);

void DLX_Set();

int Degree(int num);

void move_horn(int idx, int angle, int delay_time,const char **l);

void Moving_Circle();

void Find_Hall();

void messageCb( const std_msgs::String& msg) {
    std::stringstream stream;
    stream.str(msg.data);
    echo_msg.data = msg.data;
    stream>>receiveData;
    stream.str("");
    if(receiveData == 1) flag = 1;
    else if (receiveData == 2) flag = 2;
    Echo.publish(&echo_msg);
}

ros::Subscriber<std_msgs::String> sub("mode", messageCb );

void setup() 
{
  Serial.begin(57600);
  //while(!Serial); // Wait for Opening Serial Monitor

  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(Echo);
  
  DLX_Set();
}

void loop() 
{
  nh.spinOnce();
  delay(100);
  if(flag == 1){
    Moving_Circle();
    flag = -1;
  }
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

void Set_default(const char **l){
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(180));
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(180));
 
  move_horn(0,180,1000,l);
  move_horn(1,300,3000,l);
}

void Scan_Front(const char **l){
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(210));
  move_horn(1,60,1000,l);

  dxl_wb.jointMode(id_list_2horn[1], 0, 0,l);
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(150));
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(120));
  
  for(int i = 0; i < 120 ;  i++){
     dxl_wb.goalPosition(id_list[0], (int32_t)Degree(120+i));
     if(i<60)
        dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(90-i/4));
     else 
        dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(60+i/4));
     delay(100);
  }
  delay(900);
  dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(300));
  delay(2000);
  dxl_wb.itemWrite(id_list_2horn[1], "Torque_Enable", 0, l);
  
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(180));
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(180));
  delay(1000);
}


void Scan_Left(const char **l){
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(240));
  delay(1000);

  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(90));
  delay(1000);

  move_horn(0,240,1000,l);
  move_horn(1,270,500,l);

  Find_Hall(l);

  move_horn(0,180,1000,l);

  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(180));
  delay(500);
}

void Scan_Right(const char **l){
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree(120));
  delay(1000);
  
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree(90));
  delay(1000);

  move_horn(0,240,1000,l);

  Find_Hall(l);

  move_horn(0,180,1000,l);
}
void Moving_Circle(){
  const char *log;

//===================
// Defalut 각도 설정
//===================
  Set_default(&log);

//===================
// 전면부 scan
//===================
  Scan_Front(&log);
  
//===================
// 차량 왼쪽 scan
//===================
  Scan_Left(&log);

//===================
// 차량 오른쪽 scan
//===================  
  Scan_Right(&log);
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


void Find_Hall(const char **l){
    
  dxl_wb.jointMode(id_list_2horn[1], 0, 0, l);
  dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(270));
  delay(500);
  
  for(int i =0 ; i<60; i++){
    dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(270-i));
    delay(100);
    if(i==30) flag =2;
    int32_t get_data = 0;
    if(flag==2){
        result = dxl_wb.itemRead(id_list_2horn[1], "Present_Position", &get_data, l);
        if (result == false)
        {
          Serial.println(*l);
          Serial.println("Failed to get present position");
        }
        else
        {
          Serial.print("Succeed to get present position(value : ");
          Serial.print(get_data);
          Serial.println(")");
        }
       flag = 0;
    }
//    Echo.publish(&echo_msg);
    nh.spinOnce();
  }
  delay(1000);

  dxl_wb.goalPosition(id_list_2horn[1], (int32_t)Degree(300));
  delay(1000);
  dxl_wb.itemWrite(id_list_2horn[1], "Torque_Enable", 0, l);
}
