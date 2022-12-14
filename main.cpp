#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include <string>
#include <iostream>
#include <fstream>

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
    string command = "pdftotext " + pdf_file + " " + variable_name + ".txt";
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


void create_xml_file(char *filename, string line, string title, string abstract, string output)
{

    ofstream fp;
    fp.open (output);
    fp << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
    //delete "CORPUS_TRAIN/" from the name of the file
    string name = line;
    name.erase(0, 12);
    name.erase(name.size() - 4, name.size());

    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp, "<root>\n");
        fprintf(fp, "\t<article>\n");
        fprintf(fp, "\t\t<preamble>%s</preamble>\n", name.c_str());
        fprintf(fp, "\t\t<titre>%s</titre>\n", title.c_str());
        fprintf(fp, "\t\t<auteur>auteur ?</auteur>\n");
        fprintf(fp, "\t\t<abstract>%s</abstract>\n", abstract.c_str());
        fprintf(fp, "\t\t<biblio>%s</biblio>\n", output.c_str());
    fprintf(fp, "\t</article>\n");
    fprintf(fp, "</root>\n");
    fclose(fp);
}

void pdf_to_tab(const string &folder) {
    char * filename;
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
        fprintf(stdout, "Processing %s\n", line.c_str());
        string pdf_content = pdf_to_variable(line);
        read_line_by_line(pdf_content, title, abstract);
        //output_file << "FILE : " << replace_space_by_underscore(line) << "\n" << title << "\n" << abstract << "\n" << endl;
        filename = (char *)malloc(100 * sizeof(char));
        filename = "";
        strcat(filename, replace_space_by_underscore(line).c_str());
        strcat(filename, ".xml");
        create_xml_file(filename,line, title, abstract,output);
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