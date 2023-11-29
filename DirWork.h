#ifndef DIRWORK
#define DIRWORK

#include "ReadingMethods.h"

enum fileType { none = 0, book = 1, publisher = 2 };

fileType defineFileType(const std::string& filename);

std::string findFile(std::string title, const fileType& type);
std::string askFullPath();
std::string findFolder(std::string title);
std::string space2underscore(std::string text);
std::string currentTime();

int movingArrow(int ymin, int ymax, int cur, int key);
void makeDirList(std::string folder, std::vector<std::string>& folderList);
void folderChoice(int menuSize, std::vector<std::string> folders, int& cur, bool& agree, std::string& folder);
void fileChoice(int begCoord, std::vector<std::string> folders, int& cur, std::string& filepath);
void makeDirNFilesList(std::string filepath, std::vector<std::string>& folderList, fileType type);

#endif //DIRWORK