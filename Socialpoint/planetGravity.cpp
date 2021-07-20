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
#include <iomanip>

using namespace std;

#define Vector2 pair<double, double>

Vector2 distance(Vector2 a, Vector2 b)
{
    Vector2 d;
    d.first = b.first - a.first;
    d.second = b.second - a.second;
    return d;
}

double magnitude(Vector2 dist)
{
    return sqrt(((dist.first * dist.first) + (dist.second * dist.second)));
}

Vector2 normalize(Vector2 v)
{
    double m = magnitude(v);

    if (m == 0)
        return Vector2(0,0);
    return Vector2(v.first / m, v.second / m);
}

Vector2 gravityForce(double m, double r, Vector2 u)
{
    if (r == 0)
        return Vector2(0,0);
    // gravitatory force F = -G * (M*m)/r * u
    double x = -1 * (m / (r * r)) * u.first;
    double y = -1 * (m / (r * r)) * u.second;
    return Vector2(x, y);
}

Vector2 addForce(Vector2 a, Vector2 b, double t)
{
    return Vector2(a.first + (b.first * t), a.second + (b.second * t));
}

int main()
{
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */

    // initial input
    double mass;
    Vector2 inipos, inivel;
    cin >> mass >> inipos.first >> inipos.second >> inivel.first >> inivel.second;

    // init variables
    Vector2 pos, vel;
    pos = inipos;
    vel = inivel;

    double t, amount; // t -> deltatime 

    cout << fixed << std::setprecision(2); // setting 
    // loop
    while (cin >> t >> amount)
    {
        for (int i = 0; i < amount; i++)
        {
            Vector2 vecr = distance(Vector2(0, 0), pos);
            double r = magnitude(vecr);
            Vector2 u = normalize(vecr);
            Vector2 vecf = gravityForce(mass, r, u);

            // update vel [v = v0 + a*t]
            vel.first += vecf.first * t;
            vel.second += vecf.second * t;
            // update pos [p = p0 + v*t]
            pos.first += vel.first * t;
            pos.second += vel.second * t;

            // output
            cout << pos.first << " " << pos.second << endl;
        }
    }

    //double f = magnitude(vecf);

    /*
    cout << "vec r " << vecr.first << " " << vecr.second << endl;
    cout << "mag r " << r << endl;
    cout << "vec u " << u.first << " " << u.second << endl;
    cout << "vec f " << vecf.first << " " << vecf.second << endl;
    cout << "mag f " << f << endl;
    */

    return 0;
}