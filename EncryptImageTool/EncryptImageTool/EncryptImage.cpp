#include "stdafx.h"
#include "EncryptImage.h"
#include "EncryptImageTool.h"
#include <fstream>
#include <iostream>
#include "Struct.h"
#include "Tools.h"
#include "EncryptImageToolDlg.h"

/*
* -2 ���������ļ�ʧ��
* -1 �ļ��в�����
* 0 �Ѿ����ܣ�ֱ�ӿ���
* 1 ���ܳɹ�
*/
int CEncryptImage::EncryptPNG(const std::string filename, const aes_key &key, std::string filePath, std::string outPath)
{
	std::ofstream out_file;
	std::stringstream block_info;

	// ȡ�����·��
	auto outFile = filename.substr(filePath.size(), filename.size());

	// ����·��
	std::string out_path = outPath + outFile;
	//Tool::EnToolLog("[EncryptPng] " + outPath);

	if (Tool::filedir(out_path) == -1)
	{
		std::cerr << out_path << " �ļ��в�����" << std::endl;
		return -1;
	}


	// ����Ѿ�����ֱ�ӿ���
	if ( CEncryptImage::EnFile(filename, key) == 1)
	{
		//std::cerr << "--->" << filename << " �Ѿ�����" << std::endl;
		Tool::copy(filename, out_path);
		return 0;
	}

	out_file.open(out_path, std::ios::binary);
	if (!out_file.is_open())
	{
		out_file.close();
		block_info.str("");
		block_info.clear();
		std::cerr << "����" << filename << " ʧ�ܣ�" << std::endl;
		return -2;
	}

	// �ļ���Ϣͷ��
	for (auto ch : BLOCK_HEAD) block_info.put(ch);

	WriteFileData(filename, out_file, block_info);

	// ��¼��ʼλ��
	uint32_t pos = htonl((uint32_t)out_file.tellp());
	char *user_data = reinterpret_cast<char *>(&pos);

	// ���ݿ���Ϣ����
	Tool::EncryptBlock(block_info, key);

	// д�����ݿ���Ϣ
	Tool::StreamMove(out_file, block_info, uint32_t(block_info.tellp() - block_info.tellg()));
	for (unsigned int i = 0; i < sizeof(uint32_t); ++i) out_file.put(user_data[i]);

	std::cout << "==>������ɣ�" << out_path.c_str() << std::endl;

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
		std::cerr << "��" << filename << " ʧ�ܣ�" << std::endl;
		return;
	}

	// ��ȡ�ļ�ͷ
	auto head = Tool::ReadSome<HEAD_SIZE>(file);

	// ��ȡ���ݿ�
	while (true)
	{
		Block block;

		// ��ȡ���ݿ鳤��
		auto lenght = Tool::ReadSome<4>(file);
		if (file.eof()) break;
		std::reverse(lenght.begin(), lenght.end());
		auto block_size = *reinterpret_cast<int *>(&lenght[0]);

		// ��ȡ���ݿ�����
		auto block_name = &(Tool::ReadSome<4>(file)[0]);

		// ��ȡ���ݿ�����
		auto block_data = Tool::ReadLarge(file, block_size + CRC_SIZE);

		// ���ݿ���Ϣ
		block.size = block_size;
		block.pos = htonl((uint32_t)outstream.tellp());;
		//block.pos = outstream.tellp();
		memcpy(block.name, &block_name[0], sizeof(block.name));

		// �����������ͽ��д���
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

	//file.close();
}


/**
* 1 �Ѿ�����
* 2 δ����
* 0 ��ʧ�� �ļ���
*/
int CEncryptImage::EnFile(std::string filename, const aes_key &key)
{
	std::ifstream in_file(filename, std::ios::binary | std::ios::ate);
	if (!in_file.is_open())
	{
		std::cerr << "=== En ��" << filename << " ʧ�ܣ�" << std::endl;
		return 0;
	}

	// ��ȡ���ݿ�λ��
	uint32_t end_pos = in_file.tellg();
	in_file.seekg(end_pos - sizeof(BLOCK_HEAD));
	uint32_t block_start_pos = *reinterpret_cast<uint32_t *>(&(Tool::ReadSome<sizeof(BLOCK_HEAD)>(in_file)[0]));
	in_file.seekg(block_start_pos);

	
	auto block_info = Tool::ReadLarge(in_file, uint32_t(end_pos - sizeof(BLOCK_HEAD)-block_start_pos));
	std::string sssaasd = block_info.str();

	// �������ݿ���Ϣ
	Tool::DecryptBlock(block_info, key);

	// ��֤���ݿ�����
	auto block_head = Tool::ReadSome<sizeof(BLOCK_HEAD)>(block_info);
	for (unsigned int i = 0; i < block_head.size(); ++i)
	{
		if (block_head[i] == BLOCK_HEAD[i])
		{
			return 1;
		}
	}

	return 2;
}
