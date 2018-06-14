#pragma once
#include <vector>
#include "AES.h"

class CEncryptImage
{
public:

	/*
	* -2 ���������ļ�ʧ��
	* -1 �ļ��в�����
	* 0 �Ѿ����ܣ�ֱ�ӿ���
	* 1 ���ܳɹ�
	*/
	static int EncryptPNG(const std::string filename, const aes_key &key, std::string filePath, std::string outPath);

	static void WriteFileData(const std::string &filename, std::ofstream &outstream, std::stringstream &block_info);

	/*
	* 0 ��ʧ��
	* 1 �Ѽ���
	* 2 δ����
	*/
	static int EnFile(std::string filename, const aes_key &key);
};

