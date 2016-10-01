/*
	HMCライブラリ共通ユーティリティ
	Copyright (C)1996-2001 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_UTIL__
#define __INC_HMC_UTIL__

// VC++ 5.0対策
// ※VC++ 6.0以降のwindef.hではNO_STRICTを定義しない限りSTRICTが定義される。
// 既に他のヘッダを取り込んでいる場合は、ここで定義しても手遅れ……。
#ifndef STRICT
#define STRICT 1
#endif

// デバッグヒープ用小細工
// ※_CrtSetDbgFlag()で_CRTDBG_LEAK_CHECK_DFを
// 有効にしないとレポートされないので注意
#ifdef _USE_MFC
#ifndef _MFC_OVERRIDES_NEW
#define _MFC_OVERRIDES_NEW
#endif
#endif
#ifdef _DEBUG
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include "stdlib.h"
#include "malloc.h"
#include "crtdbg.h"
#endif

// MFCフレームワーク
#ifdef _USE_MFC
#include <afxwin.h>		// MFCの標準コンポーネント
#include <afxext.h>		// MFCの拡張部分
#include <afxmt.h>		// MFCのスレッドサポート
#endif

// 最低限のヘッダ(一応PCHのことも考慮すべし……)
#include <windows.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <crtdbg.h>
#include <stddef.h>
#include <math.h>
#include <assert.h>
#include <wchar.h>
#include <ctype.h>
#include <string.h>
#include <mbctype.h>
#include <mbstring.h>
#ifdef _CPPRTTI
#include <typeinfo.h>
#endif

// 最低限のライブラリ
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

/*****************************************************************
	その他型定義(※必ずHMC名前空間の外に置くこと)
*****************************************************************/
// 何故か一般的なヘッダに含まれていないので……
typedef LPVOID *LPLPVOID;		// dsound.h
typedef const BYTE *LPCBYTE;	// odbcss.h, sqlfront.h, winscard.h
typedef unsigned __int64 QWORD;	// sspi.h

// VC++ 5.0対策
// ※VC++ 6.0以前ではbasetsd.hが存在しないのだ
typedef __int64 LONG64, *PLONG64;
typedef __int64 INT64, *PINT64;
typedef unsigned __int64 ULONG64, *PULONG64;
typedef unsigned __int64 DWORD64, *PDWORD64;
typedef unsigned __int64 UINT64, *PUINT64;
typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

/*****************************************************************
	HMC系の名前空間
*****************************************************************/
#define _HMC		HMC::
#define _HMC_BEGIN	namespace HMC {
#define _HMC_END	};
#define _HMC_USING	using namespace HMC;

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
/*****************************************************************
	HMC系ヘッダ共通のプラグマ(※push,popしないことに注意)
*****************************************************************/
// level 1,4
//#pragma warning(disable: 4355)	// 'this' : ベース メンバ初期化リストで使用されました。
// level 1
//#pragma warning(disable: 4244)	// 'variable' : 'type' から 'type' に変換しました。データが失われている可能性があります。
//#pragma warning(disable: 4731)	// 'pointer' : インライン アセンブラ コードにより変更されたフレーム ポインタ レジスタ 'register' です。
// level 3
//#pragma warning(disable: 4018)	// 'expression' : signed と unsigned の数値を比較しようとしました。
// level 3,4
//#pragma warning(disable: 4244)	// 'expression' : signed と unsigned の数値を比較しようとしました。
// level 4
//#pragma warning(disable: 4100)	// 'identifier' : 引数は関数の本体部で 1 度も参照されません。
//#pragma warning(disable: 4127)	// 条件式が定数です
//#pragma warning(disable: 4245)	// 'conversion' : 'type1' から 'type2' に変換しました。signed/unsigned が一致しません。
//#pragma warning(disable: 4706)	// 条件式の比較値は、代入の結果になっています。
//#pragma warning(disable: 4189)	// 'identifier' : ローカル変数が初期化されましたが、参照されていません。
//#pragma warning(disable: 4238)	// 非標準の拡張機能が使用されています : 右辺値のクラスが左辺値に使用されます。
//#pragma warning(disable: 4211)	// 非標準の拡張機能が使用されています : extern が static に再定義されました。
//#pragma warning(disable: 4201)	// 非標準の拡張機能が使用されています : 無名の構造体または共用体です。
//#pragma warning(disable: 4701)	// 値が割り当てられていないローカルな変数 'name' に対する参照が行われる可能性があります。

/*****************************************************************
	グローバル関数
*****************************************************************/
// エンディアン解決付きメモリ操作
WORD GetWordLe(LPCVOID p);
void SetWordLe(LPVOID p, WORD w);
LONG GetLongLe(LPCVOID p);
void SetLongLe(LPVOID p, LONG l);
WORD GetWordBe(LPCVOID p);
void SetWordBe(LPVOID p, WORD w);
LONG GetLongBe(LPCVOID p);
void SetLongBe(LPVOID p, LONG l);

// FIFO -> FIFO 巻き込みコピー
void *fifocpy(void *dst_start, size_t dst_len, size_t dst_index, const void *src_start, size_t src_len, size_t src_index, size_t count);
void *fifocpy(void *dst_start, size_t dst_len, void *dst, const void *src_start, size_t src_len, const void *src, size_t count);
void *fifocpy(void *dst_start, void *dst_end, void *dst, const void *src_start, const void *src_end, const void *src, size_t count);

// 任意長 memset()
void *memfill(void *dest, const void *src, size_t size, size_t count);
void *memfill(void *dest, BYTE  c, size_t count);
void *memfill(void *dest, WORD  c, size_t count);
void *memfill(void *dest, DWORD c, size_t count);
void *memfill(void *dest, QWORD c, size_t count);

// メモリ版 strrev()
void *memrev(void *p, size_t count);
void *memrev(void *dest, const void *src, size_t count);

// 文字列版 fgetc(), fputc(), fgets(), fputs()
int   sgetc(char **stream);
int   sputc(int c, char **stream);
char *sgets(char *string, int n, char **stream);
int   sputs(const char *string, char **stream);

// ファイルのサイズを得る (ファイル構造体を使用)
long fsize(FILE *fp);

// BCD変換
BYTE  bcd(BYTE b);
WORD  bcd(WORD w);
DWORD bcd(DWORD dw);

// パス名変換
char *dtou_path(char *path);
char *utod_path(char *path);
char *MakeRegularPath(char *path);
char *MakeSimplePath(char *path);
char *MakeExtFName (char *NewName, const char *BaseName, const char *NewExt);
char *MakeNameFName(char *NewName, const char *BaseName, const char *NewFName);
char *MakeDirFName (char *NewName, const char *BaseName, const char *NewDir);
char *MakeDrvFName (char *NewName, const char *BaseName, const char *NewDrv);
char *MakeUncFName (char *NewName, const char *BaseName, const char *NewUnc);
char *MakeBaseFName(char *NewName, const char *BaseName);
char *MakeNameFName(char *NewName, const char *BaseName);
char *MakeDirFName (char *NewName, const char *BaseName);

// パス検索
BOOL which(LPCTSTR lpPathList, TCHAR cDelimiter, LPCTSTR lpFile, LPTSTR lpBuf, DWORD cchBuf, LPDWORD lpdwCopied=NULL);
BOOL which(LPCTSTR lpPathList, LPCTSTR lpDelimiter, LPCTSTR lpFile, LPTSTR lpBuf, DWORD cchBuf, LPDWORD lpdwCopied=NULL);

// 固定小数点数変換
FIXED FixedFromDouble(double d);
int FixedFromDouble(FIXED f);

/*****************************************************************
	デバッグヒープの有効化
*****************************************************************/
#ifdef _DEBUG
// _CRTDBG_LEAK_CHECK_DF有効化
extern class __hmc_util__ {
public:
	__hmc_util__();
} __static_initializer_hmc_util__;
#endif

/*****************************************************************
	Win32 API,COMのリザルトコード検証(expは常に評価される)
*****************************************************************/
BOOL CheckResult(BOOL bResult, LPCTSTR szFileName, UINT nLineNumber);
void PrintResult(DWORD dwResult, LPCTSTR szFileName, UINT nLineNumber);
LPCTSTR GetErrorMessage(DWORD dwResult, LPTSTR lpBuf, DWORD cbBuf);

#define ERROR_LASTERROR		(DWORD(~0))

#ifdef _DEBUG
#define CHECKRESULT_API(exp)	(HMC::CheckResult((BOOL)(exp),__FILE__,__LINE__))
#define CHECKRESULT_NT(exp)		(HMC::CheckResult((BOOL)(exp),__FILE__,__LINE__))
#else
#define CHECKRESULT_API(exp)	(exp)
#define CHECKRESULT_NT(exp)		(exp)
#endif

/*****************************************************************
	亜種ASSERT,TRACE(非デバッグではexpは評価されない)
*****************************************************************/
#ifdef _DEBUG
#define BREAK(exp,msg)	do{if(!(exp)&&_CrtDbgReport(_CRT_ASSERT,__FILE__,__LINE__,NULL,"%s\nMessage: %s",#exp,msg)){DBGBREAK();}}while(0)
#else
#define BREAK(exp,msg)	((void)0)
#endif

//#define DBGBREAK()	::DebugBreak()		// システムコード中でブレークするので×
#define DBGBREAK()		__asm { int 3 }		// こちらの方がデバッガでステップし易い

#ifdef _DEBUG
#define LASTERROR(exp)		::SetLastError(exp)
#define RESETLASTERROR()	::SetLastError(ERROR_SUCCESS)
#else
#define LASTERROR(exp)		((void)0)
#define RESETLASTERROR()	((void)0)
#endif

void Trace(LPCTSTR form, ...);

#ifndef TRACE
#ifdef _DEBUG
#define TRACE		HMC::Trace
#else
#define TRACE		1 ? ((void)0) : HMC::Trace
#endif
#endif

#ifdef _DEBUG
#define SUGGEST(exp)	do{if(!(exp))HMC::Trace("File %s, Line %u, Exp %s\n",__FILE__,__LINE__,#exp);}while(0)
#else
#define SUGGEST(exp)	((void)0)
#endif

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(exp)	BREAK(exp,#exp)
#else
#define ASSERT(exp)	((void)0)
#endif
#endif

#ifndef ASSUME
#ifdef _DEBUG
#define ASSUME(exp)	BREAK(exp,#exp)
#else
#define ASSUME(exp)	(__assume(exp))
#endif
#endif

#ifndef VERIFY
#ifdef _DEBUG
#define VERIFY(exp)	BREAK(exp,#exp)
#else
#define VERIFY(exp)	((void)exp)			// ASSERTと違いexpは常に評価される
#endif
#endif

/*****************************************************************
	その他マクロ(※引数の再評価に注意)
*****************************************************************/
// 任意境界アライメント
//#define ALIGN(size,align)	(((size)+(align)-1)/(align)*(align))
//#define ROUND(size,align)	((size)/(align)*(align))
//#define PADDING(size,align)	(((size)+(align))/(align)*(align)-1)
#define ALIGN(size,align)	(((size)+((align)-1))&(~((align)-1)))
#define ROUND(size,align)	((size)&(~((align)-1)))
#define PADDING(size,align)	((size)|((align)-1))

// WORD 境界アライメント
#define ALIGNWORD(size)		(((size)+1)&~1)
#define ROUNDWORD(size)		((size)&~1)
#define PADDINGWORD(size)	((size)|1)

// LONG 境界アライメント
#define ALIGNLONG(size)		(((size)+3)&~3)
#define ROUNDLONG(size)		((size)&~3)
#define PADDINGLONG(size)	((size)|3)

// LONG64 境界アライメント
#define ALIGNLONG64(size)	(((size)+7)&~7)
#define ROUNDLONG64(size)	((size)&~7)
#define PADDINGLONG64(size)	((size)|7)

// 任意境界巻き込み(※WINDは0以上a未満、ROLLは1以上a以下)
#define WIND(s,a)	((s)<0?(s)+((-(s)/(a)+1)*(a)):(s)+((-(s)/(a))*(a)))	// 0 <= val <  align
#define ROLL(s,a)	((s)<0?(s)+((-(s)/(a)+1)*(a)):(s)%(a)?(s)%(a):(a))	// 0 <  val <= align

// 剰余
#define MOD(a,b)	((a)%(max(1,(b))))
//#define MOD(a,b)	((a)-(((a)/(max(1,(b))))*(b)))

// a～bの範囲の数値vをx～yの範囲に拡張する(符号、桁溢れに注意)
#define SCALE(v,a,b,x,y)	(((v)-(a))*((y)-(x))/((b)-(a))+(x))

// BITMAP 配列サイズ定式
#define BMSIZE(w,h,b)	(((((DWORD(w)*(b))+31)&~31)>>3)*(h))
#define BMPITCH(w,b)	((((DWORD(w)*(b))+31)&~31)>>3)

// 配列サイズ
#define ARRAYLEN(a)		(sizeof(a)/sizeof((a)[0]))

// シノニムの定義
#define DECLARE_SYNONYM(t,s)	typedef t s,*LP##s,**LPLP##s; typedef const t C##s,*LPC##s;

// 16,32ビット整数マクロ(windef.hより)
#if 0
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

// 8ビット整数マクロ
#define MAKEBYTE(l,h)	((BYTE)((l)&0x04)|(((BYTE)(h))<<4))
#define LONIBBLE(b)		((BYTE)((b)&0x04))
#define HINIBBLE(b)		((BYTE)(((BYTE)(b)>>4)&0xff))

// 64ビット整数マクロ(そのうちwindef.hとかに入りそうな気もするが……)
#define MAKELONG64(l,h)	((LONG64)(((DWORD)(l))|((ULONG64)((DWORD)(h)))<<32))
#define LOLONG(q)		((LONG)(q))
#define HILONG(q)		((LONG)(((ULONG64)(q)>>32)&0xffffffff))

/*****************************************************************
	その他テンプレート(ほとんどSTLそのままパクリ)
*****************************************************************/
// 同型オブジェクトの交換(operator=が必要)
template <class T> void swap(T& a, T& b) { T t=a; a=b; b=t; }

// auto_ptr
template<class T> class auto_ptr
{
public:
	DECLARE_SYNONYM(T,OBJ);
	DECLARE_SYNONYM(auto_ptr<OBJ>,TYPE);
public:
	explicit auto_ptr(LPOBJ p=0) : m_bOwner(p!=NULL), m_pObj(p) {}
	auto_ptr(CTYPE& t) : m_bOwner(t.m_bOnwer), m_pObj(t.release()) {}
	TYPE& operator=(CTYPE &t) {
		if(this!=&t) {
			if(m_pObj!=t.get()) {
				if(m_bOwner) {
					delete m_pObj;
				}
				m_bOwner = t.m_bOwner;
			} else if(t.m_bOwner) {
				m_bOwner = true;
				m_pObj = t.release();
			}
			return *this;
		}
	}
	~auto_ptr() { if(m_bOwner) { delete m_pObj; } }
	OBJ& operator*() const { return *get(); }
	LPOBJ operator->() const { return get(); }
	LPOBJ get() const { return m_pObj; }
	LPOBJ release() const { const_cast<LPTYPE>(this)->m_bOwner=false; return m_pObj; }
private:
	BOOL m_bOwner;
	LPOBJ m_pObj;
};

/*****************************************************************
	その他型定義
*****************************************************************/
// 一般的なプログレスコールバック関数型
typedef BOOL (CALLBACK *LPPROGRESSCALLBACK)(LONG lCurrent, LONG lMax, LPVOID pContext);

/*****************************************************************
	以下はご都合主義に雑多なクラス
*****************************************************************/
/*****************************************************************
	POINT表現クラス
*****************************************************************/
class CPoint : public POINT
{
public:
	DECLARE_SYNONYM(CPoint,TYPE);
	DECLARE_SYNONYM(POINT,BASE);
	operator LPTYPE() { return this; }
public:
	CPoint() {}
	CPoint(LONG x, LONG y) { let(x,y); }
	CPoint(POINT pt) { let(pt); }
	CPoint(SIZE sz) { let(sz); }
public:
	// 各種操作
	void let(LONG x, LONG y) { this->x=x, this->y=y; }
	void let(POINT pt) { let(pt.x,pt.y); }
	void let(SIZE sz) { let(sz.cx,sz.cy); }
	void offset(LONG x, LONG y) { this->x+=x,this->y+=y; }
	void offset(POINT pt) { offset(pt.x,pt.y); }
	void offset(SIZE sz) { offset(sz.cx,sz.cy); }
	bool isEqual(POINT pt) const { return x==pt.x && y==pt.y; }
	bool isEqual(LONG x, LONG y) const { return this->x==x && this->y==y; }
	// 各種オペレータ
	TYPE &operator=(POINT pt) { let(pt); return *this; }
	bool operator==(POINT pt) const { return isEqual(pt); }
	bool operator!=(POINT pt) const { return !isEqual(pt); }
	TYPE &operator+=(POINT pt) { offset(pt); return *this; }
	TYPE &operator+=(SIZE sz) { offset(sz); return *this; }
	TYPE &operator-=(POINT pt) { offset(-pt.x,-pt.y); return *this; }
	TYPE &operator-=(SIZE sz) { offset(-sz.cx,-sz.cy); return *this; }
	TYPE operator+(POINT pt) const { TYPE t=*this; t+=pt; return t; }
	TYPE operator-(POINT pt) const { TYPE t=*this; t-=pt; return t; }
	TYPE operator+(SIZE sz) const { TYPE t=*this; t+=sz; return t; }
	TYPE operator-(SIZE sz) const { TYPE t=*this; t-=sz; return t; }
	TYPE operator-() const { TYPE t(-x,-y); return t; }
};
/*****************************************************************
	SIZE表現クラス
*****************************************************************/
class CSize : public SIZE
{
public:
	DECLARE_SYNONYM(CSize,TYPE);
	DECLARE_SYNONYM(SIZE,BASE);
	operator LPTYPE() { return this; }
public:
	CSize() {}
	CSize(LONG w, LONG h) { let(w,h); }
	CSize(SIZE sz) { let(sz); }
	CSize(POINT pt) { let(pt); }
public:
	// 各種操作
	void let(LONG w, LONG h) { cx=w, cy=h; }
	void let(SIZE sz) { let(sz.cx,sz.cy); }
	void let(POINT pt) { let(pt.x,pt.y); }
	void offset(LONG w, LONG h) { cx+=w,cy+=h; }
	void offset(SIZE sz) { offset(sz.cx,sz.cy); }
	void offset(POINT pt) { offset(pt.x,pt.y); }
	bool isEqual(SIZE sz) const { return cx==sz.cx && cy==sz.cy; }
	bool isEqual(LONG w, LONG h) const { return cx==w && cy==h; }
	// 各種オペレータ
	TYPE &operator=(SIZE sz) { let(sz); return *this; }
	bool operator==(SIZE sz) const { return isEqual(sz); }
	bool operator!=(SIZE sz) const { return !isEqual(sz); }
	TYPE &operator+=(SIZE sz) { offset(sz); return *this; }
	TYPE &operator+=(POINT pt) { offset(pt); return *this; }
	TYPE &operator-=(SIZE sz) { offset(-sz.cx,-sz.cy); return *this; }
	TYPE &operator-=(POINT pt) { offset(-pt.x,-pt.y); return *this; }
	TYPE operator+(SIZE sz) const { TYPE t=*this; t+=sz; return t; }
	TYPE operator-(SIZE sz) const { TYPE t=*this; t-=sz; return t; }
	TYPE operator+(POINT pt) const { TYPE t=*this; t+=pt; return t; }
	TYPE operator-(POINT pt) const { TYPE t=*this; t-=pt; return t; }
	TYPE operator-() const { TYPE t(-cx,-cy); return t; }
};
/*****************************************************************
	RECT表現クラス
*****************************************************************/
class CRect : public RECT
{
public:
	DECLARE_SYNONYM(CRect,TYPE);
	DECLARE_SYNONYM(RECT,BASE);
	operator LPTYPE() { return this; }
public:
	CRect() {}
	CRect(LONG l, LONG t, LONG r, LONG b) { let(l,t,r,b); }
	CRect(const RECT *r) { let(r); }
	CRect(const RECT &r) { let(&r); }
	CRect(POINT pt, SIZE sz) { let(pt,sz); }
public:
	// 各種操作
	void let(LONG l, LONG t, LONG r, LONG b) { left=l,top=t,right=r,bottom=b; }
	void let(const RECT *r) { let(r->left,r->top,r->right,r->bottom); }
	void let(POINT pt, SIZE sz) { let(pt.x,pt.y,pt.x+sz.cx,pt.y+sz.cy); }
	void setEmpty() { let(0,0,0,0); }
	bool isInside(LONG x, LONG y) const { return left<=x&&x<right && top<=y&&y<bottom; }
	bool isInside(POINT pt) const { return isInside(pt.x,pt.y); }
	bool isInside(const RECT *r) const { return (left<=r->left&&r->left<right)&&(left<=r->right&&r->right<=right)&&(top<=r->top&&r->top<bottom)&&(top<=r->bottom&&r->bottom<=bottom); }
	//bool isIntersect(const RECT *r) const { return min(max(left,r->left),r->right)<min(right,r->right) && min(max(top,r->top),r->bottom)<min(bottom,r->bottom); }
	BOOL isIntersect(const RECT *r) const { TYPE tmp(this); return tmp.intersect(r); }
	bool isEmpty() const { return left>=right || top>=bottom; }
	bool isNull() const { return left==0 && right==0 && top==0 && bottom==0; }
	bool isEqual(const RECT *r) const { return left==r->left && top==r->top && right==r->right && bottom==r->bottom; }
	//void intersect(const RECT *r) { left=min(max(left,r->left),r->right); top=min(max(top,r->top),r->bottom); right=min(right,r->right); bottom=min(bottom,r->bottom); }
	BOOL intersect(const RECT *r) { return ::IntersectRect(this,this,r); }
	void intersect(const RECT *r, DWORD c) { for(DWORD i=0; i<c; i++) intersect(&r[i]); }
	//void unite(const RECT *r) { left=min(left,r->left); top=min(top,r->top); right=max(right,r->right); bottom=max(bottom,r->bottom); }
	BOOL unite(const RECT *r) { return ::UnionRect(this,this,r); }
	void unite(const RECT *r, DWORD c) { for(DWORD i=0; i<c; i++) unite(&r[i]); }
	void offset(LONG x, LONG y) { left+=x,right+=x,top+=y,bottom+=y; }
	void offset(POINT pt) { offset(pt.x,pt.y); }
	void offset(SIZE sz) { offset(sz.cx,sz.cy); }
	void inflate(LONG x, LONG y) { right+=x; bottom+=y; }
	void inflate(POINT pt) { right+=pt.x; bottom+=pt.y; }
	void inflate(SIZE sz) { right+=sz.cx; bottom+=sz.cy; }
	void deflate(LONG x, LONG y) { right-=x; bottom-=y; }
	void deflate(POINT pt) { right-=pt.x; bottom-=pt.y; }
	void deflate(SIZE sz) { right-=sz.cx; bottom-=sz.cy; }
	void validate() { if(!(left<right)) swap(left,right); if(!(bottom<top)) swap(bottom,top); }
public:
	// クラス情報の取得/設定
	LONG width () const { return right-left; }
	LONG height() const { return bottom-top; }
	LONG width (LONG w) { return right=left+w; }
	LONG height(LONG h) { return bottom=top+h; }
	LONG sx() const { return left;   }
	LONG sy() const { return top;    }
	LONG ex() const { return right;  }
	LONG ey() const { return bottom; }
	LONG sx(LONG x) { return left   = x; }
	LONG sy(LONG y) { return top    = y; }
	LONG ex(LONG x) { return right  = x; }
	LONG ey(LONG y) { return bottom = y; }
	SIZE size() const { SIZE sz={ width(), height() }; return sz; }
	SIZE size(LONG w, LONG h) { width(w),height(h); return size(); }
	bool isValid() const { return left<=right && top<=bottom; }
public:
	// 各種オペレータ
	TYPE &operator=(const RECT &rc) { let(&rc); return *this; }
	bool operator==(const RECT &rc) const { return isEqual(&rc); }
	bool operator!=(const RECT &rc) const { return !isEqual(&rc); }
	TYPE &operator+=(POINT pt) { offset(pt.x,pt.y); return *this; }
	TYPE &operator+=(SIZE sz) { inflate(sz); return *this; }
	TYPE &operator-=(POINT pt) { offset(-pt.x,-pt.y); return *this; }
	TYPE &operator-=(SIZE sz) { inflate(-sz.cx,-sz.cy); return *this; }
	TYPE &operator|=(const RECT &rc) { unite(&rc); return *this; }
	TYPE &operator&=(const RECT &rc) { intersect(&rc); return *this; }
	TYPE operator+(POINT pt) const { TYPE t=*this; t+=pt; return t; }
	TYPE operator+(SIZE sz) const { TYPE t=*this; t+=sz; return t; }
	TYPE operator-(POINT pt) const { TYPE t=*this; t-=pt; return t; }
	TYPE operator-(SIZE sz) const { TYPE t=*this; t-=sz; return t; }
	TYPE operator|(const RECT &rc) const { TYPE t=*this; t.unite(&rc); return t; }
	TYPE operator&(const RECT &rc) const { TYPE t=*this; t.intersect(&rc); return t; }
};

/*****************************************************************
	固定小数点数
*****************************************************************/
#ifdef _INC_MATH
class FDWORD : public FIXED
{
public:
	FDWORD() {}
	FDWORD(SHORT v, WORD f=0) {
		value = v;
		fract = f;
	}
	FDWORD(int i)    { *this = i; }
	FDWORD(double f) { *this = f; }
public:
	operator int() const {
		return value;
	}
	operator double() const {
		return double(value) + (double(1) * fract / 0x10000);
	}
	FDWORD &operator=(const int &t) {
		value = t;
		fract = 0;
		return *this;
	}
	FDWORD &operator=(const double &t) {
		value = SHORT(fabs(t));
		fract = WORD(fabs(fmod(t,1) * 0x10000));
		return *this;
	}
	FDWORD &operator=(const FDWORD &t) {
		(*((DWORD*)this))=(*((DWORD*)&t));
		return *this;
	}
	bool operator==(const FDWORD &t) const {
		return (*((DWORD*)this))==(*((DWORD*)&t));
	}
	FDWORD operator+(const FDWORD &t) const {
		DWORD d = (*((DWORD*)this))+(*((DWORD*)&t));
		return FDWORD(HIWORD(d),LOWORD(d));
	}
	FDWORD operator-(const FDWORD &t) const {
		DWORD d = (*((DWORD*)this))-(*((DWORD*)&t));
		return FDWORD(HIWORD(d),LOWORD(d));
	}
	FDWORD &operator+=(const FDWORD &t) {
		(*((DWORD*)this))+=(*((DWORD*)&t));
		return *this;
	}
	FDWORD &operator-=(const FDWORD &t) {
		(*((DWORD*)this))-=(*((DWORD*)&t));
		return *this;
	}
};
#endif

/*****************************************************************
	エンディアン反転 WORD,DWORD (T = Twisted の意)
*****************************************************************/
// 反転 WORD
class TWORD {
public:
	TWORD() {}
	TWORD(WORD w) { *this = w; }
public:
	operator WORD() const {
		return GetWordBe(&this->w);
	}
	WORD operator=(WORD w) {
		SetWordBe(&this->w, w);
		return w;
	}
private:
	WORD w;
};
// 反転 DWORD
class TDWORD {
public:
	TDWORD() {}
	TDWORD(DWORD dw) {
		*this = dw;
	}
public:
	operator DWORD() const {
		return GetLongBe(&this->dw);
	}
	DWORD operator=(DWORD dw) {
		SetLongBe(&this->dw, dw);
		return dw;
	}
private:
	DWORD dw;
};
// 反転 double
class TDOUBLE {
public:
	TDOUBLE() {}
	TDOUBLE(double dl) {
		*this = dl;
	}
public:
	operator double() const {
		double dl;
		((LPBYTE)&dl)[7] = ((LPBYTE)&dlValue)[0];
		((LPBYTE)&dl)[6] = ((LPBYTE)&dlValue)[1];
		((LPBYTE)&dl)[5] = ((LPBYTE)&dlValue)[2];
		((LPBYTE)&dl)[4] = ((LPBYTE)&dlValue)[3];
		((LPBYTE)&dl)[3] = ((LPBYTE)&dlValue)[4];
		((LPBYTE)&dl)[2] = ((LPBYTE)&dlValue)[5];
		((LPBYTE)&dl)[1] = ((LPBYTE)&dlValue)[6];
		((LPBYTE)&dl)[0] = ((LPBYTE)&dlValue)[7];
		return dl;
	}
	double operator=(double dl) {
		((LPBYTE)&dlValue)[0] = ((LPBYTE)&dl)[7];
		((LPBYTE)&dlValue)[1] = ((LPBYTE)&dl)[6];
		((LPBYTE)&dlValue)[2] = ((LPBYTE)&dl)[5];
		((LPBYTE)&dlValue)[3] = ((LPBYTE)&dl)[4];
		((LPBYTE)&dlValue)[4] = ((LPBYTE)&dl)[3];
		((LPBYTE)&dlValue)[5] = ((LPBYTE)&dl)[2];
		((LPBYTE)&dlValue)[6] = ((LPBYTE)&dl)[1];
		((LPBYTE)&dlValue)[7] = ((LPBYTE)&dl)[0];
		return double(*this);
	}
private:
	double dlValue;
};
/*****************************************************************
	一様擬似乱数配列
*****************************************************************/
template <class T> class TRandomArray
{
public:
	TRandomArray() { init(); }
	TRandomArray(size_t n) { init(); create(n); }
	virtual ~TRandomArray() { destroy(); }
private:
	void init() { cTbl=0; pTbl=NULL; }
public:
	void destroy() { delete[]pTbl; init(); }
public:
	void create(size_t len) {
		destroy();
		cTbl = len;
		pTbl = new T[count()];
	}
	void shuffle() {
		T last = nth(count()-1);
		for(ULONG64 i=0; i<ULONG64(count())*count(); i++) {
			size_t a = SCALE(::rand(),0,RAND_MAX,0,count()-1);
			size_t b = SCALE(::rand(),0,RAND_MAX,0,count()-1);
			swap(nth(a),nth(b));
		}
		if(nth(0)==last) {
			swap(nth(0),nth(count()-1));
		}
	}
public:
	size_t count() const { return cTbl; }
	T nth(size_t n) const { return pTbl[n]; }
	T &nth(size_t n) { return pTbl[n]; }
	//T operator[](size_t n) const { return nth(n); }
	//T &operator[](size_t n) { return nth(n); }
	T operator()(size_t n) const { return nth(n); }
	T &operator()(size_t n) { return nth(n); }
private:
	size_t cTbl;
	T *pTbl;
};
class CRandomIntArray : public TRandomArray<INT> {
public:
	DECLARE_SYNONYM(CRandomIntArray,TYPE);
	DECLARE_SYNONYM(INT,OBJ);
	DECLARE_SYNONYM(TRandomArray<OBJ>,BASE);
public:
	CRandomIntArray() {}
	CRandomIntArray(size_t n) { create(n); }
public:
	void create(size_t len) {
		BASE::create(len);
		for(size_t n=0; n<len; n++) {
			nth(n) = n;
		}
		//shuffle();	// 作成時はシャフルしない
	}
};
class CRandomUIntArray : public TRandomArray<UINT> {
public:
	DECLARE_SYNONYM(CRandomUIntArray,TYPE);
	DECLARE_SYNONYM(UINT,OBJ);
	DECLARE_SYNONYM(TRandomArray<OBJ>,BASE);
public:
	CRandomUIntArray() {}
	CRandomUIntArray(size_t n) { create(n); }
public:
	void create(size_t len) {
		BASE::create(len);
		for(size_t n=0; n<len; n++) {
			nth(n) = n;
		}
		//shuffle();	// 作成時はシャフルしない
	}
};
/*****************************************************************
	XYZ値……テンプレート引数に使おう->TRandomArray<XYZ>
*****************************************************************/
class XYZ
{
public:
	XYZ() { x=0; y=0; z=0; }
public:
	XYZ &operator =(const XYZ &t) { x=t.x; y=t.y; z=t.z; return *this; }
	bool operator==(const XYZ &t) const { return x==t.x && y==t.y && z==t.z; }
	bool operator!=(const XYZ &t) const { return !(*this==t); }
public:
	LONG x;
	LONG y;
	LONG z;
};
/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_UTIL__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
