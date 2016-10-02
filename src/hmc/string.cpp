/*
	文字・文字列操作ユーティリティ
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
	mbs<->mbc変換
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
	文字列に全角文字が含まれているか検査する(文字数を返す)
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
	半角<->全角文字列変換
*****************************************************************/
static const struct {
	char han;
	char *zen;
} __zen2han_dakuten_tbl__[] = {
	{'ｳ',"ヴ"},
	{'ｶ',"ガ"},{'ｷ',"ギ"},{'ｸ',"グ"},{'ｹ',"ゲ"},{'ｺ',"ゴ"},
	{'ｻ',"ザ"},{'ｼ',"ジ"},{'ｽ',"ズ"},{'ｾ',"ゼ"},{'ｿ',"ゾ"},
	{'ﾀ',"ダ"},{'ﾁ',"ヂ"},{'ﾂ',"ヅ"},{'ﾃ',"デ"},{'ﾄ',"ド"},
	{'ﾊ',"バ"},{'ﾋ',"ビ"},{'ﾌ',"ブ"},{'ﾍ',"ベ"},{'ﾎ',"ボ"},
};
static const struct {
	char han;
	char *zen;
} __zen2han_handakuten_tbl__[] = {
	{'ﾊ',"パ"},{'ﾋ',"ピ"},{'ﾌ',"プ"},{'ﾍ',"ペ"},{'ﾎ',"ポ"},
};
static const struct {
	char han;
	char *zen;
} __zen2han_tbl__[] = {
	{'0',"０"},{'1',"１"},{'2',"２"},{'3',"３"},{'4',"４"},{'5',"５"},{'6',"６"},{'7',"７"},{'8',"８"},{'9',"９"},
	{'a',"ａ"},{'b',"ｂ"},{'c',"ｃ"},{'d',"ｄ"},{'e',"ｅ"},{'f',"ｆ"},{'g',"ｇ"},{'h',"ｈ"},{'i',"ｉ"},{'j',"ｊ"},{'k',"ｋ"},{'l',"ｌ"},{'m',"ｍ"},{'n',"ｎ"},{'o',"ｏ"},{'p',"ｐ"},{'q',"ｑ"},{'r',"ｒ"},{'s',"ｓ"},{'t',"ｔ"},{'u',"ｕ"},{'v',"ｖ"},{'w',"ｗ"},{'x',"ｘ"},{'y',"ｙ"},{'z',"ｚ"},
	{'A',"Ａ"},{'B',"Ｂ"},{'C',"Ｃ"},{'D',"Ｄ"},{'E',"Ｅ"},{'F',"Ｆ"},{'G',"Ｇ"},{'H',"Ｈ"},{'I',"Ｉ"},{'J',"Ｊ"},{'K',"Ｋ"},{'L',"Ｌ"},{'M',"Ｍ"},{'N',"Ｎ"},{'O',"Ｏ"},{'P',"Ｐ"},{'Q',"Ｑ"},{'R',"Ｒ"},{'S',"Ｓ"},{'T',"Ｔ"},{'U',"Ｕ"},{'V',"Ｖ"},{'W',"Ｗ"},{'X',"Ｘ"},{'Y',"Ｙ"},{'Z',"Ｚ"},
	{' ',"　"},{'!',"！"},{'\"',"”"},{'#',"＃"},{'$',"＄"},{'%',"％"},{'&',"＆"},{'\'',"’"},{'(',"（"},{')',"）"},{'*',"＊"},{'+',"＋"},{',',"，"},{'-',"－"},{'.',"．"},{'/',"／"},
	{':',"："},{';',"；"},{'<',"＜"},{'=',"＝"},{'>',"＞"},{'?',"？"},{'@',"＠"},{'[',"［"},{'\\',"￥"},{']',"］"},{'^',"＾"},{'_',"＿"},{'`',"｀"},
	{'{',"｛"},{'|',"｜"},{'}',"｝"},{'~',"～"},
	{'､',"、"},{'｡',"。"},{'･',"・"},{'ﾞ',"゛"},{'ﾟ',"゜"},
	{'ｱ',"ア"},{'ｲ',"イ"},{'ｳ',"ウ"},{'ｴ',"エ"},{'ｵ',"オ"},
	{'ｶ',"カ"},{'ｷ',"キ"},{'ｸ',"ク"},{'ｹ',"ケ"},{'ｺ',"コ"},
	{'ｻ',"サ"},{'ｼ',"シ"},{'ｽ',"ス"},{'ｾ',"セ"},{'ｿ',"ソ"},
	{'ﾀ',"タ"},{'ﾁ',"チ"},{'ﾂ',"ツ"},{'ﾃ',"テ"},{'ﾄ',"ト"},
	{'ﾅ',"ナ"},{'ﾆ',"ニ"},{'ﾇ',"ヌ"},{'ﾈ',"ネ"},{'ﾉ',"ノ"},
	{'ﾊ',"ハ"},{'ﾋ',"ヒ"},{'ﾌ',"フ"},{'ﾍ',"ヘ"},{'ﾎ',"ホ"},
	{'ﾏ',"マ"},{'ﾐ',"ミ"},{'ﾑ',"ム"},{'ﾒ',"メ"},{'ﾓ',"モ"},
	{'ﾔ',"ヤ"},{'ﾕ',"ユ"},{'ﾖ',"ヨ"},
	{'ﾗ',"ラ"},{'ﾘ',"リ"},{'ﾙ',"ル"},{'ﾚ',"レ"},{'ﾛ',"ロ"},
	{'ﾜ',"ワ"},{'ｦ',"ヲ"},{'ﾝ',"ン"},
	{'ｧ',"ァ"},{'ｨ',"ィ"},{'ｩ',"ゥ"},{'ｪ',"ェ"},{'ｫ',"ォ"},
	{'ｬ',"ャ"},{'ｭ',"ュ"},{'ｮ',"ョ"},
	{'ｯ',"ッ"},
	{'ｰ',"ー"},
	{'｢',"「"},
	{'｣',"」"},
};
DWORD han2zen(LPCSTR lpText, LPSTR lpBuf)
{
	DWORD nRes = 0;
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && !_ismbblead(*lpText)) {
		// 数字チェック
		if('0'<=*lpText && *lpText<='9') {
			mbc2mbs(mbs2mbc("０") + (*lpText) - '0', lpBuf);
			nRes = 1;
		}
		// アルファベット大文字チェック
		else if('A'<=*lpText && *lpText<='Z') {
			mbc2mbs(mbs2mbc("Ａ") + (*lpText) - 'A', lpBuf);
			nRes = 1;
		}
		// アルファベット小文字チェック
		else if('a'<=*lpText && *lpText<='z') {
			mbc2mbs(mbs2mbc("ａ") + (*lpText) - 'a', lpBuf);
			nRes = 1;
		}
		// カタカナ濁点チェック
		else if(*(lpText+1)=='ﾞ') {
			for(int n=0; n<ARRAYLEN(__zen2han_dakuten_tbl__); n++) {
				if(*lpText==__zen2han_dakuten_tbl__[n].han) {
					strcpy(lpBuf,__zen2han_dakuten_tbl__[n].zen);
					nRes = 2;
					break;
				}
			}
		}
		// カタカナ半濁点チェック
		else if(*(lpText+1)=='ﾟ') {
			for(int n=0; n<ARRAYLEN(__zen2han_handakuten_tbl__); n++) {
				if(*lpText==__zen2han_handakuten_tbl__[n].han) {
					strcpy(lpBuf,__zen2han_handakuten_tbl__[n].zen);
					nRes = 2;
					break;
				}
			}
		}
		// その他の文字
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
		// 数字チェック
		if(mbs2mbc("０")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("９")) {
			lpBuf[0] = '0' + mbs2mbc(lpText) - mbs2mbc("０");
			lpBuf[1] = '\0';
			nRes = 2;
		}
		// アルファベット大文字チェック
		else if(mbs2mbc("Ａ")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("Ｚ")) {
			lpBuf[0] = 'A' + mbs2mbc(lpText) - mbs2mbc("Ａ");
			lpBuf[1] = '\0';
			nRes = 2;
		}
		// アルファベット小文字チェック
		else if(mbs2mbc("ａ")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("ｚ")) {
			lpBuf[0] = 'a' + mbs2mbc(lpText) - mbs2mbc("ａ");
			lpBuf[1] = '\0';
			nRes = 2;
		}
		else {
			// カタカナ濁点チェック
			if(nRes==0) {
				for(int n=0; n<ARRAYLEN(__zen2han_dakuten_tbl__); n++) {
					if(mbs2mbc(lpText)==mbs2mbc(__zen2han_dakuten_tbl__[n].zen)) {
						lpBuf[0] = __zen2han_dakuten_tbl__[n].han;
						lpBuf[1] = 'ﾞ';
						lpBuf[2] = '\0';
						nRes = 2;
						break;
					}
				}
			}
			// カタカナ半濁点チェック
			if(nRes==0) {
				for(int n=0; n<ARRAYLEN(__zen2han_handakuten_tbl__); n++) {
					if(mbs2mbc(lpText)==mbs2mbc(__zen2han_handakuten_tbl__[n].zen)) {
						lpBuf[0] = __zen2han_handakuten_tbl__[n].han;
						lpBuf[1] = 'ﾟ';
						lpBuf[2] = '\0';
						nRes = 2;
						break;
					}
				}
			}
			// その他の文字
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
	カタカナ<->ひらがな文字列変換
*****************************************************************/
DWORD kata2hira(LPCSTR lpText, LPSTR lpBuf)
{
	DWORD nRes = 0;
	if(lpBuf) {
		lpBuf[0] = '\0';
	}
	if(lpText && lpBuf && _ismbcprint(mbs2mbc(lpText))) {
		// 共通コード順チェック
		if(mbs2mbc("ァ")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("ミ")) {
			mbc2mbs(mbs2mbc(lpText) - (mbs2mbc("ァ") - mbs2mbc("ぁ")), lpBuf);
			nRes = 2;
		} else if(mbs2mbc("ム")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("ン")) {
			mbc2mbs(mbs2mbc(lpText) - (mbs2mbc("ム") - mbs2mbc("む")), lpBuf);
			nRes = 2;
		}
		// カタカナのみの文字チェック
		else {
			static const struct {
				char *kata;
				char *hira;
			} a[] = {
				{"ヴ","う゛"},
				{"ヵ","か"},	// ※変換されないよりはマシだろう……
				{"ヶ","け"},	// ※変換されないよりはマシだろう……
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
		// 共通コード順チェック
		if(mbs2mbc("ぁ")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("み")) {
			mbc2mbs(mbs2mbc(lpText) + (mbs2mbc("ァ") - mbs2mbc("ぁ")), lpBuf);
			nRes = 2;
		} else if(mbs2mbc("む")<=mbs2mbc(lpText) && mbs2mbc(lpText)<=mbs2mbc("ん")) {
			mbc2mbs(mbs2mbc(lpText) + (mbs2mbc("ム") - mbs2mbc("む")), lpBuf);
			nRes = 2;
		}
		// カタカナのみの文字チェック
		else {
			static const struct {
				char *hira;
				char *kata;
			} a[] = {
				{"う゛","ヴ"},
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
	ローマ字カタカナ(全角)変換
*****************************************************************/
static const struct {
	char *roma;
	char *kana;
} __roma2kana_tbl__[] = {
	// ア行
	{"wwi","ッウィ"},{"wwe","ッウェ"},
	{"vva","ッヴァ"},{"vvi","ッヴィ"},               {"vve","ッヴェ"},{"vvo","ッヴォ"},
	{"vvu","ッヴ"},
	{"wi","ウィ"},{"we","ウェ"},
	{"wha","ウァ"},{"whi","ウィ"},        {"whe","ウェ"},{"who","ウォ"},
	{"va","ヴァ"},{"vi","ヴィ"},          {"ve","ヴェ"},{"vo","ヴォ"},
	{"vu","ヴ"},
	{ "a","ア"},{ "i","イ"},{ "u","ウ"},{ "e","エ"},{ "o","オ"},

	// カ行
	{"kkya","ッキャ"},{"kkyi","ッキィ"},{"kkyu","ッキュ"},{"kkye","ッキェ"},{"kkyo","ッキョ"},
	{"ggya","ッギャ"},{"ggyi","ッギィ"},{"ggyu","ッギュ"},{"ggye","ッギェ"},{"ggyo","ッギョ"},
	{ "qqa","ックァ"},{ "qqi","ックィ"},{ "qqu","ックィ"},{ "qqe","ックェ"},{ "qqo","ックォ"},
	{"kka","ッカ"},{"kki","ッキ"},{"kku","ック"},{"kke","ッケ"},{"kko","ッコ"},
	{"gga","ッガ"},{"ggi","ッギ"},{"ggu","ッグ"},{"gge","ッゲ"},{"ggo","ッゴ"},
	{"kya","キャ"},{"kyi","キィ"},{"kyu","キュ"},{"kye","キェ"},{"kyo","キョ"},
	{ "qa","クァ"},{ "qi","クィ"},               { "qe","クェ"},{ "qo","クォ"},
	{"gya","ギャ"},{"gyi","ギィ"},{"gyu","ギュ"},{"gye","ギェ"},{"gyo","ギョ"},
	{"ka","カ"},{"ki","キ"},{"ku","ク"},{"ke","ケ"},{"ko","コ"},
	{"ca","カ"},{"cu","ク"},{"co","コ"},
	{"qu","ク"  },
	{"ga","ガ"},{"gi","ギ"},{"gu","グ"},{"ge","ゲ"},{"go","ゴ"},

	// サ行
	{"ssya","ッシャ"},{"ssyi","ッシィ"},{"ssyu","ッシュ"},{"ssye","ッシェ"},{"ssyo","ッショ"},
	{"ssha","ッシャ"},                  {"sshu","ッシュ"},{"sshe","ッシェ"},{"ssho","ッショ"},
	{ "jja","ッジャ"},                  { "jju","ッジュ"},{ "jje","ッジェ"},{ "jjo","ッジョ"},
	{"jjya","ッジャ"},{"jjyi","ッジィ"},{"jjyu","ッジュ"},{"jjye","ッジェ"},{"jjyo","ッジョ"},
	{"zzya","ッジャ"},{"zzyi","ッジィ"},{"zzyu","ッジュ"},{"zzye","ッジェ"},{"zzyo","ッジョ"},
	{"ssa","ッサ"},{"ssi","ッシ"},{"ssu","ッス"},{"sse","ッセ"},{"sso","ッソ"},
	{"sshi","ッシ"  },
	{ "jji","ッジ"  },
	{"zza","ッザ"},{"zzi","ッジ"},{"zzu","ッズ"},{"zze","ッゼ"},{"zzo","ッゾ"},
	{"sya","シャ"},{"syi","シィ"},{"syu","シュ"},{"sye","シェ"},{"syo","ショ"},
	{"sha","シャ"},               {"shu","シュ"},{"she","シェ"},{"sho","ショ"},
	{ "ja","ジャ"},               { "ju","ジュ"},{ "je","ジェ"},{ "jo","ジョ"},
	{"jya","ジャ"},{"jyi","ジィ"},{"jyu","ジュ"},{"jye","ジェ"},{"jyo","ジョ"},
	{"zya","ジャ"},{"zyi","ジィ"},{"zyu","ジュ"},{"zye","ジェ"},{"zyo","ジョ"},
	{"sa","サ"},{"si","シ"},{"su","ス"},{"se","セ"},{"so","ソ"},
	{"shi","シ"  },
	{ "ji","ジ"  },
	{"za","ザ"},{"zi","ジ"},{"zu","ズ"},{"ze","ゼ"},{"zo","ゾ"},

	// タ行
	{"ttya","ッチャ"},{"ttyi","ッチィ"},{"ttyu","ッチュ"},{"ttye","ッチェ"},{"ttyo","ッチョ"},
	{"ccya","ッチャ"},{"ccyi","ッチィ"},{"ccyu","ッチュ"},{"ccye","ッチェ"},{"ccyo","ッチョ"},
	{"ccha","ッチャ"},                  {"cchu","ッチュ"},{"cche","ッチェ"},{"ccho","ッチョ"},
	{"ttha","ッテャ"},{"tthi","ッティ"},{"tthu","ッテュ"},{"tthe","ッテェ"},{"ttho","ッテョ"},
	{"ttsa","ッツァ"},{"ttsi","ッツィ"},                  {"ttse","ッツェ"},{"ttso","ッツォ"},
	{"ddya","ッヂャ"},{"ddyi","ッヂィ"},{"ddyu","ッヂュ"},{"ddye","ッヂェ"},{"ddyo","ッヂョ"},
	{"ddha","ッデャ"},{"ddhi","ッディ"},{"ddhu","ッデュ"},{"ddhe","ッデェ"},{"ddho","ッデョ"},
	{"tta","ッタ"},{"tti","ッチ"},{"ttu","ッツ"},{"tte","ッテ"},{"tto","ット"},
	{"ttsu","ッツ"  },
	{"cchi","ッチ"  },
	{"dda","ッダ"},{"ddi","ッヂ"},{"ddu","ッヅ"},{"dde","ッデ"},{"ddo","ッド"},
	{"tya","チャ"},{"tyi","チィ"},{"tyu","チュ"},{"tye","チェ"},{"tyo","チョ"},
	{"cha","チャ"},               {"chu","チュ"},{"che","チェ"},{"cho","チョ"},
	{"cya","チャ"},{"cyi","チィ"},{"cyu","チュ"},{"cye","チェ"},{"cyo","チョ"},
	{"tha","テャ"},{"thi","ティ"},{"thu","テュ"},{"the","テェ"},{"tho","テョ"},
	{"tsa","ツァ"},{"tsi","ツィ"},               {"tse","ツェ"},{"tso","ツォ"},
	{"dya","ヂャ"},{"dyi","ヂィ"},{"dyu","ヂュ"},{"dye","ヂェ"},{"dyo","ヂョ"},
	{"dha","デャ"},{"dhi","ディ"},{"dhu","デュ"},{"dhe","デェ"},{"dho","デョ"},
	{ "ta","タ"},{ "ti","チ"},{ "tu","ツ"},{ "te","テ"},{ "to","ト"},
	{"chi","チ"  },
	{"tsu","ツ"  },
	{ "da","ダ"},{ "di","ヂ"},{ "du","ヅ"},{ "de","デ"},{ "do","ド"},

	// ナ行
	{"nya","ニャ"},{"nyi","ニィ"},{"nyu","ニュ"},{"nye","ニェ"},{"nyo","ニョ"},
	{"na","ナ"},{"ni","ニ"},{"nu","ヌ"},{"ne","ネ"},{"no","ノ"},

	// ハ行
	{"hhya","ッヒャ"},{"hhyi","ッヒィ"},{"hhyu","ッヒュ"},{"hhye","ッヒェ"},{"hhyo","ッヒョ"},
	{"bbya","ッビャ"},{"bbyi","ッビィ"},{"bbyu","ッビュ"},{"bbye","ッビェ"},{"bbyo","ッビョ"},
	{"ppya","ッピャ"},{"ppyi","ッピィ"},{"ppyu","ッピュ"},{"ppye","ッピェ"},{"ppyo","ッピョ"},
	{ "ffa","ッファ"},{ "ffi","ッフィ"},                  { "ffe","ッフェ"},{ "ffo","ッフォ"},
	{"ffya","ッフャ"},{"ffyi","ッフィ"},{"ffyu","ッフュ"},{"ffye","ッフェ"},{"ffyo","ッフョ"},
	{ "ffu","ッフ"  },
	{"hha","ッハ"},{"hhi","ッヒ"},{"hhu","ッフ"},{"hhe","ッヘ"},{"hho","ッホ"},
	{"bba","ッバ"},{"bbi","ッビ"},{"bbu","ッブ"},{"bbe","ッベ"},{"bbo","ッボ"},
	{"ppa","ッパ"},{"ppi","ッピ"},{"ppu","ップ"},{"ppe","ッペ"},{"ppo","ッポ"},
	{"hya","ヒャ"},{"hyi","ヒィ"},{"hyu","ヒュ"},{"hye","ヒェ"},{"hyo","ヒョ"},
	{"bya","ビャ"},{"byi","ビィ"},{"byu","ビュ"},{"bye","ビェ"},{"byo","ビョ"},
	{"pya","ピャ"},{"pyi","ピィ"},{"pyu","ピュ"},{"pye","ピェ"},{"pyo","ピョ"},
	{ "fa","ファ"},{ "fi","フィ"},               { "fe","フェ"},{ "fo","フォ"},
	{"fya","フャ"},{"fyi","フィ"},{"fyu","フュ"},{"fye","フェ"},{"fyo","フョ"},
	{ "fu","フ"  },
	{"ha","ハ"},{"hi","ヒ"},{"hu","フ"},{"he","ヘ"},{"ho","ホ"},
	{"ba","バ"},{"bi","ビ"},{"bu","ブ"},{"be","ベ"},{"bo","ボ"},
	{"pa","パ"},{"pi","ピ"},{"pu","プ"},{"pe","ペ"},{"po","ポ"},

	// マ行
	{"mya","ミャ"},{"myi","ミィ"},{"myu","ミュ"},{"mye","ミェ"},{"myo","ミョ"},
	{"ma","マ"},{"mi","ミ"},{"mu","ム"},{"me","メ"},{"mo","モ"},

	// ヤ行
	{"yya","ッヤ"},{"yyu","ッユ"},{"yyo","ッヨ"},
	{"ya","ヤ"},{"yu","ユ"},{"yo","ヨ"},
	{"yi","イ"},{"ye","イェ"},

	// ラ行
	{"rrya","ッリャ"},{"rryi","ッリィ"},{"rryu","ッリュ"},{"rrye","ッリェ"},{"rryo","ッリョ"},
	{"rra","ッラ"},{"rri","ッリ"},{"rru","ッル"},{"rre","ッレ"},{"rro","ッロ"},
	{"rya","リャ"},{"ryi","リィ"},{"ryu","リュ"},{"rye","リェ"},{"ryo","リョ"},
	{"ra","ラ"},{"ri","リ"},{"ru","ル"},{"re","レ"},{"ro","ロ"},

	// ワ行
	{"wwa","ッワ"},{"wwo","ッヲ"},
	{"wa","ワ"},{"wo","ヲ"},
	{"nn","ン"},

	// ア行
	{"nwwi","ンッウィ"},{"nwwe","ンッウェ"},
	{"nvva","ンッヴァ"},{"nvvi","ンッヴィ"},               {"nvve","ンッヴェ"},{"nvvo","ンッヴォ"},
	{"nvvu","ンッヴ"},
	{"nwi","ンウィ"},{"nwe","ンウェ"},
	{"nva","ンヴァ"},{"nvi","ンヴィ"},          {"nve","ンヴェ"},{"nvo","ンヴォ"},
	{"nvu","ンヴ"},
	{"nna","ンア"},{"nni","ンイ"},{"nnu","ンウ"},{"nne","ンエ"},{"nno","ンオ"},

	// カ行
	{"nkkya","ンッキャ"},{"nkkyi","ンッキィ"},{"nkkyu","ンッキュ"},{"nkkye","ンッキェ"},{"nkkyo","ンッキョ"},
	{"nggya","ンッギャ"},{"nggyi","ンッギィ"},{"nggyu","ンッギュ"},{"nggye","ンッギェ"},{"nggyo","ンッギョ"},
	{ "nqqa","ンックァ"},{ "nqqi","ンックィ"},                     { "nqqe","ンックェ"},{ "nqqo","ンックォ"},
	{"nkka","ンッカ"},{"nkki","ンッキ"},{"nkku","ンック"},{"nkke","ンッケ"},{"nkko","ンッコ"},
	{"ncca","ンッカ"},{"nccu","ンック"},{"ncco","ンッコ"},
	{"nqqu","ンック"},
	{"nca","ンカ"},{"ncu","ンク"},{"nco","ンコ"},
	{"ngga","ンッガ"},{"nggi","ンッギ"},{"nggu","ンッグ"},{"ngge","ンッゲ"},{"nggo","ンッゴ"},
	{"nkya","ンキャ"},{"nkyi","ンキィ"},{"nkyu","ンキュ"},{"nkye","ンキェ"},{"nkyo","ンキョ"},
	{ "nqa","ンクァ"},{ "nqi","ンクィ"},                  { "nqe","ンクェ"},{ "nqo","ンクォ"},
	{"ngya","ンギャ"},{"ngyi","ンギィ"},{"ngyu","ンギュ"},{"ngye","ンギェ"},{"ngyo","ンギョ"},
	{"nka","ンカ"},{"nki","ンキ"},{"nku","ンク"},{"nke","ンケ"},{"nko","ンコ"},
	{"nca","ンカ"},{"ncu","ンク"},{"nco","ンコ"},
	{"nqu","ンク"  },
	{"nga","ンガ"},{"ngi","ンギ"},{"ngu","ング"},{"nge","ンゲ"},{"ngo","ンゴ"},

	// サ行
	{"nssya","ンッシャ"},{"nssyi","ンッシィ"},{"nssyu","ンッシュ"},{"nssye","ンッシェ"},{"nssyo","ンッショ"},
	{"nssha","ンッシャ"},                     {"nsshu","ンッシュ"},{"nsshe","ンッシェ"},{"nssho","ンッショ"},
	{ "njja","ンッジャ"},                     { "njju","ンッジュ"},{ "njje","ンッジェ"},{ "njjo","ンッジョ"},
	{"njjya","ンッジャ"},{"njjyi","ンッジィ"},{"njjyu","ンッジュ"},{"njjye","ンッジェ"},{"njjyo","ンッジョ"},
	{"nzzya","ンッジャ"},{"nzzyi","ンッジィ"},{"nzzyu","ンッジュ"},{"nzzye","ンッジェ"},{"nzzyo","ンッジョ"},
	{"nssa","ンッサ"},{"nssi","ンッシ"},{"nssu","ンッス"},{"nsse","ンッセ"},{"nsso","ンッソ"},
	{"nsshi","ンッシ"  },
	{"ncci","ンッシ"},{"ncce","ンッセ"},
	{ "njji","ンッジ"  },
	{"nzza","ンッザ"},{"nzzi","ンッジ"},{"nzzu","ンッズ"},{"nzze","ンッゼ"},{"nzzo","ンッゾ"},
	{"nsya","ンシャ"},{"nsyi","ンシィ"},{"nsyu","ンシュ"},{"nsye","ンシェ"},{"nsyo","ンショ"},
	{"nsha","ンシャ"},                  {"nshu","ンシュ"},{"nshe","ンシェ"},{"nsho","ンショ"},
	{ "nja","ンジャ"},                  { "nju","ンジュ"},{ "nje","ンジェ"},{ "njo","ンジョ"},
	{"njya","ンジャ"},{"njyi","ンジィ"},{"njyu","ンジュ"},{"njye","ンジェ"},{"njyo","ンジョ"},
	{"nzya","ンジャ"},{"nzyi","ンジィ"},{"nzyu","ンジュ"},{"nzye","ンジェ"},{"nzyo","ンジョ"},
	{"nsa","ンサ"},{"nsi","ンシ"},{"nsu","ンス"},{"nse","ンセ"},{"nso","ンソ"},
	{"nshi","ンシ"  },
	{"nci","ンシ"},{"nce","ンセ"},
	{ "nji","ンジ"  },
	{"nza","ンザ"},{"nzi","ンジ"},{"nzu","ンズ"},{"nze","ンゼ"},{"nzo","ンゾ"},

	// タ行
	{"nttya","ンッチャ"},{"nttyi","ンッチィ"},{"nttyu","ンッチュ"},{"nttye","ンッチェ"},{"nttyo","ンッチョ"},
	{"nccya","ンッチャ"},{"nccyi","ンッチィ"},{"nccyu","ンッチュ"},{"nccye","ンッチェ"},{"nccyo","ンッチョ"},
	{"nccha","ンッチャ"},                     {"ncchu","ンッチュ"},{"ncche","ンッチェ"},{"nccho","ンッチョ"},
	{"nttha","ンッテャ"},{"ntthi","ンッティ"},{"ntthu","ンッテュ"},{"ntthe","ンッテェ"},{"nttho","ンッテョ"},
	{"nttsa","ンッツァ"},{"nttsi","ンッツィ"},                     {"nttse","ンッツェ"},{"nttso","ンッツォ"},
	{"nddya","ンッヂャ"},{"nddyi","ンッヂィ"},{"nddyu","ンッヂュ"},{"nddye","ンッヂェ"},{"nddyo","ンッヂョ"},
	{"nddha","ンッデャ"},{"nddhi","ンッディ"},{"nddhu","ンッデュ"},{"nddhe","ンッデェ"},{"nddho","ンッデョ"},
	{"ntta","ンッタ"},{"ntti","ンッチ"},{"nttu","ンッツ"},{"ntte","ンッテ"},{"ntto","ンット"},
	{"nttsu","ンッツ"  },
	{"ncchi","ンッチ"  },
	{"ndda","ンッダ"},{"nddi","ンッヂ"},{"nddu","ンッヅ"},{"ndde","ンッデ"},{"nddo","ンッド"},
	{"ntya","ンチャ"},{"ntyi","ンチィ"},{"ntyu","ンチュ"},{"ntye","ンチェ"},{"ntyo","ンチョ"},
	{"ncha","ンチャ"},                  {"nchu","ンチュ"},{"nche","ンチェ"},{"ncho","ンチョ"},
	{"ncya","ンチャ"},{"ncyi","ンチィ"},{"ncyu","ンチュ"},{"ncye","ンチェ"},{"ncyo","ンチョ"},
	{"ntha","ンテャ"},{"nthi","ンティ"},{"nthu","ンテュ"},{"nthe","ンテェ"},{"ntho","ンテョ"},
	{"ntsa","ンツァ"},{"ntsi","ンツィ"},                  {"ntse","ンツェ"},{"ntso","ンツォ"},
	{"ndya","ンヂャ"},{"ndyi","ンヂィ"},{"ndyu","ンヂュ"},{"ndye","ンヂェ"},{"ndyo","ンヂョ"},
	{"ndha","ンデャ"},{"ndhi","ンディ"},{"ndhu","ンデュ"},{"ndhe","ンデェ"},{"ndho","ンデョ"},
	{ "nta","ンタ"},{ "nti","ンチ"},{ "ntu","ンツ"},{ "nte","ンテ"},{ "nto","ント"},
	{"nchi","ンチ"  },
	{"ntsu","ンツ"  },
	{ "nda","ンダ"},{ "ndi","ンヂ"},{ "ndu","ンヅ"},{ "nde","ンデ"},{ "ndo","ンド"},

	// ナ行
	{"nnnya","ンニャ"},{"nnnyi","ンニィ"},{"nnnyu","ンニュ"},{"nnnye","ンニェ"},{"nnnyo","ンニョ"},
	{"nnna","ンナ"},{"nnni","ンニ"},{"nnnu","ンヌ"},{"nnne","ンネ"},{"nnno","ンノ"},

	// ハ行
	{"nhhya","ンッヒャ"},{"nhhyi","ンッヒィ"},{"nhhyu","ンッヒュ"},{"nhhye","ンッヒェ"},{"nhhyo","ンッヒョ"},
	{"nbbya","ンッビャ"},{"nbbyi","ンッビィ"},{"nbbyu","ンッビュ"},{"nbbye","ンッビェ"},{"nbbyo","ンッビョ"},
	{"nppya","ンッピャ"},{"nppyi","ンッピィ"},{"nppyu","ンッピュ"},{"nppye","ンッピェ"},{"nppyo","ンッピョ"},
	{ "nffa","ンッファ"},{ "nffi","ンッフィ"},                     { "nffe","ンッフェ"},{ "nffo","ンッフォ"},
	{"nffya","ンッフャ"},{"nffyi","ンッフィ"},{"nffyu","ンッフュ"},{"nffye","ンッフェ"},{"nffyo","ンッフョ"},
	{ "nffu","ンッフ"  },
	{"nhha","ンッハ"},{"nhhi","ンッヒ"},{"nhhu","ンッフ"},{"nhhe","ンッヘ"},{"nhho","ンッホ"},
	{"nbba","ンッバ"},{"nbbi","ンッビ"},{"nbbu","ンッブ"},{"nbbe","ンッベ"},{"nbbo","ンッボ"},
	{"nppa","ンッパ"},{"nppi","ンッピ"},{"nppu","ンップ"},{"nppe","ンッペ"},{"nppo","ンッポ"},
	{"nhya","ンヒャ"},{"nhyi","ンヒィ"},{"nhyu","ンヒュ"},{"nhye","ンヒェ"},{"nhyo","ンヒョ"},
	{"nbya","ンビャ"},{"nbyi","ンビィ"},{"nbyu","ンビュ"},{"nbye","ンビェ"},{"nbyo","ンビョ"},
	{"npya","ンピャ"},{"npyi","ンピィ"},{"npyu","ンピュ"},{"npye","ンピェ"},{"npyo","ンピョ"},
	{ "nfa","ンファ"},{ "nfi","ンフィ"},                  { "nfe","ンフェ"},{ "nfo","ンフォ"},
	{"nfya","ンフャ"},{"nfyi","ンフィ"},{"nfyu","ンフュ"},{"nfye","ンフェ"},{"nfyo","ンフョ"},
	{ "nfu","ンフ"  },
	{"nha","ンハ"},{"nhi","ンヒ"},{"nhu","ンフ"},{"nhe","ンヘ"},{"nho","ンホ"},
	{"nba","ンバ"},{"nbi","ンビ"},{"nbu","ンブ"},{"nbe","ンベ"},{"nbo","ンボ"},
	{"npa","ンパ"},{"npi","ンピ"},{"npu","ンプ"},{"npe","ンペ"},{"npo","ンポ"},

	// マ行
	{"nmya","ンミャ"},{"nmyi","ンミィ"},{"nmyu","ンミュ"},{"nmye","ンミェ"},{"nmyo","ンミョ"},
	{"nma","ンマ"},{"nmi","ンミ"},{"nmu","ンム"},{"nme","ンメ"},{"nmo","ンモ"},

	// ヤ行
	{"nyya","ンッヤ"},{"nyyu","ンッユ"},{"nyyo","ンッヨ"},
	{"nnya","ンヤ"},{"nnyu","ンユ"},{"nnyo","ンヨ"},
	{"nnyi","ンイ"},{"nnye","ンイェ"},

	// ラ行
	{"nrrya","ンッリャ"},{"nrryi","ンッリィ"},{"nrryu","ンッリュ"},{"nrrye","ンッリェ"},{"nrryo","ンッリョ"},
	{"nrra","ンッラ"},{"nrri","ンッリ"},{"nrru","ンッル"},{"nrre","ンッレ"},{"nrro","ンッロ"},
	{"nrya","ンリャ"},{"nryi","ンリィ"},{"nryu","ンリュ"},{"nrye","ンリェ"},{"nryo","ンリョ"},
	{"nra","ンラ"},{"nri","ンリ"},{"nru","ンル"},{"nre","ンレ"},{"nro","ンロ"},

	// ワ行(ンを除く)
	{"nwwa","ンッワ"},{"nwwo","ンッヲ"},
	{"nwa","ンワ"},{"nwo","ンヲ"},

	// 小さい文字
	{"nxa","ンァ"},{"nxi","ンィ"},{"nxu","ンゥ"},{"nxe","ンェ"},{"nxo","ンォ"},
	{"nla","ンァ"},{"nli","ンィ"},{"nlu","ンゥ"},{"nle","ンェ"},{"nlo","ンォ"},
	{"nxka","ンヵ"},{"nxke","ンヶ"},
	{"nlka","ンヵ"},{"nlke","ンヶ"},

	{"nxa","ンァ"},{"nxi","ンィ"},{"nxu","ンゥ"},{"nxe","ンェ"},{"nxo","ンォ"},
	{"nla","ンァ"},{"nli","ンィ"},{"nlu","ンゥ"},{"nle","ンェ"},{"nlo","ンォ"},
	{"nxye","ンェ"},
	{"nlye","ンェ"},
	{"nxka","ンヵ"},{"nxke","ンヶ"},
	{"nlka","ンヵ"},{"nlke","ンヶ"},
	{"nxtu","ンッ"},{"nltu","ンッ"},
	{"nxtsu","ンッ"},{"nltsu","ンッ"},
	{"nxya","ンャ"},{"nxyu","ンュ"},{"nxyo","ンョ"},
	{"nlya","ンャ"},{"nlyu","ンュ"},{"nlyo","ンョ"},
	{"nxwa","ンヮ"},
	{"nlwa","ンヮ"},

	// ヘボン式ではb,m,pの直前の『m』は『ン』として扱う
	// ※ンより先に判断するとカナローマ変換時にヘボン式優先になる
	{"mbya","ンビャ"},{"mbyi","ンビィ"},{"mbyu","ンビュ"},{"mbye","ンビェ"},{"mbyo","ンビョ"},
	{"mpya","ンピャ"},{"mpyi","ンピィ"},{"mpyu","ンピュ"},{"mpye","ンピェ"},{"mpyo","ンピョ"},
	{"mbba","ンッバ"},{"mbbi","ンッビ"},{"mbbu","ンッブ"},{"mbbe","ンッベ"},{"mbbo","ンッボ"},
	{"mppa","ンッパ"},{"mppi","ンッピ"},{"mppu","ンップ"},{"mppe","ンッペ"},{"mppo","ンッポ"},
	{"mbbya","ンッビャ"},{"mbbyi","ンッビィ"},{"mbbyu","ンッビュ"},{"mbbye","ンッビェ"},{"mbbyo","ンッビョ"},
	{"mppya","ンッピャ"},{"mppyi","ンッピィ"},{"mppyu","ンッピュ"},{"mppye","ンッピェ"},{"mppyo","ンッピョ"},
	{"mba","ンバ"},{"mbi","ンビ"},{"mbu","ンブ"},{"mbe","ンベ"},{"mbo","ンボ"},
	{"mpa","ンパ"},{"mpi","ンピ"},{"mpu","ンプ"},{"mpe","ンペ"},{"mpo","ンポ"},
	{"mma","ンマ"},{"mmi","ンミ"},{"mmu","ンム"},{"mme","ンメ"},{"mmo","ンモ"},
	{"mmya","ンミャ"},{"mmyi","ンミィ"},{"mmyu","ンミュ"},{"mmye","ンミェ"},{"mmyo","ンミョ"},

	// 複数の解釈がある記号の優先候補
	{"n,","ン、"},{"n-","ンー"},{"n.","ン。"},{"n/","ン・"},
	{"n[","ン「"},{"n]","ン」"},
	{"n~","ン～"},

	{"n,","ン、"},{"n-","ンー"},{"n.","ン。"},{"n/","ン・"},
	{"n[","ン「"},{"n]","ン」"},
	{"n~","ン～"},

	// ローマ字ではないがローマ字入力で使用可能な文字
	{"n0","ン０"},{"n1","ン１"},{"n2","ン２"},{"n3","ン３"},{"n4","ン４"},{"n5","ン５"},{"n6","ン６"},{"n7","ン７"},{"n8","ン８"},{"n9","ン９"},
	{"nb","ンｂ"},{"nc","ンｃ"},{"nd","ンｄ"},{"nf","ンｆ"},{"ng","ンｇ"},{"nh","ンｈ"},{"nj","ンｊ"},{"nk","ンｋ"},{"nl","ンｌ"},{"nm","ンｍ"},{"np","ンｐ"},{"nq","ンｑ"},{"nr","ンｒ"},{"ns","ンｓ"},{"nt","ンｔ"},{"nv","ンｖ"},{"nw","ンｗ"},{"nx","ンｘ"},{"ny","ンｙ"},{"nz","ンｚ"},
	{"nB","ンＢ"},{"nC","ンＣ"},{"nD","ンＤ"},{"nF","ンＦ"},{"nG","ンＧ"},{"nH","ンＨ"},{"nJ","ンＪ"},{"nK","ンＫ"},{"nL","ンＬ"},{"nM","ンＭ"},{"nP","ンＰ"},{"nQ","ンＱ"},{"nR","ンＲ"},{"nS","ンＳ"},{"nT","ンＴ"},{"nV","ンＶ"},{"nW","ンＷ"},{"nX","ンＸ"},{"nY","ンＹ"},{"nZ","ンＺ"},
	{"n ","ン　"},{"n!","ン！"},{"n\"","ン”"},{"n#","ン＃"},{"n$","ン＄"},{"n%","ン％"},{"n&","ン＆"},{"n'","ン’"},{"n(","ン（"},{"n)","ン）"},{"n*","ン＊"},{"n+","ン＋"},{"n,","ン，"},{"n-","ン－"},{"n.","ン．"},{"n/","ン／"},
	{"n:","ン："},{"n;","ン；"},{"n<","ン＜"},{"n=","ン＝"},{"n>","ン＞"},{"n?","ン？"},{"n@","ン＠"},
	{"n[","ン［"},{"n\\","ン￥"},{"n]","ン］"},{"n^","ン＾"},{"n_","ン＿"},{"n`","ン｀"},
	{"n{","ン｛"},{"n|","ン｜"},{"n}","ン｝"},{"n~","ン￣"},

	// 改行対策
	{"n\n","ン"},
	{"n\r","ン"},

	// 純粋な『ン』
	//{"n","ン"},

	// 小さい文字
	{"xa","ァ"},{"xi","ィ"},{"xu","ゥ"},{"xe","ェ"},{"xo","ォ"},
	{"la","ァ"},{"li","ィ"},{"lu","ゥ"},{"le","ェ"},{"lo","ォ"},
	{"xka","ヵ"},{"xke","ヶ"},
	{"lka","ヵ"},{"lke","ヶ"},
	{"xtu","ッ"},{"ltu","ッ"},
	{"xtsu","ッ"},{"ltsu","ッ"},
	{"xya","ャ"},{"xyu","ュ"},{"xyo","ョ"},
	{"lya","ャ"},{"lyu","ュ"},{"lyo","ョ"},
	{"xye","ェ"},
	{"lye","ェ"},
	{"xwa","ヮ"},
	{"lwa","ヮ"},

	// 複数の解釈がある記号の優先候補
	{",","、"},{"-","ー"},{".","。"},{"/","・"},
	{"[","「"},{"]","」"},
	{"~","～"},

	// ローマ字ではないがローマ字入力で使用可能な文字
	{"0","０"},{"1","１"},{"2","２"},{"3","３"},{"4","４"},{"5","５"},{"6","６"},{"7","７"},{"8","８"},{"9","９"},
	{"a","ａ"},{"b","ｂ"},{"c","ｃ"},{"d","ｄ"},{"e","ｅ"},{"f","ｆ"},{"g","ｇ"},{"h","ｈ"},{"i","ｉ"},{"j","ｊ"},{"k","ｋ"},{"l","ｌ"},{"m","ｍ"},{"n","ｎ"},{"o","ｏ"},{"p","ｐ"},{"q","ｑ"},{"r","ｒ"},{"s","ｓ"},{"t","ｔ"},{"u","ｕ"},{"v","ｖ"},{"w","ｗ"},{"x","ｘ"},{"y","ｙ"},{"z","ｚ"},
	{"A","Ａ"},{"B","Ｂ"},{"C","Ｃ"},{"D","Ｄ"},{"E","Ｅ"},{"F","Ｆ"},{"G","Ｇ"},{"H","Ｈ"},{"I","Ｉ"},{"J","Ｊ"},{"K","Ｋ"},{"L","Ｌ"},{"M","Ｍ"},{"N","Ｎ"},{"O","Ｏ"},{"P","Ｐ"},{"Q","Ｑ"},{"R","Ｒ"},{"S","Ｓ"},{"T","Ｔ"},{"U","Ｕ"},{"V","Ｖ"},{"W","Ｗ"},{"X","Ｘ"},{"Y","Ｙ"},{"Z","Ｚ"},
	{" ","　"},{"!","！"},{"\"","”"},{"#","＃"},{"$","＄"},{"%","％"},{"&","＆"},{"'","’"},{"(","（"},{")","）"},{"*","＊"},{"+","＋"},{",","，"},{"-","－"},{".","．"},{"/","／"},
	{":","："},{";","；"},{"<","＜"},{"=","＝"},{">","＞"},{"?","？"},{"@","＠"},
	{"[","［"},{"\\","￥"},{"]","］"},{"^","＾"},{"_","＿"},{"`","｀"},
	{"{","｛"},{"|","｜"},{"}","｝"},{"~","￣"},
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
							// ヒントと完全に一致する場合
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
							// ヒントと部分的に一致する場合
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
							// ヒントと完全に一致する場合
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
							// ヒントと部分的に一致する場合
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
					// 完全に一致する場合
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
							// ヒントと完全に一致する場合
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
							// ヒントと部分的に一致する場合
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
							// ヒントと完全に一致する場合
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
							// ヒントと部分的に一致する場合
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
					// 完全に一致する場合
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
				// 終端の重ね"n"用パッチ
				if(strcmp(&lpText[m],"n")==0) {
					if(roma2kana("nn",str,(lpHint&&*lpHint)?&lpHint[n]:NULL,dwFlags)==2) {
						l = 1;
					}
				}
			}
			if(l!=-1) {
				// 正常に変換(あるいはヒントと部分一致)した場合
				DWORD len = strlen(str);
				if(n+len<cchBuf) {
					memcpy(&lpBuf[n],str,len);
					lpBuf[n+len] = '\0';
					n += len;
					m += l;
				} else {
					break;	// これ以上変換結果を格納できない場合
				}
				if(l==0) {
					break;	// 部分一致の場合
				}
			} else {
				// ヒントと一致しない場合
				if(lpHint && *lpHint) {
					DWORD n = roma2kana(&lpText[m],str,NULL,dwFlags);
					if(n!=-1 || n<0) {
						m = -1;
						break;
					}
				}
				// 変換不可能な場合
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
				// 正常に変換(あるいはヒントと部分一致)した場合
				DWORD len = strlen(str);
				if(n+len<cchBuf) {
					memcpy(&lpBuf[n],str,len);
					lpBuf[n+len] = '\0';
					n += len;
					m += l;
				} else {
					break;	// これ以上変換結果を格納できない場合
				}
				if(l==0) {
					break;	// 部分一致の場合
				}
			} else {
				// 変換可能だがヒントと一致しない場合
				if(lpHint && *lpHint) {
					DWORD n = kana2roma(&lpText[m],str,NULL,dwFlags);
					if(n!=-1 || n<0) {
						m = -1;
						break;
					}
				}
				// 変換できない場合
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
			// 終端の重ね"n"用パッチ
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
	部分文字列取得(csvのような文字区切りテキストで、特定インデックスの部分文字列を得る)
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
				n++;	// 最後の要素が空でもカウントする
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
				n++;	// 最後の要素が空でもカウントする
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
			} else if(tmp.substring(s).isEqual(lpString)) {	// 最後の要素は特殊扱い
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
			} else if(tmp.substring(s).isEqual(lpString)) {	// 最後の要素は特殊扱い
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
