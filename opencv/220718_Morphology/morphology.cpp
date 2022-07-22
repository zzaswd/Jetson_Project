#define NOMINMAX

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
//#include <sensor_msgs/image_encoding.h>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <vector>
#include <std_msgs/String.h>

#define GRADIENT_THRESHOLD 0.1
#define MIN_RECT_AREA 300

using namespace std;
int find_number(cv::Mat num_image);
cv::Rect rect, temp_rect;
vector<vector<cv::Point>> contours;
vector<cv::Vec4i> hierachy;

int flag = 0;
int mode = 0;
int recvSensor;
int OK_signal = 0;
char number_back[4];


void stepCallback(const std_msgs::String::ConstPtr& msg)
{
    std::stringstream stream;
    stream.str(msg->data.c_str());
    stream >> recvSensor;
    if (recvSensor == 2) {
        if (mode == 0) {
            mode = recvSensor - 1;
            ROS_INFO("Starting... Find..Number.. ");
        }
    }
    else if (recvSensor == 3) {
        if (mode == 2) {
            mode = 0;
            ROS_INFO("Finish_Find_Mode === recvSensor: [%d]", recvSensor);
        }
    }
    stream.str("");
}


void findNumberCallback(const sensor_msgs::ImageConstPtr& msg)
{
    if (mode == 1) {
        try
        {
            double ratio_d, delta_x, delta_y, gradient;
            int select, plate_width, count, friend_count = 0, refinery_count = 0;
            cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
            cv::Mat origin_img;
            cv::Mat real_img = cv_ptr->image;
            real_img.copyTo(origin_img);
            cv::Mat gray_img;
            cv::Mat test;

            cv::Mat image1, ROI_img;
            cv::cvtColor(real_img, gray_img, cv::COLOR_RGB2GRAY);
            cv::Canny(gray_img, image1, 50, 150, 3);

            cv::Mat Merge_img[] = { image1,image1,image1 };

            cv::findContours(image1, contours, hierachy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point());
            vector<vector<cv::Point>> contours_poly(contours.size());
            vector<cv::Rect> boundRect(contours.size());
            vector<cv::Rect> boundRect2(contours.size());

            for (int idx = 0; idx < contours.size(); idx++) {
                approxPolyDP(cv::Mat(contours[idx]), contours_poly[idx], 1, true);
                boundRect[idx] = boundingRect(cv::Mat(contours_poly[idx]));
            }

            for (int idx = 0; idx < contours.size(); idx++) {
                ratio_d = (double)(boundRect[idx].height / boundRect[idx].width);

                if ((ratio_d <= 2.5) && (ratio_d >= 0.5) && (boundRect[idx].area() <= 2000) && (boundRect[idx].area() >= 200)) {
                    refinery_count += 1;
                    boundRect2[refinery_count] = boundRect[idx];
                }
            }

            boundRect2.resize(refinery_count);
            cv::imshow("hi2", real_img);

            // 버블정렬
            for (int i = 0; i < boundRect2.size(); i++) {
                for (int j = 0; j < boundRect2.size() - i; j++) {
                    if (boundRect2[j].tl().x > boundRect2[j + 1].tl().x) {
                        cv::Rect tempRect = boundRect2[j];
                        boundRect2[j] = boundRect2[j + 1];
                        boundRect2[j + 1] = tempRect;
                    }
                }
            }

            for (int i = 0; i < boundRect2.size(); i++) {
                cv::rectangle(real_img, boundRect2[i].tl(), boundRect2[i].br(), cv::Scalar(0, 255, 0), 1, 8, 0);

                count = 0;

                //  Snake moves to right, for eating his freind.
                for (int j = i + 1; j < boundRect2.size(); j++) {

                    delta_x = abs(boundRect2[j].tl().x - boundRect2[i].tl().x);

                    if (delta_x > 300)  //  Can't eat snake friend too far ^-^.
                        break;
                    delta_y = abs(boundRect2[j].tl().y - boundRect2[i].tl().y);

                    //  If delta length is 0, it causes a divide-by-zero error.
                    if (delta_x == 0) {
                        delta_x = 1;
                    }

                    if (delta_y == 0) {
                        delta_y = 1;
                    }

                    gradient = delta_y / delta_x;  //  Get gradient.
                    //cout << gradient << endl;

                    if (gradient < 0.25) {  //  Can eat friends only on straight line.
                        count += 1;
                    }
                }

                //  Find the most full snake.
                if (count > friend_count) {
                    select = i;  //  Save most full snake number.
                    friend_count = count;  //  Renewal number of friends hunting.
                    cv::rectangle(real_img, boundRect2[select].tl(), boundRect2[select].br(), cv::Scalar(255, 0, 0), 1, 8, 0);
                    plate_width = delta_x;  //  Save the last friend ate position.
                }                           //  It's similar to license plate width, Right?
            }

            if (friend_count >= 6) {
                cv::rectangle(real_img, boundRect2[select].tl(), boundRect2[select].br(), cv::Scalar(0, 0, 255), 2, 8, 0);
                cv::line(real_img, boundRect2[select].tl(), cv::Point(boundRect2[select].tl().x + plate_width, boundRect2[select].tl().y), cv::Scalar(0, 0, 255), 1, 8, 0);

                if ((boundRect2[select].tl().y - 20 > 0) && (boundRect2[select].tl().x - 20 > 0) && (boundRect2[select].tl().y + 20 + plate_width * 0.3 < 480) && (boundRect2[select].tl().x + plate_width + 40 < 640)) {
                    ROI_img = origin_img(cv::Range(boundRect2[select].tl().y - 20, boundRect2[select].tl().y + plate_width * 0.3 + 20), cv::Range(boundRect2[select].tl().x - 20, boundRect2[select].tl().x + plate_width + 30));

                    ROI_img.copyTo(test);
                    cv::cvtColor(test, test, cv::COLOR_RGB2GRAY);
                    cv::GaussianBlur(test, test, cv::Size(5, 5), 0);
                    cv::Mat test_thresh;
                    //cv::threshold(test, test_thresh, 110, 255, cv::THRESH_BINARY);

                    cv::threshold(test, test_thresh, 110, 255, cv::THRESH_OTSU);
                    //cv::adaptiveThreshold(test, test_thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 17, 5);

                    //cv::imwrite("/home/kcci/catkin_ws/src/ros_realsense_opencv_tutorial/src/vehicle.png", ROI_img);
                    //cv::imwrite("/home/kcci/catkin_ws/src/ros_realsense_opencv_tutorial/src/new_image.png", new_merge);

                    cv::Mat test_erode;
                    cv::Mat test_dilate;;

                    //cv::adaptiveThreshold(test, test_thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 19, 9);
                    cv::Mat Merge_thresh[] = { test_thresh,test_thresh,test_thresh };
                    cv::Mat new_merge;
                    cv::merge(Merge_thresh, 3, new_merge);
                    cv::imshow("Before Anything", new_merge);

                    cv::Mat img_dilate;
                    //cv::erode(test_dilate, img_dilate, cv::Mat::ones(cv::Size(5, 5), CV_8UC1), cv::Point(-1, -1), 2);
                    //cv::erode(test_dilate, img_dilate, cv::Mat());
                    cv::dilate(test_thresh, img_dilate, cv::Mat(), cv::Point(-1, -1), 1);

                    cv::Mat Merge_dilate[] = { img_dilate,img_dilate,img_dilate };
                    cv::Mat dilate_merge;
                    cv::merge(Merge_dilate, 3, dilate_merge);
                    cv::imshow("After dilate", dilate_merge);


                    cv::Mat img_erode;
                    //cv::erode(test_erode, img_erode, cv::Mat());
                    //cv::erode(test_erode, img_erode, cv::Mat::ones(cv::Size(5,5),CV_8UC1),cv::Point(-1,-1),2);
                    cv::erode(img_dilate, img_erode, cv::Mat(), cv::Point(-1, -1), 1);


                    cv::Mat Merge_erode[] = { img_erode,img_erode,img_erode };
                    cv::Mat erode_merge;
                    cv::merge(Merge_erode, 3, erode_merge);
                    cv::imshow("After dilate,erode", erode_merge);



                    int OK_signal = find_number(erode_merge);
                    if (OK_signal == 2) {
                        mode = 2;
                    }
                }
            }

            cv::imshow("hi2", real_img);

            cv::waitKey(30);
        }
        catch (cv_bridge::Exception& e)
        {
            ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
        }
    }


}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "image_listener2");
    ros::NodeHandle nh;
    //cv::namedWindow("view2");
    ros::Publisher pub = nh.advertise<std_msgs::String>("camera_mode", 1000);

    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/camera/color/image_raw", 1, findNumberCallback);
    ros::Subscriber sub1 = nh.subscribe("step", 1000, stepCallback);

    ros::Rate loop_rate(1);

    std::stringstream ss1;
    std_msgs::String msg;

    while (ros::ok())
    {
        if (mode == 2) {
            ss1 << mode;
            msg.data = ss1.str();
            ROS_INFO("Finish Find_Number. : %s", msg.data.c_str());
            pub.publish(msg);

            ss1.str("");
        }
        ros::spinOnce();
        loop_rate.sleep();
    }

    //cv::destroyWindow("view2");
}

int find_number(cv::Mat num_image) {
    //cv::Mat im = cv::imread("new_image.png", cv::IMREAD_COLOR);
    cv::Mat im = num_image;

    cv::resize(im, im, cv::Size(im.cols * 2, im.rows * 2), 0, 0, 1);
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();

    if (api->Init(NULL, "kor")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    //api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    api->SetPageSegMode(tesseract::PSM_AUTO);
    //api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
    api->SetImage(im.data, im.cols, im.rows, 3, im.step);
    string outText = string(api->GetUTF8Text());

    int flag = 0;

    for (int idx = 0; idx < outText.size(); idx++) {
        if (flag == 1) {
            if (outText.size() > idx + 3) {
                if ((outText[idx] >= '0' && outText[idx] <= '9') && (outText[idx + 1] >= '0' && outText[idx + 1] <= '9') && (outText[idx + 2] >= '0' && outText[idx + 2] <= '9') && (outText[idx + 3] >= '0' && outText[idx + 3] <= '9')) {
                    flag = 2;
                    //printf("Success find number\n");
                    number_back[0] = outText[idx];
                    number_back[1] = outText[idx + 1];
                    number_back[2] = outText[idx + 2];
                    number_back[3] = outText[idx + 3];

                    std::cout << outText << "\n";
                    //std::cout << outText[idx] << outText[idx + 1] << outText[idx + 2] << outText[idx + 3] << std::endl;
                }
                else {
                    flag = 3;
                    //printf("Fail find number\n");
                }
            }
            else {
                //printf("Fail find number\n");
                flag = 3;
            }

            break;
        }

        if (outText[idx] == 32) {
            //printf("find space\n");
            flag = 1;
        }
    }
    api->End();
    delete api;

    return flag;
}