/*
	時間情報表現クラス
	Copyright (C)1998-2001 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_TIME__
#define __INC_HMC_TIME__

#include "util.h"
#include <time.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
class CTimeZone;
class CFileTime;
class CSystemTime;
class CDosTime;
class CTickTime;
class CMMTime;
class CPerfCounter;
class CPerfTime;

/*****************************************************************
	グローバル
*****************************************************************/
void FileTimeToTime(CONST FILETIME *lpft, time_t *lptime);
void TimeToFileTime(const time_t *lptime, FILETIME *lpft);

/*****************************************************************
	DOS時間(FAT時間)
*****************************************************************/
struct DOSDATETIME
{
	WORD wDate;		// bit0-4:day(1-31), bit5-8:month(1-12), bit9-15:year(-1980)
	WORD wTime;		// bit0-4:sec(/2), bit5-10:min(0-59), bit11-15:hour(0-23)
};

/*****************************************************************
	タイムゾーン
*****************************************************************/
class CTimeZone : public TIME_ZONE_INFORMATION
{
public:
	DECLARE_SYNONYM(CTimeZone,TYPE);
	DECLARE_SYNONYM(TIME_ZONE_INFORMATION,BASE);
	operator LPTYPE() { return this; }
public:
	CTimeZone() { ::GetTimeZoneInformation(this); }
	CTimeZone(CONST LPTIME_ZONE_INFORMATION lptz) { memcpy(this,lptz,sizeof(*this)); }
public:
	// 各種操作
};

/*****************************************************************
	ファイル時間
*****************************************************************/
class CFileTime : public FILETIME
{
public:
	DECLARE_SYNONYM(CFileTime,TYPE);
	DECLARE_SYNONYM(FILETIME,BASE);
	operator LPTYPE() { return this; }
	operator ULONGLONG() const { return MAKELONG64(dwLowDateTime,dwHighDateTime); }
public:
	CFileTime() { adjust(); }
	CFileTime(const FILETIME &ft) { let(ft); }
	CFileTime(CONST FILETIME *lpft) { let(lpft); }
	CFileTime(CONST SYSTEMTIME *lpst) { let(lpst); }
	CFileTime(CONST DOSDATETIME *lpdt) { let(lpdt); }
	CFileTime(ULONGLONG ulTime) { let(ulTime); }
	CFileTime(DWORD dwLow, DWORD dwHigh) { let(dwLow,dwHigh); }
public:
	// 各種操作
	VOID adjust() { ::GetSystemTimeAsFileTime(this); }
	BOOL let(const FILETIME &ft) { *this=ft; return TRUE; }
	BOOL let(CONST FILETIME *lpft) { memcpy(this,lpft,sizeof(*this)); return TRUE; }
	BOOL let(CONST SYSTEMTIME *lpst) { BOOL b=::SystemTimeToFileTime(lpst,this); CHECKRESULT_API(b); return b; }
	BOOL let(CONST DOSDATETIME *lpdt) { BOOL b=::DosDateTimeToFileTime(lpdt->wDate,lpdt->wTime,this); CHECKRESULT_API(b); return b; }
	BOOL let(ULONGLONG ulTime) { return let(LOLONG(ulTime),HILONG(ulTime)); }
	BOOL let(DWORD dwLow, DWORD dwHigh) { dwLowDateTime=dwLow,dwHighDateTime=dwHigh; return TRUE; }
	LONG compare(FILETIME &ft) const { return ::CompareFileTime(this,&ft); }
	LONG compare(CONST FILETIME *lpft) const { return ::CompareFileTime(this,lpft); }
	LONG compare(CONST SYSTEMTIME *lpst) const { TYPE t(lpst); return compare(t); }
	LONG compare(CONST DOSDATETIME *lpdt) const { TYPE t(lpdt); return compare(t); }
	LONG compare(ULONGLONG ulTime) const { TYPE t(ulTime); return compare(t); }
	LONG compare(DWORD dwLow, DWORD dwHigh) const { TYPE t(dwLow,dwHigh); return compare(t); }
	TYPE elapse() const { TYPE t(((ULONGLONG)(*this))-((ULONGLONG)current())); return t; }
	TYPE current() const { TYPE t; return t; }
	TYPE remain(FILETIME &ft) const { TYPE a(elapse()),b(&ft); return (a<b)?TYPE(((ULONGLONG)b)-((ULONGLONG)a)):TYPE(0,0); }
	TYPE remain(const FILETIME *lpft) const { TYPE a(elapse()),b(lpft); return (a<b)?TYPE(((ULONGLONG)b)-((ULONGLONG)a)):TYPE(0,0); }
	TYPE remain(CONST SYSTEMTIME *lpst) const { TYPE t(lpst); return remain(t); }
	TYPE remain(CONST DOSDATETIME *lpdt) const { TYPE t(lpdt); return remain(t); }
	TYPE remain(ULONGLONG ulTime) const { TYPE t(ulTime); return remain(t); }
	TYPE remain(DWORD dwLow, DWORD dwHigh) const { TYPE t(dwLow,dwHigh); return remain(t); }
	// ユーティリティ
	BOOL FileTimeToLocalFileTime() { BOOL b=::FileTimeToLocalFileTime(this,this); CHECKRESULT_API(b); return b; }
	BOOL LocalFileTimeToFileTime() { BOOL b=::LocalFileTimeToFileTime(this,this); CHECKRESULT_API(b); return b; }
public:
	// 各種オペレータ
	TYPE &operator=(CONST FILETIME &ft) { let(&ft); return *this; }
	bool operator==(FILETIME &ft) const { return compare(ft)==0; }
	bool operator!=(FILETIME &ft) const { return compare(ft)!=0; }
	bool operator< (FILETIME &ft) const { return compare(ft)==-1; }
	bool operator> (FILETIME &ft) const { return compare(ft)==+1; }
	bool operator==(CONST FILETIME *lpft) const { return compare(lpft)==0; }
	bool operator!=(CONST FILETIME *lpft) const { return compare(lpft)!=0; }
	bool operator< (CONST FILETIME *lpft) const { return compare(lpft)==-1; }
	bool operator> (CONST FILETIME *lpft) const { return compare(lpft)==+1; }
	bool operator==(CONST SYSTEMTIME *lpst) const { return compare(lpst)==0; }
	bool operator!=(CONST SYSTEMTIME *lpst) const { return compare(lpst)!=0; }
	bool operator< (CONST SYSTEMTIME *lpst) const { return compare(lpst)==-1; }
	bool operator> (CONST SYSTEMTIME *lpst) const { return compare(lpst)==+1; }
	bool operator==(CONST DOSDATETIME *lpdt) const { return compare(lpdt)==0; }
	bool operator!=(CONST DOSDATETIME *lpdt) const { return compare(lpdt)!=0; }
	bool operator< (CONST DOSDATETIME *lpdt) const { return compare(lpdt)==-1; }
	bool operator> (CONST DOSDATETIME *lpdt) const { return compare(lpdt)==+1; }
	bool operator==(ULONGLONG ulTime) const { return compare(ulTime)==0; }
	bool operator!=(ULONGLONG ulTime) const { return compare(ulTime)!=0; }
	bool operator< (ULONGLONG ulTime) const { return compare(ulTime)==-1; }
	bool operator> (ULONGLONG ulTime) const { return compare(ulTime)==+1; }
};

/*****************************************************************
	システム時間
*****************************************************************/
class CSystemTime : public SYSTEMTIME
{
public:
	DECLARE_SYNONYM(CSystemTime,TYPE);
	DECLARE_SYNONYM(SYSTEMTIME,BASE);
	operator LPTYPE() { return this; }
public:
	CSystemTime() { adjust(); }
	CSystemTime(const SYSTEMTIME &st) { let(st); }
	CSystemTime(WORD year, WORD month, WORD week, WORD day, WORD hour, WORD min, WORD sec, WORD msec) { let(year,month,week,day,hour,min,sec,sec); }
	CSystemTime(CONST FILETIME *lpft) { let(lpft); }
	CSystemTime(CONST SYSTEMTIME *lpst) { let(lpst); }
	CSystemTime(CONST DOSDATETIME *lpdt) { let(lpdt); }
public:
	// 各種操作
	VOID adjust() { ::GetLocalTime(this); }
	BOOL let(const SYSTEMTIME &st) { *this=st; return TRUE; }
	BOOL let(WORD year, WORD month, WORD week, WORD day, WORD hour, WORD min, WORD sec, WORD msec) { wYear=year,wMonth=month,wDayOfWeek=week,wDay=day,wHour=hour,wMinute=min,wSecond=sec,wMilliseconds=sec; return TRUE; }
	BOOL let(CONST FILETIME *lpft) { BOOL b=::FileTimeToSystemTime(lpft,this); CHECKRESULT_API(b); return b; }
	BOOL let(CONST SYSTEMTIME *lpst) { memcpy(this,lpst,sizeof(*this)); return TRUE; }
	BOOL let(CONST DOSDATETIME *lpdt) { CFileTime ft(lpdt); return let(&ft); }
	LONG compare(WORD year, WORD month, WORD week, WORD day, WORD hour, WORD min, WORD sec, WORD msec) const { TYPE t(year,month,week,day,hour,min,sec,msec); CFileTime a(this),b(&t); return a.compare(b); }
	LONG compare(CONST FILETIME *lpft) const { CFileTime a(this); return a.compare(lpft); }
	LONG compare(CONST SYSTEMTIME *lpst) const { CFileTime a(this); return a.compare(lpst); }
	LONG compare(CONST DOSDATETIME *lpdt) const { CFileTime a(this); return a.compare(lpdt); }
	// ユーティリティ
	BOOL SystemTimeToTzSpecificLocalTime(LPTIME_ZONE_INFORMATION lpTimeZoneInformation) { BOOL b=::SystemTimeToTzSpecificLocalTime(lpTimeZoneInformation,this,this); CHECKRESULT_API(b); return b; }
public:
	// 各種オペレータ
	bool operator==(CONST FILETIME *lpft) const { return compare(lpft)==0; }
	bool operator!=(CONST FILETIME *lpft) const { return compare(lpft)!=0; }
	bool operator< (CONST FILETIME *lpft) const { return compare(lpft)==-1; }
	bool operator> (CONST FILETIME *lpft) const { return compare(lpft)==+1; }
	bool operator==(CONST SYSTEMTIME *lpst) const { return compare(lpst)==0; }
	bool operator!=(CONST SYSTEMTIME *lpst) const { return compare(lpst)!=0; }
	bool operator< (CONST SYSTEMTIME *lpst) const { return compare(lpst)==-1; }
	bool operator> (CONST SYSTEMTIME *lpst) const { return compare(lpst)==+1; }
	bool operator==(CONST DOSDATETIME *lpdt) const { return compare(lpdt)==0; }
	bool operator!=(CONST DOSDATETIME *lpdt) const { return compare(lpdt)!=0; }
	bool operator< (CONST DOSDATETIME *lpdt) const { return compare(lpdt)==-1; }
	bool operator> (CONST DOSDATETIME *lpdt) const { return compare(lpdt)==+1; }
};

/*****************************************************************
	DOS時間
*****************************************************************/
class CDosTime : public DOSDATETIME
{
public:
	DECLARE_SYNONYM(CDosTime,TYPE);
	DECLARE_SYNONYM(DOSDATETIME,BASE);
	operator LPTYPE() { return this; }
	operator DWORD() const { return MAKELONG(wDate,wTime); }
public:
	CDosTime() { adjust(); }
	CDosTime(WORD year, WORD month, WORD day, WORD hour, WORD min, WORD sec) { let(year,month,day,hour,min,sec); }
	CDosTime(CONST FILETIME *lpft) { let(lpft); }
	CDosTime(CONST SYSTEMTIME *lpst) { let(lpst); }
	CDosTime(CONST DOSDATETIME *lpdt) { let(lpdt); }
public:
	// 各種操作
	BOOL adjust() { SYSTEMTIME st; ::GetLocalTime(&st); return let(&st); }
	BOOL let(WORD year, WORD month, WORD day, WORD hour, WORD min, WORD sec) { wDate=((year-1980)<<7)|((month<<5)&0x1e0)|((day)&0x1f); wTime=(hour<<11)|((min<<5)&0x7e0)|((sec/2)&0x1f); return TRUE; }
	BOOL let(CONST FILETIME *lpft) { BOOL b=::FileTimeToDosDateTime(lpft,&wDate,&wTime); CHECKRESULT_API(b); return b; }
	BOOL let(CONST SYSTEMTIME *lpst) { CFileTime ft(lpst); return let(&ft); }
	BOOL let(CONST DOSDATETIME *lpdt) { CFileTime ft(lpdt); return let(&ft); }
	LONG compare(WORD year, WORD month, WORD day, WORD hour, WORD min, WORD sec) const { TYPE t(year,month,day,hour,min,sec); CFileTime a(this),b(&t); return a.compare(b); }
	LONG compare(CONST FILETIME *lpft) const { CFileTime a(this); return a.compare(lpft); }
	LONG compare(CONST SYSTEMTIME *lpst) const { CFileTime a(this); return a.compare(lpst); }
	LONG compare(CONST DOSDATETIME *lpdt) const { CFileTime a(this); return a.compare(lpdt); }
public:
	// 各種オペレータ
	bool operator==(CONST FILETIME *lpft) const { return compare(lpft)==0; }
	bool operator!=(CONST FILETIME *lpft) const { return compare(lpft)!=0; }
	bool operator< (CONST FILETIME *lpft) const { return compare(lpft)==-1; }
	bool operator> (CONST FILETIME *lpft) const { return compare(lpft)==+1; }
	bool operator==(CONST SYSTEMTIME *lpst) const { return compare(lpst)==0; }
	bool operator!=(CONST SYSTEMTIME *lpst) const { return compare(lpst)!=0; }
	bool operator< (CONST SYSTEMTIME *lpst) const { return compare(lpst)==-1; }
	bool operator> (CONST SYSTEMTIME *lpst) const { return compare(lpst)==+1; }
	bool operator==(CONST DOSDATETIME *lpdt) const { return compare(lpdt)==0; }
	bool operator!=(CONST DOSDATETIME *lpdt) const { return compare(lpdt)!=0; }
	bool operator< (CONST DOSDATETIME *lpdt) const { return compare(lpdt)==-1; }
	bool operator> (CONST DOSDATETIME *lpdt) const { return compare(lpdt)==+1; }
};

/*****************************************************************
	チックタイム
*****************************************************************/
class CTickTime
{
public:
	DECLARE_SYNONYM(CTickTime,TYPE);
	operator LPTYPE() { return this; }
	operator DWORD() const { return dwTick; }
public:
	CTickTime() { adjust(); }
	CTickTime(DWORD dw) { let(dw); }
public:
	// クラス情報の取得/設定
	DWORD adjust() { return let(current()); }
	DWORD let(DWORD dw) { return dwTick=dw; }
	DWORD last() const { return dwTick; }
	DWORD elapse() const { return current()-last(); }
	static DWORD current() { return ::GetTickCount(); }
	DWORD remain(DWORD dw) const { DWORD n=dw-elapse(); return dw<n?0:n; }
private:
	DWORD dwTick;
};

/*****************************************************************
	マルチメディアタイム
*****************************************************************/
// ※NT系の場合デフォルトではCTickTimeと精度差がほとんど無い
// CMMTimeを有効に利用するにはtimeBeginPeriod()で精度を指定する必要がある
class CMMTime
{
public:
	DECLARE_SYNONYM(CMMTime,TYPE);
	operator LPTYPE() { return this; }
	operator DWORD() const { return dwTick; }
public:
	CMMTime() { adjust(); }
	CMMTime(DWORD dw) { let(dw); }
public:
	// クラス情報の取得/設定
	DWORD adjust() { return let(current()); }
	DWORD let(DWORD dw) { return dwTick=dw; }
	DWORD last() const { return dwTick; }
	DWORD elapse() const { return current()-last(); }
	static DWORD current() { return ::timeGetTime(); }
	DWORD remain(DWORD dw) const { DWORD n=dw-elapse(); return dw<n?0:n; }
private:
	DWORD dwTick;
};

/*****************************************************************
	パフォーマンスカウンター
*****************************************************************/
// ※PerfCounterが利用できない場合はCMMTimeで代用
class CPerfCounter
{
public:
	DECLARE_SYNONYM(CPerfCounter,TYPE);
	operator LPTYPE() { return this; }
	operator LONGLONG() const { return nCounter; }
public:
	CPerfCounter() { adjust(); }
	CPerfCounter(LONGLONG n) { let(n); }
public:
	// クラス情報の取得/設定
	LONGLONG adjust() { return let(current()); }
	LONGLONG let(LONGLONG n) { return nCounter=n; }
	LONGLONG last() const { return nCounter; }
	LONGLONG elapse() const { return max(0,current()-last()); }
	static LONGLONG current() { LARGE_INTEGER l; BOOL b=::QueryPerformanceCounter(&l); CHECKRESULT_API(b); return b ? l.QuadPart : (LONGLONG)CMMTime::current(); }
	static LONGLONG freq() { LARGE_INTEGER l; BOOL b=::QueryPerformanceFrequency(&l); CHECKRESULT_API(b); return b ? l.QuadPart : (LONGLONG)1000; }
	LONGLONG remain(LONGLONG n) const { LONGLONG m=elapse(); return (m<n)?(n-m):0; }
private:
	LONGLONG nCounter;
};
// μsec精度換算のPerfCounter
class CPerfTime : public CPerfCounter
{
public:
	DECLARE_SYNONYM(CPerfTime,TYPE);
	DECLARE_SYNONYM(CPerfCounter,BASE);
	operator LPTYPE() { return this; }
	operator LONGLONG() const { return nCounter; }
public:
	CPerfTime() { adjust(); }
	CPerfTime(LONGLONG n) { let(n); }
public:
	// クラス情報の取得/設定
	LONGLONG adjust() { return let(current()); }
	LONGLONG let(LONGLONG n) { return nCounter=n; }
	LONGLONG last() const { return nCounter; }
	LONGLONG elapse() const { return max(0,current()-last()); }
	static LONGLONG current() { return (LONGLONG)(float(BASE::current()) / BASE::freq() * freq()); }
	static LONGLONG freq() { return 1000*1000; }	// μsec精度換算
	LONGLONG remain(LONGLONG n) const { LONGLONG m=elapse(); return (m<n)?(n-m):0; }
private:
	LONGLONG nCounter;
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_TIME__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
