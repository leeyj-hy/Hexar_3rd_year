//Based on Bicycle Model

#include "serial.hpp"
#include <unistd.h>
#include <iostream>
#include <string>
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "crcModbus.h"

#define RAD2DEG 57.2958
#define _PI 3.141592

#define G_RATIO 11
#define WHEELBASE 1

#define STEER_MAX 1.5708
#define STEER_MIN -1.5708

#define RPM_CONST 6.44

using namespace std;
using namespace ros;

uint8_t buf[6];
uint8_t buf_tmp[4];
unsigned short crc;
Serial driver("/dev/ttyACM0", 115200);

union tmp_data
{
    uint16_t uint16_vel;
    struct
    {
        uint8_t upint;
        uint8_t dnint;
    };

};

struct cmd
    {
      int lin_vel;
      double ang_vel;
    };

class Converter
{
  private:
    NodeHandle n;
    Subscriber MsgSub;
    Publisher MsgPub;

    cmd cmd_obj;
    cmd cmd_kin;
    geometry_msgs::Twist tmp_pub;

    
    
  public:
    Converter()
      {
        MsgSub = n.subscribe("/cmd_vel", 1, &Converter::CmdReqCallback, this);
        MsgPub = n.advertise<geometry_msgs::Twist>("/CurrentValRet", 10);
      }
    geometry_msgs::Twist cmd_vel;

    void CmdReqCallback(const geometry_msgs::Twist &vel_msg);
    void CurrentValPub();
    
    void KinematicModel(cmd cmd_input);
};

inline void Converter::KinematicModel(cmd cmd_input)
{
  
  double gamma = atan(cmd_input.ang_vel * WHEELBASE / cmd_input.lin_vel);
  ROS_INFO("%f", gamma);
  
  if(cmd_input.lin_vel < 0)
  {
    cmd_obj.ang_vel = -1 * G_RATIO * gamma;
  }

  else
  {
    cmd_obj.ang_vel =  G_RATIO * gamma;
  }
  

  cmd_obj.lin_vel = cmd_input.lin_vel;

  Converter::CurrentValPub();

}
inline void Converter::CmdReqCallback(const geometry_msgs::Twist &vel_msg)
{
  cmd_kin.lin_vel = vel_msg.linear.x;
  cmd_kin.ang_vel = vel_msg.angular.z;
  ROS_INFO("lin %d , ang %f", cmd_kin.lin_vel, cmd_kin.ang_vel);

  Converter::KinematicModel(cmd_kin);
}

inline void Converter::CurrentValPub()
{
  tmp_data linear;
  tmp_data angular;

  linear.uint16_vel = cmd_obj.lin_vel;
  angular.uint16_vel = int(cmd_obj.ang_vel * RAD2DEG);
  int tmp_data = int(cmd_obj.ang_vel * RAD2DEG);
  tmp_pub.linear.x = cmd_obj.lin_vel;
  tmp_pub.angular.z = int(cmd_obj.ang_vel * RAD2DEG);

  // memcpy(&buf[2], &cmd_obj.lin_vel, 2);
  // memcpy(&buf[4], &tmp_data, 2);
  buf[0] = 0xFF;
  buf[1] = 0xFF;
  buf[2] = cmd_obj.lin_vel;
  buf[3] = cmd_obj.lin_vel >> 8;
  buf[4] = tmp_data;
  buf[5] = tmp_data >> 8;

  crc = crc_modbus(&buf[2], 4);
  memcpy(&buf[6], (char*)&crc, 2);
  // buf[6] = tmp_data < 0 ? 0xFF : 0x00;
  // buf[7] = tmp_data < 0 ? 0xFF : 0x00;

  if(write(driver.serial_port, buf, 8)>0)
  {
    ROS_INFO("Linear SPD : %d \t Steer ANGL : %d", linear.uint16_vel, tmp_data);
    MsgPub.publish(tmp_pub);
  }

  else
  ROS_WARN("Serial Port Write Error! Please Check Connection");
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "msg2uart_conv_node");
  Converter CVT_obj;
  // ros::Rate loop_rate(1);

  ros::spin();

  return 0;
}