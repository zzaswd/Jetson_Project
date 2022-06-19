#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>

int recvDynamixel = -1;
int recvSensor = -1;
int mode = -1;
void SensorCallback(const std_msgs::String::ConstPtr& msg)
{
    std::stringstream stream;
    stream.str(msg->data.c_str());
    stream >> recvSensor;
    if (recvSensor >= 0) {
        ROS_INFO("recvSensor: [%d]", recvSensor);
        if (mode == -1) {
            mode = recvSensor;
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
    if (mode == recvDynamixel) {
        ROS_INFO("mode Stop: [%d]", mode);
        mode = -1;
    }
}


int main(int argc, char** argv) {

    ros::init(argc, argv, "mode_node");
    ros::NodeHandle n;
    ros::Publisher mode_pub = n.advertise<std_msgs::String>("mode", 1000);
    ros::Subscriber sensor_sub = n.subscribe("chat", 1000, SensorCallback);
    ros::Subscriber dynamixel_sub = n.subscribe("echo", 1000, DynamixelCallback);

    ros::Rate loop_rate(1);

    std::stringstream ss1;
    std_msgs::String msg;
    while (ros::ok())
    {
        ss1 << mode;
        msg.data = ss1.str();
        ROS_INFO("nowMode : %s", msg.data.c_str());
        if (mode >= 0) {
            mode_pub.publish(msg);
        }
        ss1.str("");
        ros::spinOnce();
        loop_rate.sleep();
    }

}
