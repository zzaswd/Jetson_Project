/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Authors: Yoonseok Pyo, Leon Jung, Darby Lim, HanCheol Cho */
#include <DynamixelWorkbench.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Byte.h>
#include <sstream>

#if defined(__OPENCM904__)
  #define DEVICE_NAME "3" //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP
#elif defined(__OPENCR__)
  #define DEVICE_NAME ""
#endif   

ros::NodeHandle  nh;

std_msgs::String send_msg;
ros::Publisher Echo("echo", &send_msg);

#define BAUDRATE  1000000

DynamixelWorkbench dxl_wb;

int receiveData;
uint8_t id_list[4]= {1,2,3,4};
bool result = false;
void jointModeSetting();

void messageCb( const std_msgs::String& msg) {
    std::stringstream stream;
    stream.str(msg.data);
    send_msg.data = msg.data;
    stream>>receiveData;
    
    if(receiveData == 1){
        dxl_wb.goalPosition(id_list[0], (int32_t)0);
        dxl_wb.goalPosition(id_list[1], (int32_t)0);
        dxl_wb.goalPosition(id_list[2], (int32_t)0);
        dxl_wb.goalPosition(id_list[3], (int32_t)0);
    }
  
    else if(receiveData == 2){
        dxl_wb.goalPosition(id_list[0], (int32_t)1023);
        dxl_wb.goalPosition(id_list[1], (int32_t)1023);
        dxl_wb.goalPosition(id_list[2], (int32_t)1023);
        dxl_wb.goalPosition(id_list[3], (int32_t)1023);
    }
    stream.str("");
    Echo.publish(&send_msg);
}

ros::Subscriber<std_msgs::String> sub("mode", messageCb );

void setup() {
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(Echo);
  jointModeSetting();
}

void loop() {
 
  nh.spinOnce();
  delay(100);
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
