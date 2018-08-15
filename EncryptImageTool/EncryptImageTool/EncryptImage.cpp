#include "stdafx.h"
#include "EncryptImage.h"
#include "EncryptImageTool.h"
#include <fstream>
#include <iostream>
#include "Struct.h"
#include "Tools.h"
#include "EncryptImageToolDlg.h"

/*
* -2 创建解密文件失败
* -1 文件夹不存在
* 0 已经加密，直接拷贝
* 1 加密成功
*/
int CEncryptImage::EncryptPNG(const std::string filename, const aes_key &key, std::string filePath, std::string outPath)
{
	// 取出相对路径
	auto outFile = filename.substr(filePath.size(), filename.size());

	// 绝对路径
	std::string out_path = outPath + outFile;
	//Tool::EnToolLog("[EncryptPng] " + outPath);

	if (Tool::filedir(out_path) == -1)
	{
		std::cerr << out_path << " 文件夹不存在" << std::endl;
		return -1;
	}


	// 如果已经加密直接拷贝
	//if ( EnFile(filename, key) == 1)
	//{
	//	//std::cerr << "--->" << filename << " 已经加密" << std::endl;
	//	Tool::copy(filename, out_path);
	//	return 0;
	//}

	std::ofstream out_file;
	std::stringstream block_info;

	out_file.open(out_path, std::ios::binary);
	if (!out_file.is_open())
	{
		out_file.close();
		block_info.str("");
		block_info.clear();
		std::cerr << "创建" << filename << " 失败！" << std::endl;
		return -2;
	}

	// 文件信息头部
	for (auto ch : BLOCK_HEAD) block_info.put(ch);

	//写入加密的文件头
	WriteFileData(filename, out_file, block_info);

	// 记录起始位置
	//uint32_t pos = htonl((uint32_t)out_file.tellp());
	uint32_t pos = (uint32_t)out_file.tellp();
	
	// 数据块信息加密
	Tool::EncryptBlock(block_info, key);

	// 写入数据块信息
	Tool::StreamMove(out_file, block_info, uint32_t(block_info.tellp() - block_info.tellg()));

	// 写入数据块信息位置
	char *user_data = reinterpret_cast<char *>(&pos);
	for (unsigned int i = 0; i < sizeof(pos); ++i) out_file.put(user_data[i]);
	//for (unsigned int i = 0; i < sizeof(uint64_t); ++i) out_file.put(user_data[i]);

	//std::cout << "==>加密完成：" << out_path.c_str() << std::endl;
	Tool::EnToolLog("[encrypt Suc ] 加密完成 " + outPath);

	out_file.close();
	block_info.str("");
	block_info.clear();

	return 1;
}

void CEncryptImage::WriteFileData(const std::string &filename, std::ofstream &outstream, std::stringstream &block_info)
{
	std::ifstream file;
	file.open(filename, std::ios::binary);
	if (!file.is_open())
	{
		//std::cerr << "打开" << filename << " 失败！" << std::endl;
		Tool::EnToolLog("[en open fail ] 打开失败 " + filename);
		return;
	}

	// 读取文件头
	auto head = Tool::ReadSome<HEAD_SIZE>(file);

	// 读取数据块
	while (true)
	{
		Block block;

		// 获取数据块长度
		auto lenght = Tool::ReadSome<4>(file);
		if (file.eof()) break;

		std::reverse(lenght.begin(), lenght.end());

		auto block_size = *reinterpret_cast<uint32_t *>(&lenght[0]);
		//auto block_size = *reinterpret_cast<int *>(&lenght[0]);

		// 获取数据块名称
		auto block_name = &(Tool::ReadSome<4>(file)[0]);

		// 获取数据块内容
		auto block_data = Tool::ReadLarge(file, block_size + CRC_SIZE);

		// 数据块信息
		//block.size = htonl(block_size);
		block.size = block_size;
		//block.pos = htonl((uint32_t)outstream.tellp());
		block.pos = (uint32_t) outstream.tellp();
		memcpy(block.name, &block_name[0], sizeof(block.name));

		// 根据数据类型进行处理
		std::string s_name(block.name, sizeof(block.name));
		if (strcmp(s_name.c_str(), "IHDR") == 0)
		{
			IHDRBlock ihdr;
			ihdr.block = block;
			memcpy(ihdr.data, block_data.str().c_str(), sizeof(ihdr.data));
			Tool::WriteToSteam(&ihdr, sizeof(IHDRBlock), block_info);
		}
		else if (strcmp(s_name.c_str(), "IEND") == 0)
		{
			Tool::WriteToSteam(&block, sizeof(Block), block_info);
		}
		else
		{
			Tool::WriteToSteam(&block, sizeof(Block), block_info);
			Tool::StreamMove(outstream, block_data, block_size + CRC_SIZE);
		}
	}

	file.close();
}


/**
* 1 已经加密
* 2 未加密
* 0 打开失败 文件损坏
*/
int CEncryptImage::EnFile(std::string filename, const aes_key &key)
{
	std::ifstream in_file(filename, std::ios::binary | std::ios::ate);
	if (!in_file.is_open())
	{
		std::cerr << "=== En 打开" << filename << " 失败！" << std::endl;
		return 0;
	}

	// 读取数据块位置
	uint32_t end_pos = (uint32_t)in_file.tellg();
	//uint64_t end_pos = in_file.tellg();
	in_file.seekg(end_pos - sizeof(uint32_t));
	//in_file.seekg(end_pos - sizeof(uint64_t));
	uint32_t block_start_pos = *reinterpret_cast<uint32_t *>(&(Tool::ReadSome<sizeof(uint32_t)>(in_file)[0]));
	//uint64_t block_start_pos = *reinterpret_cast<uint64_t *>(&(Tool::ReadSome<sizeof(uint64_t)>(in_file)[0]));
	in_file.seekg(block_start_pos);

	std::stringstream block_info = Tool::ReadLarge(in_file, uint32_t(end_pos - sizeof(uint32_t)-block_start_pos));
	//auto block_info = Tool::ReadLarge(in_file, uint32_t(end_pos - sizeof(uint64_t) - block_start_pos));

	// 解密数据块信息
	Tool::DecryptBlock(block_info, key);

	// 验证数据块内容
	auto block_head = Tool::ReadSome<sizeof(BLOCK_HEAD)>(block_info);
	for (unsigned int i = 0; i < block_head.size(); ++i)
	{
		if (block_head[i] == BLOCK_HEAD[i])
		{
			return 1;		// 已经加密的图片
		}
		return 2;
	}

	return 2;
}
