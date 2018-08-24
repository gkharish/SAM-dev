#include <iostream>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "Rasberry_GPIOClass.h"
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <string>
#include <sstream>
using namespace std;
int cnt;
string cnt_str;
string file;
char buffer[1024];
void sig_handler(int sig);

bool ctrl_c_pressed = false;
bool trig_flag = false;
ros::Time tnow, tprev;
ros::Duration dur;
//stringstream ss;
string inputstate;
// Rasberry GPIO related calls
GPIOClass* gpio17 = new GPIOClass("17");
//GPIOClass* gpio17 = new GPIOClass("17");
void trigGPIO_CB(const std_msgs::Bool::ConstPtr& msg)
{
    trig_flag = msg->data;
    if(trig_flag)
    {
        gpio17->setval_gpio("1");
        gpio17->getval_gpio(inputstate);
        cout << "Set GPIO17 to 1: " << inputstate << endl;
        usleep(2.5e3);
        gpio17->setval_gpio("0");
        gpio17->getval_gpio(inputstate);
        cout << "Set GPIO17 to 0: " << inputstate  <<endl;
        usleep(2.5e3);
        //gpio17->setval_gpio("0");
    }
    else
        gpio17->setval_gpio("0");
}

int main(int argc, char **argv)
{

    struct sigaction sig_struct;
    sig_struct.sa_handler = sig_handler;
    sig_struct.sa_flags = 0;
    sigemptyset(&sig_struct.sa_mask);

    if (sigaction(SIGINT, &sig_struct, NULL) == -1) {
        cout << "Problem with sigaction" << endl;
        exit(1);
    }

    gpio17->export_gpio();
    //gpio17->export_gpio();

    cout << " GPIO pins exported" << endl;

    //gpio17->setdir_gpio("in");
    gpio17->setdir_gpio("out");

    cout << " Set GPIO pin directions" << endl;
 

    //Ros related calls
    ros::init(argc, argv, "raspiGPIO_node");
    ros::NodeHandle nh;
   
    ros::Subscriber trig_sub = nh.subscribe("/user_trigGPIO",1000, trigGPIO_CB);
    bool recvflag = true;
    //ros::Rate rate(100);
    tprev = ros::Time::now();
    /*while(ros::ok())
    {
        tnow = ros::Time::now();
	dur = tnow - tprev;
	tprev = tnow;

        gpio17->setval_gpio("1");
        gpio17->getval_gpio(inputstate);
        cout << "Set GPIO$ to 1: " << inputstate << endl;
        usleep(2.5e3);
        gpio17->setval_gpio("0");
        gpio17->getval_gpio(inputstate);
	cout << "Set GPIO4 to 0: " << inputstate  <<endl;
	usleep(2.5e3);
            //gpio17->setval_gpio("0");

        if(ctrl_c_pressed)
        {
            cout << "Ctrl^C Pressed" << endl;
            cout << "unexporting pins" << endl;
            gpio17->unexport_gpio();
            //gpio17->unexport_gpio();
            cout << "deallocating GPIO Objects" << endl;
            delete gpio17;
            gpio17 = 0;
            //delete gpio17;
            //gpio17 =0;
		
            break;

         }
	 ros::spinOnce();
	 cout << "Time period: " << dur << endl;
    }*/
    ros::spin();
    gpio17->setval_gpio("0");
    gpio17->getval_gpio(inputstate);
    cout << "Input state state: " << inputstate << endl;
    cout << "Exiting....." << endl;
    return 0;
}

void sig_handler(int sig)
{
    write(0,"nCtrl^C pressed in sig handlern",32);
    ctrl_c_pressed = true;
}
