#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

string replace_space_by_underscore(string str) {
    for (char &i: str) {
        if (i == ' ') {
            i = '_';
        }
    }
    return str;
}

string pdf_to_variable(const string &folder, const string &variable_name) {
    string command = "pdftotext " + folder + variable_name + ".pdf " + folder + "outputs/" + variable_name + ".txt";
    system(command.c_str());
    ifstream file(folder + "outputs/" + variable_name + ".txt");
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    command = "rm " + folder + "outputs/" + variable_name + ".txt";
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
        //cout << line << endl;
        if (break_condition) {
            break;
        }
    }
}

int dirExists(const char *path) //RETURN 0 if dir doesn't exists, else return 1
{
    struct stat info;

    if(stat( path, &info ) != 0)
        return 0;
    else if(info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
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
    int verif = dirExists(("./" + folder + "/outputs").c_str()); //check if output folder exists, if not create it, else, delete it then create it
        if(verif == 0){
        system(("mkdir ./" + folder + "/outputs").c_str());
        }
        else{
            system(("rm -r ./" + folder + "/outputs").c_str());
            system(("mkdir ./" + folder + "/outputs").c_str());
        }
    while (getline(f, line)) {
        string name = line.erase(0, 13);
        if (name.find(".pdf") != string::npos) {
            name.erase(name.find(".pdf"), 4);
        }
        string output = "./" + folder + "/";
        string pdf_content = pdf_to_variable(output, name);
        read_line_by_line(pdf_content, title, abstract);
        output = output + "outputs/" + name + ".txt";
        ofstream output_file(output);
        output_file << "FILE : " << replace_space_by_underscore(line) << "\n" << title << "\n" << abstract << "\n" << endl;
        cout << "Success " << line << endl;
    }
    command = "rm pdf_files.txt";
    system(command.c_str());
    cout << "FIN ! Un dossier \"output\" a été créé dans le dossier spécifié."  << endl;
}

int main() {
    // ask for the folder name
    string folder;
    while(true){
        cout << "Enter the folder name: ";
        cin >> folder;
        if(dirExists(folder.c_str()) == 1)
            break;
        else{
            cout << "This folder doesn't exists, please enter a valid folder path\n";
        }

    }

    pdf_to_tab(folder);
    return 0;
}