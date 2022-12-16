#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

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

void read_line_by_line(const string &str, string &title, string &abstract, string &author, string &biblio) {
    string line;
    title = "";
    abstract = "";
    author = "";
    biblio = "";
    int count = 0;
    istringstream f(str);
    bool title_found = false;
    bool break_condition = false;
    bool abstractCondition = false;
    bool authorCondition = false;
    while (getline(f, line)) {
        while (!title_found) {
            title += line;
            if (line.find("Abstract") != string::npos || count > 1 || line.find("ABSTRACT") != string::npos ||
                line.find("abstract") != string::npos) {
                title_found = true;
                abstractCondition = true;
            }
            if (line.empty()) {
                title_found = true;
            } else {
                count += 1;
                getline(f, line);
            }
        }
        while (!authorCondition) {
            author += line;
            if (line.empty() || line.find("Abstract") != string::npos || line.find("ABSTRACT") != string::npos ||
                line.find("abstract") != string::npos) {
                authorCondition = true;
                abstractCondition = true;
            } else {
                getline(f, line);
            }
        }
        if (line.find("abstract") != string::npos || line.find("Abstract") != string::npos || abstractCondition ||
            line.find("ABSTRACT") != string::npos) {
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
        if (line.find("References") != string::npos || line.find("REFERENCES") != string::npos ||
            line.find("references") != string::npos) {
            break_condition = false;
            while (getline(f, line)) {
                biblio += line;
            }
        }
    }
}

int dirExists(const char *path) //RETURN 0 if dir doesn't exists, else return 1
{
    struct stat info;
    if (stat(path, &info) != 0)
        return 0;
    else if (info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}

void pdf_to_tab(string &folder) {
    bool toXML = false;

    if (folder.find("-x") != string::npos) {
        //delete -x from the folder name and the character just before
        folder.erase(folder.find("-x") - 1, 3);
        toXML = true;
    }else if(folder.find("-t") != string::npos){
        //delete -t from the folder name
        folder.erase(folder.find("-t") - 1, 3);
    }else{
        cout << "Please specify the output format: -x for XML, -t for plain text" << endl;
        return;
    }

    string command = "find " + folder + " -name '*.pdf' > pdf_files.txt"; // find all pdf files in the folder

    system(command.c_str()); // execute the command

    ifstream file("pdf_files.txt");
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    istringstream f(content);
    string line, title, abstract, author, biblio;

    int verif = dirExists(("./" + folder +
                           "/outputs").c_str()); //check if output folder exists, if not create it, else, delete it then create it
    if (verif == 0) {
        system(("mkdir ./" + folder + "/outputs").c_str());
    }

    if (toXML) {
        while (getline(f, line)) {
            string name = line.erase(0, 13);
            if (name.find(".pdf") != string::npos) {
                name.erase(name.find(".pdf"), 4);
            }

            string output = "./" + folder + "/";

            string pdf_content = pdf_to_variable(output, name);

            read_line_by_line(pdf_content, title, abstract, author, biblio);

            output += + "outputs/" + name + ".xml";
            ofstream output_file(output);

            output_file << "<article>" << endl;
            output_file << "\t<preamble>" << line << "</preamble>" << endl;
            output_file << "\t<author>" << author << "</author>" << endl;
            output_file << "\t<titre>" << title << "</titre>" << endl;
            output_file << "\t<abstract>" << abstract << "</abstract>" << endl;
            output_file << "\t<biblio>" << biblio << "</biblio>" << endl;
            output_file << "</article>" << endl;
        }

    } else {
        while (getline(f, line)) {
            string name = line.erase(0, 13);
            if (name.find(".pdf") != string::npos) {
                name.erase(name.find(".pdf"), 4);
            }

            string output = "./" + folder + "/";

            string pdf_content = pdf_to_variable(output, name);
            read_line_by_line(pdf_content, title, abstract, author, biblio);

            output += + "outputs/" + name + ".txt";
            ofstream output_file(output);
            output_file << "FILE : " << replace_space_by_underscore(line) << "\n" << title << "\n" << abstract << "\n"
                        << endl;
        }
    }
    command = "rm pdf_files.txt";
    system(command.c_str());
}

int main() {
    // ask for the folder name
    string folder;
    cout << "Enter the folder name: ";
    getline(cin, folder);
    //cin dans take space
    pdf_to_tab(folder);
    return 0;
}