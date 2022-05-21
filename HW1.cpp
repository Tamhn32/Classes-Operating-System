#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <iomanip>
#include <bitset>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>
#include <map>

using namespace std;

//=========================PROTOYPE==============================//
void *makeThread(void *para);                               // THREAD FUNCTION 1
void *makeDecompMess(void *para);                           // THREAD FUNCTION 2
int findMaximum(struct arguments *v, int size);             // THE LENGTH'S BIT
string intTobinary(int value, int maxb);                    // CONVERT DECIMAL TO BINARY
int findFrequency(string binary, string mess, int sizeBit); // FIND THE FREQUENCY

//==============================================================//

//==========================STRUCT========================================/
struct arguments
{
    string c;              // a b c
    int decimal_num;       // 2 4 5
    int binaryLength;      // 3 - maxbit
    int line;              // 3 or 13
    string messageCode100; // 010010010100101101101
    //=====THREAD=========
    string symbol; // abc
    string bin;    // 100 101 010
    int freqency;  // 313
};
struct arguments2
{
    string messageCode101; // Message - 010010010100101101101 - aaabccc
    int maxbitt;           // Number of bit length - 3
    string symbol[100];
    string binCode[100];
    string su[100];
    int numl; // Number of symbols/lines from input
};
//==========================================================//

int main()
{
    //===================STORE SUTFF INTO STRUCT===========
    struct arguments temp; // Struct argument to store data
    int numline;           // Numline of the code - Number of symbols and lines of the input
    cin >> numline;        // Read first line of the input
    int size = numline;    // Let size of the first thread = size of numline = first input
    string tempp;          // Temporary string to store inputs
    string sub;            // Tempopary substring to be split to get the decimal value
    string messages12;     // Store the message to be decompressed
    //======================THREAD1 DATA========================//
    pthread_t *tid = new pthread_t[size];        // Thread 1 ID - size of numline - first input
    struct arguments *arg = new arguments[size]; // Struct array used for Thread 1
    struct arguments *a[numline];                // Struct array used for Thread 1
    //==================================================
    cin.ignore(50, '\n');

    for (int i = 0; i < size; i++)
    {
        getline(cin, tempp);                     // Read input from file
        arg[i].c = tempp[0];                     // Store character symbol into the first Struct
        sub = tempp.substr(2, tempp.size() - 1); // Temporary substring
        arg[i].decimal_num = stoi(sub);          // Split temporary substring to get the decimal value of the input
        arg[i].line = numline;                   // Save number of symbols/lines into the first Struct
    }

    cin >> messages12; // Read the last line of input - message to be decompressed

    // STORE COMPRESSED MESSAGE INTO STRUCT 1
    for (int i = 0; i < size; i++)
    {
        arg[i].messageCode100 = messages12; // Save encrypted message into Struct 1
    }
    //======================================================================================================

    //========================THREAD 1==============================
    for (int i = 0; i < size; i++)
    {
        pthread_create(&tid[i], NULL, makeThread, &arg[i]);
    }
    // call pthread_join here
    for (int i = 0; i < numline; i++)
    {
        pthread_join(tid[i], (void **)&a[i]);
    }
    // Print out the first part of the output
    cout << "Alphabet: " << endl;

    // Print out character symbols, its binary code and its frequency
    for (int i = 0; i < size; i++)
    {
        cout << "Character: " << a[i]->symbol << ", ";
        cout << "Code: " << a[i]->bin << ", ";
        cout << "Frequency: " << a[i]->freqency << endl;
    }
    //======================================================================================================

    //=========================THREAD2======================
    int maxBits = findMaximum(arg, numline);  // Find the binary's length
    int size1 = messages12.length();          // Size of the compressed message
    static int sizethread2 = size1 / maxBits; // Size of the second thread

    //======================THREAD2 DATA========================//
    pthread_t *tid2 = new pthread_t[sizethread2];          // CREATE THREAD2'S ID
    struct arguments2 *arg1 = new arguments2[sizethread2]; // Struct used for Thread 2
    string *result = new string[sizethread2];              // String array used to store the decompressed message to be printed out

    // Store compressed message into the second Struct
    for (int i = 0; i < sizethread2; i++)
    {
        arg1[i].messageCode101 = messages12;
    }

    // STORE CHARACTER AND BINARY CODE INTO STRUCT 2
    for (int i = 0; i < numline; i++)
    {
        arg1->symbol[i] = a[i]->c;
        arg1->binCode[i] = a[i]->bin;
    }

    //=========================THREAD2======================
    for (int i = 0; i < sizethread2; i++)
    {
        arg1[i].maxbitt = maxBits;                                    // Assign the length's bit into struct
        arg1[i].numl = numline;                                       // Assign the number of lines
        if (pthread_create(&tid2[i], NULL, makeDecompMess, &arg1[i])) // Print out error message if thread is not created
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for (int i = 0; i < sizethread2; i++) // CALL THE JOIN
    {
        pthread_join(tid2[i], NULL);
    }

    // Print out the output of the second thread
    cout << endl
         << "Decompressed message: ";
    for (int i = 0; i < sizethread2; i++)
    {
        cout << arg1->su[i];
    }
    cout << endl;

    delete[] arg;
    delete[] arg1;
    delete[] tid;
    delete[] tid2;

    return 0;
}

//===================== THREADING FUNCTION 1========================
void *makeThread(void *para)
{
    struct arguments *p1 = (struct arguments *)para;
    struct arguments2 *p2 = (struct arguments2 *)para;
    static int Max1 = findMaximum(p1, p1->line); // FIND THE LENGTH OF THE BITS

    p1->symbol = p1->c;                                   // Assign the character symbol
    string convertB = intTobinary(p1->decimal_num, Max1); // Binary code obtained from the function
    p1->bin = convertB;                                   // Save the binary code from the function into the first Struct

    int freq1 = findFrequency(p1->bin, p1->messageCode100, Max1); // Frequency obtained from the function
    p1->freqency = freq1;                                         // Save the frequency into the first Struct

    return (void *)p1;
}
//======================= THREADING FUNCTION 2============================
void *makeDecompMess(void *para)
{
    struct arguments2 *p3 = (struct arguments2 *)para;
    struct arguments2 *p4 = new struct arguments2;

    static int mabit = p3->maxbitt;                                // Max binary length
    static int size = p3->messageCode101.length();                 // Size of the compressed message
    static int sizeCompress = p3->messageCode101.length() / mabit; // Size of the decompressed message
    string de[sizeCompress];                                       // Initialize the string to store the compressed message characters
    static int numli = p3->numl;                                   // Number of lines/symbols

    for (int i = 0; i < numli; i++)
    {
        string concho = p3->binCode[i]; // Store binary code to a temporary string
        for (int j = 0; j < size; j = j + mabit)
        {
            // Compare the substring obtained from the compressed message with the binary code
            // If the difference is 0, assign it to the decompressed message string
            if (p3->messageCode101.substr(j, mabit).compare(concho) == 0)
            {
                de[j / mabit] = p3->symbol[i]; // The index is j/maxbit because we want to save it starting from index 0
            }
        }
    }
    for (int i = 0; i < sizeCompress; i++) // ASSIGN VALUE TO PRINT IT OUT
    {
        p3->su[i] = de[i];
    }

    return nullptr;
}

// THE LENGTH OF THE BIT========================
int findMaximum(struct arguments *v, int size)
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

// FIND THE FREQUENCY================================

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
//==============================================END================================================================================//
