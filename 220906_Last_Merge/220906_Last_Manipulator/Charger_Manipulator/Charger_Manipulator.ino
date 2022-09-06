#include <DynamixelWorkbench.h>
#include <ros.h>             //로스 헤더
#include <std_msgs/String.h> //스트링 메세지(로스 통신 원형 지정)
#include <sstream>
#include <string>

using namespace std;

#if defined(__OPENCM904__)
#define DEVICE_NAME "1"     //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP
#elif defined(__OPENCR__)   //opencr 보드 선언
#define DEVICE_NAME ""
#endif

#define BAUDRATE  1000000 //10만(다이나믹셀이 모드 해당 Baudrate)

DynamixelWorkbench dxl_wb;

int mode = -2;
int movingOK = 0;
int flag = 0;

string angle[2];

uint8_t id_list[4] = {1, 2, 3, 4};    //일반(1축)
uint8_t id_list_2horn[2] = {7, 8};    //2축은 2horn
bool result = false;

ros::NodeHandle  nh;

void modeCb( const std_msgs::String& msg);
void angleCb( const std_msgs::String& msg);

std_msgs::String Dynamixel_mode;
ros::Publisher Dynamixel("Dynamixel", &Dynamixel_mode);
ros::Subscriber<std_msgs::String> sub_a("angle", angleCb );
ros::Subscriber<std_msgs::String> sub_m("mode", modeCb );

void DLX_Set();
void Horn2_DLX_Set();


void setup()
{
  Serial.begin(115200);
  //while (!Serial); // Wait for Opening Serial Monitor

  nh.initNode();
  nh.subscribe(sub_m);
  nh.subscribe(sub_a);
  nh.advertise(Dynamixel);

  DLX_Set();    //초기화 함수
}

void loop()
{
  if (mode == -2) { //  매니퓰레이터 대기 상태
    Init_First();
  }
  else if (mode == 1) {  // 충전구 고르고 초기 상태로 이동.
    Check_Type();
    movingOK = 1;
  }
  else if (mode == 3) {  // Del_X 움직일 Angle Sub해서 움직이기
    while (!flag) { // Angle 받을 때까지 대기.
      nh.spinOnce();
      delay(1000);
    }
    Set_Complete_angle_x(angle[0], angle[1]);  // angle 받아왔으니 움직이기
    movingOK = 1;
  }
  else if (mode == 4) { // Del_Y 움직일 Angle Sub해서 움직이기.
    while (!flag) { // Angle 받을 때까지 대기.
      nh.spinOnce();
      delay(1000);
    }  
    Set_Complete_angle_y(angle[0], angle[1]);
    movingOK = 2;
  }
  else if (mode == 6) { // 충전 다 되었다는 의미. 따라서 충전구 반환하고, 대기 상태)
    Check_Type();
    Init_First();
    movingOK = 3;
  }
  
  if (movingOK == 1) {
    Dynamixel_mode.data = "2";
    Dynamixel.publish(&Dynamixel_mode);
  }
  else if (movingOK == 2) {
    Dynamixel_mode.data = "5";
    Dynamixel.publish(&Dynamixel_mode);
  }
  else if (movingOK == 3) {
    Dynamixel_mode.data = "7";
    Dynamixel.publish(&Dynamixel_mode);
  }

  movingOK = 0;
  mode = -1;
  flag = 0;
  angle[0] = "";
  angle[1] = "";
  nh.spinOnce();
}
