#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <mysql/mysql.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace std;

vector<vector<cv::Point>> contours;
vector<cv::Vec4i> hierachy;

static char *host = "localhost";
static char *user = "iot";
static char *pass = "kcci";
static char *dbname = "vehicle_info";

char device[] = "/dev/ttyACM0";
int fd;
unsigned long baud = 9600;
char number_back[4];
char capture[] = "sudo raspistill -o /home/kshksh/ksh_rasp/images/test1.jpg";

int find_number(cv::Mat num_image);


int main(int argc, char *argv[]){

    MYSQL *conn;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;
    conn = mysql_init(NULL);
    char in_sql[200] = {0};
    int sql_index, flag = 0;
    int res = 0 ;


    if(!(mysql_real_connect(conn,host,user,pass,dbname,0,NULL,0))){
        fprintf(stderr,"ERROR : %s[%d]\n",mysql_error(conn),mysql_errno(conn));
        exit(1);
    }
    else printf("Connection Successful!\n");

    printf("Raspberry Startup\n");
    fflush(stdout);
    fflush(stdin);
    
    if((fd = serialOpen(device, baud)) < 0 ){
        fprintf(stderr, "Unable to open %s\n ", strerror(errno));
        exit(1);
    }
    
    if(wiringPiSetup() == -1)
        return 1;

    while(1){
        if(serialDataAvail(fd)){
			char newChar = serialGetchar(fd);  //fd가 핸들러임.

            //if (1) {
            if(newChar == 'S'){
								// 이미지 처리 진행
                //system(capture);

                double ratio_d, delta_x, delta_y, gradient;
                int select, plate_width, plate_height, count, friend_count = 0, refinery_count = 0;

                cv::Mat origin_img = cv::imread(argv[1], 0);
                cv::Mat gray_img;

                origin_img.copyTo(gray_img);

                
                int original_width, original_height;

                original_width = origin_img.cols;
                original_height = origin_img.rows;
                
                if (gray_img.empty()) {
                    printf("Image load fail\n");
                    continue;
                }

                cv::equalizeHist(gray_img, gray_img);
                medianBlur(gray_img, gray_img, 3);
          
                cv::Mat Canny_img;

                cv::Canny(gray_img, Canny_img, 50, 150, 3);

                cv::findContours(Canny_img, contours, hierachy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point());
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
                        //cv::drawContours(real_img, contours, idx, cv::Scalar(0, 255, 255), 1, 8, hierachy, 0, cv::Point());
                        //cv::rectangle(real_img, boundRect[idx].tl(), boundRect[idx].br(), cv::Scalar(255, 0, 0), 1, 8, 0);
                        refinery_count += 1;
                        boundRect2[refinery_count] = boundRect[idx];
                    }
                }

                boundRect2.resize(refinery_count);

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

                    cv::rectangle(gray_img, boundRect2[i].tl(), boundRect2[i].br(), cv::Scalar(0, 0, 0), 1, 8, 0);

                    count = 0;

                    //  Snake moves to right, for eating his freind.
                    for (int j = i + 1; j < boundRect2.size(); j++) {

                        delta_x = abs(boundRect2[j].tl().x - boundRect2[i].tl().x);

                        if (delta_x > 200)  //  Can't eat snake friend too far ^-^.
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
                        //cv::rectangle(gray_img, boundRect2[select].tl(), boundRect2[select].br(), cv::Scalar(255, 255, 255), 1, 8, 0);
                        //plate_width = delta_x;  //  Save the last friend ate position.
                    }                           //  It's similar to license plate width, Right?
                }

                

                vector<cv::Rect> carNumber; // Space for real car numbers and letter
                count = 1;

                carNumber.push_back(boundRect2[select]);
                //rectangle(image4, boundRect2[select].tl(), boundRect2[select].br(), Scalar(0, 255, 0), 1, 8, 0);

                for (int i = 0; i < boundRect2.size(); i++) {
                    if (boundRect2[select].tl().x > boundRect2[i].tl().x)   // The rest of the car plate numbers are on the right side of the first number
                        continue;

                    delta_x = abs(boundRect2[select].tl().x - boundRect2[i].tl().x);

                    if (delta_x > 100)   // Car numbers are close to each other
                        continue;

                    delta_y = abs(boundRect2[select].tl().y - boundRect2[i].tl().y);

                    if (delta_x == 0) {
                        delta_x = 1;
                    }

                    if (delta_y == 0) {
                        delta_y = 1;
                    }

                    gradient = delta_y / delta_x;  // Get gradient

                    if (gradient < 0.25) {
                        select = i;
                        carNumber.push_back(boundRect2[i]);
                        rectangle(gray_img, boundRect2[i].tl(), boundRect2[i].br(), cv::Scalar(255, 255,255), 1, 8, 0);
                        count += 1;
                    }
                }



                //=============================
                // Rotation Image for improved Recognize OCR
                //==============================
                cv::Mat Rotated_image;
                cv::Mat cropped_image;
                origin_img.copyTo(Rotated_image);
                cv::Point center1 = (carNumber[0].tl() + carNumber[0].br()) * 0.5;  // Center of the first number
                cv::Point center2 = (carNumber[carNumber.size() - 1].tl() + carNumber[carNumber.size() - 1].br()) * 0.5;  // Center of the last number
                int plate_center_x = (int)(center1.x + center2.x) * 0.5;    // X-coordinate at the Center of car plate
                int plate_center_y = (int)(center1.y + center2.y) * 0.5;    // Y-coordinate at the Center of car plate

                // To calculate the height
                int sum_height = 0;
                for (int i = 0; i < carNumber.size(); i++)
                    sum_height += carNumber[i].height;

                plate_width = (-center1.x + center2.x + carNumber[carNumber.size() - 1].width) * 1.05;  // Car plate width with some paddings
                plate_height = (int)(sum_height / carNumber.size()) * 1.2;  // Car plate height with some paddings

                delta_x = center1.x - center2.x;
                delta_y = center1.y - center2.y;

                // Roatate car plate
                double angle_degree = (atan(delta_y / delta_x)) * (double)(180 / 3.141592);

                printf("angle_degree %lf\n", angle_degree);

                cv::Mat rotation_matrix = cv::getRotationMatrix2D(cv::Point(plate_center_x, plate_center_y), angle_degree, 1.0);
                cv::warpAffine(Rotated_image, Rotated_image, rotation_matrix, cv::Size(original_width, original_height));            
                cv::imshow("Rotated", Rotated_image);

                cv::getRectSubPix(Rotated_image, cv::Size(plate_width, plate_height), cv::Point(plate_center_x, plate_center_y), cropped_image, -1);
                cv::imshow("Cropped", cropped_image);



                //=============================
                // Filtered Cropped image
                //==============================
                cv::GaussianBlur(cropped_image, cropped_image, cv::Size(5, 5), 0);
                cv::adaptiveThreshold(cropped_image, cropped_image, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 19, 9);
                //threshold(cropped_image, cropped_image, 127, 255, 0);

                cv::Mat Merge_crop[] = { cropped_image,cropped_image,cropped_image };
                cv::Mat new_croped;
                cv::merge(Merge_crop, 3, new_croped);
                cv::imshow("Before mor", new_croped);  // Save the result

                //=============================
                // Morphology Start
                //==============================

                cv::Mat img_erode;
                //cv::erode(test_thresh, img_erode, cv::Mat());
                cv::erode(cropped_image, img_erode, cv::Mat::ones(cv::Size(3, 3), CV_8UC1), cv::Point(-1, -1), 1);

                cv::Mat Merge_erode[] = { img_erode,img_erode,img_erode };
                cv::Mat erode_merge;
                cv::merge(Merge_erode, 3, erode_merge);
                // cv::imshow("After erode", erode_merge);

                cv::Mat img_dilate;
                cv::dilate(img_erode, img_dilate, cv::Mat(), cv::Point(-1, -1), 1);
                cv::Mat Merge_dilate[] = { img_dilate,img_dilate,img_dilate };
                cv::Mat dilate_merge;
                cv::merge(Merge_dilate, 3, dilate_merge);

                cv::Mat img_erode2;
                //cv::erode(test_thresh, img_erode, cv::Mat());
                cv::erode(img_dilate, img_erode2, cv::Mat::ones(cv::Size(3, 3), CV_8UC1), cv::Point(-1, -1), 1);

                cv::Mat Merge_erode2[] = { img_erode2,img_erode2,img_erode2 };
                cv::Mat erode_merge2;
                cv::merge(Merge_erode2, 3, erode_merge2);

                cv::imshow("After Mop", erode_merge2);
              
                //=============================
                // OCR Start
                //==============================

                int OK_signal = find_number(erode_merge2);
                if (OK_signal == 2) {
                    printf("success OCR\n");
                }
                else {
                    printf("fail OCR\n");
                }
                //cv::imshow("hi", gray_img);

                printf("Search Start\n");
                //=============================
                // Search type
                //==============================
                int type=-1;
                char temp[100];
                sprintf(temp, "select * from car_type");
                
                int res = mysql_query(conn, temp);
                if (!res) {
                    res_ptr = mysql_store_result(conn);
                    if (res_ptr) {
                        printf("Retrived %1u rows\n", (unsigned long)mysql_num_rows(res_ptr));
                        while ((sqlrow = mysql_fetch_row(res_ptr))) {
                            printf("%10s %10s\n", sqlrow[0], sqlrow[1]);
                            if (atoi(sqlrow[0]) == atoi(number_back)) {
                                type = atoi(sqlrow[1]);
                                break;
                            }
                        }                        
                    }
                }
                else {
                    printf("Search Type Fail\n");
                }

                switch (type) {
                case -1:
                    printf("등록안된 차량 or 조회 실패\n");
                    break;
                case 1:
                    printf("전기차 A 타입\n");
                    break;
                case 2:
                    printf("전기차 B 타입\n");
                    break;
                case 3:
                    printf("전기차 C 타입\n");
                    break;

                default:
                    printf("있을 수 없는 경우\n");
                    break;
                }


                //=============================
                // Search Spot
                //==============================

                char rest_spot[10] = "";

                memset(temp, 0, sizeof(temp));
                sprintf(temp, "select * from now_stat");

                res = mysql_query(conn, temp);
                if (!res) {
                    res_ptr = mysql_store_result(conn);
                    if (res_ptr) {
                        printf("Retrived %1u rows\n", (unsigned long)mysql_num_rows(res_ptr));
                        while ((sqlrow = mysql_fetch_row(res_ptr))) {
                            printf("%10s %10s\n", sqlrow[0], sqlrow[1]);
                            if (atoi(sqlrow[1]) == 0) {
                                strcpy(rest_spot, sqlrow[0]);
                                break;
                            }
                        }
                    }
                }
                else {
                    printf("Search Spot Fail\n");
                }

                if (rest_spot != "") {
                    printf("주차는 %s에!\n", rest_spot);
                }

                else {
                    printf("주차할 곳 없엉 .. \n");
                }


                //=============================
                // Update Spot
                //==============================

                memset(temp, 0, sizeof(temp));
                sprintf(temp, "update now_stat set STATUS = 1, CARNUM = %d where SPOT = '%s'",atoi(number_back),rest_spot);

                res = mysql_query(conn, temp);
                if (!res) {
                    printf("update OK\n");
                }
                else {
                    printf("Update Spot Fail\n");
                }



                //=============================
                // Insert Log
                //==============================

                memset(temp, 0, sizeof(temp));
                sprintf(temp, "insert into parkinglot_log(ID,CARNUM,SPOT) values (null,'%s','%s')", number_back, rest_spot);
                
                res = mysql_query(conn, temp);
                if (!res)
                    printf("inserted %1u rows\n", (unsigned long)mysql_affected_rows(conn));
                else
                    fprintf(stderr, "Insert Error %d: %s\n", mysql_errno(conn), mysql_error(conn));


                //=============================
                // Send Msg to Arduino
                //==============================
                
                char send_msg[40] = "";
                sprintf(send_msg, "%s@%d@%sL", number_back, type, rest_spot);
                int i = 0;
                printf("sending start\n");
                while (send_msg[i] != '\0') {
                    serialPutchar(fd, send_msg[i++]);
                    delay(10);
                }
                printf("sending end\n");


                //cv::waitKey();
                

                /*
                if(flag == 1){

                    sprintf(in_sql,"insert into sensing(ID,DATE,TIME,MOISTURE,TEMPERATURE) values (null,curdate(),curtime(),%d,%d)", state1,state2);
                    res = mysql_query(conn,in_sql);
                    printf("res : %d\n",res);
                    if(!res)
                        printf("inserted %1u rows\n",(unsigned long)mysql_affected_rows(conn));
                    else
                        fprintf(stderr,"Insert Error %d : %s\n",mysql_errno(conn),mysql_error(conn));
                }
                */
            }
            
            fflush(stdout);
        }
    }
    mysql_close(conn);
    return 0;
}


int find_number(cv::Mat num_image) {
    //cv::Mat im = cv::imread("new_image.png", cv::IMREAD_COLOR);
    cv::Mat im = num_image;

    cv::resize(im, im, cv::Size(im.cols * 2, im.rows * 2), 0, 0, 1);
    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();

    if (api->Init(NULL, "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
    api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    //api->SetPageSegMode(tesseract::PSM_AUTO);
    //api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
    api->SetImage(im.data, im.cols, im.rows, 3, im.step);
    string outText = string(api->GetUTF8Text());
    std::cout << "OutText = " << outText << std::endl;
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

                    std::cout << outText[idx] << outText[idx + 1] << outText[idx + 2] << outText[idx + 3] << std::endl;
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
