#include <iostream>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
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

string udp_buffer;
void udprecv_CB(const std_msgs::String::ConstPtr& msg)
{
    udp_buffer = msg->data;
    

}
bool toCC(string file_name, string buffer)
{
    ccfile << file_name << ","<< buffer << std::endl;
    cout << "csv file written" << endl;
    return 0;

}
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
  {
    cv_bridge::CvImageConstPtr cv_ptr;
    cv_ptr = cv_bridge::toCvShare(msg, "mono8");
//  cv::imshow("view", cv_ptr->image);
//  int key = cv::waitKey(30);

   
    
    cnt++;
    //cnt_str = to_string(cnt);
    stringstream ss;
    ss  << cnt;
    string file_name = "file" + ss.str() + ".png";
    file = "/home/ubuntu/uEyecap/" + file_name ;
    cv::imwrite(file, cv_ptr->image);
    std:: cout << "Image file saved" << std::endl;
    
    //ccfile << file_name << ","<< buffer << std::endl;
    toCC(file_name,udp_buffer);
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
    ccfile.open("/home/ubuntu/uEyecap/ContextCapture.txt");

    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }
    //Ros related calls
    ros::init(argc, argv, "save_alldata_node");
    ros::NodeHandle nh;
//    cv::namedWindow("view");
//    cv::startWindowThread();
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("ueye_mono/image_raw", 1, imageCallback);
    ros::Subscriber udp_sub = nh.subscribe("/udpcom_node/ins_msg",10, udprecv_CB);
    ros::Publisher triggpio_pub = nh.advertise<std_msgs::Bool>("/user_trigGPIO",1000);
    std_msgs::Bool send_trig;
    ros::Rate rate(100);
    tprev = ros::Time::now();
    ros::Time tinit = tprev;
    ros::Duration trig_durclean(0), trig_dur(0), trig_period(0.5);
    while(ros::ok())
    {
        tnow = ros::Time::now();
	dur = tnow - tprev;
	tprev = tnow;
	trig_dur = tnow - tinit;
	if(trig_dur >= trig_period)
	{

	    send_trig.data = true;
	    //publish trigger
	    triggpio_pub.publish(send_trig);
	    cout << "<-----------------------Sending trigger--------------------->" << endl;
	    trig_dur = trig_durclean;
	    tinit = tnow;
	}
	

        if(ctrl_c_pressed)
        {
            cout << "Ctrl^C Pressed" << endl;
            		
            //cv::destroyWindow("view");
	    ccfile.close();
            break;

         }
         rate.sleep();
	 ros::spinOnce();
	 cout << "Time period: " << dur << endl;
    }
    cout << "Exiting....." << endl;
    return 0;
}


void sig_handler(int sig)
{
    write(0,"nCtrl^C pressed in sig handlern",32);
    ctrl_c_pressed = true;
}
