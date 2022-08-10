#include "readFile.h"

char * conStr(int ctrl, int status) {
	char * ret = (char *) malloc(3);
	ret[0] = ctrl + '0';
	ret[1] = status + '0';
	ret[2] = 0;
	return ret;
}

// \param[filename] 文件名
// \param[wr]		读写标志 0:写, 1:读
// \param[data]		读取到的数据
// \return 0:文件不存在, 1:打开成功
int tryOpen(const char * filename, int wr, char * data, int size) {
	long file;
	struct _finddata_t find;
	//_chdir(".\\"); // 转换目录
	if ((file = _findfirst(filename, &find)) == -1L) {	// 未准备好交流文件
		return 0;
	}
	while (_findfirst( conStr(FILE_PY_CTRL, FILE_BUSY | (wr ^ 1)) , &find) != -1);	// 当文件在python控制下时, 等待
	while (_findfirst( conStr(FILE_PY_CTRL, FILE_READY | (wr ^ 1)) , &find) == -1);	// 文件未准备好，等待
	_findclose(file);
	FILE *tmp = fopen( conStr(FILE_C_CTRL, FILE_BUSY | wr) , "w"); // 新建一个文件，表示正忙
	fclose(tmp);
	remove( conStr(FILE_PY_CTRL, FILE_READY ^ wr) ); // 删除文件已准备好的标志
	// 开始读写文件
	if (wr == 0) { // 写文件
		FILE *fp = fopen(filename, "w");
		if (fp != NULL) fwrite(data, sizeof(char), size, fp);
		fclose(fp);
	} else {
		FILE *fp = fopen(filename, "rb");
		fseek(fp, 0, SEEK_END);
		long fsize = ftell(fp); // 获取文件大小
		rewind(fp);
		size = size <= fsize ? size : fsize;
		if (fp != NULL) fread(data, sizeof(char), size, fp); // -1 避免溢出
		fclose(fp);
	}
	tmp = fopen( conStr(FILE_C_CTRL, FILE_READY | wr), "w"); // 新建文件表示文件操作结束
	fclose(tmp);
	remove( conStr(FILE_C_CTRL, FILE_BUSY | wr) ); // 移除忙标志
	return 1;
}