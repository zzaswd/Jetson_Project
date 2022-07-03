#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
//#include <string>
#include <opencv2/opencv.hpp>
//#include <vector>

using namespace std;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        //cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_16UC1);

        cv::Mat real_img = cv_ptr->image;     
        
//        ROS_INFO("Center Depth = %d  width : %d  height : %d",real_img.data[320 * 240],real_img.cols,real_img.rows);
        ROS_INFO("Center Depth = %lf", 0.1*real_img.at<u_int16_t>(240,320));

       // ROS_INFO("channel : %d count : %d ", real_img.channels(), count);
        //cv::imshow("view1", real_img);

        cv::waitKey(30);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_listener2");
    ros::NodeHandle nh;
    cv::namedWindow("view3");

    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/camera/aligned_depth_to_color/image_raw", 1, imageCallback);
    //image_transport::Subscriber sub = it.subscribe("/camera/depth/image_rect_raw", 1, imageCallback);
    ros::spin();
    cv::destroyWindow("view3");
}