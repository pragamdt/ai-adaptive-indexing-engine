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
    ofstream opener("queries.csv");
    if(opener.is_open() == false)
    {
        cout << "No file" << endl;
        return 1;
    }

    for(int i = 1; i <= 200; i++)
    {
        int x = randnum(100), y = randnum(100), z = randnum(100);
        if(x%3 <= 1) opener << "r,";
        else opener << "p,";

        if(y%3 == 0)
        {
            opener << "age,";
            if(x%3 == 2) opener << (x+y+z)%100 << endl;
            else opener << (x+y+z)%100 << "," << ((x+y+z)%100+(max(max(x, y), z))*2) << endl;
        }
        else if(y%3 == 1){
            opener << "name,";
            string name = "";
            name+=(char)('a'+x%15), name+=(char)('a'+y%15), name+=(char)('a'+z%15);
            if(x%3 == 2) opener << name << endl;
            else{  
                opener << name << ",";
                string nameto = "";
                nameto+=(char)('a'+(x+y+z)%15), nameto+=(char)('a'+(y-z+200)%15), nameto+=(char)('a'+(y+z)%15);
                opener << max(nameto, name) << endl;
            } 
        }
        else{
            opener << "country,";
            string ctry = "";
            ctry+=(char)('a'+y%5), ctry+=(char)('a'+z%5);
            if(x%3 == 2) opener << ctry << endl;
            else{
                opener << ctry << ",";
                string ctryto = "";
                ctryto+=(char)max(('a'+x%5), ('a'+y%5)), ctryto+=(char)('a'+(x+y+z)%5);
                opener << max(ctryto, ctry) << endl;
            } 
        }
    }
    opener.close();
}