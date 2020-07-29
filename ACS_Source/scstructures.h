// File Name: scstructures.h

#ifndef _SCSTRUCTURES_H_
#define _SCSTRUCTURES_H_

#ifndef NOMINMAX
// If NOMINMAX is not already defined, make a note if it so that it can be
// disabled at a later point.
#define SC_UNDEF_NOMINMAX
#endif

#include <windows.h> //Main Windows SDK header file
#include <stdio.h> //This header is for input/output including file input/output
#include <stdarg.h>
#include <float.h>
#include <io.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <algorithm>

#include <cstdlib>
#ifdef __GNUC__
// cstdlib on MinGW/GCC appears to always define NOMINMAX, which is not
// usually desired on remote builds for custom study DLLs.
//
// Note: we do not want do undefine NOMINMAX if it was intentionally defined
// earlier than this include.
#ifdef SC_UNDEF_NOMINMAX
#undef NOMINMAX
#endif
#endif
#include <csignal>
#include <csetjmp>
#include <cstdarg>
#include <typeinfo>
#include <bitset>
#include <functional>
#include <utility>
#include <ctime>
#include <cstddef>
#include <new>
#include <memory>
#include <climits>
#include <cfloat>
#include <limits>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <cerrno>
#include <cctype>
#include <cwctype>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <complex>
#include <valarray>
#include <numeric>
#include <iosfwd>
#include <ios>
#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <streambuf>
#include <cstdio>
#include <cstdint>
#include <cmath>

#ifndef NOMINMAX
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#endif
/****************************************************************************/

#include "scconstants.h"
#include "scdatetime.h"
#include "SCSymbolData.h"
#include "sccolors.h"
#include "VAPContainer.h"
#include "SCString.h"

#ifdef _WIN64
typedef double t_OrderQuantity32_64;
#else/*
#ifdef _DEBUG
typedef double t_OrderQuantity32_64;
#else*/
typedef int t_OrderQuantity32_64;
//#endif
#endif

struct s_sc;

typedef s_sc& SCStudyInterfaceRef;

const int CURRENT_SC_NEW_ORDER_VERSION = 17;
const int CURRENT_SC_ORDER_FILL_DATA_VERSION = 4;


#define SCDLLEXPORT extern "C" __declspec(dllexport)
#define SCDLLCALL __cdecl

#define SCSFExport SCDLLEXPORT void SCDLLCALL  // Use SCSFExport when defining a study function

#define SCDLLName(DLLName) \
	SCDLLEXPORT int SCDLLCALL scdll_DLLVersion() { return SC_DLL_VERSION; } \
	SCDLLEXPORT const char* SCDLLCALL scdll_DLLName() { return DLLName; }


#define ADVANCED_FEATURES_MESSAGE "You need to be on a software service package that includes Advanced Features to use this study."

typedef void (SCDLLCALL* fp_ACSGDIFunction)(HWND WindowHandle, HDC DeviceContext, SCStudyInterfaceRef);

#pragma pack(push, 8)


/****************************************************************************/

struct s_TimeAndSales
{
	SCDateTimeMS DateTime;
	float Price;
	uint32_t Volume;
	float Bid;
	float Ask;
	uint32_t BidSize;
	uint32_t AskSize;

	//This will always be a value >= 1.  It is unlikely to wrap around, but it could.  It will never be 0.
	uint32_t Sequence;

	int16_t UnbundledTradeIndicator;
	int16_t Type;
	uint32_t TotalBidDepth;
	uint32_t TotalAskDepth;
	
	s_TimeAndSales()
	{
		Clear();
	}

	explicit s_TimeAndSales(int i)
	{
		Clear();
	}
	
	s_TimeAndSales& operator *= (float Multiplier)
	{
		Price *= Multiplier;
		Bid *= Multiplier;
		Ask *= Multiplier;
		
		return *this;
	}

	void Clear() 
	{
		memset(this, 0, sizeof(s_TimeAndSales));
	}

};

class c_SCTimeAndSalesArray
{
	private:

	protected:
		const int m_Version;
		
		s_TimeAndSales* m_Data;
		int m_MaxSize;
		int m_NextIndex;
		int m_StartAtNext;
		
	public:
		/*==================================================================*/
		c_SCTimeAndSalesArray()
			: m_Version(1)
			
			, m_Data(NULL)
			, m_MaxSize(0)
			, m_NextIndex(0)
			, m_StartAtNext(0)
		{
		}
		
		/*==================================================================*/
		~c_SCTimeAndSalesArray()
		{
		}
		
		/*==================================================================*/
		//Thread safe as long as an instance of this object is only used by one thread at a time.
		int IsCreated() const
		{
			return (m_Data != NULL) ? 1 : 0;
		}
		
		/*==================================================================*/
		int MaxSize() const
		{
			return m_MaxSize;
		}
		
		/*==================================================================*/
		int Size() const
		{
			if (m_Data == NULL)
				return 0;
			
			if (m_StartAtNext)
				return m_MaxSize;  // Using the full buffer
			else
				return m_NextIndex;  // Using only up to m_NextIndex
		}
		
		/*====================================================================
			This function is for backwards compatibility.
		--------------------------------------------------------------------*/
		int GetArraySize() const
		{
			return Size();
		}
		
		/*====================================================================
			The oldest element is at index 0, and the newest element is at
			index Size()-1.
		--------------------------------------------------------------------*/
		const s_TimeAndSales& operator [] (int Index) const
		{
			static s_TimeAndSales DefaultElement;

			if (m_Data == NULL)
				return DefaultElement;
			
			if (Index < 0 || Index >= Size())
				return DefaultElement;
			
			// Translate the index
			if (m_StartAtNext)
			{
				Index += m_NextIndex;
				
				if (Index >= m_MaxSize)
					Index -= m_MaxSize;
			}
			
			return m_Data[Index];
		}
		
		/*====================================================================
			This does not copy the data, it just copies the pointer from the
			given Time and Sales array.
		--------------------------------------------------------------------*/
		c_SCTimeAndSalesArray& operator = (const c_SCTimeAndSalesArray& Rhs)
		{
			if (&Rhs == this)
				return *this;
			
			m_Data = Rhs.m_Data;
			m_MaxSize = Rhs.m_MaxSize;
			m_NextIndex = Rhs.m_NextIndex;
			m_StartAtNext = Rhs.m_StartAtNext;
			
			return *this;
		}

		bool IsRecordIndexTrade (int Index)
		{
			return (operator[]( Index).Type == SC_TS_ASK  || operator[](Index).Type  == SC_TS_BID );
		}

		SCDateTimeMS GetDateTimeOfLastTradeRecord()
		{
			int ArraySize = Size();
			if (ArraySize == 0)
				return 0.0;

			for (int RecordIndex = ArraySize - 1; RecordIndex >= 0; RecordIndex--)
			{
				if (IsRecordIndexTrade(RecordIndex))
					return operator[](RecordIndex).DateTime;
			}

			return 0.0;
		}

		const s_TimeAndSales& GetLessEqualRecordAtDateTime(const SCDateTimeMS& SearchDateTime) const
		{
			static s_TimeAndSales DefaultElement;

			if (Size() == 0)
				return DefaultElement;

			//Perform binary search using operator[] to locate search date-time
			//Elements are in time-ascending order

			int IndexUpper = 0;
			int IndexLower = IndexUpper;
			const size_t EndIndex = Size();
			int Count = Size();

			while (Count > 0)
			{
				const int Step = Count / 2;
				const int TestIndex = IndexUpper + Step;

				if (operator[](TestIndex).DateTime <= SearchDateTime)
				{
					IndexLower = TestIndex;
					IndexUpper = TestIndex + 1;
					Count -= Step + 1;
				}
				else
				{
					Count = Step;
				}
			}

			return operator[](IndexLower);
		}
};



/****************************************************************************/

class s_UseTool
{
	private:
		unsigned int Size;
	public:
		int ChartNumber;
		int Tool_Unused;
		int LineNumber;
		int ExtendLeft;  
		int ExtendRight;  
		int TransparencyLevel;  
		int FourthIndex;  
		float BeginValue;
		float EndValue;
		int Region;
		COLORREF Color;
		char* Old_Text;
		int FontSize;
		char ShowBeginMark;
		char ShowEndMark;
		int16_t IsSelected;
		float RetracementLevels[ACSIL_DRAWING_MAX_LEVELS];
		SCDateTimeMS BeginDateTime;
		SCDateTimeMS EndDateTime;
		SCString Text;
		
		DrawingTypeEnum DrawingType;
		uint16_t LineWidth;
		SubgraphLineStyles LineStyle;
		
		int AddMethod;  // UseToolAddMethodEnum

	private:
		unsigned int Version;
	public:
		int ReverseTextColor;

		unsigned int FontBackColor;
		SCString FontFace;
		int FontBold;
		int FontItalic;
		int TextAlignment;
		int UseRelativeVerticalValues;

		COLORREF SecondaryColor; // For Highlight tools
		int SquareEdge;

		int DisplayHorizontalLineValue;

		SCDateTimeMS  ThirdDateTime;
		float ThirdValue;

		int		BeginIndex;
		int		EndIndex;
		int		ThirdIndex;

		COLORREF LevelColor[ACSIL_DRAWING_MAX_LEVELS];
		int      LevelStyle[ACSIL_DRAWING_MAX_LEVELS];
		int      LevelWidth[ACSIL_DRAWING_MAX_LEVELS];

		int ShowTickDifference;		// Calculator tool
		int ShowCurrencyValue;		// Calculator tool
		int ShowPriceDifference;	// Calculator tool
		int ShowPercentChange;		// Calculator tool
		int ShowTimeDifference;		// Calculator tool
		int ShowNumberOfBars;		// Calculator tool
		int ShowAngle;				// Calculator tool
		int ShowEndPointPrice;		// Calculator tool
		int MultiLineLabel;			// Calculator and Text tools

		int ShowEndPointDateTime;	// vertical line, Calculator tool
		int ShowEndPointDate;		// vertical line, Calculator tool
		int ShowEndPointTime;		// vertical line, Calculator tool

		int ShowPercent;			// Retracement
		int ShowPrice;				// Retracement
		int RoundToTickSize;		// Retracement

		int FixedSizeArrowHead;
		float ArrowHeadSize;

		int MarkerType;
		int MarkerSize;
		int UseBarSpacingForMarkerSize;

		int TimeExpVerticals;
		int TimeExpTopLabel1;
		int TimeExpTopLabel2;
		int TimeExpBottomLabel1;
		int TimeExpBottomLabel2;
		int TimeExpBasedOnTime;

		int TransparentLabelBackground;
		int AddAsUserDrawnDrawing;

		int ShowVolume;				// Calculator tool
		int DrawFromEnd;
		int DrawUnderneathMainGraph;

		// 1087
		int ShowLabels;
		int ShowLabelsAtEnd;
		int AllLevelsSameColorStyle;

		// 1108
		int UseToolConfigNum;

		// 1218
		int ShowAskBidDiffVolume; // Calculator tool

		// 1247
		int FlatEdge;

		// 1262
		int DrawWithinRegion;

		// 1288
		int VerticalText;

		// 1291
		COLORREF TextColor;

		// 1420
		int CenterText;
		int CenterPriceLabels;

		// 1446
		int NoVerticalOutline;

		// 1480
		int AllowSaveToChartbook;

		// 1530
		int AssociatedStudyID;

		// 1590
		int16_t HideDrawing;
		
		// 1640
		int16_t ClearExistingText;

		//1644
		int16_t LockDrawing;

		//1670
		int16_t DrawOutlineOnly;

		// 1746
		int16_t NumCycles;

		//1889
		float ExtendMultiplier;

		//1909
		int16_t DrawMidline = -1;

		// Note: When adding new members, remember to update CopyAndUpdateUseToolStruct function.

		s_UseTool()
		{
			Clear();
		}
		
		void Clear()
		{
			Text.Initialize();

			Size = sizeof(s_UseTool);

			ChartNumber = 0;

			Version = SC_DLL_VERSION;
			Tool_Unused = 0;

			LineNumber = -1;

			AddMethod = UTAM_ADD_OR_ADJUST;
			BeginValue = FLT_MIN;
			EndValue = FLT_MIN;
			ThirdValue = FLT_MIN;

			ExtendLeft = -1;  
			ExtendRight = -1;  

			FourthIndex = -1;

			Region = -1;
			FontSize = -1;
			ShowBeginMark = -1;
			ShowEndMark = -1;
			ReverseTextColor = -1;
			Color = 0xff000000;
			LineWidth = 0xffff;
			LineStyle = LINESTYLE_UNSET;
			FontBackColor = 0xff000000;
			FontFace.Initialize();
			FontBold = -1;
			FontItalic = -1;
			TextAlignment = -1;
			UseRelativeVerticalValues = -1;
			SecondaryColor = 0xff000000;
			TextColor = 0xff000000;

			DisplayHorizontalLineValue = -1;
			TransparencyLevel = -1;

			for (int Index = 0 ; Index < ACSIL_DRAWING_MAX_LEVELS; ++Index)
			{
				RetracementLevels[Index] = -FLT_MAX;
				LevelColor[Index] = 0xff000000;
				LevelStyle[Index] = -1;
				LevelWidth[Index] = -1;
			}

			ShowTickDifference = -1;
			ShowCurrencyValue = -1;
			ShowPriceDifference = -1;
			ShowPercentChange = -1;
			ShowTimeDifference = -1;
			ShowNumberOfBars = -1;
			ShowAngle = -1;
			ShowVolume = -1;
			ShowAskBidDiffVolume = -1;
			ShowEndPointPrice = -1;
			DrawFromEnd = -1;
			MultiLineLabel = -1;
			DrawUnderneathMainGraph = -1;
			DrawWithinRegion = -1;
			VerticalText = -1;

			ShowLabels = -1;
			ShowLabelsAtEnd = -1;
			AllLevelsSameColorStyle = -1;

			ShowEndPointDateTime = -1;
			ShowEndPointDate = -1;
			ShowEndPointTime = -1;

			ShowPercent = -1;
			ShowPrice = -1;
			CenterText = -1;
			CenterPriceLabels = -1;
			NoVerticalOutline = -1;
			RoundToTickSize = -1;

			FixedSizeArrowHead = -1;
			ArrowHeadSize = -FLT_MAX;

			MarkerType = -1;
			MarkerSize = -1;
			UseBarSpacingForMarkerSize = -1;

			TimeExpVerticals = -1;
			TimeExpTopLabel1 = -1;
			TimeExpTopLabel2 = -1;
			TimeExpBottomLabel1 = -1;
			TimeExpBottomLabel2 = -1;
			TimeExpBasedOnTime = -1;

			TransparentLabelBackground = -1;

			BeginIndex = -1;
			EndIndex = -1;
			ThirdIndex = -1;

			AddAsUserDrawnDrawing = 0;

			UseToolConfigNum = -1;
			
			Old_Text = NULL;

			IsSelected = 0;
			BeginDateTime.Clear();
			EndDateTime.Clear();

			DrawingType = DRAWING_UNKNOWN;

			SquareEdge = -1;
			FlatEdge = -1;
			ThirdDateTime.Clear();
			AllowSaveToChartbook = 0;
			AssociatedStudyID = 0;
			HideDrawing = -1;
			ClearExistingText = 0;
			LockDrawing = -1;
			DrawOutlineOnly = -1;
			NumCycles = -1;
			ExtendMultiplier = -1;
			DrawMidline = -1;
		}
		
		unsigned int GetSize() const
		{
			return Size;
		}
		unsigned int GetVersion() const
		{
			return Version;
		}
};


/****************************************************************************/
struct s_ACSTradeStatistics
{
	double  ClosedProfitLoss() { return ClosedProfit + ClosedLoss; }
	double  ClosedProfit;
	double  ClosedLoss;
	double  ProfitFactor() { return ClosedLoss == 0 ? 0 : fabs(ClosedProfit / ClosedLoss); }
	double  TotalCommission;

	double  MaximumRunup;
	double  MaximumDrawdown;

	double  MaximumTradeRunup;
	double  MaximumTradeDrawdown;
	double  MaximumOpenPositionProfit;
	double  MaximumOpenPositionLoss;

	int		TotalTrades() { return TotalWinningTrades + TotalLosingTrades; }
	int		TotalWinningTrades;
	int		TotalLosingTrades;
	double  TotalPercentProfitable() { return TotalTrades() == 0 ? 0 : (double)TotalWinningTrades / (double)TotalTrades(); }
	int		TotalLongTrades;
	int		TotalShortTrades;

	t_OrderQuantity32_64 TotalClosedQuantity() { return TotalWinningQuantity + TotalLosingQuantity; }
	t_OrderQuantity32_64 TotalWinningQuantity;
	t_OrderQuantity32_64 TotalLosingQuantity;
	t_OrderQuantity32_64 TotalFilledQuantity;

	t_OrderQuantity32_64 LargestTradeQuantity;

	double  AvgQuantityPerTrade() { return TotalTrades() == 0 ? 0 : (double)TotalClosedQuantity() / (double)TotalTrades(); }
	double  AvgQuantityPerWinningTrade() { return TotalWinningTrades == 0 ? 0 : (double)TotalWinningQuantity / (double)TotalWinningTrades; }
	double  AvgQuantityPerLosingTrade() { return TotalLosingTrades == 0 ? 0 : (double)TotalLosingQuantity / (double)TotalLosingTrades; }

	double  AvgProfitLoss() { return TotalTrades() == 0 ? 0 : ClosedProfitLoss() / TotalTrades(); }
	double  AvgProfit() { return TotalWinningTrades == 0 ? 0 : ClosedProfit / TotalWinningTrades; }
	double  AvgLoss() { return TotalLosingTrades == 0 ? 0 : ClosedLoss / TotalLosingTrades; }
	double  AvgProfitFactor() { return AvgLoss() == 0 ? 0 : fabs(AvgProfit() / AvgLoss()); }

	double  LargestWinningTrade;
	double  LargestLosingTrade;
	double  LargestWinnerPercentOfProfit() { return ClosedProfit == 0 ? 0 : LargestWinningTrade / ClosedProfit; }
	double  LargestLoserPercentOfLoss() { return ClosedLoss == 0 ? 0 : fabs(LargestLosingTrade / ClosedLoss); }

	int		TimeInTrades() { return TimeInWinningTrades + TimeInLosingTrades; }
	int		TimeInWinningTrades;
	int		TimeInLosingTrades;

	int     AvgTimeInTrade() { return TotalTrades() == 0 ? 0 : TimeInTrades() / TotalTrades(); }
	int     AvgTimeInWinningTrade() { return TotalWinningTrades == 0 ? 0 : TimeInWinningTrades / TotalWinningTrades; }
	int     AvgTimeInLosingTrade() { return TotalLosingTrades == 0 ? 0 : TimeInLosingTrades / TotalLosingTrades; }
	
	int		MaxConsecutiveWinners;
	int		MaxConsecutiveLosers;

	// 1383
	static const int CURRENT_TRADESTATS_VERSION = 3;

	int			Version;
	double		LastTradeProfitLoss;
	t_OrderQuantity32_64	LastTradeQuantity;
	SCDateTime	LastFillDateTime;
	SCDateTime	LastEntryDateTime;
	SCDateTime	LastExitDateTime;

	// 1696
	t_OrderQuantity32_64 TotalBuyQuantity;
	t_OrderQuantity32_64 TotalSellQuantity;

	s_ACSTradeStatistics()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(*this));

		Version = CURRENT_TRADESTATS_VERSION;
	}
};


/****************************************************************************/
struct s_ACSTrade
{

	SCDateTime	OpenDateTime;
	SCDateTime	CloseDateTime;
	int			TradeType;			// +1=long, -1=short
	t_OrderQuantity32_64	EntryQuantity;
	t_OrderQuantity32_64	ExitQuantity;
	t_OrderQuantity32_64	MaxOpenQuantity;
	double		AverageEntryPrice;
	double		AverageExitPrice;
	double		OpenProfitLoss;
	double		ClosedProfitLoss;
	double		MaximumOpenPositionLoss;
	double		MaximumOpenPositionProfit;
	double		FlatToFlatMaximumOpenPositionProfit;
	double		FlatToFlatMaximumOpenPositionLoss;
	double		Commission;

	s_ACSTrade()
	{
		Clear();
	}

	void Clear()
	{
		OpenDateTime = 0;
		CloseDateTime = 0;
		TradeType = 0;
		EntryQuantity = 0;
		ExitQuantity = 0;
		MaxOpenQuantity = 0;
		AverageEntryPrice = 0;
		AverageExitPrice = 0;
		OpenProfitLoss = 0;
		ClosedProfitLoss = 0;
		MaximumOpenPositionLoss = 0;
		MaximumOpenPositionProfit = 0;
		FlatToFlatMaximumOpenPositionProfit = 0;
		FlatToFlatMaximumOpenPositionLoss = 0;
		Commission = 0;
	}
};

/****************************************************************************/
struct s_SCOrderFillData
{
	int	Version;

	SCString Symbol;
	SCString TradeAccount;
	uint32_t InternalOrderID;
	SCDateTime FillDateTime;	// adjusted to chart time zone
	BuySellEnum	BuySell;
	t_OrderQuantity32_64 Quantity;
	double FillPrice;

	//Version 2
	SCString FillExecutionServiceID;

	//Version 3
	t_OrderQuantity32_64 TradePositionQuantity;

	//Version 4
	int IsSimulated;

	s_SCOrderFillData()
	{
		Clear();
	}

	void Clear()
	{
		Version = CURRENT_SC_ORDER_FILL_DATA_VERSION;
		Symbol.Clear();
		TradeAccount.Clear();
		InternalOrderID = 0;
		FillDateTime = 0.0;
		BuySell = BSE_UNDEFINED;
		Quantity= 0;
		FillPrice= 0.0;
		FillExecutionServiceID = "";
		TradePositionQuantity = 0;
		IsSimulated = 0;
	}
};

/****************************************************************************/
struct s_SCNewOrder
{
	int Version;
	
	int OrderType;

	t_OrderQuantity32_64 OrderQuantity;
	
	double Price1;
	double Price2;

	uint32_t InternalOrderID;

	uint32_t InternalOrderID2;

	SCString TextTag;
	SCTimeInForceEnum TimeInForce;

	double Target1Offset;
	uint32_t Target1InternalOrderID;
	double Stop1Offset;
	uint32_t Stop1InternalOrderID;
	t_OrderQuantity32_64 OCOGroup1Quantity;

	//These depend upon the parent order price or the current price in the case of a market order
	double Target1Price;
	double Stop1Price;

	double Target2Offset;
	uint32_t Target2InternalOrderID;
	double Stop2Offset;
	uint32_t Stop2InternalOrderID;
	t_OrderQuantity32_64 OCOGroup2Quantity;
	double Target2Price;
	double Stop2Price;

	double Target3Offset;
	uint32_t Target3InternalOrderID;
	double Stop3Offset;
	uint32_t Stop3InternalOrderID;
	t_OrderQuantity32_64 OCOGroup3Quantity;
	double Target3Price;
	double Stop3Price;
	

	double Target4Offset;
	uint32_t Target4InternalOrderID;
	double Stop4Offset;
	uint32_t Stop4InternalOrderID;
	t_OrderQuantity32_64 OCOGroup4Quantity;
	double Target4Price;
	double Stop4Price;

	double Target5Offset;
	uint32_t Target5InternalOrderID;
	double Stop5Offset;
	uint32_t Stop5InternalOrderID;
	t_OrderQuantity32_64 OCOGroup5Quantity;
	double Target5Price;
	double Stop5Price;

	double StopAllOffset;
	uint32_t StopAllInternalOrderID;
	double StopAllPrice;

	double MaximumChaseAsPrice;
	
	char AttachedOrderTarget1Type;
	char AttachedOrderTarget2Type;
	char AttachedOrderTarget3Type;
	char AttachedOrderTarget4Type;
	char AttachedOrderTarget5Type;


	char AttachedOrderStop1Type;
	char AttachedOrderStop2Type;
	char AttachedOrderStop3Type;
	char AttachedOrderStop4Type;
	char AttachedOrderStop5Type;
	char AttachedOrderStopAllType;

	double AttachedOrderMaximumChase;
	double TrailStopStepPriceAmount;
	double TriggeredTrailStopTriggerPriceOffset;
	double TriggeredTrailStopTrailPriceOffset;
	
	struct s_MoveToBreakEven
	{
		int Type;
		int BreakEvenLevelOffsetInTicks;
		int TriggerOffsetInTicks;
		int TriggerOCOGroup;
	};

	//The move to breakeven settings will apply to all stop attached orders specified
	s_MoveToBreakEven MoveToBreakEven;

	double  StopLimitOrderLimitOffset;

	uint32_t Target1InternalOrderID_2;
	uint32_t Stop1InternalOrderID_2;
	uint32_t Target2InternalOrderID_2;
	uint32_t Stop2InternalOrderID_2;
	uint32_t Target3InternalOrderID_2;
	uint32_t Stop3InternalOrderID_2;
	uint32_t Target4InternalOrderID_2;
	uint32_t Stop4InternalOrderID_2;
	uint32_t Target5InternalOrderID_2;
	uint32_t Stop5InternalOrderID_2;
	uint32_t StopAllInternalOrderID_2;

	double Target1Offset_2;
	double Target1Price_2;
	double Stop1Offset_2;
	double Stop1Price_2;

	double Target2Offset_2;
	double Target2Price_2;
	double Stop2Offset_2;
	double Stop2Price_2;

	double Target3Offset_2;
	double Target3Price_2;
	double Stop3Offset_2;
	double Stop3Price_2;

	double Target4Offset_2;
	double Target4Price_2;
	double Stop4Offset_2;
	double Stop4Price_2;

	double Target5Offset_2;
	double Target5Price_2;
	double Stop5Offset_2;
	double Stop5Price_2;

	double StopAllOffset_2;
	double StopAllPrice_2;

	SCString Symbol;
	SCString TradeAccount;

	// Version 13
	double StopLimitOrderLimitOffsetForAttachedOrders;

	// Version 14
	int32_t SubmitAsHeldOrder;

	// Version 15
	double QuantityTriggeredAttachedStop_QuantityForTrigger;

	//Version 16
	double AttachedOrderStop1_TriggeredTrailStopTriggerPriceOffset;
	double AttachedOrderStop1_TriggeredTrailStopTrailPriceOffset;
	double AttachedOrderStop2_TriggeredTrailStopTriggerPriceOffset;
	double AttachedOrderStop2_TriggeredTrailStopTrailPriceOffset;
	double AttachedOrderStop3_TriggeredTrailStopTriggerPriceOffset;
	double AttachedOrderStop3_TriggeredTrailStopTrailPriceOffset;
	double AttachedOrderStop4_TriggeredTrailStopTriggerPriceOffset;
	double AttachedOrderStop4_TriggeredTrailStopTrailPriceOffset;
	double AttachedOrderStop5_TriggeredTrailStopTriggerPriceOffset;
	double AttachedOrderStop5_TriggeredTrailStopTrailPriceOffset;

	//Version 17
	double TriggeredLimitOrStopAttachedOrderTriggerOffset;//As a price offset

	s_SCNewOrder()
		: Version(CURRENT_SC_NEW_ORDER_VERSION)
		, OrderType(SCT_ORDERTYPE_MARKET)
		, OrderQuantity(0)
		, Price1(DBL_MAX)
		, Price2(DBL_MAX)
		, InternalOrderID(0)
		, InternalOrderID2(0)
		, TextTag()
		, TimeInForce(SCT_TIF_UNSET)

		, Target1Offset(0.0)
		, Target1InternalOrderID(0)
		, Stop1Offset(0.0)
		, Stop1InternalOrderID(0)
		, OCOGroup1Quantity(0)
		, Target1Price(0.0)
		, Stop1Price(0.0)

		, Target2Offset(0.0)
		, Target2InternalOrderID(0)
		, Stop2Offset(0.0)
		, Stop2InternalOrderID(0)
		, OCOGroup2Quantity(0)
		, Target2Price(0.0)
		, Stop2Price(0.0)

		, Target3Offset(0.0)
		, Target3InternalOrderID(0)
		, Stop3Offset(0.0)
		, Stop3InternalOrderID(0)
		, OCOGroup3Quantity(0)
		, Target3Price(0.0)
		, Stop3Price(0.0)

		, Target4Offset(0.0)
		, Target4InternalOrderID(0)
		, Stop4Offset(0.0)
		, Stop4InternalOrderID(0)
		, OCOGroup4Quantity(0)
		, Target4Price(0.0)
		, Stop4Price(0.0)

		, Target5Offset(0.0)
		, Target5InternalOrderID(0)
		, Stop5Offset(0.0)
		, Stop5InternalOrderID(0)
		, OCOGroup5Quantity(0)
		, Target5Price(0.0)
		, Stop5Price(0.0)

		, StopAllOffset(0.0)
		, StopAllInternalOrderID(0)
		, StopAllPrice(0.0)

		, MaximumChaseAsPrice(0.0)

		, AttachedOrderTarget1Type(SCT_ORDERTYPE_LIMIT)
		, AttachedOrderTarget2Type(SCT_ORDERTYPE_LIMIT)
		, AttachedOrderTarget3Type(SCT_ORDERTYPE_LIMIT)
		, AttachedOrderTarget4Type(SCT_ORDERTYPE_LIMIT)
		, AttachedOrderTarget5Type(SCT_ORDERTYPE_LIMIT)

		, AttachedOrderStop1Type(SCT_ORDERTYPE_STOP)
		, AttachedOrderStop2Type(SCT_ORDERTYPE_STOP)
		, AttachedOrderStop3Type(SCT_ORDERTYPE_STOP)
		, AttachedOrderStop4Type(SCT_ORDERTYPE_STOP)
		, AttachedOrderStop5Type(SCT_ORDERTYPE_STOP)
		, AttachedOrderStopAllType(SCT_ORDERTYPE_STOP)

		, AttachedOrderMaximumChase(0.0)
		, TrailStopStepPriceAmount(0.0)
		, TriggeredTrailStopTriggerPriceOffset(0.0)
		, TriggeredTrailStopTrailPriceOffset(0.0)

		, StopLimitOrderLimitOffset(DBL_MAX)

		, Target1InternalOrderID_2(0)
		, Stop1InternalOrderID_2(0)
		, Target2InternalOrderID_2(0)
		, Stop2InternalOrderID_2(0)
		, Target3InternalOrderID_2(0)
		, Stop3InternalOrderID_2(0)
		, Target4InternalOrderID_2(0)
		, Stop4InternalOrderID_2(0)
		, Target5InternalOrderID_2(0)
		, Stop5InternalOrderID_2(0)
		, StopAllInternalOrderID_2(0)

		, Target1Offset_2(0.0)
		, Target1Price_2(0.0)
		, Stop1Offset_2(0.0)
		, Stop1Price_2(0.0)

		, Target2Offset_2(0.0)
		, Target2Price_2(0.0)
		, Stop2Offset_2(0.0)
		, Stop2Price_2(0.0)

		, Target3Offset_2(0.0)
		, Target3Price_2(0.0)
		, Stop3Offset_2(0.0)
		, Stop3Price_2(0.0)

		, Target4Offset_2(0.0)
		, Target4Price_2(0.0)
		, Stop4Offset_2(0.0)
		, Stop4Price_2(0.0)

		, Target5Offset_2(0.0)
		, Target5Price_2(0.0)
		, Stop5Offset_2(0.0)
		, Stop5Price_2(0.0)

		, StopAllOffset_2(0.0)
		, StopAllPrice_2(0.0)

		, Symbol()
		, TradeAccount()

		, StopLimitOrderLimitOffsetForAttachedOrders(DBL_MAX)
		, SubmitAsHeldOrder(0)

		, QuantityTriggeredAttachedStop_QuantityForTrigger(0.0)
		, AttachedOrderStop1_TriggeredTrailStopTriggerPriceOffset(0)
		, AttachedOrderStop1_TriggeredTrailStopTrailPriceOffset(0)
		, AttachedOrderStop2_TriggeredTrailStopTriggerPriceOffset(0)
		, AttachedOrderStop2_TriggeredTrailStopTrailPriceOffset(0)
		, AttachedOrderStop3_TriggeredTrailStopTriggerPriceOffset(0)
		, AttachedOrderStop3_TriggeredTrailStopTrailPriceOffset(0)
		, AttachedOrderStop4_TriggeredTrailStopTriggerPriceOffset(0)
		, AttachedOrderStop4_TriggeredTrailStopTrailPriceOffset(0)
		, AttachedOrderStop5_TriggeredTrailStopTriggerPriceOffset(0)
		, AttachedOrderStop5_TriggeredTrailStopTrailPriceOffset(0)
		, TriggeredLimitOrStopAttachedOrderTriggerOffset(0)
	{
		memset(&MoveToBreakEven, 0, sizeof(MoveToBreakEven));
		MoveToBreakEven.Type = MOVETO_BE_ACTION_TYPE_NONE;
	}

	void Reset()
	{
		*this = s_SCNewOrder();
	}

	bool IsPrice1Set() const
	{
		return (Price1 != DBL_MAX);
	}

	bool IsPrice2Set() const
	{
		return (Price2 != DBL_MAX);
	}

	bool AreAttachedOrderVariablesSet() const
	{
		return Target1Offset != 0.0
			|| Stop1Offset != 0.0
			|| Target1Price != 0.0
			|| Stop1Price != 0.0
			|| StopAllOffset != 0.0;
	}
};

/****************************************************************************/

struct s_SCTradeOrder
{
	int Version;
	
	// Version 2
	// This cannot be made a 64-bit integer which is internally used in Sierra Chart. We can only typecast to/from the 64-bit used internally in Sierra Chart.
	uint32_t InternalOrderID;
	SCString Symbol;
	SCString OrderType;
	t_OrderQuantity32_64 OrderQuantity;
	SCString TextTag;
#ifndef _WIN64
	SCString Unused_2;
#endif
	double Price1;
	double Price2;
	double AvgFillPrice;
	int SourceChartNumber;
	
	// Version 3 (also changed in version 7)
	SCOrderStatusCodeEnum OrderStatusCode;
	
	// Version 4
	t_OrderQuantity32_64 FilledQuantity;
	
	// Version 5
	int ParentInternalOrderID;
	int LinkID;
	
	// Version 6
	SCDateTime LastActivityTime;
	
	// Version 7 - nothing new
	
	// Version 8
	OpenCloseEnum OpenClose;
	BuySellEnum BuySell;

	// Version 9
	SCDateTime EntryDateTime;
	
	// Version 10
	int OrderTypeAsInt;

	// Version 11
	t_OrderQuantity32_64 LastModifyQuantity;
	double LastModifyPrice1;

	// Version 12
	double LastFillPrice;
	t_OrderQuantity32_64 LastFillQuantity;

	//Version 13
	SCString SourceChartbookFileName;

	//Version 14
	int IsSimulated;

	//Version 15
	uint64_t TargetChildInternalOrderID;
	uint64_t StopChildInternalOrderID;
	uint64_t OCOSiblingInternalOrderID;

	int32_t EstimatedPositionInQueue;
	
	s_SCTradeOrder()
		: Version(16)
		
		, InternalOrderID(0)
		, OrderQuantity(0)
		, Price1(0.0)
		, Price2(0.0)
		, AvgFillPrice(0.0)
		, SourceChartNumber(0)
		
		, OrderStatusCode(SCT_OSC_UNSPECIFIED)

		, FilledQuantity(0)
		
		, ParentInternalOrderID(0)
		, LinkID(0)
		
		, OpenClose(OCE_UNDEFINED)
		, BuySell(BSE_UNDEFINED)
		, OrderTypeAsInt(0)
		, LastModifyQuantity(0)
		, LastModifyPrice1(0.0)

		, LastFillPrice(0.0)
		, LastFillQuantity(0)
		, IsSimulated(0)

		, TargetChildInternalOrderID(0)
		, StopChildInternalOrderID(0)
		, OCOSiblingInternalOrderID(0)
		, EstimatedPositionInQueue(0)
	{
	}

	bool IsWorking()
	{
		return IsWorkingOrderStatus(OrderStatusCode);
	}

	bool IsLimitOrder()
	{
		return (OrderTypeAsInt ==  SCT_ORDERTYPE_LIMIT 
			|| OrderTypeAsInt == SCT_ORDERTYPE_MARKET_IF_TOUCHED 
			|| OrderTypeAsInt == SCT_ORDERTYPE_LIMIT_CHASE 
			|| OrderTypeAsInt == SCT_ORDERTYPE_LIMIT_TOUCH_CHASE);
	}

	bool IsAttachedOrder()
	{
		return ParentInternalOrderID != 0;
	}
};

/****************************************************************************/
struct s_SCPositionData
{
	static const int CURRENT_POSITION_DATA_VERSION = 12;

	int Version;

	// Version 2
	SCString Symbol;
	t_OrderQuantity32_64 PositionQuantity;
	double AveragePrice;
	double OpenProfitLoss;
	double CumulativeProfitLoss;//Stats
	int WinTrades;//Stats
	int LoseTrades;//Stats
	int TotalTrades;//Stats
	double MaximumOpenPositionLoss;
	double MaximumOpenPositionProfit;
	double LastTradeProfitLoss;//Stats
	
	// Version 3
	t_OrderQuantity32_64 PositionQuantityWithAllWorkingOrders;
	t_OrderQuantity32_64 PositionQuantityWithExitWorkingOrders;
	int WorkingOrdersExist;

	// Version 4
	double DailyProfitLoss;//Stats

	// Version 5
	SCDateTime LastFillDateTime;//Stats
	SCDateTime LastEntryDateTime;//Stats
	SCDateTime LastExitDateTime;//Stats

	// Version 6
	double PriceHighDuringPosition;
	t_OrderQuantity32_64 DailyTotalQuantityFilled;//Stats
	double PriceLowDuringPosition;
	t_OrderQuantity32_64 PriorPositionQuantity;
	t_OrderQuantity32_64 PositionQuantityWithExitMarketOrders;

	//Version 7
	t_OrderQuantity32_64 TotalQuantityFilled;//Stats

	//Version 8
	t_OrderQuantity32_64 LastTradeQuantity;//Stats

	//Version 9
	double TradeStatsDailyProfitLoss;//Stats
	t_OrderQuantity32_64 TradeStatsDailyClosedQuantity;//Stats
	double TradeStatsOpenProfitLoss;
	t_OrderQuantity32_64 TradeStatsOpenQuantity;

	// Version 10
	int NonAttachedWorkingOrdersExist;

	// Version 11
	SCString TradeAccount;

	//Version 12
	t_OrderQuantity32_64 PositionQuantityWithMarketWorkingOrders;
	t_OrderQuantity32_64 PositionQuantityWithAllWorkingOrdersExceptNonMarketExits;
	t_OrderQuantity32_64 PositionQuantityWithAllWorkingOrdersExceptAllExits;
	t_OrderQuantity32_64 AllWorkingBuyOrdersQuantity;
	t_OrderQuantity32_64 AllWorkingSellOrdersQuantity;

	s_SCPositionData()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(*this));

		Version = CURRENT_POSITION_DATA_VERSION;
	}
};

/****************************************************************************/

struct s_GetOHLCOfTimePeriod
{
	s_GetOHLCOfTimePeriod(SCDateTime vStartDateTime, SCDateTime vEndDateTime, float& vOpen, float& vHigh, float& vLow, float& vClose, float& vNextOpen, int& vNumberOfBars, SCDateTime& vTotalTimeSpan)
	:	Open(vOpen), 
		High(vHigh), 
		Low(vLow), 
		Close(vClose), 
		NextOpen(vNextOpen), 
		NumberOfBars(vNumberOfBars), 
		TotalTimeSpan(vTotalTimeSpan)
	{
		StartDateTime = vStartDateTime;
		EndDateTime = vEndDateTime;
	}

	SCDateTime StartDateTime;
	SCDateTime EndDateTime;
	float& Open;
	float& High;
	float& Low;
	float& Close;
	float& NextOpen;
	int& NumberOfBars;
	SCDateTime& TotalTimeSpan;
};

/****************************************************************************/



typedef void (SCDLLCALL* fp_SCDLLOnArrayUsed)(int);

template <typename T> class c_ArrayWrapper;
template <typename T> class c_ConstArrayWrapper;

template <typename T>
class c_ArrayWrapper
{
	private:
		T* m_Data;
		int m_NumElements;
		
		fp_SCDLLOnArrayUsed m_fp_OnArrayUsed;
		int m_OnArrayUsedParam;
		
		int m_NumExtendedElements;
		
		char Reserved[44]; 
		
		T m_DefaultElement;
		
	public:
		c_ArrayWrapper()
		{
			ResetMembers();
		}
		explicit c_ArrayWrapper(int i)
		{
			ResetMembers();
		}
		c_ArrayWrapper(const c_ArrayWrapper& Src)
			: m_Data(Src.m_Data)
			, m_NumElements(Src.m_NumElements)
			, m_NumExtendedElements(Src.m_NumExtendedElements)
			, m_fp_OnArrayUsed(Src.m_fp_OnArrayUsed)
			, m_OnArrayUsedParam(Src.m_OnArrayUsedParam)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		
		const c_ArrayWrapper& operator = (const c_ArrayWrapper& Src)
		{
			m_Data = Src.m_Data;
			m_NumElements = Src.m_NumElements;
			m_NumExtendedElements = Src.m_NumExtendedElements;
			m_fp_OnArrayUsed = Src.m_fp_OnArrayUsed;
			m_OnArrayUsedParam = Src.m_OnArrayUsedParam;
			m_DefaultElement = Src.m_DefaultElement;
			
			return *this;
		}

		void ResetMembers()
		{
			m_Data = NULL;
			m_NumElements = 0;
			m_NumExtendedElements = 0;
			m_fp_OnArrayUsed = NULL;
			m_OnArrayUsedParam = 0;
			m_DefaultElement = T(0);
			
			memset(Reserved, 0, sizeof(Reserved));
		}
		
		T& operator [] (int Index)
		{
			return ElementAt(Index);
		}
		
		const T& operator [] (int Index) const
		{
			return ElementAt(Index);
		}
		
		T& ElementAt(int Index)
		{
			if (m_Data == NULL && m_fp_OnArrayUsed != NULL)
				m_fp_OnArrayUsed(m_OnArrayUsedParam);  // This will allocate and set up the array if it can
			
			if (m_Data == NULL)
				return m_DefaultElement;
			
			int ExtendedArraySize = GetExtendedArraySize();
			if (ExtendedArraySize == 0)
				return m_DefaultElement;
			
			if (Index < 0)
				Index = 0;
			if (Index >= ExtendedArraySize)
				Index = ExtendedArraySize - 1;
			
			return m_Data[Index];
		}
		
		const T& ElementAt(int Index) const
		{
			if (m_Data == NULL && m_fp_OnArrayUsed != NULL)
				m_fp_OnArrayUsed(m_OnArrayUsedParam);  // This will allocate and set up the array if it can
			
			if (m_Data == NULL)
				return m_DefaultElement;
			
			int ExtendedArraySize = GetExtendedArraySize();
			if (ExtendedArraySize == 0)
				return m_DefaultElement;
			
			if (Index < 0)
				Index = 0;
			if (Index >= ExtendedArraySize)
				Index = ExtendedArraySize - 1;
			
			return m_Data[Index];
		}
		
		// For Sierra Chart internal use only.
		void InternalSetArray(T* DataPointer, int Size, int NumExtendedElements = 0)
		{
			m_NumElements = Size;
			m_NumExtendedElements = NumExtendedElements;
			m_Data = DataPointer;
		}
		
		// For Sierra Chart internal use only
		void InternalSetOnUsed(fp_SCDLLOnArrayUsed OnArrayUsed, int Param)
		{
			m_fp_OnArrayUsed = OnArrayUsed;
			m_OnArrayUsedParam = Param;
		}

		int GetArraySize() const
		{
			return m_NumElements;
		}
		
		int GetExtendedArraySize() const
		{
			if (m_NumElements == 0)
				return 0;
			
			return m_NumElements + m_NumExtendedElements;
		}
		
		// This should only be used when absolutely necessary and you know what
		// you are doing with it.
		T* GetPointer()
		{
			//Returns non-constant data pointer which can be used for rapidly setting blocks of data
			return m_Data;
		}
		const T* GetPointer() const
		{
			return m_Data;
		}
	
	friend class c_ConstArrayWrapper<T>;
};

template <typename T>
class c_ConstArrayWrapper
{
	private:
		const T* m_Data;
		int m_NumElements;
		
		fp_SCDLLOnArrayUsed m_OnArrayUsed;
		int m_OnArrayUsedParam;
		
		int m_NumExtendedElements;
		
		char Reserved[44];
		
		T m_DefaultElement;
		
	public:
		c_ConstArrayWrapper()
			: m_Data(NULL)
			, m_NumElements(0)
			, m_NumExtendedElements(0)
			, m_OnArrayUsed(NULL)
			, m_OnArrayUsedParam(0)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		explicit c_ConstArrayWrapper(int i)
			: m_Data(NULL)
			, m_NumElements(0)
			, m_NumExtendedElements(0)
			, m_OnArrayUsed(NULL)
			, m_OnArrayUsedParam(0)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		c_ConstArrayWrapper(const c_ConstArrayWrapper& Src)
			: m_Data(Src.m_Data)
			, m_NumElements(Src.m_NumElements)
			, m_NumExtendedElements(Src.m_NumExtendedElements)
			, m_OnArrayUsed(Src.m_OnArrayUsed)
			, m_OnArrayUsedParam(Src.m_OnArrayUsedParam)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		c_ConstArrayWrapper(const c_ArrayWrapper<T>& Src)
			: m_Data(Src.m_Data)
			, m_NumElements(Src.m_NumElements)
			, m_NumExtendedElements(Src.m_NumExtendedElements)
			, m_OnArrayUsed(Src.m_OnArrayUsed)
			, m_OnArrayUsedParam(Src.m_OnArrayUsedParam)
			, m_DefaultElement(0)
		{
			memset(Reserved, 0, sizeof(Reserved));
		}
		
		~c_ConstArrayWrapper()
		{
		}
		
		const c_ConstArrayWrapper& operator = (const c_ConstArrayWrapper& Src)
		{
			m_Data = Src.m_Data;
			m_NumElements = Src.m_NumElements;
			m_NumExtendedElements = Src.m_NumExtendedElements;
			m_OnArrayUsed = Src.m_OnArrayUsed;
			m_OnArrayUsedParam = Src.m_OnArrayUsedParam;
			m_DefaultElement = Src.m_DefaultElement;
			
			return *this;
		}
		const c_ConstArrayWrapper& operator = (const c_ArrayWrapper<T>& Src)
		{
			m_Data = Src.m_Data;
			m_NumElements = Src.m_NumElements;
			m_NumExtendedElements = Src.m_NumExtendedElements;
			m_OnArrayUsed = Src.m_OnArrayUsed;
			m_OnArrayUsedParam = Src.m_OnArrayUsedParam;
			m_DefaultElement = Src.m_DefaultElement;
			
			return *this;
		}
		
		const T& operator [] (int Index) const
		{
			return ElementAt(Index);
		}
		
		const T& ElementAt(int Index) const
		{
			if (m_Data == NULL && m_OnArrayUsed != NULL)
				m_OnArrayUsed(m_OnArrayUsedParam);  // This will allocate and set up the array if it can
			
			if (m_Data == NULL)
				return m_DefaultElement;

			int ExtendedArraySize = GetExtendedArraySize();
			
			if (Index < 0)
				Index = 0;
			if (Index >= ExtendedArraySize)
				Index = ExtendedArraySize - 1;
			
			return m_Data[Index];
		}
		
		// For Sierra Chart internal use only
		void InternalSetArray(const T* DataPointer, int Size, int NumExtendedElements = 0)
		{
			m_NumElements = Size;
			m_NumExtendedElements = NumExtendedElements;
			m_Data = DataPointer;
		}
		
		// For Sierra Chart internal use only
		void InternalSetOnUsed(fp_SCDLLOnArrayUsed OnArrayUsed, int Param)
		{
			m_OnArrayUsed = OnArrayUsed;
			m_OnArrayUsedParam = Param;
		}
		
		int GetArraySize() const
		{
			return m_NumElements;
		}

		int GetExtendedArraySize() const
		{
			if (m_NumElements == 0)
				return 0;

			return m_NumElements + m_NumExtendedElements;
		}
		
		// This should only be used when absolutely necessary and you know what
		// you are doing with it.
		const T* GetPointer() const
		{
			return m_Data;
		}

		friend class c_ArrayWrapper<T>;
};

/****************************************************************************/

class SCDateTimeArray
	: public c_ArrayWrapper<SCDateTime>
{
	public:
		/*====================================================================
			Days since 1899-12-30.
		--------------------------------------------------------------------*/
		int DateAt(int Index) const
		{
			return ElementAt(Index).GetDate();
		}
		/*====================================================================
			Seconds since midnight.
		--------------------------------------------------------------------*/
		int TimeAt(int Index) const
		{
			return ElementAt(Index).GetTimeInSeconds();
		}
		
		/*====================================================================
			Returns the date that was set.
		--------------------------------------------------------------------*/
		int SetDateAt(int Index, int Date)
		{
			ElementAt(Index) = SCDateTime(Date, TimeAt(Index));
			return Date;
		}
		/*====================================================================
			Returns the time that was set.
		--------------------------------------------------------------------*/
		int SetTimeAt(int Index, int Time)
		{
			ElementAt(Index) = SCDateTime(DateAt(Index), Time);
			return Time;
		}
};

class SCConstDateTimeArray
	: public c_ConstArrayWrapper<SCDateTime>
{
	public:
		/*====================================================================
			Days since 1899-12-30.
		--------------------------------------------------------------------*/
		int DateAt(int Index) const
		{
			return ElementAt(Index).GetDate();
		}
		/*====================================================================
			Seconds since midnight.
		--------------------------------------------------------------------*/
		int TimeAt(int Index) const
		{
			return ElementAt(Index).GetTimeInSeconds();
		}
};

typedef SCDateTimeArray &SCDateTimeArrayRef;
typedef c_ArrayWrapper<unsigned short> SCUShortArray;
typedef c_ArrayWrapper<unsigned int> SCUIntArray;
typedef c_ArrayWrapper<unsigned int>& SCUIntArrayRef;
typedef c_ArrayWrapper<SCUIntArray> SCUIntArrayArray;
typedef c_ArrayWrapper<float> SCFloatArray;
typedef c_ArrayWrapper<COLORREF> SCColorArray;
typedef c_ArrayWrapper<COLORREF>& SCColorArrayRef;
typedef c_ConstArrayWrapper<unsigned short> SCConstUShortArray;
typedef c_ConstArrayWrapper<char> SCConstCharArray;

typedef c_ArrayWrapper<float> SCConstFloatArray;

typedef c_ArrayWrapper<SCFloatArray>& SCBaseDataRef;
typedef c_ArrayWrapper<float>& SCFloatArrayRef;
typedef c_ArrayWrapper<float>& SCConstFloatArrayRef;
typedef const c_ArrayWrapper<float>& SCFloatArrayInRef;

typedef c_ArrayWrapper<SCConstFloatArray> SCConstFloatArrayArray;
typedef c_ArrayWrapper<SCFloatArray> SCFloatArrayArray;

typedef c_ArrayWrapper<SCString> SCStringArray;

class SCGraphData
	: public SCFloatArrayArray
{
	private:
		SCFloatArray BaseData[SC_SUBGRAPHS_AVAILABLE];
		
	public:
		SCGraphData()
		{
			InternalSetArray(BaseData, SC_SUBGRAPHS_AVAILABLE);
		}
		
		SCFloatArrayRef InternalAccessBaseDataArray(int Index)
		{
			if (Index < 0)
				return BaseData[0];
			
			if (Index >= SC_SUBGRAPHS_AVAILABLE)
				return BaseData[SC_SUBGRAPHS_AVAILABLE - 1];
			
			return BaseData[Index];
		}
};

/****************************************************************************/
struct s_SCSubgraph_260
{
	SCString Name;
	
	unsigned int PrimaryColor;
	unsigned int SecondaryColor;
	unsigned int SecondaryColorUsed;  // boolean
	
	unsigned short DrawStyle;
	SubgraphLineStyles LineStyle;
	unsigned short LineWidth;
	unsigned char UseLabelsColor;  // Line label Name/Value
	unsigned char Unused;
	
	SCFloatArray Data;  // Array of data values
	SCColorArray DataColor;  // Array of colors for each of the data elements
	
	SCFloatArrayArray Arrays;  // Array of extra arrays
	
	unsigned char IncludeInStudySummary;
	unsigned char UseStudySummaryCellBackgroundColor;
	unsigned char AutoColoring;  // boolean
	
	int GraphicalDisplacement;
	
	int DrawZeros;  // boolean

	// This controls whether the Subgraph name and value are displayed in the Values windows and on the Region Data Line.
	unsigned short DisplayNameValueInWindowsFlags; 
	
	unsigned int LineLabel;  // Line label Name/Value
	int ExtendedArrayElementsToGraph;
	
	SCString TextDrawStyleText;
	
	float GradientAngleUnit;
	float GradientAngleMax;

	SCString ShortName;
	uint32_t StudySummaryCellBackgroundColor;
#ifdef _WIN64
	SCString StudySummaryCellText;
	uint32_t LabelsColor;
	char Reserve[42];
#else
	uint32_t LabelsColor;
	char Reserve[66];
#endif
	
	s_SCSubgraph_260()
		: Name()
		, PrimaryColor(0)
		, SecondaryColor(0)
		, SecondaryColorUsed(0)
		, DrawStyle(0)
		, LineStyle(LINESTYLE_SOLID)
		, LineWidth(0)
		, UseLabelsColor(0)
		, Unused(0)
		, LineLabel(0)
		, Data()
		, DataColor()
		, IncludeInStudySummary(0)
		, UseStudySummaryCellBackgroundColor(0)
		, AutoColoring(0)
		, GraphicalDisplacement(0)
		, DrawZeros(0)
		, DisplayNameValueInWindowsFlags(0)
		, ExtendedArrayElementsToGraph(0)
		, TextDrawStyleText()
		, GradientAngleUnit(0)
		, GradientAngleMax(0)
		, ShortName()
		, StudySummaryCellBackgroundColor(0)
		, LabelsColor(0)
	{
		memset(Reserve, 0, sizeof(Reserve));
	}

	// This constructor is so this struct can work as an c_ArrayWrapper element
	explicit s_SCSubgraph_260(int i)
		: Name()
		, PrimaryColor(0)
		, SecondaryColor(0)
		, SecondaryColorUsed(0)
		, DrawStyle(0)
		, LineStyle(LINESTYLE_SOLID)
		, LineWidth(0)
		, UseLabelsColor(0)
		, Unused(0)
		, LineLabel(0)
		, Data()
		, DataColor()
		, IncludeInStudySummary(0)
		, UseStudySummaryCellBackgroundColor(0)
		, AutoColoring(0)
		, GraphicalDisplacement(0)
		, DrawZeros(0)
		, DisplayNameValueInWindowsFlags(0)
		, ExtendedArrayElementsToGraph(0)
		, TextDrawStyleText()
		, GradientAngleUnit(0)
		, GradientAngleMax(0)
		, ShortName()
		, StudySummaryCellBackgroundColor(0)
		, LabelsColor(0)
	{
		memset(Reserve, 0, sizeof(Reserve));
	}

	float& operator [] (int Index)
	{
		return Data[Index];
	}

	operator SCFloatArray& ()
	{
		return Data;
	}
};

typedef c_ArrayWrapper<s_SCSubgraph_260> SCSubgraph260Array;
typedef s_SCSubgraph_260& SCSubgraph260Ref;
typedef s_SCSubgraph_260& SCSubgraphRef;

/****************************************************************************/
struct s_ChartStudySubgraphValues
{
	int ChartNumber;
	int StudyID;
	int SubgraphIndex;
};

struct s_SCInput_145
{
	SCString Name;
	
	unsigned char ValueType;
	unsigned char Unused1;
	unsigned short DisplayOrder;
	#pragma pack(push, 4)  // This is necessary otherwise data items larger that 4 bytes will throw off the alignment and size of the union
	union
	{
		unsigned int IndexValue;
		float FloatValue;
		unsigned int BooleanValue;
		struct
		{
			double DateTimeValue;  // SCDateTime
			unsigned int TimeZoneValue;
		};
		int IntValue;
		unsigned int ColorValue;
		s_ChartStudySubgraphValues ChartStudySubgraphValues;
		double DoubleValue;
		struct  
		{
			bool        StringModified;
			const char* StringValue;
		};

		unsigned char ValueBytes[16];
	};
	
	union
	{
		float FloatValueMin;
		double DateTimeValueMin;  // SCDateTime
		int IntValueMin;
		double DoubleValueMin;
		
		unsigned char ValueBytesMin[16];
	};
	union
	{
		float FloatValueMax;
		double DateTimeValueMax;  // SCDateTime
		int IntValueMax;
		double DoubleValueMax;
		
		unsigned char ValueBytesMax[16];
	};
	#pragma pack(pop)
	
	// Do not use or modify these.  For internal use only
	int InputIndex;
	void (SCDLLCALL* InternalSetCustomStrings)(int InputIndex, const char* CustomStrings);
	const char* SelectedCustomInputString;
	
	void (SCDLLCALL* InternalSetDescription)(int InputIndex, const char* Description);
	
	char Reserve[48];
	
	s_SCInput_145()
	{
		Clear();
	}
	
	explicit s_SCInput_145(int i)
	{
		Clear();
	}
	
	void Clear()
	{
		memset(this, 0, sizeof(s_SCInput_145));
		Name.Initialize();
	}
	
	bool UsesStringValue() const
	{
		return ValueType == STRING_VALUE
			|| ValueType == PATH_AND_FILE_NAME_VALUE
			|| ValueType == FIND_SYMBOL_VALUE;
	}
	
	unsigned int GetIndex() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			return IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return (unsigned int)((FloatValue < 0.0f) ? (FloatValue - 0.5f) : (FloatValue + 0.5f));
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1 : 0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return 0;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (unsigned int)IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return (unsigned int)ColorValue;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return (unsigned int)((DoubleValue < 0.0) ? (DoubleValue - 0.5f) : (DoubleValue + 0.5f));
			
			case TIME_WITH_TIMEZONE_VALUE:
			return TimeZoneValue;
			
			case STRING_VALUE:
			case PATH_AND_FILE_NAME_VALUE:
			case FIND_SYMBOL_VALUE:
			return 0;

			default:
			case NO_VALUE:
			return 0;
		}
	}
	
	float GetFloat() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			return (float)IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return FloatValue;
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1.0f : 0.0f;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return 0.0f;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (float)IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return 0.0f;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0.0f;
			
			case DOUBLE_VALUE:  // DoubleValue
			return (float)DoubleValue;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return 0.0f;
			
			case STRING_VALUE:
			case PATH_AND_FILE_NAME_VALUE:
			case FIND_SYMBOL_VALUE:
			return 0.0f;

			default:
			case NO_VALUE:
			return 0.0f;
		}
	}
	
	unsigned int GetBoolean() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			return (IndexValue != 0) ? 1 : 0;
			
			case FLOAT_VALUE:  // FloatValue
			return (FloatValue != 0.0f) ? 1 : 0;
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1 : 0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return (DateTimeValue != 0) ? 1 : 0;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (IntValue != 0) ? 1 : 0;
			
			case COLOR_VALUE:  // ColorValue
			return (ColorValue != 0) ? 1 : 0;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return (DoubleValue != 0.0) ? 1 : 0;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return (DateTimeValue != 0) ? 1 : 0;
			
			case STRING_VALUE:
			case PATH_AND_FILE_NAME_VALUE:
			case FIND_SYMBOL_VALUE:
			return 0;

			default:
			case NO_VALUE:
			return 0;
		}
	}
	
	SCDateTime GetDateTime() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			return 0.0;
			
			case FLOAT_VALUE:  // FloatValue
			return 0.0;
			
			case YESNO_VALUE:  // BooleanValue
			return 0.0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return DateTimeValue;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return 0.0;
			
			case COLOR_VALUE:  // ColorValue
			return 0.0;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0.0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return 0.0;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return DateTimeValue;
			
			case STRING_VALUE:
			case PATH_AND_FILE_NAME_VALUE:
			case FIND_SYMBOL_VALUE:
			return 0.0;

			default:
			case NO_VALUE:
			return 0.0;
		}
	}
	
	int GetInt() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			return (int)IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return (int)((FloatValue < 0.0f) ? (FloatValue - 0.5f) : (FloatValue + 0.5f));
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1 : 0;
			
			case DATE_VALUE:  // DateTimeValue
			return SCDateTime(DateTimeValue).GetDate();
			
			case TIME_VALUE:  // DateTimeValue
			return SCDateTime(DateTimeValue).GetTimeInSeconds();
			
			case DATETIME_VALUE:  // DateTimeValue
			return 0;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return (int)ColorValue;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return (int)((DoubleValue < 0.0) ? (DoubleValue - 0.5) : (DoubleValue + 0.5));
			
			case TIME_WITH_TIMEZONE_VALUE:
			return SCDateTime(DateTimeValue).GetTimeInSeconds();
			
			case STRING_VALUE:
			case PATH_AND_FILE_NAME_VALUE:
			case FIND_SYMBOL_VALUE:
			return 0;

			default:
			case NO_VALUE:
			return 0;
		}
	}
	
	unsigned int GetColor() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			return (unsigned int)IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return 0;
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? RGB(255,255,255) : 0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return 0;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (unsigned int)IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return ColorValue;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return 0;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return 0;
			
			case STRING_VALUE:
			case PATH_AND_FILE_NAME_VALUE:
			case FIND_SYMBOL_VALUE:
			return 0;

			default:
			case NO_VALUE:
			return 0;
		}
	}
	
	double GetDouble() const
	{
		switch (ValueType)
		{
			case OHLC_VALUE:  // IndexValue
			case STUDYINDEX_VALUE:  // IndexValue
			case SUBGRAPHINDEX_VALUE:  // IndexValue
			case MOVAVGTYPE_VALUE:  // IndexValue
			case TIME_PERIOD_LENGTH_UNIT_VALUE:
			case STUDYID_VALUE:  // IndexValue
			case CANDLESTICK_PATTERNS_VALUE:
			case CUSTOM_STRING_VALUE:
			case TIMEZONE_VALUE:  // IndexValue
			case ALERT_SOUND_NUMBER_VALUE:
			return (double)IndexValue;
			
			case FLOAT_VALUE:  // FloatValue
			return (double)FloatValue;
			
			case YESNO_VALUE:  // BooleanValue
			return (BooleanValue != 0) ? 1.0 : 0.0;
			
			case DATE_VALUE:  // DateTimeValue
			case TIME_VALUE:  // DateTimeValue
			case DATETIME_VALUE:  // DateTimeValue
			return DateTimeValue;
			
			case INT_VALUE:  // IntValue
			case CHART_NUMBER:
			return (double)IntValue;
			
			case COLOR_VALUE:  // ColorValue
			return 0.0;
			
			case CHART_STUDY_SUBGRAPH_VALUES:
			case STUDY_SUBGRAPH_VALUES:
			case CHART_STUDY_VALUES:
			return 0.0;
			
			case DOUBLE_VALUE:  // DoubleValue
			return DoubleValue;
			
			case TIME_WITH_TIMEZONE_VALUE:
			return DateTimeValue;
			
			case STRING_VALUE:
			case PATH_AND_FILE_NAME_VALUE:
			case FIND_SYMBOL_VALUE:
			return 0.0;

			default:
			case NO_VALUE:
			return 0.0;
		}
	}
	
	unsigned int GetInputDataIndex() const
	{
		unsigned int InputDataIndex = GetIndex();
		
		if (InputDataIndex > SC_SUBGRAPHS_AVAILABLE - 1)
			InputDataIndex = SC_SUBGRAPHS_AVAILABLE - 1;
		
		return InputDataIndex;
	}
	
	unsigned int GetStudyIndex() const
	{
		return GetIndex();
	}
	
	unsigned int GetSubgraphIndex() const
	{
		unsigned int SubgraphIndex;
		if (ValueType == CHART_STUDY_SUBGRAPH_VALUES || ValueType == STUDY_SUBGRAPH_VALUES)
			SubgraphIndex = (unsigned int)ChartStudySubgraphValues.SubgraphIndex;
		else
			SubgraphIndex = GetIndex();
		
		if (SubgraphIndex > SC_SUBGRAPHS_AVAILABLE - 1)
			SubgraphIndex = SC_SUBGRAPHS_AVAILABLE - 1;
		
		return SubgraphIndex;
	}
	
	unsigned int GetMovAvgType() const
	{
		unsigned int MovAvgTypeValue = GetIndex();
		if (MovAvgTypeValue >= MOVAVGTYPE_NUMBER_OF_AVERAGES)
			MovAvgTypeValue = 0;
		return MovAvgTypeValue;
	}
	
	unsigned int GetTimePeriodType() const
	{
		return GetIndex();
	}
	
	unsigned int GetAlertSoundNumber() const
	{
		return GetIndex();
	}
	
	unsigned int GetCandleStickPatternIndex() const
	{
		return GetIndex();
	}
	
	unsigned int GetStudyID() const
	{
		if (ValueType == CHART_STUDY_SUBGRAPH_VALUES || ValueType == STUDY_SUBGRAPH_VALUES || ValueType == CHART_STUDY_VALUES)
			return (unsigned int)ChartStudySubgraphValues.StudyID;
		else
			return GetIndex();
	}
	
	unsigned int GetYesNo() const
	{
		return GetBoolean();
	}
	
	int GetDate() const
	{
		return GetDateTime().GetDate();
	}
	
	int GetTime() const
	{
		return GetDateTime().GetTimeInSeconds();
	}

	int GetTimeInMilliseconds() const
	{
		return GetDateTime().GetTimeInMilliseconds();
	}
	
	void GetChartStudySubgraphValues(int& ChartNumber, int& StudyID, int& SubgraphIndex) const
	{
		ChartNumber = ChartStudySubgraphValues.ChartNumber;
		StudyID = ChartStudySubgraphValues.StudyID;
		SubgraphIndex = ChartStudySubgraphValues.SubgraphIndex;
	}
	
	s_ChartStudySubgraphValues GetChartStudySubgraphValues() const
	{
		return ChartStudySubgraphValues;
	}
	
	int GetChartNumber()
	{
		if (ValueType == CHART_STUDY_SUBGRAPH_VALUES || ValueType == CHART_STUDY_VALUES)
			return ChartStudySubgraphValues.ChartNumber;
		else if (ValueType == CHART_NUMBER)
			return IntValue;
		else
			return GetInt();
	}
	
	const SCString GetSelectedCustomString()
	{
		SCString TempString;
		if(SelectedCustomInputString != NULL)
			TempString = SelectedCustomInputString;

		return TempString;
	}
	
	TimeZonesEnum GetTimeZone()
	{
		unsigned int TimeZone = TIMEZONE_NOT_SPECIFIED;
		
		if (ValueType == TIMEZONE_VALUE)
			TimeZone = GetIndex();
		else if (ValueType == TIME_WITH_TIMEZONE_VALUE)
			TimeZone = TimeZoneValue;
		
		if (TimeZone >= NUM_TIME_ZONES)
			TimeZone = TIMEZONE_NOT_SPECIFIED;
		
		return (TimeZonesEnum)TimeZone;
	}
	
	const char* GetString()
	{
		if (UsesStringValue() && StringValue != NULL)
			return StringValue;

		return "Unset";
	}

	const char* GetPathAndFileName()
	{
		if (UsesStringValue() && StringValue != NULL)
			return StringValue;

		return "";
	}

	const char* GetSymbol()
	{
		if (UsesStringValue() && StringValue != NULL)
			return StringValue;

		return "";
	}

	s_SCInput_145& SetInputDataIndex(unsigned int Value)
	{
		ValueType = OHLC_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetStudyIndex(unsigned int Value)
	{
		ValueType = STUDYINDEX_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetSubgraphIndex(unsigned int Value)
	{
		ValueType = SUBGRAPHINDEX_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetMovAvgType(unsigned int Value)
	{
		ValueType = MOVAVGTYPE_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetTimePeriodType(unsigned int Value)
	{
		ValueType = TIME_PERIOD_LENGTH_UNIT_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetFloat(float Value)
	{
		ValueType = FLOAT_VALUE;
		FloatValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetYesNo(unsigned int Value)
	{
		ValueType = YESNO_VALUE;
		BooleanValue = (Value != 0)?1:0;
		return *this;
	}
	
	s_SCInput_145& SetDate(int Value)
	{
		ValueType = DATE_VALUE;
		DateTimeValue = SCDateTime(Value, 0);
		return *this;
	}
	
	s_SCInput_145& SetTime(int Value)
	{
		ValueType = TIME_VALUE;
		DateTimeValue = SCDateTime(0, Value);
		return *this;
	}

	s_SCInput_145& SetTimeAsSCDateTime(const SCDateTime& Time)
	{
		ValueType = TIME_VALUE;
		DateTimeValue = Time.GetTimeAsSCDateTime();
		return *this;
	}
	
	s_SCInput_145& SetDateTime(const SCDateTime& Value)
	{
		ValueType = DATETIME_VALUE;
		DateTimeValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetInt(int Value)
	{
		ValueType = INT_VALUE;
		IntValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetIntWithoutTypeChange(int Value)
	{
		switch (ValueType)
		{
		case OHLC_VALUE:  // IndexValue
		case STUDYINDEX_VALUE:  // IndexValue
		case SUBGRAPHINDEX_VALUE:  // IndexValue
		case MOVAVGTYPE_VALUE:  // IndexValue
		case TIME_PERIOD_LENGTH_UNIT_VALUE:
		case STUDYID_VALUE:  // IndexValue
		case CANDLESTICK_PATTERNS_VALUE:
		case CUSTOM_STRING_VALUE:
		case TIMEZONE_VALUE:  // IndexValue
		case ALERT_SOUND_NUMBER_VALUE:
			IndexValue = Value;
			break;

		case FLOAT_VALUE:  // FloatValue
			FloatValue = static_cast<float>(Value);
			break;

		case YESNO_VALUE:  // BooleanValue
			BooleanValue = (Value != 0.0);
			break;

		case DATE_VALUE:  // DateTimeValue
		case TIME_VALUE:  // DateTimeValue
		case DATETIME_VALUE:  // DateTimeValue
			DateTimeValue = static_cast<double>(Value);
			break;

		case INT_VALUE:  // IntValue
		case CHART_NUMBER:
			IntValue = Value;
			break;

		case COLOR_VALUE:  // ColorValue
			ColorValue = static_cast<unsigned int>(Value);
			break;

		case CHART_STUDY_SUBGRAPH_VALUES:
		case STUDY_SUBGRAPH_VALUES:
		case CHART_STUDY_VALUES:
			break;

		case DOUBLE_VALUE:  // DoubleValue
			DoubleValue = static_cast<double>(Value);

		case TIME_WITH_TIMEZONE_VALUE:
			DateTimeValue = static_cast<double>(Value);

		case STRING_VALUE:
		case PATH_AND_FILE_NAME_VALUE:
		case FIND_SYMBOL_VALUE:
			break;

		default:
		case NO_VALUE:
			break;
		}

		return *this;
	}

	s_SCInput_145& SetStudyID(unsigned int Value)
	{
		ValueType = STUDYID_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetColor(unsigned int Color)
	{
		ValueType = COLOR_VALUE;
		ColorValue = Color;
		return *this;
	}
	
	s_SCInput_145& SetColor(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
		return SetColor(RGB(Red, Green, Blue));
	}
	
	s_SCInput_145& SetAlertSoundNumber(unsigned int Value)
	{
		ValueType = ALERT_SOUND_NUMBER_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetCandleStickPatternIndex(unsigned int Value)
	{
		ValueType = CANDLESTICK_PATTERNS_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetChartStudySubgraphValues(int ChartNumber, int StudyID, int SubgraphIndex)
	{
		ValueType = CHART_STUDY_SUBGRAPH_VALUES;
		ChartStudySubgraphValues.ChartNumber = ChartNumber;
		ChartStudySubgraphValues.StudyID = StudyID;
		ChartStudySubgraphValues.SubgraphIndex = SubgraphIndex;
		return *this;
	}
	
	s_SCInput_145& SetChartNumber(int ChartNumber)
	{
		ValueType = CHART_NUMBER;
		IntValue = ChartNumber;
		return *this;
	}
	
	s_SCInput_145& SetStudySubgraphValues(int StudyID, int SubgraphIndex)
	{
		ValueType = STUDY_SUBGRAPH_VALUES;
		ChartStudySubgraphValues.StudyID = StudyID;
		ChartStudySubgraphValues.SubgraphIndex = SubgraphIndex;
		return *this;
	}
	
	s_SCInput_145& SetChartStudyValues(int ChartNumber, int StudyID)
	{
		ValueType = CHART_STUDY_VALUES;
		ChartStudySubgraphValues.ChartNumber = ChartNumber;
		ChartStudySubgraphValues.StudyID = StudyID;
		return *this;
	}
	
	s_SCInput_145& SetCustomInputIndex(unsigned int Value)
	{
		ValueType = CUSTOM_STRING_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetDouble(double Value)
	{
		ValueType = DOUBLE_VALUE;
		DoubleValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetDoubleWithoutTypeChange(double Value)
	{
		switch (ValueType)
		{
		case OHLC_VALUE:  // IndexValue
		case STUDYINDEX_VALUE:  // IndexValue
		case SUBGRAPHINDEX_VALUE:  // IndexValue
		case MOVAVGTYPE_VALUE:  // IndexValue
		case TIME_PERIOD_LENGTH_UNIT_VALUE:
		case STUDYID_VALUE:  // IndexValue
		case CANDLESTICK_PATTERNS_VALUE:
		case CUSTOM_STRING_VALUE:
		case TIMEZONE_VALUE:  // IndexValue
		case ALERT_SOUND_NUMBER_VALUE:
			IndexValue = static_cast<int>(Value);
			break;

		case FLOAT_VALUE:  // FloatValue
			FloatValue = static_cast<float>(Value);
			break;

		case YESNO_VALUE:  // BooleanValue
			BooleanValue = (Value != 0.0);
			break;

		case DATE_VALUE:  // DateTimeValue
		case TIME_VALUE:  // DateTimeValue
		case DATETIME_VALUE:  // DateTimeValue
			DateTimeValue = Value;
			break;

		case INT_VALUE:  // IntValue
		case CHART_NUMBER:
			IntValue = static_cast<int>(Value);
			break;

		case COLOR_VALUE:  // ColorValue
			ColorValue = static_cast<unsigned int>(Value);
			break;

		case CHART_STUDY_SUBGRAPH_VALUES:
		case STUDY_SUBGRAPH_VALUES:
		case CHART_STUDY_VALUES:
			break;

		case DOUBLE_VALUE:  // DoubleValue
			DoubleValue = Value;

		case TIME_WITH_TIMEZONE_VALUE:
			DateTimeValue = Value;

		case STRING_VALUE:
		case PATH_AND_FILE_NAME_VALUE:
		case FIND_SYMBOL_VALUE:
			break;

		default:
		case NO_VALUE:
			break;
		}

		return *this;
	}

	s_SCInput_145& SetTimeZone(TimeZonesEnum Value)
	{
		ValueType = TIMEZONE_VALUE;
		IndexValue = Value;
		return *this;
	}
	
	s_SCInput_145& SetTimeWithTimeZone(int ValueTime, TimeZonesEnum ValueTimeZone)
	{
		ValueType = TIME_WITH_TIMEZONE_VALUE;
		DateTimeValue = SCDateTime(0, ValueTime);
		TimeZoneValue = ValueTimeZone;
		return *this;
	}

	s_SCInput_145& SetString(const char* Value)
	{
		ClearString();

		size_t StringLength = 0;
		if (Value != NULL)
			StringLength = strlen(Value);

		if (StringLength != 0)
		{
			size_t NumBytes = StringLength + 1;

			char* NewString = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, NumBytes);

			if (NewString != NULL)
			{
				#if __STDC_WANT_SECURE_LIB__
				strcpy_s(NewString, NumBytes, Value);
				#else
				strcpy(NewString, Value);
				#endif
				StringValue = NewString;
			}
		}

		ValueType = STRING_VALUE;

		//if (StringLength > 0 && StringValue != NULL)
			StringModified = true;

		return *this;
	}

	void ClearString()
	{
		if (UsesStringValue() && StringModified && StringValue != NULL)
			HeapFree(GetProcessHeap(), 0, (char*)StringValue);

		StringModified = false;

		StringValue = NULL;
	}

	s_SCInput_145& SetPathAndFileName(const char* PathAndFileName)
	{
		SetString(PathAndFileName);
		ValueType = PATH_AND_FILE_NAME_VALUE;
		return *this;
	}

	s_SCInput_145& SetSymbol(const char* Symbol)
	{
		SetString(Symbol);
		ValueType = FIND_SYMBOL_VALUE;
		return *this;
	}

	void SetFloatLimits(float Min, float Max)
	{
		FloatValueMin = Min;
		FloatValueMax = Max;
	}
	
	void SetDateTimeLimits(SCDateTime Min, SCDateTime Max)
	{
		DateTimeValueMin = Min;
		DateTimeValueMax = Max;
	}
	
	void SetIntLimits(int Min, int Max)
	{
		IntValueMin = Min;
		IntValueMax = Max;
	}
	
	void SetDoubleLimits(double Min, double Max)
	{
		DoubleValueMin = Min;
		DoubleValueMax = Max;
	}
	
	void SetCustomInputStrings(const char* p_CustomStrings)
	{
		if(InternalSetCustomStrings != NULL)
			InternalSetCustomStrings(InputIndex, p_CustomStrings);
	}
	
	void SetDescription(const char* Description)
	{
		if(InternalSetDescription != NULL && Description != NULL)
			InternalSetDescription(InputIndex, Description);
	}
};

typedef c_ArrayWrapper<s_SCInput_145> SCInput145Array;
typedef s_SCInput_145& SCInputRef;

/****************************************************************************/

struct s_Parabolic
{ 
	s_Parabolic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, SCDateTimeArrayRef BaseDateTimeIn, int Index, float InStartAccelFactor, float InAccelIncrement, float InMaxAccelFactor, unsigned int InAdjustForGap, int InputDataHighIndex = SC_HIGH, int InputDataLowIndex = SC_LOW)
		:	 _BaseDataIn(BaseDataIn),
		_Out(Out),
		_BaseDateTimeIn(BaseDateTimeIn),
		_Index(Index),
		_InStartAccelFactor(InStartAccelFactor),
		_InAccelIncrement(InAccelIncrement),
		_InMaxAccelFactor(InMaxAccelFactor),
		_InAdjustForGap(InAdjustForGap),
		_InputDataHighIndex(InputDataHighIndex),
		_InputDataLowIndex(InputDataLowIndex)
	{
	}

	SCBaseDataRef _BaseDataIn;
	SCSubgraphRef _Out;
	SCDateTimeArrayRef _BaseDateTimeIn;
	int _Index;
	float _InStartAccelFactor;
	float _InAccelIncrement;
	float _InMaxAccelFactor;
	unsigned int _InAdjustForGap;
	int _InputDataHighIndex;
	int _InputDataLowIndex;

	inline float BaseDataHigh(int Index) 
	{ 
		//return max(_BaseDataIn[_InputDataHighIndex][Index], _BaseDataIn[_InputDataLowIndex][Index]); 
		return _BaseDataIn[_InputDataHighIndex][Index];

	}
	
	inline float BaseDataLow(int Index)  
	{ 
		//return min(_BaseDataIn[_InputDataHighIndex][Index], _BaseDataIn[_InputDataLowIndex][Index]); 
		return _BaseDataIn[_InputDataLowIndex][Index];
	}
};

/****************************************************************************/

struct s_NumericInformationGraphDrawTypeConfig
{ 
	static const int NUMBER_OF_THRESHOLDS = 3;
	COLORREF TextBackgroundColor;
	bool TransparentTextBackground;
	bool LabelsOnRight;
	bool AllowLabelOverlap;
	bool DrawGridlines;
	int GridlineStyleSubgraphIndex;
	int FontSize;
	bool ShowPullback;
	bool Unused;
	bool HideLabels;
	char Reserved;
	int ValueFormat[SC_SUBGRAPHS_AVAILABLE];
	int SubgraphOrder[SC_SUBGRAPHS_AVAILABLE];
	bool ColorBackgroundBasedOnValuePercent;
	float HighestValue[SC_SUBGRAPHS_AVAILABLE];
	float LowestValue[SC_SUBGRAPHS_AVAILABLE];
	float PercentCompareThresholds[NUMBER_OF_THRESHOLDS];
	COLORREF Range3UpColor;
	COLORREF Range2UpColor;
	COLORREF Range1UpColor;
	COLORREF Range0UpColor;
	COLORREF Range0DownColor;
	COLORREF Range1DownColor;
	COLORREF Range2DownColor;
	COLORREF Range3DownColor;
	bool DifferentPullbackAndLabelsColor;
	COLORREF PullbackAndLabelsColor;
	bool UseDefaultNumberFormattingForPercentage;

	s_NumericInformationGraphDrawTypeConfig()
	{
		TextBackgroundColor = COLOR_BLACK;
		TransparentTextBackground = true;
		LabelsOnRight = false;
		AllowLabelOverlap = false;
		DrawGridlines = true;
		GridlineStyleSubgraphIndex = -1;
		FontSize = 0;
		ShowPullback = false;
		Unused = false;
		HideLabels = false;
		Reserved = 0;
		Range3UpColor = COLOR_BLACK;
		Range2UpColor = COLOR_BLACK;
		Range1UpColor = COLOR_BLACK;
		Range0UpColor = COLOR_BLACK;
		Range0DownColor = COLOR_BLACK;
		Range1DownColor = COLOR_BLACK;
		Range2DownColor = COLOR_BLACK;
		Range3DownColor = COLOR_BLACK;
		DifferentPullbackAndLabelsColor = false;
		PullbackAndLabelsColor = COLOR_WHITE;
		UseDefaultNumberFormattingForPercentage = false;

		for (int Index = 0; Index < SC_SUBGRAPHS_AVAILABLE; Index++)
		{
			ValueFormat[Index] = VALUEFORMAT_INHERITED;
			SubgraphOrder[Index] = Index;
			HighestValue[Index] = -FLT_MAX;
			LowestValue[Index] = FLT_MAX;
		}

		ColorBackgroundBasedOnValuePercent = false;

		for (int Index = 0; Index < NUMBER_OF_THRESHOLDS; Index++)
		{
			PercentCompareThresholds[Index] = 0.0f;
		}
	}
};

/****************************************************************************/

struct s_ACSOpenChartParameters
{
	int Version;
	int PriorChartNumber;  // This gets checked first, 0 is unknown
	
	ChartDataTypeEnum ChartDataType;
	SCString Symbol;
	
	IntradayBarPeriodTypeEnum IntradayBarPeriodType;
	int IntradayBarPeriodLength;
	
	SCDateTime SessionStartTime;
	SCDateTime SessionEndTime;
	SCDateTime EveningSessionStartTime;
	SCDateTime EveningSessionEndTime;
	
	// 864
	int DaysToLoad;

	// 916
	int IntradayBarPeriodParm2;
	int IntradayBarPeriodParm3;

	// 933
	int IntradayBarPeriodParm4;

	//1155
	int HistoricalChartBarPeriod;//default is DAILY_DATA_PERIOD_DAYS


	//Version 3
	int ChartLinkNumber;

	// 1242
	int ContinuousFuturesContractOption;  // ContinuousFuturesContractOptionsEnum

	// 1283
	int LoadWeekendData;

	//1532
	int HistoricalChartBarPeriodLengthInDays;

	// 1819
	int UseEveningSession;
	int HideNewChart;

	s_ACSOpenChartParameters()
	{
		Reset();
	}

	void Reset()
	{
		Version = 5;
		PriorChartNumber = 0;
		ChartDataType = NO_DATA_TYPE;
		Symbol = "";
		IntradayBarPeriodType = IBPT_DAYS_MINS_SECS;
		IntradayBarPeriodLength = 0;
		SessionStartTime = -.1; //Flag indicating unset value
		SessionEndTime = -.1;
		EveningSessionStartTime = -.1;
		EveningSessionEndTime = -.1;
		DaysToLoad = 0;
		IntradayBarPeriodParm2 = 0;
		IntradayBarPeriodParm3 = 0;
		IntradayBarPeriodParm4 = 0;
		HistoricalChartBarPeriod = HISTORICAL_CHART_PERIOD_DAYS;
		ChartLinkNumber = 0;
		ContinuousFuturesContractOption = CFCO_NONE;
		LoadWeekendData = 1;
		HistoricalChartBarPeriodLengthInDays = 1;
		UseEveningSession = -1;
		HideNewChart = 0;
	}
};

//To eventually be removed
struct s_Old_PersistentVariables
{
	char MemoryBlock[1224];//Original size was 1216

	void Clear()
	{
		memset(this, 0, sizeof(s_Old_PersistentVariables));
	}
};


namespace n_ACSIL
{


struct s_BarPeriod
{
	ChartDataTypeEnum ChartDataType;
	IntradayBarPeriodTypeEnum IntradayChartBarPeriodType;
	int IntradayChartBarPeriodParameter1;
	int IntradayChartBarPeriodParameter2;
	int IntradayChartBarPeriodParameter3;
	int IntradayChartBarPeriodParameter4;
	HistoricalChartBarPeriodEnum HistoricalChartBarPeriodType;
	int HistoricalChartDaysPerBar; 
	SCString ACSILCustomChartStudyName;

	s_BarPeriod()
	{
		ChartDataType = NO_DATA_TYPE;
		IntradayChartBarPeriodType = IBPT_DAYS_MINS_SECS;
		IntradayChartBarPeriodParameter1 = 0;
		IntradayChartBarPeriodParameter2 = 0;
		IntradayChartBarPeriodParameter3 = 0;
		IntradayChartBarPeriodParameter4 = 0;
		HistoricalChartBarPeriodType = HISTORICAL_CHART_PERIOD_DAYS;
		HistoricalChartDaysPerBar = 0;
	}

};

struct s_ChartSessionTimes
{
	SCDateTime StartTime;
	SCDateTime EndTime;
	SCDateTime EveningStartTime;
	SCDateTime EveningEndTime;

	int UseEveningSessionTimes = 0;
	int NewBarAtSessionStart = 0;
	int LoadWeekendDataSetting = 0;
};

struct s_HTTPHeader
{
	SCString Name;
	SCString Value;

};

struct s_WriteBarAndStudyDataToFile
{
public:
	int StartingIndex = 0;
	SCString OutputPathAndFileName;
	int IncludeHiddenStudies = 0;
	int IncludeHiddenSubgraphs = 0;
	int AppendOnlyToFile = 0;
	int IncludeLastBar = 0;
};


struct s_LineUntilFutureIntersection
{
	int StartBarIndex = 0;

	//This variable can be set to the chart bar index where the future intersection line needs to stop. Once this is set to a nonzero value, the line will no longer automatically stop when it intersects a price bar. When EndBarIndex is set, the ending chart bar index is controlled by the study directly.
	int EndBarIndex = 0;

	int LineIDForBar = 0;
	float LineValue = 0;
	float LineValue2ForRange = 0;
	int UseLineValue2 = false;
	uint32_t LineColor = 0;
	unsigned short LineWidth = 0;
	unsigned short LineStyle = 0;
	int DrawValueLabel = false;
	int DrawNameLabel = false;
	SCString NameLabel;
	int AlwaysExtendToEndOfChart = false;
	int TransparencyLevel = 50;
};

};

#endif
