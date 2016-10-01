/*
	HMC���C�u�������ʃ��[�e�B���e�B
	Copyright (C)1996-2001 H.Mutsuki
*/

#include "util.h"
#include "string.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	�G���f�B�A�������t������������
*****************************************************************/
WORD GetWordLe(LPCVOID p)
{
	return *((WORD*)p);
}
void SetWordLe(LPVOID p, WORD w)
{
	*((WORD*)p) = w;
}
LONG GetLongLe(LPCVOID p)
{
	return *((LONG*)p);
}
void SetLongLe(LPVOID p, LONG l)
{
	*((LONG*)p) = l;
}
WORD GetWordBe(LPCVOID p)
{
	BYTE w[2];
	((LPBYTE)&w)[0] = ((LPBYTE)p)[1];
	((LPBYTE)&w)[1] = ((LPBYTE)p)[0];
	return *((LPWORD)&w);
}
void SetWordBe(LPVOID p, WORD w)
{
	((LPBYTE)p)[0] = ((LPBYTE)&w)[1];
	((LPBYTE)p)[1] = ((LPBYTE)&w)[0];
}
LONG GetLongBe(LPCVOID p)
{
	BYTE l[4];
	((LPBYTE)&l)[0] = ((LPBYTE)p)[3];
	((LPBYTE)&l)[1] = ((LPBYTE)p)[2];
	((LPBYTE)&l)[2] = ((LPBYTE)p)[1];
	((LPBYTE)&l)[3] = ((LPBYTE)p)[0];
	return *((LPLONG)&l);
}
void SetLongBe(LPVOID p, LONG l)
{
	((LPBYTE)p)[0] = ((LPBYTE)&l)[3];
	((LPBYTE)p)[1] = ((LPBYTE)&l)[2];
	((LPBYTE)p)[2] = ((LPBYTE)&l)[1];
	((LPBYTE)p)[3] = ((LPBYTE)&l)[0];
}
/*****************************************************************
	FIFO -> FIFO �������݃R�s�[
*****************************************************************/
void *fifocpy(void *dst_start, size_t dst_len, size_t dst_index, const void *src_start, size_t src_len, size_t src_index, size_t count)
{
	return fifocpy(dst_start,							// DST�J�n�A�h���X
				   &(((char *)dst_start)[dst_len]),		// DST�I���A�h���X(+1)
				   &(((char *)dst_start)[dst_index]),	// DST�R�s�[�J�n�A�h���X
				   src_start,							// SRC�J�n�A�h���X
				   &(((char *)src_start)[src_len]),		// SRC�I���A�h���X(+1)
				   &(((char *)src_start)[src_index]),	// SRC�R�s�[�J�n�A�h���X
				   count								// �R�s�[��
				   );
}
void *fifocpy(void *dst_start, size_t dst_len, void *dst, const void *src_start, size_t src_len, const void *src, size_t count)
{
	return fifocpy(dst_start, &(((char *)dst_start)[dst_len]), dst,
				   src_start, &(((char *)src_start)[src_len]), src, count);
}
void *fifocpy(void *dst_start, void *dst_end, void *dst, const void *src_start, const void *src_end, const void *src, size_t count)
{
	// �|�C���^��������
	if(dst < dst_start || dst_end <= dst) {
		dst = &(((char *)dst_start)[((DWORD(dst)     - DWORD(dst_start)) %
									 (DWORD(dst_end) - DWORD(dst_start)))]);
	}
	if(src < src_start || src_end <= src) {
		src = &(((char *)src_start)[((DWORD(src)     - DWORD(src_start)) %
									 (DWORD(src_end) - DWORD(src_start)))]);
	}

	// FIFO -> FIFO �R�s�[
	if(!(&(((char *)dst)[count]) > dst_end)) {
		if(!(&(((char *)src)[count]) > src_end)) {
			// �������ݖ���
			memcpy(dst, src, count);
		} else {
			// SRC�������ݗL�� (����FIFO)
			size_t count1 = size_t(src_end) - size_t(src);
			size_t count2 = count - count1;
			memcpy(dst, src, count1);
			memcpy(&(((char *)dst)[count1]), src_start, count2);
		}
	} else {
		if(!(&(((char *)src)[count]) > src_end)) {
			// DST�������ݗL�� (����FIFO)
			size_t count1 = size_t(dst_end) - size_t(dst);
			size_t count2 = count - count1;
			memcpy(dst, src, count1);
			memcpy(dst_start, &(((char *)src)[count1]), count2);
		} else {
			// SRC,DST�������ݗL�� (�ċA����FIFO)
			size_t count1 = min(size_t(src_end) - size_t(src),
								size_t(dst_end) - size_t(dst));
			size_t count2 = count - count1;
			fifocpy(dst_start, dst_end, dst,
					src_start, src_end, src, count1);
			fifocpy(dst_start, dst_end, &(((char *)dst)[count1]),
					src_start, src_end, &(((char *)src)[count1]), count2);
		}
	}

	return dst;
}

/*****************************************************************
	�C�Ӓ� memset()
*****************************************************************/
void *memfill(void *dest, const void *src, size_t size, size_t count)
{
	char *p = (char *)dest;
	for(size_t i=0; i<count; i++) {
		memcpy(p+=size, src, size);
	}
	return dest;
}
void *memfill(void *dest, BYTE c, size_t count)
{
	return memset(dest, c, count);
}
void *memfill(void *dest, WORD c, size_t count)
{
	WORD *p = (WORD *)dest;
	for(size_t i=0; i<count; i++) {
		*p++ = c;
	}
	return dest;
}
void *memfill(void *dest, DWORD c, size_t count)
{
	DWORD *p = (DWORD *)dest;
	for(size_t i=0; i<count; i++) {
		*p++ = c;
	}
	return dest;
}
void *memfill(void *dest, QWORD c, size_t count)
{
	QWORD *p = (QWORD *)dest;
	for(size_t i=0; i<count; i++) {
		*p++ = c;
	}
	return dest;
}

/*****************************************************************
	�������� strrev()
*****************************************************************/
void *memrev(void *p, size_t count)
{
	char *ps = (char *)p;
	char *pd = &(((char *)p)[count]);

	while(ps < pd) {
		*--pd = *ps++;
	}
	return p;
}
void *memrev(void *dest, void *src, size_t count)
{
	char *ps = (char *)src;
	char *pd = &(((char *)dest)[count]);

	for(size_t i=0; i<count; i++) {
		*--pd = *ps++;
	}
	return dest;
}

/*****************************************************************
	������� fgetc()
*****************************************************************/
int sgetc(char **stream)
{
	int c = **stream;
	if(c == EOF || c == '\0') {
		return EOF;
	}
	(*stream)++;
	return c;
}

/*****************************************************************
	������� fputc()
*****************************************************************/
int sputc(int c, char **stream)
{
	**stream = c;
	(*stream)++;
	return c;
}

/*****************************************************************
	������� fgets()
*****************************************************************/
char *sgets(char *string, int n, char **stream)
{
	int c = EOF;
	int i = 0;
	for(; i<n; i++) {
		c = sgetc(stream);
		if(c == EOF || c == '\n') break;
		string[i] = char(c);
		if(c == '\r') i--;
	}
	if(i == 0 && c == EOF) {
		return NULL;
	}
	string[i] = NULL;
	return string;
}

/*****************************************************************
	������� fputs()
*****************************************************************/
int sputs(const char *string, char **stream)
{
	int c = EOF;
	const char *p = string;
	while(*p != NULL) {
		c = sputc(*p++, stream);
		if(c == EOF) break;
	}
	return c;
}

/*****************************************************************
	�t�@�C���̃T�C�Y�𓾂� (�t�@�C���\���̂��g�p)
*****************************************************************/
long fsize(FILE *fp)
{
	long forig = ftell(fp);
	if(fseek(fp,     0, SEEK_END)) return -1;
	long fsize = ftell(fp);
	if(fseek(fp, forig, SEEK_SET)) return -1;
	return fsize;
}

/*****************************************************************
	BCD�ϊ�
*****************************************************************/
BYTE bcd(BYTE b)
{
	BYTE atbl[3];
	BYTE *ptbl = &atbl[3];

	for(UINT uDigit=3; uDigit; uDigit--) {
		const BYTE aBase[] = {0, 1, 10, 100};
		BYTE uBase = aBase[uDigit];
		*--ptbl = b / uBase;
		b -= (b / uBase) * uBase;
	}

	return MAKEBYTE(atbl[0], atbl[1]);
}
WORD bcd(WORD w)
{
	BYTE atbl[5];
	BYTE *ptbl = &atbl[5];

	for(UINT uDigit=5; uDigit; uDigit--) {
		const WORD aBase[] = {0, 1, 10, 100, 1000, 10000};
		WORD uBase = aBase[uDigit];
		*--ptbl = (BYTE)(w / uBase);
		w -= (w / uBase) * uBase;
	}

	return MAKEWORD(MAKEBYTE(atbl[0], atbl[1]),
					MAKEBYTE(atbl[2], atbl[3]));
}
DWORD bcd(DWORD dw)
{
	BYTE atbl[10];
	BYTE *ptbl = &atbl[10];

	for(UINT uDigit=10; uDigit; uDigit--) {
		const DWORD aBase[] = {0, 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
		DWORD uBase = aBase[uDigit];
		*--ptbl = (BYTE)(dw / uBase);
		dw -= (dw / uBase) * uBase;
	}

	return MAKELONG(MAKEWORD(MAKEBYTE(atbl[0], atbl[1]),
							 MAKEBYTE(atbl[2], atbl[3])),
					MAKEWORD(MAKEBYTE(atbl[4], atbl[5]),
							 MAKEBYTE(atbl[6], atbl[7])));
}

/*****************************************************************
	dos �X�^�C���̃p�X�� unix �X�^�C���ɕϊ�
*****************************************************************/
char *dtou_path(char *path)
{
	char *ptr;
	//strlwr(path);		// �p�X�����������ɕϊ�
	ptr = path;
	while((ptr = strchr(ptr, '\\')) != NULL) {
		strnset(ptr, '/', 1);
	}
	return path;
}

/*****************************************************************
	unix �X�^�C���̃p�X�� dos �X�^�C���ɕϊ�
*****************************************************************/
char *utod_path(char *path)
{
	char *ptr;
	//strupr(path);	// �p�X����啶���ɕϊ�
	ptr = path;
	while((ptr = strchr(ptr, '/')) != NULL) {
		strnset(ptr, '\\', 1);
	}
	return path;
}

/*****************************************************************
	�p�X���̍Ō�� "\" ��t������
*****************************************************************/
char *MakeRegularPath(char *path)
{
	size_t len = strlen(path);
	if(len != 0) {
		if(path[len-1] != '\\') {
			path[len] = '\\';
			path[len+1] = NULL;
		}
	} else {
		strcpy(path, ".\\");	// �J�����g�f�B���N�g��
	}
	return path;
}

/*****************************************************************
	�p�X���̍Ō�� "\" ���폜����
*****************************************************************/
char *MakeSimplePath(char *path)
{
	size_t len = strlen(path);
	if(len != 0) {
		if(path[len-1] == '\\') {
			path[len-1] = NULL;
		}
	}
	return path;
}

/*****************************************************************
	�g���q��u��������
*****************************************************************/
char *MakeExtFName(char *NewName, const char *BaseName, const char *NewExt)
{
	char drive[_MAX_DRIVE];
	char path[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(BaseName, drive, path, fname, ext);
	_makepath(NewName, drive, path, fname, NewExt);
	return NewName;
}

/*****************************************************************
	�t�@�C������u��������
*****************************************************************/
char *MakeNameFName(char *NewName, const char *BaseName, const char *NewFName)
{
	char drive[_MAX_DRIVE];
	char path[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(BaseName, drive, path, fname, ext);
	_makepath(NewName, drive, path, NewFName, ext);
	return NewName;
}

/*****************************************************************
	�f�B���N�g������u��������
*****************************************************************/
char *MakeDirFName(char *NewName, const char *BaseName, const char *NewDir)
{
	char BaseDrive[_MAX_DRIVE],	NewDrive[_MAX_DRIVE];
	char BasePath [_MAX_DIR],	NewPath [_MAX_DIR];
	char BaseFname[_MAX_FNAME],	NewFname[_MAX_FNAME];
	char BaseExt  [_MAX_EXT],	NewExt  [_MAX_EXT];
	char TmpDir[_MAX_DIR];
	strcpy(TmpDir,NewDir);
	MakeRegularPath(TmpDir);

	_splitpath(BaseName, BaseDrive, BasePath, BaseFname, BaseExt);
	_splitpath(TmpDir,   NewDrive,  NewPath,  NewFname,  NewExt);
	_makepath(NewName, NewDrive, NewPath, BaseFname, BaseExt);
	return NewName;
}

/*****************************************************************
	�h���C�u����u��������
*****************************************************************/
char *MakeDrvFName(char *NewName, const char *BaseName, const char *NewDrv)
{
	char drive[_MAX_DRIVE];
	char path [_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext  [_MAX_EXT];

	_splitpath(BaseName, drive, path, fname, ext);
	_makepath(NewName, NewDrv, path, fname, ext);
	return NewName;
}

/*****************************************************************
	�h���C�u����UNC�Œu��������
*****************************************************************/
char *MakeUncFName(char *NewName, const char *BaseName, const char *NewUnc)
{
	char drive[_MAX_PATH];
	char path[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(BaseName, drive, path, fname, ext);
	strcpy(NewName, NewUnc);
	if(NewName[strlen(NewName)-1] == '\\') NewName[strlen(NewName)-1] = '\0';
	strcat(NewName, path);
	strcat(NewName, fname);
	strcat(NewName, ext);
	return NewName;
}

/*****************************************************************
	�t�@�C��������h���C�u�A�p�X���폜�������̂𓾂�
*****************************************************************/
char *MakeNameFName(char *NewName, const char *BaseName)
{
	char drive[_MAX_DRIVE];
	char path[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(BaseName, drive, path, fname, ext);
	_makepath(NewName, NULL, NULL, fname, ext);
	return NewName;
}

/*****************************************************************
	�t�@�C��������h���C�u�A�p�X�A�g���q���폜�������̂𓾂�
*****************************************************************/
char *MakeBaseFName(char *NewName, const char *BaseName)
{
	char drive[_MAX_DRIVE];
	char path[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(BaseName, drive, path, fname, ext);
	_makepath(NewName, NULL, NULL, fname, NULL);
	return NewName;
}

/*****************************************************************
	�h���C�u���A�p�X���݂̂𓾂�
*****************************************************************/
char *MakeDirFName(char *NewName, const char *BaseName)
{
	char drive[_MAX_DRIVE];
	char path[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(BaseName, drive, path, fname, ext);
	_makepath(NewName, drive, path, NULL, NULL);
	return NewName;
}

/*****************************************************************
	�p�X����
*****************************************************************/
BOOL which(LPCTSTR lpPathList, TCHAR cDelimiter, LPCTSTR lpFile, LPTSTR lpBuf, DWORD cchBuf, LPDWORD lpdwCopied)
{
	BOOL bRes = FALSE;
	for(DWORD n=0;;n++) {
		TString<MAX_PATH> tmp;
		if(!GetCSVIndexedString(lpPathList,cDelimiter,n,tmp,tmp.chars(),NULL)) {
			break;
		}
		MakeNameFName(tmp,tmp,lpFile);
		DWORD dw = ::GetFileAttributes(tmp);
		//CHECKRESULT_API(dw!=0xffffffff);
		if(dw!=0xffffffff) {
			DWORD len = tmp.length();
			if(len < cchBuf) {
				tmp.copy_to(lpBuf);
				if(lpdwCopied) {
					*lpdwCopied = len;
				}
				bRes = TRUE;
			}
			break;
		}
	}
	return bRes;
}
BOOL which(LPCTSTR lpPathList, LPCTSTR lpDelimiter, LPCTSTR lpFile, LPTSTR lpBuf, DWORD cchBuf, LPDWORD lpdwCopied)
{
	BOOL bRes = FALSE;
	for(DWORD n=0;;n++) {
		TString<MAX_PATH> tmp;
		if(!GetCSVIndexedString(lpPathList,lpDelimiter,n,tmp,tmp.chars(),NULL)) {
			break;
		}
		MakeNameFName(tmp,tmp,lpFile);
		DWORD dw = ::GetFileAttributes(tmp);
		//CHECKRESULT_API(dw!=0xffffffff);
		if(dw!=0xffffffff) {
			DWORD len = tmp.length();
			if(len < cchBuf) {
				tmp.copy_to(lpBuf);
				if(lpdwCopied) {
					*lpdwCopied = len;
				}
				bRes = TRUE;
			}
			break;
		}
	}
	return bRes;
}
/*****************************************************************
	�Œ菬���_���ϊ�
*****************************************************************/
FIXED FixedFromDouble(double d)
{
	LONG l = (LONG)(d * 65536L);
	return *(FIXED *)&l;
}
int FixedFromDouble(FIXED f)
{
	if(f.fract >= 0x8000) {
		return f.value + 1;
	} else {
		return f.value;
	}
}

/*****************************************************************
	�f�o�b�O�q�[�v�̗L����
*****************************************************************/
#ifdef _DEBUG
__hmc_util__ __static_initializer_hmc_util__;
__hmc_util__::__hmc_util__()
{
#if 1
	int tmp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmp |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmp);
#else
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|
				   //_CRTDBG_CHECK_ALWAYS_DF|
				   //_CRTDBG_CHECK_CRT_DF|
				   //_CRTDBG_DELAY_FREE_MEM_DF|
				   _CRTDBG_LEAK_CHECK_DF);
#endif
}
#endif

/*****************************************************************
	Win32 API,COM�̃��U���g�R�[�h����
*****************************************************************/
BOOL CheckResult(BOOL bResult, LPCTSTR szFileName, UINT nLineNumber)
{
	if(!bResult) {
		DWORD dwResult = ::GetLastError();
		if(dwResult != ERROR_SUCCESS) {
			PrintResult(dwResult,szFileName,nLineNumber);
		}
		::SetLastError(dwResult);	// LastError����
	}
	return bResult;
}
void PrintResult(DWORD dwResult, LPCTSTR szFileName, UINT nLineNumber)
{
	TString<512> s;
	s.format("0x%08x:",dwResult);
	::OutputDebugString(s);
	LPCTSTR p = GetErrorMessage(dwResult,s,sizeof(s));
	if(s[s.length()-1] == '\n') {
		s[s.length()-1] = '\0';
	}
	::OutputDebugString(p);
	::OutputDebugString(": File ");
	::OutputDebugString(szFileName);
	::OutputDebugString(", Line ");
	s.format("%u\n",nLineNumber);
	::OutputDebugString(s);
}
LPCTSTR GetErrorMessage(DWORD dwResult, LPTSTR lpBuf, DWORD cbBuf)
{
	LPTSTR p = NULL;
	if((lpBuf&&cbBuf)) { lpBuf[0] = NULL; }
	HRESULT h = ::GetLastError();
	DWORD cb = ::FormatMessage(
				((lpBuf&&cbBuf)?0:FORMAT_MESSAGE_ALLOCATE_BUFFER)|
				FORMAT_MESSAGE_IGNORE_INSERTS|
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				dwResult,
				LANG_USER_DEFAULT,
				(lpBuf&&cbBuf)?lpBuf:(LPTSTR)&p,
				(lpBuf&&cbBuf)?cbBuf:0,
				NULL
				);
	::SetLastError(h);	// LastError����
	//return (lpBuf&&cb)?lpBuf:p;
	return (lpBuf&&cbBuf)?lpBuf:p;
}

/*****************************************************************
	����ASSERT,TRACE
*****************************************************************/
void Trace(LPCTSTR form, ...)
{
	TString<512> s;		// 512��AfxTrace�Ɠ���(�͂�)
	va_list args;
	va_start(args, form);
	s.format(form, args);
	::OutputDebugString(s);
}

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
