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

void printMenu();
void exitGame();
void playGame(vector <string> municipios);
void reportGame(vector<string> municipios);

vector <string> loadMunicipios();
string getData();
json cleanData(string text);

void getEntries();
void doWhatDadDo(pid_t child);
void doWhatChildDo(pid_t dad);
void putFile(string entry);
list<string> loadEntries();

void saveGuessing(list<pair<int,string>> guessing_list);
int sequencialSearch(vector<string> municipios, string guessing);
void countScore(vector<string> municipios, string guessing);
void moveToforward(vector<string> municipios, int position);
string stringPattern(string s);

void showReport(vector<string> municipios);
vector<pair<int, string>> getTopGuessed();
double calcTime(const struct rusage *b, const struct rusage *a);

int counter = 0;
list<pair<int, string>> guessing;

int main() {
    vector <string> municipios = loadMunicipios();

    int opcao = -1;

    while(1){
        printMenu();
        scanf("%d", &opcao);

        switch(opcao) {
            case 0:
                exitGame();
                break;
            case 1:
                playGame(municipios);
                break;
            case 2:
                reportGame(municipios);
                break;
        }
    }
    
    return 0;
}

void printMenu() {
    system("clear");
    cout << "\n\n==================================== MENU =====================================" << "\n\n";
    cout << "\t\t\t\t0 - Exit\n";
    cout << "\t\t\t\t1 - Play\n"; 
    cout << "\t\t\t\t2 - Report\n";
    cout << "\n\n===============================================================================" << "\n\n";
    cout << "Opção: ";

}

void exitGame(){
    system("clear");
    exit(0);
}

void playGame(vector <string> municipios) {
    list<string> entries;

    getEntries();
    entries = loadEntries();

    for (list<string>::iterator it = entries.begin(); it != entries.end(); ++it){
        countScore(municipios, (*it));
    }

    if(counter > 0)
        cout << "\n\nCongratulations! You hit " << counter << " cities!\n\n";
    else 
        cout << "\n\nYou can do better!\n\n";

    system("read -p 'Press Enter to continue...' var");

}

void reportGame(vector<string> municipios) {
    showReport(municipios);
    system("read -p 'Press Enter to continue...' var");
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
    cout << "\nYou have 30s to put as many cities as you can remember:\n\n";

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

list<string> loadEntries() {
    ifstream fp("../src/entries.txt");
    list<string> entries;

    string entry;
    while(!fp.eof()){
        getline(fp, entry);
        entries.push_back(stringPattern(entry));
    }

    fp.close();
    remove("../src/entries.txt");

    return entries;    
}

void saveGuessing(string name_searched) {

    if(guessing.empty() == false){
        for (list<pair<int, string>>::iterator it = guessing.begin(); it != guessing.end(); ++it){
            if((*it).second == name_searched) {
                (*it).first++;
            }
        }
        guessing.push_back(make_pair(1, name_searched));
    }
    else {
        guessing.push_back(make_pair(1, name_searched));
    }
}

int sequencialSearch(vector<string> municipios, string name_searched) {
    unsigned int i = 0;

    municipios.push_back(name_searched);

    while(name_searched != municipios[i]) {
        i++;
    }
    if(i < (municipios.size() - 1)) {
        return i;
    }
    else {
        return -1;
    }
    
}

void countScore(vector<string> municipios, string name_searched) {
    int position = sequencialSearch(municipios, name_searched);

    if(position > 0) {
        counter++;
        saveGuessing(name_searched);
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

void showReport(vector <string> municipios) {
    vector<pair<int,string>> top_guessed = getTopGuessed();

    int position = 0;

    struct rusage init_time, end_time;
    double time_search = 0;
    
    system("clear");
    cout << "\n\n=================================== REPORT ====================================" << "\n\n";
    cout << "\t\t\t   TOP GUESSED CITIES: " << "\n\n";
    cout << "\t\tNUMBER\tNAME\t\t\tTIME\t\n"; 

    for(int i = 0; i < top_guessed.size(); i++) {
        getrusage(RUSAGE_SELF, &init_time);
            position = sequencialSearch(municipios, top_guessed.at(i).second);
        getrusage(RUSAGE_SELF, &end_time);

        time_search = calcTime(&init_time, &end_time);

        cout << "\t\t" << i + 1 << "\t" << top_guessed.at(i).second << "\t\t\t" << time_search << "\t" << endl;

    }

    cout << "\n\n\n===============================================================================" << "\n\n\n";
}

vector<pair<int, string>> getTopGuessed() {
    vector<pair<int,string>> top_guessed;

    if(guessing.empty())
        return top_guessed;
    
    list<pair<int, string>> guessing_list = guessing;
    pair<int, string> bigger;

    list<pair<int, string>>::iterator remove_element;

    for(int i = 0; i < guessing.size() && i < 5; i++){
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