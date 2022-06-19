#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>

int main(int argc, char **argv)
{
  // 
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<std_msgs::String>("chat", 1000);
  ros::Rate loop_rate(1);
  int count = 0;

  while (ros::ok())
  {
    
    std_msgs::String msg;

    std::stringstream ss1;
    ss1<<1;
    msg.data = ss1.str();

    ROS_INFO("%s", msg.data.c_str());

    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
    loop_rate.sleep();


    std::stringstream ss2;
    ss2<<2;
    msg.data = ss2.str();

    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
    loop_rate.sleep();
  }


  return 0;
}
