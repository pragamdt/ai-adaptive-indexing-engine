#include <bits/stdc++.h>
using namespace std;
const char nline = '\n';

int randnum(int max)
{
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> distr(0, max);
    return distr(gen);
}

int main(void)
{
    ofstream opener("data.csv");
    if(opener.is_open() == false)
    {
        cout << "No file" << endl;
        return 1;
    }

    for(int i = 1; i <= 900; i++)
    {
        int x = randnum(1000), y = randnum(1000), z = randnum(1000);
        char cp1 = (char)('a'+x%15), cp2 = (char)('a'+y%15), cp3 = (char)('a'+z%15);
        string name = "";
        name+=cp1, name+=cp2, name+=cp3;
        string country = "";
        country+=(char)('a'+y%5), country+=(char)('a'+z%5);
        opener << name << "," << min(min(x, y), z)%100<< "," << country << endl;
    }
    opener.close();
}