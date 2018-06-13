#include <iostream>
#include "Decrypt.h"
#include "Files.h"


int main(int argc, char *argv[])
{
	aes_key key = { 0 };
	//aes_key key = {"abcdefghijklmnop"};
// 	std::cout << "请输入16位密钥：" << std::endl;
// 	std::cin.get((char *)&key[0], key.size());

	auto key_path = path::curdir() + "\\key.txt";

	std::ifstream infile;
	infile.open(key_path);   //将文件流对象与文件连接起来 
	if (!infile.is_open())   //若失败,则输出错误消息,并终止程序运行 
	{
		std::cout << "==== 秘钥读取失败" << std::endl;
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
	infile.close();             //关闭文件输入流 

	std::vector<std::string> files;
	/*for (int i = 1; i < argc; ++i)
	{
		files.push_back(argv[i]);
	}
*/
	auto all_files = path::walk(path::curdir());

	for (auto filename : all_files)
	{
		if (path::splitext(filename)[1] == ".png")
		{
			files.push_back(filename);
		}
	}
	if (files.size() <= 0)
	{
		std::cout << "-->没有需要解密文件" << std::endl;
		return 0;
	}

	std::cout << "==>开始解密文件..." << std::endl;
	DecryptPNG(files, key);

	system("pause");

	return 0;
}