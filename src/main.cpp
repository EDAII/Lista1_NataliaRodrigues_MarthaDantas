#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <list>
#include<string>
#include <algorithm>

using namespace std;

int countTime();
void saveGuessing();
string stringPattern(string s);

int main() {
/*    thread timer (countTime);
    thread guessing (saveGuessing);

    timer.join();
    guessing.join();
*/
return 0;
}

int countTime() {
    int time = 60;
    while(time >= 0) {
        time--;
        sleep(1);
    }
    return 0;
}

void saveGuessing() {
    string guessing;
    list<string> guessing_list;
    
    cin >> guessing;
    guessing = stringPattern(guessing);

    guessing_list.push_back(guessing);
    cout << guessing_list.front() << endl;

}

string stringPattern(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper);

    return s;
}