#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <list>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <signal.h>
#include <sys/time.h>
#include <bits/stdc++.h>
#include <sys/resource.h>

#include <json.hpp>

using namespace std;
using json = nlohmann::json;

vector <string> loadMunicipios();
string getData();
json cleanData(string text);

void getEntries();
void doWhatDadDo(pid_t child);
void doWhatChildDo(pid_t dad);
void putFile(string entry);
void loadEntries();

int countTime();
void saveGuessing(list<pair<int,string>> guessing_list);
int sequencialSearch(vector<string> municipios, string guessing);
void countScore(vector<string> municipios, string guessing);
void moveToforward(vector<string> municipios, int position);
string stringPattern(string s);

void showReport(vector<string> municipios, list<pair<int, string>> guessing_list);
vector<pair<int, string>> getTopGuessed(list<pair<int, string>> guessing_list);
double calcTime(const struct rusage *b, const struct rusage *a);

int counter = 0;
list<string> entries;

int main() {
    list<pair<int, string>> guessing_list;
    vector <string> municipios = loadMunicipios();

    getEntries();
    loadEntries();
    
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

void getEntries(){
    pid_t child = fork();

    if(child > 0) {
        doWhatDadDo(child);
    }
    else {
        doWhatChildDo(getppid());
    }
}

void doWhatDadDo(pid_t child) {
    system("clear");
    cout << "\nVocê tem 30s para colocar o máximo de cidades que conseguir lembrar: \n\n";

    sleep(30);
    kill(child, SIGTERM);

}

void doWhatChildDo(pid_t dad) {
    string entry;

    while(1) {
        getline(cin, entry);
        putFile(entry);
    }
    
}

void putFile(string entry) {
    const char *c = entry.c_str();

    FILE *fp = fopen("../src/entries.txt", "a+");
    fprintf(fp, "%s\n", c);
    fclose(fp);
}

void loadEntries() {
    ifstream fp("../src/entries.txt");

    string entry;
    while(!fp.eof()){
        getline(fp, entry);
        entries.push_back(entry);
    }

    fp.close();
    remove("../src/entries.txt");
    
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

string stringPattern(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

void showReport(vector <string> municipios, list<pair<int, string>> guessing_list) {
    vector<pair<int,string>> top_guessed = getTopGuessed(guessing_list);

    int position = 0;

    struct rusage init_time, end_time;
    double time_search = 0;
    
    system("clear");
    cout << "\n\n=================================== REPORT ====================================" << "\n\n";
    cout << "\t\t\t   TOP GUESSED CITIES: " << "\n\n";
    cout << "\t\tNUMBER\tNAME\t\t\tTIME\n"; 

    for(int i = 0; i < 5; i++) {
        getrusage(RUSAGE_SELF, &init_time);
            position = sequencialSearch(municipios, top_guessed.at(i).second);
        getrusage(RUSAGE_SELF, &end_time);

        time_search = calcTime(&init_time, &end_time);

        cout << "\t\t" << i + 1 << "\t" << top_guessed.at(i).second << "\t\t\t" << time_search << endl;

    }

    cout << "\n\n\n===============================================================================" << "\n\n\n";
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

double calcTime(const struct rusage *b, const struct rusage *a){
    if(b == NULL || a == NULL)
        return 0;
    else
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                 (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec)) +
                ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                 (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec)))
                / 1000000.0);
}