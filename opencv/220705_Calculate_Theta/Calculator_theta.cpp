#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
//#include <sensor_msgs/image_encoding.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

#define Pi 3.1415926535897
using namespace std;


int del_x = 0;
int del_y = 0;
int flag = 0;
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        // real_img.copyTo(gray_img);
        float theta;
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");

        cv::Mat real_img = cv_ptr->image;
        cv::Mat gray_img2;
        cv::cvtColor(real_img, gray_img2, cv::COLOR_BGR2GRAY);

        //cv::medianBlur(gray_img,gray_img2,5);

        vector<cv::Vec3f> circles;

        cv::HoughCircles(gray_img2, circles, cv::HOUGH_GRADIENT, 1, 100, 80, 70, 0, 0);
        if (circles.size() > 0) flag = 1;
        for (size_t i = 0; i < circles.size(); i++) {
            cv::Vec3i c = circles[i];
            cv::Point center(c[0], c[1]);
            del_x = c[0] - 320;
            del_y = 240 - c[1];

            ROS_INFO("x_circle : %d   y_circle : %d", c[0], c[1]);

            if (del_x >= 0 && del_y >= 0) theta = (atan((del_y * 1.0) / (del_x * 1.0)) * (180.0 / Pi));
            else if (del_x <= 0 && del_y >= 0) theta = (atan((del_y * 1.0) / (del_x * (-1.0))) * (180.0 / Pi)) + 90;
            else if (del_x <= 0 && del_y <= 0) theta = (atan((del_y * 1.0) / (del_x * 1.0)) * (180.0 / Pi)) + 90;
            else if (del_x >= 0 && del_y <= 0) theta = (atan((del_y * 1.0) / (del_x * 1.0)) * (180.0 / Pi)) + 360;

            ROS_INFO("Del_X : %d  Del_Y : %d  Theta : %f", del_x, del_y, theta);

            //int radius = c[2];
            //circle(gray_img2,center,radius,cv::Scalar(0,255,0),2);
            //circle(gray_img2,center,2,cv::Scalar(0,0,255),3);
        }


        //ROS_INFO("channel :  %d",  gray_img2.channels());

        cv::imshow("real_img", real_img);
        //cv::imshow("gray_img1",gray_img1);
      // cv::imshow("result", res);
    //    cv::imshow("gray",gray_img);
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
