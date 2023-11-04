#include "ReadingMethods.h"

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
	if (tempFields.size() == 4)
	{
		this->kind = tempFields[1];
		this->organization = tempFields[2];
		this->year = tempFields[3];
		this->address = "None";
		this->surname = "None";
	}
	else
	{
		this->kind = "None";
		this->organization = "None";
		this->year = "None";
		this->address = tempFields[1];
		this->surname = tempFields[2];
	}
}

void BookNPublisher::merge(const BookNPublisher& book, const BookNPublisher& publisher)
{
	this->name = book.name;
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


		bool found(false);
		bool quite(false);
		while (not(quite))
		{
			try
			{
				BookNPublisher Book = searchInFile("Краски", bookStream);
				BookNPublisher Publisher = searchInFile("Краски", publisherStream);
				BookNPublisher FullData;
				FullData.merge(Book, Publisher);
			}
			catch (std::exception& ex)
			{

			}
		}



		bookStream.close();
		publisherStream.close();
	}
}

BookNPublisher searchInFile(const std::string& book, std::fstream& file)
{

	std::string dataRes;
	bool found(false);
	std::string bookTemp = upperCase(book);
	while (not(found or file.eof()))
	{
		std::getline(file, dataRes);
		if (upperCase(dataRes).find(bookTemp) != -1)
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
	return;
}
