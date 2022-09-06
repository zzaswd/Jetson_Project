//=============================================================================================
// Mode Selector on ROS
// 3가지 Topic(Jetson1, Camera, Dynamixel) 을 Subscribe하고, 
// 해당 Topic Message에 따라 다른 동작 수행 + "mode" Topic Publish
// 즉, ROS 서버를 통해 데이터를 송수신하는 총 3가지(Jetson1, Camera, Manipulator) 간의 데이터 송수신 순서 설정
// 
// 1. "Jetson1" Topic message가 1이면(차가 자리에 도착하면) mode "1" publish.
// 
// 2. mode "1"을 수신받은 Manipulator가 차종에 따라 충전구 선택 및 초기 상태로 대기.
//    그 후 "Dynamixel 2"를 pub하는데, 그것을 Subscribe하면 mode "2"을 publish하면서 Camera 동작 시작.
// 
// 3. 카메라가 Delta X을 Check. 만약 abs(Delta X) > 1이면 Camera "3" Pub, angle ang1@ang2L Pub.
//    만약 abs(Delta X) < 1이면 Camera "4" Pub, angle ang3@ang4L Pub.
//    그것을 그대로 mode "3" or mode "4"로  Pub해줘서 Manipulator가 어떤 Dynamixel을 제어할 지 설정.
// 
// 4. Delta Y까지 맞춰졌다면, Dynamixel은 "5" Pub.
//    그것을 수신받은 Mode Selector는 mode "5"를 Pub하며 SPOT1에게 충전 준비되었다고 5L 송신(Socket으로)
// 
// 5. 충전이 완료되었다면 Bridge에서 6L을 수신 받음.
//    그것은 곧 Jetson1 "6"으로 Pub 되고, 따라서 mode == 6이면 충전기 때고 충전구 반납하고, 대기 상태.
//    대기 상태가 되면서 Dynamixel "7"을 Pub하면서 대기 상태임을 알려줌.
// 
// 6. 대기상태까지 왔다면 이제 차량을 이동시켜야 할 차례.
//    따라서 SPOT1에게 차량을 옮기라는 의미에 7L 송신(Socket으로) 
//=============================================================================================


#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 20

char msg[BUF_SIZE];


using namespace std;

int recvCamera = -1;
int recvDynamixel = -1;
int recvSensor = -1;
int mode = -1;

void send_msg(int M) {

    int sock;
    struct sockaddr_in serv_addr;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        printf("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("10.10.141.188");
    serv_addr.sin_port = htons(5000);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        printf("connect() error");

    memset(msg, 0, sizeof(msg));
    sprintf(msg, "[Start1:PASSWD]");
    write(sock, msg, strlen(msg));

    memset(msg, 0, sizeof(msg));
    sprintf(msg, "[SPOT1] %dL\n",M);
    write(sock, msg, strlen(msg));
    close(sock);
}

void SensorCallback(const std_msgs::String::ConstPtr& msg)
{

    std::stringstream stream;
    stream.str(msg->data.c_str());
    char ch[30] = { 0 };
    //string ch;
    stream >> ch;

    char* pToken;
    char* pArray[10] = { 0 };
    //ch[index - 1] = '\0';

    pArray[0] = strtok(ch, "]");
    
    pArray[0] = strtok(NULL, "@");
    if (strcmp(pArray[0], "6") == 0) {
        mode = 6;
        ROS_INFO("recvSensor: [%d]", mode);
        return;
    }

    pArray[1] = strtok(NULL, "@");

    pArray[2] = strtok(NULL, "L");

    int inout = stoi(pArray[0]);

    if (inout > 0) {
        ROS_INFO("recvSensor: [%d]", inout);
        if (mode == -1) {
            mode = inout;
            ROS_INFO("mode Change : [%d]", mode);
        }
    }

    stream.str("");
}


void DynamixelCallback(const std_msgs::String::ConstPtr& msg)
{
    std::stringstream stream;
    stream.str(msg->data.c_str());
    stream >> recvDynamixel;
    ROS_INFO("recvDynamixel: [%d]", recvDynamixel);
    if (recvDynamixel == 2) {   // init pose건, set_complete건 위치 완료했다는 의미.
        mode = 2;
        ROS_INFO("Dynamixel Pose Ok: [%d]", mode);
    }
    else if (recvDynamixel == 5) {  // 연결 완료했다는 의미..
        mode = 5;
        ROS_INFO("Start Charging: [%d]", mode);
    }
    else if (recvDynamixel == 7)  {  // 연결 해제 및 끝났다는 의미.
        mode = 7;

    }
}


void CameraCallback(const std_msgs::String::ConstPtr& msg)
{
    std::stringstream stream;
    stream.str(msg->data.c_str());
    stream >> recvCamera;
    ROS_INFO("recvCamera: [%d]", recvCamera);
    if (recvCamera == 3) {
        mode = 3;
        ROS_INFO("Calculate OK: [%d]", mode);
    }
    else if (recvCamera == 4) {
        mode = 4;
        ROS_INFO("Center OK: [%d]", mode);
    }

}



int main(int argc, char** argv) {

    ros::init(argc, argv, "mode_node");
    ros::NodeHandle n;
    ros::Publisher mode_pub = n.advertise<std_msgs::String>("mode", 1000);
    ros::Subscriber sensor_sub = n.subscribe("Jetson1", 1000, SensorCallback);
    ros::Subscriber dynamixel_sub = n.subscribe("Dynamixel", 1000, DynamixelCallback);
    ros::Subscriber Camera_sub = n.subscribe("Camera", 1000, CameraCallback);

    ros::Rate loop_rate(1);

    std::stringstream ss1;
    std_msgs::String msg;
    while (ros::ok())
    {
        if (mode > 0) {
            ss1 << mode;
            msg.data = ss1.str();
            ROS_INFO("nowMode : %s", msg.data.c_str());
            mode_pub.publish(msg);
            ss1.str("");

            if (mode == 5) send_msg(5); // 5라는 뜻은, Connect 했다는 뜻. 따라서 Spot에게 메세지 전송해서 충전 시작해라라는 의미.
            else if (mode == 7) send_msg(7); // 7라는 뜻은, Init Pose로 대기 중이니, 차 지하로 이동시켜라 라는 의미.
        }
        mode = -1;
        ros::spinOnce();
        loop_rate.sleep();
    }
}


