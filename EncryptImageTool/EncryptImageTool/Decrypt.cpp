#include "stdafx.h"
#include "Decrypt.h"

#include <fstream>
#include <iostream>


// 解密PNG图片
int CDecrypt::DecryptPNG(std::string &filename, const aes_key &key, std::string strEnPath, std::string strDePath)
{
	// 取出相对路径
	auto filePath = strEnPath + "\\";
	auto outFile = filename.substr(filePath.size(), filename.size());

	// 绝对路径
	std::string out_path = strDePath + "\\" + outFile;

	if (Tool::filedir(out_path) == -1)
	{
		Tool::EnToolLog("[path error ] 文件夹不存在 " + out_path);
		return -1;
	}

	// 如果已经解密直接拷贝
	if ( DeFile(filename, key) == 2)
	{
		Tool::EnToolLog("[de file ] 未加密 " + filename);
		Tool::copy(filename, out_path);
		return 0;
	}

	std::ifstream in_file(filename, std::ios::binary | std::ios::ate);
	if (!in_file.is_open())
	{
		Tool::EnToolLog("[error open] 打开失败 " + filename);
		return -3;
	}

	// 读取数据块位置
	uint64_t end_pos = in_file.tellg();
	in_file.seekg(end_pos - sizeof(uint64_t));
	uint64_t block_start_pos = *reinterpret_cast<uint64_t *>(&(Tool::ReadSome<sizeof(uint64_t)>(in_file)[0]));
	in_file.seekg(block_start_pos);

	// 解密数据块信息
	auto block_info = Tool::ReadLarge(in_file, uint64_t(end_pos - sizeof(uint64_t)-block_start_pos));
	std::string sssaasd = block_info.str();
	Tool::DecryptBlock(block_info, key);

	// 验证数据块内容
	auto block_head = Tool::ReadSome<sizeof(BLOCK_HEAD)>(block_info);
	for (unsigned int i = 0; i < block_head.size(); ++i)
	{
		if (block_head[i] != BLOCK_HEAD[i])
		{
			Tool::EnToolLog("秘钥错误");
			return -4;
		}
	}

	std::ofstream out_file(out_path, std::ios::binary);
	if (!out_file.is_open())
	{
		Tool::EnToolLog("[de error]创建失败" + filename);
		return -5;
	}

	Tool::EnToolLog("[deing]正在解密" + filename);

	// 写入文件头
	Tool::WriteToSteam(HEAD_DATA, sizeof(HEAD_DATA), out_file);

	// 读取数据块
	//uint64_t read_size = 0;
	while (true)
	{
		// 读取数据块信息
		Block block;
		memcpy(&block, &(Tool::ReadSome<sizeof(Block)>(block_info)[0]), sizeof(Block));
		if (block_info.eof())
		{
			out_file.clear();
			std::cerr << "the %s file format error!" << std::endl;
			return -6;
			//break;
		}

		// 写入数据块长度
		char reverse_size[sizeof(block.size)];
		memcpy(reverse_size, &block.size, sizeof(reverse_size));
		std::reverse(reverse_size, reverse_size + sizeof(reverse_size));
		Tool::WriteToSteam(reverse_size, sizeof(reverse_size), out_file);

		// 写入数据块名称
		Tool::WriteToSteam(&block.name, sizeof(block.name), out_file);

		// 写入数据块内容
		std::string s_name(block.name, sizeof(block.name));
		if (strcmp(s_name.c_str(), "IHDR") == 0)
		{
			IHDRBlock ihdr;
			memcpy(&ihdr, &block, sizeof(Block));
			memcpy(((char *)&ihdr) + sizeof(Block), &Tool::ReadSome<sizeof(IHDRBlock)-sizeof(Block)>(block_info)[0], sizeof(IHDRBlock)-sizeof(Block));
			Tool::WriteToSteam(ihdr.data, sizeof(ihdr.data), out_file);
		}
		else if (strcmp(s_name.c_str(), "IEND") == 0)
		{
			Tool::WriteToSteam(IEND_DATA, sizeof(IEND_DATA), out_file);
			Tool::EnToolLog("[de succ]"+filename);
			break;
		}
		else
		{
			//in_file.seekg(read_size);
			in_file.seekg(block.pos);
			Tool::StreamMove(out_file, in_file, block.size + CRC_SIZE);
			//read_size += block.size + CRC_SIZE;
		}
	}
	Tool::EnToolLog("[de finished]"+filename);
	//out_file.close();
	return 1;
}


/*
* 0 打开失败
* 1 已加密
* 2 未加密
*/
int CDecrypt::DeFile(std::string filename, const aes_key &key)
{
	std::ifstream in_file(filename, std::ios::binary | std::ios::ate);
	if (!in_file.is_open())
	{
		std::cerr << "De 打开" << filename << " 失败！" << std::endl;
		return 0;
	}

	// 读取数据块位置
	uint64_t end_pos = in_file.tellg();
	in_file.seekg(end_pos - sizeof(uint64_t));
	uint64_t block_start_pos = *reinterpret_cast<uint64_t *>(&(Tool::ReadSome<sizeof(uint64_t)>(in_file)[0]));
	in_file.seekg(block_start_pos);

	// 解密数据块信息
	auto block_info = Tool::ReadLarge(in_file, uint32_t(end_pos - sizeof(uint64_t)-block_start_pos));
	std::string sssaasd = block_info.str();
	Tool::DecryptBlock(block_info, key);

	// 验证数据块内容
	
	auto block_head = Tool::ReadSome<sizeof(BLOCK_HEAD)>(block_info);
	for (unsigned int i = 0; i < block_head.size(); ++i)
	{
		// 和加密头做比较  测试图片是否是加密的图片
		if (block_head[i] != BLOCK_HEAD[i])
		{
			return 2;		// 未加密
		}

	}

	// 加密
	return 1;
}