#include "sierrachart.h"

#include "scstudyfunctions.h"

/*==========================================================================*/
SCSFExport scsf_Accumulate(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Summation = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Summation";
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		Summation.Name = "Sum";
		Summation.DrawStyle = DRAWSTYLE_LINE;
		Summation.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = 1;

		
		
		return;
	}

	sc.CumulativeSummation(sc.BaseData[InputData.GetInputDataIndex()], Summation, sc.Index);
}

/*==========================================================================*/
SCSFExport scsf_ArmsEaseOfMovement(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_EMV = sc.Subgraph[0];
	SCSubgraphRef Subgraph_EMVAvg = sc.Subgraph[1];

	SCInputRef Input_VolumeDivisor = sc.Input[0];
	SCInputRef Input_Length = sc.Input[1];
	SCInputRef Input_MovingAverageType = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Arms Ease of Movement";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		Subgraph_EMV.Name = "EMV";
		Subgraph_EMV.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_EMV.PrimaryColor = RGB(0, 255, 0);
		Subgraph_EMV.DrawZeros = false;

		Subgraph_EMVAvg.Name = "MA of EMV";
		Subgraph_EMVAvg.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_EMVAvg.PrimaryColor = RGB(0, 0, 255);
		Subgraph_EMVAvg.DrawZeros = false;

		Input_VolumeDivisor.Name = "Volume Divisor";
		Input_VolumeDivisor.SetInt(100000000);

		Input_Length.Name = "Fast Moving Average Length";
		Input_Length.SetInt(14);
		Input_Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovingAverageType.Name = "Moving Average Type";
		Input_MovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		sc.AutoLoop = 1;

		
		

		return;
	}

	if (sc.Index < 1)
		return;

	sc.ArmsEMV(sc.BaseDataIn, Subgraph_EMV, Input_VolumeDivisor.GetInt(), sc.Index);

	sc.MovingAverage(Subgraph_EMV, Subgraph_EMVAvg, Input_MovingAverageType.GetMovAvgType(), Input_Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_ChaikinMoneyFlow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CMF = sc.Subgraph[0];


	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Chaikin Money Flow";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		CMF.Name = "CMF";
		CMF.DrawStyle = DRAWSTYLE_LINE;
		CMF.PrimaryColor = RGB(0,255,0);
		CMF.DrawZeros = false;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		
		

		return;
	}

	sc.ChaikinMoneyFlow(sc.BaseDataIn, CMF, Length.GetInt()); 
}

/*==========================================================================*/
SCSFExport scsf_Dispersion(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Dispersion = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Dispersion";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		Dispersion.Name = "Dispersion";
		Dispersion.DrawStyle = DRAWSTYLE_LINE;
		Dispersion.PrimaryColor = RGB(0,255,0);
		Dispersion.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		
		

		return;
	}

	sc.Dispersion(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], Dispersion, sc.Index, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_Envelope(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TopBand    = sc.Subgraph[0];
	SCSubgraphRef BottomBand = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Value     = sc.Input[3];
	SCInputRef BandsType = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Bands/Envelope";

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		TopBand.Name = "TopBand";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(0,255,0);
		TopBand.DrawZeros = false;

		BottomBand.Name = "BottomBand";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(255,0,255);
		BottomBand.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Value.Name = "Percentage/Fixed Value/Ticks";
		Value.SetFloat(0.05f);
		Value.SetFloatLimits(0.00001f,(float)MAX_STUDY_LENGTH);

		BandsType.Name = "Bands Type";
		BandsType.SetCustomInputStrings("Percentage;Fixed Value;Number of Ticks");
		BandsType.SetCustomInputIndex(0);

		sc.AutoLoop = 1;

		
		

		return;
	}


	SCFloatArrayRef In = sc.BaseDataIn[InputData.GetInputDataIndex()];

	if(BandsType.GetIndex() == 0)
	{
		sc.EnvelopePct(In, TopBand, Value.GetFloat());
		BottomBand[sc.Index] = TopBand.Arrays[0][sc.Index];
	}
	else if(BandsType.GetIndex() == 1)
	{
		sc.EnvelopeFixed(In, TopBand, Value.GetFloat());
		BottomBand[sc.Index] = TopBand.Arrays[0][sc.Index];
	}
	else
	{
		sc.EnvelopeFixed(In, TopBand, Value.GetFloat() * sc.TickSize);
		BottomBand[sc.Index] = TopBand.Arrays[0][sc.Index];
	}
}

/*==========================================================================*/
SCSFExport scsf_VerticalHorizontalFilter(SCStudyInterfaceRef sc)
{
	SCSubgraphRef VHF = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Vertical Horizontal Filter";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		VHF.Name = "VHF";
		VHF.DrawStyle = DRAWSTYLE_LINE;
		VHF.PrimaryColor = RGB(0,255,0);
		VHF.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(28);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		
		

		return;
	}
	if(sc.Index < Length.GetInt())
		return;

	SCFloatArrayRef In = sc.BaseDataIn[(int)InputData.GetInputDataIndex()];
	sc.VHF(In, VHF, sc.Index, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_RWI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RWIHigh = sc.Subgraph[0];
	SCSubgraphRef RWILow = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];

	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Random Walk Index";

		sc.GraphRegion = 1;			

		RWIHigh.Name = "RWI High";
		RWIHigh.PrimaryColor = RGB(0,255,0);
		RWIHigh.DrawStyle = DRAWSTYLE_LINE;

		RWILow.Name = "RWI Low";
		RWILow.PrimaryColor = RGB(255,0,255);
		RWILow.DrawStyle = DRAWSTYLE_LINE;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		
		

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;
	sc.InternalRWI(sc.BaseDataIn, RWIHigh, RWILow, Temp2, Temp3, Temp4, sc.Index, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_UltimateOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UltimateOscillatorOut1 = sc.Subgraph[0];
	SCSubgraphRef UltimateOscillatorOut2 = sc.Subgraph[1];


	SCInputRef Length1 = sc.Input[3];
	SCInputRef Length2 = sc.Input[4];
	SCInputRef Length3 = sc.Input[5];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Ultimate Oscillator";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		UltimateOscillatorOut1.Name = "UO";
		UltimateOscillatorOut1.DrawZeros = false;
		UltimateOscillatorOut1.DrawStyle = DRAWSTYLE_LINE;
		UltimateOscillatorOut1.PrimaryColor = RGB(0,255,0);

		Length1.Name = "Length 1";
		Length1.SetInt(7);
		Length1.SetIntLimits(1,MAX_STUDY_LENGTH);

		Length2.Name = "Length 2";
		Length2.SetInt(14);
		Length2.SetIntLimits(1,MAX_STUDY_LENGTH);

		Length3.Name = " Length 3";
		Length3.SetInt(28);
		Length3.SetIntLimits(1,MAX_STUDY_LENGTH);


		sc.AutoLoop = 1;

		
		

		return;
	}

	sc.DataStartIndex = max(Length1.GetInt(), max(Length2.GetInt(),Length3.GetInt()));

	sc.UltimateOscillator(sc.BaseData, UltimateOscillatorOut1,  UltimateOscillatorOut2, Length1.GetInt(), Length2.GetInt(), Length3.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_WilliamsAD(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AccumulationDistribution = sc.Subgraph[0];

	if(sc.SetDefaults)	
	{
		sc.GraphName="Accumulation/Distribution (Williams)";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		AccumulationDistribution.Name = "AD";
		AccumulationDistribution.DrawStyle = DRAWSTYLE_LINE;
		AccumulationDistribution.PrimaryColor = RGB(0,255,0);


		sc.AutoLoop = 1;

		
		

		return;
	}

	sc.DataStartIndex = 1;

	sc.WilliamsAD(sc.BaseDataIn, AccumulationDistribution, sc.Index);

}

/*==========================================================================*/
SCSFExport scsf_WilliamsR(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentR = sc.Subgraph[0];

	SCInputRef Length = sc.Input[3];
	SCInputRef Invert = sc.Input[4];
	SCInputRef InputDataHigh = sc.Input[5];
	SCInputRef InputDataLow = sc.Input[6];
	SCInputRef InputDataLast = sc.Input[7];
	SCInputRef UpdateFlag = sc.Input[8];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Williams %R";

		sc.GraphRegion= 1;
		sc.ValueFormat = 2;

		PercentR.Name = "%R";
		PercentR.DrawZeros = true;
		PercentR.DrawStyle = DRAWSTYLE_LINE;
		PercentR.PrimaryColor = RGB(0,255,0);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		Invert.Name = "Invert Output";
		Invert.SetYesNo(1);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		InputDataLast.Name = "Input Data for Last";
		InputDataLast.SetInputDataIndex(SC_LAST);

		UpdateFlag.SetInt(1); //update flag

		sc.AutoLoop = 1;


		
		


		return;
	}

	if (UpdateFlag.GetInt() == 0)
	{
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataLow.SetInputDataIndex(SC_LOW);
		InputDataLast.SetInputDataIndex(SC_LAST);
		UpdateFlag.SetInt(1);
	}

	sc.DataStartIndex = Length.GetInt();

	sc.WilliamsR(
		sc.BaseData[InputDataHigh.GetInputDataIndex()],
		sc.BaseData[InputDataLow.GetInputDataIndex()],
		sc.BaseData[InputDataLast.GetInputDataIndex()],
		PercentR,
		sc.Index,
		Length.GetInt()
		);

	if (Invert.GetYesNo())
		PercentR[sc.Index] *= -1;
}

/*==========================================================================*/
SCSFExport scsf_IslandReversal(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_IslandReversal = sc.Subgraph[0];
	SCSubgraphRef Subgraph_GapUp = sc.Subgraph[1];
	SCSubgraphRef Subgraph_GapDown = sc.Subgraph[2];
	SCSubgraphRef Subgraph_GapUpIndex = sc.Subgraph[3];
	SCSubgraphRef Subgraph_GapDownIndex = sc.Subgraph[4];


	if (sc.SetDefaults)
	{
		sc.GraphName = "Island Reversal";

		sc.GraphRegion = 0;
		sc.ValueFormat = 0;

		Subgraph_IslandReversal.Name = "IR";
		Subgraph_IslandReversal.DrawStyle = DRAWSTYLE_POINT;
		Subgraph_IslandReversal.PrimaryColor = RGB(0, 255, 0);
		Subgraph_IslandReversal.LineWidth = 8;
		Subgraph_IslandReversal.DrawZeros = false;

		Subgraph_GapUp.Name = "Gap Up";
		Subgraph_GapUp.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_GapDown.Name = "Gap Down";
		Subgraph_GapDown.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_GapUpIndex.Name = "Gap Up Index";
		Subgraph_GapUpIndex.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_GapDownIndex.Name = "Gap Down Index";
		Subgraph_GapDownIndex.DrawStyle = DRAWSTYLE_IGNORE;

		sc.AutoLoop = 1;

		

		return;
	}

	//Detect gaps in consecutive bars for sc.Index > 0
	if (sc.Index > 0)
	{
		//Gap Up
		if (sc.Low[sc.Index] > sc.High[sc.Index - 1])
		{
			Subgraph_GapUp[sc.Index] = 1;
			Subgraph_GapDown[sc.Index] = Subgraph_GapDown[sc.Index - 1];
			Subgraph_GapUpIndex[sc.Index] = (float)sc.Index;
			Subgraph_GapDownIndex[sc.Index] = Subgraph_GapDownIndex[sc.Index - 1];
		}

		//Gap Down
		else if (sc.High[sc.Index] < sc.Low[sc.Index - 1])
		{
			Subgraph_GapUp[sc.Index] = Subgraph_GapUp[sc.Index - 1];
			Subgraph_GapDown[sc.Index] = 1;
			Subgraph_GapUpIndex[sc.Index] = Subgraph_GapUpIndex[sc.Index - 1];
			Subgraph_GapDownIndex[sc.Index] = (float)sc.Index;
		}

		//No Gap (Overlapping Bars)
		else
		{
			Subgraph_GapUp[sc.Index] = Subgraph_GapUp[sc.Index - 1];
			Subgraph_GapDown[sc.Index] = Subgraph_GapDown[sc.Index - 1];
			Subgraph_GapUpIndex[sc.Index] = Subgraph_GapUpIndex[sc.Index - 1];
			Subgraph_GapDownIndex[sc.Index] = Subgraph_GapDownIndex[sc.Index - 1];
		}
	}

	//Detect Island Reversal
	if (Subgraph_GapUp[sc.Index] == 1 && Subgraph_GapDown[sc.Index] == 1)
	{
		//Gap Up occurs before Gap Down
		if (Subgraph_GapUpIndex[sc.Index] <= Subgraph_GapDownIndex[sc.Index] - 1)
		{
			float GapLevel = max(sc.High[(int)Subgraph_GapUpIndex[sc.Index] - 1], sc.High[(int)Subgraph_GapDownIndex[sc.Index]]);
			int AreBarsAboveGapLevel = 1;

			//Check each bar between Up Gap and Down Gap to make sure it does not go below the Gap Up Level
			for (int BarIndex = (int)Subgraph_GapUpIndex[sc.Index]; BarIndex < (int)Subgraph_GapDownIndex[sc.Index]; BarIndex++)
			{
				if (sc.Low[BarIndex] < GapLevel)
					AreBarsAboveGapLevel = 0;
			}

			if (AreBarsAboveGapLevel == 1)
			{
				Subgraph_IslandReversal[(int)Subgraph_GapUpIndex[sc.Index]] = GapLevel;
				Subgraph_IslandReversal[(int)Subgraph_GapDownIndex[sc.Index] - 1] = GapLevel;

			}

			//Disallow single-bar islands
			if (Subgraph_GapUpIndex[sc.Index] == Subgraph_GapDownIndex[sc.Index] - 1)
				Subgraph_IslandReversal[(int)Subgraph_GapUpIndex[sc.Index]] = 0;

			//Reset these variables to check for next pair of Gaps
			Subgraph_GapUp[sc.Index] = 0;
			Subgraph_GapUpIndex[sc.Index] = 0;
		}

		//Gap Down occurs before Gap Up
		else if (Subgraph_GapDownIndex[sc.Index] <= Subgraph_GapUpIndex[sc.Index] - 1)
		{
			float GapLevel = min(sc.Low[(int)Subgraph_GapDownIndex[sc.Index] - 1], sc.Low[(int)Subgraph_GapUpIndex[sc.Index]]);
			int AreBarsBelowGapLevel = 1;

			//Check each bar between Down Gap and Up Gap to make sure it does not go above the Gap Down Level
			for (int BarIndex = (int)Subgraph_GapDownIndex[sc.Index]; BarIndex < (int)Subgraph_GapUpIndex[sc.Index]; BarIndex++)
			{
				if (sc.High[BarIndex] > GapLevel)
					AreBarsBelowGapLevel = 0;
			}

			if (AreBarsBelowGapLevel == 1)
			{
				Subgraph_IslandReversal[(int)Subgraph_GapUpIndex[sc.Index] - 1] = GapLevel;
				Subgraph_IslandReversal[(int)Subgraph_GapDownIndex[sc.Index]] = GapLevel;
			}

			//Disallow single-bar islands
			if (Subgraph_GapDownIndex[sc.Index] == Subgraph_GapUpIndex[sc.Index] - 1)
				Subgraph_IslandReversal[(int)Subgraph_GapDownIndex[sc.Index]] = 0;

			//Reset these variables to check for next pair of Gaps
			Subgraph_GapDown[sc.Index] = 0;
			Subgraph_GapDownIndex[sc.Index] = 0;
		}
	}
}
/*==========================================================================*/
SCSFExport scsf_WeightedAverageOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Oscillator = sc.Subgraph[0];
	SCSubgraphRef Temp1 = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef SlowAvgLength = sc.Input[3];
	SCInputRef FastAvgLength = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Weighted Average Oscillator";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Oscillator.Name = "Oscillator";
		Oscillator.DrawStyle = DRAWSTYLE_LINE;
		Oscillator.PrimaryColor = RGB(0,255,0);
		Oscillator.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		SlowAvgLength.Name = "Slow Average Length";
		SlowAvgLength.SetInt(10);
		SlowAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FastAvgLength.Name = "Fast Average Length";
		FastAvgLength.SetInt(3);
		FastAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;


		
		

		return;
	}
	sc.DataStartIndex = max( SlowAvgLength.GetInt(),FastAvgLength.GetInt());


	sc.WeightedMovingAverage(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], Temp1, SlowAvgLength.GetInt());
	sc.WeightedMovingAverage(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], Temp2, FastAvgLength.GetInt());

	sc.Oscillator(Temp2, Temp1, Oscillator);
}

/*==========================================================================*/
SCSFExport scsf_PreferredStochasticDiNapoli(SCStudyInterfaceRef sc)
{
	SCSubgraphRef PercentK = sc.Subgraph[0];
	SCSubgraphRef PercentD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];

	SCInputRef FastKLength = sc.Input[3];
	SCInputRef FastDMovAvgLength = sc.Input[4];
	SCInputRef ModifiedMovAvgLength = sc.Input[5];
	SCInputRef Line1Value = sc.Input[6];
	SCInputRef Line2Value = sc.Input[7];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Preferred Stochastic - DiNapoli";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		PercentK.Name = "%K";
		PercentK.PrimaryColor = RGB(0,255,0);
		PercentK.DrawStyle = DRAWSTYLE_LINE;
		PercentK.DrawZeros = false;

		PercentD.Name = "%D";
		PercentD.PrimaryColor = RGB(255,0,255);
		PercentD.DrawStyle = DRAWSTYLE_LINE;
		PercentD.DrawZeros = false;

		Line1.Name = "Line1";
		Line1.PrimaryColor = RGB(255,255,0);
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.DrawZeros = false;

		Line2.Name = "Line2";
		Line2.PrimaryColor = RGB(255,127,0);
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.DrawZeros = false;

		FastKLength.Name = "Fast %K Length";
		FastKLength.SetInt(10);
		FastKLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		FastDMovAvgLength.Name = "Fast %D Mov Avg Length (Slow %K)";
		FastDMovAvgLength.SetInt(3);
		FastDMovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		ModifiedMovAvgLength.Name = "Modified Mov Avg Length (Slow %D)"; 
		ModifiedMovAvgLength.SetInt(3);
		ModifiedMovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30);

		sc.AutoLoop = 1;

		
		

		return;
	}

	int Index = sc.Index;
	sc.DataStartIndex = FastKLength.GetInt() + FastDMovAvgLength.GetInt() + 1;

	sc.Stochastic(sc.BaseDataIn, sc.Subgraph[4], FastKLength.GetInt(), 3, 3, MOVAVGTYPE_SIMPLE);

	sc.ExponentialMovAvg(sc.Subgraph[4], PercentK, Index, FastDMovAvgLength.GetInt());

	PercentD[Index] = PercentD[Index - 1] + 
		((PercentK[Index] - PercentD[Index - 1]) / ModifiedMovAvgLength.GetInt());

	Line1[Index] = Line1Value.GetFloat();
	Line2[Index] = Line2Value.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_ElderRay(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BullPower = sc.Subgraph[0];
	SCSubgraphRef BearPower = sc.Subgraph[1];
	SCSubgraphRef Zero = sc.Subgraph[2];
	SCSubgraphRef MovAvgTemp = sc.Subgraph[3];

	SCInputRef MovAvgLength = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Elder Ray";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		BullPower.Name = "Bull Power";
		BullPower.DrawStyle = DRAWSTYLE_LINE;
		BullPower.PrimaryColor = RGB(0,255,0);
		BullPower.DrawZeros = true;

		BearPower.Name = "Bear Power";
		BearPower.DrawStyle = DRAWSTYLE_LINE;
		BearPower.PrimaryColor = RGB(255,0,255);
		BearPower.DrawZeros = true;

		Zero.Name = "0";
		Zero.DrawStyle = DRAWSTYLE_LINE;
		Zero.PrimaryColor = RGB(255,255,0);
		Zero.DrawZeros = true;




		MovAvgLength.SetInt(10);
		MovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		MovAvgLength.Name = "Moving Average Length";


		sc.AutoLoop = 1;

		
		

		return;
	}

	sc.DataStartIndex = MovAvgLength.GetInt() - 1;


	int i = sc.Index;
	sc.ExponentialMovAvg(sc.Close, MovAvgTemp, i, MovAvgLength.GetInt());


	BullPower[i] = sc.High[i] - MovAvgTemp[i];
	BearPower[i] = sc.Low[i]	- MovAvgTemp[i];
}

/*==========================================================================*/
SCSFExport scsf_MomentumWithMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Momentum = sc.Subgraph[0];
	SCSubgraphRef MovingAverage = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MomentumLength = sc.Input[3];
	SCInputRef MovAvgLength = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Momentum with Moving Average";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Momentum.Name = "Momentum";
		Momentum.DrawStyle = DRAWSTYLE_LINE;
		Momentum.PrimaryColor = RGB(0,255,0);
		Momentum.DrawZeros = false;

		MovingAverage.Name = "Moving Average";
		MovingAverage.DrawStyle = DRAWSTYLE_LINE;
		MovingAverage.PrimaryColor = RGB(128,128,128);
		MovingAverage.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MomentumLength.SetInt(10);
		MomentumLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		MomentumLength.Name = "Momentum Length";

		MovAvgLength.SetInt(5);
		MovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);
		MovAvgLength.Name = "Moving Average Length";

		sc.AutoLoop = 1;


		
		

		return;
	}

	sc.DataStartIndex = MomentumLength.GetInt() + MovAvgLength.GetInt();
	int i = sc.Index;
	if(i < MomentumLength.GetInt())
		return;

	Momentum[i]= ( sc.BaseDataIn[(int)InputData.GetInputDataIndex()][i] / 
		sc.BaseDataIn[(int)InputData.GetInputDataIndex()][i - MomentumLength.GetInt()] ) * 100;

	sc.SimpleMovAvg(Momentum, MovingAverage, i, MovAvgLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_VolatilityChaikin(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volatility = sc.Subgraph[0];

	SCSubgraphRef Temp1 = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];

	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Volatility - Chaikin's";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Volatility.Name = "Volatility";
		Volatility.DrawStyle = DRAWSTYLE_LINE;
		Volatility.PrimaryColor = RGB(0,255,0);

		Length.Name= "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop = 1;

		
		

		return;
	}

	sc.DataStartIndex = Length.GetInt() + 4;
	int i = sc.Index;

	Temp2[i] = sc.High[i] - sc.Low[i];

	sc.ExponentialMovAvg(Temp2, Temp1, i, Length.GetInt());

	Volatility[i] = ((Temp1[i] - Temp1[i - Length.GetInt()])
		/ Temp1[i-Length.GetInt()]) * 100;
}


/*==========================================================================*/
SCSFExport scsf_RateOfChangePoints(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ROC = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Rate Of Change - Points";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		ROC.Name = "ROC";
		ROC.DrawStyle = DRAWSTYLE_LINE;
		ROC.PrimaryColor = RGB(0,255,0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		sc.AutoLoop=1;

		
		

		return;
	}
	sc.DataStartIndex = Length.GetInt();

	ROC[sc.Index] = (sc.BaseDataIn[(int)InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[(int)InputData.GetInputDataIndex()][sc.Index - Length.GetInt()]);
}

/*==========================================================================*/
SCSFExport scsf_SquareOfNine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AnchorSubgraph = sc.Subgraph[0];
	SCSubgraphRef L1PlusSubgraph = sc.Subgraph[1];
	SCSubgraphRef L1MinusSubgraph = sc.Subgraph[2];
	SCSubgraphRef L1Subgraph = sc.Subgraph[3];

	SCFloatArrayRef BaseUpperArray = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef BaseLowerArray = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef OffsetArray = sc.Subgraph[0].Arrays[2];

	SCInputRef NumberOfDaysToCalculate = sc.Input[0];
	SCInputRef AnchorValue = sc.Input[2];
	SCInputRef Degree = sc.Input[3];
	SCInputRef Level = sc.Input[4];
	SCInputRef AutoAdjust = sc.Input[5];
	SCInputRef InMultiplier = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Square of 9";

		sc.GraphRegion = 0;

		const int LineLabelFlags = (LL_DISPLAY_VALUE
			| LL_DISPLAY_NAME
			| LL_NAME_ALIGN_FAR_RIGHT
			| LL_VALUE_ALIGN_FAR_RIGHT
			| LL_NAME_ALIGN_ABOVE
			| LL_VALUE_ALIGN_BELOW
			);

		AnchorSubgraph.Name = "Anchor";
		AnchorSubgraph.DrawStyle = DRAWSTYLE_DASH;// Set line style to Dash
		AnchorSubgraph.PrimaryColor = RGB(0,255,255);
		AnchorSubgraph.DrawZeros = false;
		AnchorSubgraph.LineLabel = LineLabelFlags;

		L1PlusSubgraph.Name = "L1+";
		L1PlusSubgraph.DrawStyle = DRAWSTYLE_DASH;	
		L1PlusSubgraph.PrimaryColor = RGB(0,255,0);
		L1PlusSubgraph.DrawZeros = false;
		L1PlusSubgraph.LineLabel = LineLabelFlags;

		L1MinusSubgraph.Name = "L1-";
		L1MinusSubgraph.DrawStyle = DRAWSTYLE_DASH;
		L1MinusSubgraph.PrimaryColor = RGB(255,0,0);
		L1MinusSubgraph.DrawZeros = false;
		L1MinusSubgraph.LineLabel = LineLabelFlags;

		L1Subgraph.Name = "L1";
		L1Subgraph.DrawStyle = DRAWSTYLE_DASH;
		L1Subgraph.PrimaryColor = RGB(255,127,0);
		L1Subgraph.DrawZeros = false;
		L1Subgraph.LineLabel = LineLabelFlags;

		NumberOfDaysToCalculate.Name = "Number of Days to Calculate";
		NumberOfDaysToCalculate.SetInt(10);
		NumberOfDaysToCalculate.SetIntLimits(1, MAX_STUDY_LENGTH);

		AnchorValue.SetFloat(0);
		AnchorValue.Name = "Anchor Value";
		AnchorValue.SetFloatLimits(0,(float)MAX_STUDY_LENGTH);

		Degree.SetFloat(22.5);    
		Degree.Name = "Degrees";
		Degree.SetFloatLimits(0,360);

		Level.SetFloat(1);    
		Level.Name = "Level";
		Level.SetFloatLimits(1,10);

		AutoAdjust.SetYesNo(false);
		AutoAdjust.Name = "Auto Adjust";

		InMultiplier.SetFloat(1.0f);
		InMultiplier.Name = "Price Multiplier";

		sc.ScaleRangeType = SCALE_SAMEASREGION;

		sc.AutoLoop=1;

		

		return;
	}


	float MultiplierValue = InMultiplier.GetFloat();
	if (MultiplierValue == 0)
		MultiplierValue = 1;


	if (AnchorValue.GetFloat() == 0)
	{
		AnchorValue.SetFloat(sc.Close[sc.ArraySize - 1]*MultiplierValue);
	}

	int LevelInput = (int)(Level.GetFloat() + 0.5f);

	if (sc.BaseDateTimeIn[sc.Index].GetDate() < sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate() - NumberOfDaysToCalculate.GetInt() + 1)
		return;


	if (AutoAdjust.GetYesNo())
	{
		float SquareRoot = sqrt(AnchorValue.GetFloat());

		float Offset = 0.0f;
		float Mid = 0.0f;
		float BaseUpper = 0.0f;
		float BaseLower = 0.0f;
		float Upper = 0.0f;
		float Lower = 0.0f;

		if(sc.Index == 0)
		{
			Offset = 0;
			BaseUpper = (SquareRoot + Degree.GetFloat()/180) * (SquareRoot + Degree.GetFloat()/180);
			BaseLower = (SquareRoot - Degree.GetFloat()/180) * (SquareRoot - Degree.GetFloat()/180);
			Mid = AnchorValue.GetFloat();
			Upper = (SquareRoot + LevelInput * Degree.GetFloat()/180) * (SquareRoot + LevelInput * Degree.GetFloat()/180);
			Lower = (SquareRoot - LevelInput * Degree.GetFloat()/180) * (SquareRoot - LevelInput * Degree.GetFloat()/180);
		}
		else
		{
			Upper = L1PlusSubgraph[sc.Index-1] * MultiplierValue;
			Lower = L1MinusSubgraph[sc.Index-1] * MultiplierValue;
			Mid = L1Subgraph[sc.Index-1] * MultiplierValue;

			BaseUpper = BaseUpperArray[sc.Index-1];
			BaseLower = BaseLowerArray[sc.Index-1];
			Offset = OffsetArray[sc.Index-1];
		}


		SCString TempStr;
		TempStr.Format("+%g deg", LevelInput * Degree.GetFloat());

		L1PlusSubgraph.Name = TempStr;

		TempStr.Format("-%g deg", LevelInput * Degree.GetFloat());
		L1MinusSubgraph.Name = TempStr;

		L1Subgraph.Name = "Mid";

		float PriceInput = sc.Close[sc.Index] * MultiplierValue;


		while (PriceInput > BaseUpper)
		{
			Offset += Degree.GetFloat();
			BaseUpper = (SquareRoot + (Offset + Degree.GetFloat())/180) * (SquareRoot + (Offset + Degree.GetFloat())/180);
			BaseLower = (SquareRoot + (Offset - Degree.GetFloat())/180) * (SquareRoot + (Offset - Degree.GetFloat())/180);
			Upper = (SquareRoot + (Offset + LevelInput * Degree.GetFloat())/180) * (SquareRoot + (Offset + LevelInput * Degree.GetFloat())/180);
			Lower = (SquareRoot + (Offset - LevelInput * Degree.GetFloat())/180) * (SquareRoot + (Offset - LevelInput * Degree.GetFloat())/180);
			Mid = (SquareRoot + Offset/180) * (SquareRoot + Offset/180);
		}
		while (PriceInput < BaseLower)
		{
			Offset -= Degree.GetFloat();
			BaseUpper = (SquareRoot + (Offset + Degree.GetFloat())/180) * (SquareRoot + (Offset + Degree.GetFloat())/180);
			BaseLower = (SquareRoot + (Offset - Degree.GetFloat())/180) * (SquareRoot + (Offset - Degree.GetFloat())/180);
			Upper = (SquareRoot + (Offset + LevelInput * Degree.GetFloat())/180) * (SquareRoot + (Offset + LevelInput * Degree.GetFloat())/180);
			Lower = (SquareRoot + (Offset - LevelInput * Degree.GetFloat())/180) * (SquareRoot + (Offset - LevelInput * Degree.GetFloat())/180);

			Mid = (SquareRoot + Offset/180) * (SquareRoot + Offset/180);
		}

		AnchorSubgraph[sc.Index] = AnchorValue.GetFloat() / MultiplierValue;
		L1PlusSubgraph[sc.Index] = Upper / MultiplierValue;
		L1MinusSubgraph[sc.Index] = Lower / MultiplierValue;
		L1Subgraph[sc.Index] = Mid / MultiplierValue;
		BaseUpperArray[sc.Index] = BaseUpper;
		BaseLowerArray[sc.Index] = BaseLower;
		OffsetArray[sc.Index] = Offset;

	}
	else
	{
		float L1 = 0.0f;
		float L2 = 0.0f;
		SCString PositiveDegrees; 
		SCString NegativeDegrees;

		float squareRoot = sqrt(AnchorValue.GetFloat());
		float localDegree = Degree.GetFloat() * LevelInput;

		PositiveDegrees.Format("+%g deg", localDegree);
		NegativeDegrees.Format("-%g deg", localDegree);

		L1 = (squareRoot + localDegree/180) * (squareRoot + localDegree/180);
		L2 = (squareRoot - localDegree/180) * (squareRoot - localDegree/180);

		AnchorSubgraph[sc.Index] = AnchorValue.GetFloat() / MultiplierValue;
		L1PlusSubgraph[sc.Index] = L1 / MultiplierValue;
		L1MinusSubgraph[sc.Index] = L2 / MultiplierValue;
	}
}


/*==========================================================================*/
SCSFExport scsf_LinearRegressionEndChannel(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LinearRegression = sc.Subgraph[0];
	SCSubgraphRef StdDevHigh = sc.Subgraph[1];
	SCSubgraphRef StdDevLow = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Input_PeriodType = sc.Input[1];
	SCInputRef Input_NumberOfBarsToCalculate = sc.Input[2];
	SCInputRef NumberDeviations = sc.Input[3];
	SCInputRef UseStartDateTime = sc.Input[4];
	SCInputRef StartDateTime = sc.Input[5];
	SCInputRef CalculateOnBarClose = sc.Input[6];
	SCInputRef Input_Version = sc.Input[7];

	enum PeriodTypeEnum
	{ PERIOD_TYPE_NUMBER_OF_BARS = 0
	, PERIOD_TYPE_FROM_START_DATE_TIME = 1
	, PERIOD_TYPE_FROM_START_TIME = 2
	};

	static const int MAX_BARS_TO_CALCULATE = 10000;

	if(sc.SetDefaults)
	{
		sc.GraphName = "Linear Regression End Channel";

		sc.GraphRegion = 0;
		sc.ValueFormat =VALUEFORMAT_INHERITED;
		sc.AutoLoop = false;

		LinearRegression.Name = "Linear Regression";
		LinearRegression.DrawStyle = DRAWSTYLE_LINE;
		LinearRegression.PrimaryColor = RGB(0,255,0);
		LinearRegression.DrawZeros = false;

		StdDevHigh.Name = "StdDev High";
		StdDevHigh.DrawStyle = DRAWSTYLE_LINE;
		StdDevHigh.PrimaryColor = RGB(255,0,255);
		StdDevHigh.DrawZeros = false;

		StdDevLow.Name = "StdDev Low";
		StdDevLow.DrawStyle = DRAWSTYLE_LINE;
		StdDevLow.PrimaryColor = RGB(255,255,0);
		StdDevLow.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Input_PeriodType.Name = "Period Type";
		Input_PeriodType.SetCustomInputStrings("Number of Bars;From Start Date-Time;From Start Time");
		Input_PeriodType.SetCustomInputIndex(0);

		Input_NumberOfBarsToCalculate.Name = "Number of Bars";
		Input_NumberOfBarsToCalculate.SetInt(10);
		Input_NumberOfBarsToCalculate.SetIntLimits(1, MAX_BARS_TO_CALCULATE);

		NumberDeviations.Name = "Number of Deviations";
		NumberDeviations.SetFloat(2.0f);
		NumberDeviations.SetFloatLimits(0.001f, 2000);

		//UseStartDateTime.Name = "Use Start Date-Time instead of Number of Bars";
		UseStartDateTime.SetYesNo(false);

		StartDateTime.Name = "Start Date and Time";
		StartDateTime.SetDateTime(0);

		CalculateOnBarClose.Name = "Calculate On Bar Close";
		CalculateOnBarClose.SetYesNo(false);

		Input_Version.SetInt(1);

		
		

		return;
	}

	if (Input_Version.GetInt() < 1)
	{
		if (UseStartDateTime.GetYesNo())
			Input_PeriodType.SetCustomInputIndex(1);

		Input_Version.SetInt(1);
	}

	int NumberOfBarsToCalculate = Input_NumberOfBarsToCalculate.GetInt();
	int InputDataOffset = 0;

	if(CalculateOnBarClose.GetYesNo())
		InputDataOffset = 1;

	int ArraySize = sc.ArraySize;
	// Formula is  y= a+bx
	// a= (sumY-(b)sumX)/n
	// b= ((n)sumXY-sumX*sumY)/((n)sumXX-(sumY)(sumY))
	float sumY = 0;
	float sumX = 0;
	float sumXY = 0;
	float sumXX = 0;
	float StandardDev = 0;
	float tempY, a, b;
	int n = 0;	

	int StartIndex = 0;

	switch (Input_PeriodType.GetIndex())
	{
		case PERIOD_TYPE_NUMBER_OF_BARS:
			StartIndex = max(0, ArraySize - NumberOfBarsToCalculate - InputDataOffset);
		break;

		case PERIOD_TYPE_FROM_START_DATE_TIME:
		{
			NumberOfBarsToCalculate = MAX_BARS_TO_CALCULATE;

			StartIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, StartDateTime.GetDateTime());

			if (StartIndex < ArraySize - MAX_BARS_TO_CALCULATE)
				StartIndex = ArraySize - MAX_BARS_TO_CALCULATE;
			else
				NumberOfBarsToCalculate = ArraySize - StartIndex;
		}
		break;

		case PERIOD_TYPE_FROM_START_TIME:
		{
			const int LastBarDate = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();
			const int StartTime = StartDateTime.GetTime();

			const SCDateTime CurrentDayStartDateTime(LastBarDate, StartTime);

			StartIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, CurrentDayStartDateTime);

			NumberOfBarsToCalculate = ArraySize - StartIndex;
		}
		break;
	};

	for (int Index = StartIndex; Index < StartIndex + NumberOfBarsToCalculate && Index < ArraySize; Index++)
	{
		tempY = sc.BaseDataIn[InputData.GetInputDataIndex()][Index];
		n++;
		sumY += tempY;
		sumX += n;
		sumXY += (tempY*n);
		sumXX += n*n;
	}

	b = (n*sumXY - (sumX*sumY))/((n*sumXX) - (sumX*sumX));
	a = (sumY - (b*sumX))/n;

	// Solve StandardDev sqrt( sum((Xi-Xbar)^2)/n )
	float tempX = 0;
	for (int Index = StartIndex; Index < StartIndex + NumberOfBarsToCalculate && Index < ArraySize; Index++)
	{
		float diff = sc.BaseDataIn[InputData.GetInputDataIndex()][Index] - (a + b * (Index-StartIndex+1));
		tempX += (diff * diff);
	}

	StandardDev = sqrt(tempX/n);

	// Fill in the Linear Regression data
	for(int Index = 0; Index < n; Index++)
	{
		LinearRegression[StartIndex+Index] = a + b * (Index+1);
		StdDevHigh[StartIndex+Index] = LinearRegression[StartIndex+Index] + (StandardDev * NumberDeviations.GetFloat());
		StdDevLow[StartIndex+Index] = LinearRegression[StartIndex+Index] - (StandardDev * NumberDeviations.GetFloat());
	}

	if (!UseStartDateTime.GetYesNo())
	{
		// Zero out points that are not within the scope
		int nNewElements = sc.Open.GetArraySize() - 1 - sc.Index;
		for (int i = ArraySize - 1 - NumberOfBarsToCalculate - InputDataOffset; i >= 0 && nNewElements > 0; i--)
		{
			LinearRegression[i] = 0;
			StdDevHigh[i] = 0;
			StdDevLow[i] = 0;
			nNewElements--;
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_PriceVolumeTrend(SCStudyInterfaceRef sc)
{	
	SCSubgraphRef PVT = sc.Subgraph[0];

	if(sc.SetDefaults)
	{
		sc.GraphName = "Price Volume Trend";
		sc.GraphRegion = 1;

		PVT.Name = "PVT";
		PVT.DrawStyle = DRAWSTYLE_LINE;
		PVT.PrimaryColor = RGB(0,255,0);

		sc.AutoLoop = 1;

		
		
		return;
	}
	sc.DataStartIndex = 1;
	sc.PriceVolumeTrend(sc.BaseDataIn, PVT);
}

/*==========================================================================*/
SCSFExport scsf_StandardDeviationBands(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TopBand = sc.Subgraph[0];
	SCSubgraphRef BottomBand = sc.Subgraph[1];
	SCSubgraphRef TopMovAvg = sc.Subgraph[2];
	SCSubgraphRef BottomMovAvg = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Temp5 = sc.Subgraph[5];

	SCInputRef TopBandInputData = sc.Input[0];
	SCInputRef BottomBandInputData = sc.Input[1];
	SCInputRef Length = sc.Input[3];
	SCInputRef MultFactor = sc.Input[5];
	SCInputRef MovAvgType = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Standard Deviation Bands";

		sc.GraphRegion = 0;


		TopBand.Name = "Top";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(0,255,0);


		BottomBand.Name = "Bottom";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(0,255,0);

		TopMovAvg.Name = "Top MovAvg";
		TopMovAvg.DrawStyle = DRAWSTYLE_LINE;
		TopMovAvg.PrimaryColor = RGB(0,0,255);

		BottomMovAvg.Name = "Bottom MovAvg";
		BottomMovAvg.DrawStyle = DRAWSTYLE_LINE;
		BottomMovAvg.PrimaryColor = RGB(0,0,255);

		TopBandInputData.Name = "Top Band Input Data";
		TopBandInputData.SetInputDataIndex(SC_HIGH);

		BottomBandInputData.Name = "Bottom Band Input Data";
		BottomBandInputData.SetInputDataIndex(SC_LOW);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		MultFactor.Name = "Multiplication Factor";
		MultFactor.SetFloat(1.8f);
		MultFactor.SetFloatLimits(0.1f, 20.0f);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		sc.AutoLoop = true;

		
		

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;
	int Index = sc.Index;
	SCFloatArrayRef HighArray = sc.BaseDataIn[TopBandInputData.GetInputDataIndex()];
	SCFloatArrayRef LowArray = sc.BaseDataIn[BottomBandInputData.GetInputDataIndex()];

	sc.MovingAverage(HighArray, TopMovAvg, MovAvgType.GetMovAvgType(), Index, Length.GetInt());
	sc.MovingAverage(LowArray, BottomMovAvg, MovAvgType.GetMovAvgType(), Index, Length.GetInt());

	sc.StdDeviation(HighArray, Temp4, Index, Length.GetInt());
	sc.StdDeviation(LowArray, Temp5, Index, Length.GetInt());

	TopBand[Index] = (Temp4[Index] * MultFactor.GetFloat()) + TopMovAvg[Index];
	BottomBand[Index] = (Temp5[Index] * -MultFactor.GetFloat()) +  BottomMovAvg[Index];
}

/*==========================================================================*/
SCSFExport scsf_HighestLowestBars(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Highest = sc.Subgraph[0];
	SCSubgraphRef Lowest = sc.Subgraph[1];

	SCInputRef HighestLength = sc.Input[0];
	SCInputRef LowestLength = sc.Input[1];
	SCInputRef InputData = sc.Input[2];

	if(sc.SetDefaults)
	{
		sc.GraphName = "NumberOfBarsSinceHighestValue / NumberOfBarsSinceLowestValue";
		sc.GraphRegion = 2;

		Highest.Name = "Highest";
		Highest.DrawStyle = DRAWSTYLE_LINE;
		Highest.PrimaryColor = RGB(0,255,0);

		Lowest.Name = "Lowest";
		Lowest.DrawStyle = DRAWSTYLE_LINE;
		Lowest.PrimaryColor = RGB(255,0,255);

		HighestLength.Name = "HighestLength";
		HighestLength.SetInt(10);
		HighestLength.SetIntLimits(2,MAX_STUDY_LENGTH);

		LowestLength.Name = "LowestLength";
		LowestLength.SetInt(10);
		LowestLength.SetIntLimits(2,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		sc.AutoLoop = 1;

		
		

		return;
	}

	SCFloatArrayRef In = sc.BaseDataIn[InputData.GetInputDataIndex()];

	sc.DataStartIndex = max(HighestLength.GetInt(), LowestLength.GetInt()) - 1;

	Highest[sc.Index] = (float)sc.NumberOfBarsSinceHighestValue(In, sc.Index, HighestLength.GetInt());
	Lowest[sc.Index] = (float)sc.NumberOfBarsSinceLowestValue(In, sc.Index, LowestLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_VolumeUp(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	SCInputRef CompareType = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Volume-Up";

		sc.StudyDescription = "The volume for up bars only.";

		sc.AutoLoop = 1;

		sc.ValueFormat = 0;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.DrawZeros = false;

		CompareType.Name = "Compare Close with: 0=PrevClose, 1=Open";
		CompareType.SetInt(0);
		CompareType.SetIntLimits(0,1);

		
		

		return;
	}

	// Do data processing

	bool IsUpBar = false;
	switch (CompareType.GetInt())
	{
	case 0:
		if (sc.Index > 0)
			IsUpBar = sc.Close[sc.Index] > sc.Close[sc.Index - 1];
		break;

	case 1:
		IsUpBar = sc.Close[sc.Index] > sc.Open[sc.Index];
		break;
	}

	if (IsUpBar)
		Volume[sc.Index] = sc.Volume[sc.Index];
	else
		Volume[sc.Index] = 0;
}

/*==========================================================================*/
SCSFExport scsf_VolumeDown(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	SCInputRef CompareType = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Volume-Down";

		sc.StudyDescription = "The volume for down bars only.";

		sc.AutoLoop = 1;

		sc.ValueFormat = 0;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.DrawZeros = false;

		CompareType.Name = "Compare Close with: 0=PrevClose, 1=Open";
		CompareType.SetInt(0);
		CompareType.SetIntLimits(0,1);

		
		

		return;
	}

	// Do data processing

	bool IsDownBar = false;
	switch (CompareType.GetInt())
	{
	case 0:
		if (sc.Index > 0)
			IsDownBar = sc.Close[sc.Index] < sc.Close[sc.Index - 1];
		break;

	case 1:
		IsDownBar = sc.Close[sc.Index] < sc.Open[sc.Index];
		break;
	}

	if (IsDownBar)
		Volume[sc.Index] = sc.Volume[sc.Index];
	else
		Volume[sc.Index] = 0;
}

/*==========================================================================*/
SCSFExport scsf_BarEndTime(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BET = sc.Subgraph[0];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Bar End Time";

		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_TIME;

		BET.Name = "BET";
		BET.DrawStyle = DRAWSTYLE_BAR;
		BET.PrimaryColor = RGB(0,255,0);
		BET.DrawZeros = false;

		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;

		return;
	}

	BET[sc.Index] =  static_cast<float>(sc.BaseDataEndDateTime[sc.Index].GetTimeAsSCDateTimeMS().GetAsDouble());
}

/*==========================================================================*/
SCSFExport scsf_CorrelationCoefficient(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_CorrelationCoefficient = sc.Subgraph[0];

	SCInputRef InputArray1 = sc.Input[0];
	SCInputRef InputArray2 = sc.Input[1];
	SCInputRef Length = sc.Input[2];

	if(sc.SetDefaults)	
	{
		sc.GraphName = "Correlation Coefficient";

		sc.GraphRegion = 1;

		Subgraph_CorrelationCoefficient.Name = "CC";
		Subgraph_CorrelationCoefficient.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_CorrelationCoefficient.PrimaryColor = RGB(0,255,0);
		Subgraph_CorrelationCoefficient.DrawZeros = true;

		InputArray1.Name = "Input Array 1";
		InputArray1.SetStudySubgraphValues(0,0);

		InputArray2.Name = "Input Array 2";
		InputArray2.SetStudySubgraphValues(0,0);

		Length.Name = "Length";
		Length.SetInt(200);

		sc.AutoLoop = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;


		
		

		return;
	}
	sc.DataStartIndex = Length.GetInt();
	SCFloatArray Array1;
	SCFloatArray Array2;

	sc.GetStudyArrayUsingID(InputArray1.GetStudyID(),InputArray1.GetSubgraphIndex(),Array1);
	if(Array1.GetArraySize() < sc.ArraySize)
		return;

	sc.GetStudyArrayUsingID(InputArray2.GetStudyID(),InputArray2.GetSubgraphIndex(),Array2);
	if(Array2.GetArraySize() < sc.ArraySize)
		return;

	Subgraph_CorrelationCoefficient[sc.Index] = sc.GetCorrelationCoefficient(Array1, Array2, Length.GetInt());

}


/*==========================================================================*/
SCSFExport scsf_SierraSqueeze(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MomentumHist = sc.Subgraph[0];
	SCSubgraphRef SqueezeDots = sc.Subgraph[1];
	SCSubgraphRef MomentumHistUpColors = sc.Subgraph[2];
	SCSubgraphRef MomentumHistDownColors = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	//SCSubgraphRef Temp5 = sc.Subgraph[5];
	SCSubgraphRef SignalValues = sc.Subgraph[6];
	SCSubgraphRef Temp8 = sc.Subgraph[8];

	SCInputRef InputData = sc.Input[0];
	SCInputRef HistogramLenFirstData = sc.Input[1];
	SCInputRef SqueezeLength = sc.Input[2];
	SCInputRef NK = sc.Input[3];
	SCInputRef NB = sc.Input[4];
	SCInputRef FirstMovAvgType = sc.Input[5];
	SCInputRef HistogramLenSecondData = sc.Input[6];
	SCInputRef SecondMovAvgType = sc.Input[7];
	SCInputRef VersionUpdate = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Squeeze Indicator 2";
		sc.StudyDescription	= "Developed by the user Tony C.";

		MomentumHist.Name = "Momentum HISTOGRAM";
		MomentumHist.DrawStyle = DRAWSTYLE_BAR;
		MomentumHist.PrimaryColor = RGB(0,255,0);
		MomentumHist.LineWidth = 5;
		MomentumHist.DrawZeros = true;

		SqueezeDots.Name = "Squeeze Dots";
		SqueezeDots.DrawStyle = DRAWSTYLE_POINT;
		SqueezeDots.PrimaryColor = RGB(255,0,255);
		SqueezeDots.LineWidth = 4;
		SqueezeDots.DrawZeros = true;

		MomentumHistUpColors.Name = "Momentum HISTOGRAM Up Colors";
		MomentumHistUpColors.DrawStyle = DRAWSTYLE_IGNORE;
		MomentumHistUpColors.SecondaryColorUsed = 1;
		MomentumHistUpColors.PrimaryColor = RGB(0, 0, 255);
		MomentumHistUpColors.SecondaryColor = RGB(0, 0, 130);
		MomentumHistUpColors.DrawZeros = true;

		MomentumHistDownColors.Name	= "Momentum HISTOGRAM Down Colors";
		MomentumHistDownColors.DrawStyle = DRAWSTYLE_IGNORE;
		MomentumHistDownColors.SecondaryColorUsed = 1;
		MomentumHistDownColors.PrimaryColor = RGB(255, 0, 0);
		MomentumHistDownColors.SecondaryColor = RGB(130, 0, 0);
		MomentumHistDownColors.DrawZeros = true;

		SignalValues.Name = "Signal Values";
		SignalValues.DrawStyle = DRAWSTYLE_IGNORE;
		SignalValues.PrimaryColor = RGB(127,0,255);
		SignalValues.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST); // default data field		

		HistogramLenFirstData.Name = "Histogram Length First Data";
		HistogramLenFirstData.SetInt(20);
		HistogramLenFirstData.SetIntLimits(1,MAX_STUDY_LENGTH);

		SqueezeLength.Name = "Squeeze Length";
		SqueezeLength.SetFloat(20);

		NK.Name = "NK.GetFloat()";
		NK.SetFloat(1.5);

		NB.Name = "NB.GetFloat()";
		NB.SetFloat(2);

		FirstMovAvgType.Name = "First MA Type";
		FirstMovAvgType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		HistogramLenSecondData.Name = "Histogram Length Second Data";
		HistogramLenSecondData.SetInt(20);
		HistogramLenSecondData.SetIntLimits(1,MAX_STUDY_LENGTH);

		SecondMovAvgType.Name = "Second MA Type";
		SecondMovAvgType.SetMovAvgType(MOVAVGTYPE_LINEARREGRESSION);

		// hidden input for old versions support
		VersionUpdate.SetInt(1);

		sc.AutoLoop = 1;
		

		return;
	}

	// upgrading the default settings
	if (VersionUpdate.GetInt() != 1)
	{
		MomentumHistUpColors.Name = "Momentum Histogram Up Colors";
		MomentumHistUpColors.DrawStyle = DRAWSTYLE_IGNORE;
		MomentumHistUpColors.SecondaryColorUsed	= 1;
		MomentumHistUpColors.PrimaryColor = RGB(0, 0, 255);
		MomentumHistUpColors.SecondaryColor = RGB(0, 0, 130);

		MomentumHistDownColors.Name = "Momentum Histogram Down Colors";
		MomentumHistDownColors.DrawStyle = DRAWSTYLE_IGNORE;
		MomentumHistDownColors.SecondaryColorUsed = 1;
		MomentumHistDownColors.PrimaryColor = RGB(255, 0, 0);
		MomentumHistDownColors.SecondaryColor = RGB(130, 0, 0);

		SignalValues.Name = "Signal Values";
		SignalValues.DrawStyle = DRAWSTYLE_IGNORE;

		VersionUpdate.SetInt(1);
	}

	// Inputs
	int i = sc.Index;
	const DWORD inside		= RGB(255, 0, 0);	
	const DWORD outside		= RGB(0, 255, 0);	

	MomentumHistUpColors[i] = 0;
	MomentumHistDownColors[i] = 0;

	// First output elements are not valid
	sc.DataStartIndex = HistogramLenSecondData.GetInt();

	SCFloatArrayRef close =  sc.Close;
	sc.ExponentialMovAvg(close, MomentumHistUpColors, HistogramLenSecondData.GetInt());  // Note: EMA returns close when index is < HistogramLenSecondData.GetInt()
	sc.MovingAverage(close,  MomentumHistUpColors, FirstMovAvgType.GetMovAvgType(), HistogramLenFirstData.GetInt());

	float hlh = sc.GetHighest(sc.High, HistogramLenSecondData.GetInt());
	float lll = sc.GetLowest(sc.Low, HistogramLenSecondData.GetInt());

	SCFloatArrayRef price = sc.Open;

	MomentumHistDownColors[sc.Index] = price[sc.Index] - ((hlh + lll)/2.0f + MomentumHistUpColors[sc.Index])/2.0f;
	sc.LinearRegressionIndicator(MomentumHistDownColors, MomentumHist, HistogramLenSecondData.GetInt());
	sc.MovingAverage(close,  MomentumHistUpColors, SecondMovAvgType.GetMovAvgType(), HistogramLenSecondData.GetInt());


	if(
		(MomentumHist[i]<0) 
		&&(MomentumHist[i] < MomentumHist[i-1])
		)

	{
		MomentumHist.DataColor[sc.Index] = MomentumHistDownColors.PrimaryColor;		
	}
	else if(
		(MomentumHist[i]<=0) 
		&&(MomentumHist[i] > MomentumHist[i-1])
		)
	{
		MomentumHist.DataColor[sc.Index] = MomentumHistDownColors.SecondaryColor;		
	}
	else if(
		(MomentumHist[i]>0) 
		&&(MomentumHist[i] > MomentumHist[i-1])
		)
	{
		MomentumHist.DataColor[sc.Index] = MomentumHistUpColors.PrimaryColor;		
	}
	else if(
		(MomentumHist[i]>=0) 
		&&(MomentumHist[i] < MomentumHist[i-1])
		)
	{
		MomentumHist.DataColor[sc.Index] = MomentumHistUpColors.SecondaryColor;		
	}


	//Squeeze
	sc.Keltner(
		sc.BaseDataIn,
		sc.Close,
		Temp8,
		SqueezeLength.GetInt(),
		MOVAVGTYPE_SMOOTHED,
		SqueezeLength.GetInt(),
		MOVAVGTYPE_SMOOTHED,
		NK.GetFloat(),
		NK.GetFloat()
		);

	float TopBandOut = Temp8.Arrays[0][sc.Index];
	float BottomBandOut = Temp8.Arrays[1][sc.Index];

	sc.BollingerBands(sc.Close, Temp4, SqueezeLength.GetInt(), NB.GetFloat(), MOVAVGTYPE_SMOOTHED);

	float BU =Temp4.Arrays[0][sc.Index];
	float BL =Temp4.Arrays[1][sc.Index];

	if (
		(BU < TopBandOut)
		|| (BL > BottomBandOut)
		)
	{
		SqueezeDots[sc.Index] = 0.0;
		SqueezeDots.DataColor[sc.Index] = inside;		
		SignalValues[sc.Index] = 0.0;
		SignalValues.DataColor[sc.Index] = inside;
	}
	else
	{
		SqueezeDots[sc.Index] = 0.0;
		SqueezeDots.DataColor[sc.Index] = outside;		
		SignalValues[sc.Index] = 1.0;
		SignalValues.DataColor[sc.Index] = outside;
	}
}

/*==========================================================================*/
SCSFExport scsf_R_Squared(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSquared = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];
	SCInputRef InputData = sc.Input[1];

	if(sc.SetDefaults)
	{
		sc.GraphName="R Squared";
		sc.StudyDescription="R Squared Indicator by ertrader.";


		sc.AutoLoop = 1;
		sc.GraphRegion = 1;

		RSquared.Name="R Squared";
		RSquared.DrawStyle = DRAWSTYLE_LINE;
		RSquared.LineWidth = 2;
		RSquared.PrimaryColor = COLOR_GREEN;
		RSquared.SecondaryColor = COLOR_RED;
		RSquared.DrawZeros = false;

		Length.Name="Length";
		Length.SetInt(14);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);        
		return;
	}

	// Start plotting after good output elements are valid
	sc.DataStartIndex = Length.GetInt();

	// R^2 indicator:  Pwr(Corr(Cum( 1 ),C,14,0),2)  or Correlation_Coefficient^2.  This method calculates R^ with minimal lag.
	SCFloatArrayRef PriceData = sc.BaseData[InputData.GetInputDataIndex()];

	//The following is a cumulative calculation.  It sums up a count of 1 for each bar.  It is then used to correlate data against    
	RSquared.Arrays[0][sc.Index] = (float)(sc.Index + 1);

	//Get the correlation coefficient. Correlate Price Data against the cumulation
	float coef = sc.GetCorrelationCoefficient(RSquared.Arrays[0], PriceData, sc.Index, Length.GetInt());

	// Calculate and Draw R squared
	//Pwr(Corr(Cum( 1 ),C,14,0),2) or Correlation_Coefficient^2
	RSquared[sc.Index]=coef*coef; 
}  


/*==========================================================================*/
//http://en.wikipedia.org/wiki/Exponential_function
SCSFExport scsf_RSIInverseFisherTransform(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSI_IFT = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef RSI = sc.Subgraph[3];

	SCInputRef RSILength = sc.Input[0];
	SCInputRef RSIInternalMovAvgType = sc.Input[1];
	SCInputRef RSIMovAvgType = sc.Input[2];
	SCInputRef RSIMovAvgLength = sc.Input[3];
	SCInputRef LineValue = sc.Input[4];

	if(sc.SetDefaults)
	{
		sc.GraphName="Inverse Fisher Transform RSI";
		sc.StudyDescription="Inverse Fisher Transform RSI by John Ehlers.";


		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		

		RSI_IFT.Name="RSI IFT";
		RSI_IFT.DrawStyle = DRAWSTYLE_LINE;
		RSI_IFT.LineWidth = 2;
		RSI_IFT.PrimaryColor = COLOR_GREEN;
		RSI_IFT.DrawZeros = true;

		Line1.Name="Line 1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.LineWidth = 1;
		Line1.PrimaryColor = COLOR_RED;
		Line1.DrawZeros = true;

		Line2.Name="Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.LineWidth = 1;
		Line2.PrimaryColor = COLOR_BLUE;
		Line2.DrawZeros = true;

		RSILength.Name="RSI Length";
		RSILength.SetInt(5);
		RSILength.SetIntLimits(1,MAX_STUDY_LENGTH);

		RSIInternalMovAvgType.Name="RSI Internal MovAvg Type";
		RSIInternalMovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		RSIMovAvgType.Name="RSI MovAvg Type";
		RSIMovAvgType.SetMovAvgType(MOVAVGTYPE_WEIGHTED);

		RSIMovAvgLength.Name="RSI MovAvg Length";
		RSIMovAvgLength.SetInt(9);
		RSIMovAvgLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		LineValue.Name="Line Value";
		LineValue.SetFloat(0.5);

		return;
	}


	sc.DataStartIndex = RSILength.GetInt()+RSIMovAvgLength.GetInt();

	sc.InverseFisherTransformRSI(sc.Close, RSI_IFT, RSILength.GetInt(), RSIInternalMovAvgType.GetMovAvgType(), RSIMovAvgLength.GetInt(), RSIMovAvgType.GetMovAvgType());

	Line1[sc.Index] = LineValue.GetFloat();
	Line2[sc.Index] = -LineValue.GetFloat();
}

/*==========================================================================*/

SCSFExport scsf_InverseFisherTransform(SCStudyInterfaceRef sc)
{
	SCSubgraphRef InverseFisherTransform = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef IFTAvg = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef LineValue = sc.Input[1];
	SCInputRef OutputMovAvgLength = sc.Input[2];
	SCInputRef OutputMovAvgType = sc.Input[3];
	SCInputRef InputMovAvgLength = sc.Input[4];
	SCInputRef InputMovAvgType = sc.Input[5];
	SCInputRef HighestLowestLength = sc.Input[6];

	if(sc.SetDefaults)
	{
		sc.GraphName="Inverse Fisher Transform";
		sc.StudyDescription="Inverse Fisher Transform.";


		sc.AutoLoop = 1;
		sc.GraphRegion = 1;
		

		InverseFisherTransform.Name="IFT";
		InverseFisherTransform.DrawStyle = DRAWSTYLE_LINE;
		InverseFisherTransform.LineWidth = 2;
		InverseFisherTransform.PrimaryColor = COLOR_GREEN;
		InverseFisherTransform.DrawZeros = true;

		Line1.Name="Line 1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.LineWidth = 1;
		Line1.PrimaryColor = COLOR_RED;
		Line1.DrawZeros = true;

		Line2.Name="Line 2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.LineWidth = 1;
		Line2.PrimaryColor = COLOR_BLUE;
		Line2.DrawZeros = true;

		IFTAvg.Name="IFT Avg";
		IFTAvg.DrawStyle = DRAWSTYLE_LINE;
		IFTAvg.LineWidth = 1;
		IFTAvg.PrimaryColor = RGB(255,255,0);
		IFTAvg.DrawZeros = true;

		InputData.Name="Input Data";
		InputData.SetInputDataIndex(0);

		LineValue.Name="Line Value";
		LineValue.SetFloat(0.5);

		OutputMovAvgLength.Name="Output Mov Avg Length";
		OutputMovAvgLength.SetInt(3);

		OutputMovAvgType.Name="Output Mov Avg Type";
		OutputMovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputMovAvgLength.Name="Input Data Mov Avg Length";
		InputMovAvgLength.SetInt(9);

		InputMovAvgType.Name="Input Data Mov Avg Type";
		InputMovAvgType.SetMovAvgType(MOVAVGTYPE_WEIGHTED);

		HighestLowestLength.Name = "Highest Value/Lowest Value Length";
		HighestLowestLength.SetInt(10);

		return;
	}

	sc.DataStartIndex = InputMovAvgLength.GetInt()+OutputMovAvgLength.GetInt();

	sc.InverseFisherTransform(sc.BaseData[InputData.GetInputDataIndex()], InverseFisherTransform, HighestLowestLength.GetInt(), InputMovAvgLength.GetInt(), InputMovAvgType.GetMovAvgType());


	sc.MovingAverage(InverseFisherTransform, IFTAvg, OutputMovAvgType.GetMovAvgType(),OutputMovAvgLength.GetInt());

	Line1[sc.Index] = LineValue.GetFloat();
	Line2[sc.Index] = -LineValue.GetFloat();
}

/*============================================================================
Sine-Wave Weighted Moving Average
----------------------------------------------------------------------------*/
SCSFExport scsf_SineWaveWMA(SCStudyInterfaceRef sc)
{
	SCSubgraphRef WMA = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName="Sine Wave Weighted Moving Average";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;
		

		WMA.Name="SWA";
		WMA.DrawStyle = DRAWSTYLE_LINE;
		WMA.LineWidth = 1;
		WMA.PrimaryColor = RGB(0,255,0);
		WMA.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		return;
	}

	//PI:=3.1415926;
	//SD:=180/6;
	//S1:=Sin(1*180/6)*C;
	//S2:=Sin(2*180/6)*Ref(C,-1);
	//S3:=Sin(3*180/6)*Ref(C,-2);
	//S4:=Sin(4*180/6)*Ref(C,-3);
	//S5:=Sin(5*180/6)*Ref(C,-4);
	//Num:=S1+S2+S3+S4+S5;
	//Den:=Sin(SD)+Sin(2*SD)+Sin(3*SD)+Sin(4*SD)+Sin(5*SD);
	//Num/Den

	sc.DataStartIndex = 5;
	if (sc.Index < 4)
		return;
	
	int InputDataIndex = InputData.GetInputDataIndex();

	const float SD = (float)(M_PI/6);  // Note: the sin function takes radians, not degrees
	float S1 = sin(1*SD) * sc.BaseData[InputDataIndex][sc.Index - 0];
	float S2 = sin(2*SD) * sc.BaseData[InputDataIndex][sc.Index - 1];
	float S3 = sin(3*SD) * sc.BaseData[InputDataIndex][sc.Index - 2];
	float S4 = sin(4*SD) * sc.BaseData[InputDataIndex][sc.Index - 3];
	float S5 = sin(5*SD) * sc.BaseData[InputDataIndex][sc.Index - 4];

	float Num = S1 + S2 + S3 + S4 + S5;
	float Den = sin(1*SD) + sin(2*SD) + sin(3*SD) + sin(4*SD) + sin(5*SD);

	WMA[sc.Index] = Num / Den;
}

/*==========================================================================*/
/*
Bollinger bands squeeze.

Proportion = (kUpper - kLower) / (bbUpper - bbLower);

*/
SCSFExport scsf_BollingerSqueeze(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BandsRatio = sc.Subgraph[0];
	SCSubgraphRef SqueezeIndicator = sc.Subgraph[1];
	SCSubgraphRef Keltner = sc.Subgraph[2];
	SCSubgraphRef BollingerBands = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef InternalMovAvgType = sc.Input[1];
	SCInputRef KeltnerBandsLength = sc.Input[2];
	SCInputRef KeltnerTrueRangeMALength = sc.Input[3];
	SCInputRef KeltnerBandsMultiplier = sc.Input[4];
	SCInputRef BollingerBandsLength = sc.Input[5];
	SCInputRef BollingerBandsMultiplier = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName="Bollinger Squeeze";
		sc.StudyDescription="Bollinger Squeeze";

		sc.AutoLoop = 1;
		sc.GraphRegion = 1;


		BandsRatio.Name = "Bands Ratio";
		BandsRatio.DrawStyle = DRAWSTYLE_BAR;
		BandsRatio.PrimaryColor = RGB(0,255,0);
		BandsRatio.SecondaryColor = RGB(255,0,0);
		BandsRatio.SecondaryColorUsed = true;
		BandsRatio.DrawZeros = true;

		SqueezeIndicator.Name = "Squeeze Indicator";
		SqueezeIndicator.DrawStyle = DRAWSTYLE_POINT;
		SqueezeIndicator.LineWidth = 3;
		SqueezeIndicator.PrimaryColor = RGB(0,255,0);
		SqueezeIndicator.SecondaryColor = RGB(255,0,0);
		SqueezeIndicator.SecondaryColorUsed = true;
		SqueezeIndicator.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		InternalMovAvgType.Name="Moving Average Type for Internal Calculations";
		InternalMovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		KeltnerBandsLength.Name="Keltner Bands Length";
		KeltnerBandsLength.SetInt(20);
		KeltnerBandsLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		KeltnerTrueRangeMALength.Name = "Keltner True Range MovAvg Length";
		KeltnerTrueRangeMALength.SetInt(20);
		KeltnerTrueRangeMALength.SetIntLimits(1,MAX_STUDY_LENGTH);

		KeltnerBandsMultiplier.Name="Keltner Bands Multiplier";
		KeltnerBandsMultiplier.SetFloat(2.0f);

		BollingerBandsLength.Name="Bollinger Bands Length";
		BollingerBandsLength.SetInt(20);
		BollingerBandsLength.SetIntLimits(1,MAX_STUDY_LENGTH);

		BollingerBandsMultiplier.Name="Bollinger Bands Multiplier";
		BollingerBandsMultiplier.SetFloat(2.0f);

		return;
	}

	sc.DataStartIndex = max(BollingerBandsLength.GetInt(), max(KeltnerBandsLength.GetInt(), KeltnerTrueRangeMALength.GetInt())) - 1;

	// calculate Bollinger Bands
	sc.BollingerBands(sc.BaseData[InputData.GetInputDataIndex()],BollingerBands,BollingerBandsLength.GetInt(),BollingerBandsMultiplier.GetFloat(),InternalMovAvgType.GetMovAvgType());

	// calculate Keltner
	sc.Keltner( sc.BaseData,sc.BaseData[InputData.GetInputDataIndex()],Keltner,KeltnerBandsLength.GetInt(), InternalMovAvgType.GetMovAvgType(),BollingerBandsLength.GetInt(),InternalMovAvgType.GetMovAvgType(), KeltnerBandsMultiplier.GetFloat(),KeltnerBandsMultiplier.GetFloat());


	float KUp = Keltner.Arrays[0][sc.CurrentIndex];
	float KDown = Keltner.Arrays[1][sc.CurrentIndex];

	float UBB = BollingerBands.Arrays[0][sc.CurrentIndex];
	float LBB = BollingerBands.Arrays[1][sc.CurrentIndex];

	if ((UBB > KUp) && (LBB < KDown)) 
		SqueezeIndicator.DataColor[sc.CurrentIndex] = SqueezeIndicator.PrimaryColor;    
	else
		SqueezeIndicator.DataColor[sc.CurrentIndex] = SqueezeIndicator.SecondaryColor;

	BandsRatio[sc.CurrentIndex] = (KUp-KDown)/(UBB - LBB) - 1.0f;

	if (BandsRatio[sc.CurrentIndex] >= 0)
		BandsRatio.DataColor[sc.CurrentIndex] = BandsRatio.PrimaryColor;
	else
		BandsRatio.DataColor[sc.CurrentIndex] = BandsRatio.SecondaryColor;
}

/*==========================================================================*/
SCSFExport scsf_StudySubgraphsDifference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Diff = sc.Subgraph[0];


	SCInputRef StudySugbraph1 = sc.Input[5];
	SCInputRef StudySugbraph2 = sc.Input[6];
	SCInputRef Study2SubgraphOffset = sc.Input[7];
	SCInputRef VersionUpdate = sc.Input[8];

	SCInputRef DrawZeros = sc.Input[9];
	SCInputRef PerformSubtractWithZeroValue = sc.Input[10];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Study Subgraphs Subtract/Difference";
		sc.StudyDescription = "Calculates the difference between the two selected study Subgraphs.";

		sc.AutoLoop = 0;//manual looping
		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		

		Diff.Name = "Diff";
		Diff.DrawStyle = DRAWSTYLE_LINE;
		Diff.PrimaryColor = RGB(0,255,0);
		Diff.LineWidth = 1;

		StudySugbraph1.Name = "Input Study 1";
		StudySugbraph1.SetStudySubgraphValues(0, 0);

		StudySugbraph2.Name = "Input Study 2";
		StudySugbraph2.SetStudySubgraphValues(0, 0);

		Study2SubgraphOffset.Name = "Study 2 Subgraph Offset";// This is always converted to a positive value and is the number of bars back
		Study2SubgraphOffset.SetInt(0);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		VersionUpdate.SetInt(2);

		PerformSubtractWithZeroValue.Name = "Perform Subtract With Zero Value";
		PerformSubtractWithZeroValue.SetYesNo(true);

		return;
	}


	if (VersionUpdate.GetInt() < 2)
	{
		PerformSubtractWithZeroValue.SetYesNo(true);
		VersionUpdate.SetInt(2);
	}

	if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
		sc.GraphName = "Study Subgraphs Difference";

	Diff.DrawZeros = DrawZeros.GetYesNo();

	SCFloatArray Study1Array;
	sc.GetStudyArrayUsingID(StudySugbraph1.GetStudyID(), StudySugbraph1.GetSubgraphIndex(), Study1Array);

	SCFloatArray Study2Array;
	sc.GetStudyArrayUsingID(StudySugbraph2.GetStudyID(), StudySugbraph2.GetSubgraphIndex(), Study2Array);

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{
		float Value1 = Study1Array[Index];
		float Value2 = Study2Array[Index - labs(Study2SubgraphOffset.GetInt())];

		if (PerformSubtractWithZeroValue.GetYesNo()
			|| (Value1 != 0.0 && Value2 != 0.0))
		{
			Diff[Index] = Value1 - Value2;
		}
		else
		{
			Diff[Index] = 0.0;
		}
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;


	
}

/*==========================================================================*/
SCSFExport scsf_LRS(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LRS = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Linear Regressive Slope";
		
		sc.AutoLoop = 1;
		

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		LRS.Name = "LRS";
		LRS.DrawStyle = DRAWSTYLE_LINE;
		LRS.PrimaryColor = RGB(0,255,0);
		LRS.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	int Len=Length.GetInt();
	double SumBars = Len*(Len-1)/2.0;
	double SumSqrBars = (Len-1)*Len*(2*Len-1)/6.0;

	double Sum1       = 0;
	double SumY=0;
	for(int Index=sc.Index; Index > (sc.Index - Len); Index--)
	{
		int x =sc.Index-Index;
		Sum1= Sum1 + x*sc.BaseData[InputData.GetInputDataIndex()][Index];
		SumY += sc.BaseData[InputData.GetInputDataIndex()][Index];
	}

	double Sum2 = SumBars * SumY;
	double Num1 = Len*Sum1 - Sum2;
	double Num2 = SumBars*SumBars - Len*SumSqrBars;
	LRS[sc.Index] = (float)(Num1 / Num2);


	return;
}

/*==========================================================================*/
SCSFExport scsf_SummationPeriodic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Summation - Periodic";
		sc.StudyDescription = "The cumulative sum over the number of elements specified by the Length input.";


		sc.AutoLoop = 1;
		

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Sum.Name = "Sum";
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.PrimaryColor = RGB(0,255,0);
		Sum.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.Summation(sc.BaseData[InputData.GetInputDataIndex()], Sum, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_SummationOfStudySubgraphPeriodic(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];
	SCInputRef StudySubgraphReference = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Summation of Study Subgraph - Periodic";

		sc.AutoLoop = 1;
		

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Sum.Name = "Sum";
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.PrimaryColor = RGB(0, 255, 0);
		Sum.DrawZeros = false;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		StudySubgraphReference.Name = "Study and Subgraph To Reference";
		StudySubgraphReference.SetStudySubgraphValues(1,0);

		return;
	}

	SCFloatArray StudyArray;
	sc.GetStudyArrayUsingID(StudySubgraphReference.GetStudyID(), StudySubgraphReference.GetSubgraphIndex(), StudyArray);

	if(StudyArray.GetArraySize() == 0)
	{
		SCString Message;
		Message = sc.GraphName;
		Message += " Study being referenced does not exist.";
		sc.AddMessageToLog(Message, 1);
		return;
	}

	sc.DataStartIndex = sc.GetStudyDataStartIndexUsingID(StudySubgraphReference.GetStudyID());

	sc.Summation(StudyArray, Sum, Length.GetInt());
}
/***********************************************************************************/
SCSFExport scsf_PeriodOHLCVariablePeriod(SCStudyInterfaceRef sc)
{
	SCInputRef TimePeriodType = sc.Input[0];
	SCInputRef TimePeriodLength = sc.Input[1];
	SCInputRef UseCurrentPeriod = sc.Input[2];
	SCInputRef MinimumRequiredTP = sc.Input[3];
	SCInputRef NumberOfForwardBarsToProject = sc.Input[4];
	SCInputRef DisplayDebuggingOutput = sc.Input[5];
	SCInputRef ForwardProjectLines = sc.Input[6];
	SCInputRef NumberOfDaysToCalculate = sc.Input[7];
	SCInputRef AutoSkipPeriodOfNoTrading = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Period OHLC-Variable Period";
		sc.AutoLoop = 0;

		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.Subgraph[SC_OPEN].Name = "Open";
		sc.Subgraph[SC_OPEN].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_OPEN].PrimaryColor = RGB(0,255,0);
		sc.Subgraph[SC_OPEN].LineWidth = 2;
		sc.Subgraph[SC_OPEN].DrawZeros = false;

		sc.Subgraph[SC_HIGH].Name = "High";
		sc.Subgraph[SC_HIGH].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_HIGH].PrimaryColor = RGB(255,0,255);
		sc.Subgraph[SC_HIGH].LineWidth = 2;
		sc.Subgraph[SC_HIGH].DrawZeros = false;

		sc.Subgraph[SC_LOW].Name = "Low";
		sc.Subgraph[SC_LOW].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LOW].PrimaryColor = RGB(255,255,0);
		sc.Subgraph[SC_LOW].LineWidth = 2;
		sc.Subgraph[SC_LOW].DrawZeros = false;

		sc.Subgraph[SC_LAST].Name = "Close";
		sc.Subgraph[SC_LAST].DrawStyle = DRAWSTYLE_DASH;
		sc.Subgraph[SC_LAST].PrimaryColor = RGB(255,127,0);
		sc.Subgraph[SC_LAST].LineWidth = 2;
		sc.Subgraph[SC_LAST].DrawZeros = false;

		sc.Subgraph[SC_OHLC_AVG].Name = "OHLC Avg";
		sc.Subgraph[SC_OHLC_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_OHLC_AVG].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_OHLC_AVG].DrawZeros = false;

		sc.Subgraph[SC_HLC_AVG] .Name= "HLC Avg";
		sc.Subgraph[SC_HLC_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HLC_AVG].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_HLC_AVG].DrawZeros = false;

		sc.Subgraph[SC_HL_AVG] .Name= "HL Avg";
		sc.Subgraph[SC_HL_AVG].DrawStyle = DRAWSTYLE_IGNORE;
		sc.Subgraph[SC_HL_AVG].PrimaryColor = RGB(128,128,128);
		sc.Subgraph[SC_HL_AVG].DrawZeros = false;

		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_HL_AVG; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex].LineLabel = 
				LL_DISPLAY_NAME  | LL_NAME_ALIGN_CENTER | LL_NAME_ALIGN_FAR_RIGHT | 
				LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		unsigned short DisplayOrder = 1;

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_MINUTES);
		TimePeriodType.DisplayOrder = DisplayOrder++;

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(60);
		TimePeriodLength.SetIntLimits(1, 7*MINUTES_PER_DAY);
		TimePeriodLength.DisplayOrder = DisplayOrder++;

		UseCurrentPeriod.Name = "Use Current Period";
		UseCurrentPeriod.SetYesNo(false);
		UseCurrentPeriod.DisplayOrder = DisplayOrder++;
		
		AutoSkipPeriodOfNoTrading.Name = "Auto Skip Period of No Trading";
		AutoSkipPeriodOfNoTrading.SetYesNo(false);
		AutoSkipPeriodOfNoTrading.DisplayOrder = DisplayOrder++;

		MinimumRequiredTP.Name = "Minimum Required Time Period as %";
		MinimumRequiredTP.SetFloat(5.0f);
		MinimumRequiredTP.SetFloatLimits(1.0f, 100.0f);
		MinimumRequiredTP.DisplayOrder = DisplayOrder++;

		ForwardProjectLines.Name = "Forward Project OHLC Lines";
		ForwardProjectLines.SetYesNo(0);
		ForwardProjectLines.DisplayOrder = DisplayOrder++;

		NumberOfForwardBarsToProject.Name = "Number of Forward Bars to Project";
		NumberOfForwardBarsToProject.SetInt(10);
		NumberOfForwardBarsToProject.DisplayOrder = DisplayOrder++;

		NumberOfDaysToCalculate.Name = "Number of Days to Calculate";
		NumberOfDaysToCalculate.SetInt(250);
		NumberOfDaysToCalculate.SetIntLimits(1, MAX_STUDY_LENGTH);
		NumberOfDaysToCalculate.DisplayOrder = DisplayOrder++;

		DisplayDebuggingOutput.Name = "Display Debugging Output (slows study calculations)";
		DisplayDebuggingOutput.SetYesNo(0);
		DisplayDebuggingOutput.DisplayOrder = DisplayOrder++;


		return;
	}


	int PeriodLength = TimePeriodLength.GetInt();
	SCDateTime FirstBarDateTime = sc.BaseDateTimeIn[0];

	SCDateTime ChartStartTime = sc.StartTime1;
	if (sc.UseSecondStartEndTimes)
		ChartStartTime = sc.StartTime2;


	if (NumberOfForwardBarsToProject.GetInt() == 0)
		NumberOfForwardBarsToProject.SetInt(10);

	int NumberOfForwardBars = 0;

	if (ForwardProjectLines.GetYesNo())
	{
		NumberOfForwardBars = NumberOfForwardBarsToProject.GetInt();

		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_HL_AVG; SubgraphIndex++)
			sc.Subgraph[SubgraphIndex].ExtendedArrayElementsToGraph = NumberOfForwardBars;
	}
	else
	{
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_HL_AVG; SubgraphIndex++)
			sc.Subgraph[SubgraphIndex].ExtendedArrayElementsToGraph = 0;
	}

	if (NumberOfDaysToCalculate.GetInt() < 1)
		NumberOfDaysToCalculate.SetInt(250);

	SCDateTime LastDateInChart = sc.GetTradingDayDate(sc.BaseDateTimeIn[sc.ArraySize - 1]);
	SCDateTime FirstDateToCalculate = LastDateInChart;
	FirstDateToCalculate.SubtractDays(NumberOfDaysToCalculate.GetInt() - 1);

	float Open = 0, High = 0, Low = 0, Close = 0, NextOpen = 0;

	SCDateTime BeginOfRefDateTime,EndOfRefDateTime;
	SCDateTime CurrentPeriodBeginDateTime;
	SCDateTime CurrentPeriodEndDateTime;

	SCDateTime PriorCurrentPeriodStartDateTime;

	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize+NumberOfForwardBars; BarIndex++ )
	{
		SCDateTime CurrentBarDateTime = sc.BaseDateTimeIn[BarIndex];

		if (sc.GetTradingDayDate(CurrentBarDateTime) < FirstDateToCalculate)
			continue;

		bool GetReferenceData = true;

		CurrentPeriodBeginDateTime = sc.GetStartOfPeriodForDateTime(CurrentBarDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, 0);

		if(	CurrentPeriodBeginDateTime == PriorCurrentPeriodStartDateTime)
			GetReferenceData  = false;


		PriorCurrentPeriodStartDateTime =	CurrentPeriodBeginDateTime;

		SCDateTime TimeIncrement = sc.TimePeriodSpan(TimePeriodType.GetTimePeriodType(),  TimePeriodLength.GetInt());

		//If there are insufficient bars for a reference period, then we will walk back 1 block/period at a time. 

		if(GetReferenceData)
		{
			BeginOfRefDateTime= sc.GetStartOfPeriodForDateTime(CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(), PeriodLength, -1);

			EndOfRefDateTime = CurrentPeriodBeginDateTime - (1*SECONDS);

			CurrentPeriodEndDateTime=sc.GetStartOfPeriodForDateTime(CurrentPeriodBeginDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, 1) - (1*SECONDS);

			if (DisplayDebuggingOutput.GetYesNo() != 0)
			{
				SCString Message;

				Message.Format("Current Bar: %s, BeginOfRefDateTime: %s, EndOfRefDateTime: %s, CurrentPeriodBeginDateTime: %s, CurrentPeriodEndDateTime: %s",
					sc.FormatDateTime(CurrentBarDateTime).GetChars(),sc.FormatDateTime(BeginOfRefDateTime).GetChars(),
					sc.FormatDateTime(EndOfRefDateTime).GetChars(),sc.FormatDateTime(CurrentPeriodBeginDateTime).GetChars(),
					sc.FormatDateTime(CurrentPeriodEndDateTime).GetChars());

				sc.AddMessageToLog(Message,0);

			}
			int MaxPeriodsToGoBack = 1;

			if(AutoSkipPeriodOfNoTrading.GetYesNo())
				MaxPeriodsToGoBack = 32;

			for (int WalkBack = 0; WalkBack < MaxPeriodsToGoBack; WalkBack++)
			{
				if (!UseCurrentPeriod.GetYesNo() && WalkBack >= 1) //Walk back 1 period.
				{
					SCDateTime  PriorBeginOfRefDateTime = BeginOfRefDateTime;

					BeginOfRefDateTime = sc.GetStartOfPeriodForDateTime(BeginOfRefDateTime, TimePeriodType.GetTimePeriodType(),  PeriodLength, -1);
					EndOfRefDateTime =  PriorBeginOfRefDateTime- (1*SECONDS);

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
				int Result = 0;

				if (UseCurrentPeriod.GetYesNo())
					Result = sc.GetOHLCOfTimePeriod(CurrentPeriodBeginDateTime, CurrentPeriodEndDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);
				else
					Result = sc.GetOHLCOfTimePeriod(BeginOfRefDateTime, EndOfRefDateTime, Open, High, Low, Close, NextOpen, NumberOfBars, TotalTimeSpan);



				if (DisplayDebuggingOutput.GetYesNo() != 0)
				{
					SCString Message;
					Message.Format("Number of Bars: %d, Total Time Span In Minutes: %d",NumberOfBars,(int)(TotalTimeSpan/MINUTES+0.5));
					sc.AddMessageToLog(Message,0);

					Message.Format("RefOpen %f,RefHigh %f,RefLow %f,RefClose %f,RefNextOpen %f.",Open, High, Low, Close, NextOpen);
					sc.AddMessageToLog(Message,0);
				}

				if (UseCurrentPeriod.GetYesNo())
					break;

				if (!Result)
					continue;

				SCDateTime MinimumTimeSpan = (TimeIncrement * MinimumRequiredTP.GetFloat()/100.0f);
				if (TotalTimeSpan >= MinimumTimeSpan)
					break;
			}
		}

		if (!sc.IsFullRecalculation && UseCurrentPeriod.GetYesNo())
		{
			const int StartingBackIndex
				= sc.GetContainingIndexForSCDateTime(sc.ChartNumber, CurrentPeriodBeginDateTime);

			for (int PriorBarIndex = StartingBackIndex; PriorBarIndex <= BarIndex; ++PriorBarIndex)
			{
				sc.Subgraph[SC_OPEN][PriorBarIndex] = Open;
				sc.Subgraph[SC_HIGH][PriorBarIndex] = High;
				sc.Subgraph[SC_LOW][PriorBarIndex] = Low;
				sc.Subgraph[SC_LAST][PriorBarIndex] = Close;
				sc.CalculateOHLCAverages(PriorBarIndex);
			}
		}
		else
		{
			sc.Subgraph[SC_OPEN][BarIndex] = Open;
			sc.Subgraph[SC_HIGH][BarIndex] = High;
			sc.Subgraph[SC_LOW][BarIndex] = Low;
			sc.Subgraph[SC_LAST][BarIndex] = Close;
			sc.CalculateOHLCAverages(BarIndex);
		}
	} //for
}


/*==========================================================================*/

SCSFExport scsf_RenkoChart(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Open = sc.Subgraph[0];
	SCSubgraphRef High = sc.Subgraph[1];
	SCSubgraphRef Low = sc.Subgraph[2];
	SCSubgraphRef Last = sc.Subgraph[3];
	SCSubgraphRef Volume = sc.Subgraph[4];
	SCSubgraphRef NumTrades = sc.Subgraph[5];
	SCSubgraphRef OHLCAvg = sc.Subgraph[6];
	SCSubgraphRef HLCAvg = sc.Subgraph[7];
	SCSubgraphRef HLAvg = sc.Subgraph[8];
	SCSubgraphRef BidVol = sc.Subgraph[9];
	SCSubgraphRef AskVol = sc.Subgraph[10];
	SCSubgraphRef RenkoUpDownTrend = sc.Subgraph[11];
	SCSubgraphRef RenkoLast = sc.Subgraph[12];

	SCInputRef RenkoBoxSize = sc.Input[0];
	SCInputRef UseHighLowInsteadOfLast = sc.Input[1];
	SCInputRef IgnoreLatestBarUntilClose = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Renko Chart (unsupported)";

		sc.GraphRegion = 0;
		sc.StandardChartHeader = 1;
		sc.IsCustomChart = 1;
		sc.GraphDrawType = GDT_RENKO_BRICK;


		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0, 255, 0);
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0, 127, 0);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255, 0, 0);
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(127, 0, 0);
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

		RenkoBoxSize.Name = "Renko Box Size";
		RenkoBoxSize.SetFloat(0.0f);
		RenkoBoxSize.SetFloatLimits(0.0f,(float)MAX_STUDY_LENGTH);

		UseHighLowInsteadOfLast.Name = "Use High/Low Values instead of Last Value";
		UseHighLowInsteadOfLast.SetYesNo(0);

		IgnoreLatestBarUntilClose.Name = "Ignore Latest Underlying Bar until Close";
		IgnoreLatestBarUntilClose.SetYesNo(0);

		return;
	}



	sc.ValueFormat = sc.BaseGraphValueFormat;
	int ValueFormat = sc.BaseGraphValueFormat;

	if (RenkoBoxSize.GetFloat() == 0.0f)
	{
		RenkoBoxSize.SetFloat(sc.TickSize*4);
	}

	sc.GraphName.Format("%s Renko - BoxSize: %s", 
		sc.GetStudyName(0).GetChars(), 
		sc.FormatGraphValue(RenkoBoxSize.GetFloat(), ValueFormat).GetChars());

	int BaseGraphIndex = sc.UpdateStartIndex;

	if (IgnoreLatestBarUntilClose.GetYesNo() != false && (sc.UpdateStartIndex == sc.ArraySize - 1))
		return; // No need to process last underlying bar.  However, during a full recalculation, the last bar will be processed. This is OK.

	float UpperBox = 0;
	float LowerBox = 0;
	float CumulativeVolume = 0;
	float CumulativeOIOrNT = 0;
	float CumulativeAskVolume = 0;
	float CumulativeBidVolume = 0;

	if (BaseGraphIndex == 0)
	{

		sc.ResizeArrays(0);

		if (!sc.AddElements(1))
			return;

		if (sc.ArraySize < 1)
			return; // nothing to display

		UpperBox = sc.Close[BaseGraphIndex];
		LowerBox = sc.Close[BaseGraphIndex];

		sc.DateTimeOut[0] = sc.BaseDateTimeIn[0];

		RenkoLast[0] = sc.Close[BaseGraphIndex];
		Open[0] = sc.Close[BaseGraphIndex];
		Last[0] = sc.Close[BaseGraphIndex];
		High[0] = sc.Close[BaseGraphIndex];
		Low[0] = sc.Close[BaseGraphIndex];
	}
	else
	{
		int RenkoIndex = sc.OutArraySize - 1;

		// When new data is updated within the same bar
		if (sc.DateTimeOut[RenkoIndex] == sc.BaseDateTimeIn[BaseGraphIndex])
		{
			// Decrement BaseGraphIndex to the first matching date time
			for (; BaseGraphIndex >= 0; BaseGraphIndex--)
			{
				if (sc.DateTimeOut[RenkoIndex] != sc.BaseDateTimeIn[BaseGraphIndex])
				{
					BaseGraphIndex++;
					break;
				}
			}

			int LastIndex = RenkoIndex;
			// Delete the Renko Elements with that date time
			for (; RenkoIndex >= 0; RenkoIndex--)
			{
				if (sc.DateTimeOut[LastIndex] != sc.DateTimeOut[RenkoIndex])
				{
					RenkoIndex++;
					break;
				}
			}
			sc.ResizeArrays(RenkoIndex);
			RenkoIndex = sc.OutArraySize - 1;
		}

		if (RenkoUpDownTrend[RenkoIndex] > 0) // Uptrend Box
		{
			UpperBox = RenkoLast[RenkoIndex];
			LowerBox = UpperBox - RenkoBoxSize.GetFloat();
			LowerBox = (float)sc.RoundToTickSize(LowerBox, RenkoBoxSize.GetFloat());
		}
		else
		{
			LowerBox = RenkoLast[RenkoIndex];
			UpperBox = LowerBox + RenkoBoxSize.GetFloat();
			UpperBox = (float)sc.RoundToTickSize(UpperBox, RenkoBoxSize.GetFloat());
		}

		// Calculate Cumulative Volume/OpenInterest/Number Of Trades for current index
		// Must calculate from Base Graph since current index could 
		// potentially have grown in volume since last calculation.

		int i = BaseGraphIndex;
		for (; i >= 0; i--)
		{
			if (sc.BaseDateTimeIn[i] <= sc.DateTimeOut[RenkoIndex])
				break;
		}

		for (int n = i; n < BaseGraphIndex; n++)
		{
			CumulativeVolume += sc.Volume[n];
			CumulativeOIOrNT += sc.OpenInterest[n];
			CumulativeAskVolume += sc.AskVolume[n];
			CumulativeBidVolume += sc.BidVolume[n];
		}

	}

	int OriginalRenkoIndex = sc.OutArraySize - 1;
	int BaseGraphEnd = sc.ArraySize;
	if (IgnoreLatestBarUntilClose.GetYesNo() != 0)
		BaseGraphEnd--;

	for (;BaseGraphIndex < BaseGraphEnd; BaseGraphIndex++)
	{
		CumulativeVolume += sc.Volume[BaseGraphIndex];
		CumulativeOIOrNT += sc.OpenInterest[BaseGraphIndex];
		CumulativeAskVolume += sc.AskVolume[BaseGraphIndex];
		CumulativeBidVolume += sc.BidVolume[BaseGraphIndex];

		int NewEntry = 0;
		int UpperOrLowerTrend = 0;
		float HigherPrice = sc.Close[BaseGraphIndex];
		float LowerPrice = sc.Close[BaseGraphIndex];

		if (UseHighLowInsteadOfLast.GetYesNo() != 0)
		{
			HigherPrice = sc.High[BaseGraphIndex];
			LowerPrice = sc.Low[BaseGraphIndex];
		}

		if (sc.FormattedEvaluateUsingDoubles((double)HigherPrice, ValueFormat, GREATER_EQUAL_OPERATOR, (double)(UpperBox + RenkoBoxSize.GetFloat()), ValueFormat))
		{
			NewEntry = 1;
			UpperOrLowerTrend = 1;
		}
		else if (sc.FormattedEvaluateUsingDoubles((double)LowerPrice, ValueFormat, LESS_EQUAL_OPERATOR, (double)(LowerBox - RenkoBoxSize.GetFloat()), ValueFormat))
		{
			NewEntry = 1;
			UpperOrLowerTrend = -1;
		}

		int NewEntryCounter = 0;

		while (NewEntry != 0)
		{
			NewEntryCounter++;
			if (NewEntryCounter >= 500)
			{
				sc.AddMessageToLog("Renko Chart study: Renko error due to either data error or Box Size too small. Stopping processing and skipping until real time updates. Try increasing the Box Size input.", 1);
				return;
			}

			sc.CalculateOHLCAverages(sc.OutArraySize - 1);

			NewEntry = 0;
			sc.AddElements(1);
			int NewBoxIndex = sc.OutArraySize - 1;
			sc.DateTimeOut[NewBoxIndex] = sc.BaseDateTimeIn[BaseGraphIndex];

			Volume[NewBoxIndex-1] = CumulativeVolume - sc.Volume[BaseGraphIndex];
			NumTrades[NewBoxIndex-1] = CumulativeOIOrNT - sc.OpenInterest[BaseGraphIndex];
			AskVol[NewBoxIndex-1] = CumulativeAskVolume - sc.AskVolume[BaseGraphIndex];
			BidVol[NewBoxIndex-1] = CumulativeBidVolume - sc.BidVolume[BaseGraphIndex];

			CumulativeVolume = sc.Volume[BaseGraphIndex];
			CumulativeOIOrNT = sc.OpenInterest[BaseGraphIndex];
			CumulativeAskVolume = sc.AskVolume[BaseGraphIndex];
			CumulativeBidVolume = sc.BidVolume[BaseGraphIndex];

			float NewLast = 0;
			float NewOtherBound = 0;

			if (UpperOrLowerTrend == 1) // Upper Trend
			{
				LowerBox = UpperBox;
				UpperBox += RenkoBoxSize.GetFloat();
				UpperBox = (float)sc.RoundToTickSize(UpperBox, RenkoBoxSize.GetFloat());

				NewLast = UpperBox;
				NewOtherBound = LowerBox;

				if (sc.FormattedEvaluateUsingDoubles((double)HigherPrice, ValueFormat, GREATER_EQUAL_OPERATOR, (double)(UpperBox + RenkoBoxSize.GetFloat()), ValueFormat))
					NewEntry = 1;
			}
			else
			{
				UpperBox = LowerBox;
				LowerBox -= RenkoBoxSize.GetFloat();
				LowerBox = (float)sc.RoundToTickSize(LowerBox, RenkoBoxSize.GetFloat());

				NewLast = LowerBox;
				NewOtherBound = UpperBox;

				if (sc.FormattedEvaluateUsingDoubles((double)LowerPrice, ValueFormat, LESS_EQUAL_OPERATOR, (double)(LowerBox - RenkoBoxSize.GetFloat()), ValueFormat))
					NewEntry = 1;
			}

			Low[NewBoxIndex] = LowerBox;
			High[NewBoxIndex] = UpperBox;
			RenkoLast[NewBoxIndex] = NewLast;
			RenkoUpDownTrend[NewBoxIndex] = (float)UpperOrLowerTrend;
			if (UpperOrLowerTrend == 1)
			{
				Open[NewBoxIndex] = LowerBox;
				Last[NewBoxIndex] = UpperBox;
			}
			else
			{
				Open[NewBoxIndex] = UpperBox;
				Last[NewBoxIndex] = LowerBox;
			}
		}

		sc.CalculateOHLCAverages(sc.OutArraySize - 1);
	}

	int RenkoIndex = sc.OutArraySize - 1;
	if (RenkoIndex >= 0)
	{
		Volume[RenkoIndex] = CumulativeVolume;
		NumTrades[RenkoIndex] = CumulativeOIOrNT;
		AskVol[RenkoIndex] = CumulativeAskVolume;
		BidVol[RenkoIndex] = CumulativeBidVolume;
	}
}

/*==========================================================================*/
SCSFExport scsf_MillisecondsExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Milliseconds Example";

		sc.AutoLoop = 1;

		

		return;
	}
	//When working with milliseconds it is necessary to define variables of type SCDateTimeMS
	SCDateTimeMS DateTimeMS , DateTimeMSRefChart;

	// Get the milliseconds for the current bar.  This will be 0 if the chart bars are based upon a specific time by using the 'Days-Minutes-Seconds' Bar Period Type since the start time for a bar is always evenly aligned to a second based upon the Session Start Time and the time period of the bar. Otherwise, the milliseconds can be obtained for other Bar Period Types. 
	DateTimeMS = sc.BaseDateTimeIn[sc.Index];
	int Millisecond = DateTimeMS.GetMillisecond();


	//Get the Date-Times from another chart.
	SCDateTimeArray RefChartDateTime;
	sc.GetChartDateTimeArray(2, RefChartDateTime );

	if(RefChartDateTime.GetArraySize() != 0)
	{
		//Get milliseconds for last bar in the other chart being referenced
		DateTimeMSRefChart = RefChartDateTime[RefChartDateTime.GetArraySize() - 1];
		int Millisecond = DateTimeMSRefChart.GetMillisecond();

	}


	//Compare the Date-Times from the 2 charts with millisecond precision
	if(DateTimeMS ==DateTimeMSRefChart)
	{
		//There is a match down to the millisecond
	}

}

/*==========================================================================*/
SCSFExport scsf_RelativeVigorIndex1(SCStudyInterfaceRef sc)
{

	SCSubgraphRef Subgraph_RVI = sc.Subgraph[0];
	SCSubgraphRef Subgraph_SmoothedRVI = sc.Subgraph[1];
	SCSubgraphRef Subgraph_Signal = sc.Subgraph[2];
	SCSubgraphRef Subgraph_Buy = sc.Subgraph[3];
	SCSubgraphRef Subgraph_Sell = sc.Subgraph[4];

	SCInputRef   Input_SmoothedRVILength = sc.Input[0];
	SCInputRef   Input_SmoothedRVIAvgType = sc.Input[1];
	SCInputRef   Input_SignalLength = sc.Input[2];
	SCInputRef   Input_SignalAvgType = sc.Input[3];
	SCInputRef	 Input_OffsetPercentInput = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Relative Vigor Index 1";

		sc.AutoLoop = 1;

		

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Subgraph_RVI.Name = "RVI";
		Subgraph_RVI.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_RVI.PrimaryColor = RGB(0, 0, 255);
		Subgraph_RVI.DrawZeros = true;

		Subgraph_SmoothedRVI.Name = "Smoothed RVI Line";
		Subgraph_SmoothedRVI.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_SmoothedRVI.PrimaryColor = RGB(0, 255, 0);
		Subgraph_SmoothedRVI.DrawZeros = true;

		Subgraph_Signal.Name = "Signal Line";
		Subgraph_Signal.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Signal.PrimaryColor = RGB(255, 0, 0);
		Subgraph_Signal.DrawZeros = true;

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

		Input_SmoothedRVILength.Name = "Smoothed RVI Length";
		Input_SmoothedRVILength.SetInt(10);
		Input_SmoothedRVILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_SmoothedRVIAvgType.Name = "Smoothed RVI Average Type";
		Input_SmoothedRVIAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Input_SignalLength.Name = "Signal Length";
		Input_SignalLength.SetInt(4);
		Input_SignalLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_SignalAvgType.Name = "Signal Average Type";
		Input_SignalAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Input_OffsetPercentInput.Name = "Arrow Offset Percentage";
		Input_OffsetPercentInput.SetFloat(8);

		return;
	}

	sc.DataStartIndex = max(Input_SmoothedRVILength.GetInt(), Input_SignalLength.GetInt()) - 1;

	float BarRange = (sc.High[sc.Index] - sc.Low[sc.Index]);
	if (BarRange != 0)
		Subgraph_RVI[sc.Index] = (sc.Close[sc.Index] - sc.Open[sc.Index]) / BarRange;
	else
		Subgraph_RVI[sc.Index] = Subgraph_RVI.Arrays[0][sc.Index - 1];

	sc.MovingAverage(Subgraph_RVI, Subgraph_SmoothedRVI, Input_SmoothedRVIAvgType.GetMovAvgType(), Input_SmoothedRVILength.GetInt());
	sc.MovingAverage(Subgraph_RVI, Subgraph_Signal, Input_SignalAvgType.GetMovAvgType(), Input_SignalLength.GetInt());

	float OffsetPercent = Input_OffsetPercentInput.GetFloat() * 0.01f;

	if (sc.CrossOver(Subgraph_SmoothedRVI, Subgraph_Signal) == CROSS_FROM_BOTTOM)
	{

		Subgraph_Buy[sc.Index] = Subgraph_Signal[sc.Index] - OffsetPercent * Subgraph_Signal[sc.Index];

		Subgraph_Sell[sc.Index] = 0;
	}
	else if (sc.CrossOver(Subgraph_SmoothedRVI, Subgraph_Signal) == CROSS_FROM_TOP)
	{
		Subgraph_Sell[sc.Index] = Subgraph_Signal[sc.Index] + OffsetPercent * Subgraph_Signal[sc.Index];
		Subgraph_Buy[sc.Index] = 0;
	}
	else
	{
		Subgraph_Buy[sc.Index] = 0;
		Subgraph_Sell[sc.Index] = 0;
	}
}
/*==========================================================================*/

SCSFExport scsf_RelativeVigorIndex2(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_RVI = sc.Subgraph[0];
	SCSubgraphRef Subgraph_SmoothedRVI = sc.Subgraph[1];
	SCSubgraphRef Subgraph_Signal = sc.Subgraph[2];
	SCSubgraphRef Subgraph_Buy = sc.Subgraph[3];
	SCSubgraphRef Subgraph_Sell = sc.Subgraph[4];

	SCFloatArrayRef Array_CloseOpenAvg = Subgraph_RVI.Arrays[0];
	SCFloatArrayRef Array_HighLowAvg = Subgraph_RVI.Arrays[1];
	SCFloatArrayRef Array_SmoothedRVINumerator = Subgraph_RVI.Arrays[2];
	SCFloatArrayRef Array_SmoothedRVIDenominator = Subgraph_RVI.Arrays[3];

	SCInputRef   Input_SmoothedRVILength = sc.Input[0];
	SCInputRef   Input_SmoothedRVIAvgType = sc.Input[1];
	SCInputRef   Input_OffsetPercentInput = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Relative Vigor Index 2";

		sc.AutoLoop = 1;

		

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Subgraph_RVI.Name = "RVI";
		Subgraph_RVI.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_RVI.PrimaryColor = RGB(0, 0, 255);
		Subgraph_RVI.DrawZeros = true;

		Subgraph_SmoothedRVI.Name = "Smoothed RVI Line";
		Subgraph_SmoothedRVI.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_SmoothedRVI.PrimaryColor = RGB(0, 255, 0);
		Subgraph_SmoothedRVI.DrawZeros = true;

		Subgraph_Signal.Name = "Signal Line";
		Subgraph_Signal.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Signal.PrimaryColor = RGB(255, 0, 0);
		Subgraph_Signal.DrawZeros = true;

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

		Input_SmoothedRVILength.Name = "Smoothed RVI Length";
		Input_SmoothedRVILength.SetInt(10);
		Input_SmoothedRVILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_SmoothedRVIAvgType.Name = "Smoothed RVI Average Type";
		Input_SmoothedRVIAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Input_OffsetPercentInput.Name = "Arrow Offset Percentage";
		Input_OffsetPercentInput.SetFloat(8);

		return;
	}

	sc.DataStartIndex = Input_SmoothedRVILength.GetInt() + 5;

	Array_CloseOpenAvg[sc.Index] = ((sc.Close[sc.Index - 3] - sc.Open[sc.Index - 3]) + 2 * (sc.Close[sc.Index - 2] - sc.Open[sc.Index - 2]) + 2 * (sc.Close[sc.Index - 1] - sc.Open[sc.Index - 1]) + (sc.Close[sc.Index] - sc.Open[sc.Index])) / 6;


	Array_HighLowAvg[sc.Index] = ((sc.High[sc.Index - 3] - sc.Low[sc.Index - 3]) + 2 * (sc.High[sc.Index - 2] - sc.Low[sc.Index - 2]) + 2 * (sc.High[sc.Index - 1] - sc.Low[sc.Index - 1]) + (sc.High[sc.Index] - sc.Low[sc.Index])) / 6;

	float BarRange = (sc.High[sc.Index] - sc.Low[sc.Index]);
	if (BarRange != 0)
		Subgraph_RVI[sc.Index] = (sc.Close[sc.Index] - sc.Open[sc.Index]) / BarRange;
	else
		Subgraph_RVI[sc.Index] = Array_CloseOpenAvg[sc.Index - 1];

	
	sc.MovingAverage(Array_CloseOpenAvg, Array_SmoothedRVINumerator, Input_SmoothedRVIAvgType.GetMovAvgType(), Input_SmoothedRVILength.GetInt());


	sc.MovingAverage(Array_HighLowAvg, Array_SmoothedRVIDenominator, Input_SmoothedRVIAvgType.GetMovAvgType(), Input_SmoothedRVILength.GetInt());

	Subgraph_SmoothedRVI[sc.Index] = Array_SmoothedRVINumerator[sc.Index] / Array_SmoothedRVIDenominator[sc.Index];

	Subgraph_Signal[sc.Index] = (Subgraph_SmoothedRVI[sc.Index - 3] + 2 * Subgraph_SmoothedRVI[sc.Index - 2] + 2 * Subgraph_SmoothedRVI[sc.Index - 1] + Subgraph_SmoothedRVI[sc.Index]) / 6;

	float OffsetPercent = Input_OffsetPercentInput.GetFloat() * 0.01f;

	if (sc.CrossOver(Subgraph_SmoothedRVI, Subgraph_Signal) == CROSS_FROM_BOTTOM)
	{

		Subgraph_Buy[sc.Index] = Subgraph_Signal[sc.Index] - OffsetPercent * Subgraph_Signal[sc.Index];

		Subgraph_Sell[sc.Index] = 0;
	}
	else if (sc.CrossOver(Subgraph_SmoothedRVI, Subgraph_Signal) == CROSS_FROM_TOP)
	{
		Subgraph_Sell[sc.Index] = Subgraph_Signal[sc.Index] + OffsetPercent * Subgraph_Signal[sc.Index];
		Subgraph_Buy[sc.Index] = 0;
	}
	else
	{
		Subgraph_Buy[sc.Index] = 0;
		Subgraph_Sell[sc.Index] = 0;
	}
}
/*============================================================================*/
SCSFExport scsf_ImmediateCallExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Immediate Call Example";
		sc.AutoLoop = 1;

		//Increases CPU load. Not recommended. Use only for special purposes.
		sc.Unused_OnExternalDataImmediateStudyCall= true;

		
		
		return;
	}
	
	if(sc.Index == sc.ArraySize - 1)
	{
		// Log the current time
		SCString DateTimeString = sc.DateTimeToString(sc.CurrentSystemDateTime,FLAG_DT_COMPLETE_DATETIME_MS);

		sc.AddMessageToLog(DateTimeString, 0);
	}
}

/*============================================================================*/
SCSFExport scsf_GetStudyPersistentVariableFromChartExample(SCStudyInterfaceRef sc)
{
	SCInputRef  ChartStudyReference = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Get Study Persistent Variable from Chart Example";
		sc.AutoLoop = 1;
		

		ChartStudyReference.Name = "Chart Study Reference";
		ChartStudyReference.SetChartStudyValues(1, 0);

		return;
	}

	//Set persistent float variable with a value of 100 using integer key 1.
	float& r_PersistentFloat = sc.GetPersistentFloat(1);
	r_PersistentFloat = 100;

	// Get a reference to a persistent variable with key value 1 in the chart
	// and study specified by the "Chart Study Reference" input. Assuming the 
	// "Chart Study Reference" input is referencing this particular chart and 
	// study instance, then this function will return 100.
	const float PersistentValue = sc.GetPersistentFloatFromChartStudy(ChartStudyReference.GetChartNumber(), ChartStudyReference.GetStudyID(), 1);

	if (sc.Index == sc.ArraySize - 1)
	{
		SCString OutputText;
		OutputText.Format("%f", PersistentValue);

		sc.AddMessageToLog(OutputText, 0);
	}
}

/*============================================================================
	sc.ResetAllScales example function
----------------------------------------------------------------------------*/
SCSFExport scsf_ResetScalesExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "sc.ResetAllScales Example";
		
		
		sc.AutoLoop = 0;
		
		
		return;
	}
	
	
	sc.ResetAllScales = 1;
	
	
}

/*==========================================================================*/
SCSFExport scsf_BidAndAskPrices(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BidPrices = sc.Subgraph[0];
	SCSubgraphRef AskPrices = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Bid & Ask Prices";
		
		sc.StudyDescription = "This study requires 1-tick data in the intraday data file.";
		
		sc.AutoLoop = 1;
		sc.MaintainAdditionalChartDataArrays = 1;
		
		

		sc.GraphRegion = 0;

		BidPrices.Name = "Bid";
		BidPrices.DrawStyle = DRAWSTYLE_LINE;
		BidPrices.PrimaryColor = RGB(255, 127, 0);
		BidPrices.DrawZeros = false;

		AskPrices.Name = "Ask";
		AskPrices.DrawStyle = DRAWSTYLE_LINE;
		AskPrices.PrimaryColor = RGB(255, 255, 0);
		AskPrices.DrawZeros = false;

		return;
	}
	
	
	// Do data processing
	if ((sc.Index  < sc.ArraySize -1) || sc.IsReplayRunning())
	{
		BidPrices[sc.Index] = sc.BaseData[SC_BID_PRICE][sc.Index];
		AskPrices[sc.Index] = sc.BaseData[SC_ASK_PRICE][sc.Index];
	}
	else
	{
		BidPrices[sc.Index] = sc.Bid;
		AskPrices[sc.Index] = sc.Ask;
	}
}

/*============================================================================
	Study function to close a single specified chart in the chartbook.
----------------------------------------------------------------------------*/
SCSFExport scsf_CloseChart(SCStudyInterfaceRef sc)
{
	SCInputRef InChartNumber = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Close Chart";

		sc.StudyDescription
			= "Study function to close a single specified chart in the chartbook.";

		sc.AutoLoop = 0;

		

		sc.GraphRegion = 0;

		InChartNumber.Name = "Chart To Close";
		InChartNumber.SetChartNumber(0);

		return;
	}

	int ChartNumberToClose = InChartNumber.GetChartNumber();

	if (ChartNumberToClose == 0)
		return;

	SCString Message;
	Message.Format("Closing chart #%d.", ChartNumberToClose);
	sc.AddMessageToLog(Message, 0);

	sc.CloseChart(ChartNumberToClose);

	InChartNumber.SetChartNumber(0);
}

/*============================================================================
	Study function to close a single specified chartbook.
----------------------------------------------------------------------------*/
SCSFExport scsf_CloseChartbook(SCStudyInterfaceRef sc)
{
	SCString& r_ChartbookFileName = sc.TextInput;

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Close Chartbook";

		sc.StudyDescription
			= "Study function to close a single specified chartbook.";

		sc.AutoLoop = 0;

		

		sc.GraphRegion = 0;

		sc.TextInputName = "Chartbook File Name To Close";

		return;
	}

	if (r_ChartbookFileName.GetLength() == 0)
		return;

	SCString Message;
	Message.Format("Closing chartbook \"%s\".", r_ChartbookFileName.GetChars());
	sc.AddMessageToLog(Message, 0);

	sc.CloseChartbook(r_ChartbookFileName);

	r_ChartbookFileName = "";
}


/************************************************************************/

//
// PRC - Polynomial Regression Channel
//
// 20110329 - written by aslan
// 20100330 - added redraw option
//

SCSFExport scsf_PolynomialRegressionChannel(SCStudyInterfaceRef sc)
{
	SCInputRef PolyDegree  = sc.Input[0];
	SCInputRef Period      = sc.Input[1];
	SCInputRef StdDev1     = sc.Input[2];
	SCInputRef StdDev2     = sc.Input[3];
	SCInputRef Redraw      = sc.Input[4];

	SCSubgraphRef Fx     = sc.Subgraph[0];
	SCSubgraphRef Upper2 = sc.Subgraph[1];
	SCSubgraphRef Upper1 = sc.Subgraph[2];
	SCSubgraphRef Lower1 = sc.Subgraph[3];
	SCSubgraphRef Lower2 = sc.Subgraph[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Polynomial Regression Channel";
		sc.StudyDescription = "Polynomial Regression Channel";

		PolyDegree.Name = "Polynomial Degree (1-4)";
		PolyDegree.SetInt(3);
		PolyDegree.SetIntLimits(1, 4);

		Period.Name = "PRC Period";
		Period.SetInt(150);
		Period.SetIntLimits(1, 10000);

		StdDev1.Name = "Standard Deviation Level 1";
		StdDev1.SetFloat(1.618f);
		StdDev1.SetFloatLimits(0, 1000);

		StdDev2.Name = "Standard Deviation Level 2";
		StdDev2.SetFloat(2.0);
		StdDev2.SetFloatLimits(0, 1000);

		Redraw.Name = "Use Redraw";
		Redraw.SetYesNo(1);

		Fx.Name = "Mid";
		Fx.DrawZeros = false;
		Fx.DrawStyle = DRAWSTYLE_LINE;
		Fx.LineWidth = 1;
		Fx.LineStyle = LINESTYLE_DOT;
		Fx.PrimaryColor = COLOR_GRAY;

		Upper2.Name = "Upper 2";
		Upper2.DrawZeros = false;
		Upper2.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_TOP;
		Upper2.PrimaryColor = COLOR_RED;

		Upper1.Name = "Upper 1";
		Upper1.DrawZeros = false;
		Upper1.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_BOTTOM;
		Upper1.PrimaryColor = COLOR_RED;

		Lower1.Name = "Lower 1";
		Lower1.DrawZeros = false;
		Lower1.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_TOP;
		Lower1.PrimaryColor = COLOR_BLUE;

		Lower2.Name = "Lower 2";
		Lower2.DrawZeros = false;
		Lower2.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_BOTTOM;
		Lower2.PrimaryColor = COLOR_BLUE;

		sc.AutoLoop = true;
		sc.DrawZeros = false;
		sc.GraphRegion = 0;
		

		sc.GlobalDisplayStudyNameSubgraphNamesAndValues = 0;
		sc.DisplayStudyName = 0;
		sc.DisplayStudyInputValues = 0;

		return;
	}


	struct s_PRCData
	{
		double ai[10][10];
		double b[10];
		double x[10];
		double sx[20];
	};


	int&  lastIndex = sc.GetPersistentInt(1);
	int&  nn        = sc.GetPersistentInt(2);
	s_PRCData *p_PRCMemory = (s_PRCData *) sc.GetPersistentPointer(1);

	if (sc.LastCallToFunction)
	{
		// This study is being removed from the chart or the chart is being closed - cleanup
		if (p_PRCMemory != 0) 
		{
			sc.FreeMemory(p_PRCMemory);
			p_PRCMemory = 0;
			sc.SetPersistentPointer(1, p_PRCMemory);
		}

		return;
	}

	if (sc.Index == 0)
	{
		lastIndex = -1;
		sc.ValueFormat = sc.BaseGraphValueFormat;

		if (p_PRCMemory != 0)
			sc.FreeMemory(p_PRCMemory);

		p_PRCMemory = (s_PRCData*)sc.AllocateMemory(sizeof(s_PRCData));

		sc.SetPersistentPointer(1,p_PRCMemory);

		if (p_PRCMemory == 0) 
			return;	


		// set up static vars
		s_PRCData& PRCData = *p_PRCMemory;

		nn = PolyDegree.GetInt() + 1;

		//----------------------sx------------
		PRCData.sx[1] = Period.GetInt() + 1;
		for(int i=1;i<=nn*2-2;i++) {
			double sum = 0.0;
			for(int n=0;n<=Period.GetInt();n++) {
				sum+=pow((double)n,i);
			}
			PRCData.sx[i+1]=sum;
		}  
	}

	// make sure we have enough bars to start
	if (sc.Index < Period.GetInt())
		return;

	// erase old data on new bar
	if (sc.Index != lastIndex)
	{
		lastIndex = sc.Index;
		if (Redraw.GetYesNo())
		{
			Fx[sc.Index-Period.GetInt()-1] = 0;
			Upper2[sc.Index-Period.GetInt()-1] = 0;
			Upper1[sc.Index-Period.GetInt()-1] = 0;
			Lower1[sc.Index-Period.GetInt()-1] = 0;
			Lower2[sc.Index-Period.GetInt()-1] = 0;
		}
	}

	// now lets do the real work
	if (p_PRCMemory == 0) 
		return;

	s_PRCData& PRCData = *p_PRCMemory;

	//----------------------syx-----------
	for(int i=1;i<=nn;i++) {
		double sum=0.0;
		for(int n=0;n<=Period.GetInt();n++) {
			if(i==1) sum+=sc.Close[sc.Index-n];
			else sum+=sc.Close[sc.Index-n]*pow((double)n,i-1);
		}
		PRCData.b[i]=sum;
	} 
	//===============Matrix===============
	for(int j=1;j<=nn;j++) {
		for(int i=1; i<=nn; i++)	{
			int k=i+j-1;
			PRCData.ai[i][j]=PRCData.sx[k];  // reset ai matrix
		}
	}  
	//===============Gauss================
	for(int k=1; k<=nn-1; k++) {
		int    l=0;
		double m=0.0;
		for(int i=k; i<=nn; i++) {
			if(abs(PRCData.ai[i][k])>m) {
				m=abs(PRCData.ai[i][k]);
				l=i;
			}
		}
		if(l==0) return;   
		if (l!=k)	{
			double tt = 0.0;
			for(int j=1; j<=nn; j++)	{
				tt=PRCData.ai[k][j];
				PRCData.ai[k][j]=PRCData.ai[l][j];
				PRCData.ai[l][j]=tt;
			}
			tt=PRCData.b[k];
			PRCData.b[k]=PRCData.b[l];
			PRCData.b[l]=tt;
		}  
		for(int i=k+1;i<=nn;i++) {
			double qq=PRCData.ai[i][k]/PRCData.ai[k][k];
			for(int j=1;j<=nn;j++) {
				if(j==k) PRCData.ai[i][j]=0;
				else PRCData.ai[i][j]=PRCData.ai[i][j]-qq*PRCData.ai[k][j];
			}
			PRCData.b[i]=PRCData.b[i]-qq*PRCData.b[k];
		}
	}  
	PRCData.x[nn]=PRCData.b[nn]/PRCData.ai[nn][nn];
	for(int i=nn-1;i>=1;i--)	{
		double tt=0.0;
		for(int j=1;j<=nn-i;j++) {
			tt=tt+PRCData.ai[i][i+j]*PRCData.x[i+j];
			PRCData.x[i]=(1/PRCData.ai[i][i])*(PRCData.b[i]-tt);
		}
	} 
	//====================================
	for(int n=0;n<=Period.GetInt();n++) {
		double sum=0.0;
		for(int k=1;k<=PolyDegree.GetInt();k++) {
			sum+=PRCData.x[k+1]*pow((double)n,k);
		}
		if (n==0 || Redraw.GetYesNo())
			Fx[sc.Index-n] = (float)(PRCData.x[1]+sum);
	} 
	//-------------------Std--------------
	double sq=0.0, sq1=0.0, sq2=0.0;
	for(int n=0;n<=Period.GetInt();n++) {
		sq+=pow(sc.Close[sc.Index-n]-Fx[sc.Index-n],2);
	}
	sq=sqrt(sq/(Period.GetInt()+1));
	sq1=sq*StdDev1.GetFloat();
	sq2=sq*StdDev2.GetFloat();

	for(int n=0;n<=Period.GetInt();n++) {
		if (n==0 || Redraw.GetYesNo())
		{
			Upper1[sc.Index-n] = (float)(Fx[sc.Index-n]+sq1);
			Lower1[sc.Index-n] = (float)(Fx[sc.Index-n]-sq1);
			Upper2[sc.Index-n] = (float)(Fx[sc.Index-n]+sq2);
			Lower2[sc.Index-n] = (float)(Fx[sc.Index-n]-sq2);
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_AveragePriceForBar(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AverageSubgraph = sc.Subgraph[0];

	SCInputRef AverageFormulaInput = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults.
		
		sc.GraphName = "Average Price For Bar";
		sc.GraphRegion = 0;
		sc.AutoLoop = 1;
		
		AverageSubgraph.Name = "Average";
		AverageSubgraph.DrawStyle = DRAWSTYLE_DASH;
		AverageSubgraph.LineWidth = 2;
		AverageSubgraph.PrimaryColor = RGB(255, 128, 0);
		
		AverageFormulaInput.Name = "Average Formula";
		AverageFormulaInput.SetCustomInputStrings
			( "(High+Low)/2"
				";(High+Low+Close)/3"
				";(Open+High+Low+Close)/4"
				";(High+Low+Close+Close)/4"
			);
		AverageFormulaInput.SetCustomInputIndex(0);

		return;
	}

	// Do data processing.

	const int SelectedIndex = AverageFormulaInput.GetIndex();
	switch (SelectedIndex)
	{
		case 0:
		{
			AverageSubgraph.Data[sc.Index]
				= (sc.High[sc.Index] + sc.Low[sc.Index]) / 2.0f;
		}
		break;

		case 1:
		{
			AverageSubgraph.Data[sc.Index]
				= (sc.High[sc.Index] + sc.Low[sc.Index] + sc.Close[sc.Index])
				/ 3.0f;
		}
		break;

		case 2:
		{
			AverageSubgraph.Data[sc.Index]
				= (sc.Open[sc.Index]
					+ sc.High[sc.Index]
					+ sc.Low[sc.Index]
					+ sc.Close[sc.Index]
				)
				/ 4.0f;
		}
		break;

		case 3:
		{
			AverageSubgraph.Data[sc.Index]
				= (sc.High[sc.Index]
					+ sc.Low[sc.Index]
					+ sc.Close[sc.Index] * 2
				)
				/ 4.0f;
		}
		break;
	}
}

/*============================================================================

----------------------------------------------------------------------------*/
SCSFExport scsf_ActionWhenTimeEncountered(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Action When Time  is Encountered";
		
		sc.StudyDescription = "This study function demonstrates performing an action when a certain time is encountered in the most recent chart bar.";
		
		sc.AutoLoop = 1;
		
		
		
		return;
	}

	
	// Do data processing

	SCDateTime TimeToCheckFor;

	//The first step is to get the current date.
	int CurrentDate = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();

	//Apply the time. For this example we will use 12 PM
	TimeToCheckFor.SetDate(CurrentDate);
	TimeToCheckFor.SetTimeHMS(12, 0, 0);

	// TimeToCheckFor is contained within the current bar.
	if (sc.IsDateTimeContainedInBarIndex(TimeToCheckFor, sc.Index))
	{
		//perform the action here
	}


}

/*============================================================================
	Symbol Display on chart
----------------------------------------------------------------------------*/
SCSFExport scsf_SymbolDisplay(SCStudyInterfaceRef sc)
{

	SCSubgraphRef SymbolText = sc.Subgraph[0];

	SCInputRef Input_HorizontalPosition = sc.Input[0];
	SCInputRef Input_VerticalPosition = sc.Input[1];
	SCInputRef Input_TransparentLabelBackground = sc.Input[3];
	SCInputRef Input_UseSymbolDescription = sc.Input[5];
	SCInputRef Input_DrawAboveMainPriceGraph = sc.Input[6];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Symbol Display";
		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 0;
		sc.ValueFormat = 0;

		SymbolText.Name = "Symbol Text";
		SymbolText.LineWidth = 20;
		SymbolText.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		SymbolText.PrimaryColor = RGB(0, 0, 0); //black
		SymbolText.SecondaryColor = RGB(255, 127, 0); //Orange
		SymbolText.SecondaryColorUsed = true;
		SymbolText.DisplayNameValueInWindowsFlags = 1;

		Input_HorizontalPosition.Name.Format("Initial Horizontal Position From Left (1-%d)", (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);
		Input_HorizontalPosition.SetInt(20);
		Input_HorizontalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

		Input_VerticalPosition.Name.Format("Initial Vertical Position From Bottom (1-%d)", (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		Input_VerticalPosition.SetInt(90);
		Input_VerticalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		
		Input_TransparentLabelBackground.Name = "Transparent Label Background";
		Input_TransparentLabelBackground.SetYesNo(false);
		
		Input_UseSymbolDescription.Name = "Use Symbol Description";
		Input_UseSymbolDescription.SetYesNo(false);

		Input_DrawAboveMainPriceGraph.Name = "Draw Above Main Price Graph";
		Input_DrawAboveMainPriceGraph.SetYesNo(false);

		sc.TextInputName = "Alternate Symbol Text";

		
		
		return;
	}

	SCString TextToDisplay;
	if (Input_UseSymbolDescription.GetYesNo())
		TextToDisplay = sc.SymbolData->Description;
	else if (sc.TextInput.GetLength() > 0)
		TextToDisplay = sc.TextInput;
	else
		TextToDisplay = sc.Symbol;

	int HorizontalPosition = Input_HorizontalPosition.GetInt();
	int VerticalPosition = Input_VerticalPosition.GetInt();

	int DrawAboveMainPriceGraph = Input_DrawAboveMainPriceGraph.GetYesNo();
	int TransparentLabelBackground = Input_TransparentLabelBackground.GetYesNo();

	sc.AddAndManageSingleTextUserDrawnDrawingForStudy(sc, false, HorizontalPosition, VerticalPosition, SymbolText, TransparentLabelBackground, TextToDisplay, DrawAboveMainPriceGraph);

}

/*==========================================================================*/
SCSFExport scsf_TradingProfitManagementStatus(SCStudyInterfaceRef sc)
{
	SCSubgraphRef StatusText = sc.Subgraph[0];

	SCInputRef Input_HorizontalPosition = sc.Input[0];
	SCInputRef Input_VerticalPosition = sc.Input[1];
	SCInputRef Input_DrawAboveMainPriceGraph = sc.Input[2];
	SCInputRef Input_TransparentLabelBackground = sc.Input[3];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Trading Profit Management Status";
		sc.AutoLoop = 0;//Manual looping
		sc.GraphRegion = 0;
		sc.ValueFormat = 0;

		StatusText.Name = "Status Text";
		StatusText.LineWidth = 10;
		StatusText.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		StatusText.PrimaryColor = RGB(255, 255, 255);
		StatusText.SecondaryColor = RGB(0, 0, 160);
		StatusText.SecondaryColorUsed = true;
		StatusText.DisplayNameValueInWindowsFlags = 1;

		Input_HorizontalPosition.Name.Format("Initial Horizontal Position From Left (1-%d)", (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);
		Input_HorizontalPosition.SetInt(20);
		Input_HorizontalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_HORIZONTAL_AXIS_RELATIVE_POSITION);

		Input_VerticalPosition.Name.Format("Initial Vertical Position From Bottom (1-%d)", (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);
		Input_VerticalPosition.SetInt(90);
		Input_VerticalPosition.SetIntLimits(1, (int)CHART_DRAWING_MAX_VERTICAL_AXIS_RELATIVE_POSITION);

		Input_DrawAboveMainPriceGraph.Name = "Draw Above Main Price Graph";
		Input_DrawAboveMainPriceGraph.SetYesNo(false);
		
		Input_TransparentLabelBackground.Name = "Transparent Label Background";
		Input_TransparentLabelBackground.SetYesNo(false);

		

		return;
	}


	// Do data processing
	SCString TextToDisplay;
	sc.GetProfitManagementStringForTradeAccount(TextToDisplay);

	int HorizontalPosition = Input_HorizontalPosition.GetInt();
	int VerticalPosition = Input_VerticalPosition.GetInt();

	int DrawAboveMainPriceGraph = Input_DrawAboveMainPriceGraph.GetYesNo();
	int TransparentLabelBackground = Input_TransparentLabelBackground.GetYesNo();

	sc.AddAndManageSingleTextUserDrawnDrawingForStudy(sc, false, HorizontalPosition, VerticalPosition, StatusText, TransparentLabelBackground, TextToDisplay, DrawAboveMainPriceGraph);
	

}



/*============================================================================*/
SCSFExport scsf_TimeAndSalesIterationExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		
		sc.GraphName = "Time and Sales Iteration Example";
		
		sc.StudyDescription = "This is an example of iterating through the time and sales records which have been added since the last study function call.";
		
		sc.AutoLoop = 0;//Use manual looping
		
		

		sc.GraphRegion = 0;
		
		return;
	}

	//This code depends upon manual looping being set
	
	int64_t& LastProcessedSequence = sc.GetPersistentInt64(1);

	//reset the sequence number on a full recalculation so we start fresh for each full recalculation.
	if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
		LastProcessedSequence = 0;

	// Get the Time and Sales
	c_SCTimeAndSalesArray TimeSales;
	sc.GetTimeAndSales(TimeSales);
	if (TimeSales.Size() == 0)
		return;  // No Time and Sales data available for the symbol

	//Set the initial sequence number
	if (LastProcessedSequence == 0)
		LastProcessedSequence = TimeSales[TimeSales.Size() - 1].Sequence;

	// Loop through the Time and Sales.
	for (int TSIndex = 0; TSIndex < TimeSales.Size(); ++TSIndex)
	{
		//do not reprocess previously processed sequence numbers.
		if (TimeSales[TSIndex].Sequence <= LastProcessedSequence)
			continue;

		//only interested in trade records
		if (TimeSales[TSIndex].Type == SC_TS_BID || TimeSales[TSIndex].Type == SC_TS_ASK)
		{

			float TradePrice = TimeSales[TSIndex].Price;
			float BidPrice = TimeSales[TSIndex].Bid;
			float AskPrice = TimeSales[TSIndex].Ask;

			SCDateTime RecordAdjustedDateTime = TimeSales[TSIndex].DateTime;
			// Apply the time zone offset for the chart. This will result in the actual date-time of the record in the charts time zone.
			RecordAdjustedDateTime += sc.TimeScaleAdjustment;  

		}
	}

}

/*==========================================================================*/
SCSFExport scsf_ServerConnectionStateExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "ServerConnectionState Example";

		sc.AutoLoop = 0;

		

		return;
	}

	SCString MessageText = "Current Data/Trade Service connection state: ";

	switch (sc.ServerConnectionState) 
	{
	case SCS_DISCONNECTED:
		MessageText += "SCS_DISCONNECTED";
		break;

	case SCS_CONNECTING:
		MessageText += "SCS_CONNECTING";
		break;

	case SCS_CONNECTED:
		MessageText += "SCS_CONNECTED";
		break;

	case SCS_CONNECTION_LOST:
		MessageText += "SCS_CONNECTION_LOST";
		break;

	case SCS_DISCONNECTING:
		MessageText += "SCS_DISCONNECTING";
		break;

	case SCS_RECONNECTING:
		MessageText += "SCS_RECONNECTING";
		break;

	default:
		MessageText += "INVALID";
		break;
	}

	sc.AddMessageToLog(MessageText, 0);
}

/*============================================================================*/
SCSFExport scsf_StudySubgraphDailyHighLow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighOfDay = sc.Subgraph[0];
	SCSubgraphRef LowOfDay = sc.Subgraph[1];
	SCInputRef StudySubgraphReference = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName			= "Study Subgraph Daily High Low";
		sc.GraphRegion			= 1;
		sc.AutoLoop				= 0;

		HighOfDay.Name = "High";
		HighOfDay.DrawStyle = DRAWSTYLE_STAIR_STEP;
		HighOfDay.PrimaryColor = RGB(0,255,0);
		HighOfDay. DrawZeros = false;

		LowOfDay.Name = "Low";
		LowOfDay.DrawStyle = DRAWSTYLE_STAIR_STEP;
		LowOfDay.PrimaryColor = RGB(255,0,255);
		LowOfDay. DrawZeros = false;

		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		StudySubgraphReference.Name = "Study Subgraph Reference";

		StudySubgraphReference.SetStudySubgraphValues(1, 0);

		return;
	}

	float& r_CurrentDailyHigh = sc.GetPersistentFloat(1);
	float& r_CurrentDailyLow = sc.GetPersistentFloat(2);

	//Loop
	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++ )
	{


		if (BarIndex > 0)
		{
			SCDateTime PreviousTradingDayStartDateTimeOfBar = sc.GetTradingDayStartDateTimeOfBar (sc.BaseDateTimeIn[BarIndex - 1]);
			SCDateTime TradingDayStartDateTimeOfBar = sc.GetTradingDayStartDateTimeOfBar (sc.BaseDateTimeIn[BarIndex]);

			if (PreviousTradingDayStartDateTimeOfBar != TradingDayStartDateTimeOfBar)
			{
				r_CurrentDailyHigh = 0;
				r_CurrentDailyLow = 0;
			}
		}
		else
		{
			r_CurrentDailyHigh = 0;
			r_CurrentDailyLow = 0;
		}

		SCFloatArray StudySubgraph;
		sc.GetStudyArrayUsingID(StudySubgraphReference.GetStudyID(), StudySubgraphReference.GetSubgraphIndex(), StudySubgraph);

		if (StudySubgraph.GetArraySize() == 0)
			return;

		if (r_CurrentDailyHigh == 0 || r_CurrentDailyHigh < StudySubgraph[BarIndex])
			r_CurrentDailyHigh = StudySubgraph[BarIndex];

		if (r_CurrentDailyLow == 0 || r_CurrentDailyLow > StudySubgraph[BarIndex])
			r_CurrentDailyLow = StudySubgraph[BarIndex];

		HighOfDay[BarIndex] = r_CurrentDailyHigh;
		LowOfDay[BarIndex] = r_CurrentDailyLow;
	}
}

/*==========================================================================*/
SCSFExport scsf_TimeSeriesForecast(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TimeSeriesForecast = sc.Subgraph[0];
	SCFloatArrayRef LinearRegressionSlope = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef LinearRegressionIndicator = sc.Subgraph[0].Arrays[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Time Series Forecast";

		sc.AutoLoop = 1;
		

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		TimeSeriesForecast.Name = "TSF";
		TimeSeriesForecast.DrawStyle = DRAWSTYLE_LINE;
		TimeSeriesForecast.PrimaryColor = RGB(0,255,0);
		TimeSeriesForecast.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1,MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt()-1;

	int CalculationLength=Length.GetInt();
	double SumBars = CalculationLength*(CalculationLength-1)/2.0;
	double SumSqrBars = (CalculationLength-1)*CalculationLength*(2*CalculationLength-1)/6.0;

	double Sum1 = 0;
	double SumY = 0;
	for(int Index=sc.Index; Index > (sc.Index - CalculationLength); Index--)
	{
		int IndexDifference = sc.Index - Index;
		Sum1 = Sum1 + IndexDifference * sc.BaseData[InputData.GetInputDataIndex()][Index];
		SumY += sc.BaseData[InputData.GetInputDataIndex()][Index];
	}

	double Sum2 = SumBars * SumY;
	double Num1 = CalculationLength*Sum1 - Sum2;
	double Num2 = SumBars*SumBars - CalculationLength*SumSqrBars;
	LinearRegressionSlope[sc.Index] = (float)(Num1 / Num2);

	sc.LinearRegressionIndicator(sc.BaseData[InputData.GetInputDataIndex()], LinearRegressionIndicator, CalculationLength);

	TimeSeriesForecast[sc.Index] = LinearRegressionIndicator[sc.Index] + LinearRegressionSlope[sc.Index];


	return;
}

/*==========================================================================*/
SCSFExport scsf_ExtendClosesUntilFutureIntersection(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LinePropertiesSubgraph = sc.Subgraph[0];
	SCInputRef DisplayValueLabel = sc.Input[0];
	SCInputRef NameLabel = sc.Input[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults.
		
		sc.GraphName = "Extend Closes Until Future Intersection";

		LinePropertiesSubgraph.Name = "Line Properties";
		LinePropertiesSubgraph.DrawStyle = DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY;
		LinePropertiesSubgraph.LineWidth = 1;
		LinePropertiesSubgraph.PrimaryColor = COLOR_RED;
		LinePropertiesSubgraph.DrawZeros = false;

		DisplayValueLabel.Name = "Display Value Label";
		DisplayValueLabel.SetYesNo(true);

		NameLabel.Name = "Name Label";
		NameLabel.SetString("");

		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.GraphRegion = 0;
		
		sc.AutoLoop = 1;
		
		
		
		return;
	}
	
	if (sc.Index == 0)
		return;

	const SCDateTime CurrentBarStartDateTimeForDay
		= sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.Index]);

	const SCDateTime PriorBarStartDateTimeForDay
		= sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.Index - 1]);

	const bool IsNewDay = (CurrentBarStartDateTimeForDay != PriorBarStartDateTimeForDay);

	if (!IsNewDay)
		return;

	const float ClosePriceOfPriorBar = sc.Close[sc.Index - 1];
	const char* NameLabelString = NameLabel.GetString();

	bool DisplayNameLabel = false;

	if (NameLabelString != NULL && strlen(NameLabelString) > 0)
		DisplayNameLabel = true;
	else
		NameLabelString = "";

	sc.AddLineUntilFutureIntersection
		( sc.Index - 1
		, 0  // LineIDForBar
		, ClosePriceOfPriorBar
		, LinePropertiesSubgraph.PrimaryColor
		, LinePropertiesSubgraph.LineWidth
		, LinePropertiesSubgraph.LineStyle
		, DisplayValueLabel.GetYesNo()
		, DisplayNameLabel
		, NameLabelString
		);
}

/*==========================================================================*/
SCSFExport scsf_ExtendBarValueUntilFutureIntersection(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LinePropertiesSubgraph = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef DisplayValueLabel = sc.Input[1];
	SCInputRef NameLabel = sc.Input[2];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults.

		sc.GraphName = "Extend Bar Value Until Future Intersection";

		LinePropertiesSubgraph.Name = "Line Properties";
		LinePropertiesSubgraph.DrawStyle = DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY;
		LinePropertiesSubgraph.LineWidth = 1;
		LinePropertiesSubgraph.PrimaryColor = COLOR_RED;
		LinePropertiesSubgraph.DrawZeros = false;

		InputData.Name = "Input Data from Bar for Extension Lines";
		InputData.SetInputDataIndex(SC_LAST);

		DisplayValueLabel.Name = "Display Value Label";
		DisplayValueLabel.SetYesNo(true);

		NameLabel.Name = "Name Label";
		NameLabel.SetString("");

		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.GraphRegion = 0;

		sc.AutoLoop = 1;

		

		return;
	}


	
	const char* NameLabelString = NameLabel.GetString();

	bool DisplayNameLabel = false;

	if (NameLabelString != NULL && strlen(NameLabelString) > 0)
		DisplayNameLabel = true;
	else
		NameLabelString = "";

	sc.AddLineUntilFutureIntersection
		( sc.Index 
		, 0  // LineIDForBar
		, sc.BaseData[ InputData.GetInputDataIndex()][sc.Index]
		, LinePropertiesSubgraph.PrimaryColor
		, LinePropertiesSubgraph.LineWidth
		, LinePropertiesSubgraph.LineStyle
		, DisplayValueLabel.GetYesNo()
		, DisplayNameLabel
		, NameLabelString
		);
}

/*==========================================================================*/
SCSFExport scsf_ExtendBarGapUntilFutureIntersection(SCStudyInterfaceRef sc)
{
	SCSubgraphRef LinePropertiesSubgraph = sc.Subgraph[0];
	SCInputRef DisplayValueLabel = sc.Input[0];
	SCInputRef NameLabel = sc.Input[1];
	SCInputRef CompareToOpentoCloseRangeOnly = sc.Input[2];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults.

		sc.GraphName = "Extend Bar Gap Until Future Intersection";

		LinePropertiesSubgraph.Name = "Line Properties";
		LinePropertiesSubgraph.DrawStyle = DRAWSTYLE_SUBGRAPH_NAME_AND_VALUE_LABELS_ONLY;
		LinePropertiesSubgraph.LineWidth = 1;
		LinePropertiesSubgraph.PrimaryColor = COLOR_RED;
		LinePropertiesSubgraph.DrawZeros = false;

		DisplayValueLabel.Name = "Display Value Label";
		DisplayValueLabel.SetYesNo(true);

		NameLabel.Name = "Name Label";
		NameLabel.SetString("");

		CompareToOpentoCloseRangeOnly.Name = "Compare to Open to Close Range Only";
		CompareToOpentoCloseRangeOnly.SetYesNo(false);

		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.GraphRegion = 0;

		sc.AutoLoop = 1;

		

		return;
	}

	
	if (sc.Index == 0)
		return;

	float ExtendPrice = 0.0;

	if (!CompareToOpentoCloseRangeOnly.GetYesNo())
	{
		//Check for up gap
		if(sc.FormattedEvaluate(sc.High[sc.Index-1], sc.BaseGraphValueFormat, LESS_OPERATOR,  sc.Low[sc.Index] ,sc.BaseGraphValueFormat ) )
			ExtendPrice = sc.High[sc.Index-1];
		//Check for  down gap
		else if(sc.FormattedEvaluate(sc.Low[sc.Index-1], sc.BaseGraphValueFormat, GREATER_OPERATOR,  sc.High[sc.Index] , sc.BaseGraphValueFormat ) )
			ExtendPrice = sc.Low[sc.Index-1];
		else
			return;
	}
	else
	{
		float PriorMaxValue = max(sc.Open[sc.Index - 1], sc.Close[sc.Index - 1]);
		float PriorMinValue = min(sc.Open[sc.Index - 1], sc.Close[sc.Index - 1]);
		float MaxValue = max(sc.Open[sc.Index], sc.Close[sc.Index]);
		float MinValue = min(sc.Open[sc.Index], sc.Close[sc.Index]);

		//Check for up gap
		if(sc.FormattedEvaluate(PriorMaxValue, sc.BaseGraphValueFormat, LESS_OPERATOR, MinValue ,sc.BaseGraphValueFormat ) )
			ExtendPrice = PriorMaxValue;
		//Check for  down gap
		else if(sc.FormattedEvaluate(PriorMinValue, sc.BaseGraphValueFormat, GREATER_OPERATOR, MaxValue, sc.BaseGraphValueFormat ) )
			ExtendPrice = PriorMinValue;
		else
			return;
	}

	const char* NameLabelString = NameLabel.GetString();

	bool DisplayNameLabel = false;

	if (NameLabelString != NULL && strlen(NameLabelString) > 0)
		DisplayNameLabel = true;
	else
		NameLabelString = "";

	sc.AddLineUntilFutureIntersection
		( sc.Index - 1
		, 0  // LineIDForBar. Use 0 since there is only one line per bar.
		, ExtendPrice
		, LinePropertiesSubgraph.PrimaryColor
		, LinePropertiesSubgraph.LineWidth
		, LinePropertiesSubgraph.LineStyle
		, DisplayValueLabel.GetYesNo()
		, DisplayNameLabel
		, NameLabelString
		);
}
/*==========================================================================*/
SCSFExport scsf_MarketDepthPullingStackingValuesExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef BidPullingStackingSubgraph = sc.Subgraph[0];
	SCSubgraphRef AskPullingStackingSubgraph = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Market Depth Pulling/Stacking Values Example";

		sc.GraphRegion = 1;

		sc.AutoLoop = 0;

		

		BidPullingStackingSubgraph.Name = "Bid";
		BidPullingStackingSubgraph.DrawStyle = DRAWSTYLE_LINE;

		AskPullingStackingSubgraph.Name = "Ask";
		AskPullingStackingSubgraph.DrawStyle = DRAWSTYLE_LINE;

		return;
	}


	//The following two functions will return 0 if there is no pulling and stacking value at the specified price
	BidPullingStackingSubgraph[sc.ArraySize - 1] = (float)sc.GetBidMarketDepthStackPullValueAtPrice(sc.Bid);
	AskPullingStackingSubgraph[sc.ArraySize - 1] = (float)sc.GetAskMarketDepthStackPullValueAtPrice(sc.Ask);
}

/*==========================================================================*/

SCSFExport scsf_Repulse(SCStudyGraphRef sc)
{
	SCSubgraphRef RepulseSubgraph = sc.Subgraph[0];
	SCInputRef LengthInput = sc.Input[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Repulse";

		sc.AutoLoop = 1;

		sc.GraphRegion = 1;

		RepulseSubgraph.Name = "Repulse";
		RepulseSubgraph.PrimaryColor = RGB(0, 255, 0);
		RepulseSubgraph.DrawStyle = DRAWSTYLE_LINE;
		RepulseSubgraph.LineWidth = 2;

		LengthInput.Name = "Length";
		LengthInput.SetInt(10);

		
		return;
	}

	// Do data processing
	/*
	lo = LOWEST(LOW)
	hi = HIGHEST(HIGH)
	a = 100 * ( 3 * CLOSE - 2 * lo - OPEN[p-1] ) / CLOSE
	b = 100 * ( OPEN[p-1] + 2 * hi - 3 * CLOSE ) / CLOSE
	d = EXPONENTIALAVERAGE[5*p](a) - EXPONENTIALAVERAGE[5*p](b)
	RETURN d*/
	int Length = LengthInput.GetInt();

	sc.Highest(sc.High, RepulseSubgraph.Arrays[0], Length);
	sc.Lowest(sc.Low, RepulseSubgraph.Arrays[1], Length);

	RepulseSubgraph.Arrays[2][sc.Index] = 100 * (3 * sc.Close[sc.Index] - 2 * RepulseSubgraph.Arrays[1][sc.Index] - sc.Open[sc.Index - Length + 1]) / sc.Close[sc.Index];

	RepulseSubgraph.Arrays[3][sc.Index] = 100 * (sc.Open[sc.Index - Length + 1] + 2 * RepulseSubgraph.Arrays[0][sc.Index] - 3 * sc.Close[sc.Index]) / sc.Close[sc.Index];



	sc.MovingAverage(RepulseSubgraph.Arrays[2], RepulseSubgraph.Arrays[4], MOVAVGTYPE_EXPONENTIAL, 5*Length);
	sc.MovingAverage(RepulseSubgraph.Arrays[3], RepulseSubgraph.Arrays[5], MOVAVGTYPE_EXPONENTIAL, 5*Length);

	RepulseSubgraph.Data[sc.Index] = RepulseSubgraph.Arrays[4][sc.Index] - RepulseSubgraph.Arrays[5][sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_FullContractValue(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Result = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef DrawZeros = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Full Contract Value";
		sc.ValueFormat = VALUEFORMAT_2_DIGITS;
		sc.AutoLoop = 0;// Needed when using sc.GetCalculationStartIndexForStudy

		

		Result.Name = "Result";
		Result.DrawStyle = DRAWSTYLE_LINE;
		Result.PrimaryColor = RGB(0, 255, 0);

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		DrawZeros.Name = "Draw Zeros";
		DrawZeros.SetYesNo(false);

		return;
	}

	Result.DrawZeros = DrawZeros.GetYesNo();

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int Index = CalculationStartIndex; Index < sc.ArraySize; Index++)
	{

		Result[Index] = sc.BaseData[InputData.GetInputDataIndex()][Index] / sc.TickSize * max(sc.CurrencyValuePerTick, sc.TickSize);
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;
}

/*==========================================================================*/
SCSFExport scsf_KeyboardModifierStatesExample(SCStudyInterfaceRef sc)
{
	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Keyboard Modifier States Example";

		sc.AutoLoop = 0;///Manual looping

		
		sc.GraphRegion = 0;
		
		sc.UpdateAlways = 1;
		sc.SupportKeyboardModifierStates = 1;

		return;
	}

	if (sc.IsKeyPressed_Alt)
	{
		sc.AddMessageToLog("Alt key pressed", 0);
	}

	if (sc.IsKeyPressed_Shift)
	{
		sc.AddMessageToLog("Shift key pressed", 0);
	}

	if (sc.IsKeyPressed_Control)
	{
		sc.AddMessageToLog("Control key pressed", 0);
	}
}
/*==========================================================================*/

SCSFExport scsf_BalanceOfPower(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_BalanceOfPower = sc.Subgraph[0];
	SCSubgraphRef Subgraph_BalanceOfPowerAverage = sc.Subgraph[1];

	SCInputRef Input_MovingAverageLength = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Balance of Power";
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;

		Subgraph_BalanceOfPower.Name = "BOP";
		Subgraph_BalanceOfPower.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BalanceOfPower.PrimaryColor = RGB(0, 255, 0);
		Subgraph_BalanceOfPower.LineWidth = 2;

		Subgraph_BalanceOfPowerAverage.Name = "BOP Average";
		Subgraph_BalanceOfPowerAverage.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BalanceOfPowerAverage.PrimaryColor = RGB(0, 0, 255);
		Subgraph_BalanceOfPowerAverage.LineWidth = 2;

		Input_MovingAverageLength.Name = "Moving Average Length";
		Input_MovingAverageLength.SetInt(10);
		Input_MovingAverageLength.SetIntLimits(1, MAX_STUDY_LENGTH);
		sc.AutoLoop = 1;

		
		return;
	}

	sc.DataStartIndex = Input_MovingAverageLength.GetInt() - 1;

	Subgraph_BalanceOfPower.Data[sc.Index] = (sc.BaseData[SC_LAST][sc.Index] - sc.BaseData[SC_OPEN][sc.Index]) / (sc.BaseData[SC_HIGH][sc.Index] - sc.BaseData[SC_LOW][sc.Index]);

	sc.SimpleMovAvg(Subgraph_BalanceOfPower, Subgraph_BalanceOfPowerAverage, Input_MovingAverageLength.GetInt());

}
/*==========================================================================*/
SCSFExport scsf_MACDBollingerBandsImproved(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_MACD = sc.Subgraph[0];
	SCSubgraphRef Subgraph_MovAvgOfMACD = sc.Subgraph[1];
	SCSubgraphRef Subgraph_MACDDiff = sc.Subgraph[2];
	SCSubgraphRef Subgraph_TopBand = sc.Subgraph[3];
	SCSubgraphRef Subgraph_BottomBand = sc.Subgraph[4];
	SCSubgraphRef Subgraph_RefLine = sc.Subgraph[5];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_FastLength = sc.Input[1];
	SCInputRef Input_SlowLength = sc.Input[2];
	SCInputRef Input_MACDLength = sc.Input[3];
	SCInputRef Input_NumberOfStdDevs = sc.Input[4];
	SCInputRef Input_MovingAverageType = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "MACD Bollinger Bands - Improved";
		sc.AutoLoop = 1;

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Subgraph_MovAvgOfMACD.Name = "MA of MACD";
		Subgraph_MovAvgOfMACD.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_MovAvgOfMACD.DrawZeros = true;
		Subgraph_MovAvgOfMACD.PrimaryColor = RGB(0, 0, 255);

		Subgraph_TopBand.Name = "Top MACD Bollinger Band";
		Subgraph_TopBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_TopBand.DrawZeros = true;
		Subgraph_TopBand.PrimaryColor = RGB(0, 255, 0);

		Subgraph_BottomBand.Name = "Bottom MACD Bollinger Band";
		Subgraph_BottomBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BottomBand.DrawZeros = true;
		Subgraph_BottomBand.PrimaryColor = RGB(255, 0, 0);

		Subgraph_RefLine.Name = "Line";
		Subgraph_RefLine.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_RefLine.DrawZeros = true;
		Subgraph_RefLine.PrimaryColor = RGB(255, 127, 0);

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_FastLength.Name = "Fast Moving Average Length";
		Input_FastLength.SetInt(12);
		Input_FastLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_SlowLength.Name = "Slow Moving Average Length";
		Input_SlowLength.SetInt(26);
		Input_SlowLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MACDLength.Name = "MACD Moving Average Length";
		Input_MACDLength.SetInt(9);
		Input_MACDLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_NumberOfStdDevs.Name = "Number of Standard Deviations";
		Input_NumberOfStdDevs.SetInt(1);
		Input_NumberOfStdDevs.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovingAverageType.Name = "Moving Average Type";
		Input_MovingAverageType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		return;
	}

	sc.DataStartIndex = 2 * Input_MACDLength.GetInt() + max(Input_FastLength.GetInt(), Input_SlowLength.GetInt());

	sc.MACD(sc.BaseDataIn[Input_InputData.GetInputDataIndex()], Subgraph_MACD, sc.Index, Input_FastLength.GetInt(), Input_SlowLength.GetInt(), Input_MACDLength.GetInt(), Input_MovingAverageType.GetInt());

	Subgraph_MovAvgOfMACD[sc.Index] = Subgraph_MACD.Arrays[2][sc.Index];
	Subgraph_MACDDiff[sc.Index] = Subgraph_MACD.Arrays[3][sc.Index];

	Subgraph_MACD.Arrays[6][sc.Index] = fabs(Subgraph_MACDDiff[sc.Index]);

	sc.MovingAverage(Subgraph_MACD.Arrays[6], Subgraph_MACD.Arrays[7], Input_MovingAverageType.GetMovAvgType(), Input_MACDLength.GetInt());

	float MovAvgAbsValMACDDiff = Subgraph_MACD.Arrays[7][sc.Index];

	sc.StdDeviation(Subgraph_MACD.Arrays[6], Subgraph_MACD.Arrays[8], Input_MACDLength.GetInt());

	float StdDevAbsValMACDDiff = Subgraph_MACD.Arrays[8][sc.Index];

	Subgraph_TopBand[sc.Index] = Subgraph_MovAvgOfMACD[sc.Index] + MovAvgAbsValMACDDiff + Input_NumberOfStdDevs.GetInt()*StdDevAbsValMACDDiff;
	Subgraph_BottomBand[sc.Index] = Subgraph_MovAvgOfMACD[sc.Index] - MovAvgAbsValMACDDiff - Input_NumberOfStdDevs.GetInt()*StdDevAbsValMACDDiff;

	Subgraph_RefLine[sc.Index] = 0;
}
/*==========================================================================*/
SCSFExport scsf_MACDBollingerBandsStandard(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_MACD = sc.Subgraph[0];
	SCSubgraphRef Subgraph_MovAvgOfMACD = sc.Subgraph[1];
	SCSubgraphRef Subgraph_TopBand = sc.Subgraph[2];
	SCSubgraphRef Subgraph_BottomBand = sc.Subgraph[3];
	SCSubgraphRef Subgraph_RefLine = sc.Subgraph[4];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_FastLength = sc.Input[1];
	SCInputRef Input_SlowLength = sc.Input[2];
	SCInputRef Input_MACDLength = sc.Input[3];
	SCInputRef Input_NumberOfStdDevs = sc.Input[4];
	SCInputRef Input_MovingAverageType = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "MACD Bollinger Bands - Standard";
		sc.AutoLoop = 1;

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		Subgraph_MovAvgOfMACD.Name = "MA of MACD";
		Subgraph_MovAvgOfMACD.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_MovAvgOfMACD.DrawZeros = true;
		Subgraph_MovAvgOfMACD.PrimaryColor = RGB(0, 0, 255);

		Subgraph_TopBand.Name = "Top MACD Bollinger Band";
		Subgraph_TopBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_TopBand.DrawZeros = true;
		Subgraph_TopBand.PrimaryColor = RGB(0, 255, 0);

		Subgraph_BottomBand.Name = "Bottom MACD Bollinger Band";
		Subgraph_BottomBand.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BottomBand.DrawZeros = true;
		Subgraph_BottomBand.PrimaryColor = RGB(255, 0, 0);

		Subgraph_RefLine.Name = "Line";
		Subgraph_RefLine.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_RefLine.DrawZeros = true;
		Subgraph_RefLine.PrimaryColor = RGB(255, 127, 0);

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_FastLength.Name = "Fast Moving Average Length";
		Input_FastLength.SetInt(12);
		Input_FastLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_SlowLength.Name = "Slow Moving Average Length";
		Input_SlowLength.SetInt(26);
		Input_SlowLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MACDLength.Name = "MACD Moving Average Length";
		Input_MACDLength.SetInt(9);
		Input_MACDLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_NumberOfStdDevs.Name = "Number of Standard Deviations";
		Input_NumberOfStdDevs.SetInt(1);
		Input_NumberOfStdDevs.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_MovingAverageType.Name = "Moving Average Type";
		Input_MovingAverageType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);

		return;
	}

	sc.DataStartIndex = 2 * Input_MACDLength.GetInt() + max(Input_FastLength.GetInt(), Input_SlowLength.GetInt());

	sc.MACD(sc.BaseDataIn[Input_InputData.GetInputDataIndex()], Subgraph_MACD, sc.Index, Input_FastLength.GetInt(), Input_SlowLength.GetInt(), Input_MACDLength.GetInt(), Input_MovingAverageType.GetInt());

	Subgraph_MovAvgOfMACD[sc.Index] = Subgraph_MACD.Arrays[2][sc.Index];

	sc.StdDeviation(Subgraph_MACD, Subgraph_MACD.Arrays[6], Input_MACDLength.GetInt());
	float StdDevOfMACD = Subgraph_MACD.Arrays[6][sc.Index];

	Subgraph_TopBand[sc.Index] = Subgraph_MovAvgOfMACD[sc.Index] + Input_NumberOfStdDevs.GetInt()*StdDevOfMACD;
	Subgraph_BottomBand[sc.Index] = Subgraph_MovAvgOfMACD[sc.Index] - Input_NumberOfStdDevs.GetInt()*StdDevOfMACD;

	Subgraph_RefLine[sc.Index] = 0;
}

/*==========================================================================*/
SCSFExport scsf_GreatestSwingValue(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_BuySwing = sc.Subgraph[0];
	SCSubgraphRef Subgraph_SellSwing = sc.Subgraph[1];
	SCSubgraphRef Subgraph_AvgBuySwing = sc.Subgraph[2];
	SCSubgraphRef Subgraph_AvgSellSwing = sc.Subgraph[3];
	SCSubgraphRef Subgraph_BuyPrice = sc.Subgraph[4];
	SCSubgraphRef Subgraph_SellPrice = sc.Subgraph[5];

	SCInputRef Input_Length = sc.Input[0];
	SCInputRef Input_Multiplier = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Greatest Swing Value";

		sc.AutoLoop = 1;

		Subgraph_BuySwing.Name = "Buy Swing";
		Subgraph_BuySwing.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_SellSwing.Name = "Sell Swing";
		Subgraph_SellSwing.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_AvgBuySwing.Name = "Average Buy Swing";
		Subgraph_AvgBuySwing.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_AvgSellSwing.Name = "Average Sell Swing";
		Subgraph_AvgSellSwing.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_BuyPrice.Name = "Buy Price";
		Subgraph_BuyPrice.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_BuyPrice.PrimaryColor = RGB(0, 0, 255);

		Subgraph_SellPrice.Name = "Sell Price";
		Subgraph_SellPrice.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_SellPrice.PrimaryColor = RGB(255, 0, 0);

		Input_Length.Name = "Length";
		Input_Length.SetInt(4);
		Input_Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_Multiplier.Name = "Multiplier";
		Input_Multiplier.SetFloat(1.8f);

		return;
	}

	int Length = Input_Length.GetInt();
	float Multiplier = Input_Multiplier.GetFloat();

	sc.DataStartIndex = Input_Length.GetInt() - 1;

	if (sc.Close[sc.Index] < sc.Open[sc.Index])
		Subgraph_BuySwing[sc.Index] = sc.High[sc.Index] - sc.Open[sc.Index];
	else
		Subgraph_BuySwing[sc.Index] = 0;

	if (sc.Close[sc.Index] > sc.Open[sc.Index])
		Subgraph_SellSwing[sc.Index] = sc.Open[sc.Index] - sc.Low[sc.Index];
	else
		Subgraph_SellSwing[sc.Index] = 0;

	sc.MovingAverage(Subgraph_BuySwing, Subgraph_AvgBuySwing, MOVAVGTYPE_SIMPLE_SKIP_ZEROS, Length);
	sc.MovingAverage(Subgraph_SellSwing, Subgraph_AvgSellSwing, MOVAVGTYPE_SIMPLE_SKIP_ZEROS, Length);

	Subgraph_BuyPrice[sc.Index] = sc.Open[sc.Index] + Multiplier * Subgraph_AvgBuySwing[sc.Index - 1];
	Subgraph_SellPrice[sc.Index] = sc.Open[sc.Index] - Multiplier * Subgraph_AvgSellSwing[sc.Index - 1];
}

/*==========================================================================*/
SCSFExport scsf_KDJ(SCStudyInterfaceRef sc)
{
	SCSubgraphRef SlowK = sc.Subgraph[0];
	SCSubgraphRef SlowD = sc.Subgraph[1];
	SCSubgraphRef Line1 = sc.Subgraph[2];
	SCSubgraphRef Line2 = sc.Subgraph[3];
	SCSubgraphRef StochasticOutput = sc.Subgraph[4];
	SCSubgraphRef JLine = sc.Subgraph[5];

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
		sc.GraphName = "KDJ";

		sc.ValueFormat = 2;

		SlowK.Name = "Slow %K";
		SlowK.DrawStyle = DRAWSTYLE_LINE;
		SlowK.PrimaryColor = RGB(0, 255, 0);
		SlowK.DrawZeros = true;

		SlowD.Name = "Slow %D";
		SlowD.DrawStyle = DRAWSTYLE_LINE;
		SlowD.PrimaryColor = RGB(255, 0, 255);
		SlowD.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255, 255, 0);
		Line1.DrawZeros = true;

		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255, 127, 0);
		Line2.DrawZeros = true;

		JLine.Name = "J Line";
		JLine.DrawStyle = DRAWSTYLE_LINE;
		JLine.PrimaryColor = RGB(255, 0, 0);
		JLine.DrawZeros = true;

		FastKLength.Name = "Fast %K Length";
		FastKLength.SetInt(10);
		FastKLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		FastDLength.Name = "Fast %D Length (Slow %K)";
		FastDLength.SetInt(3);
		FastDLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		SlowDLength.Name = "Slow %D Length";
		SlowDLength.SetInt(3);
		SlowDLength.SetIntLimits(1, MAX_STUDY_LENGTH);

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
		StochasticOutput,  // Data member is Fast %K
		FastKLength.GetInt(),
		FastDLength.GetInt(),
		SlowDLength.GetInt(),
		MovAvgType.GetMovAvgType()
	);

	SlowK[sc.Index] = StochasticOutput.Arrays[0][sc.Index];
	SlowD[sc.Index] = StochasticOutput.Arrays[1][sc.Index];
	JLine[sc.Index] = 3 * SlowK[sc.Index] - 2 * SlowD[sc.Index];

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();

	return;
}