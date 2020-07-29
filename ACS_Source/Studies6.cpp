#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows header file
#include <windows.h>

#include "sierrachart.h"
#include "scstudyfunctions.h"

/*============================================================================*/
SCSFExport scsf_SumAllChartsBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Sum All Charts (Bar)" ;

		sc.StudyDescription = "Sum All Charts (Bar).";

		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;
		
		sc.ValueFormat = 2;

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
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = false;

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

		return;
	}


	// Initialize the data in the destination subgraphs to 0 before looping through the charts.
	
	for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
	{
		// Loop through the subgraphs
		for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][DataIndex]	= 0;
		}
	}


	// Loop through the charts
	for (int ChartIndex = 1; ChartIndex < 201; ChartIndex++)
	{
		SCGraphData ReferenceArrays;
		sc.GetChartBaseData(-ChartIndex, ReferenceArrays);
		
		if (ReferenceArrays[SC_OPEN].GetArraySize() < 1)
			continue;
		
		
		// Loop through the data indexes
		for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
		{

			// Get the matching index 
			int RefDataIndex = sc.GetNearestMatchForDateTimeIndex(ChartIndex, DataIndex);

			// Loop through the subgraphs
			for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
			{
				// Add in the value of the current chart
				sc.Subgraph[SubgraphIndex][DataIndex]
				+= ReferenceArrays[SubgraphIndex][RefDataIndex];
			}
			sc.CalculateOHLCAverages(DataIndex);
		}
	}

	return;

}

/*============================================================================*/


SCSFExport scsf_MultiplyAllChartsBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Multiply All Charts" ;

		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		sc.ValueFormat = 2;

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
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(0,0,255);
		Volume.DrawZeros = false;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(127,0,255);
		OpenInterest.DrawZeros = false;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);
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

		return;
	}


	// Initialize the data in the destination subgraphs to 1.0 before looping through the charts.
	for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
	{
		// Loop through the subgraphs
		for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][DataIndex]	= 1.0;
		}
	}


	// Loop through the charts. Up to 200.
	for (int ChartIndex = 1; ChartIndex < 201; ChartIndex++)
	{
		SCGraphData ReferenceArrays;
		sc.GetChartBaseData(-ChartIndex, ReferenceArrays);

		if (ReferenceArrays[SC_OPEN].GetArraySize() < 1)
			continue;


		// Loop through the data indexes
		for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
		{

			// Get the matching index 
			int RefDataIndex = sc.GetNearestMatchForDateTimeIndex(ChartIndex, DataIndex);

			// Loop through the subgraphs
			for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
			{
				// Add in the value of the current chart
				sc.Subgraph[SubgraphIndex][DataIndex]
				*= ReferenceArrays[SubgraphIndex][RefDataIndex];
			}
			sc.CalculateOHLCAverages(DataIndex);
		}
	}

	return;

}

/*============================================================================*/
SCSFExport scsf_SumChartsFromList(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	SCInputRef Divisor = sc.Input[0];
	SCInputRef SyncCharts = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Sum Charts From List" ;

		sc.StudyDescription = "Sum Charts From List.";

		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;
		
		sc.ValueFormat = 2;

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
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = false;

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


		Divisor.Name = "Divisor. Use 0 for divisor equal to number of charts.";
		Divisor.SetFloat(1);
		
		SyncCharts.Name = "Synchronize charts";
		SyncCharts.SetYesNo(false);

		sc.TextInputName = "List of Chart Numbers (comma separated)";

		return;
	}

	// Do data processing

	int Sync = 1;
	
	if(SyncCharts.GetYesNo())
		Sync = -1;

	// Make a copy of the list of chart numbers from the text input
	SCString ChartNumberList = sc.TextInput;

	// Get each of the chart numbers (from the text input) and put them into a
	// vector.  
	std::vector<int> ChartNumbers;
	std::vector<float> ChartMultipliers;
	
	char* TokenContext = NULL;
	char* Token = strtok_s((char *)ChartNumberList.GetChars(), ",", &TokenContext);
	while (Token != NULL)
	{
		ChartNumbers.push_back(atoi(Token));

		float Multiplier = 0.0;
		char* FoundMultiplier = strchr(Token, '*');
		if (FoundMultiplier != NULL)
			Multiplier =(float) atof(FoundMultiplier + 1);
		
		if (Multiplier == 0.0)
			Multiplier = 1.0;
		
		ChartMultipliers.push_back(Multiplier);

		// Get the next chart number
		Token = strtok_s(NULL, ",", &TokenContext);
	}
	
	float DivisorValue = Divisor.GetFloat();
	if (DivisorValue == 0)
		DivisorValue = (float)ChartNumbers.size();

	int Index = 0;
	for (std::vector<int>::iterator it = ChartNumbers.begin(); it != ChartNumbers.end(); ++it, ++Index)
	{
		int ChartNumber = *it;
		float ChartMultiplier = ChartMultipliers[Index];

		
		
		SCGraphData ReferenceArrays;
		sc.GetChartBaseData(ChartNumber*Sync, ReferenceArrays);
		
		if (ReferenceArrays[SC_OPEN].GetArraySize() < 1)
			continue;
		
		
		// Loop through the data indexes
		for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
		{
			// Get the matching index 
			int RefDataIndex = sc.GetNearestMatchForDateTimeIndex(ChartNumber, DataIndex);

			// Loop through the subgraphs
			for (int SubgraphIndex = 0; SubgraphIndex <= SC_LAST; SubgraphIndex++)
			{
				// Add in the value of the current chart
				if (Index == 0)
				{
					sc.Subgraph[SubgraphIndex][DataIndex] = ReferenceArrays[SubgraphIndex][RefDataIndex] * ChartMultiplier / DivisorValue;
				}
				else
				{
					sc.Subgraph[SubgraphIndex][DataIndex] += ReferenceArrays[SubgraphIndex][RefDataIndex] * ChartMultiplier / DivisorValue;

				}

			}

			sc.Subgraph[ SC_HIGH][DataIndex] = max(sc.Subgraph[SC_OPEN][DataIndex],
				max(sc.Subgraph[SC_HIGH][DataIndex],
				max(sc.Subgraph[SC_LOW][DataIndex],sc.Subgraph[SC_LAST][DataIndex])
				)
				);

			sc.Subgraph[SC_LOW][DataIndex] = min(sc.Subgraph[SC_OPEN][DataIndex],
				min(sc.Subgraph[SC_HIGH][DataIndex],
				min(sc.Subgraph[SC_LOW][DataIndex],sc.Subgraph[SC_LAST][DataIndex])
				)
				);

			// Loop through the subgraphs
			for (int SubgraphIndex = SC_VOLUME; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
			{
				// Add in the value of the current chart
				if (Index == 0)
					sc.Subgraph[SubgraphIndex][DataIndex] = ReferenceArrays[SubgraphIndex][RefDataIndex] * (1.0f / ChartMultiplier) / DivisorValue;
				else
					sc.Subgraph[SubgraphIndex][DataIndex] += ReferenceArrays[SubgraphIndex][RefDataIndex] * (1.0f / ChartMultiplier) / DivisorValue;

			}

			sc.CalculateOHLCAverages(DataIndex);
		}
	}
}

/*============================================================================
----------------------------------------------------------------------------*/
SCSFExport scsf_BidAskVolumeRatio(SCStudyInterfaceRef sc)
{
	SCSubgraphRef VolRatioAvg = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];

	SCFloatArrayRef ExtraArrayRatio = sc.Subgraph[0].Arrays[0];

	SCInputRef InMALength = sc.Input[0];
	SCInputRef InMAType = sc.Input[1];
	SCInputRef InDifferenceCalculation = sc.Input[2];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Bid Ask Volume Ratio";

		sc.StudyDescription = "Bid Ask Volume Ratio";
		
		sc.AutoLoop = 1;

		VolRatioAvg.Name = "Vol Ratio Avg";
		VolRatioAvg.DrawStyle = DRAWSTYLE_LINE;
		VolRatioAvg.PrimaryColor = RGB(0,255,0);
		VolRatioAvg.DrawZeros = false; 

		ZeroLine.Name = "Zero Line";
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.PrimaryColor = RGB(255,0,255);
		ZeroLine.DrawZeros = true; 
		
		InMALength.Name = "Length";
		InMALength.SetInt(14);
		InMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		InMAType.Name= "Moving Average Type";
		InMAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		InDifferenceCalculation.Name = "Difference Calculation Method";
		InDifferenceCalculation.SetCustomInputStrings("Ask Volume - Bid Volume;Bid Volume - Ask Volume");
		InDifferenceCalculation.SetCustomInputIndex(0);
		
		return;
	}

	//if(sc.GetBarHasClosedStatus() == BHCS_BAR_HAS_NOT_CLOSED)
		//return;

	// Do data processing

	float UpVol = sc.AskVolume[sc.Index];
	float DownVol = sc.BidVolume[sc.Index];

	float TotalVol = UpVol + DownVol;

	if(InDifferenceCalculation.GetIndex() == 0 && TotalVol > 0)
		ExtraArrayRatio[sc.Index] = 100.0f * (UpVol - DownVol) / TotalVol;
	else if(InDifferenceCalculation.GetIndex() == 1 && TotalVol > 0)
		ExtraArrayRatio[sc.Index] = 100.0f * (DownVol - UpVol) / TotalVol;
	else
		ExtraArrayRatio[sc.Index] = 0.0f;
	
	sc.MovingAverage(ExtraArrayRatio, VolRatioAvg, InMAType.GetMovAvgType(), InMALength.GetInt());
}

/*============================================================================
----------------------------------------------------------------------------*/
SCSFExport scsf_StudyAngle(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Angle = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];
	
	SCInputRef InInputData = sc.Input[0];
	SCInputRef InLength = sc.Input[1];
	SCInputRef InValuePerPoint = sc.Input[2];
	SCInputRef InSkipCalculationAtStartOfTradingDay = sc.Input[3];
	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Study Angle";
		sc.AutoLoop = 1;
		
		Angle.Name = "Angle";
		Angle.DrawStyle = DRAWSTYLE_LINE;
		Angle.PrimaryColor = RGB(0,255,0);
		Angle.DrawZeros= false;
		
		ZeroLine.Name = "Zero Line";
		ZeroLine.DrawStyle = DRAWSTYLE_LINE;
		ZeroLine.PrimaryColor = RGB(255,0,255);
		ZeroLine.DrawZeros= true;
		
		InInputData.Name = "Input Data";
		InInputData.SetInputDataIndex(0);
		
		InLength.Name = "Length";
		InLength.SetInt(10);
		InLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		InValuePerPoint.Name = "Value Per Point";
		InValuePerPoint.SetFloat(1.0f);

		InSkipCalculationAtStartOfTradingDay.Name = "Skip Calculation at Start of Trading Day";
		InSkipCalculationAtStartOfTradingDay.SetYesNo(false);

		return;
	}
	
	int Length = InLength.GetInt();
	
	sc.DataStartIndex = Length;
	
	if (InValuePerPoint.GetFloat() == 0.0f)
		InValuePerPoint.SetFloat(0.01f);
	
	int &r_IndexOfNewDay = sc.GetPersistentInt(1);
	if (sc.IsFullRecalculation && sc.Index == 0)
		r_IndexOfNewDay = -1;

	// Do data processing
	bool SkipCalculation = false;
	if (InSkipCalculationAtStartOfTradingDay.GetYesNo())
	{
		if (sc.IsNewTradingDay(sc.Index))
		{
			r_IndexOfNewDay = sc.Index;
		}

		SkipCalculation = r_IndexOfNewDay == sc.Index || (sc.Index - Length) < r_IndexOfNewDay ;
	}


	if(!SkipCalculation)
	{

		SCFloatArrayRef InData = sc.BaseData[InInputData.GetInputDataIndex()];

		float BackValue = InData[sc.Index - Length];
		float CurrentValue = InData[sc.Index];

		float PointChange = (CurrentValue - BackValue) / InValuePerPoint.GetFloat();

		Angle[sc.Index] = (float)(atan2((double)PointChange, (double)Length) * 180.0 / M_PI);
	}
	else
	{
		Angle[sc.Index] = 0;

	}
}

/*==========================================================================*/
SCSFExport scsf_DetrendedOscillator(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables
	
	SCSubgraphRef DPO = sc.Subgraph[0];
	
	SCSubgraphRef overbought = sc.Subgraph[1];
	SCSubgraphRef oversold = sc.Subgraph[2];
	
	SCFloatArrayRef MA = sc.Subgraph[0].Arrays[0];
	
	SCInputRef I_Length = sc.Input[0];
	SCInputRef I_overbought = sc.Input[1];
	SCInputRef I_oversold = sc.Input[2];
	SCInputRef I_MovingAverageType = sc.Input[3];
	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Detrended Oscillator";
		
		sc.StudyDescription = "Detrended Oscillator.";
		
		sc.AutoLoop = 1;  // true
		
		DPO.Name = "DO";
		DPO.DrawStyle = DRAWSTYLE_LINE;
		DPO.PrimaryColor = RGB(0,255,0);
		
		overbought.Name = "Level 1";
		overbought.DrawStyle = DRAWSTYLE_LINE;
		overbought.PrimaryColor = RGB(255,255,0);        
		
		oversold.Name = "Level 2";
		oversold.DrawStyle = DRAWSTYLE_LINE;
		oversold.PrimaryColor = RGB(255,255,0);
		
		I_Length.Name = "Length";
		I_Length.SetInt(20);
		
		I_overbought.Name = "Overbought Level";
		I_overbought.SetFloat(0.5);
		
		I_oversold.Name = "Oversold Level";
		I_oversold.SetFloat(-0.5);
		
		I_MovingAverageType.Name = "Moving Average Type";
		I_MovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		return;
	}
	
	
	// Section 2 - Do data processing here
	
	sc.DataStartIndex = I_Length.GetInt();
	
	int N = I_Length.GetInt()/2;
	
	overbought[sc.Index] = I_overbought.GetFloat();
	oversold[sc.Index] = I_oversold.GetFloat();
	
	
	sc.MovingAverage(sc.Close, MA, I_MovingAverageType.GetMovAvgType() , sc.Index, N);
	
	int N2 = (N/2) + 1;
	
	DPO[sc.Index] = (sc.Close[sc.Index] - MA[sc.Index - N2]); 
}

/*==========================================================================*/
SCSFExport scsf_RangeBarPredictor(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCSubgraphRef High =     sc.Subgraph[0];
	SCSubgraphRef Low  =     sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Range Bar Predictor";

		sc.StudyDescription = "Range Bar Predictor.";

		sc.GraphRegion = 0;

		sc.AutoLoop = 1;  // true

		High.Name = "Predicted High";
		High.DrawStyle = DRAWSTYLE_DASH;
		High.PrimaryColor = RGB(0,255,0);
		High.LineWidth = 2;
		High.DrawZeros = false;

		Low.Name = "Predicted Low";
		Low.DrawStyle = DRAWSTYLE_DASH;
		Low.PrimaryColor = RGB(255,0,0);
		Low.LineWidth = 2;
		Low.DrawZeros = false;

		return;
	}

	// Section 2 - Do data processing here
	if(sc.Index == sc.ArraySize-1)
	{
		High[sc.Index] = sc.Low[sc.Index] + sc.RangeBarValue;
		Low[sc.Index] =  sc.High[sc.Index] - sc.RangeBarValue; 
	}
	else
	{
		High[sc.Index] = 0;
		Low[sc.Index] =  0;
	}
}  

/*==========================================================================*/
SCSFExport scsf_RenkoBarPredictor(SCStudyInterfaceRef sc)
{
	SCSubgraphRef High = sc.Subgraph[0];
	SCSubgraphRef Low  = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Renko Bar Predictor";

		sc.StudyDescription = "Renko Bar Predictor.";

		sc.GraphRegion = 0;

		sc.AutoLoop = 1;  // true

		High.Name = "Predicted High";
		High.DrawStyle = DRAWSTYLE_DASH;
		High.PrimaryColor = RGB(0,255,0);
		High.LineWidth = 2;
		High.DrawZeros = false;

		Low.Name = "Predicted Low";
		Low.DrawStyle = DRAWSTYLE_DASH;
		Low.PrimaryColor = RGB(255,0,0);
		Low.LineWidth = 2;
		Low.DrawZeros = false;

		return;
	}

	int Index = sc.Index;

	if (Index > 0 && Index == sc.ArraySize-1)
	{
		float PriorRenkoOpen = sc.BaseData[SC_RENKO_OPEN][Index-1];
		float PriorRenkoClose = sc.BaseData[SC_RENKO_CLOSE][Index-1];
		bool  PriorBarUp = PriorRenkoOpen < PriorRenkoClose;

		n_ACSIL::s_BarPeriod  BarPeriod;
		sc.GetBarPeriodParameters(BarPeriod);
		int RenkoTicksPerBar = BarPeriod.IntradayChartBarPeriodParameter1;
		int RenkoTrendOpenOffsetInTicks = BarPeriod.IntradayChartBarPeriodParameter2;
		int RenkoReversalOpenOffsetInTicks = BarPeriod.IntradayChartBarPeriodParameter3;

		if (BarPeriod.IntradayChartBarPeriodParameter4 <= 2)//exceeded mode
			RenkoTrendOpenOffsetInTicks -= 1;

		if (PriorBarUp)
		{
			if (BarPeriod.IntradayChartBarPeriodType != IBPT_FLEX_RENKO_IN_TICKS_INVERSE_SETTINGS)
			{
				High[Index] = PriorRenkoClose - RenkoTrendOpenOffsetInTicks * sc.TickSize + RenkoTicksPerBar * sc.TickSize;
				Low[Index] = PriorRenkoClose + RenkoReversalOpenOffsetInTicks * sc.TickSize - 2.0f * RenkoTicksPerBar * sc.TickSize;
			}
			else
			{
				High[Index] = PriorRenkoClose + RenkoTrendOpenOffsetInTicks * sc.TickSize;
				Low[Index] = PriorRenkoClose - RenkoReversalOpenOffsetInTicks * sc.TickSize;
			}
		}
		else
		{
			if (BarPeriod.IntradayChartBarPeriodType != IBPT_FLEX_RENKO_IN_TICKS_INVERSE_SETTINGS)
			{
				High[Index] = PriorRenkoClose - RenkoReversalOpenOffsetInTicks * sc.TickSize + 2.0f * RenkoTicksPerBar * sc.TickSize;
				Low[Index] = PriorRenkoClose + RenkoTrendOpenOffsetInTicks * sc.TickSize - RenkoTicksPerBar * sc.TickSize;
			}
			else
			{
				High[Index] = PriorRenkoClose + RenkoReversalOpenOffsetInTicks * sc.TickSize;
				Low[Index] = PriorRenkoClose - RenkoTrendOpenOffsetInTicks * sc.TickSize;
			}
		}
	}
	else
	{
		High[Index] = 0;
		Low[Index]  = 0;
	}
}  

/*============================================================================*/
SCSFExport scsf_FastStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentK = sc.Subgraph[0];
	SCSubgraphRef PercentD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];

	SCInputRef Unused2 = sc.Input[2];
	SCInputRef KLength = sc.Input[3];
	SCInputRef DLength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Stochastic - Fast";

		sc.ValueFormat = 2;

		PercentK.Name = "%K";
		PercentK.DrawStyle = DRAWSTYLE_LINE;
		PercentK.PrimaryColor = RGB(0,255,0);
		PercentK.DrawZeros = true;

		PercentD.Name = "%D";
		PercentD.DrawStyle = DRAWSTYLE_LINE;
		PercentD.PrimaryColor = RGB(255,0,255);
		PercentD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawZeros = true;

		Unused2.Name = "";
		Unused2.SetInt(10);
		Unused2.SetIntLimits(1,MAX_STUDY_LENGTH);

		KLength.Name = "%K Length";
		KLength.SetInt(10);
		KLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		DLength.Name = "%D Length";
		DLength.SetInt(3);
		DLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		
		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = true;
		return;
	}


	sc.DataStartIndex = KLength.GetInt() + DLength.GetInt();

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		PercentK,  // Data member is Fast %K
		KLength.GetInt(),
		DLength.GetInt(),
		0,
		MovAvgType.GetMovAvgType()
	);

	
	PercentD[sc.Index] = PercentK.Arrays[0][sc.Index];  // Fast %D (Slow %K)

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/*============================================================================*/
SCSFExport scsf_KDFastStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentK = sc.Subgraph[0];
	SCSubgraphRef PercentD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];

	SCInputRef Unused2 = sc.Input[2];
	SCInputRef KLength = sc.Input[3];
	SCInputRef DLength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];

	if (sc.SetDefaults)
	{
		sc.GraphName = "KD - Fast";

		sc.ValueFormat = 2;

		PercentK.Name = "%K";
		PercentK.DrawStyle = DRAWSTYLE_LINE;
		PercentK.PrimaryColor = RGB(0,255,0);
		PercentK.DrawZeros = true;

		PercentD.Name = "%D";
		PercentD.DrawStyle = DRAWSTYLE_LINE;
		PercentD.PrimaryColor = RGB(255,0,255);
		PercentD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawZeros = true;

		Unused2.Name = "";
		Unused2.SetInt(10);
		Unused2.SetIntLimits(1,MAX_STUDY_LENGTH);

		KLength.Name = "%K Length";
		KLength.SetInt(10);
		KLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		DLength.Name = "%D Length";
		DLength.SetInt(3);
		DLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = true;
		return;
	}

	

	sc.DataStartIndex = KLength.GetInt() + DLength.GetInt();

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		PercentK,  // Data member is Fast %K
		KLength.GetInt(),
		DLength.GetInt(),
		0,
		MovAvgType.GetMovAvgType()
		);


	PercentD[sc.Index] = PercentK.Arrays[0][sc.Index];  // Fast %D (Slow %K)

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/*============================================================================*/
SCSFExport scsf_SlowStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SlowK = sc.Subgraph[0];
	SCSubgraphRef SlowD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];

	SCInputRef FastKLength = sc.Input[2];
	SCInputRef FastDLength = sc.Input[3];
	SCInputRef SlowDLength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];
	SCInputRef UpdateFlag = sc.Input[11];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Stochastic - Slow";

		sc.ValueFormat = 2;

		SlowK.Name = "Slow %K";
		SlowK.DrawStyle = DRAWSTYLE_LINE;
		SlowK.PrimaryColor = RGB(0,255,0);
		SlowK.DrawZeros = true;

		SlowD.Name = "Slow %D";
		SlowD.DrawStyle = DRAWSTYLE_LINE;
		SlowD.PrimaryColor = RGB(255,0,255);
		SlowD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawZeros = true;

		FastKLength.Name = "Fast %K Length";
		FastKLength.SetInt(10);
		FastKLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FastDLength.Name = "Fast %D Length (Slow %K)";
		FastDLength.SetInt(3);
		FastDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowDLength.Name = "Slow %D Length";
		SlowDLength.SetInt(3);
		SlowDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);

		UpdateFlag.SetInt(1); //update flag

		sc.AutoLoop = true;
		return;
	}


	sc.DataStartIndex = FastKLength.GetInt() + FastDLength.GetInt() + SlowDLength.GetInt();

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		Temp4,  // Data member is Fast %K
		FastKLength.GetInt(),
		FastDLength.GetInt(),
		SlowDLength.GetInt(),
		MovAvgType.GetMovAvgType()
		);

	SlowK[sc.Index] = Temp4.Arrays[0][sc.Index];  
	SlowD[sc.Index] = Temp4.Arrays[1][sc.Index];  

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();

	return;


}


/*============================================================================*/
SCSFExport scsf_KDSlowStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SlowK = sc.Subgraph[0];
	SCSubgraphRef SlowD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];

	SCInputRef FastKLength = sc.Input[2];
	SCInputRef FastDLength = sc.Input[3];
	SCInputRef SlowDLength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];

	if (sc.SetDefaults)
	{
		sc.GraphName = "KD - Slow";

		sc.ValueFormat = 2;

		SlowK.Name = "Slow %K";
		SlowK.DrawStyle = DRAWSTYLE_LINE;
		SlowK.PrimaryColor = RGB(0,255,0);
		SlowK.DrawZeros = true;

		SlowD.Name = "Slow %D";
		SlowD.DrawStyle = DRAWSTYLE_LINE;
		SlowD.PrimaryColor = RGB(255,0,255);
		SlowD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawZeros = true;

		FastKLength.Name = "Fast %K Length";
		FastKLength.SetInt(10);
		FastKLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FastDLength.Name = "Fast %D Length (Slow %K)";
		FastDLength.SetInt(3);
		FastDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowDLength.Name = "Slow %D Length";
		SlowDLength.SetInt(3);
		SlowDLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);


		sc.AutoLoop = true;
		return;
	}


	sc.DataStartIndex = FastKLength.GetInt() + FastDLength.GetInt() + SlowDLength.GetInt();

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		Temp4,  // Data member is Fast %K
		FastKLength.GetInt(),
		FastDLength.GetInt(),
		SlowDLength.GetInt(),
		MovAvgType.GetMovAvgType()
		);

	SlowK[sc.Index] = Temp4.Arrays[0][sc.Index];  
	SlowD[sc.Index] = Temp4.Arrays[1][sc.Index];  

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();

	return;


}
/*============================================================================*/
SCSFExport scsf_DoubleTrix(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCInputRef I_TRIXLength = sc.Input[0];
	SCInputRef I_SigLineXMALen = sc.Input[1];

	SCFloatArrayRef EMA1 = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef EMA2 = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef EMA3 = sc.Subgraph[0].Arrays[2];

	SCSubgraphRef TRIXLine = sc.Subgraph[0];
	SCSubgraphRef SignalLine = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Double Trix";

		sc.StudyDescription = "DoubleTrix.";

		sc.AutoLoop = 1;  // true

		TRIXLine.Name = "TRIXLine";
		TRIXLine.DrawStyle = DRAWSTYLE_LINE;
		TRIXLine.PrimaryColor = RGB(0,255,0);
		TRIXLine.LineWidth = 1;

		SignalLine.Name = "SignalLine";
		SignalLine.DrawStyle = DRAWSTYLE_LINE;
		SignalLine.PrimaryColor = RGB(255,0,0);
		SignalLine.LineWidth = 1;

		I_TRIXLength.Name = "TRIXLength";
		I_TRIXLength.SetInt(5);

		I_SigLineXMALen.Name = "SigLineXMALen";
		I_SigLineXMALen.SetInt(3);

		return;
	}

	// Section 2 - Do data processing here

	int TRIXLength = I_TRIXLength.GetInt();
	int SigLineXMALen = I_SigLineXMALen.GetInt();

	sc.ExponentialMovAvg(sc.Close,EMA1,TRIXLength);
	sc.ExponentialMovAvg(EMA1,EMA2,TRIXLength);
	sc.ExponentialMovAvg(EMA2,EMA3,TRIXLength);

	if(EMA3[sc.Index-1] != 0)
		TRIXLine[sc.Index] = 10 * ( EMA3[sc.Index] - EMA3[sc.Index-1] ) / EMA3[sc.Index-1] ;
	else
		TRIXLine[sc.Index] = 0;

	sc.ExponentialMovAvg(TRIXLine,SignalLine,SigLineXMALen);
}  

/*============================================================================*/
SCSFExport scsf_BollingerSqueeze2(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BollingerSqueezeSubgraph = sc.Subgraph[0];
	SCSubgraphRef SqueezeZerosSubgraph = sc.Subgraph[1];
	SCSubgraphRef KeltnerSubgraph = sc.Subgraph[2];
	SCSubgraphRef BBSubgraph = sc.Subgraph[3];
	SCSubgraphRef AverageTrueRange = sc.Subgraph[4];
	SCSubgraphRef Momentum = sc.Subgraph[5];
	SCSubgraphRef TopBollingerBand = sc.Subgraph[6];
	SCSubgraphRef BottomBollingerBand = sc.Subgraph[7];
	SCSubgraphRef TopKeltnerBand = sc.Subgraph[8];
	SCSubgraphRef BottomKeltnerBand = sc.Subgraph[9];



	SCInputRef InputInputData = sc.Input[0];
	SCInputRef InputMAType = sc.Input[1];
	SCInputRef InputKeltnerLength = sc.Input[2];
	SCInputRef InputKeltnerTRAvgLength = sc.Input[3];
	SCInputRef InputKeltnerMultiplier = sc.Input[4];
	SCInputRef InputBollingerLength = sc.Input[5];
	SCInputRef InputBollingerBandsMult = sc.Input[6];
	SCInputRef InputMomentumLength = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName="Bollinger Squeeze 2";
		sc.StudyDescription="Bollinger Squeeze 2";

		// Subgraphs
		BollingerSqueezeSubgraph.Name = "Momentum Avg";
		BollingerSqueezeSubgraph.DrawStyle = DRAWSTYLE_BAR;
		BollingerSqueezeSubgraph.PrimaryColor = RGB(0,255,0);
		BollingerSqueezeSubgraph.SecondaryColor = RGB(255,0,0);
		BollingerSqueezeSubgraph.SecondaryColorUsed = true;

		SqueezeZerosSubgraph.Name = "Squeeze Indicator";
		SqueezeZerosSubgraph.DrawStyle = DRAWSTYLE_POINT;
		SqueezeZerosSubgraph.LineWidth = 3;
		SqueezeZerosSubgraph.PrimaryColor = RGB(0,255,0);
		SqueezeZerosSubgraph.SecondaryColor = RGB(255,0,0);
		SqueezeZerosSubgraph.SecondaryColorUsed = true;

		Momentum.Name = "Momentum";
		Momentum.DrawStyle = DRAWSTYLE_IGNORE;

		KeltnerSubgraph.Name = "Keltner Average";
		KeltnerSubgraph.DrawStyle = DRAWSTYLE_IGNORE;

		AverageTrueRange.Name = "ATR";
		AverageTrueRange.DrawStyle = DRAWSTYLE_IGNORE;

		TopBollingerBand.Name = "Top Bollinger Band";
		TopBollingerBand.DrawStyle = DRAWSTYLE_IGNORE;

		BottomBollingerBand.Name = "Bottom Bollinger Band";
		BottomBollingerBand.DrawStyle = DRAWSTYLE_IGNORE;

		TopKeltnerBand.Name = "Top Keltner Band";
		TopKeltnerBand.DrawStyle = DRAWSTYLE_IGNORE;

		BottomKeltnerBand.Name = "Bottom Keltner Band";
		BottomKeltnerBand.DrawStyle = DRAWSTYLE_IGNORE;

		// Data Inputs
		InputInputData.Name = "Input Data";
		InputInputData.SetInputDataIndex(SC_LAST);

		InputMAType.Name="Moving Average Type for Internal Calculations";
		InputMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputKeltnerLength.Name="Keltner Bands Length";
		InputKeltnerLength.SetInt(20);
		InputKeltnerLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputKeltnerTRAvgLength.Name = "Keltner True Range MovAvg Length";
		InputKeltnerTRAvgLength.SetInt(20);
		InputKeltnerTRAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputKeltnerMultiplier.Name="Keltner Bands Multiplier";
		InputKeltnerMultiplier.SetFloat(2.0f);
		InputKeltnerMultiplier.SetFloatLimits(.001f,(float)MAX_STUDY_LENGTH);

		InputBollingerLength.Name="Bollinger Bands Length";
		InputBollingerLength.SetInt(20);
		InputBollingerLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputBollingerBandsMult.Name="Bollinger Bands Multiplier";
		InputBollingerBandsMult.SetFloat(10.0f);
		InputBollingerBandsMult.SetFloatLimits(.001f,(float)MAX_STUDY_LENGTH);

		InputMomentumLength.Name = "Momentum Length";
		InputMomentumLength.SetInt(20);
		InputMomentumLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		return;
	}


	int InputData = InputInputData.GetInputDataIndex();
	int MAType = InputMAType.GetMovAvgType();
	int KeltnerLength = InputKeltnerLength.GetInt();
	int KeltnerTRAVGLength = InputKeltnerTRAvgLength.GetInt();
	float KeltnerMult = InputKeltnerMultiplier.GetFloat();
	int BollingerLength = InputBollingerLength.GetInt();	
	float BandsMult = InputBollingerBandsMult.GetFloat();
	int MomentumLength = InputMomentumLength.GetInt();

	sc.DataStartIndex = max(KeltnerLength, max(KeltnerTRAVGLength, max(BollingerLength, MomentumLength))) - 1;


	// calculate Bollinger Bands
	sc.BollingerBands(sc.BaseData[InputData], BBSubgraph, BollingerLength, BandsMult, MAType);
	

// 	BollingerSqueezeSubgraph.Arrays[0][sc.Index] = 
// 		sc.Close[sc.Index] - sc.Close[sc.Index - MomentumLength];

	Momentum[sc.Index] = sc.Close[sc.Index] - sc.Close[sc.Index - MomentumLength];

	sc.ExponentialMovAvg(Momentum, BollingerSqueezeSubgraph, MomentumLength);

	// calculate Keltner
	sc.Keltner( sc.BaseData,sc.BaseData[InputData], KeltnerSubgraph, KeltnerLength, MAType,
		KeltnerTRAVGLength, MOVAVGTYPE_WILDERS, KeltnerMult,KeltnerMult);

	// calculate Average True Range: SimpleMovingAverage(True Range), use Arrays[0] to store True Range
	AverageTrueRange.Arrays[0][sc.Index] = sc.GetTrueRange(sc.BaseData, sc.Index);
	sc.SimpleMovAvg(AverageTrueRange.Arrays[0], AverageTrueRange, KeltnerLength);

	TopKeltnerBand[sc.Index] = KeltnerSubgraph[sc.Index] + (KeltnerMult * AverageTrueRange[sc.Index]);
	BottomKeltnerBand[sc.Index] = KeltnerSubgraph[sc.Index] - (KeltnerMult * AverageTrueRange[sc.Index]);
	TopBollingerBand[sc.Index] = BBSubgraph.Arrays[0][sc.Index];
	BottomBollingerBand[sc.Index] = BBSubgraph.Arrays[1][sc.Index];

	if ((TopBollingerBand[sc.Index] > TopKeltnerBand[sc.Index]) && (BottomBollingerBand[sc.Index] < BottomKeltnerBand[sc.Index])) 
	{
		SqueezeZerosSubgraph.DataColor[sc.Index] = SqueezeZerosSubgraph.PrimaryColor; 
		SqueezeZerosSubgraph[sc.Index] = 0.0001f;
	} 
	else 
	{	
		SqueezeZerosSubgraph.DataColor[sc.Index] = SqueezeZerosSubgraph.SecondaryColor;
		SqueezeZerosSubgraph[sc.Index] = -0.0001f;
	}


	if ((BollingerSqueezeSubgraph[sc.Index] > 0) &&
		(BollingerSqueezeSubgraph[sc.Index] >= BollingerSqueezeSubgraph[sc.Index - 1]))
	{
		BollingerSqueezeSubgraph.DataColor[sc.Index] = BollingerSqueezeSubgraph.PrimaryColor;
	}
	else if ((BollingerSqueezeSubgraph[sc.Index] > 0) && 
		(BollingerSqueezeSubgraph[sc.Index] < BollingerSqueezeSubgraph[sc.Index-1]))
	{
		BollingerSqueezeSubgraph.DataColor[sc.Index] = 
			RGB(GetRValue(BollingerSqueezeSubgraph.PrimaryColor)/2, 
			GetGValue(BollingerSqueezeSubgraph.PrimaryColor)/2, 
			GetBValue(BollingerSqueezeSubgraph.PrimaryColor)/2);
	}
	else if ((BollingerSqueezeSubgraph[sc.Index] < 0) && 
		(BollingerSqueezeSubgraph[sc.Index] <= BollingerSqueezeSubgraph[sc.Index-1])) 
		BollingerSqueezeSubgraph.DataColor[sc.Index] = BollingerSqueezeSubgraph.SecondaryColor;
	else if ((BollingerSqueezeSubgraph[sc.Index] < 0) && 
		(BollingerSqueezeSubgraph[sc.Index] > BollingerSqueezeSubgraph[sc.Index-1]))
	{
		BollingerSqueezeSubgraph.DataColor[sc.Index] = 
			RGB(GetRValue(BollingerSqueezeSubgraph.SecondaryColor)/2, 
			GetGValue(BollingerSqueezeSubgraph.SecondaryColor)/2, 
			GetBValue(BollingerSqueezeSubgraph.SecondaryColor)/2);
	}
}

/*==========================================================================*/

SCSFExport scsf_BollingerSqueeze3(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Ratio = sc.Subgraph[0];
	SCSubgraphRef SqueezeIndicator = sc.Subgraph[1];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_MovAvgType = sc.Input[1];
	SCInputRef Input_Length = sc.Input[2];
	SCInputRef Input_MovingAverageMultiplier = sc.Input[3];
	SCInputRef Input_StandardDeviationMultiplier = sc.Input[4];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Bollinger Squeeze 3";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		Ratio.Name = "Ratio";
		Ratio.DrawStyle = DRAWSTYLE_BAR;
		Ratio.PrimaryColor = RGB(0, 255, 0);
		Ratio.SecondaryColor = RGB(255, 0, 0);
		//Ratio.SecondaryColorUsed = true;
		Ratio.DrawZeros = true;

		SqueezeIndicator.Name = "Squeeze Indicator";
		SqueezeIndicator.DrawStyle = DRAWSTYLE_POINT;
		SqueezeIndicator.LineWidth = 3;
		SqueezeIndicator.PrimaryColor = RGB(0, 255, 0);
		SqueezeIndicator.SecondaryColor = RGB(255, 0, 0);
		SqueezeIndicator.SecondaryColorUsed = true;
		SqueezeIndicator.DrawZeros = true;

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_MovAvgType.Name = "Moving Average Type for  Calculations";
		Input_MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Input_Length.Name = "Study Length";
		Input_Length.SetInt(20);
		Input_Length.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		Input_MovingAverageMultiplier.Name = "Moving Average Multiplier";
		Input_MovingAverageMultiplier.SetFloat(1.5f);

		Input_StandardDeviationMultiplier.Name = "Standard Deviation Multiplier";
		Input_StandardDeviationMultiplier.SetFloat(2.0f);

		return;
	}
	
	sc.StdDeviation(sc.BaseData[Input_InputData.GetInputDataIndex()], Ratio.Arrays[0], Input_Length.GetInt());

	sc.TrueRange(sc.BaseData, Ratio.Arrays[1]);
	sc.MovingAverage(Ratio.Arrays[1], Ratio.Arrays[2], Input_MovAvgType.GetMovAvgType(), Input_Length.GetInt());

	float StandardDeviationValue = Ratio.Arrays[0][sc.Index];
	Ratio[sc.Index] = (Input_StandardDeviationMultiplier.GetFloat() * StandardDeviationValue) / (Input_MovingAverageMultiplier.GetFloat() * Ratio.Arrays[2][sc.Index]);

	if (Ratio[sc.Index] >= 1.0)
		SqueezeIndicator.DataColor[sc.Index] = SqueezeIndicator.PrimaryColor; //No squeeze
	else
		SqueezeIndicator.DataColor[sc.Index] = SqueezeIndicator.SecondaryColor; //Squeeze on
	
}
/*============================================================================*/
SCSFExport scsf_VolumeColoredBasedOnVolume(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Volume-Colored Based on Volume";


		sc.AutoLoop = 1;  // true

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.SecondaryColorUsed = 1;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.SecondaryColor = RGB(255,0,0);

		return;
	}

	// Section 2 - Do data processing here

	Volume[sc.Index] = sc.Volume[sc.Index];

	if(sc.Volume[sc.Index-1] <= sc.Volume[sc.Index])
		Volume.DataColor[sc.Index] = Volume.PrimaryColor;
	else
		Volume.DataColor[sc.Index] = Volume.SecondaryColor;

} 

/*==========================================================================*/
SCSFExport scsf_ExtendedArrayExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ADX = sc.Subgraph[0];
	SCSubgraphRef ADXForwardShifted = sc.Subgraph[1];

	SCSubgraphRef ADXForwardShiftedPlus = sc.Subgraph[2];


	SCInputRef DXLength = sc.Input[3];
	SCInputRef DXMovAngLength = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Extended Array Example";
		sc.StudyDescription = "Extended Array Example.  Based on ADX.";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		ADX.Name = "ADX";
		ADX.DrawStyle = DRAWSTYLE_LINE;
		ADX.PrimaryColor = RGB(0,255,0);
		ADX.DrawZeros = false;

		ADXForwardShifted.Name = "ADX Forward Shifted";
		ADXForwardShifted.DrawStyle = DRAWSTYLE_LINE;
		ADXForwardShifted.PrimaryColor = RGB(255,0,255);
		ADXForwardShifted.DrawZeros = false;

		// Set the ADXForwardShifted subgraph to support additional array elements for forward projection
		ADXForwardShifted.ExtendedArrayElementsToGraph = 50;

		ADXForwardShiftedPlus.Name = "ADX Plus Value";
		ADXForwardShiftedPlus.DrawStyle = DRAWSTYLE_LINE;
		ADXForwardShiftedPlus.ExtendedArrayElementsToGraph = 50;
		ADXForwardShiftedPlus.PrimaryColor = RGB(255,255,0);
		ADXForwardShiftedPlus.DrawZeros = false;


		DXLength.Name = "DX Length";
		DXLength.SetInt(14);
		DXLength.SetIntLimits(1, INT_MAX);


		DXMovAngLength.Name = "DX Mov Avg Length";
		DXMovAngLength.SetInt(14);
		DXMovAngLength.SetIntLimits(1, INT_MAX);
		
		
		return;
	}

	// Do data processing

	sc.DataStartIndex = DXLength.GetInt() + DXMovAngLength.GetInt() - 1;

	// Calculate ADX
	sc.ADX(
		sc.BaseData,
		ADX,
		DXLength.GetInt(),
		DXMovAngLength.GetInt());


	int NumberOfBarsToForwardShift = ADXForwardShifted.ExtendedArrayElementsToGraph;

	ADXForwardShifted[sc.Index] = ADX[sc.Index-NumberOfBarsToForwardShift];
	ADXForwardShiftedPlus[sc.Index] = ADXForwardShifted[sc.Index] + 10;

	// Extended elements are cleared when the number of bars in the chart grows. Therefore, we need to fill them in every time. However, only when we are on the final index when using sc.AutoLoop = 1.
	if(sc.Index+1 >= sc.ArraySize)	
	{
		for (int i = sc.ArraySize;i < sc.ArraySize + NumberOfBarsToForwardShift ;i++)
		{	
			ADXForwardShifted[i] = ADX[i-NumberOfBarsToForwardShift];	
			ADXForwardShiftedPlus[i] = ADXForwardShifted[i] + 10;
		}
	}

	return;
}

/*==========================================================================*/
SCSFExport scsf_ForwardProjectionBars(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];

	SCInputRef Input_NumberOfBarsToProjectForward = sc.Input[0];


	if (sc.SetDefaults)
	{
		sc.GraphName = "Forward Projection Bars";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;
		sc.GraphDrawType=GDT_OHLCBAR;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0, 255, 0);
		Open.DrawZeros = false;		

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Input_NumberOfBarsToProjectForward.Name = "Number of Bars to Project Forward";
		Input_NumberOfBarsToProjectForward.SetInt(10);

		return;
	}

	// Do data processing
	int NumberOfBarsToProjectForward = Input_NumberOfBarsToProjectForward.GetInt();
	
	Open.ExtendedArrayElementsToGraph = NumberOfBarsToProjectForward;
	High.ExtendedArrayElementsToGraph = NumberOfBarsToProjectForward;
	Low.ExtendedArrayElementsToGraph = NumberOfBarsToProjectForward;
	Last.ExtendedArrayElementsToGraph = NumberOfBarsToProjectForward;

	for (int BarIndex = sc.ArraySize - NumberOfBarsToProjectForward; BarIndex < sc.ArraySize; BarIndex++)
	{
			Open[BarIndex + NumberOfBarsToProjectForward] = sc.Open[BarIndex];
			High[BarIndex + NumberOfBarsToProjectForward] = sc.High[BarIndex];
			Low[BarIndex + NumberOfBarsToProjectForward] = sc.Low[BarIndex];
			Last[BarIndex + NumberOfBarsToProjectForward] = sc.Close[BarIndex];
	}


	return;
}

/*==========================================================================*/

SCSFExport scsf_ArrayValueAtNthOccurrenceSample(SCStudyInterfaceRef sc)
{   
	SCSubgraphRef StochasticData = sc.Subgraph[0];
	SCSubgraphRef ValueAtOccurence = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];

	SCInputRef KLength = sc.Input[3];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef MovAvgType = sc.Input[7];
	SCInputRef InputDataHigh = sc.Input[8];
	SCInputRef InputDataLow = sc.Input[9];
	SCInputRef InputDataLast = sc.Input[10];
	SCInputRef NumberOfOccurrences = sc.Input[11];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Array Value At Nth Occurrence Sample";
		sc.StudyDescription = "Array Value At Nth Occurrence.  Based on %K array of Fast Stochastic";

		sc.ValueFormat = 2;

		StochasticData.Name = "%K";
		StochasticData.DrawStyle = DRAWSTYLE_LINE;
		StochasticData.PrimaryColor = RGB(0,255,0);
		StochasticData.DrawZeros = true;

		ValueAtOccurence.Name = "Value At Nth Occurrence";
		ValueAtOccurence.DrawStyle = DRAWSTYLE_LINE;
		ValueAtOccurence.PrimaryColor = RGB(255,0,255);
		ValueAtOccurence.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawZeros = true;

		KLength.Name = "%K Length";
		KLength.SetInt(10);
		KLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputDataHigh.Name = "Input Data";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data";
		InputDataLast.SetInputDataIndex(SC_LAST);
		
		NumberOfOccurrences.Name = "Number Of Occurrences";
		NumberOfOccurrences.SetInt(10);
		NumberOfOccurrences.SetIntLimits(1,MAX_STUDY_LENGTH);


		sc.AutoLoop = true;
		return;
	}

	// This sample is based on Fast Stochastic Study.
	// The sample study use only %K subgraph
	// If %K value greater than Line1 value, then TRUE will be put to TrueFalse array, otherwise FALSE
	// The ValueAtOccurence Subgraph will have the values returned by sc.ArrayValueAtNthOccurrence function

	int InDLength = 3;
	
	sc.DataStartIndex = KLength.GetInt() + InDLength;

	sc.Stochastic(sc.BaseDataIn, sc.Subgraph[4], KLength.GetInt(), 1, 1, 1);

	sc.Stochastic(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		StochasticData,  
		KLength.GetInt(),
		InDLength,
		0,
		MovAvgType.GetMovAvgType()
		);

	// fill the TrueFalse array
	if (StochasticData[sc.Index] > Line1Value.GetFloat())
		StochasticData.Arrays[1][sc.Index] = 1.0f;
	else
		StochasticData.Arrays[1][sc.Index] = 0.0f;

	ValueAtOccurence[sc.Index] = sc.ArrayValueAtNthOccurrence(StochasticData.Arrays[1], StochasticData, NumberOfOccurrences.GetInt());

	Line1[sc.Index] = Line1Value.GetFloat();
}

/*============================================================================*/
SCSFExport scsf_SwingHighAndLow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SwingHigh = sc.Subgraph[0];
	SCSubgraphRef SwingLow = sc.Subgraph[1];
	SCSubgraphRef LinePropertiesSubgraph = sc.Subgraph[2];

	SCInputRef ArrowOffsetValue = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef AllowEqualBars = sc.Input[2];
	SCInputRef ExtendSwings = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Swing High And Low";

		sc.AutoLoop = true;
		sc.GraphRegion = 0;
		sc.ValueFormat= VALUEFORMAT_INHERITED;

		SwingHigh.Name = "Swing High";
		SwingHigh.DrawStyle = DRAWSTYLE_ARROW_DOWN;
		SwingHigh.PrimaryColor = RGB(0,255,0);
		SwingHigh.DrawZeros = false;
		SwingHigh.PrimaryColor = RGB(255,0,0);
		SwingHigh.LineWidth = 3;

		SwingLow.Name = "Swing Low";
		SwingLow.DrawStyle = DRAWSTYLE_ARROW_UP;
		SwingLow.PrimaryColor = RGB(255,0,255);
		SwingLow.DrawZeros = false;
		SwingLow.LineWidth = 3;
		SwingLow.PrimaryColor = RGB(0,255,0);

		LinePropertiesSubgraph.Name = "Line Properties";
		LinePropertiesSubgraph.DrawStyle = DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY;
		LinePropertiesSubgraph.LineWidth = 1;
		LinePropertiesSubgraph.PrimaryColor = RGB(255, 0, 0);
		LinePropertiesSubgraph.SecondaryColor = RGB(0, 255, 0);
		LinePropertiesSubgraph.SecondaryColorUsed = true;
		LinePropertiesSubgraph.DrawZeros = false;

		ArrowOffsetValue.Name = "Arrow Offset as Percentage";
		ArrowOffsetValue.SetFloat(3);
		
		Length.Name = "Length";
		Length.SetInt(1);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		AllowEqualBars.Name = "Allow Equal High/Low Bars";
		AllowEqualBars.SetYesNo(false);

		ExtendSwings.Name = "Extend Swings Until Future Intersection";
		ExtendSwings.SetYesNo(false);

		return;
	}

	int IndexToEvaluate = sc.Index - Length.GetInt();

	if(IndexToEvaluate - Length.GetInt() < 0)
	{
		return;
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = IndexToEvaluate;

	float ArrowOffset=(sc.High[IndexToEvaluate] - sc.Low[IndexToEvaluate] )*(ArrowOffsetValue.GetFloat() * 0.01f);

	SwingHigh[IndexToEvaluate] = 0;
	SwingLow[IndexToEvaluate] = 0;

	// check for Swing High
	if (!AllowEqualBars.GetYesNo())
	{
		if (sc.IsSwingHigh(sc.High, IndexToEvaluate, Length.GetInt()))
			SwingHigh[IndexToEvaluate] = sc.High[IndexToEvaluate] + ArrowOffset;
	}
	else if (IsSwingHighAllowEqual_S( sc, true , IndexToEvaluate, Length.GetInt()))
		SwingHigh[IndexToEvaluate] = sc.High[IndexToEvaluate] + ArrowOffset;
	
	// check for Swing Low
	if (!AllowEqualBars.GetYesNo())
	{
		if (sc.IsSwingLow(sc.Low, IndexToEvaluate, Length.GetInt()))
			SwingLow[IndexToEvaluate] = sc.Low[IndexToEvaluate] - ArrowOffset;
	}
	else if (IsSwingLowAllowEqual_S(sc, true , IndexToEvaluate,  Length.GetInt()))
		SwingLow[IndexToEvaluate] = sc.Low[IndexToEvaluate] - ArrowOffset;

	if (ExtendSwings.GetYesNo() && SwingHigh[IndexToEvaluate] != 0)
	{
		sc.AddLineUntilFutureIntersection
			( IndexToEvaluate
			, 1  // LineIDForBar
			, SwingHigh[IndexToEvaluate]
			, LinePropertiesSubgraph.PrimaryColor
			, LinePropertiesSubgraph.LineWidth
			, LinePropertiesSubgraph.LineStyle
			, false
			, false
			, ""
			);
	}

	if (ExtendSwings.GetYesNo() && SwingLow[IndexToEvaluate] != 0)
	{
		sc.AddLineUntilFutureIntersection
			( IndexToEvaluate
			, 2  // LineIDForBar
			, SwingLow[IndexToEvaluate]
			, LinePropertiesSubgraph.SecondaryColor
			, LinePropertiesSubgraph.LineWidth
			, LinePropertiesSubgraph.LineStyle
			, false
			, false
			, ""
			);
	}
}

/******************************************************************************************/
SCSFExport scsf_ForceIndexAverage(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCSubgraphRef ForceAverage = sc.Subgraph[0];

	SCFloatArrayRef ForceIndex = sc.Subgraph[0].Arrays[0];

	SCInputRef AverageLength = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Force Index Average";

		sc.StudyDescription = "";

		sc.AutoLoop = 1; 


		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		ForceAverage.Name = "Force Average";
		ForceAverage.DrawStyle = DRAWSTYLE_LINE;
		ForceAverage.PrimaryColor = RGB(0,255,0);
		ForceAverage.DrawZeros = true;

		AverageLength.Name = "Moving Average Length";
		AverageLength.SetInt(20);
		AverageLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	// Section 2 - Do data processing here

	sc.DataStartIndex = AverageLength.GetInt(); 

	int i = sc.Index;

	ForceIndex[i] = sc.Volume[i] * (sc.Close[i] - sc.Close[i-1]);

	sc.ExponentialMovAvg(ForceIndex,ForceAverage,AverageLength.GetInt());

}

/******************************************************************************************/
SCSFExport scsf_ForceIndex(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables

	SCSubgraphRef ForceIndex = sc.Subgraph[0];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Force Index";

		sc.StudyDescription = "";

		sc.AutoLoop = 1; 

		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		ForceIndex.Name = "Force Average";
		ForceIndex.DrawStyle = DRAWSTYLE_LINE;
		ForceIndex.PrimaryColor = RGB(0,255,0);
		ForceIndex.DrawZeros = true;

		return;
	}

	// Section 2 - Do data processing here


	int i = sc.Index;

	ForceIndex[i] = sc.Volume[i] * (sc.Close[i] - sc.Close[i-1]);
}
/******************************************************************************************/

/********************************************************************
*
*		True Strength Index (Also known as Ergodic)
*
*			Array 3-8 are used for calculations
*
*			Numerator = EMA( EMA(Price - LastPrice, LongExpMALength), ShortExpMALength)
*			Denominator = EMA( EMA( Abs(Price - LastPrice), LongExpMALength), ShortExpMaLength)
*
*			TSI = Multiplier * Numerator / Denominator
*			SignalMA = EMA(TSI, SignalMALength)
*			Oscillator = TSI - SignalMA
*
*********************************************************************/
SCSFExport scsf_TrueStrengthIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TSI = sc.Subgraph[0];
	SCSubgraphRef SignalMA = sc.Subgraph[1];
	SCSubgraphRef Oscillator = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Temp5 = sc.Subgraph[5];
	SCSubgraphRef Temp6 = sc.Subgraph[6];
	SCSubgraphRef Temp7 = sc.Subgraph[7];
	SCSubgraphRef Temp8 = sc.Subgraph[8];

	SCInputRef InputData = sc.Input[0];
	SCInputRef LongExpMALength = sc.Input[3];
	SCInputRef ShortExpMALength = sc.Input[4];
	SCInputRef Multiplier = sc.Input[5];
	SCInputRef SignalLineMALength = sc.Input[6];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "True Strength Index";
		
		sc.GraphRegion	= 1;
		sc.ValueFormat	= 2;
		sc.AutoLoop		= 1; 

		TSI.Name = "TSI";
		TSI.DrawStyle = DRAWSTYLE_LINE;
		TSI.PrimaryColor = RGB(0,255,0);
		TSI.DrawZeros = false;

		SignalMA.Name = "Signal MA";
		SignalMA.DrawStyle = DRAWSTYLE_HIDDEN;
		SignalMA.PrimaryColor = RGB(255,0,255);
		SignalMA.DrawZeros = false;

		Oscillator.Name = "Oscillator";
		Oscillator.DrawStyle = DRAWSTYLE_HIDDEN;
		Oscillator.PrimaryColor = RGB(255,255,0);
		Oscillator.DrawZeros = false;

		
		
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		LongExpMALength.Name = "Long Exp MovAvg Length";
		LongExpMALength.SetInt(15);
		LongExpMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		ShortExpMALength.Name = "Short Exp MovAvg Length";
		ShortExpMALength.SetInt(5);
		ShortExpMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Multiplier.Name = "Multiplier";
		Multiplier.SetInt(1);

		SignalLineMALength.Name = "Signal Line MovAvg Length";
		SignalLineMALength.SetInt(10);
		SignalLineMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	// Subgraphs 3-7 are used for calculation only
	sc.DataStartIndex = ((LongExpMALength.GetInt() ) + (ShortExpMALength.GetInt()) + SignalLineMALength.GetInt()) ;

	if (sc.Index < 1) // not large enough
		return;

	int LongStart = sc.Index;
	if (LongStart < LongExpMALength.GetInt() - 1)  //2
		LongStart = LongExpMALength.GetInt() - 1;  //2

	int ShortStart = sc.Index;
	if (ShortStart < LongExpMALength.GetInt() - 1 + ShortExpMALength.GetInt() - 1)  //3
		ShortStart = LongExpMALength.GetInt() - 1 + ShortExpMALength.GetInt() - 1;  //3

	int SignalStart = sc.Index;
	if (SignalStart < ShortExpMALength.GetInt() - 1 + SignalLineMALength.GetInt() - 1+LongExpMALength.GetInt() -1)  //4
		SignalStart = ShortExpMALength.GetInt() - 1 + SignalLineMALength.GetInt() - 1 +LongExpMALength.GetInt() -1;  //4


	int DataStart = sc.Index;
	if (DataStart < 1)
		DataStart = 1;

	if(sc.Index >= DataStart)
		Temp3[sc.Index] = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index-1];

	// Calculate the Numerator into SubGraphData[0]
	if(sc.Index >= LongStart)
		sc.MovingAverage(Temp3, Temp4, MOVAVGTYPE_EXPONENTIAL, LongExpMALength.GetInt());

	if(sc.Index >= ShortStart)
		sc.MovingAverage(Temp4, Temp5, MOVAVGTYPE_EXPONENTIAL, ShortExpMALength.GetInt());
	
	// Calculate the Denominator into SubGraphData[1]
	if(sc.Index >= DataStart)
		Temp6[sc.Index] = fabs(Temp3[sc.Index]);

	if(sc.Index >= LongStart)
		sc.MovingAverage(Temp6, Temp7, MOVAVGTYPE_EXPONENTIAL, LongExpMALength.GetInt());

	if(sc.Index >= ShortStart)
	{
		sc.MovingAverage(Temp7, Temp8, MOVAVGTYPE_EXPONENTIAL, ShortExpMALength.GetInt());

		// Store the TSI (Numerator / Denominator)
		if (Temp8[sc.Index] != 0)
			TSI[sc.Index] = Multiplier.GetInt() * Temp5[sc.Index]/Temp8[sc.Index];
		else
			TSI[sc.Index] = 0;
	}

	// Calculate the Signal Line ( EMA[TSI] )
	if(sc.Index >= SignalStart)
		sc.MovingAverage(TSI, SignalMA, MOVAVGTYPE_EXPONENTIAL, SignalLineMALength.GetInt());

	// Calculate the Oscillator (TSI - EMA[TSI])
	if(sc.Index >= sc.DataStartIndex)
		Oscillator[sc.Index] = TSI[sc.Index] - SignalMA[sc.Index];
}

/*============================================================================*/
SCSFExport scsf_CommodityChannelIndex(SCStudyInterfaceRef sc)
{
	SCInputRef I_InputData = sc.Input[0];
	SCInputRef I_Length = sc.Input[3];
	SCInputRef I_Multiplier = sc.Input[4];
	SCInputRef I_Line2Value = sc.Input[5];
	SCInputRef I_Line3Value = sc.Input[6];
	SCInputRef I_MAType = sc.Input[7];
	SCInputRef I_Version = sc.Input[8];

	SCSubgraphRef CCI = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef Line3 = sc.Subgraph[3];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Commodity Channel Index";
		

		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		CCI.Name = "CCI";
		CCI.DrawStyle = DRAWSTYLE_BAR;
		CCI.PrimaryColor = RGB(0,255,0);
		CCI.DrawZeros= true;

		Line1.Name = "Line 1";
		Line1.DrawStyle = DRAWSTYLE_HIDDEN;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros= true;

		Line2.Name = "Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = false;

		Line3.Name = "Line 3";
		Line3.DrawStyle = DRAWSTYLE_LINE;
		Line3.PrimaryColor = RGB(255,127,0);
		Line3.DrawZeros = false;


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

		I_MAType.Name = "Moving Average Type";
		I_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		I_Version.SetInt(1);

		return;
	}


	if(I_Version.GetInt() < 1)
	{
		I_Version.SetInt(1);
		I_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
	}

	sc.DataStartIndex = I_Length.GetInt();

	float Multiplier = I_Multiplier.GetFloat();

	if(Multiplier == 0.0f)
		Multiplier = 0.015f;

	sc.CCI(sc.BaseDataIn[I_InputData.GetInputDataIndex()], CCI, sc.Index, I_Length.GetInt(), Multiplier, I_MAType.GetMovAvgType());

	Line2[sc.Index] = I_Line2Value.GetFloat();
	Line3[sc.Index] = I_Line3Value.GetFloat();
} 


/*============================================================================*/
SCSFExport scsf_TradeVolumeIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TVI = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	

	if (sc.SetDefaults)
	{
		sc.GraphName = "Trade Volume Index";

		sc.ValueFormat = 0;
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;
		
		TVI.Name = "TVI";
		TVI.DrawStyle = DRAWSTYLE_LINE;
		TVI.PrimaryColor = RGB(0,255,0);
		TVI.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);


		return;
	}

	sc.DataStartIndex = 1;

	if(sc.Index < 1)
		return;

	int Index = sc.Index;

	float Diff = sc.BaseDataIn[InputData.GetInputDataIndex()][Index] - sc.BaseDataIn[InputData.GetInputDataIndex()][Index-1];


	if(sc.FormattedEvaluate((float) Diff,sc.BaseGraphValueFormat,GREATER_OPERATOR,sc.TickSize ,sc.BaseGraphValueFormat) != 0)
		TVI[Index] = TVI[Index-1] + sc.Volume[Index];
	else 	if(sc.FormattedEvaluate((float) Diff,sc.BaseGraphValueFormat,LESS_OPERATOR,sc.TickSize*-1 ,sc.BaseGraphValueFormat) != 0)
		TVI[Index] = TVI[Index-1] - sc.Volume[Index];
	else 
		TVI[Index] = TVI[Index-1];
}

/**************************************************************
*
*		Demand Index
*
*		Array Number Usage
*
*			Array 0 - Demand Index
*			Array 1 - Demand Index Moving Average
*			Array 2 - Zero Line
*			Array 3 - Moving Average Sell Power
*			Array 4 - High + Low + 2 * Last
*			Array 5 - Max of last 2 High's - Min of last 2 Low's
*			Array 6 - EMA of Array 5
*			Array 7 - EMA of Volume
*			Array 8 - BuyPower
*			Array 9 - SellPower
*			Array 10 - Moving Average Buy Power
*			
***************************************************************/
SCSFExport scsf_DemandIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OutDemandIndex = sc.Subgraph[0];  // Demand Index
	SCSubgraphRef OutDemandIndexMA = sc.Subgraph[1];  // Demand Index Moving Average
	SCSubgraphRef OutZeroLine = sc.Subgraph[2];  // Zero Line

	SCFloatArrayRef ArrHL2C = sc.Subgraph[0].Arrays[0];  // High + Low + 2 * Last
	SCFloatArrayRef ArrRange = sc.Subgraph[0].Arrays[1];  // Max of last 2 High's - Min of last 2 Low's
	SCFloatArrayRef ArrRangeMA = sc.Subgraph[1].Arrays[1];  // EMA of Array 5
	SCFloatArrayRef ArrVolumeMA = sc.Subgraph[1].Arrays[0];  // EMA of Volume
	SCFloatArrayRef ArrBuyPower = sc.Subgraph[0].Arrays[2];  // BuyPower
	SCFloatArrayRef ArrSellPower = sc.Subgraph[0].Arrays[3];  // SellPower
	SCFloatArrayRef ArrMABuyPower = sc.Subgraph[1].Arrays[2];  // Moving Average Buy Power
	SCFloatArrayRef ArrMASellPower = sc.Subgraph[1].Arrays[3];  // Moving Average Sell Power

	SCInputRef BSPowerLength = sc.Input[0];
	SCInputRef BSPowerMovAvgLength = sc.Input[1];
	SCInputRef DIMovAvgLength = sc.Input[2];
	SCInputRef DIMovAvgType = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Demand Index";
		
		sc.AutoLoop = 1;
		
		OutDemandIndex.Name	= "DI";
		OutDemandIndex.PrimaryColor	= RGB(0,255,0);
		OutDemandIndex.DrawStyle = DRAWSTYLE_LINE;
		OutDemandIndex.LineWidth = 1;
		OutDemandIndex.DrawZeros = true;
		
		OutDemandIndexMA.Name = "DI MovAvg";
		OutDemandIndexMA.PrimaryColor = RGB(255,125,125);
		OutDemandIndexMA.DrawStyle = DRAWSTYLE_LINE;
		OutDemandIndexMA.LineWidth = 1;
		OutDemandIndexMA.DrawZeros = true;
		
		OutZeroLine.Name = "Line";
		OutZeroLine.PrimaryColor = RGB(125,125,125);
		OutZeroLine.DrawStyle = DRAWSTYLE_LINE;
		OutZeroLine.LineWidth = 1;
		OutZeroLine.DrawZeros = true;
		
		BSPowerLength.Name = "Buy/Sell Power Length";
		BSPowerLength.SetInt(19);
		BSPowerLength.SetIntLimits(1, 100);
		
		BSPowerMovAvgLength.Name = "Buy/Sell Power Moving Average Length";
		BSPowerMovAvgLength.SetInt(19);
		BSPowerMovAvgLength.SetIntLimits(1, 100);
		
		DIMovAvgLength.Name = "Demand Index Moving Average Length";
		DIMovAvgLength.SetInt(30);
		DIMovAvgLength.SetIntLimits(1, 100);
		
		DIMovAvgType.Name = "Demand Index Moving Average Type";
		DIMovAvgType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		
		return;
	}
	
	ArrHL2C[sc.Index] = sc.High[sc.Index] + sc.Low[sc.Index] + 2 * sc.Close[sc.Index];
	
	if (sc.Index == 0)
	{
		ArrRange[0] = sc.High[0] - sc.Low[0];
	}
	else
	{
		ArrRange[sc.Index]
			= max(sc.High[sc.Index], sc.High[sc.Index - 1])
			- min(sc.Low[sc.Index], sc.Low[sc.Index - 1]);
	}
	
	//
	sc.MovingAverage(ArrRange, ArrRangeMA, DIMovAvgType.GetMovAvgType(), BSPowerLength.GetInt());
	sc.MovingAverage(sc.Volume, ArrVolumeMA, DIMovAvgType.GetMovAvgType(), BSPowerLength.GetInt());
	
	//
	if (sc.Index >= 1)
	{
		ArrBuyPower[sc.Index] = sc.Volume[sc.Index] / ArrVolumeMA[sc.Index];
		ArrSellPower[sc.Index] = sc.Volume[sc.Index] / ArrVolumeMA[sc.Index];
		
		if (ArrHL2C[sc.Index] < ArrHL2C[sc.Index - 1])
		{
			ArrBuyPower[sc.Index]
				= ArrBuyPower[sc.Index]
				/ exp(
					(
						0.375f
						* (ArrHL2C[sc.Index] + ArrHL2C[sc.Index - 1])
						/ ArrRangeMA[0]
					)
					* (ArrHL2C[sc.Index - 1] - ArrHL2C[sc.Index])
					/ ArrHL2C[sc.Index]
				);
		}
		else if (ArrHL2C[sc.Index] > ArrHL2C[sc.Index - 1])
		{
			ArrSellPower[sc.Index]
				= ArrSellPower[sc.Index]
				/ exp(
					(
						0.375f
						* (ArrHL2C[sc.Index] + ArrHL2C[sc.Index - 1])
						/ ArrRangeMA[0]
					)
					* (ArrHL2C[sc.Index] - ArrHL2C[sc.Index - 1])
					/ ArrHL2C[sc.Index - 1]
				);
		}
	}
	
	//
	sc.MovingAverage(ArrBuyPower, ArrMABuyPower, DIMovAvgType.GetMovAvgType(), BSPowerMovAvgLength.GetInt());
	sc.MovingAverage(ArrSellPower, ArrMASellPower, DIMovAvgType.GetMovAvgType(), BSPowerMovAvgLength.GetInt());
	
	//
	float divsor, divend;
	if (ArrMABuyPower[sc.Index] > ArrMASellPower[sc.Index])
	{
		divsor = ArrMABuyPower[sc.Index];
		divend = ArrMASellPower[sc.Index];
	}
	else if (ArrMABuyPower[sc.Index] < ArrMASellPower[sc.Index])
	{
		divsor = ArrMASellPower[sc.Index];
		divend = ArrMABuyPower[sc.Index];
	}
	else
	{
		divsor = ArrMASellPower[sc.Index];
		divend = ArrMASellPower[sc.Index];
	}
	
	float var2 = 1 - (divend/divsor);
	float var3 = var2;
	if (ArrMASellPower[sc.Index] > ArrMABuyPower[sc.Index])
		var3 *= -1;
	
	OutDemandIndex[sc.Index] = var3 * 100;
	OutZeroLine[sc.Index] = 0; 
	
	//
	sc.MovingAverage(OutDemandIndex, OutDemandIndexMA, MOVAVGTYPE_SIMPLE, DIMovAvgLength.GetInt());
}

/*============================================================================*/
SCSFExport scsf_BollingerBandBandwidth(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BBBandWidth = sc.Subgraph[0];
	SCSubgraphRef OutBollingerBands = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];


	SCInputRef MovingAverageType = sc.Input[1];

	SCInputRef Length = sc.Input[3];
	SCInputRef StandardDeviations = sc.Input[4];
	SCInputRef Version = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bollinger Bands: Bandwidth";
		sc.ValueFormat = 3;
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;
		
		BBBandWidth.Name = "BollingerBands: Bandwidth";
		BBBandWidth.DrawStyle = DRAWSTYLE_LINE;
		BBBandWidth.PrimaryColor = RGB(0,255,0);
		BBBandWidth.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HLC_AVG);

		MovingAverageType.Name = "Moving Average Type";
		MovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		
		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);
		StandardDeviations.SetFloatLimits(0.00001f,(float)MAX_STUDY_LENGTH);

		Version.SetInt(1);

		return;
	}

	if (Version.GetInt() < 1)
	{
		MovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		Version.SetInt(1);
	}
	
	int i = sc.Index;

	sc.DataStartIndex=Length.GetInt()-1;
	
	sc.BollingerBands(sc.BaseDataIn[InputData.GetInputDataIndex()], OutBollingerBands, Length.GetInt(), StandardDeviations.GetFloat(),MovingAverageType.GetMovAvgType());

	SCFloatArrayRef MiddleBand	= OutBollingerBands;  
	SCFloatArrayRef UpperBand	= OutBollingerBands.Arrays[0];  
	SCFloatArrayRef LowerBand	= OutBollingerBands.Arrays[1];  

	BBBandWidth[i] = (UpperBand[i] - LowerBand[i]) / MiddleBand[i];
}


/*============================================================================*/
SCSFExport scsf_BollingerBandsPercentB(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentB = sc.Subgraph[0];
	SCSubgraphRef OutBollingerBands	= sc.Subgraph[1];
	SCSubgraphRef UpperThreshold = sc.Subgraph[2];
	SCSubgraphRef LowerThreshold = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MovAvgType = sc.Input[1];
	SCInputRef Length = sc.Input[3];
	SCInputRef StandardDeviations = sc.Input[4];
	SCInputRef BaseBand = sc.Input[5];
	SCInputRef UpperThresholdValue = sc.Input[6];
	SCInputRef LowerThresholdValue = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bollinger Bands %B";
		sc.ValueFormat = 3;
		sc.GraphRegion = 1;
		sc.AutoLoop = 1;
		
		PercentB.Name	= "%B";
		PercentB.DrawStyle = DRAWSTYLE_LINE;
		PercentB.PrimaryColor = RGB(0,255,0);
		PercentB.DrawZeros = true;

		UpperThreshold.Name	= "Upper Threshold";
		UpperThreshold.DrawStyle = DRAWSTYLE_LINE;
		UpperThreshold.LineStyle = LINESTYLE_DASH;
		UpperThreshold.PrimaryColor = COLOR_GRAY;
		UpperThreshold.DrawZeros = false;

		LowerThreshold.Name	= "Lower Threshold";
		LowerThreshold.DrawStyle = DRAWSTYLE_LINE;
		LowerThreshold.LineStyle = LINESTYLE_DASH;
		LowerThreshold.PrimaryColor = COLOR_GRAY;
		LowerThreshold.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HLC_AVG);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		StandardDeviations.Name = "Standard Deviations";
		StandardDeviations.SetFloat(2.0f);

		BaseBand.Name = "Base Band";
		BaseBand.SetCustomInputStrings( "Lower Band;Middle Band");
		BaseBand.SetCustomInputIndex(0);

		UpperThresholdValue.Name = "Upper Line Threshold Value";
		UpperThresholdValue.SetFloat(0);

		LowerThresholdValue.Name = "Lower Line Threshold Value";
		LowerThresholdValue.SetFloat(0);

		return;
	}

	int Index = sc.Index;

	sc.DataStartIndex= Length.GetInt() -1;

	// Calculate Bollinger bands
	sc.BollingerBands(sc.BaseDataIn[InputData.GetInputDataIndex()], 
		OutBollingerBands, Length.GetInt(), StandardDeviations.GetFloat(), MovAvgType.GetMovAvgType());

	SCFloatArrayRef MiddleBand	= OutBollingerBands;  
	SCFloatArrayRef UpperBand	= OutBollingerBands.Arrays[0];  
	SCFloatArrayRef LowerBand	= OutBollingerBands.Arrays[1];  

	//Standard calculation: %B = (Price - Bottom Band)/(Top Band - Bottom Band)

	if (BaseBand.GetIndex()== 1)
	{
		float BandsDifference= UpperBand[Index] - MiddleBand[Index];
		if(BandsDifference)
			PercentB[Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][Index] - MiddleBand[Index]) / BandsDifference;
		else
			PercentB[Index] = 0.0;
	}
	else
	{
		

		float BandsDifference= UpperBand[Index] - LowerBand[Index];
		if(BandsDifference)
			PercentB[Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][Index] - LowerBand[Index]) / BandsDifference;
		else
			PercentB[Index] = 0.0;
	}

	UpperThreshold[Index] = UpperThresholdValue.GetFloat();
	LowerThreshold[Index] = LowerThresholdValue.GetFloat();
}

/*============================================================================*/
SCSFExport scsf_DifferenceBar(SCStudyInterfaceRef sc)
{
	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];
	SCInputRef Chart1Addition = sc.Input[6];
	SCInputRef ZeroOutputWhenNonExactDateTimeMatch = sc.Input[7];
	SCInputRef UseLatestSourceDataForLastBar = sc.Input[8];
	SCInputRef ZeroOutputWhenOneSourceGraphHasZeroValues = sc.Input[9]; 

	if (sc.SetDefaults)
	{
		sc.GraphName			= "Difference (Bar)";
		sc.ValueFormat			= VALUEFORMAT_INHERITED;
		sc.GraphRegion			= 1;
		sc.GraphUsesChartColors = 1;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;
		
		// We are using Manual looping.
		sc.AutoLoop = false;
		
		for (int SubgraphIndex = 0; SubgraphIndex <= SC_ASKVOL; ++SubgraphIndex)
		{
			sc.Subgraph[SubgraphIndex].Name = sc.GetStudySubgraphName(0, SubgraphIndex);
			sc.Subgraph[SubgraphIndex].DrawZeros = false;
			if (SubgraphIndex < SC_VOLUME)
				sc.Subgraph[SubgraphIndex].DrawStyle = DRAWSTYLE_LINE;
			else
				sc.Subgraph[SubgraphIndex].DrawStyle = DRAWSTYLE_IGNORE;

			if (SubgraphIndex == SC_LAST)
			{
				sc.Subgraph[SubgraphIndex].LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_VALUES_SCALE | LL_VALUE_ALIGN_CENTER;
			}
		}


		Chart2Number.Name = "Chart 2 Number and Graph";
		Chart2Number.SetChartStudyValues(1, 0);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);
		
		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		Chart1Addition.Name = "Chart 1 Addition";
		Chart1Addition.SetFloat(0);

		ZeroOutputWhenNonExactDateTimeMatch.Name = "Zero Output When Non Exact Date-Time Match";
		ZeroOutputWhenNonExactDateTimeMatch.SetYesNo(false);

		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(0);

		ZeroOutputWhenOneSourceGraphHasZeroValues.Name = "Zero Output When one Source Graph has Zero Values";
		ZeroOutputWhenOneSourceGraphHasZeroValues.SetYesNo(0);

		return;
	}

	int LastProcessedSourceBarIndex = sc.GetPersistentInt(1);

	if (sc.IsFullRecalculation )
	{
		// Create text indicating both charts and set to sc.GraphName
		SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
		SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), Chart2Number.GetStudyID());
		sc.GraphName.Format("Difference %s * %g - %s * %g", Chart1Name.GetChars(), Chart1Multiplier.GetFloat(), Chart2Name.GetChars(), Chart2Multiplier.GetFloat());

		LastProcessedSourceBarIndex = 0;
	}


	// Obtain a reference to the Base Data in the specified chart. This call is relatively efficient, but it should be called as minimally as possible. To reduce the number of calls we have it outside of the primary "for" loop in this study function. And we also use Manual Looping by using sc.AutoLoop = 0. In this way, sc.GetChartBaseData / sc.GetStudyArraysFromChartUsingID is called only once per call to this study function and there is a minimal number of calls to this study function. sc.GetChartBaseData is a function to get all of the Base Data arrays with one efficient call.
	SCGraphData Chart2BaseData;
	//sc.GetChartBaseData(-Chart2Number.GetChartNumber(), Chart2BaseData);
	sc.GetStudyArraysFromChartUsingID(-Chart2Number.GetChartNumber(), Chart2Number.GetStudyID(), Chart2BaseData);

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	if (!sc.IsFullRecalculation)
	{
		SCDateTimeArray Chart2DateTimeArray;
		sc.GetChartDateTimeArray(Chart2Number.GetChartNumber(), Chart2DateTimeArray);

		int DestinationStartIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, Chart2DateTimeArray[LastProcessedSourceBarIndex]);

		if (DestinationStartIndex < CalculationStartIndex)
			CalculationStartIndex = DestinationStartIndex;
	}


	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = 0;

		if(ZeroOutputWhenNonExactDateTimeMatch.GetYesNo() == false)
		{
			Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);
			
			//When 'use latest source data for last bar' is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source chart.
			if(UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() && Index == sc.ArraySize - 1)
			{
				Chart2Index = Chart2BaseData[0].GetArraySize() - 1;
			}

			LastProcessedSourceBarIndex = Chart2Index;

		}
		else
		{
	
			Chart2Index = sc.GetExactMatchForSCDateTime(Chart2Number.GetChartNumber(), sc.BaseDateTimeIn[Index]);

			LastProcessedSourceBarIndex = Chart2Index;

			if(Chart2Index == -1)
			{
				for (int SubgraphIndex = 0; SubgraphIndex <= SC_HL_AVG; SubgraphIndex++)
				{
					sc.Subgraph[SubgraphIndex][Index] = 0.0;
				}
				continue;
			}
		}

		if (ZeroOutputWhenOneSourceGraphHasZeroValues.GetYesNo())
		{
			bool AllZeros = true;
			for (int SubgraphIndex = 0; SubgraphIndex <= SC_LAST; SubgraphIndex++)
			{
				if (Chart2BaseData[SubgraphIndex][Chart2Index] != 0)
				{
					AllZeros = false;
					break;
				}
			}

			if (AllZeros)
				continue;

			for (int SubgraphIndex = 0; SubgraphIndex <= SC_LAST; SubgraphIndex++)
			{
				if (sc.BaseDataIn[SubgraphIndex][Index] != 0)
				{
					AllZeros = false;
					break;
				}
			}

			if (AllZeros)
				continue;
		}

		for (int SubgraphIndex = 0; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
			= (sc.BaseDataIn[SubgraphIndex][Index] * Chart1Multiplier.GetFloat() + Chart1Addition.GetFloat())
				- (Chart2BaseData[SubgraphIndex][Chart2Index] * Chart2Multiplier.GetFloat());
		}

		sc.Subgraph[SC_HIGH][Index] = max(sc.Subgraph[SC_OPEN][Index],
			max(sc.Subgraph[SC_HIGH][Index],
			max(sc.Subgraph[SC_LOW][Index],sc.Subgraph[SC_LAST][Index])
			)
			);

		sc.Subgraph[SC_LOW][Index] = min(sc.Subgraph[SC_OPEN][Index],
			min(sc.Subgraph[SC_HIGH][Index],
			min(sc.Subgraph[SC_LOW][Index],sc.Subgraph[SC_LAST][Index])
			)
			);

		sc.CalculateOHLCAverages(Index);
	}
	
	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;
}

/*============================================================================*/
SCSFExport scsf_DifferenceBidAskPrices(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_BidPrice = sc.Subgraph[0];
	SCSubgraphRef Subgraph_AskPrice = sc.Subgraph[1];

	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];
	SCInputRef Chart1Addition = sc.Input[6];
	SCInputRef ZeroOutputWhenNonExactDateTimeMatch = sc.Input[7];
	SCInputRef UseLatestSourceDataForLastBar = sc.Input[8];
	SCInputRef ZeroOutputWhenOneSourceGraphHasZeroValues = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Difference (Bid and Ask Prices)";
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 1;

		sc.MaintainAdditionalChartDataArrays = true;// needed to maintain bid and ask arrays in the base graph

		// Using Manual looping.
		sc.AutoLoop = false;

		Subgraph_BidPrice.Name = "Bid";
		Subgraph_BidPrice.DrawZeros = false;
		Subgraph_BidPrice.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BidPrice.LineWidth = 2;
		Subgraph_BidPrice.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_VALUES_SCALE | LL_VALUE_ALIGN_CENTER;

		Subgraph_AskPrice.Name = "Ask";
		Subgraph_AskPrice.DrawZeros = false;
		Subgraph_AskPrice.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_AskPrice.LineWidth = 2;
		Subgraph_AskPrice.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_VALUES_SCALE | LL_VALUE_ALIGN_CENTER;


		Chart2Number.Name = "Chart 2 Number and Graph";
		Chart2Number.SetChartStudyValues(1, 0);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		Chart1Addition.Name = "Chart 1 Addition";
		Chart1Addition.SetFloat(0);

		ZeroOutputWhenNonExactDateTimeMatch.Name = "Zero Output When Non Exact Date-Time Match";
		ZeroOutputWhenNonExactDateTimeMatch.SetYesNo(false);

		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(0);

		ZeroOutputWhenOneSourceGraphHasZeroValues.Name = "Zero Output When one Source Graph has Zero Values";
		ZeroOutputWhenOneSourceGraphHasZeroValues.SetYesNo(0);

		return;
	}

	if (sc.IsFullRecalculation)
	{
		// Create text indicating both charts and set to sc.GraphName
		SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
		SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), Chart2Number.GetStudyID());
		sc.GraphName.Format("Difference %s * %g - %s * %g", Chart1Name.GetChars(), Chart1Multiplier.GetFloat(), Chart2Name.GetChars(), Chart2Multiplier.GetFloat());

	}

	// Obtain a reference to the Base Data in the specified chart. 
	SCGraphData Chart2BaseData;
	sc.GetStudyArraysFromChartUsingID(-Chart2Number.GetChartNumber(), Chart2Number.GetStudyID(), Chart2BaseData);

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = 0;

		if (ZeroOutputWhenNonExactDateTimeMatch.GetYesNo() == false)
		{
			Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);

			//When 'use latest source data for last bar' is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source chart.
			if (UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() && Index == sc.ArraySize - 1)
				Chart2Index = Chart2BaseData[0].GetArraySize() - 1;

		}
		else
		{

			Chart2Index = sc.GetExactMatchForSCDateTime(Chart2Number.GetChartNumber(), sc.BaseDateTimeIn[Index]);

			if (Chart2Index == -1)
			{
				Subgraph_BidPrice[Index] = 0;
				Subgraph_AskPrice[Index] = 0;
				continue;
			}
		}

		if (ZeroOutputWhenOneSourceGraphHasZeroValues.GetYesNo())
		{
			bool AllZeros = true;
			for (int SubgraphIndex = SC_BID_PRICE; SubgraphIndex <= SC_ASK_PRICE; SubgraphIndex++)
			{
				if (Chart2BaseData[SubgraphIndex][Chart2Index] != 0)
				{
					AllZeros = false;
					break;
				}
			}

			if (AllZeros)
				continue;

			for (int SubgraphIndex = SC_BID_PRICE; SubgraphIndex <= SC_ASK_PRICE; SubgraphIndex++)
			{
				if (sc.BaseDataIn[SubgraphIndex][Index] != 0)
				{
					AllZeros = false;
					break;
				}
			}

			if (AllZeros)
				continue;
		}

		for (int SubgraphIndex = 0; SubgraphIndex <= 1; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
				= (sc.BaseDataIn[SubgraphIndex == 0 ? SC_BID_PRICE : SC_ASK_PRICE][Index] * Chart1Multiplier.GetFloat() + Chart1Addition.GetFloat())
				- (Chart2BaseData[SubgraphIndex == 0 ? SC_BID_PRICE : SC_ASK_PRICE][Chart2Index] * Chart2Multiplier.GetFloat());
		}


	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;
}

/*============================================================================*/
SCSFExport scsf_HighLowForTimePeriodExtendedLines(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighOfDay = sc.Subgraph[0];
	SCSubgraphRef LowOfDay = sc.Subgraph[1];

	SCInputRef Input_StartTime = sc.Input[0];
	SCInputRef Input_EndTime = sc.Input[1];
	SCInputRef Input_NumberDaysToCalculate = sc.Input[2];
	SCInputRef Input_UseLineStopTime = sc.Input[3];
	SCInputRef Input_Version = sc.Input[4];
	SCInputRef Input_LineStopTimeInput = sc.Input[5];
	SCInputRef Input_DisplayHighLowIncrementally = sc.Input[6];

	SCInputRef Input_InputDataHigh = sc.Input[7];
	SCInputRef Input_InputDataLow = sc.Input[8];
	SCInputRef Input_FridayEveningExtendsIntoSunday = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName			= "High/Low for Time Period - Extended";
		sc.GraphRegion			= 0;
		sc.AutoLoop				= 0;
		
		HighOfDay.Name = "High";
		HighOfDay.DrawStyle = DRAWSTYLE_STAIR_STEP;
		HighOfDay.PrimaryColor = RGB(0,255,0);
		HighOfDay. DrawZeros = false;
		
		LowOfDay.Name = "Low";
		LowOfDay.DrawStyle = DRAWSTYLE_STAIR_STEP;
		LowOfDay.PrimaryColor = RGB(255,0,255);
		LowOfDay.DrawZeros = false;

		Input_StartTime.Name = "Start Time";
		Input_StartTime.SetTime(0);

		Input_EndTime.Name = "End Time";
		Input_EndTime.SetTime(SECONDS_PER_DAY - 1);
	
		Input_NumberDaysToCalculate.Name= "Number Of Days To Calculate";
		Input_NumberDaysToCalculate.SetInt(120);

		Input_UseLineStopTime.Name = "Use Line Stop Time";
		Input_UseLineStopTime.SetYesNo(false);

		Input_LineStopTimeInput.Name = "Line Stop Time";
		Input_LineStopTimeInput.SetTime(SECONDS_PER_DAY - 1);

		Input_DisplayHighLowIncrementally.Name = "Display High Low Incrementally";
		Input_DisplayHighLowIncrementally.SetYesNo( true);

		Input_InputDataHigh.Name = "Input Data High";
		Input_InputDataHigh.SetInputDataIndex(SC_HIGH);

		Input_InputDataLow.Name = "Input Data Low";
		Input_InputDataLow.SetInputDataIndex(SC_LOW);

		Input_FridayEveningExtendsIntoSunday.Name = "Friday Evening Extends Into Sunday";
		Input_FridayEveningExtendsIntoSunday.SetYesNo(false);

		Input_Version.SetInt(2);

		return;
	}

	if (Input_Version.GetInt() == 1)
	{

		Input_InputDataHigh.SetInputDataIndex(SC_HIGH);
		Input_InputDataLow.SetInputDataIndex(SC_LOW);

		Input_Version.SetInt(2);
	}

	if (Input_NumberDaysToCalculate.GetInt() <= 0)
		Input_NumberDaysToCalculate.SetInt(10000);

	Input_StartTime.GetDateTime();
	Input_EndTime.GetDateTime();


	if (Input_StartTime.GetTime() <= Input_EndTime.GetTime()
		
		&&  (Input_StartTime.GetTime() < sc.StartTimeOfDay && Input_EndTime.GetTime() > sc.StartTimeOfDay)
		)
	{
		Input_StartTime.SetTime(sc.StartTimeOfDay);

		//Check if the start time is now greater than or equal to the end time in which case move the end time forward by 30 minutes from the start time.
		if (Input_StartTime.GetTime() >= Input_EndTime.GetTime())
		{ 
			Input_EndTime.SetTime(Input_StartTime.GetTime() + 30 * 60);
		}
	}
	
	float & HighOfPeriod = sc.GetPersistentFloat(1);
	float & LowOfPeriod = sc.GetPersistentFloat(2);

	SCDateTime DaysToCalculateStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.ArraySize - 1]) - (Input_NumberDaysToCalculate.GetInt() - 1);

	// Loop through chart bars starting at the Update Start Index
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++ )
	{

		SCDateTime CurrentBarDateTime = sc.BaseDateTimeIn[Index];

		SCDateTime PreviousBarDateTime = sc.BaseDateTimeIn[max(0, Index - 1)];

		SCDateTime StartDateTimeOfCurrentIndex = sc.GetTradingDayStartDateTimeOfBar(CurrentBarDateTime);
		SCDateTime StartDateTimeOfPreviousIndex = sc.GetTradingDayStartDateTimeOfBar(PreviousBarDateTime);

		bool NeedReset = StartDateTimeOfCurrentIndex != StartDateTimeOfPreviousIndex
			|| Index == 0;

		if (sc.BaseDateTimeIn[Index] < DaysToCalculateStartDateTime)
			continue;

		SCDateTime StartDateTime ;
		SCDateTime EndDateTime ;

		StartDateTime =  CurrentBarDateTime.GetDate();
		EndDateTime = CurrentBarDateTime.GetDate();

		StartDateTime.SetTime(Input_StartTime.GetTime());
		EndDateTime.SetTime(Input_EndTime.GetTime());

		bool TimesAreReversed = false;
		if (Input_StartTime.GetTime() > Input_EndTime.GetTime())
		{
			TimesAreReversed = true;
			if (CurrentBarDateTime.GetTimeInSeconds() < Input_StartTime.GetTime())
			{
				StartDateTime -= DAYS;
			}
			else
			{
				EndDateTime += DAYS;
			}
		}

		if(TimesAreReversed && Input_FridayEveningExtendsIntoSunday.GetYesNo())
		{
			SCDateTime TradingDayDate(sc.GetTradingDayDate(CurrentBarDateTime));
			int DayOfWeek = TradingDayDate.GetDayOfWeek();
			if (DayOfWeek == MONDAY)//DayOfWeek == SUNDAY ||
			{
				StartDateTime -= 2 * DAYS;
				NeedReset = false;
			}

		}

		//reset
		if (NeedReset)
		{
			HighOfPeriod = -FLT_MAX;
			LowOfPeriod = FLT_MAX;		
		}


		bool OutsideTimeRange = true;
		if (CurrentBarDateTime >= StartDateTime &&  CurrentBarDateTime <=  EndDateTime)
		{
			OutsideTimeRange = false;

			if (HighOfPeriod < sc.BaseData[Input_InputDataHigh.GetInputDataIndex()][Index])
				HighOfPeriod = sc.BaseData[Input_InputDataHigh.GetInputDataIndex()][Index];

			if (LowOfPeriod > sc.BaseData[Input_InputDataLow.GetInputDataIndex()][Index])
				LowOfPeriod = sc.BaseData[Input_InputDataLow.GetInputDataIndex()][Index];
		}


		SCDateTime NoDrawEndDateTime;
		if (Input_UseLineStopTime.GetYesNo())
		{
			NoDrawEndDateTime.SetDateTime(EndDateTime.GetDate() , Input_LineStopTimeInput.GetTime());

			if (CurrentBarDateTime >= NoDrawEndDateTime)
			{
				HighOfPeriod = -FLT_MAX;
				LowOfPeriod = FLT_MAX;
			}
		}

		// Set/update all values for current day
		int BackIndex = Index;
		while(true && HighOfPeriod != -FLT_MAX)
		{
			if(BackIndex < 0)
				break;

			SCDateTime IndexDateTime = sc.BaseDateTimeIn[BackIndex];

			if(!OutsideTimeRange   && IndexDateTime < StartDateTime)
				break;

			HighOfDay[BackIndex] = HighOfPeriod;
			LowOfDay[BackIndex] = LowOfPeriod;


			if (OutsideTimeRange || Input_DisplayHighLowIncrementally.GetYesNo())
				break;

			BackIndex--;

			if(sc.UpdateStartIndex != 0)
				sc.EarliestUpdateSubgraphDataArrayIndex = BackIndex;
		}
	}
}


/*============================================================================*/
SCSFExport scsf_HighLowForTimePeriod(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighOfDay = sc.Subgraph[0];
	SCSubgraphRef LowOfDay = sc.Subgraph[1];

	SCFloatArrayRef TimeRangeHigh = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef TimeRangeLow = sc.Subgraph[0].Arrays[1];

	SCInputRef StartTime = sc.Input[0];
	SCInputRef EndTime = sc.Input[1];
	SCInputRef Version = sc.Input[2];
	SCInputRef InputDataHigh = sc.Input[3];
	SCInputRef InputDataLow = sc.Input[4];
	SCInputRef DisplayHighLowIncrementally = sc.Input[5];
	SCInputRef NumberDaysToCalculate = sc.Input[6];


	if (sc.SetDefaults)
	{
		sc.GraphName			= "High/Low for Time Period";
		sc.GraphRegion			= 0;
		sc.AutoLoop				= 0;
		
		HighOfDay.Name = "High";
		HighOfDay.DrawStyle = DRAWSTYLE_STAIR_STEP;
		HighOfDay.PrimaryColor = RGB(0,255,0);
		HighOfDay.DrawZeros = false;
		
		LowOfDay.Name = "Low";
		LowOfDay.DrawStyle = DRAWSTYLE_STAIR_STEP;
		LowOfDay.PrimaryColor = RGB(255,0,255);
		LowOfDay.DrawZeros = false;

		StartTime.Name = "Start Time";
		StartTime.SetTime(0);

		EndTime.Name = "End Time";
		EndTime.SetTime(SECONDS_PER_DAY - 1);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		DisplayHighLowIncrementally.Name = "Display High Low Incrementally";
		DisplayHighLowIncrementally.SetYesNo(false);

		NumberDaysToCalculate.Name= "Number Of Days To Calculate";
		NumberDaysToCalculate.SetInt(10000);

		Version.SetInt(2);

		return;
	}

	if(Version.GetInt() < 2 )
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		Version.SetInt(2);
	}

	if (NumberDaysToCalculate.GetInt() <= 0)
		NumberDaysToCalculate.SetInt(10000);

	int InputDataHighIndex = InputDataHigh.GetInputDataIndex();
	int InputDataLowIndex = InputDataLow.GetInputDataIndex();

	SCDateTime InStartTime = StartTime.GetDateTime();
	SCDateTime InEndTime = EndTime.GetDateTime();

	for(int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		SCDateTime DaysToCalculateStartDateTime = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.ArraySize-1]) - ( NumberDaysToCalculate.GetInt() - 1 );

		if (sc.BaseDateTimeIn[Index]  <  DaysToCalculateStartDateTime)
			continue;

		SCDateTime StartDateTime = 0;
		SCDateTime EndDateTime = 0;


		SCDateTime BarDate = sc.BaseDateTimeIn[Index].GetDate();
		if(InStartTime < InEndTime)
		{
			StartDateTime =  BarDate + InStartTime;
			EndDateTime = BarDate + InEndTime;
		}
		//Times are reversed and the current time is greater than or equal to the start time
		else if (sc.BaseDateTimeIn[Index].GetTimeInSeconds() >= InStartTime.GetTimeInSeconds() )
		{

			StartDateTime = BarDate + InStartTime;
			EndDateTime = BarDate + 1 + InEndTime;
		}
		//Times are reversed and the current time is less than or equal to the end time
		else if (sc.BaseDateTimeIn[Index].GetTimeInSeconds() <= InEndTime.GetTimeInSeconds() )
		{

			StartDateTime = BarDate + InStartTime - 1;
			EndDateTime = BarDate + InEndTime;
		}

		//Initial calculations or start of new time range
		if(Index == 0 || sc.BaseDateTimeIn[Index - 1] < StartDateTime)
		{
			TimeRangeHigh[Index] = -FLT_MAX;
			TimeRangeLow[Index] = FLT_MAX;
		}
		else
		{
			TimeRangeHigh[Index] = TimeRangeHigh[Index - 1];
			TimeRangeLow[Index] = TimeRangeLow[Index - 1];
		}

		//Outside of range
		if(sc.BaseDateTimeIn[Index] > EndDateTime || sc.BaseDateTimeIn[Index] < StartDateTime)
			continue;


		if(sc.BaseData[InputDataHighIndex].GetArraySize() > 0)
		{
			if(TimeRangeHigh[Index] < sc.BaseData[InputDataHighIndex][Index])
				TimeRangeHigh[Index] = sc.BaseData[InputDataHighIndex][Index];
		}
		else
		{

			if(TimeRangeHigh[Index] < sc.BaseData[0][Index])
				TimeRangeHigh[Index] = sc.BaseData[0][Index];
		}

		if(sc.BaseData[InputDataLowIndex].GetArraySize() > 0)
		{
			if(TimeRangeLow[Index] > sc.BaseData[InputDataLowIndex][Index])
				TimeRangeLow[Index] = sc.BaseData[InputDataLowIndex][Index];
		}
		else
		{
			if(TimeRangeLow[Index] < sc.BaseData[0][Index])
				TimeRangeLow[Index] = sc.BaseData[0][Index];
		}

		if (DisplayHighLowIncrementally.GetYesNo())
		{

			HighOfDay[Index] = TimeRangeHigh[Index];
			LowOfDay[Index] = TimeRangeLow[Index];
		}
		else
		{
			int	BackIndex = Index;

			while(true)
			{
				if(BackIndex < 0)
					break;

				SCDateTime IndexDateTime = sc.BaseDateTimeIn[BackIndex];

				if(IndexDateTime < StartDateTime)
					break;

				HighOfDay[BackIndex] = TimeRangeHigh[Index];
				LowOfDay[BackIndex] = TimeRangeLow[Index];


				BackIndex--;

				if(sc.UpdateStartIndex != 0)
					sc.EarliestUpdateSubgraphDataArrayIndex = BackIndex;
			}
		}

	}
}


/*============================================================================*/
SCSFExport scsf_WriteBarDataToFile(SCStudyInterfaceRef sc)
{
	SCInputRef Separator = sc.Input[0];
	SCInputRef UseGMTTime = sc.Input[1];
	SCInputRef DateFormat = sc.Input[2];
	SCInputRef OutputMilliseconds = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName="Write Bar Data To File";
		sc.StudyDescription="Write Bar Data To File";

		sc.GraphRegion = 0;

		Separator.Name = "Separator";
		Separator.SetCustomInputStrings("Space;Comma;Tab");
		Separator.SetCustomInputIndex(1);

		UseGMTTime.Name = "Use GMT Time";
		UseGMTTime.SetYesNo(0);

		DateFormat.Name = "Date Format";
		DateFormat.SetCustomInputStrings("MM/DD/YYYY;YYYY/MM/DD;DD/MM/YYYY");
		DateFormat.SetCustomInputIndex(1);

		OutputMilliseconds.Name="Output Milliseconds";
		OutputMilliseconds.  SetYesNo(false);

		sc.TextInputName = "File Path";

		sc.AutoLoop= 0;//manual looping for efficiency
		return;
	}
	
	SCString OutputPathAndFileName;
	if(sc.TextInput.GetLength() > 0)
	{
		OutputPathAndFileName = sc.TextInput;
	}
	else
	{
		OutputPathAndFileName.Format("%s-BarData.txt", sc.Symbol.GetChars());
	}

	

	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	DWORD BytesWritten;
	if(sc.UpdateStartIndex == 0)
	{
		// create new file
		FileHandle = CreateFileA(OutputPathAndFileName.GetChars(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);


		// creating/opening fail
		if(FileHandle == INVALID_HANDLE_VALUE)
		{
			SCString LogMessage;
			LogMessage.Format("Write Bar Data To File study: Error creating output data file: %s. Make sure the path is valid and there are no invalid characters.", OutputPathAndFileName.GetChars());
			sc.AddMessageToLog(LogMessage.GetChars(), 1);
			return;
		}

		
		// write file header
		SCString FileHeader;			
		switch(Separator.GetInt())
		{
		case 0:
			FileHeader = "Date Time Open High Low Close Volume NumberOfTrades BidVolume AskVolume\r\n";
			break;
		case 1:
			FileHeader = "Date, Time, Open, High, Low, Close, Volume, NumberOfTrades, BidVolume, AskVolume\r\n";
			break;
		case 2:
			FileHeader = "Date\tTime\tOpen\tHigh\tLow\tClose\tVolume\tNumberOfTrades\tBidVolume\tAskVolume\r\n";
			break;
		}

		WriteFile(FileHandle, FileHeader.GetChars(), (DWORD)FileHeader.GetLength(), &BytesWritten, NULL);
	}
	else
	{
		// open existing file
		FileHandle = CreateFileA(OutputPathAndFileName.GetChars(),GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		// creating/opening fail
		if(FileHandle == INVALID_HANDLE_VALUE)
		{
			SCString LogMessage;
			LogMessage.Format("Write Bar Data To File study: Error creating output data file: %s", OutputPathAndFileName.GetChars());
			sc.AddMessageToLog(LogMessage.GetChars(), 1);
			return;
		}

		SetFilePointer(FileHandle, 0, NULL, FILE_END);
	}

	bool IsRenko = sc.AreRenkoBars() != 0;

	for (int Index=sc.UpdateStartIndex;Index<(sc.ArraySize-1);Index++)
	{
		if (sc.GetBarHasClosedStatus(Index) != BHCS_BAR_HAS_CLOSED)
			continue;			// write on close

		int Year,Month, Day;
		int hour, minute, second, millisecond;
		SCDateTimeMS BaseDateTimeIn = sc.BaseDateTimeIn[Index];

		if (UseGMTTime.GetBoolean() != 0)
			BaseDateTimeIn = sc.AdjustDateTimeToGMT(BaseDateTimeIn);

		BaseDateTimeIn.GetDateTimeYMDHMS_MS( Year, Month, Day, hour, minute, second, millisecond);

		SCString DateString;
		switch (DateFormat.GetInt())
		{
			case 0:
				DateString.Format("%02d/%02d/%d", Month, Day, Year);
			break;
			
			case 1:
				DateString.Format("%d/%02d/%02d", Year, Month, Day);
			break;
			
			case 2:
				DateString.Format("%02d/%02d/%d", Day, Month, Year);
			break;
		}

		// write bar data string to file
		int ValueFormat = sc.BaseGraphValueFormat;
			
		char MillisecondsText[32] ={0};

		if(OutputMilliseconds.GetYesNo())
			sprintf_s(MillisecondsText, ".%d", millisecond);

		const char *FormatString ;
		switch(Separator.GetInt()) 
		{
		case 0:
			FormatString = "%s %.2d:%.2d:%.2d%s %s %s %s %s %.0f %.0f %.0f %.0f\r\n";
			break;
		case 1:
			FormatString = "%s, %.2d:%.2d:%.2d%s, %s, %s, %s, %s, %.0f, %.0f, %.0f, %.0f\r\n";
			break;

		default:
		case 2:
			FormatString = "%s\t%.2d:%.2d:%.2d%s\t%s\t%s\t%s\t%s\t%.0f\t%.0f\t%.0f\t%.0f\r\n";
			break;
		}
		
		float OpenValue  = IsRenko ? sc.BaseData[SC_RENKO_OPEN][Index] : sc.Open[Index];
		float HighValue  = sc.High[Index];
		float LowValue   = sc.Low[Index];
		float CloseValue = IsRenko ? sc.BaseData[SC_RENKO_CLOSE][Index] : sc.Close[Index];

		SCString BarDataString;

		BarDataString.Format(FormatString,
			DateString.GetChars(),
			hour,
			minute,
			second,
			MillisecondsText,
			sc.FormatGraphValue(OpenValue, ValueFormat).GetChars(),
			sc.FormatGraphValue(HighValue, ValueFormat).GetChars(),
			sc.FormatGraphValue(LowValue, ValueFormat).GetChars(), 
			sc.FormatGraphValue(CloseValue, ValueFormat).GetChars(),
			sc.Volume[Index],
			sc.NumberOfTrades[Index],
			sc.BidVolume[Index],

			sc.AskVolume[Index]);

		WriteFile(FileHandle,BarDataString.GetChars(),(DWORD)BarDataString.GetLength(),&BytesWritten,NULL);

	}		
	CloseHandle(FileHandle);

	return;
}

/*============================================================================*/
SCSFExport scsf_WriteBarAndStudyDataToFile(SCStudyInterfaceRef sc)
{
	SCInputRef IncludeHiddenStudies = sc.Input[0];
	SCInputRef UseGMTTime = sc.Input[1];
	SCInputRef IncludeHiddenSubgraphs = sc.Input[2];

	if(sc.SetDefaults)
	{
		sc.GraphName="Write Bar and Study Data To File";

		sc.GraphRegion = 0;

		IncludeHiddenStudies.Name = "Include Hidden Studies";
		IncludeHiddenStudies.SetYesNo(true);

		//UseGMTTime.Name = "Use GMT Time";
		//UseGMTTime.SetYesNo(0);

		IncludeHiddenSubgraphs.Name = "Include Hidden Subgraphs";
		IncludeHiddenSubgraphs.SetYesNo(false);

		sc.TextInputName = "Path and File Name";

		sc.CalculationPrecedence = VERY_LOW_PREC_LEVEL;

		sc.AutoLoop = 0; //manual looping required
		return;
	}

	if (sc.LastCallToFunction)
		return;

	SCString OutputPathAndFileName;
	if(sc.TextInput.GetLength() > 0)
	{
		OutputPathAndFileName = sc.TextInput;
	}
	else
	{
		OutputPathAndFileName.Format("%s-BarStudyData.csv", sc.Symbol.GetChars());
	}

	//sc.WriteBarAndStudyDataToFile(sc.UpdateStartIndex, OutputPathAndFileName, IncludeHiddenStudies.GetYesNo(), IncludeHiddenSubgraphs.GetYesNo());

	n_ACSIL::s_WriteBarAndStudyDataToFile WriteBarAndStudyDataToFileParams;
	WriteBarAndStudyDataToFileParams.StartingIndex = sc.UpdateStartIndex;
	WriteBarAndStudyDataToFileParams.OutputPathAndFileName = OutputPathAndFileName;
	WriteBarAndStudyDataToFileParams.IncludeHiddenStudies = IncludeHiddenStudies.GetYesNo();
	WriteBarAndStudyDataToFileParams.IncludeHiddenSubgraphs = IncludeHiddenSubgraphs.GetYesNo();
	WriteBarAndStudyDataToFileParams.AppendOnlyToFile = 0;
	WriteBarAndStudyDataToFileParams.IncludeLastBar = 0;
	sc.WriteBarAndStudyDataToFileEx(WriteBarAndStudyDataToFileParams);

	return;
}

/*============================================================================*/
bool IsFractalBuySignal(SCFloatArrayRef InDataLow, SCFloatArrayRef InDataHigh, int Index); //Long Fractal

bool IsFractalSellSignal(SCFloatArrayRef InDataLow, int Index); //Short Fractal

SCSFExport scsf_FractalSignals(SCStudyInterfaceRef sc)
{
	enum SubgraphIndexes
	{
		INDEX_SIGNAL_BUY = 0,
		INDEX_SIGNAL_SELL		
	};

	SCSubgraphRef Buy = sc.Subgraph[0];
	SCSubgraphRef Sell = sc.Subgraph[1];

	SCInputRef SubgraphOffset = sc.Input[0];
	SCInputRef Version = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName="Bill Williams Fractals";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		Buy.Name = "Buy";
		Buy.DrawStyle = DRAWSTYLE_TRIANGLE_UP;
		Buy.PrimaryColor = RGB(0, 255, 0);
		Buy.LineWidth = 5;
		Buy.DrawZeros = false;

		Sell.Name = "Sell";
		Sell.DrawStyle = DRAWSTYLE_TRIANGLE_DOWN;
		Sell.PrimaryColor = RGB(255, 0, 0);
		Sell.LineWidth = 5;
		Sell.DrawZeros = false;

		Version.SetInt(2);

		SubgraphOffset.Name = "Arrow Offset Percentage";
		SubgraphOffset.SetFloat(2.0f);

		sc.AutoLoop = 0;

		return;
	}

	int CalculationStartIndex = max(0, sc.UpdateStartIndex - 4);
	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		float Range = sc.High[Index] - sc.Low[Index];
		float Offset = Range * (SubgraphOffset.GetFloat()*0.01f);

		// detect buy fractal signal
		if(IsFractalBuySignal(sc.Low, sc.High, Index))
			Buy[Index] = sc.High[Index] + Offset;
		else
			Buy[Index] = 0;


		// detect sell fractal signal
		if(IsFractalSellSignal(sc.Low, Index))
			Sell[Index] = sc.Low[Index] - Offset;
		else
			Sell[Index] = 0;
	}
} 

/*============================================================================*/
bool IsFractalBuySignal(SCFloatArrayRef InDataLow, SCFloatArrayRef InDataHigh, int Index)
{

	int ArraySize = InDataLow.GetArraySize();

	if (
		( Index + 2 < ArraySize
		&& Index - 2 >= 0
		&& InDataHigh[Index] > InDataHigh[Index + 1] 
		&& InDataHigh[Index + 1] > InDataHigh[Index + 2] 
		&& InDataHigh[Index] > InDataHigh[Index - 1] 
		&& InDataHigh[Index - 1] > InDataHigh[Index - 2] 
		&& InDataLow[Index] >= InDataLow[Index + 1]
		&& InDataLow[Index] >= InDataLow[Index - 1] 
		&& InDataLow[Index - 1] > InDataLow[Index - 2]
		) 
		||
		(  Index + 3 < ArraySize
		&& Index - 2 >= 0
		&& InDataHigh[Index] > InDataHigh[Index + 2] 
		&& InDataHigh[Index] > InDataHigh[Index + 3] 
		&& InDataHigh[Index] > InDataHigh[Index - 1] 
		&& InDataHigh[Index] > InDataHigh[Index - 2]
		&& InDataHigh[Index] == InDataHigh[Index + 1]
		) 
		||
		(  Index + 4 < ArraySize
		&& Index - 2 >= 0
		&& InDataHigh[Index] > InDataHigh[Index + 3] 
		&& InDataHigh[Index] > InDataHigh[Index + 4] 
		&& InDataHigh[Index] > InDataHigh[Index - 1] 
		&& InDataHigh[Index] > InDataHigh[Index - 2] 
		&& InDataHigh[Index] == InDataHigh[Index + 1] 
		&& InDataHigh[Index + 1] == InDataHigh[Index + 2]
		)
		)
		return true;

	return false;
} 

/*============================================================================*/
bool IsFractalSellSignal(SCFloatArrayRef InDataLow, int Index)
{
	int ArraySize = InDataLow.GetArraySize();

	if (
		(  Index + 2 < ArraySize
		&& Index - 2 >= 0
		&& InDataLow[Index] < InDataLow[Index + 1]
		&& InDataLow[Index + 1] < InDataLow[Index + 2]
		&& InDataLow[Index] < InDataLow[Index - 1]
		&& InDataLow[Index] < InDataLow[Index - 2] 
		) 
		||
		(  Index + 3 < ArraySize
		&& Index - 2 >= 0
		&& InDataLow[Index] < InDataLow[Index + 2]
		&& InDataLow[Index] < InDataLow[Index + 3]
		&& InDataLow[Index] < InDataLow[Index - 1]
		&& InDataLow[Index] < InDataLow[Index - 2]
		&& InDataLow[Index] == InDataLow[Index + 1]
		) 
		||
		(  Index + 4 < ArraySize
		&& Index - 2 >= 0 
		&& InDataLow[Index] < InDataLow[Index + 3] 
		&& InDataLow[Index] < InDataLow[Index + 4] 
		&& InDataLow[Index] < InDataLow[Index - 1] 
		&& InDataLow[Index] < InDataLow[Index - 2] 
		&& InDataLow[Index] == InDataLow[Index + 1] 
		&& InDataLow[Index + 1] == InDataLow[Index + 2]
		)
		)
		return true;

	return false;
}

/*============================================================================*/
SCSFExport scsf_RoundTick(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Value = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if(sc.SetDefaults)
	{
		sc.GraphName="Round Study Subgraph to Tick Size";
		sc.StudyDescription="Rounds the elements in the array from the Base Data selected by the Input Data option, to the nearest value based upon the Tick Size setting in the chart";

		Value.Name = "Value";
		Value.DrawStyle = DRAWSTYLE_LINE;
		Value.PrimaryColor = RGB(0, 0, 255);
		Value.LineWidth = 2;
		Value.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_OPEN);

		sc.AutoLoop = 1;
		return;
	}

	Value[sc.Index] = (float)sc.RoundToTickSize(sc.BaseDataIn[ InputData.GetInputDataIndex()][sc.Index], sc.TickSize);
}

/*============================================================================*/
SCSFExport scsf_MACDVolumeWeighted(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Macd		= sc.Subgraph[0];
	SCSubgraphRef MaMacd	= sc.Subgraph[1];
	SCSubgraphRef MacdDiff	= sc.Subgraph[2];
	SCSubgraphRef RefLine	= sc.Subgraph[3];

	SCInputRef FastLen		= sc.Input[0];
	SCInputRef SlowLen		= sc.Input[1];
	SCInputRef MacdLen		= sc.Input[2];
	SCInputRef InputData	= sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName="MACD-Volume Weighted";

		sc.AutoLoop					= 1;
		sc.GraphRegion				= 1;
		sc.ValueFormat				= 3;
		

		Macd.Name = "MACD";
		Macd.DrawStyle = DRAWSTYLE_LINE;
		Macd.PrimaryColor = RGB(0,255,0);
		Macd.DrawZeros = true;

		MaMacd.Name	= "MA of MACD";
		MaMacd.DrawStyle = DRAWSTYLE_LINE;
		MaMacd.PrimaryColor = RGB(255,0,255);
		MaMacd.DrawZeros = true;

		MacdDiff.Name = "MACD Diff";
		MacdDiff.DrawStyle = DRAWSTYLE_BAR;
		MacdDiff.PrimaryColor = RGB(255,255,0);
		MacdDiff.DrawZeros = true;

		RefLine.Name = "Line";
		RefLine.DrawStyle = DRAWSTYLE_LINE;
		RefLine.PrimaryColor = RGB(255,127,0);
		RefLine.DrawZeros = true;

		FastLen.Name ="Fast Moving Average Length";
		FastLen.SetInt(12);  
		FastLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		SlowLen.Name = "Slow Moving Average Length";
		SlowLen.SetInt(26);  
		SlowLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		MacdLen.Name = "MACD Moving Average Length";
		MacdLen.SetInt(9);   
		MacdLen.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		return;
	}

	sc.DataStartIndex = MacdLen.GetInt() + max(FastLen.GetInt(), SlowLen.GetInt());
	int i = sc.Index;

	SCFloatArrayRef FastOut = Macd.Arrays[0];
	sc.VolumeWeightedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], sc.Volume, FastOut, FastLen.GetInt());
	SCFloatArrayRef SlowOut = Macd.Arrays[1];
	sc.VolumeWeightedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], sc.Volume, SlowOut, SlowLen.GetInt());

	Macd[i] = FastOut[i] - SlowOut[i];

	if(i < max(SlowLen.GetInt(), FastLen.GetInt()) + MacdLen.GetInt())
		return;

	sc.MovingAverage(Macd, MaMacd, MOVAVGTYPE_EXPONENTIAL, MacdLen.GetInt());
	
	MacdDiff[i] = Macd[i] - MaMacd[i];
	RefLine[i]	= 0;		
}

/*============================================================================*/
SCSFExport scsf_StandardDeviation(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Deviation	= sc.Subgraph[0];
	SCInputRef InputData	= sc.Input[0];
	SCInputRef Length		= sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Standard Deviation";

		sc.AutoLoop		= 1;
		

		Deviation.Name = "Deviation";
		Deviation.DrawStyle = DRAWSTYLE_LINE;
		Deviation.PrimaryColor = RGB(0,255,0);
		Deviation.DrawZeros	= true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		Length.Name ="Length";
		Length.SetInt(14);  
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	sc.StdDeviation(sc.BaseDataIn[InputData.GetInputDataIndex()], Deviation, Length.GetInt());
}

/*============================================================================*/
SCSFExport scsf_AutoRetracementProjection(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PeriodHigh	= sc.Subgraph[0];
	SCSubgraphRef PeriodLow		= sc.Subgraph[1];
	const int SubgraphLevelsStartIndex = 2;
	const int NumberOfLevels = 14;

	SCInputRef TimePeriodType   = sc.Input[0];	
	SCInputRef TimePeriodLength = sc.Input[1];	
	SCInputRef PlotType			= sc.Input[2];
	SCInputRef Version			= sc.Input[17];
	SCInputRef NewPeriodDaySessionStart = sc.Input[18];
	SCInputRef CalculationReferenceValue =sc.Input[19];
	SCInputRef RoundToTickSize = sc.Input[20];
	SCInputRef ReferenceSingleSubgraph = sc.Input[21];
	SCInputRef NumberOfDaysToCalculate = sc.Input[22];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Auto Retracement/Projection";
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;//Manual looping
		PeriodHigh.Name = "Period High";
		PeriodHigh.DrawStyle = DRAWSTYLE_LINE;
		PeriodHigh.PrimaryColor = RGB(255, 165, 0);
		PeriodHigh.LineWidth = 2;
		PeriodHigh.DrawZeros = false;

		PeriodLow.Name = "Period Low";
		PeriodLow.DrawStyle = DRAWSTYLE_LINE;
		PeriodLow.PrimaryColor = RGB(255, 165, 0);
		PeriodLow.LineWidth = 2;
		PeriodLow.DrawZeros = false;

		for (int LevelIndex = 0; LevelIndex < NumberOfLevels; LevelIndex++)
		{
			SCString TmpStr;
			TmpStr.Format("Level%d", LevelIndex + 1);
			sc.Subgraph[LevelIndex + SubgraphLevelsStartIndex].Name = TmpStr;
			sc.Subgraph[LevelIndex + SubgraphLevelsStartIndex].DrawStyle = DRAWSTYLE_DASH;
			sc.Subgraph[LevelIndex + SubgraphLevelsStartIndex].LineStyle = LINESTYLE_SOLID;
			sc.Subgraph[LevelIndex + SubgraphLevelsStartIndex].LineWidth = 2;
			sc.Subgraph[LevelIndex + SubgraphLevelsStartIndex].PrimaryColor =  RGB(0, 255, 255);
			sc.Subgraph[LevelIndex + SubgraphLevelsStartIndex].DrawZeros = false;

			TmpStr.Format("Level %d Percentage .01 = 1%%", LevelIndex + 1);
			sc.Input[3 + LevelIndex].Name = TmpStr;
		}

		PlotType.Name = "Calculation Method";
		PlotType.SetCustomInputStrings("Projection;Retracement");
		PlotType.SetCustomInputIndex( 1);
		
		sc.Input[3].SetFloat(0.236f);
		sc.Input[4].SetFloat(0.382f);
		sc.Input[5].SetFloat(0.50f);
		sc.Input[6].SetFloat(0.618f);

		for (int LevelIndex = 7; LevelIndex < NumberOfLevels + 3; LevelIndex++)
			sc.Input[LevelIndex].SetFloat(0.0f);

		TimePeriodType.Name = "Time Period Unit";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);

		TimePeriodLength.Name = "Time Period Unit Length";
		TimePeriodLength.SetInt(1);

		Version.SetInt(2);

		NewPeriodDaySessionStart .Name ="New Period at Day Session Start when Using Evening Session";
		NewPeriodDaySessionStart.SetYesNo(false);

		CalculationReferenceValue.Name = "Calculation Reference Value";
		CalculationReferenceValue.SetCustomInputStrings("Automatic;Use High;Use Low");
		CalculationReferenceValue.SetCustomInputIndex(1);

		RoundToTickSize.Name = "Round To Nearest Tick Size";
		RoundToTickSize.SetYesNo(0);

		ReferenceSingleSubgraph.Name = "Reference Single Subgraph";
		ReferenceSingleSubgraph.SetYesNo(false);

		NumberOfDaysToCalculate.Name = "Number of Days to Calculate";
		NumberOfDaysToCalculate.SetInt(120);
		NumberOfDaysToCalculate.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	if (Version.GetInt() < 1)
	{
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);
		TimePeriodLength.SetInt(1);
		Version.SetInt(1);

	}

	if (Version.GetInt() < 2)
	{
		NumberOfDaysToCalculate.SetInt(1000);
		Version.SetInt(2);
	}

	float& OpenOfDay = sc.GetPersistentFloat(1);

	int TimePeriodTypeInput = TimePeriodType.GetTimePeriodType();
	int TimePeriodLengthInput = TimePeriodLength.GetInt();

	float LevelsPercentages[NumberOfLevels];
	for (int Index = 0; Index < NumberOfLevels; Index++)
	{
		LevelsPercentages[Index] = sc.Input[Index + 3].GetFloat();
	}

	int OpenSubgraph = SC_OPEN;
	int HighSubgraph = SC_HIGH;
	int LowSubgraph = SC_LOW;
	int LastSubgraph = SC_LAST;

	if (ReferenceSingleSubgraph.GetYesNo())
	{
		OpenSubgraph = SC_OPEN;
		HighSubgraph = SC_OPEN;
		LowSubgraph = SC_OPEN;
		LastSubgraph = SC_OPEN;
	}

	// Main loop
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		SCDateTime TradingDayDateForBar = sc.GetTradingDayDate(sc.BaseDateTimeIn[Index]);
		SCDateTime TradingDayDateForLastBar = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.ArraySize - 1]);

		if ((TradingDayDateForLastBar.GetDate() - TradingDayDateForBar.GetDate() + 1)
	> NumberOfDaysToCalculate.GetInt())
		{
			continue;
		}

		SCDateTime PriorCurrentPeriodStartDateTime = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index - 1], TimePeriodTypeInput, TimePeriodLengthInput, 0, NewPeriodDaySessionStart.GetYesNo());

		SCDateTime CurrentPeriodStartDateTime = sc.GetStartOfPeriodForDateTime(sc.BaseDateTimeIn[Index], TimePeriodTypeInput, TimePeriodLengthInput, 0, NewPeriodDaySessionStart.GetYesNo());

		bool IsInNewPeriod = PriorCurrentPeriodStartDateTime != CurrentPeriodStartDateTime ;

		if (IsInNewPeriod) //reset values
		{
			OpenOfDay = sc.BaseData[OpenSubgraph][Index];
			PeriodHigh[Index] = sc.BaseData[HighSubgraph][Index];
			PeriodLow[Index] = sc.BaseData[LowSubgraph][Index];
		}
		else
		{
			if (sc.BaseData[HighSubgraph][Index] > PeriodHigh[Index - 1] || Index == 0)
				PeriodHigh[Index] = sc.BaseData[HighSubgraph][Index];
			else
				PeriodHigh[Index] = PeriodHigh[Index - 1];
			
			if (sc.BaseData[LowSubgraph][Index] < PeriodLow[Index - 1] || Index == 0)
				PeriodLow[Index] = sc.BaseData[LowSubgraph][Index];
			else
				PeriodLow[Index] = PeriodLow[Index - 1];
		}

		float Range = PeriodHigh[Index] - PeriodLow[Index];

		for (int LevelIndex = 0; LevelIndex < NumberOfLevels; LevelIndex++)
		{
			if (LevelsPercentages[LevelIndex] == 0)
				continue;

			float Value = 0;

			if (PlotType.GetInt() == 1)//Retracement
			{
				//rising prices
				if ((CalculationReferenceValue.GetIndex() == 0 && sc.BaseData[LastSubgraph][Index] >= OpenOfDay) ||CalculationReferenceValue.GetIndex() == 1)
					Value = PeriodHigh[Index] - LevelsPercentages[LevelIndex] * Range;
				else
					Value = LevelsPercentages[LevelIndex] * Range + PeriodLow[Index];
			}
			else//Projection
			{
				if ((CalculationReferenceValue.GetIndex()== 0 && sc.BaseData[LastSubgraph][Index] >= OpenOfDay) ||CalculationReferenceValue.GetIndex()== 1)
					Value = LevelsPercentages[LevelIndex] * Range + PeriodHigh[Index];
				else
					Value = PeriodLow[Index] - LevelsPercentages[LevelIndex] * Range;
			}

			if (RoundToTickSize.GetYesNo())
				Value = (float)sc.RoundToTickSize(Value, sc.TickSize);

			sc.Subgraph[SubgraphLevelsStartIndex + LevelIndex][Index] = Value;
		}
	}
}


/*============================================================================*/
SCSFExport scsf_NewHighLowAlert(SCStudyInterfaceRef sc)
{

	SCInputRef HighAlert = sc.Input[0];
	SCInputRef LowAlert = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName = "New Daily High/Low Alert";

		sc.AutoLoop	= 0;
		sc.GraphRegion = 0;

		HighAlert.Name =  "High Alert Sound";
		HighAlert.SetAlertSoundNumber(2);

		LowAlert.Name  = "Low Alert Sound";
		LowAlert.SetAlertSoundNumber(2);

		return;
	}

	float & PreviousHigh = sc.GetPersistentFloat(1);
	float & PreviousLow = sc.GetPersistentFloat(2);

	SCDateTime TradingDayDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.ArraySize - 1]);

	float DailyOpen;
	float DailyHigh;
	float DailyLow;
	float DailyClose;

	if(!sc.GetOHLCForDate(TradingDayDate, DailyOpen, DailyHigh, DailyLow, DailyClose))
		return;

	if(sc.UpdateStartIndex == 0)
	{
		PreviousHigh = DailyHigh;
		PreviousLow = DailyLow;

		return;
	}

	SCString Message;
	if(HighAlert.GetAlertSoundNumber() > 0 && PreviousHigh != 0.0 && DailyHigh > PreviousHigh)
	{
		
		Message.Format("New Daily High: %f. Previous high: %f", DailyHigh, PreviousHigh);
		sc.PlaySound(HighAlert.GetAlertSoundNumber() - 1, Message);
	}

	if(LowAlert.GetAlertSoundNumber() > 0 && PreviousLow != 0.0 && DailyLow < PreviousLow)
	{
		Message.Format("New Daily Low: %f. Previous low: %f", DailyLow,  PreviousLow);
		sc.PlaySound(LowAlert.GetAlertSoundNumber() - 1 , Message);
	}

	PreviousHigh = DailyHigh;
	PreviousLow = DailyLow;

}

/*============================================================================*/
SCSFExport scsf_Spread4Chart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];


	SCInputRef Chart2Number = sc.Input[0];
	SCInputRef Chart3Number = sc.Input[1];
	SCInputRef Chart4Number = sc.Input[2];

	SCInputRef Chart1Multiplier = sc.Input[3];
	SCInputRef Chart2Multiplier = sc.Input[4];
	SCInputRef Chart3Multiplier = sc.Input[5];
	SCInputRef Chart4Multiplier = sc.Input[6];


	if (sc.SetDefaults)
	{
		sc.GraphName			= "Spread - 4 Chart";
		sc.GraphRegion			= 1;
		sc.UseGlobalChartColors = 1;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;

		// We are using Manual looping.
		sc.AutoLoop = false;

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
		Low.PrimaryColor = RGB(0,255,0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.DrawZeros = false;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = false;
		
		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = false;

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

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart3Number.Name = "Chart 3 Number";
		Chart3Number.SetChartNumber(1);

		Chart4Number.Name = "Chart 4 Number";
		Chart4Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0);

		Chart3Multiplier.Name = "Chart 3 Multiplier";
		Chart3Multiplier.SetFloat(1.0);

		Chart4Multiplier.Name = "Chart 4 Multiplier";
		Chart4Multiplier.SetFloat(1.0);
		

		return;
	}

	// Obtain a reference to the Base Data in the specified chart. This call is relatively efficient, but it should be called as minimally as possible. To reduce the number of calls we have it outside of the primary "for" loop in this study function. And we also use Manual Looping by using sc.AutoLoop = 0. In this way, sc.GetChartBaseData is called only once per call to this study function and there are minimal number of calls to the function. sc.GetChartBaseData is a new function to get all of the Base Data arrays with one efficient call.
	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-Chart2Number.GetChartNumber(), Chart2BaseData);

	SCGraphData Chart3BaseData;
	sc.GetChartBaseData(-Chart3Number.GetChartNumber(), Chart3BaseData);

	SCGraphData Chart4BaseData;
	sc.GetChartBaseData(-Chart4Number.GetChartNumber(), Chart4BaseData);

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);
		int Chart3Index = sc.GetNearestMatchForDateTimeIndex(Chart3Number.GetChartNumber(), Index);
		int Chart4Index = sc.GetNearestMatchForDateTimeIndex(Chart4Number.GetChartNumber(), Index);

		for (int SubgraphIndex = 0; SubgraphIndex < 6; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
			= (sc.BaseDataIn[SubgraphIndex][Index] * Chart1Multiplier.GetFloat()
				- Chart2BaseData[SubgraphIndex][Chart2Index] * Chart2Multiplier.GetFloat())
				- (Chart3BaseData[SubgraphIndex][Chart3Index] * Chart3Multiplier.GetFloat()
					- Chart4BaseData[SubgraphIndex][Chart4Index] * Chart4Multiplier.GetFloat());
		}

		sc.Subgraph[ SC_HIGH][Index] = max(sc.Subgraph[SC_OPEN][Index],
			max(sc.Subgraph[SC_HIGH][Index],
			max(sc.Subgraph[SC_LOW][Index],sc.Subgraph[SC_LAST][Index])
			)
			);

		sc.Subgraph[SC_LOW][Index] = min(sc.Subgraph[SC_OPEN][Index],
			min(sc.Subgraph[SC_HIGH][Index],
			min(sc.Subgraph[SC_LOW][Index],sc.Subgraph[SC_LAST][Index])
			)
			);

		sc.CalculateOHLCAverages(Index);
	}

	//SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	//SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	//sc.GraphName.Format("Difference %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*============================================================================*/


SCSFExport scsf_QStick(SCStudyInterfaceRef sc)
{
	SCSubgraphRef QStick = sc.Subgraph[0];
	SCInputRef Length = sc.Input[0];
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Q Stick";
		sc.AutoLoop = 1;
		
		QStick.Name = "QS";
		QStick.DrawStyle = DRAWSTYLE_LINE;
		QStick.LineWidth = 2;
		QStick.PrimaryColor = RGB(0,255,0);
		Length.Name = "Length";
		Length.SetInt(4);
		sc.GraphRegion = 1;

		
		return;
	}
	
	
	
	// Do data processing
	QStick.Arrays[0][sc.Index] = sc.BaseData[SC_LAST][sc.Index] - sc.BaseData[SC_OPEN][sc.Index] ;
	sc.Summation( QStick.Arrays[0], QStick.Arrays[1], Length.GetInt() );


	QStick.Data[sc.Index] = QStick.Arrays[1][sc.Index] / Length.GetInt() ;
	
	
}

/*============================================================================*/
SCSFExport scsf_HorizontalLineAtTime(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_HorizontalLine = sc.Subgraph[0];
	SCSubgraphRef  Subgraph_ExtensionLines = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];

	SCInputRef StartTime = sc.Input[1];
	SCInputRef UseStopTime = sc.Input[2];
	SCInputRef StopTime = sc.Input[3];
	SCInputRef LimitHorizontalLineFromTimeToOneDay = sc.Input[4];
	SCInputRef IgnoreWeekends = sc.Input[5];
	SCInputRef AddExtensionLinesUntilFutureIntersection = sc.Input[6];
	SCInputRef UseAllowedRangeForMatchInMinutes = sc.Input[7];
	SCInputRef AllowedRangeForMatchInMinutes = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Horizontal Line at Time";
		sc.GraphRegion = 0;
		sc.AutoLoop = 1;
		
		Subgraph_HorizontalLine.Name = "Line";
		Subgraph_HorizontalLine.DrawStyle = DRAWSTYLE_STAIR_STEP;
		Subgraph_HorizontalLine.LineWidth = 2;
		Subgraph_HorizontalLine.PrimaryColor = RGB(0, 255, 0);
		Subgraph_HorizontalLine.DrawZeros = false;

		Subgraph_ExtensionLines.Name = "Extension Lines";
		Subgraph_ExtensionLines.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_ExtensionLines.LineWidth = 2;
		Subgraph_ExtensionLines.PrimaryColor = RGB(255, 128, 0);
		Subgraph_ExtensionLines.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		StartTime.Name = "Start Time";
		StartTime.SetTime(HMS_TIME(12, 00, 0));
		
		UseStopTime.Name = "Use Stop Time";
		UseStopTime.SetYesNo(false);

		StopTime.Name = "Stop Time";
		StopTime.SetTime(HMS_TIME(23, 59, 59));

		LimitHorizontalLineFromTimeToOneDay.Name = "Limit Horizontal Line From Time To 1 Day";
		LimitHorizontalLineFromTimeToOneDay.SetYesNo(true);

		IgnoreWeekends.Name = "Ignore Weekends";
		IgnoreWeekends.SetYesNo(false);

		AddExtensionLinesUntilFutureIntersection.Name = "Add Extension Lines Until Future Intersection";
		AddExtensionLinesUntilFutureIntersection.SetYesNo(false);

		UseAllowedRangeForMatchInMinutes.Name = "Use Allowed Range for Match in Minutes";
		UseAllowedRangeForMatchInMinutes.SetYesNo(false);

		AllowedRangeForMatchInMinutes.Name = "Allowed Range for Match in Minutes";
		AllowedRangeForMatchInMinutes.SetInt(60);

		return;
	}

	float& r_PriorValue = sc.GetPersistentFloat(1);
	SCDateTime& r_StopDateTime = sc.GetPersistentSCDateTime(2);
	int & r_LastUsedBarIndexForExtensionLine = sc.GetPersistentInt(1);

	if (sc.Index == 0)
	{
		r_StopDateTime = 0;
		r_PriorValue = FLT_MIN;
		r_LastUsedBarIndexForExtensionLine = -1;
	}

	SCDateTime AllowedTimeRange = AllowedRangeForMatchInMinutes.GetInt() * MINUTES;

	SCDateTimeMS StartDateTime;
	StartDateTime.SetDateTimeAsDouble(sc.BaseDateTimeIn[sc.Index].GetDate() + StartTime.GetDateTime().GetTimeAsSCDateTimeMS());

	SCDateTime TradingDayDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.Index]);
	bool IgnoreDate = IgnoreWeekends.GetYesNo() && TradingDayDate.IsWeekend() ;

	bool IsTimeRangeMatch = !UseAllowedRangeForMatchInMinutes.GetYesNo()
		|| r_StopDateTime.IsGivenTimeWithinGivenTimeRange(sc.BaseDateTimeIn[sc.Index], AllowedTimeRange);

	if (r_StopDateTime != 0
		&& sc.BaseDateTimeIn[sc.Index] > r_StopDateTime 
		&& !IgnoreDate
		&& IsTimeRangeMatch)
		r_PriorValue = FLT_MIN;

	if (r_LastUsedBarIndexForExtensionLine != -1
		&& r_LastUsedBarIndexForExtensionLine == sc.Index)
	{
		sc.DeleteLineUntilFutureIntersection(r_LastUsedBarIndexForExtensionLine, 0);

	}

	IsTimeRangeMatch = !UseAllowedRangeForMatchInMinutes.GetYesNo()
		|| StartDateTime.IsGivenTimeWithinGivenTimeRange(sc.BaseDateTimeIn[sc.Index], AllowedTimeRange);

	//Start time encountered
	if (!IgnoreDate
		&& ((sc.Index == 0 && sc.BaseDateTimeIn[sc.Index]  == StartDateTime)
		|| (sc.BaseDateTimeIn[sc.Index - 1] < StartDateTime 
			&& sc.BaseDateTimeIn[sc.Index] >= StartDateTime) )	
		&& IsTimeRangeMatch
		)
	{
		Subgraph_HorizontalLine[sc.Index] = sc.BaseData[InputData.GetInputDataIndex()][sc.Index];
		r_PriorValue = Subgraph_HorizontalLine[sc.Index];


		if(UseStopTime.GetYesNo())
		{
			if(StartTime.GetTime() <= StopTime.GetTime())
				r_StopDateTime.SetDateTime(sc.BaseDateTimeIn[sc.Index].GetDate(), StopTime.GetTime());
			else
				r_StopDateTime.SetDateTime(sc.BaseDateTimeIn[sc.Index].GetDate() + 1, StopTime.GetTime());
		}
		else if (LimitHorizontalLineFromTimeToOneDay.GetYesNo())
		{
			SCDateTime StartDateTimeWithoutMilliseconds = StartDateTime;
			StartDateTimeWithoutMilliseconds.RoundToNearestSecond();
			r_StopDateTime = StartDateTimeWithoutMilliseconds + SCDateTime(0, HMS_TIME(23, 59, 59));
		}

		if (AddExtensionLinesUntilFutureIntersection.GetYesNo())
		{

			sc.AddLineUntilFutureIntersection
			(sc.Index
				, 0  // LineIDForBar
				, r_PriorValue
				, Subgraph_ExtensionLines.PrimaryColor
				, Subgraph_ExtensionLines.LineWidth
				, Subgraph_ExtensionLines.LineStyle
				, false
				, false
				, ""
			);

			r_LastUsedBarIndexForExtensionLine = sc.Index;
		}

	}
	
	if (r_PriorValue != FLT_MIN)
	{
		Subgraph_HorizontalLine[sc.Index] = r_PriorValue;
	}


}

/*============================================================================*/
SCSFExport scsf_BarsWithZeros(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef OpenInterest = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];

	if (sc.SetDefaults)
	{
		sc.GraphName			= "Chart Bars with Zero Values";
		sc.ValueFormat			=  VALUEFORMAT_INHERITED;
		sc.GraphRegion = 0;
		sc.DisplayAsMainPriceGraph = true;
		sc.GraphUsesChartColors = true;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;

		sc.AutoLoop = true;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.DrawZeros = true;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(255,0,255);
		High.DrawZeros = true;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,255,0);
		Low.DrawZeros = true;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,127,0);
		Last.DrawZeros = true;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);
		Volume.DrawZeros = true;

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
		OpenInterest.DrawZeros = true;

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(127,0,255);
		OHLCAvg.DrawZeros = true;

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);
		HLCAvg.DrawZeros = true;

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);
		HLAvg.DrawZeros = true;

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);
		BidVol.DrawZeros = true;

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);
		AskVol.DrawZeros = true;

		return;
	}

	for(int SubgraphIndex = 0; SubgraphIndex < SC_ASKVOL; SubgraphIndex++)
	{
		sc.Subgraph[SubgraphIndex][sc.Index] = sc.BaseData[SubgraphIndex][sc.Index];
	}
}


/*============================================================================*/
SCSFExport scsf_NumbersBarsAvgVolumePerPrice(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AvgVolumePerPrice = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName   = "Numbers Bars Avg Volume/Price Graph";
		sc.GraphRegion = 1;
		sc.ValueFormat =  VALUEFORMAT_WHOLE_NUMBER;
		sc.AutoLoop = true;

		sc.MaintainVolumeAtPriceData = 1;

		AvgVolumePerPrice.Name = "Avg Volume/Price";
		AvgVolumePerPrice.DrawStyle = DRAWSTYLE_LINE;
		AvgVolumePerPrice.PrimaryColor = RGB(0,255,255);
		AvgVolumePerPrice.DrawZeros = true;

		return;
	}


	int NumPrices = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(sc.Index);

	// Avg Vol/Price (Average Volume At Price)
	if (NumPrices== 0)
		AvgVolumePerPrice[sc.Index] = 0.0f;
	else
		AvgVolumePerPrice[sc.Index] = sc.Volume[sc.Index] / NumPrices;
}


/*============================================================================*/
SCSFExport scsf_InitialBalance(SCStudyInterfaceRef sc)
{
	SCSubgraphRef IBHExt6  = sc.Subgraph[0];
	SCSubgraphRef IBHExt5  = sc.Subgraph[1];
	SCSubgraphRef IBHExt4  = sc.Subgraph[2];
	SCSubgraphRef IBHExt3  = sc.Subgraph[3];
	SCSubgraphRef IBHExt2  = sc.Subgraph[4];
	SCSubgraphRef IBHExt1  = sc.Subgraph[5];
	SCSubgraphRef IBHigh   = sc.Subgraph[6];
	SCSubgraphRef IBMid    = sc.Subgraph[7];
	SCSubgraphRef IBLow    = sc.Subgraph[8];
	SCSubgraphRef IBLExt1  = sc.Subgraph[9];
	SCSubgraphRef IBLExt2  = sc.Subgraph[10];
	SCSubgraphRef IBLExt3  = sc.Subgraph[11];
	SCSubgraphRef IBLExt4  = sc.Subgraph[12];
	SCSubgraphRef IBLExt5  = sc.Subgraph[13];
	SCSubgraphRef IBLExt6  = sc.Subgraph[14];

	SCInputRef Input_IBType      = sc.Input[0];
	SCInputRef Input_StartTime   = sc.Input[1];
	SCInputRef Input_EndTime     = sc.Input[2];
	SCInputRef Input_NumDays     = sc.Input[3];
	SCInputRef Input_RoundExt    = sc.Input[4];
	SCInputRef Input_NumberDaysToCalculate = sc.Input[5];
	SCInputRef Input_NumberOfMinutes      = sc.Input[6];
	SCInputRef Input_StartEndTimeMethod = sc.Input[7];
	SCInputRef Input_PeriodEndAsMinutesFromSessionStart = sc.Input[8];

	SCInputRef Input_Multiplier1 = sc.Input[10];
	SCInputRef Input_Multiplier2 = sc.Input[11];
	SCInputRef Input_Multiplier3 = sc.Input[12];
	SCInputRef Input_Multiplier4 = sc.Input[13];
	SCInputRef Input_Multiplier5 = sc.Input[14];
	SCInputRef Input_Multiplier6 = sc.Input[15];

	if (sc.SetDefaults)
	{
		sc.GraphName		= "Initial Balance";
		sc.DrawZeros		= 0;
		sc.GraphRegion		= 0;
		sc.AutoLoop			= 1;

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		IBHExt6.Name = "IB High Ext 6";
		IBHExt6.PrimaryColor = RGB(0, 255, 0);
		IBHExt6.DrawStyle = DRAWSTYLE_IGNORE;
		IBHExt6.DrawZeros = false;

		IBHExt5.Name = "IB High Ext 5";
		IBHExt5.PrimaryColor = RGB(0, 255, 0);
		IBHExt5.DrawStyle = DRAWSTYLE_IGNORE;
		IBHExt5.DrawZeros = false;

		IBHExt4.Name = "IB High Ext 4";
		IBHExt4.PrimaryColor = RGB(0, 255, 0);
		IBHExt4.DrawStyle = DRAWSTYLE_IGNORE;
		IBHExt4.DrawZeros = false;

		IBHExt3.Name = "IB High Ext 3";
		IBHExt3.PrimaryColor = RGB(0, 255, 0);
		IBHExt3.DrawStyle = DRAWSTYLE_IGNORE;
		IBHExt3.DrawZeros = false;

		IBHExt2.Name = "IB High Ext 2";
		IBHExt2.PrimaryColor = RGB(0, 255, 0);
		IBHExt2.DrawStyle = DRAWSTYLE_IGNORE;
		IBHExt2.DrawZeros = false;

		IBHExt1.Name = "IB High Ext 1";
		IBHExt1.PrimaryColor = RGB(0, 255, 0);
		IBHExt1.DrawStyle = DRAWSTYLE_IGNORE;
		IBHExt1.DrawZeros = false;

		IBHigh.Name = "IB High";
		IBHigh.PrimaryColor = RGB(128, 255, 128);
		IBHigh.DrawStyle = DRAWSTYLE_DASH;
		IBHigh.DrawZeros = false;

		IBMid.Name = "IB Mid";
		IBMid.PrimaryColor = RGB(255, 255, 255);
		IBMid.DrawStyle = DRAWSTYLE_DASH;
		IBMid.DrawZeros = false;

		IBLow.Name = "IB Low";
		IBLow.PrimaryColor = RGB(255, 128, 128);
		IBLow.DrawStyle = DRAWSTYLE_DASH;
		IBLow.DrawZeros = false;

		IBLExt1.Name = "IB Low Ext 1";
		IBLExt1.PrimaryColor = RGB(255, 0, 0);
		IBLExt1.DrawStyle = DRAWSTYLE_IGNORE;
		IBLExt1.DrawZeros = false;

		IBLExt2.Name = "IB Low Ext 2";
		IBLExt2.PrimaryColor = RGB(255, 0, 0);
		IBLExt2.DrawStyle = DRAWSTYLE_IGNORE;
		IBLExt2.DrawZeros = false;

		IBLExt3.Name = "IB Low Ext 3";
		IBLExt3.PrimaryColor = RGB(255, 0, 0);
		IBLExt3.DrawStyle = DRAWSTYLE_IGNORE;
		IBLExt3.DrawZeros = false;

		IBLExt4.Name = "IB Low Ext 4";
		IBLExt4.PrimaryColor = RGB(255, 0, 0);
		IBLExt4.DrawStyle = DRAWSTYLE_IGNORE;
		IBLExt4.DrawZeros = false;

		IBLExt5.Name = "IB Low Ext 5";
		IBLExt5.PrimaryColor = RGB(255, 0, 0);
		IBLExt5.DrawStyle = DRAWSTYLE_IGNORE;
		IBLExt5.DrawZeros = false;

		IBLExt6.Name = "IB Low Ext 6";
		IBLExt6.PrimaryColor = RGB(255, 0, 0);
		IBLExt6.DrawStyle = DRAWSTYLE_IGNORE;
		IBLExt6.DrawZeros = false;


		// Inputs
		Input_IBType.Name = "Initial Balance Type";
		Input_IBType.SetCustomInputStrings("Daily;Weekly;Weekly Include Sunday;Intraday");
		Input_IBType.SetCustomInputIndex(0);

		Input_StartTime.Name = "Start Time";
		Input_StartTime.SetTime(HMS_TIME(9, 30, 0));
		
		Input_EndTime.Name = "End Time";
		Input_EndTime.SetTime(HMS_TIME(10, 29, 59));

		Input_NumDays.Name = "Weekly: Number of Days";
		Input_NumDays.SetInt(2);
		Input_NumDays.SetIntLimits(1, 5);

		Input_NumberOfMinutes.Name = "Intraday: Number of Minutes";
		Input_NumberOfMinutes.SetInt(15);

		Input_RoundExt.Name = "Round Extensions to TickSize";
		Input_RoundExt.SetYesNo(1);

		Input_NumberDaysToCalculate.Name = "Number of Days to Calculate";
		Input_NumberDaysToCalculate.SetInt(100000);
		Input_NumberDaysToCalculate.SetIntLimits(1,INT_MAX);

		Input_StartEndTimeMethod.Name = "Start End Time Method";
		Input_StartEndTimeMethod.SetCustomInputStrings("Use Start/End Time;Use Session Start Time and Minutes From Start");
		Input_StartEndTimeMethod.SetCustomInputIndex(0);

		Input_PeriodEndAsMinutesFromSessionStart.Name = "Period End As Minutes from Session Start";
		Input_PeriodEndAsMinutesFromSessionStart.SetInt(30);

		Input_Multiplier1.Name = "Extension Multiplier 1";
		Input_Multiplier1.SetFloat(.5f);
		Input_Multiplier2.Name = "Extension Multiplier 2";
		Input_Multiplier2.SetFloat(1.0f);
		Input_Multiplier3.Name = "Extension Multiplier 3";
		Input_Multiplier3.SetFloat(1.5f);
		Input_Multiplier4.Name = "Extension Multiplier 4";
		Input_Multiplier4.SetFloat(2.0f);
		Input_Multiplier5.Name = "Extension Multiplier 5";
		Input_Multiplier5.SetFloat(2.5f);
		Input_Multiplier6.Name = "Extension Multiplier 6";
		Input_Multiplier6.SetFloat(3.0f);

		return;
	}

	// Persist vars
	int&   PeriodFirstIndex = sc.GetPersistentInt(1);
	
	SCDateTime& PeriodStartDateTime = sc.GetPersistentSCDateTime(1);
	SCDateTime& PeriodEndDateTime   = sc.GetPersistentSCDateTime(2);

	float& PeriodHigh       = sc.GetPersistentFloat(1);
	float& PeriodLow        = sc.GetPersistentFloat(2);
	float& PeriodMid        = sc.GetPersistentFloat(3);
	float& PeriodHighExt1   = sc.GetPersistentFloat(4);
	float& PeriodHighExt2   = sc.GetPersistentFloat(5);
	float& PeriodHighExt3   = sc.GetPersistentFloat(6);
	float& PeriodHighExt4   = sc.GetPersistentFloat(7);
	float& PeriodHighExt5   = sc.GetPersistentFloat(8);
	float& PeriodHighExt6   = sc.GetPersistentFloat(9);
	float& PeriodLowExt1    = sc.GetPersistentFloat(10);
	float& PeriodLowExt2    = sc.GetPersistentFloat(11);
	float& PeriodLowExt3    = sc.GetPersistentFloat(12);
	float& PeriodLowExt4    = sc.GetPersistentFloat(13);
	float& PeriodLowExt5    = sc.GetPersistentFloat(14);
	float& PeriodLowExt6    = sc.GetPersistentFloat(15);

	// Reset persistent variables upon full calculation
	if (sc.Index == 0)
	{
		PeriodFirstIndex = -1;
		PeriodStartDateTime = 0;
		PeriodEndDateTime   = 0;
		PeriodHigh = -FLT_MAX;
		PeriodLow  =  FLT_MAX;
	}

	SCDateTime LastBarDateTime = sc.BaseDateTimeIn[sc.ArraySize-1];
	SCDateTime FirstCalculationDate = LastBarDateTime.GetDate() - (Input_NumberDaysToCalculate.GetInt()-1);

	SCDateTime CurrentBarDateTime = sc.BaseDateTimeIn[sc.Index];

	
	SCDateTime PrevBarDateTime;

	if (sc.Index > 0)
		PrevBarDateTime = sc.BaseDateTimeIn[sc.Index-1];

	if (CurrentBarDateTime.GetDate() < FirstCalculationDate) // Limit calculation to specified number of days back
		return;

	bool Daily  = Input_IBType.GetIndex() == 0;
	bool Weekly = Input_IBType.GetIndex() == 1 || Input_IBType.GetIndex() == 2;
	bool Intraday = Input_IBType.GetIndex() == 3;
	bool IncludeSunday = Input_IBType.GetIndex() == 2;

	// check if starting new period, reset vars
	SCDateTime StartDateTime = CurrentBarDateTime;

	if (Input_StartEndTimeMethod.GetIndex() == 0)
		StartDateTime.SetTime(Input_StartTime.GetTime());
	else
		StartDateTime.SetTime(sc.StartTimeOfDay);

	if (Weekly)
	{
		int PeriodStartDayOfWeek = IncludeSunday ? SUNDAY : MONDAY;

		int DayOfWeek = StartDateTime.GetDayOfWeek();

		if (DayOfWeek != PeriodStartDayOfWeek)
			StartDateTime += ((7 - DayOfWeek + PeriodStartDayOfWeek) * DAYS);
	}
		
	if (PrevBarDateTime < StartDateTime && CurrentBarDateTime >= StartDateTime)
	{
		PeriodFirstIndex = sc.Index;
		PeriodHigh = -FLT_MAX;
		PeriodLow  = FLT_MAX;

		PeriodStartDateTime = StartDateTime;

		PeriodEndDateTime = PeriodStartDateTime;

		if (Input_StartEndTimeMethod.GetIndex() == 0)
			PeriodEndDateTime.SetTime(Input_EndTime.GetTime());
		else
		{
			PeriodEndDateTime.SetTime(static_cast<int>(sc.StartTimeOfDay + Input_PeriodEndAsMinutesFromSessionStart.GetInt()*SECONDS_PER_MINUTE - 1*SECONDS));
		}

		if (Daily || Intraday)
		{
			if (PeriodEndDateTime <= PeriodStartDateTime)
				PeriodEndDateTime += (1 * DAYS);
		}
		else if (Weekly)
		{
			int PeriodEndDayOfWeek = IncludeSunday ? Input_NumDays.GetInt() - 1 : Input_NumDays.GetInt();

			int DayOfWeek = PeriodEndDateTime.GetDayOfWeek();

			if (DayOfWeek != PeriodEndDayOfWeek)
				PeriodEndDateTime += ((PeriodEndDayOfWeek - DayOfWeek) * DAYS);
		}
	}

	// Check end of period
	if (PeriodFirstIndex >= 0)
	{
		// Check start of new intraday period
		if (Intraday)
		{
			SCDateTime IntradayEndDateTime = PeriodStartDateTime + Input_NumberOfMinutes.GetInt() * MINUTES - 1 * SECONDS;

			if (PrevBarDateTime < IntradayEndDateTime && CurrentBarDateTime > IntradayEndDateTime)
			{
				PeriodFirstIndex = sc.Index;
				PeriodStartDateTime += Input_NumberOfMinutes.GetInt() * MINUTES;
				PeriodHigh = -FLT_MAX;
				PeriodLow  = FLT_MAX;
			}
		}

		if (CurrentBarDateTime > PeriodEndDateTime)
		{
			PeriodFirstIndex = -1;

			if (Intraday)
			{
				PeriodHigh = -FLT_MAX;
				PeriodLow  = FLT_MAX;
			}
		}
	}

	// Collecting data, back propagate if changed
	if (PeriodFirstIndex >= 0)
	{
		bool Changed = false;

		if (sc.High[sc.Index] > PeriodHigh)
		{
			PeriodHigh = sc.High[sc.Index];
			Changed = true;
		}

		if (sc.Low[sc.Index] < PeriodLow)
		{
			PeriodLow = sc.Low[sc.Index];
			Changed = true;
		}

		if (Changed)
		{
			PeriodMid = (PeriodHigh + PeriodLow) / 2.0f;

			float Range = PeriodHigh - PeriodLow;

			PeriodHighExt1 = PeriodHigh + Input_Multiplier1.GetFloat() * Range; 
			PeriodHighExt2 = PeriodHigh + Input_Multiplier2.GetFloat() * Range; 
			PeriodHighExt3 = PeriodHigh + Input_Multiplier3.GetFloat() * Range; 
			PeriodHighExt4 = PeriodHigh + Input_Multiplier4.GetFloat() * Range; 
			PeriodHighExt5 = PeriodHigh + Input_Multiplier5.GetFloat() * Range; 
			PeriodHighExt6 = PeriodHigh + Input_Multiplier6.GetFloat() * Range; 

			PeriodLowExt1 = PeriodLow - Input_Multiplier1.GetFloat() * Range; 
			PeriodLowExt2 = PeriodLow - Input_Multiplier2.GetFloat() * Range; 
			PeriodLowExt3 = PeriodLow - Input_Multiplier3.GetFloat() * Range; 
			PeriodLowExt4 = PeriodLow - Input_Multiplier4.GetFloat() * Range; 
			PeriodLowExt5 = PeriodLow - Input_Multiplier5.GetFloat() * Range; 
			PeriodLowExt6 = PeriodLow - Input_Multiplier6.GetFloat() * Range; 

			if (Input_RoundExt.GetYesNo())
			{
				PeriodHighExt1 = sc.RoundToTickSize(PeriodHighExt1, sc.TickSize); 
				PeriodHighExt2 = sc.RoundToTickSize(PeriodHighExt2, sc.TickSize); 
				PeriodHighExt3 = sc.RoundToTickSize(PeriodHighExt3, sc.TickSize); 
				PeriodHighExt4 = sc.RoundToTickSize(PeriodHighExt4, sc.TickSize); 
				PeriodHighExt5 = sc.RoundToTickSize(PeriodHighExt5, sc.TickSize); 
				PeriodHighExt6 = sc.RoundToTickSize(PeriodHighExt6, sc.TickSize); 

				PeriodLowExt1 = sc.RoundToTickSize(PeriodLowExt1, sc.TickSize); 
				PeriodLowExt2 = sc.RoundToTickSize(PeriodLowExt2, sc.TickSize); 
				PeriodLowExt3 = sc.RoundToTickSize(PeriodLowExt3, sc.TickSize); 
				PeriodLowExt4 = sc.RoundToTickSize(PeriodLowExt4, sc.TickSize); 
				PeriodLowExt5 = sc.RoundToTickSize(PeriodLowExt5, sc.TickSize); 
				PeriodLowExt6 = sc.RoundToTickSize(PeriodLowExt6, sc.TickSize); 
			}

			for (int Index = PeriodFirstIndex; Index < sc.Index; Index++)
			{
				IBHigh[Index]  = PeriodHigh;
				IBLow[Index]   = PeriodLow;
				IBMid[Index]   = PeriodMid;
				IBHExt1[Index] = PeriodHighExt1;
				IBHExt2[Index] = PeriodHighExt2;
				IBHExt3[Index] = PeriodHighExt3;
				IBHExt4[Index] = PeriodHighExt4;
				IBHExt5[Index] = PeriodHighExt5;
				IBHExt6[Index] = PeriodHighExt6;
				IBLExt1[Index] = PeriodLowExt1;
				IBLExt2[Index] = PeriodLowExt2;
				IBLExt3[Index] = PeriodLowExt3;
				IBLExt4[Index] = PeriodLowExt4;
				IBLExt5[Index] = PeriodLowExt5;
				IBLExt6[Index] = PeriodLowExt6;			
			}

			sc.EarliestUpdateSubgraphDataArrayIndex = PeriodFirstIndex;
		}
	}

	// Plot current values
	if (PeriodLow != FLT_MAX)
	{
		IBHigh[sc.Index]  = PeriodHigh;
		IBLow[sc.Index]   = PeriodLow;
		IBMid[sc.Index]   = PeriodMid;
		IBHExt1[sc.Index] = PeriodHighExt1;
		IBHExt2[sc.Index] = PeriodHighExt2;
		IBHExt3[sc.Index] = PeriodHighExt3;
		IBHExt4[sc.Index] = PeriodHighExt4;
		IBHExt5[sc.Index] = PeriodHighExt5;
		IBHExt6[sc.Index] = PeriodHighExt6;
		IBLExt1[sc.Index] = PeriodLowExt1;
		IBLExt2[sc.Index] = PeriodLowExt2;
		IBLExt3[sc.Index] = PeriodLowExt3;
		IBLExt4[sc.Index] = PeriodLowExt4;
		IBLExt5[sc.Index] = PeriodLowExt5;
		IBLExt6[sc.Index] = PeriodLowExt6;
	}
}

/*============================================================================*/
SCSFExport scsf_RenkoVisualOpenCloseValues(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RenkoOpen  = sc.Subgraph[0];
	SCSubgraphRef RenkoClose = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		sc.GraphName		= "Renko Visual Open/Close Values";
		sc.DrawZeros		= 0;
		sc.GraphRegion		= 0;
		sc.AutoLoop			= 1;

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		RenkoOpen.Name = "Renko Open";
		RenkoOpen.PrimaryColor = RGB(255, 255, 255);
		RenkoOpen.LineWidth = 3;
		RenkoOpen.DrawStyle = DRAWSTYLE_LEFT_PRICE_BAR_DASH;
		RenkoOpen.DrawZeros = false;

		RenkoClose.Name = "Renko Close";
		RenkoClose.PrimaryColor = RGB(255, 255, 255);
		RenkoClose.LineWidth = 3;
		RenkoClose.DrawStyle = DRAWSTYLE_RIGHT_PRICE_BAR_DASH;
		RenkoClose.DrawZeros = false;

		return;
	}

	if (sc.RenkoTicksPerBar != 0)
	{
		RenkoOpen[sc.Index]  = sc.BaseData[SC_RENKO_OPEN][sc.Index];
		RenkoClose[sc.Index] = sc.BaseData[SC_RENKO_CLOSE][sc.Index];
	}
}

/*============================================================================*/
SCSFExport scsf_StochasticPercentile(SCStudyInterfaceRef sc)
{
	SCSubgraphRef StochasticSubgraph = sc.Subgraph[0];
	SCSubgraphRef StochasticAverageSubgraph = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];


	SCInputRef InputData = sc.Input[0];
	SCInputRef StochasticLength = sc.Input[1];
	SCInputRef MovingAverageLength = sc.Input[2];
	SCInputRef Line1Value = sc.Input[3];
	SCInputRef Line2Value = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Stochastic - Percentile";

		sc.ValueFormat = 2;

		StochasticSubgraph.Name = "Stochastic%";
		StochasticSubgraph.DrawStyle = DRAWSTYLE_LINE;
		StochasticSubgraph.PrimaryColor = RGB(0,255,0);
		StochasticSubgraph.DrawZeros = true;

		StochasticAverageSubgraph.Name = "Average";
		StochasticAverageSubgraph.DrawStyle = DRAWSTYLE_LINE;
		StochasticAverageSubgraph.PrimaryColor = RGB(255, 127, 0);
		StochasticAverageSubgraph.DrawZeros = true;


		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255, 255, 0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(0, 0, 255);
		Line2.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		StochasticLength.Name = "Length";
		StochasticLength.SetInt(10);
		StochasticLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		MovingAverageLength.Name = "Moving Average Length";
		MovingAverageLength.SetInt(10);
		MovingAverageLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Line1Value.Name = "Line 1 Value";
		Line1Value.SetFloat(20);

		Line2Value.Name = "Line 2 Value";
		Line2Value.SetFloat(80);

		sc.AutoLoop = false;
		return;
	}

	std::vector<float> AscendingValues;

	SCFloatArrayRef DataArray = sc.BaseData[InputData.GetInputDataIndex()];
	sc.DataStartIndex = StochasticLength.GetInt() - 1;

	for (int CurrentIndex = sc.UpdateStartIndex; CurrentIndex < sc.ArraySize; CurrentIndex++)
	{
		AscendingValues.clear();
		int FirstIndex = CurrentIndex - StochasticLength.GetInt() + 1;
		for (int BarIndex = FirstIndex; BarIndex <= CurrentIndex; BarIndex++)
		{
			AscendingValues.push_back(DataArray[BarIndex]);
		}

		std::sort(AscendingValues.begin(), AscendingValues.end());
		float CurrentPrice = DataArray[CurrentIndex];

		int AscendingValuesSize = static_cast<int>(AscendingValues.size());
		int AscendingValuesIndex = 0;

		for(; AscendingValuesIndex < AscendingValuesSize; AscendingValuesIndex++)
		{
			if (CurrentPrice == AscendingValues[AscendingValuesIndex])
				break;

			if (AscendingValuesIndex == AscendingValuesSize - 1)
				break;
		}

		StochasticSubgraph[CurrentIndex] = 100.0f*AscendingValuesIndex/(StochasticLength.GetInt() - 1);
		
		sc.SimpleMovAvg(StochasticSubgraph, StochasticAverageSubgraph, CurrentIndex, MovingAverageLength.GetInt());


		Line1[CurrentIndex] = Line1Value.GetFloat();
		Line2[CurrentIndex] = Line2Value.GetFloat();
	}

	return;


}
