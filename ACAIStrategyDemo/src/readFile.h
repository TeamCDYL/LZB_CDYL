#ifndef __FILE_COMMU_H
#define __FILE_COMMU_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#define C_IN_PY_OUT			"cpy"	// C 输入文件，python 输出文件
#define PY_IN_C_OUT			"pyc"	// python 输入文件，C 输出文件
#define FILE_BUSY			0		// 0000
#define FILE_READY			2		// 0010
#define FILE_WRITE			0		// 0000
#define FILE_READ			1		// 0001
#define FILE_WRITE_BUSY		FILE_WRITE | FILE_BUSY		// 0000
#define FILE_READ_BUSY		FILE_READ | FILE_BUSY		// 0001
#define FILE_WRITE_READY	FILE_WRITE | FILE_READY		// 0010
#define FILE_READ_READY		FILE_READ | FILE_READY		// 0011
#define FILE_C_CTRL			0
#define FILE_PY_CTRL		1
// 组合的意思:
// 00 表示 "cpy" 正在被C写入
// 01 表示 "pyc" 正在被C读取
// 02 表示 "cpy" 已被C写入完成
// 03 表示 "pyc" 已被C读取完成

int tryOpen(const char *filename, int wr, char *data, int size);

#endif
