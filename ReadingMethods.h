#ifndef READINGMETHODS
#define READINGMETHODS

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <windows.h>

void standartSettings();

bool isNumber(const std::string& s);

int inputChoice(const int& end);
void ask(const std::vector<std::string> choice);

void menu();

void search();
void combineFiles();

#endif //READINGMETHODS