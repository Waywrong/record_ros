#ifndef RECORD_ROS_RECORD_H_
#define RECORD_ROS_RECORD_H_

#include "record_ros/String_cmd.h"
#include <std_msgs/String.h>
#include <rosbag/recorder.h>
#include "std_msgs/String.h"
#include <velodyne_msgs/VelodyneScan.h>
#include "sensor_msgs/Imu.h"
#include <string>
class Record : public rosbag::Recorder{

public:

    Record(ros::NodeHandle& nh,rosbag::RecorderOptions const& options);
    void cmdCallback(const std_msgs::String& robotCMD);
    void velodyneCallback(const velodyne_msgs::VelodyneScan::ConstPtr &scanMsg);
    void imuCallback(const sensor_msgs::Imu::ConstPtr& inImu);
    void wait_for_callback();
    void recording_heartbeat();
    
private:

    bool string_command(record_ros::String_cmd::Request& req, record_ros::String_cmd::Response& res);

private:

    bool                        b_record;
    bool                        b_sensor_velo; // check if topic is actived
    bool                        b_sensor_imu;
    ros::ServiceServer          service_srv;
    ros::Subscriber             topic_cmd;
    ros::Subscriber             topic_imu;
    ros::Subscriber             topic_velodyne;
    ros::Subscriber             topic_wifi;

};


#endif
