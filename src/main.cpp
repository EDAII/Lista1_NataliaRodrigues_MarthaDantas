#include <iostream>
#include <fstream>
#include <string>

#include <json.hpp>

using namespace std;
using json = nlohmann::json;

string getData();
json cleanData();
string stringPattern(string s);
vector <string> loadMunicipios();

int main() {
    vector <string> municipios = loadMunicipios();

    return 0;
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

vector <string> loadMunicipios() {
    json data = cleanData(getData());

    vector <string> municipios;

    for(int i = 0; i < data.size(); i++) {
        municipios.push_back(stringPattern(data.at(i)["nome"]));
    }

    return municipios;
}