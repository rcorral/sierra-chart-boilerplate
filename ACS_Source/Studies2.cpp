#include "sierrachart.h"

#include "scstudyfunctions.h"

/*==========================================================================*/
SCSFExport scsf_DoubleStochastic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Bressert   = sc.Subgraph[0];
	SCSubgraphRef UpperLine  = sc.Subgraph[1];
	SCSubgraphRef LowerLine  = sc.Subgraph[2];
	SCSubgraphRef DSTrigger = sc.Subgraph[3];

	SCInputRef HighLowPeriodLength = sc.Input[0];
	SCInputRef EmaLength           = sc.Input[1];
	SCInputRef UpperLineValue      = sc.Input[2];
	SCInputRef LowerLineValue      = sc.Input[3];
	SCInputRef Version             = sc.Input[4];
	SCInputRef SmoothingLength     = sc.Input[5];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Double Stochastic - Bressert";
		sc.StudyDescription = "";

		sc.GraphRegion = 2;
		sc.AutoLoop = 1;

		Bressert.Name = "DS";
		Bressert.DrawStyle = DRAWSTYLE_LINE;
		Bressert.PrimaryColor = RGB(0,255,0);
		Bressert.DrawZeros = false;

		DSTrigger.Name = "DS Trigger";
		DSTrigger.DrawStyle = DRAWSTYLE_LINE;
		DSTrigger.PrimaryColor = RGB(255,0,255);
		DSTrigger.DrawZeros = false;

		UpperLine.Name = "Upper Line";
		UpperLine.DrawStyle = DRAWSTYLE_LINE;
		UpperLine.PrimaryColor = RGB(255,255,0);
		UpperLine.DrawZeros = false;
		
		LowerLine.Name = "Lower Line";
		LowerLine.DrawStyle = DRAWSTYLE_LINE;
		LowerLine.PrimaryColor = RGB(255,127,0);
		LowerLine.DrawZeros = false;


		HighLowPeriodLength.Name = "High & Low Period Length";
		HighLowPeriodLength.SetInt(15);
		HighLowPeriodLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		HighLowPeriodLength.DisplayOrder = 1;

		EmaLength.Name = "Stochastic Exponential Mov Avg Length";
		EmaLength.SetInt(18);
		EmaLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		EmaLength.DisplayOrder = 2;

		SmoothingLength.Name = "Smoothing Length";
		SmoothingLength.SetInt(3);
		SmoothingLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		SmoothingLength.DisplayOrder = 3;

		UpperLineValue.Name = "Upper Line Value";
		UpperLineValue.SetFloat(80.0f);
		UpperLineValue.DisplayOrder = 4;

		LowerLineValue.Name = "Lower Line Value";
		LowerLineValue.SetFloat(20.0f);
		LowerLineValue.DisplayOrder = 5;

		Version.SetInt(1);

		return;
	}

	if (Version.GetInt() < 1)
	{
		Version.SetInt(1);
		SmoothingLength.SetInt(3);
	}

	// Do data processing

	int Index = sc.Index;
	UpperLine[Index] = UpperLineValue.GetFloat();
	LowerLine[Index] = LowerLineValue.GetFloat();


	int HighLowPeriod =  HighLowPeriodLength.GetInt();

	float High = sc.GetHighest(sc.High, HighLowPeriod);
	float Low = sc.GetLowest(sc.Low, HighLowPeriod);
	if (High - Low != 0.0f)
		Bressert.Arrays[0][Index] = ((sc.Close[Index] - Low) / (High - Low)) * 100.0f;
	else
		Bressert.Arrays[0][Index] = 0.0f;

	sc.ExponentialMovAvg(Bressert.Arrays[0], DSTrigger, EmaLength.GetInt());

	float TriggerHigh = sc.GetHighest(DSTrigger, HighLowPeriod);
	float TriggerLow = sc.GetLowest(DSTrigger, HighLowPeriod);
	if (TriggerHigh - TriggerLow != 0.0f)
		Bressert.Arrays[1][Index] = ((DSTrigger[Index] - TriggerLow) / (TriggerHigh - TriggerLow)) * 100.0f;
	else
		Bressert.Arrays[1][Index] = 0.0f;

	sc.ExponentialMovAvg(Bressert.Arrays[1], Bressert.Arrays[2], EmaLength.GetInt());
	sc.ExponentialMovAvg(Bressert.Arrays[2], Bressert, SmoothingLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_PreviousBarClose(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PrevClose = sc.Subgraph[0];

	SCInputRef InputData= sc.Input[0];
	SCInputRef Version = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Previous Bar Close";
		sc.AutoLoop = 1;

		PrevClose.DrawStyle = DRAWSTYLE_STAIR_STEP;
		PrevClose.Name = "PB Close";
		PrevClose.LineLabel  |= (LL_DISPLAY_VALUE | LL_VALUE_ALIGN_VALUES_SCALE);
		PrevClose.PrimaryColor = RGB(0,255,0);

		sc.GraphRegion = 0;
		
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		
		Version.SetInt(1);
		
		return;
	}

	if (Version.GetInt() < 1)
	{
		InputData.SetInputDataIndex(SC_LAST);
		Version.SetInt(1);
	}
	
	uint32_t InputDataIndex = InputData.GetInputDataIndex();
	
	// Do data processing
	PrevClose[sc.Index] = sc.BaseData[InputDataIndex][sc.Index - 1];
}

/*==========================================================================*/
SCSFExport scsf_ChangeForDay(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Change = sc.Subgraph[0];

	SCInputRef TickSize = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Change For Day";

		sc.GraphRegion = 1;	

		Change.Name = "Change";
		Change.DrawStyle = DRAWSTYLE_DASH;
		Change.SecondaryColorUsed = 1;
		Change.SecondaryColor = RGB(255,0,0);

		TickSize.Name = "Tick Size";
		TickSize.SetFloat(0.25f);

		return;
	}

	// Do data processing
	Change[sc.Index] = sc.Close[sc.ArraySize - 1];

	int OpeningIndex = sc.GetNearestMatchForSCDateTime(sc.ChartNumber, sc.BaseDateTimeIn.DateAt(sc.ArraySize-1));

	if (sc.BaseDateTimeIn.DateAt(OpeningIndex) != sc.BaseDateTimeIn.DateAt(sc.ArraySize-1))
		OpeningIndex++;

	float ChangeForDay = sc.Close[sc.ArraySize-1] - sc.Open[OpeningIndex];

	for (int i = OpeningIndex; i < sc.ArraySize; i++)
	{
		Change[i] = ChangeForDay/TickSize.GetFloat();
		if (Change[i] >= 0)
			Change.DataColor[i] = Change.PrimaryColor;
		else
			Change.DataColor[i] = Change.SecondaryColor ;

	}
}

/*==========================================================================*/
SCSFExport scsf_JimBergsVolatilitySystem(SCStudyInterfaceRef sc) // scsf_Name
{   
	SCSubgraphRef StopL = sc.Subgraph[0];
	SCSubgraphRef StopS = sc.Subgraph[1];

	SCSubgraphRef LLV = sc.Subgraph[6];
	SCSubgraphRef HHV = sc.Subgraph[7];
	SCSubgraphRef LongTrigger = sc.Subgraph[8];
	SCSubgraphRef ShortTrigger = sc.Subgraph[9];
	SCSubgraphRef ATR = sc.Subgraph[11];
	SCSubgraphRef Temp12 = sc.Subgraph[12];
	SCSubgraphRef Temp13 = sc.Subgraph[13];
	SCSubgraphRef LongStopValue = sc.Subgraph[14];
	SCSubgraphRef ShortStopValue = sc.Subgraph[15];

	// Configuration
	if (sc.SetDefaults)
	{   
		sc.GraphName = "Jim Berg's Volatility System"; // study name shown in F6 menu and on the chart
		sc.GraphRegion = 0;    // zero based region number
		
		StopL.Name = "Stop L";
		StopL.DrawStyle = DRAWSTYLE_STAIR_STEP;
		StopL.PrimaryColor = RGB(0,185,47); // green
		
		
		StopS.Name = "Stop S";
		StopS.DrawStyle = DRAWSTYLE_STAIR_STEP;
		StopS.PrimaryColor = RGB(255,0,128); // red
		

		sc.AutoLoop = 1;
		
		return;
	}   
	
	// Data processing

	// LongSignal  = C > ( LLV( L, 20 ) + 2 * ATR( 10 ) ); // ATR with Wilder's MA
	// ShortSignal = C < ( HHV( H, 20 ) - 2 * ATR( 10 ) );

	// LongStopValue    = HHV( C - 2 * ATR(10), 15 );
	// ShortStopValue   = LLV( C + 2 * ATR(10), 15 );

	sc.DataStartIndex = 21; // start drawing subgraph at bar #21 (zero based)
	
	// LLV(L,20): subgraph #6
	sc.Lowest(sc.Low, LLV, sc.Index, 20);
	
	// HHV(H,20): subgraph#7
	sc.Highest(sc.High, HHV, sc.Index, 20);
	
	// ATR(10, Wilder's MA): subgraph #11. Intermediate TR: subgraph #10 (not plotted)
	sc.ATR(sc.BaseDataIn, ATR, sc.Index, 10, MOVAVGTYPE_WILDERS);
		
	LongTrigger[sc.Index] = LLV[sc.Index] + 2 * ATR[sc.Index]; // Long  Signal trigger
	ShortTrigger[sc.Index] = HHV[sc.Index] - 2 * ATR[sc.Index]; // Short Signal trigger
	Temp12[sc.Index] = sc.Close[sc.Index] - 2 * ATR[sc.Index];
	Temp13[sc.Index] = sc.Close[sc.Index] + 2 * ATR[sc.Index];
	
	// LongStopValue: subgraph #14
	sc.Highest(Temp12, LongStopValue, 15);
	
	// ShortStopValue: subgraph #15
	sc.Lowest(Temp13, ShortStopValue, 15);
	
	// If LongSignal=true, plot LongStopValue in Subgraph #0, otherwise plot zero
	if (sc.Close[sc.Index] > LongTrigger[sc.Index])
		StopL[sc.Index] = LongStopValue[sc.Index];
	else
		StopL[sc.Index] = 0;
		
	// If ShortSignal=true, plot ShortStopValue in Subgraph #1, otherwise plot zero
	if (sc.Close[sc.Index] < ShortTrigger[sc.Index])
		StopS[sc.Index] = ShortStopValue[sc.Index];
	else
		StopS[sc.Index] = 0;
}  


/*==========================================================================*/
SCSFExport scsf_VolumeWeightedMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef VWMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	// Configuration
	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Volume Weighted"; 
		sc.StudyDescription = "Volume Weighted Moving Average"; 
		sc.GraphRegion = 0;    // zero based region number

		VWMA.Name = "VWMA";
		VWMA.DrawStyle = DRAWSTYLE_LINE;
		VWMA.PrimaryColor = RGB(0,185,47); // green

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); 

		Length.Name = "Length";
		Length.SetInt(5);     
		Length.SetIntLimits(1,MAX_STUDY_LENGTH); 

		sc.ValueFormat = 3;

		sc.AutoLoop = 1;

		return;
	}

	// Data processing

	sc.DataStartIndex = Length.GetInt();    
	sc.VolumeWeightedMovingAverage(sc.BaseData[InputData.GetInputDataIndex()], sc.Volume, VWMA, Length.GetInt());
}


/*==========================================================================*/
SCSFExport scsf_InsideOrEqualsBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef IB = sc.Subgraph[0];

	// Set configuration variables

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Inside Or Equals Bar";

		sc.StudyDescription = "";

		sc.GraphRegion = 0;

		IB.Name = "IB";
		IB.DrawStyle = DRAWSTYLE_COLOR_BAR;
		IB.PrimaryColor = RGB(0, 0, 255);
		IB.DrawZeros = false;

		sc.AutoLoop = 1;

		return;
	}


	// Do data processing

	if (sc.Index < 1)
		return;

	// Array references
	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;

	if (High[sc.Index] <= High[sc.Index - 1] && Low[sc.Index] >= Low[sc.Index - 1])
		IB[sc.Index] = High[sc.Index];
	else
		IB[sc.Index] = 0;
}

/*==========================================================================*/
SCSFExport scsf_AroonOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AroonOscillator = sc.Subgraph[0];
	SCSubgraphRef AroonCalculations = sc.Subgraph[1];

	SCInputRef InputDataHigh = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Aroon Oscillator";
		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		AroonOscillator.Name = "Aroon Oscillator";
		AroonOscillator.DrawStyle = DRAWSTYLE_LINE;
		AroonOscillator.PrimaryColor = RGB(0,255,0);
		AroonOscillator.DrawZeros = true;

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		Length.Name = "Length";
		Length.SetInt(5);           
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW); 

		return;
	}


	// Do data processing


	sc.AroonIndicator(sc.BaseDataIn[InputDataHigh.GetInputDataIndex()], sc.BaseDataIn[InputDataLow.GetInputDataIndex()], AroonCalculations, Length.GetInt());

	AroonOscillator[sc.Index] = AroonCalculations.Arrays[1][sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_AroonIndicator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AroonUp = sc.Subgraph[0];
	SCSubgraphRef AroonDown = sc.Subgraph[1];


	SCInputRef InputDataHigh = sc.Input[0];
	SCInputRef Length = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];


	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Aroon Indicator";
		sc.AutoLoop = 1;
		sc.GraphRegion = 1;	

		AroonUp.Name = "Aroon Up";
		AroonUp.DrawStyle = DRAWSTYLE_LINE;
		AroonUp.PrimaryColor = RGB(0,255,0);
		AroonUp.DrawZeros = true;

		AroonDown.Name = "Aroon Down";
		AroonDown.DrawStyle = DRAWSTYLE_LINE;
		AroonDown.PrimaryColor = RGB(255,0,255);
		AroonDown.DrawZeros = true;

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH); // default data field

		Length.Name = "Length";
		Length.SetInt(5);            // default integer value
		Length.SetIntLimits(1,MAX_STUDY_LENGTH); // min-max value

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW); // default data field 

		return;
	}




	sc.AroonIndicator(sc.BaseDataIn[InputDataHigh.GetInputDataIndex()], sc.BaseDataIn[InputDataLow.GetInputDataIndex()], AroonUp, Length.GetInt());

	AroonDown[sc.Index] = AroonUp.Arrays[0][sc.Index];
}

/*============================================================================
Study function for Welles Wilder's Plus and Minus Directional Indicators
+DI and -DI.
----------------------------------------------------------------------------*/
SCSFExport scsf_DMI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DIPlus = sc.Subgraph[0];
	SCSubgraphRef DIMinus = sc.Subgraph[1];

	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Directional Movement Index";

		sc.StudyDescription = "Welles Wilder's Plus and Minus Directional Indicators +DI and -DI.";

		sc.AutoLoop = 1;  // true

		DIPlus.Name = "DI+";
		DIPlus.DrawStyle = DRAWSTYLE_LINE;
		DIPlus.PrimaryColor = RGB(0,255,0);
		DIPlus.DrawZeros = true;

		DIMinus.Name = "DI-";
		DIMinus.DrawStyle = DRAWSTYLE_LINE;
		DIMinus.PrimaryColor = RGB(255,0,255);
		DIMinus.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, INT_MAX);

		return;
	}


	// Do data processing

	sc.DataStartIndex = Length.GetInt();
	
	sc.DMI(
		sc.BaseData,
		DIPlus,  // DMI+
		DIMinus,  // DMI-
		Length.GetInt());

	//sc.CrossOver(DIPlus.Data, DIMinus.Data);
}

/*============================================================================
----------------------------------------------------------------------------*/
SCSFExport scsf_DMIOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DMIDiff = sc.Subgraph[0];
	SCSubgraphRef Line = sc.Subgraph[1];

	SCInputRef Length = sc.Input[3];
	SCInputRef ShowAbsValues = sc.Input[4];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Directional Movement Oscillator";

		sc.StudyDescription = "Calculates the difference between the Directional Indicators -DI and the +DI.";

		sc.AutoLoop = 1;  // true

		DMIDiff.Name = "DMI Diff";
		DMIDiff.DrawStyle = DRAWSTYLE_LINE;
		DMIDiff.PrimaryColor = RGB(0,255,0);
		DMIDiff.DrawZeros = true;

		Line.Name = "Line";
		Line.DrawStyle = DRAWSTYLE_LINE;
		Line.PrimaryColor = RGB(255,0,255);
		Line.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, INT_MAX);

		ShowAbsValues.Name = "Show Absolute Values";
		ShowAbsValues.SetYesNo(0);

		return;
	}

	// Do data processing

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.DMIDiff(sc.BaseData, DMIDiff, sc.Index, Length.GetInt());  // DMI Diff

	if(ShowAbsValues.GetYesNo())
		DMIDiff[sc.Index] = fabs(DMIDiff[sc.Index]);
}

/*============================================================================
Average Directional Movement Index study.  This is calculated according
to the Welles Wilder formulas.
----------------------------------------------------------------------------*/
SCSFExport scsf_ADX(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ADX = sc.Subgraph[0];

	SCInputRef DXLength = sc.Input[3];
	SCInputRef DXMALength = sc.Input[4];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "ADX";

		sc.StudyDescription = "Average Directional Movement Index.  This is calculated according to the Welles Wilder formulas.";

		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 1;

		ADX.Name = "ADX";
		ADX.DrawStyle = DRAWSTYLE_LINE;
		ADX.PrimaryColor = RGB(0,255,0);
		ADX.DrawZeros = true;

		DXLength.Name = "DX Length";
		DXLength.SetInt(14);
		DXLength.SetIntLimits(1, INT_MAX);

		DXMALength.Name = "DX Mov Avg Length";
		DXMALength.SetInt(14);
		DXMALength.SetIntLimits(1, INT_MAX);

		return;
	}


	// Do data processing

	sc.DataStartIndex = DXLength.GetInt() + DXMALength.GetInt() - 1;

	sc.ADX(
		sc.BaseData,
		ADX,
		DXLength.GetInt(),
		DXMALength.GetInt());
}

/*============================================================================
Average Directional Movement Index Rating study.  This is calculated
according to the Welles Wilder formulas.
----------------------------------------------------------------------------*/
SCSFExport scsf_ADXR(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ADXR = sc.Subgraph[0];

	SCInputRef DXLength = sc.Input[3];
	SCInputRef DXMALength = sc.Input[4];
	SCInputRef ADXRInterval = sc.Input[5];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "ADXR";

		sc.StudyDescription = "Average Directional Movement Index Rating.  This is calculated according to the Welles Wilder formulas.";

		sc.AutoLoop = 1;  // true
		sc.GraphRegion = 1;

		ADXR.Name = "ADXR";
		ADXR.DrawStyle = DRAWSTYLE_LINE;
		ADXR.PrimaryColor = RGB(0,255,0);
		ADXR.DrawZeros = true;

		DXLength.Name = "DX Length";
		DXLength.SetInt(14);
		DXLength.SetIntLimits(1, INT_MAX);

		DXMALength.Name = "DX Mov Avg Length";
		DXMALength.SetInt(14);
		DXMALength.SetIntLimits(1, INT_MAX);

		ADXRInterval.Name = "ADXR Interval";
		ADXRInterval.SetInt(14);
		ADXRInterval.SetIntLimits(1, INT_MAX);

		return;
	}


	// Do data processing

	sc.DataStartIndex = DXLength.GetInt() + DXMALength.GetInt() + ADXRInterval.GetInt() - 2;

	sc.ADXR(
		sc.BaseData,
		ADXR,
		DXLength.GetInt(),
		DXMALength.GetInt(),
		ADXRInterval.GetInt());
}


/*============================================================================
This example code calculates a smoothed moving average (5 period by
default).
----------------------------------------------------------------------------*/
SCSFExport scsf_SmoothedMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SMMA = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	// Set configuration variables	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Moving Average - Smoothed";

		sc.StudyDescription = "Function for calculating a Wilders Smooth Moving Average (SMMA).";

		// Set the region to draw the graph in.  Region zero is the main
		// price graph region.
		sc.GraphRegion = 0;


		// Set the name of the first subgraph
		SMMA.Name = "SMMA";
		// Set the color and style of the graph line.  If these are not set the default will be used.
		SMMA.PrimaryColor = RGB(255, 0, 0);  // Red
		SMMA.DrawStyle = DRAWSTYLE_LINE;
		SMMA.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		// Make the Length input and default it to 5
		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(2, MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		// Must return before doing any data processing if sc.SetDefaults is set
		return;
	}

	// Do data processing		

	sc.DataStartIndex = Length.GetInt() - 1;

	if (sc.Index == Length.GetInt() - 1)
	{
		sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], SMMA, Length.GetInt());
	}
	else if (sc.Index > Length.GetInt() - 1)
	{
		SMMA[sc.Index] = (Length.GetInt() * SMMA[sc.Index - 1] - SMMA[sc.Index - 1] + sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index]) / Length.GetInt();
	}
}

/*==========================================================================*/
SCSFExport scsf_PercentagePriceOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PPO = sc.Subgraph[0];
	SCSubgraphRef ShortMA = sc.Subgraph[1];
	SCSubgraphRef LongMA = sc.Subgraph[2];

	SCInputRef LongMALength = sc.Input[0];
	SCInputRef ShortMALength = sc.Input[1];
	SCInputRef MAType = sc.Input[2];
	SCInputRef InputData = sc.Input[3];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Percentage Price Oscillator";

		PPO.Name = "PPO";
		PPO.DrawStyle = DRAWSTYLE_LINE;
		PPO.PrimaryColor = RGB(0,255,0);
		PPO.DrawZeros = true;

		LongMALength.Name = "Long Mov Avg Length";
		LongMALength.SetInt(30);
		LongMALength.SetIntLimits(1, INT_MAX);

		ShortMALength.Name = "Short Mov Avg Length";
		ShortMALength.SetInt(10);
		ShortMALength.SetIntLimits(1, INT_MAX);

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = 1;

		return;
	}

	sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()], LongMA, MAType.GetMovAvgType(),LongMALength.GetInt());

	sc.MovingAverage(sc.BaseData[InputData.GetInputDataIndex()], ShortMA, MAType.GetMovAvgType(),ShortMALength.GetInt());

	if (LongMA[sc.Index] == 0)
		PPO[sc.Index] = PPO[sc.Index - 1];
	else
		PPO[sc.Index] = (  (ShortMA[sc.Index] - LongMA[sc.Index]) / LongMA[sc.Index]) * 100;
}
/*==========================================================================*/


SCSFExport scsf_HerrickPayoffIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HPI = sc.Subgraph[0];
	SCSubgraphRef Zero = sc.Subgraph[1];

	SCInputRef ValueOfMove = sc.Input[0];
	SCInputRef SmoothingMult = sc.Input[1];
	SCInputRef MaximumOrMinimum = sc.Input[2];
	SCInputRef Divisor = sc.Input[3];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Herrick Payoff Index";

		HPI.Name = "HPI";
		HPI.PrimaryColor = RGB(0, 255, 0);  // Red
		HPI.DrawStyle = DRAWSTYLE_LINE;
		HPI.DrawZeros = true;

		Zero.Name = "Zero";
		Zero.PrimaryColor = RGB(255, 255, 0);  // Red
		Zero.DrawStyle = DRAWSTYLE_LINE;
		Zero.DrawZeros = true;

		ValueOfMove.Name = "Value of a .01 Move";
		ValueOfMove.SetFloat(0.5f);
		
		SmoothingMult.Name = "Smoothing multiplier";
		SmoothingMult.SetFloat(0.1f);
		
		MaximumOrMinimum.Name = "Maximum or Minimum Open Interest: 1= maximum, 2= minimum";
		MaximumOrMinimum.SetInt(1);
		MaximumOrMinimum.SetIntLimits(1, 2);

		Divisor.Name = "Divisor";
		Divisor.SetFloat(100000.0f);
		

		sc.AutoLoop = 1;

		return;
	}

	if (sc.Index == 0)
		return;
	
		float CurrentVolume = sc.Volume[sc.Index];

		// current and previous mean price (high + low) / 2
		float AveragePrice = sc.HLAvg[sc.Index];	
		float PriorAveragePrice = sc.HLAvg[sc.Index - 1];   

		
		float CurrentOpenInterest = sc.OpenInterest[sc.Index];	
		float PriorOpenInterest = sc.OpenInterest[sc.Index-1];	

		if (CurrentOpenInterest<=0.0f)
		{
			CurrentOpenInterest = PriorOpenInterest;
		}

		// Determine sign for calculations
		float sign = ( AveragePrice > PriorAveragePrice ) ? 1.0f : -1.0f;

		// the absolute value of the difference between current open interest and
		// prior open interest
		float PositiveOpenInterest = fabs(CurrentOpenInterest - PriorOpenInterest);

		
		float MaximumOrMinimumOpenInterest = (MaximumOrMinimum.GetInt()==1) ? max(CurrentOpenInterest, PriorOpenInterest) : min(CurrentOpenInterest, PriorOpenInterest);

		float PriorHerrickPayoffIndex = HPI[sc.Index - 1];

		if (MaximumOrMinimumOpenInterest>0.0f)
		{
			float CalculationResult = ValueOfMove.GetFloat() * CurrentVolume * (AveragePrice - PriorAveragePrice) * (1 + sign*2*PositiveOpenInterest/MaximumOrMinimumOpenInterest)/Divisor.GetFloat();

			if(sc.Index>1)
			{	
				HPI[sc.Index] = PriorHerrickPayoffIndex + (CalculationResult-PriorHerrickPayoffIndex)*SmoothingMult.GetFloat();
			}
			else
			{
				HPI[sc.Index] = CalculationResult;
			}
		}
		else
		{
			HPI[sc.Index] = PriorHerrickPayoffIndex;
		}
	
	
}

/*==========================================================================*/
SCSFExport scsf_TRIX(SCStudyInterfaceRef sc)
{	
	SCSubgraphRef TRIX = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "TRIX";

		TRIX.Name = "TRIX";
		TRIX.DrawStyle = DRAWSTYLE_BAR;
		TRIX.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		// Make the Length input and default it to 3
		Length.Name = "Length";
		Length.SetInt(3);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		return;
	}

	// Data start and update start
	sc.DataStartIndex = (Length.GetInt() - 1) * 3 + 1;
	
	sc.TRIX(sc.BaseData[InputData.GetInputDataIndex()], TRIX, Length.GetInt());
}

/*==========================================================================
* Method			- Momentum Gauging
* Description	- 
* Price = Close
* Length = 20
* 
* Histo = LinearRegression[Length] (price - ((Highest[Length](High) + Lowest[Length](Low))/2 + ExponentialAverage[Length](close))/2 )
* 
* RETURN Histo as "Histogramme"[/color][/color]
*
==========================================================================*/
SCSFExport scsf_MomentumGauging(SCStudyInterfaceRef sc)
{
	const int INDEX_EMA = 1;
	const int INDEX_LREG_SRC = 2;

	SCSubgraphRef Momentum = sc.Subgraph[0];
	SCSubgraphRef EMA = sc.Subgraph[1];
	SCSubgraphRef LREG_SRC = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Momentum Gauging";

		Momentum.Name = "Momentum";
		Momentum.DrawStyle = DRAWSTYLE_BAR;
		Momentum.PrimaryColor = RGB(0,255,0);
		//Momentum.SecondaryColorUsed = 1;
		Momentum.SecondaryColor = RGB(255,0,0);
		

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		// Make the Length input and default it to 3
		Length.Name = "Length";
		Length.SetInt(3);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		return;
	}

	// First output elements are not valid
	sc.DataStartIndex = Length.GetInt();

	sc.ExponentialMovAvg(sc.Close, EMA, sc.Index, Length.GetInt());  // Note: EMA returns close when index is < Length.GetInt()

	float hlh = sc.GetHighest(sc.High, sc.Index, Length.GetInt());
	float lll = sc.GetLowest(sc.Low, sc.Index, Length.GetInt());

	SCFloatArrayRef price = sc.BaseData[InputData.GetInputDataIndex()];

	// populate the source data for linear regression
	LREG_SRC[sc.Index] = price[sc.Index] - ((hlh + lll)/2.0f + EMA[sc.Index])/2.0f;

	sc.LinearRegressionIndicator(LREG_SRC, Momentum, sc.Index, Length.GetInt());

	/*if (Momentum[sc.Index] >= 0)
		Momentum.DataColor[sc.Index] = Momentum.PrimaryColor;
	else
		Momentum.DataColor[sc.Index] = Momentum.SecondaryColor ;*/

}

/*==========================================================================

function main(nInputLength) 
{
var vC = close(0);
var vPH = high(-1);
var vPL = low(-1);

if(vC == null || vPH == null || vPL == null) return;

if (vC > vPH) {
setPriceBarColor(Color.lime);
} else if(vC < vPL) {
setPriceBarColor(Color.red);
}

return;	
}
==========================================================================*/
SCSFExport scsf_HHLLCandles(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HH = sc.Subgraph[0];
	SCSubgraphRef LL = sc.Subgraph[1];
	SCSubgraphRef N = sc.Subgraph[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "HH LL Bars";
		sc.StudyDescription = "";

		sc.GraphRegion = 0;

		HH.Name = "HH";
		HH.DrawStyle = DRAWSTYLE_COLOR_BAR_CANDLE_FILL;
		HH.PrimaryColor = RGB(0, 255, 0);
		HH.SecondaryColor = RGB(0, 0, 0);
		HH.SecondaryColorUsed = 1;
		HH.DrawZeros = false;

		LL.Name = "LL";
		LL.DrawStyle = DRAWSTYLE_COLOR_BAR_CANDLE_FILL;
		LL.PrimaryColor = RGB(255, 0, 0);
		LL.SecondaryColor = RGB(0, 0, 0);
		LL.SecondaryColorUsed = 1;
		LL.DrawZeros = false;

		N.Name = "N";
		N.DrawStyle = DRAWSTYLE_COLOR_BAR;
		N.PrimaryColor = RGB(50, 50, 190);
		N.DrawZeros = false;

		sc.AutoLoop = 1;

		return;
	}
	
	sc.ValueFormat = sc.BaseGraphValueFormat;
	
	sc.DataStartIndex = 1;
	if (sc.Index<1)
		return;

	float priorHigh = sc.High[sc.Index - 1];
	float priorLow = sc.Low[sc.Index - 1];

	float currentClose = sc.Close[sc.Index];

	HH[sc.Index] = 0;
	LL[sc.Index] = 0;
	N[sc.Index] = 0;

	if (sc.FormattedEvaluate(currentClose, sc.BaseGraphValueFormat, GREATER_OPERATOR, priorHigh, sc.BaseGraphValueFormat))
	{
		HH[sc.Index] = 1;
		//We need to set these to 0 because conditions can change on the last bar during real-time updating.
		N[sc.Index] = 0;
		LL[sc.Index] = 0;
	}
	else if (sc.FormattedEvaluate(currentClose, sc.BaseGraphValueFormat, LESS_OPERATOR, priorLow, sc.BaseGraphValueFormat))
	{
		LL[sc.Index] = 1;
		N[sc.Index] = 0;
		HH[sc.Index] = 0;
	}
	else
	{
		N[sc.Index] = 1;
		HH[sc.Index] = 0;
		LL[sc.Index] = 0;
	}

}

/*==========================================================================*/
SCSFExport scsf_RahulMohindarOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_ST1 = sc.Subgraph[0];
	SCSubgraphRef Subgraph_ST2 = sc.Subgraph[1];
	SCSubgraphRef Subgraph_ST3 = sc.Subgraph[2];
	SCSubgraphRef Subgraph_RMO = sc.Subgraph[3];
	SCSubgraphRef Subgraph_Refline = sc.Subgraph[4];
	SCSubgraphRef Subgraph_Buy = sc.Subgraph[5];
	SCSubgraphRef Subgraph_Sell = sc.Subgraph[6];

	SCFloatArrayRef Array_MATemp3 = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef Array_MATemp4 = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef Array_MATemp5 = sc.Subgraph[0].Arrays[2];
	SCFloatArrayRef Array_MATemp6 = sc.Subgraph[0].Arrays[3];
	SCFloatArrayRef Array_MATemp7 = sc.Subgraph[0].Arrays[4];
	SCFloatArrayRef Array_MATemp8 = sc.Subgraph[0].Arrays[5];
	SCFloatArrayRef Array_MATemp9 = sc.Subgraph[0].Arrays[6];
	SCFloatArrayRef Array_MATemp10 = sc.Subgraph[0].Arrays[7];
	SCFloatArrayRef Array_MATemp11 = sc.Subgraph[0].Arrays[8];
	SCFloatArrayRef Array_MATemp12 = sc.Subgraph[0].Arrays[9];
	SCFloatArrayRef Array_Highest = sc.Subgraph[0].Arrays[10];
	SCFloatArrayRef Array_Lowest = sc.Subgraph[0].Arrays[11];

	SCInputRef Input_Len1 = sc.Input[0];
	SCInputRef Input_Len2 = sc.Input[1];
	SCInputRef Input_Len3 = sc.Input[2];
	SCInputRef Input_Len4 = sc.Input[3];
	SCInputRef Input_OffsetPercent = sc.Input[4];
	SCInputRef Input_InputData = sc.Input[5];
	SCInputRef Input_Version = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Rahul Mohindar Oscillator";

		Subgraph_ST1.Name = "Swing Trade 1";
		Subgraph_ST1.DrawStyle = DRAWSTYLE_IGNORE;
		Subgraph_ST1.PrimaryColor = RGB(0, 255, 0);

		Subgraph_ST2.Name = "Swing Trade 2";
		Subgraph_ST2.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_ST2.PrimaryColor = RGB(255, 0, 255);

		Subgraph_ST3.Name = "Swing Trade 3";
		Subgraph_ST3.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_ST3.PrimaryColor = RGB(255, 255, 0);

		Subgraph_RMO.Name = "Rahul Mohindar Oscillator";
		Subgraph_RMO.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_RMO.PrimaryColor = RGB(0, 0, 255);

		Subgraph_Refline.Name = "Reference Line";
		Subgraph_Refline.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Refline.DrawZeros = true;
		Subgraph_Refline.PrimaryColor = RGB(255, 255, 255);

		Subgraph_Buy.Name = "Buy";
		Subgraph_Buy.PrimaryColor = RGB(0, 255, 0);	// green
		Subgraph_Buy.DrawStyle = DRAWSTYLE_ARROW_UP;
		Subgraph_Buy.LineWidth = 2;	//Width of arrow
		Subgraph_Buy.DrawZeros = 0;

		Subgraph_Sell.Name = "Sell";
		Subgraph_Sell.DrawStyle = DRAWSTYLE_ARROW_DOWN;
		Subgraph_Sell.PrimaryColor = RGB(255, 0, 0);	// red
		Subgraph_Sell.LineWidth = 2; //Width of arrow
		Subgraph_Sell.DrawZeros = 0;

		Input_Len1.Name = "Length 1";
		Input_Len1.SetInt(2);

		Input_Len2.Name = "Length 2";
		Input_Len2.SetInt(10);

		Input_Len3.Name = "Length 3";
		Input_Len3.SetInt(30);

		Input_Len4.Name = "Length 4";
		Input_Len4.SetInt(81);

		Input_OffsetPercent.Name = "Arrow Offset Percentage";
		Input_OffsetPercent.SetInt(8);

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_Version.SetInt(1);

		sc.AutoLoop = 1;

		return;
	}

	if (Input_Version.GetInt() < 1)
	{
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_Version.SetInt(1);
	}

	sc.DataStartIndex = max(Input_Len1.GetInt(), max(Input_Len2.GetInt(), max(Input_Len3.GetInt(), Input_Len4.GetInt())));

	SCFloatArrayRef BaseDataArray = sc.BaseData[Input_InputData.GetInputDataIndex()];

	sc.SimpleMovAvg(BaseDataArray, Array_MATemp3, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp3, Array_MATemp4, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp4, Array_MATemp5, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp5, Array_MATemp6, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp6, Array_MATemp7, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp7, Array_MATemp8, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp8, Array_MATemp9, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp9, Array_MATemp10, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp10, Array_MATemp11, Input_Len1.GetInt());
	sc.SimpleMovAvg(Array_MATemp11, Array_MATemp12, Input_Len1.GetInt());
	sc.Highest(BaseDataArray, Array_Highest, Input_Len2.GetInt());
	sc.Lowest(BaseDataArray, Array_Lowest, Input_Len2.GetInt());

	float  Range = Array_Highest[sc.Index] - Array_Lowest[sc.Index];

	if (Range != 0)
	{
		Subgraph_ST1[sc.Index] =
			100 * (BaseDataArray[sc.Index]
				- (Array_MATemp3[sc.Index]
					+ Array_MATemp4[sc.Index]
					+ Array_MATemp5[sc.Index]
					+ Array_MATemp6[sc.Index]
					+ Array_MATemp7[sc.Index]
					+ Array_MATemp8[sc.Index]
					+ Array_MATemp9[sc.Index] + Array_MATemp10[sc.Index]
					+ Array_MATemp11[sc.Index]
					+ Array_MATemp12[sc.Index]) / 10) / Range;
	}


	sc.ExponentialMovAvg(Subgraph_ST1, Subgraph_ST2, Input_Len3.GetInt());
	sc.ExponentialMovAvg(Subgraph_ST2, Subgraph_ST3, Input_Len3.GetInt());
	sc.ExponentialMovAvg(Subgraph_ST1, Subgraph_RMO, Input_Len4.GetInt());
	Subgraph_Refline[sc.Index] = 0;

	float OffsetPercent = Input_OffsetPercent.GetFloat() * 0.01f;

	if (sc.CrossOver(Subgraph_ST3, Subgraph_ST2) == CROSS_FROM_BOTTOM)
	{

		Subgraph_Buy[sc.Index] = Subgraph_ST3[sc.Index] - OffsetPercent * Subgraph_ST3[sc.Index];
		Subgraph_Sell[sc.Index] = 0;
	}
	else if (sc.CrossOver(Subgraph_ST3, Subgraph_ST2) == CROSS_FROM_TOP)
	{
		Subgraph_Sell[sc.Index] = Subgraph_ST3[sc.Index] + OffsetPercent * Subgraph_ST3[sc.Index];
		Subgraph_Buy[sc.Index] = 0;
	}
	else
	{
		Subgraph_Buy[sc.Index] = 0;
		Subgraph_Sell[sc.Index] = 0;
	}
}

/* ****************************************************************
*
* Calculation:
* The value of the DeMarker for the "i" interval is calculated as follows:
*	The DeMax(i) is calculated: 
*
* If high(i) > high(i-1) , then DeMax(i) = high(i)-high(i-1), otherwise DeMax(i) = 0
*	The DeMin(i) is calculated: 
*
* If low(i) < low(i-1), then DeMin(i) = low(i-1)-low(i), otherwise DeMin(i) = 0
*	The value of the DeMarker is calculated as: 
*
* DMark(i) = SMA(DeMax, N)/(SMA(DeMax, N)+SMA(DeMin, N))
*
******************************************************************/
SCSFExport scsf_Demarker(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Demarker = sc.Subgraph[0];
	SCSubgraphRef Demax = sc.Subgraph[1];
	SCSubgraphRef SMA_Demax = sc.Subgraph[2];
	SCSubgraphRef Demin = sc.Subgraph[3];
	SCSubgraphRef SMA_Demin = sc.Subgraph[4];

	if (sc.SetDefaults)
	{
		sc.GraphName="Demarker";
		sc.StudyDescription="Demarker";

		Demarker.Name="Demarker";
		Demarker.DrawStyle=DRAWSTYLE_LINE;
		Demarker.PrimaryColor = RGB(0,255,0);

		sc.Input[0].Name="Length";
		sc.Input[0].SetInt(13);
		sc.Input[0].SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		return;
	}

	int length = sc.Input[0].GetInt();

	sc.DataStartIndex = length;

	if (sc.Index > 0)
	{
		// If high(i) > high(i-1) , then DeMax(i) = high(i)-high(i-1), otherwise DeMax(i) = 0

		float high = sc.High[sc.Index];
		float highOld = sc.High[sc.Index-1];

		if (high > highOld)
		{
			Demax[sc.Index] = high - highOld;
		}
		else
		{
			Demax[sc.Index] = 0.0f;
		}

		// If low(i) < low(i-1), then DeMin(i) = low(i-1)-low(i), otherwise DeMin(i) = 0

		float low = sc.Low[sc.Index];
		float lowOld = sc.Low[sc.Index-1];

		if (low < lowOld)
		{
			Demin[sc.Index] = lowOld - low;
		}
		else
		{
			Demin[sc.Index] = 0.0f;
		}
	}
	else
	{
		Demax[sc.Index] = 0.0f;
		Demin[sc.Index] = 0.0f;
	}

	// DMark(i) = SMA(DeMax, N)/(SMA(DeMax, N)+SMA(DeMin, N))

	sc.SimpleMovAvg(Demax, SMA_Demax, length);
	sc.SimpleMovAvg(Demin, SMA_Demin,  length);

	float summ = SMA_Demax[sc.Index] + SMA_Demin[sc.Index];
	if (summ != 0.0f)
	{        
		Demarker[sc.Index] = SMA_Demax[sc.Index] / summ;
	}
	else
	{
		Demarker[sc.Index] = Demarker[sc.Index-1];
	}
}


/*==========================================================================*/
SCSFExport scsf_StochasticRSI(SCStudyInterfaceRef sc)
{

	SCSubgraphRef StochasticRSI = sc.Subgraph[0];
	SCSubgraphRef RSI = sc.Subgraph[1];

	SCInputRef RSILength = sc.Input[0];
	SCInputRef RSIAvgType = sc.Input[1];
	SCInputRef RSIHighestLowestLength = sc.Input[2];

	if(sc.SetDefaults)
	{
		sc.GraphName="Stochastic RSI";

		StochasticRSI.Name="Stochastic RSI";
		StochasticRSI.DrawStyle = DRAWSTYLE_LINE;
		StochasticRSI.PrimaryColor = RGB(0,255,0);
		StochasticRSI.DrawZeros = true;

		RSILength.Name="RSI Length";
		RSILength.SetInt(14);
		RSILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RSIAvgType.Name="RSI Average Type";
		RSIAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		RSIHighestLowestLength.Name = "RSI Highest Lowest Length";
		RSIHighestLowestLength.SetInt(14);
		RSIHighestLowestLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		sc.AutoLoop = 1;

		return;
	}

	sc.DataStartIndex = RSILength.GetInt() + RSIHighestLowestLength.GetInt() - 1; 

	if (RSIHighestLowestLength.GetInt() == 1)
		RSIHighestLowestLength.SetInt(RSILength.GetInt());

	if (sc.Index<1)
	{
		return;
	}

	sc.RSI(sc.Close, RSI, RSIAvgType.GetMovAvgType(), RSILength.GetInt());

	// computing Stochastic of RSI
	float MaximumRSI = sc.GetHighest(RSI, sc.Index, RSIHighestLowestLength.GetInt());
	float MinimumRSI = sc.GetLowest(RSI, sc.Index, RSIHighestLowestLength.GetInt());

	if (MaximumRSI-MinimumRSI != 0.0f)
	{
		StochasticRSI[sc.Index] = (RSI[sc.Index]-MinimumRSI)/(MaximumRSI - MinimumRSI);
	}
	else
	{
		StochasticRSI[sc.Index] = StochasticRSI[sc.Index-1];
	}
}
/*==========================================================================*/
SCSFExport scsf_MinerDTOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SK        = sc.Subgraph[0];
	SCSubgraphRef SD        = sc.Subgraph[1];
	SCSubgraphRef UpperLine = sc.Subgraph[2];
	SCSubgraphRef ZeroLine  = sc.Subgraph[3];
	SCSubgraphRef LowerLine = sc.Subgraph[4];

	SCFloatArrayRef RSI                 = SK.Arrays[0];
	SCFloatArrayRef RsiUpSums           = SK.Arrays[1];
	SCFloatArrayRef RsiDownSums         = SK.Arrays[2];
	SCFloatArrayRef RsiSmoothedUpSums   = SK.Arrays[3];
	SCFloatArrayRef RsiSmoothedDownSums = SK.Arrays[4];
	SCFloatArrayRef StochRSI            = SK.Arrays[5];

	SCInputRef RSILength   = sc.Input[0];
	SCInputRef RSIAvgType  = sc.Input[1];
	SCInputRef StochLength = sc.Input[2];
	SCInputRef SKLength    = sc.Input[3];
	SCInputRef SKAvgType   = sc.Input[4];
	SCInputRef SDLength    = sc.Input[5];
	SCInputRef SDAvgType   = sc.Input[6];
	SCInputRef UpperValue  = sc.Input[7];
	SCInputRef LowerValue  = sc.Input[8];

	if(sc.SetDefaults)
	{
		sc.GraphName="DT Oscillator";
		sc.StudyDescription="DT Oscillator - Robert Miner"; 

		SK.Name="DT Osc SK";
		SK.DrawStyle = DRAWSTYLE_LINE;
		SK.PrimaryColor = COLOR_RED;
		SK.DrawZeros = true;

		SD.Name="DT Osc SD";
		SD.DrawStyle = DRAWSTYLE_LINE;
		SD.PrimaryColor = COLOR_GREEN;
		SD.DrawZeros = true;

		UpperLine.Name="Upper";
		UpperLine.DrawStyle = DRAWSTYLE_LINE;
		UpperLine.LineStyle = LINESTYLE_DOT;
		UpperLine.PrimaryColor = COLOR_GRAY;
		UpperLine.DisplayNameValueInWindowsFlags = 0;
		UpperLine.DrawZeros = true;

		ZeroLine.Name="Zero";
		ZeroLine.DrawStyle = DRAWSTYLE_IGNORE;
		ZeroLine.LineStyle = LINESTYLE_DOT;
		ZeroLine.PrimaryColor = COLOR_GRAY;
		ZeroLine.DisplayNameValueInWindowsFlags = 0;
		ZeroLine.DrawZeros = true;

		LowerLine.Name="Lower";
		LowerLine.DrawStyle = DRAWSTYLE_LINE;
		LowerLine.LineStyle = LINESTYLE_DOT;
		LowerLine.PrimaryColor = COLOR_GRAY;
		LowerLine.DisplayNameValueInWindowsFlags = 0;
		LowerLine.DrawZeros = true;

		RSILength.Name="RSI Length";
		RSILength.SetInt(13);

		RSIAvgType.Name="RSI Average Type";
		RSIAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		StochLength.Name="Stochastic Length";
		StochLength.SetInt(8);

		SKLength.Name="SK Length";
		SKLength.SetInt(5);

		SKAvgType.Name="SK Average Type";
		SKAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		SDLength.Name="SD Length";
		SDLength.SetInt(3);

		SDAvgType.Name="SD Average Type";
		SDAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		UpperValue.Name="Upper Line Value";
		UpperValue.SetFloat(75.0f);

		LowerValue.Name="Lower Line Value";
		LowerValue.SetFloat(25.0f);

		sc.AutoLoop = 1;

		return;
	}

	sc.DataStartIndex = RSILength.GetInt(); 

	UpperLine[sc.Index] = UpperValue.GetFloat();
	ZeroLine[sc.Index]  = 0;
	LowerLine[sc.Index] = LowerValue.GetFloat();

	if (sc.Index<1)
	{
		return;
	}

	sc.RSI(sc.Close, RSI, RsiUpSums, RsiDownSums, RsiSmoothedUpSums, RsiSmoothedDownSums, RSIAvgType.GetMovAvgType(), RSILength.GetInt());

	// computing Stochastic of RSI
	float maxRSI = sc.GetHighest(RSI, sc.Index, StochLength.GetInt());
	float minRSI = sc.GetLowest(RSI, sc.Index, StochLength.GetInt());

	if (maxRSI-minRSI != 0.0f)
	{
		StochRSI[sc.Index] = 100.0f * (RSI[sc.Index]-minRSI)/(maxRSI - minRSI);
	}
	else
	{
		StochRSI[sc.Index] = StochRSI[sc.Index-1];
	}

	sc.MovingAverage(StochRSI, SK, SKAvgType.GetMovAvgType(), SKLength.GetInt());
	sc.MovingAverage(SK, SD, SDAvgType.GetMovAvgType(), SDLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_VerticalTimeLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TimeLine = sc.Subgraph[0];
	SCSubgraphRef TimeLineBottom = sc.Subgraph[1];
	SCSubgraphRef CustomLabel = sc.Subgraph[2];

	SCInputRef TimeOfLine = sc.Input[0];
	SCInputRef DisplayCustomLabel = sc.Input[1];
	SCInputRef VerticalPositionPercent = sc.Input[2];
	SCInputRef Version = sc.Input[3];
	SCInputRef ForwardProject = sc.Input[4];
	SCInputRef AllowedRangeForMatchInMinutes = sc.Input[5];
	SCInputRef MatchingMethod = sc.Input[6];


	if(sc.SetDefaults)
	{
		sc.GraphName="Time Line";
		sc.StudyDescription="Draws a vertical line at the time specified with the Time of Line Input.";

		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 0;
		sc.ScaleRangeType = SCALE_USERDEFINED;
		sc.ScaleRangeTop = 2;
		sc.ScaleRangeBottom = 1;

		TimeLine.Name = "TimeLine";
		TimeLine.DrawStyle = DRAWSTYLE_BAR_TOP;
		TimeLine.PrimaryColor = RGB(127,127,127);
		TimeLine.DrawZeros = false;
		TimeLine.LineWidth = 1;

		TimeLineBottom.Name = "TimeLine";
		TimeLineBottom.DrawStyle = DRAWSTYLE_BAR_BOTTOM;
		TimeLineBottom.PrimaryColor = RGB(127,127,127);
		TimeLineBottom.DrawZeros =  false;
		TimeLineBottom.LineWidth = 1;

		CustomLabel.Name = "Custom Label Color and Text Size";
		CustomLabel.DrawStyle=DRAWSTYLE_CUSTOM_TEXT;
		CustomLabel.PrimaryColor = RGB(127,127,127);
		CustomLabel.LineWidth =  0;

		TimeOfLine.Name = "Time of Line";
		TimeOfLine.SetTime(HMS_TIME(12, 00, 0));

		DisplayCustomLabel.Name = "Display Custom Label";
		DisplayCustomLabel.SetYesNo(false);

		VerticalPositionPercent.Name = "Label Vertical Position %.";
		VerticalPositionPercent.SetInt(5);
		VerticalPositionPercent.SetIntLimits(0,100);

		sc.TextInputName = "Custom Label Text";

		ForwardProject.Name = "Display in Forward Projection Area";
		ForwardProject.SetYesNo(false);

		AllowedRangeForMatchInMinutes.Name = "Allowed Range for Match in Minutes";
		AllowedRangeForMatchInMinutes.SetInt(10);

		MatchingMethod.Name = "Matching Method";
		MatchingMethod.SetCustomInputStrings("Nearest;Containing");
		MatchingMethod.SetCustomInputIndex(0);

		Version.SetInt(3);

		return;
	}

	if (Version.GetInt()<2)
	{
		TimeLine.DrawStyle = DRAWSTYLE_BAR_TOP;
		TimeLineBottom.DrawStyle = DRAWSTYLE_BAR_BOTTOM;
		Version.SetInt(3);
	}

	if (Version.GetInt() < 3)
	{
		AllowedRangeForMatchInMinutes.SetInt(10);
		Version.SetInt(3);
	}

	int NumberForwardBars = 0;
	if(ForwardProject.GetYesNo())
	{
		//Subtracting 1 since the study references one bar ahead
		NumberForwardBars = sc.NumberOfForwardColumns-1;
		TimeLine.ExtendedArrayElementsToGraph = NumberForwardBars;
		TimeLineBottom.ExtendedArrayElementsToGraph = NumberForwardBars;
	}


	int NearestDateTimeIndex = -1;

	for (int Index = sc.UpdateStartIndex; Index < (sc.ArraySize + NumberForwardBars); Index++)
	{

		SCDateTime TimeLineDateTime(sc.BaseDateTimeIn[Index].GetDate(), TimeOfLine.GetTime());

		if(sc.BaseDateTimeIn[Index - 1].GetDate() != sc.BaseDateTimeIn[Index].GetDate())
			NearestDateTimeIndex = -1;

		if(NearestDateTimeIndex == -1)
		{
			if (MatchingMethod.GetIndex() == 0)
			{
				NearestDateTimeIndex
					= sc.GetNearestMatchForSCDateTimeExtended(sc.ChartNumber, TimeLineDateTime);
			}
			else
			{
//				NearestDateTimeIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, TimeLineDateTime);

				if (TimeLineDateTime > sc.BaseDateTimeIn[Index - 1]
					&& TimeLineDateTime <= sc.BaseDateTimeIn[Index])
						NearestDateTimeIndex = Index;
			}
		}

		SCDateTime TimeMargin;
		TimeMargin.SetTimeHMS(0, AllowedRangeForMatchInMinutes.GetInt(), 0);
		SCDateTime EarlierTime = TimeLineDateTime - TimeMargin;
		SCDateTime LaterTime = TimeLineDateTime + TimeMargin;

		if (Index == NearestDateTimeIndex
			&& sc.BaseDateTimeIn[Index] >= EarlierTime
			&& sc.BaseDateTimeIn[Index] <= LaterTime)
		{
			TimeLine[Index] = 100;
			TimeLineBottom[Index] = -100;
		}
		else
		{
			TimeLine[Index] = 0;
			TimeLineBottom[Index] = 0;
		}

		if (TimeLine[Index] == 0)
			continue;


		if(DisplayCustomLabel.GetYesNo() && !sc.HideStudy )
		{

			int &LastUsedIndex = sc.GetPersistentInt(1);
			int &RememberedLineNumber = sc.GetPersistentInt(2);

			if(sc.IsFullRecalculation)
			{
				LastUsedIndex = -1;
				RememberedLineNumber = 0;
			}

			if(Index == LastUsedIndex)
				continue;

			//If at the last used index there is still a vertical line, then we must be drawing a new one so do not use the prior line number and add a new drawing by specifying a 0 line number.
			if (LastUsedIndex != -1 && TimeLine[LastUsedIndex] != 0)
				RememberedLineNumber = 0;


			LastUsedIndex = Index;

			s_UseTool Tool;
			Tool.ChartNumber = sc.ChartNumber;
			Tool.DrawingType = DRAWING_TEXT;
			Tool.Region = sc.GraphRegion;

			Tool.BeginIndex = Index;
			Tool.UseRelativeVerticalValues = true;
			Tool.BeginValue = (float)VerticalPositionPercent.GetInt();
			Tool.Text = sc.TextInput;

			Tool.Color = CustomLabel.PrimaryColor;
			Tool.FontBold = true;
			Tool.FontSize = CustomLabel.LineWidth;
			if (RememberedLineNumber != 0)
			{
				Tool.LineNumber = RememberedLineNumber;
				Tool.AddMethod = UTAM_ADD_OR_ADJUST;
			}
			else
				Tool.AddMethod = UTAM_ADD_ALWAYS;

			sc.UseTool(Tool);
			RememberedLineNumber = Tool.LineNumber;

		}
	}

	return;
}


/*==========================================================================*/
SCSFExport scsf_VerticalDateTimeLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TimeLine = sc.Subgraph[0];
	SCSubgraphRef TimeLineBottom = sc.Subgraph[1];
	SCSubgraphRef CustomLabel = sc.Subgraph[2];

	SCInputRef DateTimeOfLine = sc.Input[0];
	SCInputRef DisplayCustomLabel = sc.Input[1];
	SCInputRef VerticalPositionPercent = sc.Input[2];
	SCInputRef Version = sc.Input[3];
	SCInputRef ForwardProject = sc.Input[4];
	SCInputRef AllowedRangeForMatchInMinutes = sc.Input[5];
	SCInputRef MatchingMethod = sc.Input[6];


	if(sc.SetDefaults)
	{
		sc.GraphName="Date-Time Line";
		sc.StudyDescription="Draws a vertical line at the date-time specified with the Date-Time of Line input.";

		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 0;
		sc.ScaleRangeType = SCALE_USERDEFINED;
		sc.ScaleRangeTop = 2;
		sc.ScaleRangeBottom = 1;

		TimeLine.Name = "TimeLine";
		TimeLine.DrawStyle = DRAWSTYLE_BAR_TOP;
		TimeLine.PrimaryColor = RGB(127,127,127);
		TimeLine.DrawZeros = false;
		TimeLine.LineWidth = 1;

		TimeLineBottom.Name = "TimeLine";
		TimeLineBottom.DrawStyle = DRAWSTYLE_BAR_BOTTOM;
		TimeLineBottom.PrimaryColor = RGB(127,127,127);
		TimeLineBottom.DrawZeros =  false;
		TimeLineBottom.LineWidth = 1;

		CustomLabel.Name = "Custom Label Color and Text Size";
		CustomLabel.DrawStyle=DRAWSTYLE_CUSTOM_TEXT;
		CustomLabel.PrimaryColor = RGB(127,127,127);
		CustomLabel.LineWidth =  0;

		DateTimeOfLine.Name = "Date-Time of Line";
		DateTimeOfLine.SetDateTime(sc.CurrentSystemDateTime);

		DisplayCustomLabel.Name = "Display Custom Label";
		DisplayCustomLabel.SetYesNo(false);

		VerticalPositionPercent.Name = "Label Vertical Position %.";
		VerticalPositionPercent.SetInt(5);
		VerticalPositionPercent.SetIntLimits(0,100);

		sc.TextInputName = "Custom Label Text";

		ForwardProject.Name = "Display in Forward Projection Area";
		ForwardProject.SetYesNo(false);

		AllowedRangeForMatchInMinutes.Name = "Allowed Range for Match in Minutes";
		AllowedRangeForMatchInMinutes.SetInt(10);

		MatchingMethod.Name = "Matching Method";
		MatchingMethod.SetCustomInputStrings("Nearest;Containing");
		MatchingMethod.SetCustomInputIndex(0);

		Version.SetInt(1);

		return;
	}


	int NumberForwardBars = 0;
	if(ForwardProject.GetYesNo())
	{
		// Subtracting 1 since the study references one bar ahead
		NumberForwardBars = sc.NumberOfForwardColumns -1;

		TimeLine.ExtendedArrayElementsToGraph = NumberForwardBars;
		TimeLineBottom.ExtendedArrayElementsToGraph = NumberForwardBars;
	}

	int& ChartDrawingLastUsedIndex = sc.GetPersistentInt(1);

	if(sc.UpdateStartIndex == 0)
		ChartDrawingLastUsedIndex = -1;

	int NearestDateTimeIndex = -1;

	for (int Index = sc.UpdateStartIndex; Index < (sc.ArraySize + NumberForwardBars); Index++)
	{

		SCDateTime TimeLineDateTime(DateTimeOfLine.GetDateTime());


		if(NearestDateTimeIndex == -1)
		{
			if (MatchingMethod.GetIndex() == 0)
				NearestDateTimeIndex = sc.GetNearestMatchForSCDateTimeExtended(sc.ChartNumber, TimeLineDateTime);
			else
			{

				if (TimeLineDateTime > sc.BaseDateTimeIn[Index - 1]
				&& TimeLineDateTime <= sc.BaseDateTimeIn[Index])
					NearestDateTimeIndex = Index;
			}
		}

		SCDateTime TimeMargin;
		TimeMargin.SetTimeHMS(0, AllowedRangeForMatchInMinutes.GetInt(), 0);
		SCDateTime EarlierTime = TimeLineDateTime - TimeMargin;
		SCDateTime LaterTime = TimeLineDateTime + TimeMargin;

		if (Index == NearestDateTimeIndex
			&& sc.BaseDateTimeIn[Index] >= EarlierTime
			&& sc.BaseDateTimeIn[Index] <= LaterTime)
		{
			TimeLine[Index] = 100;
			TimeLineBottom[Index] = -100;
		}
		else
		{
			TimeLine[Index] = 0;
			TimeLineBottom[Index] = 0;
		}

		if (TimeLine[Index] == 0)
			continue;


		if(DisplayCustomLabel.GetYesNo() && !sc.HideStudy )
		{
			if(Index == ChartDrawingLastUsedIndex)
				continue;

			ChartDrawingLastUsedIndex = Index;

			s_UseTool Tool;
			Tool.ChartNumber = sc.ChartNumber;
			Tool.DrawingType = DRAWING_TEXT;
			Tool.Region = sc.GraphRegion;
			//Tool.LineNumber //Automatically set
			Tool.AddMethod = UTAM_ADD_OR_ADJUST;

			Tool.BeginIndex = Index;
			Tool.UseRelativeVerticalValues = true;
			Tool.BeginValue = (float)VerticalPositionPercent.GetInt();
			Tool.Text = sc.TextInput;

			Tool.Color = CustomLabel.PrimaryColor;
			Tool.FontBold = true;
			Tool.FontSize = CustomLabel.LineWidth;
			sc.UseTool(Tool);

		}
	}

	return;
}

/*==========================================================================*/
SCSFExport scsf_RSITradestation(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSIT = sc.Subgraph[0];
	SCSubgraphRef LineUpperLimit = sc.Subgraph[1];
	SCSubgraphRef LineLowerLimit = sc.Subgraph[2];
	SCSubgraphRef RSIAvg = sc.Subgraph[3];
	SCSubgraphRef NetChangeAvg = sc.Subgraph[4];
	SCSubgraphRef TotalChangeAvg = sc.Subgraph[5];

	SCInputRef InputData = sc.Input[0];
	SCInputRef RSILength = sc.Input[1];
	SCInputRef Line1 = sc.Input[3];
	SCInputRef Line2 = sc.Input[4];
	SCInputRef RSIMovAvgLength = sc.Input[5];

	if(sc.SetDefaults)
	{
		sc.GraphName="RSI-TS";
		sc.StudyDescription="RSI implemented as in TradeStation";

		// Outputs
		RSIT.Name="RSI-T";
		RSIT.DrawStyle = DRAWSTYLE_LINE;
		RSIT.PrimaryColor = RGB(0,255,0);
		RSIT.DrawZeros = false;

		LineUpperLimit.Name = "Line1";
		LineUpperLimit.DrawStyle = DRAWSTYLE_LINE;
		LineUpperLimit.PrimaryColor = RGB(255,0,255);
		LineUpperLimit.DrawZeros = false;

		LineLowerLimit.Name = "Line2";
		LineLowerLimit.DrawStyle = DRAWSTYLE_LINE;
		LineLowerLimit.PrimaryColor = RGB(255,255,0);
		LineLowerLimit.DrawZeros = false;

		RSIAvg.Name = "RSI Avg";
		RSIAvg.DrawStyle = DRAWSTYLE_LINE;
		RSIAvg.PrimaryColor = RGB(255,127,0);
		RSIAvg.DrawZeros = false;

		// Inputs
		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		RSILength.Name="RSI Length";
		RSILength.SetInt(10);

		Line1.Name="Line 1";
		Line1.SetFloat(70);
		Line1.SetFloatLimits(50, 100);

		Line2.Name="Line 2";
		Line2.SetFloat(30);
		Line2.SetFloatLimits(0, 50);

		RSIMovAvgLength.Name = "RSI Mov Avg Length";
		RSIMovAvgLength.SetInt(3);
		RSIMovAvgLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		return;
	}

	// start charting after Length of elements is available
	sc.DataStartIndex = RSILength.GetInt(); 
	if (sc.Index<RSILength.GetInt())
		return;

	SCFloatArrayRef  price = sc.BaseData[InputData.GetInputDataIndex()];

	// smoothing factor
	float smoothingFactor = 1.0f / RSILength.GetInt();

	if (sc.Index<RSILength.GetInt())
		return;

	// compute net and total changes

	float totalChange;
	float netChange;

	if (sc.Index==RSILength.GetInt())
	{
		// NetChgAvg = (Price - Price[Length] ) / Length
		netChange = (price[sc.Index] - price[sc.Index - RSILength.GetInt()]) / RSILength.GetInt();

		// TotChgAvg = Average( AbsValue( Price - Price[1] ), Length )
		float average = 0.0;
		for (int i=1; i<=sc.Index; i++)
		{
			average += abs(price[i] - price[i - 1]);
		}       
		totalChange = average / RSILength.GetInt();
	}
	else
	{
		// Change = Price - Price[1]
		float change = price[sc.Index] - price[sc.Index - 1];

		// NetChgAvg = NetChgAvg[1] + SF * ( Change - NetChgAvg[1] )
		float previousNetChange = NetChangeAvg[sc.Index - 1];
		netChange =  previousNetChange + smoothingFactor * ( change - previousNetChange );

		// TotChgAvg = TotChgAvg[1] + SF * ( AbsValue(Change) - TotChgAvg[1] )
		float previousTotalChangeAverage = TotalChangeAvg[sc.Index - 1];
		totalChange = previousTotalChangeAverage + smoothingFactor * ( abs(change) - previousTotalChangeAverage );
	}

	TotalChangeAvg[sc.Index] = totalChange;
	NetChangeAvg[sc.Index] = netChange;

	// compute the RSI value
	float changeRatio = 0;
	if (totalChange!=0.0f)
	{
		changeRatio = netChange / totalChange;
	}
	else
	{
		changeRatio = 0.0f;
	}

	// RSI = 50 * ( ChgRatio + 1)
	RSIT[sc.Index] = 50 * (changeRatio + 1.0f);

	// Draw overbought / oversold indicators
	LineUpperLimit[sc.Index] = Line1.GetFloat();
	LineLowerLimit[sc.Index] = Line2.GetFloat();

	if (sc.Index >= RSIMovAvgLength.GetInt() + RSILength.GetInt())
	{
		// Draw smoothed RSI
		sc.MovingAverage(RSIT, RSIAvg, MOVAVGTYPE_SIMPLE, sc.Index, RSIMovAvgLength.GetInt());
	}
}

/*==========================================================================*/
/*
Triple Exponential Moving Average, by Patrick Mulloy and published in January 1994 
in the Stocks & Commodities magazine

Algorithm is also described in http://www.visualchart.com/enxx/strategies/indicators/ayuda.asp?Id=2857
*/
SCSFExport scsf_MovingAverageTEMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TEMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName="Moving Average - Triple Exp";
		sc.StudyDescription="Triple Exponential Moving Average";

		sc.GraphRegion = 0;

		TEMA.Name = "TEMA";
		TEMA.DrawStyle = DRAWSTYLE_LINE;
		TEMA.PrimaryColor = RGB(255, 0, 0);
		TEMA.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		Length.SetInt(5);

		sc.AutoLoop = 1;

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	SCFloatArrayRef data = sc.BaseData[InputData.GetInputDataIndex()];
	sc.TEMA(data,TEMA,sc.Index,Length.GetInt());
}

/*==========================================================================*/

// http://www.danielstrading.com/education/technical-analysis/historic-volatility.php


SCSFExport scsf_HistoricVolatilityStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volatility = sc.Subgraph[0];
	SCSubgraphRef LogPriceChange = sc.Subgraph[1];
	SCSubgraphRef SMALog = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef AlgorithmType = sc.Input[1];
	SCInputRef Length = sc.Input[2];
	SCInputRef NumberBarsPerYear = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName="Volatility - Historical";

		Volatility.Name = "Volatility";
		Volatility.DrawStyle = DRAWSTYLE_LINE;
		Volatility.PrimaryColor = RGB(0, 255,0);
		Volatility.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);


		Length.Name = "Length";
		Length.SetIntLimits(2,MAX_STUDY_LENGTH);
		Length.SetInt(20);

		NumberBarsPerYear.Name = "Number of Bars Per Year";
		NumberBarsPerYear.SetInt(262);

		sc.AutoLoop = 1;
		sc.ValueFormat = 2;

		return;
	}

	sc.DataStartIndex = Length.GetInt();


	// calculate the logarithm of the price change    
	LogPriceChange[sc.Index] = log(sc.BaseData[InputData.GetInputDataIndex()][sc.Index] / sc.BaseData[InputData.GetInputDataIndex()][sc.Index - 1]);

	// calculate the average of the logs
	sc.SimpleMovAvg(LogPriceChange, SMALog, sc.Index, Length.GetInt());

	// sum the squares of the difference between the individual logarithms for each period and the average logarithm
	float SumSquareOfDifference = 0;
	for (int SumIndex = max(0, sc.Index - Length.GetInt() + 1); SumIndex <= sc.Index; SumIndex++)
	{
		float diff = LogPriceChange[SumIndex] - SMALog[sc.Index];
		SumSquareOfDifference += diff * diff;
	}

	// Final volatility calculation
	Volatility[sc.Index] = sqrt(SumSquareOfDifference / float(Length.GetInt() - 1)) * sqrt(float(NumberBarsPerYear.GetInt())) * 100;


}


/******************************************************************************

Synthetic VIX indicator as described in the December 2007 issue of 
"Active Trader" magazine.  The indicator is also named the "Williams VIX Fix" 
since Larry Williams is credited with the formula's discovery.

Formula:

SynthVIX = (Highest(Close,22) - Low) / (Highest(Close,22)) * 100;

*******************************************************************************/
SCSFExport scsf_SyntheticVIX(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SynthVIX = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];

	if(sc.SetDefaults)
	{
		sc.GraphName="Synthetic VIX";
		sc.StudyDescription="Synthetic VIX";

		SynthVIX.Name = "SynthVIX";
		SynthVIX.DrawStyle = DRAWSTYLE_LINE;
		SynthVIX.PrimaryColor = RGB(255, 255, 0);
		SynthVIX.DrawZeros = false;

		Length.Name = "Length";
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);
		Length.SetInt(22);
		
		sc.AutoLoop = 1;

		return;
	}

	float HighestClose = sc.GetHighest(sc.Close, sc.Index, Length.GetInt());

	SynthVIX[sc.Index] = 100.0f * (HighestClose - sc.Low[sc.Index]) / HighestClose;
}

/*==========================================================================*/
void PassingSCStrutureExampleFunction(SCStudyInterfaceRef sc)
{
	//The "sc" structure can be used anywhere within this function.
}

/*============================================================================
	"An example of calling a function that receives the Sierra Chart ACSIL structure (sc)."
----------------------------------------------------------------------------*/
SCSFExport scsf_PassingSCStructureExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Passing sc Structure Example Function";
		
		sc.StudyDescription = "An example of calling a function that receives the Sierra Chart ACSIL structure (sc).";

		return;
	}
	
	// Do data processing

	// The definition of the function called below must be above this function.
	PassingSCStrutureExampleFunction(sc);
}

/*==========================================================================*/
SCSFExport scsf_InvertStudy(SCStudyInterfaceRef sc)
{


	SCSubgraphRef Open = sc.Subgraph[SC_OPEN];
	SCSubgraphRef High = sc.Subgraph[SC_HIGH];
	SCSubgraphRef Low = sc.Subgraph[SC_LOW];
	SCSubgraphRef Last = sc.Subgraph[SC_LAST];
	SCSubgraphRef Volume = sc.Subgraph[SC_VOLUME];
	SCSubgraphRef OHLCAvg = sc.Subgraph[SC_OHLC_AVG];
	SCSubgraphRef HLCAvg = sc.Subgraph[SC_HLC_AVG];
	SCSubgraphRef HLAvg = sc.Subgraph[SC_HL_AVG];
	
	SCSubgraphRef NumTrades = sc.Subgraph[SC_NUM_TRADES];
	SCSubgraphRef BidVol = sc.Subgraph[ SC_BIDVOL];
	SCSubgraphRef AskVol = sc.Subgraph[SC_ASKVOL];


	if(sc.SetDefaults)
	{
		sc.GraphName="Multiply Bars By -1";
		
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = true;
		sc.GraphUsesChartColors = true;

		return;
	}

	if (sc.IsFullRecalculation && sc.Index == 0)
	{
		sc.GraphName.Format("%s Inverted", sc.GetStudyName(0).GetChars());

		for (int SubgraphIndex = 0; SubgraphIndex <= SC_ASKVOL; ++SubgraphIndex)
		{
			sc.Subgraph[SubgraphIndex].Name = sc.GetStudySubgraphName(0, SubgraphIndex);
			sc.Subgraph[SubgraphIndex].DrawZeros = false;
			if(SubgraphIndex < SC_VOLUME)
				sc.Subgraph[SubgraphIndex].DrawStyle = DRAWSTYLE_LINE;
			else
				sc.Subgraph[SubgraphIndex].DrawStyle = DRAWSTYLE_IGNORE;
		}
	}

	Open[sc.Index] = sc.Open[sc.Index] * -1.0f;
	Last[sc.Index] = sc.Close[sc.Index] * -1.0f;
	High[sc.Index] = sc.High[sc.Index] * -1.0f;
	Low[sc.Index] = sc.Low[sc.Index] * -1.0f;
	OHLCAvg[sc.Index] = sc.OHLCAvg[sc.Index] * -1.0f;
	HLCAvg[sc.Index] = sc.HLCAvg[sc.Index] * -1.0f;
	HLAvg[sc.Index] = sc.HLAvg[sc.Index] * -1.0f;

	Volume[sc.Index] = sc.Volume[sc.Index];
	NumTrades[sc.Index] = sc.NumberOfTrades[sc.Index];
	BidVol[sc.Index] = sc.BidVolume[sc.Index];
	AskVol[sc.Index] = sc.AskVolume[sc.Index];	
}

/*==========================================================================*/
SCSFExport scsf_BarsInTicks(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "Bars in Ticks";
		//sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_WHOLE_NUMBER;
		sc.AutoLoop = 1;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = true;
		sc.GraphUsesChartColors = true;

		return;
	}

	if (sc.IsFullRecalculation && sc.Index == 0)
	{
		sc.GraphName.Format("%s in Ticks", sc.GetStudyName(0).GetChars());

		for (int SubgraphIndex = 0; SubgraphIndex <= NUM_BASE_GRAPH_ARRAYS; ++SubgraphIndex)
		{
			sc.Subgraph[SubgraphIndex].Name = sc.GetStudySubgraphName(0, SubgraphIndex);
		}

	}

	for (int SubGraph = 0; SubGraph <= NUM_BASE_GRAPH_ARRAYS; SubGraph++)
	{
		if (SubGraph == SC_VOLUME || SubGraph == SC_NUM_TRADES || SubGraph == SC_BIDVOL || SubGraph == SC_ASKVOL)
			sc.Subgraph[SubGraph][sc.Index] = sc.BaseDataIn[SubGraph][sc.Index];
		else
		{
			
			if (sc.RenkoTicksPerBar != 0 && SubGraph == SC_OPEN)
				sc.Subgraph[SubGraph][sc.Index] = sc.BaseDataIn[SC_RENKO_OPEN][sc.Index] / sc.TickSize;
			else if (sc.RenkoTicksPerBar != 0 && SubGraph == SC_LAST)
				sc.Subgraph[SubGraph][sc.Index] = sc.BaseDataIn[SC_RENKO_CLOSE][sc.Index] / sc.TickSize;
			else
				sc.Subgraph[SubGraph][sc.Index] = sc.BaseDataIn[SubGraph][sc.Index] / sc.TickSize;
		}
	}
}

/*==========================================================================*/

SCSFExport scsf_StudySubgraphInTicks(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph in Ticks";
		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_WHOLE_NUMBER;
		sc.GraphDrawType = GDT_CUSTOM;

		sc.AutoLoop = 0;

		Result.Name = "Ticks";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB( 255, 165, 0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		return;
	}


	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		Result[Index] =  (float)sc.Round(sc.BaseData[InputData.GetInputDataIndex()][Index] / sc.TickSize);
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;


}


/*==========================================================================*/

SCSFExport scsf_StudySubgraphAsCurrencyValue(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph as Currency Value";
		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_2_DIGITS;
		sc.GraphDrawType = GDT_CUSTOM;

		sc.AutoLoop = 0;

		Result.Name = "Currency Value";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(255, 165, 0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		return;
	}


	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		Result[Index] = (float) (sc.Round(sc.BaseData[InputData.GetInputDataIndex()][Index] / sc.TickSize)* sc.CurrencyValuePerTick);
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;


}

/*==========================================================================*/
SCSFExport scsf_ChandeMomentumOscillator(SCStudyInterfaceRef sc)
{
	enum InputIndexes
	{
		INPUT_DATA= 0,
		INPUT_CMO_LENGTH,
		INPUT_LINE_LEVEL2,
		INPUT_LINE_LEVEL1,
		INPUT_LINE_LEVEL3
	};

	SCSubgraphRef CMO = sc.Subgraph[0];
	SCSubgraphRef Line2 = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line3 = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef CMOLength = sc.Input[1];
	SCInputRef Line1Level = sc.Input[2];
	SCInputRef Line2Level = sc.Input[3];
	SCInputRef Line3Level = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName="Chande Momentum Oscillator";
		sc.StudyDescription="Chande Momentum Oscillator";


		CMO.Name="CMO";
		CMO.DrawStyle = DRAWSTYLE_LINE;
		CMO.PrimaryColor = RGB(0,255,0);
		CMO.DrawZeros = true;

		Line2.Name="Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,0,0);
		Line2.DrawZeros = true;

		Line1.Name="Line 1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,0,0);
		Line1.DrawZeros = true;

		Line3.Name="Line 3";
		Line3.DrawStyle = DRAWSTYLE_LINE;
		Line3.PrimaryColor = RGB(255,0,0);
		Line3.DrawZeros = true;

		InputData.Name="Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		CMOLength.Name="CMO Length";
		CMOLength.SetInt(14);

		Line1Level.Name="Line 1 level";
		Line1Level.SetFloat(50.0f);

		Line2Level.Name="Line 2 level";
		Line2Level.SetFloat(0.0f);

		Line3Level.Name="Line 3 level";
		Line3Level.SetFloat(-50.0f);

		sc.AutoLoop = 1;

		return;
	}
	sc.DataStartIndex = CMOLength.GetInt();

	if (sc.Index < CMOLength.GetInt()-1)
		return;

	float sum_up = 0;
	float sum_down = 0;
	for (int j = (sc.Index-CMOLength.GetInt()+1);j<=sc.Index;j++)
	{
		if (sc.BaseData[InputData.GetInputDataIndex()][j]<sc.BaseData[InputData.GetInputDataIndex()][j-1])
		{
			sum_down += sc.BaseData[InputData.GetInputDataIndex()][j-1] - sc.BaseData[InputData.GetInputDataIndex()][j];
		}
		else if (sc.BaseData[InputData.GetInputDataIndex()][j]>sc.BaseData[InputData.GetInputDataIndex()][j-1])
		{
			sum_up += sc.BaseData[InputData.GetInputDataIndex()][j] - sc.BaseData[InputData.GetInputDataIndex()][j-1];
		}
	}
	//CMO(t) = 100 * ((SumUp(t) - SumDown(t)) / (SumUp(t) + SumDown(t)))
	float fCMO;

	if(sum_up+sum_down == 0)
		fCMO = CMO[sc.Index-1];
	else
		fCMO = 100.0f*((sum_up-sum_down)/(sum_up+sum_down));

	CMO[sc.Index] = fCMO;
	Line2[sc.Index] = Line2Level.GetFloat();
	Line1[sc.Index] = Line1Level.GetFloat();
	Line3[sc.Index] = Line3Level.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_ZeroLagEMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ZeroLagEMA = sc.Subgraph[0];
	SCFloatArrayRef DataLagDifference = sc.Subgraph[0].Arrays[0];

	SCInputRef Period = sc.Input[0];
	SCInputRef InputData = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName="Moving Average - Zero Lag Exponential";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		ZeroLagEMA.Name="Zero Lag EMA";
		ZeroLagEMA.DrawStyle = DRAWSTYLE_LINE;
		ZeroLagEMA.PrimaryColor = RGB(0,255,0);
		ZeroLagEMA.DrawZeros = false;

		Period.Name="Zero Lag EMA Length";
		Period.SetInt(10);
		Period.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		return;
	}

	sc.DataStartIndex = Period.GetInt()-1;

	int Lag = sc.Round(( Period.GetInt() - 1)/2.0f);

	DataLagDifference[sc.Index] = 2 * sc.BaseData[InputData.GetInputDataIndex()][sc.Index] - sc.BaseData[InputData.GetInputDataIndex()][sc.Index - Lag];

	sc.MovingAverage(DataLagDifference, ZeroLagEMA,MOVAVGTYPE_EXPONENTIAL, sc.Index, Period.GetInt());


}
/*==========================================================================*/
SCSFExport scsf_Tenkan_Sen(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TenkanSen = sc.Subgraph[0];
	SCFloatArrayRef HH = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef LL = sc.Subgraph[0].Arrays[1];

	SCInputRef Period = sc.Input[0];
	SCInputRef InputDataHigh = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];
	SCInputRef Version = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName="Tenkan-Sen";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		TenkanSen.Name="Tenkan-Sen";
		TenkanSen.DrawStyle = DRAWSTYLE_LINE;
		TenkanSen.LineWidth = 2;
		TenkanSen.PrimaryColor = RGB(255,128,0);
		TenkanSen.DrawZeros = false;

		Period.Name="Tenkan-Sen Length";
		Period.SetInt(9);
		Period.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		Version.SetInt(1);

		return;
	}

	if (Version.GetInt()<1)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		Version.  SetInt(1);
	}

	sc.DataStartIndex = Period.GetInt()-1;

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()], HH, Period.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()], LL, Period.GetInt());
	TenkanSen[sc.Index] = (HH[sc.Index]+LL[sc.Index])/2.0f;

}
/*==========================================================================*/
SCSFExport scsf_Kijun_Sen(SCStudyInterfaceRef sc)
{
	SCSubgraphRef KijunSen = sc.Subgraph[0];
	SCFloatArrayRef HH = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef LL = sc.Subgraph[0].Arrays[1];
	

	SCInputRef Period = sc.Input[0];
	SCInputRef InputDataHigh = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];
	SCInputRef Version = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName="Kijun-Sen";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		KijunSen.Name="Kijun-Sen";
		KijunSen.DrawStyle = DRAWSTYLE_LINE;
		KijunSen.LineWidth = 2;
		KijunSen.PrimaryColor = RGB(255,0,128);
		KijunSen.DrawZeros = false;

		Period.Name="Kijun-Sen Length";
		Period.SetInt(26);
		Period.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		Version.SetInt(1);

		return;
	}

	if (Version.GetInt()<1)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);

		Version.  SetInt(1);
	}

	sc.DataStartIndex = Period.GetInt()-1;

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],HH, Period.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],LL, Period.GetInt());
	KijunSen[sc.Index] = (HH[sc.Index] + LL[sc.Index])/2.0f;


}
/*==========================================================================*/
SCSFExport scsf_ChikouSpan(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ChikouSpan = sc.Subgraph[0];

	SCInputRef Period = sc.Input[0];
	SCInputRef InputData = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName="Chikou Span";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		ChikouSpan.Name="Chikou Span";
		ChikouSpan.DrawStyle = DRAWSTYLE_LINE;
		ChikouSpan.LineWidth = 2;
		ChikouSpan.PrimaryColor = RGB(20,180,240);
		ChikouSpan.DrawZeros = false;
		ChikouSpan.GraphicalDisplacement = 0;

		Period.Name="Chikou Span Length";
		Period.SetInt(26);
		Period.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		return;
	}

	//ChikouSpan.GraphicalDisplacement = Period.GetInt()*(-1);

	int OutputIndex = sc.Index - Period.GetInt();

	//Chikou Span (Lagging Span): Close plotted 26 days in the past
	ChikouSpan[OutputIndex] = sc.BaseData[InputData.GetInputDataIndex()][sc.Index];

}

/*==========================================================================*/
SCSFExport scsf_SenkouSpanA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SpanA = sc.Subgraph[0];
	SCSubgraphRef TenkanHH = sc.Subgraph[1];
	SCSubgraphRef TenkanLL = sc.Subgraph[2];
	SCSubgraphRef KijunHH = sc.Subgraph[3];
	SCSubgraphRef KijunLL = sc.Subgraph[4];
	
	SCInputRef TenkanPeriod = sc.Input[1];
	SCInputRef KijunPeriod = sc.Input[2];

	
	if(sc.SetDefaults)
	{
		sc.GraphName="Senkou Span A";
		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		SpanA.GraphicalDisplacement = 26;
		SpanA.Name="Span A";
		SpanA.DrawStyle = DRAWSTYLE_LINE;
		SpanA.LineWidth = 4;
		SpanA.PrimaryColor = RGB(0,85,0);
		SpanA.DrawZeros = false;

		TenkanPeriod.Name="Tenkan-Sen Length";
		TenkanPeriod.SetInt(9);
		TenkanPeriod.SetIntLimits(1,MAX_STUDY_LENGTH);

		KijunPeriod.Name="Kijun-Sen Length";
		KijunPeriod.SetInt(26);
		KijunPeriod.SetIntLimits(1,MAX_STUDY_LENGTH);

		return;
	}


	sc.Highest(sc.BaseData[SC_HIGH],TenkanHH, TenkanPeriod.GetInt());
	sc.Lowest(sc.BaseData[SC_LOW],TenkanLL, TenkanPeriod.GetInt());
	sc.Highest(sc.BaseData[SC_HIGH],KijunHH, KijunPeriod.GetInt());
	sc.Lowest(sc.BaseData[SC_LOW],KijunLL, KijunPeriod.GetInt());

	SpanA[sc.Index] = (
		( 
		( (TenkanHH[sc.Index]+TenkanLL[sc.Index])/ 2.0f)+
		( (KijunHH[sc.Index]+KijunLL[sc.Index])/ 2.0f)
		)/2.0f);
}

/*==========================================================================*/
SCSFExport scsf_SenkouSpan(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SpanA = sc.Subgraph[0];
	SCSubgraphRef SpanB = sc.Subgraph[1];
	SCSubgraphRef SpanBHH = sc.Subgraph[2];
	SCSubgraphRef SpanBLL = sc.Subgraph[3];
	SCSubgraphRef TenkanHH = sc.Subgraph[4];
	SCSubgraphRef TenkanLL = sc.Subgraph[5];
	SCSubgraphRef KijunHH = sc.Subgraph[6];
	SCSubgraphRef KijunLL = sc.Subgraph[7];

	SCInputRef InputDataHigh= sc.Input[0];
	SCInputRef SpanBLength = sc.Input[1];
	SCInputRef TenkanLength = sc.Input[2];
	SCInputRef KijunLength = sc.Input[3];
	SCInputRef InputDataLow = sc.Input[4];
	SCInputRef Version = sc.Input[5];

	if(sc.SetDefaults)
	{
		sc.GraphName="Senkou Span A & B";
		
		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		SpanA.Name="Span A";
		SpanA.DrawStyle = DRAWSTYLE_FILL_TOP;
		SpanA.PrimaryColor = RGB(133,205,24);
		SpanA.GraphicalDisplacement = 26;
		SpanA.DrawZeros = false;

		SpanB.Name="Span B";
		SpanB.DrawStyle = DRAWSTYLE_FILL_BOTTOM;
		SpanB.PrimaryColor = RGB(133,205,24);
		SpanB.GraphicalDisplacement = 26;
		SpanB.DrawZeros = false;

		int DisplayOrder = 1;
		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataHigh.DisplayOrder = DisplayOrder++;

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);
		InputDataLow.DisplayOrder = DisplayOrder++;

		SpanBLength.Name="Senkou Span B Length";
		SpanBLength.SetInt(52);
		SpanBLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		SpanBLength.DisplayOrder = DisplayOrder++;

		TenkanLength.Name="Tenkan-Sen Length";
		TenkanLength.SetInt(9);
		TenkanLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		TenkanLength.DisplayOrder = DisplayOrder++;

		KijunLength.Name="Kijun-Sen Length";
		KijunLength.SetInt(26);
		KijunLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		KijunLength.DisplayOrder = DisplayOrder++;

		Version.SetInt(1);

		return;
	}

	if (Version.GetInt() <1)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		Version.SetInt(1);
	}

	// Senkou Span A

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],TenkanHH, TenkanLength.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],TenkanLL, TenkanLength.GetInt());

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],KijunHH, KijunLength.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],KijunLL, KijunLength.GetInt());

	float  Tenkan=(TenkanHH[sc.Index]+TenkanLL[sc.Index]) / 2.0f;
	float Kijun=(KijunHH[sc.Index]+KijunLL[sc.Index]) / 2.0f;
	SpanA[sc.Index] = (Tenkan +Kijun) / 2.0f;



	// Senkou Span B

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],SpanBHH, SpanBLength.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],SpanBLL, SpanBLength.GetInt());

	SpanB[sc.Index] = (SpanBHH[sc.Index]+SpanBLL[sc.Index])/2.0f ;

}

/*==========================================================================*/
SCSFExport scsf_SenkouSpanB(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SpanB = sc.Subgraph[0];
	SCSubgraphRef HH = sc.Subgraph[1];
	SCSubgraphRef LL = sc.Subgraph[2];

	SCInputRef InputDataHigh = sc.Input[0];
	SCInputRef StudyLength = sc.Input[1];
	SCInputRef InputDataLow = sc.Input[2];
	SCInputRef HiddenFlag = sc.Input[4];

	

	if(sc.SetDefaults)
	{
		sc.GraphName="Senkou Span B";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		SpanB.GraphicalDisplacement = 26;
		SpanB.Name="Span B";
		SpanB.DrawStyle = DRAWSTYLE_LINE;
		SpanB.LineWidth = 4;
		SpanB.PrimaryColor = RGB(133,205,24);
		SpanB.DrawZeros = false;

		StudyLength.Name="Senkou Span B Length";
		StudyLength.SetInt(52);
		StudyLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		HiddenFlag.SetInt(2);

		return;
	} 
	

	if (HiddenFlag.GetInt() <2)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		HiddenFlag.SetInt(2);
	}

	sc.Highest(sc.BaseData[InputDataHigh.GetInputDataIndex()],HH, StudyLength.GetInt());
	sc.Lowest(sc.BaseData[InputDataLow.GetInputDataIndex()],LL, StudyLength.GetInt());

	SpanB[sc.Index] =  (HH[sc.Index]+LL[sc.Index])/2.0f ;

}
/*==========================================================================*/
// This study function colors bars according to the slope of the Study Subgraph it is based on.

SCSFExport scsf_ColorBarBasedOnSlope(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ColorBar = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef ModeInput = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Color Bar Based On Slope";

		sc.StudyDescription = "Colors the main price graph bars based on the slope of a study Subgraph this study is Based On.";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		ColorBar.Name = "Color Bar";
		ColorBar.DrawStyle = DRAWSTYLE_COLOR_BAR;
		ColorBar.SecondaryColorUsed = 1;  // true
		ColorBar.PrimaryColor = RGB(0,255,0);
		ColorBar.SecondaryColor = RGB(255,0,0);
		ColorBar.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		ModeInput.Name = "Use +1 (+slope) and -1 (-slope) for Color Bar Values";
		ModeInput.SetYesNo(0);

		return;
	}
	
	// Do data processing

	if (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] > sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1])
	{
		ColorBar[sc.Index] = 1;
		ColorBar.DataColor[sc.Index] = ColorBar.PrimaryColor;
	}
	else if (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] < sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1])
	{
		if (ModeInput.GetYesNo()== false)
			ColorBar[sc.Index] = 1;
		else
			ColorBar[sc.Index] = -1;

		ColorBar.DataColor[sc.Index] = ColorBar.SecondaryColor;
	}
	else
	{
		ColorBar[sc.Index] = 0;
	}
	
}

/*==========================================================================*/

SCFloatArrayRef MyCCI(SCStudyInterfaceRef sc, SCFloatArrayRef In, SCFloatArrayRef SMAOut, SCFloatArrayRef CCIOut, int Index, int Length, float Multiplier)
{
	if (Length < 1)
		return CCIOut;
	
	if (Index < 2*Length - 1)
		return CCIOut;
	
	sc.SimpleMovAvg(In, SMAOut, Index, Length);
	
	float Num0 = 0;
	for (int j = Index; j > Index - Length && j >= 0; j--)
		Num0 += fabs(SMAOut[Index] - In[j]);
	
	Num0 /= Length;
	
	CCIOut[Index] = (In[Index] - SMAOut[Index]) / (Num0 * Multiplier);
	
	return CCIOut;
}

/*==========================================================================*/
SCSFExport scsf_CCI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CCI = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];

	if(sc.SetDefaults)
	{
		sc.GraphName="Commodity Channel Index";
		sc.StudyDescription="This is an example on how to call a function from your study that is not a member of the 'sc' structure.  It calculates the Commodity Channel Index.";


		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		CCI.Name="CCI";
		CCI.DrawStyle = DRAWSTYLE_LINE;
		CCI.PrimaryColor = RGB(0,255,0);
		CCI.DrawZeros = true;

		Length.Name="Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt();
	
	//We will call a custom CCI function.

	MyCCI(sc, sc.Close, CCI.Arrays[0], CCI, sc.Index, Length.GetInt(), 0.015f);
}

/*==========================================================================*/
SCSFExport scsf_DMI_ADX_ADXR(SCStudyInterfaceRef sc)
{
	SCSubgraphRef DmiPlus = sc.Subgraph[0];
	SCSubgraphRef DmiMinus = sc.Subgraph[1];
	SCSubgraphRef ADX = sc.Subgraph[2];
	SCSubgraphRef ADXR = sc.Subgraph[3];

	SCInputRef Length = sc.Input[3];
	SCInputRef MALength = sc.Input[4];
	SCInputRef ADXRInterval = sc.Input[5];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "DMI & ADX & ADXR";

		sc.StudyDescription = "DMI: Welles Wilder's Plus and Minus Directional Indicators +DI and -DI.\n "
								"ADX: Average Directional Movement Index.  This is calculated according to the Welles Wilder formulas.\n"
								"ADXR: Average Directional Movement Index Rating.  This is calculated according to the Welles Wilder formulas.";

		sc.AutoLoop = 1;  // true



		DmiPlus.Name = "DI+";
		DmiPlus.DrawStyle = DRAWSTYLE_LINE;
		DmiPlus.PrimaryColor = RGB(0,255,0);
		DmiPlus.DrawZeros = true;

		DmiMinus.Name = "DI-";
		DmiMinus.DrawStyle = DRAWSTYLE_LINE;
		DmiMinus.PrimaryColor = RGB(255,0,255);
		DmiMinus.DrawZeros = true;

		ADX.Name = "ADX";
		ADX.DrawStyle = DRAWSTYLE_LINE;
		ADX.PrimaryColor = RGB(255,255,0);
		ADX.DrawZeros = true;

		ADXR.Name = "ADXR";
		ADXR.DrawStyle = DRAWSTYLE_LINE;
		ADXR.PrimaryColor = RGB(255,127,0);
		ADXR.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, INT_MAX);

		MALength.Name = "Mov Avg Length";
		MALength.SetInt(14);
		MALength.SetIntLimits(1, INT_MAX);

		ADXRInterval.Name = "ADXR Interval";
		ADXRInterval.SetInt(14);
		ADXRInterval.SetIntLimits(1, INT_MAX);

		return;
	}

	// Do data processing

	sc.DataStartIndex = max(max(Length.GetInt(), Length.GetInt() + MALength.GetInt() + ADXRInterval.GetInt() - 2), Length.GetInt() + MALength.GetInt() - 1);

	sc.DMI(sc.BaseData, DmiPlus, DmiMinus, sc.Index, Length.GetInt());

	sc.ADX(sc.BaseData, ADX, sc.Index, Length.GetInt(), MALength.GetInt());

	sc.ADXR(sc.BaseData, ADXR, sc.Index, Length.GetInt(), MALength.GetInt(), ADXRInterval.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_ColorBarHHLL(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ColorBar = sc.Subgraph[0];
	SCSubgraphRef AverageColor = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Color Bar HH/LL";
		
		sc.StudyDescription = "This study function colors bars based on if they made a higher high or lower low.";
		
		sc.GraphRegion = 0;
		
		ColorBar.Name = "Color Bar";
		ColorBar.DrawStyle = DRAWSTYLE_COLOR_BAR;
		ColorBar.SecondaryColorUsed = 1;  // true
		ColorBar.PrimaryColor = RGB(0,255,0);
		ColorBar.SecondaryColor = RGB(255,0,0);

		AverageColor.Name = "Average Color";
		AverageColor.DrawStyle = DRAWSTYLE_COLOR_BAR;
		AverageColor.PrimaryColor = RGB(255,128,0);


		sc.AutoLoop = 1;

		return;
	}

	if (AverageColor.PrimaryColor == 0)
		AverageColor.PrimaryColor = RGB(255, 128, 0);
		
	// Do data processing
		
	float HighValue = sc.High[sc.Index];
	float LowValue = sc.Low[sc.Index];
	float PrevHighValue = sc.High[sc.Index-1];
	float PrevLowValue = sc.Low[sc.Index-1];
	
	if (HighValue > PrevHighValue && LowValue < PrevLowValue)
	{
		ColorBar[sc.Index] = 1;

		//COLORREF Pri = ColorBar.PrimaryColor;
		//COLORREF Sec = ColorBar.SecondaryColor;
		ColorBar.DataColor[sc.Index] = AverageColor.PrimaryColor;//sc.RGBInterpolate(Pri, Sec, 0.5f);
	}
	else if (HighValue > PrevHighValue)
	{
		ColorBar[sc.Index] = 1;
		ColorBar.DataColor[sc.Index] = ColorBar.PrimaryColor;
	}
	else if (LowValue < PrevLowValue)
	{
		ColorBar[sc.Index] = 1;
		ColorBar.DataColor[sc.Index] = ColorBar.SecondaryColor;
	}
	else
	{
		ColorBar[sc.Index] = 0;
	}
}

/*==========================================================================*/
SCSFExport scsf_ColorVolumeBasedOnSlope(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ColorVolume = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Color Volume Based On Slope";
		sc.StudyDescription = "Colors Volume bars based on the slope of the study subgraph it is Based On.";

		sc.GraphRegion = 1;
		sc.AutoLoop = 1;

		ColorVolume.Name = "Color Volume";
		ColorVolume.DrawStyle = DRAWSTYLE_BAR;
		ColorVolume.PrimaryColor = RGB(0,255,0);
		ColorVolume.SecondaryColorUsed = 1;  // true
		ColorVolume.SecondaryColor = RGB(0,127,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(0);

		return;
	}


	// Do data processing

	if (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] > sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1])
	{
		ColorVolume[sc.Index] = sc.Volume[sc.Index]; 
		ColorVolume.DataColor[sc.Index] = ColorVolume.PrimaryColor;
	}
	else if (sc.BaseData[InputData.GetInputDataIndex()][sc.Index] < sc.BaseData[InputData.GetInputDataIndex()][sc.Index-1])
	{
		ColorVolume[sc.Index] = sc.Volume[sc.Index]; 
		ColorVolume.DataColor[sc.Index] = ColorVolume.SecondaryColor;
	}
	else
	{
		ColorVolume[sc.Index] = 0;
	}
}

/*==========================================================================*/
SCSFExport scsf_DetrendedOsc(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Oscillator = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MALength = sc.Input[3];
	SCInputRef Line1Value = sc.Input[4];
	SCInputRef Line2Value = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Detrended Oscillator - Di Napoli"; 

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Oscillator.Name = "Oscillator";
		Oscillator.DrawStyle = DRAWSTYLE_LINE;
		Oscillator.PrimaryColor = RGB(0,255,0);
		Oscillator.DrawZeros = true;

		Line1.Name = "Line 1";
		Line1.DrawStyle = DRAWSTYLE_IGNORE;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros = true;

		Line2.Name = "Line 2";
		Line2.DrawStyle = DRAWSTYLE_IGNORE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MALength.Name = "Mov Avg Length";
		MALength.SetInt(7);
		MALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line 1 Value";
		Line1Value.SetFloat(10);
	
		Line2Value.Name = "Line 2 Value";
		Line2Value.SetFloat(-10);

		sc.DrawZeros = true;

		sc.AutoLoop = 1;

		return;
	}

	sc.DataStartIndex = MALength.GetInt();

	if (sc.Index < MALength.GetInt())
		return;
	
	sc.SimpleMovAvg(sc.BaseData[InputData.GetInputDataIndex()],Oscillator.Arrays[0], MALength.GetInt());

	Oscillator[sc.Index] = sc.BaseData[InputData.GetInputDataIndex()][sc.Index] - Oscillator.Arrays[0][sc.Index];

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/*============================================================================*/
SCSFExport scsf_CountDownTimer(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Countdown = sc.Subgraph[0];

	SCInputRef HorizontalPosition = sc.Input[0];
	SCInputRef VerticalPosition = sc.Input[1];
	SCInputRef DisplayValueOnly = sc.Input[3];
	SCInputRef DisplayContinuousTimeCountdown = sc.Input[4];
	SCInputRef DisplayRangeInTicks = sc.Input[5];
	SCInputRef AlertOnNewBar = sc.Input[6];
	SCInputRef TransparentLabelBackground = sc.Input[7];
	SCInputRef AlertForEveryNewBar = sc.Input[8];
	SCInputRef DisplayRemainingTimeAsSecondsOnly = sc.Input[9];
	SCInputRef UseCurrentPriceForVP = sc.Input[11];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "CountDown Timer";
		
		sc.StudyDescription = "Bar countdown timer";
		
		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		Countdown.Name = "CountDown";
		Countdown.LineWidth = 20;
		Countdown.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Countdown.PrimaryColor = RGB(0, 0, 0); //black
		Countdown.SecondaryColor = RGB(255, 127, 0); //Orange
		Countdown.SecondaryColorUsed = true;
		Countdown.DisplayNameValueInWindowsFlags = 1;
		

		HorizontalPosition.Name.Format("Initial Horizontal Position From Left (1-%d)", (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);
		HorizontalPosition.SetInt(20);
		HorizontalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

		VerticalPosition.Name.Format("Initial Vertical Position From Bottom (1-%d)", (int) CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		VerticalPosition.SetInt(90);
		VerticalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		
		DisplayValueOnly.Name = "Display Value Only";
		DisplayValueOnly.SetYesNo(false);
		
		DisplayContinuousTimeCountdown.Name = "Display Continuous Time Countdown Based on Real-Time Clock";
		DisplayContinuousTimeCountdown.SetYesNo(false);

		DisplayRangeInTicks.Name = "Display Remaining Range in Ticks";
		DisplayRangeInTicks.SetYesNo(false);

		AlertOnNewBar.Name = "Alert on New Bar";
		AlertOnNewBar.SetYesNo(false);

		AlertForEveryNewBar.Name = "Alert for Every New Bar";
		AlertForEveryNewBar.SetYesNo(false);
		
		TransparentLabelBackground.Name = "Transparent Label Background";
		TransparentLabelBackground.SetYesNo(false);

		DisplayRemainingTimeAsSecondsOnly.Name = "Display Remaining Time as Seconds Only";
		DisplayRemainingTimeAsSecondsOnly.SetYesNo(false);

		UseCurrentPriceForVP.Name = "Use Current Price For Vertical Position";
		UseCurrentPriceForVP.SetYesNo(false);

		return;
	}

	static const int CountDownTimerLineNumber = COUNTDOWN_TIMER_CHART_DRAWINGNUMBER;

	if ((sc.LastCallToFunction || sc.HideStudy)
		&& sc.UserDrawnChartDrawingExists(sc.ChartNumber,CountDownTimerLineNumber) > 0)
	{
		// be sure to delete user drawn type drawing when study removed or the study is hidden
		sc.DeleteUserDrawnACSDrawing(sc.ChartNumber, CountDownTimerLineNumber);
		return;
	}

	if (sc.HideStudy)
		return;

	int& PriorArraySize = sc.GetPersistentInt(1);
	
	// Disable UpdateAlways.
	sc.UpdateAlways = 0;

	s_UseTool Tool;
	Tool.ChartNumber = sc.ChartNumber;
	Tool.DrawingType = DRAWING_TEXT;
	Tool.Region = sc.GraphRegion;
	Tool.LineNumber = CountDownTimerLineNumber;
	Tool.AddMethod = UTAM_ADD_OR_ADJUST;
	Tool.AddAsUserDrawnDrawing = 1;
	Tool.AllowSaveToChartbook = 1;

	if (sc.UserDrawnChartDrawingExists(sc.ChartNumber, CountDownTimerLineNumber) == 0)
	{
		Tool.BeginDateTime = HorizontalPosition.GetInt();
		Tool.BeginValue = (float)VerticalPosition.GetInt();
		Tool.UseRelativeVerticalValues = true;

	}

	if (Tool.Region == 0 && UseCurrentPriceForVP.GetYesNo() != 0)
	{
		int ValueIndex = sc.ArraySize - 1;

		Tool.BeginValue = sc.Close[ValueIndex];
		Tool.UseRelativeVerticalValues = false;
	}
	else
	{
		Tool.UseRelativeVerticalValues = true;
	}

	Tool.Color = Countdown.PrimaryColor;
	Tool.FontBackColor = Countdown.SecondaryColor;
	if (TransparentLabelBackground.GetYesNo() != 0)
		Tool.TransparentLabelBackground = 1; 

	Tool.ReverseTextColor = false;
	Tool.FontBold = true;
	Tool.FontSize = Countdown.LineWidth;
	Tool.FontFace = sc.GetChartTextFontFaceName();

	float RemainingAmountForSubgraphValue = 0.0;

	//Continuous time countdown.
	if (DisplayContinuousTimeCountdown.GetYesNo() && sc.AreTimeSpecificBars()
		&&  sc.BaseGraphGraphDrawType != GDT_TPO_PROFILE)
	{
		SCDateTime CurrentDateTime = sc.GetCurrentDateTime();

		SCDateTime BarDuration = (sc.SecondsPerBar*SECONDS);

		SCDateTime BarEndingDateTime = sc.BaseDateTimeIn[sc.ArraySize-1]+sc.SecondsPerBar*SECONDS;

		SCDateTime RemainingTime = BarEndingDateTime - CurrentDateTime;

		if (RemainingTime < 0.0)
		{
			RemainingTime = 0;
			double ElapsedBars = (CurrentDateTime - sc.BaseDateTimeIn[sc.ArraySize-1]) / BarDuration;
			double ElapsedPercentageOfCurrentBar = ElapsedBars - (int) ElapsedBars;
			RemainingTime = (1.0 - ElapsedPercentageOfCurrentBar)  * BarDuration;
		}

		RemainingAmountForSubgraphValue = (float) (RemainingTime * SECONDS_PER_DAY);

		SCString StrRemainingTime;

		int Hour, Minute, Second;
		RemainingTime.GetTimeHMS(Hour, Minute, Second);

		if(DisplayRemainingTimeAsSecondsOnly.GetYesNo())
			StrRemainingTime.Format("%02d", RemainingTime.GetTimeInSeconds());
		else if (Hour && Minute)
			StrRemainingTime.Format("%d:%02d:%02d",Hour, Minute, Second);
		else if (Minute)
			StrRemainingTime.Format("%d:%02d", Minute, Second);
		else
			StrRemainingTime.Format("%02d", Second);

		Tool.Text = StrRemainingTime;
		

		//Enable update always so our real-time countdown is continuously visible.
		sc.UpdateAlways = 1;

	}
	else if(sc.AreRangeBars() && DisplayRangeInTicks.GetYesNo())
	{
		SCString CountDownText(sc.GetCountDownText());
		const char* CharPosition = strchr(CountDownText.GetChars(),':');
		int RangeInTicks = 0;
		if (CharPosition)
		{
			float RR = (float)atof(CharPosition+1);
			RangeInTicks = (int)(RR/sc.TickSize+0.5f);
			RemainingAmountForSubgraphValue = (float)RangeInTicks;
			Tool.Text.Format("%d ",RangeInTicks);
		}
		else
			Tool.Text = "";
		
	}
	else if (DisplayValueOnly.GetYesNo() || (DisplayRemainingTimeAsSecondsOnly.GetYesNo() && sc.AreTimeSpecificBars()))
	{
		int CountdownValue = sc.GetLatestBarCountdownAsInteger();
		RemainingAmountForSubgraphValue = (float)CountdownValue;

		Tool.Text.Format("%d",CountdownValue);

	}
	else 
		Tool.Text.Format("%s ",sc.GetCountDownText().GetChars());
	
	
	sc.UseTool(Tool);

	if (! sc.DownloadingHistoricalData 
		&&  PriorArraySize < sc.ArraySize 
		&& sc.UpdateStartIndex != 0 
		&& AlertOnNewBar.GetYesNo())
	{
		int StartIndex;

		if (AlertForEveryNewBar.GetYesNo())
			StartIndex = sc.UpdateStartIndex;
		else
			StartIndex = sc.ArraySize-2;

		for(int BarIndex = StartIndex; BarIndex < sc.ArraySize - 1; BarIndex++)
			sc.PlaySound( sc.SelectedAlertSound, "Bar has completed", 0);
	}



	PriorArraySize = sc.ArraySize;

	// Zero out prior elements and set the remaining time into the last subgraph element.
	for(int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		Countdown[Index] = 0;
	}

	Countdown[sc.ArraySize - 1] = RemainingAmountForSubgraphValue;
}

/*==========================================================================*/
SCSFExport scsf_ClockRealTime(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Clock = sc.Subgraph[0];

	SCInputRef Input_HorizontalPosition = sc.Input[0];
	SCInputRef Input_VerticalPosition = sc.Input[1];
	SCInputRef Input_UseAMPM = sc.Input[3];
	SCInputRef Input_DrawAboveMainPriceGraph = sc.Input[4];
	SCInputRef Input_TransparentLabelBackground = sc.Input[7];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Clock - RealTime";

		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 0;
		sc.ValueFormat = 0;
		sc.UpdateAlways = 1;

		Clock.Name = "Clock";
		Clock.LineWidth = 20;
		Clock.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Clock.PrimaryColor = RGB(0, 0, 0); //black
		Clock.SecondaryColor = RGB(255, 127, 0); //Orange
		Clock.SecondaryColorUsed = true;
		Clock.DisplayNameValueInWindowsFlags = 1;

		Input_HorizontalPosition.Name.Format("Initial Horizontal Position From Left (1-%d)", (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);
		Input_HorizontalPosition.SetInt(20);
		Input_HorizontalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

		Input_VerticalPosition.Name.Format("Initial Vertical Position From Bottom (1-%d)", (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		Input_VerticalPosition.SetInt(90);
		Input_VerticalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		
		Input_UseAMPM.Name = "Use A.M. and P.M.";
		Input_UseAMPM.SetYesNo(false);

		Input_DrawAboveMainPriceGraph.Name = "Draw Above Main Price Graph";
		Input_DrawAboveMainPriceGraph.SetYesNo(false);

		Input_TransparentLabelBackground.Name = "Transparent Label Background";
		Input_TransparentLabelBackground.SetYesNo(false);
		
		return;
	}


	int Hour = 0, Minute = 0, Second = 0;
	if (sc.IsReplayRunning())
		SCDateTimeMS(sc.GetEndingDateTimeForBarIndex(sc.ArraySize - 1)).GetTimeHMS(Hour, Minute, Second);
	else
		sc.CurrentSystemDateTimeMS.GetTimeHMS(Hour, Minute, Second);

	SCString RemainingTimeString;
	if(Input_UseAMPM.GetYesNo())
	{
		int AdjustedHour = Hour;
		if (Hour >= 13)
			AdjustedHour -= 12;

		RemainingTimeString.Format("%d:%02d:%02d ", AdjustedHour, Minute, Second);
		if (Hour < 12)
			RemainingTimeString += "am";
		else
			RemainingTimeString += "pm";
	}
	else
		RemainingTimeString.Format("%d:%02d:%02d", Hour, Minute, Second);
	
	int HorizontalPosition = Input_HorizontalPosition.GetInt();
	int VerticalPosition = Input_VerticalPosition.GetInt();

	int DrawAboveMainPriceGraph = Input_DrawAboveMainPriceGraph.GetYesNo();

	int TransparentLabelBackground = Input_TransparentLabelBackground.GetYesNo();

	sc.AddAndManageSingleTextUserDrawnDrawingForStudy(sc, false, HorizontalPosition, VerticalPosition, Clock, TransparentLabelBackground, RemainingTimeString, DrawAboveMainPriceGraph);

}

/*============================================================================*/
SCSFExport scsf_AccountBalanceExternalService(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_AccountBalance = sc.Subgraph[0];

	SCInputRef Input_HorizontalPosition = sc.Input[0];
	SCInputRef Input_VerticalPosition = sc.Input[1];
	SCInputRef Input_TransparentLabelBackground = sc.Input[4];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Account Balance Text - External Service";

		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		Subgraph_AccountBalance.Name = "Cash Balance";
		Subgraph_AccountBalance.LineWidth = 20;
		Subgraph_AccountBalance.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Subgraph_AccountBalance.PrimaryColor = RGB(0, 0, 0); //black
		Subgraph_AccountBalance.SecondaryColor = RGB(255, 127, 0); //Orange
		Subgraph_AccountBalance.SecondaryColorUsed = true;
		Subgraph_AccountBalance.DisplayNameValueInWindowsFlags = 0;

		Input_HorizontalPosition.Name.Format("Initial Horizontal Position From Left (1-%d)", (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);
		Input_HorizontalPosition.SetInt(20);
		Input_HorizontalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

		Input_VerticalPosition.Name.Format("Initial Vertical Position From Bottom (1-%d)", (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		Input_VerticalPosition.SetInt(90);
		Input_VerticalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		
		Input_TransparentLabelBackground.Name = "Transparent Label Background";
		Input_TransparentLabelBackground.SetYesNo(false);

		return;
	}


	int & IndexOfLastUsedSubgraphElementBalanceData = sc.GetPersistentInt(2);

	Subgraph_AccountBalance[ IndexOfLastUsedSubgraphElementBalanceData ] = 0;
	
	SCString ValueText;
	ValueText.Format("%.2f", sc.TradeServiceAccountBalance);

	Subgraph_AccountBalance[sc.ArraySize - 1] =  (float)sc.TradeServiceAccountBalance;

	IndexOfLastUsedSubgraphElementBalanceData = sc.ArraySize - 1;

	int TransparentLabelBackground = Input_TransparentLabelBackground.GetYesNo();

	sc.AddAndManageSingleTextUserDrawnDrawingForStudy(sc, false, Input_HorizontalPosition.GetInt(), Input_VerticalPosition.GetInt(), Subgraph_AccountBalance, TransparentLabelBackground, ValueText, true);

}


/*==========================================================================*/
SCSFExport scsf_AccountBalanceGraph(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Balance = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Account Balance Graph - External Service";

		sc.StudyDescription = "";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		Balance.Name = "Balance";
		Balance.DrawStyle = DRAWSTYLE_LINE;
		Balance.DrawZeros = false;  // false
		Balance.PrimaryColor = RGB(0, 255, 0);  // Green

		return;
	}


	// Do data processing

	Balance[sc.Index] = (float)sc.TradeServiceAccountBalance;
}


/*==========================================================================*/
SCSFExport scsf_WildersMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MovAvg = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	// Set configuration variables	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Moving Average - Welles Wilders";

		sc.StudyDescription = "";

		// Set the region to draw the graph in.  Region zero is the main
		// price graph region.
		sc.GraphRegion = 0;

		

		// Set the name of the first subgraph
		MovAvg.Name = "MovAvg";

		// Set the color and style of the graph line.  If these are not set
		// the default will be used.
		MovAvg.PrimaryColor = RGB(255,0,0);  // Red
		MovAvg.DrawStyle = DRAWSTYLE_LINE;
		MovAvg.DrawZeros = false;


		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		// Make the Length input and default it to 5
		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		return;
	}

	// Do data processing		
	
	sc.WildersMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], MovAvg, sc.Index, Length.GetInt());
}



/*==========================================================================*/
SCSFExport scsf_BarNumbering(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Number = sc.Subgraph[0];
	
	SCInputRef DisplayAtBottom = sc.Input[0];
	SCInputRef TextOffset = sc.Input[1];
	SCInputRef NumberingEveryNBars = sc.Input[2];
	SCInputRef DoNotResetDaily = sc.Input[4];

	// Set configuration variables	
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Bar Numbering";

		sc.GraphRegion = 0;
		sc.ValueFormat = 0;

		// Set the name of the first subgraph
		Number.Name = "Number";
		Number.DrawStyle = DRAWSTYLE_CUSTOM_VALUE_AT_Y;
		Number.PrimaryColor = RGB(0,255,0);
		Number.LineWidth = 8;
		Number.DrawZeros = false;

		DisplayAtBottom.Name = "Display At Bottom";
		DisplayAtBottom.SetYesNo(false);

		TextOffset.Name = "Text Labels Tick Offset";
		TextOffset.SetInt(2); 
		TextOffset.SetIntLimits(0, 10000);

		NumberingEveryNBars.Name = "Number Every N Bars";
		NumberingEveryNBars.SetInt(2);
		NumberingEveryNBars.SetIntLimits(1, MAX_STUDY_LENGTH);

		DoNotResetDaily.Name = "Do Not Reset at Start of New Day";
		DoNotResetDaily.SetYesNo(false);

		sc.AutoLoop = 1;

		return;
	}

	if (!DisplayAtBottom.GetYesNo())
	{
		sc.ScaleRangeType = SCALE_AUTO;
	}
	else
	{
		sc.ScaleRangeType = SCALE_USERDEFINED;
		sc.ScaleRangeTop = 100;
		sc.ScaleRangeBottom = 1;
	}

	sc.ValueFormat = 0;
	Number.DrawStyle = DRAWSTYLE_CUSTOM_VALUE_AT_Y;

	int& r_BarCounter = sc.GetPersistentInt(1);
	int& r_LastProcessedBarIndex = sc.GetPersistentInt(2);
	
	if (sc.Index == 0)
	{
		r_BarCounter = 1;
		r_LastProcessedBarIndex = -1;
	}
	else if(sc.Index != r_LastProcessedBarIndex)
	{
		
		SCDateTimeMS StartDateTimeOfDay;

		StartDateTimeOfDay.SetTime(sc.StartTimeOfDay);

		StartDateTimeOfDay.SetDate(sc.BaseDateTimeIn[sc.Index].GetDate());

		//reset counter if a new session has begun.
		if (!DoNotResetDaily.GetYesNo() &&
			(sc.BaseDateTimeIn[sc.Index] == StartDateTimeOfDay ||
			(sc.BaseDateTimeIn[sc.Index-1] < StartDateTimeOfDay && sc.BaseDateTimeIn[sc.Index] > StartDateTimeOfDay) )
			)
			r_BarCounter = 1;
		else
			r_BarCounter++; //increment counter 
	}

	r_LastProcessedBarIndex = sc.Index;

	if(r_BarCounter != 1 && r_BarCounter % NumberingEveryNBars.GetInt())
		return;

	Number[sc.Index] = (float)r_BarCounter;
	
	float Offset = sc.TickSize * TextOffset.GetInt();

	float VerticalPositionValue = 0;

	if (DisplayAtBottom.GetYesNo())
		VerticalPositionValue = 5;
	else
		VerticalPositionValue = sc.Low[sc.Index] - Offset;

	Number.Arrays[0][sc.Index] = VerticalPositionValue;

}


/*==========================================================================*/

SCSFExport scsf_StudySubgraphsReference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PriceOverlay1 = sc.Subgraph[0];
	SCSubgraphRef PriceOverlay2 = sc.Subgraph[1];


	SCInputRef StudySubgraph1 = sc.Input[0];
	SCInputRef StudySubgraph2 = sc.Input[1];
	SCInputRef DrawZeros = sc.Input[2];


	// Set configuration variables

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Reference";
		sc.GraphRegion = 1;

		sc.ScaleRangeType = SCALE_INDEPENDENT;

		PriceOverlay1.Name = "Study Subgraph 1";
		PriceOverlay1.DrawStyle = DRAWSTYLE_FILL_TOP;
		PriceOverlay1.PrimaryColor = RGB(0,255,0);

		PriceOverlay2.Name = "Study Subgraph 2";
		PriceOverlay2.DrawStyle = DRAWSTYLE_FILL_BOTTOM;
		PriceOverlay2.PrimaryColor = RGB(255,0,255);

		StudySubgraph1.Name = "Input Study 1";
		StudySubgraph1.SetStudySubgraphValues(0, 0);

		StudySubgraph2.Name = "Input Study 2";
		StudySubgraph2.SetStudySubgraphValues(0, 0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(true);

		sc.CalculationPrecedence = VERY_LOW_PREC_LEVEL;
		sc.DrawStudyUnderneathMainPriceGraph = true;

		sc.AutoLoop = 0;

		return;
	}


	// Do data processing

	//Set Draw Zeros as defined by user input
	PriceOverlay1.DrawZeros = DrawZeros.GetYesNo();
	PriceOverlay2.DrawZeros = DrawZeros.GetYesNo();

	//This will return sc.UpdateStartIndex or an earlier index if this study is calculated after a study which calculated at an earlier index and supports setting that earlier index.
	int ActualStartIndex = sc.GetCalculationStartIndexForStudy();

	// Get the array for the specified Input Data from the specified studies
	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySubgraph1.GetStudyID(), StudySubgraph1.GetSubgraphIndex(),Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySubgraph2.GetStudyID(), StudySubgraph2.GetSubgraphIndex(),Study2Array);

	for(int BarIndex = ActualStartIndex; BarIndex < sc.ArraySize;  BarIndex++)
	{
		PriceOverlay1[BarIndex] = Study1Array[BarIndex];
		PriceOverlay2[BarIndex] = Study2Array[BarIndex];
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = ActualStartIndex;
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphReference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_StudySubgraphReference = sc.Subgraph[0];

	SCInputRef Input_StudySubgraphReference = sc.Input[0];

	// Set configuration variables

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraph Reference";
		sc.GraphRegion = 2;

		sc.ScaleRangeType = SCALE_AUTO;

		Subgraph_StudySubgraphReference.Name = "Study Subgraph";
		Subgraph_StudySubgraphReference.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_StudySubgraphReference.PrimaryColor = RGB(0, 255, 0);
		Subgraph_StudySubgraphReference.DrawZeros = true;

		Input_StudySubgraphReference.Name = "Study Subgraph to Reference";
		Input_StudySubgraphReference.SetStudySubgraphValues(0, 0);
		
		sc.CalculationPrecedence = VERY_LOW_PREC_LEVEL;

		sc.AutoLoop = 0;

		return;
	}


	// Do data processing

	//This will return sc.UpdateStartIndex or an earlier index if this study is calculated after a study which calculated at an earlier index and supports setting that earlier index.
	int ActualStartIndex = sc.GetCalculationStartIndexForStudy();

	// Get the array for the specified Input Data from the specified studies
	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(Input_StudySubgraphReference.GetStudyID(), Input_StudySubgraphReference.GetSubgraphIndex(), Study1Array);

	for (int BarIndex = ActualStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		Subgraph_StudySubgraphReference[BarIndex] = Study1Array[BarIndex];
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = ActualStartIndex;
}

/*==========================================================================*/
SCSFExport scsf_VolumeCloseToMidpointColoring(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume - Close To Midpoint Colored";

		sc.ValueFormat = 0;

		sc.AutoLoop = true;
		sc.ScaleRangeType = SCALE_ZEROBASED;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0, 255, 0);
		Volume.SecondaryColor = RGB(255, 0, 0);
		Volume.SecondaryColorUsed = 1;
		Volume.LineWidth = 2;
		Volume.DrawZeros = false;


		sc.DisplayStudyInputValues = false;

		return;
	}

	Volume[sc.Index] = sc.Volume[sc.Index];

	if(sc.Close[sc.Index] >=  sc.HLAvg[sc.Index])
		Volume.DataColor[sc.Index] = Volume.PrimaryColor;
	else
		Volume.DataColor[sc.Index] = Volume.SecondaryColor;
}

/*==========================================================================*/
SCSFExport scsf_AdjustedCumulativeValue(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CumulativeValue = sc.Subgraph[0];
	SCFloatArray  AverageValue    = CumulativeValue.Arrays[0];
	SCFloatArray  AdjustedValue   = CumulativeValue.Arrays[1];

	SCInputRef InputData       = sc.Input[0];
	SCInputRef SmoothingLength = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "Cumulative Adjusted Value";
		sc.StudyDescription = "Input data is smoothed to attain a bias, then adjusted to remove the bias, and then accumulated.";

		sc.GraphRegion = 2;
		sc.AutoLoop = 1;

		CumulativeValue.Name = "Cumulative Value";
		CumulativeValue.DrawStyle = DRAWSTYLE_LINE;
		CumulativeValue.PrimaryColor = RGB(0, 255, 0);
		CumulativeValue.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_OHLC_AVG);

		SmoothingLength.Name = "Long Term Smoothing Length";
		SmoothingLength.SetInt(10000);
		SmoothingLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}


	// Do data processing
	sc.ExponentialMovAvg(sc.BaseData[InputData.GetInputDataIndex()], AverageValue, SmoothingLength.GetInt());

	AdjustedValue[sc.Index] = sc.BaseData[InputData.GetInputDataIndex()][sc.Index] - AverageValue[sc.Index];

	if (sc.Index == 0)
		CumulativeValue[sc.Index] = AdjustedValue[sc.Index];
	else
		CumulativeValue[sc.Index] = CumulativeValue[sc.Index-1] + AdjustedValue[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_TradersDynamicIndex(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		sc.GraphName = "Traders Dynamic Index";
		sc.StudyDescription = "<p><strong>Traders Dynamic Index</strong></p><p>with thanks to Dean Malone <a href=http://www.compassfx.com>CompassFX</a></p><p><a href=http://www.earnforex.com/metatrader-indicators/Traders-Dynamic-Index>Earnforex description</a></p>";

		sc.AutoLoop = 1;

		sc.Subgraph[0].Name = "RSI";
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[1].Name = "UpVol";
		sc.Subgraph[1].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[2].Name = "Mid";
		sc.Subgraph[2].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[3].Name = "DnVol";
		sc.Subgraph[3].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[4].Name = "Ma";
		sc.Subgraph[4].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[5].Name = "Mb";
		sc.Subgraph[5].DrawStyle = DRAWSTYLE_LINE;

		sc.Input[3].Name = "RSI Length";
		sc.Input[3].SetInt(13);
		sc.Input[3].SetDescription("<strong>RSI_Period</strong> (default = 13) &#151; period in bars for calculation of RSI.");

		sc.Input[4].Name = "RSI Price";
		sc.Input[4].SetInputDataIndex(SC_LAST);
		sc.Input[4].SetDescription("<strong>RSI_Price</strong> (default = MODE_CLOSE) &#151; price type to use in RSI calculation");

		sc.Input[5].Name = "Volatility Band Length";
		sc.Input[5].SetInt(34);
		sc.Input[5].SetDescription("<strong>Volatility_Band</strong> (default = 34) &#151; parameter for  volatility band calculation. Can be between 20 and 40. The lower this value is the curvier becomes the band.");

		sc.Input[6].Name = "RSI Price Line";
		sc.Input[6].SetInt(2);
		sc.Input[6].SetDescription("<strong>RSI_Price_Line</strong> (default = 2) &#151; period of the first moving average (fast).");

		sc.Input[7].Name = "RSI Price Type";
		sc.Input[7].SetMovAvgType(MOVAVGTYPE_SIMPLE);

		sc.Input[8].Name = "RSI Trade Signal Line";
		sc.Input[8].SetInt(7);
		sc.Input[8].SetDescription("<strong>Trade_Signal_Line</strong> (default = 7) &#151; period of the second moving average (slow)");

		sc.Input[9].Name = "Trade Signal Type";
		sc.Input[9].SetMovAvgType(MOVAVGTYPE_SIMPLE);

		sc.DocumentationImageURL = "http://codebase.mql4.com/c/codebase/2008/03/indicatori_gtradersmdynamicmindexuvisualqalerts.gif";

		return;
	}


		sc.RSI(sc.BaseDataIn[sc.Input[4].GetInputDataIndex()], sc.Subgraph[0], MOVAVGTYPE_SIMPLE, sc.Input[3].GetInt());
		
		sc.MovingAverage(sc.Subgraph[0], sc.Subgraph[4], sc.Input[7].GetMovAvgType(), sc.Input[6].GetInt());
		
		sc.MovingAverage(sc.Subgraph[0], sc.Subgraph[5], sc.Input[9].GetMovAvgType(), sc.Input[8].GetInt());
		
		sc.BollingerBands(sc.Subgraph[0], sc.Subgraph[2], sc.Input[5].GetInt(), 1.6185f, MOVAVGTYPE_SIMPLE);
		sc.Subgraph[1][sc.Index] = sc.Subgraph[2].Arrays[0][sc.Index];
		sc.Subgraph[3][sc.Index] = sc.Subgraph[2].Arrays[1][sc.Index];

}

/*============================================================================

----------------------------------------------------------------------------*/
SCSFExport scsf_PointFigureChartReversalMarker(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Marker = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Point and Figure Chart Reversal Marker";
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;
		sc.ValueFormat = sc.BaseGraphValueFormat;

		Marker.Name = "Reversal Mark";
		Marker.DrawStyle = DRAWSTYLE_DASH;
		Marker.LineWidth = 4;
		Marker.PrimaryColor = RGB(255, 0, 128);
		Marker.DrawZeros = false;

		return;
	}

	n_ACSIL::s_BarPeriod BarPeriodData;
	sc.GetBarPeriodParameters(BarPeriodData);

	if (BarPeriodData.ChartDataType != INTRADAY_DATA
		|| BarPeriodData.IntradayChartBarPeriodType != IBPT_POINT_AND_FIGURE)
		return;

	float ReversalAmountNeeded = BarPeriodData.IntradayChartBarPeriodParameter1 * sc.TickSize * BarPeriodData.IntradayChartBarPeriodParameter2;

	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		Marker[BarIndex] = 0;
		if (BarIndex == sc.ArraySize - 1)
		{
			if (sc.BaseData[PF_DIRECTION_ARRAY][BarIndex] == 1)//Up bar
			{
				Marker[BarIndex] =  sc.BaseData[SC_POINT_FIGURE_HIGH][BarIndex] - ReversalAmountNeeded;
			}
			else
			{
				Marker[BarIndex] = sc.BaseData[SC_POINT_FIGURE_LOW][BarIndex] + ReversalAmountNeeded;
			}
		}
	}


}
/*============================================================================

----------------------------------------------------------------------------*/
SCSFExport scsf_PointFigureBoxCount(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BoxCount = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Point and Figure Box Count";
		sc.GraphRegion = 1;
		sc.AutoLoop = 0;
		sc.ValueFormat = 0;

		BoxCount.Name = "P&F Box Count";
		BoxCount.DrawStyle = DRAWSTYLE_BAR;
		BoxCount.LineWidth = 2;
		BoxCount.PrimaryColor = RGB(255, 0, 128);
		BoxCount.DrawZeros = false;

		return;
	}

	if (!sc.ArePointAndFigureBars())
		return;


	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		BoxCount[BarIndex] = sc.BaseData[PF_NUM_BOXES_ARRAY][BarIndex];
	}


}

/*============================================================================

----------------------------------------------------------------------------*/
SCSFExport scsf_ReadFromUnderlyingIntradayFileExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Read from Underlying Intraday File Example";


		sc.AutoLoop = 0;

		return;
	}

	if (sc.LastCallToFunction)
		return;

	s_IntradayRecord IntradayRecord;

	for (int FileIndex = sc.FileRecordIndexOfLastDataRecordInChart; FileIndex >= sc.FileRecordIndexOfLastDataRecordInChart - 10; FileIndex--)
	{
		bool FirstIteration = FileIndex == sc.FileRecordIndexOfLastDataRecordInChart;
		bool LastIteration = FileIndex <= sc.FileRecordIndexOfLastDataRecordInChart - 10;
		IntradayFileLockActionEnum  IntradayFileLockAction = IFLA_NO_CHANGE;

		if (FirstIteration && LastIteration)
			IntradayFileLockAction = IFLA_LOCK_READ_RELEASE;
		else if (FirstIteration)
			IntradayFileLockAction = IFLA_LOCK_READ_HOLD;
		else if (LastIteration)
			IntradayFileLockAction = IFLA_RELEASE_AFTER_READ;

		sc.ReadIntradayFileRecordAtIndex(FileIndex, IntradayRecord, IntradayFileLockAction);

		SCDateTimeMS TradeTimestampInChartTimeZone = IntradayRecord.DateTime;

	}

	// Could also use this line of code to release the lock if not sure when will be complete with reading and want to do it as a separate operation. An index of -1 signifies not actually to perform a read.
	///sc.ReadIntradayFileRecordAtIndex( -1, IntradayRecord, IFLA_RELEASE_AFTER_READ);
		

}


/*============================================================================

----------------------------------------------------------------------------*/
SCSFExport scsf_ReadChartBarRecordsFromUnderlyingIntradayFileExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Read Chart Bar Records from Underlying Intraday File Example";
		
		sc.AutoLoop = 0;

		//this must be set to 1 in order to use the sc.ReadIntradayFileRecordForBarIndexAndSubIndex function.
		sc.MaintainAdditionalChartDataArrays = 1;

		return;
	}

	if (sc.LastCallToFunction)
		return;

	s_IntradayRecord IntradayRecord;

	if (sc.GetBarHasClosedStatus(sc.UpdateStartIndex) == BHCS_BAR_HAS_NOT_CLOSED)//Only execute on updating of last bar.
	{
		int ReadSuccess = true;
		bool FirstIteration = true;
		uint32_t TotalVolume = 0;
		int SubIndex = 0;//Start at first record within bar

		//Read records until sc.ReadIntradayFileRecordForBarIndexAndSubIndex returns 0
		while (ReadSuccess)
		{
			IntradayFileLockActionEnum  IntradayFileLockAction = IFLA_NO_CHANGE;

			if (FirstIteration)
			{
				IntradayFileLockAction = IFLA_LOCK_READ_HOLD;

				FirstIteration = false;
			}

			ReadSuccess = sc.ReadIntradayFileRecordForBarIndexAndSubIndex(sc.ArraySize - 1, SubIndex, IntradayRecord, IntradayFileLockAction);

			if (ReadSuccess)
			{
				TotalVolume += IntradayRecord.TotalVolume;
				++SubIndex;
			}
		}

		sc.ReadIntradayFileRecordForBarIndexAndSubIndex(-1, -1, IntradayRecord, IFLA_RELEASE_AFTER_READ);

		SCString TotalVolumeString;
		TotalVolumeString.Format("Total volume: %u", TotalVolume);

		sc.AddMessageToLog(TotalVolumeString, 0);
	}
	
}


/*==========================================================================*/
SCSFExport scsf_ChangeStudyInputExample(SCStudyInterfaceRef sc)
{
	// Configuration
	if (sc.SetDefaults)
	{
		sc.GraphName = "Change Study Input Example";
		sc.GraphRegion = 0;    // zero based region number

		sc.AutoLoop = 0;

		return;
	}

	// Data processing
	int IntegerInput = 0;
	sc.GetChartStudyInputInt(sc.ChartNumber, 1, 36, IntegerInput);

	double DoubleInput = 0;
	sc.GetChartStudyInputFloat(sc.ChartNumber, 1, 36, DoubleInput);

	if (DoubleInput != 20)
	{
		sc.SetChartStudyInputFloat(sc.ChartNumber, 1, 36, 20);
		sc.RecalculateChart(sc.ChartNumber);
	}
}


/*============================================================================
	sc.GetStudyPeakValleyLine example function
----------------------------------------------------------------------------*/
SCSFExport scsf_GetStudyPeakValleyLineExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "GetStudyPeakValleyLine Example";

		sc.AutoLoop = 0;

		sc.Input[0].SetStudyID(1);
		sc.Input[0].Name = "Volume by Price Study";

		return;
	}


	// Do data processing
	float PeakValleyLinePrice= 0;
	int PeakValleyType = 0;
	int StartIndex = 0;
	int PeakValleyExtensionChartColumnEndIndex = 0;

	// get the first Peak/Valley line from the last volume by price profile
	if (sc.GetStudyPeakValleyLine(sc.ChartNumber, sc.Input[0].GetStudyID(), PeakValleyLinePrice, PeakValleyType, StartIndex, PeakValleyExtensionChartColumnEndIndex, -1, 0))
	{
		// Peak/Valley line found
		int Test = 1;
	}
}


/*============================================================================
	This study periodically recalculates the studies on the chart.
----------------------------------------------------------------------------*/
SCSFExport scsf_PeriodicChartRecalculation(SCStudyInterfaceRef sc)
{
	SCInputRef Input_RecalculationTimeInSeconds = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Chart Recalculation - Periodic";

		sc.AutoLoop =  0;

		sc.UpdateAlways = 1;
		sc.GraphRegion = 0;

		Input_RecalculationTimeInSeconds.Name = "Recalculation Time in Seconds";
		Input_RecalculationTimeInSeconds.SetInt(SECONDS_PER_HOUR);// 1 hour
		Input_RecalculationTimeInSeconds.SetIntLimits(1, SECONDS_PER_DAY);
		return;
	}

	SCDateTime& r_LastCalculationDateTime = sc.GetPersistentSCDateTime(1);
	SCDateTimeMS CurrentDateTime = sc.GetCurrentDateTime();
	if (CurrentDateTime - r_LastCalculationDateTime > (Input_RecalculationTimeInSeconds.GetInt() *  SECONDS))
	{
		r_LastCalculationDateTime = CurrentDateTime;
		sc.FlagFullRecalculate = 1;
		sc.AddMessageToLog("Performing full recalculation of chart", 0);
	}



}

/*==========================================================================*/
SCSFExport scsf_HLVolatility(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_HLDiff = sc.Subgraph[0];
	SCSubgraphRef Subgraph_AvgHLDiff = sc.Subgraph[1];
	SCSubgraphRef Subgraph_AvgClose = sc.Subgraph[2];
	SCSubgraphRef Subgraph_HLVolatility = sc.Subgraph[3];

	SCInputRef Input_MAType = sc.Input[0];
	SCInputRef Input_Length = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "High Low Volatility";

		sc.GraphRegion = 1;
		sc.AutoLoop = 1;

		Subgraph_HLDiff.Name = "HL Difference";
		Subgraph_HLDiff.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_AvgHLDiff.Name = "Average HL Difference";
		Subgraph_AvgHLDiff.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_AvgClose.Name = "Average Close";
		Subgraph_AvgClose.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_HLVolatility.Name = "HL Volatility";
		Subgraph_HLVolatility.DrawZeros = true;
		Subgraph_HLVolatility.PrimaryColor = RGB(0, 255, 0);
		Subgraph_HLVolatility.DrawStyle = DRAWSTYLE_LINE;

		Input_MAType.Name = "Moving Average Type";
		Input_MAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		Input_Length.Name = "Moving Average Length";
		Input_Length.SetInt(9);
		Input_Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	Subgraph_HLDiff[sc.Index] = sc.GetHighest(sc.BaseDataIn[SC_HIGH], Input_Length.GetInt()) - sc.GetLowest(sc.BaseDataIn[SC_LOW], Input_Length.GetInt());

	sc.MovingAverage(Subgraph_HLDiff, Subgraph_AvgHLDiff, Input_MAType.GetMovAvgType(), Input_Length.GetInt());
	sc.MovingAverage(sc.BaseDataIn[SC_LAST], Subgraph_AvgClose, Input_MAType.GetMovAvgType(), Input_Length.GetInt());

	if (Subgraph_AvgClose[sc.Index] == 0.0f)
		Subgraph_HLVolatility[sc.Index] = 0.0f;
	else
		Subgraph_HLVolatility[sc.Index] = 100.0f*Subgraph_AvgHLDiff[sc.Index] / Subgraph_AvgClose[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_GetVolumeAtPriceDataForStudyProfileExample(SCStudyInterfaceRef sc)
{
	SCInputRef  Input_StudyReference = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "GetVolumeAtPriceDataForStudyProfile Example";

		sc.AutoLoop = 0;
		sc.GraphRegion = 1;

		Input_StudyReference.Name = "Profile Study Reference";
		Input_StudyReference.SetStudyID(1);

		return;
	}

	int PricesCount = sc.GetNumPriceLevelsForStudyProfile(Input_StudyReference.GetStudyID(), 0);

	for (int PriceIndex = 0; PriceIndex < PricesCount; PriceIndex++)
	{

		s_VolumeAtPriceV2 VolumeAtPrice;

		int Result = sc.GetVolumeAtPriceDataForStudyProfile
		(Input_StudyReference.GetStudyID()
			, 0
			, PriceIndex
			, VolumeAtPrice
		);

		int Volume = VolumeAtPrice.Volume;

	}


}

/*==========================================================================*/
SCSFExport scsf_RotationFactor(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_RotationFactor = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Rotation Factor";

		sc.GraphRegion = 1;
		sc.AutoLoop = 1;

		Subgraph_RotationFactor.Name = "RF";
		Subgraph_RotationFactor.DrawStyle = DRAWSTYLE_BAR;
		Subgraph_RotationFactor.AutoColoring = AUTOCOLOR_POSNEG;
		Subgraph_RotationFactor.LineWidth = 2;
		Subgraph_RotationFactor.DrawZeros = 1;
		
		sc.DataStartIndex = 1;

		return;
	}

	if (sc.Index == 0)
		return;

	int RotationFactor = 0;

	if (sc.High[sc.Index] > sc.High[sc.Index - 1])
		RotationFactor++;

	if (sc.Low[sc.Index] > sc.Low[sc.Index - 1])
		RotationFactor++;

	if (sc.High[sc.Index] < sc.High[sc.Index - 1])
		RotationFactor--;

	if (sc.Low[sc.Index] < sc.Low[sc.Index - 1])
		RotationFactor--;

	Subgraph_RotationFactor.Arrays[0][sc.Index] = (float)RotationFactor;

	if (sc.IsNewTradingDay(sc.Index))
		Subgraph_RotationFactor.Data[sc.Index] = 0;
	else
		Subgraph_RotationFactor.Data[sc.Index] = Subgraph_RotationFactor.Arrays[0][sc.Index] + Subgraph_RotationFactor.Data[sc.Index - 1];

}

/*==========================================================================*/
SCSFExport scsf_HurstBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_DisplacedPrice = sc.Subgraph[0];
	SCSubgraphRef Subgraph_MovingAverage = sc.Subgraph[1];
	SCSubgraphRef Subgraph_TopInnerBand = sc.Subgraph[2];
	SCSubgraphRef Subgraph_BottomInnerBand = sc.Subgraph[3];
	SCSubgraphRef Subgraph_TopOuterBand = sc.Subgraph[4];
	SCSubgraphRef Subgraph_BottomOuterBand = sc.Subgraph[5];
	SCSubgraphRef Subgraph_TopExtremeBand = sc.Subgraph[6];
	SCSubgraphRef Subgraph_BottomExtremeBand = sc.Subgraph[7];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_Length = sc.Input[1];
	SCInputRef Input_MovAvgType = sc.Input[2];
	SCInputRef Input_InnerMultiplier = sc.Input[3];
	SCInputRef Input_OuterMultiplier = sc.Input[4];
	SCInputRef Input_ExtremeMultiplier = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Hurst Bands";

		sc.GraphRegion = 0;
		sc.AutoLoop = 1;

		Subgraph_DisplacedPrice.Name = "Displaced Price";
		Subgraph_DisplacedPrice.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_MovingAverage.Name = "Moving Average";
		Subgraph_MovingAverage.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_MovingAverage.PrimaryColor = RGB(128, 0, 128);
		Subgraph_MovingAverage.DrawZeros = true;

		Subgraph_TopInnerBand.Name = "Top Inner Band";
		Subgraph_TopInnerBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_TopInnerBand.PrimaryColor = RGB(255, 0, 0);
		Subgraph_TopInnerBand.DrawZeros = true;

		Subgraph_BottomInnerBand.Name = "Bottom Inner Band";
		Subgraph_BottomInnerBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BottomInnerBand.PrimaryColor = RGB(255, 0, 0);
		Subgraph_BottomInnerBand.DrawZeros = true;

		Subgraph_TopOuterBand.Name = "Top Outer Band";
		Subgraph_TopOuterBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_TopOuterBand.PrimaryColor = RGB(0, 255, 0);
		Subgraph_TopOuterBand.DrawZeros = true;

		Subgraph_BottomOuterBand.Name = "Bottom Outer Band";
		Subgraph_BottomOuterBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BottomOuterBand.PrimaryColor = RGB(0, 255, 0);
		Subgraph_BottomOuterBand.DrawZeros = true;

		Subgraph_TopExtremeBand.Name = "Top Extreme Band";
		Subgraph_TopExtremeBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_TopExtremeBand.PrimaryColor = RGB(0, 0, 255);
		Subgraph_TopExtremeBand.DrawZeros = true;

		Subgraph_BottomExtremeBand.Name = "Bottom Extreme Band";
		Subgraph_BottomExtremeBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BottomExtremeBand.PrimaryColor = RGB(0, 0, 255);
		Subgraph_BottomExtremeBand.DrawZeros = true;

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_HL);

		Input_Length.Name = "Moving Average Length";
		Input_Length.SetInt(10);
		Input_Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovAvgType.Name = "Moving Average Type";
		Input_MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Input_InnerMultiplier.Name = "Inner Band Multiplier";
		Input_InnerMultiplier.SetFloat(0.05f);

		Input_OuterMultiplier.Name = "Outer Band Multiplier";
		Input_OuterMultiplier.SetFloat(0.025f);

		Input_ExtremeMultiplier.Name = "Extreme Band Multiplier";
		Input_ExtremeMultiplier.SetFloat(0.0125f);

		return;
	}

	int Displacement = Input_Length.GetInt() / 2 + 1;

	sc.DataStartIndex = Input_Length.GetInt() + Displacement - 1;

	Subgraph_DisplacedPrice[sc.Index] = sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index - Displacement];

	sc.MovingAverage(Subgraph_DisplacedPrice, Subgraph_MovingAverage, Input_MovAvgType.GetMovAvgType(), Input_Length.GetInt());


	Subgraph_TopInnerBand[sc.Index] = Subgraph_MovingAverage[sc.Index] + (Input_InnerMultiplier.GetFloat() / 100.0f) * Subgraph_MovingAverage[sc.Index];

	Subgraph_BottomInnerBand[sc.Index] = Subgraph_MovingAverage[sc.Index] - (Input_InnerMultiplier.GetFloat() / 100.0f) * Subgraph_MovingAverage[sc.Index];

	Subgraph_TopOuterBand[sc.Index] = Subgraph_MovingAverage[sc.Index] + (Input_OuterMultiplier.GetFloat() / 100.0f) * Subgraph_MovingAverage[sc.Index];

	Subgraph_BottomOuterBand[sc.Index] = Subgraph_MovingAverage[sc.Index] - (Input_OuterMultiplier.GetFloat() / 100.0f) * Subgraph_MovingAverage[sc.Index];

	Subgraph_TopExtremeBand[sc.Index] = Subgraph_MovingAverage[sc.Index] + (Input_ExtremeMultiplier.GetFloat() / 100.0f) * Subgraph_MovingAverage[sc.Index];

	Subgraph_BottomExtremeBand[sc.Index] = Subgraph_MovingAverage[sc.Index] - (Input_ExtremeMultiplier.GetFloat() / 100.0f) * Subgraph_MovingAverage[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_GannHiLoActivator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_HiAvg = sc.Subgraph[0];
	SCSubgraphRef Subgraph_LoAvg = sc.Subgraph[1];
	SCSubgraphRef Subgraph_HiLo = sc.Subgraph[2];
	SCSubgraphRef Subgraph_GannHiLoActivator = sc.Subgraph[3];

	SCInputRef Input_Length = sc.Input[0];
	SCInputRef Input_MovAvgType = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Gann HiLo Activator";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Subgraph_HiAvg.Name = "Average High Price";
		Subgraph_HiAvg.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_LoAvg.Name = "Average Low Price";
		Subgraph_LoAvg.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_HiLo.Name = "HiLo";
		Subgraph_HiLo.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_GannHiLoActivator.Name = "Gann HiLo Activator";
		Subgraph_GannHiLoActivator.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_GannHiLoActivator.PrimaryColor = RGB(0, 255, 0);
		Subgraph_GannHiLoActivator.SecondaryColor = RGB(255, 0, 0);
		Subgraph_GannHiLoActivator.DrawZeros = true;

		Input_Length.Name = "Length";
		Input_Length.SetInt(3);
		Input_Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovAvgType.Name = "Moving Average Type";
		Input_MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		return;
	}

	sc.DataStartIndex = Input_Length.GetInt();

	sc.MovingAverage(sc.High, Subgraph_HiAvg, Input_MovAvgType.GetMovAvgType(), Input_Length.GetInt());
	sc.MovingAverage(sc.Low, Subgraph_LoAvg, Input_MovAvgType.GetMovAvgType(), Input_Length.GetInt());

	if (sc.Close[sc.Index] > Subgraph_HiAvg[sc.Index - 1])
		Subgraph_HiLo[sc.Index] = 1.0f;

	else if (sc.Close[sc.Index] < Subgraph_LoAvg[sc.Index - 1])
		Subgraph_HiLo[sc.Index] = -1.0f;

	else
		Subgraph_HiLo[sc.Index] = 0.0f;

	if (Subgraph_HiLo[sc.Index] == 1.0f)
	{
		Subgraph_GannHiLoActivator.DataColor[sc.Index] = Subgraph_GannHiLoActivator.PrimaryColor;
		Subgraph_GannHiLoActivator[sc.Index] = Subgraph_LoAvg[sc.Index - 1];
	}

	else if (Subgraph_HiLo[sc.Index] == -1.0f)
	{
		Subgraph_GannHiLoActivator.DataColor[sc.Index] = Subgraph_GannHiLoActivator.SecondaryColor;
		Subgraph_GannHiLoActivator[sc.Index] = Subgraph_HiAvg[sc.Index - 1];
	}
	else
	{
		Subgraph_GannHiLoActivator.DataColor[sc.Index] = Subgraph_GannHiLoActivator.DataColor[sc.Index - 1];
		Subgraph_GannHiLoActivator[sc.Index] = Subgraph_GannHiLoActivator[sc.Index - 1];
	}
}