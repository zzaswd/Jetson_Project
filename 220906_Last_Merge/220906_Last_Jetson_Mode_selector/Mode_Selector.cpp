//=============================================================================================
// Mode Selector on ROS
// 3���� Topic(Jetson1, Camera, Dynamixel) �� Subscribe�ϰ�, 
// �ش� Topic Message�� ���� �ٸ� ���� ���� + "mode" Topic Publish
// ��, ROS ������ ���� �����͸� �ۼ����ϴ� �� 3����(Jetson1, Camera, Manipulator) ���� ������ �ۼ��� ���� ����
// 
// 1. "Jetson1" Topic message�� 1�̸�(���� �ڸ��� �����ϸ�) mode "1" publish.
// 
// 2. mode "1"�� ���Ź��� Manipulator�� ������ ���� ������ ���� �� �ʱ� ���·� ���.
//    �� �� "Dynamixel 2"�� pub�ϴµ�, �װ��� Subscribe�ϸ� mode "2"�� publish�ϸ鼭 Camera ���� ����.
// 
// 3. ī�޶� Delta X�� Check. ���� abs(Delta X) > 1�̸� Camera "3" Pub, angle ang1@ang2L Pub.
//    ���� abs(Delta X) < 1�̸� Camera "4" Pub, angle ang3@ang4L Pub.
//    �װ��� �״�� mode "3" or mode "4"��  Pub���༭ Manipulator�� � Dynamixel�� ������ �� ����.
// 
// 4. Delta Y���� �������ٸ�, Dynamixel�� "5" Pub.
//    �װ��� ���Ź��� Mode Selector�� mode "5"�� Pub�ϸ� SPOT1���� ���� �غ�Ǿ��ٰ� 5L �۽�(Socket����)
// 
// 5. ������ �Ϸ�Ǿ��ٸ� Bridge���� 6L�� ���� ����.
//    �װ��� �� Jetson1 "6"���� Pub �ǰ�, ���� mode == 6�̸� ������ ���� ������ �ݳ��ϰ�, ��� ����.
//    ��� ���°� �Ǹ鼭 Dynamixel "7"�� Pub�ϸ鼭 ��� �������� �˷���.
// 
// 6. �����±��� �Դٸ� ���� ������ �̵����Ѿ� �� ����.
//    ���� SPOT1���� ������ �ű��� �ǹ̿� 7L �۽�(Socket����) 
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
    if (recvDynamixel == 2) {   // init pose��, set_complete�� ��ġ �Ϸ��ߴٴ� �ǹ�.
        mode = 2;
        ROS_INFO("Dynamixel Pose Ok: [%d]", mode);
    }
    else if (recvDynamixel == 5) {  // ���� �Ϸ��ߴٴ� �ǹ�..
        mode = 5;
        ROS_INFO("Start Charging: [%d]", mode);
    }
    else if (recvDynamixel == 7)  {  // ���� ���� �� �����ٴ� �ǹ�.
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

            if (mode == 5) send_msg(5); // 5��� ����, Connect �ߴٴ� ��. ���� Spot���� �޼��� �����ؼ� ���� �����ض��� �ǹ�.
            else if (mode == 7) send_msg(7); // 7��� ����, Init Pose�� ��� ���̴�, �� ���Ϸ� �̵����Ѷ� ��� �ǹ�.
        }
        mode = -1;
        ros::spinOnce();
        loop_rate.sleep();
    }
}


