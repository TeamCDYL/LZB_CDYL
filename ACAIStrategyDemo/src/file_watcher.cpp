#include "file_watcher.h"

bool watch(LPCTSTR dir, const char* filename, LPFileDeal lpFileDeal, LPVOID lParam) {
	HANDLE hDir = CreateFile(dir, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL); // 打开文件夹
	if (hDir == INVALID_HANDLE_VALUE) return false;
	BYTE buf[2 * sizeof(FILE_NOTIFY_INFORMATION) + 2 * MAX_PATH + 2];
	FILE_NOTIFY_INFORMATION *pNotify = (FILE_NOTIFY_INFORMATION*) buf;
	char multiBytesFilename[MAX_PATH * 2 + 2]; // 窄字符文件名
	DWORD ret;
	int tryTimes = 1000;
	while (tryTimes--) {
		if (ReadDirectoryChangesW(hDir, pNotify, sizeof(buf) - 2,
			false, FILE_NOTIFY_CHANGE_SIZE, // 不检查子目录；检查文件是否被修改
			&ret, NULL, NULL)) {
				for (PFILE_NOTIFY_INFORMATION p = pNotify; p != NULL;) {
					WCHAR c = p->FileName[p->FileNameLength / 2];
					p->FileName[p->FileNameLength / 2] = L'\0';

					int size = WideCharToMultiByte(CP_ACP, 0, p->FileName, -1, NULL, 0, NULL, NULL); // 宽字符转窄字符
					WideCharToMultiByte(CP_ACP, 0, p->FileName, -1, multiBytesFilename, size, NULL, NULL);
					if (strcmp(multiBytesFilename, filename) == 0) { // 目标文件被修改
						lpFileDeal(filename, lParam);
						CloseHandle(hDir);
						return true;
					}

					p->FileName[p->FileNameLength / 2] = c;
					if (p->NextEntryOffset) {
						p = (PFILE_NOTIFY_INFORMATION) ((BYTE*) p + p->NextEntryOffset); // 确保地址增长长度
					} else {
						p = NULL;
					}
				}
		}
	}
	CloseHandle(hDir);
	return false;
}
