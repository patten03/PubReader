#include "ReadingMethods.h"

std::string searchByBook(const std::string& keyword, std::fstream& file, const fileType type)
{
	file.clear();
	file.seekp(0, file.beg);
	std::string dataRes;
	bool found(false);
	std::string bookTemp = upperCase(keyword);
	while (not(found or file.eof()))
	{
		std::getline(file, dataRes);
		int beg = 0;
		int end = dataRes.find(";");
		//if (upperCase(dataRes).find(bookTemp) != -1) // search by each field, not only name of book
		//	found = true;
		if (upperCase(dataRes.substr(beg, end)).find(bookTemp) != -1)
			found = true;
	}

	if (found)
	{
		return dataRes;
	}
	else
	{
		if (type == book) return "None; None; None; None";
		if (type == publisher) return "None; None; None";
	}
}

std::vector<std::string> bookByKeyword(const std::string& keyword, std::fstream& file, const fileType type)
{
	file.clear();
	file.seekp(0, file.beg);
	std::vector<std::string> dataRes;
	std::string tempLine = upperCase(keyword);
	while (!file.eof())
	{
		std::string buff;
		std::getline(file, buff);
		if (upperCase(buff).find(tempLine) != -1) // search by each field, not only name of book
		{
			dataRes.push_back(buff.substr(0, buff.find(";")));
		}
	}
	return dataRes;
}

void outSearchedBooks(std::vector<std::string> bookList, std::fstream& booksFile, std::fstream& pubsFile)
{
	int width(30);

	if(bookList.size() == 0) throw std::exception("Ничего не найдено!");

	std::cout << "Вот что было найдено по вашему запросу:" << std::endl;
	std::cout << std::string(width, '-');
	std::cout << std::endl;

	for (const auto& bookname : bookList)
	{
		std::string tempBookLine = searchByBook(bookname, booksFile, book);
		std::string tempPublisherLine = searchByBook(bookname, pubsFile, publisher);

		Book Book;
		Publisher Publisher;

		Publisher.read(tempPublisherLine);
		Book.read(tempBookLine);

		outputCLI(Book, Publisher);
		std::cout << std::string(width, '-');
		std::cout << std::endl;
	}
}

void standartSettings()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::setlocale(LC_ALL, "ru");
}

void menu()
{
	std::cout << "Добро пожаловать в программу PublisherReader" << std::endl << std::endl;

	static std::string filename1("");
	static std::string filename2("");
	bool isChoosen(false);
	bool quit(false);

	while (!isChoosen)
	{
		quit = !chooseTwoFiles(filename1, filename2);

		isChoosen = true;
		while (!quit and isChoosen)
		{
			std::cout << "Выберите действие:" << std::endl;
			std::vector<std::string> menuQuestions{
				"Найти все данные по названию книги",
				"Сгруппировать информацию из двух файлов",
				"Возврат к выбору файлов"
			};
			ask(menuQuestions);
			int choice = inputChoice(menuQuestions.size());

			switch (choice)
			{
			case 1: search(filename1, filename2); break;
			case 2: combineFiles(filename1, filename2); break;
			case 3: isChoosen = quit = false; break;
			default: throw std::invalid_argument("Некорректный формат ввода!");
			}
		}
	}
}

std::string askString(const std::string& question)
{
	std::string res;
	std::cout << question << std::endl;
	std::cout << ">>";
	std::getline(std::cin, res);
	system("cls");
	return res;
}

void ask(const std::vector<std::string> choice)
{
	for (int i(0); i < choice.size(); i++)
	{
		std::cout << i + 1 << ". " << choice[i] << std::endl;
	}
}

bool isNumber(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

fileType defineFileType(const std::string& filename)
{
	fileType result(none);
	if (filename.find("{b}") != -1) result = book;
	if (filename.find("{p}") != -1) result = publisher;

	return result;
}

void parseNCLIout(const std::string& keyword, std::fstream& bStream, std::fstream& pStream)
{
	int width(30);

	bool found;

	std::string tempLine = upperCase(keyword);

	std::cout << "Вот что было найдено по вашему запросу:" << std::endl;
	std::cout << std::string(width, '-');
	std::cout << std::endl;

	found = parseByBook(tempLine, bStream, pStream);
	found += parseByPub(tempLine, bStream, pStream);

	if (!found)
	{
		system("cls");
		throw std::exception("По вашему запросу ничего не найдено!");
	}
		
}

//@return function return true if anythink found, otherwise false
bool parseByBook(const std::string& keyword, std::fstream& bStream, std::fstream& pStream)
{
	int width(30);

	bool res = false;

	bStream.clear();
	bStream.seekp(0, bStream.beg);

	while (!bStream.eof())
	{
		std::string buffB;
		std::getline(bStream, buffB);
		if (upperCase(buffB).find(keyword) != -1) // search by each field, not only name of book
		{
			res = true;

			Book curBook;
			curBook.read(buffB);

			Publisher curPub;
			curPub.name = curPub.address = curPub.surname = "None"; // when pub file have no matches

			std::string bookname = upperCase(curBook.name);

			pStream.clear();
			pStream.seekp(0, pStream.beg);
			while (!pStream.eof())
			{
				std::string buffP;
				std::getline(pStream, buffP);
				if (upperCase(buffP).find(bookname) != -1)
				{
					curPub.read(buffP);
					outputCLI(curBook, curPub);
					std::cout << std::string(width, '-') << std::endl;
				}
			}
			if (curPub.name == "None")
			{
				outputCLI(curBook, curPub);
				std::cout << std::string(width, '-') << std::endl;
			}
		}
	}
	return res;
}

//@return function return true if anythink found, otherwise false
bool parseByPub(const std::string& keyword, std::fstream& bStream, std::fstream& pStream)
{
	int width(30);

	bool res = false;

	pStream.clear();
	pStream.seekp(0, pStream.beg);

	while (!pStream.eof())
	{
		bool alredyFound(false);
		std::string buffP;
		std::getline(pStream, buffP);
		if (upperCase(buffP).find(keyword) != -1) // search by each field, not only name of book
		{

			res = true;

			Publisher curPub;
			curPub.read(buffP); 

			Book curBook;
			curBook.name = curBook.kind = curBook.organization = curBook.year = "None"; // when book file have no matches

			std::string bookname = upperCase(curPub.name);

			bStream.clear();
			bStream.seekp(0, bStream.beg);
			while (!bStream.eof())
			{
				std::string buffB;
				std::getline(bStream, buffB);
				
				if (upperCase(buffB).find(keyword) != -1)
				{
					alredyFound = true; // not repeat found data
					break;
				}

				if (upperCase(buffB).find(bookname) != -1 and !alredyFound)
				{
					curBook.read(buffB);
					outputCLI(curBook, curPub);
					std::cout << std::string(width, '-') << std::endl;
					break; // not search duplicates
				}
			}
			if (curBook.name == "None" and !alredyFound)
			{
				outputCLI(curBook, curPub);
				std::cout << std::string(width, '-') << std::endl;
			}
		}
	}
	return res;
}

std::string upperCase(const std::string& word)
{
	std::string res(word);
	for (int i(0); i < word.size(); i++)
	{
		res[i] = toupper(word[i]);
	}
	return res;
}

/*
@brief function return true, if files is choosen, otherwise false
*/
bool chooseTwoFiles(std::string& filename1, std::string& filename2)
{
	static std::vector<std::string> twoFilesQuestion{
		"Выбрать файл издания\n(Не выбран)",
		"Выбрать файл редакции\n(Не выбран)",
		"Выйти из программы"
	};

	bool isChoosen(false);
	bool quit(false);

	while (not(isChoosen or quit))
	{
		if (filename1 != "" and filename2 != "" and twoFilesQuestion.size() < 4)
			twoFilesQuestion.push_back("Работать с выбранными файлами");

		try
		{
			std::cout << "Выберите 2 файла для работы:" << std::endl;

			ask(twoFilesQuestion);
			int choice = inputChoice(twoFilesQuestion.size());

			switch (choice)
			{
			case 4: 
			{
				if (filename1 != "" and filename2 != "")
					isChoosen = true;
				else
					throw std::exception("Выберите все файлы!");
				break;
			}
			case 1:
			{
				filename1 = findFile("Выберите файл:", book);
				if (filename1 == "") break;
				int index = twoFilesQuestion[1 - 1].find("(");
				twoFilesQuestion[1 - 1].erase(index);
				twoFilesQuestion[1 - 1].append("(" + filename1 + ")"); // appending filename to menu
				break;
			}
			case 2: 
			{
				filename2 = findFile("Выберите файл:", publisher);
				if (filename1 == "") break;
				int index = twoFilesQuestion[2 - 1].find("(");
				twoFilesQuestion[2 - 1].erase(index);
				twoFilesQuestion[2 - 1].append("(" + filename2 + ")"); // appending filename to menu
				break;
			}
			case 3: quit = true; break;
			default: throw std::invalid_argument("Некорректный формат ввода!");
			}
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
	return isChoosen;
}

void search(const std::string &filename1, const std::string& filename2)
{
	std::fstream bookStream, publisherStream;
	bookStream.open(filename1, std::ios::in);
	publisherStream.open(filename2, std::ios::in);

	bool quite(false);
	while (not(quite))
	{
		try
		{
			std::string searchWord = askString("Введите ключевое слово, по которому хотите найти информацию");

			parseNCLIout(searchWord, bookStream, publisherStream);

			//std::vector<std::string> bookList1 = bookByKeyword(searchWord, bookStream, book);
			//std::vector<std::string> bookList2 = bookByKeyword(searchWord, publisherStream, publisher);
			//bookList1 = mergeBooks(bookList1, bookList2);

			//outSearchedBooks(bookList1, bookStream, publisherStream);

			std::vector<std::string> question{
				"Найти другое издание",
				"Выйти в меню"
			};
			ask(question);
			int answer(inputChoice(question.size()));
			if (answer == 2) quite = true;
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			std::vector<std::string> question{
				"Попытаться снова",
				"Выйти в меню"
			};
			ask(question);
			int answer(inputChoice(question.size()));
			if (answer == 2) quite = true;
			system("cls");
		}
	}

	bookStream.close();
	publisherStream.close();
}

void outputCLI(const Book& Book, const Publisher& Publisher)
{
	const int width(35);

	std::string tempName;
	if (Book.name == "None")
		tempName = Publisher.name;
	else
		tempName = Book.name;
	std::cout << std::left
		<< std::setw(width) << "   Название издания: " << tempName << std::endl
		<< std::setw(width) << "   Вид издания: " << Book.kind << std::endl
		<< std::setw(width) << "   Издающая организация: " << Book.organization << std::endl
		<< std::setw(width) << "   Год издания: " << Book.year << std::endl
		<< std::setw(width) << "   Адресс редакции: " << Publisher.address << std::endl
		<< std::setw(width) << "   Фамилия главного редактора: " << Publisher.surname << std::endl;
}

void parseNHTMLout(std::fstream& bStream, std::fstream& pStream, std::fstream& res)
{
	std::string top("<!DOCTYPE html><html><body><style> table, th, td{border:1px solid black;}</style><table>");

	res << top << headerRow();

	parseByBook(bStream, pStream, res);
	parseByPub(bStream, pStream, res);

	std::string floor("</table></body>");
	res << floor;
}

void parseByBook(std::fstream& bStream, std::fstream& pStream, std::fstream& res)
{
	bStream.clear();
	bStream.seekp(0, bStream.beg);

	while (!bStream.eof())
	{
		std::string buffB;
		std::getline(bStream, buffB);
		if (buffB == "")
			break; // exit if it last line
		Book curBook;
		curBook.read(buffB);

		Publisher curPub;
		curPub.name = curPub.address = curPub.surname = "None"; // when pub file have no matches

		std::string bookname = upperCase(curBook.name);

		pStream.clear();
		pStream.seekp(0, pStream.beg);
		while (!pStream.eof())
		{
			std::string buffP;
			std::getline(pStream, buffP);
			if (upperCase(buffP).find(bookname) != -1)
			{
				curPub.read(buffP);
				res << row(curBook, curPub);
			}
		}
		if (curPub.name == "None")
		{
			res << row(curBook, curPub);
		}
	}
}

void parseByPub(std::fstream& bStream, std::fstream& pStream, std::fstream& res)
{
	pStream.clear();
	pStream.seekp(0, pStream.beg);

	while (!pStream.eof())
	{
		bool alredyFound(false);
		std::string buffP;
		std::getline(pStream, buffP);

		Publisher curPub;
		curPub.read(buffP);

		Book curBook;
		curBook.name = curBook.kind = curBook.organization = curBook.year = "None"; // when book file have no matches

		std::string bookname = upperCase(curPub.name);

		bStream.clear();
		bStream.seekp(0, bStream.beg);
		while (!bStream.eof())
		{
			std::string buffB;
			std::getline(bStream, buffB);

			if (upperCase(buffB).find(upperCase(curPub.name)) != -1)
			{
				curBook.read(buffB);
				break;
			}
		}
		if (curBook.name == "None")
		{
			res << row(curBook, curPub);
		}
	}
}

void outputHTML(const std::vector<std::string> bookList, std::fstream& bStream, std::fstream& pStream, std::fstream& res)
{
	std::string top("<!DOCTYPE html><html><body><style> table, th, td{border:1px solid black;}</style><table>");

	res << top << headerRow();

	for (auto& name : bookList)
	{
		Book Book;
		Publisher Publisher;
		std::string tempBookLine = searchByBook(name, bStream, book);
		std::string tempPublisherLine = searchByBook(name, pStream, publisher);
		Book.read(tempBookLine);
		Publisher.read(tempPublisherLine);

		res << row(Book, Publisher);
	}

	std::string floor("</table></body>");
	res << floor;
}

std::string headerRow()
{
	std::string res;
	res = res + "<th>" + "Название издания" + "</th>";
	res = res + "<th>" + "Вид издания" + "</th>";
	res = res + "<th>" + "Издающая организация" + "</th>";
	res = res + "<th>" + "Год издания" + "</th>";
	res = res + "<th>" + "Адресс редакции" + "</th>";
	res = res + "<th>" + "Фамилия главного редактора" + "</th>";
	return res;
}

std::string row(const Book& B, const Publisher& P)
{
	std::string tempName;
	if (B.name == "None")
		tempName = P.name;
	else
		tempName = B.name;

	std::string res;
	res = res + "<tr>";
	res = res + "<td>" + tempName + "</td>";
	res = res + "<td>" + B.kind + "</td>";
	res = res + "<td>" + B.organization + "</td>";
	res = res + "<td>" + B.year + "</td>";
	res = res + "<td>" + P.address + "</td>";
	res = res + "<td>" + P.surname + "</td>";
	res = res + "</tr>";

	return res;
}

void combineFiles(const std::string& filename1, const std::string& filename2)
{
	bool approved(false);
	while (!approved)
	{
		try
		{
			std::string filenameMerged = askFullPath();
			if (filenameMerged == "None")
				return; // quit, if user want

			std::fstream bookStream, publisherStream, mergedStream;
			bookStream.open(filename1, std::ios::in);
			publisherStream.open(filename2, std::ios::in);
			mergedStream.open(filenameMerged, std::ios::out);
			if (!mergedStream.is_open())
				throw std::invalid_argument("Не удалось создать файл!\nПопробуйте выбрать другую папку или не использовать специальные символы.");

			//std::vector<std::string> bookList1 = recieveAllBooks(bookStream);
			//std::vector<std::string> bookList2 = recieveAllBooks(publisherStream);
			//mergeBooks(bookList1, bookList2); // all books merged to bookList1

			//outputHTML(bookList1, bookStream, publisherStream, mergedStream);

			parseNHTMLout(bookStream, publisherStream, mergedStream);

			bookStream.close();
			publisherStream.close();
			mergedStream.close();

			std::cout << "Ваши файлы сгруппированны, для продолжения нажмите Enter" << std::endl;
			_getch();
			system("cls");
			approved = true;
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
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

std::vector<std::string> recieveAllBooks(std::fstream& file)
{
	std::vector<std::string> res;
	while (not(file.eof()))
	{
		std::string temp;
		std::getline(file, temp, ';');
		temp = upperCase(temp);
		res.push_back(temp);
		file.ignore(256, '\n');
	}
	res.erase(res.end() - 1);
	file.clear();
	return res;
}

std::vector<std::string> mergeBooks(std::vector<std::string>& bookList1, std::vector<std::string>& bookList2)
{
	bookList1.insert(bookList1.end(), bookList2.begin(), bookList2.end());
	sort(bookList1.begin(), bookList1.end());
	bookList1.erase(unique(bookList1.begin(), bookList1.end()), bookList1.end()); // unique return as last element
	return bookList1;
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

void Publisher::read(std::string& line)
{
	std::string delimiter = "; ";
	size_t pos = 0;

	std::string buff[3];
	line = line + ";";
	for (auto& field : buff)
	{
		pos = line.find(delimiter);
		field = line.substr(0, pos);
		line.erase(0, pos + delimiter.length());
	}
	this->name = buff[0];
	this->address = buff[1];
	this->surname = buff[2].substr(0, buff[2].size() - 1);
}

void Book::read(std::string& line)
{
	std::string delimiter = "; ";
	size_t pos = 0;

	std::string buff[4];
	line = line + ";";
	for (auto& field : buff)
	{
		pos = line.find(delimiter);
		field = line.substr(0, pos);
		line.erase(0, pos + delimiter.length());
	}
	this->name = buff[0];
	this->kind = buff[1];
	this->organization = buff[2];
	this->year = buff[3].substr(0, buff[3].size() - 1);
}

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

int inputChoice(const int& end)
{
	int choiceInt = _getch();
	while (choiceInt <= '0' or choiceInt > char(end + '0'))
		choiceInt = _getch();

	choiceInt = choiceInt - '0';
	system("cls");
	return choiceInt;
}