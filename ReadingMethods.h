#ifndef READINGMETHODS
#define READINGMETHODS

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <windows.h>
#include <fstream>
#include <iomanip>
#include <conio.h>
#include "DirWork.h"

struct Book
{
	void read(std::string& line);
	std::string name;
	std::string kind, organization, year;
};

struct Publisher
{
	void read(std::string& line);
	std::string name;
	std::string address, surname;
};

void standartSettings();

int inputChoice(const int& end);
void ask(const std::vector<std::string> choice);
bool chooseTwoFiles(std::string& filename1, std::string& filename2);
std::string upperCase(const std::string& word);
std::string askString(const std::string& question);

void menu();

void search(const std::string& filename1, const std::string& filename2);

void parseNCLIout(const std::string& keyword, std::fstream& bStream, std::fstream& pStream);
bool parseByBook(const std::string& keyword, std::fstream& bStream, std::fstream& pStream);
bool parseByPub(const std::string& keyword, std::fstream& bStream, std::fstream& pStream);
void outputCLI(const Book& Book, const Publisher& Publisher);

std::string row(const Book& B, const Publisher& P);
std::string headerRow();
void parseNHTMLout(std::fstream& bStream, std::fstream& pStream, std::fstream& res);
void parseByBook(std::fstream& bStream, std::fstream& pStream, std::fstream& res);
void parseByPub(std::fstream& bStream, std::fstream& pStream, std::fstream& res);

void combineFiles(const std::string& filename1, const std::string& filename2);

#endif //READINGMETHODS