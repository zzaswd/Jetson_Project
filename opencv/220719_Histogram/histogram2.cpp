#define NOMINMAX

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
//#include <sensor_msgs/image_encoding.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <std_msgs/String.h>
#include <sstream>

#define GRADIENT_THRESHOLD 0.1
#define MIN_RECT_AREA 300

using namespace std;

cv::Rect rect, temp_rect;
vector<vector<cv::Point>> contours;
vector<cv::Vec4i> hierachy;

int flag = 0;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
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

        cv::Mat image1, image2, drawing, ROI_img;
        cv::cvtColor(real_img, gray_img, cv::COLOR_RGB2GRAY);
        cv::Canny(gray_img, image1, 50, 150, 3);

        cv::Mat Merge_img[] = { image1,image1,image1 };
        cv::Mat new_thresh;
        cv::merge(Merge_img, 3, new_thresh);

        cv::imshow("image1", new_thresh);
        cv::findContours(image1, contours, hierachy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point());
        vector<vector<cv::Point>> contours_poly(contours.size());
        vector<cv::Rect> boundRect(contours.size());
        vector<cv::Rect> boundRect2(contours.size());

        for (int idx = 0; idx < contours.size(); idx++) {
            approxPolyDP(cv::Mat(contours[idx]), contours_poly[idx], 1, true);
            boundRect[idx] = boundingRect(cv::Mat(contours_poly[idx]));
        }

        //drawing = cv::Mat::zeros(image2.size(), CV_8UC3);

        for (int idx = 0; idx < contours.size(); idx++) {
            ratio_d = (double)(boundRect[idx].height / boundRect[idx].width);

            if ((ratio_d <= 2.5) && (ratio_d >= 0.5) && (boundRect[idx].area() <= 2000) && (boundRect[idx].area() >= 200)) {
                //cv::drawContours(real_img, contours, idx, cv::Scalar(0, 255, 255), 1, 8, hierachy, 0, cv::Point());
                //cv::rectangle(real_img, boundRect[idx].tl(), boundRect[idx].br(), cv::Scalar(255, 0, 0), 1, 8, 0);
                refinery_count += 1;
                boundRect2[refinery_count] = boundRect[idx];
            }
        }

        // cv::imshow("drawing", drawing);
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
                cout << gradient << endl;

                if (gradient < 0.15) {  //  Can eat friends only on straight line.
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

            //cv::rectangle(real_img,cv::Rect(boundRect2[select].tl().x - 20, boundRect2[select].tl().y - 20, plate_width + 40, plate_width * 0.3), cv::Scalar(0, 0, 255), 1, 8, 0);
            if ((boundRect2[select].tl().y - 20 > 0) && (boundRect2[select].tl().x - 20 > 0) && (boundRect2[select].tl().y + 20 + plate_width * 0.3 < 480) && (boundRect2[select].tl().x + plate_width + 40 < 640)) {
                ROI_img = origin_img(cv::Range(boundRect2[select].tl().y - 20, boundRect2[select].tl().y + 20 + plate_width * 0.3), cv::Range(boundRect2[select].tl().x - 20, boundRect2[select].tl().x + plate_width + 40));
                cv::imshow("hi3", ROI_img);
                ROI_img.copyTo(test);
                cv::cvtColor(test, test, cv::COLOR_RGB2GRAY);
                
                cv::Mat img_2 = test * 2;
                cv::Mat img_3 = test / 2;
                cv::Mat img_4;
                
                cv::equalizeHist(test, img_4);

                cv::Mat img_hist;


                cv::MatND histogram;

                const int* channel_numbers = { 0 };
                float channel_range[] = { 0.0,255.0 };
                const float* channel_ranges = channel_range;
                int number_bins = 255;

                cv::calcHist(&img_4, 1, channel_numbers, cv::Mat(), histogram, 1, &number_bins, &channel_ranges);
                int hist_w = test.cols;
                int hist_h = test.rows;
                int bin_w = cvRound((double)hist_w / number_bins);

                cv::Mat hist_img(hist_h, hist_w, CV_8UC1, cv::Scalar::all(0));
                cv::normalize(histogram, histogram, 0, hist_img.rows, cv::NORM_MINMAX, -1, cv::Mat());

                for (int i = 1; i < number_bins; i++) {
                    cv::line(hist_img, cv::Point(bin_w* (i - 1), hist_h - cvRound(histogram.at<float>(i - 1))), cv::Point(bin_w* (i), hist_h - cvRound(histogram.at<float>(i))), cv::Scalar(255, 0, 0), 1, 8, 0);
                }

                //cv::imshow(hist_img);
                
               // cv::Mat hist_img;
               // 
//                cv::equalizeHist(test, hist_img);


                /* ------- histogram 적용 이미지 확인용
                cv::Mat merge_test1[] = { hist_img,hist_img,hist_img };
                cv::Mat new_A;
                cv::merge(merge_test1, 3, new_A);
                cv::imshow("ROI Histo", new_A);

                cv::Mat merge_test2[] = { test,test,test };
                cv::Mat new_B;
                cv::merge(merge_test2, 3, new_B);
                cv::imshow("ROI Gray", new_B);
                */

                
                cv::GaussianBlur(test, test, cv::Size(5, 5), 0);
                // median blur 가 더 낫다. 선의 경계선 유지.
                cv::Mat test_thresh;
             //   cv::Mat hist_thresh;

                cv::threshold(test, test_thresh, 110, 255, cv::THRESH_BINARY);
             //   cv::threshold(hist_img, hist_thresh, 110, 255, cv::THRESH_BINARY);

                //cv::adaptiveThreshold(test, test_thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 19, 9);
                cv::Mat Merge_thresh[] = { test_thresh,test_thresh,test_thresh };
                cv::Mat new_merge;
                cv::merge(Merge_thresh, 3, new_merge);
                cv::imshow("Just Thresh", new_merge);

                //cv::adaptiveThreshold(test, test_thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 19, 9);
                cv::Mat Merge_Hist[] = { hist_img,hist_img,hist_img };
                cv::Mat hist_merge;
                cv::merge(Merge_Hist, 3, hist_merge);
                cv::imshow("Hist Thresh", hist_merge);



                cv::Mat img_erode;
                //cv::erode(test_thresh, img_erode, cv::Mat());
                cv::erode(test_thresh, img_erode, cv::Mat::ones(cv::Size(3, 3), CV_8UC1), cv::Point(-1, -1), 2);

                cv::Mat Merge_erode[] = { img_erode,img_erode,img_erode };
                cv::Mat erode_merge;
                cv::merge(Merge_erode, 3, erode_merge);
                // cv::imshow("After erode", erode_merge);

                cv::Mat img_dilate;
                cv::erode(test_thresh, img_dilate, cv::Mat());
                cv::Mat Merge_dilate[] = { img_dilate,img_dilate,img_dilate };
                cv::Mat dilate_merge;
                cv::merge(Merge_dilate, 3, dilate_merge);
                //cv::imshow("After dilate", dilate_merge);


                //cv::medianBlur(new_merge, new_merge, 5);

                /*
                cv::imshow("hi4", new_merge);
                if (flag == 0) {
                    cv::imwrite("vehicle.png", ROI_img);
                    flag = 1;
                    cv::imwrite("new_image.png", new_merge);
                }
                */

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