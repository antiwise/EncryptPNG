#include <iostream>
#include "Files.h"
#include "Encrypt.h"

int main()
{
	// ������Կ
	aes_key key = { 0 };
	//aes_key key = { "abcdefghijklmnop" };
	//std::cout << "������16λ��Կ��" << std::endl;
	//std::cin.get((char *)&key[0], key.size());
	auto key_path = path::curdir()+"\\key.txt";

	std::ifstream infile;
	infile.open(key_path);   //���ļ����������ļ��������� 
	if ( !infile.is_open())   //��ʧ��,�����������Ϣ,����ֹ�������� 
	{
		std::cout << "==== ��Կ��ȡʧ��" << std::endl;
		system("pause");
		return 0;
	}

	char c;
	int count = 0;
	while (infile >> c)
	{
		//infile >> c;
		key[count] = c;
		count++;

	}
	infile.close();             //�ر��ļ������� 

	// Ѱ������pngͼƬ
	std::cout << "���Ҽ����ļ���" << path::curdir() << std::endl;
	std::vector<std::string> pngfiles;
	auto all_files = path::walk(path::curdir());

	for (auto filename : all_files)
	{
		if (path::splitext(filename)[1] == ".png")
		{
			pngfiles.push_back(filename);
		}
	}

	if ( pngfiles.begin() == pngfiles.end() )
	{
		std::cout << "û����Ҫ�����ļ�" << std::endl;
		system("pause");
		return 0;
	}

	// ȡ���·��
	/*auto absolute_path = path::curdir() + "\\encrypt\\";
	for (auto &filename : pngfiles)
	{
		filename = filename.substr(absolute_path.size(), filename.size());
	}*/

	std::cout << "��ʼ�����ļ�" << std::endl;

	// ͼƬ���ܲ���
	EncryptPNG(pngfiles, key);

	system("pause");

	return 0;
}