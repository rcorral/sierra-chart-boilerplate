#ifndef _SCDATETIME_H_
#define _SCDATETIME_H_

class SCDateTime;
class SCDateTimeMS;

#include <cstdint>  // std::int64_t
#include <math.h>  // ceil, floor, fabs
#include <string.h>  // stricmp
#include <time.h>  // time_t

// A date in the number of days since 1899-12-30.
typedef int t_SCDate;

// A time of day in the number of seconds since midnight.
typedef int t_SCTime;

enum MonthEnum
{ JANUARY = 1
, FEBRUARY = 2
, MARCH = 3
, APRIL = 4
, MAY = 5
, JUNE = 6
, JULY = 7
, AUGUST = 8
, SEPTEMBER = 9
, OCTOBER = 10
, NOVEMBER = 11
, DECEMBER = 12
};
static const char* FULL_NAME_FOR_MONTH[13] = {"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
static const char* SHORT_NAME_FOR_MONTH[13] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const int DAYS_IN_MONTH[13] = {0, 31, 28, 31,  30,  31,  30,  31,  31,  30,  31,  30,  31};
static const int DAYS_IN_YEAR_AT_MONTH_END[13]   = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
static const char FUTURES_CODE_FOR_MONTH[13] = {'\0', 'F', 'G', 'H', 'J', 'K', 'M', 'N', 'Q', 'U', 'V', 'X', 'Z'};

const int MAXIMUM_INTRADAY_DATA_FILE_MILLISECONDS = 999;
const int MAXIMUM_INTRADAY_DATA_FILE_MICROSECONDS = 794;

enum DayOfWeekEnum
{ DAY_OF_WEEK_NOT_SET = -1
, SUNDAY = 0
, MONDAY = 1
, TUESDAY = 2
, WEDNESDAY = 3
, THURSDAY = 4
, FRIDAY = 5
, SATURDAY = 6
};

enum DayOfWeekMondayBasedEnum
{ DAY_OF_WEEK_MONDAY_BASED_NOT_SET = -1
, MONDAY_MONDAY_BASED = 0
, TUESDAY_MONDAY_BASED = 1
, WEDNESDAY_MONDAY_BASED = 2
, THURSDAY_MONDAY_BASED = 3
, FRIDAY_MONDAY_BASED = 4
, SATURDAY_MONDAY_BASED = 5
, SUNDAY_MONDAY_BASED = 6
};

const double DATETIME_UNSET = 0.0;

static const char* FULL_NAME_FOR_DAY_OF_WEEK[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
static const char* SHORT_NAME_FOR_DAY_OF_WEEK[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char* TWO_CHAR_NAME_FOR_DAY_OF_WEEK[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

static const int MONTHS_PER_YEAR = 12;
static const int DAYS_PER_WEEK = 7;  // number of days in a week
static const int HOURS_PER_DAY = 24;  // number of hours in a day
static const int MINUTES_PER_HOUR = 60;  // number of minutes in an hour
static const int SECONDS_PER_MINUTE = 60;  // number of seconds in a minute
static const int MILLISECONDS_PER_SECOND = 1000;  // number of milliseconds in a second
static const int NANOSECONDS_PER_MILLISECOND = 1000 * 1000;  // number of nanoseconds in a millisecond

static const std::int64_t MICROSECONDS_PER_MILLISECOND = 1000;

static const int MINUTES_PER_DAY = MINUTES_PER_HOUR*HOURS_PER_DAY;  // number of minutes in a day = 1 440
static const int SECONDS_PER_DAY = SECONDS_PER_MINUTE*MINUTES_PER_DAY;  // number of seconds in a day = 86 400
static const int SECONDS_PER_HOUR = SECONDS_PER_MINUTE*MINUTES_PER_HOUR;  // number of seconds in an hour = 3 600
static const int MILLISECONDS_PER_DAY = MILLISECONDS_PER_SECOND*SECONDS_PER_DAY;  // number of milliseconds in a day = 86 400 000
static const int MILLISECONDS_PER_HOUR = MILLISECONDS_PER_SECOND*SECONDS_PER_HOUR;  // number of milliseconds in an hour = 3 600 000
static const int MILLISECONDS_PER_MINUTE = MILLISECONDS_PER_SECOND*SECONDS_PER_MINUTE;  // number of milliseconds in a minute = 60 000
static const std::int64_t NANOSECONDS_PER_SECOND = NANOSECONDS_PER_MILLISECOND*MILLISECONDS_PER_SECOND;  // number of nanoseconds in a second = 1 000 000 000
static const std::int64_t NANOSECONDS_PER_DAY = static_cast<std::int64_t>(NANOSECONDS_PER_MILLISECOND)*MILLISECONDS_PER_DAY;  // number of nanoseconds in a day = 86 400 000 000 000

static const std::int64_t MICROSECONDS_PER_DAY = MICROSECONDS_PER_MILLISECOND*MILLISECONDS_PER_DAY;  // number of microseconds in a day = 86 400 000 000
static const std::int64_t MICROSECONDS_PER_HOUR = MICROSECONDS_PER_MILLISECOND*MILLISECONDS_PER_HOUR;  // number of microseconds in an hour = 3 600 000 000
static const std::int64_t MICROSECONDS_PER_MINUTE = MICROSECONDS_PER_MILLISECOND*MILLISECONDS_PER_MINUTE;  // number of microseconds in a minute = 60 000 000
static const std::int64_t MICROSECONDS_PER_SECOND = MICROSECONDS_PER_MILLISECOND*MILLISECONDS_PER_SECOND;  // number of microseconds in a second = 1 000 000

static const double YEARS = 365.25;  // 1 year in days (approx.)
static const double WEEKS = 7.0;  // 1 week in days
static const double DAYS = 1.0;  // 1 day in days
static const double HOURS = (1.0/HOURS_PER_DAY)*DAYS;  // 1 hour in days
static const double MINUTES = (1.0/MINUTES_PER_HOUR)*HOURS;  // 1 minute in days
static const double SECONDS = (1.0/SECONDS_PER_MINUTE)*MINUTES;  // 1 second in days
static const double MILLISECONDS = (1.0/MILLISECONDS_PER_SECOND)*SECONDS;  // 1 millisecond in days
static const double NANOSECONDS = (1.0/NANOSECONDS_PER_MILLISECOND)*MILLISECONDS;  // 1 nanosecond in days

static const double HALF_MILLISECOND_IN_SECONDS = (1.0/MILLISECONDS_PER_SECOND) * 0.5;  // = 0.0005

static const double HALF_SECOND = 0.5*SECONDS;  // Half a second
static const double HALF_MILLISECOND = 0.5*MILLISECONDS;  // Half a millisecond

// The epsilon should always be half of the smallest unit that is used
static const double SCDATETIME_EPSILON = HALF_SECOND;
static const double SCDATETIME_EPSILON_MS = HALF_MILLISECOND;

static const int MIN_DATE = -657434;  // Year 100
static const int MAX_DATE = 2958465;  // Year 9999

static const int EPOCH_YEAR = 1899;
static const int EPOCH_MONTH = 12;
static const int EPOCH_DAY = 30;

static const int SCDATETIME_UNIX_EPOCH_DATE = 25569;  // 1970-01-01
static const double SCDATETIME_UNIX_EPOCH = 25569.0;  // 1970-01-01 00:00:00

/*==========================================================================*/
inline bool IS_UNSET(const double DateTime)
{
	return DateTime == DATETIME_UNSET;
}

/*============================================================================
	Gets the date part of a SCDateTime.  Returns the number of days since
	1899-12-30.
----------------------------------------------------------------------------*/
inline int DATE_PART(double DateTime)
{
	if (DateTime < 0)
		return (int)(DateTime - SCDATETIME_EPSILON);
	else
		return (int)(DateTime + SCDATETIME_EPSILON);
}

/*============================================================================
	Returns only the time part (ignoring the date) of the given DateTime
	value as the number of seconds since midnight.  The return value will be
	in the range [0..86399].  The given value is rounded to the nearest
	second.
----------------------------------------------------------------------------*/
inline int TIME_PART(double DateTime)
{
	int TimeInSeconds = 0;
	if (DateTime < 0)  // Special handling for negative numbers
	{
		TimeInSeconds = (int)(
			(DateTime
				- ceil(DateTime - SCDATETIME_EPSILON)  // remove the date part
			)  // this is the fraction (time) part
			* SECONDS_PER_DAY  // convert to seconds
			- 0.5  // subtract half a second before truncating to round to the nearest second
		);
	}
	else  // Typical handling
	{
		TimeInSeconds = (int)(
			(DateTime
				- floor(DateTime + SCDATETIME_EPSILON)  // remove the date part
			)  // this is the fraction (time) part
			* SECONDS_PER_DAY  // convert to seconds
			+ 0.5  // add half a second before truncating to round to the nearest second
		);
	}

	if(TimeInSeconds >= SECONDS_PER_DAY)
		TimeInSeconds = 0;

	return TimeInSeconds;
}

/*============================================================================
	Gets the date part of a SCDateTime with millisecond rounding.  Returns
	the number of days since 1899-12-30.
----------------------------------------------------------------------------*/
inline int DATE_PART_MS(double DateTime)
{
	if (DateTime < 0)
		return (int)(DateTime - SCDATETIME_EPSILON_MS);
	else
		return (int)(DateTime + SCDATETIME_EPSILON_MS);
}

/*============================================================================
	Returns only the time part (ignoring the date) of the given DateTime
	value as the number of seconds since midnight.  The return value will be
	in the range 0 to 86399. The given value is rounded to the nearest
	millisecond, and then the millisecond component is discarded so that the
	value is truncated to the containing second.
----------------------------------------------------------------------------*/
inline int TIME_PART_MS(double DateTime)
{
	if (DateTime < 0)  // Special handling for negative numbers
	{
		return (int)(
			(DateTime
				- ceil(DateTime - SCDATETIME_EPSILON_MS)  // remove the date part
			)  // this is the fraction (time) part
			* SECONDS_PER_DAY  // convert to seconds
			- HALF_MILLISECOND_IN_SECONDS  // round down by no more than half a millisecond for (negative) integer conversion
		);
	}
	else  // Typical handling
	{
		return (int)(
			(DateTime
				- floor(DateTime + SCDATETIME_EPSILON_MS)  // remove the date part
			)  // this is the fraction (time) part
			* SECONDS_PER_DAY  // convert to seconds
			+ HALF_MILLISECOND_IN_SECONDS  // round up by no more than half a millisecond for integer conversion
		);
	}
}

/*============================================================================
	Returns only the seconds part of the given DateTime, ignoring any
	date part or time part that is 1 minute or greater.  The return value
	will be in the range [0..59].  The given value is rounded to the nearest
	second.
----------------------------------------------------------------------------*/
inline int SECONDS_PART(double DateTime)
{
	// Convert to seconds
	const double AbsoluteDateTimeInSeconds
		= fabs(DateTime) * SECONDS_PER_DAY;

	// Get seconds part
	int SecondsPart
		= (int)(((std::int64_t)floor(AbsoluteDateTimeInSeconds + 0.5))
		% SECONDS_PER_MINUTE);

	if (DateTime < 0)
		SecondsPart *= -1;

	return SecondsPart;
}

/*============================================================================
	Returns only the milliseconds part of the given DateTime, ignoring any
	date part or time part that is 1 second or greater.  The return value
	will be in the range [0..999].  The given value is rounded to the nearest
	millisecond.
----------------------------------------------------------------------------*/
inline int MS_PART(double DateTime)
{
	// Convert to seconds
	double DateTimeInSeconds = DateTime * SECONDS_PER_DAY;

	if (DateTimeInSeconds < 0)  // Special handling for negative numbers
	{
		// Remove the seconds part, leaving only the fractional milliseconds
		DateTimeInSeconds -= ceil(DateTimeInSeconds + HALF_MILLISECOND_IN_SECONDS);

		// Convert to milliseconds, rounding to the nearest millisecond
		return (int)(DateTimeInSeconds * MILLISECONDS_PER_SECOND - 0.5);
	}
	else  // Typical handling
	{
		// Remove the seconds part, leaving only the fractional milliseconds
		DateTimeInSeconds -= floor(DateTimeInSeconds + HALF_MILLISECOND_IN_SECONDS);

		// Convert to milliseconds, rounding to the nearest millisecond
		return (int)(DateTimeInSeconds * MILLISECONDS_PER_SECOND + 0.5);
	}
}

/*============================================================================
	Combines a date (days since 1899-12-30) and time (seconds since midnight)
	into a SCDateTime/OLE Date Time value.
----------------------------------------------------------------------------*/
inline double COMBINE_DATE_TIME(int Date, int Time)
{
	return Date + Time*SECONDS;
}

/*==========================================================================*/
inline DayOfWeekEnum DAY_OF_WEEK(int Date)
{
	// Special handling for zero and negative values.
	if (Date < MONDAY)
	{
		return static_cast<DayOfWeekEnum>
			( ((Date - MONDAY) % DAYS_PER_WEEK + DAYS_PER_WEEK)
				% DAYS_PER_WEEK
			);
	}

	return static_cast<DayOfWeekEnum>((Date - MONDAY) % DAYS_PER_WEEK);
}

/*==========================================================================*/
inline DayOfWeekEnum DAY_OF_WEEK(double DateTime)
{
	return DAY_OF_WEEK(DATE_PART(DateTime));
}

/*==========================================================================*/
inline DayOfWeekMondayBasedEnum DAY_OF_WEEK_MONDAY_BASED(int Date)
{
	return (DayOfWeekMondayBasedEnum)((Date - 2) % DAYS_PER_WEEK);
}
/*==========================================================================*/
inline const char* GetFullNameForMonth(int Month)
{
	if (Month < JANUARY || Month > DECEMBER)
		return FULL_NAME_FOR_MONTH[0];
	
	return FULL_NAME_FOR_MONTH[Month];
}

/*==========================================================================*/
inline const char* GetShortNameForMonth(int Month)
{
	if (Month < JANUARY || Month > DECEMBER)
		return SHORT_NAME_FOR_MONTH[0];
	
	return SHORT_NAME_FOR_MONTH[Month];
}

/*==========================================================================*/
inline int GetMonthFromShortName(const char* ShortName)
{
	if (ShortName == NULL)
		return 0;
	
	for (int Month = JANUARY; Month <= DECEMBER; ++Month)
	{
		#if _MSC_VER >= 1400
		if (_stricmp(ShortName, SHORT_NAME_FOR_MONTH[Month]) == 0)
		#else
		if (stricmp(ShortName, SHORT_NAME_FOR_MONTH[Month]) == 0)
		#endif
			return Month;
	}
	
	return 0;
}

/*============================================================================
	Returns the month (JANUARY-DECEMBER) for the given MonthCode.  Returns 0
	if the given MonthCode does not match any of the futures codes for months.
----------------------------------------------------------------------------*/
inline int GetMonthFromFuturesCode(char MonthCode)
{
	// Make upper-case
	if (MonthCode >= 'a' && MonthCode <= 'z')
		MonthCode += 'A' - 'a';
	
	for (int Month = JANUARY; Month <= DECEMBER; ++Month)
	{
		if (MonthCode == FUTURES_CODE_FOR_MONTH[Month])
			return Month;
	}
	
	return 0;
}

/*==========================================================================*/
inline bool IsValidFuturesMonthCode(char MonthCode)
{
	return (GetMonthFromFuturesCode(MonthCode) != 0);
}

/*==========================================================================*/
inline const char* GetFullNameForDayOfWeek(int DayOfWeek)
{
	if (DayOfWeek < SUNDAY || DayOfWeek > SATURDAY)
		return "";
	
	return FULL_NAME_FOR_DAY_OF_WEEK[DayOfWeek];
}

/*==========================================================================*/
inline const char* GetShortNameForDayOfWeek(int DayOfWeek)
{
	if (DayOfWeek < SUNDAY || DayOfWeek > SATURDAY)
		return "";
	
	return SHORT_NAME_FOR_DAY_OF_WEEK[DayOfWeek];
}

/*==========================================================================*/
inline const char* GetTwoCharNameForDayOfWeek(int DayOfWeek)
{
	if (DayOfWeek < SUNDAY || DayOfWeek > SATURDAY)
		return "";

	return TWO_CHAR_NAME_FOR_DAY_OF_WEEK[DayOfWeek];
}

/*==========================================================================*/
inline DayOfWeekEnum GetDayOfWeekFromInt(int DayOfWeek)
{
	if (DayOfWeek < SUNDAY || DayOfWeek > SATURDAY)
		return DAY_OF_WEEK_NOT_SET;

	return (DayOfWeekEnum)DayOfWeek;
}

/*==========================================================================*/
inline DayOfWeekEnum GetDayOfWeekFromFullName(const char* FullName)
{
	if (FullName == NULL)
		return DAY_OF_WEEK_NOT_SET;

	for (int DayOfWeek = SUNDAY; DayOfWeek <= SATURDAY; ++DayOfWeek)
	{
		#if _MSC_VER >= 1400
		if (_stricmp(FullName, FULL_NAME_FOR_DAY_OF_WEEK[DayOfWeek]) == 0)
		#else
		if (stricmp(FullName, FULL_NAME_FOR_DAY_OF_WEEK[DayOfWeek]) == 0)
		#endif
			return (DayOfWeekEnum)DayOfWeek;
	}

	return DAY_OF_WEEK_NOT_SET;
}

/*==========================================================================*/
inline DayOfWeekEnum GetDayOfWeekFromShortName(const char* ShortName)
{
	if (ShortName == NULL)
		return DAY_OF_WEEK_NOT_SET;

	for (int DayOfWeek = SUNDAY; DayOfWeek <= SATURDAY; ++DayOfWeek)
	{
		#if _MSC_VER >= 1400
		if (_stricmp(ShortName, SHORT_NAME_FOR_DAY_OF_WEEK[DayOfWeek]) == 0)
		#else
		if (stricmp(ShortName, SHORT_NAME_FOR_DAY_OF_WEEK[DayOfWeek]) == 0)
		#endif
			return (DayOfWeekEnum)DayOfWeek;
	}

	return DAY_OF_WEEK_NOT_SET;
}

/*==========================================================================*/
inline bool IsSaturday(int Date)
{
	return (DAY_OF_WEEK(Date) == SATURDAY);
}

/*==========================================================================*/
inline bool IsSaturday(double DateTime)
{
	return (DAY_OF_WEEK(DateTime) == SATURDAY);
}

/*==========================================================================*/
inline bool IsSunday(int Date)
{
	return (DAY_OF_WEEK(Date) == SUNDAY);
}

/*==========================================================================*/
inline bool IsSunday(double DateTime)
{
	return (DAY_OF_WEEK(DateTime) == SUNDAY);
}

/*==========================================================================*/
inline bool IsWeekend(int Date)
{
	const int DayOfWeek = DAY_OF_WEEK(Date);

	return (DayOfWeek == SUNDAY || DayOfWeek == SATURDAY);
}

/*==========================================================================*/
inline bool IsWeekend(double DateTime)
{
	const int DayOfWeek = DAY_OF_WEEK(DateTime);

	return (DayOfWeek == SUNDAY || DayOfWeek == SATURDAY);
}

/*==========================================================================*/
inline bool IsWeekend(int Date, bool UseSaturdayData)
{
	const int DayOfWeek = DAY_OF_WEEK(Date);

	return (DayOfWeek == SUNDAY || (!UseSaturdayData && DayOfWeek == SATURDAY));
}

/*==========================================================================*/
inline bool IsWeekend(double DateTime, bool UseSaturdayData)
{
	const int DayOfWeek = DAY_OF_WEEK(DateTime);

	return (DayOfWeek == SUNDAY || (!UseSaturdayData && DayOfWeek == SATURDAY));
}

/*==========================================================================*/
inline bool IsBusinessDay(int Date)
{
	return !IsWeekend(Date);
}

/*==========================================================================*/
inline bool IsLeapYear(int Year)
{
	return ((Year & 3) == 0) && ((Year % 100) != 0 || (Year % 400) == 0);
}

/*==========================================================================*/
inline int GetDaysInMonth(int Month, int Year)
{
	if (Month < JANUARY || Month > DECEMBER)
		return 0;
	
	if (Month == FEBRUARY && IsLeapYear(Year))
		return DAYS_IN_MONTH[Month] + 1;
	else
		return DAYS_IN_MONTH[Month];
}

/*============================================================================
	This function returns the number of weekdays within the given date span,
	including both the FirstDate and the LastDate.  Neither Saturdays nor
	Sundays are counted.
----------------------------------------------------------------------------*/
inline int DaysInDateSpanNotIncludingWeekends(int FirstDate, int LastDate)
{
	if (FirstDate > LastDate)
	{
		// Swap the dates
		const int EarlierDate = LastDate;
		LastDate = FirstDate;
		FirstDate = EarlierDate;
	}
	
	const int SpanDifference = LastDate - FirstDate;
	const int FirstDayOfWeek = DAY_OF_WEEK(FirstDate);
	const int SundayBasedSpan = SpanDifference + FirstDayOfWeek;
	
	int NumSaturdays;
	if (SundayBasedSpan - SATURDAY < 0)
		NumSaturdays = 0;
	else
		NumSaturdays = (SundayBasedSpan - SATURDAY) / DAYS_PER_WEEK + 1;
	
	int NumSundays = SundayBasedSpan / DAYS_PER_WEEK;
	if (FirstDayOfWeek == SUNDAY)
		++NumSundays;
	
	return SpanDifference + 1 - NumSaturdays - NumSundays;
}

/*============================================================================
	SCDateTime values are the same as Excel DateTime values.  These are
	stored as a floating point value (double) representing the number of days
	since 1899-12-30 00:00:00.  (Time is represented as a fraction of a day.)
	Unix-style timestamps are stored as integer values representing the
	number of seconds since 1970-01-01 00:00:00.  These two functions are
	used to convert from one to the other.
----------------------------------------------------------------------------*/
inline time_t SCDateTimeToUNIXTime(double DateTime)
{
	if (IS_UNSET(DateTime))
		return 0;

	return (time_t)( (DateTime - SCDATETIME_UNIX_EPOCH) * SECONDS_PER_DAY + 0.5);
}
/*==========================================================================*/
inline double SCDateTimeToFloatUNIXTime(double DateTime)
{
	if (IS_UNSET(DateTime))
		return 0.0;

	return ((DateTime - SCDATETIME_UNIX_EPOCH) * SECONDS_PER_DAY);
}


/*==========================================================================*/
inline double UNIXTimeToSCDateTime(time_t UnixTime) 
{
	if (UnixTime == 0)
		return DATETIME_UNSET;
	
	return UnixTime * SECONDS + SCDATETIME_UNIX_EPOCH;
}

/*==========================================================================*/
inline double UNIXTimeWithMillisecondsToSCDateTime(double UnixTimeWithMilliseconds) 
{
	if (UnixTimeWithMilliseconds == 0.0)
		return DATETIME_UNSET;
	
	return UnixTimeWithMilliseconds * SECONDS + SCDATETIME_UNIX_EPOCH;
}

/*==========================================================================*/
inline double HMS_MS_DATETIME(int Hour, int Minute, int Second, int Millisecond)
{
	return Hour*HOURS + Minute*MINUTES + Second*SECONDS + Millisecond*MILLISECONDS;
}

/*============================================================================
	This function expects one based values for Month and Day.
----------------------------------------------------------------------------*/
inline double YMDHMS_MS_DATETIME(int Year, int Month, int Day, int Hour, int Minute, int Second, int Millisecond)
{
	// Validate year and month (ignore day of week)
	if (Year > 9999 || Month < 1 || Month > 12)
		return DATETIME_UNSET;
	
	//  Check for leap year and set the number of days in the month
	const bool bLeapYear = IsLeapYear(Year);
	
	const int nDaysInMonth
		= DAYS_IN_YEAR_AT_MONTH_END[Month] - DAYS_IN_YEAR_AT_MONTH_END[Month-1]
		+ ((bLeapYear && Day == 29 && Month == 2) ? 1 : 0);
	
	// Finish validating the date
	if (Day < 1 || Day > nDaysInMonth ||
		Hour > 23 || Minute > 59 ||
		Second > 59 || Millisecond > 999)
	{
		return DATETIME_UNSET;
	}
	
	// Cache the date in days and time in fractional days
	int nDate;
	double dblTime;
	
	//It is a valid date; make Jan 1, 1AD be 1
	nDate = Year*365 + Year/4 - Year/100 + Year/400 +
		DAYS_IN_YEAR_AT_MONTH_END[Month-1] + Day;
	
	//  If leap year and it's before March, subtract 1:
	if (Month <= 2 && bLeapYear)
		--nDate;
	
	//  Offset so that 12/30/1899 is 0
	nDate -= 693959;
	
	dblTime = ((Hour * 3600) +  // hrs in seconds
		(Minute * 60) +  // mins in seconds
		(Second) +
		+ (Millisecond/1000.)) / 86400.
		;
	
	return (double)nDate + ((nDate >= 0) ? dblTime : -dblTime);
}

/*==========================================================================*/
inline double YMDHMS_DATETIME(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
	return YMDHMS_MS_DATETIME(Year, Month, Day, Hour, Minute, Second, 0);
}

/*============================================================================
	This function is thread safe.
----------------------------------------------------------------------------*/
inline void DATETIME_TO_YMDHMS_MS(double DateTime, int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second, int& Millisecond)
{
	Year = Month = Day = Hour = Minute = Second = 0;

	int WeekDay = 0;
	int DayOfYear = 0;
	
	
	// The legal range does not actually span year 0 to 9999.
	if (DateTime > MAX_DATE || DateTime < MIN_DATE) // about year 100 to about 9999
		return;
	
	int nDays;				// Number of days since Dec. 30, 1899
	int nDaysAbsolute;		// Number of days since 1/1/0
	int MillisecondsInDayForGivenTime;		

	int n400Years;			// Number of 400 year increments since 1/1/0
	int n400Century;		// Century within 400 year block (0,1,2 or 3)
	int n4Years;			// Number of 4 year increments since 1/1/0
	int n4Day;				// Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	int n4Yr;				// Year within 4 year block (0,1,2 or 3)
	int bLeap4 = 1;			// TRUE if 4 year block includes leap year
	
	
	double TempDateTime = DateTime; // temporary serial date
	
	
	
	if(DateTime != 0)
	{
	//If resolving for Milliseconds
		if(Millisecond != -1) 
			// Round to the millisecond.  
			TempDateTime += ((DateTime > 0.0) ? HALF_MILLISECOND  : - (HALF_MILLISECOND));
		else
			TempDateTime += ((DateTime > 0.0) ?( HALF_SECOND) : -(HALF_SECOND) );//Round to the second
	}
	Millisecond = 0;
	
	// If a valid date, then this conversion should not overflow
	nDays = (int)TempDateTime;
	
	nDaysAbsolute = (int)TempDateTime + 693959; // Add days from 1/1/0 to 12/30/1899
	
	TempDateTime = fabs(TempDateTime);
	MillisecondsInDayForGivenTime = (int)((TempDateTime - floor(TempDateTime)) * MILLISECONDS_PER_DAY);
	
	// Calculate the day of week (sun=1, mon=2...)
	//   -1 because 1/1/0 is Sat.  +1 because we want 1-based
	WeekDay = ((nDaysAbsolute - 1) % 7) + 1;
	
	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = nDaysAbsolute / 146097;
	
	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097;
	
	// -1 because first century has extra day
	n400Century = ((nDaysAbsolute - 1) / 36524);
	
	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524;
		
		// +1 because 1st 4 year increment has 1460 days
		n4Years = ((nDaysAbsolute + 1) / 1461);
		
		if (n4Years != 0)
			n4Day = ((nDaysAbsolute + 1) % 1461);
		else
		{
			bLeap4 = 0;
			n4Day = nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = nDaysAbsolute / 1461;
		n4Day = nDaysAbsolute % 1461;
	}
	
	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;
		
		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}
	
	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	DayOfYear = n4Day + 1;
	Year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;
	
	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			Month = 2;
			Day = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}
	
	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;
	
	// Month number always >= n/32, so save some loop time */
	for (Month = (n4Day >> 5) + 1;
		n4Day > DAYS_IN_YEAR_AT_MONTH_END[Month]; Month++);
	
	Day = (n4Day - DAYS_IN_YEAR_AT_MONTH_END[Month-1]);
	
DoTime:
	
	if (MillisecondsInDayForGivenTime != 0)
	{
		Hour = MillisecondsInDayForGivenTime / MILLISECONDS_PER_HOUR;
		MillisecondsInDayForGivenTime -= Hour * MILLISECONDS_PER_HOUR;
		Minute = MillisecondsInDayForGivenTime / MILLISECONDS_PER_MINUTE;
		MillisecondsInDayForGivenTime -= Minute * MILLISECONDS_PER_MINUTE;
		Second = MillisecondsInDayForGivenTime / MILLISECONDS_PER_SECOND;
		MillisecondsInDayForGivenTime -= Second * MILLISECONDS_PER_SECOND;
		Millisecond = MillisecondsInDayForGivenTime;
	}
}

/*============================================================================
	This function is thread safe.
----------------------------------------------------------------------------*/
inline void DATETIME_TO_YMDHMS(double DateTime, int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second)
{
	int Millisecond = -1;
	DATETIME_TO_YMDHMS_MS(DateTime, Year, Month, Day,  Hour, Minute, Second, Millisecond);
}

/*==========================================================================*/
inline int YMD_DATE(int Year, int Month, int Day)
{
	return static_cast<int>(YMDHMS_DATETIME(Year, Month, Day, 0, 0, 0));
}

/*==========================================================================*/
inline void DATE_TO_YMD(int Date, int& Year, int& Month, int& Day)
{
	int Hour, Minute, Second;
	DATETIME_TO_YMDHMS(Date, Year, Month, Day, Hour, Minute, Second);
}

/*============================================================================
	Converts hours, minutes, seconds to an integer time.  This is seconds
	since midnight.
----------------------------------------------------------------------------*/
inline int HMS_TIME(int Hour, int Minute, int Second)
{
	return (Hour*SECONDS_PER_HOUR + Minute*SECONDS_PER_MINUTE + Second);
}

/*==========================================================================*/
inline int HMS_TIME_WITH_VALIDATION(int Hour, int Minute, int Second)
{
	if (Hour < 0 || Hour > 23 || Minute < 0 || Minute > 59 || Second < 0 || Second > 59)
		return 0;

	return (Hour*SECONDS_PER_HOUR + Minute*SECONDS_PER_MINUTE + Second);
}

/*==========================================================================*/
inline int HOUR_OF_TIME(int Time)
{
	return ((Time / SECONDS_PER_HOUR) % HOURS_PER_DAY);
}

/*==========================================================================*/
inline int MINUTE_OF_TIME(int Time)
{
	return ((Time / SECONDS_PER_MINUTE) % MINUTES_PER_HOUR);
}

/*==========================================================================*/
inline int SECOND_OF_TIME(int Time)
{
	return (Time % SECONDS_PER_MINUTE);
}

/*==========================================================================*/
inline void TIME_TO_HMS(int Time, int& Hour, int& Minute, int& Second)
{
	Hour = HOUR_OF_TIME(Time);
	Minute = MINUTE_OF_TIME(Time);
	Second = SECOND_OF_TIME(Time);
}

/*****************************************************************************
// SCDateTime

The SCDateTime object wraps a single 8-byte floating-point (double) value
representing a complete date and time.

Comparison operators on this class are designed to compare date-time values
(including doubles) within half a second.  For best results, at least one of
the date-time values should be as close to the exact second as possible,
meaning avoid any milliseconds.
----------------------------------------------------------------------------*/

class SCDateTime
{
	private:
		double m_dt;

	public:
		SCDateTime();
		SCDateTime(double DateTime);
		SCDateTime(const SCDateTime& DateTime);
		SCDateTime(int Date, int TimeInSeconds);
		SCDateTime(int Hour, int Minute, int Second, int Millisecond);
		SCDateTime(int Year, int Month, int Day, int Hour, int Minute, int Second);
		SCDateTime(int Year, int Month, int Day, int Hour, int Minute, int Second, int Millisecond);

		operator double() const;

		const SCDateTime& operator = (double dt);
		const SCDateTime& operator = (const SCDateTime& DateTime);
		const SCDateTime& operator += (double dt);
		const SCDateTime& operator += (const SCDateTime& DateTime);
		const SCDateTime& operator -= (double dt);
		const SCDateTime& operator -= (const SCDateTime& DateTime);

		bool operator == (int Date) const;
		bool operator != (int Date) const;
		bool operator < (int Date) const;
		bool operator <= (int Date) const;
		bool operator > (int Date) const;
		bool operator >= (int Date) const;

		bool operator == (double dt) const;
		bool operator != (double dt) const;
		bool operator < (double dt) const;
		bool operator <= (double dt) const;
		bool operator > (double dt) const;
		bool operator >= (double dt) const;

		bool operator == (const SCDateTime& DateTime) const;
		bool operator != (const SCDateTime& DateTime) const;
		bool operator < (const SCDateTime& DateTime) const;
		bool operator <= (const SCDateTime& DateTime) const;
		bool operator > (const SCDateTime& DateTime) const;
		bool operator >= (const SCDateTime& DateTime) const;

		bool operator == (const SCDateTimeMS& DateTimeMS) const;
		bool operator != (const SCDateTimeMS& DateTimeMS) const;
		bool operator < (const SCDateTimeMS& DateTimeMS) const;
		bool operator <= (const SCDateTimeMS& DateTimeMS) const;
		bool operator > (const SCDateTimeMS& DateTimeMS) const;
		bool operator >= (const SCDateTimeMS& DateTimeMS) const;

		void Clear();
		void SetToMaximum();
		bool IsMaximum() const;
		bool IsUnset() const;
		bool IsDateSet() const;
		bool IsTimeAtMidnight() const;
		void ClearDate();
		void SetAsNegative();
		int GetDate() const;
		int GetDateMS() const;
		SCDateTime GetDateAsSCDateTime() const;

		int GetTime() const;
		int GetTimeInSeconds() const;
		int GetTimeInMilliseconds() const;
		int GetTimeWithoutMilliseconds() const;

		SCDateTime GetTimeAsSCDateTime() const;
		SCDateTimeMS GetTimeAsSCDateTimeMS() const;

		void GetDateYMD(int& Year, int& Month, int& Day) const;
		void GetTimeHMS(int& Hour, int& Minute, int& Second) const;
		void GetDateTimeYMDHMS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second) const;
		void GetDateTimeYMDHMS_MS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second, int& Millisecond) const;
		int GetYear() const;
		int GetMonth() const;
		int GetDay() const;
		int GetHour() const;
		int GetMinute() const;
		int GetSecond() const;
		int GetMillisecond() const;
		int GetDayOfWeek() const;
		int GetDayOfWeekMondayBased() const;
		SCDateTime& SetToPriorSundayForDateTime(const SCDateTime& OriginalDateTime);

		std::int64_t GetMillisecondsSinceBaseDate() const;
		SCDateTime GetAbsoluteValue() const;
		double GetAsDouble() const;

		bool IsWeekend() const;
		bool IsSaturday() const;
		bool IsSunday() const;
		bool IsMonday() const;
		bool IsGivenTimeWithinGivenTimeRange(const SCDateTime& DateTimeToCompareTo, const SCDateTime& TimeWithin);

		bool IsSameTimeToHour(const SCDateTime& DateTime) const;
		bool IsSameTimeToMinute(const SCDateTime& DateTime) const;
		bool IsSameTimeToSecond(const SCDateTime& DateTime) const;
		// Note: SCDDateTime does not have IsSameTimeToMillisecond, because
		// this method has no meaning with SCDateTime, which only has
		// accuracy down to the second.  Use IsSameTimeToMillisecond on
		// SCDateTimeMS instead.

		bool IsSameTimeToMilliSecond(const SCDateTime& DateTime) const;  // DEPRECATED 2018-01-05

		void RoundDateTimeDownToMinute();
		void RoundDateTimeDownToSecond();
		void RoundToNearestSecond();
		void RoundToNearestMillisecond();
		SCDateTime& SetDateTimeAsDouble(double DateTime);
		SCDateTime& SetDateTime(int Date, int Time);
		SCDateTime& SetDate(int Date);
		SCDateTime& SetDate(const SCDateTime& Date);
		SCDateTime& SetTime(int Time);
		SCDateTime& SetTimeFromSCDateTime(const SCDateTime& Time);
		SCDateTime& SetDateYMD(int Year, int Month, int Day);
		SCDateTime& SetTimeHMS(int Hour, int Minute, int Second);
		SCDateTime& SetTimeHMS_MS(int Hour, int Minute, int Second, int Millisecond);
		SCDateTime& SetDateTimeYMDHMS(int Year, int Month, int Day, int Hour, int Minute, int Second);
		SCDateTime& SetDateTimeYMDHMS_MS(int Year, int Month, int Day, int Hour, int Minute, int Second, int Millisecond);

		SCDateTime& AddNanoseconds(const std::uint64_t Nanoseconds);
		SCDateTime& AddMilliseconds(int Milliseconds);
		SCDateTime& SubtractMilliseconds(int Milliseconds);
		SCDateTime& AddSeconds(int Seconds);
		SCDateTime& SubtractSeconds(int Seconds);
		SCDateTime& AddMinutes(int Minutes);
		SCDateTime& SubtractMinutes(int Minutes);
		SCDateTime& AddHours(int Hours);
		SCDateTime& SubtractHours(int Hours);

		SCDateTime& AddDays(int Days);
		SCDateTime& SubtractDays(int Days);
		SCDateTime& AddYears(int Years);
		SCDateTime& SubtractYears(int Years);
		SCDateTime& SetDayToLastDayInMonth();
		SCDateTime& MultiplyTime(float Multiplier);

		time_t ToUNIXTime() const;
		int64_t ToUNIXTimeInMicroseconds() const;
		double ToUNIXTimeWithMillisecondsFraction() const;

		void SetFromUnixTime(double DateTime);
		void SetFromUNIXTimeMilliseconds(int64_t UNIXTimeMilliseconds);
		void SetFromUnixTimeInDays(const int Days);

		static SCDateTime GetMinimumDate();
		static SCDateTime GetMaximumDate();
};

/*****************************************************************************
// SCDateTimeMS

The SCDateTime object wraps a single 8-byte floating-point (double) value
representing a complete date and time.

Comparison operators on this class are designed to compare date-time values
(including doubles) within half a millisecond.  For best results, at least
one of the date-time values should be as close to the exact millisecond as
possible, meaning avoid any fractional milliseconds.

When comparing the value of a SCDateTime object with the value of a
SCDateTimeMS object, the comparison will be done at the milliseconds level.
If the SCDateTime object's value contains a millisecond value other than 0,
that millisecond value will be considered, so be careful about that.

Class has been reviewed and updated 2018-02-27 to properly handle conversion
to year, month, day from an internal Date-Time value.  Proper half
millisecond rounding is applied where necessary.
----------------------------------------------------------------------------*/
class SCDateTimeMS
	: public SCDateTime
{
	public:
		SCDateTimeMS();
		SCDateTimeMS(double DateTime);
		SCDateTimeMS(const SCDateTime& DateTime);
		SCDateTimeMS(const SCDateTimeMS& DateTime);
		SCDateTimeMS(int Date, int Time);

		const SCDateTimeMS& operator = (double dt);
		const SCDateTimeMS& operator = (const SCDateTime& DateTime);

		bool operator == (int Date) const;
		bool operator != (int Date) const;
		bool operator < (int Date) const;
		bool operator <= (int Date) const;
		bool operator > (int Date) const;
		bool operator >= (int Date) const;

		bool operator == (double dt) const;
		bool operator != (double dt) const;
		bool operator < (double dt) const;
		bool operator <= (double dt) const;
		bool operator > (double dt) const;
		bool operator >= (double dt) const;

		bool operator == (const SCDateTime& DateTime) const;
		bool operator != (const SCDateTime& DateTime) const;
		bool operator < (const SCDateTime& DateTime) const;
		bool operator <= (const SCDateTime& DateTime) const;
		bool operator > (const SCDateTime& DateTime) const;
		bool operator >= (const SCDateTime& DateTime) const;

		bool operator == (const SCDateTimeMS& DateTimeMS) const;
		bool operator != (const SCDateTimeMS& DateTimeMS) const;
		bool operator < (const SCDateTimeMS& DateTimeMS) const;
		bool operator <= (const SCDateTimeMS& DateTimeMS) const;
		bool operator > (const SCDateTimeMS& DateTimeMS) const;
		bool operator >= (const SCDateTimeMS& DateTimeMS) const;

		bool IsSameTimeToHour(const SCDateTimeMS& DateTime) const;
		bool IsSameTimeToMinute(const SCDateTimeMS& DateTime) const;
		bool IsSameTimeToSecond(const SCDateTimeMS& DateTime) const;
		bool IsSameTimeToMillisecond(const SCDateTimeMS& DateTime) const;

		void GetDateYMD(int& Year, int& Month, int& Day) const;

		int GetTime() const;
		void GetTimeHMS(int& Hour, int& Minute, int& Second) const;
		int GetHour() const;
		int GetMinute() const;
		int GetSecond() const;
};

/****************************************************************************/
// c_SCDateTime

/*==========================================================================*/
inline SCDateTime::SCDateTime()
	: m_dt(DATETIME_UNSET)
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(double DateTime)
	: m_dt(DateTime)
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(const SCDateTime& DateTime)
	: m_dt(DateTime.GetAsDouble())
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(int Date, int TimeInSeconds)
	: m_dt(COMBINE_DATE_TIME(Date, TimeInSeconds))
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(int Year, int Month, int Day, int Hour, int Minute, int Second)
	: m_dt(YMDHMS_DATETIME(Year, Month, Day, Hour, Minute, Second))
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(int Year, int Month, int Day, int Hour, int Minute, int Second, int Millisecond)
	: m_dt(YMDHMS_MS_DATETIME(Year, Month, Day, Hour, Minute, Second, Millisecond))
{
}

/*==========================================================================*/
inline SCDateTime::SCDateTime(int Hour, int Minute, int Second, int Millisecond)
	: m_dt(0)
{
	SetTimeHMS_MS(Hour, Minute, Second, Millisecond);
}

/*==========================================================================*/
inline SCDateTime::operator double() const
{
	return GetAsDouble();
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator = (double dt)
{
	m_dt = dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator = (const SCDateTime& DateTime)
{
	m_dt = DateTime.GetAsDouble();
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator += (double dt)
{
	m_dt += dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator += (const SCDateTime& DateTime)
{
	m_dt += DateTime.GetAsDouble();
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator -= (double dt)
{
	m_dt -= dt;
	return *this;
}

/*==========================================================================*/
inline const SCDateTime& SCDateTime::operator -= (const SCDateTime& DateTime)
{
	m_dt -= DateTime.GetAsDouble();
	return *this;
}

/*==========================================================================*/
inline bool SCDateTime::operator == (int Date) const
{
	return (fabs(GetAsDouble() - Date) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator != (int Date) const
{
	return !(fabs(GetAsDouble() - Date) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator < (int Date) const
{
	return (GetAsDouble() - Date < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator <= (int Date) const
{
	return (GetAsDouble() - Date < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator > (int Date) const
{
	return (GetAsDouble() - Date > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator >= (int Date) const
{
	return (GetAsDouble() - Date > -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator == (double dt) const
{
	return (fabs(GetAsDouble() - dt) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator != (double dt) const
{
	return !(fabs(GetAsDouble() - dt) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator < (double dt) const
{
	return (GetAsDouble() - dt < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator <= (double dt) const
{
	return (GetAsDouble() - dt < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator > (double dt) const
{
	return (GetAsDouble() - dt > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator >= (double dt) const
{
	return (GetAsDouble() - dt > -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator == (const SCDateTime& DateTime) const
{
	return (fabs(GetAsDouble() - DateTime.GetAsDouble()) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator != (const SCDateTime& DateTime) const
{
	return !(fabs(GetAsDouble() - DateTime.GetAsDouble()) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator < (const SCDateTime& DateTime) const
{
	return (GetAsDouble() - DateTime.GetAsDouble() < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator <= (const SCDateTime& DateTime) const
{
	return (GetAsDouble() - DateTime.GetAsDouble() < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator > (const SCDateTime& DateTime) const
{
	return (GetAsDouble() - DateTime.GetAsDouble() > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator >= (const SCDateTime& DateTime) const
{
	return (GetAsDouble() - DateTime.GetAsDouble() > -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool SCDateTime::operator == (const SCDateTimeMS& DateTimeMS) const
{
	return (fabs(GetAsDouble() - DateTimeMS.GetAsDouble()) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator != (const SCDateTimeMS& DateTimeMS) const
{
	return (fabs(GetAsDouble() - DateTimeMS.GetAsDouble()) >= SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator < (const SCDateTimeMS& DateTimeMS) const
{
	return (GetAsDouble() - DateTimeMS.GetAsDouble() < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator <= (const SCDateTimeMS& DateTimeMS) const
{
	return (GetAsDouble() - DateTimeMS.GetAsDouble() < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator > (const SCDateTimeMS& DateTimeMS) const
{
	return (GetAsDouble() - DateTimeMS.GetAsDouble() > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTime::operator >= (const SCDateTimeMS& DateTimeMS) const
{
	return (GetAsDouble() - DateTimeMS.GetAsDouble() > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline void SCDateTime::Clear()
{
	m_dt = DATETIME_UNSET;
}

/*==========================================================================*/
inline void SCDateTime::SetToMaximum()
{
	SetDateTimeAsDouble(MAX_DATE);
}
/*==========================================================================*/
inline bool SCDateTime::IsMaximum() const
{
	return GetAsDouble() == MAX_DATE;
}
/*==========================================================================*/
inline bool SCDateTime::IsUnset() const
{
	return IS_UNSET(GetAsDouble());
}

/*============================================================================
	Returns true only if the date is not at or before the base date of
	1900-01-01.
----------------------------------------------------------------------------*/
inline bool SCDateTime::IsDateSet() const
{
	return (GetDate() > 0);
}
/*==========================================================================*/
inline bool SCDateTime::IsTimeAtMidnight() const
{
	return GetTimeInSeconds() == 0;
}

/*==========================================================================*/
inline void SCDateTime::ClearDate()
{
	SetDate(0);
}

/*==========================================================================*/
inline void SCDateTime::SetAsNegative()
{
	if (GetAsDouble() <= 0)
		return;

	m_dt *= -1;
}

/*==========================================================================*/
inline int SCDateTime::GetDate() const
{
	return DATE_PART(GetAsDouble());
}
/*==========================================================================*/
inline int SCDateTime::GetDateMS() const
{
	return DATE_PART_MS(GetAsDouble());
}

/*==========================================================================*/
inline SCDateTime SCDateTime::GetDateAsSCDateTime() const
{
	return  SCDateTime(GetDate(), 0);
}

/*==========================================================================*/
inline int SCDateTime::GetTime() const
{
	return TIME_PART(GetAsDouble());
}

/*==========================================================================*/
inline int SCDateTime::GetTimeInSeconds() const
{
	return GetTime();
}

/*==========================================================================*/
inline int SCDateTime::GetTimeInMilliseconds() const
{
	int TimeInMilliseconds = 0;
	if (GetAsDouble() < 0)  // Special handling for negative numbers
	{
		TimeInMilliseconds = (int)(
			(GetAsDouble()
				- ceil(GetAsDouble() - SCDATETIME_EPSILON_MS)  // remove the date part
				)  // this is the fraction (time) part
			* MILLISECONDS_PER_DAY  // convert to milliseconds
			- 0.5  // subtract half a millisecond before truncating to round to the nearest millisecond
			);
	}
	else  // Typical handling
	{
		TimeInMilliseconds = (int)(
			(GetAsDouble()
				- floor(GetAsDouble() + SCDATETIME_EPSILON_MS)  // remove the date part
				)  // this is the fraction (time) part
			* MILLISECONDS_PER_DAY  // convert to milliseconds
			+ 0.5  // add half a millisecond before truncating to round to the nearest millisecond
			);
	}

	if (TimeInMilliseconds >=  MILLISECONDS_PER_DAY)
		TimeInMilliseconds = 0;

	return TimeInMilliseconds;
}

/*==========================================================================*/
inline int SCDateTime::GetTimeWithoutMilliseconds() const
{
	return TIME_PART_MS(GetAsDouble());
}

/*==========================================================================*/
inline SCDateTime SCDateTime::GetTimeAsSCDateTime() const
{
	return fabs(GetAsDouble()) - DATE_PART(fabs(GetAsDouble()));
}

/*==========================================================================*/
inline SCDateTimeMS SCDateTime::GetTimeAsSCDateTimeMS() const
{
	return fabs(GetAsDouble()) - DATE_PART(fabs(GetAsDouble()));
}

/*==========================================================================*/
inline void SCDateTime::GetDateYMD(int& Year, int& Month, int& Day) const
{
	DATE_TO_YMD(DATE_PART(GetAsDouble()), Year, Month, Day);
}

/*==========================================================================*/
inline void SCDateTime::GetTimeHMS(int& Hour, int& Minute, int& Second) const
{
	TIME_TO_HMS(TIME_PART(GetAsDouble()), Hour, Minute, Second);
}

/*==========================================================================*/
inline void SCDateTime::GetDateTimeYMDHMS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second) const
{
	DATETIME_TO_YMDHMS(GetAsDouble(), Year, Month, Day, Hour, Minute, Second);
}

/*==========================================================================*/
inline void SCDateTime::GetDateTimeYMDHMS_MS(int& Year, int& Month, int& Day, int& Hour, int& Minute, int& Second, int& Millisecond) const
{
	DATETIME_TO_YMDHMS_MS(GetAsDouble(), Year, Month, Day, Hour, Minute, Second, Millisecond);
}

/*==========================================================================*/
inline int SCDateTime::GetYear() const
{
	int Year, Month, Day;
	GetDateYMD(Year, Month, Day);
	return Year;
}

/*==========================================================================*/
inline int SCDateTime::GetMonth() const
{
	int Year, Month, Day;
	GetDateYMD(Year, Month, Day);
	return Month;
}

/*==========================================================================*/
inline int SCDateTime::GetDay() const
{
	int Year, Month, Day;
	GetDateYMD(Year, Month, Day);
	return Day;
}

/*==========================================================================*/
inline int SCDateTime::GetHour() const
{
	return HOUR_OF_TIME(TIME_PART(GetAsDouble()));
}

/*==========================================================================*/
inline int SCDateTime::GetMinute() const
{
	return MINUTE_OF_TIME(TIME_PART(GetAsDouble()));
}

/*==========================================================================*/
inline int SCDateTime::GetSecond() const
{
	return SECOND_OF_TIME(TIME_PART(GetAsDouble()));
}

/*==========================================================================*/
inline int SCDateTime::GetMillisecond() const
{
	return MS_PART(GetAsDouble());
}

/*==========================================================================*/
inline int SCDateTime::GetDayOfWeek() const
{
	return DAY_OF_WEEK(GetAsDouble());
}

/*==========================================================================*/
inline int SCDateTime::GetDayOfWeekMondayBased() const
{
	return DAY_OF_WEEK_MONDAY_BASED((int)GetAsDouble());
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetToPriorSundayForDateTime(const SCDateTime& OriginalDateTime)
{
	SetDateTimeAsDouble(OriginalDateTime.GetAsDouble());
	SetTime(0);
	SubtractDays(OriginalDateTime.GetDayOfWeek());
	return *this;
}

/*==========================================================================*/
inline std::int64_t SCDateTime::GetMillisecondsSinceBaseDate() const
{
	return (std::int64_t)(GetAsDouble() * MILLISECONDS_PER_DAY + .5);
}

/*==========================================================================*/
inline SCDateTime SCDateTime::GetAbsoluteValue() const
{
	return fabs(GetAsDouble());
}

/*==========================================================================*/
inline double SCDateTime::GetAsDouble() const
{
	return m_dt;
}

/*==========================================================================*/
inline bool SCDateTime::IsWeekend() const
{
	const int DayOfWeek = GetDayOfWeek();

	return (DayOfWeek == SATURDAY || DayOfWeek == SUNDAY);
}

/*==========================================================================*/
inline bool SCDateTime::IsSaturday() const
{
	return (GetDayOfWeek() == SATURDAY);
}

/*==========================================================================*/
inline bool SCDateTime::IsSunday() const
{
	return (GetDayOfWeek() == SUNDAY);
}

/*==========================================================================*/
inline bool SCDateTime::IsMonday() const
{
	return (GetDayOfWeek() == MONDAY);
}

/*==========================================================================*/
inline bool SCDateTime::IsGivenTimeWithinGivenTimeRange(const SCDateTime& DateTimeToCompareTo, const SCDateTime& TimeWithin)
{
	return SCDateTimeMS(fabs(GetAsDouble() - DateTimeToCompareTo.GetAsDouble())) <= TimeWithin;
}

/*============================================================================
	Returns true only if the given DateTime is at exactly the same date and
	hour as this SCDateTime.  Any minutes, seconds, or smaller components are
	excluded from the comparison.

	The accuracy of this comparison is to the nearest second.  For example, a
	value of 12:59:59.499 will be considered the same hour as 12:00:00, but a
	value of 12:59:59.501 will not, because the fractional second is closer
	to 13:00:00.

	Note: This method currently only supports positive values.
----------------------------------------------------------------------------*/
inline bool SCDateTime::IsSameTimeToHour(const SCDateTime& DateTime) const
{
	const int ThisDate = static_cast<int>(GetAsDouble() + SCDATETIME_EPSILON);
	const int ThatDate = static_cast<int>(DateTime.GetAsDouble() + SCDATETIME_EPSILON);

	if (ThatDate != ThisDate)
		return false;

	const double ThisFracTime = GetAsDouble() - ThisDate;
	const double ThatFracTime = DateTime.GetAsDouble() - ThatDate;

	const int ThisHour = static_cast<int>((ThisFracTime + SCDATETIME_EPSILON) * HOURS_PER_DAY);
	const int ThatHour = static_cast<int>((ThatFracTime + SCDATETIME_EPSILON) * HOURS_PER_DAY);

	return (ThatHour == ThisHour);
}

/*============================================================================
	Returns true only if the given DateTime is at exactly the same date, hour,
	and minute as this SCDateTime.  Any seconds, milliseconds, or smaller
	components are excluded from the comparison.

	The accuracy of this comparison is to the nearest second.  For example, a
	value of 12:34:59.499 will be considered the same minute as 12:34:00, but
	a value of 12:34:59.501 will not, because the fractional second is closer
	to 12:35:00.

	Note: This method currently only supports positive values.
----------------------------------------------------------------------------*/
inline bool SCDateTime::IsSameTimeToMinute(const SCDateTime& DateTime) const
{
	const int ThisDate = static_cast<int>(GetAsDouble() + SCDATETIME_EPSILON);
	const int ThatDate = static_cast<int>(DateTime.GetAsDouble() + SCDATETIME_EPSILON);

	if (ThatDate != ThisDate)
		return false;

	const double ThisFracTime = GetAsDouble() - ThisDate;
	const double ThatFracTime = DateTime.GetAsDouble() - ThatDate;

	const int ThisMinute = static_cast<int>((ThisFracTime + SCDATETIME_EPSILON) * MINUTES_PER_DAY);
	const int ThatMinute = static_cast<int>((ThatFracTime + SCDATETIME_EPSILON) * MINUTES_PER_DAY);

	return (ThatMinute == ThisMinute);
}

/*============================================================================
	Returns true only if the given DateTime is at exactly the same date, hour,
	minute, and second as this SCDateTime.  Any milliseconds, or smaller
	components are excluded from the comparison.

	The accuracy of this comparison is to the nearest second.  For example, a
	value of 12:34:56.499 will be considered the same second as 12:34:56, but
	a value of 12:34:56.501 will not, because the fractional second is closer
	to 12:34:57.

	Note: This method currently only supports positive values.
----------------------------------------------------------------------------*/
inline bool SCDateTime::IsSameTimeToSecond(const SCDateTime& DateTime) const
{
	const int ThisDate = static_cast<int>(GetAsDouble() + SCDATETIME_EPSILON);
	const int ThatDate = static_cast<int>(DateTime.GetAsDouble() + SCDATETIME_EPSILON);

	if (ThatDate != ThisDate)
		return false;

	const double ThisFracTime = GetAsDouble() - ThisDate;
	const double ThatFracTime = DateTime.GetAsDouble() - ThatDate;

	const int ThisSecond = static_cast<int>((ThisFracTime + SCDATETIME_EPSILON) * SECONDS_PER_DAY);
	const int ThatSecond = static_cast<int>((ThatFracTime + SCDATETIME_EPSILON) * SECONDS_PER_DAY);

	return (ThatSecond == ThisSecond);
}

/*============================================================================
	DEPRECATED 2018-01-05
----------------------------------------------------------------------------*/
inline bool SCDateTime::IsSameTimeToMilliSecond(const SCDateTime& DateTime) const
{
	return SCDateTimeMS(*this).IsSameTimeToMillisecond(DateTime);
}

/*==========================================================================*/
inline void SCDateTime::RoundDateTimeDownToMinute()
{
	m_dt -= SECONDS_PART(GetAsDouble())*SECONDS;
}

/*==========================================================================*/
inline void SCDateTime::RoundDateTimeDownToSecond()
{
	m_dt -= MS_PART(GetAsDouble())*MILLISECONDS;
}

/*==========================================================================*/
inline void SCDateTime::RoundToNearestSecond()
{
	const std::int64_t TimeAsSeconds = (std::int64_t)((GetAsDouble() + HALF_SECOND) * SECONDS_PER_DAY);
	SetDateTimeAsDouble(TimeAsSeconds * SECONDS + (0.25 * MILLISECONDS));
}

/*==========================================================================*/
inline void SCDateTime::RoundToNearestMillisecond()
{
	const std::int64_t TimeAsMilliseconds = static_cast<std::int64_t>(GetAsDouble() + HALF_MILLISECOND) * MILLISECONDS_PER_DAY;
	SetDateTimeAsDouble(TimeAsMilliseconds * MILLISECONDS);
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateTimeAsDouble(double DateTime)
{
	m_dt = DateTime;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateTime(int Date, int Time)
{
	m_dt = COMBINE_DATE_TIME(Date, Time);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDate(int Date)
{
	SetDateTimeAsDouble(GetAsDouble() - GetDate() + Date);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDate(const SCDateTime& Date)
{
	SetDateTimeAsDouble(GetAsDouble() - (int)GetAsDouble() + Date.GetDate());
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetTime(int Time)
{
	SetDateTimeAsDouble((int)GetAsDouble() + Time*SECONDS);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetTimeFromSCDateTime(const SCDateTime& Time)
{
	SetDateTimeAsDouble(GetDate() + Time.GetTimeAsSCDateTime());
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateYMD(int Year, int Month, int Day)
{
	return SetDate(YMD_DATE(Year, Month, Day));
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetTimeHMS(int Hour, int Minute, int Second)
{
	return SetTime(HMS_TIME(Hour, Minute, Second));
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetTimeHMS_MS(int Hour, int Minute, int Second, int Millisecond)
{
	SetTime(HMS_TIME(Hour, Minute, Second));
	m_dt += (Millisecond*MILLISECONDS);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateTimeYMDHMS(int Year, int Month, int Day, int Hour, int Minute, int Second)
{
	return SetDateTimeAsDouble(YMDHMS_DATETIME(Year, Month, Day, Hour, Minute, Second));
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDateTimeYMDHMS_MS(int Year, int Month, int Day, int Hour, int Minute, int Second, int Millisecond)
{
	return SetDateTimeAsDouble(YMDHMS_MS_DATETIME(Year, Month, Day, Hour, Minute, Second, Millisecond));
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::AddNanoseconds(const std::uint64_t Nanoseconds)
{
	const double Days = ((double)Nanoseconds) / NANOSECONDS_PER_DAY;
	m_dt += Days;
	return *this;
}


/*==========================================================================*/
inline SCDateTime& SCDateTime::AddMilliseconds(int Milliseconds)
{
	m_dt += Milliseconds * MILLISECONDS;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SubtractMilliseconds(int Milliseconds)
{
	m_dt -= Milliseconds * MILLISECONDS;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::AddSeconds(int Seconds)
{
	m_dt += Seconds * SECONDS;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SubtractSeconds(int Seconds)
{
	m_dt -= Seconds* SECONDS;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::AddMinutes(int  Minutes)
{
	m_dt +=  Minutes*MINUTES;

	return *this;
}
/*==========================================================================*/
inline SCDateTime& SCDateTime::SubtractMinutes(int Minutes)
{
	m_dt -=  Minutes*MINUTES;
	return *this;
}
/*==========================================================================*/
inline SCDateTime& SCDateTime::AddHours(int  Hours)
{
	m_dt  += Hours*HOURS;
	return *this;
}
/*==========================================================================*/
inline SCDateTime& SCDateTime::SubtractHours(int   Hours)
{
	m_dt  -= Hours*HOURS;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::AddDays(int Days)
{
	m_dt += Days*DAYS;
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SubtractDays(int Days)
{
	m_dt -= Days*DAYS; 
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::AddYears(int Years)
{
	AddDays(Years * DAYS_IN_YEAR_AT_MONTH_END[12]);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SubtractYears(int Years)
{
	SubtractDays(Years * DAYS_IN_YEAR_AT_MONTH_END[12]);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::SetDayToLastDayInMonth()
{
	int Year, Month, Day;
	GetDateYMD(Year, Month, Day);

	Day = GetDaysInMonth(Month, Year);

	SetDateYMD(Year, Month, Day);
	return *this;
}

/*==========================================================================*/
inline SCDateTime& SCDateTime::MultiplyTime(float Multiplier)
{
	m_dt *= Multiplier;
	return *this;
}

/*==========================================================================*/
inline time_t SCDateTime::ToUNIXTime() const
{
	if (IsUnset())
		return 0;

	return (time_t)((GetAsDouble() - SCDATETIME_UNIX_EPOCH) * SECONDS_PER_DAY + 0.5);
}

/*==========================================================================*/
inline int64_t SCDateTime::ToUNIXTimeInMicroseconds() const
{
	if (IsUnset())
		return 0;

	return static_cast<int64_t>((GetAsDouble() - SCDATETIME_UNIX_EPOCH) * MICROSECONDS_PER_DAY + 0.5);
}

/*==========================================================================*/
inline double SCDateTime::ToUNIXTimeWithMillisecondsFraction() const
{
	if (IsUnset())
		return DATETIME_UNSET;

	return static_cast<int64_t>((GetAsDouble() - SCDATETIME_UNIX_EPOCH) * MILLISECONDS_PER_DAY + 0.5) / 1000.0;
}

/*==========================================================================*/
inline void SCDateTime::SetFromUnixTime(double UnixTimeWithMilliseconds)
{
	if (UnixTimeWithMilliseconds != 0.0)
		m_dt = UnixTimeWithMilliseconds * SECONDS + SCDATETIME_UNIX_EPOCH;
	else
		m_dt = DATETIME_UNSET;
}

/*==========================================================================*/
inline void SCDateTime::SetFromUNIXTimeMilliseconds(int64_t UNIXTimeMilliseconds)
{
	if (UNIXTimeMilliseconds == 0)
	{
		m_dt = DATETIME_UNSET;
		return;
	}

	m_dt = UNIXTimeMilliseconds * MILLISECONDS + SCDATETIME_UNIX_EPOCH;
}

/*==========================================================================*/
inline void SCDateTime::SetFromUnixTimeInDays(const int Days)
{
	if (Days != 0)
		m_dt = Days + SCDATETIME_UNIX_EPOCH;
	else
		m_dt = DATETIME_UNSET;
}

/*==========================================================================*/
//Static
inline SCDateTime SCDateTime::GetMinimumDate()
{
	return MIN_DATE;
}

/*==========================================================================*/
//Static
inline SCDateTime SCDateTime::GetMaximumDate()
{
	return MAX_DATE;
}

/****************************************************************************/
// SCDateTimeMS

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS()
	:SCDateTime(DATETIME_UNSET)
{
}

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS(double DateTime)
	: SCDateTime(DateTime)
{
}

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS(const SCDateTime& DateTime)
	: SCDateTime(DateTime)
{
}

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS(const SCDateTimeMS& DateTime)
	: SCDateTime(DateTime.GetAsDouble())

{
}

/*==========================================================================*/
inline SCDateTimeMS::SCDateTimeMS(int Date, int Time)
	: SCDateTime(COMBINE_DATE_TIME(Date, Time))
{
}

/*==========================================================================*/
inline const SCDateTimeMS& SCDateTimeMS::operator = (double dt)
{
	SetDateTimeAsDouble(dt);
	return *this;
}

/*==========================================================================*/
inline const SCDateTimeMS& SCDateTimeMS::operator = (const SCDateTime& DateTime)
{
	SetDateTimeAsDouble(DateTime.GetAsDouble());
	return *this;
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator == (int Date) const
{
	return (fabs(GetAsDouble() - Date) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator != (int Date) const
{
	return !(fabs(GetAsDouble() - Date) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator < (int Date) const
{
	return (GetAsDouble() - Date < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator <= (int Date) const
{
	return (GetAsDouble() - Date < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator > (int Date) const
{
	return (GetAsDouble() - Date > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator >= (int Date) const
{
	return (GetAsDouble() - Date > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator == (double dt) const
{
	return (fabs(GetAsDouble() - dt) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator != (double dt) const
{
	return !(fabs(GetAsDouble() - dt) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator < (double dt) const
{
	return (GetAsDouble() - dt < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator <= (double dt) const
{
	return (GetAsDouble() - dt < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator > (double dt) const
{
	return (GetAsDouble() - dt > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator >= (double dt) const
{
	return (GetAsDouble() - dt > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator == (const SCDateTime& DateTime) const
{
	return (fabs(GetAsDouble() - DateTime.GetAsDouble()) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator != (const SCDateTime& DateTime) const
{
	return !(fabs(GetAsDouble() - DateTime.GetAsDouble()) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator < (const SCDateTime& DateTime) const
{
	return (GetAsDouble() - DateTime.GetAsDouble() < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator <= (const SCDateTime& DateTime) const
{
	return (GetAsDouble() - DateTime.GetAsDouble() < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator > (const SCDateTime& DateTime) const
{
	return (GetAsDouble() - DateTime.GetAsDouble() > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator >= (const SCDateTime& DateTime) const
{
	return (GetAsDouble() - DateTime.GetAsDouble() > -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator == (const SCDateTimeMS& DateTimeMS) const
{
	return (fabs(GetAsDouble() - DateTimeMS.GetAsDouble()) < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator != (const SCDateTimeMS& DateTimeMS) const
{
	return (fabs(GetAsDouble() - DateTimeMS.GetAsDouble()) >= SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator < (const SCDateTimeMS& DateTimeMS) const
{
	return (GetAsDouble() - DateTimeMS.GetAsDouble() < -SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator <= (const SCDateTimeMS& DateTimeMS) const
{
	return (GetAsDouble() - DateTimeMS.GetAsDouble() < SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator > (const SCDateTimeMS& DateTimeMS) const
{
	return (GetAsDouble() - DateTimeMS.GetAsDouble() > SCDATETIME_EPSILON_MS);
}

/*==========================================================================*/
inline bool SCDateTimeMS::operator >= (const SCDateTimeMS& DateTimeMS) const
{
	return (GetAsDouble() - DateTimeMS.GetAsDouble() > -SCDATETIME_EPSILON_MS);
}

/*============================================================================
	Returns true only if the given DateTime is at exactly the same date and
	hour as this SCDateTimeMS.  Any minutes, seconds, or smaller components
	are excluded from the comparison.

	The accuracy of this comparison is to the nearest millisecond.  For
	example, a value of 12:59:59.9994 will be considered the same hour as
	12:00:00, but a value of 12:59:59.9996 will not, because the fractional
	millisecond is closer to 13:00:00.

	Note: This method currently only supports positive values.
----------------------------------------------------------------------------*/
inline bool SCDateTimeMS::IsSameTimeToHour(const SCDateTimeMS& DateTime) const
{
	const int ThisDate = static_cast<int>(GetAsDouble() + SCDATETIME_EPSILON_MS);
	const int ThatDate = static_cast<int>(DateTime.GetAsDouble() + SCDATETIME_EPSILON_MS);

	if (ThatDate != ThisDate)
		return false;

	const double ThisFracTime = GetAsDouble() - ThisDate;
	const double ThatFracTime = DateTime.GetAsDouble() - ThatDate;

	const int ThisHour = static_cast<int>((ThisFracTime + SCDATETIME_EPSILON_MS) * HOURS_PER_DAY);
	const int ThatHour = static_cast<int>((ThatFracTime + SCDATETIME_EPSILON_MS) * HOURS_PER_DAY);

	return (ThatHour == ThisHour);
}

/*============================================================================
	Returns true only if the given DateTime is at exactly the same date, hour,
	and minute as this SCDateTimeMS.  Any seconds, milliseconds, or smaller
	components are excluded from the comparison.

	The accuracy of this comparison is to the nearest millisecond.  For
	example, a value of 12:34:59.9994 will be considered the same minute as
	12:34:00, but a value of 12:34:59.9996 will not, because the fractional
	millisecond is closer to 12:35:00.

	Note: This method currently only supports positive values.
----------------------------------------------------------------------------*/
inline bool SCDateTimeMS::IsSameTimeToMinute(const SCDateTimeMS& DateTime) const
{
	const int ThisDate = static_cast<int>(GetAsDouble() + SCDATETIME_EPSILON_MS);
	const int ThatDate = static_cast<int>(DateTime.GetAsDouble() + SCDATETIME_EPSILON_MS);

	if (ThatDate != ThisDate)
		return false;

	const double ThisFracTime = GetAsDouble() - ThisDate;
	const double ThatFracTime = DateTime.GetAsDouble() - ThatDate;

	const int ThisMinute = static_cast<int>((ThisFracTime + SCDATETIME_EPSILON_MS) * MINUTES_PER_DAY);
	const int ThatMinute = static_cast<int>((ThatFracTime + SCDATETIME_EPSILON_MS) * MINUTES_PER_DAY);

	return (ThatMinute == ThisMinute);
}

/*============================================================================
	Returns true only if the given DateTime is at exactly the same date, hour,
	minute, and second as this SCDateTimeMS.  Any milliseconds, or smaller
	components are excluded from the comparison.

	The accuracy of this comparison is to the nearest millisecond.  For
	example, a value of 12:34:56.9994 will be considered the same second as
	12:34:56, but a value of 12:34:56.9996 will not, because the fractional
	millisecond is closer to 12:34:57.

	Note: This method currently only supports positive values.
----------------------------------------------------------------------------*/
inline bool SCDateTimeMS::IsSameTimeToSecond(const SCDateTimeMS& DateTime) const
{
	const int ThisDate = static_cast<int>(GetAsDouble() + SCDATETIME_EPSILON_MS);
	const int ThatDate = static_cast<int>(DateTime.GetAsDouble() + SCDATETIME_EPSILON_MS);

	if (ThatDate != ThisDate)
		return false;

	const double ThisFracTime = GetAsDouble() - ThisDate;
	const double ThatFracTime = DateTime.GetAsDouble() - ThatDate;

	const int ThisSecond = static_cast<int>((ThisFracTime + SCDATETIME_EPSILON_MS) * SECONDS_PER_DAY);
	const int ThatSecond = static_cast<int>((ThatFracTime + SCDATETIME_EPSILON_MS) * SECONDS_PER_DAY);

	return (ThatSecond == ThisSecond);
}

/*============================================================================
	Returns true only if the given DateTime is at exactly the same date, hour,
	minute, second, and millisecond as this SCDateTimeMS.  Any smaller
	components are excluded from the comparison.

	The accuracy of this comparison is to the nearest millisecond.  For
	example, a value of 12:34:56.7894 will be considered the same millisecond
	as 12:34:56.789, but a value of 12:34:56.7896 will not, because the
	fractional millisecond is closer to 12:34:56.790.

	Note: This method currently only supports positive values.
----------------------------------------------------------------------------*/
inline bool SCDateTimeMS::IsSameTimeToMillisecond(const SCDateTimeMS& DateTime) const
{
	const int ThisDate = static_cast<int>(GetAsDouble() + SCDATETIME_EPSILON_MS);
	const int ThatDate = static_cast<int>(DateTime.GetAsDouble() + SCDATETIME_EPSILON_MS);

	if (ThatDate != ThisDate)
		return false;

	const double ThisFracTime = GetAsDouble() - ThisDate;
	const double ThatFracTime = DateTime.GetAsDouble() - ThatDate;

	const int ThisMillisecond = static_cast<int>((ThisFracTime + SCDATETIME_EPSILON_MS) * MILLISECONDS_PER_DAY);
	const int ThatMillisecond = static_cast<int>((ThatFracTime + SCDATETIME_EPSILON_MS) * MILLISECONDS_PER_DAY);

	return (ThatMillisecond == ThisMillisecond);
}

/*==========================================================================*/
inline void SCDateTimeMS::GetDateYMD(int& Year, int& Month, int& Day) const
{
	DATE_TO_YMD(DATE_PART_MS(GetAsDouble()), Year, Month, Day);
}

/*==========================================================================*/
inline int SCDateTimeMS::GetTime() const
{
	return TIME_PART_MS(GetAsDouble());
}

/*==========================================================================*/
inline void SCDateTimeMS::GetTimeHMS(int& Hour, int& Minute, int& Second) const
{
	TIME_TO_HMS(TIME_PART_MS(GetAsDouble()), Hour, Minute, Second);
}

/*==========================================================================*/
inline int SCDateTimeMS::GetHour() const
{
	return HOUR_OF_TIME(TIME_PART_MS(GetAsDouble()));
}

/*==========================================================================*/
inline int SCDateTimeMS::GetMinute() const
{
	return MINUTE_OF_TIME(TIME_PART_MS(GetAsDouble()));
}

/*==========================================================================*/
inline int SCDateTimeMS::GetSecond() const
{
	return SECOND_OF_TIME(TIME_PART_MS(GetAsDouble()));
}

/*==========================================================================*/
inline std::int64_t SCDateTimeToUNIXTimeRemoveMilliseconds(SCDateTime DateTime)
{
	DateTime.RoundDateTimeDownToSecond();
	return( std::int64_t) ( (DateTime - SCDATETIME_UNIX_EPOCH) * SECONDS_PER_DAY + 0.5);
}

/****************************************************************************/
// Integer comparison operators

/*==========================================================================*/
inline bool operator == (int IntDateTime, const SCDateTime& DateTime)
{
	return (fabs(IntDateTime - DateTime) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator != (int dt, const SCDateTime& DateTime)
{
	return !(fabs(dt - DateTime) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator < (int dt, const SCDateTime& DateTime)
{
	return (dt - DateTime < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator <= (int dt, const SCDateTime& DateTime)
{
	return (dt - DateTime < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator > (int dt, const SCDateTime& DateTime)
{
	return (dt - DateTime > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator >= (int dt, const SCDateTime& DateTime)
{
	return (dt - DateTime > -SCDATETIME_EPSILON);
}

/****************************************************************************/
// Double comparison operators

/*==========================================================================*/
inline bool operator == (double dt, const SCDateTime& DateTime)
{
	return (fabs(dt - DateTime) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator != (double dt, const SCDateTime& DateTime)
{
	return !(fabs(dt - DateTime) < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator < (double dt, const SCDateTime& DateTime)
{
	return (dt - DateTime < -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator <= (double dt, const SCDateTime& DateTime)
{
	return (dt - DateTime < SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator > (double dt, const SCDateTime& DateTime)
{
	return (dt - DateTime > SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline bool operator >= (double dt, const SCDateTime& DateTime)
{
	return (dt - DateTime > -SCDATETIME_EPSILON);
}

/*==========================================================================*/
inline double GetNextMillisecondForSameSecondDateTime(double PreviousDateTime, double NextDateTime)
{
	//Purpose: We need to make a new record timestamp unique by using milliseconds.
	const SCDateTimeMS PreviousDateTimeCopy(PreviousDateTime);

	if (PreviousDateTimeCopy.IsSameTimeToSecond(NextDateTime)) //Timestamp to the second is the same.  
	//if (PreviousDateTimeCopy == NextDateTime)
	{
		const int Milliseconds = PreviousDateTimeCopy.GetMillisecond();

		if (Milliseconds >= MAXIMUM_INTRADAY_DATA_FILE_MILLISECONDS)
			NextDateTime = PreviousDateTimeCopy;
		else
			NextDateTime = PreviousDateTimeCopy + 1 * MILLISECONDS;
	}

	return NextDateTime;
}

/*==========================================================================*/
inline SCDateTimeMS GetNextMillisecondForSameSecondDateTime(const SCDateTimeMS PreviousDateTime, SCDateTimeMS NextDateTime)
{
	//Purpose: We need to make a new record timestamp unique by using milliseconds.

	if (PreviousDateTime.IsSameTimeToSecond(NextDateTime) //Timestamp to the second is the same.  
		&& NextDateTime <= PreviousDateTime)
	{
		const int Milliseconds = PreviousDateTime.GetMillisecond();

		if (Milliseconds >= MAXIMUM_INTRADAY_DATA_FILE_MILLISECONDS)
			NextDateTime = PreviousDateTime;
		else
			NextDateTime = PreviousDateTime + 1 * MILLISECONDS;
	}

	return NextDateTime;
}

/*****************************************************************************
// SCDateTimeSpan

The SCDateTime object wraps a single 8-byte floating-point (double) value
representing a time span between two SCDateTime objects.

----------------------------------------------------------------------------*/
class SCDateTimeSpan
{
public:
	double m_TimeSpan;

	SCDateTimeSpan SetTimeSpan(const SCDateTime& FirstDateTime, const SCDateTime& SecondDateTime)
	{
		m_TimeSpan = FirstDateTime - SecondDateTime;

		return *this;
	}

	SCDateTime GetTimeSpan() const
	{
		return m_TimeSpan;
	}

	SCDateTimeSpan& operator = (double TimeSpan)
	{
		m_TimeSpan = TimeSpan;

		return *this;
	}
};

#endif
