#include "ReadingMethods.h"

std::ostream& operator<<(std::ostream& stream, const BookNPublisher& data)
{
	stream
		<< data.name << "; " 
		<< data.kind << "; "
		<< data.organization << "; "
		<< data.year << "; "
		<< data.address << "; "
		<< data.surname;
	return stream;
}

void standartSettings()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::setlocale(LC_ALL, "ru");
}

BookNPublisher::BookNPublisher()
{
	this->name = "None";
	this->kind = this->organization = this->year = "None";
	this->address = this->surname = "None";
}

BookNPublisher::BookNPublisher(const std::string& data)
{
	std::string tempData = data;
	std::vector<std::string> tempFields;

	std::string delimiter = "; ";
	size_t pos = 0;
	while ((pos = tempData.find(delimiter)) != -1) {
		tempFields.push_back(tempData.substr(0, pos));
		tempData.erase(0, pos + delimiter.length());
	}
	tempFields.push_back(tempData);

	this->name = tempFields[0];
	switch (tempFields.size())
	{
	case 3:
	{
		this->kind = "None";
		this->organization = "None";
		this->year = "None";
		this->address = tempFields[1];
		this->surname = tempFields[2];
		break;
	}
	case 4:
	{
		this->kind = tempFields[1];
		this->organization = tempFields[2];
		this->year = tempFields[3];
		this->address = "None";
		this->surname = "None";
		break;
	}
	case 6:
	{
		this->kind = tempFields[1];
		this->organization = tempFields[2];
		this->year = tempFields[3];
		this->address = tempFields[4];
		this->surname = tempFields[5];
		break;
	}
	default:
	{
		throw std::invalid_argument("Пустая строка или некорректные данные");
		break;
	}
	}
}

bool BookNPublisher::isEmpty()
{
	if (this->name == "None") return true;
	else return false;
}

void BookNPublisher::merge(const BookNPublisher& book, const BookNPublisher& publisher)
{
	if (book.name != "None")
		this->name = book.name;
	else
		this->name = publisher.name;
	this->kind = book.kind;
	this->organization = book.organization;
	this->year = book.year;
	this->address = publisher.address;
	this->surname = publisher.surname;
}

void menu()
{
	std::cout << "Добро пожаловать в программу PublisherReader" << std::endl;

	bool work = true;

	while (work)
	{
		std::cout << "Выберите задачу, которую хотите выполнить:" << std::endl;

		std::vector<std::string> menuQuestions{
			"Найти все данные по названию книги",
			"Сгруппировать информацию из двух файлов",
			"Выйти из программы"
		};

		ask(menuQuestions);
		int choice = inputChoice(menuQuestions.size());

		switch (choice)
		{
		case 1: search(); break;
		case 2: combineFiles(); break;
		case 3: work = false; break;
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
	filename1 = "";
	filename2 = "";

	std::cout << "Выберите 2 файла для работы с ними:" << std::endl;

	std::vector<std::string> twoFilesQuestion{
	"Далее",
	"Выбрать файл издания\n(Не выбран)",
	"Выбрать файл редакции\n(Не выбран)",
	"Выйти в главное меню"
	};

	bool isChoosen(false);
	bool quit(false);

	while (not(isChoosen or quit))
	{
		try
		{
			ask(twoFilesQuestion);
			int choice = inputChoice(twoFilesQuestion.size());

			switch (choice)
			{
			case 1: 
				if (filename1 != "" and filename2 != "") isChoosen = true; break; //required some exception
			case 2:
			{
				filename1 = findFile(book);
				int index = twoFilesQuestion[2 - 1].find("(");
				twoFilesQuestion[2 - 1].erase(index);
				twoFilesQuestion[2 - 1].append("(" + filename1 + ")"); // appending filename to menu
				break;
			}
			case 3: 
			{
				filename2 = findFile(publisher);
				int index = twoFilesQuestion[3 - 1].find("(");
				twoFilesQuestion[3 - 1].erase(index);
				twoFilesQuestion[3 - 1].append("(" + filename2 + ")"); // appending filename to menu
				break;
			}
			case 4: quit = true; break;
			}
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
	return isChoosen;
}

void search()
{
	std::string filename1, filename2;

	if (chooseTwoFiles(filename1, filename2)) // chooseTwoFiles define should it works or quits
	{
		std::fstream bookStream, publisherStream;
		bookStream.open(filename1, std::ios::in);
		publisherStream.open(filename2, std::ios::in);

		bool quite(false);
		while (not(quite))
		{
			try
			{
				std::string searchBook = askString("Введите название издания, по которому хотите найти информацию");
				BookNPublisher Book = searchInFile(searchBook, bookStream);
				BookNPublisher Publisher = searchInFile(searchBook, publisherStream);
				BookNPublisher FullData;
				FullData.merge(Book, Publisher);
				if (FullData.isEmpty()) throw std::exception("Ничего не найдено!");

				std::cout << "Вот что было найдено по вашему запросу:" << std::endl;
				std::cout << FullData << std::endl;

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
}

BookNPublisher searchInFile(const std::string& book, std::fstream& file)
{
	file.clear();
	file.seekp(0, file.beg);
	std::string dataRes;
	bool found(false);
	std::string bookTemp = upperCase(book);
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
		BookNPublisher res(dataRes);
		return res;
	}
	else
	{
		BookNPublisher res;
		return res;
	}
	
}

void combineFiles()
{
	std::string filename1, filename2;

	if (chooseTwoFiles(filename1, filename2)) // chooseTwoFiles define should it works or quits
	{
		//std::string filenameMerged = askString("Введите как хотите назвать файл");
		//filenameMerged = filenameMerged + ".txt";
		std::string filenameMerged = askFullPath();

		std::fstream bookStream, publisherStream, mergedStream;
		bookStream.open(filename1, std::ios::in);
		publisherStream.open(filename2, std::ios::in);
		mergedStream.open(filenameMerged, std::ios::out);

		std::vector<std::string> bookList1 = recieveAllBooks(bookStream);
		std::vector<std::string> bookList2 = recieveAllBooks(publisherStream);
		mergeBooks(bookList1, bookList2); // all books merged to bookList1

		for (int i(0); i < bookList1.size(); i++)
		{
			BookNPublisher Book = searchInFile(bookList1[i], bookStream);
			BookNPublisher Publisher = searchInFile(bookList1[i], publisherStream);
			BookNPublisher FullData;
			FullData.merge(Book, Publisher);
			mergedStream << FullData << std::endl;
		}

		bookStream.close();
		publisherStream.close();
		mergedStream.close();

		std::cout << "Ваши файлы сгруппированны, для продолжения нажмите Enter" << std::endl << ">>";
		std::cin.get();
		system("cls");
	}
}

std::string askFullPath()
{
	std::cout << "Выберите папку, где будете хранить файл" << std::endl;
	std::string folder = findFolder();
	std::string filename = askString("Введите название файла");

	filename = space2underscore(filename);
	filename = filename + "_" + currentTime();

	std::string fullPath = folder + "\\" + filename + ".txt";

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

			folderList.push_back("Назад");

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