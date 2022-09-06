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

int tar_x = 0;
int tar_y = 0;
int del_x_image = 0;
int del_y_image = 0;
float del_x_real = 0;
float del_y_real = 0;

int flag = 0;

float theta_image;

void GetDepth(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_16UC1);
        cv::Mat real_img = cv_ptr->image;
        float target_depth = 0.1 * real_img.at<u_int16_t>(tar_y, tar_x);
        float origin_depth = 0.1 * real_img.at<u_int16_t>(240, 320);

        ROS_INFO(" target_depth = %.2f  origin_depth = %.2f", target_depth, origin_depth);
        float theta_real = acos((origin_depth * 1.0) / (target_depth * 1.0));//* (180.0 / Pi);
        float distance = target_depth * sin(theta_real);
        del_x_real = (-1) * distance * cos(theta_image * Pi / 180.0);
        del_y_real = (-1) * distance * sin(theta_image * Pi / 180.0);

        ROS_INFO(" theta_real = %.2f  dis = %.2f", theta_real*(180.0/Pi), distance);
        //ROS_INFO("Target Depth = % lf", target_depth);
        ROS_INFO(" del_x_real = %.2f   del_y_real = %.2f", del_x_real, del_y_real);


        cv::waitKey(30);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

void FindCircle(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        // real_img.copyTo(gray_img);
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");

        cv::Mat real_img = cv_ptr->image;
        cv::Mat gray_img2;
        cv::Mat origin_img;
        real_img.copyTo(origin_img);
        cv::imshow("Origin_img", origin_img);

        cv::cvtColor(real_img, gray_img2, cv::COLOR_BGR2GRAY);
        medianBlur(gray_img2, gray_img2, 5);

        cv::Mat test1[] = { gray_img2,gray_img2,gray_img2 };
        cv::Mat test1_merge;
        cv::merge(test1, 3, test1_merge);
        cv::imshow("gray_img", test1_merge);


        //cv::medianBlur(gray_img,gray_img2,5);
        vector<cv::Vec3f> circles;

        cv::HoughCircles(gray_img2, circles, cv::HOUGH_GRADIENT, 1, 150, 80, 70, 0, 100);
        if (circles.size() > 0) flag = 1;
        for (size_t i = 0; i < circles.size(); i++) {
            cv::Vec3i c = circles[i];
            cv::Point center(c[0], c[1]);
            tar_x = c[0];
            tar_y = c[1];
            del_x_image = c[0] - 320;
            del_y_image = 240 - c[1];
            cv::circle(test1_merge, cv::Point(c[0], c[1]), c[2], CV_RGB(255, 0, 0),5);
            //ROS_INFO("x_circle : %d   y_circle : %d", c[0], c[1]);

            if (del_x_image >= 0 && del_y_image >= 0) theta_image = (atan((del_y_image * 1.0) / (del_x_image * 1.0)) * (180.0 / Pi));
            else if (del_x_image <= 0 && del_y_image >= 0) theta_image = 180 - (atan((del_y_image * 1.0) / (del_x_image * (-1.0))) * (180.0 / Pi));
            else if (del_x_image <= 0 && del_y_image <= 0) theta_image = (atan((del_y_image * 1.0) / (del_x_image * 1.0)) * (180.0 / Pi)) + 180;
            else if (del_x_image >= 0 && del_y_image <= 0) theta_image = 360 - (atan((del_y_image * 1.0) / (del_x_image * (-1.0))) * (180.0 / Pi));

            //ROS_INFO("Del_X : %d  Del_Y : %d  Theta : %f", del_x_image, del_y_image, theta_image);

            //int radius = c[2];
            //circle(gray_img2,center,radius,cv::Scalar(0,255,0),2);
            //circle(gray_img2,center,2,cv::Scalar(0,0,255),3);
        }


        //ROS_INFO("channel :  %d",  gray_img2.channels());

        cv::imshow("Calculate Distance", test1_merge);
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
    image_transport::Subscriber sub1 = it.subscribe("/camera/aligned_depth_to_color/image_raw", 1, GetDepth);
    image_transport::Subscriber sub2 = it.subscribe("/camera/color/image_raw", 1, FindCircle);
    ros::spin();
    cv::destroyWindow("view2");
}
