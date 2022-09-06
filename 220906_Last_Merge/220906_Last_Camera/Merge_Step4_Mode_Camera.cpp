//===============================================================================
// Calculate Depth & Realworld Distance
// Mode_selector���� mode "2"�� ���Ź��� ������ RealWorld Distance�� ���ϰ�, Angle ������ ��ȯ
// 
// Find_Circle�� �������� Ž���ϰ� "�߽���ǥ, Ÿ�ٰ� ī�޶��� ����"�� ��ȯ���ִ� �Լ�
// Get_Depth�� FindCircle�κ��� ���� ��ǥ�� Depth ������, ������ ����ϴ� �Լ�
// Calculate_X�� X�� ����ȭ�� �ʿ��� ���� ���ϴ� �Լ�
// Calculate_Y�� Y�� ����ȭ�� �ʿ��� ���� ���ϴ� �Լ�
//===============================================================================

#include <ros/ros.h>
#include "std_msgs/String.h"
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
//#include <sensor_msgs/image_encoding.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <unistd.h>


const double Pi=3.1415926535897;  // �ﰢ�Լ� ��� �� Radian ���� �ʿ��ѵ� ���� ���� ����.
                                                    // ����� ���� ���� ������ ��ȯ�ϱ� ���� �� 
using namespace std;

int tar_x = -1000;    // target�� pixel ��ǥ
int tar_y = -1000;    // target�� pixel ��ǥ
int del_x_image = 0;  // target�� pixel ��ǥ ���� ��, �̸� ���� ���� ���� ���� ���� ��
int del_y_image = 0;  // target�� pixel ��ǥ ���� ��, �̸� ���� ���� ���� ���� ���� ��

float del_x_real = 0; // ���� ���󿡼� �̵��ؾ��� �Ÿ�
float del_y_real = 0; // ���� ���󿡼� �̵��ؾ��� �Ÿ�

int flag = -1;
int recvMode = -1;
float theta_image;  // �ȼ������� Ÿ�ٰ��� ���� ����.
                                        // �̸� �̿��Ͽ� ���� Distance ����


// 180�� = PI
// 1 = PI / 180;
float depth, del_y;
float a = 13.5;
float b = 12.5;
float c = 2.0;


void Calculate_x(float del_x, int *th1, int *th2) {
    *th1 = asin(del_x / 13) * 180 / Pi;
    *th2 = *th1 * (-1);
}


void Calculate_y(int* th1, int* th2) {
    float x3 = a - c - depth+20;
    float y3 = -b - del_y*1.5;

    //float x3 = depth;
    //float y3 = del_y;
    int theta1, theta2;

    float min = 10000;
    int Min_angle1, Min_angle2;

    for (theta1 = -100; theta1 < 100; theta1++) {
        float x1 = a * cos(Pi * theta1 / 180.0);
        float y1 = -a * sin(Pi * theta1 / 180.0);
        //cout << " x1 = " << x1 << " y1 " << y1;
        theta2 = asin((x1 - x3 - c) / (b)) * 180.0 / Pi - theta1;

        cout << "  Theta 1 = " << theta1 << " Theta2 = " << theta2;

        float x2 = x3 + c;
        float y2 = y1 - b * cos(Pi * (theta1 + theta2) / 180);

        cout << " x2 = " << x2 << " y2 " << y2;

        if (((y2 < y3 + 1) && (y2 > y3 - 1)) && !(theta2 > 110 || theta2 < -110)) {
            cout << "  Yes";
            if (min > abs(y3 - y2)) {
                Min_angle1 = theta1;
                Min_angle2 = theta2;
                min = abs(y3 - y2);
            }
        }
        cout << endl;
    }

    if (min != 10000) {
        cout << "Min angle 1 = " << Min_angle1 << " Min angle 2 = " << Min_angle2 << endl;
        *th1 = Min_angle1;
        *th2 = Min_angle2;
    }
    else {
        cout << "Can't Condition";
        *th1 = -10000;
        *th2 = -10000;
    }
}


void GetDepth(const sensor_msgs::ImageConstPtr& msg) // �ȼ��� �Ÿ� ������ ���ϱ�
{
    if (recvMode != 2) return;           // Mode sub�� 2�϶����� ����
    if (!(tar_x <= 640) || !(tar_x >= 0) || !(tar_y <= 480) || !(tar_y >= 0)) return;
   // if (!((tar_x != -1000) && (tar_y != -1000))) return;    // FindCircle�� ���� ���� �ٲ���� ������ ����

    try
    {
        // 1. FindCircle���� �Ѱ� ���� Ÿ���� ��ǥ�� ���� ��ǥ�� �Ÿ� �����͸� ���Ѵ�.
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_16UC1);
        cv::Mat real_img = cv_ptr->image;
        float target_depth = 0.1 * real_img.at<u_int16_t>(tar_y, tar_x);
        float origin_depth = 0.1 * real_img.at<u_int16_t>(240, 320);

        ROS_INFO(" target_depth = %.2f  origin_depth = %.2f", target_depth, origin_depth);
        // 2. ������ ���� ������� Ÿ�����κ��� ������ ���� �������� ������ ���� ������ ���Ѵ�.
        // �׸��� �� ������ ������� ���� Distance�� ���Ѵ�.
        float theta_real = acos((origin_depth * 1.0) / (target_depth * 1.0));//* (180.0 / Pi);
        float distance = target_depth * sin(theta_real);
        // 3. �̹������� ���� ������ ���� ������ ��鿡���� ������ �����Ƿ� �׸� ���� �������� �� �Ÿ��� ���Ѵ�.
        del_x_real = (-1) * distance * cos(theta_image * Pi / 180.0);
        del_y_real = (-1) * distance * sin(theta_image * Pi / 180.0);

        ROS_INFO(" theta_real = %.2f  dis = %.2f", theta_real * (180.0 / Pi), distance);
        //ROS_INFO("Target Depth = % lf", target_depth);
        ROS_INFO(" del_x_real = %.2f   del_y_real = %.2f", del_x_real, del_y_real);
        if (abs(del_x_real) >= 1) flag = 1;
        else if (abs(del_x_real) < 1) {
            flag = 2;
            depth = origin_depth;
            del_y = del_y_real * (-1);
        }
   
        
        /*
        std_msgs::String dis;
        std_msgs::String Smode;
        int imode;
        if (del_x_real > 1 || del_x_real < -1 || del_y_real > 1 || del_y_real < -1) {
            flag = 1;
            
            imode = 3;
            char Del_Dis[20];
            sprintf(Del_Dis, "%.2f@%.2fL", del_x_real, del_y_real);
            dis.data = Del_Dis;
            Distance_pub.publish(&dis);
            
        }
        else {
            flag = 2;
            imode = 4;
        }
        stringstream ss;
        ss << imode;
        Smode.data = ss.str();

        camera_pub.publish(Smode);

        recvMode = -1;
        tar_x = -1000;
        tar_y = -1000;
        */
        cv::waitKey(30);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }

}

void FindCircle(const sensor_msgs::ImageConstPtr& msg)
{
    if (recvMode != 2) return;

    try
    {
        // real_img.copyTo(gray_img);
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");

        cv::Mat real_img = cv_ptr->image;
        cv::Mat gray_img2;
        cv::cvtColor(real_img, gray_img2, cv::COLOR_BGR2GRAY);

        //cv::medianBlur(gray_img,gray_img2,5);
        medianBlur(gray_img2, gray_img2, 5); // ������ ����. ���� ����ϰ� ����
        vector<cv::Vec3f> circles; // ã�� �� �߽� ��ǥ�� �������� ������ ��ŭ ���� �׷��ֱ� ���� vector

        cv::HoughCircles(gray_img2, circles, cv::HOUGH_GRADIENT, 1, 150, 80, 70, 0, 100);// ������ ã�� ���� �Լ�. ������ �ǹ̴� ���� ��ũ Ȯ��.
        //if (circles.size() > 0) flag = 1;
        for (size_t i = 0; i < circles.size(); i++) {
            cv::Vec3i c = circles[i];
            cv::Point center(c[0], c[1]);
            tar_x = c[0]; // ã�� target�� ��ǥ�� ���������� �����Ͽ� depth�� ���
            tar_y = c[1]; // ã�� target�� ��ǥ�� ���������� �����Ͽ� depth�� ���
            del_x_image = c[0] - 320; // del_x del_y�� ���Ͽ� ���� ���ϴµ� ���
            del_y_image = 240 - c[1]; // del_x del_y�� ���Ͽ� ���� ���ϴµ� ���
            cv::circle(real_img, cv::Point(c[0], c[1]), c[2], CV_RGB(255, 0, 0));
            //ROS_INFO("x_circle : %d   y_circle : %d", c[0], c[1]);

                        // del_x, del_y�� ���� �� ��и����� �ľ��ϰ� ���� �Ի�.
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

void ModeCallback(const std_msgs::String::ConstPtr& msg)
{
    std::stringstream stream;
    stream.str(msg->data.c_str());
    stream >> recvMode;

    ROS_INFO("recvMode: [%d]", recvMode);
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_listener2");
    ros::NodeHandle nh;
    //cv::namedWindow("view2");

    image_transport::ImageTransport it(nh);
    ros::Publisher camera_pub = nh.advertise<std_msgs::String>("Camera", 1000);
    ros::Publisher Angle_pub = nh.advertise<std_msgs::String>("angle", 1000);
    ros::Subscriber mode_sub = nh.subscribe("mode", 1000, ModeCallback);
    image_transport::Subscriber sub1 = it.subscribe("/camera/aligned_depth_to_color/image_raw", 1, GetDepth);
    image_transport::Subscriber sub2 = it.subscribe("/camera/color/image_raw", 1, FindCircle);

    //ros::spin();
    //cv::destroyWindow("view2");
    
    ros::Rate loop_rate(1);
    std_msgs::String dis;
    std_msgs::String Smode;
    int imode;
    
    while (ros::ok())
    {

        if (flag <= 0) {  // Angle ����� ��� ������ �ʾҴٸ� loop.
            ros::spinOnce();
            loop_rate.sleep();
            continue;
        }

        if (flag == 1) {  // Angle ����� �����µ�, abs(Delta_X) > 1 �� ���, �� x�� ����ȭ ����
            recvMode = -1;
            flag = -1;
            imode = 3;
        }  
        else if (flag == 2) {   // Angle ����� �����µ�, abs(Delta_X) < 1 �� ���, �� y�� ����ȭ ����
            recvMode = -1;
            flag = -1;
            imode = 4;
        }
        
        stringstream ss;
        ss << imode;
        Smode.data = ss.str();
        camera_pub.publish(Smode);

        sleep(3);   // mode topic pub�ϰ� ���� �ð� �Ŀ� angle pub�ϵ��� ����.
                    // �̷��� �ؾ� ���̳��ͼ��� mode sub�ϰ� �غ��� �ð��� �����.


        if (imode == 3) {
            int angle1, angle2;
            Calculate_x(del_x_real, &angle1, &angle2);
            char ANGLE12[20];
            //sprintf(Del_Dis, "%.2f@%.2fL", del_x_real, del_y_real);
            sprintf(ANGLE12, "%d@%dL", angle1, angle2);
            dis.data = ANGLE12;
            Angle_pub.publish(dis);

        }

        else if (imode == 4) {
            int angle3, angle4;
            Calculate_y(&angle3, &angle4);
            char ANGLE34[20];
            sprintf(ANGLE34, "%d@%dL", angle3, angle4);
            dis.data = ANGLE34;
            Angle_pub.publish(dis);
        }


        tar_x = -1000;
        tar_y = -1000;
        depth = 0;
        del_y = 0;

        ros::spinOnce();
        loop_rate.sleep();
    }
    
}