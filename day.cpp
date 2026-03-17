
//#include "stdafx.h"
#include "pch.h"
#include "day.h"


// 現地時刻と世界時刻(UTC)の差を保存
volatile __time64_t SDate::m_tm64World = -1;


//=======================================================================================
// 日付
//=======================================================================================
#define ONE_DAY_TIME   86400
#define MAX_DATE       47480	// (2099.12.31.) / 86400

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="nNum">日付オブジェクト値</param>
SDate::SDate(const int nNum)
{
	if (m_tm64World == -1) {
		m_tm64World  =  0;  // スレッド時の再突入防止

		struct tm *ptmGMT;
		struct tm  dmyGMT;

		CTime tmNow = CTime::GetCurrentTime();
		ptmGMT = tmNow.GetGmtTm(&dmyGMT);
		if (ptmGMT != NULL) {
			CTime tmGMT(
				ptmGMT->tm_year + 1900,
				ptmGMT->tm_mon  + 1,
				ptmGMT->tm_mday,
				ptmGMT->tm_hour,
				ptmGMT->tm_min,
				ptmGMT->tm_sec);

			m_tm64World = tmNow.GetTime() - tmGMT.GetTime();
		}
	}

	Set(nNum);
}


/// <summary>
/// 年月日を「2000/00/00」にリセット
/// </summary>
void SDate::Clear()
{
	m_yy = 2000;
	m_mm = 0;
	m_dd = 0;
}


/// <summary>
/// 現在の日付に設定する
/// </summary>
/// <returns>日付オブジェクト値</returns>
int SDate::Now()
{
	CTime tm = CTime::GetCurrentTime();

	m_yy = tm.GetYear();
	m_mm = tm.GetMonth();
	m_dd = tm.GetDay();

	return Get();
}


/// <summary>
/// 日付オブジェクト値を取得します。
/// </summary>
/// <returns>日付オブジェクトの値を返す</returns>
int SDate::Get()
{
	if (IsFault()) { return 0; }	

	CTime tm(m_yy, m_mm, m_dd, 0, 0, 0);

	return (long)((tm.GetTime() + m_tm64World) / ONE_DAY_TIME);
}


/// <summary>
/// 日付を返す
/// </summary>
/// <returns>年月日(YYYYMMDD)</returns>
int SDate::GetYMD()
{
	int nVal = 0;

	nVal  = m_yy;
	nVal *= 100;
	nVal += m_mm;
	nVal *= 100;
	nVal += m_dd;

	return nVal;
}


/// <summary>
/// 日付を設定する
/// </summary>
/// <param name="nNum">日付オブジェクト値</param>
void SDate::Set(const int nNum)
{
	if (nNum <= 0)        { Clear(); return; }
	if (nNum >  MAX_DATE) { return; } 

	__time64_t tm64Tim = (__time64_t)nNum * ONE_DAY_TIME - m_tm64World;

	CTime tm(tm64Tim);

	m_dd = tm.GetDay();
	m_mm = tm.GetMonth();
	m_yy = tm.GetYear();
}


/// <summary>
/// 日付を設定する
/// </summary>
/// <param name="nNum">年月日(YYYYMMDD)</param>
void SDate::SetYMD(const int nNum)
{
	int nDate = nNum;

	m_dd   = nDate % 100;
	nDate /= 100;
	m_mm   = nDate % 100;
	nDate /= 100;
	m_yy   = nDate % 100;
}


/// <summary>
/// 日付が正しいか？
/// </summary>
/// <returns>正常:FALSE/異常:TRUE</returns>
BOOL SDate::IsFault()
{
	if (m_yy < 2000 || m_yy > 2099) { return TRUE; }
	if (m_mm < 1    || m_mm > 12  ) { return TRUE; }
	int nDay = GetDays(m_yy, m_mm);
	if (m_dd < 1    || m_dd > nDay) { return TRUE; }

	return FALSE;
}


/// <summary>
/// 日付オブジェクト値の日数を返す。
/// </summary>
/// <returns>日数</returns>
int SDate::GetDays()
{
	return GetDays(m_yy, m_mm);
}


/// <summary>
/// 指定した年と月の日数を取得します。
/// </summary>
/// <param name="yy">年</param>
/// <param name="mm">月</param>
/// <returns>指定した年と月の日数を返します。</returns>
int SDate::GetDays(const int yy, const int mm)
{
	const static int dtbl[13] = {29,31,28,31,30,31,30,31,31,30,31,30,31};
		/* 各月の日数（dtbl[0]は、閏年の２月の日数） */
	int n = mm;

	if (n == 2 && IsLeap(yy)) {
		n  = 0;
	}
	return dtbl[n];
}


/// <summary>
/// 閏年？
/// </summary>
/// <returns>閏年:TRUE/違う:FALSE</returns>
BOOL SDate::IsLeap()
{
	return IsLeap(m_yy);
}


/// <summary>
/// 閏年？
/// </summary>
/// <param name="yy">年</param>
/// <returns>閏年:TRUE/違う:FALSE</returns>
BOOL SDate::IsLeap(const int yy)
{
	if ((yy%4 == 0 && yy%100 != 0) || yy%400 == 0) { return TRUE; }

	return FALSE;
}


/// <summary>
/// 日付の曜日を返す
/// </summary>
/// <returns>曜日(不正:WEEK_NONE)</returns>
int SDate::GetWeek()
{
	if (IsFault()) { return WEEK_NONE; }

	return GetWeek(m_yy, m_mm, m_dd);
}


/// <summary>
/// 指定された日付の曜日を返す
/// </summary>
/// <param name="yy">年</param>
/// <param name="mm">月</param>
/// <param name="dd">日</param>
/// <returns>曜日(不正:WEEK_NONE)</returns>
int SDate::GetWeek(const int yy, const int mm, const int dd)
{
	int nVal = WEEK_NONE;

	SDate dt;

	dt.m_yy = yy;
	dt.m_mm = mm;
	dt.m_dd = dd;

	if (dt.IsFault()) { return nVal; }

	CTime tm(yy, mm, dd, 0, 0, 0);

	nVal = tm.GetDayOfWeek();

	return nVal;
}


//---------------------------------------------------------------------------------------
// operator
//---------------------------------------------------------------------------------------
void SDate::operator  = (const int nNum)
{
	Set(nNum);
}

void SDate::operator -= (const int nNum)
{
	if (nNum == 0) { return; }

	__time64_t tm64Tim;
	
	CTime tm1(m_yy, m_mm, m_dd, 0, 0, 0);
	
	tm64Tim  = tm1.GetTime();
	tm64Tim -= (__time64_t)nNum * ONE_DAY_TIME;

	CTime tm2(tm64Tim);

	m_yy = tm2.GetYear();
	m_mm = tm2.GetMonth();
	m_dd = tm2.GetDay();
}


void SDate::operator += (const int nNum)
{
	if (nNum == 0) { return; }

	__time64_t tm64Tim;
	
	CTime tm1(m_yy, m_mm, m_dd, 0, 0, 0);
	
	tm64Tim  = tm1.GetTime();
	tm64Tim += (__time64_t)nNum * ONE_DAY_TIME;

	CTime tm2(tm64Tim);

	m_yy = tm2.GetYear();
	m_mm = tm2.GetMonth();
	m_dd = tm2.GetDay();
}


// 前置
SDate SDate::operator ++ ()
{
	(*this) += 1;

	return *this;
}

SDate SDate::operator -- ()
{
	(*this) -= 1;

	return *this;
}

// 後置
SDate SDate::operator ++ (int)
{
	SDate tmp = *this;

	(*this) += 1;

	return tmp;
}

SDate SDate::operator -- (int)
{
	SDate tmp = *this;

	(*this) -= 1;

	return tmp;
}

BOOL SDate::operator != (SDate &p)
{
	return (Get() != p.Get());
}

BOOL SDate::operator == (SDate &p)
{
	return (Get() == p.Get());
}

BOOL SDate::operator >= (SDate &p)
{
	return (Get() >= p.Get());
}

BOOL SDate::operator <= (SDate &p)
{
	return (Get() <= p.Get());
}

BOOL SDate::operator  < (SDate &p)
{
	return (Get() < p.Get());
}

BOOL SDate::operator  > (SDate &p)
{
	return (Get() > p.Get());
}

SDate::operator int()
{
	return Get();
}


//=======================================================================================
// 時間 
//=======================================================================================

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="nTim">秒</param>
STime::STime(const int nTim)
{
	m_bCarry = FALSE;
	
	SetSS(nTim);
}


/// <summary>
/// 時刻を「0」に設定する
/// </summary>
void STime::Clear()
{
	m_bCarry = FALSE;

	m_hh = 0;
	m_mm = 0;
	m_ss = 0;
}


/// <summary>
/// 現在時刻をセットする
/// </summary>
/// <returns>秒</returns>
int STime::Now()
{
	CTime tm = CTime::GetCurrentTime();

	m_hh = tm.GetHour();
	m_mm = tm.GetMinute();
	m_ss = tm.GetSecond();

	return GetSS();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/// <summary>
/// 時刻を返す
/// </summary>
/// <returns>時分</returns>
int STime::GetHM()
{
	int nNum;

	nNum  = m_hh;
	nNum *= 100;
	nNum += m_mm;

	return nNum;
}


/// <summary>
/// 時刻を返す
/// </summary>
/// <returns>時分秒</returns>
int STime::GetHMS()
{
	int nNum;

	nNum  = m_hh;
	nNum *= 100;
	nNum += m_mm;
	nNum *= 100;
	nNum += m_ss;

	return nNum;
}


/// <summary>
/// 時刻を返す
/// </summary>
/// <returns>分(0～1439or2879)</returns>
int STime::GetMM()
{
	int nNum;

	nNum =  m_hh;
	nNum *= 60;
	nNum += m_mm;

	return nNum;
}


/// <summary>
/// 時刻を返す
/// </summary>
/// <returns>秒(0～86399or172799)</returns>
int STime::GetSS()
{
	int nNum;

	nNum =  m_hh;
	nNum *= 60;
	nNum += m_mm;
	nNum *= 60;
	nNum += m_ss;

	return nNum;
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/// <summary>
/// 時刻の設定
/// </summary>
/// <param name="nNum">時分秒</param>
/// <returns>正常:FALSE/異常:TRUE</returns>
BOOL STime::SetHMS(const int nNum)
{
	BOOL bRet = TRUE;

	if (nNum < 0) { Clear(); return bRet; }

	int nTime = nNum;

	m_ss   = nTime % 100;
	nTime /= 100;
	m_mm   = nTime % 100;
	nTime /= 100;
	m_hh   = nTime % 100;

	if (TRUE == (bRet = IsFault())) { Clear(); }

	return bRet;
}


/// <summary>
/// 時刻の設定
/// </summary>
/// <param name="nNum">時分</param>
/// <returns>正常:FALSE/異常:TRUE</returns>
BOOL STime::SetHM(const int nNum)
{
	BOOL bRet = TRUE;

	if (nNum < 0) { Clear(); return bRet; }

	long nTime = nNum;

	m_ss   = 0;
	m_mm   = nTime % 100;
	nTime /= 100;
	m_hh   = nTime % 100;

	if (TRUE == (bRet = IsFault())) { Clear(); }

	return bRet;
}


/// <summary>
/// 時刻の設定
/// </summary>
/// <param name="nNum">分</param>
/// <returns>正常:FALSE/異常:TRUE</returns>
BOOL STime::SetMM(const int nNum)
{
	BOOL bRet = TRUE;

	if (nNum < 0) { Clear(); return bRet; }

	m_hh = nNum / 60;
	m_mm = nNum % 60;
	m_ss = 0;

	if (TRUE == (bRet = IsFault())) { Clear(); }

	return bRet;
}


/// <summary>
/// 時刻の設定
/// </summary>
/// <param name="nNum">秒</param>
/// <returns>正常:FALSE/異常:TRUE</returns>
BOOL STime::SetSS(const int nNum)
{
	if (nNum <= 0) { Clear(); return TRUE; }

	BOOL bRet  = FALSE;
	int  nTime = nNum;

	while (nTime > ONE_DAY_TIME) {
		nTime -= ONE_DAY_TIME;
	}
	m_ss   = nTime % 60;
	nTime /= 60;
	m_mm   = nTime % 60;
	nTime /= 60;
	m_hh   = nTime % 60;

	if (TRUE == (bRet = IsFault())) { Clear(); }

	return bRet;
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/// <summary>
/// 時刻が正しいか？
/// </summary>
/// <param name="bNotCheckHour">1日を48時間とする(Default:FALSE)</param>
/// <returns>正常:FALSE/異常:TRUE</returns>
BOOL STime::IsFault(const BOOL bNotCheckHour)
{
	if (!bNotCheckHour) {
		if (m_hh < 0 || m_hh > 23) { return TRUE; }
	} else {
		if (m_hh < 0 || m_hh > 47) { return TRUE; }
	}
	if (m_mm < 0 || m_mm > 59) { return TRUE; }
	if (m_ss < 0 || m_ss > 59) { return TRUE; }

	return FALSE;
}


//---------------------------------------------------------------------------------------
// operator
//---------------------------------------------------------------------------------------
int STime::operator  = (const int nNum)
{
	return SetSS(nNum);
}

int STime::operator -= (const int nNum)
{
	m_bCarry = FALSE;

	if (nNum == 0) { return GetSS(); }

	int nTim = GetSS();

	if (nTim >= nNum) {
		nTim -= nNum;
	} else {
		m_bCarry = TRUE;
		do {
			nTim += ONE_DAY_TIME;
		} while (nNum > nTim);
		nTim -= nNum;
	}
	return SetSS(nTim);
}

int STime::operator += (const int nNum)
{
	m_bCarry = FALSE;

	if (nNum == 0) { return GetSS(); }

	int nTim = GetSS() + nNum;

	while (nTim > ONE_DAY_TIME) {
		nTim -= ONE_DAY_TIME;
		m_bCarry = TRUE;
	}

	return SetSS(nTim);
}

// 前置
int STime::operator ++ ()
{
	(*this) += 1;

	return *this;
}

int STime::operator -- ()
{
	(*this) -= 1;

	return *this;
}

// 後置
int STime::operator ++ (int)
{
	int nVal = *this;

	(*this) += 1;

	return nVal;
}

int STime::operator -- (int)
{
	int nVal = *this;

	(*this) -= 1;

	return nVal;
}

BOOL STime::operator != (STime &p)
{
	return (GetSS() != p.GetSS());
}

BOOL STime::operator == (STime &p)
{
	return (GetSS() == p.GetSS());
}

BOOL STime::operator >= (STime &p)
{
	return (GetSS() >= p.GetSS());
}

BOOL STime::operator <= (STime &p)
{
	return (GetSS() <= p.GetSS());
}


BOOL STime::operator  < (STime &p)
{
	return (GetSS() < p.GetSS());
}

BOOL STime::operator  > (STime &p)
{
	return (GetSS() > p.GetSS());
}

STime::operator int()
{
	return GetSS();
}


//=======================================================================================
// 開始処理
//=======================================================================================

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="bNow">現在日時で初期化する(初期値TRUE)</param>
/// <param name="bIgnoreSecond">operatorの比較時に“秒”を無視する(初期値:FALSE)</param>
CDay::CDay(const BOOL bNow, const BOOL bIgnoreSecond)
{
	if (bNow) {
		m_Date.Now();	// Default
		m_Time.Now();
	} else {
		m_Date.Clear();
		m_Time.Clear();
	}

	m_bIgnoreSecond = bIgnoreSecond;
}


/// <summary>
/// CDayオブジェクトの日付と時刻情報をクリアします。
/// </summary>
void CDay::Clear()
{
	m_Date.Clear();
	m_Time.Clear();
}


/// <summary>
/// 現在の日付と時刻を設定します。
/// </summary>
void CDay::Now()
{
	m_Date.Now();
	m_Time.Now();
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/// <summary>
/// 年を返す。
/// </summary>
/// <returns>年</returns>
int CDay::GetYear()
{
	return m_Date.m_yy;
}


/// <summary>
/// 月を返す
/// </summary>
/// <returns>月</returns>
int CDay::GetMonth()
{
	return m_Date.m_mm;
}


/// <summary>
/// 日を返す
/// </summary>
/// <returns>日</returns>
int CDay::GetDay()
{
	return m_Date.m_dd;
}


/// <summary>
/// 曜日を返します
/// </summary>
/// <returns>曜日</returns>
int CDay::GetWeek()
{
	return m_Date.GetWeek();
}


/// <summary>
/// 日付を返します
/// </summary>
/// <returns>年月日(YYYYMMDD)</returns>
int CDay::GetDateYMD()
{
	long lNum;

	lNum  = GetYear();
	lNum *= 100;
	lNum += GetMonth();
	lNum *= 100;
	lNum += GetDay();

	return lNum;
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/// <summary>
/// 時刻を返します
/// </summary>
/// <returns>時(00～23or47)</returns>
int CDay::GetHour()
{
	return m_Time.m_hh;
}

/// <summary>
/// 時刻を返します
/// </summary>
/// <returns>分(00～59)</returns>
int CDay::GetMin()
{
	return m_Time.m_mm;
}

/// <summary>
/// 時刻を返します
/// </summary>
/// <returns>秒(00～59)</returns>
int CDay::GetSec()
{
	return m_Time.m_ss;
}

/// <summary>
/// 時刻を返します
/// </summary>
/// <returns>時分</returns>
int CDay::GetTimeHM()
{
	return m_Time.GetHM();
}

/// <summary>
/// 時刻を返します
/// </summary>
/// <returns>時分秒</returns>
int CDay::GetTimeHMS()
{
	return m_Time.GetHMS();
}


/// <summary>
/// 日時オブジェクトを返します
/// </summary>
/// <returns>日時オブジェクト</returns>
__time64_t CDay::GetTimet()
{
	SDate ymd;
	STime hms;

	ymd = m_Date;
	hms = m_Time;
	while (hms.m_hh >= 24) {
		hms.m_hh -= 24;
		ymd++;
	}

	CTime tm(
		ymd.m_yy,
		ymd.m_mm,
		ymd.m_dd,
		hms.m_hh,
		hms.m_mm,
		hms.m_ss);

	return tm.GetTime();
}


/// <summary>
/// グリニッジ・日時オブジェクトを返します
/// </summary>
/// <returns>日時オブジェクト</returns>
__time64_t CDay::GetGmtTm()
{
	SDate ymd;
	STime hms;

	ymd = m_Date;
	hms = m_Time;
	while (hms.m_hh >= 24) {
		hms.m_hh -= 24;
		ymd++;
	}

	CTime tm(
		ymd.m_yy,
		ymd.m_mm,
		ymd.m_dd,
		hms.m_hh,
		hms.m_mm,
		hms.m_ss);

	struct tm *ptmGMT;
	struct tm  dmyGMT;

	ptmGMT = tm.GetGmtTm(&dmyGMT);
	if (ptmGMT != NULL) {
		CTime tmGMT(
			ptmGMT->tm_year + 1900,
			ptmGMT->tm_mon  + 1,
			ptmGMT->tm_mday,
			ptmGMT->tm_hour,
			ptmGMT->tm_min,
			ptmGMT->tm_sec);

		return tmGMT.GetTime();
	}
	
	return 0;
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/// <summary>
/// 日時を設定する
/// </summary>
/// <param name="nYear">年</param>
/// <param name="nMonth">月</param>
/// <param name="nDay">日</param>
/// <param name="nHour">時(Default:0)</param>
/// <param name="nMin">分(Default:0)</param>
/// <param name="nSec">秒(Default:0)</param>
/// <returns>不正:TRUE/成功:FALSE</returns>
BOOL CDay::SetDate(const int nYear, const int nMonth, const int nDay, const int nHour, const int nMin, const int nSec)
{
	m_Date.m_yy = nYear;
	m_Date.m_mm = nMonth;
	m_Date.m_dd = nDay;
	m_Time.m_hh = nHour;
	m_Time.m_mm = nMin;
	m_Time.m_ss = nSec;

	if (m_Date.IsFault()) { return TRUE; }
	if (m_Time.IsFault()) { return TRUE; }

	return FALSE;
}


/// <summary>
/// 時刻の時を設定します。
/// </summary>
/// <param name="nNum">時</param>
void CDay::SetHour(const int nNum)
{
	if (0 <= nNum && nNum < 24) {
		m_Time.m_hh = nNum;
	} else {
		m_Time.m_hh = 0;
	}
}


/// <summary>
/// 時刻の分を設定します。
/// </summary>
/// <param name="nNum">分</param>
void CDay::SetMin(const int nNum)
{
	if (0 <= nNum && nNum < 60) {
		m_Time.m_mm = nNum;
	} else {
		m_Time.m_mm = 0;
	}
}


/// <summary>
/// 時刻の秒を設定します。
/// </summary>
/// <param name="nNum">秒</param>
void CDay::SetSec(const int nNum)
{
	if (nNum <= 0 && nNum < 60) {
		m_Time.m_ss = nNum;
	} else {
		m_Time.m_ss = 0;
	}
}


/// <summary>
/// 時刻を設定します
/// </summary>
/// <param name="nNum">時分</param>
void CDay::SetTimeHM(const int nNum)
{
	m_Time.SetHM(nNum);
}


/// <summary>
/// 時刻を設定します。
/// </summary>
/// <param name="nNum">時分秒</param>
void CDay::SetTimeHMS(const int nNum)
{
	m_Time.SetHMS(nNum);
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/// <summary>
/// 日時が正しいか？
/// </summary>
/// <param name="bNotCheckHour">1日を48時間とする(Default:FALSE)</param>
/// <returns>正常:FALSE/異常:TRUE</returns>
BOOL CDay::IsFault(const BOOL bNotCheckHour)
{
	if (m_Date.IsFault())              { return TRUE; }
	if (m_Time.IsFault(bNotCheckHour)) { return TRUE; }

	return FALSE;
}


/// <summary>
/// 時間を進める
/// </summary>
/// <param name="nNum">時</param>
void CDay::IncHour(const int nNum)
{
	if (nNum > 0) {
		m_Time += (60 * nNum) * 60;
		if (m_Time.IsCarry()) { m_Date++; }
	} else if (nNum < 0) {
		DecHour(-1 * nNum);
	}
}


/// <summary>
/// 時間を進める
/// </summary>
/// <param name="nNum">分</param>
void CDay::IncMin(const int nNum)
{
	if (nNum > 0) {
		m_Time += 60 * nNum;
		if (m_Time.IsCarry()) { m_Date++; }
	} else if (nNum < 0) {
		DecMin(-1 * nNum);
	}
}


/// <summary>
/// 時間を進める
/// </summary>
/// <param name="nNum">秒</param>
void CDay::IncSec(const int nNum)
{
	if (nNum > 0) {
		m_Time += nNum;
		if (m_Time.IsCarry()) { m_Date++; }
	} else if (nNum < 0) {
		DecSec(-1 * nNum);
	}
}


/// <summary>
/// 日付を進める
/// </summary>
/// <param name="nNum">日</param>
void CDay::IncDay(const int nNum)
{
	if (nNum > 0) {
		m_Date += nNum;
	} else if (nNum < 0) {
		m_Date -= nNum;
	}
}


/// <summary>
/// 時刻を戻す
/// </summary>
/// <param name="nNum">時</param>
void CDay::DecHour(const int nNum)
{
	if (nNum > 0) {
		m_Time -= (60 * nNum) * 60;
		if (m_Time.IsCarry()) { m_Date--; }
	} else if (nNum < 0) {
		IncHour(-1 * nNum);
	}
}


/// <summary>
/// 時刻を戻す
/// </summary>
/// <param name="nNum"分></param>
void CDay::DecMin(const int nNum)
{
	if (nNum > 0) {
		m_Time -= 60 * nNum;
		if (m_Time.IsCarry()) { m_Date--; }
	} else if (nNum < 0) {
		IncMin(-1 * nNum);
	}
}


/// <summary>
/// 時刻を戻す
/// </summary>
/// <param name="nNum">秒</param>
void CDay::DecSec(const int nNum)
{
	if (nNum > 0) {
		m_Time -= nNum;
		if (m_Time.IsCarry()) { m_Date--; }
	} else if (nNum < 0) {
		IncSec(-1 * nNum);
	}
}


/// <summary>
/// 日付を戻す
/// </summary>
/// <param name="nNum"></param>
void CDay::DecDay(const int nNum)
{
	IncDay(-1 * nNum);
}


//---------------------------------------------------------------------------
// operator
//---------------------------------------------------------------------------
BOOL CDay::operator != (CDay &p)
{
	if (m_Date != p.m_Date) { return TRUE; }

	if (m_bIgnoreSecond) {
		if (m_Time.GetHM() != p.m_Time.GetHM()) { return TRUE; }
	} else {
		if (m_Time != p.m_Time) { return TRUE; }
	}

	return FALSE;
}

BOOL CDay::operator == (CDay &p)
{
	if (m_Date != p.m_Date) { return FALSE; }

	if (m_bIgnoreSecond) {
		if (m_Time.GetHM() != p.m_Time.GetHM()) { return FALSE; }
	} else {
		if (m_Time != p.m_Time) { return FALSE; }
	}

	return TRUE;
}

BOOL CDay::operator >= (CDay &p)
{
	if (m_Date < p.m_Date) { return FALSE; }
	if (m_Date > p.m_Date) { return TRUE;  }

	if (m_bIgnoreSecond) {
		return (m_Time.GetHM() >= p.m_Time.GetHM());
	}
	return (m_Time >= p.m_Time);
}

BOOL CDay::operator <= (CDay &p)
{
	if (m_Date > p.m_Date) { return FALSE; }
	if (m_Date < p.m_Date) { return TRUE;  }

	if (m_bIgnoreSecond) {
		return (m_Time.GetHM() <= p.m_Time.GetHM());
	}
	return (m_Time <= p.m_Time);
}

BOOL CDay::operator  < (CDay &p)
{
	if (m_Date > p.m_Date) { return FALSE; }
	if (m_Date < p.m_Date) { return TRUE;  }

	if (m_bIgnoreSecond) {
		return (m_Time.GetHM() < p.m_Time.GetHM());
	}
	return (m_Time < p.m_Time);
}

BOOL CDay::operator  > (CDay &p)
{
	if (m_Date < p.m_Date) { return FALSE; }
	if (m_Date > p.m_Date) { return TRUE;  }

	if (m_bIgnoreSecond) {
		return (m_Time.GetHM() > p.m_Time.GetHM());
	}
	return (m_Time > p.m_Time);
}

CDay::operator time_t()
{
	return GetTimet();
}


/// <summary>
/// operatorの比較時に“秒”を無視する
/// </summary>
/// <param name="bIgnoreSecond">TRUE:“秒”を無視する</param>
void CDay::SetIgnoreSecond(const BOOL bIgnoreSecond)
{
	m_bIgnoreSecond = bIgnoreSecond;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CYear

/// <summary>
/// コンストラクタ(初期値:2000)
/// </summary>
SDate::CYear::CYear()
{
	m_yy = 2000;
}


/// <summary>
/// コンストラクタ(年を設定)
/// </summary>
/// <param name="nNum">年</param>
SDate::CYear::CYear(const int nNum)
{
	m_yy = nNum;
}


int SDate::CYear::operator = (const int nNum)
{
	if (nNum < 0) {
		m_yy = 2000;
	} else {
		m_yy = (nNum % 100) + 2000;
	}

	return m_yy;
}

int SDate::CYear::operator -= (const int nNum)
{
	m_yy -= nNum;

	return m_yy;
}

int SDate::CYear::operator += (const int nNum)
{
	m_yy += nNum;

	return m_yy;
}

// 前置
int SDate::CYear::operator ++ ()
{
	(*this) += 1;

	return *this;
}

int SDate::CYear::operator -- ()
{
	(*this) -= 1;

	return *this;
}

// 後置
int SDate::CYear::operator ++ (int)
{
	long lVal = *this;

	(*this) += 1;

	return lVal;
}

int SDate::CYear::operator -- (int)
{
	long lVal = *this;

	(*this) -= 1;

	return lVal;
}

SDate::CYear::operator int()
{
	return m_yy;
}
// CYear
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



