#include <iostream>
#include <string>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

//===========PROTOTYPE==========
void *childThread(void *i);
//==============================

void error(char *msg)
{
    perror(msg);
    exit(0);
}

struct child_struct
{
    struct hostent *child_server;
    int child_port;
    string binry;
};

struct message2
{
    int value;
    char buffer[256];
};

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    //=====================READ INPUT ======================
    string message101;
    cin >> message101;
    //======================SUBSTRING========================

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    struct message2 msg2;
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer, 256);

    n = read(sockfd, &msg2, sizeof(struct message2));
    if (n < 0)
        error("ERROR reading from socket");

    //========================TO FIND THE MESSAGE'S SUBSTRING==============
    vector<string> temp;
    string bin;
    for (int i = 0; i < message101.length(); i = i + msg2.value)
    {

        bin = message101.substr(i, msg2.value);
        temp.push_back(bin);
    }
    //======================================================================
    static int threadSize = message101.length() / msg2.value;
    pthread_t tid[threadSize];

    static struct child_struct *arg = new struct child_struct[threadSize];

    cout << "Decompressed message: ";
    for (int i = 0; i < threadSize; i++)
    {
        arg[i].binry = temp[i];
        arg[i].child_server = server;
        arg[i].child_port = portno;
        if (pthread_create(&tid[i], nullptr, childThread, &arg[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
        pthread_join(tid[i], nullptr);
    }

    // for (int i = 0; i < threadSize; i++)
    // {
    //     pthread_join(tid[i], nullptr);
    // }

    return 0;
}

void *childThread(void *i)
{
    struct child_struct *ptr = (struct child_struct *)i;
    int sockfd, n;
    struct sockaddr_in serv_addr;
    char buffer[256];
    string value = ptr->binry;
    strncpy(buffer, value.c_str(), value.length());

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)ptr->child_server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          ptr->child_server->h_length);
    serv_addr.sin_port = htons(ptr->child_port);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    n = write(sockfd, &buffer, strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");

    char datarcv[256];
    bzero(datarcv, 256);
    n = read(sockfd, datarcv, 255);
    if (n < 0)
        error("ERROR reading from socket");

    char *compress_m = new char[256];
    sscanf(datarcv, "%[^,]", compress_m);
    printf("%s", compress_m);
    close(sockfd);
    return nullptr;
}
