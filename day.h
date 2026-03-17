
#pragma once


#define WEEK_NONE 0
#define WEEK_SUN  1
#define WEEK_MON  2
#define WEEK_TUE  3
#define WEEK_WED  4
#define WEEK_THU  5
#define WEEK_FRI  6
#define WEEK_SAT  7
#define WEEK_SIZE 8


struct SDate {
	/////////////////////////////////////////////////////////////////////////////
	// CYear
	class CYear {
		int m_yy;

	public:
		CYear();
		CYear(const int nNum);

		int operator  = (const int nNum);
		int operator -= (const int nNum);
		int operator += (const int nNum);
		int operator ++ ();	// 前置
		int operator -- ();
		int operator ++ (int);	// 後置
		int operator -- (int);
		operator int();
	};
	// CYear
	/////////////////////////////////////////////////////////////////////////////

private:
	volatile static __time64_t m_tm64World;

public:
	CYear m_yy;
	int   m_mm;
	int   m_dd;

	SDate(const int nNum = 0);

	void Clear();
	int  Now();

	int  Get();
	int  GetYMD();
	void Set(const int nNum);
	void SetYMD(const int nNum);

	int  GetDays();
	int  GetDays(const int yy, const int mm);
	int  GetWeek();
	int  GetWeek(const int yy, const int mm, const int dd);
	BOOL IsLeap();
	BOOL IsLeap(const int yy);
	BOOL IsFault();

	void  operator  = (const int nNum);
	void  operator -= (const int nNum);
	void  operator += (const int nNum);
	SDate operator ++ ();	// 前置
	SDate operator -- ();
	SDate operator ++ (int);	// 後置
	SDate operator -- (int);
	BOOL  operator != (SDate &);
	BOOL  operator == (SDate &);
	BOOL  operator >= (SDate &);
	BOOL  operator <= (SDate &);
	BOOL  operator  < (SDate &);
	BOOL  operator  > (SDate &);
	operator int();
};


struct STime {
private:
	BOOL m_bCarry;
public:
	BOOL IsCarry() { return m_bCarry; }

public:
	int m_hh;
	int m_mm;
	int m_ss;

	STime(const int nTim = 0);

	void Clear();
	int  Now();

	int  GetHM();
	int  GetHMS();
	BOOL SetHM(const int nNum);
	BOOL SetHMS(const int nNum);

	int  GetMM();
	int  GetSS();
	BOOL SetMM(const int nNum);
	BOOL SetSS(const int nNum);

	BOOL IsFault(const BOOL bNotCheckHour = FALSE);

	int  operator  = (const int nNum);
	int  operator += (const int nNum);
	int  operator -= (const int nNum);
	int  operator ++ ();	// 前置
	int  operator -- ();
	int  operator ++ (int);	// 後置
	int  operator -- (int);
	BOOL operator != (STime &);
	BOOL operator == (STime &);
	BOOL operator >= (STime &);
	BOOL operator <= (STime &);
	BOOL operator  < (STime &);
	BOOL operator  > (STime &);
	operator int();
};


class CDay {
	// operatorの比較時に“秒”を無視する
private:
	BOOL m_bIgnoreSecond;
public:
	void SetIgnoreSecond(const BOOL bIgnoreSecond = FALSE);
	BOOL IsIgnoreSecond() { return m_bIgnoreSecond; }

public:
	CDay(const BOOL bNow = TRUE, const BOOL bIgnoreSecond = FALSE);

	SDate m_Date;
	STime m_Time;

	void Clear();
	void Now();

	int GetYear();
	int GetMonth();
	int GetDay();
	int GetWeek();
	int GetDateYMD();
	
	int GetHour();
	int GetMin();
	int GetSec();
	int GetTimeHM();
	int GetTimeHMS();
	__time64_t GetTimet();
	__time64_t GetGmtTm();

	BOOL SetDate(const int nYear, const int nMonth, const int nDay, const int nHour = 0, const int nMin = 0, const int nSec = 0);
	
	void SetHour   (const int nNum);
	void SetMin    (const int nNum);
	void SetSec    (const int nNum);
	void SetTimeHM (const int nNum);
	void SetTimeHMS(const int nNum);

	void IncHour(const int nNum = 1);
	void IncMin (const int nNum = 1);
	void IncSec (const int nNum = 1);
	void IncDay (const int nNum = 1);
	void DecHour(const int nNum = 1);
	void DecMin (const int nNum = 1);
	void DecSec (const int nNum = 1);
	void DecDay (const int nNum = 1);

	BOOL IsFault(const BOOL bNotCheckHour = FALSE);

	BOOL operator != (CDay &);
	BOOL operator == (CDay &);
	BOOL operator >= (CDay &);
	BOOL operator <= (CDay &);
	BOOL operator  < (CDay &);
	BOOL operator  > (CDay &);
	operator time_t();
};


