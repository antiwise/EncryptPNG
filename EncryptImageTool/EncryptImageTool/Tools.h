#pragma once

#include <array>
#include <vector>
#include <sstream>
#include "AES.h"
#include "Struct.h"
#include <fstream>
#include <iostream>
#include <io.h>
#include <direct.h>
#include <time.h>
/**
 * 从流中读取一些数据
 */
namespace Tool
{
	static std::string getTime()
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		
		char buff[256];

		sprintf_s(buff, "%02d:%02d:%02d  ", st.wHour, st.wMinute, st.wSecond);
		return buff;

	}
	/*
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

	// 打印log日志
	static void EnToolLog(std::string str){
		//char* path = "C:\\1.txt"; // 你要创建文件的路径
		std::string logPath = curdir() + "\\log.txt";
		static bool bClear = false;
		if (bClear == false)
		{
			//std::fstream file(logPath, std::ios::out);		// 清空文件
			bClear = true;
		}

		std::string curTime = getTime();
		std::ofstream fout(logPath, std::ios::app);
		if (fout) { // 如果创建成功
			fout <<curTime + str << std::endl; // 使用与cout同样的方式进行写入

			fout.close();  // 执行完操作后关闭文件句柄
		}
	}

	template <int _Value, typename _Stream>
	static std::array<char, _Value> ReadSome(_Stream &stream)
	{
		std::array<char, _Value> buffer;
		for (unsigned int i = 0; i < _Value; ++i) buffer[i] = stream.get();
		return buffer;
	}

	/**
	 * 从流中读取大量数据
	 */
	template <typename _Stream>
	static std::stringstream ReadLarge(_Stream &stream, const int readsize)
	{
		std::stringstream ss;
		for (int i = 0; i < readsize; ++i) ss.put(stream.get());
		return ss;
	}

	/**
	 * 将数据写到流中
	 */
	template <typename _Stream>
	static void WriteToSteam(const void *data, uint32_t size, _Stream &stream)
	{
		unsigned char *p = reinterpret_cast<unsigned char *>(const_cast<void *>(data));
		for (unsigned int i = 0; i < size; ++i) stream.put(p[i]);
	}

	/**
	 * 移动流数据到另一个流
	 */
	template <typename _Source, typename _Target>
	static void StreamMove(_Target &target, _Source &source, const uint32_t size)
	{
		for (uint64_t i = 0; i < size; ++i) target.put(source.get());
	}

	/**
	 * 数据块加密
	 */
	static void EncryptBlock(std::stringstream &ss, const aes_key &key)
	{
		const std::streamoff contents_size = ss.tellp() - ss.tellg();
		const uint32_t block_size = (uint32_t)(contents_size + AES_BLOCK_SIZE - contents_size % AES_BLOCK_SIZE);
		std::vector<uint8_t> buffer;
		buffer.resize(block_size);
		for (uint32_t i = 0; i < contents_size; ++i) buffer[i] = ss.get();
		AES::EncryptData(&buffer[0], block_size, key);
		ss.seekg(0); ss.seekp(0);
		for (uint32_t i = 0; i < block_size; ++i) ss.put(buffer[i]);
	}

	/**
	 * 数据块解密
	 */
	static void DecryptBlock(std::stringstream &ss, const aes_key &key)
	{
		const std::streamoff contents_size = ss.tellp() - ss.tellg();
		const uint32_t block_size = (uint32_t)(contents_size + AES_BLOCK_SIZE - contents_size % AES_BLOCK_SIZE);
		std::vector<uint8_t> buffer;
		buffer.resize(block_size);
		for (uint32_t i = 0; i < contents_size; ++i) buffer[i] = ss.get();
		AES::DecryptData(&buffer[0], block_size, key);
		ss.seekg(0); ss.seekp(0);
		for (uint32_t i = 0; i < block_size; ++i) ss.put(buffer[i]);
	}

	static int filedir(std::string &filename)
	{

		int lastPos = filename.find_last_of("\\");
		if (lastPos != std::string::npos)
		{
			std::string dirName = filename.substr(0, lastPos + 1 );
			EnToolLog("==>【check start】：" + dirName);

			int e_pos = dirName.length();
			int f_pos = dirName.find("\\", 0);

			std::string subdir = "";
			do 
			{
				e_pos = dirName.find("\\", f_pos + 2);
				if (e_pos != std::string::npos)
				{
					subdir = dirName.substr(0, e_pos);
					if (!PathIsDirectory(CString(subdir.c_str())))
					{
						int state = ::CreateDirectory(CString(subdir.c_str()), NULL);
						if (state == 0)
						{
							EnToolLog("【error】创建失败：" + subdir);
							return -1;
						}
						else{
							EnToolLog("【create】创建成功：" + subdir);
						}
					}

				}
				f_pos = e_pos;
			} while (f_pos != std::string::npos);

			if (!PathIsDirectory(CString(subdir.c_str())))
			{
				EnToolLog("【check create fail】：" + subdir);
				return -1;
			}
			else
			{
				EnToolLog("【check create OK】：" + subdir);
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
				if ( (strcmp(file_info.name, ".") != 0) && (strcmp(file_info.name, "..") != 0) )
				{
					std::string new_path = start_path + "\\" + file_info.name;

					EnToolLog("查找加密文件：" + new_path);
					for (auto filename : walk(new_path)) file_list.push_back(filename);
				}
			}
			else
			{
				std::string new_path = start_path + "\\";
				new_path += file_info.name;
				if (splitext(file_info.name)[1] == ".png")
				{
					EnToolLog("[png] 需要加密文件：" + new_path);
					file_list.push_back(new_path);
				}

			}
		} while (_findnext(handle, &file_info) == 0);

		_findclose(handle);

		return file_list;
	}

	/**
	* 获得文件下所有文件
	*/
	static std::vector<std::string> getAllFiles(const std::string &start_path)
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
				if ((strcmp(file_info.name, ".") != 0) && (strcmp(file_info.name, "..") != 0))
				{
					std::string new_path = start_path + "\\" + file_info.name;

					EnToolLog("查找加密文件：" + new_path);
					for (auto filename : getAllFiles(new_path)) file_list.push_back(filename);
				}
			}
			else
			{
				std::string new_path = start_path + "\\";
				new_path += file_info.name;

				file_list.push_back(new_path);

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

		EnToolLog("拷贝完成 " + src);

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
