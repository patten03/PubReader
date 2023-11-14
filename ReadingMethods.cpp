#include "ReadingMethods.h"

std::string searchByKeyword(const std::string& keyword, std::fstream& file, const fileType type)
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

void standartSettings()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::setlocale(LC_ALL, "ru");
}

void menu()
{
	std::cout << "Добро пожаловать в программу PublisherReader" << std::endl;

	bool work = true;

	while (work)
	{
		std::cout << "Выберите задачу, которую хотите выполнить:" << std::endl;

		std::vector<std::string> menuQuestions{
			"Выбрать два файла для поиска данных",
			"Выйти из программы"
		};

		ask(menuQuestions);
		int choice = inputChoice(menuQuestions.size());

		switch (choice)
		{
		case 1: subMenu(); break;
		case 2: work = false; break;
		default: throw std::invalid_argument("Некорректный формат ввода!");
		}
	}
}

void subMenu()
{
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

int inputChoice(const int& end)
{
	bool incorrectInput = true;
	std::string choiceString;
	int choiceInt;
	std::exception ex;

	while (incorrectInput)
	{
		try
		{
			std::cout << ">>";
			std::getline(std::cin, choiceString);

			choiceInt = std::stoi(choiceString);

			if (choiceInt >= 1 and choiceInt <= end and isNumber(choiceString))
				incorrectInput = false;
			else
				throw ex;
		}
		catch (const std::exception& ex)
		{
			std::cout << "Выбор выходит из диапазона или не является числом!" << std::endl;
		}
	}

	system("cls");

	return choiceInt;
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

std::string findFile(const fileType& type)
{
	//std::string folder = "C:";
	std::filesystem::path p = ".";
	std::string filepath = std::filesystem::absolute(p).string();

	while (filepath.find(".txt") == -1)
	{
		try
		{
			std::vector<std::string> folderList;
			folderList.push_back("Назад");

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

			std::cout << "Текущая папка - (" + filepath + ")" << std::endl;
			ask(folderList);
			int choice = inputChoice(folderList.size());

			switch (choice)
			{
			case 1: filepath = filepath.substr(0, filepath.rfind("\\")); break; //return from last folder
			default: filepath = filepath + "\\" + folderList[choice - 1]; break;
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << "Вы не можете выбрать этот файл или папку!" << std::endl;
			filepath = filepath.substr(0, filepath.rfind("\\"));
		}
	}

	return filepath;
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
		"Выйти в главное меню"
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
				filename1 = findFile(book);
				int index = twoFilesQuestion[1 - 1].find("(");
				twoFilesQuestion[1 - 1].erase(index);
				twoFilesQuestion[1 - 1].append("(" + filename1 + ")"); // appending filename to menu
				break;
			}
			case 2: 
			{
				filename2 = findFile(publisher);
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
			std::string searchWord = askString("Введите название издания, по которому хотите найти информацию");

			Book Book;
			Publisher Publisher;

			std::string tempBookLine = searchByKeyword(searchWord, bookStream, book);
			std::string tempPublisherLine = searchByKeyword(searchWord, publisherStream, publisher);

			Publisher.read(tempPublisherLine);
			Book.read(tempBookLine);

			if (Book.name == "None" and Publisher.name == "None") throw std::exception("Ничего не найдено!");

			outputCLI(Book, Publisher);

			std::vector<std::string> question{
				"Найти другое издание",
				"Выйти в главное меню"
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
				"Выйти в главное меню"
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

	std::cout << "Вот что было найдено по вашему запросу:" << std::endl;

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

void combineFiles(const std::string& filename1, const std::string& filename2)
{
	bool approved(false);
	while (!approved)
	{
		try
		{
			std::string filenameMerged = askFullPath();
			if (filenameMerged == "None") return;

			std::fstream bookStream, publisherStream, mergedStream;
			bookStream.open(filename1, std::ios::in);
			publisherStream.open(filename2, std::ios::in);
			mergedStream.open(filenameMerged, std::ios::out);
			if (!mergedStream.is_open()) throw std::invalid_argument("Не удалось создать файл!\nПопробуйте выбрать другую папку или не использовать специальные символы.");

			std::vector<std::string> bookList1 = recieveAllBooks(bookStream);
			std::vector<std::string> bookList2 = recieveAllBooks(publisherStream);
			mergeBooks(bookList1, bookList2); // all books merged to bookList1

			for (auto &name: bookList1)
			{
				Book Book;
				Publisher Publisher;
				std::string tempBookLine = searchByKeyword(name, bookStream, book);
				std::string tempPublisherLine = searchByKeyword(name, publisherStream, publisher);
				Book.read(tempBookLine);
				Publisher.read(tempPublisherLine);

				std::string tempName;
				if (Book.name == "None")
					tempName = Publisher.name;
				else
					tempName = Book.name;

				mergedStream
					<< tempName << std::endl
					<< Book.kind << std::endl
					<< Book.organization << std::endl
					<< Book.year << std::endl
					<< Publisher.address << std::endl
					<< Publisher.surname << std::endl << std::endl;
			}

			bookStream.close();
			publisherStream.close();
			mergedStream.close();

			std::cout << "Ваши файлы сгруппированны, для продолжения нажмите Enter" << std::endl << ">>";
			std::cin.get();
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
	std::cout << "Выберите папку, где будете хранить файл" << std::endl;
	std::string folder = findFolder();
	if (folder == "")
		return "None";

	std::string filename = askString("Введите название файла");

	filename = space2underscore(filename);
	filename = filename + "_" + currentTime();

	std::string fullPath = folder + "\\" + filename + "{a}.txt";

	system("cls");
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

std::string findFolder()
{
	bool agree = false;
	//std::string folder = "C:";
	std::filesystem::path p = ".";
	std::string folder = std::filesystem::absolute(p).string();

	while (agree != true)
	{
		try
		{
			std::vector<std::string> folderList;
			folderList.push_back("Выбрать текущую папку");
			folderList[0] = folderList[0] + " (" + folder + ")";

			folderList.push_back("Выйти из папки");
			folderList.push_back("Возврат в меню");

			for (auto const& dirFolder : std::filesystem::directory_iterator(folder + "\\")) //maybe "\"
			{
				//"../../ghj.txt"
				if (dirFolder.is_directory())
				{
					std::string path = dirFolder.path().string();
					path = path.substr(path.rfind("\\") + 1, path.size());

					folderList.push_back(path);
				}
			}

			ask(folderList);
			int choice = inputChoice(folderList.size());

			switch (choice)
			{
			case 1: agree = true; break; //save current folder
			case 2: folder = folder.substr(0, folder.rfind("\\")); break; //return from last folder
			case 3:
			{
				agree = true;
				folder = "";
				break;
			}
			default: folder = folder + "\\" + folderList[choice - 1]; break;
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << "Вы не можете выбрать этот файл или папку!" << std::endl;
			folder = folder.substr(0, folder.rfind("\\"));
		}
	}
	return folder;
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