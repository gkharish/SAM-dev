#include <iostream>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "/home/ubuntu/catkin_ws/src/sam_ins/src/Rasberry_GPIOClass.h"
#include "ServerClass.h"
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <string>
#include <sstream>
using namespace std;
int cnt;
string cnt_str;
string file;
char buffer[1024];
void sig_handler(int sig);

bool ctrl_c_pressed = false;
ros::Time tnow, tprev;
ros::Duration dur;

//stringstream ss;
ofstream ccfile;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
  {
    cv_bridge::CvImageConstPtr cv_ptr;
    cv_ptr = cv_bridge::toCvShare(msg, "mono8");
//    cv::imshow("view", cv_ptr->image);
//    int key = cv::waitKey(30);

   
    
    cnt++;
    //cnt_str = to_string(cnt);
    stringstream ss;
    ss  << cnt;
    string file_name = "file" + ss.str() + ".png";
    file = "/home/ubuntu/uEyecap/" + file_name ;
    cv::imwrite(file, cv_ptr->image);
    std:: cout << "Image file saved" << std::endl;
    
    ccfile << file_name << ","<< buffer << std::endl;
    //toCC(file,buffer);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

int main(int argc, char **argv)
{

    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);
    ccfile.open("ContextCapture.txt");

    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }
    //Ros related calls
    ros::init(argc, argv, "image_listener");
    ros::NodeHandle nh;
//    cv::namedWindow("view");
//    cv::startWindowThread();
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("ueye_mono/image_raw", 1, imageCallback);

    string inputstate;
    // Rasberry GPIO related calls
    GPIOClass* gpio4 = new GPIOClass("17");
    //GPIOClass* gpio17 = new GPIOClass("17");

    gpio4->export_gpio();
    //gpio17->export_gpio();

    cout << " GPIO pins exported" << endl;

    //gpio17->setdir_gpio("in");
    gpio4->setdir_gpio("out");

    cout << " Set GPIO pin directions" << endl;
    
    // UDP socket realted
    //char buffer[1024];
    Server_UDP udp_obj;
    bool bflag = udp_obj.Server_start();
    bool recvflag = true;
    ros::Rate rate(100);
    tprev = ros::Time::now();
    while(ros::ok())
    {
        tnow = ros::Time::now();
	dur = tnow - tprev;
	tprev = tnow;

	recvflag = udp_obj.Server_recv(buffer, 1024);
	if(recvflag)
	{
	    gpio4->setval_gpio("1");
	    gpio4->getval_gpio(inputstate);
	    cout << "Set GPIO$ to 1: " << inputstate << endl;
	    usleep(2.5e3);

	    gpio4->setval_gpio("0");
	    gpio4->getval_gpio(inputstate);
	    cout << "Set GPIO4 to 0: " << inputstate  <<endl;
	    usleep(2.5e3);
            //gpio4->setval_gpio("0");


	}
	else
	{ 
	    gpio4->setval_gpio("0");
	    cout << "No recv, Setting GPIO4 to 0";
	}

        if(ctrl_c_pressed)
        {
            cout << "Ctrl^C Pressed" << endl;
            cout << "unexporting pins" << endl;
            gpio4->unexport_gpio();
            //gpio17->unexport_gpio();
            cout << "deallocating GPIO Objects" << endl;
            delete gpio4;
            gpio4 = 0;
            //delete gpio17;
            //gpio17 =0;
		
            //cv::destroyWindow("view");
            udp_obj.~Server_UDP();
	    ccfile.close();
            break;

         }
         rate.sleep();
	 ros::spinOnce();
	 cout << "Time period: " << dur << endl;
    }
    gpio4->setval_gpio("0");
    gpio4->getval_gpio(inputstate);
    cout << "Input state state: " << inputstate << endl;
    cout << "Exiting....." << endl;
    return 0;
}

void sig_handler(int sig)
{
    write(0,"nCtrl^C pressed in sig handlern",32);
    ctrl_c_pressed = true;
}
