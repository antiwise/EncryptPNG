#pragma once

#include <array>
#include <string>
#include <vector>
#include "Tools.h"

/**
 * 加密PNG图片
 * @param filelist 文件刘表
 */
void EncryptPNG(const std::vector<std::string> &filelist, const aes_key &key);

void WriteFileData(const std::string &filename, std::ofstream &outstream, std::stringstream &block_info);

bool CheckFileEn(const std::string &filename);
