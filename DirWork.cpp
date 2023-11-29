#include "DirWork.h"

void showDir(const std::vector<std::string>& dir)
{
	for (const auto& item : dir)
	{
		std::cout << "  " << item << std::endl;
	}
}

std::string findFolder(std::string title)
{
	bool agree = false;
	std::filesystem::path p = ".";
	std::string folder = std::filesystem::absolute(p).string();

	while (agree != true)
	{
		try
		{
			std::vector<std::string> folderList;
			std::cout << title << std::endl;
			std::vector<std::string> menu{
				"Выбрать текущую папку",
				"Выйти из папки",
				"Возврат в меню"
			};
			menu[0] = menu[0] + " (" + folder + ")";

			int begCoord = menu.size() + 1;
			int cur(begCoord); // arrow coordinate for folders

			makeDirList(folder, folderList);

			ask(menu);
			showDir(folderList);

			folderChoice(begCoord, folderList, cur, agree, folder);

			system("cls");
		}
		catch (const std::exception& ex)
		{
			std::cout << "Вы не можете выбрать этот файл или папку!" << std::endl;
			folder = folder.substr(0, folder.rfind("\\"));
		}
	}
	return folder;
}

void makeDirList(std::string folder, std::vector<std::string>& folderList)
{
	for (auto const& dirFolder : std::filesystem::directory_iterator(folder + "\\")) //maybe "\"
	{
		if (dirFolder.is_directory())
		{
			std::string path = dirFolder.path().string();
			path = path.substr(path.rfind("\\") + 1, path.size());

			folderList.push_back(path);
		}
	}
}

void folderChoice(int menuSize, std::vector<std::string> folders, int& cur, bool& agree, std::string& folder)
{
	movingArrow(menuSize, menuSize + folders.size(), cur, 0); // just show the arrow

	bool choosedMenu(false);
	while (!choosedMenu)
	{
		switch (int c = _getch())
		{
		case 224: cur = movingArrow(menuSize, menuSize + folders.size() - 1, cur, _getch()); break;
		case '1':
		{
			choosedMenu = true;
			cur = menuSize;
			agree = true; break; //save current folder
		}
		case '2':
		{
			choosedMenu = true;
			cur = menuSize;
			folder = folder.substr(0, folder.rfind("\\")); break; //return from last folder
		}
		case '3':
		{
			choosedMenu = true;
			cur = menuSize;
			agree = true;
			folder = "";
			break;
		}
		case 13:
		{
			if (folders.size() == 0)
				folder = folder.substr(0, folder.rfind("\\"));
			else
				folder = folder + "\\" + folders[cur - menuSize];
			choosedMenu = true;
			cur = menuSize;
			break;
		}
		default: break;
		}
	}
}

std::string findFile(std::string title, const fileType& type)
{
	std::filesystem::path p = ".";
	std::string filepath = std::filesystem::absolute(p).string();

	while (filepath.find(".txt") == -1 and filepath != "")
	{
		try
		{
			std::vector<std::string> folderList;
			makeDirNFilesList(filepath, folderList, type);
			std::cout << title << std::endl;

			std::cout << "Текущая папка - (" + filepath + ")" << std::endl;
			std::vector<std::string> menu{
				"Выйти из папки",
				"Возврат в меню"
			};

			int begCoord = menu.size() + 1 + 1;
			int cur(begCoord); // arrow coordinate for folders

			ask(menu);
			showDir(folderList);

			fileChoice(begCoord, folderList, cur, filepath);

			system("cls");
		}
		catch (const std::exception& ex)
		{
			std::cout << "Вы не можете выбрать этот файл или папку!" << std::endl;
			filepath = filepath.substr(0, filepath.rfind("\\"));
		}
	}
	return filepath;
}

void fileChoice(int begCoord, std::vector<std::string> folders, int& cur, std::string& filepath)
{
	movingArrow(begCoord, begCoord + folders.size(), cur, 0); // just show the arrow

	bool choosedMenu(false);
	while (!choosedMenu)
	{
		switch (int c = _getch())
		{	// arrow code
		case 224: cur = movingArrow(begCoord, begCoord + folders.size() - 1, cur, _getch()); break;
		case '1':
		{
			filepath = filepath.substr(0, filepath.rfind("\\")); //return from last folder
			choosedMenu = true;
			cur = begCoord;
			break;
		}
		case '2':
		{
			choosedMenu = true;
			cur = begCoord;
			filepath = ""; break; //stop process
		}
		case 13: //enter key
		{
			if (folders.size() == 0)
				filepath = filepath.substr(0, filepath.rfind("\\"));
			else
				filepath = filepath + "\\" + folders[cur - begCoord];
			choosedMenu = true;
			cur = begCoord;
			break;
		}
		default: break;
		}
	}
}

void makeDirNFilesList(std::string filepath, std::vector<std::string>& folderList, fileType type)
{
	for (auto const& dirFolder : std::filesystem::directory_iterator(filepath + "\\"))
	{
		if ((dirFolder.is_regular_file()
			and dirFolder.path().extension() == ".txt"
			and defineFileType(dirFolder.path().string()) == type)

			or dirFolder.is_directory())
		{
			std::string path = dirFolder.path().string();
			path = path.substr(path.rfind("\\") + 1, path.size());

			folderList.push_back(path);
		}
	}
}

int movingArrow(int ymin, int ymax, int cur, int key)
{
	DWORD dw;
	COORD here{ 0, cur };
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE)
	{
		printf("Invalid handle");
	}

	if (here.Y > ymin and key == 72)
	{
		WriteConsoleOutputCharacter(hStdOut, L"  ", 2, here, &dw);
		here.Y -= 1;
	}
	if (here.Y < ymax and key == 80)
	{
		WriteConsoleOutputCharacter(hStdOut, L"  ", 2, here, &dw);
		here.Y += 1;
	}

	WriteConsoleOutputCharacter(hStdOut, L">>", 2, here, &dw);

	return here.Y;
}

fileType defineFileType(const std::string& filename)
{
	fileType result(none);
	if (filename.find("{b}") != -1) result = book;
	if (filename.find("{p}") != -1) result = publisher;

	return result;
}

std::string askFullPath()
{
	std::string folder = findFolder("Выберите папку, где будете хранить файл");
	if (folder == "")
		return "None";

	std::string filename = askString("Введите название файла");

	filename = space2underscore(filename);
	filename = filename + "_" + currentTime();

	std::string fullPath = folder + "\\" + filename + ".html";

	system("cls");
	std::cout << "Файл под названием " << filename << " создан!" << std::endl;
	return fullPath;
}

std::string space2underscore(std::string text)
{
	std::replace(text.begin(), text.end(), ' ', '_');
	return text;
}

std::string currentTime()
{
	std::string res;

	std::time_t t = std::time(NULL);
	std::tm now{};
	localtime_s(&now, &t);

	std::string date = std::to_string(now.tm_mday) + "_"
		+ std::to_string(now.tm_mon + 1) + "_"
		+ std::to_string(now.tm_year + 1900);


	std::string clockTime;

	if (now.tm_min < 10)
	{
		clockTime = std::to_string(now.tm_hour) + "_"
			+ "0" + std::to_string(now.tm_min);
	}
	else
	{
		clockTime = std::to_string(now.tm_hour) + "_"
			+ std::to_string(now.tm_min);
	}

	res = clockTime + "_" + date;
	return res;
}