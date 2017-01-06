#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <std_msgs/String.h>
#include <velodyne_msgs/VelodyneScan.h>

ros::Publisher pub_cmd;
int iCount_velo;
int iCount_imu;
bool b_sensor_check; // check if topic is actived
bool b_record;// get record msg or not
bool bVeloUP;
bool bIMU_up;
int SendStringCMD(void)
{
  ROS_INFO("sensor ready ^^");
  std_msgs::String robotCMD;
  std::stringstream ss;
  ss << "start";
  robotCMD.data = ss.str();
  pub_cmd.publish(robotCMD);
  return 0;
}

void cmdCallback(const std_msgs::String& robotCMD)
{
  ROS_INFO("STO_pre_check_node, msg: %s", robotCMD.data.c_str());
  //const char* cCMD = robotCMD.data.c_str();
  if(robotCMD.data == "record"){
        if(b_record){
	    b_sensor_check = false;
        }else{
            b_record = true;
	    b_sensor_check = true;
	    iCount_velo = 0;
	    iCount_imu = 0;
	    bVeloUP = false;
	    bIMU_up = false;
        }
    }else if(robotCMD.data == "stop"){
	b_sensor_check = false;
    }
}

void velodyneCallback(const velodyne_msgs::VelodyneScan::ConstPtr &scanMsg)
{
  //ROS_INFO("velo");
  if(b_sensor_check)
  {
    iCount_velo++;
    if(iCount_velo>5)
    {
      bVeloUP = true;
      //ROS_INFO("velodyne get");
      if(bVeloUP&&bIMU_up)
      {
	SendStringCMD();
	b_sensor_check = false;
	b_record = false;
      }
    }
  }
}

void imuCallback(const sensor_msgs::Imu::ConstPtr& inImu)
{
  if(b_sensor_check)
  {
    iCount_imu++;
    if(iCount_imu>5)
    {
      bIMU_up = true;
      //ROS_INFO("imu get");
      if(bVeloUP&&bIMU_up)
      {
	SendStringCMD();
	b_sensor_check = false;
	b_record = false;
      }
    }
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "pre_check_node");
  ros::NodeHandle nh;
  ros::Rate r(100);
  ros::Subscriber imuSub = nh.subscribe<sensor_msgs::Imu>("/imu", 1, &imuCallback);
  ros::Subscriber topic_velodyne = nh.subscribe("/velodyne_packets", 10, &velodyneCallback);
  ros::Subscriber topic_cmd = nh.subscribe("/syscommand", 2, &cmdCallback);
  pub_cmd = nh.advertise<std_msgs::String>("/syscommand", 1);
  
  b_record = false;
  bVeloUP = false;
  bIMU_up = false;
  
  while(ros::ok())
  {
    r.sleep();
    ros::spinOnce();
  }
  return 0;
}