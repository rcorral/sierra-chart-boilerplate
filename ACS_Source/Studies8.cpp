#include  <iterator>
#include <vector>

#include "sierrachart.h"
#include "scstudyfunctions.h"


/*==========================================================================*/
SCSFExport scsf_Ergodic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Erg = sc.Subgraph[0];
	SCSubgraphRef ErgSignalLine = sc.Subgraph[1];
	SCSubgraphRef ErgOsc = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Temp5 = sc.Subgraph[5];
	SCSubgraphRef Temp6 = sc.Subgraph[6];
	SCSubgraphRef Temp7 = sc.Subgraph[7];
	SCSubgraphRef Temp8 = sc.Subgraph[8];

	SCInputRef InputData          = sc.Input[0];
	SCInputRef MAType             = sc.Input[1];
	SCInputRef LongMALength       = sc.Input[3];
	SCInputRef ShortMALength      = sc.Input[4];
	SCInputRef Multiplier         = sc.Input[5];
	SCInputRef SignalLineMALength = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ergodic";

		sc.GraphRegion	= 1;
		sc.ValueFormat	= 2;
		sc.AutoLoop		= 1; 

		Erg.Name = "Erg";
		Erg.DrawStyle = DRAWSTYLE_LINE;
		Erg.PrimaryColor = RGB(0,255,0);
		Erg.DrawZeros = true;

		ErgSignalLine.Name = "Erg Signal Line";
		ErgSignalLine.DrawStyle = DRAWSTYLE_IGNORE;
		ErgSignalLine.PrimaryColor = RGB(255,0,255);
		ErgSignalLine.DrawZeros = true;

		ErgOsc.Name = "Erg Osc";
		ErgOsc.DrawStyle = DRAWSTYLE_IGNORE;
		ErgOsc.PrimaryColor = RGB(255,255,0);
		ErgOsc.DrawZeros = true;

		// inputs
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MAType.Name = "MovAvg Type";
		MAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		LongMALength.Name = "Long MovAvg Length";
		LongMALength.SetInt(15);
		LongMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		ShortMALength.Name = "Short MovAvg Length";
		ShortMALength.SetInt(5);
		ShortMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Multiplier.Name = "Multiplier";
		Multiplier.SetInt(1);

		SignalLineMALength.Name = "Signal Line MovAvg Length";
		SignalLineMALength.SetInt(10);
		SignalLineMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	// Subgraphs 3-7 are used for calculation only
	sc.DataStartIndex = ((LongMALength.GetInt() ) + (ShortMALength.GetInt()) + SignalLineMALength.GetInt()) ;

	if (sc.Index < 1) // not large enough
		return;

	int LongStart = sc.Index;
	if (LongStart < LongMALength.GetInt() - 1)  //2
		LongStart = LongMALength.GetInt() - 1;  //2

	int ShortStart = sc.Index;
	if (ShortStart < LongMALength.GetInt() - 1 + ShortMALength.GetInt() - 1)  //3
		ShortStart = LongMALength.GetInt() - 1 + ShortMALength.GetInt() - 1;  //3

	int SignalStart = sc.Index;
	if (SignalStart < ShortMALength.GetInt() - 1 + SignalLineMALength.GetInt() - 1+LongMALength.GetInt() -1)  //4
		SignalStart = ShortMALength.GetInt() - 1 + SignalLineMALength.GetInt() - 1 +LongMALength.GetInt() -1;  //4


	int DataStart = sc.Index;
	if (DataStart < 1)
		DataStart = 1;

	if(sc.Index >= DataStart)
		Temp3[sc.Index] = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index-1];

	// Calculate the Numerator into SubGraphData[0]
	if(sc.Index >= LongStart)
		sc.MovingAverage(Temp3, Temp4, MAType.GetMovAvgType(), LongMALength.GetInt());

	if(sc.Index >= ShortStart)
		sc.MovingAverage(Temp4, Temp5, MAType.GetMovAvgType(), ShortMALength.GetInt());

	// Calculate the Denominator into SubGraphData[1]
	if(sc.Index >= DataStart)
		Temp6[sc.Index] = fabs(Temp3[sc.Index]);

	if(sc.Index >= LongStart)
		sc.MovingAverage(Temp6, Temp7, MAType.GetMovAvgType(), LongMALength.GetInt());

	if(sc.Index >= ShortStart)
	{
		sc.MovingAverage(Temp7, Temp8, MAType.GetMovAvgType(), ShortMALength.GetInt());

		// Store the TSI (Numerator / Denominator)
		if (Temp8[sc.Index] != 0)
			Erg[sc.Index] = Multiplier.GetInt() * Temp5[sc.Index]/Temp8[sc.Index];
		else
			Erg[sc.Index] = 0;
	}

	// Calculate the Signal Line ( EMA[TSI] )
	if(sc.Index >= SignalStart)
		sc.MovingAverage(Erg, ErgSignalLine, MAType.GetMovAvgType(), SignalLineMALength.GetInt());

	// Calculate the Oscillator (TSI - EMA[TSI])
	if(sc.Index >= sc.DataStartIndex)
		ErgOsc[sc.Index] = Erg[sc.Index] - ErgSignalLine[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageHull(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HullMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef HullMALength = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Hull";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		HullMA.Name ="Hull MA";
		HullMA.DrawStyle = DRAWSTYLE_LINE;
		HullMA.PrimaryColor = RGB(0,255,0);
		HullMA.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		HullMALength.Name = "Moving Average Length";
		HullMALength.SetInt(25);
		HullMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	int SqrtLength = (int)(sc.Round(sqrt((float)HullMALength.GetInt())));
	sc.DataStartIndex = HullMALength.GetInt() + SqrtLength - 1;

	sc.HullMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], HullMA, HullMALength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_AverageOfTwoSubgraphs(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Average = sc.Subgraph[0];

	SCInputRef InputStudy1 = sc.Input[2];
	SCInputRef Study1Subgraph = sc.Input[3];
	SCInputRef InputStudy2 = sc.Input[4];
	SCInputRef Study2Subgraph = sc.Input[5];
	SCInputRef VersionCheckInput = sc.Input[6];
	SCInputRef StudySubgraph1 = sc.Input[7];
	SCInputRef StudySubgraph2 = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Average";

		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(0, 255,0 );
		Average.DrawZeros = false;

		InputStudy1.SetStudyID(0);
	
		InputStudy2.SetStudyID(0);


		Study1Subgraph.SetSubgraphIndex(0);
		Study2Subgraph.SetSubgraphIndex(0);

		StudySubgraph1.Name = "Study Subgraph Reference 1";
		StudySubgraph1.SetStudySubgraphValues(0,0);

		StudySubgraph2.Name = "Study Subgraph Reference 2";
		StudySubgraph2.SetStudySubgraphValues(0,0);

		VersionCheckInput.SetInt(2);

		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		return;
	}

	if (VersionCheckInput.GetInt() < 2)
	{


		StudySubgraph1.SetStudySubgraphValues(InputStudy1.GetStudyID(), StudySubgraph1.GetSubgraphIndex());
		StudySubgraph2.SetStudySubgraphValues(InputStudy2.GetStudyID(), StudySubgraph2.GetSubgraphIndex());


		VersionCheckInput.SetInt(2);
	}

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySubgraph1.GetStudyID(), StudySubgraph1.GetSubgraphIndex(), Study1Array);
	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySubgraph2.GetStudyID(), StudySubgraph2.GetSubgraphIndex(), Study2Array);

	Average[sc.Index] = (Study1Array[sc.Index] + Study2Array[sc.Index]) / 2.0f;
}



/*==========================================================================*/
SCSFExport scsf_NarrowRangeBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_NarrowRange = sc.Subgraph[0];


	SCInputRef Input_LookBackLength_NRn = sc.Input[0];
	SCInputRef Input_LookBackLength_xBarNR = sc.Input[1];
	SCInputRef Input_LookBackType = sc.Input[2];
	SCInputRef Input_UseOpenToCloseRange_NRn = sc.Input[3];
	SCInputRef Input_GroupSize = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Narrow Range Bar";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		int InputDisplayOrder = 0;

		Subgraph_NarrowRange.Name = "Narrow Range";
		Subgraph_NarrowRange.DrawStyle = DRAWSTYLE_COLOR_BAR;
		Subgraph_NarrowRange.PrimaryColor = RGB(0, 0, 255);
		Subgraph_NarrowRange.SecondaryColor = RGB(127, 0, 127);
		Subgraph_NarrowRange.DrawZeros = false;

		Input_LookBackType.Name = "Lookback Type";
		Input_LookBackType.SetCustomInputStrings
		("NR n;xBar NR");
		Input_LookBackType.SetCustomInputIndex(0);
		Input_LookBackType.DisplayOrder = ++InputDisplayOrder;

		Input_UseOpenToCloseRange_NRn.Name = "Use Open To Close Range for NR n Lookback Type";
		Input_UseOpenToCloseRange_NRn.SetYesNo(false);
		Input_UseOpenToCloseRange_NRn.DisplayOrder = ++InputDisplayOrder;

		Input_LookBackLength_NRn.Name = "Lookback Length - NR n";
		Input_LookBackLength_NRn.SetInt(5);
		Input_LookBackLength_NRn.SetIntLimits(0, MAX_STUDY_LENGTH);
		Input_LookBackLength_NRn.DisplayOrder = ++InputDisplayOrder;

		Input_LookBackLength_xBarNR.Name = "Lookback Length - xBar NR";
		Input_LookBackLength_xBarNR.SetInt(20);
		Input_LookBackLength_xBarNR.SetIntLimits(Input_GroupSize.GetInt(), MAX_STUDY_LENGTH);
		Input_LookBackLength_xBarNR.DisplayOrder = ++InputDisplayOrder;

		Input_GroupSize.Name = "Number of Bars in Comparison Group";
		Input_GroupSize.SetInt(3);
		Input_GroupSize.SetIntLimits(0, Input_LookBackLength_xBarNR.GetInt());
		Input_GroupSize.DisplayOrder = ++InputDisplayOrder;

		return;
	}

	const int SelectedIndex = Input_LookBackType.GetIndex();

	switch (Input_LookBackType.GetIndex())
	{
	case 0:
	{
		if (sc.Index < Input_LookBackLength_NRn.GetInt())
			return;

		float isLowest = 1.0f;

		/* Find range of current bar. */
		float RangeOfCurrentBar;

		if (Input_UseOpenToCloseRange_NRn.GetYesNo())
			RangeOfCurrentBar = fabs(sc.Open[sc.Index] - sc.Close[sc.Index]);
		else
			RangeOfCurrentBar = sc.High[sc.Index] - sc.Low[sc.Index];

		/* Find the minimum range in the lookback period. */
		int LookBackLength = Input_LookBackLength_NRn.GetInt();
		float PriorRange;

		for (int i = 1; i <= LookBackLength; i++)
		{
			if (Input_UseOpenToCloseRange_NRn.GetYesNo())
				PriorRange = fabs(sc.Open[sc.Index - i] - sc.Close[sc.Index - i]);
			else
				PriorRange = sc.High[sc.Index - i] - sc.Low[sc.Index - i];

			if (!sc.FormattedEvaluate(PriorRange, sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, RangeOfCurrentBar, sc.BaseGraphValueFormat))
			{
				isLowest = 0.0f;
				break;
			}
		}

		Subgraph_NarrowRange.DataColor[sc.Index] = Subgraph_NarrowRange.PrimaryColor;
		Subgraph_NarrowRange[sc.Index] = isLowest;
	}
	break;

	case 1:
	{
		if (sc.Index < Input_LookBackLength_xBarNR.GetInt())
			return;

		int GroupSize = Input_GroupSize.GetInt();
		float isLowest = 1.0f;

		/* Find the range of the current group of bars. */
		float RangeOfCurrentBarGroup;
		float CurrentHighestHigh = sc.High[sc.Index];
		float CurrentLowestLow = sc.Low[sc.Index];

		for (int i = 1; i < GroupSize; i++)
		{
			if (sc.High[sc.Index - i] > CurrentHighestHigh)
				CurrentHighestHigh = sc.High[sc.Index - i];

			if (sc.Low[sc.Index - i] < CurrentLowestLow)
				CurrentLowestLow = sc.Low[sc.Index - i];
		}

		RangeOfCurrentBarGroup = CurrentHighestHigh - CurrentLowestLow;

		/* Find the minimum x-bar range in the lookback period. */
		int LookBackLength = Input_LookBackLength_xBarNR.GetInt();
		float PriorLowestGroupRange = FLT_MAX;

		/* Find the group of x bars with the lowest range. */
		for (int i = 1; i <= LookBackLength - GroupSize + 1; i++)
		{
			float GroupHighestHigh = sc.High[sc.Index - i];
			float GroupLowestLow = sc.Low[sc.Index - i];
			float GroupRange;

			/* Find range of single group within lookback period. */
			for (int j = i + 1; j < i + GroupSize; j++)
			{
				if (sc.High[sc.Index - j] > GroupHighestHigh)
					GroupHighestHigh = sc.High[sc.Index - j];
				if (sc.Low[sc.Index - j] < GroupLowestLow)
					GroupLowestLow = sc.Low[sc.Index - j];
			}

			GroupRange = GroupHighestHigh - GroupLowestLow;

			if (GroupRange < PriorLowestGroupRange)
				PriorLowestGroupRange = GroupRange;

			if (!sc.FormattedEvaluate(PriorLowestGroupRange, sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, RangeOfCurrentBarGroup, sc.BaseGraphValueFormat))
			{
				isLowest = 0.0f;
				break;
			}
		}

		Subgraph_NarrowRange.DataColor[sc.Index] = Subgraph_NarrowRange.PrimaryColor;
		Subgraph_NarrowRange[sc.Index] = isLowest;

		if (isLowest == 1.0f)
		{
			for (int i = 1; i < GroupSize; i++)
			{
				Subgraph_NarrowRange.DataColor[sc.Index - i] = Subgraph_NarrowRange.SecondaryColor;
				Subgraph_NarrowRange[sc.Index - i] = isLowest;
			}

		}
	}
	break;
	}
}

/*==========================================================================*/
SCSFExport scsf_WideRangeBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_WideRange = sc.Subgraph[0];

	SCInputRef Input_LookBackLength_WRn = sc.Input[0];
	SCInputRef Input_UseOpenToCloseRange_WRn = sc.Input[1];
	SCInputRef Input_LookBackLength_xBarWR = sc.Input[2];
	SCInputRef Input_LookBackType = sc.Input[3];
	SCInputRef Input_GroupSize = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Wide Range Bar";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;
		int InputDisplayOrder = 0;

		Subgraph_WideRange.Name = "Wide Range";
		Subgraph_WideRange.DrawStyle = DRAWSTYLE_COLOR_BAR;
		Subgraph_WideRange.PrimaryColor = RGB(0, 0, 255);
		Subgraph_WideRange.SecondaryColor = RGB(127, 0, 127);
		Subgraph_WideRange.DrawZeros = false;

		Input_LookBackType.Name = "Lookback Type";
		Input_LookBackType.SetCustomInputStrings
		("WR n;xBar WR");
		Input_LookBackType.SetCustomInputIndex(0);
		Input_LookBackType.DisplayOrder = ++InputDisplayOrder;

		Input_UseOpenToCloseRange_WRn.Name = "Use Open To Close Range for WR n Lookback Type";
		Input_UseOpenToCloseRange_WRn.SetYesNo(false);
		Input_UseOpenToCloseRange_WRn.DisplayOrder = ++InputDisplayOrder;

		Input_LookBackLength_WRn.Name = "Lookback Length - WR n";
		Input_LookBackLength_WRn.SetInt(5);
		Input_LookBackLength_WRn.SetIntLimits(0, MAX_STUDY_LENGTH);
		Input_LookBackLength_WRn.DisplayOrder = ++InputDisplayOrder;

		Input_LookBackLength_xBarWR.Name = "Lookback Length - xBar WR";
		Input_LookBackLength_xBarWR.SetInt(20);
		Input_LookBackLength_xBarWR.SetIntLimits(Input_GroupSize.GetInt(), MAX_STUDY_LENGTH);
		Input_LookBackLength_xBarWR.DisplayOrder = ++InputDisplayOrder;

		Input_GroupSize.Name = "Number of Bars in Comparison Group";
		Input_GroupSize.SetInt(3);
		Input_GroupSize.SetIntLimits(0, Input_LookBackLength_xBarWR.GetInt());
		Input_GroupSize.DisplayOrder = ++InputDisplayOrder;

		return;
	}

	const int SelectedIndex = Input_LookBackType.GetIndex();

	switch (Input_LookBackType.GetIndex())
	{
	case 0:
	{
		if (sc.Index < Input_LookBackLength_WRn.GetInt())
			return;

		float isHighest = 1.0f;

		/* Find range of current bar. */
		float RangeOfCurrentBar;

		if (Input_UseOpenToCloseRange_WRn.GetYesNo())
			RangeOfCurrentBar = fabs(sc.Open[sc.Index] - sc.Close[sc.Index]);
		else
			RangeOfCurrentBar = sc.High[sc.Index] - sc.Low[sc.Index];

		/* Find the maximum range in the lookback period. */
		int LookBackLength = Input_LookBackLength_WRn.GetInt();
		float PriorRange;

		for (int i = 1; i <= LookBackLength; i++)
		{
			if (Input_UseOpenToCloseRange_WRn.GetYesNo())
				PriorRange = fabs(sc.Open[sc.Index - i] - sc.Close[sc.Index - i]);
			else
				PriorRange = sc.High[sc.Index - i] - sc.Low[sc.Index - i];

			if (!sc.FormattedEvaluate(PriorRange, sc.BaseGraphValueFormat, LESS_EQUAL_OPERATOR, RangeOfCurrentBar, sc.BaseGraphValueFormat))
			{
				isHighest = 0.0f;
				break;
			}
		}

		Subgraph_WideRange.DataColor[sc.Index] = Subgraph_WideRange.PrimaryColor;
		Subgraph_WideRange[sc.Index] = isHighest;
	}
	break;

	case 1:
	{
		if (sc.Index < Input_LookBackLength_xBarWR.GetInt())
			return;

		int GroupSize = Input_GroupSize.GetInt();
		float isHighest = 1.0f;

		/* Find the range of the current group of bars. */
		float RangeOfCurrentBarGroup;
		float CurrentHighestHigh = sc.High[sc.Index];
		float CurrentLowestLow = sc.Low[sc.Index];

		for (int i = 1; i < GroupSize; i++)
		{
			if (sc.High[sc.Index - i] > CurrentHighestHigh)
				CurrentHighestHigh = sc.High[sc.Index - i];

			if (sc.Low[sc.Index - i] < CurrentLowestLow)
				CurrentLowestLow = sc.Low[sc.Index - i];
		}

		RangeOfCurrentBarGroup = CurrentHighestHigh - CurrentLowestLow;

		/* Find the maximum x-bar range in the lookback period. */
		int LookBackLength = Input_LookBackLength_xBarWR.GetInt();
		float PriorHighestGroupRange = -1.0f*FLT_MAX;

		/* Find the group of x bars with the highest range. */
		for (int i = 1; i <= LookBackLength - GroupSize + 1; i++)
		{
			float GroupHighestHigh = sc.High[sc.Index - i];
			float GroupLowestLow = sc.Low[sc.Index - i];
			float GroupRange;

			/* Find range of single group within lookback period. */
			for (int j = i + 1; j < i + GroupSize; j++)
			{
				if (sc.High[sc.Index - j] > GroupHighestHigh)
					GroupHighestHigh = sc.High[sc.Index - j];
				if (sc.Low[sc.Index - j] < GroupLowestLow)
					GroupLowestLow = sc.Low[sc.Index - j];
			}

			GroupRange = GroupHighestHigh - GroupLowestLow;

			if (GroupRange > PriorHighestGroupRange)
				PriorHighestGroupRange = GroupRange;

			if (!sc.FormattedEvaluate(PriorHighestGroupRange, sc.BaseGraphValueFormat, LESS_EQUAL_OPERATOR, RangeOfCurrentBarGroup, sc.BaseGraphValueFormat))
			{
				isHighest = 0.0f;
				break;
			}
		}

		Subgraph_WideRange.DataColor[sc.Index] = Subgraph_WideRange.PrimaryColor;
		Subgraph_WideRange[sc.Index] = isHighest;

		if (isHighest == 1.0f)
		{
			for (int i = 1; i < GroupSize; i++)
			{
				Subgraph_WideRange.DataColor[sc.Index - i] = Subgraph_WideRange.SecondaryColor;
				Subgraph_WideRange[sc.Index - i] = isHighest;
			}

		}
	}
	break;
	}
}

/*==========================================================================*/
SCSFExport scsf_BidVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BidVol = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bid Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_BAR;
		BidVol.PrimaryColor = RGB(0,255,0);
		BidVol.DrawZeros = true;

		
		return;
	}

	BidVol[sc.Index] = sc.BidVolume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_AskVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AskVol = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ask Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_BAR;
		AskVol.PrimaryColor = RGB(0,255,0);
		AskVol.DrawZeros = true;

		return;
	}

	AskVol[sc.Index] = sc.AskVolume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_UpTickVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpTickVol = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "UpTick Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		UpTickVol.Name = "UpTick Volume";
		UpTickVol.DrawStyle = DRAWSTYLE_BAR;
		UpTickVol.PrimaryColor = RGB(0,255,0);
		UpTickVol.DrawZeros = true;

		return;
	}

	UpTickVol[sc.Index] = sc.UpTickVolume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_DownTickVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DownTickVol = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "DownTick Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		DownTickVol.Name = "DownTick Volume";
		DownTickVol.DrawStyle = DRAWSTYLE_BAR;
		DownTickVol.PrimaryColor = RGB(0,255,0);
		DownTickVol.DrawZeros = true;

		return;
	}

	DownTickVol[sc.Index] = sc.DownTickVolume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_MaximumMinimumUpTickVolumeDowntickVolumeDiff(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MinDifference = sc.Subgraph[0];
	SCSubgraphRef MaxDifference = sc.Subgraph[1];
	SCSubgraphRef MinBidAskVolumeDifference = sc.Subgraph[2];
	SCSubgraphRef MaxBidAskVolumeDifference = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Maximum/Minimum UpTick Volume Downtick Volume Difference";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		MinDifference.Name = "Minimum UpTick Volume Downtick Volume Difference";
		MinDifference.DrawStyle = DRAWSTYLE_BAR;
		MinDifference.PrimaryColor = RGB(0,255,0);
		MinDifference.DrawZeros = true;

		MaxDifference.Name = "Maximum UpTick Volume Downtick Volume Difference";
		MaxDifference.DrawStyle = DRAWSTYLE_BAR;
		MaxDifference.PrimaryColor = RGB(0,255, 255);
		MaxDifference.DrawZeros = true;

		MinBidAskVolumeDifference.Name = "Minimum Bid Ask Volume Difference";
		MinBidAskVolumeDifference.DrawStyle = DRAWSTYLE_BAR;
		MinBidAskVolumeDifference.PrimaryColor = RGB(255,255,0);
		MinBidAskVolumeDifference.DrawZeros = true;

		MaxBidAskVolumeDifference.Name = "Maximum Bid Ask Volume Difference";
		MaxBidAskVolumeDifference.DrawStyle = DRAWSTYLE_BAR;
		MaxBidAskVolumeDifference.PrimaryColor = RGB(255, 0, 0);
		MaxBidAskVolumeDifference.DrawZeros = true;

		return;
	}

	MinDifference[sc.Index] = sc.BaseDataIn[SC_UPDOWN_VOL_DIFF_LOW][sc.Index];
	MaxDifference[sc.Index] = sc.BaseDataIn[SC_UPDOWN_VOL_DIFF_HIGH][sc.Index];
	MinBidAskVolumeDifference[sc.Index] = sc.BaseDataIn[SC_ASKBID_DIFF_LOW][sc.Index];
	MaxBidAskVolumeDifference[sc.Index] = sc.BaseDataIn[SC_ASKBID_DIFF_HIGH][sc.Index];
}


/*==========================================================================*/
SCSFExport scsf_BidNT(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BidNT = sc.Subgraph[0];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Number of Trades - Bid";
		
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		BidNT.Name = "Bid Trades";
		BidNT.DrawStyle = DRAWSTYLE_BAR;
		BidNT.PrimaryColor = RGB(0,255,0);
		BidNT.DrawZeros = true;
		
		return;
	}
	
	BidNT[sc.Index] = sc.NumberOfBidTrades[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_AskNT(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AskNT = sc.Subgraph[0];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Number of Trades - Ask";
		
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		AskNT.Name = "Ask Trades";
		AskNT.DrawStyle = DRAWSTYLE_BAR;
		AskNT.PrimaryColor = RGB(0,255,0);
		AskNT.DrawZeros = true;
		
		return;
	}
	
	AskNT[sc.Index] = sc.NumberOfAskTrades[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_McClellanOscillator1Chart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MO = sc.Subgraph[0];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];

	SCInputRef UseAbsValue = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "McClellan Oscillator - 1 Chart" ;

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		MO.Name = "MO";
		MO.DrawStyle = DRAWSTYLE_LINE;
		MO.PrimaryColor = RGB(0,255,0);
		MO.DrawZeros = true;

		UseAbsValue.Name = "Use ABS Value.";
		UseAbsValue.SetYesNo(0);

		return;
	}

	sc.DataStartIndex = 1;

	Temp3[0] = Temp2[0] = sc.Close[0];

	if (sc.Index < sc.DataStartIndex)
		return;

	Temp3[sc.Index] = Temp3[sc.Index - 1] * 0.9f + sc.Close[sc.Index] * 0.1f;

	if (UseAbsValue.GetYesNo())
	{
		Temp2[sc.Index] = Temp2[sc.Index - 1] * 0.95f + fabs(sc.Close[sc.Index] * 0.05f);
	}
	else
	{
		Temp2[sc.Index] = Temp2[sc.Index - 1] * 0.95f + sc.Close[sc.Index] * 0.05f;
	}

	MO[sc.Index] = Temp3[sc.Index] - Temp2[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_McClellanSumIndex1Chart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "McClellan Sum. Index - 1 Chart" ;

		sc.GraphRegion = 1;
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		Sum.Name = "Sum";
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.PrimaryColor = RGB(0,255,0);
		Sum.DrawZeros = true;

		return;
	}

	sc.DataStartIndex = 1;

	Temp3[0] = Temp2[0] = sc.Close[0];

	if (sc.Index < sc.DataStartIndex)
		return;

	if (sc.Close[sc.Index] == 0)
	{
		Sum[sc.Index] = Sum[sc.Index - 1];
	}
	else
	{
		Temp3[sc.Index] = Temp3[sc.Index - 1] * 0.9f + sc.Close[sc.Index] * 0.1f;
		Temp2[sc.Index] = Temp2[sc.Index - 1] * 0.95f + sc.Close[sc.Index] * 0.05f;

		Sum[sc.Index] = Sum[sc.Index - 1] + (Temp3[sc.Index] - Temp2[sc.Index]);
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeSumOfStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_Sum = sc.Subgraph[0];
	SCInputRef Input_ResetAtSessionStart = sc.Input[2];
	SCInputRef Input_StudySubgraphReference = sc.Input[3];
	SCInputRef Input_Version = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Sum Of Study";
		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		Subgraph_Sum.Name = "Sum";
		Subgraph_Sum.PrimaryColor = RGB(0,255,0);
		Subgraph_Sum.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Sum.LineWidth = 2;
		Subgraph_Sum.DrawZeros = false;

		Input_StudySubgraphReference.Name = "Study and Subgraph to Reference";
		Input_StudySubgraphReference.SetStudySubgraphValues(1,0);

		Input_ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		Input_ResetAtSessionStart.SetYesNo(0);

		Input_Version.SetInt(2);

		return;
	}

	SCFloatArray StudyArray;
	sc.GetStudyArrayUsingID(Input_StudySubgraphReference.GetStudyID(), Input_StudySubgraphReference.GetSubgraphIndex(), StudyArray);
	if(StudyArray.GetArraySize() == 0)
	{
		sc.AddMessageToLog("Subgraph array being referenced is empty.", 1);
		return;
	}

	sc.DataStartIndex = sc.GetStudyDataStartIndexUsingID(Input_StudySubgraphReference.GetStudyID());

	if (StudyArray.GetArraySize() != sc.ArraySize)
	{
		sc.AddMessageToLog("Subgraph array being referenced is not of the correct array size. Check the settings for the 'Study and Subgraph to Reference' input.", 1);
		return;
	}

	SCDateTime NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1*DAYS;

	if (sc.UpdateStartIndex == 0)
	{
		Subgraph_Sum[0] = StudyArray[0];
	}

	for (int Index = max(sc.UpdateStartIndex, 1); Index < sc.ArraySize; Index++)
	{
		Subgraph_Sum[Index] = Subgraph_Sum[Index - 1] + StudyArray[Index];

		if (Input_ResetAtSessionStart.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];

			if (IndexDateTime >= NextSessionStart)
			{
				Subgraph_Sum[Index] = StudyArray[Index];

				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeDailyVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Daily Volume";
		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 1;

		Sum.Name = "Volume";
		Sum.PrimaryColor = RGB(0, 255, 0);
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.LineWidth = 2;
		Sum.DrawZeros = false;
		
		return;
	}

	
	SCDateTime NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1 * DAYS;

	if (sc.UpdateStartIndex == 0)
	{
		Sum[0] = sc.Volume[0];
	}

	for (int Index = max(sc.UpdateStartIndex, 1); Index < sc.ArraySize; Index++)
	{
		SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];
			

		if (IndexDateTime >= NextSessionStart)
		{
			Sum[Index] = sc.Volume[Index];

			NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1 * DAYS;
		}
		else
			Sum[Index] = Sum[Index - 1] + sc.Volume[Index];

	}
}

/*==========================================================================*/
SCSFExport scsf_AverageContinuous(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AverageResultSubgraph = sc.Subgraph[0];
	SCFloatArrayRef SummationArray = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef BarCountArray = sc.Subgraph[0].Arrays[1];

	SCInputRef StudySubgraphReference = sc.Input[0];
	SCInputRef ResetAtSessionStart = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Average - Continuous";
		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		AverageResultSubgraph.Name = "Average";
		AverageResultSubgraph.PrimaryColor = RGB(0, 255, 0);
		AverageResultSubgraph.DrawStyle = DRAWSTYLE_LINE;
		AverageResultSubgraph.LineWidth = 2;
		AverageResultSubgraph.DrawZeros = false;


		StudySubgraphReference.Name = "Study and Subgraph to Reference";
		StudySubgraphReference.SetStudySubgraphValues(1, 0);

		ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		ResetAtSessionStart.SetYesNo(0);

		return;
	}


	SCFloatArray StudyArray;
	sc.GetStudyArrayUsingID(StudySubgraphReference.GetStudyID(), StudySubgraphReference.GetSubgraphIndex(), StudyArray);
	if (StudyArray.GetArraySize() == 0)
	{
		sc.AddMessageToLog("Subgraph array being referenced is empty.", 1);
		return;
	}

	sc.DataStartIndex = sc.GetStudyDataStartIndexUsingID(StudySubgraphReference.GetStudyID());

	if (StudyArray.GetArraySize() != sc.ArraySize)
	{
		sc.AddMessageToLog("Subgraph array being referenced is not of the correct array size. Check the settings for the 'Study and Subgraph to Reference' Input.", 1);
		return;
	}

	SCDateTime NextSessionStart;
	if (ResetAtSessionStart.GetYesNo() != 0) 
		NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[max(0, sc.UpdateStartIndex - 1)]) + 1 * DAYS;

	if (sc.UpdateStartIndex == 0)
	{
		SummationArray[0] = StudyArray[0];
		BarCountArray[0] = 1;
	}

	for (int Index = max(sc.UpdateStartIndex, 1); Index < sc.ArraySize; Index++)
	{
		SummationArray[Index] = SummationArray[Index - 1] + StudyArray[Index];
		BarCountArray[Index] = BarCountArray[Index-1] + 1;


		if (ResetAtSessionStart.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];

			if (IndexDateTime >= NextSessionStart)
			{
				SummationArray[Index] = StudyArray[Index];
				BarCountArray[Index] = 1;
				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1 * DAYS;
			}
		}

		AverageResultSubgraph [Index] = SummationArray[Index] / BarCountArray[Index];
	}
}
/*==========================================================================*/
SCSFExport scsf_PercentChangeSinceOpen(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentChange = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];
	SCInputRef StartTime = sc.Input[0];
	SCInputRef InputData = sc.Input[1];
	SCInputRef Version = sc.Input[2];
	SCInputRef Input_Multiplier = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Percent Change Since Open";
		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_PERCENT;

		PercentChange.Name = "Percent Change";
		PercentChange.PrimaryColor = RGB(0,255,0);
		PercentChange.DrawStyle = DRAWSTYLE_LINE;
		PercentChange.LineWidth = 1;
		PercentChange.DrawZeros= true;

		ZeroLine.Name= "Zero Line";
		ZeroLine.PrimaryColor = RGB(255,255,0);
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.LineWidth = 1;
		ZeroLine.DrawZeros= true;

		StartTime.Name = "Start Time";
		StartTime.SetCustomInputStrings("Trading Day Start Time;First Bar In Chart");
		StartTime.SetCustomInputIndex(0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Version.SetInt(3);

		Input_Multiplier.Name = "Multiplier";
		Input_Multiplier.SetFloat(1);

		return;
	}

	if (Version.GetInt() < 2)
	{
		Version.SetInt(2);
		InputData.SetInputDataIndex(SC_LAST);
		sc.ValueFormat = VALUEFORMAT_PERCENT;
	}

	if (Version.GetInt() < 3)
	{
		Version.SetInt(3);
		Input_Multiplier.SetFloat(1.0);
	}

	float& OpenValue = sc.GetPersistentFloat(1);
	SCDateTime& SessionStart = sc.GetPersistentSCDateTime(1);

	int OpenSubgraphIndex = SC_OPEN;

	if (InputData.GetInputDataIndex() != SC_LAST)
		OpenSubgraphIndex = InputData.GetInputDataIndex();

	if (sc.Index == 0)
	{
		OpenValue = sc.BaseData[OpenSubgraphIndex][sc.Index];
		SessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.Index]);
	}

	SCDateTime BarSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.Index]);

	if (BarSessionStart != SessionStart)
	{
		if (StartTime.GetIndex() == 0)
		{
			OpenValue = sc.BaseData[OpenSubgraphIndex][sc.Index];
		}

		SessionStart = BarSessionStart;
	}

	if (OpenValue == 0)
		PercentChange[sc.Index] = 0;
	else
	{
		float CurrentValue = sc.BaseData[InputData.GetInputDataIndex()][sc.Index];

		PercentChange[sc.Index] = (CurrentValue - OpenValue) / OpenValue * Input_Multiplier.GetFloat();
	}
}

/*==========================================================================*/
SCSFExport scsf_PercentChangeSincePreviousClose(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentChange = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[1];
	SCInputRef Version = sc.Input[2];
	SCInputRef Input_Multiplier = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Percent Change Since Previous Close";
		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_PERCENT;

		PercentChange.Name = "Percent Change";
		PercentChange.PrimaryColor = RGB(0,255,0);
		PercentChange.DrawStyle = DRAWSTYLE_LINE;
		PercentChange.LineWidth = 1;
		PercentChange.DrawZeros= true;

		ZeroLine.  Name= "Zero Line";
		ZeroLine.PrimaryColor = RGB(255,255,0);
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.LineWidth = 1;
		ZeroLine.DrawZeros= true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Version.SetInt(3);

		Input_Multiplier.Name = "Multiplier";
		Input_Multiplier.SetFloat(1);

		return;
	}

	if (Version.GetInt() < 2)
	{
		Version.SetInt(2);
		InputData.SetInputDataIndex(SC_LAST);
		sc.ValueFormat = VALUEFORMAT_PERCENT;
	}

	if (Version.GetInt() < 3)
	{
		Version.SetInt(3);
		Input_Multiplier.SetFloat(1);
	}


	float& PreviousCloseValue  = sc.GetPersistentFloat(1);
	SCDateTime& SessionStart = sc.GetPersistentSCDateTime(1);

	if (sc.Index == 0)
	{
		PreviousCloseValue = sc.BaseData[InputData.GetInputDataIndex()][sc.Index];
		SessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.Index]);
	}

	SCDateTime BarSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.Index]);

	if (BarSessionStart != SessionStart)
	{
		PreviousCloseValue = sc.BaseData[InputData.GetInputDataIndex()][sc.Index -1];

		SessionStart = BarSessionStart;
	}

	if (PreviousCloseValue == 0)
		PercentChange[sc.Index] = 0;
	else
	{
		PercentChange[sc.Index] = (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] - PreviousCloseValue) / PreviousCloseValue * Input_Multiplier.GetFloat();
	}
}

/*==========================================================================*/
double PointAndFigureAddBoxes(SCStudyInterfaceRef sc, double Value, double BoxSize, int Direction, float TickSize)
{
	double Epsilon = 0.5 * TickSize;

	if (Direction == 1) //up
	{
		int NumberOfBoxes = (int)floor(Value / BoxSize + Epsilon);
		return (NumberOfBoxes * BoxSize);
	}
	else if (Direction == -1) //down
	{
		int NumberOfBoxes = (int)ceil(Value / BoxSize - Epsilon);
		return (NumberOfBoxes * BoxSize);
	}
	else //unknown
		return sc.Round((float)(Value / BoxSize)) * BoxSize;

	return 0;
}

/**************************************************************************************/
SCSFExport scsf_PointAndFigureChart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open           = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High           = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low            = sc.Subgraph[SC_LOW];
	SCSubgraphRef Last           = sc.Subgraph[SC_LAST];
	SCSubgraphRef Volume         = sc.Subgraph[SC_VOLUME];
	SCSubgraphRef NumTrades      = sc.Subgraph[SC_NUM_TRADES];
	SCSubgraphRef OHLCAvg        = sc.Subgraph[SC_OHLC_AVG];
	SCSubgraphRef HLCAvg         = sc.Subgraph[SC_HLC_AVG];
	SCSubgraphRef HLAvg          = sc.Subgraph[SC_HL_AVG];
	SCSubgraphRef BidVol         = sc.Subgraph[SC_BIDVOL];
	SCSubgraphRef AskVol         = sc.Subgraph[SC_ASKVOL];
	SCSubgraphRef UpTickVol      = sc.Subgraph[SC_UPVOL];
	SCSubgraphRef DownTickVol    = sc.Subgraph[SC_DOWNVOL];
	SCSubgraphRef BidTrades      = sc.Subgraph[SC_BIDNT];
	SCSubgraphRef AskTrades      = sc.Subgraph[SC_ASKNT];
	SCSubgraphRef BidAskDiffMax  = sc.Subgraph[SC_ASKBID_DIFF_HIGH];
	SCSubgraphRef BidAskDiffMin  = sc.Subgraph[SC_ASKBID_DIFF_LOW];
	SCSubgraphRef NumberOfBoxes  = sc.Subgraph[PF_NUM_BOXES_ARRAY];
	SCSubgraphRef DirectionArray = sc.Subgraph[PF_DIRECTION_ARRAY];
	SCSubgraphRef TrueLast       = sc.Subgraph[PF_TRUELAST_ARRAY];

	const int ArrayCount = PF_TRUELAST_ARRAY + 1;

	SCInputRef BoxSize                    = sc.Input[0];
	SCInputRef ReversalSize               = sc.Input[1];
	SCInputRef AllowOneBoxReversals       = sc.Input[2];
	SCInputRef NewBarAtStartOfDay         = sc.Input[3];
	SCInputRef IgnoreLastBarUntilComplete = sc.Input[4];
	SCInputRef UsePercentageForBoxSize    = sc.Input[5];
	SCInputRef BoxSizePercentage          = sc.Input[6];
	SCInputRef OnlyUseClosePrices         = sc.Input[8];
	SCInputRef Version                    = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Point & Figure Chart";
		sc.AutoLoop = 0;
		sc.GraphRegion = 0;
		sc.StandardChartHeader = 1;
		sc.IsCustomChart = 1;
		sc.GraphDrawType = GDT_POINT_AND_FIGURE_BARS;//GDT_POINT_AND_FIGURE_XO
		sc.MaintainAdditionalChartDataArrays = 1;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,0,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,255,255);
		Volume.DrawZeros = false;

		NumTrades.Name = "# of Trades / OI";
		NumTrades.DrawStyle = DRAWSTYLE_IGNORE;
		NumTrades.PrimaryColor = RGB(255,255,255);
		NumTrades.DrawZeros = false;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(255,255,255);
		OHLCAvg.DrawZeros = false;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(255,255,255);
		HLCAvg.DrawZeros = false;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(255,255,255);
		HLAvg.DrawZeros = false;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(255,255,255);
		BidVol.DrawZeros = false;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(255,255,255);
		AskVol.DrawZeros = false;

		NumberOfBoxes.Name = "Number of Boxes";
		NumberOfBoxes.DrawStyle = DRAWSTYLE_IGNORE;
		NumberOfBoxes.PrimaryColor = RGB(0xFF,0x99,0x00);
		NumberOfBoxes.DrawZeros = false;

		TrueLast.Name = "True Last";
		TrueLast.DrawStyle = DRAWSTYLE_IGNORE;
		TrueLast.PrimaryColor = RGB(255,255,255);
		TrueLast.DrawZeros = false;

		BoxSize.Name = "Box Size (in ticks)";
		BoxSize.SetFloat(0.0f);
		BoxSize.SetFloatLimits(0, FLT_MAX);

		ReversalSize.Name = "Reversal Size (num boxes)";
		ReversalSize.SetFloat(3.0f);
		ReversalSize.SetFloatLimits(FLT_MIN, FLT_MAX);

		AllowOneBoxReversals.Name = "Allow One Box Reversals";
		AllowOneBoxReversals.SetYesNo(1);
	
		NewBarAtStartOfDay.Name = "New Bar at Start of Day";
		NewBarAtStartOfDay.SetYesNo(0);

		IgnoreLastBarUntilComplete.Name = "Ignore Last Bar Until Completed";
		IgnoreLastBarUntilComplete.SetYesNo(0);

		UsePercentageForBoxSize.Name = "Use Percentage of Last Price for Box Size";
		UsePercentageForBoxSize.SetYesNo(0);

		BoxSizePercentage.Name = "Box Size Percentage (%)";
		BoxSizePercentage.SetFloat(1.0f);
		BoxSizePercentage.SetFloatLimits(0.0001f,5000.0f);

		OnlyUseClosePrices.Name = "Calculate Only Using Close Prices";
		OnlyUseClosePrices.SetYesNo(0);

		Version.SetInt(1);

		return;
	}

	if (Version.GetInt() < 1)
	{
		Version.SetInt(1);

		// convert box size value to ticks
		if (sc.TickSize != 0)
			BoxSize.SetFloat(BoxSize.GetFloat() / sc.TickSize);
	}

	const int PersistentPriorOutDataBaseKey = 100;


	int& r_PersistentPriorOutArraySize = sc.GetPersistentInt(1);

	SCDateTime& NextSessionStart = sc.GetPersistentSCDateTime(1);


	int InputBarIndex = sc.UpdateStartIndex;
	int AvgStartIndex = 0;

	float& r_CurrentBoxSize = sc.PointAndFigureXOGraphDrawTypeBoxSize;

	if (InputBarIndex == 0)
	{
		r_CurrentBoxSize = BoxSize.GetFloat() * sc.TickSize;

		if (UsePercentageForBoxSize.GetYesNo())
			r_CurrentBoxSize = sc.BaseData[SC_LAST][sc.ArraySize-1] * BoxSizePercentage.GetFloat() * 0.01f;

		r_CurrentBoxSize = sc.RoundToTickSize(r_CurrentBoxSize, sc.TickSize);

		if (r_CurrentBoxSize == 0.0)
		{
			if (sc.TickSize != 0)
				r_CurrentBoxSize = sc.TickSize;
			else
				r_CurrentBoxSize = 1;

			BoxSize.SetFloat((float)sc.Round(r_CurrentBoxSize / sc.TickSize));
		}

		sc.ResizeArrays(0);

		AvgStartIndex = 0;

		r_PersistentPriorOutArraySize = 0;
		for (int Index = 0; Index < ArrayCount; ++Index)
		{
			sc.GetPersistentFloat(Index + PersistentPriorOutDataBaseKey) = 0;
		}

		if (!sc.AddElements(1))
			return;

		sc.DateTimeOut[0] = sc.BaseDateTimeIn[0];
		
		Open[0] = sc.Open[0];
		High[0] = (float)sc.RoundToTickSize(sc.Close[0], r_CurrentBoxSize);//round off to the nearest box size
		Low[0] = (float)sc.RoundToTickSize(sc.Close[0], r_CurrentBoxSize);//round off to the nearest box size
		Last[0] = sc.Close[0];
		TrueLast[0] = sc.Close[0];

		NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[0]) + 1*DAYS;

		if (OnlyUseClosePrices.GetYesNo())
			IgnoreLastBarUntilComplete.SetYesNo(1);

		if (sc.ChartDataType == DAILY_DATA)
			NumTrades.Name = "Open Interest";
		else
			NumTrades.Name = "# of Trades";

		sc.GraphName.Format("Point & Figure Chart %.6gx%g %s", 
			r_CurrentBoxSize, 
			ReversalSize.GetFloat(),
			sc.GetStudyNameFromChart(sc.ChartNumber, 0).GetChars());
	}
	else if (!IgnoreLastBarUntilComplete.GetYesNo())
	{	
		AvgStartIndex = r_PersistentPriorOutArraySize - 1;

		// Restore array to size just before processing last input bar (sc.BaseDataIn) and restore state of last output bar.
		sc.ResizeArrays(r_PersistentPriorOutArraySize);

		for (int SubgraphIndex = 0; SubgraphIndex < ArrayCount; ++SubgraphIndex)
		{
			sc.Subgraph[SubgraphIndex][AvgStartIndex] = sc.GetPersistentFloat(SubgraphIndex + PersistentPriorOutDataBaseKey);
		}
	}

	int OutputIndex = sc.DateTimeOut.GetArraySize() - 1;
	if (InputBarIndex == 0)
		InputBarIndex = 1;

	SCFloatArrayRef OpenInputData  = OnlyUseClosePrices.GetYesNo() != 0 ? sc.Close : sc.Open;
	SCFloatArrayRef HighInputData  = OnlyUseClosePrices.GetYesNo() != 0 ? sc.Close : sc.High;
	SCFloatArrayRef LowInputData   = OnlyUseClosePrices.GetYesNo() != 0 ? sc.Close : sc.Low;
	SCFloatArrayRef CloseInputData = sc.Close;

	for (; InputBarIndex < sc.ArraySize; ++InputBarIndex)
	{
		bool NewBar = false;

		if (IgnoreLastBarUntilComplete.GetYesNo() && sc.GetBarHasClosedStatus(InputBarIndex) == BHCS_BAR_HAS_NOT_CLOSED)
			return;

		if (NewBarAtStartOfDay.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[InputBarIndex];

			if (IndexDateTime >= NextSessionStart)
			{
				sc.CalculateOHLCAverages(OutputIndex);

				if (!sc.AddElements(1))
					return;

				NewBar = true;
				OutputIndex = sc.DateTimeOut.GetArraySize() - 1;

				sc.DateTimeOut[OutputIndex] = sc.BaseDateTimeIn[InputBarIndex];

				Open[OutputIndex] = OpenInputData[InputBarIndex];
				High[OutputIndex] = (float)sc.RoundToTickSize(CloseInputData[InputBarIndex], r_CurrentBoxSize);//round off to the nearest box size
				Low[OutputIndex] = (float)sc.RoundToTickSize(CloseInputData[InputBarIndex], r_CurrentBoxSize);//round off to the nearest box size
				Last[OutputIndex] = CloseInputData[InputBarIndex];
				TrueLast[OutputIndex] = CloseInputData[InputBarIndex];

				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}

		// Remember state of last P&F bar when reach last input bar because last
		// input bar can change due to updating, causing for example a new P&F bar
		// to be added when on a later update no new bar is added.
		if (!IgnoreLastBarUntilComplete.GetYesNo() && InputBarIndex == sc.ArraySize - 1)
		{
			r_PersistentPriorOutArraySize = OutputIndex + 1;

			for (int SubgraphIndex = 0; SubgraphIndex < ArrayCount; ++SubgraphIndex)
			{
				sc.GetPersistentFloat(SubgraphIndex + PersistentPriorOutDataBaseKey) = sc.Subgraph[SubgraphIndex][OutputIndex];
			}
		}

		// Determine our initial direction if not known
		if (DirectionArray[OutputIndex] == 0.0f)
		{
			if (Last[OutputIndex] > CloseInputData[InputBarIndex])
				DirectionArray[OutputIndex] = -1;  // Down

			else if (Last[OutputIndex] < CloseInputData[InputBarIndex])
				DirectionArray[OutputIndex] = 1;  // Up
		}

		if (DirectionArray[OutputIndex] == 0.0f)
			continue;  // No direction


		bool UseHighFirst = false;

		int OpenToHighLow = sc.CompareOpenToHighLow(OpenInputData[InputBarIndex], HighInputData[InputBarIndex], LowInputData[InputBarIndex], sc.BaseGraphValueFormat);

		if(OpenToHighLow == 1)
			UseHighFirst = true;
		else if(OpenToHighLow == -1)
			UseHighFirst = false;
		else if (sc.FormattedEvaluate(OpenInputData[InputBarIndex] , sc.BaseGraphValueFormat, GREATER_OPERATOR, CloseInputData[InputBarIndex], sc.BaseGraphValueFormat))
			UseHighFirst = true;
		else if (sc.FormattedEvaluate(OpenInputData[InputBarIndex] , sc.BaseGraphValueFormat, LESS_OPERATOR, CloseInputData[InputBarIndex], sc.BaseGraphValueFormat))
			UseHighFirst = false;
		else if (DirectionArray[OutputIndex] == 1) // Up Bar
			UseHighFirst = true;
		else
			UseHighFirst = false;

		for (int PassNumber = 0; PassNumber < 2; PassNumber++)
		{
			if (DirectionArray[OutputIndex] == 1)//Up bar
			{
				if ((UseHighFirst && PassNumber == 0 ) || (!UseHighFirst && PassNumber == 1))
				{
					if (sc.FormattedEvaluate(HighInputData[InputBarIndex], sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, High[OutputIndex] + r_CurrentBoxSize, sc.BaseGraphValueFormat))
					{
						High[OutputIndex] = (float)PointAndFigureAddBoxes(sc, HighInputData[InputBarIndex], r_CurrentBoxSize, 1, sc.TickSize);
					}
				}

				if ((PassNumber == 0) || (UseHighFirst && PassNumber == 1))
				{
					if (sc.FormattedEvaluate(LowInputData[InputBarIndex], sc.BaseGraphValueFormat, LESS_EQUAL_OPERATOR, High[OutputIndex] - r_CurrentBoxSize * ReversalSize.GetFloat(), sc.BaseGraphValueFormat))
					{
						Last[OutputIndex] = High[OutputIndex];

						bool SetOpen = false;

						if (AllowOneBoxReversals.GetYesNo() //If one box is allowed upon a reversal
							//Column contains at least two boxes. One box would have a zero range. Two boxes has a range of r_CurrentBoxSize. 
							|| sc.FormattedEvaluate((High[OutputIndex] - Low[OutputIndex]), sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, r_CurrentBoxSize, sc.BaseGraphValueFormat)
							)
						{
							// Calculate the number of boxes for the column
							NumberOfBoxes[OutputIndex] = (float)sc.Round((High[OutputIndex] - Low[OutputIndex]) / r_CurrentBoxSize + 1);

							sc.CalculateOHLCAverages(OutputIndex);

							if (!sc.AddElements(1))
								return;

							NewBar = true;
							OutputIndex++;
							sc.DateTimeOut[OutputIndex] = sc.BaseDateTimeIn[InputBarIndex];

							High[OutputIndex] = High[OutputIndex - 1] - r_CurrentBoxSize;

							SetOpen = true;
						}

						DirectionArray[OutputIndex] = -1;
						Low[OutputIndex] = (float)PointAndFigureAddBoxes(sc, LowInputData[InputBarIndex], r_CurrentBoxSize, -1, sc.TickSize);

						if (SetOpen)
							Open[OutputIndex] = Low[OutputIndex];

					}
				}

				if (UseHighFirst)
					PassNumber++;  // Skip next pass

			}
			else// down column
			{
				if ((!UseHighFirst && PassNumber == 0) || (UseHighFirst && PassNumber == 1))
				{
					if (sc.FormattedEvaluate(LowInputData[InputBarIndex] , sc.BaseGraphValueFormat, LESS_EQUAL_OPERATOR, Low[OutputIndex] - r_CurrentBoxSize, sc.BaseGraphValueFormat))
					{
						Low[OutputIndex] = (float)PointAndFigureAddBoxes(sc, LowInputData[InputBarIndex], r_CurrentBoxSize, -1, sc.TickSize);
					}
				}

				if (PassNumber == 0 || (!UseHighFirst && PassNumber == 1))
				{
					if (sc.FormattedEvaluate(HighInputData[InputBarIndex] , sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, Low[OutputIndex] + r_CurrentBoxSize * ReversalSize.GetFloat(), sc.BaseGraphValueFormat))
					{
						Last[OutputIndex] = Low[OutputIndex];

						bool SetOpen = false;
						if (AllowOneBoxReversals.GetYesNo()
							|| sc.FormattedEvaluate((High[OutputIndex] - Low[OutputIndex]), sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, r_CurrentBoxSize, sc.BaseGraphValueFormat)
							)
						{
							// Calculate the number of boxes for the column
							NumberOfBoxes[OutputIndex] = (float)sc.Round((High[OutputIndex] - Low[OutputIndex]) / r_CurrentBoxSize + 1);

							sc.CalculateOHLCAverages(OutputIndex);

							if (!sc.AddElements(1))
								return;

							NewBar = true;
							OutputIndex++;
							sc.DateTimeOut[OutputIndex] = sc.BaseDateTimeIn[InputBarIndex];
	
							Low[OutputIndex] = Low[OutputIndex - 1] + r_CurrentBoxSize;

							SetOpen = true;
						}

						DirectionArray[OutputIndex] = 1;
						High[OutputIndex] = (float)PointAndFigureAddBoxes(sc, HighInputData[InputBarIndex], r_CurrentBoxSize, 1, sc.TickSize);

						if (SetOpen)
							Open[OutputIndex] = High[OutputIndex];
					}
				}

				if (!UseHighFirst)
					PassNumber++;  // Skip next pass

			}//else down column
		} // PassNumber for loop

		TrueLast[OutputIndex] = CloseInputData[InputBarIndex];
		Last[OutputIndex] = CloseInputData[InputBarIndex];
		sc.CalculateOHLCAverages(OutputIndex);
		NumberOfBoxes[OutputIndex] = (float)sc.Round((High[OutputIndex] - Low[OutputIndex]) / r_CurrentBoxSize + 1);

		// Add in volume and open interest
		Volume[OutputIndex] += sc.Volume[InputBarIndex];
		NumTrades[OutputIndex] += sc.OpenInterest[InputBarIndex];
		float BidAskDiffHigh = 0;
		float BidAskDiffLow = 0;
		if (sc.BaseDataIn.GetArraySize() >= SC_ASKBID_DIFF_LOW+1)
		{
			BidAskDiffHigh = AskVol[OutputIndex] - BidVol[OutputIndex] + sc.BaseDataIn[SC_ASKBID_DIFF_HIGH][InputBarIndex];
			BidAskDiffLow  = AskVol[OutputIndex] - BidVol[OutputIndex] + sc.BaseDataIn[SC_ASKBID_DIFF_LOW][InputBarIndex];
		}
		BidVol[OutputIndex] += sc.BidVolume[InputBarIndex];
		AskVol[OutputIndex] += sc.AskVolume[InputBarIndex];
		UpTickVol[OutputIndex] += sc.UpTickVolume[InputBarIndex];
		DownTickVol[OutputIndex] += sc.DownTickVolume[InputBarIndex];
		BidTrades[OutputIndex] += sc.NumberOfBidTrades[InputBarIndex];
		AskTrades[OutputIndex] += sc.NumberOfAskTrades[InputBarIndex];

		if (NewBar || BidAskDiffHigh > BidAskDiffMax[OutputIndex]) 
			BidAskDiffMax[OutputIndex] = BidAskDiffHigh;

		if (NewBar || BidAskDiffLow < BidAskDiffMin[OutputIndex]) 
			BidAskDiffMin[OutputIndex] = BidAskDiffLow;
	}
}


/*==========================================================================*/
SCSFExport scsf_StudyOverlayOHLC(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];

	SCInputRef UseZeroValuesFromSource = sc.Input[0];
	SCInputRef Multiplier = sc.Input[3];
	SCInputRef ChartStudySubgraphReference = sc.Input[4];
	SCInputRef Version = sc.Input[5];
	SCInputRef DrawZeros = sc.Input[6];
	SCInputRef InUseStandardOHLCSubgraphsFromSourceStudy = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Overlay - OHLC";

		sc.ValueFormat = 3;
		sc.GraphRegion = 1;
		sc.GraphDrawType = GDT_OHLCBAR;

		sc.AutoLoop = 0;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = 1;
		Open.DrawZeros = false;
		Open.LineWidth = 1;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(128,255,128);
		High.DrawZeros = false;
		High.LineWidth = 1;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = 1;
		Low.DrawZeros = false;
		Low.LineWidth = 1;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,128,128);
		Last.DrawZeros = false;
		Last.LineWidth = 1;

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

		UseZeroValuesFromSource.Name = "Use Zero Values from Source Study";
		UseZeroValuesFromSource.SetYesNo(false);

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		ChartStudySubgraphReference.Name = "Chart, Study, Subgraph Reference";
		ChartStudySubgraphReference.SetChartStudySubgraphValues(1,1,0);

		Version.SetInt(1);

		DrawZeros.Name = "Draw Zero Values";
		DrawZeros.SetYesNo(false);

		InUseStandardOHLCSubgraphsFromSourceStudy.Name = "Use Standard OHLC Subgraphs from Source Study";
		InUseStandardOHLCSubgraphsFromSourceStudy.SetYesNo(false);

		return;
	}

	if (DrawZeros.GetYesNo())
	{
		Open.DrawZeros =  true;
		High.DrawZeros = true;
		Low.DrawZeros = true;
		Last.DrawZeros = true;
		OHLCAvg.DrawZeros = true;
		HLCAvg.DrawZeros = true;
		HLAvg.DrawZeros = true;
	}
	else
	{
		Open.DrawZeros = false;
		High.DrawZeros = false;
		Low.DrawZeros = false;
		Last.DrawZeros = false;
		OHLCAvg.DrawZeros = false;
		HLCAvg.DrawZeros = false;
		HLAvg.DrawZeros = false;
	}

	int ChartNumber = ChartStudySubgraphReference.GetChartNumber();
	int StudyID = ChartStudySubgraphReference.GetStudyID();
	int StudySubgraphNumber = ChartStudySubgraphReference.GetSubgraphIndex();

	float MultiplierVal = Multiplier.GetFloat();
	if (MultiplierVal == 0.0f)
		MultiplierVal = 1.0f;

	SCDateTimeArray ReferenceChartDateTimeArray;
	sc.GetChartDateTimeArray(ChartNumber, ReferenceChartDateTimeArray);
	if (ReferenceChartDateTimeArray.GetArraySize() == 0)
		return;

	SCGraphData ReferenceArrays;
	sc.GetStudyArraysFromChartUsingID(ChartNumber, StudyID, ReferenceArrays);

	c_ArrayWrapper<float>* p_ReferenceOpen = NULL;
	c_ArrayWrapper<float>* p_ReferenceHigh = NULL;
	c_ArrayWrapper<float>* p_ReferenceLow = NULL;
	c_ArrayWrapper<float>* p_ReferenceClose = NULL;

	if (InUseStandardOHLCSubgraphsFromSourceStudy.GetYesNo())
	{
		p_ReferenceOpen = &ReferenceArrays[SC_OPEN];
		p_ReferenceHigh = &ReferenceArrays[SC_HIGH];
		p_ReferenceLow = &ReferenceArrays[SC_LOW];
		p_ReferenceClose = &ReferenceArrays[SC_LAST];
	}
	else
	{
		p_ReferenceOpen = &ReferenceArrays[StudySubgraphNumber];
		p_ReferenceHigh = &ReferenceArrays[StudySubgraphNumber];
		p_ReferenceLow = &ReferenceArrays[StudySubgraphNumber];
		p_ReferenceClose = &ReferenceArrays[StudySubgraphNumber];
	}

	SCFloatArrayRef ReferenceSubgraphArray = ReferenceArrays[StudySubgraphNumber];

	if (InUseStandardOHLCSubgraphsFromSourceStudy.GetYesNo())
	{
		if (ReferenceArrays[SC_OPEN].GetArraySize() == 0
			|| ReferenceArrays[SC_HIGH].GetArraySize() == 0
			|| ReferenceArrays[SC_LOW].GetArraySize() == 0
			|| ReferenceArrays[SC_LAST].GetArraySize() == 0
		)
		{
			sc.AddMessageToLog("Study references a Study Subgraph that does not exist.", 1);
			return;
		}
	}
	else if (ReferenceSubgraphArray.GetArraySize() == 0)
	{
		sc.AddMessageToLog("Study references a Study Subgraph that does not exist.", 1);
		return;
	}

	if (sc.UpdateStartIndex == 0)
	{
		SCString StudyName = sc.GetStudyNameFromChart(ChartNumber, StudyID);
		if (ChartNumber == sc.ChartNumber && StudyID == sc.StudyGraphInstanceID)
			StudyName = "Self";

		sc.GraphName.Format("Overlay OHLC of %s", StudyName.GetChars());

		//Zero all output values at all chart bars
		for (int Index = 0; Index < sc.ArraySize; Index++)
		{
			Open[Index] = 0.0f;
			High[Index] = 0.0f;
			Low[Index] = 0.0f;
			Last[Index] = 0.0f;
			sc.CalculateOHLCAverages(Index);
		}
	}

	int RefDataStartIndex = sc.GetStudyDataStartIndexFromChartUsingID(ChartNumber, StudyID);

	sc.DataStartIndex = 
		sc.GetContainingIndexForSCDateTime(sc.ChartNumber, ReferenceChartDateTimeArray[RefDataStartIndex]);

	int ArraySize = sc.BaseDateTimeIn.GetArraySize();
	int RefArraySize = ReferenceChartDateTimeArray.GetArraySize();

	int RefIndex = 0;
	int StudyStartIndex = max(sc.DataStartIndex, sc.UpdateStartIndex - 1);

	// Find the appropriate reference index
	RefIndex = sc.GetContainingIndexForSCDateTime(ChartNumber, sc.BaseDateTimeIn[StudyStartIndex]);

	// If no exact match is found, IndexForDateTime returns index right before the specified date and time
	if (sc.BaseDateTimeIn[StudyStartIndex] > ReferenceChartDateTimeArray[RefIndex])
	{
		RefIndex++;
	}

	if (RefIndex >= RefArraySize || StudyStartIndex >= ArraySize)
		return;

	int StartingPoint = StudyStartIndex; // for CalculateAverages afterwards

	float OpenVal = (*p_ReferenceOpen)[RefIndex];
	float HighVal = (*p_ReferenceHigh)[RefIndex];
	float LowVal = (*p_ReferenceLow)[RefIndex];
	float CloseVal = (*p_ReferenceClose)[RefIndex];

	for (int OutputArrayIndex = StudyStartIndex; OutputArrayIndex < ArraySize; OutputArrayIndex++)
	{
		// If DateTime is not contained, then skip. Most likely because referenced graph has a larger duration time period
		if (OutputArrayIndex + 1 < ArraySize && sc.BaseDateTimeIn[OutputArrayIndex + 1] <= ReferenceChartDateTimeArray[RefIndex])
		{
			Open[OutputArrayIndex] = 0;
			Last[OutputArrayIndex] =  0;
			High[OutputArrayIndex] = 0;
			Low[OutputArrayIndex] = 0;
			continue;
		}

		SCDateTime NextIndexDateTime = MAX_DATE;

		if(OutputArrayIndex + 1 < ArraySize)
		{
			NextIndexDateTime = sc.BaseDateTimeIn[OutputArrayIndex + 1];
		}

		// Increment RefIndex while Next BaseTime is greater than RefTime
		while (NextIndexDateTime > ReferenceChartDateTimeArray[RefIndex])
		{
			if ((UseZeroValuesFromSource.GetYesNo() || (*p_ReferenceHigh)[RefIndex] != 0.0f)
				&& HighVal < (*p_ReferenceHigh)[RefIndex]
			)
			{
				HighVal = (*p_ReferenceHigh)[RefIndex];
			}
			else if ((UseZeroValuesFromSource.GetYesNo() || (*p_ReferenceLow)[RefIndex] != 0.0f)
				&& LowVal > (*p_ReferenceLow)[RefIndex]
			)
			{
				LowVal = (*p_ReferenceLow)[RefIndex];
			}

			if (UseZeroValuesFromSource.GetYesNo() || (*p_ReferenceClose)[RefIndex] != 0.0f)
				CloseVal = (*p_ReferenceClose)[RefIndex];

			RefIndex++;

			if (RefIndex >= RefArraySize)
				break;
		}

		Open[OutputArrayIndex] = OpenVal * MultiplierVal;
		Last[OutputArrayIndex] =  CloseVal * MultiplierVal;
		High[OutputArrayIndex] = HighVal * MultiplierVal;
		Low[OutputArrayIndex] = LowVal * MultiplierVal;

		sc.CalculateOHLCAverages(OutputArrayIndex);

		if (RefIndex >= RefArraySize)
			break;

		OpenVal = (*p_ReferenceOpen)[RefIndex];
		HighVal = (*p_ReferenceHigh)[RefIndex];
		LowVal = (*p_ReferenceLow)[RefIndex];
		CloseVal = (*p_ReferenceClose)[RefIndex];
	}
}

/*==========================================================================*/
SCSFExport scsf_MarketFacilitationIndexColored(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MFI = sc.Subgraph[0];
	SCSubgraphRef MfiUpVolumeUp = sc.Subgraph[1];
	SCSubgraphRef MfiDownVolumeDown = sc.Subgraph[2];
	SCSubgraphRef MfiUpVolumeDown = sc.Subgraph[3];
	SCSubgraphRef MfiDownVolumeUp = sc.Subgraph[4];

	SCInputRef Multiplier = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Market Facilitation Index Colored";
		sc.StudyDescription = "Market Facilitation Index with Coloring";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		MFI.Name = "MFI";
		MFI.DrawStyle = DRAWSTYLE_LINE;
		MFI.PrimaryColor = RGB(0,255,0);
		MFI.DrawZeros = true;

		MfiUpVolumeUp.Name = "MFI Up Volume Up Color";
		MfiUpVolumeUp.DrawStyle = DRAWSTYLE_IGNORE;
		MfiUpVolumeUp.PrimaryColor = RGB (0, 255, 0);// Green
		MfiUpVolumeUp.DrawZeros = true;

		MfiDownVolumeDown.Name = "MFI Down Volume Down Color";
		MfiDownVolumeDown.DrawStyle = DRAWSTYLE_IGNORE;
		MfiDownVolumeDown.PrimaryColor = RGB (140, 69, 16);// Brown
		MfiDownVolumeDown.DrawZeros = true;

		MfiUpVolumeDown.Name = "MFI Up Volume Down Color";
		MfiUpVolumeDown.DrawStyle = DRAWSTYLE_IGNORE;
		MfiUpVolumeDown.PrimaryColor = RGB (0, 0, 255);// Blue
		MfiUpVolumeDown.DrawZeros = true;

		MfiDownVolumeUp.Name = "MFI Down Volume Up Color";
		MfiDownVolumeUp.DrawStyle = DRAWSTYLE_IGNORE;
		MfiDownVolumeUp.PrimaryColor = RGB (255, 0, 0);// Red
		MfiDownVolumeUp.DrawZeros = true;

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		return;
	}

	

	MFI[sc.Index] = Multiplier.GetFloat() * (sc.High[sc.Index] - sc.Low[sc.Index]) / 
		sc.Volume[sc.Index];

	if (sc.Index == 0)
	{
		MFI.DataColor[sc.Index] = MfiUpVolumeUp.PrimaryColor;
		return;
	}

	// Coloring
	if (MFI[sc.Index] > MFI[sc.Index - 1] && sc.Volume[sc.Index] > sc.Volume[sc.Index - 1])
	{
		MFI.DataColor[sc.Index] = MfiUpVolumeUp.PrimaryColor;
	}
	else if (MFI[sc.Index] < MFI[sc.Index - 1] && sc.Volume[sc.Index] < sc.Volume[sc.Index - 1])
	{
		MFI.DataColor[sc.Index] = MfiDownVolumeDown.PrimaryColor;
	}
	else if (MFI[sc.Index] > MFI[sc.Index - 1] && sc.Volume[sc.Index] < sc.Volume[sc.Index - 1])
	{
		MFI.DataColor[sc.Index] = MfiUpVolumeDown.PrimaryColor;
	}
	else if (MFI[sc.Index] < MFI[sc.Index - 1] && sc.Volume[sc.Index] > sc.Volume[sc.Index - 1])
	{
		MFI.DataColor[sc.Index] = MfiDownVolumeUp.PrimaryColor;
	}
}

/*==========================================================================*/
SCSFExport scsf_StarcBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef KeltnerAverage = sc.Subgraph[0];
	SCSubgraphRef TopBand = sc.Subgraph[1];
	SCSubgraphRef BottomBand = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];

	SCInputRef InputData = sc.Input[0];
	SCInputRef KeltnerMALength = sc.Input[3];
	SCInputRef TrueRangeAvgLength = sc.Input[4];
	SCInputRef TopBandMult = sc.Input[5];
	SCInputRef BottomBandMult = sc.Input[6];
	SCInputRef KeltnerMAType = sc.Input[7];
	SCInputRef ATR_MAType = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Starc Bands";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		KeltnerAverage.Name = "Starc Average";
		KeltnerAverage.DrawStyle = DRAWSTYLE_LINE;
		KeltnerAverage.PrimaryColor = RGB(0,255,0);
		KeltnerAverage.DrawZeros = true;

		TopBand.Name = "Top";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(255,0,255);
		TopBand.DrawZeros = true;

		BottomBand.Name = "Bottom";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(255,255,0);
		BottomBand.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		KeltnerMALength.Name = "Starc Mov Avg Length";
		KeltnerMALength.SetInt(6);
		KeltnerMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		TrueRangeAvgLength.Name = "True Range Avg Length";
		TrueRangeAvgLength.SetInt(15);
		TrueRangeAvgLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		TopBandMult.Name = "Top Band Multiplier";
		TopBandMult.SetFloat(2.0f);

		BottomBandMult.Name="Bottom Band Multiplier";
		BottomBandMult.SetFloat(2.0f);

		KeltnerMAType.Name = "Starc Mov Avg Type (Center line)";
		KeltnerMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		ATR_MAType.Name = "ATR Mov Avg Type";
		ATR_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		return;
	}

	sc.DataStartIndex = max(KeltnerMALength.GetInt(), TrueRangeAvgLength.GetInt());

	sc.MovingAverage(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], KeltnerAverage, KeltnerMAType.GetMovAvgType(), KeltnerMALength.GetInt());

	sc.ATR(sc.BaseDataIn, Temp3, TrueRangeAvgLength.GetInt(), ATR_MAType.GetMovAvgType());
	sc.ATR(sc.BaseDataIn, Temp4, TrueRangeAvgLength.GetInt(), ATR_MAType.GetMovAvgType());

	TopBand[sc.Index] = Temp3[sc.Index] * TopBandMult.GetFloat() + KeltnerAverage[sc.Index];
	BottomBand[sc.Index] = KeltnerAverage[sc.Index] - (Temp4[sc.Index] * BottomBandMult.GetFloat());
}


/*==========================================================================*/
SCSFExport scsf_BarTimeDuration(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TimeDiff = sc.Subgraph[0];

	SCInputRef Multiplier = sc.Input[0];

	SCInputRef UseMaxDuration =  sc.Input[1];

	SCInputRef MaxDuration =  sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bar Time Duration";
		sc.StudyDescription = "This study calculates and displays the time duration of a bar. For the highest accuracy, this study requires that the Intraday Data Storage Time Unit setting is 1 second or less.";

		TimeDiff.Name = "Duration";
		TimeDiff.DrawStyle = DRAWSTYLE_BAR;
		TimeDiff.PrimaryColor = RGB(0,255,0);


		UseMaxDuration.Name = "Use Maximum Duration";
		UseMaxDuration.SetYesNo(0);

		MaxDuration.Name="Maximum Duration";
		MaxDuration.SetTime(0);


		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_TIME;
		sc.AutoLoop = 1;

		return;
	}


	SCDateTime BarEndDateTime =  sc.GetEndingDateTimeForBarIndex(sc.Index);

	TimeDiff[sc.Index] = (float)(BarEndDateTime - sc.BaseDateTimeIn[sc.Index] + 1 * MILLISECONDS);

	if (TimeDiff[sc.Index] < 0)
		TimeDiff[sc.Index] = 0;

	if(UseMaxDuration.GetYesNo())
	{
		SCDateTime MaxTime(0,MaxDuration.GetTime());
		if (TimeDiff[sc.Index]  > (double)MaxTime)
			TimeDiff[sc.Index] = (float)MaxTime;
	}
}

/*==========================================================================*/

SCSFExport scsf_ReferenceDataFromAnotherChart(SCStudyInterfaceRef sc)
{
	SCInputRef ChartStudyInput = sc.Input[0];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Reference Data";
		sc.StudyDescription = "This is an example of referencing data from another chart.";

		sc.AutoLoop = 1;
		

		ChartStudyInput.Name = "Study Reference";
		ChartStudyInput.SetChartStudyValues(1,1);


		return;
	}

	// The following code is for getting the High array
	// and corresponding index from another chart.
	
	// Define a graph data object to get all of the base graph data from the specified chart
	SCGraphData BaseGraphData;
	
	// Get the base graph data from the specified chart
	sc.GetChartBaseData(ChartStudyInput.GetChartNumber(), BaseGraphData);
	
	// Define a reference to the High array
	SCFloatArrayRef HighArray = BaseGraphData[SC_HIGH];
	
	// Array is empty. Nothing to do.
	if(HighArray.GetArraySize() == 0)
		return;
	
	
	// Get the index in the specified chart that is 
	// nearest to current index.
	int RefChartIndex = sc.GetNearestMatchForDateTimeIndex(ChartStudyInput.GetChartNumber(), sc.Index);
	float NearestRefChartHigh = HighArray[RefChartIndex];

	// Get the index in the specified chart that contains
	// the DateTime of the bar at the current index.
	RefChartIndex = sc.GetContainingIndexForDateTimeIndex(ChartStudyInput.GetChartNumber(), sc.Index);
	float ContainingRefChartHigh = HighArray[RefChartIndex];

	// Get the index in the specified chart that exactly
	// matches the DateTime of the current index.
	RefChartIndex = sc.GetExactMatchForSCDateTime(ChartStudyInput.GetChartNumber(), sc.BaseDateTimeIn[sc.Index]);
	if(RefChartIndex != -1)
	{
		float ExactMatchRefChartHigh = HighArray[RefChartIndex];
	}
	
	
	// The following code is for getting a study subgraph array
	// and corresponding index from another chart. 
	// For example, this could be a moving average study subgraph.

	// Define a graph data object to get all of the study data
	SCGraphData StudyData;
	
	// Get the study data from the specified chart
	sc.GetStudyArraysFromChartUsingID(ChartStudyInput.GetChartNumber(), ChartStudyInput.GetStudyID(), StudyData);

	//Check if the study has been found.  If it has, GetArraySize() will return the number of Subgraphs in the study.
	if(StudyData.GetArraySize() == 0)
		return;
	
	// Define a reference to the first subgraph array. Note the type must be 'SCFloatArrayRef' and not 'SCFloatArray'.
	SCFloatArrayRef SubgraphArray = StudyData[0];
	
	// Array is empty. Nothing to do.
	if(SubgraphArray.GetArraySize() == 0)
		return;
	
	
	// Get the index in the specified chart that is nearest
	// to current index.
	RefChartIndex = sc.GetNearestMatchForDateTimeIndex(ChartStudyInput.GetChartNumber(), sc.Index);
	float NearestSubgraphValue = SubgraphArray[RefChartIndex];

	// Get the index in the specified chart that contains 
	// the DateTime of the bar at the current index.
	RefChartIndex = sc.GetContainingIndexForDateTimeIndex(ChartStudyInput.GetChartNumber(), sc.Index);
	float ContainingSubgraphValue = SubgraphArray[RefChartIndex];

	// Get the index in the specified chart that exactly
	// matches the DateTime of the current index.
	RefChartIndex = sc.GetExactMatchForSCDateTime(ChartStudyInput.GetChartNumber(), sc.BaseDateTimeIn[sc.Index]);
	if(RefChartIndex != -1)//-1 means that there was not an exact match and therefore we do not have a valid index to work with
	{
		float ExactMatchSubgraphValue = SubgraphArray[RefChartIndex];
	}
}

/*****************************************************************/
SCSFExport scsf_ReferenceStudyData(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Average = sc.Subgraph[0];
	SCInputRef Study1 = sc.Input[0];
	SCInputRef Study1Subgraph = sc.Input[1];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Reference Study Data";
		sc.StudyDescription = "This study function is an example of referencing data from other studies on the chart.";

		sc.AutoLoop = 1;

		// We must use a low precedence level to ensure
		// the other studies are already calculated first.
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(0,255,0);

		Study1.Name = "Input Study 1";
		Study1.SetStudyID(0);

		Study1Subgraph.Name = "Study 1 Subgraph";
		Study1Subgraph.SetSubgraphIndex(0);

		return;
	}

	// Get the subgraph specified with the Study 1
	// Subgraph input from the study specified with
	// the Input Study 1 input.
	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(Study1.GetStudyID(),Study1Subgraph.GetSubgraphIndex(),Study1Array);

	// We are getting the value of the study subgraph
	// at sc.Index. For example, this could be
	// a moving average value if the study we got in
	// the prior step is a moving average.
	float RefStudyCurrentValue = Study1Array[sc.Index];

	// Here we will add 10 to this value and compute 
	// an average of it. Since the moving average
	// function we will be calling requires an input
	// array, we will use one of the internal arrays
	// on a subgraph to hold this intermediate 
	// calculation. This internal array could be 
	// thought of as a Worksheet column where you 
	// are performing intermediate calculations.

	Average.Arrays[9][sc.Index] = RefStudyCurrentValue + 10;

	sc.SimpleMovAvg(Average.Arrays[9],Average,15);
}


/*==========================================================================*/
SCSFExport scsf_IntermediateStudyCalculationsUsingArrays(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Average = sc.Subgraph[0];
	SCInputRef Length = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Intermediate Study Calculations Using Arrays";
		sc.StudyDescription = "For more information about this study, refer to the Using ACSIL Study Calculation Functions documentation page.";

		sc.AutoLoop = 1;

		

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(0,255,0);

		Length.Name = "Moving Average Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}


	// Here we will add 10 to the sc.BaseData Last value at the current index
	// and compute an average of it. Since the moving average function we
	// will be calling requires an input array and not a single value, we
	// will use one of the internal extra arrays on a Subgraph to hold 
	// this intermediate calculation. This internal extra array could be
	// thought of as a Worksheet column where you are performing intermediate
	// calculations. We will use one of the internal extra arrays that is
	// part of the Subgraph we are using to hold the output from the moving
	// average study calculation function we will be calling next. Although
	// any Subgraph internal extra array or even a Subgraph Data array
	// could be used.

	SCFloatArrayRef Last =  sc.Close;
	SCFloatArrayRef Array8 = Average.Arrays[8];

	Array8[sc.Index] =Last[sc.Index] + 10;

	// In this function call we are passing this internal extra array and
	// we also pass in, sc.Subgraph[0], to receive the result at the 
	// current index.

	sc.SimpleMovAvg(Array8,Average,Length.GetInt());
}


/*==========================================================================*/
SCSFExport scsf_StudySubgraphMultiply(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];
	
	SCInputRef InputData = sc.Input[0];
	SCInputRef Multiplier = sc.Input[1];
	SCInputRef DrawZeros = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Multiply";
		sc.StudyDescription = "This study multiplies selected study subgraph by the specified Multiplier. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study subgraph in that study to multiply by the specified Multiplier.";

		sc.AutoLoop = 0;// Needed when using sc.GetCalculationStartIndexForStudy

		

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0, 255, 0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}

	Result.DrawZeros = DrawZeros.GetYesNo();

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		Result[Index] = sc.BaseData[InputData.GetInputDataIndex()][Index] * Multiplier.GetFloat();
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

}

/*==========================================================================*/

SCSFExport scsf_StudySubgraphDivide(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Divider = sc.Input[1];

	SCInputRef DrawZeros = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Divide";
		sc.StudyDescription = "This study divides selected study subgraph by the specified Divider. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study subgraph in that study to divide by the specified Divider.";
		sc.AutoLoop = 0;

		

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		Divider.Name = "Divider";
		Divider.SetFloat(1.0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);


		return;
	}

	Result.DrawZeros = DrawZeros.GetYesNo();

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{
		if (Divider.GetFloat() != 0)
			Result[Index] = sc.BaseData[InputData.GetInputDataIndex()][Index] / Divider.GetFloat();
		else
			Result[Index] = 0;
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

	
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphAddition(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef AmountToAdd = sc.Input[1];
	SCInputRef DrawZeros = sc.Input[2];
	SCInputRef AddToZeroValuesInBasedOnStudy = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Add";

		sc.StudyDescription = "This study adds the specified value to the selected study Subgraph. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study Subgraph in that study to Add the value to.";

		sc.AutoLoop = 0;

		

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		AmountToAdd.Name = "Amount to Add";
		AmountToAdd.SetFloat(0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		AddToZeroValuesInBasedOnStudy.Name = "Add to Zero Values in Based On Study";
		AddToZeroValuesInBasedOnStudy.SetYesNo(true);

		return;
	}

	Result.DrawZeros = DrawZeros.GetYesNo();

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		float BasedOnStudySubgraphValue = sc.BaseData[InputData.GetInputDataIndex()][Index];

		if (AddToZeroValuesInBasedOnStudy.GetYesNo() == 0 && BasedOnStudySubgraphValue == 0.0)
		{
			Result[Index] = 0.0;
		}
		else
			Result[Index] = BasedOnStudySubgraphValue + AmountToAdd.GetFloat();
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;


}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphSubtraction(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef AmountToSubtract = sc.Input[1];
	SCInputRef DrawZeros = sc.Input[2];
	SCInputRef SubtractFromZeroValuesInBasedOnStudy = sc.Input[3];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Study Subgraph Subtract";
		sc.StudyDescription = "This study subtracts the specified value from the selected study subgraph. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study subgraph in that study to Subtract the value from.";

		sc.AutoLoop = 0;

		

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		AmountToSubtract.Name = "Amount to Subtract";
		AmountToSubtract.SetFloat(0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		SubtractFromZeroValuesInBasedOnStudy.Name = "Subtract From Zero Values in Based On Study";
		SubtractFromZeroValuesInBasedOnStudy.SetYesNo(true);

		return;
	}

	Result.DrawZeros= DrawZeros.GetYesNo();


	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		float BasedOnStudySubgraphValue = sc.BaseData[InputData.GetInputDataIndex()][Index];

		if (SubtractFromZeroValuesInBasedOnStudy.GetYesNo() == 0 && BasedOnStudySubgraphValue == 0.0)
		{
			Result[Index] = 0.0;
		}
		else
			Result[Index] = BasedOnStudySubgraphValue - AmountToSubtract.GetFloat();
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

}


/*==========================================================================*/
SCSFExport scsf_StudySubgraphStandardDeviation(SCStudyInterfaceRef sc)
{
	SCSubgraphRef StdDevTop     = sc.Subgraph[0];
	SCSubgraphRef StdDevBottom  = sc.Subgraph[1];

	SCFloatArrayRef StdDev = StdDevTop.Arrays[0];

	SCInputRef InputData        = sc.Input[0];
	SCInputRef StdDevLength     = sc.Input[1];
	SCInputRef StdDevMultiplier = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Standard Deviation";
		sc.StudyDescription = "This study calculates the standard deviation of the selected study subgraph using the specified length and multiplier. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study subgraph in that study to calculate the standard deviation on.";

		sc.AutoLoop = 0;

		

		StdDevTop.Name = "Standard Deviation Top";
		StdDevTop.DrawStyle = DRAWSTYLE_LINE;
		StdDevTop.PrimaryColor = RGB(0,255,0);

		StdDevBottom.Name = "Standard Deviation Bottom";
		StdDevBottom.DrawStyle = DRAWSTYLE_LINE;
		StdDevBottom.PrimaryColor = RGB(255,0,255);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		StdDevLength.Name = "Standard Deviation Length";
		StdDevLength.SetInt(20);
		StdDevLength.SetIntLimits(1, INT_MAX);

		StdDevMultiplier.Name = "Standard Deviation Multiplier";
		StdDevMultiplier.SetFloat(1.0f);

		return;
	}

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		sc.StdDeviation(sc.BaseData[InputData.GetInputDataIndex()], StdDev, Index, StdDevLength.GetInt());

		float Value = sc.BaseData[InputData.GetInputDataIndex()][Index];
		float ChannelSize = StdDevMultiplier.GetFloat() * StdDev[Index];

		StdDevTop[Index] = Value + ChannelSize;
		StdDevBottom[Index] = Value - ChannelSize;
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;


}


/*=====================================================================
	Volume At Price array test.
----------------------------------------------------------------------------*/
SCSFExport scsf_VolumeAtPriceArrayTest(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];
	SCSubgraphRef VolumeAtPriceVolume = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Volume At Price Array Test";
		
		sc.StudyDescription = "This study tests the sc.VolumeAtPriceForBars array.";
		
		sc.AutoLoop = 1;
		
		
		
		
		sc.MaintainVolumeAtPriceData = 1;  // true
		
		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.LineWidth = 2;
		
		VolumeAtPriceVolume.Name = "VAP Volume";
		VolumeAtPriceVolume.DrawStyle = DRAWSTYLE_DASH;
		VolumeAtPriceVolume.LineWidth = 2;
		
		return;
	}

	// Do data processing
	
	Volume[sc.Index] = sc.Volume[sc.Index];// For comparison to the subgraph VolumeAtPriceVolume
	
	if ((int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
		return;


	// Get the sum of the volumes from all of the prices at this bar
	unsigned int TotalVolume = 0;
	
	const s_VolumeAtPriceV2 *p_VolumeAtPrice=NULL;
	int VAPSizeAtBarIndex = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(sc.Index);
	for (int VAPIndex = 0; VAPIndex < VAPSizeAtBarIndex; VAPIndex++)
	{
		if (!sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, VAPIndex, &p_VolumeAtPrice))
			break;

		TotalVolume += p_VolumeAtPrice->Volume;

		//Calculate the price.  This requires multiplying p_VolumeAtPrice->PriceInTicks by the tick size
		float Price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;

		//Other members available:
		unsigned int AskVolume = p_VolumeAtPrice->AskVolume;
		unsigned int BidVolume = p_VolumeAtPrice->BidVolume;
		unsigned int NumberOfTrades = p_VolumeAtPrice->NumberOfTrades;
	}

	VolumeAtPriceVolume[sc.Index] = (float)TotalVolume;


	// Get the sum of the volumes from all of the prices at this bar by using sc.VolumeAtPriceForBars->GetVAPElementAtIndex()
	TotalVolume = 0;
	s_VolumeAtPriceV2 *p_VolumeAtPriceAtIndex;
	int Count = sc.VolumeAtPriceForBars-> GetSizeAtBarIndex(sc.Index);
	for (int ElementIndex = 0;ElementIndex < Count; ElementIndex ++)
	{
			sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, ElementIndex, &p_VolumeAtPriceAtIndex);

			//verify the pointer is not a null, otherwise an exception will occur
			if (p_VolumeAtPriceAtIndex)
				TotalVolume += p_VolumeAtPriceAtIndex->Volume;
		
	}

	VolumeAtPriceVolume[sc.Index] = (float)TotalVolume;

}



/*==========================================================================*/
/*
Here are the other markets (in addition to the e-mini S&P):

* 30-year Bonds (ZB)
o Tick Setting: 6 ticks (Please adjust the calculation of Ping Pong Entries accordingly)
* Euro FX (6E)
o Tick Setting: 16 ticks (Please adjust the calculation of Ping Pong Entries accordingly)
* e-mini Dow (YM)
o Tick Setting: 16 ticks (Please adjust the calculation of Ping Pong Entries accordingly)

*/


SCSFExport scsf_RockwellTrading(SCStudyInterfaceRef sc)
{
	//--------------#1
	SCSubgraphRef BaseColor = sc.Subgraph[0];
	SCSubgraphRef StudySubgraphAddition = sc.Subgraph[1];
	SCSubgraphRef StudySubgraphSubtraction = sc.Subgraph[2];
	SCInputRef Offset = sc.Input[0];

	//--------------#2
	SCSubgraphRef DailyRangeAverage = sc.Subgraph[3];
	SCSubgraphRef StopLoss = sc.Subgraph[4];
	SCSubgraphRef ProfitTarget = sc.Subgraph[5];

	SCInputRef DailyRangeChartNumber = sc.Input[1];
	SCInputRef DailyRangeAverageStudyNumber = sc.Input[2];

	SCInputRef DailyRangeAverageReference = sc.Input[9];

	SCInputRef DisplayTargetAndStopValues=sc.Input[8];

	//--------------#3 Coloring the Main Price Graph
	SCSubgraphRef ColorBarSubgraph = sc.Subgraph[6];
	SCInputRef SelectMACD = sc.Input[3];

	//--------------#4
	SCSubgraphRef PointOnHigh = sc.Subgraph[7];
	SCSubgraphRef PointOnLow = sc.Subgraph[8];
	SCInputRef SelectRSI = sc.Input[4];

	//--------------
	SCInputRef Version = sc.Input[5];
	SCInputRef StopLossMultiplier = sc.Input[6];
	SCInputRef ProfitTargetMultiplier = sc.Input[7];

	

	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Rockwell Trading";

		sc.StudyDescription = "The study is for the Rockwell trading method. Notes: The Offset input specifies the amount as an actual value added to the Low of the last bar and subtracted from the High the last bar. These results are displayed as text on the right side of the chart.";

		sc.AutoLoop = 1;

		
		

		sc.GraphRegion = 0; 

		//--------------#1
		//Subgraphs
		BaseColor.Name = "Base Color";
		BaseColor.PrimaryColor = RGB(255,255,255);
		BaseColor.DrawStyle = DRAWSTYLE_COLOR_BAR;
		BaseColor.LineLabel = 0;

		StudySubgraphAddition.Name = "Addition";
		StudySubgraphAddition.DrawStyle = DRAWSTYLE_TEXT;
		StudySubgraphAddition.LineWidth = 8;
		StudySubgraphAddition.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_RIGHT | LL_VALUE_ALIGN_CENTER;
		StudySubgraphAddition.PrimaryColor = RGB(0,255,0);

		StudySubgraphSubtraction.Name = "Subtraction";
		StudySubgraphSubtraction.DrawStyle = DRAWSTYLE_TEXT;
		StudySubgraphSubtraction.LineWidth = 8;
		StudySubgraphSubtraction.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_RIGHT | LL_VALUE_ALIGN_CENTER;
		StudySubgraphSubtraction.PrimaryColor = RGB(255,0,0);

		//Inputs
		Offset.Name = "Offset";
		Offset.SetFloat(2.25f);

		//--------------#2
		//Subgraphs

		DailyRangeAverage.Name = "Daily Range Average";
		DailyRangeAverage.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		DailyRangeAverage.PrimaryColor = RGB(255,255,255);
		DailyRangeAverage.LineWidth = 10;

		StopLoss.Name = "Stop Loss";
		StopLoss.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		StopLoss.PrimaryColor = RGB(255,0,0);
		StopLoss.LineWidth = 10;

		ProfitTarget.Name = "Profit Target";
		ProfitTarget.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		ProfitTarget.PrimaryColor = RGB(0,255,0);
		ProfitTarget.LineWidth = 10;

		//DailyRangeChartNumber.Name = "Daily Chart # Reference";
		//DailyRangeChartNumber.SetChartNumber(2);

		//DailyRangeAverageStudyNumber.Name = "Daily Range Average Study Number"; 
		//DailyRangeAverageStudyNumber.SetInt(2);


		DailyRangeAverageReference.Name = "Daily Range Average Reference";
		DailyRangeAverageReference.SetChartStudySubgraphValues(2,2, 0);



		//--------------#3 Coloring the Main Price Graph
		ColorBarSubgraph.Name = "Color Bar";
		ColorBarSubgraph.DrawStyle = DRAWSTYLE_COLOR_BAR;
		ColorBarSubgraph.SecondaryColorUsed = 1;
		ColorBarSubgraph.PrimaryColor = RGB(0,255,0);
		ColorBarSubgraph.SecondaryColor = RGB(255,0,0);

		SelectMACD.Name = "MACD Study Reference"; 
		SelectMACD.SetStudyID(2);

		//--------------#4

		PointOnHigh.Name = "RSI Low";
		PointOnHigh.DrawStyle = DRAWSTYLE_POINT_ON_HIGH;
		PointOnHigh.PrimaryColor = RGB(128,0,0); 
		PointOnHigh.LineWidth = 5;

		PointOnLow.Name = "RSI High";
		PointOnLow.DrawStyle = DRAWSTYLE_POINT_ON_LOW;
		PointOnLow.PrimaryColor = RGB(0,128,0);
		PointOnLow.LineWidth = 5;

		SelectRSI.Name = "RSI Study Reference"; 
		SelectRSI.SetStudyID(9);
		//--------------

		DisplayTargetAndStopValues.  Name = "Display Target and Stop Values";
		DisplayTargetAndStopValues.SetYesNo(true);

		Version.SetInt(4);

		StopLossMultiplier.Name = "Stop Loss Multiplier";
		StopLossMultiplier.SetFloat(0.1f);
		ProfitTargetMultiplier.Name = "Profit Target Multiplier";
		ProfitTargetMultiplier.SetFloat(0.15f);

		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		return;
	}

	

	//Version update
	if(Version.GetInt()<3)
	{
		StopLossMultiplier.SetFloat(0.1f);
		ProfitTargetMultiplier.SetFloat(0.15f);
	}
	else if (Version.GetInt()<4)
	{
		DisplayTargetAndStopValues.SetYesNo(true);


		DailyRangeAverageReference. SetChartStudySubgraphValues(DailyRangeChartNumber.GetChartNumber(), DailyRangeAverageStudyNumber.GetInt(),0);

	}

	Version.SetInt(4);
	

	// Do data processing

	//--------------#1
	

	StudySubgraphAddition[sc.Index] = sc.Low[sc.Index] + Offset.GetFloat();
	StudySubgraphSubtraction[sc.Index] = sc.High[sc.Index] - Offset.GetFloat();
	
	//--------------#2
	if (sc.Index == sc.ArraySize - 1)
	{


		SCFloatArray  DailyRangeAverageData;

		sc.GetStudyArrayFromChartUsingID(DailyRangeAverageReference.GetChartStudySubgraphValues() , DailyRangeAverageData);

		if(DailyRangeAverageData.GetArraySize() == 0)
			return;

		float PriorDayDailyRangeAverage = DailyRangeAverageData[DailyRangeAverageData.GetArraySize()-2]; // Get the prior day's daily range average
		float CurrentStopLoss = (float)sc.RoundToTickSize(PriorDayDailyRangeAverage*StopLossMultiplier.GetFloat(),sc.TickSize);
		float CurrentProfitTarget = (float)sc.RoundToTickSize(PriorDayDailyRangeAverage*ProfitTargetMultiplier.GetFloat(),sc.TickSize);



		s_UseTool Tool_DailyRangeAverage;

		Tool_DailyRangeAverage.Clear();
		Tool_DailyRangeAverage.ChartNumber = sc.ChartNumber;
		Tool_DailyRangeAverage.DrawingType = DRAWING_TEXT;
		Tool_DailyRangeAverage.LineNumber = 72342;
		Tool_DailyRangeAverage.BeginDateTime = 5;
		Tool_DailyRangeAverage.BeginValue = 95;
		Tool_DailyRangeAverage.UseRelativeVerticalValues= true;
		Tool_DailyRangeAverage.Region = sc.GraphRegion;
		Tool_DailyRangeAverage.Color = DailyRangeAverage.PrimaryColor;
		Tool_DailyRangeAverage.FontBold = true;
		Tool_DailyRangeAverage.FontFace = "Lucida Console";
		Tool_DailyRangeAverage.FontSize = DailyRangeAverage.LineWidth;
		Tool_DailyRangeAverage.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool_DailyRangeAverage.ReverseTextColor = false;

		Tool_DailyRangeAverage.Text.Format("DailyRangeAvg: %s",sc.FormatGraphValue(PriorDayDailyRangeAverage,sc.BaseGraphValueFormat).GetChars());

		sc.UseTool(Tool_DailyRangeAverage);


		if (DisplayTargetAndStopValues.GetYesNo())
		{
			s_UseTool Tool_StopLoss;

			Tool_StopLoss.Clear();
			Tool_StopLoss.ChartNumber = sc.ChartNumber;
			Tool_StopLoss.DrawingType = DRAWING_TEXT;
			Tool_StopLoss.LineNumber = 72343;
			Tool_StopLoss.BeginDateTime = 5;
			Tool_StopLoss.BeginValue = 90;
			Tool_StopLoss.UseRelativeVerticalValues= true;
			Tool_StopLoss.Region = sc.GraphRegion;
			Tool_StopLoss.Color = StopLoss.PrimaryColor;
			Tool_StopLoss.FontBold = true;
			Tool_StopLoss.FontFace = "Lucida Console";
			Tool_StopLoss.FontSize = StopLoss.LineWidth;
			Tool_StopLoss.AddMethod = UTAM_ADD_OR_ADJUST;
			Tool_StopLoss.ReverseTextColor = false;

			Tool_StopLoss.Text.Format("StopLoss: %s ",sc.FormatGraphValue(CurrentStopLoss,sc.BaseGraphValueFormat).GetChars());

			sc.UseTool(Tool_StopLoss);

		}


		if (DisplayTargetAndStopValues.GetYesNo())
		{

			s_UseTool Tool_ProfitTarget;

			Tool_ProfitTarget.Clear();
			Tool_ProfitTarget.ChartNumber = sc.ChartNumber;
			Tool_ProfitTarget.DrawingType = DRAWING_TEXT;
			Tool_ProfitTarget.LineNumber = 72344;
			Tool_ProfitTarget.BeginDateTime = 5;
			Tool_ProfitTarget.BeginValue = 85;
			Tool_ProfitTarget.UseRelativeVerticalValues= true;
			Tool_ProfitTarget.Region = sc.GraphRegion;
			Tool_ProfitTarget.Color = ProfitTarget.PrimaryColor;
			Tool_ProfitTarget.FontBold = true;
			Tool_ProfitTarget.FontFace = "Lucida Console";
			Tool_ProfitTarget.FontSize = ProfitTarget.LineWidth;
			Tool_ProfitTarget.AddMethod = UTAM_ADD_OR_ADJUST;
			Tool_ProfitTarget.ReverseTextColor = false;

			Tool_ProfitTarget.Text.Format("Target: %s ",sc.FormatGraphValue(CurrentProfitTarget,sc.BaseGraphValueFormat).GetChars());

			sc.UseTool(Tool_ProfitTarget);
		}
	}

	//--------------#3 Coloring the Main Price Graph

	SCFloatArray  MACD;
	sc.GetStudyArrayUsingID(SelectMACD.GetStudyID(), 0, MACD);

	SCFloatArray  MACDDiff;
	sc.GetStudyArrayUsingID(SelectMACD.GetStudyID(), 2, MACDDiff);

	//BaseColor 
	ColorBarSubgraph[sc.Index] = 1;
	ColorBarSubgraph.DataColor[sc.Index] = BaseColor.PrimaryColor;

	
	if(MACD[sc.Index] > 0.0 && MACDDiff[sc.Index] > 0.0)
		ColorBarSubgraph.DataColor[sc.Index] = ColorBarSubgraph.PrimaryColor;
	
	else if(MACD[sc.Index] < 0.0 && MACDDiff[sc.Index] < 0.0)
		ColorBarSubgraph.DataColor[sc.Index] = ColorBarSubgraph.SecondaryColor;

	//--------------#4
	SCFloatArray  RSI;
	sc.GetStudyArrayUsingID(SelectRSI.GetStudyID(), 0, RSI);

	SCFloatArray  RSIHigh;
	sc.GetStudyArrayUsingID(SelectRSI.GetStudyID(), 1, RSIHigh);

	SCFloatArray  RSILow;
	sc.GetStudyArrayUsingID(SelectRSI.GetStudyID(), 2, RSILow);

	if(RSI[sc.Index] < RSILow[sc.Index])
		PointOnHigh[sc.Index] = 1;
	else 
		PointOnHigh[sc.Index] = 0;

	if(RSI[sc.Index] > RSIHigh[sc.Index])
		PointOnLow[sc.Index] = 1;
	else
		PointOnLow[sc.Index] = 0;

}

/*==========================================================================*/
SCSFExport scsf_DailyOHLC(SCStudyInterfaceRef sc)
{

	SCInputRef UseThisIntradayChart = sc.Input[0];
	SCInputRef DailyChartNumber = sc.Input[1];
	SCInputRef ReferenceDaysBack = sc.Input[2];
	SCInputRef GraphHighAndLowHistorically = sc.Input[3];
	SCInputRef UseSaturdayData = sc.Input[4];
	SCInputRef NumberOfDaysToCalculate = sc.Input[5];
	SCInputRef RoundToTickSize = sc.Input[6];
	SCInputRef UseDaySessionOnly = sc.Input[7];
	SCInputRef GraphOpenCloseHistorically = sc.Input[8];
	SCInputRef DisplayOnDaySessionOnly = sc.Input [9];
	SCInputRef DisplayTotalDailyVolumeOnly = sc.Input[10];
	SCInputRef DrawZeros = sc.Input[11];
	SCInputRef UsePreviousValuesUntilNewDaySession = sc.Input[12];
	SCInputRef IncludeFridayEveningSessionWithSundayEveningSession = sc.Input[13];
	SCInputRef AlwaysUseSameAsRegionForScaleRange = sc.Input[14];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Daily OHLC";
		
		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.Subgraph[SC_OPEN].Name = "Open";
		sc.Subgraph[SC_OPEN].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_OPEN].PrimaryColor = RGB(255,128,0);
		sc.Subgraph[SC_OPEN].DrawZeros = false;

		sc.Subgraph[SC_HIGH].Name = "High";
		sc.Subgraph[SC_HIGH].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_HIGH].PrimaryColor = RGB(0,255,0);
		sc.Subgraph[SC_HIGH].LineWidth = 2;
		sc.Subgraph[SC_HIGH].DrawZeros = false;

		sc.Subgraph[SC_LOW].Name = "Low";
		sc.Subgraph[SC_LOW].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LOW].PrimaryColor = RGB(255,0,0);
		sc.Subgraph[SC_LOW].LineWidth = 2;
		sc.Subgraph[SC_LOW].DrawZeros = false;

		sc.Subgraph[SC_LAST].Name = "Close";
		sc.Subgraph[SC_LAST].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LAST].PrimaryColor = RGB(128,0,255);
		sc.Subgraph[SC_LAST].DrawZeros = false;

		sc.Subgraph[SC_OHLC_AVG].Name = "OHLC Avg";
		sc.Subgraph[SC_OHLC_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_OHLC_AVG].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_OHLC_AVG].DrawZeros = false;

		sc.Subgraph[SC_HLC_AVG].Name= "HLC Avg";
		sc.Subgraph[SC_HLC_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HLC_AVG].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_HLC_AVG].DrawZeros = false;

		sc.Subgraph[SC_HL_AVG].Name= "HL Avg";
		sc.Subgraph[SC_HL_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HL_AVG].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_HL_AVG].DrawZeros = false;

		sc.Subgraph[SC_VOLUME].Name= "Volume";
		sc.Subgraph[SC_VOLUME].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_VOLUME].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_VOLUME].DrawZeros = false;

		UseThisIntradayChart.Name = "Use this Intraday Chart";
		UseThisIntradayChart.SetYesNo(1);

		DailyChartNumber.Name = "Daily Chart Number";
		DailyChartNumber.SetChartNumber(1);
		
		ReferenceDaysBack.Name = "Reference Days Back";
		ReferenceDaysBack.SetInt(1);
		ReferenceDaysBack.SetIntLimits(0,MAX_STUDY_LENGTH);
		
		GraphHighAndLowHistorically.Name = "Graph High and Low Historically (Reference Days Back=0)";
		GraphHighAndLowHistorically.SetYesNo(0);
		
		UseSaturdayData.Name = "Use Saturday Data";
		UseSaturdayData.SetYesNo(0);
		
		NumberOfDaysToCalculate.Name = "Number of Days To Calculate";
		NumberOfDaysToCalculate.SetInt(50);
		NumberOfDaysToCalculate.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		RoundToTickSize.Name = "Round to Tick Size";
		RoundToTickSize.SetYesNo(0);

		UseDaySessionOnly.Name = "Use Day Session Only";
		UseDaySessionOnly.SetYesNo(0);

		GraphOpenCloseHistorically.Name ="Graph Open and Close Historically (Reference Days Back=0)";
		GraphOpenCloseHistorically.SetYesNo(0);

		DisplayOnDaySessionOnly.Name = "Display on Day Session Only";
		DisplayOnDaySessionOnly.SetYesNo(false);

		DisplayTotalDailyVolumeOnly.Name = "Display Total Daily Volume Only";
		DisplayTotalDailyVolumeOnly.SetYesNo(false);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		UsePreviousValuesUntilNewDaySession.Name = "Use Previous Values Until New Day Session When Based on Day Session Only";
		UsePreviousValuesUntilNewDaySession.SetYesNo(false);

		IncludeFridayEveningSessionWithSundayEveningSession.Name = "Include Friday Evening with Sunday Evening Session";
		IncludeFridayEveningSessionWithSundayEveningSession.SetYesNo(false);

		AlwaysUseSameAsRegionForScaleRange.Name = "Always Use Same As Region For Scale Range";
		AlwaysUseSameAsRegionForScaleRange.SetYesNo(true);

		return;
	}

	int InUseThisIntradayChart = UseThisIntradayChart.GetYesNo();
	int InDailyChartNumber = DailyChartNumber.GetInt();
	int InNumberOfDaysBack = ReferenceDaysBack.GetInt();
	int InGraphHighLowHistorically = GraphHighAndLowHistorically.GetYesNo();

	if (InNumberOfDaysBack > 0)
		InGraphHighLowHistorically = 0;

	int InUseSaturdayData = UseSaturdayData.GetYesNo();
	int InNumberOfDaysToCalculate = NumberOfDaysToCalculate.GetInt();
	int InRoundToTickSize = RoundToTickSize.GetYesNo();

	int InIncludeFridayEveningSessionWithSundayEveningSession = IncludeFridayEveningSessionWithSundayEveningSession.GetYesNo();

	if (AlwaysUseSameAsRegionForScaleRange.GetYesNo())
		sc.ScaleRangeType = SCALE_SAMEASREGION;

	if (DisplayTotalDailyVolumeOnly.GetYesNo()
		&& sc.Subgraph[SC_OPEN].DrawStyle != DRAWSTYLE_IGNORE)
	{
		sc.Subgraph[SC_OPEN].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HIGH].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_LOW].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_LAST].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_OHLC_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HLC_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HL_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_VOLUME].DrawStyle = DRAWSTYLE_DASH;
	}
	else if(!DisplayTotalDailyVolumeOnly.GetYesNo()
		&& sc.Subgraph[SC_OPEN].DrawStyle == DRAWSTYLE_IGNORE
		&& sc.Subgraph[SC_VOLUME].DrawStyle != DRAWSTYLE_IGNORE)
	{
		sc.Subgraph[SC_OPEN].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_HIGH].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LOW].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LAST].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_VOLUME].DrawStyle = DRAWSTYLE_IGNORE;
	}

	for (int Index = SC_OPEN; Index <= SC_HL_AVG; ++Index)
		sc.Subgraph[Index].DrawZeros = DrawZeros.GetYesNo();
	

	float Open = 0.0f, High = 0.0f, Low = 0.0f, Close = 0.0f, Volume = 0.0f;
	int IntradayChartDate = 0;
	int IsValid = 1;

	// we get chart data only once for speed
	SCGraphData DailyChartData;
	SCDateTimeArray DailyChartDateTimes;
	if (!InUseThisIntradayChart)
	{
		sc.GetChartBaseData(InDailyChartNumber, DailyChartData);
		sc.GetChartDateTimeArray(InDailyChartNumber, DailyChartDateTimes);
	}
	
	float HighestHigh = -FLT_MAX, LowestLow = FLT_MAX;
	
	int StartIndex = sc.UpdateStartIndex;
	if (StartIndex != 0)
	{
		SCDateTime TradingDayStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[StartIndex]);

		if (InIncludeFridayEveningSessionWithSundayEveningSession
			&& TradingDayStartDateTime.IsSunday())
		{
			TradingDayStartDateTime -= 2 * DAYS;
		}

		StartIndex = sc.GetFirstIndexForDate(sc.ChartNumber, TradingDayStartDateTime.GetDate());

	}

	//This is used by the Study/Price Overlay study and Spreadsheet study.
	sc.EarliestUpdateSubgraphDataArrayIndex = StartIndex;

	for (int Index = StartIndex; Index < sc.ArraySize; Index++ ) 
	{
		bool IsNewDay = false;

		SCDateTime BarDateTime = sc.BaseDateTimeIn[Index];
		
		int IntradayChartTradingDayDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

		if (InIncludeFridayEveningSessionWithSundayEveningSession
			&& SCDateTime(IntradayChartTradingDayDate).IsSaturday())
		{
			IntradayChartTradingDayDate += (int)(2 * DAYS);
		}

		if (IntradayChartDate != IntradayChartTradingDayDate)
		{
			IsNewDay = true;

			IntradayChartDate = IntradayChartTradingDayDate;

			IsValid = 
				CalculateDailyOHLC(
					sc,
					IntradayChartDate,
					InNumberOfDaysBack,
					InNumberOfDaysToCalculate,
					InUseSaturdayData,
					InUseThisIntradayChart,
					InDailyChartNumber,
					DailyChartData,
					DailyChartDateTimes,
					UseDaySessionOnly.GetYesNo(),
					Open,
					High,
					Low,
					Close,
					Volume,
					InIncludeFridayEveningSessionWithSundayEveningSession
				);
			
			HighestHigh = -FLT_MAX;
			LowestLow = FLT_MAX;
		}
		
		if (!IsValid)
			continue;


		if (InGraphHighLowHistorically)
		{
			if (! UseDaySessionOnly.GetYesNo() || ( UseDaySessionOnly.GetYesNo() 
				&& sc.IsDateTimeInDaySession(BarDateTime)) )
			{
				if (sc.High[Index] > HighestHigh)
					HighestHigh = sc.High[Index];

				if (sc.Low[Index] < LowestLow)
					LowestLow = sc.Low[Index];

				High = HighestHigh;
				Low = LowestLow;
			}
			else
			{
				High = 0.0f;
				Low = 0.0f;
			}
		}

		if (GraphOpenCloseHistorically.GetYesNo())
		{
			Open = sc.Open[Index];
			Close = sc.Close[Index];
		}


		float SubgraphOpen = Open;
		float SubgraphHigh = High;
		float SubgraphLow = Low;
		float SubgraphClose = Close;
		float SubgraphVolume = Volume;

		if (UseDaySessionOnly.GetYesNo() 
			&& UsePreviousValuesUntilNewDaySession.GetYesNo()
			&& !sc.IsDateTimeInDaySession(BarDateTime) 
			&& Index > 0)
		{
			SubgraphOpen = sc.Subgraph[SC_OPEN][Index - 1];
			SubgraphHigh = sc.Subgraph[SC_HIGH][Index - 1];
			SubgraphLow = sc.Subgraph[SC_LOW][Index - 1];
			SubgraphClose = sc.Subgraph[SC_LAST][Index - 1];
		}


		if (InRoundToTickSize)
		{
			SubgraphOpen = (float)sc.RoundToTickSize(SubgraphOpen, sc.TickSize);
			SubgraphHigh = (float)sc.RoundToTickSize(SubgraphHigh, sc.TickSize);
			SubgraphLow = (float)sc.RoundToTickSize(SubgraphLow, sc.TickSize);
			SubgraphClose = (float)sc.RoundToTickSize(SubgraphClose, sc.TickSize);
		}


		if (DisplayOnDaySessionOnly.GetYesNo() && !sc.IsDateTimeInDaySession (BarDateTime) ) 
		{
			SubgraphOpen = 0;
			SubgraphHigh = 0;
			SubgraphLow = 0;
			SubgraphClose = 0;
			SubgraphVolume = 0;

		}

		sc.Subgraph[SC_OPEN][Index] = SubgraphOpen;
		sc.Subgraph[SC_HIGH][Index] = SubgraphHigh;
		sc.Subgraph[SC_LOW][Index] = SubgraphLow;
		sc.Subgraph[SC_LAST][Index] = SubgraphClose;
		sc.Subgraph[SC_VOLUME][Index] = SubgraphVolume;


		sc.CalculateOHLCAverages( Index);
	} 
}

/*==========================================================================*/
SCSFExport scsf_DailyOHLCSinglePoint(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ExtensionLinePropertiesSubgraph = sc.Subgraph[SC_LAST + 1];

	SCInputRef UseThisIntradayChart = sc.Input[0];
	SCInputRef DailyChartNumber = sc.Input[1];
	SCInputRef UseSaturdayData = sc.Input[4];
	SCInputRef NumberOfDaysToCalculate = sc.Input[5];
	SCInputRef UseDaySessionOnly = sc.Input[7];
	SCInputRef DisplayHighLowPriceLabels = sc.Input[8];
	SCInputRef DrawExtensionLines = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Daily OHLC - Single Point";

		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.AutoLoop = 0;//manual looping

		sc.Subgraph[SC_OPEN].Name = "Open";
		sc.Subgraph[SC_OPEN].DrawStyle = DRAWSTYLE_HIDDEN;
		sc.Subgraph[SC_OPEN].PrimaryColor = RGB(255,128,0);
		sc.Subgraph[SC_OPEN].DrawZeros = false;

		sc.Subgraph[SC_HIGH].Name = "High";
		sc.Subgraph[SC_HIGH].DrawStyle = DRAWSTYLE_TRIANGLE_DOWN;
		sc.Subgraph[SC_HIGH].PrimaryColor = RGB(0,255,0);
		sc.Subgraph[SC_HIGH].LineWidth =5;
		sc.Subgraph[SC_HIGH].DrawZeros = false;

		sc.Subgraph[SC_LOW].Name = "Low";
		sc.Subgraph[SC_LOW].DrawStyle = DRAWSTYLE_TRIANGLE_UP;
		sc.Subgraph[SC_LOW].PrimaryColor = RGB(255,0,0);
		sc.Subgraph[SC_LOW].LineWidth =5;
		sc.Subgraph[SC_LOW].DrawZeros = false;

		sc.Subgraph[SC_LAST].Name = "Close";
		sc.Subgraph[SC_LAST].DrawStyle = DRAWSTYLE_HIDDEN;
		sc.Subgraph[SC_LAST].PrimaryColor = RGB(128,0,255);
		sc.Subgraph[SC_LAST].DrawZeros = false;

		ExtensionLinePropertiesSubgraph.Name = "Extension Line Properties";
		ExtensionLinePropertiesSubgraph.DrawStyle = DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY;
		ExtensionLinePropertiesSubgraph.LineWidth = 1;
		ExtensionLinePropertiesSubgraph.PrimaryColor =  RGB (255, 0, 255);
		ExtensionLinePropertiesSubgraph.DrawZeros = false;

		UseThisIntradayChart.Name = "Use this Intraday Chart";
		UseThisIntradayChart.SetYesNo(1);

		DailyChartNumber.Name = "Daily Chart Number";
		DailyChartNumber.SetChartNumber(1);

		UseSaturdayData.Name = "Use Saturday Data";
		UseSaturdayData.SetYesNo(0);

		NumberOfDaysToCalculate.Name = "Number of Days To Calculate";
		NumberOfDaysToCalculate.SetInt(50);
		NumberOfDaysToCalculate.SetIntLimits(1, MAX_STUDY_LENGTH);

		UseDaySessionOnly.Name = "Use Day Session Only";
		UseDaySessionOnly.SetYesNo(0);

		DisplayHighLowPriceLabels.Name = "Display High/Low Price Labels";
		DisplayHighLowPriceLabels.SetYesNo(0);

		DrawExtensionLines.Name = "Draw Extension Lines";
		DrawExtensionLines.SetYesNo(false);

		return;
	}

	int InUseThisIntradayChart = UseThisIntradayChart.GetYesNo();
	int InDailyChartNumber = DailyChartNumber.GetInt();
	int InUseSaturdayData = UseSaturdayData.GetYesNo();
	int InNumberOfDaysToCalculate = NumberOfDaysToCalculate.GetInt();
	int InUseDaySessionOnly = UseDaySessionOnly.GetYesNo();
	int DisplayPriceLabels = DisplayHighLowPriceLabels.GetYesNo();
	int InDrawExtensionLines = DrawExtensionLines.GetYesNo();

	float Open = 0.0f, High = 0.0f, Low = 0.0f, Close = 0.0f, Volume = 0.0f;
	int IntradayChartDate = 0;
	int IsValid = 1;


	int& IndexOfMostRecentHigh = sc.GetPersistentInt(1);
	int& IndexOfMostRecentLow = sc.GetPersistentInt(2);
	int& LineNumberOfMostRecentHighValue = sc.GetPersistentInt(3);
	int& LineNumberOfMostRecentLowValue = sc.GetPersistentInt(4);

	// we get chart data only once for speed
	SCGraphData DailyChartData;
	SCDateTimeArray DailyChartDateTime;

	if (!InUseThisIntradayChart)
	{
		sc.GetChartBaseData(InDailyChartNumber, DailyChartData);
		sc.GetChartDateTimeArray(InDailyChartNumber, DailyChartDateTime);
	}

	if (sc.IsFullRecalculation)
	{
		IndexOfMostRecentHigh = -1;
		IndexOfMostRecentLow =  -1;
		LineNumberOfMostRecentHighValue = 0;
		LineNumberOfMostRecentLowValue = 0;
	}


	int StartIndex = sc.UpdateStartIndex;
	if (StartIndex != 0)
	{
		SCDateTime TradingDayStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[StartIndex]);
		StartIndex = sc.GetFirstIndexForDate(sc.ChartNumber, TradingDayStartDateTime.GetDate());
	}

	for (int Index = StartIndex; Index < sc.ArraySize; Index++ ) 
	{

		if (IntradayChartDate != sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]))
		{
			// A new day has been entered, existing text drawings and extension lines need to remain. So remove the reference to those.
			if (IndexOfMostRecentHigh < Index || IndexOfMostRecentLow < Index)
			{
				IndexOfMostRecentHigh = -1;
				IndexOfMostRecentLow = -1;
				LineNumberOfMostRecentHighValue = 0;
				LineNumberOfMostRecentLowValue = 0;
			}

			IntradayChartDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

			IsValid = 
				CalculateDailyOHLC(
				sc,
				IntradayChartDate,
				0,
				InNumberOfDaysToCalculate,
				InUseSaturdayData,
				InUseThisIntradayChart,
				InDailyChartNumber,
				DailyChartData,
				DailyChartDateTime,
				InUseDaySessionOnly,
				Open,
				High,
				Low,
				Close,
				Volume,
				0
				);

		}

		if (!IsValid)
			continue;


		if(sc.FormattedEvaluate(Open,sc.BaseGraphValueFormat,EQUAL_OPERATOR,sc.Open[Index], sc.BaseGraphValueFormat))
			sc.Subgraph[SC_OPEN][Index] = Open;
		else
			sc.Subgraph[SC_OPEN][Index] = 0.0;

		if(sc.FormattedEvaluate(High,sc.BaseGraphValueFormat,EQUAL_OPERATOR,sc.High[Index], sc.BaseGraphValueFormat))
		{
			sc.Subgraph[SC_HIGH][Index] = High;
			

			if (DisplayPriceLabels)
			{
				s_UseTool Tool;
				Tool.ChartNumber = sc.ChartNumber;
				Tool.DrawingType = DRAWING_TEXT;
				Tool.Region		 = sc.GraphRegion;

				if (LineNumberOfMostRecentHighValue != 0)
					Tool.LineNumber  = LineNumberOfMostRecentHighValue;

				Tool.BeginIndex  = Index;
				Tool.BeginValue	 = High;

				Tool.Text = "  ";
				Tool.Text += sc.FormatGraphValue(High, sc.BaseGraphValueFormat);
				Tool.Color = sc.Subgraph[SC_HIGH].PrimaryColor;
				Tool.TextAlignment	= DT_LEFT | DT_VCENTER;

				Tool.AddMethod = UTAM_ADD_OR_ADJUST;

				sc.UseTool(Tool);

				LineNumberOfMostRecentHighValue = Tool.LineNumber;
			}

			if (InDrawExtensionLines)
			{
				if (IndexOfMostRecentHigh != -1)//First delete the existing line
					sc.DeleteLineUntilFutureIntersection(IndexOfMostRecentHigh, 1);

				sc.AddLineUntilFutureIntersection
					( Index
					, 1
					, High
					, ExtensionLinePropertiesSubgraph.PrimaryColor
					, ExtensionLinePropertiesSubgraph.LineWidth
					, ExtensionLinePropertiesSubgraph.LineStyle
					, false
					, false
					, ""
					);
			}

			IndexOfMostRecentHigh = Index;
		}
		else
			sc.Subgraph[SC_HIGH][Index] = 0.0;


		if(sc.FormattedEvaluate(Low,sc.BaseGraphValueFormat,EQUAL_OPERATOR,sc.Low[Index], sc.BaseGraphValueFormat))
		{
			sc.Subgraph[SC_LOW][Index] = Low;

			if (DisplayPriceLabels)
			{
				s_UseTool Tool;
				Tool.ChartNumber = sc.ChartNumber;
				Tool.DrawingType = DRAWING_TEXT;
				Tool.Region		 = sc.GraphRegion;

				if (LineNumberOfMostRecentLowValue != 0)
					Tool.LineNumber  = LineNumberOfMostRecentLowValue;

				Tool.BeginIndex  = Index;
				Tool.BeginValue	 = Low;

				Tool.Text = "  ";
				Tool.Text += sc.FormatGraphValue(Low, sc.BaseGraphValueFormat);
				Tool.Color = sc.Subgraph[SC_LOW].PrimaryColor;
				Tool.TextAlignment	= DT_LEFT | DT_VCENTER;

				Tool.AddMethod = UTAM_ADD_OR_ADJUST;

				sc.UseTool(Tool);

				LineNumberOfMostRecentLowValue = Tool.LineNumber;
			}

			if (InDrawExtensionLines)
			{
				if (IndexOfMostRecentLow != -1)//First delete the existing line
					sc.DeleteLineUntilFutureIntersection(IndexOfMostRecentLow, 2);

				sc.AddLineUntilFutureIntersection
					( Index
					, 2
					, Low
					, ExtensionLinePropertiesSubgraph.PrimaryColor
					, ExtensionLinePropertiesSubgraph.LineWidth
					, ExtensionLinePropertiesSubgraph.LineStyle
					, false
					, false
					, ""
					);
			}

			IndexOfMostRecentLow = Index;
		}
		else
			sc.Subgraph[SC_LOW][Index] = 0.0;


		if(sc.FormattedEvaluate(Close,sc.BaseGraphValueFormat,EQUAL_OPERATOR,sc.Close[Index], sc.BaseGraphValueFormat))
			sc.Subgraph[SC_LAST][Index] = Close;
		else
			sc.Subgraph[SC_LAST][Index] = 0.0;
	} 
}

/*==========================================================================*/
SCSFExport scsf_PivotPointsDaily(SCStudyInterfaceRef sc)
{
	SCSubgraphRef R1 = sc.Subgraph[0];
	SCSubgraphRef R2 = sc.Subgraph[1];
	SCSubgraphRef S1 = sc.Subgraph[2];
	SCSubgraphRef S2 = sc.Subgraph[3];
	SCSubgraphRef R0_5 = sc.Subgraph[4];
	SCSubgraphRef R1_5 = sc.Subgraph[5];
	SCSubgraphRef R2_5 = sc.Subgraph[6];
	SCSubgraphRef R3 = sc.Subgraph[7];
	SCSubgraphRef S0_5 = sc.Subgraph[8];
	SCSubgraphRef S1_5 = sc.Subgraph[9];
	SCSubgraphRef S2_5 = sc.Subgraph[10];
	SCSubgraphRef S3 = sc.Subgraph[11];
	SCSubgraphRef PP = sc.Subgraph[12];
	SCSubgraphRef R4 = sc.Subgraph[13];
	SCSubgraphRef S4 = sc.Subgraph[14];
	SCSubgraphRef R3_5 = sc.Subgraph[15];
	SCSubgraphRef S3_5 = sc.Subgraph[16];
	SCSubgraphRef R5 = sc.Subgraph[17];
	SCSubgraphRef S5 = sc.Subgraph[18];
	SCSubgraphRef R6 = sc.Subgraph[19];
	SCSubgraphRef S6 = sc.Subgraph[20];
	SCSubgraphRef R7 = sc.Subgraph[21];
	SCSubgraphRef S7 = sc.Subgraph[22];
	SCSubgraphRef R8 = sc.Subgraph[23];
	SCSubgraphRef S8 = sc.Subgraph[24];
	SCSubgraphRef R9 = sc.Subgraph[25];
	SCSubgraphRef S9 = sc.Subgraph[26];
	SCSubgraphRef R10 = sc.Subgraph[27];
	SCSubgraphRef S10 = sc.Subgraph[28];
	SCSubgraphRef R4_5 = sc.Subgraph[29];
	SCSubgraphRef S4_5 = sc.Subgraph[30];
	const int NUMBER_OF_STUDY_SUBGRAPHS = 31;
	
	SCInputRef FormulaType = sc.Input[4];
	SCInputRef NumberOfDays = sc.Input[5];
	SCInputRef RoundToTickSize = sc.Input[6];
	SCInputRef UseSaturdayData = sc.Input[7];
	SCInputRef DailyChartNumber = sc.Input[9];//previously input 3
	SCInputRef ReferenceDailyChartForData = sc.Input[10];
	SCInputRef ForwardProjectLines = sc.Input[11];
	SCInputRef UseManualValues = sc.Input[12];
	SCInputRef UserOpen = sc.Input[13];
	SCInputRef UserHigh = sc.Input[14];
	SCInputRef UserLow = sc.Input[15];
	SCInputRef UserClose = sc.Input[16];
	SCInputRef UseDaySessionOnly = sc.Input[17];
	SCInputRef Version = sc.Input[18];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Pivot Points-Daily";
		
		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		R1.Name = "R1";
		R2.Name = "R2";
		S1.Name = "S1";
		S2.Name = "S2";
		R0_5.Name = "R.5";
		R1_5.Name = "R1.5";
		R2_5.Name = "R2.5";
		R3.Name = "R3";
		S0_5.Name = "S.5";
		S1_5.Name = "S1.5";
		S2_5.Name = "S2.5";
		S3.Name = "S3";
		PP.Name = "PP";
		R4.Name = "R4";
		S4.Name = "S4";
		R3_5.Name = "R3.5";
		S3_5.Name = "S3.5";
		R5.Name = "R5";
		S5.Name = "S5";
		R6.Name = "R6";
		S6.Name = "S6";
		R7.Name = "R7";
		S7.Name = "S7";
		R8.Name = "R8";
		S8.Name = "S8";
		R9.Name = "R9";
		S9.Name = "S9";
		R10.Name = "R10";
		S10.Name = "S10";
		R4_5.Name = "R4_5";
		S4_5.Name = "S4_5";

		R1.DrawStyle = DRAWSTYLE_DASH;
		R2.DrawStyle = DRAWSTYLE_DASH;
		S1.DrawStyle = DRAWSTYLE_DASH;
		S2.DrawStyle = DRAWSTYLE_DASH;
		R0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R3.DrawStyle = DRAWSTYLE_DASH;
		S0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S3.DrawStyle = DRAWSTYLE_DASH;
		PP.DrawStyle = DRAWSTYLE_DASH;
		R4.DrawStyle = DRAWSTYLE_HIDDEN;
		S4.DrawStyle = DRAWSTYLE_HIDDEN;
		R3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R5.DrawStyle = DRAWSTYLE_HIDDEN;
		S5.DrawStyle = DRAWSTYLE_HIDDEN;
		R6.DrawStyle = DRAWSTYLE_HIDDEN;
		S6.DrawStyle = DRAWSTYLE_HIDDEN;
		R7.DrawStyle = DRAWSTYLE_HIDDEN;
		S7.DrawStyle = DRAWSTYLE_HIDDEN;
		R8.DrawStyle = DRAWSTYLE_HIDDEN;
		S8.DrawStyle = DRAWSTYLE_HIDDEN;
		R9.DrawStyle = DRAWSTYLE_HIDDEN;
		S9.DrawStyle = DRAWSTYLE_HIDDEN;
		R10.DrawStyle = DRAWSTYLE_HIDDEN;
		S10.DrawStyle = DRAWSTYLE_HIDDEN;
		R4_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S4_5.DrawStyle = DRAWSTYLE_HIDDEN;

		
		R1.DrawZeros = false;
		R2.DrawZeros = false;
		S1.DrawZeros = false;
		S2.DrawZeros = false;
		R0_5.DrawZeros = false;
		R1_5.DrawZeros = false;
		R2_5.DrawZeros = false;
		R3.DrawZeros = false;
		S0_5.DrawZeros = false;
		S1_5.DrawZeros = false;
		S2_5.DrawZeros = false;
		S3.DrawZeros = false;
		PP.DrawZeros = false;
		R4.DrawZeros = false;
		S4.DrawZeros = false;
		R3_5.DrawZeros = false;
		S3_5.DrawZeros = false;
		R5.DrawZeros = false;
		S5.DrawZeros = false;
		R6.DrawZeros = false;
		S6.DrawZeros = false;
		R7.DrawZeros = false;
		S7.DrawZeros = false;
		R8.DrawZeros = false;
		S8.DrawZeros = false;
		R9.DrawZeros = false;
		S9.DrawZeros = false;
		R10.DrawZeros = false;
		S10.DrawZeros = false;
		R4_5.DrawZeros = false;
		S4_5.DrawZeros = false;
		

		PP.PrimaryColor = RGB(255, 0, 255);

		R1.PrimaryColor = RGB(255, 0, 0);
		R2.PrimaryColor = RGB(255, 0, 0);
		R0_5.PrimaryColor = RGB(255, 0, 0);
		R1_5.PrimaryColor = RGB(255, 0, 0);
		R2_5.PrimaryColor = RGB(255, 0, 0);
		R3.PrimaryColor = RGB(255, 0, 0);
		R4.PrimaryColor = RGB(255, 0, 0);
		R4_5.PrimaryColor = RGB(255, 0, 0);
		R3_5.PrimaryColor = RGB(255, 0, 0);
		R5.PrimaryColor = RGB(255, 0, 0);
		R6.PrimaryColor = RGB(255, 0, 0);
		R7.PrimaryColor = RGB(255, 0, 0);
		R8.PrimaryColor = RGB(255, 0, 0);
		R9.PrimaryColor = RGB(255, 0, 0);
		R10.PrimaryColor = RGB(255, 0, 0);

		S1.PrimaryColor = RGB(0, 255, 0);
		S2.PrimaryColor = RGB(0, 255, 0);
		S0_5.PrimaryColor = RGB(0, 255, 0);
		S1_5.PrimaryColor = RGB(0, 255, 0);
		S2_5.PrimaryColor = RGB(0, 255, 0);
		S3.PrimaryColor = RGB(0, 255, 0);
		S4.PrimaryColor = RGB(0, 255, 0);
		S4_5.PrimaryColor = RGB(0, 255, 0);
		S3_5.PrimaryColor = RGB(0, 255, 0);
		S5.PrimaryColor = RGB(0, 255, 0);
		S6.PrimaryColor = RGB(0, 255, 0);
		S7.PrimaryColor = RGB(0, 255, 0);
		S8.PrimaryColor = RGB(0, 255, 0);
		S9.PrimaryColor = RGB(0, 255, 0);
		S10.PrimaryColor = RGB(0, 255, 0);

		for (int SubgraphIndex = 0; SubgraphIndex < NUMBER_OF_STUDY_SUBGRAPHS; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex].LineLabel = LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		sc.Input[3].SetChartNumber(1);

		DailyChartNumber.Name = "Daily Chart Number";
		DailyChartNumber.SetChartNumber(1);

		FormulaType.Name = "Formula Type";
		FormulaType.SetInt(0);

		NumberOfDays.Name = "Number of Days To Calculate";
		NumberOfDays.SetInt(50);
		NumberOfDays.SetIntLimits(1,MAX_STUDY_LENGTH);

		RoundToTickSize.Name = "Round to Tick Size";
		RoundToTickSize.SetYesNo(0);

		UseSaturdayData.Name = "Use Saturday Data";
		UseSaturdayData.SetYesNo(0);

		ReferenceDailyChartForData.Name = "Reference Daily Chart For Data";
		ReferenceDailyChartForData.SetYesNo(false);

		ForwardProjectLines.Name ="Forward Project Pivot Point Lines";
		ForwardProjectLines.SetYesNo(0);
		
		UseManualValues.Name = "Use User Entered OHLC Values";
		UseManualValues.SetYesNo(0);
		
		UserOpen.Name = "User Entered Open Value";
		UserOpen.SetFloat(0.0f);
		
		UserHigh.Name = "User Entered High Value";
		UserHigh.SetFloat(0.0f);
		
		UserLow.Name = "User Entered Low Value";
		UserLow.SetFloat(0.0f);
		
		UserClose.Name = "User Entered Close Value";
		UserClose.SetFloat(0.0f);

		UseDaySessionOnly.Name = "Use Day Session Only";
		UseDaySessionOnly.SetYesNo(false);

		Version.SetInt(2);
		
		return;
	}
	//Upgrade code
	if (Version.GetInt()<2)
	{
		Version.SetInt(2);
		DailyChartNumber.SetInt(sc.Input[3].GetInt() );
		ReferenceDailyChartForData .SetYesNo(!ReferenceDailyChartForData .GetYesNo());
	}

	float fPivotPoint= 0;

	float fR0_5 = 0, fR1 = 0, fR1_5 = 0, fR2 = 0, fR2_5 = 0, fR3 = 0, fR3_5 = 0, fR4 = 0, fR4_5 = 0, fR5 = 0, fR6 = 0, fR7 = 0, fR8 = 0, fR9 = 0, fR10 = 0;
	float fS0_5 = 0, fS1 = 0, fS1_5 = 0, fS2 = 0, fS2_5 = 0, fS3 = 0, fS3_5 = 0, fS4 = 0, fS4_5 = 0, fS5 = 0, fS6 = 0, fS7 = 0, fS8 = 0, fS9 = 0, fS10 = 0;
	
	int IntradayChartDate = 0;
	int ValidPivotPoint = 1;

	// we get chart data only once for speed
	SCGraphData DailyChartData;
	SCDateTimeArray DailyChartDateTime;
	if (ReferenceDailyChartForData.GetYesNo() )
	{
		sc.GetChartBaseData(DailyChartNumber.GetChartNumber(), DailyChartData);
		sc.GetChartDateTimeArray(DailyChartNumber.GetChartNumber(), DailyChartDateTime);
	}

	int NumberOfForwardBars = 0;

	if(ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = 20;

		if(sc.UpdateStartIndex == 0)
		{
			for (int SubgraphIndex = 0; SubgraphIndex < NUMBER_OF_STUDY_SUBGRAPHS; SubgraphIndex++)
				sc.Subgraph[SubgraphIndex].ExtendedArrayElementsToGraph = NumberOfForwardBars;
		}
	}

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize+NumberOfForwardBars; Index++ ) 
	{
		if (IntradayChartDate != sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]))
		{
			IntradayChartDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);

			ValidPivotPoint = 
				CalculateDailyPivotPoints(
					sc,
					IntradayChartDate,
					FormulaType.GetInt(),
					DailyChartNumber.GetChartNumber(),
					DailyChartData,
					DailyChartDateTime,
					NumberOfDays.GetInt(),
					UseSaturdayData.GetYesNo(),
					ReferenceDailyChartForData.GetYesNo() ? 0 : 1,
					UseManualValues.GetYesNo(),
					UserOpen.GetFloat(),
					UserHigh.GetFloat(),
					UserLow.GetFloat(),
					UserClose.GetFloat(),
					UseDaySessionOnly.GetYesNo(),
					fPivotPoint,
					fR0_5,
					fR1, fR1_5,
					fR2, fR2_5,
					fR3,
					fS0_5,
					fS1, fS1_5,
					fS2, fS2_5,
					fS3,
					fR3_5,
					fS3_5,
					fR4,
					fR4_5,
					fS4,
					fS4_5,
					fR5,
					fS5,
					fR6,
					fS6,
					fR7,
					fS7,
					fR8,
					fS8,
					fR9,
					fS9,
					fR10,
					fS10
				);
		}

		if (!ValidPivotPoint)
			continue;
		
		if (RoundToTickSize.GetYesNo() != 0)
		{
			R1[Index] = (float)sc.RoundToTickSize(fR1, sc.TickSize);
			R2[Index] = (float)sc.RoundToTickSize(fR2, sc.TickSize);
			S1[Index] = (float)sc.RoundToTickSize(fS1, sc.TickSize);
			S2[Index] = (float)sc.RoundToTickSize(fS2, sc.TickSize);

			R0_5[Index] = (float)sc.RoundToTickSize(fR0_5, sc.TickSize);
			R1_5[Index] = (float)sc.RoundToTickSize(fR1_5, sc.TickSize);
			R2_5[Index] = (float)sc.RoundToTickSize(fR2_5, sc.TickSize);
			R3[Index] = (float)sc.RoundToTickSize(fR3, sc.TickSize);
			S0_5[Index] = (float)sc.RoundToTickSize(fS0_5, sc.TickSize);
			S1_5[Index] = (float)sc.RoundToTickSize(fS1_5, sc.TickSize);
			S2_5[Index] = (float)sc.RoundToTickSize(fS2_5, sc.TickSize);
			S3[Index] = (float)sc.RoundToTickSize(fS3, sc.TickSize);
			PP[Index] = (float)sc.RoundToTickSize(fPivotPoint, sc.TickSize);
			R4[Index] = (float)sc.RoundToTickSize(fR4, sc.TickSize);
			R4_5[Index] = (float)sc.RoundToTickSize(fR4_5, sc.TickSize);
			S4[Index] = (float)sc.RoundToTickSize(fS4, sc.TickSize);
			S4_5[Index] = (float)sc.RoundToTickSize(fS4_5, sc.TickSize);
			R3_5[Index] = (float)sc.RoundToTickSize(fR3_5, sc.TickSize);
			S3_5[Index] = (float)sc.RoundToTickSize(fS3_5, sc.TickSize);
			R5[Index] = (float)sc.RoundToTickSize(fR5, sc.TickSize);
			S5[Index] = (float)sc.RoundToTickSize(fS5, sc.TickSize);
			R6[Index] = (float)sc.RoundToTickSize(fR6, sc.TickSize);
			S6[Index] = (float)sc.RoundToTickSize(fS6, sc.TickSize);
			R7[Index] = (float)sc.RoundToTickSize(fR7, sc.TickSize);
			S7[Index] = (float)sc.RoundToTickSize(fS7, sc.TickSize);
			R8[Index] = (float)sc.RoundToTickSize(fR8, sc.TickSize);
			S8[Index] = (float)sc.RoundToTickSize(fS8, sc.TickSize);
			R9[Index] = (float)sc.RoundToTickSize(fR9, sc.TickSize);
			S9[Index] = (float)sc.RoundToTickSize(fS9, sc.TickSize);
			R10[Index] = (float)sc.RoundToTickSize(fR10, sc.TickSize);
			S10[Index] = (float)sc.RoundToTickSize(fS10, sc.TickSize);
		}
		else 
		{
			R1[Index] = fR1;
			R2[Index] = fR2;
			S1[Index] = fS1;
			S2[Index] = fS2;

			R0_5[Index] = fR0_5;
			R1_5[Index] = fR1_5;
			R2_5[Index] = fR2_5;
			R3[Index] = fR3;
			S0_5[Index] = fS0_5;
			S1_5[Index] = fS1_5;
			S2_5[Index] = fS2_5;
			S3[Index] = fS3;
			PP[Index] = fPivotPoint;
			R4[Index] = fR4;
			R4_5[Index] = fR4_5;
			S4[Index] = fS4;
			S4_5[Index] = fS4_5;
			R3_5[Index] = fR3_5;
			S3_5[Index] = fS3_5;
			R5[Index] = fR5;
			S5[Index] = fS5;
			R6[Index] = fR6;
			S6[Index] = fS6;
			R7[Index] = fR7;
			S7[Index] = fS7;
			R8[Index] = fR8;
			S8[Index] = fS8;
			R9[Index] = fR9;
			S9[Index] = fS9;
			R10[Index] = fR10;
			S10[Index] = fS10;
		}
	} 
}

/*==========================================================================*/

SCSFExport scsf_PivotPointsVariablePeriod(SCStudyInterfaceRef sc)
{
	SCSubgraphRef R1 = sc.Subgraph[0];
	SCSubgraphRef R2 = sc.Subgraph[1];
	SCSubgraphRef S1 = sc.Subgraph[2];
	SCSubgraphRef S2 = sc.Subgraph[3];
	SCSubgraphRef R0_5 = sc.Subgraph[4];
	SCSubgraphRef R1_5 = sc.Subgraph[5];
	SCSubgraphRef R2_5 = sc.Subgraph[6];
	SCSubgraphRef R3 = sc.Subgraph[7];
	SCSubgraphRef S0_5 = sc.Subgraph[8];
	SCSubgraphRef S1_5 = sc.Subgraph[9];
	SCSubgraphRef S2_5 = sc.Subgraph[10];
	SCSubgraphRef S3 = sc.Subgraph[11];
	SCSubgraphRef PP = sc.Subgraph[12];
	SCSubgraphRef R4 = sc.Subgraph[13];
	SCSubgraphRef S4 = sc.Subgraph[14];
	SCSubgraphRef R3_5 = sc.Subgraph[15];
	SCSubgraphRef S3_5 = sc.Subgraph[16];
	SCSubgraphRef R5 = sc.Subgraph[17];
	SCSubgraphRef S5 = sc.Subgraph[18];
	SCSubgraphRef R6 = sc.Subgraph[19];
	SCSubgraphRef S6 = sc.Subgraph[20];
	SCSubgraphRef R7 = sc.Subgraph[21];
	SCSubgraphRef S7 = sc.Subgraph[22];
	SCSubgraphRef R8 = sc.Subgraph[23];
	SCSubgraphRef S8 = sc.Subgraph[24];
	SCSubgraphRef R9 = sc.Subgraph[25];
	SCSubgraphRef S9 = sc.Subgraph[26];
	SCSubgraphRef R10 = sc.Subgraph[27];
	SCSubgraphRef S10 = sc.Subgraph[28];
	SCSubgraphRef R4_5 = sc.Subgraph[29];
	SCSubgraphRef S4_5 = sc.Subgraph[30];
	const int NUMBER_OF_STUDY_SUBGRAPHS = 31;

	SCInputRef TimePeriodType = sc.Input[0];
	SCInputRef TimePeriodLength = sc.Input[1];
	SCInputRef FormulaType = sc.Input[2];
	SCInputRef MinimumRequiredTimePeriodAsPercent = sc.Input[3];
	SCInputRef RoundToTickSize = sc.Input[4];
	SCInputRef DisplayDebuggingOutput = sc.Input[5];
	SCInputRef ForwardProjectLines = sc.Input[6];
	SCInputRef NumberOfDaysToCalculate = sc.Input[7];
	SCInputRef AutoSkipPeriodOfNoTrading = sc.Input[8];
	SCInputRef NumberForwardProjectionBars = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Pivot Points-Variable Period";

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 0;

		R1.Name = "R1";
		R2.Name = "R2";
		S1.Name = "S1";
		S2.Name = "S2";
		R0_5.Name = "R.5";
		R1_5.Name = "R1.5";
		R2_5.Name = "R2.5";
		R3.Name = "R3";
		S0_5.Name = "S.5";
		S1_5.Name = "S1.5";
		S2_5.Name = "S2.5";
		S3.Name = "S3";
		PP.Name = "PP";
		R4.Name = "R4";
		S4.Name = "S4";
		R3_5.Name = "R3.5";
		S3_5.Name = "S3.5";
		R5.Name = "R5";
		S5.Name = "S5";
		R6.Name = "R6";
		S6.Name = "S6";
		R7.Name = "R7";
		S7.Name = "S7";
		R8.Name = "R8";
		S8.Name = "S8";
		R9.Name = "R9";
		S9.Name = "S9";
		R10.Name = "R10";
		S10.Name = "S10";
		R4_5.Name = "R4.5";
		S4_5.Name = "S4.5";

		R1.DrawStyle = DRAWSTYLE_DASH;
		R2.DrawStyle = DRAWSTYLE_DASH;
		S1.DrawStyle = DRAWSTYLE_DASH;
		S2.DrawStyle = DRAWSTYLE_DASH;
		R0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R3.DrawStyle = DRAWSTYLE_DASH;
		S0_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S1_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S2_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S3.DrawStyle = DRAWSTYLE_DASH;
		PP.DrawStyle = DRAWSTYLE_DASH;
		R4.DrawStyle = DRAWSTYLE_HIDDEN;
		S4.DrawStyle = DRAWSTYLE_HIDDEN;
		R3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S3_5.DrawStyle = DRAWSTYLE_HIDDEN;
		R5.DrawStyle = DRAWSTYLE_HIDDEN;
		S5.DrawStyle = DRAWSTYLE_HIDDEN;
		R6.DrawStyle = DRAWSTYLE_HIDDEN;
		S6.DrawStyle = DRAWSTYLE_HIDDEN;
		R7.DrawStyle = DRAWSTYLE_HIDDEN;
		S7.DrawStyle = DRAWSTYLE_HIDDEN;
		R8.DrawStyle = DRAWSTYLE_HIDDEN;
		S8.DrawStyle = DRAWSTYLE_HIDDEN;
		R9.DrawStyle = DRAWSTYLE_HIDDEN;
		S9.DrawStyle = DRAWSTYLE_HIDDEN;
		R10.DrawStyle = DRAWSTYLE_HIDDEN;
		S10.DrawStyle = DRAWSTYLE_HIDDEN;
		R4_5.DrawStyle = DRAWSTYLE_HIDDEN;
		S4_5.DrawStyle = DRAWSTYLE_HIDDEN;

		
		R1.DrawZeros = false;
		R2.DrawZeros = false;
		S1.DrawZeros = false;
		S2.DrawZeros = false;
		R0_5.DrawZeros = false;
		R1_5.DrawZeros = false;
		R2_5.DrawZeros = false;
		R3.DrawZeros = false;
		S0_5.DrawZeros = false;
		S1_5.DrawZeros = false;
		S2_5.DrawZeros = false;
		S3.DrawZeros = false;
		PP.DrawZeros = false;
		R4.DrawZeros = false;
		S4.DrawZeros = false;
		R3_5.DrawZeros = false;
		S3_5.DrawZeros = false;
		R5.DrawZeros = false;
		S5.DrawZeros = false;
		R6.DrawZeros = false;
		S6.DrawZeros = false;
		R7.DrawZeros = false;
		S7.DrawZeros = false;
		R8.DrawZeros = false;
		S8.DrawZeros = false;
		R9.DrawZeros = false;
		S9.DrawZeros = false;
		R10.DrawZeros = false;
		S10.DrawZeros = false;
		R4_5.DrawZeros = false;
		S4_5.DrawZeros = false;

		PP.PrimaryColor = RGB(255,0,255);

		R1.PrimaryColor = RGB(255, 0, 0);
		R2.PrimaryColor = RGB(255, 0, 0);
		R0_5.PrimaryColor = RGB(255, 0, 0);
		R1_5.PrimaryColor = RGB(255, 0, 0);
		R2_5.PrimaryColor = RGB(255, 0, 0);
		R3.PrimaryColor = RGB(255, 0, 0);
		R3_5.PrimaryColor = RGB(255, 0, 0);
		R4.PrimaryColor = RGB(255, 0, 0);
		R4_5.PrimaryColor = RGB(255, 0, 0);
		R5.PrimaryColor = RGB(255, 0, 0);
		R6.PrimaryColor = RGB(255, 0, 0);
		R7.PrimaryColor = RGB(255, 0, 0);
		R8.PrimaryColor = RGB(255, 0, 0);
		R9.PrimaryColor = RGB(255, 0, 0);
		R10.PrimaryColor = RGB(255, 0, 0);

		S1.PrimaryColor = RGB(0, 255, 0);
		S2.PrimaryColor = RGB(0, 255, 0);
		S0_5.PrimaryColor = RGB(0, 255, 0);
		S1_5.PrimaryColor = RGB(0, 255, 0);
		S2_5.PrimaryColor = RGB(0, 255, 0);
		S3.PrimaryColor = RGB(0, 255, 0);
		S3_5.PrimaryColor = RGB(0, 255, 0);
		S4.PrimaryColor = RGB(0, 255, 0);
		S4_5.PrimaryColor = RGB(0, 255, 0);
		S5.PrimaryColor = RGB(0, 255, 0);
		S6.PrimaryColor = RGB(0, 255, 0);
		S7.PrimaryColor = RGB(0, 255, 0);
		S8.PrimaryColor = RGB(0, 255, 0);
		S9.PrimaryColor = RGB(0, 255, 0);
		S10.PrimaryColor = RGB(0, 255, 0);		

		for (int Index = 0; Index < NUMBER_OF_STUDY_SUBGRAPHS; Index++)
		{
			sc.Subgraph[Index].LineLabel = 
				LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_LEFT | 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_MINUTES);

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(60);
		TimePeriodLength.SetIntLimits(1, 7*MINUTES_PER_DAY);

		FormulaType.Name = "Formula Type";
		FormulaType.SetInt(0);

		MinimumRequiredTimePeriodAsPercent.Name = "Minimum Required Time Period as %";
		MinimumRequiredTimePeriodAsPercent.SetFloat(25.0f);
		MinimumRequiredTimePeriodAsPercent.SetFloatLimits(1.0f, 100.0f);

		RoundToTickSize.Name = "Round to Tick Size";
		RoundToTickSize.SetYesNo(0);

		DisplayDebuggingOutput.Name = "Display Debugging Output (slows study calculations)";
		DisplayDebuggingOutput.SetYesNo(0);

		ForwardProjectLines.Name ="Forward Project Pivot Point Lines";
		ForwardProjectLines.SetYesNo(0);

		NumberOfDaysToCalculate.Name = "Number Of Days To Calculate";
		NumberOfDaysToCalculate.SetInt(1000);

		AutoSkipPeriodOfNoTrading.Name = "Auto Skip Period Of No Trading";
		AutoSkipPeriodOfNoTrading.SetYesNo(false);

		NumberForwardProjectionBars.Name = "Number of Forward Project Bars";
		NumberForwardProjectionBars.SetInt(20);
		NumberForwardProjectionBars.SetIntLimits(1,200);
		return;
	}

	float f_PivotPoint= 0;

	float fR0_5 = 0, fR1 = 0, fR1_5 = 0, fR2 = 0, fR2_5 = 0, fR3 = 0, fR3_5 = 0, fR4 = 0, fR4_5 = 0, fR5 = 0, fR6 = 0, fR7 = 0, fR8 = 0, fR9 = 0, fR10 = 0;
	
	float fS0_5 = 0, fS1 = 0, fS1_5 = 0, fS2 = 0, fS2_5 = 0, fS3 = 0, fS3_5 = 0, fS4 = 0, fS4_5 = 0, fS5 = 0, fS6 = 0, fS7 = 0, fS8 = 0, fS9 = 0, fS10 = 0;

	int ValidPivotPoint = 1;

	int PeriodLength = TimePeriodLength.GetInt();

	int NumberOfForwardBars = 0;

	if (ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = NumberForwardProjectionBars.GetInt();

		if(sc.UpdateStartIndex == 0)
		{
			PP.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R1.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S1.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R0_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R1_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R2_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R3.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S0_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S1_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S2_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S3.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R4.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S4.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R3_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S3_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R6.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S6.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R7.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S7.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R8.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S8.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R9.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S9.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R10.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S10.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			R4_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			S4_5.ExtendedArrayElementsToGraph = NumberOfForwardBars;
		}
	}

	if (NumberOfDaysToCalculate.GetInt() < 1)
		NumberOfDaysToCalculate.SetInt(1000);

	int LastDateInChart = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();
	int FirstDateToCalculate = LastDateInChart - NumberOfDaysToCalculate.GetInt() + 1;

	float Open = 0, High = 0, Low = 0, Close = 0, NextOpen = 0;

	SCDateTime BeginOfRefDateTime,EndOfRefDateTime;
	SCDateTime CurrentPeriodBeginDateTime;
	SCDateTime CurrentPeriodEndDateTime;

	SCDateTime PriorCurrentPeriodStartDateTime;



	for (int index = sc.UpdateStartIndex; index < sc.ArraySize+NumberOfForwardBars; index++ )
	{

		SCDateTime CurrentBarDT = sc.BaseDateTimeIn[index];
		

		if (CurrentBarDT < FirstDateToCalculate)
			continue;

		bool GetReferenceData = true;

		CurrentPeriodBeginDateTime = sc.GetStartOfPeriodForDateTime(CurrentBarDT, TimePeriodType.GetTimePeriodType(),  PeriodLength, 0);

		if(	CurrentPeriodBeginDateTime == PriorCurrentPeriodStartDateTime)
			GetReferenceData  = false;


		PriorCurrentPeriodStartDateTime =	CurrentPeriodBeginDateTime;

		SCDateTime TimeIncrement = sc.TimePeriodSpan(TimePeriodType.GetTimePeriodType(),  PeriodLength);

		if(GetReferenceData)
		{

			BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(), PeriodLength, -1);
			EndOfRefDateTime=  	CurrentPeriodBeginDateTime - (1*SECONDS);
			
			CurrentPeriodEndDateTime=sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, 1) - (1*SECONDS);

			if (DisplayDebuggingOutput.GetYesNo() != 0)
			{
				SCString Message;

				Message.Format("Current Bar: %s, BeginOfRefDateTime: %s, EndOfRefDateTime: %s, CPBeginDateTime: %s, CPEndDateTime: %s",
					sc.FormatDateTime(CurrentBarDT).GetChars(),sc.FormatDateTime(BeginOfRefDateTime).GetChars(),
					sc.FormatDateTime(EndOfRefDateTime).GetChars(),sc.FormatDateTime(CurrentPeriodBeginDateTime).GetChars(),
					sc.FormatDateTime(CurrentPeriodEndDateTime).GetChars());

				sc.AddMessageToLog(Message,0);

			}

			int MaxPeriodsToGoBack = 1;

			if(AutoSkipPeriodOfNoTrading.GetYesNo())
				MaxPeriodsToGoBack = 32;

			for (int WalkBack = 0; WalkBack < MaxPeriodsToGoBack; WalkBack++)
			{
				if (WalkBack >= 1) //Walk back 1 period.
				{

					SCDateTime  PriorBeginOfRefDateTime = BeginOfRefDateTime;

					BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime(BeginOfRefDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, -1);
					EndOfRefDateTime=  	 PriorBeginOfRefDateTime- (1*SECONDS);

					if (DisplayDebuggingOutput.GetYesNo() != 0)
					{
						SCString Message;

						Message.Format("Moving back 1 period. BeginOfRefDateTime: %s, EndOfRefDateTime: %s.",
							sc.FormatDateTime(BeginOfRefDateTime).GetChars(),sc.FormatDateTime(EndOfRefDateTime).GetChars());

						sc.AddMessageToLog(Message,0);
					}
				}

				int NumberOfBars = 0;
				SCDateTime TotalTimeSpan;
				int Result = sc.GetOHLCOfTimePeriod(BeginOfRefDateTime, EndOfRefDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);

				if (!Result)
					continue;

				if (DisplayDebuggingOutput.GetYesNo() != 0)
				{
					SCString Message;
					Message.Format("Number of Bars: %d, Total Time Span In Minutes: %d",NumberOfBars,(int)(TotalTimeSpan/MINUTES+0.5));
					sc.AddMessageToLog(Message,0);

					Message.Format("RefOpen %f,RefHigh %f,RefLow %f,RefClose %f,RefNextOpen %f.",Open, High, Low, Close, NextOpen);
					sc.AddMessageToLog(Message,0);
				}

				SCDateTime MinimumTimeSpan = (TimeIncrement * MinimumRequiredTimePeriodAsPercent.GetFloat()/100.0f);

				if (TotalTimeSpan >= MinimumTimeSpan)
					break;
			}
		}


		ValidPivotPoint = 
			CalculatePivotPoints(Open,High,Low,Close,NextOpen,
			f_PivotPoint, fR0_5, fR1, fR1_5, fR2, fR2_5, fR3, fS0_5,
			fS1, fS1_5, fS2, fS2_5, fS3, fR3_5, fS3_5, fR4, fR4_5, fS4, fS4_5, fR5, fS5,
			fR6, fS6, fR7, fS7, fR8, fS8, fR9, fS9, fR10, fS10, FormulaType.GetInt());


		if (!ValidPivotPoint)
			continue;

		if (RoundToTickSize.GetYesNo() != 0)
		{
			R1[index] = (float)sc.RoundToTickSize(fR1, sc.TickSize);
			R2[index] = (float)sc.RoundToTickSize(fR2, sc.TickSize);
			S1[index] = (float)sc.RoundToTickSize(fS1, sc.TickSize);
			S2[index] = (float)sc.RoundToTickSize(fS2, sc.TickSize);

			R0_5[index] = (float)sc.RoundToTickSize(fR0_5, sc.TickSize);
			R1_5[index] = (float)sc.RoundToTickSize(fR1_5, sc.TickSize);
			R2_5[index] = (float)sc.RoundToTickSize(fR2_5, sc.TickSize);
			R3[index] = (float)sc.RoundToTickSize(fR3, sc.TickSize);
			S0_5[index] = (float)sc.RoundToTickSize(fS0_5, sc.TickSize);
			S1_5[index] = (float)sc.RoundToTickSize(fS1_5, sc.TickSize);
			S2_5[index] = (float)sc.RoundToTickSize(fS2_5, sc.TickSize);
			S3[index] = (float)sc.RoundToTickSize(fS3, sc.TickSize);
			PP[index] = (float)sc.RoundToTickSize(f_PivotPoint, sc.TickSize);
			R4[index] = (float)sc.RoundToTickSize(fR4, sc.TickSize);
			R4_5[index] = (float)sc.RoundToTickSize(fR4_5, sc.TickSize);
			S4[index] = (float)sc.RoundToTickSize(fS4, sc.TickSize);
			S4_5[index] = (float)sc.RoundToTickSize(fS4_5, sc.TickSize);
			R3_5[index] = (float)sc.RoundToTickSize(fR3_5, sc.TickSize);
			S3_5[index] = (float)sc.RoundToTickSize(fS3_5, sc.TickSize);
			R5[index] = (float)sc.RoundToTickSize(fR5, sc.TickSize);
			S5[index] = (float)sc.RoundToTickSize(fS5, sc.TickSize);
			R6[index] = (float)sc.RoundToTickSize(fR6, sc.TickSize);
			S6[index] = (float)sc.RoundToTickSize(fS6, sc.TickSize);
			R7[index] = (float)sc.RoundToTickSize(fR7, sc.TickSize);
			S7[index] = (float)sc.RoundToTickSize(fS7, sc.TickSize);
			R8[index] = (float)sc.RoundToTickSize(fR8, sc.TickSize);
			S8[index] = (float)sc.RoundToTickSize(fS8, sc.TickSize);
			R9[index] = (float)sc.RoundToTickSize(fR9, sc.TickSize);
			S9[index] = (float)sc.RoundToTickSize(fS9, sc.TickSize);
			R10[index] = (float)sc.RoundToTickSize(fR10, sc.TickSize);
			S10[index] = (float)sc.RoundToTickSize(fS10, sc.TickSize);
		}
		else 
		{
			R1[index] = fR1;
			R2[index] = fR2;
			S1[index] = fS1;
			S2[index] = fS2;

			R0_5[index] = fR0_5;
			R1_5[index] = fR1_5;
			R2_5[index] = fR2_5;
			R3[index] = fR3;
			S0_5[index] = fS0_5;
			S1_5[index] = fS1_5;
			S2_5[index] = fS2_5;
			S3[index] = fS3;
			PP[index] = f_PivotPoint;
			R4[index] = fR4;
			R4_5[index] = fR4_5;
			S4[index] = fS4;
			S4_5[index] = fS4_5;
			R3_5[index] = fR3_5;
			S3_5[index] = fS3_5;
			R5[index] = fR5;
			S5[index] = fS5;
			R6[index] = fR6;
			S6[index] = fS6;
			R7[index] = fR7;
			S7[index] = fS7;
			R8[index] = fR8;
			S8[index] = fS8;
			R9[index] = fR9;
			S9[index] = fS9;
			R10[index] = fR10;
			S10[index] = fS10;
		}
	} //for
}

/*==========================================================================*/

SCSFExport scsf_PivotRangeVariablePeriod(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HLCAvgPlusHLCAvgMinusHLAvg = sc.Subgraph[0];
	SCSubgraphRef PreviousHLCAvg = sc.Subgraph[1];
	SCSubgraphRef PreviousHLCAvg_2 = sc.Subgraph[2];
	SCSubgraphRef HLCAvgMinusHLCAvgMinusHLAvg = sc.Subgraph[3];
	SCSubgraphRef HLAvg = sc.Subgraph[4];

	SCInputRef TimePeriodType = sc.Input[0];
	SCInputRef TimePeriodLength = sc.Input[1];
	
	SCInputRef MinimumRequiredTP = sc.Input[3];
	SCInputRef RoundToTickSize = sc.Input[4];
	SCInputRef DisplayDebuggingOutput = sc.Input[5];
	SCInputRef ForwardProjectLines = sc.Input[6];
	SCInputRef NumberOfDaysToCalculate = sc.Input[7];
	SCInputRef AutoSkipPeriodOfNoTrading = sc.Input[8];


	if (sc.SetDefaults)
	{
		sc.GraphName = "Pivot Range-Variable Period";

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.DrawStudyUnderneathMainPriceGraph= true;

		sc.AutoLoop = 0;

		HLCAvgPlusHLCAvgMinusHLAvg.Name = "Top Range";
		PreviousHLCAvg.Name = "HLC Avg";

		PreviousHLCAvg_2.Name = "HLC Avg";
		HLCAvgMinusHLCAvgMinusHLAvg.Name = "Bottom Range";
				
		HLCAvgPlusHLCAvgMinusHLAvg.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_TOP;
		PreviousHLCAvg.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_BOTTOM;

		PreviousHLCAvg_2.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_TOP;
		HLCAvgMinusHLCAvgMinusHLAvg.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_BOTTOM;

		HLCAvgPlusHLCAvgMinusHLAvg.DrawZeros = false;
		PreviousHLCAvg.DrawZeros = false;
		PreviousHLCAvg_2.DrawZeros = false;
		HLCAvgMinusHLCAvgMinusHLAvg.DrawZeros = false;

		HLCAvgPlusHLCAvgMinusHLAvg.PrimaryColor = RGB(128, 255, 255);
		PreviousHLCAvg.PrimaryColor = RGB(128, 255, 255);
		PreviousHLCAvg_2.PrimaryColor = RGB(255, 255, 128);
		HLCAvgMinusHLCAvgMinusHLAvg.PrimaryColor = RGB(255, 255, 128);


		HLAvg.Name= "HL Avg";
		HLAvg.PrimaryColor = RGB(255, 128, 0);
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.DrawZeros = false;
	

		for (int i = 0; i <= 4; i++)
		{
			sc.Subgraph[i].LineLabel = 
				LL_DISPLAY_NAME | LL_DISPLAY_VALUE | LL_NAME_ALIGN_ABOVE | LL_NAME_ALIGN_RIGHT| 
				LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_MINUTES);

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(60);
		TimePeriodLength.SetIntLimits(1, 7*MINUTES_PER_DAY);


		MinimumRequiredTP.Name = "Minimum Required Time Period as %";
		MinimumRequiredTP.SetFloat(25.0f);
		MinimumRequiredTP.SetFloatLimits(1.0f, 100.0f);

		RoundToTickSize.Name = "Round to Tick Size";
		RoundToTickSize.SetYesNo(0);

		DisplayDebuggingOutput.Name = "Display Debugging Output (slows study calculations)";
		DisplayDebuggingOutput.SetYesNo(0);

		ForwardProjectLines.Name ="Forward Project Lines";
		ForwardProjectLines.SetYesNo(0);

		NumberOfDaysToCalculate.Name = "Number Of Days To Calculate";
		NumberOfDaysToCalculate.SetInt(1000);

		AutoSkipPeriodOfNoTrading.Name = "Auto Skip Period Of No Trading";
		AutoSkipPeriodOfNoTrading.SetYesNo(false);

		return;
	}


	int PeriodLength = TimePeriodLength.GetInt();

	int NumberOfForwardBars = 0;

	if (ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = 20;

		if(sc.UpdateStartIndex == 0)
		{
			HLCAvgPlusHLCAvgMinusHLAvg.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			PreviousHLCAvg.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			PreviousHLCAvg_2.ExtendedArrayElementsToGraph = NumberOfForwardBars;
			HLCAvgMinusHLCAvgMinusHLAvg.ExtendedArrayElementsToGraph = NumberOfForwardBars;

		}
	}

	if (NumberOfDaysToCalculate.GetInt() < 1)
		NumberOfDaysToCalculate.SetInt(1000);

	int LastDateInChart = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();
	int FirstDateToCalculate = LastDateInChart - NumberOfDaysToCalculate.GetInt() + 1;

	float PreviousOpen = 0, PreviousHigh = 0, PreviousLow = 0, PreviousClose = 0, NextOpen = 0;

	SCDateTime BeginOfRefDateTime,EndOfRefDateTime;
	SCDateTime CurrentPeriodBeginDateTime;
	SCDateTime CurrentPeriodEndDateTime;

	SCDateTime PriorCurrentPeriodStartDateTime;


	for (int index = sc.UpdateStartIndex; index < sc.ArraySize+NumberOfForwardBars; index++ )
	{

		SCDateTime CurrentBarDT = sc.BaseDateTimeIn[index];


		if (CurrentBarDT < FirstDateToCalculate)
			continue;

		bool GetReferenceData = true;

		CurrentPeriodBeginDateTime = sc.GetStartOfPeriodForDateTime(CurrentBarDT, TimePeriodType.GetTimePeriodType(),  PeriodLength, 0);

		if(	CurrentPeriodBeginDateTime == PriorCurrentPeriodStartDateTime)
			GetReferenceData  = false;


		PriorCurrentPeriodStartDateTime =	CurrentPeriodBeginDateTime;

		SCDateTime TimeIncrement = sc.TimePeriodSpan(TimePeriodType.GetTimePeriodType(),  PeriodLength);

		if(GetReferenceData)
		{

			BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(), PeriodLength, -1);
			EndOfRefDateTime=  	CurrentPeriodBeginDateTime - (1*SECONDS);

			CurrentPeriodEndDateTime=sc.GetStartOfPeriodForDateTime( 	CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, 1) - (1*SECONDS);

			if (DisplayDebuggingOutput.GetYesNo() != 0)
			{
				SCString Message;

				Message.Format("Current Bar: %s, BeginOfRefDateTime: %s, EndOfRefDateTime: %s, CPBeginDateTime: %s, CPEndDateTime: %s",
					sc.FormatDateTime(CurrentBarDT).GetChars(),sc.FormatDateTime(BeginOfRefDateTime).GetChars(),
					sc.FormatDateTime(EndOfRefDateTime).GetChars(),sc.FormatDateTime(CurrentPeriodBeginDateTime).GetChars(),
					sc.FormatDateTime(CurrentPeriodEndDateTime).GetChars());

				sc.AddMessageToLog(Message,0);

			}

			int MaxPeriodsToGoBack = 1;

			if(AutoSkipPeriodOfNoTrading.GetYesNo())
				MaxPeriodsToGoBack = 32;

			for (int WalkBack = 0; WalkBack<MaxPeriodsToGoBack;WalkBack++)
			{
				if (WalkBack >= 1) //Walk back 1 period.
				{
					SCDateTime  PriorBeginOfRefDateTime=BeginOfRefDateTime;

					BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime(BeginOfRefDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, -1);
					EndOfRefDateTime=  	 PriorBeginOfRefDateTime- (1*SECONDS);

					if (DisplayDebuggingOutput.GetYesNo() != 0)
					{
						SCString Message;

						Message.Format("Moving back 1 period. BeginOfRefDateTime: %s, EndOfRefDateTime: %s.",
							sc.FormatDateTime(BeginOfRefDateTime).GetChars(),sc.FormatDateTime(EndOfRefDateTime).GetChars());

						sc.AddMessageToLog(Message,0);
					}
				}

				int NumberOfBars = 0;
				SCDateTime TotalTimeSpan;
				int Result = sc.GetOHLCOfTimePeriod(BeginOfRefDateTime, EndOfRefDateTime, PreviousOpen, PreviousHigh, PreviousLow, PreviousClose, NextOpen, NumberOfBars, TotalTimeSpan);
				if (!Result)
					continue;

				if (DisplayDebuggingOutput.GetYesNo() != 0)
				{
					SCString Message;
					Message.Format("Number of Bars: %d, Total Time Span In Minutes: %d",NumberOfBars,(int)(TotalTimeSpan/MINUTES+0.5));
					sc.AddMessageToLog(Message,0);

					Message.Format("RefOpen %f,RefHigh %f,RefLow %f,RefClose %f,RefNextOpen %f.",PreviousOpen, PreviousHigh, PreviousLow, PreviousClose, NextOpen);
					sc.AddMessageToLog(Message,0);
				}

				SCDateTime MinimumTimeSpan = (TimeIncrement * MinimumRequiredTP.GetFloat()/100.0f);
				if (TotalTimeSpan >= MinimumTimeSpan)
					break;
			}
		}
		


		PreviousHLCAvg[index] = (PreviousHigh + PreviousLow + PreviousClose)/3.0f;
		PreviousHLCAvg_2[index] =PreviousHLCAvg[index] ;

		HLAvg[index]= (PreviousHigh + PreviousLow)/2.0f ;

		float HLCAvgMinusHLAvg = PreviousHLCAvg[index] - HLAvg[index];
		
		HLCAvgPlusHLCAvgMinusHLAvg [index]=PreviousHLCAvg[index] + HLCAvgMinusHLAvg;
		HLCAvgMinusHLCAvgMinusHLAvg [index]=PreviousHLCAvg[index] - HLCAvgMinusHLAvg;

		if (RoundToTickSize.GetYesNo() != 0)
		{
			PreviousHLCAvg[index] = sc.RoundToTickSize(PreviousHLCAvg[index], sc.TickSize);
			PreviousHLCAvg_2[index] =sc.RoundToTickSize(PreviousHLCAvg_2[index], sc.TickSize);

			HLCAvgPlusHLCAvgMinusHLAvg [index]=sc.RoundToTickSize(HLCAvgPlusHLCAvgMinusHLAvg[index], sc.TickSize);
			HLCAvgMinusHLCAvgMinusHLAvg [index]=sc.RoundToTickSize(HLCAvgMinusHLCAvgMinusHLAvg[index], sc.TickSize);
		}

	} //for
}
/*==========================================================================*/
// This function is used internally by scsf_SwingHighAndLowCustom
int IsSwingLowAllowEqualBackwardsOnly(SCStudyInterfaceRef sc, int AllowEqual, int Index, int Length)
{
	for(int i = 1; i <= Length; i++)
	{
		if (AllowEqual)
		{
			if (sc.FormattedEvaluate(sc.BaseData [SC_LOW][Index] , sc.BaseGraphValueFormat, GREATER_OPERATOR, sc.BaseData [SC_LOW][Index-i], sc.BaseGraphValueFormat))
				return 0;
		}
		else
		{
			if (sc.FormattedEvaluate(sc.BaseData [SC_LOW][Index] , sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR, sc.BaseData [SC_LOW][Index-i], sc.BaseGraphValueFormat))
				return 0;
		}
	}

	return 1;
}

/*==========================================================================*/
SCSFExport scsf_SwingHighAndLowCustom(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SwingHigh = sc.Subgraph[0];
	SCSubgraphRef SwingLow = sc.Subgraph[1];

	SCSubgraphRef Former = sc.Subgraph[2];
	SCSubgraphRef ColorForInsideBar = sc.Subgraph[3];
	SCSubgraphRef ColorForPotentialSwingHigh = sc.Subgraph[4];
	SCSubgraphRef ColorForPotentialSwingLow = sc.Subgraph[5];

	SCInputRef ArrowOffsetValue = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef AllowEqualBars = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Swing High And Low Hollow Bar";
		sc.StudyDescription = "Swing High And Low Hollow Bar";

		SwingHigh.LineWidth = 3;
		SwingLow.LineWidth = 3;

		
		sc.GraphRegion = 0;

		SwingHigh.Name = "Swing High";
		SwingHigh.DrawStyle = DRAWSTYLE_ARROW_DOWN;
		SwingHigh.PrimaryColor = RGB(255,0,0);
		SwingHigh.DrawZeros = false;

		SwingLow.Name = "Swing Low";
		SwingLow.DrawStyle = DRAWSTYLE_ARROW_UP;
		SwingLow.PrimaryColor = RGB(0,255,0);
		SwingLow.DrawZeros = false;

		Former.Name = "Potential Former";
		Former.DrawStyle = DRAWSTYLE_COLOR_BAR_HOLLOW;
		Former.PrimaryColor = RGB(255,255,0);
		Former.DrawZeros = false;

		ColorForInsideBar.Name = "Color for Potential Inside Bar";
		ColorForInsideBar.DrawStyle = DRAWSTYLE_LINE;
		ColorForInsideBar.PrimaryColor = RGB(255,128,0);
		ColorForInsideBar.DrawZeros = false;

		ColorForPotentialSwingHigh.Name = "Color for Potential Swing High";
		ColorForPotentialSwingHigh.DrawStyle = DRAWSTYLE_LINE;
		ColorForPotentialSwingHigh.PrimaryColor = RGB(0,0,255);
		ColorForPotentialSwingHigh.DrawZeros = false;

		ColorForPotentialSwingLow.Name = "Color for Potential Swing Low";
		ColorForPotentialSwingLow.DrawStyle = DRAWSTYLE_LINE;
		ColorForPotentialSwingLow.PrimaryColor = RGB(255,255,255);
		ColorForPotentialSwingLow.DrawZeros = false;

		ArrowOffsetValue.Name = "Arrow Offset as Value";
		ArrowOffsetValue.SetFloat(0);

		Length.Name = "Length";
		Length.SetInt(1);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		AllowEqualBars.Name = "Allow Equal High/Low Bars";
		AllowEqualBars.SetYesNo(false);

		sc.AutoLoop = true;
		return;
	}

	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;

	int MiddleIndex = sc.Index - Length.GetInt();

	Former[sc.Index] = High[sc.Index]; 

	if (IsSwingHighAllowEqual_S(sc, AllowEqualBars.GetYesNo(), sc.Index, Length.GetInt()))
	{
		Former.Arrays[0][sc.Index] = 1;
		Former.DataColor[sc.Index] = ColorForPotentialSwingHigh.PrimaryColor;

		// erase all previous potential swing highs
		for (int BarIndex = sc.Index - 1; BarIndex >= MiddleIndex && BarIndex >= 0; BarIndex--)
		{
			if (Former.Arrays[0][BarIndex] == 1)
			{
				Former[BarIndex] = 0;
				Former.Arrays[0][BarIndex] = 0;
			}
		}
	}
	else if (IsSwingLowAllowEqualBackwardsOnly(sc, AllowEqualBars.GetYesNo(), sc.Index, Length.GetInt()))
	{
		Former.Arrays[0][sc.Index] = -1;
		Former.DataColor[sc.Index] = ColorForPotentialSwingLow.PrimaryColor;

		// erase all previous potential swing lows
		for (int BarIndex = sc.Index - 1; BarIndex >= MiddleIndex && BarIndex >= 0; BarIndex--)
		{
			if (Former.Arrays[0][BarIndex] == -1)
			{
				Former[BarIndex] = 0;
				Former.Arrays[0][BarIndex] = 0;
			}
		}
	}
	else
	{
		Former.Arrays[0][sc.Index] = 0;
		Former.DataColor[sc.Index] = ColorForInsideBar.PrimaryColor;
	}

	// erase all previous inside-bars in any case
	for (int BarIndex = sc.Index - 1; BarIndex >= MiddleIndex && BarIndex >= 0; BarIndex--)
	{
		if (Former.Arrays[0][BarIndex] == 0)
		{
			Former[BarIndex] = 0;
		}
	}

	if (sc.GetBarHasClosedStatus(sc.Index) != BHCS_BAR_HAS_CLOSED)
		return;

	if(sc.Index < 2 * Length.GetInt())
	{
		SwingHigh[sc.Index] = 0;
		SwingLow[sc.Index] = 0;

		return;
	}

	if (IsSwingHighAllowEqual_S(sc, AllowEqualBars.GetYesNo(), MiddleIndex, Length.GetInt()))
		SwingHigh[MiddleIndex] = sc.High[MiddleIndex] + ArrowOffsetValue.GetFloat();

	if (IsSwingLowAllowEqual_S(sc, AllowEqualBars.GetYesNo(), MiddleIndex, Length.GetInt()))
		SwingLow[MiddleIndex] = sc.Low[MiddleIndex] - ArrowOffsetValue.GetFloat();

	// Former needs to be set to 0, as we have calculated real Swing High/Low
	Former[MiddleIndex] = 0;

	sc.EarliestUpdateSubgraphDataArrayIndex = MiddleIndex;
}

/*==========================================================================*/
SCSFExport scsf_StochasticMomentumIndicator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SMI = sc.Subgraph[0];
	SCSubgraphRef AvgSMI = sc.Subgraph[1];
	SCSubgraphRef Overbought = sc.Subgraph[2];
	SCSubgraphRef Oversold = sc.Subgraph[3];

	SCInputRef OverboughtInput = sc.Input[0];
	SCInputRef OversoldInput = sc.Input[1];
	SCInputRef PercentDLength = sc.Input[2];
	SCInputRef PercentKLength = sc.Input[3];
	SCInputRef ExpMovAvgLength = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Stochastic Momentum Indicator";
		sc.StudyDescription = "Stochastic Momentum Indicator";

		
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;

		SMI.Name = "SMI";
		SMI.DrawStyle = DRAWSTYLE_LINE;
		SMI.PrimaryColor = RGB(0,255,0);
		SMI.DrawZeros = false;

		AvgSMI.Name = "SMI Average";
		AvgSMI.DrawStyle = DRAWSTYLE_LINE;
		AvgSMI.PrimaryColor = RGB(255,0,255);
		AvgSMI.DrawZeros = false;

		Overbought.Name = "Overbought";
		Overbought.DrawStyle = DRAWSTYLE_LINE;
		Overbought.PrimaryColor = RGB(255,255,0);
		Overbought.DrawZeros = false;

		Oversold.Name = "Oversold";
		Oversold.DrawStyle = DRAWSTYLE_LINE;
		Oversold.PrimaryColor = RGB(255,127,0);
		Oversold.DrawZeros = false;

		OverboughtInput.Name = "Overbought Value";
		OverboughtInput.SetFloat(40.0f);

		OversoldInput.Name = "Oversold Value";
		OversoldInput.SetFloat(-40.0f);

		PercentDLength.Name = "%D Length";
		PercentDLength.SetInt(3);

		PercentKLength.Name = "%K Length";
		PercentKLength.SetInt(5);

		ExpMovAvgLength.Name = "EMA Length";
		ExpMovAvgLength.SetInt(3);

		return;
	}


	sc.DataStartIndex = max(PercentKLength.GetInt(), PercentDLength.GetInt());

	if (sc.Index < sc.DataStartIndex)
		return;

	SCFloatArrayRef TempLowest = SMI.Arrays[0];
	SCFloatArrayRef TempHighest = SMI.Arrays[1];
	SCFloatArrayRef TempRelDiff = SMI.Arrays[2];
	SCFloatArrayRef TempDiff = SMI.Arrays[3];
	SCFloatArrayRef TempAvgRelMA = SMI.Arrays[4];
	SCFloatArrayRef TempAvgRel = SMI.Arrays[5];
	SCFloatArrayRef TempAvgDiffMA = SMI.Arrays[6];
	SCFloatArrayRef TempAvgDiff = SMI.Arrays[7];

	sc.Lowest(sc.Low, TempLowest, PercentKLength.GetInt());
	sc.Highest(sc.High, TempHighest, PercentKLength.GetInt());

	TempRelDiff[sc.Index] = sc.Close[sc.Index] - (TempHighest[sc.Index] + TempLowest[sc.Index]) / 2.0f;
	TempDiff[sc.Index] = TempHighest[sc.Index] - TempLowest[sc.Index];

	sc.ExponentialMovAvg(TempRelDiff, TempAvgRelMA, PercentDLength.GetInt());
	sc.ExponentialMovAvg(TempAvgRelMA, TempAvgRel, PercentDLength.GetInt());
	sc.ExponentialMovAvg(TempDiff, TempAvgDiffMA, PercentDLength.GetInt());
	sc.ExponentialMovAvg(TempAvgDiffMA, TempAvgDiff, PercentDLength.GetInt());

	if (TempAvgDiff[sc.Index] != 0.0f)
		SMI[sc.Index] = TempAvgRel[sc.Index] / (TempAvgDiff[sc.Index] / 2.0f) * 100.0f;
	else
		SMI[sc.Index] = 0.0f;

	sc.ExponentialMovAvg(SMI, AvgSMI, ExpMovAvgLength.GetInt());

	Overbought[sc.Index] = OverboughtInput.GetFloat();
	Oversold[sc.Index] = OversoldInput.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_DoubleExponentialMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DEMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Double Exponential";

		
		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		DEMA.Name = "DEMA";
		DEMA.DrawStyle = DRAWSTYLE_LINE;
		DEMA.PrimaryColor = RGB(255, 0, 0);
		DEMA.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		Length.SetInt(10);

		return;
	}

	sc.DataStartIndex = Length.GetInt();
	if (sc.Index < sc.DataStartIndex)
		return;

	SCFloatArrayRef EmaPrice = DEMA.Arrays[0];
	SCFloatArrayRef EmaEmaPrice = DEMA.Arrays[1];

	sc.ExponentialMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], EmaPrice, Length.GetInt());
	sc.ExponentialMovAvg(EmaPrice, EmaEmaPrice, Length.GetInt());

	DEMA[sc.Index] = 2 * EmaPrice[sc.Index] - EmaEmaPrice[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_BidVolumeVersusAskVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AskVolume = sc.Subgraph[0];
	SCSubgraphRef BidVolume = sc.Subgraph[1];
	SCSubgraphRef ZeroLine = sc.Subgraph[3];
	SCSubgraphRef Difference = sc.Subgraph[4];

	SCInputRef InvertStudy = sc.Input[3];
	SCInputRef ShowBidVolumePositive = sc.Input[4];
	SCInputRef TryUseNonzeroVolumes = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "BidVolume vs. AskVolume";
		sc.ValueFormat= 0;

		AskVolume.Name = "Ask Volume";
		AskVolume.PrimaryColor = RGB(0, 255, 0);
		AskVolume.DrawStyle = DRAWSTYLE_BAR;
		AskVolume.DrawZeros = true;

		BidVolume.Name = "Bid Volume";
		BidVolume.PrimaryColor = RGB(255, 0, 0);
		BidVolume.DrawStyle = DRAWSTYLE_BAR;
		BidVolume.DrawZeros = true;

		ZeroLine.Name = "Zero Line";
		ZeroLine.PrimaryColor = RGB(255,127,0);
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.DrawZeros = true;

		Difference.Name = "Difference";
		Difference.PrimaryColor = RGB(255, 255, 0);
		Difference.DrawStyle = DRAWSTYLE_LINE;
		Difference.DrawZeros = true;

		InvertStudy.Name = "Invert Study";
		InvertStudy.SetYesNo(0);

		ShowBidVolumePositive.Name = "Show Bid Volume As Positive";
		ShowBidVolumePositive.SetYesNo(0);

		TryUseNonzeroVolumes.Name = "Try Use Non-Zero Bid/Ask Volumes for Difference";
		TryUseNonzeroVolumes.SetYesNo(0);

		return;
	}

	int Invert = 1;
	if(InvertStudy.GetYesNo() == 1)
		Invert = -1;


	// Check for the appropriate chart type
	if (sc.ChartDataType != INTRADAY_DATA)
	{
		if (sc.Index == 0)
		{
			sc.AddMessageToLog("This study can only be applied to an Intraday Chart", 1);
		}
		return;
	}

	int StartIndex = sc.UpdateStartIndex;


	
		// Loop through the new indexes to fill in
		for (int DestIndex = StartIndex; DestIndex < sc.ArraySize; ++DestIndex)
		{
			float BidVolumeVal = sc.BidVolume[DestIndex];
			float AskVolumeVal = sc.AskVolume[DestIndex];

			AskVolume[DestIndex] = AskVolumeVal * Invert;

			if (ShowBidVolumePositive.GetYesNo() == 1)
				BidVolume[DestIndex] = BidVolumeVal * Invert;
			else
				BidVolume[DestIndex] = - BidVolumeVal * Invert;

			ZeroLine[DestIndex] = 0;

			if(TryUseNonzeroVolumes.GetYesNo() == 1)
			{
				// if both volume are zero skip this iteration
				if(BidVolumeVal == 0 && AskVolumeVal == 0)
					continue;

				// if Bid Volume is zero try to find nonzero volume in previous elements
				if(BidVolumeVal == 0)
				{
					int Index = DestIndex - 1;
					while(Index >= 0 && BidVolumeVal == 0)
					{
						BidVolumeVal = sc.BidVolume[Index];
						Index--;
					}
				}

				// if Ask Volume is zero try to find nonzero volume in previous elements
				if(AskVolumeVal == 0)
				{
					int Index = DestIndex - 1;
					while(Index >= 0 && AskVolumeVal == 0)
					{
						AskVolumeVal = sc.AskVolume[Index];
						Index--;
					}
				}
			}
			Difference[DestIndex] = AskVolumeVal - BidVolumeVal;
		}
	
}

/*============================================================================
	Test function for HTTP request.
----------------------------------------------------------------------------*/
SCSFExport scsf_HTTPTest(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "HTTP Request Test";
		
		sc.StudyDescription = "This is a study function that demonstrates how to request data from a web server.";
		
		sc.AutoLoop = 1;
		
		
		
		
		return;
	}
	
	int& RequestState = sc.GetPersistentInt(1);
	
	
	// Do data processing
	
	if (sc.Index == 0)
	{
		if (RequestState == HTTP_REQUEST_ERROR || RequestState == HTTP_REQUEST_RECEIVED)
		{
			// Make a request for a text file on the server. When the request is complete and all of the data has been downloaded, this study function will be called with the file placed into the sc.HTTPResponse character string array.
			if (!sc.MakeHTTPRequest("http://www.sierrachart.com/ACSILHTTPTest.txt"))
				sc.AddMessageToLog("Error making HTTP request.", 1);
			
			RequestState = HTTP_REQUEST_MADE;
		}
	}
	
	if (RequestState == HTTP_REQUEST_MADE && sc.HTTPRequestID != 0)
	{
		RequestState = HTTP_REQUEST_RECEIVED;
		
		// Display the response from the Web server in the Message Log
		sc.AddMessageToLog(sc.HTTPResponse, 1); 
	}	
}

/*============================================================================
	Test function for HTTP request with POST.
----------------------------------------------------------------------------*/
SCSFExport scsf_HTTPTestWithPOST(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "HTTP Request Test with POST";

		sc.StudyDescription = "This is a study function that demonstrates how to request data from a web server.";

		sc.AutoLoop = 0;
		return;
	}

	int& RequestState = sc.GetPersistentInt(1);

	// Do data processing

	if (sc.UpdateStartIndex == 0 && sc.IsFullRecalculation)
	{
		if (RequestState == HTTP_REQUEST_ERROR || RequestState == HTTP_REQUEST_RECEIVED)
		{
			n_ACSIL::s_HTTPHeader HTTPHeader;
			HTTPHeader.Name = "Custom";
			HTTPHeader.Value = "Value";

			// Make a request to the server.  
			// When the request is complete and all of the data has been downloaded,  
			// this study function will be called with the file placed into the
			// sc.HTTPResponse character string array. 
			if (!sc.MakeHTTPPOSTRequest("https://sierrachart.com/Test/ACSILPOSTTest.php", "Message=PostData", &HTTPHeader, 1))
			{
				sc.AddMessageToLog("Error making HTTP request.", 1);
			}

			RequestState = HTTP_REQUEST_MADE;
		}
	}

	if (RequestState == HTTP_REQUEST_MADE && sc.HTTPRequestID != 0)
	{
		RequestState = HTTP_REQUEST_RECEIVED;

		// Display the response from the Web server in the Message Log
		sc.AddMessageToLog(sc.HTTPResponse, 1);
	}
}

/*==========================================================================*/
SCSFExport scsf_TimeRangeHighlight(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ColorBackground = sc.Subgraph[0];

	SCInputRef StartTime = sc.Input[0];
	SCInputRef EndTime = sc.Input[1];
	SCInputRef Version = sc.Input[2];

	SCInputRef ForwardProject = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Time Range Highlight";
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;
		sc.ValueFormat = 0;
		sc.ScaleRangeType = SCALE_INDEPENDENT;


		ColorBackground.Name = "TR";
		ColorBackground.DrawStyle = DRAWSTYLE_BACKGROUND;
		ColorBackground.PrimaryColor = RGB(64, 64, 64);
		ColorBackground.DrawZeros = false;



		StartTime.Name = "Start Time";
		StartTime.SetTime(0);

		EndTime.Name = "End Time";
		EndTime.SetTime(SECONDS_PER_DAY - 1);

		Version.SetInt(1);

		ForwardProject.Name = "Display in Forward Projection Area";
		ForwardProject.SetYesNo(false);

		return;
	}


	if(Version.GetInt() < 1)
	{
		Version.SetInt(1);
		ColorBackground.DrawStyle = DRAWSTYLE_BACKGROUND;
	}

	int NumberForwardBars = 0;
	if(ForwardProject.GetYesNo())
	{
		NumberForwardBars = 200;
		ColorBackground.ExtendedArrayElementsToGraph = NumberForwardBars;
	}

	for (int Index = sc.UpdateStartIndex; Index < (sc.ArraySize + NumberForwardBars); Index++)
	{
		bool InputsReversed = (StartTime.GetTime() > EndTime.GetTime());

		SCDateTime& IndexDateTime =  sc.BaseDateTimeIn[Index];
		bool ShouldHighLight = false;

		if (InputsReversed)
			ShouldHighLight = (IndexDateTime.GetTimeInSeconds() >= StartTime.GetTime() || IndexDateTime.GetTimeInSeconds() <= EndTime.GetTime());
		else
			ShouldHighLight = (IndexDateTime.GetTimeInSeconds() >= StartTime.GetTime() && IndexDateTime.GetTimeInSeconds() <= EndTime.GetTime());

		int CloseIndex = Index;
		if(CloseIndex >= sc.ArraySize)
			CloseIndex = sc.ArraySize - 1;

		if (ShouldHighLight)
			ColorBackground[Index] =  sc.Close[CloseIndex];
		else
			ColorBackground[Index] = 0;
	}


}
/*==========================================================================*/
SCSFExport scsf_TimeRangeHighlightTransparent(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TRTop = sc.Subgraph[0];
	SCSubgraphRef TRBottom = sc.Subgraph[1];

	SCInputRef StartTime = sc.Input[0];
	SCInputRef EndTime = sc.Input[1];

	SCInputRef ForwardProject = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Time Range Highlight - Transparent";
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;//Manual looping
		sc.ValueFormat =0;
		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.DrawStudyUnderneathMainPriceGraph = true;
		

		TRTop.Name = "Top";
		TRTop.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TOP;
		TRTop.PrimaryColor = RGB(64, 64, 64);
		TRTop.DrawZeros = false;


		TRBottom.Name = "Bottom";
		TRBottom.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_BOTTOM;
		TRBottom.PrimaryColor = RGB(64, 64, 64);
		TRBottom.DrawZeros = false;

		StartTime.Name = "Start Time";
		StartTime.SetTime(0);

		EndTime.Name = "End Time";
		EndTime.SetTime(SECONDS_PER_DAY - 1);

		ForwardProject.Name = "Display in Forward Projection Area";
		ForwardProject.SetYesNo(false);

		return;
	}



	bool InputsReversed = (StartTime.GetTime() > EndTime.GetTime());


	int NumberForwardBars = 0;
	if(ForwardProject.GetYesNo())
	{
		NumberForwardBars = 200;
		TRTop.ExtendedArrayElementsToGraph = NumberForwardBars;
		TRBottom.ExtendedArrayElementsToGraph = NumberForwardBars;
	}

	for (int Index = sc.UpdateStartIndex; Index < (sc.ArraySize + NumberForwardBars); Index++)
	{
		SCDateTime& IndexDateTime =  sc.BaseDateTimeIn[Index];
		bool ShouldHighLight = false;

		if (InputsReversed)
			ShouldHighLight = (IndexDateTime.GetTimeInSeconds() >= StartTime.GetTime() || IndexDateTime.GetTimeInSeconds() <= EndTime.GetTime());
		else
			ShouldHighLight = (IndexDateTime.GetTimeInSeconds() >= StartTime.GetTime() && IndexDateTime.GetTimeInSeconds() <= EndTime.GetTime());

		if (ShouldHighLight)
		{
			TRTop[Index] = 1000000;
			TRBottom[Index] = -1000000;
		}
		else
		{
			TRTop[Index] = 0;
			TRBottom[Index] = 0;

		}
	}
}


/***************************************************************/
SCSFExport scsf_ColoredCCI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CCI = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef Line3 = sc.Subgraph[3];


	SCInputRef I_InputData = sc.Input[0];
	SCInputRef I_Length = sc.Input[3];
	SCInputRef I_Multiplier = sc.Input[4];
	SCInputRef I_Line2Value = sc.Input[5];
	SCInputRef I_Line3Value = sc.Input[6];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Colored CCI";


		
		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		CCI.Name = "CCI";
		CCI.DrawStyle = DRAWSTYLE_BAR;
		CCI.DrawZeros= true;
		CCI.PrimaryColor = RGB(0, 255, 255);
		CCI.SecondaryColorUsed = 1;
		CCI.SecondaryColor= RGB(0, 255, 0);

		Line1.Name = "Line 1";
		Line1.DrawStyle = DRAWSTYLE_HIDDEN;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros= true;

		Line2.Name = "Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = true;

		Line3.Name = "Line 3";
		Line3.DrawStyle = DRAWSTYLE_LINE;
		Line3.PrimaryColor = RGB(255,127,0);
		Line3.DrawZeros = true;



		I_InputData.Name = "Input Data";
		I_InputData.SetInputDataIndex(SC_HLC_AVG);

		I_Length.Name = "Length";
		I_Length.SetInt(10);
		I_Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		I_Multiplier.Name = "Multiplier";
		I_Multiplier.SetFloat(0.015f);

		I_Line2Value.Name = "Line2 Value";
		I_Line2Value.SetFloat(100);

		I_Line3Value.Name = "Line3 Value";
		I_Line3Value.SetFloat(-100);

		return;
	}

	float CCILow = -50.0f, CCHigh = 50.0f;

	sc.DataStartIndex = I_Length.GetInt();

	float Multiplier = I_Multiplier.GetFloat();

	if(Multiplier == 0.0f)
		Multiplier = 0.015f;

	sc.CCI(sc.BaseDataIn[I_InputData.GetInputDataIndex()], CCI, sc.Index, I_Length.GetInt(), Multiplier);

	Line2[sc.Index] = I_Line2Value.GetFloat();
	Line3[sc.Index] = I_Line3Value.GetFloat();

	if (CCI[sc.Index - 1] < CCILow && CCI[sc.Index] > CCHigh)
		CCI.DataColor[sc.Index] = CCI.SecondaryColor;
	else
		CCI.DataColor[sc.Index] = CCI.PrimaryColor;
}

/***************************************************************/
SCSFExport scsf_ColorBarsBasedOnMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCSubgraphRef ColorSMA = sc.Subgraph[1];
	SCSubgraphRef ArrowSMA = sc.Subgraph[2];

	SCFloatArrayRef Slope = Avg.Arrays[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Color Bars Based on Moving Average";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;

		sc.AutoLoop = 1;

		Avg.Name = "Avg";

		ColorSMA.Name = "Color SMA";
		ColorSMA.DrawStyle = DRAWSTYLE_COLOR_BAR;
		ColorSMA.PrimaryColor = RGB(0, 255, 0);
		ColorSMA.DrawZeros = false;

		ArrowSMA.Name = "Arrow SMA";
		ArrowSMA.DrawStyle = DRAWSTYLE_ARROW_UP;
		ArrowSMA.PrimaryColor = RGB(0, 255, 0);
		ArrowSMA.LineWidth = 3;
		ArrowSMA.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg,  Length.GetInt());
	sc.Slope(Avg, Slope);

	if (sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] > Avg[sc.Index] && Slope[sc.Index] > 0)
	{
		ColorSMA[sc.Index] = 1;
		ArrowSMA[sc.Index] = sc.Low[sc.Index];
	}
	else
	{
		ColorSMA[sc.Index] = 0;
		ArrowSMA[sc.Index] = 0;
	}
}

/*==========================================================================*/
//The best way to examine the operation of this study is to perform a chart replay.
SCSFExport scsf_SetAlertExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Set Alert Example";
		
		sc.StudyDescription = "";
		
		sc.AutoLoop = true;
		
		
		
		return;
	}


	// Do data processing
	if (sc.GetBarHasClosedStatus(sc.Index) == BHCS_BAR_HAS_NOT_CLOSED)
		return;

	int Index = sc.Index; 

	SCString AlertMessageText;
	AlertMessageText.Format("Alert at index %d", Index);

	//To configure the sound file for Alert 5 and other settings, select 'Global Settings >> General Settings >> Alerts' within the user interface.
	sc.SetAlert(5, AlertMessageText);
	sc.SetAlert(6, AlertMessageText);
	
}

/*==========================================================================*/
SCSFExport scsf_ClearMethodSwingLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SwingLine = sc.Subgraph[0];

	SCFloatArrayRef HH = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef LL = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef LH = sc.Subgraph[0].Arrays[2];
	SCFloatArrayRef HL = sc.Subgraph[0].Arrays[3];
	SCFloatArrayRef UpSwing = sc.Subgraph[0].Arrays[4];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Clear Method Swing Line";

		sc.AutoLoop = true;

		sc.GraphRegion = 0;

		SwingLine.Name = "SwingLine";
		SwingLine.PrimaryColor = COLOR_CYAN;
		SwingLine.SecondaryColor = COLOR_MAGENTA;
		SwingLine.SecondaryColorUsed = 1;

		
		

		return;
	}

	// Do data processing


	//Carry forward prior values in the background data arrays
	HH[sc.Index] = HH[sc.Index-1];
	HL[sc.Index] = HL[sc.Index-1];
	LL[sc.Index] = LL[sc.Index-1];
	LH[sc.Index] = LH[sc.Index-1];
	UpSwing[sc.Index] = UpSwing[sc.Index-1];



	if (UpSwing[sc.Index] == 1)
	{
		if (sc.High[sc.Index] > HH[sc.Index])
			HH[sc.Index] = sc.High[sc.Index];

		if (sc.Low[sc.Index] > HL[sc.Index])
			HL[sc.Index] = sc.Low[sc.Index];

		if (sc.High[sc.Index] < HL[sc.Index])
		{
			UpSwing[sc.Index] = 0;
			LL[sc.Index] = sc.Low[sc.Index];
			LH[sc.Index] = sc.High[sc.Index];
		}
	}

	if (UpSwing[sc.Index] == 0)
	{
		if (sc.Low[sc.Index] < LL[sc.Index])
			LL[sc.Index] = sc.Low[sc.Index];

		if (sc.High[sc.Index] < LH[sc.Index])
			LH[sc.Index] = sc.High[sc.Index];

		if (sc.Low[sc.Index] > LH[sc.Index])
		{
			UpSwing[sc.Index] = 1;
			HH[sc.Index] = sc.High[sc.Index];
			HL[sc.Index] = sc.Low[sc.Index];
		}
	}

	if (UpSwing[sc.Index] == 1)
	{
		SwingLine[sc.Index] = HL[sc.Index];
		SwingLine.DataColor[sc.Index] = SwingLine.PrimaryColor;
	}
	else
	{
		SwingLine[sc.Index] = LH[sc.Index];
		SwingLine.DataColor[sc.Index] = SwingLine.SecondaryColor;
	}
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphsAdd(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Add = sc.Subgraph[0];

	SCInputRef StudySugbraph1 = sc.Input[0];
	SCInputRef StudySugbraph2 = sc.Input[1];
	SCInputRef Study2SubgraphOffset = sc.Input[2];
	SCInputRef DrawZeros = sc.Input[3];
	SCInputRef PerformAddWithZeroValue = sc.Input[4];
	SCInputRef Version = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Add";

		sc.AutoLoop = 0;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		

		Add.Name = "Add";
		Add.DrawStyle = DRAWSTYLE_LINE;
		Add.LineWidth = 1;
		Add.PrimaryColor = RGB(0,255,0);

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		Study2SubgraphOffset.Name = "Study 2 Subgraph Offset";
		Study2SubgraphOffset.SetInt(0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		PerformAddWithZeroValue.Name = "Perform Add With Zero Value";
		PerformAddWithZeroValue.SetYesNo(true);

		Version.SetInt(2);

		return;
	}

	if (Version.GetInt() < 2)
	{
		PerformAddWithZeroValue.SetYesNo(true);
		Version.SetInt(2);
	}

	Add.DrawZeros = DrawZeros.GetYesNo();

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(),StudySugbraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(),StudySugbraph2.GetSubgraphIndex(),Study2Array);

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();
	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		float Value1 = Study1Array[Index];
		float Value2 = Study2Array[Index - Study2SubgraphOffset.GetInt()];

		if (PerformAddWithZeroValue.GetYesNo()
			|| (Value1 != 0.0 && Value2 != 0.0))
		{
			Add[Index] = Value1 + Value2;
		}
		else
		{
			Add[Index] = 0;
		}
	}

	
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphsMultiply(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Multiply = sc.Subgraph[0];

	SCInputRef StudySugbraph1 = sc.Input[0];
	SCInputRef StudySugbraph2 = sc.Input[1];
	SCInputRef Study2SubgraphOffset = sc.Input[2];

	SCInputRef DrawZeros = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Multiply";
		sc.AutoLoop = 0;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		

		Multiply.Name = "Multiply";
		Multiply.DrawStyle = DRAWSTYLE_LINE;
		Multiply.PrimaryColor = RGB(0,255,0);
		Multiply.LineWidth = 1;

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		Study2SubgraphOffset.Name = "Study 2 Subgraph Offset";
		Study2SubgraphOffset.SetInt(0);
		
		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}


	Multiply.DrawZeros= DrawZeros.GetYesNo();
	
	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(),StudySugbraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(),StudySugbraph2.GetSubgraphIndex(),Study2Array);

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		Multiply[Index] = Study1Array[Index] * Study2Array[Index - Study2SubgraphOffset.GetInt()];
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphsDivide(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Ratio = sc.Subgraph[0];

	SCInputRef StudySugbraph1 = sc.Input[0];
	SCInputRef StudySugbraph2 = sc.Input[1];
	SCInputRef Study2SubgraphOffset = sc.Input[2];
	SCInputRef DrawZeros = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Divide/Ratio";

		sc.AutoLoop = 0;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		

		Ratio.Name = "Ratio";
		Ratio.DrawStyle = DRAWSTYLE_LINE;
		Ratio.PrimaryColor = RGB(0,255,0);
		Ratio.LineWidth = 1;

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		Study2SubgraphOffset.Name = "Study 2 Subgraph Offset";
		Study2SubgraphOffset.SetInt(0);


		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}


	Ratio.DrawZeros= DrawZeros.GetYesNo();

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(),StudySugbraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(),StudySugbraph2.GetSubgraphIndex(),Study2Array);

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{
		float Denominator = Study2Array[Index - Study2SubgraphOffset.GetInt()];
		if (Denominator != 0.0f)
			Ratio[Index] = Study1Array[Index] / Denominator;
		else
			Ratio[Index] = 0.0f;
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

}

/*==========================================================================*/
SCSFExport scsf_Inertia(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Inertia = sc.Subgraph[0];
	SCSubgraphRef RVI = sc.Subgraph[1];

	SCInputRef RVILength = sc.Input[0];
	SCInputRef LinearRegressionLength = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Inertia";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		

		Inertia.Name = "Inertia";
		Inertia.PrimaryColor = COLOR_GREEN;
		Inertia.DrawStyle = DRAWSTYLE_LINE;
		Inertia.PrimaryColor = RGB(0,255,0);
		Inertia.DrawZeros = true;
		Inertia.LineWidth = 1;

		RVILength.Name = "Relative Vigor Index Length";
		RVILength.SetInt(10);

		LinearRegressionLength.Name = "Linear Regression Length";
		LinearRegressionLength.SetInt(10);

		return;
	}

	sc.DataStartIndex = max(RVILength.GetInt(), LinearRegressionLength.GetInt());

	// Calculate RVI
	SCFloatArrayRef Value1 = RVI.Arrays[0];
	SCFloatArrayRef Value2 = RVI.Arrays[1];
	SCFloatArrayRef RVINumerator  = RVI.Arrays[2];
	SCFloatArrayRef RVIDenominator = RVI.Arrays[3];

	Value1[sc.Index] = ((sc.Close[sc.Index] - sc.Open[sc.Index])
		+ 2 * (sc.Close[sc.Index - 1] - sc.Open[sc.Index - 1])
		+ 2 * (sc.Close[sc.Index - 2] - sc.Open[sc.Index - 2])
		+ (sc.Close[sc.Index - 3] - sc.Open[sc.Index - 3])) / 6;

	Value2[sc.Index] = ((sc.High[sc.Index] - sc.Low[sc.Index])
		+ 2 * (sc.High[sc.Index - 1] - sc.Low[sc.Index - 1])
		+ 2 * (sc.High[sc.Index - 2] - sc.Low[sc.Index - 2])
		+ (sc.High[sc.Index - 3] - sc.Low[sc.Index - 3])) / 6;

	if (sc.Index < sc.DataStartIndex)
		return;

	sc.Summation(Value1, RVINumerator , RVILength.GetInt());
	sc.Summation(Value2, RVIDenominator, RVILength.GetInt());

	if (RVIDenominator[sc.Index] != 0.0f)
		RVI[sc.Index] = RVINumerator [sc.Index] / RVIDenominator[sc.Index];
	else
		RVI[sc.Index] = 0.0f;

	// Calculate Inertia
	sc.LinearRegressionIndicator(RVI, Inertia, LinearRegressionLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_Inertia2(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Inertia = sc.Subgraph[0];
	SCSubgraphRef RVI = sc.Subgraph[1];

	SCInputRef StdDeviationLength = sc.Input[0];
	SCInputRef RviLength = sc.Input[1];
	SCInputRef LinearRegressionLength = sc.Input[2];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Inertia 2";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		

		Inertia.Name = "Inertia";
		Inertia.PrimaryColor = COLOR_GREEN;
		Inertia.DrawStyle = DRAWSTYLE_LINE;
		Inertia.DrawZeros = true;
		Inertia.LineWidth = 1;

		StdDeviationLength.Name = "Standard Deviation Length";
		StdDeviationLength.SetInt(9);

		RviLength.Name = "Relative Volatility Index Length";
		RviLength.SetInt(14);

		LinearRegressionLength.Name = "Linear Regression Length";
		LinearRegressionLength.SetInt(10);

		return;
	}

	sc.DataStartIndex = max(StdDeviationLength.GetInt(), max(RviLength.GetInt(), LinearRegressionLength.GetInt()));
	if (sc.Index < sc.DataStartIndex)
		return;

	// Calculate RVI
	SCFloatArrayRef RviUp = RVI.Arrays[0];
	SCFloatArrayRef RviDown = RVI.Arrays[1];
	SCFloatArrayRef RviUpAvg = RVI.Arrays[2];
	SCFloatArrayRef RviDownAvg = RVI.Arrays[3];
	SCFloatArrayRef StdDeviation = RVI.Arrays[4];

	sc.StdDeviation(sc.Close, StdDeviation, StdDeviationLength.GetInt());

	if (sc.Close[sc.Index] > sc.Close[sc.Index - 1])
	{
		RviUp[sc.Index] = StdDeviation[sc.Index];
		RviDown[sc.Index] = 0.0f;
	}
	else
	{
		RviUp[sc.Index] = 0.0f;
		RviDown[sc.Index] = StdDeviation[sc.Index];
	}

	//At the first index calculated the prior RVI up average and down average is going to be zero.
	if (RviLength.GetInt() != 0)
	{
		RviUpAvg[sc.Index] = (RviUpAvg[sc.Index - 1] * (RviLength.GetInt() - 1) +  RviUp[sc.Index]) / RviLength.GetInt();
		RviDownAvg[sc.Index] = (RviDownAvg[sc.Index - 1] * (RviLength.GetInt() - 1) +  RviDown[sc.Index]) / RviLength.GetInt();
	}
	else
	{
		RviUpAvg[sc.Index] = 0.0f;
		RviDownAvg[sc.Index] = 0.0f;
	}

	if (RviUpAvg[sc.Index] + RviDownAvg[sc.Index] != 0.0f)
		RVI[sc.Index] = (RviUpAvg[sc.Index] / (RviUpAvg[sc.Index] + RviDownAvg[sc.Index])) * 100;
	else
		RVI[sc.Index] = 0.0f;

	// Calculate Inertia
	sc.LinearRegressionIndicator(RVI, Inertia, LinearRegressionLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_AskBidVolumeDifferenceBars(SCStudyInterfaceRef sc)
{
	// reference the base data
	SCFloatArrayRef NumTradesArray = sc.BaseDataIn[SC_NUM_TRADES];
	SCFloatArrayRef BidVolumeArray    = sc.BaseDataIn[SC_BIDVOL];
	SCFloatArrayRef AskVolumeArray    = sc.BaseDataIn[SC_ASKVOL];
	SCFloatArrayRef AskBidDiffHighArray  = sc.BaseDataIn[SC_ASKBID_DIFF_HIGH];
	SCFloatArrayRef AskBidDiffLowArray   = sc.BaseDataIn[SC_ASKBID_DIFF_LOW];

	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Close = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ask/Bid Volume Difference Bars";
		
		sc.MaintainAdditionalChartDataArrays = 1;

		sc.ValueFormat = 0;
		sc.GraphDrawType = GDT_CANDLESTICK;
		
		Open.Name = "Open";
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.DrawZeros = true;

		High.Name = "High";
		High.PrimaryColor = RGB(0,128,0);
		High.DrawStyle = DRAWSTYLE_LINE;
		High.DrawZeros = true;

		Low.Name  = "Low";
		Low.PrimaryColor = RGB(255,0,0);
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.DrawZeros = true;

		Close.Name = "Close";
		Close.PrimaryColor = RGB(128,0,0);
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.DrawZeros = true;

		return;
	}

	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		float BidAskDifference = AskVolumeArray[BarIndex] - BidVolumeArray[BarIndex];
		if (NumTradesArray[BarIndex] == 1)
			Open[BarIndex] = BidAskDifference;
		else
			Open[BarIndex] = 0;

		High[BarIndex]  = AskBidDiffHighArray[BarIndex];
		Low[BarIndex]   = AskBidDiffLowArray[BarIndex];
		Close[BarIndex] = BidAskDifference;

		if (Open[BarIndex] > High[BarIndex]) 
			Open[BarIndex] = High[BarIndex];

		if (Open[BarIndex] < Low[BarIndex]) 
			Open[BarIndex] = Low[BarIndex];
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeDeltaBarsVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BarOpens    = sc.Subgraph[SC_OPEN];
	SCSubgraphRef BarHighs    = sc.Subgraph[SC_HIGH];
	SCSubgraphRef BarLows     = sc.Subgraph[SC_LOW];
	SCSubgraphRef BarCloses   = sc.Subgraph[SC_LAST];
	SCSubgraphRef BarVolumes = sc.Subgraph[SC_VOLUME];

	SCSubgraphRef OHLCAvg = sc.Subgraph[SC_OHLC_AVG];
	SCSubgraphRef HLCAvg  = sc.Subgraph[SC_HLC_AVG];
	SCSubgraphRef HLAvg   = sc.Subgraph[SC_HL_AVG];

	SCInputRef ResetAtSessionStart = sc.Input[2];
	SCInputRef ResetAtBothSessionStarts = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Delta Bars - Volume";

		
		sc.MaintainAdditionalChartDataArrays = 1;
		sc.AutoLoop = 0;

		sc.ValueFormat = 0;
		sc.GraphDrawType = GDT_CANDLESTICK;

		BarOpens.Name = "Open";
		BarOpens.DrawStyle = DRAWSTYLE_LINE;
		BarOpens.PrimaryColor = RGB(0,255,0);
		BarOpens.SecondaryColor = RGB(0,255,0);
		BarOpens.SecondaryColorUsed = true;
		BarOpens.DrawZeros = true;

		BarHighs.Name = "High";
		BarHighs.DrawStyle = DRAWSTYLE_LINE;
		BarHighs.PrimaryColor = RGB(0,128,0);
		BarHighs.DrawZeros = true;

		BarLows.Name  = "Low";
		BarLows.DrawStyle = DRAWSTYLE_LINE;
		BarLows.PrimaryColor = RGB(255,0,0);
		BarLows.SecondaryColor = RGB(255,0,0);
		BarLows.SecondaryColorUsed = true;
		BarLows.DrawZeros = true;

		BarCloses.Name = "Close";
		BarCloses.DrawStyle = DRAWSTYLE_LINE;
		BarCloses.PrimaryColor = RGB(128,0,0);
		BarCloses.DrawZeros = true;

		BarVolumes.Name = "Volume";
		BarVolumes.DrawStyle = DRAWSTYLE_IGNORE;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = COLOR_GREEN;
		OHLCAvg.DrawZeros = true;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = COLOR_GREEN;
		HLCAvg.DrawZeros = true;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = COLOR_GREEN;
		HLAvg.DrawZeros = true;

		ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		ResetAtSessionStart.SetYesNo(1);

		ResetAtBothSessionStarts.Name = "Reset at Both Session Start Times";
		ResetAtBothSessionStarts.SetYesNo(false);

		return;
	}

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		bool Reset = false;

		if (Index == 0)
			Reset = true;

		else if (ResetAtBothSessionStarts.GetYesNo() != 0 )
		{
			SCDateTime PriorStartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index - 1], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,1);

			SCDateTime StartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,1);

			if (StartOfPeriod != PriorStartOfPeriod)
			{
				Reset = true;
			}

		}
		else if (ResetAtSessionStart.GetYesNo() != 0 )
		{
			SCDateTime PriorStartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index - 1], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0, 0);

			SCDateTime StartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,0);

			if (StartOfPeriod != PriorStartOfPeriod)
			{
				Reset = true;
			}

		}

		sc.CumulativeDeltaVolume(sc.BaseDataIn, BarCloses, Index, Reset);
		BarOpens[Index] = BarCloses.Arrays[0] [Index];
		BarHighs[Index] = BarCloses.Arrays[1] [Index];
		BarLows[Index] = BarCloses.Arrays[2] [Index];

		BarVolumes[Index] = sc.Volume[Index];

		sc.CalculateOHLCAverages(Index);
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeDeltaBarsTicks(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open    = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High    = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low     = sc.Subgraph[SC_LOW];
	SCSubgraphRef Close   = sc.Subgraph[SC_LAST];
	SCSubgraphRef OHLCAvg = sc.Subgraph[SC_OHLC_AVG];
	SCSubgraphRef HLCAvg  = sc.Subgraph[SC_HLC_AVG];
	SCSubgraphRef HLAvg   = sc.Subgraph[SC_HL_AVG];

	SCInputRef ResetAtSessionStart = sc.Input[2];
	SCInputRef ResetAtBothSessionStarts = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Delta Bars - Trades";

		
		sc.MaintainAdditionalChartDataArrays = 1;
		sc.AutoLoop = 0;//Manual looping

		sc.ValueFormat = 0;
		sc.GraphDrawType = GDT_CANDLESTICK;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = true;
		Open.DrawZeros = true;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,128,0);
		High.DrawZeros = true;

		Low.Name  = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = true;
		Low.DrawZeros = true;

		Close.Name = "Close";
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.PrimaryColor = RGB(128,0,0);
		Close.DrawZeros = true;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = COLOR_GREEN;
		OHLCAvg.DrawZeros = true;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = COLOR_GREEN;
		HLCAvg.DrawZeros = true;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = COLOR_GREEN;
		HLAvg.DrawZeros = true;

		ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		ResetAtSessionStart.SetYesNo(1);

		ResetAtBothSessionStarts.Name = "Reset at Both Session Start Times";
		ResetAtBothSessionStarts.SetYesNo(false);

		return;
	}

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		bool Reset = false;

		if (Index == 0)
			Reset = true;
		else if (ResetAtBothSessionStarts.GetYesNo() != 0 )
		{
			SCDateTime PriorStartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index - 1], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,1);

			SCDateTime StartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,1);

			if (StartOfPeriod != PriorStartOfPeriod)
				Reset = true;
		}
		else if (ResetAtSessionStart.GetYesNo() != 0 )
		{
			SCDateTime PriorStartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index - 1], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,0);

			SCDateTime StartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,0);

			if (StartOfPeriod != PriorStartOfPeriod)
				Reset = true;
		}


		sc.CumulativeDeltaTicks(sc.BaseDataIn, Close, Index, Reset);

		Open[Index] = Close.Arrays[0][Index];
		High[Index] = Close.Arrays[1][Index];
		Low[Index] = Close.Arrays[2][Index];

		sc.CalculateOHLCAverages(Index);
	}
}

/*==========================================================================*/
SCSFExport scsf_CumulativeDeltaBarsTickVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open    = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High    = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low     = sc.Subgraph[SC_LOW];
	SCSubgraphRef Close   = sc.Subgraph[SC_LAST];
	SCSubgraphRef OHLCAvg = sc.Subgraph[SC_OHLC_AVG];
	SCSubgraphRef HLCAvg  = sc.Subgraph[SC_HLC_AVG];
	SCSubgraphRef HLAvg   = sc.Subgraph[SC_HL_AVG];

	SCInputRef ResetAtSessionStart = sc.Input[2];
	SCInputRef ResetAtBothSessionStarts = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Cumulative Delta Bars - Up/Down Tick Volume";

		sc.MaintainAdditionalChartDataArrays = 1;
		sc.AutoLoop = 0;//Manual looping

		sc.ValueFormat = 0;
		sc.GraphDrawType = GDT_CANDLESTICK;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = true;
		Open.DrawZeros = true;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,128,0);
		High.DrawZeros = true;

		Low.Name  = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = true;
		Low.DrawZeros = true;

		Close.Name = "Close";
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.PrimaryColor = RGB(128,0,0);
		Close.DrawZeros = true;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = COLOR_GREEN;
		OHLCAvg.DrawZeros = true;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = COLOR_GREEN;
		HLCAvg.DrawZeros = true;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = COLOR_GREEN;
		HLAvg.DrawZeros = true;

		ResetAtSessionStart.Name = "Reset at Start of Trading Day";
		ResetAtSessionStart.SetYesNo(1);

		ResetAtBothSessionStarts.Name = "Reset at Both Session Start Times";
		ResetAtBothSessionStarts.SetYesNo(false);

		return;
	}


	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		bool Reset = false;

		if (Index == 0)
			Reset = true;
		else if (ResetAtBothSessionStarts.GetYesNo() != 0 )
		{
			SCDateTime PriorStartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index - 1], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,1);

			SCDateTime StartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,1);

			if (StartOfPeriod != PriorStartOfPeriod)
				Reset = true;
		}
		else if (ResetAtSessionStart.GetYesNo() != 0 )
		{
			SCDateTime PriorStartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index - 1], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,0);

			SCDateTime StartOfPeriod = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index], TIME_PERIOD_LENGTH_UNIT_DAYS ,1,0,0);

			if (StartOfPeriod != PriorStartOfPeriod)
				Reset = true;
		}

		sc.CumulativeDeltaTickVolume(sc.BaseDataIn, Close, Index, Reset);

		Open[Index] = Close.Arrays[0][Index];
		High[Index] = Close.Arrays[1][Index];
		Low[Index] = Close.Arrays[2][Index];


		sc.CalculateOHLCAverages(Index);
	}
}

/*==========================================================================*/
SCSFExport scsf_BackgroundDrawStyleExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Background   = sc.Subgraph[0];
	SCSubgraphRef BackgroundDC = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Background DrawStyle Example";
		sc.StudyDescription = "Background DrawStyle Example";

		sc.AutoLoop = true;
		

		sc.GraphRegion = 0;
		sc.DrawStudyUnderneathMainPriceGraph = 1;    // not required in studies, but want color behind price for this study

		Background.Name = "Background";
		Background.DrawStyle = DRAWSTYLE_BACKGROUND;
		Background.PrimaryColor = COLOR_LIGHTGREEN;
		Background.SecondaryColor = COLOR_LIGHTPINK;
		Background.SecondaryColorUsed = true;        // turn on if both colors are to be used
		Background.AutoColoring = AUTOCOLOR_POSNEG;  // use positive/negative values to signify colors

		BackgroundDC.Name = "Background DataColor";
		BackgroundDC.DrawStyle = DRAWSTYLE_BACKGROUND;
		BackgroundDC.PrimaryColor = RGB(255,0,255);

		return;
	}

	// Do data processing
	int min = sc.BaseDateTimeIn[sc.Index].GetMinute();

	if (min > 0 && min < 15)
		Background[sc.Index] = 0;    // do not color background
	else if (min >= 15 && min < 30)
		Background[sc.Index] = 1;    // use primary color
	else if (min >= 30 && min < 45)
		Background[sc.Index] = -1;   // use secondary color
	else if (min >= 45 && min < 60)
	{
		BackgroundDC[sc.Index] = 1;
		BackgroundDC.DataColor[sc.Index] = RGB(0, 0, 17*(60-min));
	}
}


/*==========================================================================*/
SCSFExport scsf_NumericInformationGraphDrawTypeExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Grid   = sc.Subgraph[0];
	SCSubgraphRef Open   = sc.Subgraph[1];
	SCSubgraphRef High   = sc.Subgraph[2];
	SCSubgraphRef Low    = sc.Subgraph[3];
	SCSubgraphRef Close  = sc.Subgraph[4];
	SCSubgraphRef Volume = sc.Subgraph[5];
	SCSubgraphRef AskBidVolumeDiff = sc.Subgraph[6];
	SCSubgraphRef PriceSMA = sc.Subgraph[7];

	SCInputRef FontSize = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Numeric Information Graph Draw Type Example";
		sc.StudyDescription = "Numeric Information Graph Draw Type Example";

		sc.AutoLoop = true;
		

		sc.GraphDrawType = GDT_NUMERIC_INFORMATION;

		FontSize.Name = "Font Size";
		FontSize.SetInt(8);
		FontSize.SetIntLimits(0,100);


		Grid.Name = "Grid Style";
		Grid.DrawStyle = DRAWSTYLE_LINE;
		Grid.PrimaryColor = COLOR_GRAY;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = COLOR_CYAN;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = COLOR_GREEN;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = COLOR_RED;

		Close.Name = "Close";
		Close.DrawStyle = DRAWSTYLE_LINE;
		Close.PrimaryColor = COLOR_PURPLE;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_LINE;
		Volume.PrimaryColor = COLOR_WHITE;

		AskBidVolumeDiff.Name = "AB Vol Diff";
		AskBidVolumeDiff.DrawStyle = DRAWSTYLE_LINE;
		AskBidVolumeDiff.PrimaryColor = COLOR_WHITE;

		PriceSMA.Name = "SMA";
		PriceSMA.DrawStyle = DRAWSTYLE_LINE;
		PriceSMA.PrimaryColor = COLOR_WHITE;

		return;
	}

	if (sc.Index == 0)
	{
		sc.ValueFormat = sc.BaseGraphValueFormat;

		// set up the information graph draw type
		s_NumericInformationGraphDrawTypeConfig NumericInformationGraphDrawTypeConfig;

		// only need to override defaults (see s_NumericInformationGraphDrawTypeConfig for defaults and other options to override)

		NumericInformationGraphDrawTypeConfig.TransparentTextBackground = false;
		NumericInformationGraphDrawTypeConfig.GridlineStyleSubgraphIndex = 0;  // Subgraph 1 to specify grid line style
		NumericInformationGraphDrawTypeConfig.FontSize = FontSize.GetInt();
		NumericInformationGraphDrawTypeConfig.ValueFormat[5] = 0;  // set value format for volume, others are inherited
		NumericInformationGraphDrawTypeConfig.ShowPullback = true;

		sc.SetNumericInformationGraphDrawTypeConfig(NumericInformationGraphDrawTypeConfig);

		// display volume first
		sc.SetNumericInformationDisplayOrderFromString("6,2,3,4,5,7,8");
	}

	// Do data processing
	Open[sc.Index]   = sc.Open[sc.Index];
	High[sc.Index]   = sc.High[sc.Index];
	Low[sc.Index]    = sc.Low[sc.Index];
	Close[sc.Index]  = sc.Close[sc.Index];
	Volume[sc.Index] = sc.Volume[sc.Index];

	SCFloatArrayRef SMA = Close.Arrays[0];

	sc.MovingAverage(sc.Close, SMA, MOVAVGTYPE_SIMPLE, 10);

	PriceSMA[sc.Index] = SMA[sc.Index];

	AskBidVolumeDiff[sc.Index] = sc.AskVolume[sc.Index] - sc.BidVolume[sc.Index];

	//Set value in column after last chart bar column (pullback column)
	if (sc.Index == sc.ArraySize - 1)
		AskBidVolumeDiff[sc.Index + 1] = AskBidVolumeDiff[sc.Index];

	Low.DataColor[sc.Index] = sc.CombinedForegroundBackgroundColorRef(COLOR_BLACK, COLOR_YELLOW);


	// set cell foreground/background color
	if (Close[sc.Index] > Close[sc.Index-1])
		Close.DataColor[sc.Index] = sc.CombinedForegroundBackgroundColorRef(COLOR_BLACK, COLOR_GREEN);
	else
		Close.DataColor[sc.Index] = sc.CombinedForegroundBackgroundColorRef(COLOR_WHITE, COLOR_RED);
}




/*==========================================================================*/
SCSFExport scsf_StudySubgraphAbsoluteValue(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if (sc.SetDefaults)
	{

		sc.GraphName = "Study Subgraph Absolute Value";
		sc.StudyDescription = "This study calculates and displays the absolute value of the selected study Subgraph. To select the study to use, set the Based On study setting to that study and set the Input Data input to the specific study Subgraph in that study selected.";

		sc.AutoLoop = 0;

		

		Result.Name = "Abs";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0,255,0);
		Result.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		return;
	}

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		Result[Index] = fabs(sc.BaseData[InputData.GetInputDataIndex()][Index]);
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

}



/*==========================================================================*/
SCSFExport scsf_VolumeBitcoin(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];
	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume - Bitcoin";

		sc.ValueFormat = sc.VolumeValueFormat;
		sc.AutoLoop = 1;

		Volume.Name = "Vol";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.LineWidth= 2;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.SecondaryColor = RGB(255,0,0);
		Volume.SecondaryColorUsed = true;
		Volume.DrawZeros = true;
		Volume.AutoColoring= AUTOCOLOR_BASEGRAPH;

		sc.ScaleRangeType=SCALE_ZEROBASED;

		return;
	}

	Volume[sc.Index] = (float)sc.Volume[sc.Index]*sc.MultiplierFromVolumeValueFormat();
}



/*==========================================================================*/
SCSFExport scsf_VolumeZoneOscillator(SCStudyInterfaceRef sc)
{
	SCInputRef Period           = sc.Input[1];
	SCInputRef OverBoughtLevel3 = sc.Input[2];
	SCInputRef OverBoughtLevel2 = sc.Input[3];
	SCInputRef OverBoughtLevel1 = sc.Input[4];
	SCInputRef OverSoldLevel1   = sc.Input[5];
	SCInputRef OverSoldLevel2   = sc.Input[6];
	SCInputRef OverSoldLevel3   = sc.Input[7];


	SCSubgraphRef OverBought3 = sc.Subgraph[0];
	SCSubgraphRef OverBought2 = sc.Subgraph[1];
	SCSubgraphRef OverBought1 = sc.Subgraph[2];
	SCSubgraphRef Zero        = sc.Subgraph[3];
	SCSubgraphRef OverSold1   = sc.Subgraph[4];
	SCSubgraphRef OverSold2   = sc.Subgraph[5];
	SCSubgraphRef OverSold3   = sc.Subgraph[6];
	SCSubgraphRef VZO         = sc.Subgraph[7];

	SCFloatArrayRef R  = VZO.Arrays[0];
	SCFloatArrayRef VP = VZO.Arrays[1];
	SCFloatArrayRef TV = VZO.Arrays[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume Zone Oscillator";

		sc.StudyDescription = "TASC Article, May, 2011, In The Volume Zone";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;
		sc.DrawZeros = 1;

		// inputs
		Period.Name = "Period";
		Period.SetInt(14);
		Period.SetIntLimits(1, MAX_STUDY_LENGTH);

		OverBoughtLevel3.Name = "Overbought Line3 Value";
		OverBoughtLevel3.SetFloat(60.0f);

		OverBoughtLevel2.Name = "Overbought Line2 Value";
		OverBoughtLevel2.SetFloat(40.0f);

		OverBoughtLevel1.Name = "Overbought Line1 Value";
		OverBoughtLevel1.SetFloat(15.0f);

		OverSoldLevel1.Name = "Oversold Line1 Value";
		OverSoldLevel1.SetFloat(-5.0f);

		OverSoldLevel2.Name = "Oversold Line2 Value";
		OverSoldLevel2.SetFloat(-40.0f);

		OverSoldLevel3.Name = "Oversold Line3 Value";
		OverSoldLevel3.SetFloat(-60.0f);

		// subgraphs
		VZO.Name = "VZO";
		VZO.DrawStyle = DRAWSTYLE_LINE;
		VZO.PrimaryColor = RGB(0,255,0);
		VZO.DrawZeros = true;

		OverBought3.Name = "Over Bought Level 3";
		OverBought3.DrawStyle = DRAWSTYLE_LINE;
		OverBought3.PrimaryColor = RGB(255,0,255);
		OverBought3.DrawZeros = true;
		OverBought3.DisplayNameValueInWindowsFlags = 0;

		OverBought2.Name = "Over Bought Level 2";
		OverBought2.DrawStyle = DRAWSTYLE_LINE;
		OverBought2.PrimaryColor = RGB(255,0,255);
		OverBought2.DrawZeros = true;
		OverBought2.DisplayNameValueInWindowsFlags = 0;

		OverBought1.Name = "Over Bought Level 1";
		OverBought1.DrawStyle = DRAWSTYLE_LINE;
		OverBought1.PrimaryColor = RGB(255,0,255);
		OverBought1.DrawZeros = true;
		OverBought1.DisplayNameValueInWindowsFlags = 0;

		Zero.Name = "Zero";
		Zero.DrawStyle = DRAWSTYLE_LINE;
		Zero.PrimaryColor = RGB(255,255,255);
		Zero.DrawZeros = true;
		Zero.DisplayNameValueInWindowsFlags = 0;

		OverSold1.Name = "Over Sold Level 1";
		OverSold1.DrawStyle = DRAWSTYLE_LINE;
		OverSold1.PrimaryColor = RGB(255,255,0);
		OverSold1.DrawZeros = true;
		OverSold1.DisplayNameValueInWindowsFlags = 0;

		OverSold2.Name = "Over Sold Level 2";
		OverSold2.DrawStyle = DRAWSTYLE_LINE;
		OverSold2.PrimaryColor = RGB(255,255,0);
		OverSold2.DrawZeros = true;
		OverSold2.DisplayNameValueInWindowsFlags = 0;

		OverSold3.Name = "Over Sold Level 3";
		OverSold3.DrawStyle = DRAWSTYLE_LINE;
		OverSold3.PrimaryColor = RGB(255,255,0);
		OverSold3.DrawZeros = true;
		OverSold3.DisplayNameValueInWindowsFlags = 0;

		return;
	}

	OverBought3[sc.Index] = OverBoughtLevel3.GetFloat();
	OverBought2[sc.Index] = OverBoughtLevel2.GetFloat();
	OverBought1[sc.Index] = OverBoughtLevel1.GetFloat();
	Zero[sc.Index] = 0;
	OverSold1[sc.Index] = OverSoldLevel1.GetFloat();
	OverSold2[sc.Index] = OverSoldLevel2.GetFloat();
	OverSold3[sc.Index] = OverSoldLevel3.GetFloat();

	if (sc.Index == 0)
	{
		R[sc.Index] = sc.Volume[sc.Index];
		return;
	}
	else
	{
		if (sc.Close[sc.Index] > sc.Close[sc.Index-1])
			R[sc.Index] = sc.Volume[sc.Index];
		else
			R[sc.Index] = -sc.Volume[sc.Index];
	}

	sc.MovingAverage(R, VP, MOVAVGTYPE_EXPONENTIAL, Period.GetInt());
	sc.MovingAverage(sc.Volume, TV, MOVAVGTYPE_EXPONENTIAL, Period.GetInt());

	if (TV[sc.Index] != 0)
	{
		VZO[sc.Index] = VP[sc.Index] / TV[sc.Index] * 100.0f;
	}
}

/*==========================================================================*/
SCSFExport scsf_FisherTransform(SCStudyInterfaceRef sc)
{
	SCInputRef iPrice  = sc.Input[0];
	SCInputRef iPeriod = sc.Input[1];

	SCSubgraphRef Zero              = sc.Subgraph[0];
	SCSubgraphRef FisherTransform   = sc.Subgraph[1];
	SCSubgraphRef FisherTransformM1 = sc.Subgraph[2];

	SCFloatArrayRef Value = Zero.Arrays[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Fisher Transform";

		sc.StudyDescription = "Fisher Transform";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;
		sc.DrawZeros = 1;

		// inputs
		iPrice.Name = "Price";
		iPrice.SetInputDataIndex(SC_HL_AVG);

		iPeriod.Name = "Period";
		iPeriod.SetInt(10);
		iPeriod.SetIntLimits(1, MAX_STUDY_LENGTH);

		// subgraphs
		FisherTransform.Name = "FisherTransform";
		FisherTransform.DrawStyle = DRAWSTYLE_LINE;
		FisherTransform.PrimaryColor = RGB(255,0,0);
		FisherTransform.DrawZeros = true;

		FisherTransformM1.Name = "FisherTransformOffset";
		FisherTransformM1.DrawStyle = DRAWSTYLE_LINE;
		FisherTransformM1.PrimaryColor = RGB(0,0,255);
		FisherTransformM1.DrawZeros = true;

		Zero.Name = "Zero";
		Zero.DrawStyle = DRAWSTYLE_LINE;
		Zero.PrimaryColor = RGB(255,255,255);
		Zero.DrawZeros = true;
		Zero.DisplayNameValueInWindowsFlags = 0;

		return;
	}

	Zero[sc.Index] = 0;

	if (sc.Index != 0)
	{
		SCFloatArrayRef Price = sc.BaseData[iPrice.GetInputDataIndex()];

		float Highest = sc.GetHighest(Price, iPeriod.GetInt());
		float Lowest  = sc.GetLowest(Price, iPeriod.GetInt());
		float Range   = Highest - Lowest;

		if (Range == 0)
			Value[sc.Index] = 0;
		else
			Value[sc.Index] = .66f * ((Price[sc.Index] - Lowest) / Range - 0.5f) + 0.67f * Value[sc.Index-1];

		float TruncValue = Value[sc.Index];

		if (TruncValue > .99f)
			TruncValue = .999f;
		else if (TruncValue < -.99f)
			TruncValue = -.999f;

		float Fisher = .5f * (log((1 + TruncValue) / (1 - TruncValue)) + FisherTransform[sc.Index-1]);

		FisherTransformM1[sc.Index] = FisherTransform[sc.Index-1];
		FisherTransform[sc.Index] = Fisher;
	}
}

/*==========================================================================*/
SCSFExport scsf_ZScore(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ZScore    = sc.Subgraph[0];

	SCFloatArrayRef Avg     = ZScore.Arrays[0];
	SCFloatArrayRef StdDev  = ZScore.Arrays[1];

	SCInputRef InputData    = sc.Input[0];
	SCInputRef MeanLength   = sc.Input[1];
	SCInputRef StdDevLength = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Z-Score";

		sc.ValueFormat = 2;
		sc.AutoLoop = 1;
		

		ZScore.Name = "Z-Score";
		ZScore.DrawStyle = DRAWSTYLE_LINE;
		ZScore.PrimaryColor = RGB(192, 192, 192);
		ZScore.SecondaryColor = RGB(192, 192, 192);
		ZScore.SecondaryColorUsed = 1;
		ZScore.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MeanLength.Name = "Mean Length";
		MeanLength.SetInt(10);
		MeanLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		StdDevLength.Name = "Standard Deviation Length";
		StdDevLength.SetInt(10);
		StdDevLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	if (sc.Index == 0)
	{
		sc.DataStartIndex = max(MeanLength.GetInt() - 1, StdDevLength.GetInt() - 1);
	}

	sc.SimpleMovAvg(sc.BaseData[InputData.GetInputDataIndex()], Avg,  MeanLength.GetInt());

	sc.StdDeviation(sc.BaseData[InputData.GetInputDataIndex()], StdDev, StdDevLength.GetInt());

	if (StdDev[sc.Index] <= 0)
		ZScore[sc.Index] = 0;
	else
		ZScore[sc.Index] = (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] - Avg[sc.Index]) / StdDev[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_VolumeAtPriceThresholdAlert(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PriceTriggeredForBid   = sc.Subgraph[0];
	SCSubgraphRef PriceTriggeredForAsk   = sc.Subgraph[1];
	SCSubgraphRef PriceTriggeredForTotal = sc.Subgraph[2];

	SCInputRef BidVolumeThreshold   = sc.Input[1];
	SCInputRef BidAlertNum          = sc.Input[2];
	SCInputRef AskVolumeThreshold   = sc.Input[3];
	SCInputRef AskAlertNum          = sc.Input[4];
	SCInputRef TotalVolumeThreshold = sc.Input[5];
	SCInputRef TotalAlertNum        = sc.Input[6];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Volume At Price Threshold Alert";
		sc.StudyDescription = "This study triggers alerts based on configurable volume thresholds for the bid/ask/total volume at price.";
		
		sc.GraphRegion = 0;		
		sc.AutoLoop = 0;//Manual looping
		sc.ValueFormat = sc.BaseGraphValueFormat;

		
		
		sc.MaintainVolumeAtPriceData = 1;  // true

		PriceTriggeredForBid.Name = "Price Triggered By Bid Threshold";
		PriceTriggeredForBid.DrawStyle = DRAWSTYLE_IGNORE;
		PriceTriggeredForBid.DrawZeros = 0;

		PriceTriggeredForAsk.Name = "Price Triggered By Ask Threshold";
		PriceTriggeredForAsk.DrawStyle = DRAWSTYLE_IGNORE;
		PriceTriggeredForAsk.DrawZeros = 0;

		PriceTriggeredForTotal.Name = "Price Triggered By Total Threshold";
		PriceTriggeredForTotal.DrawStyle = DRAWSTYLE_IGNORE;
		PriceTriggeredForTotal.DrawZeros = 0;

		BidVolumeThreshold.Name = "Bid Volume Threshold (0 disables)";
		BidVolumeThreshold.SetInt(100);

		BidAlertNum.Name = "Bid Volume Alert";
		BidAlertNum.SetAlertSoundNumber(0);

		AskVolumeThreshold.Name = "Ask Volume Threshold (0 disables)";
		AskVolumeThreshold.SetInt(100);

		AskAlertNum.Name = "Ask Volume Alert";
		AskAlertNum.SetAlertSoundNumber(0);

		TotalVolumeThreshold.Name = "Total Volume Threshold (0 disables)";
		TotalVolumeThreshold.SetInt(100);

		TotalAlertNum.Name = "Total Volume Alert";
		TotalAlertNum.SetAlertSoundNumber(0);

		return;
	}

	// Do data processing
	if (sc.UpdateStartIndex == 0)
	{
		sc.ValueFormat = sc.BaseGraphValueFormat;
	}

	if ((int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
		return;//This is an indication that the volume at price data does not exist

	SCString AlertMessage;
	int Volume = 0;
	int VolumeThreshold = 0;
	bool EnableAlerts = sc.IsFullRecalculation == 0 && !sc.DownloadingHistoricalData;

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		const s_VolumeAtPriceV2 *p_VolumeAtPrice = nullptr;

		int NumVAPElementsAtBarIndex = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(Index);
		for (int VAPIndex = 0; VAPIndex < NumVAPElementsAtBarIndex; VAPIndex++)
		{
			sc.VolumeAtPriceForBars->GetVAPElementAtIndex(Index, VAPIndex, &p_VolumeAtPrice);

			float Price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;

			// check the bid volume at price
			VolumeThreshold = BidVolumeThreshold.GetInt();

			if (VolumeThreshold > 0 && PriceTriggeredForBid[Index] == 0)
			{
				Volume = p_VolumeAtPrice->BidVolume;

				if (Volume >= VolumeThreshold)
				{
					PriceTriggeredForBid[Index] = Price;

					if (EnableAlerts && BidAlertNum.GetAlertSoundNumber() > 0  && Index == sc.ArraySize - 1)
					{
						AlertMessage.Format("Bid Volume %i triggered alert at %s", 
							Volume, 
							sc.FormatGraphValue(Price, sc.ValueFormat).GetChars()
							);

						sc.SetAlert(BidAlertNum.GetAlertSoundNumber() - 1 , AlertMessage);
					}
				}
			}

			// check the ask volume at price
			VolumeThreshold = AskVolumeThreshold.GetInt();

			if (VolumeThreshold > 0 && PriceTriggeredForAsk[Index] == 0)
			{
				Volume = p_VolumeAtPrice->AskVolume;

				if (Volume >= VolumeThreshold)
				{
					PriceTriggeredForAsk[Index] = Price;

					if (EnableAlerts && AskAlertNum.GetAlertSoundNumber() > 0  && Index == sc.ArraySize - 1)
					{
						AlertMessage.Format("Ask Volume %i triggered alert at %s", 
							Volume, 
							sc.FormatGraphValue(Price, sc.ValueFormat).GetChars()
							);

						sc.SetAlert(AskAlertNum.GetAlertSoundNumber() - 1 , AlertMessage);
					}
				}
			}

			// check the total volume at price
			VolumeThreshold = TotalVolumeThreshold.GetInt();

			if (VolumeThreshold > 0 && PriceTriggeredForTotal[Index] == 0)
			{
				Volume = p_VolumeAtPrice->Volume;

				if (Volume >= VolumeThreshold)
				{
					PriceTriggeredForTotal[Index] = Price;

					if (EnableAlerts && TotalAlertNum.GetAlertSoundNumber() > 0 && Index == sc.ArraySize - 1)
					{
						AlertMessage.Format("Total Volume %i triggered alert at %s", 
							Volume,
							sc.FormatGraphValue(Price, sc.ValueFormat).GetChars()
							);

						sc.SetAlert(TotalAlertNum.GetAlertSoundNumber() - 1 , AlertMessage);
					}
				}
			}
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_VolumeAtPriceThresholdAlertV2(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ExtensionLinePropertiesSubgraph = sc.Subgraph[SC_SUBGRAPHS_AVAILABLE - 1];
	SCSubgraphRef CountOfAlerts = sc.Subgraph[SC_SUBGRAPHS_AVAILABLE - 2];

	int AdjacentAlertsHighlightSubgraphStartingNumber = SC_SUBGRAPHS_AVAILABLE - 12;

	SCInputRef Input_ComparisonMethod = sc.Input[0];
	SCInputRef Input_VolumeThreshold = sc.Input[1];
	SCInputRef Input_AlertNumber = sc.Input[2];
	SCInputRef Input_DrawExtensionLines = sc.Input[3];
	SCInputRef Input_DrawExtensionLinesWithTransparentRange = sc.Input[4];
	SCInputRef Input_PercentageThreshold = sc.Input[5];
	SCInputRef Input_AdditionalVolumeThreshold = sc.Input[6];
	SCInputRef Input_AllowZeroValueCompares = sc.Input[7];
	SCInputRef Input_DivideByZeroAction = sc.Input[8];
	SCInputRef Input_HighlightAdjacentAlertsGroupSize = sc.Input[9];
	SCInputRef Input_DrawExtensionLinesUntilEndOfChart = sc.Input[10];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Volume At Price Threshold Alert V2";

		sc.GraphRegion = 0;		
		sc.AutoLoop = 0;//Manual looping
		sc.ValueFormat = sc.BaseGraphValueFormat;

		

		sc.MaintainVolumeAtPriceData = 1;  // true
	
		for (int SubgraphIndex = 0; SubgraphIndex < SC_SUBGRAPHS_AVAILABLE - 13; ++SubgraphIndex)
		{
			SCString SubgraphName;
			SubgraphName.Format("Trigger %d", SubgraphIndex);

			sc.Subgraph[SubgraphIndex].Name = SubgraphName;
			sc.Subgraph[SubgraphIndex].PrimaryColor = RGB(255, 128, 0);
			sc.Subgraph[SubgraphIndex].DrawStyle = DRAWSTYLE_SQUARE_OFFSET_LEFT_FOR_CANDLESTICK;
			sc.Subgraph[SubgraphIndex].LineWidth = 8;
			sc.Subgraph[SubgraphIndex].DrawZeros = 0;
			sc.Subgraph[SubgraphIndex].DisplayNameValueInWindowsFlags = 0;
		}

		CountOfAlerts.Name = "Count of Alerts";
		CountOfAlerts.DrawStyle = DRAWSTYLE_IGNORE;
		CountOfAlerts.PrimaryColor = RGB(0, 255, 0);
		CountOfAlerts.DrawZeros = 1;

		for (int SubgraphIndex = 0; SubgraphIndex < 10; ++SubgraphIndex)
		{
			SCString SubgraphName;
			SubgraphDrawStyles AdjacentAlertsDrawStyle;
			if (SubgraphIndex % 2 == 0)
			{
				SubgraphName.Format("Adjacent Alert Highlight Bottom %d", (int)(SubgraphIndex / 2) + 1);
				AdjacentAlertsDrawStyle = DRAWSTYLE_LEFT_OFFSET_BOX_TOP_FOR_CANDLESTICK;
			}
			else
			{
				SubgraphName.Format("Adjacent Alert Highlight Top %d", (int)(SubgraphIndex / 2) + 1);
				AdjacentAlertsDrawStyle = DRAWSTYLE_LEFT_OFFSET_BOX_BOTTOM_FOR_CANDLESTICK;
			}

			sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + SubgraphIndex].Name = SubgraphName;
			sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + SubgraphIndex].PrimaryColor = RGB(255, 255, 0);
			sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + SubgraphIndex].DrawStyle = AdjacentAlertsDrawStyle;
			sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + SubgraphIndex].LineWidth = 8;
		}

		ExtensionLinePropertiesSubgraph.Name = "Extension Line Properties";
		ExtensionLinePropertiesSubgraph.DrawStyle = DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY;
		ExtensionLinePropertiesSubgraph.LineWidth = 1;
		ExtensionLinePropertiesSubgraph.PrimaryColor =  RGB (255, 0, 255);
		ExtensionLinePropertiesSubgraph.DrawZeros = false;

		Input_ComparisonMethod.Name = "Comparison Method";
		Input_ComparisonMethod.SetCustomInputStrings("Bid Volume;Ask Volume;Total Volume;Number of Trades;Ask Volume Bid Volume Difference;Ask Volume Bid Volume Diagonal Difference;Ask Volume Bid Volume Ratio;Ask Volume Bid Volume Diagonal Ratio;Bid Volume and Ask Volume Separately");
		Input_ComparisonMethod.SetCustomInputIndex(2);

		Input_VolumeThreshold.Name = "Volume Threshold";
		Input_VolumeThreshold.SetInt(100);

		Input_AlertNumber.Name = "Volume Alert Number";
		Input_AlertNumber.SetAlertSoundNumber(0);

		Input_DrawExtensionLines.Name = "Draw Extension Lines";
		Input_DrawExtensionLines.SetYesNo(false);

		Input_DrawExtensionLinesWithTransparentRange.Name = "Draw Extension Lines With Transparent Range";
		Input_DrawExtensionLinesWithTransparentRange.SetYesNo(false);

		Input_PercentageThreshold.Name = "Percentage Threshold";
		Input_PercentageThreshold.SetInt(150);
		
		Input_AdditionalVolumeThreshold.Name = "Additional Volume Threshold";
		Input_AdditionalVolumeThreshold.SetInt(100);

		Input_AllowZeroValueCompares.Name = "Enable Zero Bid/Ask Compares";
		Input_AllowZeroValueCompares.SetYesNo(0);

		Input_DivideByZeroAction.Name = "Zero Value Compare Action";
		Input_DivideByZeroAction.SetCustomInputStrings("Set 0 to 1;Set Percentage to +/- 1000%");
		Input_DivideByZeroAction.SetCustomInputIndex(0);

		Input_HighlightAdjacentAlertsGroupSize.Name = "Highlight Adjacent Alerts Minimum Group Size";
		Input_HighlightAdjacentAlertsGroupSize.SetInt(0);

		Input_DrawExtensionLinesUntilEndOfChart.Name = "Draw Extension Lines until End of Chart";
		Input_DrawExtensionLinesUntilEndOfChart.SetYesNo(false);

		sc.ValueFormat = VALUEFORMAT_INHERITED;

		return;
	}
			
	//This is an indication that the volume at price data does not exist
	if ((int)sc.VolumeAtPriceForBars->GetNumberOfBars() < sc.ArraySize)
		return;

	//The Subgraph display properties need to be the same for all Subgraphs. If the properties at Subgraph index 1 are different than at subgraph index 0, then apply Subgraph 0 properties to the rest.

	if (sc.Subgraph[1].PrimaryColor != sc.Subgraph[0].PrimaryColor
		|| sc.Subgraph[1].DrawStyle != sc.Subgraph[0].DrawStyle
		|| sc.Subgraph[1].LineWidth != sc.Subgraph[0].LineWidth
	)
	{
		for (int SubgraphIndex = 1; SubgraphIndex < SC_SUBGRAPHS_AVAILABLE - 13; ++SubgraphIndex)
		{
			sc.Subgraph[SubgraphIndex].PrimaryColor = sc.Subgraph[0].PrimaryColor;
			sc.Subgraph[SubgraphIndex].DrawStyle = sc.Subgraph[0].DrawStyle;
			sc.Subgraph[SubgraphIndex].LineWidth = sc.Subgraph[0].LineWidth;
		}
	}

	SCString AlertMessage;
	bool EnableAlerts = sc.IsFullRecalculation == 0 && !sc.DownloadingHistoricalData;

	if (Input_HighlightAdjacentAlertsGroupSize.GetInt() == 0)
	{
		for (int SubgraphIndex = 0; SubgraphIndex < 10; ++SubgraphIndex)
		{
			sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + SubgraphIndex].DrawStyle = DRAWSTYLE_IGNORE;
		}
	}
	else if (Input_HighlightAdjacentAlertsGroupSize.GetInt() != 0)
	{
		bool AllSetToIgnore = true;
		for (int SubgraphIndex = 0; SubgraphIndex < 10; ++SubgraphIndex)
		{
			if (sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + SubgraphIndex].DrawStyle != DRAWSTYLE_IGNORE)
			{
				AllSetToIgnore = false;
				break;
			}
		}
		if (AllSetToIgnore)
		{
			for (int SubgraphIndex = 0; SubgraphIndex < 10; ++SubgraphIndex)
			{
				if (SubgraphIndex % 2 == 0)
				{
					sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + SubgraphIndex].DrawStyle = DRAWSTYLE_LEFT_OFFSET_BOX_TOP_FOR_CANDLESTICK;
				}
				else
				{
					sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + SubgraphIndex].DrawStyle = DRAWSTYLE_LEFT_OFFSET_BOX_BOTTOM_FOR_CANDLESTICK;
				}
			}
		}
	}

	int HighlightAdjacentAlertsGroupSizeValue = Input_HighlightAdjacentAlertsGroupSize.GetInt();

	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		int AvailableSubgraphIndex = 0;

		CountOfAlerts[BarIndex] = 0;

		bool AdjacentAlertsHaveBottomHighlightAlertPrice = false;
		int AdjacentAlertsHighlightBottomPriceIndex = 0;
		bool AdjacentAlertsGotGroup = false;
		int AdjacentAlertsGroupNumber = -1;
		int AdjacentAlertsCountInGroup = 0;

		//Reset all subgraph values
		for (int SubgraphIndex = 0; SubgraphIndex < SC_SUBGRAPHS_AVAILABLE - 2; ++SubgraphIndex)
			sc.Subgraph[SubgraphIndex].Data[BarIndex] = 0;

		int NumberOfPricesAtBarIndex = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(BarIndex);

		for (int PriceIndex = 0; PriceIndex < NumberOfPricesAtBarIndex; ++PriceIndex)
		{
			s_VolumeAtPriceV2 *p_VolumeAtPrice = NULL;

			if (!sc.VolumeAtPriceForBars->GetVAPElementAtIndex(BarIndex, PriceIndex, &p_VolumeAtPrice))
				continue;

			s_VolumeAtPriceV2 *p_NextVolumeAtPrice = NULL;

			if (PriceIndex < NumberOfPricesAtBarIndex - 1)
				sc.VolumeAtPriceForBars->GetVAPElementAtIndex(BarIndex, PriceIndex + 1, &p_NextVolumeAtPrice);

			float Price = p_VolumeAtPrice->PriceInTicks * sc.TickSize;
			float PriceForExtensionLine = Price;

			// Check if condition has been met
			int ComparisonMethodIndex = Input_ComparisonMethod.GetIndex();
			bool ConditionMet = false;
			unsigned int VolumeThresholdValue = Input_VolumeThreshold.GetInt();
			unsigned int AdditionalVolumeThresholdValue = Input_AdditionalVolumeThreshold.GetInt();

			if (ComparisonMethodIndex == 0)//Bid Volume
			{
				if ((VolumeThresholdValue > 0
					&& p_VolumeAtPrice->BidVolume >= VolumeThresholdValue)
					|| (VolumeThresholdValue == 0 && p_VolumeAtPrice->BidVolume == 0)
					)
					ConditionMet = true;

			}
			else if (ComparisonMethodIndex == 1)//Ask Volume
			{
				if (( VolumeThresholdValue > 0
					&& p_VolumeAtPrice->AskVolume >= VolumeThresholdValue)
					|| (VolumeThresholdValue == 0 && p_VolumeAtPrice->AskVolume == 0)
					)
					ConditionMet = true;
			}
			else if (ComparisonMethodIndex == 2)//Total Volume
			{
				if ((VolumeThresholdValue > 0
					&& p_VolumeAtPrice->Volume >= VolumeThresholdValue)
					|| (VolumeThresholdValue == 0 && p_VolumeAtPrice->Volume == 0)
					)
					ConditionMet = true;
			}
			else if (ComparisonMethodIndex == 3)//Number of Trades
			{
				if ((VolumeThresholdValue > 0
					&& p_VolumeAtPrice->NumberOfTrades >= VolumeThresholdValue)
					|| (VolumeThresholdValue == 0 && p_VolumeAtPrice->NumberOfTrades == 0)
					)
					ConditionMet = true;
			}
			else if (ComparisonMethodIndex == 4)//Ask Volume Bid Volume Difference
			{
				int AskVolumeBidVolumeDifference = p_VolumeAtPrice->AskVolume - p_VolumeAtPrice->BidVolume;

				int VolumeThresholdSigned = Input_VolumeThreshold.GetInt();

				if (AskVolumeBidVolumeDifference > 0 && VolumeThresholdSigned > 0 && AskVolumeBidVolumeDifference >= VolumeThresholdSigned)
					ConditionMet = true;
				else if (AskVolumeBidVolumeDifference < 0 && VolumeThresholdSigned < 0 && AskVolumeBidVolumeDifference <= VolumeThresholdSigned)
					ConditionMet = true;

			}
			else if (ComparisonMethodIndex == 5)//Ask Volume Bid Volume Diagonal Difference
			{
				int AskVolumeBidVolumeDifference = 0;

				if (p_NextVolumeAtPrice != NULL)
				{
					AskVolumeBidVolumeDifference = p_NextVolumeAtPrice->AskVolume - p_VolumeAtPrice->BidVolume;
					if (p_NextVolumeAtPrice->AskVolume > p_VolumeAtPrice->BidVolume)
						PriceForExtensionLine = p_NextVolumeAtPrice->PriceInTicks * sc.TickSize;
				}

				int VolumeThresholdSigned = Input_VolumeThreshold.GetInt();

				if (AskVolumeBidVolumeDifference > 0 && VolumeThresholdSigned > 0 && AskVolumeBidVolumeDifference >= VolumeThresholdSigned)
					ConditionMet = true;
				else if (AskVolumeBidVolumeDifference < 0 && VolumeThresholdSigned < 0 && AskVolumeBidVolumeDifference <= VolumeThresholdSigned)
					ConditionMet = true;

			}
			else if (ComparisonMethodIndex == 6)//Ask Volume Bid Volume Ratio
			{
				bool AllowZeroValueComparesSetting = Input_AllowZeroValueCompares.GetYesNo();
				unsigned int DivideByZeroActionIndex = Input_DivideByZeroAction.GetIndex();
				int AskVolumeBidVolumeRatioPercent = 0;

				if ((p_VolumeAtPrice->AskVolume > 0 && p_VolumeAtPrice->BidVolume > 0) || AllowZeroValueComparesSetting)
				{
					if (p_VolumeAtPrice->AskVolume >= p_VolumeAtPrice->BidVolume)
					{
						if (p_VolumeAtPrice->BidVolume == 0 && DivideByZeroActionIndex == 0)
							AskVolumeBidVolumeRatioPercent = (p_VolumeAtPrice->AskVolume / 1) * 100;
						else if (p_VolumeAtPrice->BidVolume == 0 && DivideByZeroActionIndex == 1)
							AskVolumeBidVolumeRatioPercent = 1000;
						else
							AskVolumeBidVolumeRatioPercent = sc.Round(((float)p_VolumeAtPrice->AskVolume / p_VolumeAtPrice->BidVolume) * 100);
					}
					else
					{
						if (p_VolumeAtPrice->AskVolume == 0 && DivideByZeroActionIndex == 0)
							AskVolumeBidVolumeRatioPercent = (p_VolumeAtPrice->BidVolume / 1) * -100;
						else if (p_VolumeAtPrice->AskVolume == 0 && DivideByZeroActionIndex == 1)
							AskVolumeBidVolumeRatioPercent = -1000;
						else
							AskVolumeBidVolumeRatioPercent = sc.Round(((float)p_VolumeAtPrice->BidVolume / p_VolumeAtPrice->AskVolume) * -100);
					}
				}

				int PercentThresholdSigned = Input_PercentageThreshold.GetInt();

				if (AskVolumeBidVolumeRatioPercent > 0 && PercentThresholdSigned > 0 && AskVolumeBidVolumeRatioPercent >= PercentThresholdSigned)
					ConditionMet = true;
				else if (AskVolumeBidVolumeRatioPercent < 0 && PercentThresholdSigned < 0 && AskVolumeBidVolumeRatioPercent <= PercentThresholdSigned)
					ConditionMet = true;
			}
			else if (ComparisonMethodIndex == 7)//Ask Volume Bid Volume Diagonal Ratio
			{
				bool AllowZeroValueComparesSetting = Input_AllowZeroValueCompares.GetYesNo();
				unsigned int DivideByZeroActionIndex = Input_DivideByZeroAction.GetIndex();
				int AskVolumeBidVolumeRatioPercent = 0;

				if (p_NextVolumeAtPrice != NULL)
				{
					if ((p_NextVolumeAtPrice->AskVolume >= p_VolumeAtPrice->BidVolume) && (p_VolumeAtPrice->BidVolume > 0 || AllowZeroValueComparesSetting))
					{
						if (p_VolumeAtPrice->BidVolume == 0 && DivideByZeroActionIndex == 0)
							AskVolumeBidVolumeRatioPercent = (p_NextVolumeAtPrice->AskVolume / 1) * 100;
						else if (p_VolumeAtPrice->BidVolume == 0 && DivideByZeroActionIndex == 1)
							AskVolumeBidVolumeRatioPercent = 1000;
						else
							AskVolumeBidVolumeRatioPercent = sc.Round(((float)p_NextVolumeAtPrice->AskVolume / p_VolumeAtPrice->BidVolume) * 100);

						Price = p_NextVolumeAtPrice->PriceInTicks * sc.TickSize;
						PriceForExtensionLine = p_NextVolumeAtPrice->PriceInTicks * sc.TickSize;
					}
					else if (p_VolumeAtPrice->BidVolume > p_NextVolumeAtPrice->AskVolume && (p_NextVolumeAtPrice->AskVolume > 0 || AllowZeroValueComparesSetting))
					{
						if (p_NextVolumeAtPrice->AskVolume == 0 && DivideByZeroActionIndex == 0)
							AskVolumeBidVolumeRatioPercent = (p_VolumeAtPrice->BidVolume / 1) * -100;
						else if (p_NextVolumeAtPrice->AskVolume == 0 && DivideByZeroActionIndex == 1)
							AskVolumeBidVolumeRatioPercent = -1000;
						else
							AskVolumeBidVolumeRatioPercent = sc.Round(((float)p_VolumeAtPrice->BidVolume / p_NextVolumeAtPrice->AskVolume) * -100);
					}
				}

				int PercentThresholdSigned = Input_PercentageThreshold.GetInt();

				if (AskVolumeBidVolumeRatioPercent > 0 && PercentThresholdSigned > 0 && AskVolumeBidVolumeRatioPercent >= PercentThresholdSigned)
					ConditionMet = true;
				else if (AskVolumeBidVolumeRatioPercent < 0 && PercentThresholdSigned < 0 && AskVolumeBidVolumeRatioPercent <= PercentThresholdSigned)
					ConditionMet = true;
			}
			else if (ComparisonMethodIndex == 8)//Bid Volume and Ask Volume Separately
			{
				if (
					((VolumeThresholdValue > 0
						&& p_VolumeAtPrice->BidVolume >= VolumeThresholdValue)
						|| (VolumeThresholdValue == 0 && p_VolumeAtPrice->BidVolume == 0))
					&&
					((AdditionalVolumeThresholdValue > 0
						&& p_VolumeAtPrice->AskVolume >= AdditionalVolumeThresholdValue)
						|| (AdditionalVolumeThresholdValue == 0 && p_VolumeAtPrice->AskVolume == 0))
					)
					ConditionMet = true;
			}


			if (ConditionMet)
			{
				CountOfAlerts[BarIndex]++;
				
				// Adjacent Alerts Highlight
				if (HighlightAdjacentAlertsGroupSizeValue > 0 && AdjacentAlertsGroupNumber < 5)
				{
					if (AdjacentAlertsHaveBottomHighlightAlertPrice && PriceIndex != AdjacentAlertsHighlightBottomPriceIndex + AdjacentAlertsCountInGroup)
					{
						AdjacentAlertsHaveBottomHighlightAlertPrice = false;
						AdjacentAlertsCountInGroup = 0;
						if (!AdjacentAlertsGotGroup)
						{
							AdjacentAlertsGroupNumber--;
						}
						AdjacentAlertsGotGroup = false;
					}

					if (AdjacentAlertsHaveBottomHighlightAlertPrice == false)
					{
						AdjacentAlertsGroupNumber++;
						sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + AdjacentAlertsGroupNumber * 2].Data[BarIndex] = Price;
						AdjacentAlertsHaveBottomHighlightAlertPrice = true;
						AdjacentAlertsCountInGroup++;
						AdjacentAlertsHighlightBottomPriceIndex = PriceIndex;
						if (AdjacentAlertsCountInGroup >= HighlightAdjacentAlertsGroupSizeValue)
						{
							sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + AdjacentAlertsGroupNumber * 2 + 1].Data[BarIndex] = Price;
							AdjacentAlertsGotGroup = true;
						}

					}
					else if (AdjacentAlertsHaveBottomHighlightAlertPrice && PriceIndex == AdjacentAlertsHighlightBottomPriceIndex + AdjacentAlertsCountInGroup)
					{
						AdjacentAlertsCountInGroup++;
						if (AdjacentAlertsCountInGroup >= HighlightAdjacentAlertsGroupSizeValue)
						{
							sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + AdjacentAlertsGroupNumber * 2 + 1].Data[BarIndex] = Price;
							AdjacentAlertsGotGroup = true;
						}
					}
					else if (AdjacentAlertsHaveBottomHighlightAlertPrice && !AdjacentAlertsGotGroup)
					{
						AdjacentAlertsCountInGroup = 0;
						AdjacentAlertsHaveBottomHighlightAlertPrice = false;
						sc.Subgraph[AdjacentAlertsHighlightSubgraphStartingNumber + AdjacentAlertsGroupNumber * 2].Data[BarIndex] = 0.0;
						AdjacentAlertsGroupNumber--;
					}
				}

				sc.Subgraph[AvailableSubgraphIndex][BarIndex] = Price;

				if(Input_DrawExtensionLines.GetYesNo()
					&& sc.GetBarHasClosedStatus(BarIndex) == BHCS_BAR_HAS_CLOSED)
				{
					n_ACSIL::s_LineUntilFutureIntersection LineUntilFutureIntersection;
					LineUntilFutureIntersection.StartBarIndex = BarIndex;
					LineUntilFutureIntersection.LineIDForBar = AvailableSubgraphIndex;
					if (Input_DrawExtensionLinesWithTransparentRange.GetYesNo())
					{
						LineUntilFutureIntersection.LineValue = PriceForExtensionLine + sc.TickSize * 0.5f;
						LineUntilFutureIntersection.UseLineValue2 = true;
						LineUntilFutureIntersection.LineValue2ForRange = PriceForExtensionLine - sc.TickSize * 0.5f;
					}
					else
						LineUntilFutureIntersection.LineValue = PriceForExtensionLine;

					LineUntilFutureIntersection.LineColor = ExtensionLinePropertiesSubgraph.PrimaryColor;
					LineUntilFutureIntersection.LineWidth = ExtensionLinePropertiesSubgraph.LineWidth;
					LineUntilFutureIntersection.LineStyle = ExtensionLinePropertiesSubgraph.LineStyle;


					if (Input_DrawExtensionLinesUntilEndOfChart.GetYesNo())
						LineUntilFutureIntersection.AlwaysExtendToEndOfChart = true;

					
					sc.AddLineUntilFutureIntersectionEx(LineUntilFutureIntersection);
				}

				if (AvailableSubgraphIndex < SC_SUBGRAPHS_AVAILABLE - 13)
					++AvailableSubgraphIndex;

				if (EnableAlerts && Input_AlertNumber.GetAlertSoundNumber() > 0  && BarIndex == sc.ArraySize - 1)
				{

					sc.SetAlert(Input_AlertNumber.GetAlertSoundNumber() - 1 , "Volume threshold triggered");
				}


			}
		}
	}
}

/*============================================================================
	Sample code for sc.ContinuousFuturesContractOption.
----------------------------------------------------------------------------*/
SCSFExport scsf_ChangingContinuousFuturesContractChartOption(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Changing Continuous Futures Contract Chart Option";
		
		sc.StudyDescription = "This is example code for changing the Continuous Futures Contract option for the chart. The symbol must support this for this to have an effect.";
		
		sc.AutoLoop = 1;
		
		
		
		return;
	}
	
	
	// Do data processing

	// Make chart a continuous futures contract chart. This must be supported by the symbol for it to have an effect. This option will not go into effect until returning from the study function, at that time the chart will be reloaded.
	if (sc.ContinuousFuturesContractOption != CFCO_DATE_RULE_ROLLOVER)
	{
		sc.ContinuousFuturesContractOption = CFCO_DATE_RULE_ROLLOVER;
	}
}

/*==========================================================================*/
SCSFExport scsf_RelativeVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RelativeVolume = sc.Subgraph[0];
	SCSubgraphRef CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio = sc.Subgraph[1];
	SCSubgraphRef HundredLine = sc.Subgraph[2];
	SCSubgraphRef AverageVolumeSubgraph = sc.Subgraph[3];
	SCSubgraphRef AverageCumulativeVolumeSubgraph = sc.Subgraph[4];
	SCSubgraphRef CurrentDayCumulativeVolume = sc.Subgraph[5];
	SCSubgraphRef Subgraph_VolumeAverageVolumeDiff = sc.Subgraph[6];
	SCSubgraphRef Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff = sc.Subgraph[7];

	SCFloatArrayRef TimespanForDayArray = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef AverageVolume = CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.Arrays[0];
	SCFloatArrayRef AverageCumulativeVolumeForPriorDays = CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.Arrays[1];
	SCFloatArrayRef MedianVolumeTemp = CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.Arrays[2];
	SCFloatArrayRef MedianCumulativeVolumeTemp = CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.Arrays[3];
	SCFloatArrayRef MedianTemp = CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.Arrays[4];

	SCInputRef Period = sc.Input[0];
	SCInputRef AccumulateType = sc.Input[1];
	SCInputRef DaySessionOnly = sc.Input[2];
	SCInputRef VolumeAvergeType = sc.Input[3];
	SCInputRef VolumeSubgraphToUse = sc.Input[4];
	SCInputRef TimeComparisonTolerance = sc.Input[5];
	SCInputRef MinimumRequiredDataForDayAsPercentage = sc.Input[6];
	SCInputRef HighPercentThresh = sc.Input[8];
	SCInputRef HighThresholdColor = sc.Input[9];
	SCInputRef LowPercentThresh = sc.Input[10];
	SCInputRef LowThresholdColor = sc.Input[11];
	SCInputRef Version = sc.Input[13];
// 	SCInputRef Input_DisplayAsVolumeAverageVolumeDiff = sc.Input[14];
// 	SCInputRef Input_DisplayCumulativeVolumeAverageCumulativeVolumeDiff = sc.Input[15];

	const static int CURRENT_VERSION = 4;

	if (sc.SetDefaults)
	{
		sc.GraphName = "Relative Volume";

		sc.GraphRegion = 1;
		sc.ValueFormat = 1;
		sc.DrawZeros = 0;
		sc.AutoLoop = 0;//manual looping
		sc.MaintainAdditionalChartDataArrays = 1;// needed to calculate the time span of the bar accurately.

		RelativeVolume.Name = "Relative Volume";
		RelativeVolume.DrawStyle = DRAWSTYLE_BAR;
		RelativeVolume.PrimaryColor = RGB(0, 128, 192);
		RelativeVolume.LineWidth = 3;

		CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.Name = "Cumulative Volume Ratio";
		CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.DrawStyle = DRAWSTYLE_LINE_SKIP_ZEROS;
		CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.PrimaryColor = RGB(255, 128, 0);
		CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.LineWidth = 2;

		AverageVolumeSubgraph.Name = "Average Volume";
		AverageVolumeSubgraph.DrawStyle = DRAWSTYLE_IGNORE;
		AverageVolumeSubgraph.PrimaryColor = RGB(0, 128, 255);
		AverageVolumeSubgraph.LineWidth = 1;

		AverageCumulativeVolumeSubgraph.Name = "Average Cumulative Volume";
		AverageCumulativeVolumeSubgraph.DrawStyle = DRAWSTYLE_IGNORE;
		AverageCumulativeVolumeSubgraph.PrimaryColor = COLOR_GRAY;
		AverageCumulativeVolumeSubgraph.LineWidth = 1;

		HundredLine.Name = "100%";
		HundredLine.DrawStyle = DRAWSTYLE_LINE;
		HundredLine.LineStyle = LINESTYLE_DOT;
		HundredLine.PrimaryColor = COLOR_GRAY;

		CurrentDayCumulativeVolume.Name = "Cumulative Volume";
		CurrentDayCumulativeVolume.DrawStyle = DRAWSTYLE_IGNORE;
		CurrentDayCumulativeVolume.PrimaryColor = COLOR_GRAY;

		Subgraph_VolumeAverageVolumeDiff.Name = "Volume Average Volume Difference";
		Subgraph_VolumeAverageVolumeDiff.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_VolumeAverageVolumeDiff.LineWidth = 2;
		Subgraph_VolumeAverageVolumeDiff.PrimaryColor = RGB(0, 200, 0);

		Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff.Name = "Cum Volume Average Cum Volume Difference";
		Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff.LineWidth = 2;
		Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff.PrimaryColor = RGB(0, 0, 200);

		Period.Name = "Period - Days To Include";
		Period.SetInt(5);
		Period.SetIntLimits(1, INT_MAX);

		AccumulateType.Name = "Accumulation Type";
		AccumulateType.SetCustomInputStrings("All Days;Day Of Week");
		AccumulateType.SetCustomInputIndex(0);

		DaySessionOnly.Name = "Day Session Only";
		DaySessionOnly.SetYesNo(1);

		VolumeAvergeType.Name = "Volume Average Type";
		VolumeAvergeType.SetCustomInputStrings("Average Volume;Median Volume");
		VolumeAvergeType.SetCustomInputIndex(0);

		VolumeSubgraphToUse.Name = "Volume Subgraph To Use";
		VolumeSubgraphToUse.SetInputDataIndex(SC_VOLUME);

		TimeComparisonTolerance.Name = "Time Comparison Tolerance in Minutes";
		TimeComparisonTolerance.SetInt(1);
		TimeComparisonTolerance.SetIntLimits(0, 240);

		MinimumRequiredDataForDayAsPercentage.Name = "Minimum Required Data for Day as Percentage";
		MinimumRequiredDataForDayAsPercentage.SetInt(50);
		MinimumRequiredDataForDayAsPercentage.SetIntLimits(0, 100);

		HighPercentThresh.Name = "High Percent Threshold (0-disabled, 120->120%)";
		HighPercentThresh.SetFloat(120);
		HighPercentThresh.SetFloatLimits(0, FLT_MAX);

		HighThresholdColor.Name = "High Threshold Color";
		HighThresholdColor.SetColor(COLOR_GREEN);

		LowPercentThresh.Name = "Low Percent Threshold (0-disabled, 80->80%)";
		LowPercentThresh.SetFloat(80);
		LowPercentThresh.SetFloatLimits(0, FLT_MAX);

		LowThresholdColor.Name = "Low Threshold Color";
		LowThresholdColor.SetColor(COLOR_RED);

// 		Input_DisplayAsVolumeAverageVolumeDiff.Name = "Display As Volume Average Volume Difference";
// 		Input_DisplayAsVolumeAverageVolumeDiff.SetYesNo(false);
// 
// 		Input_DisplayCumulativeVolumeAverageCumulativeVolumeDiff.Name = "Display Cum Volume Average Cum Volume Difference";
// 		Input_DisplayCumulativeVolumeAverageCumulativeVolumeDiff.SetYesNo(false);

		Version.SetInt(CURRENT_VERSION);

		return;
	}

	if (Version.GetInt() < 2)
	{
		VolumeAvergeType.SetCustomInputIndex(0);
		AverageVolumeSubgraph.DrawStyle = DRAWSTYLE_IGNORE;
		AverageCumulativeVolumeSubgraph.DrawStyle = DRAWSTYLE_IGNORE;

		Version.SetInt(CURRENT_VERSION);
	}

	if (Version.GetInt() < 3)
	{
		VolumeSubgraphToUse.SetInputDataIndex(SC_VOLUME);
		Version.SetInt(CURRENT_VERSION);
	}

	if (Version.GetInt() < 4)
	{
		MinimumRequiredDataForDayAsPercentage.SetInt(50);
		Version.SetInt(CURRENT_VERSION);
	}

	int VolumeSubgraphIndex = VolumeSubgraphToUse.GetInputDataIndex();

	// References to persistent variables
	int& LastIndex = sc.GetPersistentInt(1);

	if (sc.LastCallToFunction)
	{
		return;
	}

	if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
	{
		sc.ClearAllPersistentData();

		LastIndex = 0;

		CurrentDayCumulativeVolume[0] = sc.BaseData[VolumeSubgraphIndex][0];

// 		if (Input_DisplayAsVolumeAverageVolumeDiff.GetYesNo() || Input_DisplayCumulativeVolumeAverageCumulativeVolumeDiff.GetYesNo())
// 		{
// 			if (Input_DisplayAsVolumeAverageVolumeDiff.GetYesNo() && (Subgraph_VolumeAverageVolumeDiff.DrawStyle == DRAWSTYLE_IGNORE
// 				|| Subgraph_VolumeAverageVolumeDiff.DrawStyle == DRAWSTYLE_HIDDEN))
// 				Subgraph_VolumeAverageVolumeDiff.DrawStyle = DRAWSTYLE_LINE;
// 
// 			if (Input_DisplayCumulativeVolumeAverageCumulativeVolumeDiff.GetYesNo() && (Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff.DrawStyle == DRAWSTYLE_IGNORE
// 				|| Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff.DrawStyle == DRAWSTYLE_HIDDEN))
// 				Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff.DrawStyle = DRAWSTYLE_LINE;
// 
// 			RelativeVolume.DrawStyle = DRAWSTYLE_IGNORE;
// 			CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.DrawStyle = DRAWSTYLE_IGNORE;
// 			HundredLine.DrawStyle = DRAWSTYLE_IGNORE;
// 		}
// 		else
// 		{
// 			if (!Input_DisplayAsVolumeAverageVolumeDiff.GetYesNo())
// 				Subgraph_VolumeAverageVolumeDiff.DrawStyle = DRAWSTYLE_IGNORE;
// 
// 			if (!Input_DisplayCumulativeVolumeAverageCumulativeVolumeDiff.GetYesNo())
// 				Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff.DrawStyle = DRAWSTYLE_IGNORE;
// 
// 			RelativeVolume.DrawStyle = DRAWSTYLE_BAR;
// 			CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio.DrawStyle = DRAWSTYLE_LINE_SKIP_ZEROS;
// 			HundredLine.DrawStyle = DRAWSTYLE_LINE;
//		}
	}

	enum TradingDateSufficientDataEnum
	{
		SufficientData_Unset = 0
		, SufficientData_True = 1
		, SufficientData_False = 2
	};


	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		SCDateTime BarDateTime = sc.BaseDateTimeIn[BarIndex];

		HundredLine[BarIndex] = 100.0f;

		bool DrawValuesAtIndex = !DaySessionOnly.GetYesNo() || sc.IsDateTimeInDaySession(BarDateTime);

		const int CurrentBarTradeDate = sc.GetTradingDayDate(BarDateTime);
		const int PriorBarTradeDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[BarIndex - 1]);
		const int NextBarTradeDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[BarIndex + 1]);

		bool IsNewDay = BarIndex == 0 || CurrentBarTradeDate != PriorBarTradeDate;

		TimespanForDayArray[BarIndex] = (float)(sc.TimeSpanOfBar(BarIndex));

		if (BarIndex >= 1 && !IsNewDay)
			TimespanForDayArray[BarIndex] += TimespanForDayArray[BarIndex - 1];

		if (CurrentBarTradeDate != NextBarTradeDate)
		{
			int&  SufficientData = sc.GetPersistentInt(CurrentBarTradeDate);

			if (SufficientData == SufficientData_Unset)
			{
				if (TimespanForDayArray[BarIndex] >= (1 * DAYS * (MinimumRequiredDataForDayAsPercentage.GetInt() * 0.01)))
				{
					SufficientData = SufficientData_True;
				}
				else
				{
					SufficientData = SufficientData_False;
#ifdef _DEBUG
					SCString MessageText;
					SCString BarDateString = sc.DateTimeToString(CurrentBarTradeDate, FLAG_DT_COMPLETE_DATE);
					MessageText.Format("Date %s has insufficient data.", BarDateString.GetChars());
					sc.AddMessageToLog(MessageText, 0);
#endif
				}
			}
		}

		// track cumulative volume
		if (!DrawValuesAtIndex)
			CurrentDayCumulativeVolume[BarIndex] = 0;
		else if (CurrentBarTradeDate != PriorBarTradeDate)
			CurrentDayCumulativeVolume[BarIndex] = sc.BaseData[VolumeSubgraphIndex][BarIndex];
		else
			CurrentDayCumulativeVolume[BarIndex] = CurrentDayCumulativeVolume[BarIndex - 1] + sc.BaseData[VolumeSubgraphIndex][BarIndex];

		bool CalculateMedian = VolumeAvergeType.GetIndex() == 1;

		if (BarIndex != LastIndex)
		{
			float SumOfBarVol = 0;
			float SumOfAccumVol = 0;
			int   NumValues = 0;

			SCDateTime PriorDateTime = BarDateTime;
			const int NumDaysToCalc = Period.GetInt();
			int DaysBack = 0;

			for (; DaysBack < NumDaysToCalc; ++DaysBack)
			{
				int PriorIndex = -1;
				int SkipCount = 0;
				while (SkipCount <= 3)
				{
					if (AccumulateType.GetIndex() == 0)//All days
					{
						PriorDateTime -= 1 * DAYS;
					}
					else//Day of week
						PriorDateTime -= 7 * DAYS;

					SCDateTime PriorTradingDayDate = sc.GetTradingDayDate(PriorDateTime);

					int&  SufficientData = sc.GetPersistentInt(PriorTradingDayDate.GetDate());

					if (SufficientData == SufficientData_False || SufficientData == SufficientData_Unset)
					{
						SkipCount++;
						continue;
					}

					break;
				}


				PriorIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, PriorDateTime);

				//If the found Date-Time is within the time comparison tolerance of what is expected, then use it
				if (SCDateTime(fabs(sc.BaseDateTimeIn[PriorIndex] - PriorDateTime)) > TimeComparisonTolerance.GetInt()*MINUTES)
				{
					continue;
				}


				SumOfBarVol += sc.BaseData[VolumeSubgraphIndex][PriorIndex];
				SumOfAccumVol += CurrentDayCumulativeVolume[PriorIndex];

				if (CalculateMedian && BarIndex >= NumValues)
				{
					MedianVolumeTemp[BarIndex - NumValues] = sc.BaseData[VolumeSubgraphIndex][PriorIndex];
					MedianCumulativeVolumeTemp[BarIndex - NumValues] = CurrentDayCumulativeVolume[PriorIndex];
				}

				NumValues++;

				if (PriorIndex == 0)
					break;
			}

			if (NumValues > 0 && DaysBack >= NumDaysToCalc - 1)
			{
				if (CalculateMedian)
				{
					MovingMedian_S(MedianVolumeTemp, AverageVolume, MedianTemp, BarIndex, NumValues);
					MovingMedian_S(MedianCumulativeVolumeTemp, AverageCumulativeVolumeForPriorDays, MedianTemp, BarIndex, NumValues);
				}
				else
				{
					AverageVolume[BarIndex] = SumOfBarVol / NumValues;
					AverageCumulativeVolumeForPriorDays[BarIndex] = SumOfAccumVol / NumValues;
				}
			}

			LastIndex = BarIndex;
		}

		if (DrawValuesAtIndex)
		{
			// express relative and average accumulated volume as percentages
			if (AverageVolume[BarIndex] != 0)
				RelativeVolume[BarIndex] = (sc.BaseData[VolumeSubgraphIndex][BarIndex] / AverageVolume[BarIndex]) * 100.0f;

			if (AverageCumulativeVolumeForPriorDays[BarIndex] != 0)
			{
				CurrentDayCumulativeVolumeToAverageCumulativeVolumePriorDaysRatio[BarIndex] = (CurrentDayCumulativeVolume[BarIndex] / AverageCumulativeVolumeForPriorDays[BarIndex]) * 100.0f;
			}

			AverageVolumeSubgraph[BarIndex] = AverageVolume[BarIndex];
			AverageCumulativeVolumeSubgraph[BarIndex] = AverageCumulativeVolumeForPriorDays[BarIndex];
			Subgraph_VolumeAverageVolumeDiff[BarIndex] = sc.Volume[BarIndex] - AverageVolumeSubgraph[BarIndex];
			Subgraph_CumulativeVolumeAverageCumulativeVolumeDiff[BarIndex] = CurrentDayCumulativeVolume[BarIndex] - AverageCumulativeVolumeSubgraph[BarIndex];

			// finally color bars
			if (RelativeVolume[BarIndex] <= LowPercentThresh.GetFloat() && LowPercentThresh.GetFloat() != 0)
				RelativeVolume.DataColor[BarIndex] = LowThresholdColor.GetColor();
			else if (RelativeVolume[BarIndex] >= HighPercentThresh.GetFloat() && HighPercentThresh.GetFloat() != 0)
				RelativeVolume.DataColor[BarIndex] = HighThresholdColor.GetColor();
			else
				RelativeVolume.DataColor[BarIndex] = RelativeVolume.PrimaryColor;
		}

	}
}
/*==========================================================================*/
SCSFExport scsf_ForecastOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ForecastOscillator = sc.Subgraph[0];
	SCSubgraphRef ZeroLine           = sc.Subgraph[1];

	SCFloatArrayRef Avg = ForecastOscillator.Arrays[0];

	SCInputRef InputData  = sc.Input[0];
	SCInputRef Period     = sc.Input[1];


	if (sc.SetDefaults)
	{
		sc.GraphName = "Chande Forecast Oscillator";

		sc.ValueFormat = 2;
		sc.DrawZeros = 1;
		sc.AutoLoop = 1;
		
		
		// subgraphs
		ForecastOscillator.Name = "Chande Forecast Oscillator";
		ForecastOscillator.DrawStyle = DRAWSTYLE_LINE;
		ForecastOscillator.PrimaryColor = COLOR_GREEN;
		ForecastOscillator.LineWidth = 2;

		ZeroLine.Name = "Zero";
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.LineStyle = LINESTYLE_DOT;
		ZeroLine.PrimaryColor = COLOR_DARKGRAY;
		ZeroLine.DisplayNameValueInWindowsFlags = 0;

		// inputs
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Period.Name = "Period";
		Period.SetInt(14);
		Period.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Period.GetInt() - 1;

	SCFloatArrayRef BaseDataArray = sc.BaseDataIn[InputData.GetInputDataIndex()];

	sc.LinearRegressionIndicator(BaseDataArray, Avg, Period.GetInt());

	float InputValue = BaseDataArray[sc.Index];

	if (InputValue != 0)
		ForecastOscillator[sc.Index] = 100.0f * ((InputValue - Avg[sc.Index]) / InputValue);

	ZeroLine[sc.Index] = 0.0f;
}


/*==========================================================================*/
SCSFExport scsf_DetectingNewBarsAdded(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Detecting New Bars Added";
		sc.AutoLoop = 1;
		

		return;
	}

	// Do data processing
	int& PriorArraySize = sc.GetPersistentInt(1);

	//This is not a full recalculation of the study
	if (!sc.IsFullRecalculation)
	{
		// If there are new bars added
		if (PriorArraySize < sc.ArraySize)
		{
			// put processing here that is required for when new bars are added to the chart
		}
	}
	
	PriorArraySize = sc.ArraySize;
}

/*==========================================================================*/
SCSFExport scsf_DeltaBelowBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DeltaValue = sc.Subgraph[0];

	SCFloatArrayRef TextPosition = DeltaValue.Arrays[0];

	SCInputRef  OffsetInTicks = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bar Delta Below Bar";

		sc.GraphRegion = 0;
		sc.ValueFormat = 0;
		sc.DrawZeros = 1;
		sc.AutoLoop = 1;
		
		sc.HideDLLAndFunctionNames = 1;

		// subgraphs
		DeltaValue.Name = "Bar Delta";
		DeltaValue.DrawStyle = DRAWSTYLE_CUSTOM_VALUE_AT_Y;
		DeltaValue.PrimaryColor = COLOR_GREEN;
		DeltaValue.SecondaryColor = COLOR_RED;
		DeltaValue.SecondaryColorUsed = 1;
		DeltaValue.AutoColoring = AUTOCOLOR_POSNEG;

		OffsetInTicks.Name = "Offset In Ticks";
		OffsetInTicks.SetInt(1);

		return;
	}

	DeltaValue[sc.Index] = sc.AskVolume[sc.Index] - sc.BidVolume[sc.Index];

	TextPosition[sc.Index] = sc.Low[sc.Index] - (OffsetInTicks.GetInt() * sc.TickSize);  // top of text
}

/*==========================================================================*/
SCSFExport scsf_AskVolumeBidVolumeDifferenceText(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_Difference = sc.Subgraph[0];

	SCFloatArrayRef TextPosition = Subgraph_Difference.Arrays[0];

	SCInputRef Input_DisplayAboveOrBelow = sc.Input[0];
	SCInputRef Input_OffsetInTicks = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ask Volume Bid Volume Difference Text";

		sc.GraphRegion = 0;
		sc.ValueFormat = 0;
		sc.DrawZeros = 1;
		sc.AutoLoop = 1;		

		Subgraph_Difference.Name = "Difference";
		Subgraph_Difference.DrawStyle = DRAWSTYLE_CUSTOM_VALUE_AT_Y;
		Subgraph_Difference.PrimaryColor = RGB(0,128,0);
		Subgraph_Difference.SecondaryColor = RGB(128,0,0);
		Subgraph_Difference.SecondaryColorUsed = 1;
		Subgraph_Difference.AutoColoring = AUTOCOLOR_POSNEG;
		Subgraph_Difference.LineWidth = 8;

		Input_DisplayAboveOrBelow.Name = "Display Location";
		Input_DisplayAboveOrBelow.SetCustomInputStrings("Top of Bar;Bottom of Bar");
		Input_DisplayAboveOrBelow.SetCustomInputIndex(0);

		Input_OffsetInTicks.Name = "Offset In Ticks";
		Input_OffsetInTicks.SetInt(1);

		return;
	}

	Subgraph_Difference[sc.Index] = sc.AskVolume[sc.Index] - sc.BidVolume[sc.Index];

	if (Input_DisplayAboveOrBelow.GetIndex() == 0)
		TextPosition[sc.Index] = sc.High[sc.Index]  + (Input_OffsetInTicks.GetInt() * sc.TickSize);
	else		
		TextPosition[sc.Index] = sc.Low[sc.Index] - (Input_OffsetInTicks.GetInt() * sc.TickSize); 
}

/*==========================================================================*/
SCSFExport scsf_LoadDaySessionOnlyAtOpen(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Load Day Session Only at Open";

		sc.StudyDescription = "If the evening session option is enabled for a chart, then when the Start Time is crossed over using the actual time, then the evening session is disabled and the chart is reloaded.";

		sc.AutoLoop = 0;

		

		return;
	}


	// Do data processing
	SCDateTime& r_PreviousCurrentTime = sc.GetPersistentSCDateTime(1);

	if (sc.IsFullRecalculation)
		r_PreviousCurrentTime = 0.0;

	SCDateTime CurrentDateTime;
	
	if (sc.IsReplayRunning())
		CurrentDateTime = sc.CurrentDateTimeForReplay;
	else
		CurrentDateTime = sc.CurrentSystemDateTime;

	// This code does not consider if the Start Time is 00:00:00. It is assumed to be greater than that.
	if (r_PreviousCurrentTime != 0.0
		&& sc.UseSecondStartEndTimes
		&& CurrentDateTime.GetTimeInSeconds() >= sc.StartTime1
		&& r_PreviousCurrentTime.GetTimeInSeconds() < sc.StartTime1)
	{
		sc.UseSecondStartEndTimes = 0;

	}

	// This code does not consider if the Start Time is 00:00:00. It is assumed to be greater than that.
	if (!r_PreviousCurrentTime.IsUnset()
		&& !sc.UseSecondStartEndTimes
		&& CurrentDateTime.GetTimeInSeconds() > sc.EndTime1
		&& r_PreviousCurrentTime.GetTimeInSeconds() <= sc.EndTime1)
	{
		sc.UseSecondStartEndTimes = 1;
	}

	r_PreviousCurrentTime = CurrentDateTime;
}
/*==========================================================================*/
SCSFExport scsf_DivergenceDetector(SCStudyInterfaceRef sc)
{
	SCSubgraphRef  MainGraphAngleSubgraph  = sc.Subgraph[0];
	SCFloatArrayRef  BaseDataInTicks  = sc.Subgraph[0].Arrays[0];
	SCSubgraphRef  StudyAngleSubgraph  = sc.Subgraph[1];
	SCFloatArrayRef  StudyDataInPoints  = sc.Subgraph[1].Arrays[0];
	SCSubgraphRef  AngleDifferenceSubgraph  = sc.Subgraph[2];
	SCSubgraphRef PositiveDivergenceIndicatorSubgraph = sc.Subgraph[3];
	SCSubgraphRef NegativeDivergenceIndicatorSubgraph = sc.Subgraph[4];


	SCInputRef InputData    = sc.Input[0];
	SCInputRef StudySubgraphReference    = sc.Input[1];
	SCInputRef Length   = sc.Input[2];
	SCInputRef DivergenceThreshold   = sc.Input[3];
	SCInputRef OppositeSlopeDivergenceThreshold = sc.Input[4];
	SCInputRef ValuePerPointForStudyReference = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Divergence Detector";

		sc.ValueFormat = 2;
		sc.AutoLoop = 1;
		

		MainGraphAngleSubgraph.Name = "Main Graph Angle";
		MainGraphAngleSubgraph.DrawStyle = DRAWSTYLE_IGNORE;
		MainGraphAngleSubgraph.DrawZeros = false;

		StudyAngleSubgraph.Name = "Study Angle";
		StudyAngleSubgraph.DrawStyle = DRAWSTYLE_IGNORE;
		StudyAngleSubgraph.DrawZeros = false;

		AngleDifferenceSubgraph.Name = "Angle Difference";
		AngleDifferenceSubgraph.DrawStyle = DRAWSTYLE_IGNORE;
		AngleDifferenceSubgraph.PrimaryColor = RGB(0,192,0);
		AngleDifferenceSubgraph.DrawZeros = false;

		PositiveDivergenceIndicatorSubgraph.Name = "Positive Divergence";
		PositiveDivergenceIndicatorSubgraph.DrawStyle = DRAWSTYLE_ARROW_UP;
		PositiveDivergenceIndicatorSubgraph.LineWidth = 3;
		PositiveDivergenceIndicatorSubgraph.DrawZeros = false;

		NegativeDivergenceIndicatorSubgraph.Name = "Negative Divergence";
		NegativeDivergenceIndicatorSubgraph.DrawStyle = DRAWSTYLE_ARROW_DOWN;
		NegativeDivergenceIndicatorSubgraph.LineWidth = 3;
		NegativeDivergenceIndicatorSubgraph.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		StudySubgraphReference.Name = "Study Subgraph Reference";
		StudySubgraphReference.SetStudySubgraphValues(0, 0);

		Length.Name = "Divergence Length";
		Length.SetInt(10);

		//If the divergence of the slope of the main price graph and the slope of the study subgraph being referenced exceeds this threshold, then it is considered there is a divergence.
		DivergenceThreshold.Name = "Divergence Threshold in Degrees";
		DivergenceThreshold.SetFloat(45.0f);
		DivergenceThreshold.SetFloatLimits(-90,90);

		OppositeSlopeDivergenceThreshold.Name = "Opposite Slope Divergence Threshold in Degrees";
		OppositeSlopeDivergenceThreshold.SetFloat(10.0f);
		OppositeSlopeDivergenceThreshold.SetFloatLimits(-90,90);

		ValuePerPointForStudyReference.Name = "Value Per Point for Study Reference";
		ValuePerPointForStudyReference.SetFloat(1.0f);


		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		sc.GraphRegion = 0;

		return;
	}

	SCFloatArray StudySubgraphArray;

	sc.GetStudyArrayUsingID(StudySubgraphReference.GetStudyID(), StudySubgraphReference.GetSubgraphIndex(), StudySubgraphArray);

	if(StudySubgraphArray.GetArraySize() < sc.ArraySize)
		return;

	double MainGraphSlope = 0;
	double MainGraphY_Intercept = 0;
	BaseDataInTicks[sc.Index] = sc.BaseData[InputData.GetInputDataIndex()][sc.Index] / sc.ValueIncrementPerBar;

	sc.CalculateRegressionStatistics(BaseDataInTicks,MainGraphSlope, MainGraphY_Intercept ,Length.GetInt());

	double StudySlope = 0;
	double StudyY_Intercept = 0;
	StudyDataInPoints[sc.Index] = StudySubgraphArray[sc.Index] / ValuePerPointForStudyReference.GetFloat();
	sc.CalculateRegressionStatistics(StudyDataInPoints, StudySlope, StudyY_Intercept ,Length.GetInt());

	
	float  ThresholdDegrees = DivergenceThreshold.GetFloat();
	float  OppositeSlopeDivergenceThresholdDegrees = OppositeSlopeDivergenceThreshold.GetFloat();

	double MainGraphAngle = sc.SlopeToAngleInDegrees(MainGraphSlope);

	double StudyAngle = sc.SlopeToAngleInDegrees(StudySlope);

	double AngleDifference = (MainGraphAngle-StudyAngle);

	AngleDifferenceSubgraph[sc.Index] = (float)AngleDifference;

	MainGraphAngleSubgraph.Data[sc.Index] = (float)MainGraphAngle;
	StudyAngleSubgraph.Data[sc.Index] = (float)StudyAngle;

	if ((fabs(AngleDifference) >= ThresholdDegrees)
		|| (MainGraphAngle > 0 && StudyAngle < 0 && fabs(AngleDifference) >= OppositeSlopeDivergenceThresholdDegrees)
		|| (MainGraphAngle < 0 && StudyAngle > 0 && fabs(AngleDifference) >= OppositeSlopeDivergenceThresholdDegrees))
	{

		//A positive divergence is when the study angle is greater than the main price graph angle
		if (StudyAngle > MainGraphAngle)
		{
			PositiveDivergenceIndicatorSubgraph[sc.Index] = sc.Low[sc.Index] - sc.TickSize;
			NegativeDivergenceIndicatorSubgraph[sc.Index] = 0;
		}
		else
		{
			PositiveDivergenceIndicatorSubgraph[sc.Index] = 0;
			NegativeDivergenceIndicatorSubgraph[sc.Index] = sc.High[sc.Index] + sc.TickSize;

		}
	}
	else
	{
		PositiveDivergenceIndicatorSubgraph[sc.Index] = 0;
		NegativeDivergenceIndicatorSubgraph[sc.Index] = 0;
	}
}


/*==========================================================================*/
SCSFExport scsf_CandlestickBodyGapExtend(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LineExtension = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Candlestick Body Gap Extend";

		sc.AutoLoop = 1;

		

		LineExtension.Name = "Line Extension";
		LineExtension.DrawStyle = DRAWSTYLE_COLOR_BAR;
		LineExtension.PrimaryColor = RGB(0,255,0);
		LineExtension.DrawZeros = false;

		return;
	}

	if(sc.Index == 0)
		return;

	float PriorBarOpen = sc.BaseData[SC_OPEN][sc.Index-1];
	float PriorBarClose = sc.BaseData[SC_LAST][sc.Index-1];
	float BarOpen = sc.BaseData[SC_OPEN][sc.Index];
	float BarClose = sc.BaseData[SC_LAST][sc.Index];
	float PriorBarMin = min(PriorBarOpen, PriorBarClose);
	float PriorBarMax = max(PriorBarOpen, PriorBarClose);
	float BarMin = min(BarOpen, BarClose);
	float BarMax = max(BarOpen, BarClose);
	


	if(sc.FormattedEvaluate(PriorBarMin, sc.BaseGraphValueFormat, GREATER_OPERATOR, BarMax, sc.BaseGraphValueFormat))
		LineExtension[sc.Index] = 1;
	else if(sc.FormattedEvaluate(PriorBarMax, sc.BaseGraphValueFormat, LESS_OPERATOR, BarMin, sc.BaseGraphValueFormat))
		LineExtension[sc.Index] = 1;
	else
		LineExtension[sc.Index] = 0;


	
}


/*==========================================================================*/
SCSFExport scsf_CalculateTimeSpanAcrossChartBarsExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TimeDiff = sc.Subgraph[0];


	if (sc.SetDefaults)
	{
		sc.GraphName = "CalculateTimeSpanAcrossChartBars Example";

		TimeDiff.Name = "Duration";
		TimeDiff.DrawStyle = DRAWSTYLE_BAR;
		TimeDiff.PrimaryColor = RGB(0, 255, 0);

		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_TIME;
		sc.AutoLoop = 1;
	}

	TimeDiff[sc.Index] = (float)sc.CalculateTimeSpanAcrossChartBars(sc.Index, sc.Index );

}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphAsTextAboveBelowBarV2(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_StudyReference = sc.Subgraph[0];

	SCFloatArrayRef Array_TextPosition = Subgraph_StudyReference.Arrays[0];

	SCInputRef Input_StudySugbraph1 = sc.Input[0];
	SCInputRef Input_DisplayAboveOrBelow = sc.Input[1];
	SCInputRef Input_OffsetInTicks = sc.Input[2];
	SCInputRef Input_DrawZeros = sc.Input[3];
	SCInputRef Input_AdditionalForwardColumns = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Above/Below Bar as Text";

		sc.GraphRegion = 0;
		sc.ValueFormat = 0;
		sc.AutoLoop = 0;
		
		//sc.CalculationPrecedence = LOW_PREC_LEVEL;

		Subgraph_StudyReference.Name = "Reference";
		Subgraph_StudyReference.DrawStyle = DRAWSTYLE_CUSTOM_VALUE_AT_Y;
		Subgraph_StudyReference.PrimaryColor = RGB(252, 196, 120);
		Subgraph_StudyReference.LineWidth = 12;

		Input_StudySugbraph1.Name = "Input Study Subgraph";
		Input_StudySugbraph1.SetStudySubgraphValues(0, 0);

		Input_DisplayAboveOrBelow.Name = "Display Location";
		Input_DisplayAboveOrBelow.SetCustomInputStrings("Top of Bar;Bottom of Bar");
		Input_DisplayAboveOrBelow.SetCustomInputIndex(0);

		Input_OffsetInTicks.Name = "Offset In Ticks";
		Input_OffsetInTicks.SetInt(1);

		Input_DrawZeros.Name = "Draw Zeros";
		Input_DrawZeros.SetYesNo(false);

		Input_AdditionalForwardColumns.Name = "Additional Forward Columns";
		Input_AdditionalForwardColumns.SetInt(0);

		return;
	}

	if (sc.IsFullRecalculation)
		Subgraph_StudyReference.ExtendedArrayElementsToGraph = Input_AdditionalForwardColumns.GetInt();

	Subgraph_StudyReference.DrawZeros = Input_DrawZeros.GetYesNo();

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(Input_StudySugbraph1.GetStudyID(), Input_StudySugbraph1.GetSubgraphIndex(), Study1Array);


	int EndIndex  = sc.ArraySize + Input_AdditionalForwardColumns.GetInt();

	for (int BarIndex = sc.UpdateStartIndex; BarIndex < EndIndex; ++BarIndex)
	{	
		int BarValueIndex = min(BarIndex, sc.ArraySize - 1);

		float ValueFromReferencedSubgraph = Study1Array[BarIndex];
		
		if(ValueFromReferencedSubgraph != FLT_MAX && ValueFromReferencedSubgraph != -FLT_MAX)
		{
			Subgraph_StudyReference[BarIndex] = ValueFromReferencedSubgraph;

			

			if (Input_DisplayAboveOrBelow.GetIndex() == 0)
				Array_TextPosition[BarIndex] = sc.High[BarValueIndex] + (Input_OffsetInTicks.GetInt() * sc.TickSize);
			else
				Array_TextPosition[BarIndex] = sc.Low[BarValueIndex] - (Input_OffsetInTicks.GetInt() * sc.TickSize);
		}
		else
		{
			Subgraph_StudyReference[BarIndex] = 0;
			Array_TextPosition[BarIndex] = 0;
		}
	}
}


/*==========================================================================*/

SCSFExport scsf_MovingAverageColored(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Colored";

		sc.Subgraph[0].Name = "Up Color Top";	
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_FILLTOP;
		sc.Subgraph[0].DrawZeros = 1;

		sc.Subgraph[1].Name = "Up Color Bottom";		
		sc.Subgraph[1].DrawStyle = DRAWSTYLE_FILLBOTTOM;
		sc.Subgraph[1].DrawZeros = 1;

		sc.Subgraph[2].Name = "Down Color";		
		sc.Subgraph[2].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[2].DrawZeros = 1;

		sc.Subgraph[3].Name = "Down Color";	
		sc.Subgraph[3].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[3].DrawZeros = 1;

		sc.Input[0].Name = "MA Type";				
		sc.Input[0].SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		sc.Input[1].Name = "MA Input";
		sc.Input[1].SetInputDataIndex(SC_LAST);

		sc.Input[3].Name = "Short MA Length";		
		sc.Input[3].SetInt(13);

		sc.Input[4].Name = "Long MA Length";		
		sc.Input[4].SetInt(21);

		
		sc.GraphRegion = 0;  // First region	
		return;
	}

	int& SLength = sc.Input[3].IntValue;
	int& LLength = sc.Input[4].IntValue;


	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		sc.MovingAverage(sc.BaseDataIn[sc.Input[1].GetInputDataIndex()], sc.Subgraph[5], sc.Input[0].GetMovAvgType(), BarIndex, SLength);

		sc.MovingAverage(sc.BaseDataIn[sc.Input[1].GetInputDataIndex()], sc.Subgraph[6], sc.Input[0].GetMovAvgType(), BarIndex, LLength);

		if (sc.Subgraph[5][BarIndex] > sc.Subgraph[6][BarIndex]) 
		{
			sc.Subgraph[0][BarIndex] = sc.Subgraph[5][BarIndex];
			sc.Subgraph[1][BarIndex] = sc.Subgraph[6][BarIndex];
			if (sc.BaseDataIn[3][BarIndex] > sc.Subgraph[5][BarIndex])
				sc.Subgraph[0].DataColor[BarIndex] = sc.Subgraph[0].PrimaryColor; 
			else 
				sc.Subgraph[0].DataColor[BarIndex] = sc.Subgraph[1].PrimaryColor;
		}
		else 
		{
			sc.Subgraph[0][BarIndex] = sc.Subgraph[6][BarIndex];
			sc.Subgraph[1][BarIndex] = sc.Subgraph[5][BarIndex];

			if (sc.BaseDataIn[3][BarIndex] < sc.Subgraph[5][BarIndex])
				sc.Subgraph[0].DataColor[BarIndex] = sc.Subgraph[3].PrimaryColor;
			else 
				sc.Subgraph[0].DataColor[BarIndex] = sc.Subgraph[2].PrimaryColor;
		}
	}
}

/*==========================================================================*/

SCSFExport scsf_ConnieBrownCompositeIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_RSI1 = sc.Subgraph[0];
	SCSubgraphRef Subgraph_RSI2 = sc.Subgraph[1];
	SCSubgraphRef Subgraph_RSIMO = sc.Subgraph[2];
	SCSubgraphRef Subgraph_RSIMA = sc.Subgraph[3];
	SCSubgraphRef Subgraph_CBI1 = sc.Subgraph[4];
	SCSubgraphRef Subgraph_CBI2 = sc.Subgraph[5];
	SCSubgraphRef Subgraph_CBI3 = sc.Subgraph[6];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_RSILength1 = sc.Input[1];
	SCInputRef Input_RSILength2 = sc.Input[2];
	SCInputRef Input_MomentumLength = sc.Input[3];
	SCInputRef Input_MovAvgLength1 = sc.Input[4];
	SCInputRef Input_MovAvgLength2 = sc.Input[5];
	SCInputRef Input_MovAvgLength3 = sc.Input[6];
	SCInputRef Input_MovAvgType1 = sc.Input[7];
	SCInputRef Input_MovAvgType2 = sc.Input[8];
	SCInputRef Input_MovAvgType3 = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Connie Brown's Composite Index";

		

		sc.AutoLoop = 1;

		Subgraph_RSI1.Name = "RSI 1";
		Subgraph_RSI1.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_RSI2.Name = "RSI 2";
		Subgraph_RSI2.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_RSIMO.Name = "Momentum of RSI";
		Subgraph_RSIMO.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_RSIMA.Name = "Moving Average of RSI";
		Subgraph_RSIMA.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_CBI1.Name = "Connie Brown Index 1";
		Subgraph_CBI1.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_CBI1.PrimaryColor = RGB(255, 0, 0);
		Subgraph_CBI1.DrawZeros = true;

		Subgraph_CBI2.Name = "Connie Brown Index 2";
		Subgraph_CBI2.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_CBI2.PrimaryColor = RGB(0, 255, 0);
		Subgraph_CBI2.DrawZeros = true;

		Subgraph_CBI3.Name = "Connie Brown Index 3";
		Subgraph_CBI3.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_CBI3.PrimaryColor = RGB(0, 0, 255);
		Subgraph_CBI3.DrawZeros = true;

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_RSILength1.Name = "RSI Length 1";
		Input_RSILength1.SetInt(14);
		Input_RSILength1.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_RSILength2.Name = "RSI Length 2";
		Input_RSILength2.SetInt(3);
		Input_RSILength2.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MomentumLength.Name = "Momentum Length";
		Input_MomentumLength.SetInt(9);
		Input_MomentumLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovAvgLength1.Name = "Moving Average Length 1";
		Input_MovAvgLength1.SetInt(3);
		Input_MovAvgLength1.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovAvgLength2.Name = "Moving Average Length 2";
		Input_MovAvgLength2.SetInt(13);
		Input_MovAvgLength2.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovAvgLength3.Name = "Moving Average Length 3";
		Input_MovAvgLength3.SetInt(33);
		Input_MovAvgLength3.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovAvgType1.Name = "Moving Average Type 1";
		Input_MovAvgType1.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Input_MovAvgType2.Name = "Moving Average Type 2";
		Input_MovAvgType2.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Input_MovAvgType3.Name = "Moving Average Type 3";
		Input_MovAvgType3.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		return;
	}

	sc.DataStartIndex = max(Input_RSILength1.GetInt() + Input_MomentumLength.GetInt(), Input_RSILength2.GetInt() + Input_MovAvgLength1.GetInt()) + max(Input_MovAvgLength2.GetInt(), Input_MovAvgLength3.GetInt()) - 1;

	sc.RSI(sc.BaseDataIn[Input_InputData.GetInputDataIndex()], Subgraph_RSI1, Input_MovAvgType1.GetMovAvgType(), Input_RSILength1.GetInt());

	sc.RSI(sc.BaseDataIn[Input_InputData.GetInputDataIndex()], Subgraph_RSI2, Input_MovAvgType1.GetMovAvgType(), Input_RSILength2.GetInt());
	sc.MovingAverage(Subgraph_RSI2, Subgraph_RSIMA, Input_MovAvgType1.GetMovAvgType(), Input_MovAvgLength1.GetInt());

	Subgraph_RSIMO[sc.Index] = Subgraph_RSI1[sc.Index] - Subgraph_RSI1[sc.Index - Input_MomentumLength.GetInt()];

	Subgraph_CBI1[sc.Index] = Subgraph_RSIMO[sc.Index] + Subgraph_RSIMA[sc.Index];

	sc.MovingAverage(Subgraph_CBI1, Subgraph_CBI2, Input_MovAvgType2.GetMovAvgType(), Input_MovAvgLength2.GetInt());

	sc.MovingAverage(Subgraph_CBI1, Subgraph_CBI3, Input_MovAvgType3.GetMovAvgType(), Input_MovAvgLength3.GetInt());
}

/*==========================================================================*/

SCSFExport scsf_MovingAverageBlock(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ATR = sc.Subgraph[0];
	SCSubgraphRef WorkBoxHalf1 = sc.Subgraph[1];
	SCSubgraphRef Direction1 = sc.Subgraph[2];
	SCSubgraphRef Top1 = sc.Subgraph[3];
	SCSubgraphRef Mid1 = sc.Subgraph[4];
	SCSubgraphRef Bot1 = sc.Subgraph[5];
	SCSubgraphRef WorkBoxHalf2 = sc.Subgraph[6];
	SCSubgraphRef Direction2 = sc.Subgraph[7];
	SCSubgraphRef Top2 = sc.Subgraph[8];
	SCSubgraphRef Mid2 = sc.Subgraph[9];
	SCSubgraphRef Bot2 = sc.Subgraph[10];

	SCInputRef ATRLength = sc.Input[0];
	SCInputRef ATRMovAvgType = sc.Input[1];
	SCInputRef BoxMultiplier1 = sc.Input[2];
	SCInputRef BoxMultiplier2 = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Block";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		ATR.Name = "Average True Range";
		ATR.DrawStyle = DRAWSTYLE_IGNORE;

		WorkBoxHalf1.Name = "Work Box Half 1";
		WorkBoxHalf1.DrawStyle = DRAWSTYLE_IGNORE;

		Direction1.Name = "Direction 1";
		Direction1.DrawStyle = DRAWSTYLE_IGNORE;

		Top1.Name = "Top 1";
		Top1.DrawStyle = DRAWSTYLE_IGNORE;

		Mid1.Name = "Middle 1";
		Mid1.DrawStyle = DRAWSTYLE_LINE;
		Mid1.PrimaryColor = RGB(0, 255, 255); // cyan
		Mid1.SecondaryColor = RGB(255, 0, 255); // magenta
		Mid1.SecondaryColorUsed = true;
		Mid1.DrawZeros = true;

		Bot1.Name = "Bottom 1";
		Bot1.DrawStyle = DRAWSTYLE_IGNORE;

		WorkBoxHalf2.Name = "Work Box Half 2";
		WorkBoxHalf2.DrawStyle = DRAWSTYLE_IGNORE;

		Direction2.Name = "Direction 2";
		Direction2.DrawStyle = DRAWSTYLE_IGNORE;

		Top2.Name = "Top 2";
		Top2.DrawStyle = DRAWSTYLE_IGNORE;

		Mid2.Name = "Middle 2";
		Mid2.DrawStyle = DRAWSTYLE_LINE;
		Mid2.PrimaryColor = RGB(0, 255, 255); // cyan
		Mid2.SecondaryColor = RGB(255, 0, 255); // magenta
		Mid2.SecondaryColorUsed = true;
		Mid2.DrawZeros = true;

		Bot2.Name = "Bottom 2";
		Bot2.DrawStyle = DRAWSTYLE_IGNORE;

		ATRLength.Name = "Average True Range Length";
		ATRLength.SetInt(10);
		ATRLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		ATRMovAvgType.Name = "ATR Moving Average Type";
		ATRMovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		BoxMultiplier1.Name = "Box Multiplier 1";
		BoxMultiplier1.SetFloat(1.0f);

		BoxMultiplier2.Name = "Box Multiplier 2";
		BoxMultiplier2.SetFloat(2.0f);

		

		return;
	}

	sc.DataStartIndex = ATRLength.GetInt() - 1;

	sc.ATR(sc.BaseDataIn, ATR, ATRLength.GetInt(), ATRMovAvgType.GetMovAvgType());

	WorkBoxHalf1[sc.Index] = 0.5f*BoxMultiplier1.GetFloat()*ATR[sc.Index];

	if (sc.High[sc.Index] > Top1[sc.Index - 1])
	{
		Direction1[sc.Index] = 1;
		Top1[sc.Index] = sc.High[sc.Index];
		Mid1[sc.Index] = Top1[sc.Index] - WorkBoxHalf1[sc.Index];
		Bot1[sc.Index] = Top1[sc.Index] - 2 * WorkBoxHalf1[sc.Index];
	}

	else if (sc.Low[sc.Index] < Bot1[sc.Index - 1])
	{
		Direction1[sc.Index] = -1;
		Bot1[sc.Index] = sc.Low[sc.Index];
		Mid1[sc.Index] = Bot1[sc.Index] + WorkBoxHalf1[sc.Index];
		Top1[sc.Index] = Bot1[sc.Index] + 2 * WorkBoxHalf1[sc.Index];
	}

	else
	{
		Direction1[sc.Index] = Direction1[sc.Index - 1];
		Top1[sc.Index] = Top1[sc.Index - 1];
		Mid1[sc.Index] = Mid1[sc.Index - 1];
		Bot1[sc.Index] = Bot1[sc.Index - 1];
	}

	WorkBoxHalf2[sc.Index] = 0.5f*BoxMultiplier2.GetFloat()*ATR[sc.Index];

	if (sc.High[sc.Index] > Top2[sc.Index - 1])
	{
		Direction2[sc.Index] = 1;
		Top2[sc.Index] = sc.High[sc.Index];
		Mid2[sc.Index] = Top2[sc.Index] - WorkBoxHalf2[sc.Index];
		Bot2[sc.Index] = Top2[sc.Index] - 2 * WorkBoxHalf2[sc.Index];
	}

	else if (sc.Low[sc.Index] < Bot2[sc.Index - 1])
	{
		Direction2[sc.Index] = -1;
		Bot2[sc.Index] = sc.Low[sc.Index];
		Mid2[sc.Index] = Bot2[sc.Index] + WorkBoxHalf2[sc.Index];
		Top2[sc.Index] = Bot2[sc.Index] + 2 * WorkBoxHalf2[sc.Index];
	}

	else
	{
		Direction2[sc.Index] = Direction2[sc.Index - 1];
		Top2[sc.Index] = Top2[sc.Index - 1];
		Mid2[sc.Index] = Mid2[sc.Index - 1];
		Bot2[sc.Index] = Bot2[sc.Index - 1];
	}

	if (Direction1[sc.Index] == 1)
		Mid1.DataColor[sc.Index] = Mid1.PrimaryColor;
	else if (Direction1[sc.Index] == -1)
		Mid1.DataColor[sc.Index] = Mid1.SecondaryColor;

	if (Direction2[sc.Index] == 1)
		Mid2.DataColor[sc.Index] = Mid2.PrimaryColor;
	else if (Direction2[sc.Index] == -1)
		Mid2.DataColor[sc.Index] = Mid2.SecondaryColor;
}

/*==========================================================================*/

SCSFExport scsf_PolarizedFractalEfficiency(SCStudyInterfaceRef sc)
{

	SCSubgraphRef Subgraph_PFE = sc.Subgraph[0];
	SCSubgraphRef Subgraph_SmoothedPFE = sc.Subgraph[1];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_Period = sc.Input[1];
	SCInputRef Input_SmoothingPeriod = sc.Input[2];
	SCInputRef Input_MovingAverageType = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Polarized Fractal Efficiency";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Subgraph_PFE.Name = "Polarized Fractal Efficiency";
		Subgraph_PFE.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_PFE.PrimaryColor = RGB(0, 255, 0);
		Subgraph_PFE.LineWidth = 1;
		Subgraph_PFE.DrawZeros = true;

		Subgraph_SmoothedPFE.Name = "Smoothed Polarized Fractal Efficiency";
		Subgraph_SmoothedPFE.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_SmoothedPFE.PrimaryColor = RGB(0, 0, 255);
		Subgraph_SmoothedPFE.LineWidth = 1;
		Subgraph_SmoothedPFE.DrawZeros = true;

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_Period.Name = "Period";
		Input_Period.SetInt(14);
		Input_Period.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_SmoothingPeriod.Name = "Smoothing Period";
		Input_SmoothingPeriod.SetInt(3);
		Input_SmoothingPeriod.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovingAverageType.Name = "Moving Average Type";
		Input_MovingAverageType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		

		return;
	}

	float DenominatorSum = 0;

	if (sc.Index < Input_Period.GetInt())
		Subgraph_PFE[sc.Index] = 0;
	else
	{
		for (int Index = 0; Index <= Input_Period.GetInt() - 2; Index++)
		{
			float PriceChangeDenominator = sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - Index] - sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - Index - 1];
			float PriceChangeDenominatorSquared = PriceChangeDenominator * PriceChangeDenominator;

			DenominatorSum += sqrt(PriceChangeDenominatorSquared + 1);
		}

		float PriceChangeNumerator = sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - Input_Period.GetInt()];
		float PriceChangeNumeratorSquared = PriceChangeNumerator * PriceChangeNumerator;

		Subgraph_PFE[sc.Index] = 100 * sqrt(PriceChangeNumeratorSquared + Input_Period.GetInt()*Input_Period.GetInt()) / DenominatorSum;

		if (sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index] < sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - 1])
		{
			Subgraph_PFE[sc.Index] = -1 * Subgraph_PFE[sc.Index];
		}
	}

	sc.MovingAverage(Subgraph_PFE, Subgraph_SmoothedPFE, Input_MovingAverageType.GetMovAvgType(), Input_SmoothingPeriod.GetInt());
}

/*==========================================================================*/

SCSFExport scsf_PriceMomentumOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_RateOfChange = sc.Subgraph[0];
	SCSubgraphRef Subgraph_SmoothedRateOfChange = sc.Subgraph[1];
	SCSubgraphRef Subgraph_PriceMomentumOscillatorLine = sc.Subgraph[2];
	SCSubgraphRef Subgraph_PriceMomentumOscillatorSignal = sc.Subgraph[3];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_PriceMomentumOscillatorLineLength1 = sc.Input[1];
	SCInputRef Input_PriceMomentumOscillatorLineLength2 = sc.Input[2];
	SCInputRef Input_PriceMomentumOscillatorSignalLength = sc.Input[3];
	SCInputRef Input_MovingAverageType = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Price Momentum Oscillator";

		

		sc.AutoLoop = 1;

		Subgraph_RateOfChange.Name = "Rate of Change";
		Subgraph_RateOfChange.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_SmoothedRateOfChange.Name = "Smoothed Rate of Change";
		Subgraph_SmoothedRateOfChange.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_PriceMomentumOscillatorLine.Name = "Price Momentum Oscillator Line";
		Subgraph_PriceMomentumOscillatorLine.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_PriceMomentumOscillatorLine.PrimaryColor = RGB(0, 255, 0);
		Subgraph_PriceMomentumOscillatorLine.DrawZeros = true;

		Subgraph_PriceMomentumOscillatorSignal.Name = "Price Momentum Oscillator Signal";
		Subgraph_PriceMomentumOscillatorSignal.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_PriceMomentumOscillatorSignal.PrimaryColor = RGB(0, 0, 255);
		Subgraph_PriceMomentumOscillatorSignal.DrawZeros = true;

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_PriceMomentumOscillatorLineLength1.Name = "PMO Line Length 1";
		Input_PriceMomentumOscillatorLineLength1.SetInt(35);
		Input_PriceMomentumOscillatorLineLength1.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_PriceMomentumOscillatorLineLength2.Name = "PMO Line Length 2";
		Input_PriceMomentumOscillatorLineLength2.SetInt(20);
		Input_PriceMomentumOscillatorLineLength2.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_PriceMomentumOscillatorSignalLength.Name = "PMO Signal Line Length";
		Input_PriceMomentumOscillatorSignalLength.SetInt(10);
		Input_PriceMomentumOscillatorSignalLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovingAverageType.Name = "PMO Signal Line Moving Average Type";
		Input_MovingAverageType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		return;
	}

	sc.DataStartIndex = max(max(Input_PriceMomentumOscillatorLineLength1.GetInt(), Input_PriceMomentumOscillatorLineLength2.GetInt()), Input_PriceMomentumOscillatorSignalLength.GetInt()) - 1;

	if (sc.Index == 0)
		return;

	else if (sc.Index == 1)
	{
		Subgraph_RateOfChange[sc.Index] = 100 * (sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - 1]) / sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - 1];

		Subgraph_SmoothedRateOfChange[sc.Index] = Subgraph_RateOfChange[sc.Index];
		Subgraph_PriceMomentumOscillatorLine[sc.Index] = 10 * Subgraph_SmoothedRateOfChange[sc.Index];
	}
	else
	{
		Subgraph_RateOfChange[sc.Index] = 100 * (sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - 1]) / sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - 1];
		
		Subgraph_SmoothedRateOfChange[sc.Index] = (2.0f / Input_PriceMomentumOscillatorLineLength1.GetInt())*(Subgraph_RateOfChange[sc.Index] - Subgraph_SmoothedRateOfChange[sc.Index - 1]) + Subgraph_SmoothedRateOfChange[sc.Index - 1];
		
		Subgraph_PriceMomentumOscillatorLine[sc.Index] = (2.0f / Input_PriceMomentumOscillatorLineLength2.GetInt())*(10 * Subgraph_SmoothedRateOfChange[sc.Index] - Subgraph_PriceMomentumOscillatorLine[sc.Index - 1]) + Subgraph_PriceMomentumOscillatorLine[sc.Index - 1];
	}

	sc.MovingAverage(Subgraph_PriceMomentumOscillatorLine, Subgraph_PriceMomentumOscillatorSignal, Input_MovingAverageType.GetMovAvgType(), Input_PriceMomentumOscillatorSignalLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphReverseOrder(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ReverseOrder = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef DrawZeros = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Reverse Order";
		
		sc.AutoLoop = 0;

		

		ReverseOrder.Name = "ReverseOrder";
		ReverseOrder.DrawStyle = DRAWSTYLE_LINE;
		ReverseOrder.PrimaryColor = RGB(0, 255, 0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}

	ReverseOrder.DrawZeros = DrawZeros.GetYesNo();

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	//Is a full recalculation or there are new chart bars
	if (sc.IsFullRecalculation || (sc.UpdateStartIndex < sc.ArraySize - 1))
		CalculationStartIndex=0;

	int OutputIndex = sc.ArraySize - 1 - CalculationStartIndex;

	if (OutputIndex < 0)
		return;


	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{
		float BasedOnStudySubgraphValue = sc.BaseData[InputData.GetInputDataIndex()][Index];

		ReverseOrder[OutputIndex] = BasedOnStudySubgraphValue;
		OutputIndex--;

		if (OutputIndex < 0)
			break;
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = 0;
}
