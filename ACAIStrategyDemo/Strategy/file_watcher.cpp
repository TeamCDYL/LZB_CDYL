#include "file_watcher.h"

bool watch(LPCTSTR dir, LPFileDeal lpFileDeal, LPVOID lParam) {
	HANDLE hDir = CreateFile(dir, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL); // 打开文件
	if (hDir == INVALID_HANDLE_VALUE) return false;
	lpFileDeal lpfd = lpFileDeal;
	LPVOID lpa = lParam;
	BYTE buf[2 * sizeof(FILE_NOTIFY_INFORMATION) + 2 * MAX_PATH + 2];
	FILE_NOTIFY_INFORMATION *pNotify = (FILE_NOTIFY_INFORMATION*) buf;
	DWORD ret;
	int tryTimes = 100;
	while (tryTimes--) {
		if (ReadDirectoryChangesW(hDir, pNotify, sizeof(buf) - 2,
			false, FILE_NOTIFY_CHANGE_SIZE, // 不检查子目录，只检查文件是否被改变
			&ret, NULL, NULL)) {
				for (PFILE_NOTIFY_INFORMATION p = pNotify; p != NULL;) {
					WCHAR c = p->FileName[p->FileNameLength / 2];
					p->FileName[p->FileNameLength / 2] = L'\0';

					if (strcmp(p->FileName, "action.csv")) {
						lpFileDeal(lParam);
						return true;
					}

					p->FileName[p->FileNameLength / 2] = c;
					if (p->NextEntryOffset) {
						p = (PFILE_NOTIFY_INFORMATION) ((BYTE*) p + p->NextEntryOffset);
					} else {
						p = NULL;
					}
				}
		} else {
			return false;
		}
	}
	return false;
}
