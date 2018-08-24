#include <iostream>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ServerClass.h"
#include <ros/ros.h>
#include <std_msgs/String.h>
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
ros::Duration dur, dur_thres(0.018);
stringstream ss;


int main(int argc, char **argv)
{

    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);

    std_msgs::String ins_msgs;
    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }
    //Ros related calls
    ros::init(argc, argv, "udpcom_node");
    ros::NodeHandle nh;
    //IMU string publisher
    ros::Publisher ins_pub = nh.advertise<std_msgs::String>("/udpcom_node/ins_msg", 10);
       
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
	ss << buffer;
	
	if(recvflag)
	{
	    //ins_msgs.header.stamp = ros::Time::now();
	    ins_msgs.data = ss.str();
	    ins_pub.publish(ins_msgs);

	}
	else
	{ 
	    cout << "Buffer not received";
	}

        if(ctrl_c_pressed)
        {
            cout << "Ctrl^C Pressed" << endl;
            udp_obj.~Server_UDP();
            break;

         }
         rate.sleep();
	 ros::spinOnce();
	 ss.str(std::string());
	 if(dur > dur_thres)
	     cout << "Time period crosses the THRESHOLD -------------------------------------------------->>>>>: " << dur << endl;
    }
    cout << "Exiting....." << endl;
    return 0;
}

void sig_handler(int sig)
{
    write(0,"nCtrl^C pressed in sig handlern",32);
    ctrl_c_pressed = true;
}
