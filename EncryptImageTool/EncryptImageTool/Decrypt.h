#pragma once

#include <array>
#include <string>
#include <vector>
#include "Tools.h"

/**
 * 解密PNG图片
 * @param filelist 文件列表
 */
class CDecrypt
{
public:
	static int DecryptPNG(std::string &filename, const aes_key &key, std::string strEnPath, std::string strDePath);
	static int DeFile(std::string filename, const aes_key &key);
private:

};