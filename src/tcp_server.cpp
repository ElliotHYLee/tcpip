#include <ros/ros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include "tcpip/MsgTutorial.h"
#include <vector>

using namespace std;

void getTokens(vector<string>& tokens, string str, int lim)
{
  //vector<string> tokens(3);
  int cs = 0;
  char x = str.at(cs);
  int start = cs;
  while(cs<lim)
  {
    //cout << x << endl;
    x = str.at(cs);

    if (x==',')
    {
      cout << "asdf  " << str.substr(start, cs-start) << endl;
      tokens.push_back(str.substr(start, cs-start));
      cout << tokens[0] <<endl;
      start = cs+1;
    }
    cs++;
  }

}

void connection(int sock, ros::Publisher att_pub, tcpip::MsgTutorial msg, ros::Rate loop_rate)
{
  char buf[1024];
  //printf("im here\n");
  int n, count = 0;
  while(1)
  {
    //printf("im here 1\n");
    n = read(sock,buf,1024);
    printf("received: %d bytes.\n", n);
    std::string rcvMsg(buf);
    std::cout << rcvMsg.substr(0,4) <<std::endl;
    //printf("im here 2\n");
    // Pretty much your C++ code verbatim.
    std::string sendMsg = "Acknowledged.\r\n";
    int amt = send(sock, sendMsg.c_str(), sendMsg.size(), 0);
    printf("Send %d bytes.\n", amt);
    //printf("im here 3\n");

    vector<string> tokens;
    getTokens(tokens, rcvMsg, n);
    // cout << tokens.size() << endl;
    // cout << rcvMsg << endl;
    cout << tokens[0] << endl;
    cout << tokens[1] << endl;
    cout << tokens[2] << endl;

    std::string::size_type sz;   // alias of size_t
    msg.pitch = atoi(tokens[0].c_str());
    msg.roll  = atoi(tokens[1].c_str());
    msg.yaw = atoi(tokens[2].c_str());
    ROS_INFO("send pitch = %d", msg.pitch);
    ROS_INFO("send roll = %d", msg.roll);
    ROS_INFO("send yaw = %d", msg.yaw);
    att_pub.publish(msg);
    loop_rate.sleep();
    ++count;
  }

  close(sock);
}


int main(int argc, char *argv[])
{
    ros::init(argc, argv, "tcp_server");
    ros::NodeHandle nh;
    ros::Publisher att_pub = nh.advertise<tcpip::MsgTutorial>("att_pub", 100);
    ros::Rate loop_rate(10);

    tcpip::MsgTutorial msg;
    int count = 0;

    int sock, csock;
    struct sockaddr_in sin;
    char *host = "10.18.1.31";
    unsigned short port = 2222;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(EXIT_FAILURE);
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &sin.sin_addr) != 1) {
      perror("inet_pton");
      exit(EXIT_FAILURE);
    }

    if (bind(sock, (struct sockaddr*) &sin, sizeof(sin)) != 0) {
      perror("bind");
      exit(EXIT_FAILURE);
    }

    if (listen(sock, SOMAXCONN) != 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }

    if ((csock = accept(sock, NULL, NULL)) == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    connection(csock, att_pub, msg, loop_rate);
    //close(sock);
    return EXIT_SUCCESS;
  }
