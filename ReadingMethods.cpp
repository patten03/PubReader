#include "ReadingMethods.h"

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

void checkSpecialSymbols(const std::string& word)
{
	std::string forbiddenSymbols("\\/:*?<>\"|");
	for (int i(0); i < forbiddenSymbols.size(); i++)
	{
		if (word.find(forbiddenSymbols[i]) != -1)
			throw std::invalid_argument("Файл не может содержать \\/:*?<>\"|");
	}
}

std::string askString(const std::string& question)
{
	std::string res;
	std::cout << question << std::endl;
	std::cout << ">>";

	bool approved(false);
	while (!approved)
	{
		try
		{
			std::getline(std::cin, res);
			checkSpecialSymbols(res);
			approved = true;
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
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

//@brief function return true, if files is choosen, otherwise false
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
				std::string buff;
				buff = findFile("Выберите файл:", book);
				if (buff == "")
					break;
				filename1 = buff;
				int index = twoFilesQuestion[1 - 1].find("(");
				twoFilesQuestion[1 - 1].erase(index);
				twoFilesQuestion[1 - 1].append("(" + filename1 + ")"); // appending filename to menu
				break;
			}
			case 2: 
			{
				std::string buff;
				buff = findFile("Выберите файл:", publisher);
				if (buff == "")
					break;
				filename2 = buff;
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

int inputChoice(const int& end)
{
	int choiceInt = _getch();
	while (choiceInt <= '0' or choiceInt > char(end + '0'))
		choiceInt = _getch();

	choiceInt = choiceInt - '0';
	system("cls");
	return choiceInt;
}