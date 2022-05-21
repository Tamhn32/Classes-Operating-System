#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
#include <algorithm>
#include <cmath>

using namespace std;

struct message // To store the values in input file
{
    string c;
    int decimal_num;
};

struct message2
{
    int value;
    char buffer[256];
};

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void fireman(int)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }
}
//=======================PROTOTYPE======================
int findMaximum(vector<struct message> v, int size); // THE LENGTH'S BIT
string intTobinary(int value, int maxb);             // CONVERT DECIMAL TO BINARY
//======================================================

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char bincode[256];
    //==================================================
    string temp0;
    string sub;
    int numline;
    static int size = numline;
    struct message temp1;
    vector<struct message> list;
    //===================STORE SUTFF INTO STRUCT AND READ INPUT===========
    cin >> numline;
    cin.ignore(50, '\n');
    for (int i = 0; i < numline; i++)
    {
        getline(cin, temp0);
        temp1.c = temp0[0];
        sub = temp0.substr(2, temp0.size() - 1);
        temp1.decimal_num = stoi(sub);
        list.push_back(temp1);
    }
    int maxBits = findMaximum(list, numline);
    //===========================================================
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    if (n < 0)
        error("ERROR reading from socket");

    struct message2 m2;
    m2.value = findMaximum(list, numline);
    strcpy(m2.buffer, "Message from the server\0");
    n = write(newsockfd, &m2, sizeof(struct message2));
    if (n < 0)
        cout << "Unable to write" << endl;

    //==================CALLING AND STORING BINARY=============
    string concho[numline];
    for (int i = 0; i < numline; i++)
    {
        concho[i] = intTobinary(list[i].decimal_num, m2.value);
    }
    //======================================================
    signal(SIGCHLD, fireman);
    char datasent[256]; // TO SAVE THE DATA FROM CLIENT
    string msg;         // TO STORE THE COMPRESS MESSAGE
    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

        if (fork() == 0)
        {
            if (newsockfd < 0)
                error("ERROR on accept");

            bzero(buffer, 256);
            n = read(newsockfd, &buffer, 256);
            if (n < 0)
                error("ERROR reading from socket");

            sscanf(buffer, "%[^,]", bincode);
            string tempS(bincode);

            for (int i = 0; i < numline; i++)
            {
                if (concho[i] == tempS)
                {
                    msg = list[i].c;
                    strcpy(datasent, msg.c_str());
                    n = write(newsockfd, datasent, strlen(datasent));
                    if (n < 0)
                        error("ERROR writing to socket");
                }
            }

            if (n < 0)
                error("ERROR writing to socket");

            close(newsockfd);
            _exit(0);
        }
    }
    wait(0);
    close(sockfd);

    return 0;
}

int findMaximum(vector<struct message> v, int size)
{
    int max = 0;
    for (int i = 0; i < size; i++)
    {
        if (v[i].decimal_num > max) // Find the highest decimal value to be used to find the bit of the binary code
        {
            max = v[i].decimal_num;
        }
    }
    int bit = ceil(log2(max)); // Find the bit of the binary code
    return bit;
}

string intTobinary(int value, int maxb)
{
    int decimal = value; // Decimal value to be used for calculation
    string toBinary;     // String to hold the binary code obtained
    for (int j = maxb - 1; j >= 0; j--)
    {
        int tempplace = pow(2, j);
        if (decimal - tempplace >= 0)
        {
            decimal = decimal - tempplace;
            toBinary.append("1");
        }
        else
        {
            toBinary.append("0");
        }
    }
    return toBinary;
}
