#ifndef _FILE_WATCHER_H
#define _FILE_WATCHER_H

#include <windows.h>

typedef void (*LPFileDeal) (const char*, LPVOID); // �ļ��ı�ص�����

bool watch(const char* filename, LPFileDeal lpFileDeal, LPVOID lParam); // �����ļ�

#endif