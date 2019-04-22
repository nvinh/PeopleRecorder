/*
 * Vinh Nguyen, pedestrian recorder
 */
// %Tag(FULLTEXT)%
// %Tag(ROS_HEADER)%
#include "ros/ros.h"
// %EndTag(ROS_HEADER)%
// %Tag(MSG_HEADER)%
#include "std_msgs/String.h"
// %EndTag(MSG_HEADER)%
//-------------------------
#define _USE_MATH_DEFINES
#include <math.h>
//-------------------------
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
//-------------------------
#include "sensor_msgs/LaserScan.h"
//-------------------------
#include <sstream>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/Pose2D.h>
#include <peoplerecorder/Person.h>
#include <peoplerecorder/PersonArray.h>

/*! just for quickly & temporarily implementing the Subscriber node */
 long laserdata[1000];
 long laserdata_n;
 long seq;
struct robotposeData {
	robotposeData(): minx(10000), maxx(-10000), miny(10000), maxy(-10000), mintheta(10000), maxtheta(-10000) {}
	long minx, maxx, miny, maxy;
	double mintheta, maxtheta;
	long x, y;
	double theta;
};

robotposeData rp;
//---------------------------
//-------- Global variables
//--------------------------
// this procedure will read the data from people_tracked node (leg_tracker module)
void scanPeople(const peoplerecorder::PersonArray& people_tracked)	//(const geometry_msgs::PoseArray people)
{
    time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    puts(buffer);
    //-------------------
	std_msgs::Header h;
	h= people_tracked.header;
	printf("header: seq: %d, stamp: secs= %d, nsecs= %d\n", h.seq, h.stamp.sec,h.stamp.nsec);
    unsigned int datasize= (unsigned int)(people_tracked.people.size());
    printf("datasize: %d\n", datasize);
    seq++;
    printf("sequence: %ld\n", seq);
    
    for (unsigned int i=0; i<datasize;i++) 
    {
		long id;
		double x, y;
		peoplerecorder::Person p;
		p= people_tracked.people[i];
		id= p.id;
		x= p.pose.position.x;
		y= p.pose.position.y;
		x= x * 1000;
		y= y * 1000;
		long xi, yi;
		xi= (long)x;
		yi= (long)y;
		printf("run here...id=%ld, x= %ld, y= %ld\n", id, xi, yi);
	}
	printf("---end of row---\n");
}
void robotPose(const geometry_msgs::Pose2D& rpin)
{
		long x, y;
		double theta;
		x= (long)(rpin.x * 1000);
		y= (long)(rpin.y * 1000);
		theta= rpin.theta;
		
		rp.x= x;
		rp.y= y;
		rp.theta= theta;
		if (rp.minx > x) { rp.minx = x; }
		if (rp.maxx < x) { rp.maxx = x; }
		if (rp.miny > y) { rp.miny = y; }
		if (rp.maxy < y)  { rp.maxy = y; }
		if (rp.mintheta > theta) { rp.mintheta = theta; }
		if (rp.maxtheta < theta) { rp.maxtheta = theta; }
		printf("robot Pose: x= %ld, y= %ld, theta=%0.02f\n", x, y, theta);
		printf("min max Pose: minx= %ld, maxx= %ld, miny= %ld, maxy= %ld, mintheta= %0.02f, maxtheta= %0.02f\n", 
			rp.minx, rp.maxx, rp.miny, rp.maxy, rp.mintheta, rp.maxtheta);
}
//--------------------------
int main(int argc, char **argv)
{
	//---------------------------
	long loop_count= 0;
	int wait_after_init= 0; 	// temporary variables
	seq= 0;	// number of sequences
	//--------------------------
	// %Tag(INIT)%
	ros::init(argc, argv, "peoplerecorder_pub");
	// %EndTag(INIT)%
	// %Tag(NODEHANDLE)%
	ros::NodeHandle m;
	// %EndTag(NODEHANDLE)%
	//-----------------------------	
	ros::NodeHandle peoplerecorder;
	printf("Ready...\n");

	// %Tag(SUBSCRIBER)%
	ros::Subscriber peopleSubscriber = peoplerecorder.subscribe("people_tracked", 1000, scanPeople);
	ros::Subscriber robotPoseSubscriber = peoplerecorder.subscribe("pose2D", 1000, robotPose);
	// %EndTag(SUBSCRIBER)%
	//-----------------------------	

	// %Tag(PUBLISHER)%
	ros::Publisher chatter_pub = m.advertise<std_msgs::String>("cmd", 1000);
	// %EndTag(PUBLISHER)%

	// %Tag(LOOP_RATE)%
	ros::Rate loop_rate(10); // 10Hz
	// %EndTag(LOOP_RATE)%
    long time_stamp;

    while (ros::ok())
    {
		// compute values from measurement values
		std::stringstream ss;
		// ---- publish the command
		std_msgs::String pub_cmd;
		++loop_count;
		// %Tag(SPINONCE)%
		ros::spinOnce();
		// %EndTag(SPINONCE)%

		// %Tag(RATE_SLEEP)%
		loop_rate.sleep();
		// %EndTag(RATE_SLEEP)%
    }

//--------------------------

  return 0;
}
// %EndTag(FULLTEXT)%
