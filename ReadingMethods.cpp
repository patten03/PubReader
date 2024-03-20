#include "ReadingMethods.h"

void standartSettings()
{
	//��������� ������� ��������� ��� �������
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::setlocale(LC_ALL, "ru");
}

void menu()
{
	std::cout << "����� ���������� � ��������� PublisherReader" << std::endl << std::endl
		<< "��������� ��������� �������� � ������� ����� ������� � �������� � ���������:" << std::endl
		<< "- �������� ���������� �� ���� ������" << std::endl
		<< "- ���������� ����� ���� ������� � �������" << std::endl << std::endl
		<< "��� ������ � ������� ��� ������ ��������� � ��� �� ����������, ��� � ���������" << std::endl << std::endl;

	static std::string filename1("");
	static std::string filename2("");
	bool isChoosen(false);	//���������� �����, ������� �������� �� ����� �� ���������
	bool quit(false);		//���������� �����, ������� �������� �� ����� �� ������ ������

	while (!isChoosen) //���� ������ ������
	{
		quit = !chooseTwoFiles(filename1, filename2);

		isChoosen = true;
		while (!quit and isChoosen) //���� ������ ��������
		{
			std::cout << "�������� ��������:" << std::endl;
			std::vector<std::string> menuQuestions{
				"����� ��� ������ �� ��������� �����",
				"������������� ���������� �� ���� ������",
				"������� � ������ ������"
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
			throw std::invalid_argument("�������� ����� �� ����� ��������� \\/:*?<>\"|");
	}
}

std::string askString(const std::string& question)
{
	std::string res;
	std::cout << question << std::endl;

	bool approved(false); //���������� �����, ���������� true ����� ������ ������� ��� ������
	while (!approved)
	{
		std::cout << ">>";
		std::getline(std::cin, res);
		if (res != "" and res != ";") //�������� ����� ������������ �� ������ ������ � ����������� ������
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

	bool approved(false); //���������� �����, ���������� true ����� ������ ������� ��� ������
	while (!approved)
	{
		try
		{
			std::cout << ">>";
			std::getline(std::cin, res);
			checkSpecialSymbols(res); //������ �� ���� �������� \/:*?<>"|
			if (res != "")
				approved = true;
			else
			{
				system("cls");
				std::cout << question << std::endl;
			}
			if (res == "0") //����� �� ������ �������� �����
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

	bool found; //���� ���-�� �������, �� true, ����� false

	std::string tempLine = upperCase(keyword);

	std::cout << "��� ��� ���� ������� �� ������ �������:" << std::endl;
	std::cout << std::string(width, '-');
	std::cout << std::endl;

	//����� ������ � ����������� found
	found = parseByBook(tempLine, bStream, pStream);
	found += parseByPub(tempLine, bStream, pStream);

	if (!found)
	{
		system("cls");
		throw std::exception("�� ������ ������� ������ �� �������!");
	}
		
}

//@return ������� ���������� true ���� ���-�� �������, ����� false
bool parseByBook(const std::string& keyword, std::fstream& bStream, std::fstream& pStream)
{
	int width(30);

	bool res = false;

	bStream.clear();
	bStream.seekp(0, bStream.beg);

	while (!bStream.eof()) //���� �� ����� �������
	{
		std::string buffB;
		std::getline(bStream, buffB);
		if (upperCase(buffB).find(keyword) != -1) //����� �� ������� ����, �� ������ �� �������
		{
			res = true;

			Book curBook;
			curBook.read(buffB);

			Publisher curPub;
			//������������ �������� ��� �������, ����� � ����� �������� ������ �� �������
			curPub.name = curPub.address = curPub.surname = "None"; 

			std::string bookname = upperCase(curBook.name);

			pStream.clear();
			pStream.seekp(0, pStream.beg);
			while (!pStream.eof()) //���� �� ����� ��������
			{
				std::string buffP;
				std::getline(pStream, buffP);

				//����� ���� ������ ��� ���������� �������� ���� � ���� ������
				if (upperCase(buffP).find(bookname) != -1) 
				{
					curPub.read(buffP);
					outputCLI(curBook, curPub);
					std::cout << std::string(width, '-') << std::endl;
				}
			}
			//����� ���� ������ � ������, ����� � ����� �������� ������ �� �������
			if (curPub.name == "None")
			{
				outputCLI(curBook, curPub);
				std::cout << std::string(width, '-') << std::endl;
			}
		}
	}
	return res;
}

//@return ������� ���������� true ���� ���-�� �������, ����� false
bool parseByPub(const std::string& keyword, std::fstream& bStream, std::fstream& pStream)
{
	int width(30);

	bool res = false;

	pStream.clear();
	pStream.seekp(0, pStream.beg);

	while (!pStream.eof()) //���� �� ����� ��������
	{
		bool alredyFound(false);
		std::string buffP;
		std::getline(pStream, buffP);
		if (upperCase(buffP).find(keyword) != -1) //����� �� ������� ����, �� ������ �� �������
		{

			res = true;

			Publisher curPub;
			curPub.read(buffP); 

			Book curBook;
			//������������ �������� ��� �������, ����� � ����� �������� ������ �� �������
			curBook.name = curBook.kind = curBook.organization = curBook.year = "None";
			std::string bookname = upperCase(curPub.name);

			bStream.clear();
			bStream.seekp(0, bStream.beg);
			while (!bStream.eof()) //���� �� ����� �������
			{
				std::string buffB;
				std::getline(bStream, buffB);
				
				if (upperCase(buffB).find(keyword) != -1)
				{
					alredyFound = true; //�� ��������� ��� ��������� ����� parseByBook ������ 
					break;
				}

				if (upperCase(buffB).find(bookname) != -1 and !alredyFound)
				{
					curBook.read(buffB);
					outputCLI(curBook, curPub);
					std::cout << std::string(width, '-') << std::endl;
					break; //��������� ������ ����������
				}
			}
			//����� ���� ������ � ������, ����� � ����� ������� ������ �� �������
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

//@return ������� ���������� true, ���� ����� �������, ����� false
bool chooseTwoFiles(std::string& filename1, std::string& filename2)
{
	static std::vector<std::string> twoFilesQuestion{
		"������� ���� �������\n(�� ������)",
		"������� ���� ��������\n(�� ������)",
		"����� �� ���������"
	};

	bool isChoosen(false);	//true ����� ��� ����� �������
	bool quit(false);		//true ����� ������������ ����� �����

	while (not(isChoosen or quit))
	{
		if (filename1 != "" and filename2 != "" and twoFilesQuestion.size() < 4) 
			twoFilesQuestion.push_back("�������� � ���������� �������"); //���������� � ���� �������� ������ � �������

		try
		{
			std::cout << "�������� 2 ����� ��� ������:" << std::endl;

			ask(twoFilesQuestion);
			int choice = inputChoice(twoFilesQuestion.size());

			switch (choice)
			{
			case 4: //���� � ���� ������ ��������
			{
				if (filename1 != "" and filename2 != "")
					isChoosen = true;
				break;
			}
			case 1: //����� ����� ���� �������
			{
				std::string buff;
				buff = findFile("�������� ����:", book);
				if (buff == "")
					break;
				filename1 = buff;
				int index = twoFilesQuestion[1 - 1].find("(");
				twoFilesQuestion[1 - 1].erase(index);
				int beg = filename1.rfind("\\") + 1;
				int end = filename1.length();
				twoFilesQuestion[1 - 1].append("(" + filename1.substr(beg, end) + ")"); //���������� �������� ����� � ����
				break;
			}
			case 2: //����� ����� ���� ��������
			{
				std::string buff;
				buff = findFile("�������� ����:", publisher);
				if (buff == "")
					break;
				filename2 = buff;
				int index = twoFilesQuestion[2 - 1].find("(");
				twoFilesQuestion[2 - 1].erase(index);
				int beg = filename2.rfind("\\") + 1;
				int end = filename2.length();
				twoFilesQuestion[2 - 1].append("(" + filename2.substr(beg, end) + ")"); //���������� �������� ����� � ����
				break;
			}
			case 3: //����� �� ����
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

	bool quite(false); //���������� �����, �������� �� ����������� ������
	while (not(quite))
	{
		try
		{
			std::string searchWord = askString("������� �������� �����, �� �������� ������ ����� ����������");

			parseNCLIout(searchWord, bookStream, publisherStream); //����� ���������� � �������� ������

			std::vector<std::string> question{
				"����� ������ �������",
				"����� � ����"
			};
			ask(question);
			int answer(inputChoice(question.size()));
			if (answer == 2) //����� �� �����
				quite = true;
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			std::vector<std::string> question{
				"���������� �����",
				"����� � ����"
			};
			ask(question);
			int answer(inputChoice(question.size()));
			if (answer == 2) //����� �� �����
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

	//��� ���� �� ����������� ���������, �� �����
	//�������� ������� ������������ �� ���� �����,
	//��� ��� ����
	std::string tempName; 
	if (Book.name == "None")
		tempName = Publisher.name;
	else
		tempName = Book.name;

	std::cout << std::left
		<< std::setw(width) << "   �������� �������: " << tempName << std::endl
		<< std::setw(width) << "   ��� �������: " << Book.kind << std::endl
		<< std::setw(width) << "   �������� �����������: " << Book.organization << std::endl
		<< std::setw(width) << "   ��� �������: " << Book.year << std::endl
		<< std::setw(width) << "   ������ ��������: " << Publisher.address << std::endl
		<< std::setw(width) << "   ������� �������� ���������: " << Publisher.surname << std::endl;
}

void parseNHTMLout(std::fstream& bStream, std::fstream& pStream, std::fstream& res)
{
	//������� ����� HTML �����
	std::string top("<!DOCTYPE html><html><body><style> table, th, td{border:1px solid black;}</style><table>");

	//�������� �����
	res << top << headerRow(); 

	//���������� HTML �����
	parseByBook(bStream, pStream, res);
	parseByPub(bStream, pStream, res);

	//������ ����� HTML �����
	std::string floor("</table></body>");
	res << floor;
}

void parseByBook(std::fstream& bStream, std::fstream& pStream, std::fstream& res)
{
	bStream.clear();
	bStream.seekp(0, bStream.beg);

	while (!bStream.eof()) //���� �� ����� ������������
	{
		std::string buffB;
		std::getline(bStream, buffB);
		if (buffB == "")
			break; //����� �� ����� ���� ��������� ������ ������
		Book curBook;
		curBook.read(buffB);

		Publisher curPub;
		//������������ �������� ��� �������, ����� � ����� �������� ������ �� �������
		curPub.name = curPub.address = curPub.surname = "None";

		std::string bookname = upperCase(curBook.name);

		pStream.clear();
		pStream.seekp(0, pStream.beg);
		while (!pStream.eof()) //���� �� ����� ��������
		{
			std::string buffP;
			std::getline(pStream, buffP);
			if (upperCase(buffP).find(bookname) != -1)
			{
				curPub.read(buffP);
				res << row(curBook, curPub);
			}
		}
		//����� ���� ������ � ������, ����� � ����� ������� ������ �� �������
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

	while (!pStream.eof()) //���� �� ����� ��������
	{
		bool alredyFound(false);
		std::string buffP;
		std::getline(pStream, buffP);

		Publisher curPub;
		curPub.read(buffP);

		Book curBook;
		//������������ �������� ��� �������, ����� � ����� �������� ������ �� �������
		curBook.name = curBook.kind = curBook.organization = curBook.year = "None";

		std::string bookname = upperCase(curPub.name);

		bStream.clear();
		bStream.seekp(0, bStream.beg);
		while (!bStream.eof()) //���� �� ����� �������
		{
			std::string buffB;
			std::getline(bStream, buffB);
			
			if (upperCase(buffB).find(upperCase(curPub.name)) != -1) 
			{
				curBook.read(buffB);
				break; //���������� ���������� 
			}
		}
		//����� ���� ������ � ������, ����� � ����� ������� ������ �� �������
		if (curBook.name == "None")
		{
			res << row(curBook, curPub);
		}
	}
}

std::string headerRow()
{
	std::string res;
	res = res + "<th>" + "�������� �������" + "</th>";
	res = res + "<th>" + "��� �������" + "</th>";
	res = res + "<th>" + "�������� �����������" + "</th>";
	res = res + "<th>" + "��� �������" + "</th>";
	res = res + "<th>" + "������ ��������" + "</th>";
	res = res + "<th>" + "������� �������� ���������" + "</th>";
	return res;
}

std::string row(const Book& B, const Publisher& P)
{
	//��� ���� �� ����������� ���������, �� �����
	//�������� ������� ������������ �� ���� �����,
	//��� ��� ����
	std::string tempName;
	if (B.name == "None")
		tempName = P.name;
	else
		tempName = B.name;
	if (P.name == ";" and B.name == "None")
	{
		system("cls");
		throw std::invalid_argument("���� �� ������ �������� ������ � ������� ����������");
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
	bool approved(false); //���������� �����, ��� ������������ ���� �������� ��� ������ ���������� true
	while (!approved)
	{
		std::string filenameMerged;
		try
		{
			filenameMerged = askName(); //��������� �������� ����� �� ������������
			if (filenameMerged == "") //����� �� ����� ������ �������� �����
				approved = true;
			else
			{
				std::fstream bookStream, publisherStream, mergedStream;
				bookStream.open(filename1, std::ios::in);
				publisherStream.open(filename2, std::ios::in);
				mergedStream.open(filenameMerged, std::ios::out);

				if (!mergedStream.is_open())
					throw std::invalid_argument("�� ������� ������� ����"); //��� �������� �������� �����

				parseNHTMLout(bookStream, publisherStream, mergedStream); //������� ������

				bookStream.close();
				publisherStream.close();
				mergedStream.close();

				std::cout << "���� ����� ��������������, ��� ����������� ������� Enter" << std::endl;
				_getch();
				system("cls");
				approved = true;
			}
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			system(("del \"" + filenameMerged + "\"").c_str());
			return; //����� �� ����� ��-�� ������
		}
	}
}

//@brief ������������� ��������� �� ������
void Publisher::read(std::string& line)
{
	std::string delimiter = "; ";
	size_t pos = 0;

	std::string buff[3];
	line = line + ";";

	//���������� ������ �� ������� "; "
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

//@brief ������������� ���������
void Book::read(std::string& line)
{
	std::string delimiter = "; ";
	size_t pos = 0;

	std::string buff[4];
	line = line + ";";

	//���������� ������ �� ������� "; "
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

	//���� ����������� ������ ��� ������� ������� �� 1 �� <end>
	while (choiceInt <= '0' or choiceInt > char(end + '0'))
		choiceInt = _getch();

	choiceInt = choiceInt - '0'; //�������������� �������� ����� �� char � int 
	system("cls");
	return choiceInt;
}