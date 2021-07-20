#include <iostream>
#include <cmath>
using namespace std;

#define N 10

void quadraticEquation()
{
    float a, b, c, determinant;
    cout << "Introduce los valores de a, b, c:" << endl;
    cin >> a >> b >> c;
    determinant = b * b - 4 * a * c;

    cout << "determinant = " << determinant << endl;

    if (determinant > 0)
    {
        float root1 = (-b + sqrtf(determinant)) / (2 * a);
        float root2 = (-b - sqrtf(determinant)) / (2 * a);

        cout << "root1 = " << root1 << endl;
        cout << "root2 = " << root2 << endl;
    }
    else
    {
        float realroot = -b / 2 * a;
        if (determinant == 0)
        {
            cout << "root1&root2 = " << realroot << endl;
        }
        else
        {
            float iroot = (sqrtf(-determinant)) / 2 * a;
            cout << "root1 = " << realroot << " + i" << iroot << endl;
            cout << "root2 = " << realroot << " - i" << iroot << endl;
        }
    }
}

void doubleSwap()
{
    string s;
    char c1, c2;
    cout << "Introduce una frase con punto final y dos caracteres a reemplazar" << endl;
    char c;
    cin >> c;
    while (c != '.')
    {
        s.push_back(c);
        cin >> c;
    }
    cin >> c1 >> c2;

    for (int i = 0; i < s.length(); ++i)
    {
        if (s[i] == c1)
        {
            s[i] = c2;
        }
        else if (s[i] == c2)
        {
            s[i] = c1;
        }
    }
    cout << s << endl;
}

int main(int argc, char const *argv[])
{
    string operations[N] = {
        "quadraticEquation",
        "doubleSwap"};

    cout << "Select operation by introducing its number:" << endl;
    for(int i = 0; i < N; i++)
    {
        if(operations[i] != "")
        {
            cout << i << " " << operations[i] << endl;
        }
    }

    int selected;
    cin >> selected;

    if (selected == 0) quadraticEquation();
    else if (selected == 1) doubleSwap();
    else return 0;
}
