#include <DynamixelWorkbench.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <sstream>

#if defined(__OPENCM904__)
  #define DEVICE_NAME "3" //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP
#elif defined(__OPENCR__)
  #define DEVICE_NAME ""
#endif   

#define Degree0 0
#define Degree90 1023
#define Degree180 2046
#define Degree270 3069
#define Degree360 4092

#define BAUDRATE  1000000

DynamixelWorkbench dxl_wb;

uint8_t id_list[4]= {1,2,3,4};
bool result = false;

ros::NodeHandle  nh;
std_msgs::String str_msg;
ros::Publisher Position("Position", &str_msg);

void jointModeSetting();

char hello1[] = "Right";
char hello2[] = "Left";


void setup() 
{
  Serial.begin(57600);
  nh.initNode();
  nh.advertise(Position);
  jointModeSetting();
}

void loop() 
{
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree90);
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree0);
  dxl_wb.goalPosition(id_list[2], (int32_t)Degree0);
  dxl_wb.goalPosition(id_list[3], (int32_t)Degree0);

  delay(3000);
  
  dxl_wb.goalPosition(id_list[0], (int32_t)Degree90/3);
  delay(1000);
  dxl_wb.goalPosition(id_list[1], (int32_t)Degree90);
  dxl_wb.goalPosition(id_list[2], (int32_t)(Degree90/3 + Degree90));

  for(int count = 0; count<60;count++){
    dxl_wb.goalPosition(id_list[1], (int32_t)(Degree90 + count*12));
    dxl_wb.goalPosition(id_list[3], (int32_t)(count*2*12));
    delay(100);
  }
}


void jointModeSetting(void){
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
  
  bool result_move[4];
  for(int idx = 0 ; idx < 4 ; idx ++){
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
