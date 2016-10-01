/*
	�����E�����񑀍샆�[�e�B���e�B
	Copyright (C)2000-2002 H.Mutsuki
*/

#include "util.h"
#include "string.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	mbs<->mbc�ϊ�
*****************************************************************/
UINT mbs2mbc(LPCSTR szChar)
{
	UINT uChar;
	if(_ismbslead(LPBYTE(szChar),LPBYTE(szChar))) {
		uChar = (WORD)(*((LPBYTE)szChar)) << 8;
		uChar |= *(((LPBYTE)szChar)+1);
	} else {
		uChar = (BYTE)*szChar;
	}
	return uChar;
}
void mbc2mbs(UINT uChar, LPSTR szChar)
{
	if(uChar<256) {
		szChar[0] = uChar;
		szChar[1] = '\0';
	} else {
		szChar[0] = (uChar >> 8) &0xff;
		szChar[1] = (uChar >> 0) &0xff;
		szChar[2] = '\0';
	}
}

/*****************************************************************
	������ɑS�p�������܂܂�Ă��邩��������(��������Ԃ�)
*****************************************************************/
DWORD HaveMBChar(LPCSTR lpText)
{
	DWORD dw=0;
	for(LPCSTR p=lpText; *p;) {
		if(_ismbslead((LPBYTE)lpText,(LPBYTE)p)&&_ismbstrail((LPBYTE)lpText,(LPBYTE)p+1)) {
			dw++;
			p += 2;
		} else {
			p += 1;
		}
	}
	return dw;
}

/*****************************************************************
	���p<->�S�p������ϊ�
*****************************************************************/
static const struct {
	char han;
	char *zen;
} __zen2han_dakuten_tbl__[] = {
	{'�',"��"},
	{'�',"�K"},{'�',"�M"},{'�',"�O"},{'�',"�Q"},{'�',"�S"},
	{'�',"�U"},{'�',"�W"},{'�',"�Y"},{'�',"�["},{'�',"�]"},
	{'�',"�_"},{'�',"�a"},{'�',"�d"},{'�',"�f"},{'�',"�h"},
	{'�',"�o"},{'�',"�r"},{'�',"�u"},{'�',"�x"},{'�',"�{"},
};
static const struct {
	char han;
	char *zen;
} __zen2han_handakuten_tbl__[] = {
	{'�',"�p"},{'�',"�s"},{'�',"�v"},{'�',"�y"},{'�',"�|"},
};
static const struct {
	char han;
	char *zen;
} __zen2han_tbl__[] = {
	{'0',"�O"},{'1',"�P"},{'2',"�Q"},{'3',"�R"},{'4',"�S"},{'5',"�T"},{'6',"�U"},{'7',"�V"},{'8',"�W"},{'9',"�X"},
	{'a',"��"},{'b',"��"},{'c',"��"},{'d',"��"},{'e',"��"},{'f',"��"},{'g',"��"},{'h',"��"},{'i',"��"},{'j',"��"},{'k',"��"},{'l',"��"},{'m',"��"},{'n',"��"},{'o',"��"},{'p',"��"},{'q',"��"},{'r',"��"},{'s',"��"},{'t',"��"},{'u',"��"},{'v',"��"},{'w',"��"},{'x',"��"},{'y',"��"},{'z',"��"},
	{'A',"�`"},{'B',"�a"},{'C',"�b"},{'D',"�c"},{'E',"�d"},{'F',"�e"},{'G',"�f"},{'H',"�g"},{'I',"�h"},{'J',"�i"},{'K',"�j"},{'L',"�k"},{'M',"�l"},{'N',"�m"},{'O',"�n"},{'P',"�o"},{'Q',"�p"},{'R',"�q"},{'S',"�r"},{'T',"�s"},{'U',"�t"},{'V',"�u"},{'W',"�v"},{'X',"�w"},{'Y',"�x"},{'Z',"�y"},
	{' ',"�@"},{'!',"�I"},{'\"',"�h"},{'#',"��"},{'$',"��"},{'%',"��"},{'&',"��"},{'\'',"�f"},{'(',"�i"},{')',"�j"},{'*',"��"},{'+',"�{"},{',',"�C"},{'-',"�|"},{'.',"�D"},{'/',"�^"},
	{':',"�F"},{';',"�G"},{'<',"��"},{'=',"��"},{'>',"��"},{'?',"�H"},{'@',"��"},{'[',"�m"},{'\\',"��"},{']',"�n"},{'^',"�O"},{'_',"�Q"},{'`',"�M"},
	{'{',"�o"},{'|',"�b"},{'}',"�p"},{'~',"�`"},
	{'�',"�A"},{'�',"�B"},{'�',"�E"},{'�',"�J"},{'�',"�K"},
	{'�',"�A"},{'�',"�C"},{'�',"�E"},{'�',"�G"},{'�',"�I"},
	{'�',"�J"},{'�',"�L"},{'�',"�N"},{'�',"�P"},{'�',"�R"},
	{'�',"�T"},{'�',"�V"},{'�',"�X"},{'�',"�Z"},{'�',"�\"},
	{'�',"�^"},{'�',"�`"},{'�',"�c"},{'�',"�e"},{'�',"�g"},
	{'�',"�i"},{'�',"�j"},{'�',"�k"},{'�',"�l"},{'�',"�m"},
	{'�',"�n"},{'�',"�q"},{'�',"�t"},{'�',"�w"},{'�',"�z"},
	{'�',"�}"},{'�',"�~"},{'�',"��"},{'�',"��"},{'�',"��"},
	{'�',"��"},{'�',"��"},{'�',"��"},
	{'�',"��"},{'�',"��"},{'�',"��"},{'�',"��"},{'�',"��"},
	{'�',"��"},{'�',"��"},{'�',"��"},
	{'�',"�@"},{'�',"�B"},{'�',"�D"},{'�',"�F"},{'�',"�H"},
	{'�',"��"},{'�',"��"},{'�',"��"},
	{'�',"�b"},
	{'�',"�["},
	{'�',"�u"},
	{'�',"�v"},
};
DWORD han2zen(LPCSTR lpText, LPSTR lpBuf)
{
	DWORD nRes = 0;
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && !_ismbblead(*lpText)) {
		// �����`�F�b�N
		if('0'<=*lpText && *lpText<='9') {
			mbc2mbs(mbs2mbc("�O") + (*lpText) - '0', lpBuf);
			nRes = 1;
		}
		// �A���t�@�x�b�g�啶���`�F�b�N
		else if('A'<=*lpText && *lpText<='Z') {
			mbc2mbs(mbs2mbc("�`") + (*lpText) - 'A', lpBuf);
			nRes = 1;
		}
		// �A���t�@�x�b�g�������`�F�b�N
		else if('a'<=*lpText && *lpText<='z') {
			mbc2mbs(mbs2mbc("��") + (*lpText) - 'a', lpBuf);
			nRes = 1;
		}
		// �J�^�J�i���_�`�F�b�N
		else if(*(lpText+1)=='�') {
			for(int n=0; n<ARRAYLEN(__zen2han_dakuten_tbl__); n++) {
				if(*lpText==__zen2han_dakuten_tbl__[n].han) {
					strcpy(lpBuf,__zen2han_dakuten_tbl__[n].zen);
					nRes = 2;
					break;
				}
			}
		}
		// �J�^�J�i�����_�`�F�b�N
		else if(*(lpText+1)=='�') {
			for(int n=0; n<ARRAYLEN(__zen2han_handakuten_tbl__); n++) {
				if(*lpText==__zen2han_handakuten_tbl__[n].han) {
					strcpy(lpBuf,__zen2han_handakuten_tbl__[n].zen);
					nRes = 2;
					break;
				}
			}
		}
		// ���̑��̕���
		else {
			for(int n=0; n<ARRAYLEN(__zen2han_tbl__); n++) {
				if(*lpText==__zen2han_tbl__[n].han) {
					strcpy(lpBuf,__zen2han_tbl__[n].zen);
					nRes = 1;
					break;
				}
			}
		}
		ASSERT(0<nRes);
	}
	return nRes;
}
DWORD zen2han(LPCSTR lpText, LPSTR lpBuf)
{
	DWORD nRes = 0;
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && _ismbcprint(mbs2mbc(lpText))) {
		// �����`�F�b�N
		if(mbs2mbc("�O")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("�X")) {
			lpBuf[0] = '0' + mbs2mbc(lpText) - mbs2mbc("�O");
			lpBuf[1] = '\0';
			nRes = 2;
		}
		// �A���t�@�x�b�g�啶���`�F�b�N
		else if(mbs2mbc("�`")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("�y")) {
			lpBuf[0] = 'A' + mbs2mbc(lpText) - mbs2mbc("�`");
			lpBuf[1] = '\0';
			nRes = 2;
		}
		// �A���t�@�x�b�g�������`�F�b�N
		else if(mbs2mbc("��")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("��")) {
			lpBuf[0] = 'a' + mbs2mbc(lpText) - mbs2mbc("��");
			lpBuf[1] = '\0';
			nRes = 2;
		}
		else {
			// �J�^�J�i���_�`�F�b�N
			if(nRes==0) {
				for(int n=0; n<ARRAYLEN(__zen2han_dakuten_tbl__); n++) {
					if(mbs2mbc(lpText)==mbs2mbc(__zen2han_dakuten_tbl__[n].zen)) {
						lpBuf[0] = __zen2han_dakuten_tbl__[n].han;
						lpBuf[1] = '�';
						lpBuf[2] = '\0';
						nRes = 2;
						break;
					}
				}
			}
			// �J�^�J�i�����_�`�F�b�N
			if(nRes==0) {
				for(int n=0; n<ARRAYLEN(__zen2han_handakuten_tbl__); n++) {
					if(mbs2mbc(lpText)==mbs2mbc(__zen2han_handakuten_tbl__[n].zen)) {
						lpBuf[0] = __zen2han_handakuten_tbl__[n].han;
						lpBuf[1] = '�';
						lpBuf[2] = '\0';
						nRes = 2;
						break;
					}
				}
			}
			// ���̑��̕���
			if(nRes==0) {
				for(int n=0; n<ARRAYLEN(__zen2han_tbl__); n++) {
					if(mbs2mbc(lpText)==mbs2mbc(__zen2han_tbl__[n].zen)) {
						lpBuf[0] = __zen2han_tbl__[n].han;
						lpBuf[1] = '\0';
						nRes = 2;
						break;
					}
				}
			}
		}
		ASSERT(0<nRes);
	}
	return nRes;
}
DWORD han2zen(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf)
{
	DWORD nRes = 0;
	char str[3] = {0};
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && 1<cchBuf) {
		DWORD m = 0;
		for(DWORD n=0; lpText[m]!='\0'&&m<cchBuf-1;) {
			DWORD l = han2zen(&lpText[m],str);
			if(l==0) {
				if(_ismbblead(lpText[m])) {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = lpText[m+1];
					lpBuf[n+2] = '\0';
					n += 2;
					m += 2;
				} else {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = '\0';
					n += 1;
					m += 1;
				}
			} else {
				DWORD len = strlen(str);
				if(n+len<cchBuf) {
					memcpy(&lpBuf[n],str,len);
					lpBuf[n+len] = '\0';
					n += len;
					m += l;
				} else {
					break;
				}
			}
		}
		nRes = m;
	}
	return nRes;
}
DWORD zen2han(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf)
{
	DWORD nRes = 0;
	char str[3] = {0};
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && 1<cchBuf) {
		DWORD m = 0;
		for(DWORD n=0; lpText[m]!='\0'&&m<cchBuf-1;) {
			DWORD l = zen2han(&lpText[m],str);
			if(l==0) {
				if(_ismbblead(lpText[m])) {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = lpText[m+1];
					lpBuf[n+2] = '\0';
					n += 2;
					m += 2;
				} else {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = '\0';
					n += 1;
					m += 1;
				}
			} else {
				DWORD len = strlen(str);
				if(n+len<cchBuf) {
					memcpy(&lpBuf[n],str,len);
					lpBuf[n+len] = '\0';
					n += len;
					m += l;
				} else {
					break;
				}
			}
		}
		nRes = m;
	}
	return nRes;
}

/*****************************************************************
	�J�^�J�i<->�Ђ炪�ȕ�����ϊ�
*****************************************************************/
DWORD kata2hira(LPCSTR lpText, LPSTR lpBuf)
{
	DWORD nRes = 0;
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && _ismbcprint(mbs2mbc(lpText))) {
		// ���ʃR�[�h���`�F�b�N
		if(mbs2mbc("�@")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("�~")) {
			mbc2mbs(mbs2mbc(lpText) - (mbs2mbc("�@") - mbs2mbc("��")), lpBuf);
			nRes = 2;
		} else if(mbs2mbc("��")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("��")) {
			mbc2mbs(mbs2mbc(lpText) - (mbs2mbc("��") - mbs2mbc("��")), lpBuf);
			nRes = 2;
		}
		// �J�^�J�i�݂̂̕����`�F�b�N
		else {
			static const struct {
				char *kata;
				char *hira;
			} a[] = {
				{"��","���J"},
				{"��","��"},	// ���ϊ�����Ȃ����̓}�V���낤�c�c
				{"��","��"},	// ���ϊ�����Ȃ����̓}�V���낤�c�c
			};
			for(int n=0; n<ARRAYLEN(a); n++) {
				if(mbs2mbc(lpText)==mbs2mbc(a[n].kata)) {
					strcpy(lpBuf,a[n].hira);
					nRes = strlen(a[n].kata);
					break;
				}
			}
		}
	}
	return nRes;
}
DWORD hira2kata(LPCSTR lpText, LPSTR lpBuf)
{
	DWORD nRes = 0;
	if(lpText && lpBuf && _ismbcprint(mbs2mbc(lpText))) {
		// ���ʃR�[�h���`�F�b�N
		if(mbs2mbc("��")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("��")) {
			mbc2mbs(mbs2mbc(lpText) + (mbs2mbc("�@") - mbs2mbc("��")), lpBuf);
			nRes = 2;
		} else if(mbs2mbc("��")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("��")) {
			mbc2mbs(mbs2mbc(lpText) + (mbs2mbc("��") - mbs2mbc("��")), lpBuf);
			nRes = 2;
		}
		// �J�^�J�i�݂̂̕����`�F�b�N
		else {
			static const struct {
				char *hira;
				char *kata;
			} a[] = {
				{"���J","��"},
			};
			for(int n=0; n<ARRAYLEN(a); n++) {
				if(strncmp(lpText,a[n].hira,strlen(a[n].hira))==0) {
					strcpy(lpBuf,a[n].kata);
					nRes = strlen(a[n].hira);
					break;
				}
			}
		}
	} else if(lpBuf) {
		lpBuf[0] = '\0';
	}
	return nRes;
}
DWORD kata2hira(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf)
{
	DWORD nRes = 0;
	char str[5] = {0};
	if(lpText && lpBuf && 1<cchBuf) {
		DWORD m = 0;
		for(DWORD n=0; lpText[m]!='\0'&&m<cchBuf-1;) {
			DWORD l = kata2hira(&lpText[m],str);
			if(l==0) {
				if(_ismbblead(lpText[m])) {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = lpText[m+1];
					lpBuf[n+2] = '\0';
					n += 2;
					m += 2;
				} else {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = '\0';
					n += 1;
					m += 1;
				}
			} else {
				DWORD len = strlen(str);
				if(n+len<cchBuf) {
					memcpy(&lpBuf[n],str,len);
					lpBuf[n+len] = '\0';
					n += len;
					m += l;
				} else {
					break;
				}
			}
		}
		nRes = m;
	}
	return nRes;
}
DWORD hira2kata(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf)
{
	DWORD nRes = 0;
	char str[3] = {0};
	if(lpText && lpBuf && 1<cchBuf) {
		DWORD m = 0;
		for(DWORD n=0; lpText[m]!='\0'&&m<cchBuf-1;) {
			DWORD l = hira2kata(&lpText[m],str);
			if(l==0) {
				if(_ismbblead(lpText[m])) {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = lpText[m+1];
					lpBuf[n+2] = '\0';
					n += 2;
					m += 2;
				} else {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = '\0';
					n += 1;
					m += 1;
				}
			} else {
				DWORD len = strlen(str);
				if(n+len<cchBuf) {
					memcpy(&lpBuf[n],str,len);
					lpBuf[n+len] = '\0';
					n += len;
					m += l;
				} else {
					break;
				}
			}
		}
		nRes = m;
	}
	return nRes;
}

/*****************************************************************
	���[�}���J�^�J�i(�S�p)�ϊ�
*****************************************************************/
static const struct {
	char *roma;
	char *kana;
} __roma2kana_tbl__[] = {
	// �A�s
	{"wwi","�b�E�B"},{"wwe","�b�E�F"},
	{"vva","�b���@"},{"vvi","�b���B"},               {"vve","�b���F"},{"vvo","�b���H"},
	{"vvu","�b��"},
	{"wi","�E�B"},{"we","�E�F"},
	{"wha","�E�@"},{"whi","�E�B"},        {"whe","�E�F"},{"who","�E�H"},
	{"va","���@"},{"vi","���B"},          {"ve","���F"},{"vo","���H"},
	{"vu","��"},
	{ "a","�A"},{ "i","�C"},{ "u","�E"},{ "e","�G"},{ "o","�I"},

	// �J�s
	{"kkya","�b�L��"},{"kkyi","�b�L�B"},{"kkyu","�b�L��"},{"kkye","�b�L�F"},{"kkyo","�b�L��"},
	{"ggya","�b�M��"},{"ggyi","�b�M�B"},{"ggyu","�b�M��"},{"ggye","�b�M�F"},{"ggyo","�b�M��"},
	{ "qqa","�b�N�@"},{ "qqi","�b�N�B"},{ "qqu","�b�N�B"},{ "qqe","�b�N�F"},{ "qqo","�b�N�H"},
	{"kka","�b�J"},{"kki","�b�L"},{"kku","�b�N"},{"kke","�b�P"},{"kko","�b�R"},
	{"gga","�b�K"},{"ggi","�b�M"},{"ggu","�b�O"},{"gge","�b�Q"},{"ggo","�b�S"},
	{"kya","�L��"},{"kyi","�L�B"},{"kyu","�L��"},{"kye","�L�F"},{"kyo","�L��"},
	{ "qa","�N�@"},{ "qi","�N�B"},               { "qe","�N�F"},{ "qo","�N�H"},
	{"gya","�M��"},{"gyi","�M�B"},{"gyu","�M��"},{"gye","�M�F"},{"gyo","�M��"},
	{"ka","�J"},{"ki","�L"},{"ku","�N"},{"ke","�P"},{"ko","�R"},
	{"ca","�J"},{"cu","�N"},{"co","�R"},
	{"qu","�N"  },
	{"ga","�K"},{"gi","�M"},{"gu","�O"},{"ge","�Q"},{"go","�S"},

	// �T�s
	{"ssya","�b�V��"},{"ssyi","�b�V�B"},{"ssyu","�b�V��"},{"ssye","�b�V�F"},{"ssyo","�b�V��"},
	{"ssha","�b�V��"},                  {"sshu","�b�V��"},{"sshe","�b�V�F"},{"ssho","�b�V��"},
	{ "jja","�b�W��"},                  { "jju","�b�W��"},{ "jje","�b�W�F"},{ "jjo","�b�W��"},
	{"jjya","�b�W��"},{"jjyi","�b�W�B"},{"jjyu","�b�W��"},{"jjye","�b�W�F"},{"jjyo","�b�W��"},
	{"zzya","�b�W��"},{"zzyi","�b�W�B"},{"zzyu","�b�W��"},{"zzye","�b�W�F"},{"zzyo","�b�W��"},
	{"ssa","�b�T"},{"ssi","�b�V"},{"ssu","�b�X"},{"sse","�b�Z"},{"sso","�b�\"},
	{"sshi","�b�V"  },
	{ "jji","�b�W"  },
	{"zza","�b�U"},{"zzi","�b�W"},{"zzu","�b�Y"},{"zze","�b�["},{"zzo","�b�]"},
	{"sya","�V��"},{"syi","�V�B"},{"syu","�V��"},{"sye","�V�F"},{"syo","�V��"},
	{"sha","�V��"},               {"shu","�V��"},{"she","�V�F"},{"sho","�V��"},
	{ "ja","�W��"},               { "ju","�W��"},{ "je","�W�F"},{ "jo","�W��"},
	{"jya","�W��"},{"jyi","�W�B"},{"jyu","�W��"},{"jye","�W�F"},{"jyo","�W��"},
	{"zya","�W��"},{"zyi","�W�B"},{"zyu","�W��"},{"zye","�W�F"},{"zyo","�W��"},
	{"sa","�T"},{"si","�V"},{"su","�X"},{"se","�Z"},{"so","�\"},
	{"shi","�V"  },
	{ "ji","�W"  },
	{"za","�U"},{"zi","�W"},{"zu","�Y"},{"ze","�["},{"zo","�]"},

	// �^�s
	{"ttya","�b�`��"},{"ttyi","�b�`�B"},{"ttyu","�b�`��"},{"ttye","�b�`�F"},{"ttyo","�b�`��"},
	{"ccya","�b�`��"},{"ccyi","�b�`�B"},{"ccyu","�b�`��"},{"ccye","�b�`�F"},{"ccyo","�b�`��"},
	{"ccha","�b�`��"},                  {"cchu","�b�`��"},{"cche","�b�`�F"},{"ccho","�b�`��"},
	{"ttha","�b�e��"},{"tthi","�b�e�B"},{"tthu","�b�e��"},{"tthe","�b�e�F"},{"ttho","�b�e��"},
	{"ttsa","�b�c�@"},{"ttsi","�b�c�B"},                  {"ttse","�b�c�F"},{"ttso","�b�c�H"},
	{"ddya","�b�a��"},{"ddyi","�b�a�B"},{"ddyu","�b�a��"},{"ddye","�b�a�F"},{"ddyo","�b�a��"},
	{"ddha","�b�f��"},{"ddhi","�b�f�B"},{"ddhu","�b�f��"},{"ddhe","�b�f�F"},{"ddho","�b�f��"},
	{"tta","�b�^"},{"tti","�b�`"},{"ttu","�b�c"},{"tte","�b�e"},{"tto","�b�g"},
	{"ttsu","�b�c"  },
	{"cchi","�b�`"  },
	{"dda","�b�_"},{"ddi","�b�a"},{"ddu","�b�d"},{"dde","�b�f"},{"ddo","�b�h"},
	{"tya","�`��"},{"tyi","�`�B"},{"tyu","�`��"},{"tye","�`�F"},{"tyo","�`��"},
	{"cha","�`��"},               {"chu","�`��"},{"che","�`�F"},{"cho","�`��"},
	{"cya","�`��"},{"cyi","�`�B"},{"cyu","�`��"},{"cye","�`�F"},{"cyo","�`��"},
	{"tha","�e��"},{"thi","�e�B"},{"thu","�e��"},{"the","�e�F"},{"tho","�e��"},
	{"tsa","�c�@"},{"tsi","�c�B"},               {"tse","�c�F"},{"tso","�c�H"},
	{"dya","�a��"},{"dyi","�a�B"},{"dyu","�a��"},{"dye","�a�F"},{"dyo","�a��"},
	{"dha","�f��"},{"dhi","�f�B"},{"dhu","�f��"},{"dhe","�f�F"},{"dho","�f��"},
	{ "ta","�^"},{ "ti","�`"},{ "tu","�c"},{ "te","�e"},{ "to","�g"},
	{"chi","�`"  },
	{"tsu","�c"  },
	{ "da","�_"},{ "di","�a"},{ "du","�d"},{ "de","�f"},{ "do","�h"},

	// �i�s
	{"nya","�j��"},{"nyi","�j�B"},{"nyu","�j��"},{"nye","�j�F"},{"nyo","�j��"},
	{"na","�i"},{"ni","�j"},{"nu","�k"},{"ne","�l"},{"no","�m"},

	// �n�s
	{"hhya","�b�q��"},{"hhyi","�b�q�B"},{"hhyu","�b�q��"},{"hhye","�b�q�F"},{"hhyo","�b�q��"},
	{"bbya","�b�r��"},{"bbyi","�b�r�B"},{"bbyu","�b�r��"},{"bbye","�b�r�F"},{"bbyo","�b�r��"},
	{"ppya","�b�s��"},{"ppyi","�b�s�B"},{"ppyu","�b�s��"},{"ppye","�b�s�F"},{"ppyo","�b�s��"},
	{ "ffa","�b�t�@"},{ "ffi","�b�t�B"},                  { "ffe","�b�t�F"},{ "ffo","�b�t�H"},
	{"ffya","�b�t��"},{"ffyi","�b�t�B"},{"ffyu","�b�t��"},{"ffye","�b�t�F"},{"ffyo","�b�t��"},
	{ "ffu","�b�t"  },
	{"hha","�b�n"},{"hhi","�b�q"},{"hhu","�b�t"},{"hhe","�b�w"},{"hho","�b�z"},
	{"bba","�b�o"},{"bbi","�b�r"},{"bbu","�b�u"},{"bbe","�b�x"},{"bbo","�b�{"},
	{"ppa","�b�p"},{"ppi","�b�s"},{"ppu","�b�v"},{"ppe","�b�y"},{"ppo","�b�|"},
	{"hya","�q��"},{"hyi","�q�B"},{"hyu","�q��"},{"hye","�q�F"},{"hyo","�q��"},
	{"bya","�r��"},{"byi","�r�B"},{"byu","�r��"},{"bye","�r�F"},{"byo","�r��"},
	{"pya","�s��"},{"pyi","�s�B"},{"pyu","�s��"},{"pye","�s�F"},{"pyo","�s��"},
	{ "fa","�t�@"},{ "fi","�t�B"},               { "fe","�t�F"},{ "fo","�t�H"},
	{"fya","�t��"},{"fyi","�t�B"},{"fyu","�t��"},{"fye","�t�F"},{"fyo","�t��"},
	{ "fu","�t"  },
	{"ha","�n"},{"hi","�q"},{"hu","�t"},{"he","�w"},{"ho","�z"},
	{"ba","�o"},{"bi","�r"},{"bu","�u"},{"be","�x"},{"bo","�{"},
	{"pa","�p"},{"pi","�s"},{"pu","�v"},{"pe","�y"},{"po","�|"},

	// �}�s
	{"mya","�~��"},{"myi","�~�B"},{"myu","�~��"},{"mye","�~�F"},{"myo","�~��"},
	{"ma","�}"},{"mi","�~"},{"mu","��"},{"me","��"},{"mo","��"},

	// ���s
	{"yya","�b��"},{"yyu","�b��"},{"yyo","�b��"},
	{"ya","��"},{"yu","��"},{"yo","��"},
	{"yi","�C"},{"ye","�C�F"},

	// ���s
	{"rrya","�b����"},{"rryi","�b���B"},{"rryu","�b����"},{"rrye","�b���F"},{"rryo","�b����"},
	{"rra","�b��"},{"rri","�b��"},{"rru","�b��"},{"rre","�b��"},{"rro","�b��"},
	{"rya","����"},{"ryi","���B"},{"ryu","����"},{"rye","���F"},{"ryo","����"},
	{"ra","��"},{"ri","��"},{"ru","��"},{"re","��"},{"ro","��"},

	// ���s
	{"wwa","�b��"},{"wwo","�b��"},
	{"wa","��"},{"wo","��"},
	{"nn","��"},

	// �A�s
	{"nwwi","���b�E�B"},{"nwwe","���b�E�F"},
	{"nvva","���b���@"},{"nvvi","���b���B"},               {"nvve","���b���F"},{"nvvo","���b���H"},
	{"nvvu","���b��"},
	{"nwi","���E�B"},{"nwe","���E�F"},
	{"nva","�����@"},{"nvi","�����B"},          {"nve","�����F"},{"nvo","�����H"},
	{"nvu","����"},
	{"nna","���A"},{"nni","���C"},{"nnu","���E"},{"nne","���G"},{"nno","���I"},

	// �J�s
	{"nkkya","���b�L��"},{"nkkyi","���b�L�B"},{"nkkyu","���b�L��"},{"nkkye","���b�L�F"},{"nkkyo","���b�L��"},
	{"nggya","���b�M��"},{"nggyi","���b�M�B"},{"nggyu","���b�M��"},{"nggye","���b�M�F"},{"nggyo","���b�M��"},
	{ "nqqa","���b�N�@"},{ "nqqi","���b�N�B"},                     { "nqqe","���b�N�F"},{ "nqqo","���b�N�H"},
	{"nkka","���b�J"},{"nkki","���b�L"},{"nkku","���b�N"},{"nkke","���b�P"},{"nkko","���b�R"},
	{"ncca","���b�J"},{"nccu","���b�N"},{"ncco","���b�R"},
	{"nqqu","���b�N"},
	{"nca","���J"},{"ncu","���N"},{"nco","���R"},
	{"ngga","���b�K"},{"nggi","���b�M"},{"nggu","���b�O"},{"ngge","���b�Q"},{"nggo","���b�S"},
	{"nkya","���L��"},{"nkyi","���L�B"},{"nkyu","���L��"},{"nkye","���L�F"},{"nkyo","���L��"},
	{ "nqa","���N�@"},{ "nqi","���N�B"},                  { "nqe","���N�F"},{ "nqo","���N�H"},
	{"ngya","���M��"},{"ngyi","���M�B"},{"ngyu","���M��"},{"ngye","���M�F"},{"ngyo","���M��"},
	{"nka","���J"},{"nki","���L"},{"nku","���N"},{"nke","���P"},{"nko","���R"},
	{"nca","���J"},{"ncu","���N"},{"nco","���R"},
	{"nqu","���N"  },
	{"nga","���K"},{"ngi","���M"},{"ngu","���O"},{"nge","���Q"},{"ngo","���S"},

	// �T�s
	{"nssya","���b�V��"},{"nssyi","���b�V�B"},{"nssyu","���b�V��"},{"nssye","���b�V�F"},{"nssyo","���b�V��"},
	{"nssha","���b�V��"},                     {"nsshu","���b�V��"},{"nsshe","���b�V�F"},{"nssho","���b�V��"},
	{ "njja","���b�W��"},                     { "njju","���b�W��"},{ "njje","���b�W�F"},{ "njjo","���b�W��"},
	{"njjya","���b�W��"},{"njjyi","���b�W�B"},{"njjyu","���b�W��"},{"njjye","���b�W�F"},{"njjyo","���b�W��"},
	{"nzzya","���b�W��"},{"nzzyi","���b�W�B"},{"nzzyu","���b�W��"},{"nzzye","���b�W�F"},{"nzzyo","���b�W��"},
	{"nssa","���b�T"},{"nssi","���b�V"},{"nssu","���b�X"},{"nsse","���b�Z"},{"nsso","���b�\"},
	{"nsshi","���b�V"  },
	{"ncci","���b�V"},{"ncce","���b�Z"},
	{ "njji","���b�W"  },
	{"nzza","���b�U"},{"nzzi","���b�W"},{"nzzu","���b�Y"},{"nzze","���b�["},{"nzzo","���b�]"},
	{"nsya","���V��"},{"nsyi","���V�B"},{"nsyu","���V��"},{"nsye","���V�F"},{"nsyo","���V��"},
	{"nsha","���V��"},                  {"nshu","���V��"},{"nshe","���V�F"},{"nsho","���V��"},
	{ "nja","���W��"},                  { "nju","���W��"},{ "nje","���W�F"},{ "njo","���W��"},
	{"njya","���W��"},{"njyi","���W�B"},{"njyu","���W��"},{"njye","���W�F"},{"njyo","���W��"},
	{"nzya","���W��"},{"nzyi","���W�B"},{"nzyu","���W��"},{"nzye","���W�F"},{"nzyo","���W��"},
	{"nsa","���T"},{"nsi","���V"},{"nsu","���X"},{"nse","���Z"},{"nso","���\"},
	{"nshi","���V"  },
	{"nci","���V"},{"nce","���Z"},
	{ "nji","���W"  },
	{"nza","���U"},{"nzi","���W"},{"nzu","���Y"},{"nze","���["},{"nzo","���]"},

	// �^�s
	{"nttya","���b�`��"},{"nttyi","���b�`�B"},{"nttyu","���b�`��"},{"nttye","���b�`�F"},{"nttyo","���b�`��"},
	{"nccya","���b�`��"},{"nccyi","���b�`�B"},{"nccyu","���b�`��"},{"nccye","���b�`�F"},{"nccyo","���b�`��"},
	{"nccha","���b�`��"},                     {"ncchu","���b�`��"},{"ncche","���b�`�F"},{"nccho","���b�`��"},
	{"nttha","���b�e��"},{"ntthi","���b�e�B"},{"ntthu","���b�e��"},{"ntthe","���b�e�F"},{"nttho","���b�e��"},
	{"nttsa","���b�c�@"},{"nttsi","���b�c�B"},                     {"nttse","���b�c�F"},{"nttso","���b�c�H"},
	{"nddya","���b�a��"},{"nddyi","���b�a�B"},{"nddyu","���b�a��"},{"nddye","���b�a�F"},{"nddyo","���b�a��"},
	{"nddha","���b�f��"},{"nddhi","���b�f�B"},{"nddhu","���b�f��"},{"nddhe","���b�f�F"},{"nddho","���b�f��"},
	{"ntta","���b�^"},{"ntti","���b�`"},{"nttu","���b�c"},{"ntte","���b�e"},{"ntto","���b�g"},
	{"nttsu","���b�c"  },
	{"ncchi","���b�`"  },
	{"ndda","���b�_"},{"nddi","���b�a"},{"nddu","���b�d"},{"ndde","���b�f"},{"nddo","���b�h"},
	{"ntya","���`��"},{"ntyi","���`�B"},{"ntyu","���`��"},{"ntye","���`�F"},{"ntyo","���`��"},
	{"ncha","���`��"},                  {"nchu","���`��"},{"nche","���`�F"},{"ncho","���`��"},
	{"ncya","���`��"},{"ncyi","���`�B"},{"ncyu","���`��"},{"ncye","���`�F"},{"ncyo","���`��"},
	{"ntha","���e��"},{"nthi","���e�B"},{"nthu","���e��"},{"nthe","���e�F"},{"ntho","���e��"},
	{"ntsa","���c�@"},{"ntsi","���c�B"},                  {"ntse","���c�F"},{"ntso","���c�H"},
	{"ndya","���a��"},{"ndyi","���a�B"},{"ndyu","���a��"},{"ndye","���a�F"},{"ndyo","���a��"},
	{"ndha","���f��"},{"ndhi","���f�B"},{"ndhu","���f��"},{"ndhe","���f�F"},{"ndho","���f��"},
	{ "nta","���^"},{ "nti","���`"},{ "ntu","���c"},{ "nte","���e"},{ "nto","���g"},
	{"nchi","���`"  },
	{"ntsu","���c"  },
	{ "nda","���_"},{ "ndi","���a"},{ "ndu","���d"},{ "nde","���f"},{ "ndo","���h"},

	// �i�s
	{"nnnya","���j��"},{"nnnyi","���j�B"},{"nnnyu","���j��"},{"nnnye","���j�F"},{"nnnyo","���j��"},
	{"nnna","���i"},{"nnni","���j"},{"nnnu","���k"},{"nnne","���l"},{"nnno","���m"},

	// �n�s
	{"nhhya","���b�q��"},{"nhhyi","���b�q�B"},{"nhhyu","���b�q��"},{"nhhye","���b�q�F"},{"nhhyo","���b�q��"},
	{"nbbya","���b�r��"},{"nbbyi","���b�r�B"},{"nbbyu","���b�r��"},{"nbbye","���b�r�F"},{"nbbyo","���b�r��"},
	{"nppya","���b�s��"},{"nppyi","���b�s�B"},{"nppyu","���b�s��"},{"nppye","���b�s�F"},{"nppyo","���b�s��"},
	{ "nffa","���b�t�@"},{ "nffi","���b�t�B"},                     { "nffe","���b�t�F"},{ "nffo","���b�t�H"},
	{"nffya","���b�t��"},{"nffyi","���b�t�B"},{"nffyu","���b�t��"},{"nffye","���b�t�F"},{"nffyo","���b�t��"},
	{ "nffu","���b�t"  },
	{"nhha","���b�n"},{"nhhi","���b�q"},{"nhhu","���b�t"},{"nhhe","���b�w"},{"nhho","���b�z"},
	{"nbba","���b�o"},{"nbbi","���b�r"},{"nbbu","���b�u"},{"nbbe","���b�x"},{"nbbo","���b�{"},
	{"nppa","���b�p"},{"nppi","���b�s"},{"nppu","���b�v"},{"nppe","���b�y"},{"nppo","���b�|"},
	{"nhya","���q��"},{"nhyi","���q�B"},{"nhyu","���q��"},{"nhye","���q�F"},{"nhyo","���q��"},
	{"nbya","���r��"},{"nbyi","���r�B"},{"nbyu","���r��"},{"nbye","���r�F"},{"nbyo","���r��"},
	{"npya","���s��"},{"npyi","���s�B"},{"npyu","���s��"},{"npye","���s�F"},{"npyo","���s��"},
	{ "nfa","���t�@"},{ "nfi","���t�B"},                  { "nfe","���t�F"},{ "nfo","���t�H"},
	{"nfya","���t��"},{"nfyi","���t�B"},{"nfyu","���t��"},{"nfye","���t�F"},{"nfyo","���t��"},
	{ "nfu","���t"  },
	{"nha","���n"},{"nhi","���q"},{"nhu","���t"},{"nhe","���w"},{"nho","���z"},
	{"nba","���o"},{"nbi","���r"},{"nbu","���u"},{"nbe","���x"},{"nbo","���{"},
	{"npa","���p"},{"npi","���s"},{"npu","���v"},{"npe","���y"},{"npo","���|"},

	// �}�s
	{"nmya","���~��"},{"nmyi","���~�B"},{"nmyu","���~��"},{"nmye","���~�F"},{"nmyo","���~��"},
	{"nma","���}"},{"nmi","���~"},{"nmu","����"},{"nme","����"},{"nmo","����"},

	// ���s
	{"nyya","���b��"},{"nyyu","���b��"},{"nyyo","���b��"},
	{"nnya","����"},{"nnyu","����"},{"nnyo","����"},
	{"nnyi","���C"},{"nnye","���C�F"},

	// ���s
	{"nrrya","���b����"},{"nrryi","���b���B"},{"nrryu","���b����"},{"nrrye","���b���F"},{"nrryo","���b����"},
	{"nrra","���b��"},{"nrri","���b��"},{"nrru","���b��"},{"nrre","���b��"},{"nrro","���b��"},
	{"nrya","������"},{"nryi","�����B"},{"nryu","������"},{"nrye","�����F"},{"nryo","������"},
	{"nra","����"},{"nri","����"},{"nru","����"},{"nre","����"},{"nro","����"},

	// ���s(��������)
	{"nwwa","���b��"},{"nwwo","���b��"},
	{"nwa","����"},{"nwo","����"},

	// ����������
	{"nxa","���@"},{"nxi","���B"},{"nxu","���D"},{"nxe","���F"},{"nxo","���H"},
	{"nla","���@"},{"nli","���B"},{"nlu","���D"},{"nle","���F"},{"nlo","���H"},
	{"nxka","����"},{"nxke","����"},
	{"nlka","����"},{"nlke","����"},

	{"nxa","���@"},{"nxi","���B"},{"nxu","���D"},{"nxe","���F"},{"nxo","���H"},
	{"nla","���@"},{"nli","���B"},{"nlu","���D"},{"nle","���F"},{"nlo","���H"},
	{"nxye","���F"},
	{"nlye","���F"},
	{"nxka","����"},{"nxke","����"},
	{"nlka","����"},{"nlke","����"},
	{"nxtu","���b"},{"nltu","���b"},
	{"nxtsu","���b"},{"nltsu","���b"},
	{"nxya","����"},{"nxyu","����"},{"nxyo","����"},
	{"nlya","����"},{"nlyu","����"},{"nlyo","����"},
	{"nxwa","����"},
	{"nlwa","����"},

	// �w�{�����ł�b,m,p�̒��O�́wm�x�́w���x�Ƃ��Ĉ���
	// ��������ɔ��f����ƃJ�i���[�}�ϊ����Ƀw�{�����D��ɂȂ�
	{"mbya","���r��"},{"mbyi","���r�B"},{"mbyu","���r��"},{"mbye","���r�F"},{"mbyo","���r��"},
	{"mpya","���s��"},{"mpyi","���s�B"},{"mpyu","���s��"},{"mpye","���s�F"},{"mpyo","���s��"},
	{"mbba","���b�o"},{"mbbi","���b�r"},{"mbbu","���b�u"},{"mbbe","���b�x"},{"mbbo","���b�{"},
	{"mppa","���b�p"},{"mppi","���b�s"},{"mppu","���b�v"},{"mppe","���b�y"},{"mppo","���b�|"},
	{"mbbya","���b�r��"},{"mbbyi","���b�r�B"},{"mbbyu","���b�r��"},{"mbbye","���b�r�F"},{"mbbyo","���b�r��"},
	{"mppya","���b�s��"},{"mppyi","���b�s�B"},{"mppyu","���b�s��"},{"mppye","���b�s�F"},{"mppyo","���b�s��"},
	{"mba","���o"},{"mbi","���r"},{"mbu","���u"},{"mbe","���x"},{"mbo","���{"},
	{"mpa","���p"},{"mpi","���s"},{"mpu","���v"},{"mpe","���y"},{"mpo","���|"},
	{"mma","���}"},{"mmi","���~"},{"mmu","����"},{"mme","����"},{"mmo","����"},
	{"mmya","���~��"},{"mmyi","���~�B"},{"mmyu","���~��"},{"mmye","���~�F"},{"mmyo","���~��"},

	// �����̉��߂�����L���̗D����
	{"n,","���A"},{"n-","���["},{"n.","���B"},{"n/","���E"},
	{"n[","���u"},{"n]","���v"},
	{"n~","���`"},

	{"n,","���A"},{"n-","���["},{"n.","���B"},{"n/","���E"},
	{"n[","���u"},{"n]","���v"},
	{"n~","���`"},

	// ���[�}���ł͂Ȃ������[�}�����͂Ŏg�p�\�ȕ���
	{"n0","���O"},{"n1","���P"},{"n2","���Q"},{"n3","���R"},{"n4","���S"},{"n5","���T"},{"n6","���U"},{"n7","���V"},{"n8","���W"},{"n9","���X"},
	{"nb","����"},{"nc","����"},{"nd","����"},{"nf","����"},{"ng","����"},{"nh","����"},{"nj","����"},{"nk","����"},{"nl","����"},{"nm","����"},{"np","����"},{"nq","����"},{"nr","����"},{"ns","����"},{"nt","����"},{"nv","����"},{"nw","����"},{"nx","����"},{"ny","����"},{"nz","����"},
	{"nB","���a"},{"nC","���b"},{"nD","���c"},{"nF","���e"},{"nG","���f"},{"nH","���g"},{"nJ","���i"},{"nK","���j"},{"nL","���k"},{"nM","���l"},{"nP","���o"},{"nQ","���p"},{"nR","���q"},{"nS","���r"},{"nT","���s"},{"nV","���u"},{"nW","���v"},{"nX","���w"},{"nY","���x"},{"nZ","���y"},
	{"n ","���@"},{"n!","���I"},{"n\"","���h"},{"n#","����"},{"n$","����"},{"n%","����"},{"n&","����"},{"n'","���f"},{"n(","���i"},{"n)","���j"},{"n*","����"},{"n+","���{"},{"n,","���C"},{"n-","���|"},{"n.","���D"},{"n/","���^"},
	{"n:","���F"},{"n;","���G"},{"n<","����"},{"n=","����"},{"n>","����"},{"n?","���H"},{"n@","����"},
	{"n[","���m"},{"n\\","����"},{"n]","���n"},{"n^","���O"},{"n_","���Q"},{"n`","���M"},
	{"n{","���o"},{"n|","���b"},{"n}","���p"},{"n~","���P"},

	// ���s�΍�
	{"n\n","��"},
	{"n\r","��"},

	// �����ȁw���x
	//{"n","��"},

	// ����������
	{"xa","�@"},{"xi","�B"},{"xu","�D"},{"xe","�F"},{"xo","�H"},
	{"la","�@"},{"li","�B"},{"lu","�D"},{"le","�F"},{"lo","�H"},
	{"xka","��"},{"xke","��"},
	{"lka","��"},{"lke","��"},
	{"xtu","�b"},{"ltu","�b"},
	{"xtsu","�b"},{"ltsu","�b"},
	{"xya","��"},{"xyu","��"},{"xyo","��"},
	{"lya","��"},{"lyu","��"},{"lyo","��"},
	{"xye","�F"},
	{"lye","�F"},
	{"xwa","��"},
	{"lwa","��"},

	// �����̉��߂�����L���̗D����
	{",","�A"},{"-","�["},{".","�B"},{"/","�E"},
	{"[","�u"},{"]","�v"},
	{"~","�`"},

	// ���[�}���ł͂Ȃ������[�}�����͂Ŏg�p�\�ȕ���
	{"0","�O"},{"1","�P"},{"2","�Q"},{"3","�R"},{"4","�S"},{"5","�T"},{"6","�U"},{"7","�V"},{"8","�W"},{"9","�X"},
	{"a","��"},{"b","��"},{"c","��"},{"d","��"},{"e","��"},{"f","��"},{"g","��"},{"h","��"},{"i","��"},{"j","��"},{"k","��"},{"l","��"},{"m","��"},{"n","��"},{"o","��"},{"p","��"},{"q","��"},{"r","��"},{"s","��"},{"t","��"},{"u","��"},{"v","��"},{"w","��"},{"x","��"},{"y","��"},{"z","��"},
	{"A","�`"},{"B","�a"},{"C","�b"},{"D","�c"},{"E","�d"},{"F","�e"},{"G","�f"},{"H","�g"},{"I","�h"},{"J","�i"},{"K","�j"},{"L","�k"},{"M","�l"},{"N","�m"},{"O","�n"},{"P","�o"},{"Q","�p"},{"R","�q"},{"S","�r"},{"T","�s"},{"U","�t"},{"V","�u"},{"W","�v"},{"X","�w"},{"Y","�x"},{"Z","�y"},
	{" ","�@"},{"!","�I"},{"\"","�h"},{"#","��"},{"$","��"},{"%","��"},{"&","��"},{"'","�f"},{"(","�i"},{")","�j"},{"*","��"},{"+","�{"},{",","�C"},{"-","�|"},{".","�D"},{"/","�^"},
	{":","�F"},{";","�G"},{"<","��"},{"=","��"},{">","��"},{"?","�H"},{"@","��"},
	{"[","�m"},{"\\","��"},{"]","�n"},{"^","�O"},{"_","�Q"},{"`","�M"},
	{"{","�o"},{"|","�b"},{"}","�p"},{"~","�P"},
};
DWORD roma2kana(LPCSTR lpText, LPSTR lpBuf, LPCSTR lpHint, DWORD dwFlags)
{
	DWORD nRes = -1;
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && !_ismbblead(*lpText)) {
		lpBuf[0] = '\0';
		if(lpHint && *lpHint) {
			if(nRes==-1 && (dwFlags & ROMA2KANA_KANA2HINT_MATCH)) {
				for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
					if(strncmp(lpHint,__roma2kana_tbl__[n].kana,strlen(__roma2kana_tbl__[n].kana))==0) {
						if(strncmp(lpText,__roma2kana_tbl__[n].roma,strlen(__roma2kana_tbl__[n].roma))==0) {
							// �q���g�Ɗ��S�Ɉ�v����ꍇ
							strcpy(lpBuf,__roma2kana_tbl__[n].kana);
							nRes = strlen(__roma2kana_tbl__[n].roma);
							break;
						}
					}
				}
			}
			if(nRes==-1 && (dwFlags & ROMA2KANA_KANA2HINT_PARTIALMATCH)) {
				for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
					if(strncmp(lpHint,__roma2kana_tbl__[n].kana,strlen(__roma2kana_tbl__[n].kana))==0) {
						if(strncmp(lpText,__roma2kana_tbl__[n].roma,strlen(lpText))==0) {
							// �q���g�ƕ����I�Ɉ�v����ꍇ
							strcpy(lpBuf,__roma2kana_tbl__[n].kana);
							nRes = 0;
							break;
						}
					}
				}
			}
			if(nRes==-1 && (dwFlags & ROMA2KANA_HINT2KANA_MATCH)) {
				for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
					if(strncmp(lpHint,__roma2kana_tbl__[n].kana,strlen(lpHint))==0) {
						if(strncmp(lpText,__roma2kana_tbl__[n].roma,strlen(__roma2kana_tbl__[n].roma))==0) {
							// �q���g�Ɗ��S�Ɉ�v����ꍇ
							strcpy(lpBuf,__roma2kana_tbl__[n].kana);
							nRes = strlen(__roma2kana_tbl__[n].roma);
							break;
						}
					}
				}
			}
			if(nRes==-1 && (dwFlags & ROMA2KANA_HINT2KANA_PARTIALMATCH)) {
				for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
					if(strncmp(lpHint,__roma2kana_tbl__[n].kana,strlen(lpHint))==0) {
						if(strncmp(lpText,__roma2kana_tbl__[n].roma,strlen(lpText))==0) {
							// �q���g�ƕ����I�Ɉ�v����ꍇ
							strcpy(lpBuf,__roma2kana_tbl__[n].kana);
							nRes = 0;
							break;
						}
					}
				}
			}
		} else {
			for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
				if(strncmp(lpText,__roma2kana_tbl__[n].roma,strlen(__roma2kana_tbl__[n].roma))==0) {
					// ���S�Ɉ�v����ꍇ
					strcpy(lpBuf,__roma2kana_tbl__[n].kana);
					nRes = strlen(__roma2kana_tbl__[n].roma);
					break;
				}
			}
		}
	}
	return nRes;
}
DWORD kana2roma(LPCSTR lpText, LPSTR lpBuf, LPCSTR lpHint, DWORD dwFlags)
{
	DWORD nRes = -1;
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && _ismbblead(*lpText)) {
		if(lpHint && *lpHint) {
			if(nRes==-1 && (dwFlags & KANA2ROMA_ROMA2HINT_MATCH)) {
				for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
					if(strncmp(lpHint,__roma2kana_tbl__[n].roma,strlen(__roma2kana_tbl__[n].roma))==0) {
						if(strncmp(lpText,__roma2kana_tbl__[n].kana,strlen(__roma2kana_tbl__[n].kana))==0) {
							// �q���g�Ɗ��S�Ɉ�v����ꍇ
							strcpy(lpBuf,__roma2kana_tbl__[n].roma);
							nRes = strlen(__roma2kana_tbl__[n].kana);
							break;
						}
					}
				}
			}
			if(nRes==-1 && (dwFlags & KANA2ROMA_ROMA2HINT_PARTIALMATCH)) {
				for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
					if(strncmp(lpHint,__roma2kana_tbl__[n].roma,strlen(__roma2kana_tbl__[n].roma))==0) {
						if(strncmp(lpText,__roma2kana_tbl__[n].kana,strlen(lpText))==0) {
							// �q���g�ƕ����I�Ɉ�v����ꍇ
							strcpy(lpBuf,__roma2kana_tbl__[n].roma);
							nRes = 0;
							break;
						}
					}
				}
			}
			if(nRes==-1 && (dwFlags & KANA2ROMA_HINT2ROMA_MATCH)) {
				for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
					if(strncmp(lpHint,__roma2kana_tbl__[n].roma,strlen(lpHint))==0) {
						if(strncmp(lpText,__roma2kana_tbl__[n].kana,strlen(__roma2kana_tbl__[n].kana))==0) {
							// �q���g�Ɗ��S�Ɉ�v����ꍇ
							strcpy(lpBuf,__roma2kana_tbl__[n].roma);
							nRes = strlen(__roma2kana_tbl__[n].kana);
							break;
						}
					}
				}
			}
			if(nRes==-1 && (dwFlags & KANA2ROMA_HINT2ROMA_PARTIALMATCH)) {
				for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
					if(strncmp(lpHint,__roma2kana_tbl__[n].roma,strlen(lpHint))==0) {
						if(strncmp(lpText,__roma2kana_tbl__[n].kana,strlen(lpText))==0) {
							// �q���g�ƕ����I�Ɉ�v����ꍇ
							strcpy(lpBuf,__roma2kana_tbl__[n].roma);
							nRes = 0;
							break;
						}
					}
				}
			}
		} else {
			for(int n=0; n<ARRAYLEN(__roma2kana_tbl__); n++) {
				if(strncmp(lpText,__roma2kana_tbl__[n].kana,strlen(__roma2kana_tbl__[n].kana))==0) {
					// ���S�Ɉ�v����ꍇ
					strcpy(lpBuf,__roma2kana_tbl__[n].roma);
					nRes = strlen(__roma2kana_tbl__[n].kana);
					break;
				}
			}
		}
	}
	return nRes;
}
DWORD roma2kana(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf, LPCSTR lpHint, DWORD dwFlags)
{
	DWORD nRes = -1;
	char str[7] = {0};
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && 1<cchBuf) {
		DWORD cHint = lpHint ? strlen(lpHint) : 0;
		DWORD m = 0;
		for(DWORD n=0; lpText[m]!='\0'&&m<cchBuf-1&&(!lpHint||n<cHint);) {
			DWORD l = roma2kana(&lpText[m],str,(lpHint&&*lpHint)?&lpHint[n]:NULL,dwFlags);
			if(dwFlags & ROMA2KANA_REMOVE_ENDOFNN) {
				// �I�[�̏d��"n"�p�p�b�`
				if(strcmp(&lpText[m],"n")==0) {
					if(roma2kana("nn",str,(lpHint&&*lpHint)?&lpHint[n]:NULL,dwFlags)==2) {
						l = 1;
					}
				}
			}
			if(l!=-1) {
				// ����ɕϊ�(���邢�̓q���g�ƕ�����v)�����ꍇ
				DWORD len = strlen(str);
				if(n+len<cchBuf) {
					memcpy(&lpBuf[n],str,len);
					lpBuf[n+len] = '\0';
					n += len;
					m += l;
				} else {
					break;	// ����ȏ�ϊ����ʂ��i�[�ł��Ȃ��ꍇ
				}
				if(l==0) {
					break;	// ������v�̏ꍇ
				}
			} else {
				// �q���g�ƈ�v���Ȃ��ꍇ
				if(lpHint && *lpHint) {
					DWORD n = roma2kana(&lpText[m],str,NULL,dwFlags);
					if(n!=-1 || n<0) {
						m = -1;
						break;
					}
				}
				// �ϊ��s�\�ȏꍇ
				if(_ismbblead(lpText[m])) {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = lpText[m+1];
					lpBuf[n+2] = '\0';
					n += 2;
					m += 2;
				} else {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = '\0';
					n += 1;
					m += 1;
				}
			}
		}
		nRes = m;
	}
	return nRes;
}
DWORD kana2roma(LPCSTR lpText, LPSTR lpBuf, DWORD cchBuf, LPCSTR lpHint, DWORD dwFlags)
{
	DWORD nRes = -1;
	char str[5] = {0};
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && 1<cchBuf) {
		DWORD cHint = lpHint ? strlen(lpHint) : 0;
		DWORD n=0,m=0;
		for(; lpText[m]!='\0'&&m<cchBuf-1&&(!lpHint||n<cHint);) {
			DWORD l = kana2roma(&lpText[m],str,(lpHint&&*lpHint)?&lpHint[n]:NULL,dwFlags);
			if(l!=-1) {
				// ����ɕϊ�(���邢�̓q���g�ƕ�����v)�����ꍇ
				DWORD len = strlen(str);
				if(n+len<cchBuf) {
					memcpy(&lpBuf[n],str,len);
					lpBuf[n+len] = '\0';
					n += len;
					m += l;
				} else {
					break;	// ����ȏ�ϊ����ʂ��i�[�ł��Ȃ��ꍇ
				}
				if(l==0) {
					break;	// ������v�̏ꍇ
				}
			} else {
				// �ϊ��\�����q���g�ƈ�v���Ȃ��ꍇ
				if(lpHint && *lpHint) {
					DWORD n = kana2roma(&lpText[m],str,NULL,dwFlags);
					if(n!=-1 || n<0) {
						m = -1;
						break;
					}
				}
				// �ϊ��ł��Ȃ��ꍇ
				if(_ismbblead(lpText[m])) {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = lpText[m+1];
					lpBuf[n+2] = '\0';
					n += 2;
					m += 2;
				} else {
					lpBuf[n+0] = lpText[m+0];
					lpBuf[n+1] = '\0';
					n += 1;
					m += 1;
				}
			}
		}
		nRes = m;
		if(dwFlags & KANA2ROMA_REMOVE_ENDOFNN) {
			// �I�[�̏d��"n"�p�p�b�`
			if(strcmp(&lpBuf[n-2],"nn")==0) {
				if(!lpHint || strcmp(&lpHint[n-2],"nn")!=0) {
					lpBuf[n-1] = '\0';
				}
			}
		}
	}
	return nRes;
}

/*****************************************************************
	����������擾(csv�̂悤�ȕ�����؂�e�L�X�g�ŁA����C���f�b�N�X�̕���������𓾂�)
*****************************************************************/
BOOL GetCSVIndexedString(LPCTSTR lpText, TCHAR cDelimiter, DWORD nIndex, LPTSTR lpBuf, DWORD cchBuf, LPDWORD lpdwCopied)
{
	if(lpText && *lpText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD n=0,s=0,e=0;
		for(;; s=e+1,n++) {
			LPTSTR p = tmp.substring(s).find(cDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				e = tmp.length();
			}
			if(!p||nIndex<=n) break;
		}
		if(n==nIndex) {
			DWORD len = e-s;
			if(lpBuf && cchBuf) {
				len = min(len,cchBuf-1);
				tmp.substring(s).copy_to(lpBuf,len);
			}
			if(lpdwCopied) {
				*lpdwCopied = len;
			}
			return TRUE;
		}
	}
	return FALSE;
}
BOOL GetCSVIndexedString(LPCTSTR lpText, LPCTSTR lpDelimiter, DWORD nIndex, LPTSTR lpBuf, DWORD cchBuf, LPDWORD lpdwCopied)
{
	if(lpText && *lpText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD cDelimiter = ((TString<0>*)lpDelimiter)->length();
		DWORD n=0,s=0,e=0;
		for(;; s=e+cDelimiter,n++) {
			LPTSTR p = tmp.substring(s).find(lpDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				e = tmp.length();
			}
			if(!p||nIndex<=n) break;
		}
		if(n==nIndex) {
			DWORD len = e-s;
			if(lpBuf && cchBuf) {
				len = min(len,cchBuf-1);
				tmp.substring(s).copy_to(lpBuf,len);
			}
			if(lpdwCopied) {
				*lpdwCopied = len;
			}
			return TRUE;
		}
	}
	return FALSE;
}
BOOL SetCSVIndexedString(LPTSTR lpText, DWORD cchText, TCHAR cDelimiter, DWORD nIndex, LPCTSTR lpString)
{
	if(lpText && cchText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD n=0,s=0,e=0;
		for(;; s=e+1,n++) {
			LPTSTR p = tmp.substring(s).find(cDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				e = tmp.length();
			}
			if(!p||nIndex<=n) break;
		}
		if(n<nIndex) {
			DWORD cchString = ((TString<0>*)lpString)->length() + 1;
			if(e+nIndex-n+cchString <= cchText) {
				for(; n<nIndex; n++) {
					tmp[e++] = cDelimiter;
				}
				tmp.substring(e).let(lpString);
				return TRUE;
			}
		} else {
			DWORD cString = ((TString<0>*)lpString)->length();
			DWORD len = e-s;
			if(tmp.length()-len+cString <= cchText) {
				tmp.replaceAt(s,len,lpString);
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL SetCSVIndexedString(LPTSTR lpText, DWORD cchText, LPCTSTR lpDelimiter, DWORD nIndex, LPCTSTR lpString)
{
	if(lpText && cchText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD cDelimiter = ((TString<0>*)lpDelimiter)->length();
		DWORD n=0,s=0,e=0;
		for(;; s=e+cDelimiter,n++) {
			LPTSTR p = tmp.substring(s).find(lpDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				e = tmp.length();
			}
			if(!p||nIndex<=n) break;
		}
		if(n<nIndex) {
			DWORD cchString = ((TString<0>*)lpString)->length() + 1;
			if(e+(nIndex-n)*cDelimiter+cchString <= cchText) {
				for(; n<nIndex; n++) {
					tmp.concat(lpDelimiter);
				}
				tmp.concat(lpString);
				return TRUE;
			}
		} else {
			DWORD cString = ((TString<0>*)lpString)->length();
			DWORD len = e-s;
			if(tmp.length()-len+cString <= cchText) {
				tmp.replaceAt(s,len,lpString);
				return TRUE;
			}
		}
	}
	return FALSE;
}
DWORD CountCSVIndexedString(LPCTSTR lpText, TCHAR cDelimiter)
{
	if(lpText && *lpText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD n=0,s=0,e=0;
		for(;; s=e+1,n++) {
			LPTSTR p = tmp.substring(s).find(cDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				n++;	// �Ō�̗v�f����ł��J�E���g����
				break;
			}
		}
		return n;
	}
	return 0;
}
DWORD CountCSVIndexedString(LPCTSTR lpText, LPCTSTR lpDelimiter)
{
	if(lpText && *lpText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD cDelimiter = ((TString<0>*)lpDelimiter)->length();
		DWORD n=0,s=0,e=0;
		for(;; s=e+cDelimiter,n++) {
			LPTSTR p = tmp.substring(s).find(lpDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				n++;	// �Ō�̗v�f����ł��J�E���g����
				break;
			}
		}
		return n;
	}
	return 0;
}
DWORD FindCSVIndexedString(LPTSTR lpText, TCHAR cDelimiter, LPCTSTR lpString)
{
	if(lpText && *lpText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD n=0,s=0,e=0;
		for(;; s=e+1,n++) {
			LPTSTR p = tmp.substring(s).find(cDelimiter);
			if(p) {
				e = p-tmp;
				if(tmp.substring(s).isEqualSubstring(lpString)) {
					return n;
				}
			} else if(tmp.substring(s).isEqual(lpString)) {	// �Ō�̗v�f�͓��ꈵ��
				return n;
			} else {
				return -1;
			}
		}
	}
	return -1;
}
DWORD FindCSVIndexedString(LPTSTR lpText, LPCTSTR lpDelimiter, LPCTSTR lpString)
{
	if(lpText && *lpText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD cDelimiter = ((TString<0>*)lpDelimiter)->length();
		DWORD n=0,s=0,e=0;
		for(;; s=e+cDelimiter,n++) {
			LPTSTR p = tmp.substring(s).find(lpDelimiter);
			if(p) {
				e = p-tmp;
				if(tmp.substring(s).isEqualSubstring(lpString)) {
					return n;
				}
			} else if(tmp.substring(s).isEqual(lpString)) {	// �Ō�̗v�f�͓��ꈵ��
				return n;
			} else {
				return -1;
			}
		}
	}
	return -1;
}
BOOL RemoveCSVIndexedString(LPTSTR lpText, TCHAR cDelimiter, DWORD nIndex)
{
	if(lpText && *lpText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD cString = ((TString<0>*)lpText)->length();
		DWORD n=0,s=0,e=0;
		for(;; s=e+1,n++) {
			LPTSTR p = tmp.substring(s).find(cDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				e = cString;
			}
			if(!p||nIndex<=n) break;
		}
		if(n==nIndex) {
			if(s==0) {
				tmp.removeAt(s,e-s+1);
			} else if(e==cString) {
				tmp.removeAt(s-1,e-s+1);
			} else {
				tmp.removeAt(s,e-s+1);
			}
			return TRUE;
		}
	}
	return FALSE;
}
BOOL RemoveCSVIndexedString(LPTSTR lpText, LPCTSTR lpDelimiter, DWORD nIndex)
{
	if(lpText && *lpText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD cDelimiter = ((TString<0>*)lpDelimiter)->length();
		DWORD cString = ((TString<0>*)lpText)->length();
		DWORD n=0,s=0,e=0;
		for(;; s=e+cDelimiter,n++) {
			LPTSTR p = tmp.substring(s).find(lpDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				e = cString;
			}
			if(!p||nIndex<=n) break;
		}
		if(n==nIndex) {
			if(s==0) {
				tmp.removeAt(s,e-s+cDelimiter);
			} else if(e==cString) {
				tmp.removeAt(s-1,e-s+1);
			} else {
				tmp.removeAt(s,e-s+cDelimiter);
			}
			return TRUE;
		}
	}
	return FALSE;
}
BOOL InsertCSVIndexedString(LPTSTR lpText, DWORD cchText, TCHAR cDelimiter, DWORD nIndex, LPCTSTR lpString)
{
	if(lpText && cchText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD n=0,s=0,e=0;
		for(;; s=e+1,n++) {
			LPTSTR p = tmp.substring(s).find(cDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				e = tmp.length();
			}
			if(!p||nIndex<=n) break;
		}
		if(n<nIndex) {
			DWORD cchString = ((TString<0>*)lpString)->length() + 1;
			if(e+nIndex-n+cchString <= cchText) {
				for(; n<nIndex; n++) {
					tmp[e++] = cDelimiter;
				}
				tmp.substring(e).let(lpString);
				return TRUE;
			}
		} else {
			DWORD len = ((TString<0>*)lpString)->length();
			if(tmp.length()+len+1 <= cchText) {
				tmp.insertAt(s,len+1);
				tmp.replaceAt(s,len,lpString);
				tmp[s+len] = cDelimiter;
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL InsertCSVIndexedString(LPTSTR lpText, DWORD cchText, LPCTSTR lpDelimiter, DWORD nIndex, LPCTSTR lpString)
{
	if(lpText && cchText) {
		TString<0> &tmp = *((TString<0>*)lpText);
		DWORD cDelimiter = ((TString<0>*)lpDelimiter)->length();
		DWORD n=0,s=0,e=0;
		for(;; s=e+1,n++) {
			LPTSTR p = tmp.substring(s).find(lpDelimiter);
			if(p) {
				e = p-tmp;
			} else {
				e = tmp.length();
			}
			if(!p||nIndex<=n) break;
		}
		if(n<nIndex) {
			DWORD cchString = ((TString<0>*)lpString)->length() + 1;
			if(e+nIndex-n+cchString <= cchText) {
				for(; n<nIndex; n++) {
					tmp.substring(e+=cDelimiter).let(lpDelimiter);
				}
				tmp.substring(e).let(lpString);
				return TRUE;
			}
		} else {
			DWORD len = ((TString<0>*)lpString)->length();
			if(tmp.length()+len+cDelimiter <= cchText) {
				tmp.insertAt(s,len+cDelimiter);
				tmp.replaceAt(s,len,lpString);
				tmp.replaceAt(s+len,len,lpDelimiter);
				return TRUE;
			}
		}
	}
	return FALSE;
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
