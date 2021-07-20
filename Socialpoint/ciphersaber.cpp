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

unsigned char S[256], T[256];
unsigned int i, j, L;
string messaje;

void intercambio(int i, int j)
{
    // this func name is in spanish bc calling it swap brokes the logic bc potato
    unsigned char tmp = S[i];
    S[i] = S[j];
    S[j] = tmp;
}

void ksa(string key, int vec_length, int loop_it, unsigned char *cripto, bool mode)
{
    for (i = 0; i < 256; i++)
        S[i] = i;

    if (vec_length != 0)
    {

        // It should work bc the end vector T repeats itself until 256 so in theory it works.

        int key_vec_lenght = key.length() + vec_length;

        for (i = 0; i < key.length(); i++)
            T[i] = key[i];

        unsigned char I[vec_length];
        if (mode)
        {
            // encrypt
            for (i = 0; i < vec_length; i++)
                I[i] = rand() % 256; //S[i]; //T[i % key.length()]; //rand() % 256; // do I have to generate random or copy the first vec_length bytes????
        }
        else
        {
            // decrypt
            for (i = 0; i < vec_length; i++)
                I[i] = T[i % key.length()]; //cripto[i];
        }

        for (i = key.length(); i < key_vec_lenght; i++)
            T[i] = I[i - key.length()];

        // append until T filled
        for (i = key_vec_lenght; i < 256; i++)
            T[i] = T[i % key_vec_lenght];

        /*
        for (i = 0; i < 256; i++)
        {
            if (i % 16 == 0)
                printf("\n");
            printf("\t%02hhX", T[i]);
        }
        printf("\n");
        printf("\n");
        */
    }
    else
    {
        for (i = 0; i < 256; i++)
            T[i] = key[i % key.length()];
    }

    for (int n = 0; n < loop_it; ++n)
    {
        j = 0;
        for (i = 0; i < 256; i++)
        {
            j = (j + S[i] + T[i]) % 256;
            intercambio(i, j);
        }
    }
}

int main()
{
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    messaje = "";

    string key;
    int v_len, n_loop;

    // Input
    char c;
    cin >> c;
    while (c != '-')
    {
        messaje.push_back(c);
        cin >> c;
    }
    cin >> key >> v_len >> n_loop >> key;

    L = messaje.length();
    unsigned char cripto[L / 2];

    for (int i = 0; i < L; i += 2)
    {
        std::string byte = messaje.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
        cripto[i / 2] = chr;
    }

    // decrypt
    ksa(key, v_len, n_loop, cripto, false);
    // prga
    i = 0;
    j = 0;
    int k = 0, t = 0;
    unsigned char key_stream[L];
    while (k < L)
    {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        intercambio(i, j);
        t = (S[i] + S[j]) % 256;
        key_stream[k] = S[t];
        k++;
    }
    //printf("\n");
    for (i = 0; i < L/2; i++)
    {
        printf("%c", (key_stream[i] ^ cripto[i]));
    }

    /*encrypt
    ksa(key, v_len, n_loop, cripto, true);
    // prga
    i = 0;
    j = 0;
    k = 0;
    t = 0;
    //unsigned char key_stream[L];
    while (k < L)
    {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        intercambio(i, j);
        t = (S[i] + S[j]) % 256;
        key_stream[k] = S[t];
        k++;
    }
    for (i = 0; i < L; i++)
    {
        printf("%02hhX ", (key_stream[i] ^ messaje[i]));
    }
    */
    

    return 0;
}