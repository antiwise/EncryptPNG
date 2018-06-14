#pragma once
#include <vector>
#include "AES.h"

class CEncryptImage
{
public:

	/*
	* -2 创建解密文件失败
	* -1 文件夹不存在
	* 0 已经加密，直接拷贝
	* 1 加密成功
	*/
	static int EncryptPNG(const std::string filename, const aes_key &key, std::string filePath, std::string outPath);

	static void WriteFileData(const std::string &filename, std::ofstream &outstream, std::stringstream &block_info);

	/*
	* 0 打开失败
	* 1 已加密
	* 2 未加密
	*/
	static int EnFile(std::string filename, const aes_key &key);
};

