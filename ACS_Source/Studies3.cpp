#include "sierrachart.h"

#include "scstudyfunctions.h"

/***********************************************************************/
SCSFExport scsf_ChaikinOsc(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Oscillator = sc.Subgraph[0];
	SCSubgraphRef Overbought = sc.Subgraph[2];
	SCSubgraphRef Oversold = sc.Subgraph[3];


	SCInputRef LongMALength = sc.Input[2];
	SCInputRef ShortMALength = sc.Input[3];
	SCInputRef Divisor = sc.Input[4];
	SCInputRef OverboughtValue = sc.Input[5];
	SCInputRef OversoldValue = sc.Input[6];
	
	if(sc.SetDefaults)
	{
		sc.GraphName = "Chaikin Oscillator";
		
		Oscillator.Name = "Oscillator";
		Oscillator.DrawStyle = DRAWSTYLE_LINE;
		Oscillator.PrimaryColor = RGB(0,255,0);

		Overbought.Name = "Overbought";
		Overbought.DrawStyle = DRAWSTYLE_LINE;
		Overbought.PrimaryColor = RGB(255,255,0);
		Overbought.DrawZeros = false;

		Oversold.Name = "Oversold";
		Oversold.DrawStyle = DRAWSTYLE_LINE;
		Oversold.PrimaryColor = RGB(255,127,0);
		Oversold.DrawZeros = false;

		LongMALength.Name = "Long MovAvg Length";
		LongMALength.SetInt(10);
		LongMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		ShortMALength.Name = "Short MovAvg Length";
		ShortMALength.SetInt(3);
		ShortMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Divisor.Name = "Divisor";
		Divisor.SetFloat(1);
		Divisor.SetFloatLimits(.00001f, (float)MAX_STUDY_LENGTH);

		OverboughtValue.Name = "Overbought";
		OverboughtValue.SetInt(0);

		OversoldValue.Name = "Oversold";
		OversoldValue.SetInt(0);

		

		sc.AutoLoop = 1;

		return;
	}
	
	sc.DataStartIndex = max(LongMALength.GetInt(),ShortMALength.GetInt()) - 1;


	sc.AccumulationDistribution(sc.BaseDataIn, Oscillator.Arrays[0], sc.Index);
	sc.ExponentialMovAvg(Oscillator.Arrays[0], Oscillator.Arrays[1], sc.Index, LongMALength.GetInt());
	sc.ExponentialMovAvg(Oscillator.Arrays[0], Oscillator.Arrays[2], sc.Index, ShortMALength.GetInt());

	Oscillator[sc.Index] = (Oscillator.Arrays[2][sc.Index] - Oscillator.Arrays[1][sc.Index]) / Divisor.GetInt();
	Overbought[sc.Index] = (float)OverboughtValue.GetInt();
	Oversold[sc.Index] = (float)OversoldValue.GetInt();
}

/*==========================================================================*/
SCSFExport scsf_HistoricalVolatilityRatio(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HVR = sc.Subgraph[0];
	SCFloatArrayRef TempLog = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef TempShort = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef TempLong = sc.Subgraph[0].Arrays[2];

	SCInputRef ShortLength = sc.Input[2];
	SCInputRef LongLength = sc.Input[3];

	SCFloatArrayRef Close = sc.Close;

	// Configuration
	if (sc.SetDefaults)
	{
		sc.GraphName = "Historical Volatility Ratio";
		
		HVR.Name = "HVR";
		HVR.DrawStyle = DRAWSTYLE_LINE;
		HVR.PrimaryColor = RGB(0,255,0);
		
		ShortLength.Name = "Short Length";
		ShortLength.SetInt(6);
		ShortLength.SetIntLimits(1, INT_MAX);
		
		LongLength.Name = "Long Length";
		LongLength.SetInt(100);
		LongLength.SetIntLimits(1, INT_MAX);

		
		

		sc.AutoLoop = 1;
		
		return;
	}
	

	// Data Processing
		
	sc.DataStartIndex = max(LongLength.GetInt(), ShortLength.GetInt());

	if (sc.Index == 0)
		return;
	
	if (Close[sc.Index - 1] != 0.0)
		TempLog[sc.Index] = logf(Close[sc.Index] / Close[sc.Index - 1]);
	else
		TempLog[sc.Index] = 0;

	sc.StdDeviation(TempLog, TempShort, sc.Index, ShortLength.GetInt());
	sc.StdDeviation(TempLog, TempLong, sc.Index, LongLength.GetInt());

	if (TempLog[sc.Index] != 0)
		HVR[sc.Index] = TempShort[sc.Index] / TempLong[sc.Index];
	else
		HVR[sc.Index] = HVR[sc.Index-1];
}

/*==========================================================================*/
SCSFExport scsf_CurrentPriceLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_CurrentPriceLine = sc.Subgraph[0];

	SCInputRef NumberOfBarsBack = sc.Input[0];
	SCInputRef InputData = sc.Input[1];
	SCInputRef Version = sc.Input[2];
	SCInputRef DisplayCPLOnBars = sc.Input[3];
	SCInputRef ProjectForward = sc.Input[4];
	SCInputRef UseLastFromCurrentQuoteData = sc.Input [5];
	SCInputRef NumberOfForwardProjectionBars = sc.Input[6];
	
	if(sc.SetDefaults)
	{
		sc.GraphName = "Current Price Line";

		Subgraph_CurrentPriceLine.Name = "CPL";
		Subgraph_CurrentPriceLine.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_CurrentPriceLine.PrimaryColor = RGB(0,255,0);
		Subgraph_CurrentPriceLine.DrawZeros = false;
		Subgraph_CurrentPriceLine.LineStyle = LINESTYLE_DOT;
		
		sc.ScaleRangeType =  SCALE_SAMEASREGION;
		sc.GraphRegion = 0;

		NumberOfBarsBack.Name = "Number of Bars to Display On. 0 = Current Day";
		NumberOfBarsBack.SetInt(0);
		NumberOfBarsBack.SetIntLimits(0,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		
		DisplayCPLOnBars.Name = "Display Current Price Line on Chart Bars";
		DisplayCPLOnBars.SetYesNo(true);

		ProjectForward.Name = "Forward Project Current Price Line";
		ProjectForward.SetYesNo(false);

		UseLastFromCurrentQuoteData.Name = "Use Last Trade Price From Current Quote Data";
		UseLastFromCurrentQuoteData.SetYesNo(false);

		NumberOfForwardProjectionBars.Name = "Number of Forward Projection Bars";
		NumberOfForwardProjectionBars.SetInt(10);
		
		Version.SetInt(2);

		sc.AutoLoop = 0;

		
		return;
	}

	if (NumberOfForwardProjectionBars.GetInt() == 0)
		NumberOfForwardProjectionBars.SetInt(10);
	
	SCFloatArrayRef InputDataArray = sc.BaseData[InputData.GetInputDataIndex()];

	float LineValue = 0;

	if(UseLastFromCurrentQuoteData.GetYesNo())
		LineValue = sc.LastTradePrice;
	else
		LineValue = InputDataArray[sc.ArraySize - 1];

	int &PriorLineStartIndex = sc.GetPersistentInt(1);
	
	if(DisplayCPLOnBars.GetYesNo())
	{
		
		int BarsBack = NumberOfBarsBack.GetInt();
		
		if (BarsBack == 0)// Current Day
		{
			SCDateTime TradingDayStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.ArraySize - 1]);

			int BarIndex=0;
			for (BarIndex = sc.ArraySize - 1; 
				sc.BaseDateTimeIn[BarIndex] >= TradingDayStartDateTime && BarIndex >= 0; BarIndex--)
			{
				Subgraph_CurrentPriceLine[BarIndex] = LineValue;

				sc.EarliestUpdateSubgraphDataArrayIndex = BarIndex;
			}

		}
		else
		{
			int StartingIndex = sc.ArraySize - BarsBack;

			sc.EarliestUpdateSubgraphDataArrayIndex = sc.UpdateStartIndex;

			for (int Index = sc.UpdateStartIndex - BarsBack; Index < sc.ArraySize; ++Index)
			{
				if(Index >= StartingIndex)
					break;

				Subgraph_CurrentPriceLine[Index] = 0;
			}

			for (int Index = sc.ArraySize - 1; BarsBack > 0 && Index >=0; Index--)
			{
				Subgraph_CurrentPriceLine[Index] = LineValue;
				BarsBack--;

				if (Index < sc.EarliestUpdateSubgraphDataArrayIndex)
					sc.EarliestUpdateSubgraphDataArrayIndex = Index;
			}
		}

	}
	else if (PriorLineStartIndex != 0)
	{
		//Clear prior array elements for when projecting forward
		for(int Index = PriorLineStartIndex; Index < sc.ArraySize; Index ++)
			Subgraph_CurrentPriceLine[Index] = 0.0;
	}

	// Zero out prior values
	int StartClearIndex = 0;
	if (DisplayCPLOnBars.GetYesNo())
		StartClearIndex = sc.EarliestUpdateSubgraphDataArrayIndex - 1;
	else
		StartClearIndex = sc.ArraySize - 1;

	for (int BarIndex = StartClearIndex; BarIndex >= 0; --BarIndex)
	{
		if (Subgraph_CurrentPriceLine[BarIndex] == 0.0f)
			break;

		Subgraph_CurrentPriceLine[BarIndex] = 0.0f;
		sc.EarliestUpdateSubgraphDataArrayIndex = BarIndex;
	}

	if (ProjectForward.GetYesNo())
	{
		int NumberOfBars = NumberOfForwardProjectionBars.GetInt();
		Subgraph_CurrentPriceLine.ExtendedArrayElementsToGraph = NumberOfBars;
		for(int Index = 0; Index < NumberOfBars; Index++)
		{
			Subgraph_CurrentPriceLine[sc.ArraySize + Index] = LineValue;
		}

		PriorLineStartIndex = sc.ArraySize;
	}
	else
		Subgraph_CurrentPriceLine.ExtendedArrayElementsToGraph = 0;

}

/*==========================================================================*/
SCSFExport scsf_CurrentBidAskLines(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_CurrentBidLine = sc.Subgraph[0];
	SCSubgraphRef Subgraph_CurrentAskLine = sc.Subgraph[1];

	SCInputRef NumberOfBarsBack = sc.Input[0];
	SCInputRef DisplayLinesOnBars = sc.Input[1];
	SCInputRef ProjectForward = sc.Input[2];
	SCInputRef NumberOfForwardProjectionBars = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Current Bid Ask Lines";

		Subgraph_CurrentBidLine.Name = "Bid";
		Subgraph_CurrentBidLine.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_CurrentBidLine.PrimaryColor = RGB(255, 0, 0);
		Subgraph_CurrentBidLine.DrawZeros = false;
		Subgraph_CurrentBidLine.LineStyle = LINESTYLE_SOLID;
		Subgraph_CurrentBidLine.LineWidth = 2;

		Subgraph_CurrentAskLine.Name = "Ask";
		Subgraph_CurrentAskLine.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_CurrentAskLine.PrimaryColor = RGB(0, 255, 0);
		Subgraph_CurrentAskLine.DrawZeros = false;
		Subgraph_CurrentAskLine.LineStyle = LINESTYLE_SOLID;
		Subgraph_CurrentAskLine.LineWidth = 2;

		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.GraphRegion = 0;

		NumberOfBarsBack.Name = "Number of Bars to Display On. 0 = Current Day";
		NumberOfBarsBack.SetInt(0);
		NumberOfBarsBack.SetIntLimits(0, MAX_STUDY_LENGTH);
		
		DisplayLinesOnBars.Name = "Display Current Lines on Chart Bars";
		DisplayLinesOnBars.SetYesNo(true);

		ProjectForward.Name = "Forward Project Bid and Ask Lines";
		ProjectForward.SetYesNo(false);

		NumberOfForwardProjectionBars.Name = "Number of Forward Projection Bars";
		NumberOfForwardProjectionBars.SetInt(10);

		sc.AutoLoop = 0;

		
		return;
	}

	if (NumberOfForwardProjectionBars.GetInt() == 0)
		NumberOfForwardProjectionBars.SetInt(10);

	float BidLineValue = sc.Bid;
	float AskLineValue = sc.Ask;

	int &PriorLineStartIndex = sc.GetPersistentInt(1);

	if (DisplayLinesOnBars.GetYesNo())
	{
		int BarsBack = NumberOfBarsBack.GetInt();

		if (BarsBack == 0)
		{
			SCDateTime TradingDayStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.ArraySize - 1]);

			int BarIndex = 0;
			for (BarIndex = sc.ArraySize - 1;
				sc.BaseDateTimeIn[BarIndex] >= TradingDayStartDateTime && BarIndex >= 0; BarIndex--)
			{
				Subgraph_CurrentBidLine[BarIndex] = BidLineValue;
				Subgraph_CurrentAskLine[BarIndex] = AskLineValue;

				sc.EarliestUpdateSubgraphDataArrayIndex = BarIndex;
			}


		}
		else
		{
			int StartingIndex = sc.ArraySize - BarsBack;

			sc.EarliestUpdateSubgraphDataArrayIndex = sc.UpdateStartIndex;

			for (int Index = sc.UpdateStartIndex - BarsBack; Index < sc.ArraySize; ++Index)
			{
				if (Index >= StartingIndex)
					break;

				Subgraph_CurrentBidLine[Index] = 0;
				Subgraph_CurrentAskLine[Index] = 0;
			}

			for (int Index = sc.ArraySize - 1; BarsBack > 0 && Index >= 0; Index--)
			{
				Subgraph_CurrentBidLine[Index] = BidLineValue;
				Subgraph_CurrentAskLine[Index] = AskLineValue;
				BarsBack--;

				if (Index < sc.EarliestUpdateSubgraphDataArrayIndex)
					sc.EarliestUpdateSubgraphDataArrayIndex = Index;
			}
		}

	}
	else if (PriorLineStartIndex != 0)
	{
		//Clear prior array elements for when projecting forward
		for (int Index = PriorLineStartIndex; Index < sc.ArraySize; ++Index)
		{
			Subgraph_CurrentBidLine[Index] = 0.0;
			Subgraph_CurrentAskLine[Index] = 0.0;
		}
	}

	// Zero out prior values
	int StartClearIndex = 0;
	if (DisplayLinesOnBars.GetYesNo())
		StartClearIndex = sc.EarliestUpdateSubgraphDataArrayIndex - 1;
	else
		StartClearIndex = sc.ArraySize - 1;

	for (int BarIndex = StartClearIndex; BarIndex >= 0; --BarIndex)
	{
		if (Subgraph_CurrentBidLine[BarIndex] == 0.0f
			&& Subgraph_CurrentAskLine[BarIndex] == 0.0f)
			break;

		Subgraph_CurrentBidLine[BarIndex] = 0.0f;
		Subgraph_CurrentAskLine[BarIndex] = 0.0f;
		sc.EarliestUpdateSubgraphDataArrayIndex = BarIndex;
	}


	if (ProjectForward.GetYesNo())
	{
		int NumberOfBars = NumberOfForwardProjectionBars.GetInt();
		Subgraph_CurrentBidLine.ExtendedArrayElementsToGraph = NumberOfBars;
		Subgraph_CurrentAskLine.ExtendedArrayElementsToGraph = NumberOfBars;

		for (int Index = 0; Index < NumberOfBars; Index++)
		{
			Subgraph_CurrentBidLine[sc.ArraySize + Index] = BidLineValue;
			Subgraph_CurrentAskLine[sc.ArraySize + Index] = AskLineValue;
		}

		PriorLineStartIndex = sc.ArraySize;
	}
	else
	{
		Subgraph_CurrentBidLine.ExtendedArrayElementsToGraph = 0;
		Subgraph_CurrentAskLine.ExtendedArrayElementsToGraph = 0;
	}
}

/*==========================================================================*/
SCSFExport scsf_OutOfSequence(SCStudyInterfaceRef sc) 
{	
	SCSubgraphRef OOOT = sc.Subgraph[0];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Out of Order Timestamps Detector";

		OOOT.Name = "OoOT";
		OOOT.PrimaryColor = RGB(255,64,32);
		OOOT.DrawStyle = DRAWSTYLE_BACKGROUND;
		OOOT.LineWidth = 5;
		OOOT.DrawZeros = false;

		sc.GraphRegion = 0;
		
		sc.DisplayStudyInputValues= false;

		sc.ScaleRangeType = SCALE_INDEPENDENT;

		sc.DrawStudyUnderneathMainPriceGraph;

		
		

		sc.AutoLoop = 1;

		return;
	}
	
	sc.DataStartIndex = 0;
	if (sc.Index <= 0)
		return;

	double prevDateTime = sc.BaseDateTimeIn[sc.Index-1];
	double curDateTime = sc.BaseDateTimeIn[sc.Index];

	if(curDateTime < prevDateTime)
		OOOT.Data[sc.Index] = 1;
	else 
		OOOT.Data[sc.Index] = 0;
	
	return;
}

/************************************************************
*  MoneyFlowIndex
*
*		typical price:			(high + low + close )/3
*		money flow:				typical price * volume
*		money ratio:			positive money flow / negative money flow
************************************************************/
SCSFExport scsf_MoneyFlowIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MFI = sc.Subgraph[0];

	SCInputRef Length = sc.Input[2];
		
	if (sc.SetDefaults)
	{
		sc.GraphName = "Money Flow Index";
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		MFI.DrawStyle = DRAWSTYLE_LINE;	
		MFI.Name = "MFI";
		MFI.PrimaryColor = RGB(0, 255, 0);
		MFI.DrawZeros = true;
		
		Length.Name = "Length";
		Length.SetInt(14); 
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		return;
	}
	
	const double BIAS_DIVISION_DOUBLE = (double)10e-20;

	double typ_price = 0.0, prev_typ_price = 0.0, money_flow = 0.0, volume = 0.0, N_pos_mon_flow = 0.0, N_neg_mon_flow = 0.0, MoneyFlowIndex = 0.0, money_ratio = 0.0;

	sc.DataStartIndex = Length.GetInt();

	if ((sc.Index - Length.GetInt() + 1) != 0)
		prev_typ_price = sc.HLCAvg[sc.Index - Length.GetInt()];

	for(int IndexOffset= 0; IndexOffset < Length.GetInt(); IndexOffset++)
	{
		volume			= sc.Volume[sc.Index - Length.GetInt() + 1 + IndexOffset];
		typ_price		= sc.HLCAvg[sc.Index - Length.GetInt() + 1 + IndexOffset];
		money_flow		= typ_price * volume;
		N_pos_mon_flow += typ_price > prev_typ_price ? money_flow : 0;
		N_neg_mon_flow += typ_price < prev_typ_price ? money_flow : 0;
		prev_typ_price	= typ_price;
	}

	money_ratio = N_pos_mon_flow / (N_neg_mon_flow + BIAS_DIVISION_DOUBLE);

	MoneyFlowIndex = 100.0 - (100.0 / (1.0 + money_ratio));

	MFI[sc.Index] = (float)MoneyFlowIndex;		
}


/*============================================================================*/
SCSFExport scsf_VolumeWeightedAveragePrice(SCStudyInterfaceRef sc)
{
	SCSubgraphRef   VWAPSubgraph = sc.Subgraph[0];
	SCFloatArrayRef BarSumPriceTimesVolume = VWAPSubgraph.Arrays[0];  
	SCFloatArrayRef BarSumPriceSquaredTimesVolume = VWAPSubgraph.Arrays[1];
	SCFloatArrayRef StandardDeviationArray = VWAPSubgraph.Arrays[2];
	SCFloatArrayRef BarSumPriceVWAPDifferenceSquaredTimesVolume = VWAPSubgraph.Arrays[3];

	SCSubgraphRef   SD1Top    = sc.Subgraph[1];
	SCSubgraphRef   SD1Bottom = sc.Subgraph[2];
	SCSubgraphRef   SD2Top    = sc.Subgraph[3];
	SCSubgraphRef   SD2Bottom = sc.Subgraph[4];
	SCSubgraphRef   SD3Top    = sc.Subgraph[5];
	SCSubgraphRef   SD3Bottom = sc.Subgraph[6];
	SCSubgraphRef   SD4Top    = sc.Subgraph[7];
	SCSubgraphRef   SD4Bottom = sc.Subgraph[8];

	SCInputRef InputData              = sc.Input[0];
	SCInputRef BandCalculationMethod  = sc.Input[1];
	SCInputRef TimePeriodType         = sc.Input[3];	
	SCInputRef TimePeriodLength       = sc.Input[4];	
	SCInputRef Version                = sc.Input[5];	
	SCInputRef DistanceMultiplier_V3  = sc.Input[6];
	SCInputRef IgnoreTimePeriodTypeAndLength = sc.Input[7];
	SCInputRef BaseOnUnderlyingData   = sc.Input[9];
	SCInputRef UseFixedOffset         = sc.Input[10];
	SCInputRef StartDateTime		  = sc.Input[11];
	SCInputRef Band1OffsetMultiplier  = sc.Input[12];
	SCInputRef Band2OffsetMultiplier  = sc.Input[13];
	SCInputRef Band3OffsetMultiplier  = sc.Input[14];
	SCInputRef Band4OffsetMultiplier  = sc.Input[15];
	

	if(sc.SetDefaults)
	{
		sc.GraphName = "Volume Weighted Average Price";
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;
		sc.ScaleRangeType=SCALE_SAMEASREGION;
		
		VWAPSubgraph.Name = "VWAP";
		VWAPSubgraph.DrawStyle = DRAWSTYLE_DASH;
		VWAPSubgraph.LineWidth = 2;
		VWAPSubgraph.PrimaryColor = RGB (237, 13, 255);
		VWAPSubgraph.DrawZeros = false;

		SD1Top.Name = "Top Band 1";
		SD1Top.DrawStyle = DRAWSTYLE_DASH;
		SD1Top.LineWidth = 2;
		SD1Top.PrimaryColor = RGB (192, 192, 192);
		SD1Top.DrawZeros = false;

		SD1Bottom.Name = "Bottom Band 1";
		SD1Bottom.DrawStyle = DRAWSTYLE_DASH;
		SD1Bottom.LineWidth = 2;
		SD1Bottom.PrimaryColor = RGB (192, 192, 192);
		SD1Bottom.DrawZeros = false;

		SD2Top.Name = "Top Band 2";
		SD2Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD2Top.LineWidth = 2;
		SD2Top.PrimaryColor = RGB (192, 192, 192);
		SD2Top.DrawZeros = false;

		SD2Bottom.Name = "Bottom Band 2";
		SD2Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD2Bottom.LineWidth = 2;
		SD2Bottom.PrimaryColor = RGB (192, 192, 192);
		SD2Bottom.DrawZeros = false;

		SD3Top.Name = "Top Band 3";
		SD3Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD3Top.LineWidth = 2;
		SD3Top.PrimaryColor = RGB (192, 192, 192);
		SD3Top.DrawZeros = false;

		SD3Bottom.Name = "Bottom Band 3";
		SD3Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD3Bottom.LineWidth = 2;
		SD3Bottom.PrimaryColor = RGB (192, 192, 192);
		SD3Bottom.DrawZeros = false;

		SD4Top.Name = "Top Band 4";
		SD4Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD4Top.LineWidth = 2;
		SD4Top.PrimaryColor = RGB (192, 192, 192);
		SD4Top.DrawZeros = false;

		SD4Bottom.Name = "Bottom Band 4";
		SD4Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD4Bottom.LineWidth = 2;
		SD4Bottom.PrimaryColor = RGB (192, 192, 192);
		SD4Bottom.DrawZeros = false;

		int DisplayOrder = 1;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);  
		InputData.DisplayOrder = DisplayOrder++;

		BaseOnUnderlyingData.Name = "Base On Underlying Data";
		BaseOnUnderlyingData.SetYesNo(0);
		BaseOnUnderlyingData.DisplayOrder = DisplayOrder++;

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);
		TimePeriodType.DisplayOrder = DisplayOrder++;
		
		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(1);
		TimePeriodLength.DisplayOrder = DisplayOrder++;

		StartDateTime.Name = "Start Date-Time";
		StartDateTime.SetDateTime(0.0);
		StartDateTime.DisplayOrder = DisplayOrder++;

		IgnoreTimePeriodTypeAndLength.Name = "Ignore Time Period Type And Length";
		IgnoreTimePeriodTypeAndLength.SetYesNo(false);
		IgnoreTimePeriodTypeAndLength.DisplayOrder = DisplayOrder++;

		DistanceMultiplier_V3.SetFloat(0.5);

		//UseFixedOffset.Name = "Use Fixed Offset Instead of Std. Deviation";
		UseFixedOffset.SetYesNo(0);
		//UseFixedOffset.DisplayOrder = DisplayOrder++;

		BandCalculationMethod.Name = "Std. Deviation Band Calculation Method";
		BandCalculationMethod.SetCustomInputStrings("VWAP Variance;Fixed Offset;Standard Deviation");
		BandCalculationMethod.SetCustomInputIndex(0);
		BandCalculationMethod.DisplayOrder = DisplayOrder++;

		Band1OffsetMultiplier.Name = "Band 1 Std. Deviation Multiplier/Fixed Offset";
		Band1OffsetMultiplier.SetFloat(0.5);
		Band1OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band2OffsetMultiplier.Name = "Band 2 Std. Deviation Multiplier/Fixed Offset";
		Band2OffsetMultiplier.SetFloat(1.0);
		Band2OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band3OffsetMultiplier.Name = "Band 3 Std. Deviation Multiplier/Fixed Offset";
		Band3OffsetMultiplier.SetFloat(1.5);
		Band3OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band4OffsetMultiplier.Name = "Band 4 Std. Deviation Multiplier/Fixed Offset";
		Band4OffsetMultiplier.SetFloat(2.0);
		Band4OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Version.SetInt(6);

		

		return;
	}

	if(Version.GetInt() < 2)
	{
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);
		TimePeriodLength.SetInt(1);

		Version.SetInt(2);
	}

	if(Version.GetInt() < 3)
	{
		DistanceMultiplier_V3.SetFloat(0.5);

		Version.SetInt(3);
	}
	
	if (Version.GetInt() < 4)
	{
		Band1OffsetMultiplier.SetFloat(DistanceMultiplier_V3.GetFloat());
		Band2OffsetMultiplier.SetFloat(DistanceMultiplier_V3.GetFloat()*2.0f);
		Band3OffsetMultiplier.SetFloat(DistanceMultiplier_V3.GetFloat()*3.0f);
		Band4OffsetMultiplier.SetFloat(DistanceMultiplier_V3.GetFloat()*4.0f);

		Version.SetInt(4);
	}

	if (Version.GetInt() < 5)
	{
		BandCalculationMethod.SetCustomInputIndex(UseFixedOffset.GetYesNo() ? 1 : 0);
		Version.SetInt(5);
	}

	if (Version.GetInt() < 6)
	{
		IgnoreTimePeriodTypeAndLength.SetYesNo(false);
		Version.SetInt(6);
	}

	sc.DataStartIndex = 1;

	int& LastIndexProcessed = sc.GetPersistentInt(1);
	int& StartIndexOfCurrentPeriod = sc.GetPersistentInt(3);
	
	double& PriorSumPriceTimesVolume        = sc.GetPersistentDouble(1);
	double& PriorSumPriceSquaredTimesVolume = sc.GetPersistentDouble(2);
	double& PriorCumulativeVolume           = sc.GetPersistentDouble(3);

	if (sc.Index == 0)
	{
		BarSumPriceTimesVolume[sc.Index] = 0;
		BarSumPriceSquaredTimesVolume[sc.Index] = 0;

		LastIndexProcessed = 0;
		PriorSumPriceTimesVolume = 0;
		PriorSumPriceSquaredTimesVolume = 0;
		PriorCumulativeVolume = 0;

		if (BaseOnUnderlyingData.GetYesNo())
		{
			if (sc.MaintainVolumeAtPriceData == 0)
			{
				sc.MaintainVolumeAtPriceData = 1;
				sc.FlagToReloadChartData = 1;
			}
		}
		else
			sc.MaintainVolumeAtPriceData = 0;
	}

	if (StartDateTime.GetDate() != 0 
		&&  sc.BaseDateTimeIn[sc.Index] < StartDateTime.GetDateTime())
		return;

	bool PriorBarIndexExcludedBasedOnDateTime = sc.Index > 0
		&& StartDateTime.GetDate() != 0 
		&&  sc.BaseDateTimeIn[sc.Index - 1] < StartDateTime.GetDateTime();

	SCDateTime PriorCurrentPeriodStartDateTime;

	SCDateTime CurrentPeriodStartDateTime;

	if(!IgnoreTimePeriodTypeAndLength.GetYesNo())
	{
		CurrentPeriodStartDateTime = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[sc.Index], TimePeriodType.GetTimePeriodType(), TimePeriodLength.GetInt(), 0);
		PriorCurrentPeriodStartDateTime = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[sc.Index - 1], TimePeriodType.GetTimePeriodType(), TimePeriodLength.GetInt(), 0);
	}

	bool IsStartOfNewPeriod = false;

	//Reset at start of new period
	if (sc.Index == 0 
		|| CurrentPeriodStartDateTime != PriorCurrentPeriodStartDateTime 
		|| PriorBarIndexExcludedBasedOnDateTime)
	{
		PriorSumPriceTimesVolume = 0;
		PriorSumPriceSquaredTimesVolume = 0;
		PriorCumulativeVolume = 0;

		LastIndexProcessed = sc.Index;

		StartIndexOfCurrentPeriod = sc.Index;
		IsStartOfNewPeriod = true;
	}
	else if (sc.Index != LastIndexProcessed)
	{
		// add in last values
		for (; LastIndexProcessed < sc.Index; ++LastIndexProcessed)
		{
			PriorSumPriceTimesVolume += BarSumPriceTimesVolume[LastIndexProcessed];
			PriorSumPriceSquaredTimesVolume += BarSumPriceSquaredTimesVolume[LastIndexProcessed];
			PriorCumulativeVolume += sc.Volume[LastIndexProcessed];
		}
	}

	double BarVolume = 0;

	float CurrentPrice = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index];

	if (BaseOnUnderlyingData.GetYesNo())
	{
		BarSumPriceTimesVolume[sc.Index] = 0;
		BarSumPriceSquaredTimesVolume[sc.Index] = 0;

		if ((int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
			return;

		const s_VolumeAtPriceV2 *p_VolumeAtPrice = nullptr;

		int NumVAPElementsAtBarIndex = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(sc.Index);
		for (int VAPIndex = 0; VAPIndex < NumVAPElementsAtBarIndex; VAPIndex++)
		{
			sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, VAPIndex, &p_VolumeAtPrice);

			float Price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;

			BarSumPriceTimesVolume[sc.Index] += (Price * p_VolumeAtPrice->Volume);
			BarSumPriceSquaredTimesVolume[sc.Index] += (Price * Price * p_VolumeAtPrice->Volume);
			BarVolume += p_VolumeAtPrice->Volume;
		}
	}
	else
	{

		BarSumPriceTimesVolume[sc.Index] = CurrentPrice * sc.Volume[sc.Index];
		BarSumPriceSquaredTimesVolume[sc.Index] = CurrentPrice * CurrentPrice * sc.Volume[sc.Index];
		BarVolume = sc.Volume[sc.Index];
	}


	double PriceTimesVolume = PriorSumPriceTimesVolume + BarSumPriceTimesVolume[sc.Index];
	double PriceSquaredTimesVolume = PriorSumPriceSquaredTimesVolume + BarSumPriceSquaredTimesVolume[sc.Index];
	double CumulativeVolume = PriorCumulativeVolume + BarVolume;

	double VolumeWeightedAveragePrice = 0;
	if (CumulativeVolume != 0)
		VolumeWeightedAveragePrice = PriceTimesVolume / CumulativeVolume;

	VWAPSubgraph[sc.Index] = (float)VolumeWeightedAveragePrice;

	float BandDistance = 1.0f;
	if (BandCalculationMethod.GetIndex() == 1)//Fixed offset
	{
		BandDistance = 1.0f;
	}
	else if (BandCalculationMethod.GetIndex() == 0)//VWAP variance
	{

		double PriceVWAPDifferenceSquaredTimesVolume = (CurrentPrice - VWAPSubgraph[sc.Index])*(CurrentPrice - VWAPSubgraph[sc.Index])* sc.Volume[sc.Index];
		if (IsStartOfNewPeriod)
			BarSumPriceVWAPDifferenceSquaredTimesVolume[sc.Index] =(float) PriceVWAPDifferenceSquaredTimesVolume;
		else
			BarSumPriceVWAPDifferenceSquaredTimesVolume[sc.Index] = 
			(float) ( BarSumPriceVWAPDifferenceSquaredTimesVolume[sc.Index-1] + PriceVWAPDifferenceSquaredTimesVolume);
		
		if (CumulativeVolume != 0)
			BandDistance = (float)sqrt(BarSumPriceVWAPDifferenceSquaredTimesVolume[sc.Index]/CumulativeVolume);
		else
			BandDistance = 1.0f;

	}
	else//Standard deviation
	{
		sc.StdDeviation(VWAPSubgraph, StandardDeviationArray, sc.Index - StartIndexOfCurrentPeriod + 1);
		BandDistance = StandardDeviationArray[sc.Index];
	}
	
	SD1Top[sc.Index]    = VWAPSubgraph[sc.Index] + Band1OffsetMultiplier.GetFloat() * BandDistance;
	SD1Bottom[sc.Index] = VWAPSubgraph[sc.Index] - Band1OffsetMultiplier.GetFloat() * BandDistance;

	SD2Top[sc.Index]    = VWAPSubgraph[sc.Index] + Band2OffsetMultiplier.GetFloat() * BandDistance;
	SD2Bottom[sc.Index] = VWAPSubgraph[sc.Index] - Band2OffsetMultiplier.GetFloat() * BandDistance;

	SD3Top[sc.Index]    = VWAPSubgraph[sc.Index] + Band3OffsetMultiplier.GetFloat() * BandDistance;
	SD3Bottom[sc.Index] = VWAPSubgraph[sc.Index] - Band3OffsetMultiplier.GetFloat() * BandDistance;

	SD4Top[sc.Index]    = VWAPSubgraph[sc.Index] + Band4OffsetMultiplier.GetFloat() * BandDistance;
	SD4Bottom[sc.Index] = VWAPSubgraph[sc.Index] - Band4OffsetMultiplier.GetFloat() * BandDistance;
}

/*===========================================================================*/
void GetIndexForStartofDayPeriodForDaysBack
( SCStudyInterfaceRef sc
, const SCDateTime InitialStartDateTime
, const int NumDaysToInclude
, const bool ExcludeWeekends
, const int MinimumRequiredTimePeriodInDayAsPercent
, int& r_InitialBarIndex
)
{
	int DaysBack = -1;

	for (int DaysCount = 0; DaysCount < NumDaysToInclude; ++DaysCount)
	{
		for (int Iterations = 10; Iterations > 0; --Iterations)
		{
			DaysBack++;

			SCDateTime PriorPeriodStartDateTime = InitialStartDateTime - DaysBack * DAYS;
			SCDateTime PriorPeriodEndDateTime = PriorPeriodStartDateTime + 1 * DAYS - 1 * SECONDS;

			r_InitialBarIndex = sc.GetNearestMatchForSCDateTime(sc.ChartNumber, PriorPeriodStartDateTime);

			const SCDateTime TradingDayDate = sc.GetTradingDayDate(PriorPeriodStartDateTime);

			if (ExcludeWeekends && TradingDayDate.IsWeekend())
				continue;

			// for the current day, do not do any check to see if there is sufficient data because there may not be if the day is not complete.
			if (NumDaysToInclude == 1)
				break;

			const int PriorPeriodLastIndex = sc.GetNearestMatchForSCDateTime(sc.ChartNumber, PriorPeriodEndDateTime);

			const double ActualTimeSpan
				= sc.CalculateTimeSpanAcrossChartBars
					( r_InitialBarIndex
					, PriorPeriodLastIndex
					);

			//If there is a sufficient amount of data in this time period
			if (ActualTimeSpan > 1*DAYS * (MinimumRequiredTimePeriodInDayAsPercent * 0.01))
				break;
		}
	} 
}

/*============================================================================*/
SCSFExport scsf_RollingVolumeWeightedAveragePrice(SCStudyInterfaceRef sc)
{
	SCSubgraphRef   VWAPSubgraph = sc.Subgraph[0];
	SCFloatArrayRef BarSumPriceTimesVolume = VWAPSubgraph.Arrays[0];  
	SCFloatArrayRef StandardDeviationArray = VWAPSubgraph.Arrays[1];
	SCFloatArrayRef PriceTimesVolumeArray = VWAPSubgraph.Arrays[2];	
	SCSubgraphRef BackgroundVWAPForCurrentPeriod =  sc.Subgraph[9];
	SCSubgraphRef PriceVWAPDifferenceSquaredTimesVolume = sc.Subgraph[10];
	SCSubgraphRef BarSumPriceVWAPDifferenceSquaredTimesVolumeArray = sc.Subgraph[11];
	SCSubgraphRef CumulativeVolumeForPeriodArray = sc.Subgraph[12];
	SCSubgraphRef BandDistance = sc.Subgraph[13];

	SCSubgraphRef   SD1Top    = sc.Subgraph[1];
	SCSubgraphRef   SD1Bottom = sc.Subgraph[2];
	SCSubgraphRef   SD2Top    = sc.Subgraph[3];
	SCSubgraphRef   SD2Bottom = sc.Subgraph[4];
	SCSubgraphRef   SD3Top    = sc.Subgraph[5];
	SCSubgraphRef   SD3Bottom = sc.Subgraph[6];
	SCSubgraphRef   SD4Top    = sc.Subgraph[7];
	SCSubgraphRef   SD4Bottom = sc.Subgraph[8];

	SCInputRef InputData              = sc.Input[0];
	SCInputRef BandCalculationMethod  = sc.Input[1];
	SCInputRef MinimumRequiredTimePeriodAsPercentForSkipDays = sc.Input[2];
	SCInputRef TimePeriodType         = sc.Input[3];	
	SCInputRef TimePeriodLength       = sc.Input[4];	
	SCInputRef Version                = sc.Input[5];	
	SCInputRef DistanceMultiplier_V3  = sc.Input[6];
	SCInputRef NumberOfDaysToCalculate  = sc.Input[7];
	SCInputRef BaseOnUnderlyingData   = sc.Input[9];
	SCInputRef UseFixedOffset         = sc.Input[10];
	SCInputRef Band1OffsetMultiplier  = sc.Input[12];
	SCInputRef Band2OffsetMultiplier  = sc.Input[13];
	SCInputRef Band3OffsetMultiplier  = sc.Input[14];
	SCInputRef Band4OffsetMultiplier  = sc.Input[15];
	SCInputRef ExcludeWeekendInDateCalc = sc.Input[16];


	if(sc.SetDefaults)
	{
		sc.GraphName = "Volume Weighted Average Price - Rolling";
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 0;//Manual looping
		sc.MaintainVolumeAtPriceData = 1;

		sc.ScaleRangeType=SCALE_SAMEASREGION;

		VWAPSubgraph.Name = "VWAP";
		VWAPSubgraph.DrawStyle = DRAWSTYLE_DASH;
		VWAPSubgraph.LineWidth = 2;
		VWAPSubgraph.PrimaryColor = RGB (237, 13, 255);
		VWAPSubgraph.DrawZeros = false;
		
		SD1Top.Name = "Top Band 1";
		SD1Top.DrawStyle = DRAWSTYLE_DASH;
		SD1Top.LineWidth = 2;
		SD1Top.PrimaryColor = RGB (192, 192, 192);
		SD1Top.DrawZeros = false;

		SD1Bottom.Name = "Bottom Band 1";
		SD1Bottom.DrawStyle = DRAWSTYLE_DASH;
		SD1Bottom.LineWidth = 2;
		SD1Bottom.PrimaryColor = RGB (192, 192, 192);
		SD1Bottom.DrawZeros = false;

		SD2Top.Name = "Top Band 2";
		SD2Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD2Top.LineWidth = 2;
		SD2Top.PrimaryColor = RGB (192, 192, 192);
		SD2Top.DrawZeros = false;

		SD2Bottom.Name = "Bottom Band 2";
		SD2Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD2Bottom.LineWidth = 2;
		SD2Bottom.PrimaryColor = RGB (192, 192, 192);
		SD2Bottom.DrawZeros = false;

		SD3Top.Name = "Top Band 3";
		SD3Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD3Top.LineWidth = 2;
		SD3Top.PrimaryColor = RGB (192, 192, 192);
		SD3Top.DrawZeros = false;

		SD3Bottom.Name = "Bottom Band 3";
		SD3Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD3Bottom.LineWidth = 2;
		SD3Bottom.PrimaryColor = RGB (192, 192, 192);
		SD3Bottom.DrawZeros = false;

		SD4Top.Name = "Top Band 4";
		SD4Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD4Top.LineWidth = 2;
		SD4Top.PrimaryColor = RGB (192, 192, 192);
		SD4Top.DrawZeros = false;

		SD4Bottom.Name = "Bottom Band 4";
		SD4Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD4Bottom.LineWidth = 2;
		SD4Bottom.PrimaryColor = RGB (192, 192, 192);
		SD4Bottom.DrawZeros = false;

		BackgroundVWAPForCurrentPeriod.Name = "BackgroundVWAPForCurrentPeriod";
		BackgroundVWAPForCurrentPeriod.DrawStyle = DRAWSTYLE_IGNORE;
		BackgroundVWAPForCurrentPeriod.DisplayNameValueInWindowsFlags = 1;

		PriceVWAPDifferenceSquaredTimesVolume.Name = "PriceVWAPDifferenceSquaredTimesVolume";
		PriceVWAPDifferenceSquaredTimesVolume.DrawStyle = DRAWSTYLE_IGNORE;
		PriceVWAPDifferenceSquaredTimesVolume.DisplayNameValueInWindowsFlags = 1;

		BarSumPriceVWAPDifferenceSquaredTimesVolumeArray.Name = "BarSumPriceVWAPDifferenceSquaredTimesVolumeArray";
		BarSumPriceVWAPDifferenceSquaredTimesVolumeArray.DrawStyle = DRAWSTYLE_IGNORE;
		BarSumPriceVWAPDifferenceSquaredTimesVolumeArray.DisplayNameValueInWindowsFlags = 1;

		CumulativeVolumeForPeriodArray.Name = "CumulativeVolumeForPeriodArray";
		CumulativeVolumeForPeriodArray.DrawStyle = DRAWSTYLE_IGNORE;
		CumulativeVolumeForPeriodArray.DisplayNameValueInWindowsFlags = 1;
		
		BandDistance.Name = "BandDistance";
		BandDistance.DrawStyle = DRAWSTYLE_IGNORE;
		BandDistance.DisplayNameValueInWindowsFlags = 1;
		
		int DisplayOrder = 1;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);  
		InputData.DisplayOrder = DisplayOrder++;

		BaseOnUnderlyingData.Name = "Base On Underlying Data";
		BaseOnUnderlyingData.SetYesNo(0);
		BaseOnUnderlyingData.DisplayOrder = DisplayOrder++;

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetCustomInputStrings("Days - Trading Days;Days - 24 Hour Period;Minutes;Bars");
		TimePeriodType.SetCustomInputIndex(0);
		TimePeriodType.DisplayOrder = DisplayOrder++;

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(1);
		TimePeriodLength.DisplayOrder = DisplayOrder++;

		NumberOfDaysToCalculate.Name = "Number of Days to Calculate";
		NumberOfDaysToCalculate.SetInt(10);
		NumberOfDaysToCalculate.DisplayOrder = DisplayOrder++;

		DistanceMultiplier_V3.SetFloat(0.5);

		ExcludeWeekendInDateCalc.Name = "Exclude Weekends In Date Look Back";
		ExcludeWeekendInDateCalc.SetYesNo(1);
		ExcludeWeekendInDateCalc.DisplayOrder = DisplayOrder++;

		MinimumRequiredTimePeriodAsPercentForSkipDays.Name = "Minimum Required Time Period as Percent for Skip Days";
		MinimumRequiredTimePeriodAsPercentForSkipDays.SetInt(0);
		MinimumRequiredTimePeriodAsPercentForSkipDays.DisplayOrder = DisplayOrder++;

		//UseFixedOffset.Name = "Use Fixed Offset Instead of Std. Deviation";
		UseFixedOffset.SetYesNo(0);
		//UseFixedOffset.DisplayOrder = DisplayOrder++;

		BandCalculationMethod.Name = "Std. Deviation Band Calculation Method";
		BandCalculationMethod.SetCustomInputStrings("VWAP Variance;Fixed Offset;Standard Deviation");
		BandCalculationMethod.SetCustomInputIndex(0);
		BandCalculationMethod.DisplayOrder = DisplayOrder++;

		Band1OffsetMultiplier.Name = "Band 1 Std. Deviation Multiplier/Fixed Offset";
		Band1OffsetMultiplier.SetFloat(0.5);
		Band1OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band2OffsetMultiplier.Name = "Band 2 Std. Deviation Multiplier/Fixed Offset";
		Band2OffsetMultiplier.SetFloat(1.0);
		Band2OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band3OffsetMultiplier.Name = "Band 3 Std. Deviation Multiplier/Fixed Offset";
		Band3OffsetMultiplier.SetFloat(1.5);
		Band3OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band4OffsetMultiplier.Name = "Band 4 Std. Deviation Multiplier/Fixed Offset";
		Band4OffsetMultiplier.SetFloat(2.0);
		Band4OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Version.SetInt(8);

		

		return;
	}

	if (NumberOfDaysToCalculate.GetInt() == 0)
		NumberOfDaysToCalculate.SetInt(10);


	// calculate the period start index
	const int PeriodType = TimePeriodType.GetIndex();

	if (PeriodType == 0 || PeriodType == 1)
	{
		int DaysRequired = TimePeriodLength.GetInt();

		if (NumberOfDaysToCalculate.GetInt() < DaysRequired)
			NumberOfDaysToCalculate.SetInt(DaysRequired);
	}

	int& r_LastIndexProcessed = sc.GetPersistentInt(1);
	int& r_FirstIndexIncluded = sc.GetPersistentInt(2);
	int& r_PriorFirstIndexIncluded = sc.GetPersistentInt(3);

	int& r_LastExcludedIndex = sc.GetPersistentInt(4);

	SCDateTime& r_PriorTradeDate = sc.GetPersistentSCDateTime(1);

	if (sc.IsFullRecalculation)//reset
	{
		r_LastIndexProcessed = -1;
		r_FirstIndexIncluded = 0;
		r_PriorFirstIndexIncluded = -1;
		r_LastExcludedIndex = -1;

		if (BaseOnUnderlyingData.GetYesNo())
		{
			if (sc.MaintainVolumeAtPriceData == 0)
			{
				sc.MaintainVolumeAtPriceData = 1;
				sc.FlagToReloadChartData = 1;
			}
		}
		else
			sc.MaintainVolumeAtPriceData = 0;
	}

	if (BaseOnUnderlyingData.GetYesNo() &&
		(int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
		return;

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; ++Index)
	{
		bool NewBar = Index != r_LastIndexProcessed;

		r_LastIndexProcessed = Index;

		BarSumPriceTimesVolume[Index] = 0;

		SCDateTime TradingDayDateForBar = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);
		SCDateTime TradingDayDateForLastBar = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.ArraySize - 1]);

		if ((TradingDayDateForLastBar.GetDate() - TradingDayDateForBar.GetDate() + 1)
			> NumberOfDaysToCalculate.GetInt() )
		{
			r_LastExcludedIndex = Index;
			continue;
		}

		float CurrentPrice = sc.BaseDataIn[InputData.GetInputDataIndex()][Index];

		// Calculate bar values
		if (BaseOnUnderlyingData.GetYesNo())
		{

			const s_VolumeAtPriceV2 *p_VolumeAtPrice = nullptr;
			int NumberOfPrices = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(Index);

			for (int VAPIndex = 0; VAPIndex < NumberOfPrices; VAPIndex++)
			{
				sc.VolumeAtPriceForBars->GetVAPElementAtIndex(Index, VAPIndex, &p_VolumeAtPrice);

				float Price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;
				float PriceTimesVolume = Price * p_VolumeAtPrice->Volume;

				BarSumPriceTimesVolume[Index] += PriceTimesVolume;
			}
#ifdef _DEBUG

			if(Index == sc.ArraySize - 1)
			{
				float  BarSumPriceTimesVolumeValue = BarSumPriceTimesVolume[Index];
				SCString Message;
				Message.Format("%f", BarSumPriceTimesVolumeValue);
				sc.AddMessageToLog(Message, 0);
			}

#endif // _DEBUG

		}
		else
		{
			BarSumPriceTimesVolume[Index] = CurrentPrice * sc.Volume[Index];
		}

		if (NewBar)
		{

			if (PeriodType == 0)  // Days - trading days
			{
				int TradeDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

				SCDateTime CurrentDayStartDateTime = sc.GetStartDateTimeForTradingDate(TradeDate);

				r_PriorTradeDate = TradeDate;

				r_FirstIndexIncluded = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, CurrentDayStartDateTime);

				GetIndexForStartofDayPeriodForDaysBack
					( sc
					, CurrentDayStartDateTime
					, TimePeriodLength.GetInt()
					, ExcludeWeekendInDateCalc.GetYesNo() != 0
					, MinimumRequiredTimePeriodAsPercentForSkipDays.GetInt()
					, r_FirstIndexIncluded
					);
			}
			else if (PeriodType == 1)  // Days - 24 hour periods
			{
				SCDateTime CurrentPeriodStartDateTime = sc.BaseDateTimeIn[Index] - 1*DAYS + 1*SECONDS;
				r_FirstIndexIncluded = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, CurrentPeriodStartDateTime);

				GetIndexForStartofDayPeriodForDaysBack
					( sc
					, CurrentPeriodStartDateTime
					, TimePeriodLength.GetInt()
					, ExcludeWeekendInDateCalc.GetYesNo() != 0
					, MinimumRequiredTimePeriodAsPercentForSkipDays.GetInt()
					, r_FirstIndexIncluded
					);
			}
			else if (PeriodType == 2)  // Minutes
			{
				SCDateTime RollingStartDateTime = 0;

				if (sc.AreTimeSpecificBars() && sc.SecondsPerBar != 0)
				{
					RollingStartDateTime  = sc.BaseDateTimeIn[Index] + (sc.SecondsPerBar - 1) * SECONDS;
					RollingStartDateTime -= TimePeriodLength.GetInt() * MINUTES;
				}
				else
				{
					if (Index == sc.ArraySize - 1)
						RollingStartDateTime = sc.LatestDateTimeForLastBar;
					else
						RollingStartDateTime = sc.BaseDateTimeIn[Index+1] - 1*SECONDS;

					RollingStartDateTime -= TimePeriodLength.GetInt() * MINUTES;
				}

				for (; r_FirstIndexIncluded <= Index; ++r_FirstIndexIncluded)
				{
					if (sc.BaseDateTimeIn[r_FirstIndexIncluded] < RollingStartDateTime)
						continue;

					break;
				}
			}
			else  // Bars
			{
				r_FirstIndexIncluded = max(0, Index - TimePeriodLength.GetInt()+1);
			}

#ifdef _DEBUG
			SCString MessageText;
			SCString IndexDateTime = sc.DateTimeToString(sc.BaseDateTimeIn[Index], FLAG_DT_COMPLETE_DATETIME);
			SCString FirstIndexIncludedDateTime = sc.DateTimeToString(sc.BaseDateTimeIn[r_FirstIndexIncluded], FLAG_DT_COMPLETE_DATETIME);
			SCString IndexTradingDate = sc.DateTimeToString(sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]), FLAG_DT_COMPLETE_DATE);
			MessageText.Format("NewBar. FirstIndexIncluded: %d, CurrentIndex: %d, FirstIndexIncluded_DateTime: %s, CurrentIndex_DateTime: %s, CurrentIndex_TradingDate: %s"
				, r_FirstIndexIncluded, Index, FirstIndexIncludedDateTime.GetChars(), IndexDateTime.GetChars(), IndexTradingDate.GetChars());
			sc.AddMessageToLog(MessageText, 0);

#endif

		}

		if (r_FirstIndexIncluded <= r_LastExcludedIndex)
			r_FirstIndexIncluded = r_LastExcludedIndex + 1;

		int CalculationStartIndex = Index;
		bool ResetCalculation = false;
		if (r_PriorFirstIndexIncluded != r_FirstIndexIncluded
			|| r_FirstIndexIncluded == Index)
		{
			CalculationStartIndex = r_FirstIndexIncluded;
			ResetCalculation = true;
		}

		r_PriorFirstIndexIncluded = r_FirstIndexIncluded;

		bool TempResetCalculation = ResetCalculation;
		for (int AvgIndex = CalculationStartIndex; AvgIndex <= Index; ++AvgIndex)
		{
			if (TempResetCalculation)
			{
				TempResetCalculation = false;
				PriceTimesVolumeArray[AvgIndex] =  BarSumPriceTimesVolume[AvgIndex];
				CumulativeVolumeForPeriodArray[AvgIndex] = sc.Volume[AvgIndex];
			}
			else
			{

				PriceTimesVolumeArray[AvgIndex] = PriceTimesVolumeArray[ AvgIndex - 1] + BarSumPriceTimesVolume[AvgIndex];
				CumulativeVolumeForPeriodArray[AvgIndex] = CumulativeVolumeForPeriodArray[ AvgIndex - 1] + sc.Volume[AvgIndex];
			}

			if (CumulativeVolumeForPeriodArray[AvgIndex] != 0)
				BackgroundVWAPForCurrentPeriod[AvgIndex] = PriceTimesVolumeArray[AvgIndex] / CumulativeVolumeForPeriodArray[AvgIndex];
			else
				BackgroundVWAPForCurrentPeriod[AvgIndex] = 0;
		}


		if (CumulativeVolumeForPeriodArray[Index] != 0)
			VWAPSubgraph[Index] = PriceTimesVolumeArray[Index] / CumulativeVolumeForPeriodArray[Index];


		if (BandCalculationMethod.GetIndex() == 1)//Fixed offset
		{
			BandDistance[Index] = 1.0f;
		}
		else if (BandCalculationMethod.GetIndex() == 0)//VWAP variance
		{
			for (int BarIndex = CalculationStartIndex; BarIndex <= Index; ++BarIndex)
			{
				PriceVWAPDifferenceSquaredTimesVolume[BarIndex] = 
				(sc.BaseDataIn[InputData.GetInputDataIndex()][BarIndex] - BackgroundVWAPForCurrentPeriod[BarIndex])
				* (sc.BaseDataIn[InputData.GetInputDataIndex()][BarIndex] - BackgroundVWAPForCurrentPeriod[BarIndex]) 
				* sc.Volume[BarIndex];

				if (ResetCalculation)
				{
					ResetCalculation = false;
					BarSumPriceVWAPDifferenceSquaredTimesVolumeArray[BarIndex] = PriceVWAPDifferenceSquaredTimesVolume[BarIndex];
				}
				else
				{
					BarSumPriceVWAPDifferenceSquaredTimesVolumeArray[BarIndex] = BarSumPriceVWAPDifferenceSquaredTimesVolumeArray[BarIndex - 1] + PriceVWAPDifferenceSquaredTimesVolume[BarIndex];

				}

				//float CheckValue1 = BarSumPriceVWAPDifferenceSquaredTimesVolumeArray[BarIndex];
				//float CheckValue2 = PriceVWAPDifferenceSquaredTimesVolume[BarIndex];
				//float CheckValue3 = 0;
			}

			if (CumulativeVolumeForPeriodArray[Index] != 0)
			{
				BandDistance[Index] = (float)sqrt(BarSumPriceVWAPDifferenceSquaredTimesVolumeArray[Index] / CumulativeVolumeForPeriodArray[Index]);
			}
			else
				BandDistance[Index] = 1.0f;

		}
		else//Standard deviation
		{
			sc.StdDeviation(VWAPSubgraph, StandardDeviationArray, Index, Index - r_FirstIndexIncluded + 1);
			BandDistance[Index] = StandardDeviationArray[Index];
		}

		
		SD1Top[Index]    = VWAPSubgraph[Index] + Band1OffsetMultiplier.GetFloat() * BandDistance[Index];
		SD1Bottom[Index] = VWAPSubgraph[Index] - Band1OffsetMultiplier.GetFloat() * BandDistance[Index];

		SD2Top[Index]    = VWAPSubgraph[Index] + Band2OffsetMultiplier.GetFloat() * BandDistance[Index];
		SD2Bottom[Index] = VWAPSubgraph[Index] - Band2OffsetMultiplier.GetFloat() * BandDistance[Index];

		SD3Top[Index]    = VWAPSubgraph[Index] + Band3OffsetMultiplier.GetFloat() * BandDistance[Index];
		SD3Bottom[Index] = VWAPSubgraph[Index] - Band3OffsetMultiplier.GetFloat() * BandDistance[Index];

		SD4Top[Index]    = VWAPSubgraph[Index] + Band4OffsetMultiplier.GetFloat() * BandDistance[Index];
		SD4Bottom[Index] = VWAPSubgraph[Index] - Band4OffsetMultiplier.GetFloat() * BandDistance[Index];
	}
}

/*============================================================================*/
SCSFExport scsf_RollingHighAccuracyMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef   MovingAverage = sc.Subgraph[0];
	SCFloatArrayRef PriceSumArray = MovingAverage.Arrays[0];
	SCFloatArrayRef PriceCountArray = MovingAverage.Arrays[1];
	SCFloatArrayRef StandardDeviationArray = MovingAverage.Arrays[2];

	SCSubgraphRef   SD1Top    = sc.Subgraph[1];
	SCSubgraphRef   SD1Bottom = sc.Subgraph[2];
	SCSubgraphRef   SD2Top    = sc.Subgraph[3];
	SCSubgraphRef   SD2Bottom = sc.Subgraph[4];
	SCSubgraphRef   SD3Top    = sc.Subgraph[5];
	SCSubgraphRef   SD3Bottom = sc.Subgraph[6];
	SCSubgraphRef   SD4Top    = sc.Subgraph[7];
	SCSubgraphRef   SD4Bottom = sc.Subgraph[8];

	SCInputRef InputData              = sc.Input[0];
	SCInputRef BandCalculationMethod  = sc.Input[1];
	SCInputRef MinimumRequiredTimePeriodAsPercentForSkipDays = sc.Input[2];
	SCInputRef TimePeriodType         = sc.Input[3];	
	SCInputRef TimePeriodLength       = sc.Input[4];	
	SCInputRef Version                = sc.Input[5];	
	SCInputRef NumberOfDaysToCalculate  = sc.Input[7];
	SCInputRef BaseOnUnderlyingData   = sc.Input[8];
	SCInputRef Band1OffsetMultiplier  = sc.Input[9];
	SCInputRef Band2OffsetMultiplier  = sc.Input[10];
	SCInputRef Band3OffsetMultiplier  = sc.Input[11];
	SCInputRef Band4OffsetMultiplier  = sc.Input[12];
	SCInputRef ExcludeWeekendInDateCalc = sc.Input[13];


	if(sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Rolling High Accuracy";
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 0;//Manual looping
		sc.MaintainVolumeAtPriceData = 1;

		MovingAverage.Name = "Moving Average";
		MovingAverage.DrawStyle = DRAWSTYLE_LINE;
		MovingAverage.LineWidth = 2;
		MovingAverage.PrimaryColor = RGB(237, 13, 255);
		MovingAverage.DrawZeros = false;
		
		SD1Top.Name = "Top Band 1";
		SD1Top.DrawStyle = DRAWSTYLE_DASH;
		SD1Top.LineWidth = 2;
		SD1Top.PrimaryColor = RGB (192, 192, 192);
		SD1Top.DrawZeros = false;

		SD1Bottom.Name = "Bottom Band 1";
		SD1Bottom.DrawStyle = DRAWSTYLE_DASH;
		SD1Bottom.LineWidth = 2;
		SD1Bottom.PrimaryColor = RGB (192, 192, 192);
		SD1Bottom.DrawZeros = false;

		SD2Top.Name = "Top Band 2";
		SD2Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD2Top.LineWidth = 2;
		SD2Top.PrimaryColor = RGB (192, 192, 192);
		SD2Top.DrawZeros = false;

		SD2Bottom.Name = "Bottom Band 2";
		SD2Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD2Bottom.LineWidth = 2;
		SD2Bottom.PrimaryColor = RGB (192, 192, 192);
		SD2Bottom.DrawZeros = false;

		SD3Top.Name = "Top Band 3";
		SD3Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD3Top.LineWidth = 2;
		SD3Top.PrimaryColor = RGB (192, 192, 192);
		SD3Top.DrawZeros = false;

		SD3Bottom.Name = "Bottom Band 3";
		SD3Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD3Bottom.LineWidth = 2;
		SD3Bottom.PrimaryColor = RGB (192, 192, 192);
		SD3Bottom.DrawZeros = false;

		SD4Top.Name = "Top Band 4";
		SD4Top.DrawStyle = DRAWSTYLE_IGNORE;
		SD4Top.LineWidth = 2;
		SD4Top.PrimaryColor = RGB (192, 192, 192);
		SD4Top.DrawZeros = false;

		SD4Bottom.Name = "Bottom Band 4";
		SD4Bottom.DrawStyle = DRAWSTYLE_IGNORE;
		SD4Bottom.LineWidth = 2;
		SD4Bottom.PrimaryColor = RGB (192, 192, 192);
		SD4Bottom.DrawZeros = false;

		int DisplayOrder = 1;

		InputData.Name = "Input Data";
		InputData.SetCustomInputStrings("Price;Volume;Number of Trades;AskVolume-BidVolume");
		InputData.SetCustomInputIndex(0);
		InputData.DisplayOrder = DisplayOrder++;

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetCustomInputStrings("Days - Trading Days;Days - 24 Hour Period;Minutes;Bars");
		TimePeriodType.SetCustomInputIndex(0);
		TimePeriodType.DisplayOrder = DisplayOrder++;

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(1);
		TimePeriodLength.DisplayOrder = DisplayOrder++;

		NumberOfDaysToCalculate.Name = "Number of Days to Calculate";
		NumberOfDaysToCalculate.SetInt(10);
		NumberOfDaysToCalculate.DisplayOrder = DisplayOrder++;

		ExcludeWeekendInDateCalc.Name = "Exclude Weekends In Date Look Back";
		ExcludeWeekendInDateCalc.SetYesNo(1);
		ExcludeWeekendInDateCalc.DisplayOrder = DisplayOrder++;

		MinimumRequiredTimePeriodAsPercentForSkipDays.Name = "Minimum Required Time Period as Percent for Skip Days";
		MinimumRequiredTimePeriodAsPercentForSkipDays.SetInt(25);
		MinimumRequiredTimePeriodAsPercentForSkipDays.DisplayOrder = DisplayOrder++;

		BandCalculationMethod.Name = "Std. Deviation Band Calculation Method";
		BandCalculationMethod.SetCustomInputStrings("Standard Deviation;Fixed Offset");
		BandCalculationMethod.SetCustomInputIndex(1);
		BandCalculationMethod.DisplayOrder = DisplayOrder++;

		Band1OffsetMultiplier.Name = "Band 1 Std. Deviation Multiplier/Fixed Offset";
		Band1OffsetMultiplier.SetFloat(0.5);
		Band1OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band2OffsetMultiplier.Name = "Band 2 Std. Deviation Multiplier/Fixed Offset";
		Band2OffsetMultiplier.SetFloat(1.0);
		Band2OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band3OffsetMultiplier.Name = "Band 3 Std. Deviation Multiplier/Fixed Offset";
		Band3OffsetMultiplier.SetFloat(1.5);
		Band3OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Band4OffsetMultiplier.Name = "Band 4 Std. Deviation Multiplier/Fixed Offset";
		Band4OffsetMultiplier.SetFloat(2.0);
		Band4OffsetMultiplier.DisplayOrder = DisplayOrder++;

		Version.SetInt(2);

		

		return;
	}

	if(Version.GetInt() < 2)
	{
		Version.SetInt(2);
		InputData.SetCustomInputIndex(0);
	}

	if ((int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
		return;

	if (NumberOfDaysToCalculate.GetInt() == 0)
		NumberOfDaysToCalculate.SetInt(10);


	// calculate the period start index
	int PeriodType = TimePeriodType.GetIndex();

	if (PeriodType == 0 || PeriodType == 1)
	{
		int DaysRequired = TimePeriodLength.GetInt();

		if (NumberOfDaysToCalculate.GetInt() < DaysRequired)
			NumberOfDaysToCalculate.SetInt(DaysRequired);
	}


	int& LastIndexProcessed = sc.GetPersistentInt(1);
	int& FirstIndexIncluded = sc.GetPersistentInt(2);
	int& LastExcludedIndex = sc.GetPersistentInt(3);


	if (sc.IsFullRecalculation)//reset
	{
		LastIndexProcessed = -1;
		FirstIndexIncluded = 0;
		LastExcludedIndex = -1;


		if (sc.MaintainVolumeAtPriceData == 0)
		{
			sc.MaintainVolumeAtPriceData = 1;
			sc.FlagToReloadChartData = 1;
		}

	}

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; ++Index)
	{
		bool NewBar = Index != LastIndexProcessed;

		LastIndexProcessed = Index;

		SCDateTime TradingDayDateForBar = sc.GetTradingDayDate( sc.BaseDateTimeIn[Index]);
		SCDateTime TradingDayDateForLastBar = sc.GetTradingDayDate( sc.BaseDateTimeIn[sc.ArraySize - 1]);

		if ((TradingDayDateForLastBar.GetDate() - TradingDayDateForBar.GetDate() + 1)
	> NumberOfDaysToCalculate.GetInt())
		{
			LastExcludedIndex = Index;
			continue;
		}

		PriceSumArray[Index] = 0;
		PriceCountArray[Index] = 0;

		// calculate bar summation

		const s_VolumeAtPriceV2* p_VolumeAtPrice = NULL;

		int NumVAPElementsAtBarIndex = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(Index);
		for (int VAPIndex = 0; VAPIndex < NumVAPElementsAtBarIndex; VAPIndex++)
		{
			sc.VolumeAtPriceForBars->GetVAPElementAtIndex(Index, VAPIndex, &p_VolumeAtPrice);

			int InputDataIndex = InputData.GetIndex();
			if (InputDataIndex == 0)
			{
				float Price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;
				float PriceTimeTrades = Price * p_VolumeAtPrice->NumberOfTrades;

				PriceSumArray[Index] += PriceTimeTrades;
				PriceCountArray[Index] += p_VolumeAtPrice->NumberOfTrades;
			}
			else if (InputDataIndex == 1)
			{
				PriceSumArray[Index] += p_VolumeAtPrice->Volume;
				PriceCountArray[Index] += 1;

			}
			else if (InputDataIndex == 2)
			{
				PriceSumArray[Index] += p_VolumeAtPrice->NumberOfTrades;
				PriceCountArray[Index] += 1;
			}
			else if (InputDataIndex == 3)
			{
				PriceSumArray[Index] += labs(p_VolumeAtPrice->AskVolume - p_VolumeAtPrice->BidVolume);
				PriceCountArray[Index] += 1;
			}
		}

		if (NewBar)
		{

			if (PeriodType == 0)  // Days - trading days
			{
				int TradeDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

				SCDateTime CurrentDayStartDateTime = sc.GetStartDateTimeForTradingDate(TradeDate);
				FirstIndexIncluded = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, CurrentDayStartDateTime);

				GetIndexForStartofDayPeriodForDaysBack
					( sc
					, CurrentDayStartDateTime
					, TimePeriodLength.GetInt()
					, ExcludeWeekendInDateCalc.GetYesNo() != 0
					, MinimumRequiredTimePeriodAsPercentForSkipDays.GetInt()
					, FirstIndexIncluded
					);
			}
			else if (PeriodType == 1)  // Days - 24 hour periods
			{
				SCDateTime CurrentPeriodStartDateTime = sc.BaseDateTimeIn[Index] - 1*DAYS + 1*SECONDS;
				FirstIndexIncluded = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, CurrentPeriodStartDateTime);

				GetIndexForStartofDayPeriodForDaysBack
					( sc
					, CurrentPeriodStartDateTime
					, TimePeriodLength.GetInt()
					, ExcludeWeekendInDateCalc.GetYesNo() != 0
					, MinimumRequiredTimePeriodAsPercentForSkipDays.GetInt()
					, FirstIndexIncluded
					);
			}
			else if (PeriodType == 2)  // Minutes
			{
				SCDateTime RollingStartDateTime = 0;

				if (sc.AreTimeSpecificBars() && sc.SecondsPerBar != 0)
				{
					RollingStartDateTime  = sc.BaseDateTimeIn[Index] + (sc.SecondsPerBar - 1) * SECONDS;
					RollingStartDateTime -= TimePeriodLength.GetInt() * MINUTES;
				}
				else
				{
					if (Index == sc.ArraySize - 1)
						RollingStartDateTime = sc.LatestDateTimeForLastBar;
					else
						RollingStartDateTime = sc.BaseDateTimeIn[Index+1] - 1*SECONDS;

					RollingStartDateTime -= TimePeriodLength.GetInt() * MINUTES;
				}

				for (; FirstIndexIncluded <= Index; ++FirstIndexIncluded)
				{
					if (sc.BaseDateTimeIn[FirstIndexIncluded] < RollingStartDateTime)
						continue;

					break;
				}
			}
			else  // Bars
			{
				FirstIndexIncluded = max(0, Index - TimePeriodLength.GetInt()+1);
			}


		}

		if (FirstIndexIncluded <= LastExcludedIndex)
			FirstIndexIncluded = LastExcludedIndex + 1;


		// calculate average
		double CumulativePriceSum = 0;
		double CumulativePriceCount = 0;

		for (int AvgIndex=FirstIndexIncluded; AvgIndex <= Index; ++AvgIndex)
		{
			CumulativePriceSum += PriceSumArray[AvgIndex];
			CumulativePriceCount += PriceCountArray[AvgIndex];
		}

		if (CumulativePriceCount != 0)
			MovingAverage[Index] = (float)(CumulativePriceSum / CumulativePriceCount);
		else
			MovingAverage[Index] = MovingAverage[Index - 1];


		float BandDistance = 1.0f;
		
		//Standard deviation
		if (BandCalculationMethod.GetInt() == 0)
		{
			const int StandardDeviationLength = Index - FirstIndexIncluded + 1;
			if (Index - StandardDeviationLength + 1 > LastExcludedIndex)
			{
				sc.StdDeviation(MovingAverage, StandardDeviationArray, Index, StandardDeviationLength);
				BandDistance = StandardDeviationArray[Index];
			}
		}


		SD1Top[Index]    = MovingAverage[Index] + Band1OffsetMultiplier.GetFloat() * BandDistance;
		SD1Bottom[Index] = MovingAverage[Index] - Band1OffsetMultiplier.GetFloat() * BandDistance;

		SD2Top[Index]    = MovingAverage[Index] + Band2OffsetMultiplier.GetFloat() * BandDistance;
		SD2Bottom[Index] = MovingAverage[Index] - Band2OffsetMultiplier.GetFloat() * BandDistance;

		SD3Top[Index]    = MovingAverage[Index] + Band3OffsetMultiplier.GetFloat() * BandDistance;
		SD3Bottom[Index] = MovingAverage[Index] - Band3OffsetMultiplier.GetFloat() * BandDistance;

		SD4Top[Index]    = MovingAverage[Index] + Band4OffsetMultiplier.GetFloat() * BandDistance;
		SD4Bottom[Index] = MovingAverage[Index] - Band4OffsetMultiplier.GetFloat() * BandDistance;
	}
}

/*============================================================================*/

SCSFExport scsf_TrueBarAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef   Average = sc.Subgraph[0];

	if(sc.SetDefaults)
	{
		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphName = "True Bar Average";
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;
		sc.MaintainVolumeAtPriceData = 1;

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.LineWidth = 2;
		Average.PrimaryColor = RGB(237, 13, 255);
		Average.DrawZeros = false;


		

		return;
	}


	int PriceCount = 0;
	float Sum = 0.0;

	const s_VolumeAtPriceV2 *p_VolumeAtPrice = NULL;

	int NumVAPElementsAtBarIndex = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(sc.Index);
	for (int VAPIndex = 0; VAPIndex < NumVAPElementsAtBarIndex; VAPIndex++)
	{
		sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, VAPIndex, &p_VolumeAtPrice);

		Sum += p_VolumeAtPrice->PriceInTicks * sc.TickSize;
		++PriceCount;
	}

	if (PriceCount != 0)
		Average[sc.Index] = Sum / PriceCount;		 

}

/*============================================================================*/
SCSFExport scsf_MovingLinearRegressionLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MLR = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[2];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Moving Linear Regression";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;  
		sc.AutoLoop = 1;

		MLR.Name = "MLR";
		MLR.DrawStyle = DRAWSTYLE_LINE;
		MLR.PrimaryColor = RGB(0,255,0);
		MLR.DrawZeros = false;
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 
		
		Length.Name     = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);


		
		

		return;
	}

	sc.DataStartIndex = Length.GetInt()-1;	
	
	sc.LinearRegressionIndicator(sc.BaseDataIn[InputData.GetInputDataIndex()], MLR,  Length.GetInt());		
}
/*============================================================================*/

SCSFExport scsf_MACD(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MACD		= sc.Subgraph[0];
	SCSubgraphRef MovAvgOfMACD	= sc.Subgraph[1];
	SCSubgraphRef MACDDiff	= sc.Subgraph[2];
	SCSubgraphRef RefLine	= sc.Subgraph[3];

	SCInputRef InputData	= sc.Input[0];
	SCInputRef FastLength		= sc.Input[3];
	SCInputRef SlowLength		= sc.Input[4];
	SCInputRef MACDLength		= sc.Input[5];
	SCInputRef MovingAverageType		= sc.Input[6];


	if(sc.SetDefaults)
	{
		sc.GraphName = "MACD";
		sc.AutoLoop					= 1;

		sc.GraphRegion				= 1;
		sc.ValueFormat				= 3;
		
		MACD.Name = "MACD";
		MACD.DrawStyle = DRAWSTYLE_LINE;
		MACD.DrawZeros = true;
		MACD.PrimaryColor = RGB(0,255,0);

		MovAvgOfMACD.Name	= "MA of MACD";
		MovAvgOfMACD.DrawStyle = DRAWSTYLE_LINE;
		MovAvgOfMACD.DrawZeros = true;
		MovAvgOfMACD.PrimaryColor = RGB(255,0,255);

		MACDDiff.Name = "MACD Diff";
		MACDDiff.DrawStyle = DRAWSTYLE_BAR;
		MACDDiff.DrawZeros = true;
		MACDDiff.PrimaryColor = RGB(255,255,0);

		RefLine.Name = "Line";
		RefLine.DrawStyle = DRAWSTYLE_LINE;
		RefLine.DrawZeros = true;
		RefLine.PrimaryColor = RGB(255,127,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		FastLength.Name ="Fast Moving Average Length";
		FastLength.SetInt(12);  
		FastLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowLength.Name = "Slow Moving Average Length";
		SlowLength.SetInt(26);  
		SlowLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		MACDLength.Name = "MACD Moving Average Length";
		MACDLength.SetInt(9);   
		MACDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		MovingAverageType.Name = "Moving Average Type";
		MovingAverageType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		return;
	}

	if (MovingAverageType.GetMovAvgType()==MOVAVGTYPE_EXPONENTIAL)
		sc.DataStartIndex = 2;
	else
		sc.DataStartIndex = MACDLength.GetInt() + max(FastLength.GetInt(), SlowLength.GetInt());

	sc.MACD(sc.BaseDataIn[InputData.GetInputDataIndex()], MACD, sc.Index, FastLength.GetInt(), SlowLength.GetInt(), MACDLength.GetInt(), MovingAverageType.GetInt()); 

	MovAvgOfMACD[sc.Index]	= MACD.Arrays[2][sc.Index];
	MACDDiff[sc.Index]	= MACD.Arrays[3][sc.Index];
	RefLine[sc.Index]	= 0;	
}


/*============================================================================*/

SCSFExport scsf_3_10_Oscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MACD = sc.Subgraph[0];
	SCSubgraphRef MovAvgOfMACD = sc.Subgraph[1];
	SCSubgraphRef MACDDiff = sc.Subgraph[2];
	SCSubgraphRef RefLine = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef FastLength = sc.Input[1];
	SCInputRef SlowLength = sc.Input[2];
	SCInputRef MACDLength = sc.Input[3];


	if (sc.SetDefaults)
	{
		sc.GraphName = "3/10 Oscillator";
		sc.AutoLoop = 1;

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		MACD.Name = "3/10";
		MACD.DrawStyle = DRAWSTYLE_LINE;
		MACD.DrawZeros = true;
		MACD.PrimaryColor = RGB(0, 255, 0);

		MovAvgOfMACD.Name = "Mov Avg 3/10";
		MovAvgOfMACD.DrawStyle = DRAWSTYLE_LINE;
		MovAvgOfMACD.DrawZeros = true;
		MovAvgOfMACD.PrimaryColor = RGB(255, 0, 255);

		MACDDiff.Name = "3/10 Diff";
		MACDDiff.DrawStyle = DRAWSTYLE_BAR;
		MACDDiff.DrawZeros = true;
		MACDDiff.PrimaryColor = RGB(255, 255, 0);

		RefLine.Name = "Line";
		RefLine.DrawStyle = DRAWSTYLE_LINE;
		RefLine.DrawZeros = true;
		RefLine.PrimaryColor = RGB(255, 127, 0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		FastLength.Name = "Fast Moving Average Length";
		FastLength.SetInt(3);
		FastLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		SlowLength.Name = "Slow Moving Average Length";
		SlowLength.SetInt(10);
		SlowLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		MACDLength.Name = "3/10 Moving Average Length";
		MACDLength.SetInt(16);
		MACDLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		return;
	}

	sc.DataStartIndex = MACDLength.GetInt() + max(FastLength.GetInt(), SlowLength.GetInt());

	sc.MACD(sc.BaseDataIn[InputData.GetInputDataIndex()], MACD, sc.Index, FastLength.GetInt(), SlowLength.GetInt(), MACDLength.GetInt(), MOVAVGTYPE_SIMPLE);

	MovAvgOfMACD[sc.Index] = MACD.Arrays[2][sc.Index];
	MACDDiff[sc.Index] = MACD.Arrays[3][sc.Index];
	RefLine[sc.Index] = 0;
}

/*============================================================================*/

SCSFExport scsf_MACDLeader(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MACD = sc.Subgraph[0];
	SCFloatArrayRef FastExponential = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef SlowExponential = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef  InputDataMinusFastAverage = sc.Subgraph[0].Arrays[2];
	SCFloatArrayRef  InputDataMinusSlowAverage = sc.Subgraph[0].Arrays[3];

	SCFloatArrayRef  AvgInputDataMinusFastAverage = sc.Subgraph[0].Arrays[4];
	SCFloatArrayRef  AvgInputDataMinusSlowAverage = sc.Subgraph[0].Arrays[5];

	SCSubgraphRef MovAvgOfMACD = sc.Subgraph[1];
	SCSubgraphRef MACDLeader = sc.Subgraph[2];
	SCSubgraphRef RefLine = sc.Subgraph[3];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef FastLength = sc.Input[1];
	SCInputRef SlowLength = sc.Input[2];
	SCInputRef MACDMovingAverageLength = sc.Input[3];
	SCInputRef MovingAverageType = sc.Input[4];
	SCInputRef MACDMovingAverageType = sc.Input[5];


	if (sc.SetDefaults)
	{
		sc.GraphName = "MACD Leader";
		sc.AutoLoop = 1;

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		MACD.Name = "MACD";
		MACD.DrawStyle = DRAWSTYLE_LINE;
		MACD.DrawZeros = true;
		MACD.PrimaryColor = RGB(0, 255, 0);

		MovAvgOfMACD.Name = "MA of MACD";
		MovAvgOfMACD.DrawStyle = DRAWSTYLE_LINE;
		MovAvgOfMACD.DrawZeros = true;
		MovAvgOfMACD.PrimaryColor = RGB(255, 0, 255);

		MACDLeader.Name = "MACD Leader";
		MACDLeader.DrawStyle = DRAWSTYLE_LINE;
		MACDLeader.LineWidth = 2;
		MACDLeader.DrawZeros = true;
		MACDLeader.PrimaryColor = RGB(255, 128, 0);

		RefLine.Name = "Line";
		RefLine.DrawStyle = DRAWSTYLE_LINE;
		RefLine.DrawZeros = true;
		RefLine.PrimaryColor = RGB(255, 127, 0);

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		FastLength.Name = "Fast Moving Average Length";
		FastLength.SetInt(12);
		FastLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		SlowLength.Name = "Slow Moving Average Length";
		SlowLength.SetInt(26);
		SlowLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		MACDMovingAverageLength.Name = "MACD Moving Average Length";
		MACDMovingAverageLength.SetInt(9);
		MACDMovingAverageLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		MovingAverageType.Name = "Moving Average Type";
		MovingAverageType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		MACDMovingAverageType.Name = "MACD Moving Average Type";
		MACDMovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		return;
	}

	sc.DataStartIndex = MACDMovingAverageLength.GetInt() + max(FastLength.GetInt(), SlowLength.GetInt());

	int InputData = Input_InputData.GetInputDataIndex();
	sc.MovingAverage(sc.BaseDataIn[InputData], FastExponential, MovingAverageType.GetInt(), FastLength.GetInt() );
	sc.MovingAverage(sc.BaseDataIn[InputData], SlowExponential, MovingAverageType.GetInt(), SlowLength.GetInt());

	MACD[sc.Index] = FastExponential[sc.Index] - SlowExponential[sc.Index];

	sc.MovingAverage(MACD, MovAvgOfMACD, MACDMovingAverageType.GetInt(), MACDMovingAverageLength.GetInt());

	InputDataMinusFastAverage[sc.Index] = sc.BaseDataIn[InputData][sc.Index] - FastExponential[sc.Index];

	InputDataMinusSlowAverage[sc.Index] = sc.BaseDataIn[InputData][sc.Index] - SlowExponential[sc.Index];

	sc.MovingAverage(InputDataMinusFastAverage, AvgInputDataMinusFastAverage, MovingAverageType.GetInt(), FastLength.GetInt());
	sc.MovingAverage(InputDataMinusSlowAverage, AvgInputDataMinusSlowAverage, MovingAverageType.GetInt(), SlowLength.GetInt());

	MACDLeader[sc.Index] = MACD[sc.Index] + AvgInputDataMinusFastAverage[sc.Index] - AvgInputDataMinusSlowAverage[sc.Index];

	RefLine[sc.Index] = 0;
}

/*============================================================================*/

SCSFExport scsf_StandardErrorBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpperBand = sc.Subgraph[0];
	SCSubgraphRef MidBand = sc.Subgraph[1];
	SCSubgraphRef LowerBand = sc.Subgraph[2];
	

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[2];
	SCInputRef StandardDeviations = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Standard Error Bands";
		sc.GraphRegion = 0;
		sc.ValueFormat = 3;  
		
		MidBand.Name = "Middle Band";
		MidBand.DrawStyle = DRAWSTYLE_LINE;
		MidBand.DrawZeros = false;
		MidBand.PrimaryColor = RGB(255,0,255);

		LowerBand.Name = "Bottom Band";
		LowerBand.DrawStyle = DRAWSTYLE_LINE;
		LowerBand.DrawZeros = false;
		LowerBand.PrimaryColor = RGB(255,255,0);

		UpperBand.Name = "Top Band";
		UpperBand.DrawStyle = DRAWSTYLE_LINE;
		UpperBand.DrawZeros = false;
		UpperBand.PrimaryColor = RGB(0,255,0);
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);  
		
		Length.Name = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);
		StandardDeviations.SetFloatLimits(.001f,(float)MAX_STUDY_LENGTH);
		

		
		

		sc.AutoLoop = 1;

		return;
	}

	sc.DataStartIndex = Length.GetInt()-1;	



	sc.LinearRegressionIndicator(sc.BaseDataIn[InputData.GetInputDataIndex()], MidBand, Length.GetInt());		

	double StandardErrorValue = sc.GetStandardError(sc.BaseDataIn[InputData.GetInputDataIndex()], Length.GetInt());
	UpperBand[sc.Index] = (float)(MidBand[sc.Index] + StandardErrorValue * StandardDeviations.GetFloat());
	LowerBand[sc.Index] = (float)(MidBand[sc.Index] - StandardErrorValue * StandardDeviations.GetFloat());
}

/*===========================================================================*/
SCSFExport scsf_BollingerBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpperBand = sc.Subgraph[0];
	SCSubgraphRef MidBand = sc.Subgraph[1];
	SCSubgraphRef LowerBand = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];
	SCInputRef StandardDeviations = sc.Input[4];
	SCInputRef Version = sc.Input[5];
	SCInputRef MAType = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Bollinger Bands";
		sc.GraphRegion = 0;
		sc.ValueFormat = 3;  
		sc.AutoLoop = true;
		
		MidBand.Name = "Middle Band";
		MidBand.DrawStyle = DRAWSTYLE_LINE;
		MidBand.PrimaryColor = RGB(255,0,255);

		UpperBand.Name = "Top Band";
		UpperBand.DrawStyle = DRAWSTYLE_LINE;
		UpperBand.PrimaryColor = RGB(0,255,0);

		LowerBand.Name = "Bottom Band";
		LowerBand.DrawStyle = DRAWSTYLE_LINE;
		LowerBand.PrimaryColor = RGB(255,255,0);
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HLC_AVG);
		
		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);
		StandardDeviations.SetFloatLimits(.0001f,(float)MAX_STUDY_LENGTH);

		Version.SetInt(2);

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);


		

		return;
	}


	sc.DataStartIndex = Length.GetInt()-1; 
	
	sc.BollingerBands(sc.BaseDataIn[InputData.GetInputDataIndex()],MidBand,Length.GetInt(),StandardDeviations.GetFloat(),MAType.GetMovAvgType());

	UpperBand[sc.Index] = MidBand.Arrays[0][sc.Index];
	LowerBand[sc.Index] = MidBand.Arrays[1][sc.Index];
}
/*===========================================================================*/


SCSFExport scsf_BollingerBands_StandardDeviationOfAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpperBand = sc.Subgraph[0];
	SCSubgraphRef MidBand = sc.Subgraph[1];
	SCSubgraphRef LowerBand = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef StandardDeviations = sc.Input[2];
	SCInputRef MAType = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Bollinger Bands - StandardDeviationOfAverage";
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = true;

		MidBand.Name = "Middle Band";
		MidBand.DrawStyle = DRAWSTYLE_LINE;
		MidBand.PrimaryColor = RGB(255,0,255);

		UpperBand.Name = "Top Band";
		UpperBand.DrawStyle = DRAWSTYLE_LINE;
		UpperBand.PrimaryColor = RGB(0,255,0);

		LowerBand.Name = "Bottom Band";
		LowerBand.DrawStyle = DRAWSTYLE_LINE;
		LowerBand.PrimaryColor = RGB(255,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HLC_AVG);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);
		StandardDeviations.SetFloatLimits(.0001f, (float)MAX_STUDY_LENGTH);


		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);


		

		return;
	}


	sc.DataStartIndex = Length.GetInt()-1; 

	BollingerBands_StandardDeviationOfAverage_S(sc.BaseDataIn[InputData.GetInputDataIndex()],
		MidBand,
		UpperBand,
		LowerBand,
		MidBand.Arrays[0],
		sc.Index,
		Length.GetInt(),
		StandardDeviations.GetFloat(),
		MAType.GetMovAvgType());

}

/*===========================================================================*/
SCSFExport scsf_PositiveVolumeIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PVI = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef InitialValue = sc.Input[2];
	
	if(sc.SetDefaults)
	{
		sc.GraphName = "Positive Volume Index";
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1; // true
		
		PVI.Name = "PVI"; 
		PVI.DrawStyle = DRAWSTYLE_LINE;
		PVI.PrimaryColor = RGB(0,255,0);
		PVI.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		
		InitialValue.Name = "Initial Value";
		InitialValue.SetFloat(2.5f);
		InitialValue.SetFloatLimits(.01f,(float)MAX_STUDY_LENGTH);

		

		return;
	}
	
	float prev_PVI;
	float prev_data;
	float data;
	float PVIval;
	float volume;
	float prev_volume;

	sc.DataStartIndex = 0;
	
	int pos = sc.Index;
	if(pos == 0)// We are starting a new PVI study
	{
		PVI[0] = InitialValue.GetFloat();
		pos++;
	}

	volume = sc.Volume[pos]; 
	prev_volume = sc.Volume[pos-1];
	prev_PVI = PVI[pos-1];
	if(volume > prev_volume) // current volume less than equal to previous volume. 
	{
		data = sc.BaseDataIn[InputData.GetInputDataIndex()][pos]; 
		prev_data = sc.BaseDataIn[InputData.GetInputDataIndex()][pos-1];							
		PVIval = prev_PVI + ((data-prev_data) / prev_data * prev_PVI );
	}
	else
	{
		PVIval = prev_PVI;				
	}

	PVI[pos] = PVIval;
}
/************************************************************************************************************************/
SCSFExport scsf_NegativeVolumeIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef NVI = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef InitialValue = sc.Input[2];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Negative Volume Index";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop= true;

		NVI.Name = "NVI";
		NVI.DrawStyle = DRAWSTYLE_LINE;
		NVI.PrimaryColor = RGB(0,255,0);
		NVI.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);  

		InitialValue.Name = "Initial Value";
		InitialValue.SetFloat(2.5f);
		InitialValue.SetFloatLimits(0.01f,(float)MAX_STUDY_LENGTH);

		
		

		return;
	}

	sc.DataStartIndex = 0;

	float prev_NVI;
	float prev_data;
	float data;
	float NVIval;
	float volume;
	float prev_volume;

	int pos = sc.Index;
	if(pos == 0)// We are starting a new NVI study
	{
		NVI[0] = InitialValue.GetFloat();
		pos++;
	}

	volume = sc.Volume[pos]; 
	prev_volume	= sc.Volume[pos-1];
	prev_NVI = NVI[pos-1];
	if(volume < prev_volume) // current volume greater then equal to previous volume. 
	{
		data = sc.BaseDataIn[InputData.GetInputDataIndex()][pos]; 
		prev_data = sc.BaseDataIn[InputData.GetInputDataIndex()][pos-1];							
		NVIval = prev_NVI + ((data-prev_data) / prev_data * prev_NVI);
	}
	else
	{
		NVIval	= prev_NVI;
	}

	NVI[pos] = NVIval;
}
/************************************************************************************************************************/
SCSFExport scsf_DoubleStochasticNew(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DS = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[5];
	SCSubgraphRef Line2 = sc.Subgraph[6];
		
	SCInputRef Length = sc.Input[0];
	SCInputRef EMALength = sc.Input[2];
	SCInputRef MovAvgType = sc.Input[3];
	SCInputRef Line1Value = sc.Input[4];
	SCInputRef Line2Value = sc.Input[5];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Double Stochastic";
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;  
		sc.AutoLoop = true;

		DS.Name = "DS";
		DS.DrawStyle = DRAWSTYLE_LINE;
		DS.PrimaryColor = RGB(0,255,0);
		DS.DrawZeros = false;
		
		Line1.Name = "Line 1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(0,255,0);
		Line1.DrawZeros = false;
		
		Line2.Name = "Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(0,255,0);
		Line2.DrawZeros = false;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		EMALength.Name = "Moving Average Length";
		EMALength.SetInt(3);
		EMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		
		Line1Value.Name = "Line 1";
		Line1Value.SetInt(10);
		
		Line2Value.Name = "Line 2";
		Line2Value.SetInt(85);

		
		
		
		return;
	}

	sc.DoubleStochastic(sc.BaseDataIn, DS, Length.GetInt(), EMALength.GetInt(), MovAvgType.GetMovAvgType());

	Line1[sc.Index] = (float)Line1Value.GetInt();
	Line2[sc.Index] = (float)Line2Value.GetInt();
}

/*==========================================================================*/
SCSFExport scsf_CandlesNoTails(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef NumTrades = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Candles without Tails";
	
		sc.GraphDrawType = GDT_CANDLESTICK;
		sc.StandardChartHeader = 1;
		sc.DisplayAsMainPriceGraph = 1;
		sc.GraphRegion = 0;
			
		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;
		
		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,127,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_LINE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;

		NumTrades.Name = "# of Trades / OI";
		NumTrades.DrawStyle = DRAWSTYLE_IGNORE;
		NumTrades.PrimaryColor = RGB(0,0,255);
		NumTrades.DrawZeros = false;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(127,0,255);
		OHLCAvg.DrawZeros = false;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);
		HLCAvg.DrawZeros = false;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);
		HLAvg.DrawZeros = false;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);
		BidVol.DrawZeros = false;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);
		AskVol.DrawZeros = false;

		
		

		sc.AutoLoop = 1;
		
		return;
	}
	//create a new name that includes the base graph name.
	sc.GraphName.Format("%s No Tails", sc.GetStudyName(0).GetChars());
	
	// Do data processing
	
	Open[sc.Index] = sc.Open[sc.Index];
	Last[sc.Index] = sc.Close[sc.Index];
	Volume[sc.Index] = sc.Volume[sc.Index];
	NumTrades[sc.Index] = sc.NumberOfTrades[sc.Index];
}

/**************************************************************************************************/
SCSFExport scsf_KiwisTrailingStop(SCStudyInterfaceRef sc)
{	
	SCSubgraphRef TSUp = sc.Subgraph[0];
	SCSubgraphRef TSDown = sc.Subgraph[1];	

	SCSubgraphRef direction = sc.Subgraph[3];
	SCSubgraphRef hiclose = sc.Subgraph[4];
	SCSubgraphRef loclose = sc.Subgraph[5];

	SCFloatArrayRef e1 = sc.Subgraph[6];
	SCFloatArrayRef e2 = sc.Subgraph[7];
	SCFloatArrayRef e3 = sc.Subgraph[8];
	SCFloatArrayRef e4 = sc.Subgraph[9];
	SCFloatArrayRef e5 = sc.Subgraph[10];
	SCFloatArrayRef e6 = sc.Subgraph[11];

	SCSubgraphRef TempRange = sc.Subgraph[6];
	SCSubgraphRef TempAvgRange = sc.Subgraph[7];
	SCSubgraphRef diff2 = sc.Subgraph[8];
	SCSubgraphRef atrmod = sc.Subgraph[9];

	SCInputRef Use2Subgraphs = sc.Input[0];
	SCInputRef ATRLength = sc.Input[2];
	SCInputRef ATRFactor = sc.Input[3];
	SCInputRef Vervoort = sc.Input[4];
	SCInputRef ATRMax = sc.Input[5];



	if(sc.SetDefaults)
	{
		sc.GraphName="Kiwi's Trailing Stop";
		sc.StudyDescription = "Kiwi's Trailing Stop (Chandelier) plus Sylvain Vervoort variation";

		TSUp.Name="TS";
		TSUp.DrawStyle = DRAWSTYLE_LINE_SKIP_ZEROS;
		TSUp.SecondaryColorUsed = 1;
		TSUp.PrimaryColor = RGB(0,255,0);
		TSUp.SecondaryColor = RGB(255,0,0);
		TSUp.DrawZeros = false;

		TSDown.Name = "TS";
		TSDown.DrawStyle = DRAWSTYLE_LINE_SKIP_ZEROS;
		TSDown.PrimaryColor = RGB(255,0,0);
		TSDown.DrawZeros = false;

		TempRange.Name = "TR";
		TempRange.DrawStyle = DRAWSTYLE_IGNORE;
		TempRange.PrimaryColor = RGB(127,0,255);
		TempRange.DrawZeros = false;

		TempAvgRange.Name = "TAR";
		TempAvgRange.DrawStyle = DRAWSTYLE_IGNORE;
		TempAvgRange.PrimaryColor = RGB(0,255,255);
		TempAvgRange.DrawZeros = false;

		diff2.Name = "diff2";
		diff2.DrawStyle = DRAWSTYLE_IGNORE;
		diff2.PrimaryColor = RGB(0,127,255);
		diff2.DrawZeros = false;

		atrmod.Name = "atrmod";
		atrmod.DrawStyle = DRAWSTYLE_IGNORE;
		atrmod.PrimaryColor = RGB(0,255,0);
		atrmod.DrawZeros = false;

		sc.AutoLoop = 0;

		ATRLength.Name="ATR Length";
		ATRLength.SetFloat(9.0);

		ATRFactor.Name="ATR Factor";
		ATRFactor.SetFloat(2.5);

		Vervoort.Name="Vervoort Variation?";
		Vervoort.SetYesNo(false);

		ATRMax.Name="Vervoort Maximum Range";
		ATRMax.SetFloat(1.5);

		Use2Subgraphs.Name="Use 2 subgraphs";
		Use2Subgraphs.SetYesNo(true);

		sc.GraphRegion = 0;

		return;
	}

	int 	pos;
	float 	Ave;
	float  	b,b2,b3,c1,c2,c3,c4,w1=0,w2=0;

	sc.DataStartIndex=10;

	if(Use2Subgraphs.GetYesNo())
	{
		TSUp.Name = "TS Up";
		TSDown.Name = "TS Down";

		TSUp.SecondaryColorUsed = false;
	}
	else
	{
		TSUp.Name="TS";

		TSUp.SecondaryColorUsed = 1;
	}


	if (sc.UpdateStartIndex == 0)
	{
		direction[sc.UpdateStartIndex] = 1;
		loclose[sc.UpdateStartIndex] = sc.Close[sc.UpdateStartIndex]; 
		hiclose[sc.UpdateStartIndex] = loclose[sc.UpdateStartIndex];

		e1[sc.UpdateStartIndex]=sc.High[sc.UpdateStartIndex]-sc.Low[sc.UpdateStartIndex]; 
		e2[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex]; 
		e3[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex]; 
		e4[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex]; 
		e5[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex]; 
		e6[sc.UpdateStartIndex]=e1[sc.UpdateStartIndex];
	}

	if(!Vervoort.GetYesNo()) 
	{
		b=0.5;					// 0.5
		b2=(b*b);				// 0.25
		b3=(b2*b);				// 0.125

		c1=-b3;					// - 0.125
		c2=(3*(b2+b3));			// 0.45
		c3=(-3*(2*b2+b+b3)); 
		c4=(1+3*b+b3+3*b2);
		w1 = 2/(ATRLength.GetFloat()+1);  
		w2 = 1-w1;

		for (pos=max(sc.UpdateStartIndex, 1); pos < sc.ArraySize; pos++)
		{
			float temp = max(sc.High[pos]-sc.Low[pos], sc.High[pos]-sc.Close[pos-1]);
			float P   = max(temp,sc.Close[pos-1]-sc.Low[pos]);

			e1[pos] = w1*P + w2*e1[pos - 1];
			e2[pos] = w1*e1[pos] + w2*e2[pos - 1];
			e3[pos] = w1*e2[pos] + w2*e3[pos - 1];
			e4[pos] = w1*e3[pos] + w2*e4[pos - 1];
			e5[pos] = w1*e4[pos] + w2*e5[pos - 1];
			e6[pos] = w1*e5[pos] + w2*e6[pos - 1];

			Ave = c1*e6[pos] + c2*e5[pos] + c3*e4[pos] + c4*e3[pos];

			if((direction[pos-1]==1 && sc.Close[pos]<(loclose[pos-1]))
				|| 
				(direction[pos-1]==-1 && sc.Close[pos]>(hiclose[pos-1])))
			{
				if(direction[pos-1]==1)
				{
					direction[pos] = -1; //reverse short
					hiclose[pos] = sc.Close[pos]+(Ave*ATRFactor.GetFloat());
					loclose[pos] = 0;
				} 
				else
				{
					direction[pos] = 1; //reverse long
					loclose[pos] = sc.Close[pos]-(Ave*ATRFactor.GetFloat());
					hiclose[pos] = 0;
				}
			} 
			else
			{
				if(direction[pos-1]==1)
				{
					if(sc.Close[pos]-(Ave*ATRFactor.GetFloat())>loclose[pos-1])
					{
						loclose[pos] = sc.Close[pos]-(Ave*ATRFactor.GetFloat());
						hiclose[pos] = hiclose[pos-1];
					}
					else
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = hiclose[pos-1];
					}
				} 
				else
				{
					if(sc.Close[pos]+(Ave*ATRFactor.GetFloat())<hiclose[pos-1])
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = sc.Close[pos]+(Ave*ATRFactor.GetFloat());
					}
					else
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = hiclose[pos-1];
					}
				};

				direction[pos] = direction[pos-1]; // no change
			};

			if(direction[pos]==1) 
			{
				if(Use2Subgraphs.GetYesNo() == false)
				{
					TSUp[pos] = (loclose[pos]); 
					TSUp.DataColor[pos] = TSUp.PrimaryColor;
				}
				else
				{
					TSUp[pos] = (loclose[pos]); 
					TSDown[pos] = 0;
				}
			} 
			else
			{
				if(Use2Subgraphs.GetYesNo() == false)
				{
					TSUp[pos] = (hiclose[pos]); 
					TSUp.DataColor[pos] =  TSUp.SecondaryColor;
				}
				else
				{
					TSDown[pos] = (hiclose[pos]);  
					TSUp[pos] = 0;
				}
			}	
		}
	} else
	{

		/*	
		HiLo:=If(H-L<1.5*Mov(H-L,period,S),H-L, 1.5*Mov
		(H-L,period,S));
		Href:=If(L<=Ref(H,-1),H-Ref(C,-1),(H-Ref(C,-1))-(L-Ref(H,-1))/2);
		Lref:=If(H>=Ref(L,-1),Ref(C,-1)-L,(Ref(C,-1)-L)-(Ref(L,-1)-H)/2);
		diff1:=Max(HiLo,Href);
		diff2:=Max(diff1,Lref);
		atrmod:=Wilders(diff2,period);
		loss:=atrfact*atrmod;
		trail:= 
		If(C>PREV AND Ref(C,-1)>PREV,
		Max(PREV,C-loss),
		If(C<PREV AND Ref(C,-1)<PREV,
		Min(PREV,C+loss),
		If(C>PREV,C-loss,C+loss)));
		*/	


		for (pos=max(sc.UpdateStartIndex, 1); pos < sc.ArraySize; pos++)
		{
			float HiLo,Href,Lref;
			int period = (int)ATRLength.GetFloat();
			TempRange[pos] = sc.High[pos]-sc.Low[pos];
			sc.MovingAverage(TempRange, TempAvgRange, MOVAVGTYPE_SIMPLE, pos, period);

			HiLo=min(TempRange[pos],ATRMax.GetFloat()*TempAvgRange[pos]);
			if(sc.Low[pos]<=sc.High[pos-1]) Href=sc.High[pos]-sc.Close[pos-1]; else Href=(sc.High[pos]-sc.Close[pos-1])-(sc.Low[pos]-sc.High[pos-1])/2;
			if(sc.High[pos]>=sc.Low[pos-1]) Lref=sc.Close[pos-1]-sc.Low[pos];  else Lref=(sc.Close[pos-1]-sc.Low[pos])- (sc.Low[pos-1]-sc.High[pos])/2;

			diff2[pos] = max(Lref,max(HiLo,Href));
			sc.MovingAverage(diff2, atrmod, MOVAVGTYPE_WILDERS, pos, period);

			Ave = atrmod[pos];

			if((direction[pos-1]==1 && sc.Close[pos]<(loclose[pos-1]))
				|| 
				(direction[pos-1]==-1 && sc.Close[pos]>(hiclose[pos-1])))
			{
				if(direction[pos-1]==1)
				{
					direction[pos] = -1; //reverse short
					hiclose[pos] = sc.Close[pos]+(Ave*ATRFactor.GetFloat());
					loclose[pos] = 0;
				} 
				else
				{
					direction[pos] = 1; //reverse long
					loclose[pos] = sc.Close[pos]-(Ave*ATRFactor.GetFloat());
					hiclose[pos] = 0;
				}
			} 
			else
			{
				if(direction[pos-1]==1)
				{
					if(sc.Close[pos]-(Ave*ATRFactor.GetFloat())>loclose[pos-1])
					{
						loclose[pos] = sc.Close[pos]-(Ave*ATRFactor.GetFloat());
						hiclose[pos] = hiclose[pos-1];
					}
					else
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = hiclose[pos-1];
					}
				} 
				else
				{
					if(sc.Close[pos]+(Ave*ATRFactor.GetFloat())<hiclose[pos-1])
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = sc.Close[pos]+(Ave*ATRFactor.GetFloat());
					}
					else
					{
						loclose[pos] = loclose[pos-1];
						hiclose[pos] = hiclose[pos-1];
					}
				};

				direction[pos] = direction[pos-1]; // no change
			};

			if(direction[pos]==1) 
			{
				if(Use2Subgraphs.GetYesNo() == false)
				{
					TSUp[pos] = (loclose[pos]); 
					TSUp.DataColor[pos] = TSUp.PrimaryColor;
				}
				else
				{
					TSUp[pos] = (loclose[pos]); 
					TSDown[pos] = 0;
				}
			} 
			else
			{
				if(Use2Subgraphs.GetYesNo() == false)
				{
					TSUp[pos] = (hiclose[pos]); 
					TSUp.DataColor[pos] =  TSUp.SecondaryColor;
				}
				else
				{
					TSDown[pos] = (hiclose[pos]);  
					TSUp[pos] = 0;
				}
			}
		}
	}
}

/*============================================================================*/
SCSFExport scsf_LargeTextDisplayForStudyFromChart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[0];

	SCInputRef Input_HorizontalPosition = sc.Input[0];
	SCInputRef Input_VerticalPosition = sc.Input[1];
	SCInputRef Input_StudySubgraphReference = sc.Input[3];
	SCInputRef Input_DisplaySubgraphName = sc.Input[4];
	SCInputRef Input_SubgraphOffset = sc.Input[5];
	SCInputRef Input_UseTextForNonZeroValue = sc.Input[6];
	SCInputRef Input_NonZeroValueText = sc.Input[7];
	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Text Display For Study from Chart";
		
		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		Subgraph_TextDisplay.Name = "Text Display";
		Subgraph_TextDisplay.LineWidth = 20;
		Subgraph_TextDisplay.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Subgraph_TextDisplay.PrimaryColor = RGB(0, 0, 0); //black
		Subgraph_TextDisplay.SecondaryColor = RGB(128, 255, 255);
		Subgraph_TextDisplay.SecondaryColorUsed = true;
		Subgraph_TextDisplay.DisplayNameValueInWindowsFlags = 0;

		Input_HorizontalPosition.Name.Format("Horizontal Position From Left (1-%d)", (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);
		Input_HorizontalPosition.SetInt(20);
		Input_HorizontalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

		Input_VerticalPosition.Name.Format("Vertical Position From Bottom (1-%d)", (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		Input_VerticalPosition.SetInt(90);
		Input_VerticalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);

		Input_StudySubgraphReference.Name = "Study And Subgraph To Display";
		Input_StudySubgraphReference.SetChartStudySubgraphValues(1, 1, 0);

		Input_DisplaySubgraphName.Name = "Display Subgraph Name";
		Input_DisplaySubgraphName.SetYesNo(false);

		Input_SubgraphOffset.Name = "Subgraph Columns Back";
		Input_SubgraphOffset.SetInt(0);

		Input_UseTextForNonZeroValue.Name = "Use Text for Non-Zero Value";
		Input_UseTextForNonZeroValue.SetYesNo(false);

		Input_NonZeroValueText.Name = "Non-Zero Value Text";
		Input_NonZeroValueText.SetString("");

		sc.TextInputName = "Prefix Text for Display Value";

		
		return;
	}
	
	// Do data processing


	SCFloatArray StudyReference;

	sc.GetStudyArrayFromChartUsingID(Input_StudySubgraphReference.GetChartNumber(), Input_StudySubgraphReference.GetStudyID(), Input_StudySubgraphReference.GetSubgraphIndex(), StudyReference);
	
	
	SCString ValueText;
	if (sc.TextInput.GetLength()>0)
	{
		ValueText += sc.TextInput;
		ValueText += " ";
	}

	int ColumnsBack = Input_SubgraphOffset.GetInt();
	float StudyValue = StudyReference[StudyReference.GetArraySize() - 1 - ColumnsBack];

	//Patch for when referencing data from the Numbers Bars Calculated Values study. This value could be used.
	if (StudyValue == -FLT_MAX)
		StudyValue = 0;

	if (Input_UseTextForNonZeroValue.GetYesNo())
	{
		if (StudyValue != 0)
			ValueText += Input_NonZeroValueText.GetString();
		else
			ValueText += " ";
	}
	else
		ValueText += sc.FormatGraphValue(StudyValue, sc.GetValueFormat());

	sc.AddAndManageSingleTextDrawingForStudy
	(sc
		, false
		, Input_HorizontalPosition.GetInt()
		, Input_VerticalPosition.GetInt()
		, Subgraph_TextDisplay
		, false
		, ValueText
		, true
	);
}


/*============================================================================*/
SCSFExport scsf_LargeTextDisplayForStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_TextDisplay = sc.Subgraph[0];

	SCInputRef Input_HorizontalPosition = sc.Input[0];
	SCInputRef Input_VerticalPosition = sc.Input[1];
	SCInputRef Input_StudySubgraphReference = sc.Input[3];
	SCInputRef Input_DisplaySubgraphName = sc.Input[4];
	SCInputRef Input_SubgraphOffset = sc.Input[5];
	SCInputRef Input_PrefixTextForDisplayValue = sc.Input[6];
	SCInputRef Input_SuffixTextForDisplayValue = sc.Input[7];
	SCInputRef Input_UseTextForNonZeroValue = sc.Input[8];
	SCInputRef Input_NonZeroValueText = sc.Input[9];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Text Display For Study";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		Subgraph_TextDisplay.Name = "Text Display";
		Subgraph_TextDisplay.LineWidth = 20;
		Subgraph_TextDisplay.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Subgraph_TextDisplay.PrimaryColor = RGB(0, 0, 0); //black
		Subgraph_TextDisplay.SecondaryColor = RGB(128, 255, 255);
		Subgraph_TextDisplay.SecondaryColorUsed = true;
		Subgraph_TextDisplay.DisplayNameValueInWindowsFlags = 0;

		Input_HorizontalPosition.Name.Format("Horizontal Position From Left (1-%d)", (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);
		Input_HorizontalPosition.SetInt(20);
		Input_HorizontalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

		Input_VerticalPosition.Name.Format("Vertical Position From Bottom (1-%d)", (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		Input_VerticalPosition.SetInt(90);
		Input_VerticalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);

		Input_StudySubgraphReference.Name = "Study and Subgraph to Display";
		Input_StudySubgraphReference.SetStudySubgraphValues(1, 0);

		Input_DisplaySubgraphName.Name = "Display Subgraph Name";
		Input_DisplaySubgraphName.SetYesNo(false);

		Input_SubgraphOffset.Name = "Subgraph Columns Back";
		Input_SubgraphOffset.SetInt(0);

		Input_PrefixTextForDisplayValue.Name = "Prefix Text for Display Value";
		Input_PrefixTextForDisplayValue.SetString("");

		Input_SuffixTextForDisplayValue.Name = "Suffix Text for Display Value";
		Input_SuffixTextForDisplayValue.SetString("");

		Input_UseTextForNonZeroValue.Name = "Use Text for Non-Zero Value";
		Input_UseTextForNonZeroValue.SetYesNo(false);

		Input_NonZeroValueText.Name = "Non-Zero Value Text";
		Input_NonZeroValueText.SetString("");

		
		return;
	}

	// Do data processing

	SCFloatArray StudyReference;
	sc.GetStudyArrayUsingID(Input_StudySubgraphReference.GetStudyID(), Input_StudySubgraphReference.GetSubgraphIndex(), StudyReference);


	int ColumnsBack = Input_SubgraphOffset.GetInt();
	SCString ValueText;
	if (Input_PrefixTextForDisplayValue.GetString()[0] != '\0')
	{
		ValueText += Input_PrefixTextForDisplayValue.GetString();
		ValueText += " ";
	}

	if (Input_DisplaySubgraphName.GetYesNo())
	{
		const char* SubgraphName = sc.GetStudySubgraphName(Input_StudySubgraphReference.GetStudyID(), Input_StudySubgraphReference.GetSubgraphIndex());

		if (SubgraphName != NULL)
		{
			ValueText += SubgraphName;
			ValueText += ": ";
		}
	}

	float StudyValue = StudyReference[StudyReference.GetArraySize() - 1 - ColumnsBack];

	if (StudyValue == -FLT_MAX)//Patch for when referencing data from the Numbers Bars Calculated Values study. This value could be used.
		StudyValue = 0;

	if(Input_UseTextForNonZeroValue.GetYesNo() )
	{
		if (StudyValue != 0)
			ValueText += Input_NonZeroValueText.GetString();
		else
			ValueText += " ";
	}
	else
		ValueText += sc.FormatGraphValue(StudyValue, sc.GetValueFormat());

	ValueText += Input_SuffixTextForDisplayValue.GetString();

	sc.AddAndManageSingleTextDrawingForStudy
		( sc
		, false
		, Input_HorizontalPosition.GetInt()
		, Input_VerticalPosition.GetInt()
		, Subgraph_TextDisplay
		, false
		, ValueText
		, true
		);

}

/*============================================================================ */
SCSFExport scsf_ProfitLossTextStudy(SCStudyInterfaceRef sc)
{
	SCInputRef HorizontalPosition = sc.Input[0];
	SCInputRef VerticalPosition   = sc.Input[1];
	SCInputRef DisplayInFillSpace = sc.Input[2];
	SCInputRef ProfitLossFormat   = sc.Input[3];
	SCInputRef RTCommission       = sc.Input[4];

	SCInputRef ShowOpenPL         = sc.Input[5];
	SCInputRef ShowClosedPL       = sc.Input[6];
	SCInputRef ShowDailyPL        = sc.Input[7];
	SCInputRef IncludeOpenPLInDailyPL = sc.Input[8];

	SCInputRef TextSize                = sc.Input[10];
	SCInputRef TextColor               = sc.Input[11];
	SCInputRef TextBackColor           = sc.Input[12];
	SCInputRef ShowTotalTrades         = sc.Input[13];
	SCInputRef ShowTotalQuantityFilled = sc.Input[14];
	SCInputRef ShowDailyTotalTrades    = sc.Input[15];
	SCInputRef ShowDailyTotalQuantityFilled = sc.Input[16];
	SCInputRef ShowMaximumOpenPositionProfit = sc.Input[17];
	SCInputRef ShowMaximumOpenPositionLoss = sc.Input[18];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Trading: Profit/Loss Text";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;
		sc.MaintainTradeStatisticsAndTradesData = 1;

		HorizontalPosition.Name.Format("Horizontal Position From Left (1-%d)", (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);
		HorizontalPosition.SetInt(20);
		HorizontalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

		VerticalPosition.Name.Format("Vertical Position From Bottom (1-%d)", (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		VerticalPosition.SetInt(90);
		VerticalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);

		DisplayInFillSpace.Name = "Display in Fill Space";
		DisplayInFillSpace.SetYesNo(false);

		ShowOpenPL.Name = "Show Open Profit/Loss";
		ShowOpenPL.SetYesNo(1);

		ShowClosedPL.Name = "Show Closed Profit/Loss";
		ShowClosedPL.SetYesNo(1);

		ShowDailyPL.Name = "Show Daily Profit/Loss";
		ShowDailyPL.SetYesNo(1);

		IncludeOpenPLInDailyPL.Name = "Include Open Profit/Loss in Daily Profit/Loss";
		IncludeOpenPLInDailyPL.SetYesNo(0);

		TextSize.Name = "Text Size";
		TextSize.SetInt(14);
		TextSize.SetIntLimits(3, 50);

		TextColor.Name = "Text Color";
		TextColor.SetColor(0,0,0);  // black

		TextBackColor.Name = "Text Background Color";
		TextBackColor.SetColor(128,255,255);

		ShowTotalTrades.Name = "Show Total Trades";
		ShowTotalTrades.SetYesNo(0);

		ShowTotalQuantityFilled.Name = "Show Total Quantity Filled";
		ShowTotalQuantityFilled.SetYesNo(0);

		ShowDailyTotalTrades.Name = "Show Daily Total Trades";
		ShowDailyTotalTrades.SetYesNo(0);

		ShowDailyTotalQuantityFilled.Name = "Show Daily Total Quantity Filled";
		ShowDailyTotalQuantityFilled.SetYesNo(0);

		ShowMaximumOpenPositionProfit.Name = "Show Maximum Open Position Profit";
		ShowMaximumOpenPositionProfit.SetYesNo(0);

		ShowMaximumOpenPositionLoss.Name = "Show Maximum Open Position Loss";
		ShowMaximumOpenPositionLoss.SetYesNo(0);

		ProfitLossFormat.Name = "Profit/Loss Display Format";
		ProfitLossFormat.SetCustomInputStrings("Currency Value;Points (P);Points - Ignore Quantity (p);Ticks (T);Ticks - Ignore Quantity (t);Currency Value & Points (P);Currency Value & Points - Ignore Quantity (p);Currency Value & Ticks (T);Currency Value & Ticks - Ignore Quantity (t)");
		ProfitLossFormat.SetCustomInputIndex(0);

		RTCommission.Name = "Round Turn Commission To Include";
		RTCommission.SetFloat(0);

		
		return;
	}

	// This is to ensure that we access the trade data that matches the simulation mode setting.
	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

	if (sc.HideStudy)
		return;

	int& r_TextDrawingLineNumber = sc.GetPersistentInt(1);

	// Do data processing
	s_UseTool Tool;

	Tool.Clear(); // Reset tool structure for our next use. Unnecessary in this case, but good practice.
	Tool.DrawingType = DRAWING_TEXT;
	Tool.ChartNumber = sc.ChartNumber;
	Tool.Region = sc.GraphRegion;

	if (r_TextDrawingLineNumber != 0)
		Tool.LineNumber = r_TextDrawingLineNumber;

	Tool.AddMethod = UTAM_ADD_OR_ADJUST;

	if(DisplayInFillSpace.GetYesNo())
		Tool.BeginDateTime = -3;
	else
		Tool.BeginDateTime = HorizontalPosition.GetInt();

	Tool.BeginValue = (float)VerticalPosition.GetInt();
	Tool.UseRelativeVerticalValues = true;
	Tool.Color = TextColor.GetColor();
	Tool.FontFace = sc.ChartTextFont;
	Tool.FontBackColor = TextBackColor.GetColor();
	Tool.FontSize = TextSize.GetInt();
	Tool.FontBold = true;
	Tool.ReverseTextColor = false;
	Tool.MultiLineLabel = true;
	Tool.TextAlignment = DT_LEFT;

	ProfitLossDisplayFormatEnum ProfitLossDisplayFormat = (ProfitLossDisplayFormatEnum)(ProfitLossFormat.GetIndex()+1);

	s_SCPositionData PositionData;
	sc.GetTradePosition(PositionData) ;

	double RoundTurnCommissionRate = sc.CurrencyValuePerTick == 0 ? 0 : RTCommission.GetFloat();

	SCString Text;

	s_ACSTradeStatistics AllStats;
	s_ACSTradeStatistics DailyStats;

	sc.GetTradeStatisticsForSymbol(0, false, AllStats);
	sc.GetTradeStatisticsForSymbol(0, true, DailyStats);

	if (ShowOpenPL.GetYesNo())
	{
		SCString PLString;
		double Commission = RoundTurnCommissionRate * PositionData.PositionQuantity / 2.0;
		sc.CreateProfitLossDisplayString(PositionData.OpenProfitLoss-Commission, PositionData.PositionQuantity, ProfitLossDisplayFormat, PLString);
		Text.Format("Open PL: %s", PLString.GetChars());
	}

	if (ShowClosedPL.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString PLString;
		double Commission = RoundTurnCommissionRate * AllStats.TotalClosedQuantity();
		sc.CreateProfitLossDisplayString(AllStats.ClosedProfitLoss()-Commission, AllStats.TotalClosedQuantity(), ProfitLossDisplayFormat, PLString);
		SCString ClosedPLText;
		ClosedPLText.Format("Closed PL: %s", PLString.GetChars());

		Text.Append(ClosedPLText);
	}

	if (ShowDailyPL.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		double DailyPL = DailyStats.ClosedProfitLoss();
		t_OrderQuantity32_64  DailyClosedQuantity = DailyStats.TotalClosedQuantity();
		double Commission = RoundTurnCommissionRate * DailyClosedQuantity;

		if (IncludeOpenPLInDailyPL.GetYesNo())
		{
			DailyPL += PositionData.OpenProfitLoss;
			t_OrderQuantity32_64 OpenQuantity = abs(PositionData.PositionQuantity);
			DailyClosedQuantity += OpenQuantity;
			Commission += (RoundTurnCommissionRate * OpenQuantity / 2.0);
			Text.Append("Daily Net PL: ");
		}
		else
			Text.Append("Daily PL: ");

		SCString PLString;
		sc.CreateProfitLossDisplayString(DailyPL-Commission, DailyClosedQuantity, ProfitLossDisplayFormat, PLString);

		Text.Append(PLString);
	}

	if (ShowTotalTrades.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString TotalTradesText;
		TotalTradesText.Format("Total Trades: %i", AllStats.TotalTrades());

		Text.Append(TotalTradesText);
	}

	if (ShowTotalQuantityFilled.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString TotalFilledText("Total Filled Qty: ");
		SCString QuantityString;
		sc.OrderQuantityToString(AllStats.TotalFilledQuantity, QuantityString);
		TotalFilledText += QuantityString;

		Text.Append(TotalFilledText);
	}

	if (ShowDailyTotalTrades.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString TotalTradesText;
		TotalTradesText.Format("Daily Trades: %i", DailyStats.TotalTrades());

		Text.Append(TotalTradesText);
	}

	if (ShowDailyTotalQuantityFilled.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString TotalFilledText("Daily Filled Qty: ");
		SCString QuantityString;
		sc.OrderQuantityToString(DailyStats.TotalFilledQuantity, QuantityString);
		TotalFilledText += QuantityString;

		Text.Append(TotalFilledText);
	}

	if (ShowMaximumOpenPositionProfit.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString PLString;
		sc.CreateProfitLossDisplayString(PositionData.MaximumOpenPositionProfit, PositionData.PositionQuantity, ProfitLossDisplayFormat, PLString);
		Text.AppendFormat("Max Open Profit: %s", PLString.GetChars());
	}

	if (ShowMaximumOpenPositionLoss.GetYesNo())
	{
		if (Text != "")
			Text.Append("\n");

		SCString PLString;
		sc.CreateProfitLossDisplayString(PositionData.MaximumOpenPositionLoss, PositionData.PositionQuantity, ProfitLossDisplayFormat, PLString);
		Text.AppendFormat("Max Open Loss: %s", PLString.GetChars());
	}

	Tool.Text = Text;

	sc.UseTool(Tool);

	r_TextDrawingLineNumber = Tool.LineNumber;
}


/*==========================================================================*/
SCSFExport scsf_UpDownVolumeRatio(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpDownVolumeRatio = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];

	SCInputRef MovingAverageLength = sc.Input[0];
	SCInputRef VolumeOrTrades = sc.Input[1];
	SCInputRef MovingAverageType = sc.Input [2];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Up/Down Volume Ratio";
		sc.MaintainAdditionalChartDataArrays = 1;

		sc.GraphRegion =1;

		UpDownVolumeRatio.Name = "VR";
		UpDownVolumeRatio.DrawStyle = DRAWSTYLE_BAR;
		UpDownVolumeRatio.PrimaryColor = RGB(0, 255, 0);
		UpDownVolumeRatio.SecondaryColor = RGB(255, 0, 0);
		UpDownVolumeRatio.LineWidth = 3;
		UpDownVolumeRatio.AutoColoring = AUTOCOLOR_POSNEG;
		UpDownVolumeRatio.DrawZeros = false;  // false

		ZeroLine.Name = "Zero";
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.PrimaryColor = RGB(128, 128,128);
		ZeroLine.LineWidth = 2;
		ZeroLine.DrawZeros = true;  // true

		MovingAverageLength.Name = "Moving Average Length";
		MovingAverageLength.SetInt(14);
		MovingAverageLength.SetIntLimits(1, INT_MAX);

		VolumeOrTrades.Name = "Calculation Based On";
		VolumeOrTrades.SetCustomInputStrings("Up/Down Volume;Ask/Bid Volume;Up/Down Trades");
		VolumeOrTrades.SetCustomInputIndex(0);

		MovingAverageType.Name = "Moving Average Type";
		MovingAverageType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		
		

		sc.AutoLoop = 1;

		return;
	}
	
	
	// Do data processing
	int Up = SC_UPVOL;
	int Down = SC_DOWNVOL;

	if(VolumeOrTrades.GetIndex() == 0)
	{	
		Up = SC_UPVOL;
		Down = SC_DOWNVOL;
	}
	else if(VolumeOrTrades.GetIndex() == 1)
	{	
		Up = SC_ASKVOL;
		Down = SC_BIDVOL;
	}
	else
	{
		Up = SC_ASKNT;
		Down = SC_BIDNT;
	}

	float TotalTrades = sc.BaseData[Up][sc.Index] + sc.BaseData[Down][sc.Index];
	
	if (TotalTrades > 0)
		UpDownVolumeRatio.Arrays[0][sc.Index] = 100 * (sc.BaseData[Up][sc.Index] - sc.BaseData[Down][sc.Index]) / TotalTrades;
	else
		UpDownVolumeRatio.Arrays[0][sc.Index] = 0.0f;
	
	sc.MovingAverage(UpDownVolumeRatio.Arrays[0], UpDownVolumeRatio, MovingAverageType.GetMovAvgType(), MovingAverageLength.GetInt());
}

/*==========================================================================*/

SCSFExport scsf_MovAvgCross(SCStudyInterfaceRef sc)
{ 
	SCInputRef MA1Price = sc.Input[0];
	SCInputRef MA1Type = sc.Input[1];
	SCInputRef MA1Length = sc.Input[2];
	SCInputRef MA2Price = sc.Input[3];
	SCInputRef MA2Type = sc.Input[4];
	SCInputRef MA2Length = sc.Input[5];
	SCInputRef DotsSpacing = sc.Input[6];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Moving Average Crossover";

		sc.AutoLoop = 1;

		sc.GraphRegion = 0;

		sc.Subgraph[0].Name = "Moving Average 1";
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[0].PrimaryColor = RGB(0,255,0);

		sc.Subgraph[1].Name = "Moving Average 2";
		sc.Subgraph[1].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[1].PrimaryColor = RGB(255,0,255);

		sc.Subgraph[2].Name = "Buy/Sell";
		sc.Subgraph[2].SecondaryColorUsed = 1;
		sc.Subgraph[2].PrimaryColor = RGB(0,255,0);
		sc.Subgraph[2].SecondaryColor = RGB(255,0,0);
		sc.Subgraph[2].DrawStyle = DRAWSTYLE_POINT;
		sc.Subgraph[2].LineWidth = 4;

		MA1Price.Name = "Moving Average 1 Input Data";
		MA1Price.SetInputDataIndex(SC_LAST);
		MA1Type.Name = "MA1.Type";
		MA1Type.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		MA1Length.Name = "MA1.Length";
		MA1Length.SetInt(5);
		MA1Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		MA2Price.Name = "Moving Average 2 Input Data";
		MA2Price.SetInputDataIndex(SC_OPEN);
		MA2Type.Name = "MA2.Type";
		MA2Type.SetMovAvgType(MOVAVGTYPE_WEIGHTED);
		MA2Length.Name = "MA2.Length";
		MA2Length.SetInt(6);
		MA2Length.SetIntLimits(1,MAX_STUDY_LENGTH);


		return;
	}

	// Do data processing
	sc.MovingAverage(sc.BaseData[MA1Price.GetInputDataIndex()], sc.Subgraph[0],MA1Type.GetMovAvgType(),MA1Length.GetInt());

	sc.MovingAverage(sc.BaseData[MA2Price.GetInputDataIndex()], sc.Subgraph[1],MA2Type.GetMovAvgType(),MA2Length.GetInt());

	int direction = sc.CrossOver(sc.Subgraph[0],sc.Subgraph[1]);
	if (direction == CROSS_FROM_TOP)
	{
		sc.Subgraph[2][sc.Index] = sc.BaseData[SC_HIGH][sc.Index];
		sc.Subgraph[2].DataColor[sc.Index] = sc.Subgraph[2].SecondaryColor;
	}
	else if (direction == CROSS_FROM_BOTTOM)
	{	
		sc.Subgraph[2][sc.Index] = sc.BaseData[SC_LOW][sc.Index];
		sc.Subgraph[2].DataColor[sc.Index] = sc.Subgraph[2].PrimaryColor;
	}
	else
	{
		sc.Subgraph[2][sc.Index] = 0;
	}
}


/*==========================================================================*/
SCSFExport scsf_BillWilliamsAC(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Up = sc.Subgraph[0];
	SCSubgraphRef Down = sc.Subgraph[1];
	SCSubgraphRef Flat = sc.Subgraph[2];
	SCSubgraphRef All = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Sum1 = sc.Subgraph[5];
	SCSubgraphRef Avg1 = sc.Subgraph[6];
	SCSubgraphRef Sum2 = sc.Subgraph[7];
	SCSubgraphRef Avg2 = sc.Subgraph[8];
	SCSubgraphRef Sum4 = sc.Subgraph[9];
	SCSubgraphRef Avg4 = sc.Subgraph[10];

	SCInputRef LongMaLength = sc.Input[0];
	SCInputRef ShortMaLength = sc.Input[1];
	SCInputRef SignalMaLength = sc.Input[2];
	SCInputRef MaType = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bill Williams AC";

		sc.StudyDescription = "";

		

		Up.Name = "Up";
		Up.DrawStyle = DRAWSTYLE_DASH;
		Up.PrimaryColor = RGB( 0, 255, 0);
		Up.DrawZeros = false;

		Down.Name = "Down";
		Down.DrawStyle = DRAWSTYLE_DASH;
		Down.PrimaryColor = RGB( 255, 0, 255);
		Down.DrawZeros = false;

		Flat.Name = "Flat";
		Flat.DrawStyle = DRAWSTYLE_DASH;
		Flat.PrimaryColor = RGB( 192, 192, 192);
		Flat.DrawZeros = false;

		All.Name = "All";
		All.DrawStyle = DRAWSTYLE_IGNORE;
		All.PrimaryColor = RGB( 255, 127, 0);
		All.DrawZeros = false;

		LongMaLength.Name = "Long Moving Average Length";
		LongMaLength.SetInt(34);
		LongMaLength.SetIntLimits(1, INT_MAX);

		ShortMaLength.Name = "Short Moving Average Length";
		ShortMaLength.SetInt(5);
		ShortMaLength.SetIntLimits(1, INT_MAX);

		SignalMaLength.Name = "Signal Moving Average Length";
		SignalMaLength.SetInt(5);
		SignalMaLength.SetIntLimits(1, INT_MAX);

		MaType.Name = "Moving Average Type";
		MaType.SetCustomInputStrings("Bill Williams Smoothed;Exponential;Simple");
		MaType.SetCustomInputIndex(0);


		sc.AutoLoop = 1;

		return;
	}

	// Do data processing

	const float cfactor1 = 2.0f / (LongMaLength.GetInt() + 1);		const float cinvfactor1 = 1.0f - cfactor1;
	const float cfactor2 = 2.0f / (ShortMaLength.GetInt() + 1);	const float cinvfactor2 = 1.0f - cfactor2;
	const float cfactor4 = 2.0f / (SignalMaLength.GetInt() + 1);	const float cinvfactor4 = 1.0f - cfactor4;

	sc.DataStartIndex = 50;

	int pos = sc.Index;
	if (pos < 50)
		return;

	int j;
	float avg3, avg4;

	switch (MaType.GetInt())
	{
	case 0:
		{
			Sum1[pos] = Sum1[pos - 1] + sc.Close[pos];
			if (pos >= 50 + LongMaLength.GetInt())
				Sum1[pos] = Sum1[pos] - Avg1[pos - 1];
			Avg1[pos] = Sum1[pos] / LongMaLength.GetInt();

			Sum2[pos] = Sum2[pos - 1] + sc.Close[pos];
			if (pos >= 50 + ShortMaLength.GetInt())
				Sum2[pos] = Sum2[pos] - Avg2[pos - 1];
			Avg2[pos] = Sum2[pos] / ShortMaLength.GetInt();

			avg3 = Avg2[pos] - Avg1[pos];

			Temp4.Data[pos] = avg3;
			Sum4[pos] = Sum4[pos - 1] + avg3;
			avg4 = Sum4[pos] / SignalMaLength.GetInt();
			j = pos - SignalMaLength.GetInt() + 1;
			if (pos > 50 + SignalMaLength.GetInt() - 2)
				Sum4[pos] = Sum4[pos] - Temp4.Data[j];
		}
		break;

	case 1:
		{
			Avg1[pos] = Avg1[pos - 1] * cinvfactor1 + sc.Close[pos] * cfactor1;
			Avg2[pos] = Avg2[pos - 1] * cinvfactor2 + sc.Close[pos] * cfactor2;
			avg3 = Avg2[pos] - Avg1[pos];
			//Temp4.Data[pos] = avg3;
			avg4 = Avg4[pos] = Avg4[pos - 1] * cinvfactor4 + avg3 * cfactor4;
		}
		break;

	case 2:
	default:
		{
			Sum1[pos] = Sum1[pos - 1] + sc.Close[pos];
			float avg1 = Sum1[pos] / LongMaLength.GetInt();
			j = pos - LongMaLength.GetInt() + 1;
			if (pos > 50 + LongMaLength.GetInt() - 2)
				Sum1[pos] = Sum1[pos] - sc.Close[j];

			Sum2[pos] = Sum2[pos - 1] + sc.Close[pos];
			float avg2 = Sum2[pos] / ShortMaLength.GetInt();
			j = pos - ShortMaLength.GetInt() + 1;
			if (pos > 50 + ShortMaLength.GetInt() - 2)
				Sum2[pos] = Sum2[pos] - sc.Close[j];

			avg3 = avg2 - avg1;

			Temp4.Data[pos] = avg3;
			Sum4[pos] = Sum4[pos - 1] + avg3;
			avg4 = Sum4[pos] / SignalMaLength.GetInt();
			j = pos - SignalMaLength.GetInt() + 1;
			if (pos > 50 + SignalMaLength.GetInt() - 2)
				Sum4[pos] = Sum4[pos] - Temp4.Data[j];
		}
		break;
	}


	All.Data[pos] = avg3 - avg4;

	if (All.Data[pos] > All.Data[pos - 1])
	{
		Up.Data[pos] = All.Data[pos];
		Down.Data[pos] = 0;
		Flat.Data[pos] = 0;
	}
	else if(All.Data[pos] < All.Data[pos - 1])
	{
		Up.Data[pos] = 0;
		Down.Data[pos] = All.Data[pos];
		Flat.Data[pos] = 0;
	}
	else
	{
		Up.Data[pos] = 0;
		Down.Data[pos] = 0;
		Flat.Data[pos] = All.Data[pos];
	}
}

/*==========================================================================*/
SCSFExport scsf_BillWilliamsAO(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AO = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MA1Length = sc.Input[2];
	SCInputRef MA2Length = sc.Input[3];
	SCInputRef MAType = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bill Williams Awesome Oscillator";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		AO.Name = "AO";
		AO.DrawStyle = DRAWSTYLE_BAR;
		AO.LineWidth = 2;
		AO.SecondaryColor = RGB(255,0,0);
		AO.AutoColoring = AUTOCOLOR_SLOPE;
		AO.DrawZeros = false;

		ZeroLine.Name ="Zero Line";
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.LineWidth =1;
		ZeroLine.PrimaryColor = RGB(255,0,255);
		ZeroLine.DrawZeros = true;

		InputData.SetInputDataIndex(SC_HL_AVG);

		MA1Length.Name = "Moving Average 1 Length";
		MA1Length.SetInt(34);
		MA1Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA2Length.Name = "Moving Average 2 Length";
		MA2Length.SetInt(5);
		MA2Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);


		return;
	}

	sc.DataStartIndex = max(MA1Length.GetInt(), MA2Length.GetInt());



	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], AO.Arrays[0], MAType.GetMovAvgType(), MA1Length.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], AO.Arrays[1], MAType.GetMovAvgType(), MA2Length.GetInt());

	AO.Data[sc.Index] = AO.Arrays[1][sc.Index] - AO.Arrays[0][sc.Index];

}

/*==========================================================================*/
SCSFExport scsf_BillWilliamsMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MovAvg = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MovingAverageLength = sc.Input[2];
	SCInputRef MovingAverageType = sc.Input[4];
	SCInputRef Version = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bill Williams Moving Average";

		sc.StudyDescription = "To offset the moving average, use the Displacement setting for the MovAvg Subgraph.";
		sc.GraphRegion= 0;

		MovAvg.Name = "MovAvg";
		MovAvg.DrawStyle = DRAWSTYLE_LINE;
		MovAvg.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MovingAverageLength.Name = "MovAvg Length";
		MovingAverageLength.SetInt(34);
		MovingAverageLength.SetIntLimits(1, INT_MAX);

		MovingAverageType.Name = "Moving Average Type";
		MovingAverageType.SetCustomInputStrings(";Bill Willams EMA;Smoothed Moving Average");
		MovingAverageType.SetCustomInputIndex(2);

		Version.SetInt(1);

		sc.AutoLoop = 1;

		return;
	}

	// Do data processing

	sc.DataStartIndex = 0;

	if (Version.GetInt() < 1)
	{
		InputData.SetInputDataIndex(SC_LAST);
		Version.SetInt(1);
	}

	if (MovingAverageType.GetIndex() == 0)
		MovingAverageType.SetCustomInputIndex(1);

	uint32_t InputIndex = InputData.GetInputDataIndex();

	if (MovingAverageType.GetIndex() == 1)
	{
		if (sc.Index == 0)
		{
			MovAvg[0] = sc.BaseData[InputIndex][0];
			MovAvg.Arrays[4][sc.Index] = sc.BaseData[InputIndex][0]*MovingAverageLength.GetInt();
		}
		else
		{
			MovAvg.Arrays[4][sc.Index] = MovAvg.Arrays[4][sc.Index - 1] + sc.BaseData[InputIndex][sc.Index];
			MovAvg.Arrays[4][sc.Index] -=  MovAvg[sc.Index - 1];
			MovAvg[sc.Index] = MovAvg.Arrays[4][sc.Index] / MovingAverageLength.GetInt();
		}
	}
	else
	{
		sc.SmoothedMovingAverage(sc.BaseData[InputIndex], MovAvg, sc.Index, MovingAverageLength.GetInt());
	}
}

/*==========================================================================*/
SCSFExport scsf_BillWilliamsAlligator(SCStudyInterfaceRef sc)
{
	const int jaw = 0;
	const int teeth = 1;
	const int lips = 2;

	SCSubgraphRef Jaw = sc.Subgraph[0];
	SCSubgraphRef Teeth = sc.Subgraph[1];
	SCSubgraphRef Lips = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef JawLength = sc.Input[1];
	SCInputRef TeethLength = sc.Input[3];
	SCInputRef LipsLength = sc.Input[5];
	SCInputRef VersionUpdate1 = sc.Input[7];
	SCInputRef MovAvgType = sc.Input[8];
	SCInputRef VersionUpdate2 = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bill Williams Alligator";

		sc.StudyDescription = "";

		// Set the region to draw the graph in.  Region zero is the main
		// price graph region.
		sc.GraphRegion = 0;
		

		// Set the name of the first subgraph
		// Set the color and style of the graph line.  If these are not set
		// the default will be used.
		Jaw.Name = "Jaw";		
		Jaw.PrimaryColor = RGB(0,0,255);  // Blue
		Jaw.DrawStyle = DRAWSTYLE_LINE;
		Jaw.GraphicalDisplacement = 8;
		Jaw.DrawZeros = false;

		// Set the color and style of the graph line.  If these are not set
		// the default will be used.
		Teeth.Name = "Teeth";
		Teeth.PrimaryColor = RGB(255,0,0);  // Red
		Teeth.DrawStyle = DRAWSTYLE_LINE;
		Teeth.GraphicalDisplacement = 5;
		Teeth.DrawZeros = false;

		// Set the color and style of the graph line.  If these are not set
		// the default will be used.
		Lips.Name = "Lips";		
		Lips.PrimaryColor = RGB(0, 255, 0);  // Red
		Lips.DrawStyle = DRAWSTYLE_LINE;
		Lips.GraphicalDisplacement = 3;
		Lips.DrawZeros = false;

		sc.AutoLoop = 1;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HL_AVG);

		JawLength.Name = "Jaw Length";
		JawLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		JawLength.SetInt(13);

		TeethLength.Name = "Teeth Length";
		TeethLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		TeethLength.SetInt(8);

		LipsLength.Name = "Lips Length";
		LipsLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		LipsLength.SetInt(5);

		VersionUpdate1.SetYesNo(true); //Used For Settings Updating

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SMOOTHED);

		VersionUpdate2.SetYesNo(true);
		return;
	}

	// data processing

	if (VersionUpdate2.GetYesNo() == false)
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SMOOTHED);

	if (VersionUpdate1.GetYesNo() == false)
	{
		Jaw.GraphicalDisplacement = 8;
		Teeth.GraphicalDisplacement = 5;
		Lips.GraphicalDisplacement = 3;
	}

	// calculate jaw, teeth and lips

	sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()], Jaw, MovAvgType.GetMovAvgType(), JawLength.GetInt());
	sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()], Teeth, MovAvgType.GetMovAvgType(), TeethLength.GetInt());
	sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()], Lips, MovAvgType.GetMovAvgType(), LipsLength.GetInt());
}



/*==========================================================================*/

SCSFExport scsf_AverageOfAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCSubgraphRef Temp1 = sc.Subgraph[1];

	SCInputRef Length = sc.Input[0];
	SCInputRef EmaLength = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Average of Average";
		
		sc.StudyDescription = "";

		sc.AutoLoop = 1;  // true
		
		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,255,0);
		
		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, INT_MAX);
		
		EmaLength.Name = "ExpMA Length";
		EmaLength.SetInt(10);
		EmaLength.SetIntLimits(1, INT_MAX);
		
		return;
	}
	
	
	// Do data processing
	
	
	/* This calculates the Exponential Moving Average of a Linear Regression Indicator
		by passing the output array from the Linear Regression Indicator function to the
		Input data parameter of Exponential Moving Average function.*/
	
	sc.LinearRegressionIndicator(sc.Close, Temp1, sc.Index, Length.GetInt());
	sc.ExponentialMovAvg(Temp1, Avg, sc.Index, EmaLength.GetInt());
}



/*==========================================================================*/
SCSFExport scsf_WoodiesEMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef EMA = sc.Subgraph[0];
	SCSubgraphRef UpDownColors = sc.Subgraph[1];
	SCSubgraphRef EMAOutput = sc.Subgraph[2];

	SCInputRef NumberOfBars = sc.Input[0];
	SCInputRef PriceChange = sc.Input[1];
	SCInputRef EMALength = sc.Input[2];
	SCInputRef FixedValue = sc.Input[4];
	SCInputRef InputData = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName="Woodies EMA";

		sc.AutoLoop = 1;

		sc.GraphRegion = 0;

		EMA.Name = "EMA";
		EMA.DrawStyle = DRAWSTYLE_LINE;
		EMA.PrimaryColor = RGB(0,255,0);
		EMA.DrawZeros = false;

		UpDownColors.Name = "Up / Down Color";
		UpDownColors.DrawStyle = DRAWSTYLE_IGNORE;
		UpDownColors.PrimaryColor = RGB(0,255,0);
		UpDownColors.SecondaryColor = RGB(255,0,0);
		UpDownColors.SecondaryColorUsed = 1;
		UpDownColors.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HLC_AVG);

		NumberOfBars.Name = "Number of Bars for Angle";
		NumberOfBars.SetInt(4);
		NumberOfBars.SetIntLimits(1,MAX_STUDY_LENGTH);

		PriceChange.Name = "Price Change for Angle";
		PriceChange.SetFloat(0.000001f);


		EMALength.Name = "EMA Length";
		EMALength.SetInt(34);
		EMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FixedValue.Name = "Fixed Value to Use";
		FixedValue.SetFloat(0);

		return;
	}


	sc.DataStartIndex = EMALength.GetInt() + NumberOfBars.GetInt();

	sc.ExponentialMovAvg(sc.BaseData[InputData.GetInputDataIndex()], EMAOutput, EMALength.GetInt());

	if (sc.Index < NumberOfBars.GetInt())
		return;

	int BarIndex = sc.Index;

	if(FixedValue.GetFloat() == 0.0f) 
		EMA[BarIndex] = EMAOutput[BarIndex];
	else 
		EMA[BarIndex] = FixedValue.GetFloat();

	if (EMAOutput[BarIndex] - EMAOutput[BarIndex-NumberOfBars.GetInt()] >= PriceChange.GetFloat())
	{
		EMA.DataColor[BarIndex] = UpDownColors.PrimaryColor;  
	}
	else if (EMAOutput[BarIndex] - EMAOutput[BarIndex-NumberOfBars.GetInt()] <= -PriceChange.GetFloat())
	{
		EMA.DataColor[BarIndex] = UpDownColors.SecondaryColor; 
	}
	else
	{
		EMA.DataColor[BarIndex] = EMA.PrimaryColor;
	}

}

/*==========================================================================*/
SCSFExport scsf_WoodieCCITrend(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Above = sc.Subgraph[0];
	SCSubgraphRef Below = sc.Subgraph[1];
	SCSubgraphRef AboveColorValues = sc.Subgraph[2];
	SCSubgraphRef DoNotDraw1 = sc.Subgraph[3];
	SCSubgraphRef DoNotDraw2 = sc.Subgraph[4];
	SCSubgraphRef CCI = sc.Subgraph[5];

	SCInputRef WarningBar = sc.Input[0];
	SCInputRef AlternateColor = sc.Input[1];
	SCInputRef CCILength = sc.Input[2];
	SCInputRef NumberOfBars = sc.Input[3];
	SCInputRef BarsIntertrendState = sc.Input[4];
	SCInputRef BarsFixPos = sc.Input[5];

	if(sc.SetDefaults)
	{
		sc.GraphName="Woodies CCI Trend - Old";

		sc.AutoLoop = 0;

		Above.Name = "Above";
		Above.DrawStyle = DRAWSTYLE_BAR;
		Above.PrimaryColor = RGB(0,0,255);
		Above.DrawZeros = false;

		Below.Name = "Below";
		Below.DrawStyle = DRAWSTYLE_BAR;
		Below.PrimaryColor = RGB(255,0,0);
		Below.DrawZeros = false;

		AboveColorValues.Name = "Color Value Output";
		AboveColorValues.PrimaryColor = RGB(255, 255, 255);
		AboveColorValues.DrawStyle = DRAWSTYLE_IGNORE;
		AboveColorValues.DrawZeros = false;

		DoNotDraw1.DrawStyle = DRAWSTYLE_HIDDEN;
		DoNotDraw1.PrimaryColor = RGB(255,255,0);
		DoNotDraw1.DrawZeros = false;

		DoNotDraw2.DrawStyle = DRAWSTYLE_HIDDEN;
		DoNotDraw2.PrimaryColor = RGB(98,98,98);
		DoNotDraw2.DrawZeros = false;

		CCI.Name = "CCI";
		CCI.DrawStyle = DRAWSTYLE_IGNORE;
		CCI.PrimaryColor = RGB(0,0,255);
		CCI.DrawZeros = false;


		WarningBar.Name = "Use a Warning Bar";
		WarningBar.SetYesNo(false);		

		AlternateColor.Name = "Alternate Colour for uncertain trend states 0, 1, 2 or 4";
		AlternateColor.SetInt(0);
		AlternateColor.SetIntLimits(0,4);

		CCILength.Name = "CCI Length";
		CCILength.SetInt(14);
		CCILength.SetIntLimits(1,MAX_STUDY_LENGTH);

		NumberOfBars.Name = "Number of Bars";
		NumberOfBars.SetInt(6);
		NumberOfBars.SetIntLimits(1,MAX_STUDY_LENGTH);

		BarsIntertrendState.Name = "# of Bars for Intertrend State";
		BarsIntertrendState.SetInt(1);
		BarsIntertrendState.SetIntLimits(1,MAX_STUDY_LENGTH);

		BarsFixPos.Name = "0 = Bars or n = Fixed Position (n/1000)";
		BarsFixPos.SetFloat(0);	

		return;
	}
	

	AboveColorValues.DrawStyle = DRAWSTYLE_IGNORE;
	DoNotDraw1.DrawStyle = DRAWSTYLE_HIDDEN;
	DoNotDraw2.DrawStyle = DRAWSTYLE_HIDDEN;

	COLORREF Color0 = Above.PrimaryColor;
	COLORREF Color1 = Below.PrimaryColor;
	COLORREF Color2 = DoNotDraw1.PrimaryColor;
	COLORREF Color3 = DoNotDraw2.PrimaryColor;

	int ArraySize = sc.ArraySize;

	switch (AlternateColor.GetInt())
	{
	case 4:
	case 2:
		DoNotDraw1.Name ="Change on Next Bar";
		DoNotDraw2.Name ="Trend & Position Confused";
		break;
	case 1:
		DoNotDraw1.Name = "Trend Up but below Zero";
		DoNotDraw2.Name = "Trend Dn but above Zero";
		break;
	case 0:
	default:
		DoNotDraw1.Name = "Change on Next Bar";
	}

	int ud = -2, goingpos = 0, goingneg = 0, posbar = 0, negbar = 0, up = 0, 
		dn = 0, next = 0;


	for(int i = sc.UpdateStartIndex; i < ArraySize;i++)
	{
		sc.CCI(sc.HLCAvg, CCI.Arrays[0], CCI, i, CCILength.GetInt(), 0.015f);
	}

	int Start = max (sc.UpdateStartIndex, CCILength.GetInt()*2 - 1);

	float cci0;
	for (int i = Start-1; i >= CCILength.GetInt()*2; i--)
	{
		cci0 = sc.Subgraph[5][i];
		if (cci0 > 0)
		{
			dn = 0;
			up++;
		}

		else if (cci0 < 0)
		{
			up = 0;
			dn++;
		}

		if(up >= NumberOfBars.GetInt()) 
		{
			Start = i + NumberOfBars.GetInt();
			ud=2;
			break;
		}		
		else if(dn >= NumberOfBars.GetInt()) 
		{
			ud=-2;
			Start = i + NumberOfBars.GetInt();
			break;
		}
	}

	sc.DataStartIndex = CCILength.GetInt()*2 - 1;

	for (int pos=Start; pos < ArraySize; pos++)
	{
		cci0 = CCI[pos];

		if (cci0>0) 
		{
			if(AlternateColor.GetInt() == 4 && BarsIntertrendState.GetInt() <= up+1) 
				dn = 0;

			if(ud <= 0)
			{
				if (goingpos == 0)
				{
					goingpos = 1;
					up = 1;
					dn = 0; 
					negbar = 0; 
					if(AlternateColor.GetInt() == 4 && BarsIntertrendState.GetInt() == 1) 
						ud=0;
				}
				else
				{
					up++; 
					if(up == NumberOfBars.GetInt()) 
						dn = 0;

					if(AlternateColor.GetInt() == 4 && BarsIntertrendState.GetInt() <= up) 
						ud = 0;
				}
			} 
			else	if(ud >= 2)
			{
				if (goingneg == 1) 
				{
					posbar++; 
					if(posbar >= 2) 
					{
						goingneg = 0; 
						dn = 0;
					} 
				}
				else
					up++;
			} 
		}

		if (cci0<0) 
		{
			if(AlternateColor.GetInt()==4 && BarsIntertrendState.GetInt()<=dn+1)
				up=0;

			if(ud>= 0) 
			{
				if (goingneg==0)
				{
					goingneg=1;
					dn=1; 
					up=0; 
					posbar=0; 
					if(AlternateColor.GetInt()==4 && BarsIntertrendState.GetInt()==1) 
						ud=0;
				}
				else
				{
					dn++; 
					if(dn==NumberOfBars.GetInt()) 
						up=0;

					if(AlternateColor.GetInt()==4 && BarsIntertrendState.GetInt()<=dn) 
						ud=0;
				}
			} 
			else	if(ud <= -2) 
			{
				if (goingpos == 1)
				{
					negbar++; 
					if(negbar>=2) 
					{
						goingpos=0; 
						up=0;
					} 
				}
				else
					dn++;
			} 
		};


		next=0;
		if(up==(NumberOfBars.GetInt()-1) && (ud<=-2 || (ud==0 && AlternateColor.GetInt()==4)) && WarningBar.GetYesNo()!=0) 
			next=1; 

		else	if(dn==(NumberOfBars.GetInt()-1) && (ud>= 2 || (ud==0 && AlternateColor.GetInt()==4)) && WarningBar.GetYesNo()!=0) 
			next=1;

		if(up>=NumberOfBars.GetInt()) 
			ud=2;

		if(dn>=NumberOfBars.GetInt()) 
			ud=-2;

		if (BarsFixPos.GetFloat())
			Above[pos] = BarsFixPos.GetFloat()/1000;
		else
			Above[pos] = cci0; 

		if (next==1)
		{
			if(AlternateColor.GetInt()==0 || AlternateColor.GetInt()==2 || AlternateColor.GetInt()==4)
			{
				Above.DataColor[pos] = Color2;
			} 
			else	if(AlternateColor.GetInt()==1)
			{ 
				if(BarsFixPos.GetFloat()==0) 
				{ 
					Above.DataColor[pos] = Color0; 
					Below[pos]=cci0/2; 
				};
			};
		} 
		else
		{
			if (ud==0 && AlternateColor.GetInt()==4)
			{ 
				Above.DataColor[pos] = Color3;
			} 
			else if (ud>=2)
			{
				if(AlternateColor.GetInt()==0 || cci0>=0)
				{ 
					Above.DataColor[pos] = Color0;
				} 
				else	if(AlternateColor.GetInt()==1)
				{ 
					Above.DataColor[pos] = Color2;
				} 
				else if(AlternateColor.GetInt()==2 || AlternateColor.GetInt()==4)
				{ 
					Above.DataColor[pos] = Color3;
				}
			} 
			else if (ud<=-2)
			{
				if(AlternateColor.GetInt()==0 || cci0<=0)
				{ 
					Above.DataColor[pos] = Color1;
				} 
				else
				{
					Above.DataColor[pos] = Color3;
				}
			}
		}

		if (Above.DataColor[pos] == Color0)
		{
			AboveColorValues[pos] = 1;
		}
		else if (Above.DataColor[pos] == Color1)
		{
			AboveColorValues[pos] = 2;
		}
		else if (Above.DataColor[pos] == Color2)
		{
			AboveColorValues[pos] = 3;
		}
		else if (Above.DataColor[pos] == Color3)
		{
			AboveColorValues[pos] = 4;
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_Sidewinder(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SwTop = sc.Subgraph[0];
	SCSubgraphRef SwBottom = sc.Subgraph[1];
	SCSubgraphRef FlatColor = sc.Subgraph[2];
	SCSubgraphRef OutWorksheet = sc.Subgraph[3];
	SCSubgraphRef Lsma = sc.Subgraph[4];
	SCSubgraphRef LsmaAngle = sc.Subgraph[5];
	SCSubgraphRef Ema = sc.Subgraph[6];
	SCSubgraphRef EmaAngle = sc.Subgraph[7];

	SCInputRef LsmaLength = sc.Input[0];
	SCInputRef EmaLength = sc.Input[1];
	SCInputRef TrendingThreshold = sc.Input[3];
	SCInputRef NormalThreshold = sc.Input[4];
	SCInputRef LineValue = sc.Input[5];
	SCInputRef TickValueOverride = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName="Sidewinder";
		sc.StudyDescription="Sidewinder";

		sc.AutoLoop = 0;

		SwTop.Name = "SW Top";
		SwTop.DrawStyle = DRAWSTYLE_LINE;
		SwTop.LineWidth = 2;
		SwTop.PrimaryColor = RGB(255,255,0);
		SwTop.SecondaryColor = RGB(0,255,0);
		SwTop.SecondaryColorUsed = 1;  // true
		SwTop.DrawZeros = false;

		SwBottom.Name = "SW Bottom";
		SwBottom.DrawStyle = DRAWSTYLE_LINE;
		SwBottom.LineWidth = 2;
		SwBottom.PrimaryColor = RGB(255,255,0);
		SwBottom.SecondaryColor = RGB(0,255,0);
		SwBottom.SecondaryColorUsed = 1;  // true
		SwBottom.DrawZeros = false;

		FlatColor.Name = "Flat Color";
		FlatColor.DrawStyle = DRAWSTYLE_IGNORE;
		FlatColor.PrimaryColor = RGB(255,0,0);
		FlatColor.DrawZeros = false;

		OutWorksheet.Name = "Output for Spreadsheets";
		OutWorksheet.DrawStyle = DRAWSTYLE_IGNORE;
		OutWorksheet.PrimaryColor = RGB(255,255,255);
		OutWorksheet.DrawZeros = false;

		Lsma.DrawStyle = DRAWSTYLE_IGNORE;
		Lsma.DrawZeros = false;

		LsmaAngle.DrawStyle = DRAWSTYLE_IGNORE;
		LsmaAngle.DrawZeros = false;

		Ema.DrawStyle = DRAWSTYLE_IGNORE;
		Ema.DrawZeros = false;

		EmaAngle.DrawStyle = DRAWSTYLE_IGNORE;
		EmaAngle.DrawZeros = false;

		LsmaLength.Name = "LSMA Length";
		LsmaLength.SetInt(25);
		LsmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		EmaLength.Name = "EMA Length";
		EmaLength.SetInt(34);
		EmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		TrendingThreshold.Name = "Trending Threshold";
		TrendingThreshold.SetFloat(100.0f);

		NormalThreshold.Name = "Normal Threshold";
		NormalThreshold.SetFloat(30.0f);

		LineValue.Name = "Line Value";
		LineValue.SetFloat(200.0f);

		TickValueOverride.Name = "Tick Value (Enter number to override default)";
		TickValueOverride.SetFloat(0.0f);

		sc.AutoLoop = 1;

		return;
	}

	OutWorksheet.DrawStyle = DRAWSTYLE_IGNORE;
	OutWorksheet.PrimaryColor = RGB(255,255,255);

	const float RAD_TO_DEG = (180.0f/3.14159265358979f);

	// Colors
	COLORREF colorFlat = FlatColor.PrimaryColor;
	COLORREF colorNormalUp = SwTop.PrimaryColor;
	COLORREF colorTrendingUp = SwTop.SecondaryColor;
	COLORREF colorNormalDown = SwBottom.PrimaryColor;
	COLORREF colorTrendingDown = SwBottom.SecondaryColor;

	if (TickValueOverride.GetFloat() == 1)
		TickValueOverride.SetFloat(0);

	float inTickSize = sc.TickSize;

	if(TickValueOverride.GetFloat() != 0)
		inTickSize = TickValueOverride.GetFloat();

	float &PriorTickSize = sc.GetPersistentFloat(1);

	if(PriorTickSize != inTickSize)
	{
		sc.UpdateStartIndex = 0;
		PriorTickSize = inTickSize;
	}


	float InverseTickSize = 1/inTickSize;


	sc.DataStartIndex = LsmaLength.GetInt() - 1;

	// Determine the input array element to begin calculation at
	int Pos = sc.Index;
	if (Pos < 2)
		return;

	// Calculate the EMA
	sc.ExponentialMovAvg(sc.Close, Ema, Pos, EmaLength.GetInt());

	// Calculate the LSMA
	sc.LinearRegressionIndicator(sc.Close,Lsma,Pos, LsmaLength.GetInt());

	// Calculate the LSMA angle
	float lsma_Angle = (float)sc.SlopeToAngleInDegrees((Lsma[Pos]-((Lsma[Pos-1]+Lsma[Pos-2])/2.0f))*InverseTickSize);
	LsmaAngle[Pos] = lsma_Angle;

	// Calculate the EMA angle
	float ema_Angle	= (float)sc.SlopeToAngleInDegrees((Ema[Pos]-((Ema[Pos-1]+Ema[Pos-2])/2.0f))*InverseTickSize);

	EmaAngle[Pos] = ema_Angle;

	// Calculate SW
	COLORREF color;
	float worksheetValue;

	if (fabs(EmaAngle[Pos]) >= 15.0f
		&& (EmaAngle[Pos] + LsmaAngle[Pos]) >= TrendingThreshold.GetFloat())
	{
		color = colorTrendingUp; 
		worksheetValue = 2;
	}
	else if (fabs(EmaAngle[Pos]) >= 15.0f
		&& (EmaAngle[Pos] + LsmaAngle[Pos]) <= -TrendingThreshold.GetFloat())
	{
		color = colorTrendingDown; 
		worksheetValue = -2;
	}
	else if (EmaAngle[Pos] > 5.0f && LsmaAngle[Pos] > 0.0f
		&& (EmaAngle[Pos] + LsmaAngle[Pos]) >= NormalThreshold.GetFloat())
	{
		color = colorNormalUp;
		worksheetValue = 1;
	}
	else if (EmaAngle[Pos] < 5.0f && LsmaAngle[Pos] < 0.0f
		&& (EmaAngle[Pos] + LsmaAngle[Pos]) <= -NormalThreshold.GetFloat())
	{
		color = colorNormalDown;
		worksheetValue = -1;
	}
	else
	{
		color = colorFlat;
		worksheetValue = 0;
	}

	SwTop[Pos] = LineValue.GetFloat();
	SwBottom[Pos] = -LineValue.GetFloat();

	SwTop.DataColor[Pos] = color;
	SwBottom.DataColor[Pos] = color;

	OutWorksheet[Pos] = worksheetValue;
}


/*==========================================================================*/
SCSFExport scsf_LSMAAboveBelowCCI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LsmaCCI = sc.Subgraph[0];
	SCSubgraphRef DoNotDraw1 = sc.Subgraph[1];
	SCSubgraphRef DoNotDraw2 = sc.Subgraph[2];
	SCSubgraphRef DoNotDraw3 = sc.Subgraph[3];
	SCSubgraphRef DoNotDraw4 = sc.Subgraph[4];

	SCInputRef LsmaLength = sc.Input[0];
	SCInputRef CCILength = sc.Input[2];
	SCInputRef BarsPointsLsma = sc.Input[3];
	

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName ="LSMA Above/Below CCI";
		sc.GraphRegion = 1;	

		LsmaCCI.DrawStyle = DRAWSTYLE_BAR;
		LsmaCCI.Name = "LSMA Above/Below CCI";
		LsmaCCI.PrimaryColor = RGB(0, 255, 0);
		LsmaCCI.SecondaryColorUsed = 1;
		LsmaCCI.SecondaryColor = RGB(255, 0, 255);
		LsmaCCI.DrawZeros = false;

		DoNotDraw4.DrawStyle = DRAWSTYLE_IGNORE;
		DoNotDraw4.Name = "Spreadsheet Output";
		DoNotDraw4.PrimaryColor = RGB(255,0,0);
		DoNotDraw4.DrawZeros = false;

		LsmaLength.Name = "MA Length";
		LsmaLength.SetInt(25);
		LsmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		CCILength.Name = "CCI Length";
		CCILength.SetInt(14);
		CCILength.SetIntLimits(1,MAX_STUDY_LENGTH);

		BarsPointsLsma.Name = "Bars (0) or Points (n/1000) or LSMA (9999)";
		BarsPointsLsma.SetInt(0);
		BarsPointsLsma.SetIntLimits(0, 9999);

		//MovAvgType.Name = "Moving Average Type";
		//MovAvgType.SetMovAvgType(MOVAVGTYPE_LINEARREGRESSION);

		sc.AutoLoop = 1;

		return;
	}

	COLORREF color0 = LsmaCCI.PrimaryColor;
	COLORREF color1 = LsmaCCI.SecondaryColor;

	sc.DataStartIndex = max(LsmaLength.GetInt(), CCILength.GetInt());

	int MAType = MOVAVGTYPE_LINEARREGRESSION;


	sc.MovingAverage(sc.Close, DoNotDraw1, MAType,sc.Index, LsmaLength.GetInt());
	sc.CCI(sc.HLCAvg, DoNotDraw2, DoNotDraw3, sc.Index, CCILength.GetInt(), 0.015f);

	float fLSMA = DoNotDraw1[sc.Index];
	float fCCI = DoNotDraw3[sc.Index];

	if (BarsPointsLsma.GetInt() == 0) 
		LsmaCCI[sc.Index] = fCCI; 
	else if (BarsPointsLsma.GetInt() == 9999) 
		LsmaCCI[sc.Index] = fLSMA; 
	else
		LsmaCCI[sc.Index] = (float)BarsPointsLsma.GetInt() / 1000.0f;

	if (sc.Close[sc.Index] > fLSMA)
	{
		LsmaCCI.DataColor[sc.Index] = color0;
		DoNotDraw4[sc.Index] = 1;
	}
	else if (sc.Close[sc.Index] < fLSMA)
	{
		LsmaCCI.DataColor[sc.Index] = color1;
		DoNotDraw4[sc.Index] = -1;
	}
}

/*==========================================================================*/
SCSFExport scsf_LSMAAboveBelow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Lsma = sc.Subgraph[0];
	SCSubgraphRef DoNotDraw1 = sc.Subgraph[1];
	SCSubgraphRef WorksheetOutput = sc.Subgraph[2];

	SCInputRef LsmaLength = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName ="LSMA Above/Below Last";
		sc.GraphRegion = 1;	

		Lsma.DrawStyle = DRAWSTYLE_SQUARE;
		Lsma.LineWidth = 2;
		Lsma.Name = "Above/Below";
		Lsma.PrimaryColor = RGB(0, 255, 0);
		Lsma.SecondaryColorUsed = 1;
		Lsma.SecondaryColor = RGB(255, 0, 0);
		Lsma.DrawZeros = true;

		WorksheetOutput.Name = "Spreadsheet Output";
		WorksheetOutput.DrawStyle = DRAWSTYLE_IGNORE;
		WorksheetOutput.PrimaryColor = RGB(255,255,0);
		WorksheetOutput.DrawZeros = true;

		LsmaLength.Name = "MovAvg Length";
		LsmaLength.SetInt(25);
		LsmaLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		return;
	}

	COLORREF color0 = Lsma.PrimaryColor;
	COLORREF color1 = Lsma.SecondaryColor;

	sc.DataStartIndex = LsmaLength.GetInt();

	int index = sc.Index;

	sc.MovingAverage(sc.Close, DoNotDraw1, MOVAVGTYPE_LINEARREGRESSION, index, LsmaLength.GetInt());

	float fLSMA = DoNotDraw1[index];

	if (sc.Close[index] > fLSMA)
	{
		Lsma.DataColor[index] = color0;
		WorksheetOutput[index] = 1;

	}
	else if (sc.Close[index] < fLSMA)
	{
		Lsma.DataColor[index] = color1;
		WorksheetOutput[index] = -1;

	}
	else 
	{
		Lsma.DataColor[index] = 0;
		WorksheetOutput[index] = 0;
	}
}
/*==========================================================================*/
SCSFExport scsf_EMAAboveBelow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ExponentialMovingAverage = sc.Subgraph[0];
	SCSubgraphRef DoNotDraw1 = sc.Subgraph[1];
	SCSubgraphRef WorksheetOutput = sc.Subgraph[2];

	SCInputRef MovingAverageLength = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName ="EMA Above/Below Last";
		sc.GraphRegion = 1;

		ExponentialMovingAverage.DrawStyle = DRAWSTYLE_SQUARE;
		ExponentialMovingAverage.LineWidth = 2;
		ExponentialMovingAverage.Name = "Above/Below";
		ExponentialMovingAverage.PrimaryColor = RGB(0, 255, 0);
		ExponentialMovingAverage.SecondaryColorUsed = 1;
		ExponentialMovingAverage.SecondaryColor = RGB(255, 0, 0);
		ExponentialMovingAverage.DrawZeros = true;

		WorksheetOutput.Name = "Spreadsheet Output";
		WorksheetOutput.DrawStyle= DRAWSTYLE_IGNORE;
		WorksheetOutput.PrimaryColor = RGB(255, 255, 0);
		WorksheetOutput.DrawZeros = true;

		MovingAverageLength.Name = "Moving Average Length";
		MovingAverageLength.SetInt(25);
		MovingAverageLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		return;
	}

	COLORREF color0 = ExponentialMovingAverage.PrimaryColor;
	COLORREF color1 = ExponentialMovingAverage.SecondaryColor;

	sc.DataStartIndex = MovingAverageLength.GetInt();

	sc.MovingAverage(sc.Close, DoNotDraw1, MOVAVGTYPE_EXPONENTIAL, sc.Index, MovingAverageLength.GetInt());

	if (sc.Close[sc.Index] > DoNotDraw1[sc.Index])
	{
		ExponentialMovingAverage.DataColor[sc.Index] = color0;
		WorksheetOutput[sc.Index] = 1;

	}
	else if (sc.Close[sc.Index] < DoNotDraw1[sc.Index])
	{
		ExponentialMovingAverage.DataColor[sc.Index] = color1;
		WorksheetOutput[sc.Index] = -1;

	}
	else 
	{
		ExponentialMovingAverage.DataColor[sc.Index] = 0;
		WorksheetOutput[sc.Index] = 0;
	}
}

/*==========================================================================*/
SCSFExport scsf_MurrayMath(SCStudyInterfaceRef sc)
{
	const int NumberOfVisibleLines = 25;

	SCInputRef SquareWidth = sc.Input[0];
	SCInputRef UseLastBarAsEndDateTime = sc.Input[1];
	SCInputRef EndDateTime = sc.Input[2];
	SCInputRef SquareWidthNew = sc.Input[3];
	SCInputRef Version = sc.Input[4];
	SCInputRef SquareWidthMultiplier = sc.Input[5];
	SCInputRef RoundToTickSize = sc.Input[6];
	SCInputRef InputCustomSquareWidth = sc.Input[7];


	// Configuration
	if (sc.SetDefaults)
	{   
		sc.GraphName = "Murray Math"; 
		sc.AutoLoop = 0;  
		sc.GraphRegion = 0; 
		sc.ScaleRangeType = SCALE_SAMEASREGION;

		for (int SubgraphIndex = 0; SubgraphIndex < NumberOfVisibleLines; SubgraphIndex++)
		{
			SCString SubgraphString;

			if(SubgraphIndex <= 16)
				SubgraphString.Format("%d/8", SubgraphIndex-8);
			else
				SubgraphString.Format("+%d/8", SubgraphIndex-16);

			sc.Subgraph[SubgraphIndex].Name = SubgraphString;
			sc.Subgraph[SubgraphIndex].DrawStyle = DRAWSTYLE_DASH;
			sc.Subgraph[SubgraphIndex].LineWidth = 1;


			sc.Subgraph[SubgraphIndex].PrimaryColor = RGB(0, 255, 0);
			sc.Subgraph[SubgraphIndex].LineLabel |= LL_NAME_ALIGN_LEFT | LL_DISPLAY_NAME | LL_NAME_ALIGN_CENTER;

		}

		//SquareWidth.Name = "Square Width";
		SquareWidth.SetInt(64);  
		SquareWidth.SetIntLimits(1, MAX_STUDY_LENGTH);

		SquareWidthNew.Name = "Square Width";
		SquareWidthNew.SetCustomInputStrings("4;8;16;32;64;128;256;512");  
		SquareWidthNew.SetCustomInputIndex(0);

		UseLastBarAsEndDateTime.Name = "Use Last Bar as End Date-Time";
		UseLastBarAsEndDateTime.SetYesNo(true);

		EndDateTime.Name = "End Date-Time";
		EndDateTime.SetDateTime(0);

		Version.SetInt(1);

		SquareWidthMultiplier.Name = "Square Width Multiplier";
		SquareWidthMultiplier.SetFloat(1.5);
		SquareWidthMultiplier.SetFloatLimits(1.0, 100);

		RoundToTickSize.Name = "Round to Tick Size";
		RoundToTickSize.SetYesNo(false);

		InputCustomSquareWidth.Name = "Custom Square Width";
		InputCustomSquareWidth.SetInt(0);
		InputCustomSquareWidth.SetIntLimits(0, MAX_STUDY_LENGTH);

		return;
	}   


	if(Version.GetInt() < 1)
	{
		Version.SetInt(1);
		int OldSquareWidth = SquareWidth.GetInt();

		switch(OldSquareWidth)
		{
			case 4:
				SquareWidthNew.SetCustomInputIndex(0);
			break;

			case 8:
				SquareWidthNew.SetCustomInputIndex(1);
			break;

			case 16:
				SquareWidthNew.SetCustomInputIndex(2);
			break;

			case 32:
				SquareWidthNew.SetCustomInputIndex(3);
			break;

			default:
			case 64:
				SquareWidthNew.SetCustomInputIndex(4);
			break;

			case 128:
				SquareWidthNew.SetCustomInputIndex(5);
			break;

			case 256:
				SquareWidthNew.SetCustomInputIndex(6);
			break;

			case 512:
				SquareWidthNew.SetCustomInputIndex(7);
			break;
		}
	}

	int SquareWidthNumber = atoi(SquareWidthNew.GetSelectedCustomString());

	if (InputCustomSquareWidth.GetInt() > 0)
		SquareWidthNumber = InputCustomSquareWidth.GetInt();

	int BarsBackToReference = sc.Round(SquareWidthNumber * SquareWidthMultiplier.GetFloat());

	int &PreviousStartOutputBarIndex = sc.GetPersistentInt(1);

	//Clear previously used elements
	for(int BarIndex = PreviousStartOutputBarIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		for (int LineIndex = 0; LineIndex < NumberOfVisibleLines; LineIndex++)
		{
			sc.Subgraph[LineIndex][BarIndex] = 0;
		}
	}

	//Determine the starting index
	if (UseLastBarAsEndDateTime.GetYesNo())
		PreviousStartOutputBarIndex = sc.ArraySize - BarsBackToReference;
	else
	{
		PreviousStartOutputBarIndex = sc.GetNearestMatchForSCDateTime(sc.ChartNumber, EndDateTime.GetDateTime()) -  BarsBackToReference + 1;
		
	}
	
	double HighestHighOfSquareWidthLength = (double) sc.GetHighest(sc.High, PreviousStartOutputBarIndex + BarsBackToReference - 1, BarsBackToReference);

	double LowestLowOfSquareWidthLength = (double) sc.GetLowest(sc.Low, PreviousStartOutputBarIndex + BarsBackToReference - 1, BarsBackToReference);

	double PriceRange = HighestHighOfSquareWidthLength - LowestLowOfSquareWidthLength;
	double SR = 0.0, OctaveCount = 0.0, Si = 0.0, M = 0.0, B = 0.0, TT = 0.0;
	
	int I = 0;


	if(HighestHighOfSquareWidthLength > 25)
	{
		if(log(0.4*HighestHighOfSquareWidthLength)/log(10.0)-int(log(0.4*HighestHighOfSquareWidthLength)/log(10.0))>0)
			SR = exp(log(10.0)*(int(log(0.4*HighestHighOfSquareWidthLength)/log(10.0))+1));
		else
			SR = exp(log(10.0)*(int(log(0.4*HighestHighOfSquareWidthLength)/log(10.0))));
	}
	else 
	{
		SR = 100*exp(log(8.0)*(int(log(0.005*HighestHighOfSquareWidthLength)/log(8.0))));
	}

	double RangeMMI =  log(SR/PriceRange)/log(8.0);
	if(RangeMMI<=0)
	{
		OctaveCount = 0;
	}
	else
	{
		if(log(SR/PriceRange)/log(8.0)- fmod(log(SR/PriceRange),log(8.0))== 0)
		{
			OctaveCount = int(RangeMMI);
		}
		else
		{
			OctaveCount = int(RangeMMI)+1.0;
		}
	}


	Si = SR *exp(-OctaveCount * log(8.0));
	M = int(((1/log(2.0))*log(PriceRange/Si))+.00001);

	double ValueToRound = (((HighestHighOfSquareWidthLength + LowestLowOfSquareWidthLength) * .5)/(Si* exp((M-1)*log(2.0))));
	I = sc.Round((float)ValueToRound);

	B =(I-1) * Si * exp((M-1)* log(2.0));
	TT =(I+1) * Si * exp((M-1) * log(2.0));

	for (int LineIndex = 0; LineIndex < NumberOfVisibleLines; LineIndex++)
	{
		for(int BarIndex = PreviousStartOutputBarIndex; BarIndex < sc.ArraySize; BarIndex++)
		{
			float MurreyMathValue = (float)(B + ((LineIndex-8) * 0.125 * (TT -B)));

			if (RoundToTickSize.GetYesNo())
				MurreyMathValue = sc.RoundToTickSize(MurreyMathValue);

			sc.Subgraph[LineIndex][BarIndex] = MurreyMathValue;
		}
	}
}




/*==========================================================================*/
SCSFExport scsf_PreviousCloseLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PreviousCloseLine = sc.Subgraph[0];
	SCInputRef UsePreviousCloseFromQuoteData = sc.Input[0];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Previous Close Line";

		PreviousCloseLine.Name = "PCL";
		PreviousCloseLine.DrawStyle = DRAWSTYLE_LINE;
		
		PreviousCloseLine.LineStyle = LINESTYLE_DOT;
		PreviousCloseLine.PrimaryColor = RGB(0, 255, 0);
		PreviousCloseLine.LineWidth = 2;
		PreviousCloseLine.DrawZeros = false;

		UsePreviousCloseFromQuoteData.Name = "Use Previous Close From Quote Data";
		UsePreviousCloseFromQuoteData.SetYesNo(0);

		sc.GraphRegion = 0;

		
		return;
	}

	SCDateTime  PriorDayCloseDateTime = sc.GetStartDateTimeForTradingDate(sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.ArraySize - 1]) );

	PriorDayCloseDateTime -= 1 * SECONDS;

	int PriorDayCloseDateTimeIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, PriorDayCloseDateTime);

	float PriorDayCloseValue = 0.0;
	if (UsePreviousCloseFromQuoteData.GetYesNo())
		PriorDayCloseValue = sc.PreviousClose;
	else
		PriorDayCloseValue = sc.BaseData[SC_LAST][PriorDayCloseDateTimeIndex];

	for (int BarIndex = sc.UpdateStartIndex ; BarIndex < sc.ArraySize; ++BarIndex)
	{
		if (BarIndex < PriorDayCloseDateTimeIndex)
			continue;

		PreviousCloseLine[BarIndex] = PriorDayCloseValue;
	}

}

/*==========================================================================*/
SCSFExport scsf_ValueChartLevels(SCStudyInterfaceRef sc)
{
	SCInputRef ModOBLevel = sc.Input[1];
	SCInputRef SigOBLevel = sc.Input[2];

	SCSubgraphRef PlusModerateOverboughtLevel = sc.Subgraph[0];
	SCSubgraphRef PlusSignificantOverboughtLevel= sc.Subgraph[1];
	SCSubgraphRef  ZeroLevel= sc.Subgraph[2];
	SCSubgraphRef MinusModerateOverboughtLevel= sc.Subgraph[3];
	SCSubgraphRef MinusSignificantOverboughtLevel= sc.Subgraph[4];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName ="Value Chart Levels";

		sc.GraphDrawType = GDT_CUSTOM;
		sc.AutoLoop = 1;
		



		ModOBLevel.Name = "Moderately Overbought Level";
		ModOBLevel.SetFloat(4.0f);
		ModOBLevel.SetFloatLimits(0,100.0f);

		SigOBLevel.Name = "Significantly Overbought Level";
		SigOBLevel.SetFloat(8.0f);
		SigOBLevel.SetFloatLimits(0,100.0f);

		PlusModerateOverboughtLevel.Name = "+ Moderate Overbought Level";
		PlusModerateOverboughtLevel.PrimaryColor = RGB(0,255,0);
		PlusModerateOverboughtLevel.DrawStyle = DRAWSTYLE_LINE;
		PlusModerateOverboughtLevel.DisplayNameValueInWindowsFlags= 0;
		
		PlusSignificantOverboughtLevel.Name = "+ Significant Overbought Level";
		PlusSignificantOverboughtLevel.PrimaryColor = RGB(255,0,0);
		PlusSignificantOverboughtLevel.DrawStyle = DRAWSTYLE_LINE;
		PlusSignificantOverboughtLevel.DisplayNameValueInWindowsFlags= 0;
		
		MinusModerateOverboughtLevel.Name = "- Moderate Overbought Level";
		MinusModerateOverboughtLevel.PrimaryColor = RGB(0,255,0);
		MinusModerateOverboughtLevel.DrawStyle = DRAWSTYLE_LINE;
		MinusModerateOverboughtLevel.DisplayNameValueInWindowsFlags= 0;

		MinusSignificantOverboughtLevel.Name = "- Significant Overbought Level";
		MinusSignificantOverboughtLevel.PrimaryColor = RGB(255,0,0);
		MinusSignificantOverboughtLevel.DrawStyle = DRAWSTYLE_LINE;
		MinusSignificantOverboughtLevel.DisplayNameValueInWindowsFlags= 0;

		ZeroLevel.Name = "Zero Level";
		ZeroLevel.PrimaryColor = RGB(128,128,128);
		ZeroLevel.DrawStyle = DRAWSTYLE_LINE;
		ZeroLevel.DisplayNameValueInWindowsFlags= 0;
		ZeroLevel.DrawZeros = 1;
		

	}

	PlusModerateOverboughtLevel[sc.Index] = ModOBLevel.GetFloat();
	PlusSignificantOverboughtLevel[sc.Index] = SigOBLevel.GetFloat();

	MinusModerateOverboughtLevel[sc.Index] = ModOBLevel.GetFloat() * -1.0f;
	MinusSignificantOverboughtLevel[sc.Index] = SigOBLevel.GetFloat() * -1.0f;


}

/*============================================================================*/
SCSFExport scsf_ValueChart(SCStudyInterfaceRef sc)
{
	SCInputRef Length     = sc.Input[0];

	SCSubgraphRef VcOpen  = sc.Subgraph[0];
	SCSubgraphRef VcHigh  = sc.Subgraph[1];
	SCSubgraphRef VcLow   = sc.Subgraph[2];
	SCSubgraphRef VcClose = sc.Subgraph[3];

	SCFloatArrayRef VcHLMA = VcOpen.Arrays[1];
	SCFloatArrayRef TR     = VcOpen.Arrays[2];
	SCFloatArrayRef ATR    = VcOpen.Arrays[3];
	SCFloatArrayRef HH     = VcOpen.Arrays[4];
	SCFloatArrayRef LL     = VcOpen.Arrays[5];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName ="Value Chart";

		sc.GraphDrawType = GDT_CANDLESTICK;
		sc.AutoLoop = 1;
		

		// Inputs
		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(2,10000);

		// Subgraphs
		VcOpen.Name = "VC Open";
		VcOpen.DrawStyle = DRAWSTYLE_LINE;
		VcOpen.PrimaryColor = RGB(0,255,0);
		VcOpen.SecondaryColorUsed = true;
		VcOpen.SecondaryColor = RGB(0,255,0);
		VcOpen.DrawZeros = 1;

		VcHigh.Name = "VC High";
		VcHigh.DrawStyle = DRAWSTYLE_LINE;
		VcHigh.PrimaryColor = RGB(128,255,128);
		VcHigh.DrawZeros = 1;

		VcLow.Name = "VC Low";
		VcLow.DrawStyle = DRAWSTYLE_LINE;
		VcLow.PrimaryColor = RGB(255,0,0);
		VcLow.SecondaryColor = RGB(255,0,0);
		VcLow.SecondaryColorUsed = true;
		VcLow.DrawZeros = 1;

		VcClose.Name = "VC Close";
		VcClose.DrawStyle = DRAWSTYLE_LINE;
		VcClose.PrimaryColor = RGB(255,128,128);
		VcClose.DrawZeros = 1;

		return;
	}

	
	
	int NumBars = Length.GetInt();
	float LRange = 1;

	if (NumBars > 7)
	{
		int VarNumBars = sc.Round((float)NumBars / 5.0f);

		sc.Highest(sc.High, HH, VarNumBars);
		sc.Lowest(sc.Low, LL, VarNumBars);

		float R1 = HH[sc.Index] - LL[sc.Index];
		if (R1 == 0 && VarNumBars == 1)
			R1 = abs(sc.Close[sc.Index] - sc.Close[sc.Index-VarNumBars]);

		float R2 = HH[sc.Index-VarNumBars-1] - LL[sc.Index-VarNumBars];
		if (R2 == 0 && VarNumBars == 1)
			R2 = abs(sc.Close[sc.Index-VarNumBars] - sc.Close[sc.Index-(VarNumBars*2)]);

		float R3 = HH[sc.Index-(VarNumBars*2)] - LL[sc.Index-(VarNumBars*2)];
		if (R3 == 0 && VarNumBars == 1)
			R3 = abs(sc.Close[sc.Index-(VarNumBars*2)] - sc.Close[sc.Index-(VarNumBars*3)]);

		float R4 = HH[sc.Index-(VarNumBars*3)] - LL[sc.Index-(VarNumBars*3)];
		if (R4 == 0 && VarNumBars == 1)
			R4 = abs(sc.Close[sc.Index-(VarNumBars*3)] - sc.Close[sc.Index-(VarNumBars*4)]);

		float R5 = HH[sc.Index-(VarNumBars*4)] - LL[sc.Index-(VarNumBars*4)];
		if (R5 == 0 && VarNumBars == 1)
			R5 = abs(sc.Close[sc.Index-(VarNumBars*4)] - sc.Close[sc.Index-(VarNumBars*5)]);

		LRange = (R1 + R2 + R3 + R4 + R5) / 5 / 5;
	}
	else
	{
		sc.ATR(sc.BaseDataIn, TR, ATR, 5, MOVAVGTYPE_SIMPLE);
		LRange = ATR[sc.Index] / 5;
	}


	sc.MovingAverage(sc.HLAvg, VcHLMA, MOVAVGTYPE_SIMPLE, Length.GetInt());
	float AvgVcHL = VcHLMA[sc.Index];

	VcOpen[sc.Index]  = (sc.Open[sc.Index]  - AvgVcHL) / LRange;
	VcHigh[sc.Index]  = (sc.High[sc.Index]  - AvgVcHL) / LRange;
	VcLow[sc.Index]   = (sc.Low[sc.Index]   - AvgVcHL) / LRange;
	VcClose[sc.Index] = (sc.Close[sc.Index] - AvgVcHL) / LRange;
}
