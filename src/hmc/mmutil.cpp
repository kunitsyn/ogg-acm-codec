/*
	mmsystem サポートクラス
	Copyright (C)1996-98 H.Mutsuki
*/

#include "util.h"
#include "mmutil.h"

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
	MMSystem ユーティリティクラス
*****************************************************************/
/*****************************************************************
	リザルトコード検証
*****************************************************************/
MMRESULT CmmSys::CheckResult(MMRESULT dwResult, LPCTSTR szFileName, UINT nLineNumber)
{
	if(dwResult) {
		PrintResult(dwResult,szFileName,nLineNumber);
	}
	return dwResult;
}
void CmmSys::PrintResult(MMRESULT dwResult, LPCTSTR szFileName, UINT nLineNumber)
{
	char s[16];
	sprintf(s,"%u\n",nLineNumber);
	::OutputDebugString(GetErrorMessage(dwResult));
	::OutputDebugString(": File ");
	::OutputDebugString(szFileName);
	::OutputDebugString(", Line ");
	::OutputDebugString(s);
}
LPCTSTR CmmSys::GetErrorMessage(MMRESULT dwResult)
{
	static const struct {
		DWORD code;
		LPCTSTR str;
	} assoc[] = {
#define METHOD(eno) {eno,#eno},
		METHOD(MMSYSERR_NOERROR)		// no error
		METHOD(MMSYSERR_ERROR)			// unspecified error
		METHOD(MMSYSERR_BADDEVICEID)	// device ID out of range
		METHOD(MMSYSERR_NOTENABLED)		// driver failed enable
		METHOD(MMSYSERR_ALLOCATED)		// device already allocated
		METHOD(MMSYSERR_INVALHANDLE)	// device handle is invalid
		METHOD(MMSYSERR_NODRIVER)		// no device driver present
		METHOD(MMSYSERR_NOMEM)			// memory allocation error
		METHOD(MMSYSERR_NOTSUPPORTED)	// function isn't supported
		METHOD(MMSYSERR_BADERRNUM)		// error value out of range
		METHOD(MMSYSERR_INVALFLAG)		// invalid flag passed
		METHOD(MMSYSERR_INVALPARAM)		// invalid parameter passed
		METHOD(MMSYSERR_HANDLEBUSY)		// handle being used
		METHOD(MMSYSERR_INVALIDALIAS)	// specified alias not found
		METHOD(MMSYSERR_BADDB)			// bad registry database
		METHOD(MMSYSERR_KEYNOTFOUND)	// registry key not found
		METHOD(MMSYSERR_READERROR)		// registry read error
		METHOD(MMSYSERR_WRITEERROR)		// registry write error
		METHOD(MMSYSERR_DELETEERROR)	// registry delete error
		METHOD(MMSYSERR_VALNOTFOUND)	// registry value not found
		METHOD(MMSYSERR_NODRIVERCB)		// driver does not call DriverCallback
		METHOD(MMSYSERR_LASTERROR)		// last error in range
#undef METHOD
	};
	for(int i=0; i<ARRAYLEN(assoc); i++) {
		if(assoc[i].code == dwResult) {
			return assoc[i].str;
		}
	}
	return assoc[ARRAYLEN(assoc)-1].str;
}

/*****************************************************************
	FOURCCで指定されるチャンクのポインタを得る
*****************************************************************/
FOURCC *FindRIFFChunk(LPCVOID pvForm, FOURCC fcc)
{
	FOURCC *pcc = (FOURCC *)pvForm;
	FOURCC fcRIFF   = *pcc++;	// RIFF ヘッダ
	FOURCC fcLength = *pcc++;	// RIFF データ長
	FOURCC fcType   = *pcc++;	// WAVE ヘッダ
	FOURCC *pccEnd = GetRIFFNext((FOURCC *)pvForm);
	while(pcc < pccEnd) {
		if(fcc == 0 || *pcc == fcc) return pcc;
		pcc = GetRIFFNext(pcc);
	}
	return NULL;
}
FOURCC *GetRIFFNext(const FOURCC *pfcc)
{
	return (FOURCC *)(LPBYTE(pfcc) + sizeof(FOURCC)*2 + GetRIFFLength(pfcc));
}
DWORD GetRIFFLength(const FOURCC *pfcc)
{
	return ((DWORD *)pfcc)[1];
}
DWORD GetRIFFWaveDataLength(LPCVOID pv)
{
	DWORD cbLength = 0;
	do {
		if(!pv) {
			break;
		}
		FOURCC *p = (FOURCC *)pv;
		if(!(p[0] == MAKEFOURCC('R','I','F','F') &&
			 p[2] == MAKEFOURCC('W','A','V','E') &&
			 p[3] == MAKEFOURCC('f','m','t',' '))) {
			break;
		}
		p = FindRIFFChunk(p,MAKEFOURCC('d','a','t','a'));
		if(!p) {
			break;
		}
		cbLength = GetRIFFLength(p);
	} while(0);
	return cbLength;
}
LPCVOID GetRIFFWaveData(LPCVOID pv)
{
	LPBYTE pData = NULL;
	do {
		if(!pv) {
			break;
		}
		FOURCC *p = (FOURCC *)pv;
		if(!(p[0] == MAKEFOURCC('R','I','F','F') &&
			 p[2] == MAKEFOURCC('W','A','V','E') &&
			 p[3] == MAKEFOURCC('f','m','t',' '))) {
			break;
		}
		p = FindRIFFChunk(p,MAKEFOURCC('d','a','t','a'));
		if(!p) {
			break;
		}
		pData = (LPBYTE)p + sizeof(FOURCC) + sizeof(DWORD);
	} while(0);
	return pData;
}

/*****************************************************************
	無音データを書き込む
*****************************************************************/
LPVOID WriteMute(LPVOID dest, size_t count, LPWAVEFORMATEX pwfx)
{
	return WriteMute(dest, count, pwfx->nChannels, pwfx->wBitsPerSample);
}

LPVOID WriteMute(LPVOID dest, size_t count, WORD nChannels, WORD wBitsPerSample)
{
	if(nChannels == 1) {
		if(wBitsPerSample == 8) {
			memfill(dest, BYTE(0x80), count);
		} else {
			memfill(dest, WORD(0x0000), count / sizeof(WORD));
		}
	} else {
		if(wBitsPerSample == 8) {
			memfill(dest, WORD(0x8080), count / sizeof(WORD));
		} else {
			memfill(dest, DWORD(0x00000000), count / sizeof(DWORD));
		}
	}
	return dest;
}


/*****************************************************************
	db<->Percent変換(数値表現に注意)
*****************************************************************/
LONG db2perByLong(LONG lValue)
{
	double d = lValue;
	d = db2per(d/100);
	LONG l = (LONG)d*10000;
	l = max(-10000,min(l,10000));
	return l;
}
LONG per2dbByLong(LONG lValue)
{
	double d = lValue;
	d = per2db(d/10000);
	LONG l = (LONG)d*100;
	l = max(-10000,min(l,10000));
	return l;
}
double db2per(double dValue)
{
	double d = dValue;
	if(d<0) {
		d = -(1.0 - pow(10,d/20));
	} else if(0<d) {
		d = 1.0 - pow(10,-d/20);
	} else {
		d = 0.0;
	}
	return d;
}
double per2db(double dValue)
{
	double d = dValue;
	if(1.0<=d) {
		d = +100.00;	// essentially +INF
	} else if(d<=-1.0) {
		d = -100.00;	// essentially -INF
	} else if(d<0) {
		d = log10(1.0+d)*20;
	} else if(0<d) {
		d = -log10(1.0-d)*20;
	} else {
		d = 0.0;		// no gain
	}
	return d;
}

/*****************************************************************
	色空間変換
*****************************************************************/
// RGB<->CMY変換
void rgb2cmy(double r, double g, double b, double &c, double &m, double &y)
{
	c = 1.0 - r;
	m = 1.0 - g;
	y = 1.0 - b;
}
void cmy2rgb(double c, double m, double y, double &r, double &g, double &b)
{
	r = 1.0 - c;
	g = 1.0 - m;
	b = 1.0 - y;
}

// CMY<->CMYK変換
void cmy2cmyk(double c, double m, double y, double &C, double &M, double &Y, double &k)
{
	k = min(min(c,m),y);
	c = (c-k) / (1.0-k);
	m = (m-k) / (1.0-k);
	y = (c-k) / (1.0-k);
}
void cmyk2cmy(double c, double m, double y, double k, double &C, double &M, double &Y)
{
	C = min(1.0, c*(1.0-k)+k);
	M = min(1.0, m*(1.0-k)+k);
	Y = min(1.0, y*(1.0-k)+k);
}

// RGB<->CMYK変換
void rgb2cmyk(double r, double g, double b, double &c, double &m, double &y, double &k)
{
	k = min(min(1.0-r,1.0-g),1.0-b);
	c = (1.0-r-k)/(1.0-k);
	m = (1.0-g-k)/(1.0-k);
	y = (1.0-b-k)/(1.0-k);
}
void cmyk2rgb(double c, double m, double y, double k, double &r, double &g, double &b)
{
	r = 1.0 - min(1.0, c*(1.0-k)+k);
	g = 1.0 - min(1.0, m*(1.0-k)+k);
	b = 1.0 - min(1.0, y*(1.0-k)+k);
}

// RGB<->HSL変換
void rgb2hsl(double r, double g, double b, double &h, double &s, double &l)
{
}
void hsl2rgb(double h, double s, double l, double &r, double &g, double &b)
{
}
// RGB<->CIE XYZ変換(CIE XYZccir601-1 (C illuminant))
void rgb2xyz(double r, double g, double b, double &x, double &y, double &z)
{
	x = 0.607*r + 0.174*g + 0.200*b;
	y = 0.299*r + 0.587*g + 0.114*b;
	z = 0.000*r + 0.066*g + 1.116*b;
}
void xyz2rgb(double x, double y, double z, double &r, double &g, double &b)
{
	r = 1.910*x - 0.532*y - 0.288*z;
	g =-0.985*x + 1.999*y - 0.028*z;
	b = 0.058*x - 0.118*y + 0.898*z;
}
// RGB<->CIE XYZ変換(CIE XYZccir709 (D65))
void rgb2xyz709(double r, double g, double b, double &x, double &y, double &z)
{
	x = 0.412*r + 0.358*g + 0.180*b;
	y = 0.213*r + 0.715*g + 0.072*b;
	z = 0.019*r + 0.119*g + 0.950*b;
}
void xyz7092rgb(double x, double y, double z, double &r, double &g, double &b)
{
	r = 3.241*x - 1.537*y - 0.499*z;
	g =-0.969*x + 1.876*y + 0.042*z;
	b = 0.056*x - 0.204*y + 1.057*z;
}
// RGB<->CIE ITU変換(CIE ITU(D65))
void rgb2itu(double r, double g, double b, double &x, double &y, double &z)
{
	x = 0.4305*r + 0.3415*g + 0.1784*b;
	y = 0.2220*r + 0.7067*g + 0.0713*b;
	z = 0.0202*r + 0.1295*g + 0.9394*b;
}
void itu2rgb(double x, double y, double z, double &r, double &g, double &b)
{
	r = 3.0527*x - 1.3928*y - 0.4759*z;
	g =-0.9689*x + 1.8756*y + 0.0417*z;
	b = 0.0585*x - 0.2286*y + 1.0690*z;
}

// CIE XYZ<->CIE Luv変換
void xyz2luv(double x, double y, double z, double &l, double &u, double &v)
{
}
void luv2xyz(double l, double u, double v, double &x, double &y, double &z)
{
}

// CIE XYZ<->CIE Lab変換
void xyz2lab(double x, double y, double z, double &l, double &a, double &b)
{
}
void lab2xyz(double l, double a, double b, double &x, double &y, double &z)
{
}

// CIE Lab<->LCH変換
void lab2lch(double l, double a, double b, double &L, double &c, double &h)
{
}
void lch2lab(double l, double c, double h, double &L, double &a, double &b)
{
}

// RGB<->YUV
void rgb2yuv(double r, double g, double b, double &y, double &u, double &v)
{
}
void yuv2rgb(double y, double u, double v, double &r, double &g, double &b)
{
}

// RGB<->YIQ
void rgb2yiq(double r, double g, double b, double &y, double &i, double &q)
{
}
void yiq2rgb(double y, double i, double q, double &r, double &g, double &b)
{
}

// YUV<->YIQ
void yuv2yiq(double y, double u, double v, double &Y, double &i, double &q)
{
}
void yiq2yuv(double y, double i, double q, double &Y, double &u, double &v)
{
}

// RGB<->YCbCr(with Rec 709 specs, ITU)
void rgb2ycbcr(double r, double g, double b, double &y, double &cb, double &cr)
{
}
void ycbcr2rgb(double y, double cb, double cr, double &r, double &g, double &b)
{
}

// RGB<->YCbCr(with Rec 601-1 specs ... TIFF and JPEG works)
void rgb2ycbcr601(double r, double g, double b, double &y, double &cb, double &cr)
{
}
void ycbcr2rgb601(double y, double cb, double cr, double &r, double &g, double &b)
{
}

// SMPTE-C RGB<->CIE XYZ(D65)
void rgb2xyzd65(double r, double g, double b, double &x, double &y, double &z)
{
}
void xyzd652rgb(double x, double y, double z, double &r, double &g, double &b)
{
}

// SMPTE-240M YPbPr<->RGB
void rgb2ypbpr(double r, double g, double b, double &y, double &pb, double &pr)
{
}
void ypbpr2rgb(double y, double pb, double pr, double &r, double &g, double &b)
{
}
// Xerox Corporation YES<->CIE XYZ(D50)
void xyzd502yes(double x, double y, double z, double &Y, double &e, double &s)
{
}
void yes2xyzd50(double y, double e, double s, double &x, double &Y, double &z)
{
}
// Xerox Corporation YES<->CIE XYZ(D65)
void xyzd652yes(double x, double y, double z, double &Y, double &e, double &s)
{
}
void yes2xyzd65(double y, double e, double s, double &x, double &Y, double &z)
{
}
// Kodak Photo CD YCC<->RGB
void rgb2ycc(double r, double g, double b, double &y, double &c1, double &c2)
{
}
void ycc2rgb(double y, double c1, double c2, double &r, double &g, double &b)
{
}

/*****************************************************************
*****************************************************************/
/*****************************************************************
	情報チャンクからの情報取得
*****************************************************************/
tWaveCuePoint *GetCuePoint(tWaveCueInfo *pi, DWORD dwSection)
{
	tWaveCuePoint *pc = NULL;
	if(dwSection < GetCuePoint(pi)) {
		pc = &pi->sect[dwSection];
	}
	return pc;
}

tWavePlstSegment *GetPlaySegment(tWavePlstInfo *pi, DWORD dwSection)
{
	tWavePlstSegment *pc = NULL;
	if(dwSection < GetPlaySegment(pi)) {
		pc = &pi->sect[dwSection];
	}
	return pc;
}

tWaveSmplLoop *GetSamplerLoop(tWaveSmplInfo *pi, DWORD dwSection)
{
	tWaveSmplLoop *pc = NULL;
	if(dwSection < GetSamplerLoop(pi)) {
		pc = &pi->sect[dwSection];
	}
	return pc;
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
