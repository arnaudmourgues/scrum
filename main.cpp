#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string replace_space_by_underscore(string str) {
    for (char &i: str) {
        if (i == ' ') {
            i = '_';
        }
    }
    return str;
}

string pdf_to_variable(const string &pdf_file) {
    string variable_name = replace_space_by_underscore(pdf_file);
    string command = "pdftotext -colspacing 0.7 " + pdf_file + " " + variable_name + ".txt";
    system(command.c_str());
    ifstream file(variable_name + ".txt");
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    command = "rm " + variable_name + ".txt";
    system(command.c_str());
    return content;
}

void read_line_by_line(const string &str, string &title, string &abstract) {
    string line;
    title = "";
    abstract = "";
    int count = 0;
    istringstream f(str);
    bool title_found = false;
    bool break_condition = false;
    bool abstractCondition = false;
    while (getline(f, line)) {
        while (!title_found) {
            title += line;
            if(line.find("Abstract") != string::npos || count > 1 || line.find("ABSTRACT") != string::npos || line.find("abstract") != string::npos){
                title_found = true;
                abstractCondition = true;
            }
            if (line.empty() ) {
                title_found = true;
            } else {
                count += 1;
                getline(f, line);
            }
        }
        if (line.find("abstract") != string::npos || line.find("Abstract") != string::npos || abstractCondition || line.find("ABSTRACT") != string::npos) {
            while (!break_condition) {
                abstract += line;
                if (line.empty() || line.find("1.") != string::npos || line.find("Introduction") != string::npos ||
                    line.find("INTRODUCTION") != string::npos || line.find("Introduction") != string::npos ||
                    line.find("introduction") != string::npos) {
                    break_condition = true;
                } else {
                    getline(f, line);
                }
            }
        }
        cout << line << endl;
        if (break_condition) {
            break;
        }
    }
}

void pdf_to_tab(const string &folder) {
    string command = "find " + folder + " -name '*.pdf' > pdf_files.txt";
    system(command.c_str());
    ifstream file("pdf_files.txt");
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    istringstream f(content);
    string line;
    string title;
    string abstract;
    string output = "output" + folder + ".txt";
    ofstream output_file(output);
    while (getline(f, line)) {
        string pdf_content = pdf_to_variable(line);
        read_line_by_line(pdf_content, title, abstract);
        output_file << "FILE : " << replace_space_by_underscore(line) << "\n" << title << "\n" << abstract << "\n" << endl;
    }
    command = "rm pdf_files.txt";
    system(command.c_str());
    cout << "FIN ! Le nom du fichier output est : " << output << endl;
}

int main() {
    // ask for the folder name
    string folder;
    cout << "Enter the folder name: ";
    cin >> folder;
    pdf_to_tab(folder);
    return 0;
}