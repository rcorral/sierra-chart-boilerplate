// File Name: sierrachart.h

#ifndef _SIERRACHART_H_
#define _SIERRACHART_H_

// SC_DLL_VERSION gets updated only when there have been changes to the DLL
// study interface.  This should be set to the Sierra Chart version number
// that includes the changes made to the interface.
#define SC_DLL_VERSION 1903

#ifndef _CRT_SECURE_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#endif

#include "scstructures.h"
#include "ACSILDepthBars.h"
#include "IntradayRecord.h"
#include "ACSILCustomChartBars.h"

#ifdef USE_SC_1225_HEADER
//1225 is the version of the new cleaned up s_sc structure.  This is the transition version. So this header file below is for versions 1224 and earlier.
#include "SierraChart_1225.h"

#endif

/****************************************************************************/

// Structure passed to custom DLL study functions.
// s_sc = structure _ sierra chart

struct s_sc
{
	/************************************************************************/
	// Functions (These do not expand the size of the structure)

	s_sc();//Only implemented internally within Sierra Chart

	//Non static functions
	int CompareOpenToHighLow(float Open, float High, float Low, int ValueFormat)
	{
		float OpenToHighDiff = High - Open;
		float OpenToLowDiff = Open - Low;

		if (FormattedEvaluate(OpenToHighDiff, ValueFormat, LESS_OPERATOR, OpenToLowDiff, ValueFormat))
			return 1; //Open closer to High
		else if (FormattedEvaluate(OpenToHighDiff, ValueFormat, GREATER_OPERATOR, OpenToLowDiff, ValueFormat))
			return -1; //Open closer to Low

		return 0; //must be equal distance

	}

	void AddMessageToLog(const char* Message, int ShowLog)
	{
		p_AddMessageToLog(Message, ShowLog);
	}
	void AddMessageToLog(const SCString& Message, int ShowLog)
	{
		p_AddMessageToLog(Message.GetChars(), ShowLog);
	}

	void AddMessageToTradeServiceLog(const char* Message, int ShowLog = 0, int AddChartStudyDetails = 0)
	{
		Internal_AddMessageToTradeServiceLog(Message, ShowLog, AddChartStudyDetails);
	}
	void AddMessageToTradeServiceLog(const SCString& Message, int ShowLog = 0, int AddChartStudyDetails = 0)
	{
		Internal_AddMessageToTradeServiceLog(Message.GetChars(), ShowLog, AddChartStudyDetails);
	}

	int GetStudyIDByName(int ChartNumber, const char* Name, const int UseShortNameIfSet)
	{
		return Internal_GetStudyIDByName(ChartNumber, Name, UseShortNameIfSet);
	}

	int GetStudyIDByName(int ChartNumber, const SCString& Name, const int UseShortNameIfSet)
	{
		return Internal_GetStudyIDByName(ChartNumber, Name.GetChars(), UseShortNameIfSet);
	}


	/*========================================================================

	BHCS_BAR_HAS_CLOSED: Element at BarIndex has closed.

	BHCS_BAR_HAS_NOT_CLOSED: Element at BarIndex has not closed.

	BHCS_SET_DEFAULTS: Configuration and defaults are being set.  Allow your SetDefaults code block to run.
	------------------------------------------------------------------------*/

	int GetBarHasClosedStatus()
	{
		return GetBarHasClosedStatus(Index);
	}

	int GetBarHasClosedStatus(int BarIndex)
	{
		if (SetDefaults)
			return BHCS_SET_DEFAULTS;

		if (BarIndex != ArraySize - 1)
			return BHCS_BAR_HAS_CLOSED;
		else
			return BHCS_BAR_HAS_NOT_CLOSED;
	}

	float GetHighest(SCFloatArrayRef In, int Index, int Length)
	{
		float High = -FLT_MAX;

		// Get the high starting at Index going back by Length in the In array
		for (int SrcIndex = Index; SrcIndex > Index - Length; --SrcIndex)
		{
			if (SrcIndex < 0 || SrcIndex >= In.GetArraySize())
				continue;

			if (In[SrcIndex] > High)
				High = In[SrcIndex];
		}

		return High;
	}

	float GetHighest(SCFloatArrayRef In, int Length)
	{
		return GetHighest( In, Index,  Length);
	}

	float GetLowest(SCFloatArrayRef In, int Index, int Length)
	{
		float Low = FLT_MAX;

		// Get the low starting at Index going back by Length in the In array
		for (int SrcIndex = Index; SrcIndex > Index - Length; --SrcIndex)
		{
			if (SrcIndex < 0 || SrcIndex >= In.GetArraySize())
				continue;

			if (In[SrcIndex] < Low)
				Low = In[SrcIndex];
		}

		return Low;
	}
	float GetLowest(SCFloatArrayRef In,int Length)
	{
		return GetLowest( In, Index, Length);
	}

	int GetIndexOfHighestValue(SCFloatArrayRef In, int Index, int Length)
	{
		float High = -FLT_MAX;

		int IndexAtHighest = 0;
		// Get the high starting at Index going back by Length in the In array
		for (int SrcIndex = Index; SrcIndex > Index - Length; --SrcIndex)
		{
			if (SrcIndex < 0 || SrcIndex >= In.GetArraySize())
				continue;

			if (In[SrcIndex] > High)
			{
				High = In[SrcIndex];
				IndexAtHighest = SrcIndex;
			}
		}

		return IndexAtHighest;
	}

	int GetIndexOfHighestValue(SCFloatArrayRef In, int Length)
	{
		return GetIndexOfHighestValue( In, Index,  Length);
	}


	int GetIndexOfLowestValue(SCFloatArrayRef In, int Index, int Length)
	{
		float Low = FLT_MAX;

		int IndexAtLowest = 0;
		// Get the high starting at Index going back by Length in the In array
		for (int SrcIndex = Index; SrcIndex > Index - Length; --SrcIndex)
		{
			if (SrcIndex < 0 || SrcIndex >= In.GetArraySize())
				continue;

			if (In[SrcIndex] < Low)
			{
				Low = In[SrcIndex];
				IndexAtLowest = SrcIndex;
			}
		}

		return IndexAtLowest;
	}

	int GetIndexOfLowestValue(SCFloatArrayRef In, int Length)
	{
		return GetIndexOfLowestValue( In, Index,  Length);
	}

	int CrossOver(SCFloatArrayRef First, SCFloatArrayRef Second)
	{
		return CrossOver( First,  Second,  Index);
	}

	int CrossOver(SCFloatArrayRef First, SCFloatArrayRef Second, int Index)
	{
		float X1 = First[Index-1];
		float X2 = First[Index];
		float Y1 = Second[Index-1];
		float Y2 = Second[Index];

		if (X2 != Y2)  // The following is not useful if X2 and Y2 are equal
		{
			// Find non-equal values for prior values
			int PriorIndex = Index - 1;
			while (X1 == Y1 && PriorIndex > 0 && PriorIndex > Index - 100)
			{
				--PriorIndex;
				X1 = First[PriorIndex];
				Y1 = Second[PriorIndex];
			}
		}

		if (X1 > Y1 && X2 < Y2)
			return CROSS_FROM_TOP;
		else if (X1 < Y1 && X2 > Y2)
			return CROSS_FROM_BOTTOM;
		else
			return NO_CROSS;
	}

	int ResizeArrays(int NewSize)
	{
		return p_ResizeArrays(NewSize);
	}


	int AddElements(int NumElements)
	{
		return p_AddElements(NumElements);
	}

	int FormattedEvaluate(float Value1, unsigned int Value1Format,
		OperatorEnum Operator,
		float Value2, unsigned int Value2Format,
		float PrevValue1 = 0.0f, float PrevValue2 = 0.0f,
		int* CrossDirection = NULL)
	{
		return InternalFormattedEvaluate(Value1, Value1Format, Operator, Value2, Value2Format, PrevValue1, PrevValue2, CrossDirection)?1:0;
	}

	int FormattedEvaluateUsingDoubles(double Value1, unsigned int Value1Format,
		OperatorEnum Operator,
		double Value2, unsigned int Value2Format,
		double PrevValue1 = 0.0f, double PrevValue2 = 0.0f,
		int* CrossDirection = NULL)
	{
		return InternalFormattedEvaluateUsingDoubles(Value1, Value1Format, Operator, Value2, Value2Format, PrevValue1, PrevValue2, CrossDirection)?1:0;
	}

	int PlaySound(int AlertNumber)
	{
		return InternalAlertWithMessage(AlertNumber, "", 0 );
	}

	int PlaySound(int AlertNumber, const SCString& AlertMessage, int ShowAlertLog = 0)
	{
		return InternalAlertWithMessage(AlertNumber, AlertMessage.GetChars(), ShowAlertLog);
	}

	int PlaySound(int AlertNumber, const char* AlertMessage, int ShowAlertLog = 0)
	{
		return InternalAlertWithMessage(AlertNumber, AlertMessage, ShowAlertLog);
	}

	int AlertWithMessage(int AlertNumber, const SCString& AlertMessage, int ShowAlertLog = 0)
	{
		return InternalAlertWithMessage(AlertNumber, AlertMessage.GetChars(), ShowAlertLog);
	}

	int AlertWithMessage(int AlertNumber, const char* AlertMessage, int ShowAlertLog = 0)
	{
		return InternalAlertWithMessage(AlertNumber, AlertMessage, ShowAlertLog);
	}

	int PlaySound(const char* AlertPathAndFileName, int NumberTimesPlayAlert = 1)
	{
		return InternalPlaySoundPath(AlertPathAndFileName, NumberTimesPlayAlert, "", 0);
	}

	int PlaySound(const SCString& AlertPathAndFileName, int NumberTimesPlayAlert = 1)
	{
		return InternalPlaySoundPath(AlertPathAndFileName.GetChars(), NumberTimesPlayAlert, "", 0);
	}

	int PlaySound( SCString& AlertPathAndFileName, int NumberTimesPlayAlert, SCString& AlertMessage, int ShowAlertLog = 0)
	{
		return InternalPlaySoundPath(AlertPathAndFileName.GetChars(), NumberTimesPlayAlert, AlertMessage.GetChars(), ShowAlertLog);
	}

	int PlaySound(const char* AlertPathAndFileName, int NumberTimesPlayAlert, const char* AlertMessage, int ShowAlertLog = 0)
	{
		return InternalPlaySoundPath(AlertPathAndFileName, NumberTimesPlayAlert, AlertMessage, ShowAlertLog);
	}

	void AddAlertLine(const SCString& Message, int ShowAlertLog = 0)
	{
		InternalAddAlertLine(Message.GetChars(), ShowAlertLog);
	}

	void AddAlertLine(const char* Message, int ShowAlertLog = 0)
	{
		InternalAddAlertLine(Message, ShowAlertLog);
	}

	SCString GetStudyName(int StudyIndex)
	{
		SCString Temp;
		Temp = InternalGetStudyName(StudyIndex);
		return Temp;
	}

	SCFloatArrayRef CCI(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier, unsigned int MovingAverageType = MOVAVGTYPE_SIMPLE)
	{
		return InternalCCI(In, SMAOut, CCIOut, Index, Length, Multiplier, MovingAverageType);
	}

	SCFloatArrayRef CCI(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Length, float Multiplier, unsigned int MovingAverageType = MOVAVGTYPE_SIMPLE)
	{
		return InternalCCI(In, SMAOut, CCIOut, Index, Length, Multiplier, MovingAverageType);
	}

	SCFloatArrayRef CCI(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length, float Multiplier, unsigned int MovingAverageType = MOVAVGTYPE_SIMPLE)
	{
		return InternalCCI(In, Out.Arrays[0], Out, Index, Length, Multiplier, MovingAverageType);
	}

	SCFloatArrayRef CCI(SCFloatArrayRef In, SCSubgraphRef Out, int Length, float Multiplier, unsigned int MovingAverageType = MOVAVGTYPE_SIMPLE)
	{
		return InternalCCI(In, Out.Arrays[0], Out, Index, Length, Multiplier, MovingAverageType);
	}

	SCFloatArrayRef RSI(SCFloatArrayRef In, SCSubgraphRef RsiOut, int Index, unsigned int MovingAverageType, int Length)
	{
		SCFloatArrayRef UpSumsTemp = RsiOut.Arrays[0];
		SCFloatArrayRef DownSumsTemp = RsiOut.Arrays[1];
		SCFloatArrayRef SmoothedUpSumsTemp = RsiOut.Arrays[2];
		SCFloatArrayRef SmoothedDownSumsTemp = RsiOut.Arrays[3];

		return InternalRSI(In, RsiOut, UpSumsTemp, DownSumsTemp, SmoothedUpSumsTemp, SmoothedDownSumsTemp,
							Index, MovingAverageType, Length);
	};


	SCFloatArrayRef RSI(SCFloatArrayRef In, SCSubgraphRef RsiOut, unsigned int MovingAverageType, int Length)
	{
		SCFloatArrayRef UpSumsTemp = RsiOut.Arrays[0];
		SCFloatArrayRef DownSumsTemp = RsiOut.Arrays[1];
		SCFloatArrayRef SmoothedUpSumsTemp = RsiOut.Arrays[2];
		SCFloatArrayRef SmoothedDownSumsTemp = RsiOut.Arrays[3];

		return InternalRSI(In, RsiOut,
							UpSumsTemp, DownSumsTemp, SmoothedUpSumsTemp, SmoothedDownSumsTemp,
							Index, MovingAverageType, Length);
	};


	SCFloatArrayRef RSI(SCFloatArrayRef In, SCFloatArrayRef RsiOut,
						SCFloatArrayRef UpSumsTemp, SCFloatArrayRef DownSumsTemp,
						SCFloatArrayRef SmoothedUpSumsTemp, SCFloatArrayRef SmoothedDownSumsTemp,
						int Index, unsigned int MovingAverageType, int Length)
	{
		return InternalRSI(In, RsiOut,
							UpSumsTemp, DownSumsTemp, SmoothedUpSumsTemp, SmoothedDownSumsTemp,
							Index, MovingAverageType, Length);
	}


	SCFloatArrayRef RSI(SCFloatArrayRef In, SCFloatArrayRef RsiOut,
						SCFloatArrayRef UpSumsTemp, SCFloatArrayRef DownSumsTemp,
						SCFloatArrayRef SmoothedUpSumsTemp, SCFloatArrayRef SmoothedDownSumsTemp,
						unsigned int MovingAverageType, int Length)
	{
		return InternalRSI(In, RsiOut,
							UpSumsTemp, DownSumsTemp, SmoothedUpSumsTemp, SmoothedDownSumsTemp,
							Index, MovingAverageType, Length);
	}

	void DMI(SCBaseDataRef BaseDataIn, SCSubgraphRef PosDMIOut, SCSubgraphRef NegDMIOut, int Index, int Length)
	{
		SCFloatArrayRef InternalPosDM = PosDMIOut.Arrays[0];
		SCFloatArrayRef InternalNegDM = PosDMIOut.Arrays[1];
		SCFloatArrayRef InternalTrueRangeSummation = PosDMIOut.Arrays[2];
		SCFloatArrayRef DiffDMIOut = PosDMIOut.Arrays[3];

		InternalDMI(BaseDataIn,Index, Length, 1 /* rounding disabled */,
							PosDMIOut, NegDMIOut, DiffDMIOut,
							InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
		return;
	}

	void DMI(SCBaseDataRef BaseDataIn, SCSubgraphRef PosDMIOut, SCSubgraphRef NegDMIOut, int Length)
	{
		SCFloatArrayRef InternalPosDM = PosDMIOut.Arrays[0];
		SCFloatArrayRef InternalNegDM = PosDMIOut.Arrays[1];
		SCFloatArrayRef InternalTrueRangeSummation = PosDMIOut.Arrays[2];
		SCFloatArrayRef DiffDMIOut = PosDMIOut.Arrays[3];

		InternalDMI(BaseDataIn,Index, Length, 1 /* rounding disabled */,
							PosDMIOut, NegDMIOut, DiffDMIOut,
							InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
		return;
	}

	SCSubgraphRef DMI(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length)
	{
		SCFloatArrayRef InternalPosDM = Out.Arrays[1];
		SCFloatArrayRef InternalNegDM = Out.Arrays[2];
		SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[3];
		SCFloatArrayRef DiffDMIOut = Out.Arrays[4];

		InternalDMI(BaseDataIn,Index, Length, 1 /* rounding disabled */,
			Out, Out.Arrays[0], DiffDMIOut, InternalTrueRangeSummation, InternalPosDM, InternalNegDM);

		return Out;
	}

	SCSubgraphRef DMI(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
	{
		SCFloatArrayRef InternalPosDM = Out.Arrays[1];
		SCFloatArrayRef InternalNegDM = Out.Arrays[2];
		SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[3];
		SCFloatArrayRef DiffDMIOut = Out.Arrays[4];

		InternalDMI(BaseDataIn,Index, Length, 1 /* rounding disabled */,
			Out, Out.Arrays[0], DiffDMIOut, InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
		return Out;
	}



	SCFloatArrayRef DMIDiff(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length)
	{
		SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
		SCFloatArrayRef InternalPosDM = Out.Arrays[1];
		SCFloatArrayRef InternalNegDM = Out.Arrays[2];

		return InternalDMIDiff(BaseDataIn, Index, Length, Out,
								InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
	}


	SCFloatArrayRef DMIDiff(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
	{
		SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
		SCFloatArrayRef InternalPosDM = Out.Arrays[1];
		SCFloatArrayRef InternalNegDM = Out.Arrays[2];

		return InternalDMIDiff(BaseDataIn, Index, Length, Out,
								InternalTrueRangeSummation, InternalPosDM, InternalNegDM);
	}

	SCFloatArrayRef ADX(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int DXLength, int DXMovAvgLength)
	{
		SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
		SCFloatArrayRef InternalPosDM = Out.Arrays[1];
		SCFloatArrayRef InternalNegDM  = Out.Arrays[2];
		SCFloatArrayRef InternalDX  = Out.Arrays[3];

		return InternalADX(BaseDataIn, Index, DXLength, DXMovAvgLength, Out,
							InternalTrueRangeSummation, InternalPosDM, InternalNegDM, InternalDX);
	}

	SCFloatArrayRef ADX(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int DXLength, int DXMovAvgLength)
	{
		SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
		SCFloatArrayRef InternalPosDM = Out.Arrays[1];
		SCFloatArrayRef InternalNegDM  = Out.Arrays[2];
		SCFloatArrayRef InternalDX  = Out.Arrays[3];

		return InternalADX(BaseDataIn, Index, DXLength, DXMovAvgLength, Out,
							InternalTrueRangeSummation, InternalPosDM, InternalNegDM, InternalDX);
	}

	SCFloatArrayRef ADXR(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index,
							int DXLength, int DXMovAvgLength, int ADXRInterval)
	{
		SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
		SCFloatArrayRef InternalPosDM  = Out.Arrays[1];
		SCFloatArrayRef InternalNegDM  = Out.Arrays[2];
		SCFloatArrayRef InternalDX = Out.Arrays[3];
		SCFloatArrayRef InternalADX = Out.Arrays[4];

		return InternalADXR(BaseDataIn, Index, DXLength, DXMovAvgLength, ADXRInterval, Out,
							InternalTrueRangeSummation, InternalPosDM, InternalNegDM, InternalDX, InternalADX);
	}

	SCFloatArrayRef ADXR(SCBaseDataRef BaseDataIn, SCSubgraphRef Out,
							int DXLength, int DXMovAvgLength, int ADXRInterval)
	{
		SCFloatArrayRef InternalTrueRangeSummation = Out.Arrays[0];
		SCFloatArrayRef InternalPosDM  = Out.Arrays[1];
		SCFloatArrayRef InternalNegDM  = Out.Arrays[2];
		SCFloatArrayRef InternalDX = Out.Arrays[3];
		SCFloatArrayRef InternalADX = Out.Arrays[4];

		return InternalADXR(BaseDataIn, Index, DXLength, DXMovAvgLength, ADXRInterval, Out,
							InternalTrueRangeSummation, InternalPosDM, InternalNegDM, InternalDX, InternalADX);
	}

	SCFloatArrayRef Ergodic(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int LongEMALength, int ShortEMALength, float Multiplier)
	{
		return Internal_Ergodic( In,  Out,  Index,  LongEMALength,  ShortEMALength,  Multiplier,
			Out.Arrays[0],  Out.Arrays[1],  Out.Arrays[2],  Out.Arrays[3],  Out.Arrays[4],  Out.Arrays[5]);
	}

	SCFloatArrayRef Ergodic(SCFloatArrayRef In, SCSubgraphRef Out, int LongEMALength, int ShortEMALength, float Multiplier)
	{
		return Internal_Ergodic( In,  Out,  Index,  LongEMALength,  ShortEMALength,  Multiplier,
			Out.Arrays[0],  Out.Arrays[1],  Out.Arrays[2],  Out.Arrays[3],  Out.Arrays[4],  Out.Arrays[5]);
	}


	SCFloatArrayRef Keltner(SCBaseDataRef BaseDataIn, SCFloatArrayRef In, SCSubgraphRef Out, int Index, int KeltnerMALength, unsigned int KeltnerMAType, int TrueRangeMALength, unsigned int TrueRangeMAType, float TopBandMultiplier, float BottomBandMultiplier)
	{
		return Internal_Keltner( BaseDataIn,  In,  Out, Out.Arrays[0], Out.Arrays[1], Index,  KeltnerMALength, KeltnerMAType,  TrueRangeMALength, TrueRangeMAType,  TopBandMultiplier, BottomBandMultiplier,  Out.Arrays[2], Out.Arrays[3]);
	}

	SCFloatArrayRef Keltner(SCBaseDataRef BaseDataIn, SCFloatArrayRef In, SCSubgraphRef Out, int KeltnerMALength, unsigned int KeltnerMAType, int TrueRangeMALength, unsigned int TrueRangeMAType, float TopBandMultiplier, float BottomBandMultiplier)
	{
		return Internal_Keltner( BaseDataIn,  In,  Out, Out.Arrays[0], Out.Arrays[1], Index,  KeltnerMALength, KeltnerMAType,  TrueRangeMALength, TrueRangeMAType,  TopBandMultiplier, BottomBandMultiplier,  Out.Arrays[2], Out.Arrays[3]);
	}


	SCFloatArrayRef  SmoothedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return Internal_SmoothedMovingAverage( In,  Out,  Index,  Length, 0);
	}

	SCFloatArrayRef  SmoothedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return Internal_SmoothedMovingAverage( In,  Out,  Index,  Length, 0);
	}

	SCFloatArrayRef SmoothedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, int Offset)
	{
		return Internal_SmoothedMovingAverage( In,  Out,  Index,  Length, Offset);
	}


	SCFloatArrayRef ATR(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length, unsigned int MovingAverageType)
	{
		return InternalATR( BaseDataIn,  Out.Arrays[0],  Out,  Index,  Length, MovingAverageType);
	}

	SCFloatArrayRef ATR(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length, unsigned int MovingAverageType)
	{
		return InternalATR( BaseDataIn,  Out.Arrays[0],  Out,  Index,  Length, MovingAverageType);
	}

	SCFloatArrayRef ATR(SCBaseDataRef BaseDataIn, SCFloatArrayRef TROut, SCFloatArrayRef ATROut, int Index, int Length, unsigned int MovingAverageType)
	{
		return InternalATR( BaseDataIn,  TROut,  ATROut,  Index,  Length, MovingAverageType);
	}

	SCFloatArrayRef ATR(SCBaseDataRef BaseDataIn, SCFloatArrayRef TROut, SCFloatArrayRef ATROut, int Length, unsigned int MovingAverageType)
	{
		return InternalATR( BaseDataIn,  TROut,  ATROut,  Index,  Length, MovingAverageType);
	}

	void Vortex(SCBaseDataRef BaseDataIn, SCSubgraphRef VMPlusOut, SCSubgraphRef VMMinusOut, int Index, int VortexLength)
	{
		InternalVortex(BaseDataIn, VMPlusOut.Arrays[0], VMPlusOut.Arrays[1], VMPlusOut.Arrays[2], VMPlusOut, VMMinusOut, Index, VortexLength);
	}

	void Vortex(SCBaseDataRef BaseDataIn, SCSubgraphRef VMPlusOut, SCSubgraphRef VMMinusOut, int VortexLength)
	{
		InternalVortex(BaseDataIn, VMPlusOut.Arrays[0], VMPlusOut.Arrays[1], VMPlusOut.Arrays[2], VMPlusOut, VMMinusOut, Index, VortexLength);
	}

	SCFloatArrayRef Stochastic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
	{
		InternalStochastic( BaseDataIn,  Out,  Out.Arrays[0],  Out.Arrays[1],  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
		return Out;
	}

	SCFloatArrayRef Stochastic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
	{
		InternalStochastic( BaseDataIn,  Out,  Out.Arrays[0],  Out.Arrays[1],  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
		return Out;
	}

	//Three Separate Input Arrays
	SCFloatArrayRef Stochastic(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCSubgraphRef Out, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
	{
		InternalStochastic2( InputDataHigh, InputDataLow, InputDataLast,  Out,  Out.Arrays[0],  Out.Arrays[1],  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
		return Out;
	}

	//Three Separate Input Arrays
	SCFloatArrayRef Stochastic(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCSubgraphRef Out, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType)
	{
		InternalStochastic2( InputDataHigh, InputDataLow, InputDataLast,  Out,  Out.Arrays[0],  Out.Arrays[1],  Index,  FastKLength,  FastDLength,  SlowDLength, MovingAverageType);
		return Out;
	}

	SCFloatArrayRef MACD(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int FastMALength, int SlowMALength, int MACDMALength, int MovAvgType)
	{
		return InternalMACD( In, Out.Arrays[0], Out.Arrays[1], Out.Data, Out.Arrays[2], Out.Arrays[3], Index, FastMALength, SlowMALength, MACDMALength, MovAvgType);
	}

	SCFloatArrayRef MACD(SCFloatArrayRef In, SCSubgraphRef Out, int FastMALength, int SlowMALength, int MACDMALength, int MovAvgType)
	{
		return InternalMACD( In, Out.Arrays[0], Out.Arrays[1], Out.Data, Out.Arrays[2], Out.Arrays[3], Index, FastMALength, SlowMALength, MACDMALength, MovAvgType);
	}


	SCFloatArrayRef HullMovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalHullMovingAverage( In,  Out,  Out.Arrays[0], Out.Arrays[1], Out.Arrays[2],  Index,  Length);
	}

	SCFloatArrayRef HullMovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, int Length)
	{
		return InternalHullMovingAverage( In,  Out,  Out.Arrays[0], Out.Arrays[1], Out.Arrays[2],  Index,  Length);
	}

	SCFloatArrayRef TriangularMovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalTriangularMovingAverage( In, Out, Out.Arrays[0], Index, Length);
	}

	SCFloatArrayRef TriangularMovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, int Length)
	{
		return InternalTriangularMovingAverage( In, Out, Out.Arrays[0], Index, Length);
	}

	SCFloatArrayRef TEMA(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int  Length)
	{
		return InternalTEMA( In,  Out, Out.Arrays[0],Out.Arrays[1],Out.Arrays[2],  Index,   Length);
	}

	SCFloatArrayRef TEMA(SCFloatArrayRef In, SCSubgraphRef Out, int  Length)
	{
		return InternalTEMA( In,  Out, Out.Arrays[0],Out.Arrays[1],Out.Arrays[2],  Index,   Length);
	}

	SCFloatArrayRef BollingerBands(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int  Length, float Multiplier, int MovingAverageType)
	{
		InternalBollingerBands(In, Out,  Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Index, Length, Multiplier, MovingAverageType);
		return Out;
	}

	SCFloatArrayRef BollingerBands(SCFloatArrayRef In, SCSubgraphRef Out, int  Length, float Multiplier, int MovingAverageType)
	{
		InternalBollingerBands(In, Out,  Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Index, Length, Multiplier, MovingAverageType);
		return Out;
	}


	SCFloatArrayRef WeightedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalWeightedMovingAverage( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef WeightedMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalWeightedMovingAverage( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef Momentum(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalMomentum(In,Out,Index,Length);
	}

	SCFloatArrayRef Momentum(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalMomentum(In,Out,Index,Length);
	}

	SCFloatArrayRef TRIX(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalTRIX(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Index, Length);
	}

	SCFloatArrayRef TRIX(SCFloatArrayRef In, SCSubgraphRef Out, int Length)
	{
		return InternalTRIX(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Index, Length);
	}

	SCFloatArrayRef AroonIndicator(SCFloatArrayRef FloatArrayInHigh, SCFloatArrayRef FloatArrayInLow, SCSubgraphRef Out, int Index, int Length)
	{
		return Internal_AroonIndicator( FloatArrayInHigh, FloatArrayInLow,  Out.Data, Out.Arrays[0], Out.Arrays[1], Index,  Length);
	}

	SCFloatArrayRef AroonIndicator(SCFloatArrayRef FloatArrayInHigh, SCFloatArrayRef FloatArrayInLow, SCSubgraphRef Out, int Length)
	{
			return Internal_AroonIndicator( FloatArrayInHigh, FloatArrayInLow,  Out.Data, Out.Arrays[0], Out.Arrays[1], Index,  Length);
	}

	SCFloatArrayRef Demarker(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length)
	{
		return Internal_Demarker( BaseDataIn,  Out, Out.Arrays[1], Out.Arrays[3], Out.Arrays[2], Out.Arrays[4], Index,  Length);
	}

	SCFloatArrayRef Demarker(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
	{
		return Internal_Demarker( BaseDataIn,  Out, Out.Arrays[1], Out.Arrays[3], Out.Arrays[2], Out.Arrays[4], Index,  Length);
	}


	int GetOHLCOfTimePeriod(SCDateTime StartDateTime, SCDateTime EndDateTime, float& Open, float& High, float& Low, float& Close, float& NextOpen, int& NumberOfBars, SCDateTime& TotalTimeSpan)
	{
		s_GetOHLCOfTimePeriod Data(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);

		return InternalGetOHLCOfTimePeriod( Data );

	}

	int GetOHLCOfTimePeriod(SCDateTime StartDateTime, SCDateTime EndDateTime, float& Open, float& High, float& Low, float& Close, float& NextOpen)
	{
		int NumberOfBars;
		SCDateTime TotalTimeSpan;

		s_GetOHLCOfTimePeriod Data(StartDateTime, EndDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);

		return InternalGetOHLCOfTimePeriod( Data );

	}

	int GetOHLCForDate(SCDateTime Date, float& Open, float& High, float& Low, float& Close)
	{
		return Internal_GetOHLCForDate(Date, Open, High, Low, Close);
	}
	
	//Parameters reviewed for safety with different compilers
	int GetOpenHighLowCloseVolumeForDate(const SCDateTime& Date, float& r_Open, float& r_High, float& r_Low, float& r_Close, float& r_Volume)
	{
		return Internal_GetOpenHighLowCloseVolumeForDate(Date, r_Open, r_High, r_Low, r_Close, r_Volume);
	}

	//Parameters reviewed for safety with different compilers
	int GetOpenHighLowCloseVolumeForDate(const SCDateTime& Date, float& r_Open, float& r_High, float& r_Low, float& r_Close, float& r_Volume, int IncludeFridayEveningSessionWithSundayEveningSession)
	{
		return Internal_GetOpenHighLowCloseVolumeForDate2(Date, r_Open, r_High, r_Low, r_Close, r_Volume, IncludeFridayEveningSessionWithSundayEveningSession);
	}
	

	int AreTimeSpecificBars()
	{
		if (NumberOfTradesPerBar == 0 &&
			VolumePerBar == 0 &&
			RangeBarValue == 0 &&
			RenkoTicksPerBar == 0 &&
			ReversalTicksPerBar==0 &&
			PriceChangesPerBar == 0 &&
			DeltaVolumePerBar == 0)
			return 1;

		return 0;
	}

	int AreRangeBars()
	{
		if(RangeBarValue != 0)
			return 1;

		return 0;
	}

	int AreNumberOfTradesBars()
	{
		if(NumberOfTradesPerBar != 0)
			return 1;

		return 0;
	}

	int AreVolumeBars()
	{
		if(VolumePerBar != 0)
			return 1;

		return 0;
	}

	int AreRenkoBars()
	{
		if (RenkoTicksPerBar != 0)
			return 1;

		return 0;
	}

	int AreReversalBars()
	{
		if (ReversalTicksPerBar != 0)
			return 1;

		return 0;
	}

	int ArePriceChangesBars()
	{
		if (PriceChangesPerBar != 0)
			return 1;

		return 0;
	}

	int AreDeltaVolumeBars()
	{
		if (DeltaVolumePerBar != 0)
			return 1;

		return 0;
	}

	int ArePointAndFigureBars()
	{
		if (PointAndFigureBoxSizeInTicks != 0)
			return 1;

		return 0;
	}

	int IsReplayRunning()
	{
		if(ReplayStatus != REPLAY_STOPPED)
			return 1;

		return 0;
	}

	SCDateTimeMS GetCurrentDateTime()
	{
		if (IsReplayRunning())
			return CurrentDateTimeForReplay;
		else
			return CurrentSystemDateTimeMS;
	}

	int GetExactMatchForSCDateTime(int ChartNumber, const SCDateTime& DateTime)
	{
		int Index = GetContainingIndexForSCDateTime(ChartNumber, DateTime);

		SCDateTimeArray ChartDateTimeArray;
		GetChartDateTimeArray(ChartNumber, ChartDateTimeArray);

		if (ChartDateTimeArray[Index] == DateTime)
			return Index;
		else
			return -1;
	}

	SCFloatArrayRef Highest(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalHighest( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef Highest(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalHighest( In,  Out,  Index,  Length);
	}


	SCFloatArrayRef Lowest(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalLowest( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef Lowest(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalLowest( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef ChaikinMoneyFlow(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalChaikinMoneyFlow(BaseDataIn, Out, Out.Arrays[0], Index, Length);
	}

	SCFloatArrayRef ChaikinMoneyFlow(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
	{
		return InternalChaikinMoneyFlow(BaseDataIn, Out, Out.Arrays[0], Index, Length);
	}

	SCFloatArrayRef EnvelopePct(SCFloatArrayRef In, SCSubgraphRef Out, float pct, int Index)
	{
		return InternalEnvelopePercent(In, Out, Out.Arrays[0], pct, Index);
	}

	SCFloatArrayRef EnvelopePct(SCFloatArrayRef In, SCSubgraphRef Out, float pct)
	{
		return InternalEnvelopePercent(In, Out, Out.Arrays[0], pct, Index);
	}

	SCFloatArrayRef EnvelopeFixed(SCFloatArrayRef In, SCSubgraphRef Out, float FixedValue, int Index)
	{
		return InternalEnvelopeFixed(In, Out, Out.Arrays[0], FixedValue, Index);
	}

	SCFloatArrayRef EnvelopeFixed(SCFloatArrayRef In, SCSubgraphRef Out, float FixedValue)
	{
		return InternalEnvelopeFixed(In, Out, Out.Arrays[0], FixedValue, Index);
	}

	SCFloatArrayRef RandomWalkIndicator(SCBaseDataRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalRWI(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Index, Length);
	}

	SCFloatArrayRef RandomWalkIndicator(SCBaseDataRef In, SCSubgraphRef Out, int Length)
	{
		return InternalRWI(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Index, Length);
	}

	SCFloatArrayRef UltimateOscillator(SCBaseDataRef In, SCSubgraphRef Out1, SCSubgraphRef Out2,int Index, int Length1, int Length2, int Length3)
	{
		return InternalUltimateOscillator( In,
			Out1,
			Out1.Arrays[0],
			Out1.Arrays[1],
			Out1.Arrays[2],
			Out1.Arrays[3],
			Out1.Arrays[4],
			Out1.Arrays[5],
			Out1.Arrays[6],
			Out1.Arrays[7],
			Out1.Arrays[8],
			Out2.Arrays[0],
			Out2.Arrays[1],
			Out2.Arrays[2],
			Out2.Arrays[3],
			Index, Length1, Length2, Length3);
	}

	SCFloatArrayRef UltimateOscillator(SCBaseDataRef In, SCSubgraphRef Out1, SCSubgraphRef Out2, int Length1, int Length2, int Length3)
	{
		return InternalUltimateOscillator( In,
			Out1,
			Out1.Arrays[0],
			Out1.Arrays[1],
			Out1.Arrays[2],
			Out1.Arrays[3],
			Out1.Arrays[4],
			Out1.Arrays[5],
			Out1.Arrays[6],
			Out1.Arrays[7],
			Out1.Arrays[8],
			Out2.Arrays[0],
			Out2.Arrays[1],
			Out2.Arrays[2],
			Out2.Arrays[3],
			Index, Length1, Length2, Length3);
	}

	SCFloatArrayRef Oscillator(SCFloatArrayRef In1, SCFloatArrayRef In2, SCFloatArrayRef Out, int Index)
	{
		return InternalOscillator(In1, In2, Out, Index);
	}

	SCFloatArrayRef Oscillator(SCFloatArrayRef In1, SCFloatArrayRef In2, SCFloatArrayRef Out)
	{
		return InternalOscillator(In1, In2, Out, Index);
	}

	int UseTool(s_UseTool& ToolDetails)
	{
		return Internal_UseToolEx(ToolDetails);
	}

	void CalculateRegressionStatistics(SCFloatArrayRef In, double &Slope, double &Y_Intercept,  int Index, int Length)
	{
		return  Internal_CalculateRegressionStatistics( In,  Slope, Y_Intercept, Index, Length);
	}

	void CalculateRegressionStatistics(SCFloatArrayRef In, double &Slope, double &Y_Intercept,  int Length)
	{
		return  Internal_CalculateRegressionStatistics( In,  Slope, Y_Intercept, Index, Length);
	}

	void CalculateLogLogRegressionStatistics(SCFloatArrayRef In, double &Slope, double &Y_Intercept, int Index, int Length)
	{
		return  Internal_CalculateLogLogRegressionStatistics(In, Slope, Y_Intercept, Index, Length);
	}

	void CalculateLogLogRegressionStatistics(SCFloatArrayRef In, double &Slope, double &Y_Intercept, int Length)
	{
		return  Internal_CalculateLogLogRegressionStatistics(In, Slope, Y_Intercept, Index, Length);
	}
	SCFloatArrayRef LinearRegressionIndicatorAndStdErr(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef StdErr, int Index, int Length)
	{
		return  InternalLinearRegressionIndicatorAndStdErr( In,  Out,  StdErr, Index, Length);
	}

	SCFloatArrayRef LinearRegressionIndicatorAndStdErr(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef StdErr, int Length)
	{
		return InternalLinearRegressionIndicatorAndStdErr( In,  Out,  StdErr, Index, Length);
	}

	SCFloatArrayRef PriceVolumeTrend(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalPriceVolumeTrend( BaseDataIn,  Out,  Index);
	}

	SCFloatArrayRef PriceVolumeTrend(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalPriceVolumeTrend( BaseDataIn,  Out,  Index);
	}

	int NumberOfBarsSinceLowestValue(SCFloatArrayRef in, int Index, int Length)
	{
		return InternalNumberOfBarsSinceLowestValue( in,  Index,  Length);
	}
	int NumberOfBarsSinceLowestValue(SCFloatArrayRef in, int Length)
	{
		return InternalNumberOfBarsSinceLowestValue( in,  Index,  Length);
	}

	int NumberOfBarsSinceHighestValue(SCFloatArrayRef in, int Index, int Length)
	{
		return InternalNumberOfBarsSinceHighestValue( in,  Index,  Length);
	}

	int NumberOfBarsSinceHighestValue(SCFloatArrayRef in, int Length)
	{
		return InternalNumberOfBarsSinceHighestValue( in,  Index,  Length);
	}

	float GetCorrelationCoefficient(SCFloatArrayRef In1, SCFloatArrayRef In2, int Index, int Length)
	{
		float value;
		value = InternalGetCorrelationCoefficient( In1,  In2,  Index,  Length);
		return value;
	}
	float GetCorrelationCoefficient(SCFloatArrayRef In1, SCFloatArrayRef In2, int Length)
	{
		float value;
		value = InternalGetCorrelationCoefficient( In1,  In2,  Index,  Length);
		return value;
	}

	float GetTrueRange(SCBaseDataRef BaseDataIn, int Index)
	{
		return InternalGetTrueRange( BaseDataIn,  Index);
	}
	float GetTrueRange(SCBaseDataRef BaseDataIn)
	{
		return InternalGetTrueRange( BaseDataIn,  Index);
	}

	float GetTrueLow(SCBaseDataRef BaseDataIn, int Index)
	{
		return InternalGetTrueLow( BaseDataIn,  Index);
	}
	float GetTrueLow(SCBaseDataRef BaseDataIn)
	{
		return InternalGetTrueLow( BaseDataIn,  Index);
	}

	float GetTrueHigh(SCBaseDataRef BaseDataIn, int Index)
	{
		return InternalGetTrueHigh( BaseDataIn,  Index);
	}
	float GetTrueHigh(SCBaseDataRef BaseDataIn)
	{
		return InternalGetTrueHigh( BaseDataIn,  Index);
	}

	int GetIslandReversal(SCBaseDataRef BaseDataIn, int Index)
	{
		return InternalGetIslandReversal( BaseDataIn,  Index);
	}

	int GetIslandReversal(SCBaseDataRef BaseDataIn)
	{
		return InternalGetIslandReversal( BaseDataIn,  Index);
	}


	SCFloatArrayRef Parabolic(SCBaseDataRef BaseDataIn, SCDateTimeArrayRef BaseDateTimeIn, SCSubgraphRef Out,  float InStartAccelFactor, float InAccelIncrement, float InMaxAccelFactor, unsigned int InAdjustForGap, int InputDataHighIndex = SC_HIGH, int InputDataLowIndex = SC_LOW)
	{
		s_Parabolic ParabolicData( BaseDataIn,  Out, BaseDateTimeIn, Index, InStartAccelFactor, InAccelIncrement, InMaxAccelFactor, InAdjustForGap, InputDataHighIndex, InputDataLowIndex);

		return InternalParabolic(ParabolicData);
	}

	SCFloatArrayRef Parabolic(SCBaseDataRef BaseDataIn, SCDateTimeArrayRef BaseDateTimeIn, SCSubgraphRef Out, int Index, float InStartAccelFactor, float InAccelIncrement, float InMaxAccelFactor, unsigned int InAdjustForGap, int InputDataHighIndex = SC_HIGH, int InputDataLowIndex = SC_LOW)
	{
		s_Parabolic ParabolicData( BaseDataIn,  Out, BaseDateTimeIn, Index, InStartAccelFactor, InAccelIncrement, InMaxAccelFactor, InAdjustForGap, InputDataHighIndex, InputDataLowIndex);

		return InternalParabolic(ParabolicData);
	}

	SCFloatArrayRef ZigZag(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, int Index, float ReversalPercent, int StartIndex = 0)
	{
		return InternalResettableZigZag(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1], StartIndex, Index, ReversalPercent, 0.0f, *this);
	}

	SCFloatArrayRef ZigZag(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, float ReversalPercent, int StartIndex = 0)
	{
		return InternalResettableZigZag(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1],  StartIndex, Index, ReversalPercent, 0.0f, *this);
	}

	SCFloatArrayRef ZigZag(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, int Index, float ReversalPercent, float ReversalAmount, int StartIndex = 0)
	{
		return InternalResettableZigZag(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1],  StartIndex, Index, ReversalPercent, ReversalAmount, *this);
	}

	SCFloatArrayRef ZigZag(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, float ReversalPercent, float ReversalAmount, int StartIndex = 0)
	{
		return InternalResettableZigZag(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1],  StartIndex, Index, ReversalPercent, ReversalAmount, *this);
	}

	SCFloatArrayRef ZigZag2(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, int Index, int NumberOfBars, float ReversalAmount, int StartIndex = 0)
	{
		return InternalResettableZigZag2(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1], StartIndex, Index, NumberOfBars, ReversalAmount, *this);
	}

	SCFloatArrayRef ZigZag2(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCSubgraphRef Out, int NumberOfBars, float ReversalAmount, int StartIndex = 0)
	{
		return InternalResettableZigZag2(InputDataHigh, InputDataLow, Out, Out.Arrays[0], Out.Arrays[1], StartIndex, Index, NumberOfBars, ReversalAmount, *this);
	}

	SCFloatArrayRef WilliamsR(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalWilliamsR( BaseDataIn,  Out,  Index,  Length);
	}
	SCFloatArrayRef WilliamsR(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Length)
	{
		return InternalWilliamsR( BaseDataIn,  Out,  Index,  Length);
	}


	//Three Separate Input Arrays
	SCFloatArrayRef WilliamsR(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalWilliamsR2( InputDataHigh, InputDataLow, InputDataLast,  Out,  Index, Length);
	}

	//Three Separate Input Arrays without Index
	SCFloatArrayRef WilliamsR(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCFloatArrayRef Out, int Length)
	{
		return InternalWilliamsR2( InputDataHigh, InputDataLow, InputDataLast,  Out,  Index, Length);
	}

	SCFloatArrayRef WilliamsAD(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalWilliamsAD( BaseDataIn,  Out,  Index);
	}
	SCFloatArrayRef WilliamsAD(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalWilliamsAD( BaseDataIn,  Out,  Index);
	}

	SCFloatArrayRef VHF(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalVHF( In,  Out, Index,  Length);
	}
	SCFloatArrayRef VHF(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalVHF( In,  Out,  Index,  Length);
	}

	float GetDispersion(SCFloatArrayRef In, int Index, int Length)
	{
		return InternalGetDispersion( In,  Index,  Length);
	}
	float GetDispersion(SCFloatArrayRef In, int Length)
	{
		return InternalGetDispersion( In,  Index,  Length);
	}

	SCFloatArrayRef Dispersion(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalDispersion( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef Dispersion(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalDispersion( In,  Out,  Index, Length);
	}

	SCFloatArrayRef Summation(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalSummation( In,  Out, Index, Length);
	}
	SCFloatArrayRef Summation(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalSummation( In,  Out, Index, Length);
	}

	SCFloatArrayRef ArmsEMV(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int volinc, int Index)
	{
		return InternalArmsEMV( BaseDataIn,  Out,  volinc,  Index);
	}
	SCFloatArrayRef ArmsEMV(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int volinc)
	{
		return InternalArmsEMV( BaseDataIn,  Out,  volinc,  Index);
	}

	SCFloatArrayRef CumulativeSummation(SCFloatArrayRef In, SCFloatArrayRef Out, int Index)
	{
		return InternalCumulativeSummation( In,  Out,  Index);
	}
	SCFloatArrayRef CumulativeSummation(SCFloatArrayRef In, SCFloatArrayRef Out)
	{
		return InternalCumulativeSummation( In,  Out,  Index);
	}

	float GetSummation(SCFloatArrayRef In,int Index,int Length)
	{
		return InternalGetSummation( In, Index, Length);
	}
	float GetSummation(SCFloatArrayRef In,int Length)
	{
		return InternalGetSummation( In, Index, Length);
	}

	SCFloatArrayRef VolumeWeightedMovingAverage(SCFloatArrayRef InData, SCFloatArrayRef InVolume, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalVolumeWeightedMovingAverage( InData,  InVolume,  Out,  Index,  Length);
	}
	SCFloatArrayRef VolumeWeightedMovingAverage(SCFloatArrayRef InData, SCFloatArrayRef InVolume, SCFloatArrayRef Out, int Length)
	{
		return InternalVolumeWeightedMovingAverage( InData,  InVolume,  Out,  Index,  Length);
	}

	SCFloatArrayRef WellesSum(SCFloatArrayRef In ,SCFloatArrayRef Out , int Index, int Length)
	{
		return InternalWellesSum( In , Out ,  Index, Length);
	}
	SCFloatArrayRef WellesSum(SCFloatArrayRef In ,SCFloatArrayRef Out , int Length)
	{
		return InternalWellesSum( In , Out ,  Index, Length);
	}

	SCFloatArrayRef TrueRange(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalTrueRange( BaseDataIn,  Out,  Index);
	}
	SCFloatArrayRef TrueRange(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalTrueRange( BaseDataIn,  Out,  Index);
	}


	int CalculateOHLCAverages(int Index)
	{
		return InternalCalculateOHLCAverages(Index);
	}
	int CalculateOHLCAverages()
	{
		return InternalCalculateOHLCAverages(Index);
	}

	SCFloatArrayRef StdError(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalStdError( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef StdError(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalStdError( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef MovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, unsigned int MovingAverageType, int Index, int Length)
	{
		return InternalMovingAverage( In,  Out,  MovingAverageType,  Index,  Length);
	}
	SCFloatArrayRef MovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, unsigned int MovingAverageType, int Length)
	{
		return InternalMovingAverage( In,  Out, MovingAverageType,  Index,  Length);
	}

	SCFloatArrayRef MovingMedian(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length)
	{
		return InternalMovingMedian( In,  Out, Out.Arrays[0],  Index,  Length);
	}
	SCFloatArrayRef MovingMedian(SCFloatArrayRef In, SCSubgraphRef Out, int Length)
	{
		return InternalMovingMedian( In,  Out, Out.Arrays[0],  Index,  Length);
	}

	SCDateTime GetStartOfPeriodForDateTime(SCDateTime DateTime, unsigned int TimePeriodType, int TimePeriodLength, int PeriodOffset, int NewPeriodAtBothSessionStarts )
	{
		return InternalGetStartOfPeriodForDateTime( DateTime,   TimePeriodType,  TimePeriodLength,  PeriodOffset,  NewPeriodAtBothSessionStarts );
	}

	SCDateTime GetStartOfPeriodForDateTime(SCDateTime DateTime, unsigned int TimePeriodType, int TimePeriodLength, int PeriodOffset)
	{
		return InternalGetStartOfPeriodForDateTime( DateTime,   TimePeriodType,  TimePeriodLength,  PeriodOffset,  false );
	}

	SCFloatArrayRef OnBalanceVolumeShortTerm(SCBaseDataRef BaseDataIn, SCSubgraphRef Out,  int Index, int Length)
	{
		return InternalOnBalanceVolumeShortTerm( BaseDataIn,  Out,  Out.Arrays[0], Index,  Length);
	}
	SCFloatArrayRef OnBalanceVolumeShortTerm(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length)
	{
		return InternalOnBalanceVolumeShortTerm( BaseDataIn,  Out,  Out.Arrays[0],  Index,  Length);
	}

	SCFloatArrayRef OnBalanceVolume(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalOnBalanceVolume( BaseDataIn,  Out,  Index);
	}
	SCFloatArrayRef OnBalanceVolume(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalOnBalanceVolume( BaseDataIn,  Out,  Index);
	}

	SCFloatArrayRef StdDeviation(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalStdDeviation( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef StdDeviation(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalStdDeviation( In,  Out,  Index, Length);
	}

	SCFloatArrayRef WildersMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalWildersMovingAverage( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef WildersMovingAverage(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalWildersMovingAverage( In,  Out,  Index, Length);
	}

	SCFloatArrayRef SimpleMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalSimpleMovAvg( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef SimpleMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalSimpleMovAvg( In,  Out,  Index, Length);
	}

	SCFloatArrayRef AdaptiveMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, float FastSmoothConst, float SlowSmoothConst)
	{
		return InternalAdaptiveMovAvg( In,  Out,  Index, Length,  FastSmoothConst,  SlowSmoothConst);
	}
	SCFloatArrayRef AdaptiveMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Length, float FastSmoothConst, float SlowSmoothConst)
	{
		return InternalAdaptiveMovAvg( In,  Out,  Index,  Length,  FastSmoothConst,  SlowSmoothConst);
	}

	SCFloatArrayRef LinearRegressionIndicator(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalLinearRegressionIndicator( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef LinearRegressionIndicator(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalLinearRegressionIndicator( In,  Out,  Index,  Length);
	}

	SCFloatArrayRef ExponentialMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length)
	{
		return InternalExponentialMovAvg( In,  Out,  Index,  Length);
	}
	SCFloatArrayRef ExponentialMovAvg(SCFloatArrayRef In, SCFloatArrayRef Out, int Length)
	{
		return InternalExponentialMovAvg( In,  Out,  Index,  Length);
	}

	float ArrayValueAtNthOccurrence (SCFloatArrayRef TrueFalseIn, SCFloatArrayRef ValueArrayIn, int Index, int NthOccurrence )
	{
		return InternalArrayValueAtNthOccurrence (TrueFalseIn, ValueArrayIn, Index, NthOccurrence );
	}

	float ArrayValueAtNthOccurrence (SCFloatArrayRef TrueFalseIn, SCFloatArrayRef ValueArrayIn, int NthOccurrence )
	{
		return InternalArrayValueAtNthOccurrence (TrueFalseIn, ValueArrayIn, Index, NthOccurrence );
	}


	t_OrderQuantity32_64 BuyEntry(s_SCNewOrder& NewOrder, int DataArrayIndex)
	{
		return InternalBuyEntry(NewOrder, DataArrayIndex);
	}

	t_OrderQuantity32_64 BuyEntry(s_SCNewOrder& NewOrder)
	{
		return InternalBuyEntry(NewOrder, Index);
	}

	t_OrderQuantity32_64 BuyOrder(s_SCNewOrder& NewOrder)
	{
		// For different Symbol or Trade Account
		if ((!NewOrder.Symbol.IsEmpty()
				&& (NewOrder.Symbol.CompareNoCase(GetTradeSymbol()) != 0))
			|| (!NewOrder.TradeAccount.IsEmpty()
				&& (NewOrder.TradeAccount.CompareNoCase(SelectedTradeAccount) != 0))
			)
		{
			return InternalSubmitOrder(NewOrder, Index, BSE_BUY);
		}

		return InternalBuyEntry(NewOrder, Index);
	}

	t_OrderQuantity32_64 BuyExit(s_SCNewOrder& NewOrder, int DataArrayIndex)
	{
		return InternalBuyExit(NewOrder, DataArrayIndex);
	}
	t_OrderQuantity32_64 BuyExit(s_SCNewOrder& NewOrder)
	{
		return InternalBuyExit(NewOrder, Index);
	}

	t_OrderQuantity32_64 SellEntry(s_SCNewOrder& NewOrder, int DataArrayIndex)
	{
		return InternalSellEntry(NewOrder, DataArrayIndex);
	}
	t_OrderQuantity32_64 SellEntry(s_SCNewOrder& NewOrder)
	{
		return InternalSellEntry(NewOrder, Index);
	}

	t_OrderQuantity32_64 SellOrder(s_SCNewOrder& NewOrder)
	{
		// For different Symbol or Trade Account
		if ((!NewOrder.Symbol.IsEmpty()
			&& (NewOrder.Symbol.CompareNoCase(GetTradeSymbol()) != 0))
			|| (!NewOrder.TradeAccount.IsEmpty()
			&& (NewOrder.TradeAccount.CompareNoCase(SelectedTradeAccount) != 0))
			)
		{
			return InternalSubmitOrder(NewOrder, Index, BSE_SELL);
		}

		return InternalSellEntry(NewOrder, Index);
	}

	t_OrderQuantity32_64 SellExit(s_SCNewOrder& NewOrder, int DataArrayIndex)
	{
		return InternalSellExit(NewOrder, DataArrayIndex);
	}
	t_OrderQuantity32_64 SellExit(s_SCNewOrder& NewOrder)
	{
		return InternalSellExit(NewOrder, Index);
	}

	t_OrderQuantity32_64 SubmitOCOOrder(s_SCNewOrder& NewOrder)
	{
		if(NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_STOP_SELL_STOP
			|| NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_STOP_LIMIT_SELL_STOP_LIMIT
			|| NewOrder.OrderType == SCT_ORDERTYPE_OCO_BUY_LIMIT_SELL_LIMIT
			)
			return InternalSubmitOCOOrder(NewOrder, Index);
		else
			return SCTRADING_NOT_OCO_ORDER_TYPE;//only allow this function to be used with OCO order types

	}


	int GetTradePosition(s_SCPositionData& PositionData)
	{
		return InternalGetTradePosition(PositionData);
	}


	int IsSwingHigh(SCFloatArrayRef In, int Index, int Length)
	{
		for(int IndexOffset = 1; IndexOffset <= Length; IndexOffset++)
		{

			if (FormattedEvaluate(In[Index],  BasedOnGraphValueFormat, LESS_EQUAL_OPERATOR,In[Index - IndexOffset], BasedOnGraphValueFormat)
				|| FormattedEvaluate(In[Index],  BasedOnGraphValueFormat, LESS_EQUAL_OPERATOR,In[Index + IndexOffset], BasedOnGraphValueFormat) )
			{
				return 0;
			}
		}

		return 1;
	}

	int IsSwingHigh(SCFloatArrayRef In, int Length)
	{
		return IsSwingHigh(In, Index, Length);
	}

	int IsSwingLow(SCFloatArrayRef In, int Index, int Length)
	{
		for(int IndexOffset = 1; IndexOffset <= Length; IndexOffset++)
		{

			if (FormattedEvaluate(In[Index],  BasedOnGraphValueFormat, GREATER_EQUAL_OPERATOR,In[Index - IndexOffset], BasedOnGraphValueFormat)
				|| FormattedEvaluate(In[Index],  BasedOnGraphValueFormat, GREATER_EQUAL_OPERATOR,In[Index + IndexOffset], BasedOnGraphValueFormat) )
			{
				return 0;
			}
		}

		return 1;
	}

	int IsSwingLow(SCFloatArrayRef In, int Length)
	{
		return IsSwingLow(In, Index, Length);
	}

	SCFloatArrayRef AwesomeOscillator(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int Length1, int Length2)
	{
		SCFloatArrayRef TempMA1 = Out.Arrays[0];
		SCFloatArrayRef TempMA2 = Out.Arrays[1];

		return InternalAwesomeOscillator(In, Out, TempMA1, TempMA2, Index, Length1, Length2);
	}

	SCFloatArrayRef AwesomeOscillator(SCFloatArrayRef In, SCSubgraphRef Out, int Length1, int Length2)
	{
		SCFloatArrayRef TempMA1 = Out.Arrays[0];
		SCFloatArrayRef TempMA2 = Out.Arrays[1];

		return InternalAwesomeOscillator(In, Out, TempMA1, TempMA2, Index, Length1, Length2);
	}

	SCFloatArrayRef Slope(SCFloatArrayRef In, SCFloatArrayRef Out)
	{
		return InternalSlope(In, Out, Index);
	}

	SCFloatArrayRef Slope(SCFloatArrayRef In, SCFloatArrayRef Out, int Index)
	{
		return InternalSlope(In, Out, Index);
	}

	SCFloatArrayRef CumulativeDeltaVolume(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaVolume(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	SCFloatArrayRef CumulativeDeltaVolume(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaVolume(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	SCFloatArrayRef CumulativeDeltaTicks(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaTicks(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	SCFloatArrayRef CumulativeDeltaTicks(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaTicks(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	SCFloatArrayRef CumulativeDeltaTickVolume(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaTickVolume(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	SCFloatArrayRef CumulativeDeltaTickVolume(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int ResetCumulativeCalculation)
	{
		return InternalCumulativeDeltaTickVolume(BaseDataIn, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out, Index, ResetCumulativeCalculation);
	}

	int Round(float Number)
	{
		int integer=(int)Number;


		if ((Number > 0.0) && ((Number-integer) >= .5))
			return ++integer;
		else if ((Number < 0.0) && ((Number - integer) <= -.5))
			return --integer;

		return integer;

	}

	//rounds to the nearest TickSize
	float RoundToTickSize(float Value)
	{
		return (float)RoundToTickSize((double)Value, (double)TickSize);
	}

	double RoundToTickSize(double Value)
	{
		return RoundToTickSize(Value, (double)TickSize);
	}

	float RoundToTickSize(float Value, float TickSize)
	{
		return (float)RoundToTickSize((double)Value, (double)TickSize);
	}

	double RoundToTickSize(double Value, float TickSize)
	{
		return RoundToTickSize(Value, (double)TickSize);
	}

	double RoundToIncrement(double Value, float TickSize)
	{
		return RoundToTickSize(Value, (double)TickSize);
	}

	double RoundToTickSize(double Value, double TickSize)
	{
		if (TickSize == 0)
			return Value;  // cannot round

		double ClosestMult = (int)(Value / TickSize) * TickSize;
		double Diff = Value - ClosestMult;

		double DifferenceFromIncrement = TickSize*0.5 - Diff;

		double Result;
		if (Value > 0.0 && DifferenceFromIncrement <= TickSize * 0.001)
			Result = ClosestMult + TickSize;
		else if (Value < 0.0 && DifferenceFromIncrement <= TickSize * 0.001)
			Result = ClosestMult - TickSize;
		else
			Result = ClosestMult;

		return Result;
	}

	unsigned int PriceValueToTicks(float PriceValue)
	{

		return Round(PriceValue/TickSize);
	}

	float TicksToPriceValue(unsigned int Ticks)
	{

		return Ticks*TickSize;
	}

	// This uses the Volume Value Format chart setting to determine the multiplier.  This is used when volumes have fractional values and are stored as an integer.
	float MultiplierFromVolumeValueFormat() const
	{
		float Multiplier;
		switch (VolumeValueFormat)
		{
			default:
			case 0: Multiplier = 1.0f; break;
			case 1: Multiplier = 0.1f; break;
			case 2: Multiplier = 0.01f; break;
			case 3: Multiplier = 0.001f; break;
			case 4: Multiplier = 0.0001f; break;
			case 5: Multiplier = 0.00001f; break;
			case 6: Multiplier = 0.000001f; break;
			case 7: Multiplier = 0.0000001f; break;
			case 8: Multiplier = 0.00000001f; break;
		}
		return Multiplier;
	}

	void SetAlert(int AlertNumber, int ArrayIndex, const SCString& Message = "")
	{

		InternalSetAlert(AlertNumber, ArrayIndex, Message);
	}
	void SetAlert(int AlertNumber, const SCString& Message = "")
	{
		SetAlert(AlertNumber, Index, Message);
	}

	int GetNumberOfBaseGraphArrays() const
	{
		return BaseDataIn.GetArraySize();
	}

	SCString &GetRealTimeSymbol()
	{
		if (TradeAndCurrentQuoteSymbol.GetLength() > 0)
			return TradeAndCurrentQuoteSymbol;
		else
			return Symbol;

	}

	/*========================================================================
		Returns the color at the RGB distance between Color1 and Color2,
		where ColorDistance is a value between 0 and 1.  If ColorDistance is 0, then Color1 is returned.
		If ColorDistance is 1, then Color2 is returned.  If ColorDistance is 0.5f, then the color half
		way between Color1 and Color2 is returned.
	------------------------------------------------------------------------*/
	COLORREF RGBInterpolate(const COLORREF& Color1, const COLORREF& Color2, float ColorDistance)
	{
		return RGB((int)(GetRValue(Color1)*(1.0f-(ColorDistance)) + GetRValue(Color2)*(ColorDistance) + 0.5f),
			(int)(GetGValue(Color1)*(1.0f-(ColorDistance)) + GetGValue(Color2)*(ColorDistance) + 0.5f),
			(int)(GetBValue(Color1)*(1.0f-(ColorDistance)) + GetBValue(Color2)*(ColorDistance) + 0.5f));
	}

	SCDateTime ConvertDateTimeToChartTimeZone(const SCDateTime& DateTime, const char* TimeZonePOSIXString)
	{
		return InternalConvertDateTimeToChartTimeZone(DateTime, TimeZonePOSIXString);
	}

	SCDateTime ConvertDateTimeToChartTimeZone(const SCDateTime& DateTime, TimeZonesEnum TimeZone)
	{
		// The reason we are passing a string instead of the enumeration
		// value is because the enumeration value could change if the list of
		// time zones ever changes, but the string will not change.
		return InternalConvertDateTimeToChartTimeZone(DateTime, GetPosixStringForTimeZone(TimeZone));
	}

	SCDateTime ConvertDateTimeFromChartTimeZone(const SCDateTime& DateTime, const char* TimeZonePOSIXString)
	{
		return InternalConvertDateTimeFromChartTimeZone(DateTime, TimeZonePOSIXString);
	}

	SCDateTime ConvertDateTimeFromChartTimeZone(const SCDateTime& DateTime, TimeZonesEnum TimeZone)
	{
		return InternalConvertDateTimeFromChartTimeZone(DateTime, GetPosixStringForTimeZone(TimeZone));
	}

	SCFloatArrayRef DoubleStochastic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Length, int MovAvgLength, int MovingAverageType)
	{
		InternalDoubleStochastic(BaseDataIn, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Index, Length, MovAvgLength, MovingAverageType);
		return Out;
	}

	SCFloatArrayRef DoubleStochastic(SCBaseDataRef BaseDataIn, SCSubgraphRef Out, int Index, int Length, int MovAvgLength, int MovingAverageType)
	{
		InternalDoubleStochastic(BaseDataIn, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Index, Length, MovAvgLength, MovingAverageType);
		return Out;
	}

	int GetStudyIDByIndex(int ChartNumber, int StudyNumber)
	{
		return Internal_GetStudyIDByIndex(ChartNumber, StudyNumber);
	}

	double GetStandardError(SCFloatArrayRef In, int Index, int Length)
	{
		return InternalGetStandardError(In, Index, Length);
	}

	double GetStandardError(SCFloatArrayRef In, int Length)
	{
		return InternalGetStandardError(In, Index, Length);
	}

	SCFloatArrayRef AccumulationDistribution ( SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index)
	{
		return InternalAccumulationDistribution(BaseDataIn, Out, Index);
	}

	SCFloatArrayRef AccumulationDistribution ( SCBaseDataRef BaseDataIn, SCFloatArrayRef Out)
	{
		return InternalAccumulationDistribution(BaseDataIn, Out, Index);
	}

	int GetValueFormat()
	{
		if(ValueFormat == VALUEFORMAT_INHERITED)
		{
			return BasedOnGraphValueFormat;
		}
		else
			return ValueFormat;
	}

	void* AllocateMemory(int Size)
	{
		char* Pointer = (char*)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)Size);

		if (Pointer != NULL)
			memset(Pointer, 0, Size);

		return Pointer;
	}

	void FreeMemory(void* Pointer)
	{
		if (Pointer != NULL)
			HeapFree(GetProcessHeap(), 0, Pointer);
	}

	//This function is for Intraday charts and determines if the given BarIndex is the beginning of a new trading day according to the Session Times for the chart.
	bool IsNewTradingDay(int BarIndex)
	{
		SCDateTime CurrentBarTradingDayStartDateTime = GetTradingDayStartDateTimeOfBar(BaseDateTimeIn[BarIndex]);
		SCDateTime PriorBarTradingDayStartDateTime = GetTradingDayStartDateTimeOfBar(BaseDateTimeIn[BarIndex -1]);

		return PriorBarTradingDayStartDateTime != CurrentBarTradingDayStartDateTime;


	}

	const char * GetTradingErrorTextMessage(int ErrorCode)
	{
		switch (ErrorCode)
		{
		case SCTRADING_ORDER_ERROR:
			return "General order error. Refer to 'Trade >> Trade Service Log' for specific message for this trading action error" ;
		case SCTRADING_NOT_OCO_ORDER_TYPE:
			return "Not an OCO order type";
		case SCTRADING_ATTACHED_ORDER_OFFSET_NOT_SUPPORTED_WITH_MARKET_PARENT:
			return "Attached order offset not supported with a parent market order. Use actual price instead";
		case SCTRADING_UNSUPPORTED_ATTACHED_ORDER:
			return "Unsupported Attached Order";
		case SCTRADING_SYMBOL_SETTINGS_NOT_FOUND:
			return "Symbol Settings not found for symbol";
		case ACSIL_GENERAL_NULL_POINTER_ERROR:
			return "General null pointer error";
		case SCTRADING_SYMBOL_SETTINGS_DO_NOT_MATCH_SERVICE:
			return "Symbol Settings do not match current service";
		case SCT_SKIPPED_DOWNLOADING_HISTORICAL_DATA:
			return "Order entry skipped because downloading historical data";
		case SCT_SKIPPED_FULL_RECALC:
			return "Order entry skipped because full recalculation";
		case SCT_SKIPPED_ONLY_ONE_TRADE_PER_BAR:
			return "Order entry skipped because only one trade per bar is allowed";
		case SCT_SKIPPED_INVALID_INDEX_SPECIFIED:
			return "Order entry skipped because invalid bar index specified";
		case SCT_SKIPPED_TOO_MANY_NEW_BARS_DURING_UPDATE:
			return "Order entry skipped because too many new bars during chart update";
		case SCT_SKIPPED_AUTO_TRADING_DISABLED:
			return "Order entry skipped because automated trading is disabled";
		case SCTRADING_UNSUPPORTED_ORDER_TYPE:
			return "Unsupported order type";
		case SCTRADING_ERROR_SETTING_ORDER_PRICES:
			return "Error setting order prices";
		default:
			return "unknown order error";
		}
	}
	
	void GetStudyArrayFromChartUsingID(const s_ChartStudySubgraphValues& ChartStudySubgraphValues, SCFloatArrayRef SubgraphArray)
	{
		InternalGetStudyArrayFromChartUsingID(ChartStudySubgraphValues, SubgraphArray);
	}


	void GetStudyArrayFromChartUsingID(int ChartNumber, int StudyID , int SubgraphIndex, SCFloatArrayRef SubgraphArray)
	{
		s_ChartStudySubgraphValues ChartStudySubgraphValues;
		ChartStudySubgraphValues.ChartNumber = ChartNumber;
		ChartStudySubgraphValues.StudyID = StudyID;
		ChartStudySubgraphValues.SubgraphIndex = SubgraphIndex;

		InternalGetStudyArrayFromChartUsingID(ChartStudySubgraphValues, SubgraphArray);
	}

	const SCString& GetTradeSymbol() const
	{
		if (!TradeAndCurrentQuoteSymbol.IsEmpty())
			return TradeAndCurrentQuoteSymbol;

		return Symbol;
	}

	bool IsDateTimeContainedInBarIndex(const SCDateTime& DateTime, int BarIndex) const
	{
		SCDateTime BeginDateTime = BaseDateTimeIn[BarIndex];
		SCDateTime EndDateTime;

		if (BarIndex < ArraySize - 1)
			EndDateTime = BaseDateTimeIn[BarIndex + 1];
		else
			EndDateTime = DateTimeOfLastFileRecord;

		return DateTime >= BeginDateTime && DateTime < EndDateTime;		
	}

	void HeikinAshi(SCBaseDataRef BaseDataIn, SCSubgraphRef HeikinAshiOut, int Index, int Length, int SetCloseToCurrentPriceAtLastBar)
	{
		InternalHeikinAshi(BaseDataIn, Index, Length,  HeikinAshiOut.Data, HeikinAshiOut.Arrays[0], HeikinAshiOut.Arrays[1], HeikinAshiOut.Arrays[2], SetCloseToCurrentPriceAtLastBar);
	}

	void HeikinAshi(SCBaseDataRef BaseDataIn, SCSubgraphRef HeikinAshiOut, int Length, int SetCloseToCurrentPriceAtLastBar)
	{
		InternalHeikinAshi(BaseDataIn, Index, Length,  HeikinAshiOut.Data, HeikinAshiOut.Arrays[0], HeikinAshiOut.Arrays[1], HeikinAshiOut.Arrays[2], SetCloseToCurrentPriceAtLastBar);
	}

	//Parameters reviewed for safety with different compilers.
	bool IsDateTimeContainedInBarAtIndex(const SCDateTime& DateTime, int BarIndex)
	{
		return DateTime >= BaseDateTimeIn[BarIndex] && DateTime  < BaseDateTimeIn[BarIndex+1];
	}

	void InverseFisherTransform(SCFloatArrayRef In, SCSubgraphRef Out, int HighestLowestLength, int MovingAverageLength, int MovAvgType)
	{

		Internal_InverseFisherTransform(In, Out, Out.Arrays[0], Out.Arrays[1], Index, HighestLowestLength, MovingAverageLength, MovAvgType);
	}

	void InverseFisherTransform(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int HighestLowestLength, int MovingAverageLength, int MovAvgType)
	{

		Internal_InverseFisherTransform(In, Out, Out.Arrays[0], Out.Arrays[1], Index, HighestLowestLength, MovingAverageLength, MovAvgType);
	}

	void InverseFisherTransformRSI(SCFloatArrayRef In, SCSubgraphRef Out, int RSILength, int  InternalRSIMovAvgType, int RSIMovingAverageLength, int  MovingAverageOfRSIType)
	{

		Internal_InverseFisherTransformRSI(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Out.Arrays[4], Out.Arrays[5], Out.Arrays[6], Index, RSILength, InternalRSIMovAvgType, RSIMovingAverageLength, MovingAverageOfRSIType);
	}

	void InverseFisherTransformRSI(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int RSILength, int  InternalRSIMovAvgType, int RSIMovingAverageLength, int  MovingAverageOfRSIType)
	{

		Internal_InverseFisherTransformRSI(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Out.Arrays[4], Out.Arrays[5], Out.Arrays[6], Index, RSILength, InternalRSIMovAvgType, RSIMovingAverageLength, MovingAverageOfRSIType);
	}

	void MovingAverageCumulative(SCFloatArrayRef In, SCFloatArrayRef Out)
	{

		Internal_MovingAverageCumulative(In, Out, Index);
	}

	void MovingAverageCumulative(SCFloatArrayRef In, SCFloatArrayRef Out, int Index)
	{

		Internal_MovingAverageCumulative(In, Out, Index);
	}

	SCFloatArrayRef HurstExponent(SCFloatArrayRef In, SCSubgraphRef Out, int LengthIndex)
	{
		return Internal_HurstExponent(In, Out, Index, LengthIndex);
	}

	SCFloatArrayRef HurstExponent(SCFloatArrayRef In, SCSubgraphRef Out, int Index, int LengthIndex)
	{
		return  Internal_HurstExponent(In, Out, Index, LengthIndex);
	}

	int BarIndexToRelativeHorizontalCoordinate(int BarIndex)
	{
		//This function is not always guaranteed to give an accurate result. It can be an inaccurate when a  chart is first loaded

		if (BarIndex < IndexOfFirstVisibleBar)
			return 0;
		
		if (BarIndex > IndexOfLastVisibleBar)
			return (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION;


		int NumberOfBars = IndexOfLastVisibleBar - IndexOfFirstVisibleBar + 1 + NumFillSpaceBars;

		float Percent = (BarIndex - IndexOfFirstVisibleBar) / (float)NumberOfBars;

		return (int)(Percent * CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

	}

	void FillSubgraphElementsWithLinearValuesBetweenBeginEndValues(int SubgraphIndex, int BeginIndex, int EndIndex)
	{
		if (BeginIndex >= EndIndex)
			return;

		int  Distance = EndIndex - BeginIndex;

		double Difference = Subgraph[SubgraphIndex].Data[EndIndex] - Subgraph[SubgraphIndex].Data[BeginIndex];
		double Increment = Difference / Distance;

		for (int BarIndex = BeginIndex +1; BarIndex < EndIndex; BarIndex++)
		{
			int Position = BarIndex - BeginIndex;
			Subgraph[SubgraphIndex].Data[BarIndex] =(float) ( Position*Increment + Subgraph[SubgraphIndex].Data[BeginIndex]);
		}
	}

	bool IsNewBar(int BarIndex)
	{
		if (IsFullRecalculation || UpdateStartIndex == 0)
			return false;

		//This function is not supported for automatic looping
		if (AutoLoop)
			return false;

		return BarIndex > UpdateStartIndex;
	}
	/*============================================================================*/
	/* This function receives a BarIndex parameter specifying a particular chart bar index. It gets the Date-Time of that chart bar and returns the beginning and ending indexes into the c_SCTimeAndSalesArray array set by the sc.GetTimeAndSales() function. The beginning and ending time and sales array indexes are set through the r_BeginIndex and r_EndIndex integers passed by reference to the function.

	r_BeginIndex and r_EndIndex will be set to -1 if there is no element in the time and sales array contained within the bar specified by BarIndex
	*/
	void GetTimeSalesArrayIndexesForBarIndex(int BarIndex, int& r_BeginIndex, int & r_EndIndex)
	{
		//-1 indicates there are no valid indexes in the time and sales array for the given BarIndex.
		r_BeginIndex = -1;
		r_EndIndex = -1;

		c_SCTimeAndSalesArray TimeAndSalesArray;
		GetTimeAndSales(TimeAndSalesArray);
		int TSSize = TimeAndSalesArray.Size();
		if (TSSize == 0)
			return;

		SCDateTimeMS  BarBeginDateTime = BaseDateTimeIn[BarIndex];
		SCDateTimeMS  BarEndDateTime = GetEndingDateTimeForBarIndex(BarIndex);
		for (int TSIndex = TSSize - 1; TSIndex >= 0; --TSIndex)
		{
			// Adjust a time and sales record date-time to the time zone of the chart
			SCDateTime RecordDateTime = TimeAndSalesArray[TSIndex].DateTime;
			RecordDateTime +=TimeScaleAdjustment;  

			if (TSIndex == TSSize - 1//at the end of the array
				&& RecordDateTime <= BarEndDateTime && RecordDateTime >= BarBeginDateTime)
				r_EndIndex = TSIndex;

			if (TSIndex <= TSSize - 1)
			{
				SCDateTime NextRecordDateTime = TimeAndSalesArray[TSIndex + 1].DateTime;
				NextRecordDateTime += TimeScaleAdjustment;

				if(RecordDateTime <= BarEndDateTime && NextRecordDateTime > BarEndDateTime
					&& RecordDateTime >= BarBeginDateTime)
					r_EndIndex = TSIndex;
			}

			if (TSIndex == 0//at the beginning of the array
				&& RecordDateTime >= BarBeginDateTime && RecordDateTime <= BarEndDateTime)
				r_BeginIndex = TSIndex;

			if (TSIndex > 0)
			{
				SCDateTime PreviousRecordDateTime = TimeAndSalesArray[TSIndex - 1].DateTime;
				PreviousRecordDateTime += TimeScaleAdjustment;

				if (RecordDateTime >= BarBeginDateTime && PreviousRecordDateTime < BarBeginDateTime
					&& RecordDateTime <= BarEndDateTime)
					r_BeginIndex = TSIndex;
			}


			if (r_BeginIndex != -1 && r_EndIndex != -1)
				break;

		}



		return;
	}
	/*============================================================================*/
	void T3MovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, float Multiplier, int Length)
	{
		Internal_T3MovingAverage(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Out.Arrays[4], Out.Arrays[5], Multiplier, Index, Length);
	}
	/*============================================================================*/
	void T3MovingAverage(SCFloatArrayRef In, SCSubgraphRef Out, float Multiplier, int Index, int Length)
	{
		Internal_T3MovingAverage(In, Out, Out.Arrays[0], Out.Arrays[1], Out.Arrays[2], Out.Arrays[3], Out.Arrays[4], Out.Arrays[5], Multiplier, Index, Length);
	}
	/*============================================================================*/
	void CalculateAngle(SCFloatArrayRef InputArray, SCFloatArrayRef OutputArray, int Length, float ValuePerPoint)
	{
		Internal_CalculateAngle(InputArray, OutputArray, Index, Length, ValuePerPoint);
	}
	/*============================================================================*/
	void CalculateAngle(SCFloatArrayRef InputArray, SCFloatArrayRef OutputArray, int Index, int Length, float ValuePerPoint)
	{
		Internal_CalculateAngle(InputArray, OutputArray, Index, Length, ValuePerPoint);
	}
	/*============================================================================*/

	int GetBarsSinceLastTradeOrderEntry()
	{
		s_SCPositionData PositionData;
		GetTradePosition(PositionData);

		if (PositionData.LastEntryDateTime.IsUnset())
			return 0;

		int BarsSinceEntry = ArraySize - GetContainingIndexForSCDateTime(ChartNumber, PositionData.LastEntryDateTime) - 1;

		return BarsSinceEntry;
	}
	/*============================================================================*/
	int GetBarsSinceLastTradeOrderExit()
	{
		s_SCPositionData PositionData;
		GetTradePosition(PositionData);

		if (PositionData.LastExitDateTime.IsUnset())
			return 0;

		int BarsSinceExit = ArraySize - GetContainingIndexForSCDateTime(ChartNumber, PositionData.LastExitDateTime) - 1;

		return BarsSinceExit;
	}
	/*============================================================================*/
	//Parameters reviewed for safety with different compilers.
	bool IsIsSufficientTimePeriodInDate(const SCDateTime& DateTime, float Percentage)
	{
		SCDateTime  StartDateTime = GetTradingDayStartDateTimeOfBar(DateTime);
		SCDateTime  EndDateTime = StartDateTime + 24 * HOURS - 1 * SECONDS;
		int  FirstIndex = GetContainingIndexForSCDateTime(ChartNumber, StartDateTime);
		int  LastIndex = GetContainingIndexForSCDateTime(ChartNumber, EndDateTime);

		const SCDateTime ActualTimeSpan = CalculateTimeSpanAcrossChartBars(FirstIndex, LastIndex);

		//If there is a sufficient amount of data in this time period
		return (ActualTimeSpan >= (1 * DAYS * (Percentage * 0.01)));


	}	
	/*============================================================================*/
	SCDateTimeMS TimeSpanOfBar( int BarIndex)
	{
		//this function requires that sc.MaintainAdditionalChartDataArrays = 1 be set. 
		return (BaseDataEndDateTime[BarIndex] - BaseDateTimeIn[BarIndex] ) + 1 * SECONDS;

	}

	/*============================================================================*/

	double SlopeToAngleInDegrees(double Slope)
	{
		return atan(Slope) * 180.0 / M_PI;

	}

	/*============================================================================*/

	double AngleInDegreesToSlope(double AngleInDegrees)
	{

		return tan(AngleInDegrees*M_PI / 180);

	}

	/*============================================================================*/
	void AddAndManageSingleTextUserDrawnDrawingForStudy(SCStudyInterfaceRef &sc, bool Unused, int HorizontalPosition, int VerticalPosition, SCSubgraphRef Subgraph, int TransparentLabelBackground, SCString& TextToDisplay, int DrawAboveMainPriceGraph)
	{

		int& r_DrawingTextLineNumber = *static_cast<int*>(sc.StorageBlock);

		if (sc.LastCallToFunction)
		{
			// If the Chartbook is being closed, the Chartbook will have already been saved first by the time we enter this block.  So this particular chart drawing if it exists, will have already been saved.
			if (r_DrawingTextLineNumber != 0
				&& sc.UserDrawnChartDrawingExists(sc.ChartNumber, r_DrawingTextLineNumber) > 0)
			{
				// be sure to delete user drawn type drawing when study removed
				sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, r_DrawingTextLineNumber);
			}

			return;
		}

		//Reset the line number if the drawing no longer exists.
		if (r_DrawingTextLineNumber != 0
			&& sc.UserDrawnChartDrawingExists(sc.ChartNumber, r_DrawingTextLineNumber) == 0)
		{
			r_DrawingTextLineNumber = 0;
		}

		if (sc.HideStudy && r_DrawingTextLineNumber != 0)
		{
			sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, r_DrawingTextLineNumber);
			r_DrawingTextLineNumber = 0;
		}

		if (sc.HideStudy)
			return;

		s_UseTool Tool;
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.Region = sc.GraphRegion;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.AddAsUserDrawnDrawing = true;
		Tool.AllowSaveToChartbook = 1;

		bool CreateNeeded = r_DrawingTextLineNumber == 0;

		if (CreateNeeded)
		{
			Tool.BeginDateTime = HorizontalPosition;

			Tool.BeginValue = (float)VerticalPosition;
		}

		Tool.UseRelativeVerticalValues = true;
		Tool.Color = Subgraph.PrimaryColor;
		Tool.FontBackColor = Subgraph.SecondaryColor;
		Tool.TransparentLabelBackground = TransparentLabelBackground;

		Tool.ReverseTextColor = false;
		Tool.FontBold = true;
		Tool.FontSize = Subgraph.LineWidth;
		Tool.FontFace = sc.GetChartTextFontFaceName();

		Tool.Text = TextToDisplay;

		Tool.DrawUnderneathMainGraph = !DrawAboveMainPriceGraph;

		if (r_DrawingTextLineNumber != 0)
			Tool.LineNumber = r_DrawingTextLineNumber;

		if (sc.UseTool(Tool) > 0)
			r_DrawingTextLineNumber = Tool.LineNumber;
	}

	/*============================================================================*/
	void AddAndManageSingleTextDrawingForStudy(SCStudyInterfaceRef &sc, bool DisplayInFillSpace, int HorizontalPosition, int VerticalPosition, SCSubgraphRef Subgraph, int TransparentLabelBackground, SCString& TextToDisplay, int DrawAboveMainPriceGraph)
	{
		int& r_DrawingTextLineNumber = sc.GetPersistentInt(11110000);

		if (sc.IsFullRecalculation)
			r_DrawingTextLineNumber = 0;

		//Reset the line number if the drawing no longer exists.
		if (r_DrawingTextLineNumber != 0
			&& sc.ChartDrawingExists(sc.ChartNumber, r_DrawingTextLineNumber) == 0)
		{
			r_DrawingTextLineNumber = 0;
		}

		if (sc.HideStudy && r_DrawingTextLineNumber != 0)
		{
			sc.DeleteACSChartDrawing(sc.ChartNumber, TOOL_DELETE_CHARTDRAWING,  r_DrawingTextLineNumber);
			r_DrawingTextLineNumber = 0;
		}

		if (sc.HideStudy)
			return;

		s_UseTool Tool;
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.Region = sc.GraphRegion;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;

		if (!DisplayInFillSpace)
			Tool.BeginDateTime = HorizontalPosition;
		else
			Tool.BeginDateTime = -3;

		Tool.BeginValue = (float)VerticalPosition;

		Tool.UseRelativeVerticalValues = true;
		Tool.Color = Subgraph.PrimaryColor;
		Tool.FontBackColor = Subgraph.SecondaryColor;
		Tool.TransparentLabelBackground = TransparentLabelBackground;

		Tool.ReverseTextColor = false;
		Tool.FontBold = true;
		Tool.FontSize = Subgraph.LineWidth;
		Tool.FontFace = sc.GetChartTextFontFaceName();

		Tool.Text = TextToDisplay;

		Tool.DrawUnderneathMainGraph = !DrawAboveMainPriceGraph;

		if (r_DrawingTextLineNumber != 0)
			Tool.LineNumber = r_DrawingTextLineNumber;

		if (sc.UseTool(Tool) > 0)
			r_DrawingTextLineNumber = Tool.LineNumber;
	}

	/*============================================================================*/
	double GetSymbolDataValue(SymbolDataValuesEnum ValueToReturn, const SCString& SymbolForData = SCString(), bool SubscribeToMarketData = false, bool SubscribeToMarketDepth = false)
	{
		return Internal_GetSymbolDataValue(ValueToReturn, SymbolForData.GetChars(), SubscribeToMarketData, SubscribeToMarketDepth);
	}

	/*============================================================================*/
	void OrderQuantityToString(const t_OrderQuantity32_64 Value, SCString& OutputString)
	{
#ifdef _WIN64
		OutputString.Format("%g", Value);
#else
		OutputString.Format("%d", Value);
#endif
	}
	/*============================================================================*/
	double CreateDoublePrecisionPrice(const float PriceValue)
	{
		return Round(PriceValue / TickSize)*TickSize;
	}

	/*============================================================================*/
	/************************************************************************/

	// Data Members

	int ArraySize;

	int DrawZeros;
	int DataStartIndex;
	int UpdateStartIndex;

	int GraphRegion;

	int FreeDLL;//No longer used
	float Bid;
	float Ask;

	unsigned int DailyVolume;
	int ChartNumber;
	int ChartDataType;  // 1 = historical daily data, 2 = intraday data
	int NumberOfTradesPerBar;

	int DailyDataBarPeriod;  // 1 = days, 2 = weekly, 3 = monthly
	int DaysPerBar;
	int BidSize;
	int AskSize;
	int LastSize;
	int VolumePerBar;  // This is the volume per bar setting

	int (SCDLLCALL* GetNearestMatchForDateTimeIndex)(int ChartNumber, int CallingChartIndex);

	int (SCDLLCALL* ChartDrawingExists)(int ChartNumber, int LineNumber);

	const char* VersionNumber;

	void (SCDLLCALL* p_AddMessageToLog)(const char* Message, int showlog);

	int (SCDLLCALL* DeleteACSChartDrawing)(int ChartNumber, int Tool, int LineNumber);

	void * ChartWindowHandle;
	unsigned int ProcessIdentifier;
	int LastCallToFunction;

	void* StorageBlock;  // This is 512 bytes of permanent storage for any data type


	// 73.4

	float DailyHigh;
	float DailyLow;

	// 73.7

	float RangeBarValue;
	unsigned int RangeBarType;


	// 73.8

	unsigned int NumFillSpaceBars;
	unsigned int PreserveFillSpace;  // Boolean
	SCDateTime TimeScaleAdjustment;


	// 74.4

	int (SCDLLCALL* GetNearestMatchForSCDateTime)(int ChartNumber, const double& DateTime);
	int (SCDLLCALL* GetContainingIndexForDateTimeIndex)(int ChartNumber, int CallingChartIndex);
	int (SCDLLCALL* GetContainingIndexForSCDateTime)(int ChartNumber, const double& DateTime);

	short GraphicalDisplacement;


	// 75.1

	int SetDefaults;  // Boolean


	// 78

	int UpdateAlways;  // Boolean
	unsigned int ActiveToolIndex;
	int ActiveToolYPosition;



	int IndexOfLastVisibleBar;

	unsigned int ChartBackgroundColor;
	const char* ChartTextFont;
	int StudyRegionTopCoordinate;
	int StudyRegionBottomCoordinate;


	// 119

	int IsCustomChart;  // Boolean
	int NumberOfArrays;

	int OutArraySize;
	const char* (SCDLLCALL* InternalGetStudyName)(int StudyIndex);


	// 123
	int (SCDLLCALL* Internal_UseToolEx)(s_UseTool& ToolDetails);


	// 124
	unsigned short ScaleType;  // ScaleTypeEnum
	unsigned short ScaleRangeType;  // ScaleRangeTypeEnum
	float ScaleRangeTop;
	float ScaleRangeBottom;
	float ScaleIncrement;
	float ScaleConstRange;


	// 132
	s_Old_PersistentVariables* Old_PersistVars;// Do not document.

	// 135

	float BaseGraphScaleIncrement;
	float BaseGraphHorizontalGridLineIncrement ; // Added in version 1186


	// 145
	SCString GraphName;
	SCString TextInput;
	SCString TextInputName;

	SCDateTimeArray BaseDateTimeIn;  // From the base graph
	SCFloatArrayArray BaseDataIn;  // From the base graph
	SCDateTimeArray DateTimeOut;
	SCInput145Array Input;

	SCDateTime DateTimeOfLastFileRecord;  // This is the DateTime of the last file record added to the BaseDataIn array
	int SecondsPerBar;
	SCString Symbol;
	SCString DataFile;  // The path + filename for the chart data file

	void (SCDLLCALL* GetChartArray)(int ChartNumber, int DataItem, SCFloatArrayRef PriceArray);
	void (SCDLLCALL* GetChartDateTimeArray)(int ChartNumber, SCDateTimeArrayRef DateTimeArray);
	int (SCDLLCALL* GetStudyArray)(int StudyNumber, int StudySubgraphNumber, SCFloatArrayRef SubgraphArray);

	void (SCDLLCALL* GetStudyArrayFromChart)(int ChartNumber, int StudyNumber, int StudySubgraphNumber, SCFloatArrayRef SubgraphArray);

	int ValueFormat;
	SCString StudyDescription;

	int StartTime1;
	int EndTime1;
	int StartTime2;
	int EndTime2;
	int UseSecondStartEndTimes;

	// 148

	int IndexOfFirstVisibleBar;


	// 157

	SCString& (SCDLLCALL* FormatString)(SCString& Out, const char* Format, ...);


	// 158

	const s_SCBasicSymbolData* SymbolData;

	int CalculationPrecedence;


	// 162

	int AutoLoop;
	int CurrentIndex;
	int Index;
	int StandardChartHeader;
	int DisplayAsMainPriceGraph;


	// 165

	SCFloatArrayRef (SCDLLCALL* InternalExponentialMovAvg)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalLinearRegressionIndicator)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalAdaptiveMovAvg)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, float FastSmoothConst, float SlowSmoothConst);

	SCFloatArrayRef (SCDLLCALL* InternalSimpleMovAvg)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalWildersMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalWeightedMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalStdDeviation)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalCCISMA)(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier);

	SCFloatArrayRef (SCDLLCALL* InternalHighest)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalLowest)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalATR)(SCBaseDataRef BaseDataIn, SCFloatArrayRef TROut, SCFloatArrayRef ATROut, int Index, int Length, unsigned int MovingAverageType);

	SCFloatArrayRef (SCDLLCALL* InternalOnBalanceVolume)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);

	SCFloatArrayRef (SCDLLCALL* InternalOnBalanceVolumeShortTerm)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArray& OBVTemp, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, unsigned int MovingAverageType, int Index, int Length);

	void (SCDLLCALL* InternalStochastic)(SCBaseDataRef BaseDataIn, SCFloatArrayRef FastKOut, SCFloatArrayRef FastDOut, SCFloatArrayRef SlowDOut, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType);


	// 178

	SCString UserName;

	SCFloatArrayRef (SCDLLCALL* InternalStdError)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);
	int (SCDLLCALL* InternalCalculateOHLCAverages)(int Index);


	// 199

	int (SCDLLCALL* p_ResizeArrays)(int NewSize);
	int (SCDLLCALL* p_AddElements)(int NumElements);


	// 206

	int (SCDLLCALL* GetUserDrawnChartDrawing)(int ChartNumber, int DrawingType, s_UseTool& ChartDrawing, int DrawingIndex);

	SCFloatArrayRef (SCDLLCALL* Internal_Ergodic)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int LongEMALength, int ShortEMALength, float Multiplier,
		SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2, SCFloatArrayRef InternalArray3, SCFloatArrayRef InternalArray4, SCFloatArrayRef InternalArray5, SCFloatArrayRef InternalArray6);

	SCFloatArrayRef (SCDLLCALL* Internal_Keltner)(SCBaseDataRef BaseDataIn, SCFloatArrayRef In, SCFloatArrayRef KeltnerAverageOut, SCFloatArrayRef TopBandOut, SCFloatArrayRef BottomBandOut,	int Index, int KeltnerMALength, unsigned int KeltnerMAType, int TrueRangeMALength, unsigned int TrueRangeMAType, float TopBandMultiplier, float BottomBandMultiplier,SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2);

	// 210

	SCFloatArrayArray BaseData;  // From the base graph


	// 222

	SCFloatArrayRef (SCDLLCALL* InternalTrueRange)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);

	SCFloatArrayRef (SCDLLCALL* InternalWellesSum)(SCFloatArrayRef In ,SCFloatArrayRef Out , int Index, int Length);

	void (SCDLLCALL* InternalDMI)(SCBaseDataRef BaseDataIn,
		int Index,
		int Length,  int DisableRounding,
		SCFloatArrayRef PosDMIOut, SCFloatArrayRef NegDMIOut, SCFloatArrayRef DiffDMIOut,
		SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM);

	SCFloatArrayRef (SCDLLCALL* InternalDMIDiff)(SCBaseDataRef BaseDataIn,
		int Index,
		int Length,
		SCFloatArrayRef Out,
		SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM);

	SCFloatArrayRef (SCDLLCALL* InternalADX)(SCBaseDataRef BaseDataIn,
		int Index,
		int DXLength, int DXMovAvgLength,
		SCFloatArrayRef Out,
		SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM, SCFloatArrayRef InternalDX);

	SCFloatArrayRef (SCDLLCALL* InternalADXR)(SCBaseDataRef BaseDataIn,
		int Index,
		int DXLength, int DXMovAvgLength, int ADXRInterval,
		SCFloatArrayRef Out,
		SCFloatArrayRef InternalTrueRangeSummation, SCFloatArrayRef InternalPosDM, SCFloatArrayRef InternalNegDM, SCFloatArrayRef InternalDX, SCFloatArrayRef InternalADX);

	// 235

	int FileRecordIndexOfLastDataRecordInChart;


	// 241

	//The return value for this function must be implemented as a four byte integer, because a Boolean type may not be implemented the same way on all compilers and can cause corruptions.
	int (SCDLLCALL* InternalFormattedEvaluate)(float Value1, unsigned int Value1Format,
		OperatorEnum Operator,
		float Value2, unsigned int Value2Format,
		float PrevValue1, float PrevValue2,
		int* CrossDirection);


	// 245

	int (SCDLLCALL* InternalAlertWithMessage)(int AlertNumber, const char* AlertMessage, int ShowAlertLog);

	void (SCDLLCALL* InternalAddAlertLine)(const char* Message, int ShowAlertLog);


	// 247

	SCFloatArrayRef (SCDLLCALL *InternalRSI)(SCFloatArrayRef In,  SCFloatArrayRef RsiOut, SCFloatArrayRef UpSumsTemp, SCFloatArrayRef DownSumsTemp, SCFloatArrayRef SmoothedUpSumsTemp, SCFloatArrayRef SmoothedDownSumsTemp, int Index, unsigned int AveragingType, int Length);

	SCDateTime CurrentSystemDateTime;


	// 248

	int (SCDLLCALL* InternalPlaySoundPath)(const char* AlertPathAndFileName, int NumberTimesPlayAlert, const char* AlertMessage, int ShowLog);


	// 260

	SCSubgraph260Array Subgraph;


	// 263


	// 264

	float TickSize;


	// 270

	SCFloatArrayRef (SCDLLCALL* InternalHullMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1, SCFloatArrayRef InternalArray2, SCFloatArrayRef InternalArray3, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalTriangularMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalVolumeWeightedMovingAverage)(SCFloatArrayRef InData, SCFloatArrayRef InVolume, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* Internal_SmoothedMovingAverage)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length, int Offset);


	// 273

	float (SCDLLCALL* InternalGetSummation)(SCFloatArrayRef In,int Index,int Length);


	// 275

	SCFloatArrayRef (SCDLLCALL* InternalMACD)(SCFloatArrayRef In, SCFloatArrayRef FastMAOut, SCFloatArrayRef SlowMAOut, SCFloatArrayRef MACDOut, SCFloatArrayRef MACDMAOut, SCFloatArrayRef MACDDiffOut, int Index, int FastMALength, int SlowMALength, int MACDMALength, int MovAvgType);

	void (SCDLLCALL* GetMainGraphVisibleHighAndLow)(float& High, float& Low);


	// 276

	SCFloatArrayRef (SCDLLCALL* InternalTEMA)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalArray1,SCFloatArrayRef InternalArray2,SCFloatArrayRef InternalArray3, int Index, int  Length);

	SCFloatArrayRef (SCDLLCALL* InternalBollingerBands)(SCFloatArrayRef In, SCFloatArrayRef Avg, SCFloatArrayRef TopBand,SCFloatArrayRef BottomBand,SCFloatArrayRef StdDev, int Index, int Length,float Multiplier,int MovAvgType);


	// 278

	int (SCDLLCALL* Internal_GetOHLCForDate)(double Date, float& Open, float& High, float& Low, float& Close);


	// 280

	int ReplayStatus;


	// 281

	int AdvancedFeatures;
	float PreviousClose;

	int BaseGraphValueFormat;

	SCString ((SCDLLCALL* FormatGraphValue)(double Value, int ValueFormat));

	int UseGlobalChartColors;

	unsigned int ScaleBorderColor;

	int (SCDLLCALL* GetStudyArrayUsingID)(unsigned int StudyID, unsigned int StudySubgraphIndex, SCFloatArrayRef SubgraphArray);

	int SelectedAlertSound;


	// 297

	SCFloatArrayRef (SCDLLCALL* InternalCumulativeSummation)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index);

	SCFloatArrayRef (SCDLLCALL* InternalArmsEMV)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int volinc, int Index);

	SCFloatArrayRef (SCDLLCALL* InternalChaikinMoneyFlow)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef temp, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalSummation)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalDispersion)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);

	float (SCDLLCALL* InternalGetDispersion)(SCFloatArrayRef In, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalEnvelopePercent)(SCFloatArrayRef In, SCFloatArrayRef Out1, SCFloatArrayRef Out2,  float pct, int Index);
	SCFloatArrayRef (SCDLLCALL* InternalVHF)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);
	SCFloatArrayRef (SCDLLCALL* InternalRWI)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out1, SCFloatArrayRef Out2,
		SCFloatArrayRef TrueRangeArray, SCFloatArrayRef LookBackLowArray, SCFloatArrayRef LookBackHighArray, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalUltimateOscillator)(SCBaseDataRef BaseDataIn,
		SCFloatArrayRef Out,
		SCFloatArrayRef CalcE,
		SCFloatArrayRef CalcF,
		SCFloatArrayRef CalcG,
		SCFloatArrayRef CalcH,
		SCFloatArrayRef CalcI,
		SCFloatArrayRef CalcJ,
		SCFloatArrayRef CalcK,
		SCFloatArrayRef CalcL,
		SCFloatArrayRef CalcM,
		SCFloatArrayRef CalcN,
		SCFloatArrayRef CalcO,
		SCFloatArrayRef CalcP,
		SCFloatArrayRef CalcQ,
		int Index, int Length1, int Length2, int Length3);

	SCFloatArrayRef (SCDLLCALL* InternalWilliamsAD)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);
	SCFloatArrayRef (SCDLLCALL* InternalWilliamsR)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index, int Length);
	int (SCDLLCALL* InternalGetIslandReversal)(SCBaseDataRef BaseDataIn, int Index);
	SCFloatArrayRef (SCDLLCALL* InternalOscillator)(SCFloatArrayRef In1, SCFloatArrayRef In2, SCFloatArrayRef Out, int Index);
	float (SCDLLCALL* InternalGetTrueHigh)(SCBaseDataRef BaseDataIn, int Index);
	float (SCDLLCALL* InternalGetTrueLow)(SCBaseDataRef BaseDataIn, int Index);
	float (SCDLLCALL* InternalGetTrueRange)(SCBaseDataRef BaseDataIn, int Index);
	float (SCDLLCALL* InternalGetCorrelationCoefficient)(SCFloatArrayRef In1, SCFloatArrayRef In2, int Index, int Length);

	int (SCDLLCALL* InternalNumberOfBarsSinceHighestValue)(SCFloatArrayRef in, int Index, int Length);
	int (SCDLLCALL* InternalNumberOfBarsSinceLowestValue)(SCFloatArrayRef in, int Index, int Length);

	SCFloatArrayRef(SCDLLCALL* InternalPriceVolumeTrend)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);


	// 298

	SCString ((SCDLLCALL* GetStudyNameUsingID)(unsigned int StudyID));
	int (SCDLLCALL* GetStudyDataStartIndexUsingID)(unsigned int StudyID);


	// 307

	SCFloatArrayRef (SCDLLCALL* InternalLinearRegressionIndicatorAndStdErr)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef StdErr, int Index, int Length);


	// 315

	float ValueIncrementPerBar;
	SCString ((SCDLLCALL* GetCountDownText)());


	// 318

	int GraphDrawType;


	// 335

	SCFloatArrayRef (SCDLLCALL* InternalMomentum)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int Length);


	// 338

	int (SCDLLCALL* InternalGetOHLCOfTimePeriod)(s_GetOHLCOfTimePeriod& Data);

	// 340

	int AlertOnlyOncePerBar;
	int ResetAlertOnNewBar;


	// 346

	int AllowMultipleEntriesInSameDirection;
	int SupportReversals;
	int SendOrdersToTradeService;
	int AllowOppositeEntryWithOpposingPositionOrOrders;
	t_OrderQuantity32_64(SCDLLCALL* InternalBuyEntry)(s_SCNewOrder& NewOrder, int Index);
	t_OrderQuantity32_64(SCDLLCALL* InternalBuyExit)(s_SCNewOrder& NewOrder, int Index);
	t_OrderQuantity32_64(SCDLLCALL* InternalSellEntry)(s_SCNewOrder& NewOrder, int Index);
	t_OrderQuantity32_64(SCDLLCALL* InternalSellExit)(s_SCNewOrder& NewOrder, int Index);
	int (SCDLLCALL* CancelOrder)(int InternalOrderID);
	int (SCDLLCALL* GetOrderByOrderID)(int InternalOrderID, s_SCTradeOrder& OrderDetails);
	int (SCDLLCALL* GetOrderByIndex)(int OrderIndex, s_SCTradeOrder& OrderDetails);


	// 351

	SCFloatArrayRef (SCDLLCALL* InternalTRIX)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef InternalEma_1, SCFloatArrayRef InternalEma_2, SCFloatArrayRef InternalEma_3, int Index, int Length);


	// 359

	int DownloadingHistoricalData;
	SCString CustomAffiliateCode;


	// 363

	int AllowOnlyOneTradePerBar;

	void (SCDLLCALL* InternalStochastic2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast, SCFloatArrayRef FastKOut, SCFloatArrayRef FastDOut, SCFloatArrayRef SlowDOut, int Index, int FastKLength, int FastDLength, int SlowDLength, unsigned int MovingAverageType);

	SCFloatArrayRef (SCDLLCALL* InternalWilliamsR2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef InputDataLast,  SCFloatArrayRef Out, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalParabolic)(s_Parabolic& ParabolicData);

	// 373

	int (SCDLLCALL* CancelAllOrders)();


	// 380

	int StartTimeOfDay;


	// 388

	float (SCDLLCALL* InternalArrayValueAtNthOccurrence)(SCFloatArrayRef TrueFalseIn, SCFloatArrayRef ValueArrayIn, int Index, int NthOccurrence );


	// 408

	SCFloatArrayRef (SCDLLCALL* Internal_Demarker)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef DemMax, SCFloatArrayRef DemMin, SCFloatArrayRef SmaDemMax, SCFloatArrayRef SmaDemMin, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* Old_Internal_AroonIndicator)(SCFloatArrayRef In, SCFloatArrayRef OutUp, SCFloatArrayRef OutDown, int Index, int Length);


	// 409

	int SupportAttachedOrdersForTrading;


	// 412

	int GlobalTradeSimulationIsOn;
	int ChartTradeModeEnabled;//Version 946


	// 419

	int CancelAllOrdersOnEntriesAndReversals;


	// 421

	SCFloatArrayRef (SCDLLCALL* InternalEnvelopeFixed)(SCFloatArrayRef In, SCFloatArrayRef Out1, SCFloatArrayRef Out2,  float FixedValue, int Index);


	// 429

	float CurrencyValuePerTick;


	// 434

	SCString ((SCDLLCALL* GetStudyNameFromChart)(int ChartNumber, int StudyID));


	// 436

	t_OrderQuantity32_64 MaximumPositionAllowed;


	// 450

	short ChartBarSpacing;


	// 453

	int (SCDLLCALL* InternalIsSwingHigh)(SCFloatArrayRef In, int Index, int Length);
	int (SCDLLCALL* InternalIsSwingLow)(SCFloatArrayRef In, int Index, int Length);

	SCFloatArrayRef (SCDLLCALL* InternalZigZag2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int Index, int NumberOfBars, float ReversalAmount);

	// 475

	int (SCDLLCALL* ModifyOrder)( s_SCNewOrder& OrderModifications);


	// 493

	SCFloatArrayRef (SCDLLCALL* InternalZigZag)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef  ZigZagValues,SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex , int Index, float ReversalPercent, float ReversalAmount);


	// 507

	int AllowEntryWithWorkingOrders;


	// 508

	SCFloatArrayRef (SCDLLCALL* InternalAwesomeOscillator)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef TempMA1, SCFloatArrayRef TempMA2, int Index, int Length1, int Length2);


	// 510

	int IsUserAllowedForSCDLLName;

	int (SCDLLCALL* FlattenAndCancelAllOrders)();


	// 519

	int CancelAllWorkingOrdersOnExit;


	// 521

	int (SCDLLCALL* SessionStartTime)();
	int (SCDLLCALL* IsDateTimeInSession)(const SCDateTime& DateTime);
	int (SCDLLCALL* TradingDayStartsInPreviousDate)();
	int (SCDLLCALL* GetTradingDayDate)(const SCDateTime& DateTime);
	double (SCDLLCALL* GetStartDateTimeForTradingDate)(int TradingDate);
	double (SCDLLCALL* GetTradingDayStartDateTimeOfBar)(const SCDateTime& BarDateTime);
	int (SCDLLCALL* SecondsSinceStartTime)(const SCDateTime& BarDateTime);


	// 530

	int HideStudy;


	// 538

	float ScaleValueOffset;
	float AutoScalePaddingPercentage;


	// 539

	SCString ((SCDLLCALL* GetChartName)(int ChartNumber));


	// 543

	int (SCDLLCALL* InternalGetTradePosition)(s_SCPositionData& PositionData);



	// 552

	void (SCDLLCALL* GetChartBaseData)(int ChartNumber, SCGraphData& BaseData);


	// 553

	void (SCDLLCALL* GetStudyArraysFromChart)(int ChartNumber, int StudyNumber, SCGraphData& GraphData);


	// 557

	void (SCDLLCALL* GetTimeAndSales)(c_SCTimeAndSalesArray& TSArray);


	// 560

	void * Old_VolumeAtPriceForBars;


	// 562

	int (SCDLLCALL* InternalFormattedEvaluateUsingDoubles)(double Value1, unsigned int Value1Format,
		OperatorEnum Operator,
		double Value2, unsigned int Value2Format,
		double PrevValue1, double PrevValue2,
		int* CrossDirection);


	// 569
	SCString TradeWindowConfigFileName;

	//594
	int StudyGraphInstanceID;

	//601

	int (SCDLLCALL* GetTradingDayDateForChartNumber)(int ChartNumber, const SCDateTime& DateTime);
	int (SCDLLCALL* GetFirstIndexForDate)(int ChartNumber, int Date);


	// 602

	int MaintainVolumeAtPriceData;

	SCString ((SCDLLCALL* FormatDateTime)(const SCDateTime& DateTime));


	// 613

	int DrawBaseGraphOverStudies;


	// 614
	unsigned int IntradayDataStorageTimeUnit;

	// 617
	int (SCDLLCALL* MakeHTTPRequest)(const SCString& URL);
	SCString HTTPResponse;

	void (SCDLLCALL* SetStudySubgraphDrawStyle)(int ChartNumber, int StudyID, int StudySubgraphNumber, int DrawStyle);

	// 627
	void (SCDLLCALL* InternalGetStudyArrayFromChartUsingID)(const s_ChartStudySubgraphValues& ChartStudySubgraphValues, SCFloatArrayRef SubgraphArray);

	// 630
	SCFloatArrayRef (SCDLLCALL* InternalSlope)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index);

	// 632
	void (SCDLLCALL* InternalSetAlert)(int AlertNumber, int Index, const SCString& Message);

	// 634
	int HideDLLAndFunctionNames;

	// 636
	SCFloatArray Open;
	SCFloatArray High;
	SCFloatArray Low;
	SCFloatArray Close;
	SCFloatArray Volume;
	SCFloatArray OpenInterest;
	SCFloatArray NumberOfTrades;
	SCFloatArray OHLCAvg;
	SCFloatArray HLCAvg;
	SCFloatArray HLAvg;
	SCFloatArray BidVolume;
	SCFloatArray AskVolume;
	SCFloatArray UpTickVolume;
	SCFloatArray DownTickVolume;
	SCFloatArray NumberOfBidTrades;
	SCFloatArray NumberOfAskTrades;


	// 641
	int DrawStudyUnderneathMainPriceGraph;

	// 649
	SCDateTime LatestDateTimeForLastBar;

	// 650
	int  GlobalDisplayStudyNameSubgraphNamesAndValues ;
	int DisplayStudyName;
	int DisplayStudyInputValues;

	// 652
	double TradeServiceAccountBalance;

	float ActiveToolYValue;

	// 657
	SCString ExternalServiceUsername;

	// 659
	int NewBarAtSessionStart;

	// 673
	double (SCDLLCALL* InternalConvertDateTimeToChartTimeZone)(const SCDateTime& DateTime, const char* TimeZonePosixString);
	float LastTradePrice;

	// 676
	SCDateTime ScrollToDateTime;

	SCFloatArrayRef (SCDLLCALL* InternalDoubleStochastic)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, SCFloatArrayRef MovAvgIn, SCFloatArrayRef MovAvgOut, 	SCFloatArrayRef MovAvgIn2, SCFloatArrayRef Unused, int Index, int Length, int EmaLength, int MovAvgType);

	// 690
	int SupportTradingScaleIn;
	int SupportTradingScaleOut;

	// 691
	t_OrderQuantity32_64 TradeWindowOrderQuantity;

	// 692
	int IsAutoTradingEnabled;

	// 693
	int CurrentlySelectedDrawingTool;
	int CurrentlySelectedDrawingToolState;

	// 703
	int ServerConnectionState;  // ServerConnectionStateEnum

	// 711
	int MaintainAdditionalChartDataArrays;

	// 712
	SCString GraphShortName;

	// 716
	void (SCDLLCALL* GetStudyArraysFromChartUsingID)(int ChartNumber, int StudyID, SCGraphData& GraphData);
	int (SCDLLCALL* Internal_GetStudyIDByIndex)(int ChartNumber, int StudyNumber);

	// 725
	double (SCDLLCALL* DateStringToSCDateTime)(const SCString& DateString);

	// 745
	int BaseGraphScaleRangeType;

	// 749
	double (SCDLLCALL* InternalGetStandardError)(SCFloatArrayRef In, int Index, int Length);

	// 754
	SCString DocumentationImageURL;

	// 766
	int BaseGraphGraphDrawType;

	// 786
	int (SCDLLCALL* UserDrawnChartDrawingExists)(int ChartNumber, int LineNumber);

	// 787
	void (SCDLLCALL* SetCustomStudyControlBarButtonHoverText)(int ControlBarButtonNum, const char* HoverText);
	void (SCDLLCALL* SetCustomStudyControlBarButtonEnable)(int ControlBarButtonNum, int Enable);

	int  (SCDLLCALL* AddACSChartShortcutMenuItem)(int ChartNumber, const char* MenuText);
	int (SCDLLCALL* RemoveACSChartShortcutMenuItem)(int ChartNumber, int MenuID);

	int MenuEventID;
	int PointerEventType;

	// 792
	int (SCDLLCALL* Old_GetUserDrawingByLineNumber)(int ChartNumber, int LineNumber, s_UseTool& ChartDrawing, int DrawingType, int DrawingIndex);

	// 794
	SCString TradeAndCurrentQuoteSymbol;

	int GraphUsesChartColors;

	void (SCDLLCALL* GetBasicSymbolData)(const char* Symbol, s_SCBasicSymbolData& BasicSymbolData, int Subscribe);

	// 798
	void (SCDLLCALL* SetCustomStudyControlBarButtonText)(int ControlBarButtonNum, const char* ButtonText);

	// 799
	double (SCDLLCALL* AdjustDateTimeToGMT)(const SCDateTime& DateTime);

	// 805
	int (SCDLLCALL* DeleteUserDrawnACSDrawing)(int ChartNumber, int LineNumber);

	// 806
	int (SCDLLCALL* UploadChartImage)();

	SCString ChartbookName;

	//808
	void (SCDLLCALL* RefreshTradeData)();

	//810
	SCFloatArrayRef (SCDLLCALL* InternalCumulativeDeltaVolume)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation);
	SCFloatArrayRef (SCDLLCALL* InternalCumulativeDeltaTicks)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation);

	// 813
	void (SCDLLCALL* GetStudyDataColorArrayFromChartUsingID)(int ChartNumber, int StudyID, int SubgraphIndex , SCColorArrayRef DataColorArray);

	// 817
	SCFloatArrayRef (SCDLLCALL* Internal_AroonIndicator)(SCFloatArrayRef FloatArrayInHigh, SCFloatArrayRef FloatArrayInLow, SCFloatArrayRef OutUp, SCFloatArrayRef OutDown, SCFloatArrayRef OutOscillator, int Index, int Length);

	// 821
	void* CreateRelayServer_NoLongerUsed;
	int  (SCDLLCALL* RelayNewSymbol)(const SCString& Symbol, int ValueFormat, float TickSize, const SCString& ServiceCode);
	int  (SCDLLCALL* RelayTradeUpdate)(const SCString& Symbol, SCDateTime& DateTime, float TradeValue, unsigned int TradeVolume, int WriteRecord);
	void (SCDLLCALL* RelayDataFeedAvailable)();
	void (SCDLLCALL* RelayDataFeedUnavailable)();

	// 822
	int  MaintainTradeStatisticsAndTradesData;

	// 823
	SCFloatArrayRef (SCDLLCALL* InternalMovingMedian)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef Temp, int Index, int Length);

	// 826
	s_Old_PersistentVariables const* (SCDLLCALL* Old_GetStudyPersistentVariablesFromChartUsingID)(int ChartNumber, int StudyID);//Do not document.
	int (SCDLLCALL* ChartIsDownloadingHistoricalData)(int ChartNumber);
	int (SCDLLCALL* RelayServerConnected)();

	// 828
	void (SCDLLCALL* CreateProfitLossDisplayString)(double ProfitLoss, t_OrderQuantity32_64 Quantity, ProfitLossDisplayFormatEnum ProfitLossFormat, SCString& Result);

	// 829
	int FlagFullRecalculate;

	// 830 //Parameters reviewed for safety with different compilers
	double (SCDLLCALL* InternalGetStartOfPeriodForDateTime)(double DateTime, unsigned int TimePeriodType, int TimePeriodLength, int PeriodOffset, int NewPeriodAtBothSessionStarts );

	// 834
	c_VAPContainer *VolumeAtPriceForBars;

	// 841
	double (SCDLLCALL* TimePeriodSpan)(unsigned int TimePeriodType, int TimePeriodLength);

	// 860
	int (SCDLLCALL* OpenChartOrGetChartReference)(s_ACSOpenChartParameters& Parameters);

	// 868
	int ProtectStudy;

	// 874
	int RenkoTicksPerBar;

	// 875
	float RealTimePriceMultiplier;
	float HistoricalPriceMultiplier;

	// 878
	int (SCDLLCALL* GetTradeStatisticsForSymbol)(int Unused, int DailyStats, s_ACSTradeStatistics& TradeStats);

	// 879
	int ConnectToExternalServiceServer;
	int ReconnectToExternalServiceServer;
	int DisconnectFromExternalServiceServer;

	// 892
	int (SCDLLCALL* IsDateTimeInDaySession) ( const SCDateTime &DateTime);

	// 895
	int DeltaVolumePerBar;

	// 896
	int  AlertConditionFlags;

	// 896
	unsigned char (SCDLLCALL* GetTimeAndSalesForSymbol)(const SCString& Symbol, c_SCTimeAndSalesArray& TSArray);

	// 905
	unsigned int StudyVersion;

	// 909
	int AdvancedFeaturesLevel2;
	void (SCDLLCALL* SetNumericInformationGraphDrawTypeConfig)(const s_NumericInformationGraphDrawTypeConfig& NumericInformationGraphDrawTypeConfig);

	// 913
	double ChartTradingOrderPrice;
	int UseGUIAttachedOrderSetting;
	int BasedOnGraphValueFormat;

	int  RenkoTrendOpenOffsetInTicks;
	int  RenkoReversalOpenOffsetInTicks;

	// 915
	SCFloatArrayRef (SCDLLCALL* InternalCumulativeDeltaTickVolume)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Open, SCFloatArrayRef High, SCFloatArrayRef Low, SCFloatArrayRef Close, int Index, int ResetCumulativeCalculation);

	// 917
	std::int64_t DLLNameUserServiceLevel;

	// 918
	unsigned int (SCDLLCALL* CombinedForegroundBackgroundColorRef)(COLORREF ForegroundColor, COLORREF BackgroundColor);

	// 921
	SCFloatArrayRef (SCDLLCALL* InternalAccumulationDistribution)(SCBaseDataRef BaseDataIn, SCFloatArrayRef Out, int Index);

	// 922
	int SaveChartImageToFile;

	// 926
	int (SCDLLCALL* Old_ACSToolBlocksChartValuesTool)(int Block);

	// 930
	int (SCDLLCALL* SetStudyVisibilityState)(int StudyID, int Visible);

	c_VAPContainer *HistoricalHighPullbackVolumeAtPriceForBars;

	// 931
	int ReversalTicksPerBar;

	// 933
	int Unused_RenkoNewBarWhenExceeded;

	// 934
	c_VAPContainer *HistoricalLowPullbackVolumeAtPriceForBars;

	// 937
	int  ReceivePointerEvents;

	// 938
	c_VAPContainer *PullbackVolumeAtPrice;

	//939
	int (SCDLLCALL* GetNearestTargetOrder)(s_SCTradeOrder& OrderDetails);
	int (SCDLLCALL* GetNearestStopOrder)(s_SCTradeOrder& OrderDetails);

	// 943
	int (SCDLLCALL* FlattenPosition)();

	// 945
	int (SCDLLCALL* BarIndexToXPixelCoordinate)(int Index);

	// 952
	int (SCDLLCALL* RegionValueToYPixelCoordinate)(float RegionValue, int ChartRegionNumber);

	// 962
	int (SCDLLCALL* SetACSChartShortcutMenuItemDisplayed)(int ChartNumber, int MenuItemID, int DisplayItem);
	int (SCDLLCALL* SetACSChartShortcutMenuItemEnabled) (int ChartNumber, int MenuItemID, int Enabled);
	int (SCDLLCALL* SetACSChartShortcutMenuItemChecked)(int ChartNumber, int MenuItemID, int Checked);

	// 964
	fp_ACSGDIFunction p_GDIFunction;

	// 968
	unsigned int VolumeValueFormat;

	int (SCDLLCALL* GetStudyVisibilityState)(int StudyID);

	// 970
	t_OrderQuantity32_64 (SCDLLCALL* InternalSubmitOCOOrder)(s_SCNewOrder& NewOrder, int Index);

	float BaseGraphScaleConstRange;

	// 983
	int ACSVersion;  // This is set to the header version the custom study was compiled with

	int (SCDLLCALL* MakeHTTPBinaryRequest)(const SCString& URL);
	SCConstCharArray HTTPBinaryResponse;

	// 985
	int (SCDLLCALL* GetTradeListEntry)(unsigned int TradeIndex, s_ACSTrade& TradeEntry);

	// 1000
	void (SCDLLCALL* GetStudyExtraArrayFromChartUsingID)(int ChartNumber, int StudyID, int SubgraphIndex, int ExtraArrayIndex, SCFloatArrayRef ExtraArrayRef);

	// 1005
	int (SCDLLCALL* GetTradeListSize)();

	// 1014
	int Unused_OnExternalDataImmediateStudyCall;
	double (SCDLLCALL* YPixelCoordinateToGraphValue)(int YPixelCoordinate);

	// 1037
	int ContinuousFuturesContractLoading;

	// 1044
	bool PlaceACSChartShortcutMenuItemsAtTopOfMenu;

	int (SCDLLCALL* AddACSChartShortcutMenuSeparator)(int ChartNumber);

	// 1055
	int UsesMarketDepthData;

	float BaseGraphScaleRangeTop;
	float BaseGraphScaleRangeBottom;
	float BaseGraphScaleValueOffset;
	float BaseGraphAutoScalePaddingPercentage;

	// 1076
	int StudyRegionLeftCoordinate;
	int StudyRegionRightCoordinate;

	// 1086
	void (SCDLLCALL* SetNumericInformationDisplayOrderFromString)(const SCString& CommaSeparatedDisplayOrder);

	// 1091
	SCString CustomChartTitleBarName;

	// 1129
	SCFloatArrayRef (SCDLLCALL* InternalCCI)(SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier, unsigned int MovingAverageType);
	int (SCDLLCALL* GetOrderFillArraySize)();
	int (SCDLLCALL* GetOrderFillEntry)(unsigned int FillIndex, s_SCOrderFillData& FillData);

	// 1142
	int CancelAllOrdersOnReversals;

	// 1147
	SCFloatArrayRef (SCDLLCALL* Old_InternalResettableZigZag2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int StartIndex, int Index, int NumberOfBars, float ReversalAmount);
	SCFloatArrayRef (SCDLLCALL* Old_InternalResettableZigZag)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef  ZigZagValues,SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex , int StartIndex, int Index, float ReversalPercent, float ReversalAmount);

	// 1148
	SCString ((SCDLLCALL * DateTimeToString)(const double &DateTime,int Flags));

	// 1149
	SCDateTimeMS CurrentSystemDateTimeMS;

	// 1166
	float FilterChartVolumeGreaterThanEqualTo;
	float FilterChartVolumeLessThanEqualTo;

	// 1174
	SCString Unused_TitleBarName;

	//1177
	int PriceChangesPerBar;

	// 1190
	int ResetAllScales;

	// 1193
	SCString ((SCDLLCALL* FormatVolumeValue)(int64_t Volume, int VolumeValueFormat, bool UseLargeNumberSuffix));

	bool FilterChartVolumeTradeCompletely;

	// 1197
	int ChartRegion1TopCoordinate;
	int ChartRegion1BottomCoordinate;
	int ChartRegion1LeftCoordinate;
	int ChartRegion1RightCoordinate;

	// 1210
	int	BlockChartDrawingSelection;

	// 1216
	int DaysToLoadInChart;

	// 1218
	int EarliestUpdateSubgraphDataArrayIndex;

	// 1221
	int MonthsPerBar;

	int& ((SCDLLCALL* GetPersistentInt)(int Key));
	void ((SCDLLCALL* SetPersistentInt)(int Key, int Value));

	float& ((SCDLLCALL* GetPersistentFloat)(int Key));
	void ((SCDLLCALL* SetPersistentFloat)(int Key, float Value));

	double& ((SCDLLCALL* GetPersistentDouble)(int Key));
	void ((SCDLLCALL* SetPersistentDouble)(int Key, double Value));

	std::int64_t& ((SCDLLCALL* GetPersistentInt64)(int Key));
	void ((SCDLLCALL* SetPersistentInt64)(int Key, std::int64_t Value));

	SCDateTime& ((SCDLLCALL* GetPersistentSCDateTime)(int Key));
	void ((SCDLLCALL* Old_SetPersistentSCDateTime)(int Key, SCDateTime Value));

	void*& ((SCDLLCALL* GetPersistentPointer)(int Key));
	void ((SCDLLCALL* SetPersistentPointer)(int Key, void* Value));

	int& ((SCDLLCALL* GetPersistentIntFromChartStudy)(int ChartNumber, int StudyID, int Key));
	void ((SCDLLCALL* SetPersistentIntForChartStudy)(int ChartNumber, int StudyID, int Key, int Value));

	float& ((SCDLLCALL* GetPersistentFloatFromChartStudy)(int ChartNumber, int StudyID, int Key));
	void ((SCDLLCALL* SetPersistentFloatForChartStudy)(int ChartNumber, int StudyID, int Key, float Value));

	double& ((SCDLLCALL* GetPersistentDoubleFromChartStudy)(int ChartNumber, int StudyID, int Key));
	void ((SCDLLCALL* SetPersistentDoubleForChartStudy)(int ChartNumber, int StudyID, int Key, double Value));

	std::int64_t& ((SCDLLCALL* GetPersistentInt64FromChartStudy)(int ChartNumber, int StudyID, int Key));
	void ((SCDLLCALL* SetPersistentInt64ForChartStudy)(int ChartNumber, int StudyID, int Key, std::int64_t Value));

	SCDateTime& ((SCDLLCALL* GetPersistentSCDateTimeFromChartStudy)(int ChartNumber, int StudyID, int Key));
	void ((SCDLLCALL* Old_SetPersistentSCDateTimeForChartStudy)(int ChartNumber, int StudyID, int Key, SCDateTime Value));

	void*& ((SCDLLCALL* GetPersistentPointerFromChartStudy)(int ChartNumber, int StudyID, int Key));
	void ((SCDLLCALL* SetPersistentPointerForChartStudy)(int ChartNumber, int StudyID, int Key, void* Value));

	// 1236
	int ((SCDLLCALL* GetLatestBarCountdownAsInteger)());

	// 1242
	int ContinuousFuturesContractOption;

	// 1246
	double ((SCDLLCALL* Unused_GetEndingDateTimeForBarDateTime)(const double BarDateTime, const double NextBarDateTime));

	// 1249
	int (SCDLLCALL* IsChartDataLoadingInChartbook)();
	SCString ((SCDLLCALL* DataTradeServiceName)());

	// 1261
	void (SCDLLCALL* CloseChart)(int ChartNumber);
	void (SCDLLCALL* CloseChartbook)(const SCString& ChartbookFileName);

	// 1266
	unsigned int DataFeedActivityCounter;

	// 1267
	SCString ((SCDLLCALL* GetChartTextFontFaceName)());

	SCString SelectedTradeAccount;
	float RoundTurnCommission;

	// 1268
	SCString& ((SCDLLCALL* GetPersistentSCString)(int Key));
	void ((SCDLLCALL* SetPersistentSCString)(int Key, SCString Value));
	SCString& ((SCDLLCALL* GetPersistentSCStringFromChartStudy)(int ChartNumber, int StudyID, int Key));
	void ((SCDLLCALL* SetPersistentSCStringForChartStudy)(int ChartNumber, int StudyID, int Key, SCString Value));

	void ((SCDLLCALL* AddAlertLineWithDateTime)(const char* Message, int ShowAlertLog, double AlertDateTime));

	//1275
	int IsFullRecalculation;

	//1279
	int IntradayChartRecordingState;

	//1282
	int IsChartTradeModeOn;

	//1283
	SCDateTime DailyStatsResetTime;

	//1299
	double TradeServiceAvailableFundsForNewPositions;

	//1300
	void (SCDLLCALL* GetAttachedOrderIDsForParentOrder)(int ParentInternalOrderID, int& TargetInternalOrderID, int& StopInternalOrderID);

	// 1311
	int FlagToReloadChartData;

	// 1319
	int (SCDLLCALL* GetTradePositionForSymbolAndAccount)(s_SCPositionData& r_PositionData, const SCString& Symbol, const SCString& TradeAccount);

	int ReceiveNotificationsForChangesToOrdersPositionsForAnySymbol;

	int (SCDLLCALL* InternalSubmitOrder)(s_SCNewOrder& r_NewOrder, int BarIndex, BuySellEnum BuySell);

	// 1328
	int LoadChartDataByDateRange;
	int ChartDataStartDate;
	int ChartDataEndDate;

	// 1348
	int (SCDLLCALL* GetStudyDataStartIndexFromChartUsingID)(int ChartNumber, unsigned int StudyID);

	// 1352
	void (SCDLLCALL* InternalVortex)(SCBaseDataRef BaseDataIn, SCFloatArrayRef TrueRangeOut, SCFloatArrayRef VortexMovementUpOut, SCFloatArrayRef VortexMovementDownOut, SCFloatArrayRef VMPlusOut, SCFloatArrayRef VMMinusOut, int Index, int VortexLength);

	// 1353
	int (SCDLLCALL* GetFirstNearestIndexForTradingDayDate)(int ChartNumber, int TradingDayDate);

	// 1354
	int (SCDLLCALL* Internal_GetOpenHighLowCloseVolumeForDate)(double Date, float& r_Open, float& r_High, float& r_Low, float& r_Close, float& r_Volume);

	// 1357
	const char * (SCDLLCALL* GetStudySubgraphName)(int StudyID, int SubgraphIndex);

	// 1360
	double (SCDLLCALL* TimeStringToSCDateTime)(const SCString& TimeString);

	// 1371
	int PointAndFigureBoxSizeInTicks;
	int PointAndFigureReversalSizeNumBoxes;

	// 1372
	int (SCDLLCALL* ChangeACSChartShortcutMenuItemText)(int ChartNumber, int MenuIdentifier, const char* NewMenuText);

	int ChartWindowIsActive;

	// 1376
	double (SCDLLCALL* DateStringDDMMYYYYToSCDateTime)(const SCString& DateString);

	// 1381
	float PointAndFigureXOGraphDrawTypeBoxSize;

	// 1383
	int (SCDLLCALL* SaveChartbook)();

	// 1388
	c_VAPContainer * VolumeAtPriceForStudy;

	// 1389
	SCDateTime CurrentDateTimeForReplay;

	//1394

	void (SCDLLCALL* InternalHeikinAshi)(SCBaseDataRef BaseDataIn,  int Index, int Length,  SCFloatArrayRef OpenOut, SCFloatArrayRef HighOut,  SCFloatArrayRef LowOut, SCFloatArrayRef LastOut , int SetCloseToCurrentPriceAtLastBar);

	//1409
	double (SCDLLCALL* CalculateTimeSpanAcrossChartBars)(int FirstIndex, int LastIndex);

	//1416
	int (SCDLLCALL* GetHighestChartNumberUsedInChartBook)();

	//1417
	int (SCDLLCALL * GetCalculationStartIndexForStudy)();

	double (SCDLLCALL* GetEndingDateTimeForBarIndex)(int BarIndex);

	// 1423
	HWND (SCDLLCALL* GetChartWindowHandle)(int ChartNumber);

	// 1435
	c_ACSILDepthBars* (SCDLLCALL* GetMarketDepthBars)();
	c_ACSILDepthBars* (SCDLLCALL* GetMarketDepthBarsFromChart)(int ChartNumber);
	void (SCDLLCALL* SaveChartImageToFileExtended)(int ChartNumber, SCString &OutputPathAndFileName, int Width, int Height, int IncludeOverlays);

	// 1444
	int DrawACSDrawingsAboveOtherDrawings;

	//1447
	SCString DataFilesFolder;

	// 1454
	double (SCDLLCALL* GetLastPriceForTrading)();

	//1455
	int (SCDLLCALL* Old_WriteBarAndStudyDataToFile)(int StartingIndex, SCString &OutputPathAndFileName);

	//1461
	SCString (SCDLLCALL* GetChartSymbol)(int ChartNumber);

	//1464
	void (SCDLLCALL* Internal_CalculateRegressionStatistics)(SCFloatArrayRef In, double &Slope, double &Y_Intercept, int Index, int Length);

	//1471
	double (SCDLLCALL* GetEndingDateTimeForBarIndexFromChart)(int ChartNumber, int BarIndex);
	int (SCDLLCALL* Old2_WriteBarAndStudyDataToFile)(int StartingIndex, SCString &OutputPathAndFileName, int IncludeHiddenStudies);

	// 1474
	int (SCDLLCALL* AddLineUntilFutureIntersection)
		( int StartBarIndex
		, int LineIDForBar
		, float LineValue
		, COLORREF LineColor
		, unsigned short LineWidth
		, unsigned short LineStyle
		, int DrawValueLabel
		, int DrawNameLabel
		, const SCString& NameLabel
		);

	// 1482
	int (SCDLLCALL* GetBidMarketDepthStackPullValueAtPrice)(float Price);
	int (SCDLLCALL* GetAskMarketDepthStackPullValueAtPrice)(float Price);

	// 1489
	int (SCDLLCALL* SetChartWindowState)(int ChartNumber, int WindowState);

	// 1494
	double (SCDLLCALL* InternalConvertDateTimeFromChartTimeZone)(const double& DateTime, const char* TimeZonePosixString);

	// 1503
	int (SCDLLCALL* DeleteLineUntilFutureIntersection)(int StartBarIndex, int LineIDForBar);

	// 1509
	int (SCDLLCALL* GetParentOrderIDFromAttachedOrderID)(int AttachedOrderInternalOrderID);

	// 1510
	int (SCDLLCALL* StartDownloadHistoricalData)(double StartingDateTime);

	// 1519
	void (SCDLLCALL* ClearAllPersistentData)();

	// 1523
	int MaintainHistoricalMarketDepthData;

	void (SCDLLCALL* Internal_CalculateLogLogRegressionStatistics)(SCFloatArrayRef In, double &Slope, double &Y_Intercept, int Index, int Length);

	// 1526

	void (SCDLLCALL* Internal_InverseFisherTransform)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef CalcArray1, SCFloatArrayRef CalcArray2, int Index, int HighestLowestLength, int MovingAverageLength, int MovAvgType);

	void  (SCDLLCALL* Internal_InverseFisherTransformRSI)(SCFloatArrayRef In, SCFloatArrayRef Out, SCFloatArrayRef RSIArray1, SCFloatArrayRef RSIArray2, SCFloatArrayRef RSIArray3, SCFloatArrayRef RSIArray4, SCFloatArrayRef RSIArray5, SCFloatArrayRef CalcArray1, SCFloatArrayRef CalcArray2, int Index, int RSILength, int  InternalRSIMovAvgType, int RSIMovingAverageLength, int  MovingAverageOfRSIType);

	void  (SCDLLCALL* Internal_MovingAverageCumulative)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index);

	// 1528
	SCFloatArrayRef(SCDLLCALL* Internal_HurstExponent)(SCFloatArrayRef In, SCFloatArrayRef Out, int Index, int LengthIndex);

	SCString ((SCDLLCALL* FormatDateTimeMS)(const SCDateTimeMS& DateTimeMS));
	SCString ((SCDLLCALL* DateToString)(const SCDateTime& DateTime));
	SCString ((SCDLLCALL* TimeToString)(const SCDateTime& DateTime));
	SCString ((SCDLLCALL* TimeMSToString)(const SCDateTimeMS& DateTimeMS));

	SCString SCDataFeedSymbol;

	// 1530
	unsigned int (SCDLLCALL* GetRecentBidVolumeAtPrice)(float Price);
	unsigned int (SCDLLCALL* GetRecentAskVolumeAtPrice)(float Price);

	// 1546
	SCFloatArrayRef(SCDLLCALL* InternalResettableZigZag)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef  ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int StartIndex, int Index, float ReversalPercent, float ReversalAmount, SCStudyInterfaceRef sc);
	SCFloatArrayRef(SCDLLCALL* InternalResettableZigZag2)(SCFloatArrayRef InputDataHigh, SCFloatArrayRef InputDataLow, SCFloatArrayRef ZigZagValues, SCFloatArrayRef ZigZagPeakType, SCFloatArrayRef ZigZagPeakIndex, int StartIndex, int Index, int NumberOfBars, float ReversalAmount, SCStudyInterfaceRef sc);

	// 1563
	int BaseGraphConstantRangeScaleMode;

	// 1568
	int (SCDLLCALL* GetStudyPeakValleyLine)(int ChartNumber, int StudyID, float& PeakValleyLinePrice, int& PeakValleyType, int& StartIndex, int& PeakValleyExtensionChartColumnEndIndex, int ProfileIndex, int PeakValleyIndex);

	int (SCDLLCALL* GetStudyLineUntilFutureIntersection)(int ChartNumber, int StudyID, int BarIndex, int LineIndex, int& LineIDForBar, float &LineValue, int  &ExtensionLineChartColumnEndIndex);

	// 1573
	SCDateTimeArray BaseDataEndDateTime;  // From the base graph

	// 1575
	int (SCDLLCALL* Internal_GetOpenHighLowCloseVolumeForDate2)(double Date, float& r_Open, float& r_High, float& r_Low, float& r_Close, float& r_Volume, int IncludeFridayEveningSessionWithSundayEveningSession);

	// 1577
	int (SCDLLCALL* GetUserDrawnDrawingByLineNumber)(int ChartNumber, int LineNumber, s_UseTool& ChartDrawing);

	int (SCDLLCALL* GetACSDrawingByLineNumber)(int ChartNumber, int LineNumber, s_UseTool& ChartDrawing);

	int (SCDLLCALL* GetACSDrawingByIndex)(int ChartNumber, int Index, s_UseTool& ChartDrawing, int ExcludeOtherStudyInstances);

	int(SCDLLCALL* GetACSDrawingsCount)(int ChartNumber, int ExcludeOtherStudyInstances);

	// 1580
	int (SCDLLCALL* GetNearestMatchForSCDateTimeExtended)(int ChartNumber, const SCDateTimeMS& DateTime);

	// 1587

	int ReceiveKeyboardKeyEvents;
	int ReceiveCharacterEvents;
	int	KeyboardKeyEventCode;
	int CharacterEventCode;

	//1591
	//From the perspective of the study developer, this is best to be set to true when they do not want the chart redrawn after the study returns, rather than having it true by default and setting it to 0
	short DoNotRedrawChartAfterStudyReturns;

	//1593
	uint32_t RightValuesScaleLeftCoordinate;
	uint32_t RightValuesScaleRightCoordinate;

	//1595
	int LastFullCalculationTimeInMicroseconds;


	//1598
	uint16_t MaintainReferenceToOtherChartsForPersistentVariables;

	//1599
	int HTTPRequestID;

	void (SCDLLCALL* GetBasicSymbolDataWithDepthSupport)(const char* Symbol, s_SCBasicSymbolData& BasicSymbolData, int SubscribeToData, int SubsribeToMarketDepth);

	//1607
	void (SCDLLCALL* CalculateTimeSpanAcrossChartBarsInChart)(int ChartNumber, int FirstIndex, int LastIndex, SCDateTime& TimeSpan);

	// 1608
	double (SCDLLCALL* StringToDouble)(const char* NumberString);

	SCString ServiceCodeForSelectedDataTradingService;

	void (SCDLLCALL* GetProfitManagementStringForTradeAccount)(SCString& r_TextString);

	// 1612
	uint16_t SupportKeyboardModifierStates;
	uint16_t IsKeyPressed_Control;
	uint16_t IsKeyPressed_Shift;
	uint16_t IsKeyPressed_Alt;

	// 1614
	SCFloatArrayRef (SCDLLCALL* Internal_T3MovingAverage)
		( SCFloatArrayRef InputArray
		, SCFloatArrayRef OutputArray
		, SCFloatArrayRef CalcArray0
		, SCFloatArrayRef CalcArray1
		, SCFloatArrayRef CalcArray2
		, SCFloatArrayRef CalcArray3
		, SCFloatArrayRef CalcArray4
		, SCFloatArrayRef CalcArray5
		, float Multiplier
		, int Index
		, int Length
		);

	uint16_t TradingIsLocked;

	uint32_t (SCDLLCALL* GetNumberOfDataFeedSymbolsTracked)();

	// 1619
	int (SCDLLCALL* GetDOMColumnLeftCoordinate)(n_ACSIL::DOMColumnTypeEnum DOMColumn);
	int (SCDLLCALL* GetDOMColumnRightCoordinate)(n_ACSIL::DOMColumnTypeEnum DOMColumn);

	// 1620
	uint32_t (SCDLLCALL* GetCurrentTradedBidVolumeAtPrice)(float Price);
	uint32_t (SCDLLCALL* GetCurrentTradedAskVolumeAtPrice)(float Price);

	//1622
	double  (SCDLLCALL* ConvertCurrencyValueToCommonCurrency)(double CurrencyValue, const SCString& SourceSymbol, SCString &OutputCurrency);

	//1627
	fp_ACSCustomChartBarFunction fp_ACSCustomChartBarFunction;

	//1629
	int UsesCustomChartBarFunction;

	int (SCDLLCALL* ReadIntradayFileRecordAtIndex)(int64_t Index, s_IntradayRecord& r_Record, IntradayFileLockActionEnum IntradayFileLockAction);

	//1631
	int (SCDLLCALL* ReadIntradayFileRecordForBarIndexAndSubIndex)(int BarIndex, int SubRecordIndex, s_IntradayRecord& r_Record, IntradayFileLockActionEnum IntradayFileLockAction);

	//1635
	SCFloatArrayRef(SCDLLCALL* Internal_CalculateAngle)(SCFloatArrayRef InputArray, SCFloatArrayRef OutputArray, int Index, int Length, float ValuePerPoint);

	//1640
	int NumberOfForwardColumns;

	//1642
	int AllocateAndNameRenkoChartBarArrays;

	// 1645
	void (SCDLLCALL* OpenChartbook)(const SCString& ChartbookFileName);

	// 1646
	int (SCDLLCALL* IsDateTimeInEveningSession) (const SCDateTime &DateTime);

	//1649
	uint16_t TransparencyLevel;

	//1651
	int (SCDLLCALL* IsChartNumberExist) (int ChartNumber, const SCString& ChartbookFileName);

	//1665
	void* (SCDLLCALL* GetStudyStorageBlockFromChart) (int ChartNumber, int StudyID);
	int (SCDLLCALL* WriteBarAndStudyDataToFile)(int StartingIndex, SCString &OutputPathAndFileName, int IncludeHiddenStudies, int IncludeHiddenSubgraphs);

	//1671
	uint32_t Unused_1;

	//1673
	int32_t ConstantRangeScaleModeTicksFromCenterOrEdge;

	// 1675
	void (SCDLLCALL* GetBarPeriodParameters)(n_ACSIL::s_BarPeriod& r_BarPeriod);

	// 1676
	uint16_t IsChartbookBeingSaved;

	// 1678
	double (SCDLLCALL* Internal_GetSymbolDataValue)(SymbolDataValuesEnum Value, const char* Symbol, int SubscribeToMarketData, int SubscribeToMarketDepth);

	// 1679
	void (SCDLLCALL* SetBarPeriodParameters)(const n_ACSIL::s_BarPeriod& BarPeriod);

	// 1684
	uint16_t IncludeInStudySummary;

	uint16_t PointerVertWindowCoord;
	uint16_t PointerHorzWindowCoord;

	// 1693
	void (SCDLLCALL* GetPointOfControlPriceVolumeForBar)(int BarIndex, s_VolumeAtPriceV2& VolumeAtPrice);

	// 1695
	int (SCDLLCALL* EvaluateAlertConditionFormulaAsBoolean)(int BarIndex, int ParseAndSetFormula);

	// 1698
	void (SCDLLCALL* Internal_AddMessageToTradeServiceLog)(const char* Message, int ShowLog, int AddChartStudyDetails);

	// 1701
	void (SCDLLCALL* SetCustomStudyControlBarButtonColor)(int ControlBarButtonNum, const COLORREF Color);

	// 1705
	int (SCDLLCALL* GetTradePositionByIndex)(s_SCPositionData& r_PositionData, int Index);

	// 1706
	int CancelAllOrdersOnEntries;

	// 1709
	int (SCDLLCALL* SetHorizontalGridState)(int GridIndex, int State);
	int (SCDLLCALL* SetVerticalGridState)(int State);

	// 1717
	int (SCDLLCALL* GetOrderForSymbolAndAccountByIndex)(const char* Symbol, const char* TradeAccount, int OrderIndex, s_SCTradeOrder& ACSOrderDetails);

	// 1718
	int (SCDLLCALL* GetBidMarketDepthNumberOfLevels)();
	int (SCDLLCALL* GetAskMarketDepthNumberOfLevels)();
	int (SCDLLCALL* GetMaximumMarketDepthLevels)();
	int (SCDLLCALL* GetBidMarketDepthEntryAtLevel)(s_MarketDepthEntry& r_DepthEntry, int LevelIndex);
	int (SCDLLCALL* GetAskMarketDepthEntryAtLevel)(s_MarketDepthEntry& r_DepthEntry, int LevelIndex);
	int IsAutoTradingOptionEnabledForChart;

	//1726
	int (SCDLLCALL* GetChartFontProperties)(SCString& r_FontName, int32_t& r_FontSize, int32_t&  r_FontBold, int32_t& r_FontUnderline, int32_t& r_FontItalic);

	// 1750
	uint16_t IncludeInSpreadsheet;

	// 1756
	int (SCDLLCALL* GetVolumeAtPriceDataForStudyProfile)
		( const int StudyID
		, const int ProfileIndex
		, const int PriceIndex//zero-based
		, s_VolumeAtPriceV2& r_VolumeAtPrice
		);
	int (SCDLLCALL* GetNumPriceLevelsForStudyProfile)(int StudyID, int ProfileIndex);

	//1764
	void (SCDLLCALL* GetTradingDayStartDateTimeOfBarForChart)(const SCDateTime& BarDateTime, SCDateTime& r_TradingDayStartDateTime, int ChartNumber);

	//1768
	int (SCDLLCALL* GetChartStudyInputInt)(int ChartNumber, int StudyID, int InputIndex, int& r_IntegerValue);
	int (SCDLLCALL* SetChartStudyInputInt)(int ChartNumber, int StudyID, int InputIndex, int IntegerValue);
	int (SCDLLCALL* GetChartStudyInputFloat)(int ChartNumber, int StudyID, int InputIndex, double& r_FloatValue);
	int (SCDLLCALL* SetChartStudyInputFloat)(int ChartNumber, int StudyID, int InputIndex, double FloatValue);

	int (SCDLLCALL* RecalculateChart)(int ChartNumber);

	int (SCDLLCALL* Internal_GetStudyIDByName)(int ChartNumber, const char* Name, const int UseShortNameIfSet);

	// 1780
	n_ACSIL::DTCSecurityTypeEnum SecurityType;

	// 1783
	void (SCDLLCALL* GetGraphVisibleHighAndLow)(double& High, double& Low);

	// 1788
	uint32_t VolumeAtPriceMultiplier;

	// 1804
	bool (SCDLLCALL* OpenFile)(const SCString& PathAndFileName, const int Mode, int& FileHandle);
	bool (SCDLLCALL* ReadFile)(const int FileHandle, char* Buffer, const int BytesToRead, unsigned int* p_BytesRead);
	bool (SCDLLCALL* WriteFile)(const int FileHandle, const char* Buffer, const int BytesToWrite, unsigned int* p_BytesWritten);
	bool (SCDLLCALL* CloseFile)(const int FileHandle);
	int (SCDLLCALL* GetLastFileErrorCode)(const int FileHandle);
	SCString (SCDLLCALL* GetLastFileErrorMessage)(const int FileHandle);

	// 1821
	int AlertConditionEnabled;
	int (SCDLLCALL* GetSessionTimesFromChart)(const int ChartNumber, n_ACSIL::s_ChartSessionTimes& r_ChartSessionTimes);

	// 1824
	void* (SCDLLCALL* GetSpreadsheetSheetHandleByName)(const char* SheetCollectionName, const char* SheetName, const int CreateSheetIfNotExist);
	int (SCDLLCALL* GetSheetCellAsDouble)(void* SheetHandle, const int Column, const int Row, double& r_CellValue);
	int (SCDLLCALL* SetSheetCellAsDouble)(void* SheetHandle, const int Column, const int Row, const double CellValue);
	int (SCDLLCALL* GetSheetCellAsString)(void* SheetHandle, const int Column, const int Row, SCString& r_CellString);
	int (SCDLLCALL* SetSheetCellAsString)(void* SheetHandle, const int Column, const int Row, const SCString& CellString);

	//1834
	SCDateTime ContractRolloverDate;

	//1847
	double (SCDLLCALL* GetTotalNetProfitLossForAllSymbols)(int DailyValues);

	// 1848
	void (SCDLLCALL* SetAttachedOrders)(const s_SCNewOrder& AttachedOrdersConfiguration);

	//1850
	int (SCDLLCALL* GetLineNumberOfSelectedUserDrawnDrawing)();

	//1852
	int (SCDLLCALL* MakeHTTPPOSTRequest)(const SCString& URL, const SCString& POSTData, const n_ACSIL::s_HTTPHeader* Headers, int NumberOfHeaders);

	//1864
	void (SCDLLCALL* ClearAlertSoundQueue)();

	//1866
	void((SCDLLCALL* SetPersistentSCDateTime)(int Key, const SCDateTime& Value));
	void((SCDLLCALL* SetPersistentSCDateTimeForChartStudy)(int ChartNumber, int StudyID, int Key, const SCDateTime& Value));
	int ChartUpdateIntervalInMilliseconds;

	//1869
	int (SCDLLCALL* GetTradeWindowOrderType)();
	
	// 1876
	int (SCDLLCALL* EvaluateGivenAlertConditionFormulaAsBoolean)(int BarIndex, int ParseAndSetFormula, const SCString& Formula);

	//1878
	double (SCDLLCALL* EvaluateGivenAlertConditionFormulaAsDouble)(int BarIndex, int ParseAndSetFormula, const SCString& Formula);

	//1881
	int (SCDLLCALL* WriteBarAndStudyDataToFileEx)(const n_ACSIL::s_WriteBarAndStudyDataToFile& WriteBarAndStudyDataToFileParams);

	// 1892
	int (SCDLLCALL* RecalculateChartImmediate)(int ChartNumber);

	// 1895
	int (SCDLLCALL* GetCustomStudyControlBarButtonEnableState)(int ControlBarButtonNum);

	// 1898
	uint32_t (SCDLLCALL* ClearCurrentTradedAskVolume)();

	// 1903
	int (SCDLLCALL* AddLineUntilFutureIntersectionEx)(const n_ACSIL::s_LineUntilFutureIntersection& LineUntilFutureIntersection);


	// When adding new functions that use arrays as parameters, always use
	// SCFloatArrayRef and not SCSubgraphRef.  All required internal arrays
	// must be passed in as parameters.  Write a wrapper function that takes
	// a SCSubgraphRef and pass into the Internal function the necessary
	// internal arrays.  This ensures back compatibility if there are changes
	// to the Subgraph structure and it allows our functions to also be used
	// internally in Sierra Chart where Subgraph internal arrays are not
	// automatically supported.

	// When adding pointers to functions that return a SCString object,
	// include an extra set of parentheses around the function part.
	// Otherwise g++ will give errors.
	// Example: SCString ((SCDLLCALL* DataTradeServiceName)());

	// When using structures/classes in functions as parameters, 
	// they must always be passed by reference and not as a copy.
	// Since other compilers can generate a different structure for them.


	// Always update SC_DLL_VERSION when updating the interface
};

/*************************************************************/

#pragma pack(pop)

#define SCStudyGraphRef SCStudyInterfaceRef

#define GetUserDrawingByLineNumber GetUserDrawnDrawingByLineNumber
#define IsWorkingOrderStatusNoChild IsWorkingOrderStatusIgnorePendingChildren

#endif
