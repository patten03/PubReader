#include "ReadingMethods.h"

void standartSettings()
{
	//установка русской кодировки для консоли
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::setlocale(LC_ALL, "ru");
}

void menu()
{
	std::cout << "Добро пожаловать в программу PublisherReader" << std::endl << std::endl
		<< "Программа позволяет работать с файлами типов издание и редакция и позволяет:" << std::endl
		<< "- находить информацию из двух файлов" << std::endl
		<< "- объединять файлы типа издание и редации" << std::endl << std::endl
		<< "Для работы с файлами они должны находится в той же директории, что и программа" << std::endl << std::endl;

	static std::string filename1("");
	static std::string filename2("");
	bool isChoosen(false);	//переменная цикла, которая отвечает за выход из программы
	bool quit(false);		//переменная цикла, которая отвечает за выход из выбора файлов

	while (!isChoosen) //цикл выбора файлов
	{
		quit = !chooseTwoFiles(filename1, filename2);

		isChoosen = true;
		while (!quit and isChoosen) //цикл выбора действий
		{
			std::cout << "Выберите действие:" << std::endl;
			std::vector<std::string> menuQuestions{
				"Найти все данные по ключевому слову",
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
			default: break;
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
			throw std::invalid_argument("Название файла не может содержать \\/:*?<>\"|");
	}
}

std::string askString(const std::string& question)
{
	std::string res;
	std::cout << question << std::endl;

	bool approved(false); //переменная цикла, становится true когда строка введена без ошибок
	while (!approved)
	{
		std::cout << ">>";
		std::getline(std::cin, res);
		if (res != "" and res != ";") //проверка ввода пользователя на пустую строку и специальный символ
			approved = true;
		else
		{
			system("cls");
			std::cout << question << std::endl;
		}
	}
	system("cls");
	return res;
}

std::string askFilename(const std::string& question)
{
	std::string res;
	std::cout << question << std::endl;

	bool approved(false); //переменная цикла, становится true когда строка введена без ошибок
	while (!approved)
	{
		try
		{
			std::cout << ">>";
			std::getline(std::cin, res);
			checkSpecialSymbols(res); //запрет на ввод символов \/:*?<>"|
			if (res != "")
				approved = true;
			else
			{
				system("cls");
				std::cout << question << std::endl;
			}
			if (res == "0") //выход из выбора названия файла
				res = "";
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

	bool found; //если что-то найдено, то true, иначе false

	std::string tempLine = upperCase(keyword);

	std::cout << "Вот что было найдено по вашему запросу:" << std::endl;
	std::cout << std::string(width, '-');
	std::cout << std::endl;

	//вызов поиска и определение found
	found = parseByBook(tempLine, bStream, pStream);
	found += parseByPub(tempLine, bStream, pStream);

	if (!found)
	{
		system("cls");
		throw std::exception("По вашему запросу ничего не найдено!");
	}
		
}

//@return функция возвращает true если что-то найдено, иначе false
bool parseByBook(const std::string& keyword, std::fstream& bStream, std::fstream& pStream)
{
	int width(30);

	bool res = false;

	bStream.clear();
	bStream.seekp(0, bStream.beg);

	while (!bStream.eof()) //цикл по файлу издания
	{
		std::string buffB;
		std::getline(bStream, buffB);
		if (upperCase(buffB).find(keyword) != -1) //поиск по каждому полю, не только по изданию
		{
			res = true;

			Book curBook;
			curBook.read(buffB);

			Publisher curPub;
			//возвращаемое значение при условии, когда в файле редакции ничего не найдено
			curPub.name = curPub.address = curPub.surname = "None"; 

			std::string bookname = upperCase(curBook.name);

			pStream.clear();
			pStream.seekp(0, pStream.beg);
			while (!pStream.eof()) //цикл по файлу редакций
			{
				std::string buffP;
				std::getline(pStream, buffP);

				//вывод всех данных при совпадении названий книг в двух файлах
				if (upperCase(buffP).find(bookname) != -1) 
				{
					curPub.read(buffP);
					outputCLI(curBook, curPub);
					std::cout << std::string(width, '-') << std::endl;
				}
			}
			//вывод всех данных в случае, когда в файле редакции ничего не найдено
			if (curPub.name == "None")
			{
				outputCLI(curBook, curPub);
				std::cout << std::string(width, '-') << std::endl;
			}
		}
	}
	return res;
}

//@return функция возвращает true если что-то найдено, иначе false
bool parseByPub(const std::string& keyword, std::fstream& bStream, std::fstream& pStream)
{
	int width(30);

	bool res = false;

	pStream.clear();
	pStream.seekp(0, pStream.beg);

	while (!pStream.eof()) //цикл по файлу редакции
	{
		bool alredyFound(false);
		std::string buffP;
		std::getline(pStream, buffP);
		if (upperCase(buffP).find(keyword) != -1) //поиск по каждому полю, не только по изданию
		{

			res = true;

			Publisher curPub;
			curPub.read(buffP); 

			Book curBook;
			//возвращаемое значение при условии, когда в файле редакции ничего не найдено
			curBook.name = curBook.kind = curBook.organization = curBook.year = "None";
			std::string bookname = upperCase(curPub.name);

			bStream.clear();
			bStream.seekp(0, bStream.beg);
			while (!bStream.eof()) //цикл по файлу изданий
			{
				std::string buffB;
				std::getline(bStream, buffB);
				
				if (upperCase(buffB).find(keyword) != -1)
				{
					alredyFound = true; //не повторять уже найденные через parseByBook данные 
					break;
				}

				if (upperCase(buffB).find(bookname) != -1 and !alredyFound)
				{
					curBook.read(buffB);
					outputCLI(curBook, curPub);
					std::cout << std::string(width, '-') << std::endl;
					break; //остановка поиска дубликатов
				}
			}
			//вывод всех данных в случае, когда в файле изданий ничего не найдено
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

//@return функция возвращает true, если файлы выбраны, иначе false
bool chooseTwoFiles(std::string& filename1, std::string& filename2)
{
	static std::vector<std::string> twoFilesQuestion{
		"Выбрать файл издания\n(Не выбран)",
		"Выбрать файл редакции\n(Не выбран)",
		"Выйти из программы"
	};

	bool isChoosen(false);	//true когда оба файла выбраны
	bool quit(false);		//true когда пользователь решил выйти

	while (not(isChoosen or quit))
	{
		if (filename1 != "" and filename2 != "" and twoFilesQuestion.size() < 4) 
			twoFilesQuestion.push_back("Работать с выбранными файлами"); //добавление в меню действия работы с файлами

		try
		{
			std::cout << "Выберите 2 файла для работы:" << std::endl;

			ask(twoFilesQuestion);
			int choice = inputChoice(twoFilesQuestion.size());

			switch (choice)
			{
			case 4: //вход в меню выбора действий
			{
				if (filename1 != "" and filename2 != "")
					isChoosen = true;
				break;
			}
			case 1: //выбор файла типа изданий
			{
				std::string buff;
				buff = findFile("Выберите файл:", book);
				if (buff == "")
					break;
				filename1 = buff;
				int index = twoFilesQuestion[1 - 1].find("(");
				twoFilesQuestion[1 - 1].erase(index);
				int beg = filename1.rfind("\\") + 1;
				int end = filename1.length();
				twoFilesQuestion[1 - 1].append("(" + filename1.substr(beg, end) + ")"); //добавление названия файла в меню
				break;
			}
			case 2: //выбор файла типа редакций
			{
				std::string buff;
				buff = findFile("Выберите файл:", publisher);
				if (buff == "")
					break;
				filename2 = buff;
				int index = twoFilesQuestion[2 - 1].find("(");
				twoFilesQuestion[2 - 1].erase(index);
				int beg = filename2.rfind("\\") + 1;
				int end = filename2.length();
				twoFilesQuestion[2 - 1].append("(" + filename2.substr(beg, end) + ")"); //добавление названия файла в меню
				break;
			}
			case 3: //выход из меню
			{
				quit = true;
				break;
			}
			default: break;
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

	bool quite(false); //переменная цикла, отвечает за прекращения поиска
	while (not(quite))
	{
		try
		{
			std::string searchWord = askString("Введите ключевое слово, по которому хотите найти информацию");

			parseNCLIout(searchWord, bookStream, publisherStream); //поиск совпадений с ключевым словом

			std::vector<std::string> question{
				"Найти другое издание",
				"Выйти в меню"
			};
			ask(question);
			int answer(inputChoice(question.size()));
			if (answer == 2) //выход из цикла
				quite = true;
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
			if (answer == 2) //выход из цикла
				quite = true;
			system("cls");
		}
	}

	bookStream.close();
	publisherStream.close();
}

void outputCLI(const Book& Book, const Publisher& Publisher)
{
	const int width(35);

	//все поля не обязательно заполнены, по этому
	//название издания определяется из того файла,
	//где оно есть
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
	//верхняя часть HTML файла
	std::string top("<!DOCTYPE html><html><body><style> table, th, td{border:1px solid black;}</style><table>");

	//названия полей
	res << top << headerRow(); 

	//содержание HTML файла
	parseByBook(bStream, pStream, res);
	parseByPub(bStream, pStream, res);

	//нижняя часть HTML файла
	std::string floor("</table></body>");
	res << floor;
}

void parseByBook(std::fstream& bStream, std::fstream& pStream, std::fstream& res)
{
	bStream.clear();
	bStream.seekp(0, bStream.beg);

	while (!bStream.eof()) //цикл по файлу издательства
	{
		std::string buffB;
		std::getline(bStream, buffB);
		if (buffB == "")
			break; //выход из файла если последняя строка пустая
		Book curBook;
		curBook.read(buffB);

		Publisher curPub;
		//возвращаемое значение при условии, когда в файле редакции ничего не найдено
		curPub.name = curPub.address = curPub.surname = "None";

		std::string bookname = upperCase(curBook.name);

		pStream.clear();
		pStream.seekp(0, pStream.beg);
		while (!pStream.eof()) //цикл по файлу редакции
		{
			std::string buffP;
			std::getline(pStream, buffP);
			if (upperCase(buffP).find(bookname) != -1)
			{
				curPub.read(buffP);
				res << row(curBook, curPub);
			}
		}
		//вывод всех данных в случае, когда в файле изданий ничего не найдено
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

	while (!pStream.eof()) //цикл по файлу редакции
	{
		bool alredyFound(false);
		std::string buffP;
		std::getline(pStream, buffP);

		Publisher curPub;
		curPub.read(buffP);

		Book curBook;
		//возвращаемое значение при условии, когда в файле редакции ничего не найдено
		curBook.name = curBook.kind = curBook.organization = curBook.year = "None";

		std::string bookname = upperCase(curPub.name);

		bStream.clear();
		bStream.seekp(0, bStream.beg);
		while (!bStream.eof()) //цикл по файлу изданий
		{
			std::string buffB;
			std::getline(bStream, buffB);
			
			if (upperCase(buffB).find(upperCase(curPub.name)) != -1) 
			{
				curBook.read(buffB);
				break; //исключение повторений 
			}
		}
		//вывод всех данных в случае, когда в файле изданий ничего не найдено
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
	//все поля не обязательно заполнены, по этому
	//название издания определяется из того файла,
	//где оно есть
	std::string tempName;
	if (B.name == "None")
		tempName = P.name;
	else
		tempName = B.name;
	if (P.name == ";" and B.name == "None")
	{
		system("cls");
		throw std::invalid_argument("Один из файлов является пустым и слияние невозможно");
	}

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
	bool approved(false); //переменная цикла, при выполненении всех действий без ошибок становится true
	while (!approved)
	{
		std::string filenameMerged;
		try
		{
			filenameMerged = askName(); //получение названия файла от пользователя
			if (filenameMerged == "") //выход из цикла набора названия файла
				approved = true;
			else
			{
				std::fstream bookStream, publisherStream, mergedStream;
				bookStream.open(filename1, std::ios::in);
				publisherStream.open(filename2, std::ios::in);
				mergedStream.open(filenameMerged, std::ios::out);

				if (!mergedStream.is_open())
					throw std::invalid_argument("Не удалось создать файл"); //для проверки открытия файла

				parseNHTMLout(bookStream, publisherStream, mergedStream); //слияние файлов

				bookStream.close();
				publisherStream.close();
				mergedStream.close();

				std::cout << "Ваши файлы сгруппированны, для продолжения нажмите Enter" << std::endl;
				_getch();
				system("cls");
				approved = true;
			}
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			system(("del \"" + filenameMerged + "\"").c_str());
			return; //выход из цикла из-за ошибки
		}
	}
}

//@brief инициализация структуры из строки
void Publisher::read(std::string& line)
{
	std::string delimiter = "; ";
	size_t pos = 0;

	std::string buff[3];
	line = line + ";";

	//разделение строки по символу "; "
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

//@brief инициализация структуры
void Book::read(std::string& line)
{
	std::string delimiter = "; ";
	size_t pos = 0;

	std::string buff[4];
	line = line + ";";

	//разделение строки по символу "; "
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

	//цикл прерывается только при нажатии клавиши от 1 до <end>
	while (choiceInt <= '0' or choiceInt > char(end + '0'))
		choiceInt = _getch();

	choiceInt = choiceInt - '0'; //преобразование значения цифры из char в int 
	system("cls");
	return choiceInt;
}