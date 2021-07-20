#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;

unsigned char S[256], S2[256];
unsigned int i, j;
unsigned int ini_vec_length, key_loop_it;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void swap(unsigned char s[], unsigned int i, unsigned int j)
{
    unsigned char temp = s[i];
    s[i] = s[j];
    s[j] = temp;
}

void cipher_ini(unsigned char key[], int key_length)
{
    // ini S
    for (i = 0; i < 256; i++)
        S[i] = i;

    // copy key to S2
    for (i = 0; i < key_length; i++)
        S2[i] = key[i];

    // initialization vector
    int key_vec_lenght = ini_vec_length + key_length;
    if (ini_vec_length != 0)
    {
        unsigned char I[ini_vec_length];
        for (i = 0; i < ini_vec_length; i++)
            I[i] = rand() & 255;

        for (i = key_length; i < key_vec_lenght; i++)
            S2[i] = I[i - key_length];
    }

    // append until S2 filled
    for (i = key_vec_lenght; i < 256; i++)
        S2[i] = S2[i % key_vec_lenght];


    // randomize S
    for (int w = 0; w < key_loop_it; ++w)
    {
        for (i = j = 0; i < 256; i++)
        {
            //j = (j + S[i] + S2[i]) & 255;
            j = (j + S[i] + key[i % key_length]) & 255;
            swap(S, i, j);
        }
    }

    i = j = 0;
}

unsigned char cipher_output()
{
    i = (i + 1) & 255;
    j = (j + S[i]) & 255;

    swap(S, i, j);

    return S[(S[i] + S[j]) & 255];
}

int main()
{
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    string encrypted, key;

    cin >> encrypted >> key;
    cin >> ini_vec_length >> key_loop_it >> key;

    unsigned char k[key.length()];
    for (i = 0; i < key.length(); i++)
    {
        k[i] = key[i];
    }
    cipher_ini(k, key.length());

    cout << encrypted.length();
    for(int y = 0; y < encrypted.length(); ++y)
    {
        unsigned char c = encrypted[y];
        c = c ^ cipher_output();
        cout << cipher_output();
    }

    return 0;
}