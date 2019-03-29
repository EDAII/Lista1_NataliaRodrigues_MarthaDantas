#include <iostream>
#include <fstream>
#include <string>

#include <json.hpp>

using namespace std;
using json = nlohmann::json;

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

vector <string> loadCities() {
    json data = cleanData(getData());

    vector <string> cities;

    for(int i = 0; i < data.size(); i++) {
        cities.push_back(stringPattern(data.at(i)["nome"]));
    }

    return cities;
}


int main() {
    vector <string> cities = loadCities();

    return 0;
}