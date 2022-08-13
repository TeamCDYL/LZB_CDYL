#ifndef _FILE_WATCHER_H
#define _FILE_WATCHER_H

#include <windows.h>

typedef void (LPFileDeal*) (LPVOID); // 文件改变回调函数

bool watch(LPCTSTR dir, LPFileDeal lpFileDeal, LPVOID lParam); // 监听文件

#endif