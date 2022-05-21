#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;

void *makeThread(void *i);
string intTobinary(int value, int maxb);
int findFrequency(string binary, string mess, int sizeB);
int findMaximum(struct arg1 *v, int size);
void *makeDecompMess(void *m);

struct arg1
{
    string c;
    int decimal_num;
    int line;
};

struct arg2
{
    string c2;
    int decimal_num2;
    int line2;
    int maxBit;
    string binCode;
    int fre;
    string messageCode22;
    int count;
    int *turn2;
    pthread_cond_t *c_empty2;
    pthread_mutex_t *c_bsem2;
    sem_t *sem2;
};

struct arg3
{
    string code;
    string c3[100];
    string bin3[100];
    int line3;
    int threadsiZ;
    int count3;
    int *turn3;
    pthread_cond_t *c_empty;
    pthread_mutex_t *c_bsem;
    sem_t *sem3;
};


int main()
{
    static pthread_mutex_t bsem1;
    static pthread_cond_t empty1 = PTHREAD_COND_INITIALIZER;
    sem_t nsem2;

    struct arg1 temp;   // Struct argument to store data
    int numline;        // Numline of the code - Number of symbols and lines of the input
    cin >> numline;     // Read first line of the input
    int size = numline; // Let size of the first thread = size of numline = first input
    string tempp;       // Temporary string to store inputs
    string sub;         // Tempopary substring to be split to get the decimal value
    string messages12;  // Store the message to be decompressed

    //======================READ INPUT========================//
    struct arg1 *arg = new arg1[size]; // Struct array used for Thread 1
    cin.ignore(50, '\n');
    for (int i = 0; i < size; i++)
    {
        getline(cin, tempp);                     // Read input from file
        arg[i].c = tempp[0];                     // Store character symbol into the first Struct
        sub = tempp.substr(2, tempp.size() - 1); // Temporary substring
        arg[i].decimal_num = stoi(sub);          // Split temporary substring to get the decimal value of the input
        arg[i].line = numline;                   // Save number of symbols/lines into the first Struct
    }
    cin >> messages12;
    //===================================================
    //======================THREAD1 DATA========================//
    pthread_t *tid = new pthread_t[size]; // Thread 1 ID - size of numline - first input
    struct arg2 arg22;

    // Semaphore mutex
    arg22.c_bsem2 = &bsem1;
    arg22.c_empty2 = &empty1;

    //  Store maxbit and message into struct number2
    arg22.maxBit = findMaximum(arg, numline);
    arg22.messageCode22 = messages12;

    arg22.count = 0;
    static int cturn2 = 0;
    arg22.turn2 = &cturn2;

    // Semaphore
    arg22.sem2 = &nsem2;
    sem_init(arg22.sem2, 0, 0);

    cout << "Alphabet:" << endl;

    for (int i = 0; i < size; i++)
    {
        arg22.count = i;
        arg22.c2 = arg[i].c;
        arg22.decimal_num2 = arg[i].decimal_num;
        if (pthread_create(&tid[i], NULL, makeThread, &arg22))
        {
            cerr << "Cannot create the thread" << endl;
            return 1;
        }
        // Wait for another thread
        sem_wait(arg22.sem2);
    }
    for (int i = 0; i < size; i++)
    {
        pthread_join(tid[i], nullptr);
    }
    //======================THREAD 1 END========================//

    //======================THREAD 2 DATA========================//
    static pthread_mutex_t bsem3;
    static pthread_cond_t empty3 = PTHREAD_COND_INITIALIZER;
    sem_t nsem3;

    static int size2 = messages12.length() / arg22.maxBit; // Size of the second thread
    struct arg3 arg33;
    pthread_t *tid2 = new pthread_t[size2]; // CREATE THREAD2'S ID

    // Semaphore
    arg33.sem3 = &nsem3;
    sem_init(arg33.sem3, 0, 0);

    // Semaphore (mutex)
    arg33.c_empty = &empty3;
    arg33.c_bsem = &bsem3;

    static int cturn3 = 0;
    arg33.count3 = 0;
    arg33.threadsiZ = size2;
    arg33.turn3 = &cturn3;

    // Find the substring
    vector<string> bina;
    string bin;
    for (int i = 0; i < messages12.length(); i = i + arg22.maxBit)
    {
        bin = messages12.substr(i, arg22.maxBit);
        bina.push_back(bin);
    }

    for (int i = 0; i < size; i++)
    {
        arg33.c3[i] = arg[i].c;
        arg33.bin3[i] = intTobinary(arg[i].decimal_num, arg22.maxBit);
    }

    cout << "\nDecompressed message: ";
    for (int i = 0; i < size2; i++)
    {

        arg33.code = bina[i];
        arg33.count3 = i;
        pthread_create(&tid2[i], NULL, makeDecompMess, &arg33); // Print out error message if thread is not created
        sem_wait(arg33.sem3);
    }

    for (int i = 0; i < size2; i++) // CALL THE JOIN
    {
        pthread_join(tid2[i], NULL);
    }

    // cout << arg33.decompmess1[0] << endl;
    delete[] tid;
    delete[] tid2;
    delete[] arg;
    // Semaphore
    sem_destroy(&nsem2);
    sem_destroy(&nsem3);
    return 0;
}

void *makeThread(void *i)
{
    struct arg2 argFromMain = *((struct arg2 *)i);

    //  Semaphore
    sem_post(argFromMain.sem2);

    static int c_turn = *argFromMain.turn2;

    argFromMain.binCode = intTobinary(argFromMain.decimal_num2, argFromMain.maxBit);
    argFromMain.fre = findFrequency(argFromMain.binCode, argFromMain.messageCode22, argFromMain.maxBit);

    // Critical Section
    pthread_mutex_lock(argFromMain.c_bsem2);
    while (argFromMain.count != c_turn)
        pthread_cond_wait(argFromMain.c_empty2, argFromMain.c_bsem2);

    // Unlock and end critcal section
    pthread_mutex_unlock(argFromMain.c_bsem2);

    cout
        << "Character: " << argFromMain.c2 << ", "
        << "Code: " << argFromMain.binCode << ", "
        << "Frequency: " << argFromMain.fre << endl;

    // Critical Section
    pthread_mutex_lock(argFromMain.c_bsem2);
    c_turn++;

    // Wake up thread which is waiting in main
    pthread_cond_broadcast(argFromMain.c_empty2);

    // Unlock and end critcal section
    pthread_mutex_unlock(argFromMain.c_bsem2);
    return nullptr;
}

void *makeDecompMess(void *m)
{
    struct arg3 argFromMain = *((struct arg3 *)m);

    //   Semaphore
    sem_post(argFromMain.sem3);
    string de;
    static int size = argFromMain.threadsiZ;

    static int c_turn3 = *argFromMain.turn3;

    // Critical Section
    pthread_mutex_lock(argFromMain.c_bsem);
    while (argFromMain.count3 != c_turn3)
        pthread_cond_wait(argFromMain.c_empty, argFromMain.c_bsem);

    // Unlock and end critcal section
    pthread_mutex_unlock(argFromMain.c_bsem);

    for (int i = 0; i < argFromMain.threadsiZ; i++)
    {
        if (argFromMain.code == argFromMain.bin3[i])
        {
            de = argFromMain.c3[i];
            cout << de;
        }
    }

    // Critical Section
    pthread_mutex_lock(argFromMain.c_bsem);
    c_turn3++;

    // Wake up thread which is waiting in main
    pthread_cond_broadcast(argFromMain.c_empty);

    // Unlock and end critcal section
    pthread_mutex_unlock(argFromMain.c_bsem);

    return nullptr;
}

// THE LENGTH OF THE BIT========================
int findMaximum(struct arg1 *v, int size)
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

// CONVERT DECIAMLL TO BINARY===========================
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

// // FIND THE FREQUENCY================================
int findFrequency(string binary, string mess, int sizeB)
{
    int freqq = 0; // Initialize the count variable freqq to be 0

    for (int i = 0; i < mess.length(); i = i + sizeB)
    {
        // Compare to see if the binary code from the previous function is the same as the substring from the compressed message
        if ((mess.substr(i, sizeB)) == binary)
        {
            freqq++; // If yes, then increment the count variable
        }
    }
    return freqq;
}
