#include "Decrypt.h"

#include <fstream>
#include <iostream>
#include "Files.h"


// 解密PNG图片
void DecryptPNG(const std::vector<std::string> &filelist, const aes_key &key)
{
	for (auto &filename : filelist)
	{
		// 写入路径
// 		std::string out_path = "\\encrypt\\" + filename; // + path::splitext(filename)[0] + ".png";//.epng
// 		auto absolute_path = path::curdir();
// 		out_path = absolute_path + out_path;

		// 取出相对路径
		auto filePath = path::curdir() + "\\encrypted\\";
		auto outFile = filename.substr(filePath.size(), filename.size());

		// 绝对路径
		std::string out_path = "\\encrypt\\" + outFile;
		auto absolute_path = path::curdir();
		out_path = absolute_path + out_path;

		if (path::filedir(out_path) == -1)
		{
			std::cerr << out_path << " 文件夹不存在" << std::endl;
			break;
		}

		// 如果已经解密直接拷贝
		if ( DeFile(filename, key) == 2)
		{
			//std::cerr << "--->" << filename << " 已经解密" << std::endl;
			path::copy(filename, out_path);
			continue;
		}

		std::ifstream in_file(filename, std::ios::binary | std::ios::ate);
		if (!in_file.is_open())
		{
			std::cerr << "打开" << filename << " 失败！" << std::endl;
			return;
		}

		// 读取数据块位置
		uint64_t end_pos = in_file.tellg();
		in_file.seekg(end_pos - sizeof(uint64_t));
		uint64_t block_start_pos = *reinterpret_cast<uint64_t *>(&(ReadSome<sizeof(uint64_t)>(in_file)[0]));
		in_file.seekg(block_start_pos);

		// 解密数据块信息
		auto block_info = ReadLarge(in_file, uint32_t(end_pos - sizeof(uint64_t)-block_start_pos));
		std::string sssaasd = block_info.str();
		DecryptBlock(block_info, key);

		// 验证数据块内容
		auto block_head = ReadSome<sizeof(BLOCK_HEAD)>(block_info);
		for (unsigned int i = 0; i < block_head.size(); ++i)
		{
			if (block_head[i] != BLOCK_HEAD[i])
			{
				std::cerr << "===>密钥错误，解密" << filename << " 失败！<===" << std::endl;
				return;
			}
		}

		std::ofstream out_file(out_path, std::ios::binary);
		if (!out_file.is_open())
		{
			std::cerr << "--->创建" << path::splitext(filename)[0] << ".png" << " 失败！" << std::endl;
			continue;
		}
		std::cerr << "-->正在解密" << path::splitext(filename)[0] << ".png" << " 文件" << std::endl;
		// 写入文件头
		WriteToSteam(HEAD_DATA, sizeof(HEAD_DATA), out_file);

		// 读取数据块
		uint64_t read_size = 0;
		while (true)
		{
			// 读取数据块信息
			Block block;
			memcpy(&block, &ReadSome<sizeof(Block)>(block_info)[0], sizeof(Block));
			if (block_info.eof())
			{
				out_file.clear();
				std::cerr << "the %s file format error!" << std::endl;
				break;
			}

			// 写入数据块长度
			char reverse_size[sizeof(block.size)];
			memcpy(reverse_size, &block.size, sizeof(reverse_size));
			std::reverse(reverse_size, reverse_size + sizeof(reverse_size));
			WriteToSteam(reverse_size, sizeof(reverse_size), out_file);

			// 写入数据块名称
			WriteToSteam(&block.name, sizeof(block.name), out_file);

			// 写入数据块内容
			std::string s_name(block.name, sizeof(block.name));
			if (strcmp(s_name.c_str(), "IHDR") == 0)
			{
				IHDRBlock ihdr;
				memcpy(&ihdr, &block, sizeof(Block));
				memcpy(((char *)&ihdr) + sizeof(Block), &ReadSome<sizeof(IHDRBlock) - sizeof(Block)>(block_info)[0], sizeof(IHDRBlock) - sizeof(Block));
				WriteToSteam(ihdr.data, sizeof(ihdr.data), out_file);
			}
			else if (strcmp(s_name.c_str(), "IEND") == 0)
			{
				WriteToSteam(IEND_DATA, sizeof(IEND_DATA), out_file);
				std::cout << "===成功解密：" << filename << std::endl;
				break;
			}
			else
			{
				in_file.seekg(read_size);
				StreamMove(out_file, in_file, block.size + CRC_SIZE);
				read_size += block.size + CRC_SIZE;
			}
		}
		std::cerr << "=== 解密 :" << path::splitext(filename)[0] << ".png" << " 文件完成" << std::endl;
		//out_file.close();
	}
}