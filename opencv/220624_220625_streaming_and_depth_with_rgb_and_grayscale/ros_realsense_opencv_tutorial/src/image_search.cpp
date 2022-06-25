#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
//#include <sensor_msgs/image_encoding.h>



void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
  {
    cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
    cv::Mat real_img = cv_ptr->image;
    cv::Mat gray_img= cv_ptr->image;


    for(int idx =0; idx < 480; idx++){
	    for(int jdx = 0; jdx <640; jdx++){
		    int intensity = (int)(0.333*gray_img.data[idx*640*3 + jdx*3 +0]+
				    0.333*gray_img.data[idx*640*3 + jdx*3 +1]+
				    0.333*gray_img.data[idx*640*3 + jdx*3 +2]);
		    for(int kdx =0; kdx <3 ; kdx++){
			    gray_img.data[idx*640*3+jdx*3+kdx] = intensity;
		    }
	    }
    }
			

/*

    for(int i =0 ; i<921600;i++){
	    if(i%3==0)
		    gray_img.data[i]=gray_img.data[i]*0.299;
	    else if(i%3==1)
                    gray_img.data[i]=gray_img.data[i]*0.587;
	    else if(i%3==2)
                    gray_img.data[i]=gray_img.data[i]*0.114;

    }
    */

/*    if(real_img.channels()==4){
	cv::cvtColor(real_img,grayscale_img,CV_BGRA2GRAY);
   }

    else if(real_img.channels()==3){
        cv::cvtColor(real_img,grayscale_img,CV_BGR2GRAY);
    }

    else if(real_img.channels()==2){
        cv::cvtColor(real_img,grayscale_img,CV_BGR5652GRAY);
    }

    else {
        grayscale_img = real_img;
    }

*/




//    cv::cvtColor(cv_ptr->image,grayscale_img,cv::COLOR_BGR2GRAY);
    
    cv::imshow("view2", real_img);
    cv::imshow("gray",gray_img);
    cv::waitKey(30);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "image_listener2");
  ros::NodeHandle nh;
  cv::namedWindow("view2");
 
  image_transport::ImageTransport it(nh);
  image_transport::Subscriber sub = it.subscribe("/camera/color/image_raw", 1, imageCallback);
  ros::spin();
  cv::destroyWindow("view2");
}
