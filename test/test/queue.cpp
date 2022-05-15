#include <iostream>
#include <vector>
using namespace std;

class Person
{
public:
	Person(char* name, int age)
	{
		this->pName = new char[strlen(name) + 1];
		strcpy_s(this->pName, strlen(name) + 1, name);
		this->mAge = age;
	}
	// ���ӿ������캯��
	Person(const Person& p)
	{
		this->pName = new char[strlen(p.pName) + 1];
		strcpy_s(this->pName, strlen(p.pName) + 1, p.pName);
		this->mAge = p.mAge;
	}
	// ���� operator=������
	Person& operator=(Person& p)
	{
		if (this->pName == NULL)
		{
			delete[] this->pName;
		}
		this->pName = new char[strlen(p.pName) + 1];
		strcpy_s(this->pName, strlen(p.pName) + 1, p.pName);
		this->mAge = p.mAge;
		return *this;
	}
	~Person()
	{
		if (this->pName == NULL)
		{
			delete[] this->pName;
		}
	}
public:
	char* pName; // ָ������ǳ����������
	int mAge;
};
vector<Person> vPerson;

void test01()
{
	Person a("aaa", 20);
	//vector<Person> vPerson;
	vPerson.push_back(a);
}

int main()
{
	test01();
	cout << vPerson.at(0).pName << endl;
	cout << vPerson.at(0).mAge << endl;

	getchar();
	return 0;
}