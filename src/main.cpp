#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <list>
#include<string>
#include <algorithm>

using namespace std;

int countTime();
void saveGuessing(list<pair<int,string>> guessing_list);
string stringPattern(string s);
int sequencialSearch(vector<string> municipios, string guessing);
void countScore(vector<string> municipios, string guessing);
void moveToforward(vector<string> municipios, int position);

int counter = 0;

int main() {
    list<pair<int, string>> guessing_list;
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

void saveGuessing(list<pair<int,string>> guessing_list) {
    string guessing;
    
    cin >> guessing;
    guessing = stringPattern(guessing);

    guessing_list.push_back(make_pair(0, guessing));

}

string stringPattern(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

int sequencialSearch(vector<string> municipios, string guessing) {
    unsigned int i = 0;

    municipios.push_back(guessing);

    while(guessing != municipios[i]) {
        i++;
    }
    if(i < (municipios.size() - 1)) {
        return i;
    }
    else {
        return -1;
    }
    
}

void countScore(vector<string> municipios, string guessing) {
    int position = sequencialSearch(municipios, guessing);

    if(position > 0) {
        counter++;
        moveToforward(municipios, position);
    }
    else {
        // nothing to do
    }
}

void moveToforward(vector<string> municipios, int position) {
    string aux = municipios.front();

    municipios.front() = municipios[position];

    for(int i = position; i > 0; i--) {
        municipios[i] = municipios[i-1];
    }

    municipios[0] = aux;
}

