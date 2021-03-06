﻿#pragma once

#include <cstdint>

static const uint32_t HEAD_SIZE = 8;

static const uint32_t CRC_SIZE = 4;

/* 文件头部 */
static const unsigned char HEAD_DATA[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };

/* IEND CRC码 */
static const unsigned char IEND_DATA[] = { 0xae, 0x42, 0x60, 0x82 };

/* 数据块头部（用于验证解密是否成功） */
static const unsigned char BLOCK_HEAD[] = { 0x45, 0x6e, 0x63, 0x72, 0x79, 0x70, 0x74, 0x50, 0x4e, 0x47 };

#pragma pack(push, 1)

struct Block
{
	char name[4];
	uint32_t pos;
	uint32_t size;
};

struct IHDRBlock
{
	Block block;
	char data[13 + CRC_SIZE];
};

#pragma pack(pop)