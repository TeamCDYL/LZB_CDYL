#include "file_watcher.h"

FILETIME ftLastWriteTime;

// return 1:fta > ftb, 0:fta == ftb, -1:fta < ftb
int cmpFileTime(FILETIME fta, FILETIME ftb) {
	if (fta.dwHighDateTime > ftb.dwHighDateTime) return 1;
	if (fta.dwHighDateTime < ftb.dwHighDateTime) return -1;
	if (fta.dwLowDateTime > ftb.dwLowDateTime) return 1;
	if (fta.dwLowDateTime < ftb.dwLowDateTime) return -1;
	return 0;
}

bool watch(const char* filename, LPFileDeal lpFileDeal, LPVOID lParam) {
	TCHAR tWideCharFilename[MAX_PATH * 2 + 1];
	int size = MultiByteToWideChar(CP_ACP, 0, filename, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, filename, -1, tWideCharFilename, size);
	HANDLE hf = CreateFile(tWideCharFilename, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL); // 打开文件
	if (hf == INVALID_HANDLE_VALUE) return false;
	FILETIME ftWriteTime;
	GetFileTime(hf, NULL, NULL, &ftWriteTime);
	if (cmpFileTime(ftLastWriteTime, ftWriteTime) < 0) {
		ftLastWriteTime = ftWriteTime;
		lpFileDeal(filename, lParam);
		CloseHandle(hf);
		return true;
	}
	CloseHandle(hf);
	return false;
}
