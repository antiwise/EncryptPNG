#include <iostream>
#include "Decrypt.h"
#include "Files.h"


int main(int argc, char *argv[])
{
	aes_key key = { 0 };
	std::cout << "请输入16位密钥：" << std::endl;
	std::cin.get((char *)&key[0], key.size());

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