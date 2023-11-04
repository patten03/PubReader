#ifndef READINGMETHODS
#define READINGMETHODS

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <windows.h>
#include <fstream>

enum fileType { none = 0, book = 1, publisher = 2 };

class BookNPublisher
{
private:
	std::string name;
	std::string kind, organization, year;
	std::string address, surname;
public:
	BookNPublisher merge(const BookNPublisher& book, const BookNPublisher& publisher);
	BookNPublisher();
	BookNPublisher(const std::string& data);
};

void standartSettings();

bool isNumber(const std::string& s);

int inputChoice(const int& end);
void ask(const std::vector<std::string> choice);
bool chooseTwoFiles(std::string& filename1, std::string& filename2);
std::string findFile(const fileType& type);
fileType defineFileType(const std::string& filename);
std::string upperCase(const std::string& word);

void menu();

void search();
BookNPublisher searchInFile(const std::string& book, std::fstream& file);

void combineFiles();

#endif //READINGMETHODS