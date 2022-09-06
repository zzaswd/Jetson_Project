
//====================================================================
// Bridge From Socket To ROS
// 1. Jetson1 이라는 이름으로 Socket server에 Login
// 2. Jetson1에게 오는 message 그대로 ROS Topic "Jetson1"으로 Publish
//====================================================================


#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define ARR_CNT 5

void* recv_msg(void* arg);
void error_handling(char* msg);

char name[NAME_SIZE] = "[Default]";
char msg[BUF_SIZE];

class PASS {
public:
	int sock;
	int argc;
	char** argv;
};

int main(int argc, char* argv[])
{
	PASS pass;
	pass.argc = argc;
	pass.argv = argv;

	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void* thread_return;

	sprintf(name, "%s", "Jetson1");
	pass.sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("10.10.141.188");
	serv_addr.sin_port = htons(atoi("5000"));

	if (connect(pass.sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");


	sprintf(msg, "[%s:PASSWD]", name);
	write(pass.sock, msg, strlen(msg));
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&pass);

	
	pthread_join(rcv_thread, &thread_return);

	close(sock);
	return 0;
}

void* recv_msg(void* arg)
{
	PASS* recv = (PASS*)arg;
	int sock = recv->sock;

	ros::init(recv->argc, recv->argv, "Jetson1");
	ros::NodeHandle n;
	ros::Publisher jetson1_pub = n.advertise<std_msgs::String>("Jetson1", 1000);
	ros::Rate loop_rate(1);

	int i;
	char* pToken;
	char* pArray[ARR_CNT] = { 0 };

	char name_msg[NAME_SIZE + BUF_SIZE + 1];
	int str_len;
	while (1) {
		memset(name_msg, 0x0, sizeof(name_msg));
		str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE);
		if (str_len <= 0)
		{
			sock = -1;
			return NULL;
		}
		name_msg[str_len] = 0;
		//std_msg 
		//fputs(name_msg, stdout);

		if (ros::ok()) {
			std_msgs::String msg;
			std::stringstream ss1;
			ss1 << name_msg;
			msg.data = ss1.str();
			ROS_INFO("%s", msg.data.c_str());

			jetson1_pub.publish(msg);
			ros::spinOnce();
			loop_rate.sleep();
		}
	}
}

void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}