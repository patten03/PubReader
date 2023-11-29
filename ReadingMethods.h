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

std::string searchByBook(const std::string& keyword, std::fstream& file, const fileType type);

void standartSettings();

bool isNumber(const std::string& s);

int inputChoice(const int& end);
void ask(const std::vector<std::string> choice);
bool chooseTwoFiles(std::string& filename1, std::string& filename2);
std::string findFile(std::string title, const fileType& type);
fileType defineFileType(const std::string& filename);
std::string upperCase(const std::string& word);
std::string askString(const std::string& question);

void menu();
void menu();

void search(const std::string& filename1, const std::string& filename2);

void parseNCLIout(const std::string& keyword, std::fstream& bStream, std::fstream& pStream);
bool parseByBook(const std::string& keyword, std::fstream& bStream, std::fstream& pStream);
bool parseByPub(const std::string& keyword, std::fstream& bStream, std::fstream& pStream);
void outputCLI(const Book& Book, const Publisher& Publisher);

std::string row(const Book& B, const Publisher& P);
std::string headerRow();
void outputHTML(const std::vector<std::string> bookList, std::fstream& bStream, std::fstream& pStream, std::fstream& res);
void parseNHTMLout(std::fstream& bStream, std::fstream& pStream, std::fstream& res);
void parseByBook(std::fstream& bStream, std::fstream& pStream, std::fstream& res);
void parseByPub(std::fstream& bStream, std::fstream& pStream, std::fstream& res);

void combineFiles(const std::string& filename1, const std::string& filename2);
std::vector<std::string> recieveAllBooks(std::fstream& file);
std::vector<std::string> mergeBooks(std::vector<std::string>& bookList1, std::vector<std::string>& bookList2);
std::string askFullPath();
std::string findFolder(std::string title);
std::string space2underscore(std::string text);
std::string currentTime();

int movingArrow(int ymin, int ymax, int cur, int key);
void makeDirList(std::string folder, std::vector<std::string>& folderList);
void folderChoice(int menuSize, std::vector<std::string> folders, int& cur, bool& agree, std::string& folder);
void fileChoice(int begCoord, std::vector<std::string> folders, int& cur, std::string& filepath);
void makeDirNFilesList(std::string filepath, std::vector<std::string>& folderList, fileType type);

#endif //READINGMETHODS