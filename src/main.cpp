#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <list>
#include <algorithm>
#include <fstream>
#include <string>

#include <json.hpp>

using namespace std;
using json = nlohmann::json;

vector <string> loadMunicipios();
string getData();
json cleanData(string text);
string stringPattern(string s);

int countTime();
void saveGuessing(list<pair<int,string>> guessing_list);
int sequencialSearch(vector<string> municipios, string guessing);
void countScore(vector<string> municipios, string guessing);
void moveToforward(vector<string> municipios, int position);

void showReport(list<pair<int, string>> guessing_list);
vector<pair<int, string>> getTopGuessed(list<pair<int, string>> guessing_list);

int counter = 0;

int main() {
    list<pair<int, string>> guessing_list;
    vector <string> municipios = loadMunicipios();

    guessing_list.push_back(make_pair(1, "N"));
    guessing_list.push_back(make_pair(2, "N"));
    guessing_list.push_back(make_pair(3, "N"));
    guessing_list.push_back(make_pair(4, "N"));
    guessing_list.push_back(make_pair(5, "N"));

    showReport(guessing_list);
/*    thread timer (countTime);
    thread guessing (saveGuessing);

    timer.join();
    guessing.join();
*/
    return 0;
}

vector <string> loadMunicipios() {
    json data = cleanData(getData());

    vector <string> municipios;

    for(int i = 0; i < data.size(); i++) {
        municipios.push_back(stringPattern(data.at(i)["nome"]));
    }

    return municipios;
}

string getData() {
    ifstream ifs("../src/municipios.json");
    string content( (istreambuf_iterator<char>(ifs) ),
                    (istreambuf_iterator<char>()    ) );

    return content;
}

json cleanData(string text) {
    
    json::parser_callback_t cb = [](int depth, json::parse_event_t event, json & parsed) {
        if(event == json::parse_event_t::key and parsed == json("id")) {
            return false;
        }
        else if (event == json::parse_event_t::key and parsed == json("microrregiao")){
            return false;
        }
        else {
            return true;
        }
     };
    
    json j_filtered = json::parse(text, cb);

    return j_filtered;
}

string stringPattern(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
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

void showReport(list<pair<int, string>> guessing_list) {
    vector<pair<int,string>> top_guessed = getTopGuessed(guessing_list);
}

vector<pair<int, string>> getTopGuessed(list<pair<int, string>> guessing_list) {
    vector<pair<int,string>> top_guessed;
    pair<int, string> bigger;

    list<pair<int, string>>::iterator remove_element;

    for(int i = 0; i < 5; i++){
        pair<int, string> bigger = make_pair(0, "n");

        for (list<pair<int, string>>::iterator it = guessing_list.begin(); it != guessing_list.end(); ++it){
            if((*it).first > bigger.first) {
                bigger = (*it);
                remove_element = it;
            }
        }

        guessing_list.erase(remove_element);
        top_guessed.push_back(bigger);
    }

    return top_guessed;
}