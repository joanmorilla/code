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

list<string> makeList(string path)
{
    list<string> l;
    int j = 0;
    for (int i = 0; i < path.length(); i++)
    {
        if (path[i] == '.')
        {
            string s = path.substr(i, 2);
            if (s == "..")
            {
                l.pop_back();
                j = i + 3;
                i += 2;
            }
            else
            {
                j = i + 2;
                i++;
            }
            continue;
        }
        else if (path[i] == '/')
        {
            string s = path.substr(j, i - j);
            if (!s.empty())
                l.push_back(s);
            j = i + 1;
        }
    }
    if (j < path.length()) l.push_back(path.substr(j, path.length()));

    return l;
}

int main()
{
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    string fromPath, toPath;
    cin >> fromPath >> toPath;

    // if (fromPath == toPath)    return 0;

    list<string> fromList = makeList(fromPath);
    list<string> toList = makeList(toPath);
    list<string> tmpPath;
    list<string> finalPath;

    // cout << "fromPath formado por:" << endl;
    // for (string s : fromList){cout << s << endl;}
    // cout << "toPath formado por:" << endl;
    // for (string s : toList){cout << s << endl;}

    int it = 0;
    int fromSize = fromList.size();
    while (it < fromSize)
    {
        if (fromList.front() == toList.front())
        {
            tmpPath.push_back(fromList.front());
            fromList.pop_front();
            toList.pop_front();
        }
        else if (!tmpPath.empty())
        {
            finalPath.push_back("..");
        }
        ++it;

        //cout << "recuento from: " << fromList.size() << endl;
        //for (string s : fromList){cout << s << " ";}
        //cout << endl << "to: " << endl;
        //for (string s : toList){cout << s << " ";}
        //cout << endl << "tmp: " << endl;
        //for (string s : tmpPath){cout << s << " ";}
        //cout << endl;
    }

    //cout << "Result: " << endl;
    if (!toList.empty())
        for (string s : toList)
            finalPath.push_back(s);

    while (!finalPath.empty())
    {
        string s = finalPath.front();
        finalPath.pop_front();
        cout << s;
        if (!finalPath.empty())
            cout << "/";
    }

    return 0;
}