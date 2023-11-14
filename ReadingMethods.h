#ifndef READINGMETHODS
#define READINGMETHODS

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <windows.h>
#include <fstream>
#include <iomanip>

enum fileType { none = 0, book = 1, publisher = 2 };

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

std::string searchByKeyword(const std::string& keyword, std::fstream& file, const fileType type);

void standartSettings();

bool isNumber(const std::string& s);

int inputChoice(const int& end);
void ask(const std::vector<std::string> choice);
bool chooseTwoFiles(std::string& filename1, std::string& filename2);
std::string findFile(const fileType& type);
fileType defineFileType(const std::string& filename);
std::string upperCase(const std::string& word);
std::string askString(const std::string& question);

void menu();

void search();
void outputCLI(const Book& Book, const Publisher& Publisher);

void combineFiles();
std::vector<std::string> recieveAllBooks(std::fstream& file);
std::vector<std::string> mergeBooks(std::vector<std::string>& bookList1, std::vector<std::string>& bookList2);
std::string askFullPath();
std::string findFolder();
std::string space2underscore(std::string text);
std::string currentTime();

#endif //READINGMETHODS