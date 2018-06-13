#pragma once

#include <io.h>
#include <array>
#include <string>
#include <vector>
#include <direct.h>
#include <windows.h>
#include <fstream>

namespace path
{
	/**
	 * 获取当前目录
	 */
	static std::string curdir()
	{
		char exe_full_path[MAX_PATH];
		GetModuleFileNameA(NULL, exe_full_path, MAX_PATH);
		std::string current_path(exe_full_path);
		int pos = current_path.find_last_of('\\', current_path.length());
		return current_path.substr(0, pos);
	}

	/*
	* 子文件是否存在，不存在则创建 成功 返回0 失败 -1
	*/
	static int filedir(std::string &filename)
	{
		int lastPos = filename.find_last_of("\\");
		if (lastPos != std::string::npos)
		{
			std::string dirName = filename.substr(0, lastPos);
			if (_access(dirName.c_str(), 0) == -1)
			{
				int i = _mkdir(dirName.c_str());
				return i;
			}
		}
		return 0;

	}

	/**
	 * 文件名分解
	 * 0 文件格式
	 * 1 文件路径
	 * 2 文件名字
	 */
	static std::array<std::string, 2> splitext(const std::string &file_path)
	{
		auto pos = file_path.rfind('.');
		std::array<std::string, 2> text;
		if (std::string::npos != pos)
		{
			text[1] = file_path.substr(pos);
			text[0] = file_path.substr(0, pos);
		}
		else
		{
			text[0] = file_path;
		}
		return text;
	}

	/**
	 * 列出子目录下所有文件
	 */
	static std::vector<std::string> walk(const std::string &start_path)
	{
		_finddata_t file_info;
		std::vector<std::string> file_list;
		std::string find_path = start_path + "\\*";
		long handle = _findfirst(find_path.c_str(), &file_info);

		if (handle == -1L)
		{
			return file_list;
		}

		do
		{
			// 文件夹
			if (file_info.attrib & _A_SUBDIR)
			{
				if ((strcmp(file_info.name, ".") != 0) && (strcmp(file_info.name, "..") != 0) && (strcmp(file_info.name, "encrypt") != 0))
				{
					std::string new_path = start_path + "\\" + file_info.name;
					std::cout << "查找加密文件：" << new_path << std::endl;
					for (auto filename : walk(new_path)) file_list.push_back(filename);
				}
			}
			else
			{
				std::string new_path = start_path + "\\";
				new_path += file_info.name;
				if (path::splitext(file_info.name)[1] == ".png")
				{
					std::cout << "--> 需要加密文件：" << new_path << std::endl;
					file_list.push_back(new_path);
				}

			}
		} while (_findnext(handle, &file_info) == 0);

		_findclose(handle);

		return file_list;
	}

	// 直接拷贝加密文件
	static void copy(std::string src, std::string dst)
	{
		std::ifstream in(src, std::ios::binary);
		if (!in.is_open()) {
			std::cout << "error open file " << src << std::endl;
			exit(EXIT_FAILURE);
		}

		std::ofstream out(dst, std::ios::binary);
		if (!out.is_open()) {
			std::cout << "error open file " << dst << std::endl;
			exit(EXIT_FAILURE);
		}

		if (src == dst) {
			std::cout << "the src file can't be same with dst file" << std::endl;
			exit(EXIT_FAILURE);
		}
		std::cout << "--> " << src << "拷贝完成" << std::endl;
		char buf[2048];
		long long totalBytes = 0;
		while (in)
		{
			//read从in流中读取2048字节，放入buf数组中，同时文件指针向后移动2048字节
			//若不足2048字节遇到文件结尾，则以实际提取字节读取。
			in.read(buf, 2048);
			//gcount()用来提取读取的字节数，write将buf中的内容写入out流。
			out.write(buf, in.gcount());
			totalBytes += in.gcount();
		}

		in.close();
		out.close();
	}
}