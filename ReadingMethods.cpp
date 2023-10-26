#include "ReadingMethods.h"

void standartSettings()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::setlocale(LC_ALL, "ru");
}

void menu()
{
	std::cout << "����� ���������� � ��������� PublisherReader" << std::endl;

	bool work = true;

	while (work)
	{
		std::cout << "�������� ������, ������� ������ ���������:" << std::endl;

		std::vector<std::string> menuQuestions{
			"����� ��� ������ �� �������� �����",
			"������������� ���������� �� ���� ������",
			"����� �� ���������"
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
			std::cout << "����� ������� �� ��������� ��� �� �������� ������!" << std::endl;
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

std::string findFile()
{
	//std::string folder = "C:";
	std::string filepath = ".";

	while (filepath.find(".txt") == -1)
	{
		try
		{
			std::vector<std::string> folderList;
			folderList.push_back("�����");

			for (auto const& dirFolder : std::filesystem::directory_iterator(filepath + "/")) //maybe "\"
			{
				//"../../ghj.txt"
				std::string path = dirFolder.path().string();
				path = path.substr(path.rfind("/") + 1, path.size());

				folderList.push_back(path);
				//folderList.push_back(dirFolder.path().string().substr();
			}

			std::cout << "������� ����� - (" + filepath + ")" << std::endl;
			ask(folderList);
			int choice = inputChoice(folderList.size());

			switch (choice)
			{
			case 1: filepath = filepath.substr(0, filepath.rfind("/")); break; //return from last folder
			default: filepath = filepath + "/" + folderList[choice - 1]; break;
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << "�� �� ������ ������� ���� ���� ��� �����!" << std::endl;
			filepath = filepath.substr(0, filepath.rfind("/"));
		}
	}

	return filepath;
}

void search()
{
	std::cout << "�������� ��� ����� ��� ������ ������" << std::endl;

	std::vector<std::string> searchQuestion{
		"������� ���� ���� <�������� �������> <��� �������> <�������� �����������> <��� ������� XXXX>",
		"������� ���� ���� <�������� �������> <����� ��������> <������� �������� ���������>",
		"����� ������ �� �������� �������",
		"����� � ������� ����"
	};
	
	ask(searchQuestion);
	int choice = inputChoice(searchQuestion.size());

}

void combineFiles()
{
	return;
}
