#ifndef READINGMETHODS
#define READINGMETHODS

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <windows.h>
#include <fstream>

enum fileType { none = 0, book = 1, publisher = 2 };

void standartSettings();

bool isNumber(const std::string& s);

int inputChoice(const int& end);
void ask(const std::vector<std::string> choice);
bool chooseTwoFiles(std::string& filename1, std::string& filename2);
std::string findFile(const fileType& type);
fileType defineFileType(const std::string& filename);

void menu();

void search();

void combineFiles();

#endif //READINGMETHODS