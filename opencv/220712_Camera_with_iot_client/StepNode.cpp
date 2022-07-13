#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>


string recvSensor;

int recvDynamixel = -1;
int recvCamera = -1;
//int recvSensor = -1;
int step = 0;
void SensorCallback(const std_msgs::String::ConstPtr& msg)
{
    std::stringstream stream;
    stream.str(msg->data.c_str());
    stream >> recvSensor;

    if (recvSensor == [IN1]) {
        if(step == 0)
            step = 1;  
            ROS_INFO("Car IN, So FindNumber");   
    }

    if (recvSensor >= 0) {
        ROS_INFO("recvSensor: [%d]", recvSensor);
        if (mode == -1) {
            mode = recvSensor;
            ROS_INFO("mode Change : [%d]", mode);
        }
    }
    stream.str("");
}

void CameraCallback(const std_msgs::String::ConstPtr& msg)
{
    std::stringstream stream;
    stream.str(msg->data.c_str());
    int vehicle_num;
    stream >> recvCamera;
    stream >> vehicle_num;
    if (step == 2) {
        if (recvCamera == 2) {
            step = 3;
            ROS_INFO("Jetson - get_vehicle: [%d]", vehicle_num);
        }

    }
}


void DynamixelCallback(const std_msgs::String::ConstPtr& msg)
{

    std::stringstream stream;
    stream.str(msg->data.c_str());
    stream >> recvDynamixel;
    if (step == 1) {
        if (recvDynamixel == 2) {
            step = 2;
            ROS_INFO("Dynamix - Camera Ready.");
        }
    }

    ROS_INFO("recvDynamixel: [%d]", recvDynamixel);
    if (mode == recvDynamixel) {
        ROS_INFO("mode Stop: [%d]", mode);
        mode = -1;
    }
}


int main(int argc, char** argv) {

    ros::init(argc, argv, "mode_node");
    ros::NodeHandle n;
    ros::Publisher step_pub = n.advertise<std_msgs::String>("step", 1000);
    ros::Subscriber Sensor_sub = n.subscribe("Sensor", 1000, SensorCallback);
    ros::Subscriber Camera_sub = n.subscribe("CameraMode", 1000, CameraCallback);
    ros::Subscriber Dynamix_sub = n.subscribe("DynamixMode", 1000, DynamixelCallback);

    ros::Rate loop_rate(1);

    std::stringstream ss1;
    std_msgs::String msg;
    while (ros::ok())
    {
        ss1 << step;
        msg.data = ss1.str();
        //ROS_INFO("nowMode : %s", msg.data.c_str());
        if (step >= 0) {
            step_pub.publish(msg);
        }
        ss1.str("");
        ros::spinOnce();
        loop_rate.sleep();
    }

}
