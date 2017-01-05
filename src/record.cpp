#include "record_ros/record.h"
#include <ros/ros.h>
#include <boost/thread.hpp>

void Record::cmdCallback(const std_msgs::String& robotCMD)
{
  ROS_INFO("STO_sysMsgCallback, msg contents: %s", robotCMD.data.c_str());
  //const char* cCMD = robotCMD.data.c_str();
  if(robotCMD.data == "record"){
        if(b_record){
            ros::shutdown();
        }else{
            b_record = true;
        }
    }else if(robotCMD.data == "stop"){
        ros::shutdown();
    }
}


Record::Record(ros::NodeHandle &nh,rosbag::RecorderOptions const& options):
    rosbag::Recorder(options)
{
    service_srv = nh.advertiseService("cmd",&Record::string_command,this);
    topic_cmd = nh.subscribe("/syscommand", 2, &Record::cmdCallback,this);
    b_record    = false;

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
            ros::shutdown();
            res.res = "stopping recorder";
        }else{
            b_record = true;
            res.res  = "starting recorder";
	    //recording_heartbeat();
        }
        return true;
    }else if(cmd == "stop"){
        ros::shutdown();
        res.res = "stopping recorder";
        return true;
    }else{
        res.res = "No such command[" + cmd + "] in [Record::string_command]";
        ROS_WARN_STREAM(res.res);
        return false;
    }
}
