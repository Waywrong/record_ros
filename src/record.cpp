#include "record_ros/record.h"
#include <ros/ros.h>
#include <boost/thread.hpp>

void Record::cmdCallback(const std_msgs::String& robotCMD)
{
  ROS_INFO("STO_sysMsgCallback, msg contents: %s", robotCMD.data.c_str());
  //const char* cCMD = robotCMD.data.c_str();
  if(robotCMD.data == "record"){
        if(b_record){
	    b_sensor_velo = false;
            ros::shutdown();
        }else{
            b_record = true;
	    if(b_sensor_velo)
	      ROS_INFO("STO: velodyne_msgs error !!!!!");
        }
    }else if(robotCMD.data == "stop"){
	b_sensor_velo = false;
        ros::shutdown();
    }
}

void Record::velodyneCallback(const velodyne_msgs::VelodyneScan::ConstPtr &scanMsg)
{
  int lidar_packets_size = scanMsg->packets.size();//76
  if(lidar_packets_size>37)
    b_sensor_velo = true;
}

void Record::imuCallback(const sensor_msgs::Imu::ConstPtr& inImu)
{
}

Record::Record(ros::NodeHandle &nh,rosbag::RecorderOptions const& options):
    rosbag::Recorder(options)
{
    service_srv = nh.advertiseService("cmd",&Record::string_command,this);
    topic_cmd = nh.subscribe("/syscommand", 2, &Record::cmdCallback,this);
    topic_velodyne = nh.subscribe("velodyne_packets", 10, &Record::velodyneCallback,this);
    topic_imu = nh.subscribe<sensor_msgs::Imu>("/imu", 1, &Record::imuCallback,this);
    b_record    = false;
    b_sensor_velo = false;
    b_sensor_imu = false;
}

void Record::wait_for_callback(){
    ros::Rate r(100); // 60 hz
    ros::NodeHandle n;
    ros::Publisher heartbeat_pub = n.advertise<std_msgs::String>("heartbeat", 10);
    int count = 0;
    int _count = 0;
    while (!b_record)
    {
	std_msgs::String msg;
	std::stringstream ss;
	ss << "heartbeat: " << _count;
	msg.data = ss.str();
	if(count % 100 == 0)
	{
		heartbeat_pub.publish(msg);
		++_count;
	}	
//	heartbeat_pub.publish(msg);
	++count;

        ros::spinOnce();
        r.sleep();
    }

}

void Record::recording_heartbeat(){
    ros::Rate r(10);
    ros::NodeHandle n;
    ros::Publisher heartbeat_pub = n.advertise<std_msgs::String>("heartbeat", 10);
    int count = 0;
    while (b_record)
    {
        std_msgs::String msg;
        std::stringstream ss;
        ss << "recording heartbeat: " << count;
        msg.data = ss.str();
        heartbeat_pub.publish(msg);
        ++count;

        ros::spinOnce();
        r.sleep();
    }

}


bool Record::string_command(record_ros::String_cmd::Request& req, record_ros::String_cmd::Response& res){
    std::string cmd = req.cmd;
    ROS_INFO("Record callback");
    if(cmd == "record"){
        if(b_record){
	    b_sensor_velo = false;
            ros::shutdown();
            res.res = "stopping recorder";
        }else{
            b_record = true;
            res.res  = "starting recorder";
	    //recording_heartbeat();
        }
        return true;
    }else if(cmd == "stop"){
	b_sensor_velo = false;
        ros::shutdown();
        res.res = "stopping recorder";
        return true;
    }else{
	b_sensor_velo = false;
        res.res = "No such command[" + cmd + "] in [Record::string_command]";
        ROS_WARN_STREAM(res.res);
        return false;
    }
}
