#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");

        cv::Mat real_img = cv_ptr->image;
        cv::Mat gray_img1;
        cv::Mat gray_img2;
        cv::Mat gray_img3;
        real_img.convertTo(gray_img1, CV_8UC1);
        cv::cvtColor(real_img, gray_img2, cv::COLOR_BGR2GRAY);

        gray_img3 = cv::Mat_<uchar>(real_img.rows, real_img.cols);

        uchar* ptr_src = real_img.data;
        uchar* ptr_dst2 = gray_img3.data;

        for (int i = 0; i < real_img.rows; i++) {
            for (int j = 0; j < real_img.cols; j++) {
                ptr_dst2[i * real_img.cols + j] = 0.114 * ptr_src[i * real_img.cols * 3 + j * 3] + 0.587 * ptr_src[i * real_img.cols * 3 + j * 3 + 1] + 0.299 * ptr_src[i * real_img.cols * 3 + j * 3 + 2];
            }
        }


        int count = 0;
        for (int idx = 0; idx < gray_img2.rows; idx++) {
            for (int jdx = 0; jdx < gray_img2.cols; jdx++) {
                if (gray_img2.data[idx * jdx + jdx] <= gray_img3.data[idx * jdx + jdx] + 20 && gray_img2.data[idx * jdx + jdx] >= gray_img3.data[idx * jdx + jdx] - 20)
                    count++;
            }

        }
        ROS_INFO("count : %d", count);

        ROS_INFO("ch1 : %d  ch2 : %d  ch3 : %d", gray_img1.channels(), gray_img2.channels(), gray_img3.channels());

        cv::imshow("real_img", real_img);
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