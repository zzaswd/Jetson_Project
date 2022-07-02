#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
//#include <sensor_msgs/image_encoding.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");

        cv::Mat real_img = cv_ptr->image;
        cv::Mat gray_img2;
        cv::cvtColor(real_img, gray_img2, cv::COLOR_BGR2GRAY);

        //cv::medianBlur(gray_img,gray_img2,5);

        vector<cv::Vec3f> circles;

        cv::HoughCircles(gray_img2, circles, cv::HOUGH_GRADIENT, 1, 100, 80, 70, 0, 0);

        for (size_t i = 0; i < circles.size(); i++) {
            cv::Vec3i c = circles[i];
            cv::Point center(c[0], c[1]);
            ROS_INFO("real : %d   gray : %d", c[0], c[1]);
            int radius = c[2];

        }

        ROS_INFO("channel :  %d", gray_img2.channels());

        cv::imshow("real_img", real_img);
    //
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
    cv::namedWindow("view2");

    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/camera/color/image_raw", 1, imageCallback);
    ros::spin();
    cv::destroyWindow("view2");
}
