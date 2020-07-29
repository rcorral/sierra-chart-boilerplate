#include  <iterator>

#include "sierrachart.h"
#include "scstudyfunctions.h"



/*==========================================================================*/
SCSFExport scsf_MarketStructure(SCStudyInterfaceRef sc)
{
	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;
	SCFloatArrayRef Open = sc.Open;
	SCFloatArrayRef Close = sc.Close;

	SCSubgraphRef MSH = sc.Subgraph[0];
	SCSubgraphRef MSL = sc.Subgraph[1];
	SCSubgraphRef Labels = sc.Subgraph[2];
	SCSubgraphRef BarTypes = sc.Subgraph[3];

	SCInputRef DrawLabels = sc.Input[0];
	SCInputRef LabelsOffset = sc.Input[1];
	SCInputRef DisplayPriceValues = sc.Input[2];
	
	// Set configuration variables
	if (sc.SetDefaults)
	{

		sc.GraphName="Market Structure MSL/MSH";
		

		sc.GraphRegion = 0; // Use the main price graph region

		MSH.Name = "MSH";
		MSH.PrimaryColor = RGB(255,0 ,255); 
		MSH.DrawStyle = DRAWSTYLE_COLOR_BAR;
		MSH.LineWidth = 2;
		MSH.DrawZeros = false;

		MSL.Name = "MSL";
		MSL.PrimaryColor = RGB(255, 128, 0);
		MSL.DrawStyle = DRAWSTYLE_COLOR_BAR;
		MSL.LineWidth = 2;
		MSL.DrawZeros = false;

		Labels.Name = "Labels";
		Labels.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		Labels.PrimaryColor = RGB(255, 0 ,255);
		Labels.SecondaryColor = RGB(255, 128, 0);
		Labels.LineWidth = 10;
		Labels.SecondaryColorUsed = true;
		Labels.DrawZeros = false;

		DrawLabels.Name = "Draw Labels";
		DrawLabels.SetYesNo(true);

		LabelsOffset.Name = "Text Labels Percentage Offset";
		LabelsOffset.SetFloat(5.0f);


		DisplayPriceValues.Name = "Display Price Values";
		DisplayPriceValues.SetYesNo(0);

		sc.AutoLoop				= 1;

		return;
	}

	// define constants
	enum MarketStructureValues
	{
		NONE	= 0,
		HH		= 1,
		LH		= 2,
		LL		= 3,
		HL		= 4
	};

	enum BarTypeValues
	{
		BT_DOWN,
		BT_UP,
		BT_NEUTRAL
	};

	const int UniqueID	= 1546846579;


	if(Close[sc.Index] < Open[sc.Index])
		BarTypes[sc.Index] = BT_DOWN;
	else 
		if(Close[sc.Index] > Open[sc.Index])
			BarTypes[sc.Index] = BT_UP;
		else
			BarTypes[sc.Index] = BT_NEUTRAL;

	// we need at least 3 bars
	if(sc.Index < 2)
		return;

	sc.DataStartIndex = 2;

	// Start calculations here
	int CurrentIndex = sc.Index - 1;

	if(BarTypes[CurrentIndex-2] == BT_DOWN && 
		BarTypes[CurrentIndex-1] == BT_DOWN &&
		BarTypes[CurrentIndex] == BT_UP)
	{
		// these three bars can make MSL
		if(Low[CurrentIndex-2] > Low[CurrentIndex-1] && 
			Low[CurrentIndex] > Low[CurrentIndex-1])
		{
			// this is MSL
			MSL[CurrentIndex-1] = 1;
		}
	}

	if(BarTypes[CurrentIndex-2] == BT_UP && 
		BarTypes[CurrentIndex-1] == BT_UP &&
		BarTypes[CurrentIndex] == BT_DOWN)
	{
		// these three bars can make MSH
		if(High[CurrentIndex-2] < High[CurrentIndex-1] && 
			High[CurrentIndex] < High[CurrentIndex-1])
		{
			// this is MSH
			MSH[CurrentIndex-1] = 1;
		}
	}

	// Fill Labels array
	if(MSH[CurrentIndex-1] == 1)
	{
		// look back through Labels array to find previous label HH or LH
		int Index=CurrentIndex-1;
		for(; Index >= 0; Index--)
		{
			if(Labels[Index] == HH || Labels[Index] == LH)
				break;
		}

		// now Index store index of the element
		if(Index < 0)	// this is a first label, assume HH
			Labels[CurrentIndex-1]	= HH;
		else
		{
			// compare High values
			if(High[Index] == High[CurrentIndex-1])
				Labels[CurrentIndex-1] = Labels[Index];
			else
				if(High[Index] < High[CurrentIndex-1])
					Labels[CurrentIndex-1]	= HH;
				else
					Labels[CurrentIndex-1]	= LH;
		}
	}


	if(MSL[CurrentIndex-1] == 1)
	{
		// look back through Labels array to find previous label LL or HL
		int Index=CurrentIndex-1;
		for(; Index >= 0; Index--)
		{
			if(Labels[Index] == LL || Labels[Index] == HL)
				break;
		}

		// now Index store index of the element
		if(Index < 0)	// this is a first label, assume LL
			Labels[CurrentIndex-1]	= LL;
		else
		{
			// compare Low values
			if(Low[Index] == Low[CurrentIndex-1])
				Labels[CurrentIndex-1] = Labels[Index];
			else
				if(Low[Index] > Low[CurrentIndex-1])
					Labels[CurrentIndex-1]	= LL;
				else
					Labels[CurrentIndex-1]	= HL;
		}
	}


	// check if we need draw labels
	if(!DrawLabels.GetYesNo())
		return;

	// if pattern is not set do not add the empty label
	if(Labels[sc.Index-2] == 0)
		return;


	// Since we are using UTAM_ADD_ALWAYS, we must not attempt to draw labels when we are on the last bar, even if the labels do not appear on the last bar. Otherwise, we get the same label added again and again as the bar is updated. 
	if(sc.GetBarHasClosedStatus(sc.Index) == BHCS_BAR_HAS_NOT_CLOSED)
		return;

	s_UseTool Tool;
	Tool.Clear(); // reset tool structure for our next use
	Tool.ChartNumber	= sc.ChartNumber;
	Tool.DrawingType	= DRAWING_TEXT;
	Tool.Region			= sc.GraphRegion;
	Tool.FontFace = sc.ChartTextFont; 
	Tool.FontBold = true;

	Tool.ReverseTextColor = 0;
	Tool.FontSize = Labels.LineWidth;

	Tool.LineNumber = UniqueID;
	Tool.BeginIndex =sc.Index-2;

	SCString	Label;
	SCString	Value;

	float Offset = LabelsOffset.GetFloat()*0.01f*(sc.High[sc.Index - 2]-sc.Low[sc.Index - 2]);


	switch((int)Labels[sc.Index-2])
	{
	case HH:
		{
			if(!DisplayPriceValues.GetYesNo())
			{
				Label = "HH";
			}
			else
			{
				Value = sc.FormatGraphValue(sc.High[sc.Index-2], sc.BaseGraphValueFormat);
				Label.Format("HH %s", Value.GetChars());
			}
			Tool.Text			= Label;
			Tool.Color			= Labels.PrimaryColor;
			
			Tool.BeginValue		= sc.High[sc.Index-2] + Offset;
			Tool.TextAlignment	= DT_CENTER | DT_BOTTOM;
			break;
		}

	case LH:
		{
			if(!DisplayPriceValues.GetYesNo())
			{
				Label = "LH";
			}
			else
			{
				Value = sc.FormatGraphValue(sc.High[sc.Index-2], sc.BaseGraphValueFormat);
				Label.Format("LH %s", Value.GetChars());
			}
			Tool.Text			= Label;
			Tool.Color			= Labels.PrimaryColor;

			Tool.BeginValue		= sc.High[sc.Index-2] + Offset;
			Tool.TextAlignment	= DT_CENTER | DT_BOTTOM;
			break;
		}

	case LL:
		{
			if(!DisplayPriceValues.GetYesNo())
			{
				Label = "LL";
			}
			else
			{
				Value = sc.FormatGraphValue(sc.Low[sc.Index-2], sc.BaseGraphValueFormat);
				Label.Format("LL %s", Value.GetChars());
			}
			Tool.Text			= Label;
			Tool.Color			= Labels.SecondaryColor;
			
			Tool.BeginValue		= sc.Low[sc.Index-2] - Offset;
			Tool.TextAlignment	= DT_CENTER | DT_TOP;
			break;
		}

	case HL:
		{
			if(!DisplayPriceValues.GetYesNo())
			{
				Label = "HL";
			}
			else
			{
				Value = sc.FormatGraphValue(sc.Low[sc.Index-2], sc.BaseGraphValueFormat);
				Label.Format("HL %s", Value.GetChars());
			}
			Tool.Text			= Label;
			Tool.Color			= Labels.SecondaryColor;
			
			Tool.BeginValue		= sc.Low[sc.Index-2] - Offset;
			Tool.TextAlignment	= DT_CENTER | DT_TOP;
			break;
		}

	default:
		return;
	}

	Tool.AddMethod = UTAM_ADD_ALWAYS;

	sc.UseTool(Tool);
}


/*==========================================================================*/
SCSFExport scsf_WoodiesPanel(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CCIDiff = sc.Subgraph[0];
	SCSubgraphRef ProjectedBuy = sc.Subgraph[1];
	SCSubgraphRef ProjectedSell = sc.Subgraph[2];
	SCSubgraphRef CCIPred = sc.Subgraph[3];
	SCSubgraphRef BackColors = sc.Subgraph[4];
	SCSubgraphRef CCIDifferenceHigh = sc.Subgraph[5];
	SCSubgraphRef CCIDifferenceLow = sc.Subgraph[6];
	SCSubgraphRef HighPC = sc.Subgraph[7];
	SCSubgraphRef LastPC = sc.Subgraph[8];
	SCSubgraphRef LowPC = sc.Subgraph[9];
	SCSubgraphRef EmaAngle = sc.Subgraph[10];

	SCInputRef PanelRangeTopVal = sc.Input[0];
	SCInputRef PanelRangeBottomVal = sc.Input[1];
	SCInputRef ShowLabels = sc.Input[2];
	SCInputRef ColumnsNumber = sc.Input[3];
	SCInputRef CCIStudyRef = sc.Input[4];
	SCInputRef EntryOffset = sc.Input[5];
	SCInputRef TradingStopTime = sc.Input[7];
	SCInputRef CCIPredictorStudyRef = sc.Input[8];
	SCInputRef ShowPreviousClose = sc.Input[9];
	SCInputRef SideWinderStudyRef = sc.Input[11];
	SCInputRef LeftEdgeTextOffset = sc.Input[12];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Woodies Panel";

		sc.AutoLoop = 0;
		sc.GraphRegion = 1;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;

		CCIDiff.Name = "CCIDiff";
		CCIDiff.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		CCIDiff.PrimaryColor = RGB(255,255,255);
		CCIDiff.LineWidth = 10; //font size
		CCIDiff.DrawZeros = false;

		ProjectedBuy.Name = "Projected Buy";
		ProjectedBuy.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		ProjectedBuy.PrimaryColor = RGB(184,251,197);
		ProjectedBuy.LineWidth = 10; //font size
		ProjectedBuy.DrawZeros = false;

		ProjectedSell.Name = "Projected Sell";
		ProjectedSell.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		ProjectedSell.PrimaryColor = RGB(239,204,246);
		ProjectedSell.LineWidth = 10; //font size
		ProjectedSell.DrawZeros = false;

		CCIPred.Name = "CCI Pred. H/L";
		CCIPred.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		CCIPred.PrimaryColor = RGB(0,0,0);
		CCIPred.LineWidth = 10; //font size
		CCIPred.DrawZeros = false;

		BackColors.Name = "Background Colors";
		BackColors.DrawStyle = DRAWSTYLE_IGNORE;
		BackColors.PrimaryColor = RGB(64,128,128);
		BackColors.SecondaryColor = RGB(45,89,89);
		BackColors.SecondaryColorUsed = 1;
		BackColors.DrawZeros = false;

		CCIDifferenceHigh.Name = "CCIDiff H";
		CCIDifferenceHigh.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		CCIDifferenceHigh.PrimaryColor = RGB(0,255,0);
		CCIDifferenceHigh.LineWidth = 10; //font size
		CCIDifferenceHigh.DrawZeros = false;

		CCIDifferenceLow.Name = "CCIDiff L";
		CCIDifferenceLow.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		CCIDifferenceLow.PrimaryColor = RGB(237,80,255);
		CCIDifferenceLow.LineWidth = 10; //font size
		CCIDifferenceLow.DrawZeros = false;

		HighPC.Name = "High Prev/Cur";
		HighPC.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		HighPC.PrimaryColor = RGB(0,0,0);
		HighPC.SecondaryColor = RGB(0,255,0);
		HighPC.SecondaryColorUsed = 1;
		HighPC.LineWidth = 10; //font size
		HighPC.DrawZeros = false;

		LastPC.Name = "Last Prev/Cur";
		LastPC.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		LastPC.PrimaryColor = RGB(0,0,0);
		LastPC.LineWidth = 14; //font size
		LastPC.DrawZeros = false;

		LowPC.Name = "Low Prev/Cur";
		LowPC.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		LowPC.PrimaryColor = RGB(0,0,0);
		LowPC.SecondaryColor = RGB(255,0,0);
		LowPC.SecondaryColorUsed = 1;
		LowPC.LineWidth = 10; //font size
		LowPC.DrawZeros = false;

		EmaAngle.Name = "EMA Angle";
		EmaAngle.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		EmaAngle.PrimaryColor = RGB(0,0,0);
		EmaAngle.LineWidth = 10; //font size
		EmaAngle.DrawZeros = false;

		PanelRangeTopVal.Name = "Panel Range Top Value";
		PanelRangeTopVal.SetInt(250);

		PanelRangeBottomVal.Name = "Panel Range Bottom Value";
		PanelRangeBottomVal.SetInt(-250);

		ShowLabels.Name = "Show Descriptive Labels";
		ShowLabels.SetYesNo(true);

		ColumnsNumber.Name = "Number of Fill Space Columns";
		ColumnsNumber.SetInt(10);

		CCIStudyRef.Name = "CCI Study Reference";
		CCIStudyRef.SetStudyID(1);

		EntryOffset.Name = "Projected Entry Offset in Ticks";
		EntryOffset.SetInt(1);


		TradingStopTime.Name = "Trading Stop Time";
		TradingStopTime.SetTime(HMS_TIME(15, 30, 0));

		CCIPredictorStudyRef.Name = "CCI Predictor Study Reference";
		CCIPredictorStudyRef.SetStudyID(1);

		ShowPreviousClose.Name = "Show Previous Close";
		ShowPreviousClose.SetYesNo(false);

		SideWinderStudyRef.Name = "SideWinder Study Reference";
		SideWinderStudyRef.SetStudyID(1);

		LeftEdgeTextOffset.Name = "Left Edge Text Offset";
		LeftEdgeTextOffset.SetInt(1);
		LeftEdgeTextOffset.SetIntLimits(0,4);

		sc.UpdateAlways = 1;

		return;
	}

	if (sc.LastCallToFunction)
	{

		sc.PreserveFillSpace = 0;
		sc.NumFillSpaceBars = 0;
		sc.UseGlobalChartColors = true;
		sc.ScaleBorderColor = RGB(128,128,128);

		return;
	}

	{
		sc.PreserveFillSpace = 1;
		sc.NumFillSpaceBars = ColumnsNumber.GetInt();	
	}

	int CurrentVisibleIndex = sc.IndexOfLastVisibleBar;

	if (BackColors.PrimaryColor == RGB(255,0,0))
	{
		BackColors.PrimaryColor = RGB(64,128,128);
		BackColors.SecondaryColor = RGB(45,89,89);
	}



	if (sc.GraphRegion >= 2)
		sc.GraphRegion = 1;



	float RangeIncrement = 0.0f;

	//Determine our value increment for the panel data lines based on what lines are visible.
	int NumberOfLines = 0;
	for(int c= 0; c<16; c++)
	{
		if(sc.Subgraph[c].DrawStyle != DRAWSTYLE_IGNORE && sc.Subgraph[c].DrawStyle != DRAWSTYLE_HIDDEN)
			NumberOfLines++;
	}

	RangeIncrement = (PanelRangeTopVal.GetInt() - PanelRangeBottomVal.GetInt())/(NumberOfLines+1.0f);

	float YValue = PanelRangeTopVal.GetInt()-RangeIncrement;

	int TextOffset = -(2+LeftEdgeTextOffset.GetInt());


	s_UseTool Tool;

	if(CCIDifferenceHigh.DrawStyle != DRAWSTYLE_IGNORE && CCIDifferenceHigh.DrawStyle != DRAWSTYLE_HIDDEN)
	{


		SCFloatArray CCIProjHigh;
		sc.GetStudyArrayUsingID(CCIPredictorStudyRef.GetStudyID(),0,CCIProjHigh);


		float CCIDifferenceHighValue = 0;
		SCFloatArray CCI;
		sc.GetStudyArrayUsingID(CCIStudyRef.GetStudyID(),0,CCI);
		CCIDifferenceHighValue = CCIProjHigh[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];


		if (CurrentVisibleIndex != sc.ArraySize-1)
			CCIDifferenceHighValue = CCI[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];

		//Worksheet Output
		CCIDifferenceHigh[sc.UpdateStartIndex] = 0;
		CCIDifferenceHigh[sc.ArraySize - 1] = CCIDifferenceHighValue;

		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5035;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = CCIDifferenceHigh.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = CCIDifferenceHigh.Name;
		}


		Tool.Text.Format(" %.2f %s", CCIDifferenceHighValue, Label.GetChars());
		Tool.FontSize = CCIDifferenceHigh.LineWidth;
		Tool.FontBold = true;
		//Tool.FontFace= "Courier";
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;


		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}


	if(CCIDiff.DrawStyle != DRAWSTYLE_IGNORE && CCIDiff.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		float CCIDiffVal = 0;
		SCFloatArray CCI;
		sc.GetStudyArrayUsingID(CCIStudyRef.GetStudyID(),0,CCI);
		CCIDiffVal = CCI[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];

		CCIDiff[sc.UpdateStartIndex] = 0;
		CCIDiff[sc.ArraySize - 1] = CCIDiffVal;

		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5030;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = CCIDiff.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = "CCIDiff";
		}

		Tool.Text.Format(" %.2f %s", CCIDiffVal , Label.GetChars());
		Tool.FontSize = CCIDiff.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}


	if(CCIDifferenceLow.DrawStyle != DRAWSTYLE_IGNORE && CCIDifferenceLow.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		SCFloatArray CCIProjLow;
		sc.GetStudyArrayUsingID(CCIPredictorStudyRef.GetStudyID(),1,CCIProjLow);


		float CCIDifferenceLowValue = 0;
		SCFloatArray CCI;
		sc.GetStudyArrayUsingID(CCIStudyRef.GetStudyID(),0,CCI);
		CCIDifferenceLowValue = CCIProjLow[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];

		if (CurrentVisibleIndex != sc.ArraySize-1)
			CCIDifferenceLowValue = CCI[CurrentVisibleIndex] - CCI[CurrentVisibleIndex-1];

		CCIDifferenceLow[sc.UpdateStartIndex] = 0;
		CCIDifferenceLow[sc.ArraySize - 1] = CCIDifferenceLowValue;

		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5036;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = CCIDifferenceLow.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = CCIDifferenceLow.Name;
		}

		Tool.Text.Format(" %.2f %s", CCIDifferenceLowValue, Label.GetChars());
		Tool.FontSize = CCIDifferenceLow.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}

	if(HighPC.DrawStyle != DRAWSTYLE_IGNORE && HighPC.DrawStyle != DRAWSTYLE_HIDDEN)
	{	
		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5037;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		if (sc.High[CurrentVisibleIndex] > sc.High[CurrentVisibleIndex-1])
		{
			Tool.Color = HighPC.SecondaryColor;
			Tool.ReverseTextColor = 0;
		}
		else
		{
			Tool.Color = HighPC.PrimaryColor;
			Tool.ReverseTextColor = 0;
		}

		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = HighPC.Name;
		}

		SCString CurrentHigh = sc.FormatGraphValue(sc.High[CurrentVisibleIndex], sc.BaseGraphValueFormat);
		SCString PrevHigh = sc.FormatGraphValue(sc.High[CurrentVisibleIndex-1], sc.BaseGraphValueFormat);
		Tool.Text.Format(" %s %s %s", PrevHigh.GetChars(), CurrentHigh.GetChars(), Label.GetChars());
		Tool.FontSize = HighPC.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;


		sc.UseTool(Tool);
		YValue -= RangeIncrement;
		//Worksheet Output
		HighPC[sc.UpdateStartIndex] = 0;
		HighPC[sc.UpdateStartIndex-1] = 0;
		HighPC[sc.ArraySize - 1] = sc.High[CurrentVisibleIndex];
		HighPC[sc.ArraySize - 2] = sc.High[CurrentVisibleIndex-1];
	}

	if(LastPC.DrawStyle != DRAWSTYLE_IGNORE && LastPC.DrawStyle != DRAWSTYLE_HIDDEN)
	{	
		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5038;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;


		{
			Tool.Color = LastPC.PrimaryColor;
			Tool.ReverseTextColor = 0;
		}


		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			if(ShowPreviousClose.GetYesNo())
				Label = LastPC.Name;
			else
				Label = "Last Price";
		}

		SCString CurLast = sc.FormatGraphValue(sc.Close[CurrentVisibleIndex], sc.BaseGraphValueFormat);
		SCString PrevLast = sc.FormatGraphValue(sc.Close[CurrentVisibleIndex-1], sc.BaseGraphValueFormat);

		//Worksheet Output
		LastPC[sc.UpdateStartIndex] = 0;
		LastPC[sc.UpdateStartIndex-1] = 0;
		LastPC[sc.ArraySize - 1] = sc.Close[CurrentVisibleIndex];
		LastPC[sc.ArraySize - 2] = sc.Close[CurrentVisibleIndex-1];

		if (ShowPreviousClose.GetYesNo())
		{
			Tool.Text.Format(" %s %s %s", PrevLast.GetChars(), CurLast.GetChars(), Label.GetChars());
		}
		else
		{
			Tool.Text.Format(" %s %s", CurLast.GetChars(), Label.GetChars());
		}

		Tool.FontSize = LastPC.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;



		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}

	if(LowPC.DrawStyle != DRAWSTYLE_IGNORE && LowPC.DrawStyle != DRAWSTYLE_HIDDEN)
	{	
		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5039;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		if (sc.Low[CurrentVisibleIndex] < sc.Low[CurrentVisibleIndex-1])
		{
			Tool.Color = LowPC.SecondaryColor;
			Tool.ReverseTextColor = 0;
		}
		else
		{
			Tool.Color = LowPC.PrimaryColor;
			Tool.ReverseTextColor = 0;
		}

		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = LowPC.Name;
		}

		SCString CurLow = sc.FormatGraphValue(sc.Low[CurrentVisibleIndex], sc.BaseGraphValueFormat);
		SCString PrevLow = sc.FormatGraphValue(sc.Low[CurrentVisibleIndex-1], sc.BaseGraphValueFormat);

		//Worksheet Output
		LowPC[sc.UpdateStartIndex] = 0;
		LowPC[sc.UpdateStartIndex-1] = 0;
		LowPC[sc.ArraySize - 1] = sc.Low[CurrentVisibleIndex];
		LowPC[sc.ArraySize - 2] = sc.Low[CurrentVisibleIndex-1];

		Tool.Text.Format(" %s %s %s",PrevLow.GetChars(),CurLow.GetChars(),Label.GetChars());
		Tool.FontSize = LowPC.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;


		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}

	if(ProjectedBuy.DrawStyle != DRAWSTYLE_IGNORE && ProjectedBuy.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		float ProjHigh = 0;

		ProjHigh = sc.Low[CurrentVisibleIndex]+sc.RangeBarValue+(EntryOffset.GetInt()*sc.TickSize);

		//Worksheet Output
		ProjectedBuy[sc.UpdateStartIndex] = 0;
		ProjectedBuy[sc.ArraySize - 1] = ProjHigh;

		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5031;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = ProjectedBuy.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = "ProjHigh";
		}

		SCString StrValue = sc.FormatGraphValue(ProjHigh, sc.BaseGraphValueFormat);
		Tool.Text.Format(" %s %s", StrValue.GetChars(), Label.GetChars());
		Tool.FontSize = ProjectedBuy.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;

	}

	if(ProjectedSell.DrawStyle != DRAWSTYLE_IGNORE && ProjectedSell.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		float ProjLow = 0;

		ProjLow = sc.High[CurrentVisibleIndex]-(sc.RangeBarValue+(EntryOffset.GetInt()*sc.TickSize));

		//Worksheet Output
		ProjectedSell[sc.UpdateStartIndex] = 0;
		ProjectedSell[sc.ArraySize - 1] = ProjLow;


		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5032;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = ProjectedSell.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = "ProjLow";
		}

		SCString StrValue = sc.FormatGraphValue(ProjLow, sc.BaseGraphValueFormat);
		Tool.Text.Format(" %s %s", StrValue.GetChars(), Label.GetChars());
		Tool.FontSize = ProjectedSell.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;
	}

	if(CCIPred.DrawStyle != DRAWSTYLE_IGNORE && CCIPred.DrawStyle != DRAWSTYLE_HIDDEN)
	{

		SCFloatArray CCIProjHigh;
		SCFloatArray CCIProjLow;

		sc.GetStudyArrayUsingID(CCIPredictorStudyRef.GetStudyID(),0,CCIProjHigh);
		sc.GetStudyArrayUsingID(CCIPredictorStudyRef.GetStudyID(),1,CCIProjLow);



		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5033;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = CCIPred.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = CCIPred.Name;
		}

		float ProjHigh = CCIProjHigh[sc.ArraySize-1];
		float ProjLow = CCIProjLow[sc.ArraySize-1];

		if (CurrentVisibleIndex != sc.ArraySize-1)
		{
			SCFloatArray CCI;
			sc.GetStudyArrayUsingID(CCIStudyRef.GetStudyID(),0,CCI);
			ProjLow = ProjHigh = CCI[CurrentVisibleIndex];
		}

		Tool.Text.Format(" %.1f  %.1f %s",ProjHigh,ProjLow,Label.GetChars());
		Tool.FontSize = CCIPred.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;

		//Worksheet Output
		CCIPred[sc.UpdateStartIndex] = 0;
		CCIPred[sc.UpdateStartIndex-1] = 0;
		CCIPred[sc.ArraySize - 1] = ProjHigh;
		CCIPred[sc.ArraySize - 2] = ProjLow;
	}



	if(LowPC.DrawStyle != DRAWSTYLE_IGNORE && LowPC.DrawStyle != DRAWSTYLE_HIDDEN)
	{
		SCFloatArray EMAAngle;
		sc.GetStudyArrayUsingID(SideWinderStudyRef.GetStudyID(),7,EMAAngle);


		Tool.Clear(); // reset tool structure for our next use
		Tool.ChartNumber = sc.ChartNumber;
		Tool.DrawingType = DRAWING_TEXT;
		Tool.LineNumber = 5040;
		Tool.BeginDateTime = TextOffset;
		Tool.BeginValue = YValue;
		Tool.Color = LowPC.PrimaryColor;
		Tool.Region = sc.GraphRegion;

		SCString Label;

		if (ShowLabels.GetYesNo())
		{
			Label = LowPC.Name;
		}


		Tool.Text.Format(" %.1f\u00B0 %s", EMAAngle[CurrentVisibleIndex], Label.GetChars());
		Tool.FontSize = LowPC.LineWidth;
		Tool.FontBold = true;
		Tool.AddMethod = UTAM_ADD_OR_ADJUST;
		Tool.ReverseTextColor = 0;

		sc.UseTool(Tool);
		YValue -= RangeIncrement;

		//Worksheet Output
		LowPC[sc.UpdateStartIndex] = 0;
		LowPC[sc.ArraySize - 1] = EMAAngle[CurrentVisibleIndex];

	}

}

/*==========================================================================*/
void AccumulateZigZagVolume(SCStudyInterfaceRef sc, SCSubgraphRef AccumulatedVolume, SCFloatArrayRef ZigZagPeakType, int VolumeTypeToAccumulate, SCFloatArrayRef ZigZagReset)
{
	if (VolumeTypeToAccumulate == 0)
		return;

	//Accumulate the volume across the last two zigzags

	//Determine starting point
	int Count = 0;//To count the number of zigzag lines
	int StartingIndex = sc.UpdateStartIndex+1;
	while (StartingIndex > 0 && Count < 2)
	{
		StartingIndex--;
		if (ZigZagPeakType[StartingIndex] != 0.0f)
			Count ++;
	}

	double Volume = 0;
	int Direction = 0;

	for (int Index = StartingIndex+1; Index < sc.ArraySize; ++Index)
	{
		if(ZigZagPeakType[Index-1] != 0)
		{
			Volume = 0;
			Direction = (int)ZigZagPeakType[Index-1];
		}

		if (ZigZagReset[Index-1] != 0) // First index, pick up initial volume
		{
			if (VolumeTypeToAccumulate == 1)
				Volume = sc.Volume[Index-1];
			else if (VolumeTypeToAccumulate == 2)
				Volume = sc.BidVolume[Index-1];
			else if (VolumeTypeToAccumulate == 3)
				Volume = sc.AskVolume[Index-1];
			else if (VolumeTypeToAccumulate == 4)
				Volume = sc.AskVolume[Index-1] - sc.BidVolume[Index-1];
			AccumulatedVolume[Index-1] = (float)Volume;
			Direction = (int)ZigZagPeakType[Index-1];
		}

		if (Direction == 1)
			AccumulatedVolume.DataColor[Index]= AccumulatedVolume.PrimaryColor;
		else if (Direction == -1)
			AccumulatedVolume.DataColor[Index]= AccumulatedVolume.SecondaryColor;

		if (VolumeTypeToAccumulate == 1)
			Volume += sc.Volume[Index];
		else if (VolumeTypeToAccumulate == 2)
			Volume += sc.BidVolume[Index];
		else if (VolumeTypeToAccumulate == 3)
			Volume += sc.AskVolume[Index];
		else if (VolumeTypeToAccumulate == 4)
			Volume += sc.AskVolume[Index] - sc.BidVolume[Index];

		AccumulatedVolume[Index] = (float)Volume;
	}
}

void CreateZigZagLineLabel(SCString &BarLabelText, SCFloatArrayRef ZigZagPeakType, int ZigZagIndex, SCFloatArrayRef ZigZagPeakIndex, SCInputRef ShowRevPrice, SCInputRef UseMultiLineLabels, SCStudyInterfaceRef sc, SCSubgraphRef ZigZagLine, SCInputRef ShowLength, bool ShowLabelPrefixes, SCInputRef ShowTime, SCInputRef IncludeSecondsInTimeDisplay, SCInputRef ShowZigZagVolume, SCSubgraphRef AccumulatedVolume, SCInputRef FormatVolumeWithLargeNumberSuffix, SCInputRef ShowZigZagDuration, SCInputRef ShowZigZagNumBars, SCInputRef DisplayZigZagVolumeDividedByLengthInTicks, SCFloatArrayRef ZigZagReset, SCInputRef DisplayHHHLLLLHLabels, SCSubgraphRef TextLabels, double& ZigzagLineLength, int& CurrentZigZagNumBars)
{


	int BackRefIndex = (int)ZigZagPeakIndex[ZigZagIndex - 1];

	if (ZigZagPeakType[BackRefIndex] == -1 * ZigZagPeakType[ZigZagIndex])
	{
		if (ShowRevPrice.GetYesNo())
		{
			if (BarLabelText.GetLength() != 0)
			{
				if (UseMultiLineLabels.GetYesNo())
					BarLabelText += "\n";
				else
					BarLabelText += " ";
			}

			SCString ReversalPrice;
			ReversalPrice = sc.FormatGraphValue(ZigZagLine[ZigZagIndex], sc.GetValueFormat());
			BarLabelText += ReversalPrice;
		}

		ZigzagLineLength = ZigZagLine[ZigZagIndex] - ZigZagLine[BackRefIndex];
		if (ShowLength.GetIndex() != 0)
		{
			if (BarLabelText.GetLength() != 0)
			{
				if (UseMultiLineLabels.GetYesNo())
					BarLabelText += "\n";
				else
					BarLabelText += " ";
			}

			SCString LengthStr;
			if (ShowLabelPrefixes)
				LengthStr = "L:";

			if (ShowLength.GetIndex() == 1)
				LengthStr += sc.FormatGraphValue(ZigzagLineLength, sc.GetValueFormat());
			else
			{
				double Ticks = sc.TickSize == 0 ? 0 : ZigzagLineLength / sc.TickSize;
				LengthStr += sc.FormatGraphValue(Ticks, 0);
				LengthStr += "T";
			}

			BarLabelText += LengthStr;
		}

		if (ShowTime.GetYesNo())
		{
			if (BarLabelText.GetLength() != 0)
			{
				if (UseMultiLineLabels.GetYesNo())
					BarLabelText += "\n";
				else
					BarLabelText += " ";
			}

			SCDateTime DT = sc.BaseDateTimeIn[ZigZagIndex];
			SCString TimeStr;
			if (IncludeSecondsInTimeDisplay.GetYesNo())
				TimeStr.Format("%i:%02i:%02i", DT.GetHour(), DT.GetMinute(), DT.GetSecond());
			else
				TimeStr.Format("%i:%02i", DT.GetHour(), DT.GetMinute());

			BarLabelText += TimeStr;
		}

		if (ShowZigZagVolume.GetYesNo())
		{
			if (BarLabelText.GetLength() != 0)
			{
				if (UseMultiLineLabels.GetYesNo())
					BarLabelText += "\n";
				else
					BarLabelText += " ";
			}

			SCString LengthStr;
			if (ShowLabelPrefixes)
				LengthStr = "V:";

			LengthStr += sc.FormatVolumeValue((int64_t)AccumulatedVolume[ZigZagIndex], 0, FormatVolumeWithLargeNumberSuffix.GetYesNo() != 0);

			BarLabelText += LengthStr;
		}

		if (ShowZigZagDuration.GetYesNo())
		{
			if (BarLabelText.GetLength() != 0)
			{
				if (UseMultiLineLabels.GetYesNo())
					BarLabelText += "\n";
				else
					BarLabelText += " ";
			}

			double ZigZagTimeSpan = 0;
			if (sc.ChartDataType == INTRADAY_DATA)
			{
				if (ZigZagIndex < sc.ArraySize - 1)
					ZigZagTimeSpan = sc.BaseDateTimeIn[ZigZagIndex + 1] - sc.BaseDateTimeIn[BackRefIndex + 1];
				else
					ZigZagTimeSpan = sc.LatestDateTimeForLastBar - sc.BaseDateTimeIn[BackRefIndex + 1];
			}
			else
			{
				ZigZagTimeSpan = sc.BaseDateTimeIn[ZigZagIndex] - sc.BaseDateTimeIn[BackRefIndex + 1];
			}

			SCString TimeDurationStr;
			if (sc.ChartDataType == INTRADAY_DATA)
			{
				TimeDurationStr = sc.FormatGraphValue(ZigZagTimeSpan, 20);

				if (TimeDurationStr.GetLength() >= 3 && TimeDurationStr[0] == '0' && TimeDurationStr[1] == '0' && TimeDurationStr[2] == ':')
					TimeDurationStr = TimeDurationStr.GetSubString(TimeDurationStr.GetLength() - 3, 3);
			}

			SCString LengthStr;
			if (ShowLabelPrefixes)
				LengthStr = "D:";
			if (ZigZagTimeSpan >= 1.0)
			{
				SCString DaysStr;
				if (sc.ChartDataType == INTRADAY_DATA)
					DaysStr.Format("%iD ", (int)ZigZagTimeSpan);
				else
					DaysStr.Format("%i ", (int)ZigZagTimeSpan);

				LengthStr += DaysStr;
			}
			LengthStr += TimeDurationStr;

			BarLabelText += LengthStr;
		}

		CurrentZigZagNumBars = ZigZagIndex - BackRefIndex;

		if (ShowZigZagNumBars.GetYesNo())
		{
			if (BarLabelText.GetLength() != 0)
			{
				if (UseMultiLineLabels.GetYesNo())
					BarLabelText += "\n";
				else
					BarLabelText += " ";
			}

			SCString LengthStr;
			if (ShowLabelPrefixes)
				LengthStr = "B:";

			SCString NumStr;
			NumStr.Format("%i ", CurrentZigZagNumBars);
			LengthStr += NumStr;

			BarLabelText += LengthStr;
		}

		if (DisplayZigZagVolumeDividedByLengthInTicks.GetYesNo())
		{
			if (BarLabelText.GetLength() != 0)
			{
				if (UseMultiLineLabels.GetYesNo())
					BarLabelText += "\n";
				else
					BarLabelText += " ";
			}

			if (ShowLabelPrefixes)
				BarLabelText += "V/L:";

			int64_t VolumeDividedByLengthInTicks = int64_t(AccumulatedVolume[ZigZagIndex] / (ZigzagLineLength/ sc.TickSize));

			BarLabelText += sc.FormatVolumeValue(VolumeDividedByLengthInTicks, 0, FormatVolumeWithLargeNumberSuffix.GetYesNo() != 0);

		}
	}

	// now go back one more swing
	if (ZigZagReset[BackRefIndex] == 0)
	{
		BackRefIndex = (int)ZigZagPeakIndex[BackRefIndex - 1];

		if (ZigZagPeakType[BackRefIndex] == ZigZagPeakType[ZigZagIndex])
		{
			if (DisplayHHHLLLLHLabels.GetYesNo())
			{
				if (BarLabelText.GetLength() != 0)
				{
					if (UseMultiLineLabels.GetYesNo())
						BarLabelText += "\n";
					else
						BarLabelText += " ";
				}

				if (ZigZagPeakType[BackRefIndex] == 1.0f)//High
				{
					if (ZigZagLine[BackRefIndex] > ZigZagLine[ZigZagIndex]) //LowerHigh
					{
						BarLabelText += "LH";
						TextLabels.Data[ZigZagIndex] = 1;
					}
					else //HigherHigh
					{
						BarLabelText += "HH";
						TextLabels.Data[ZigZagIndex] = 2;
					}
				}
				else //Low
				{
					if (ZigZagLine[BackRefIndex] < ZigZagLine[ZigZagIndex]) //HigherLow
					{
						BarLabelText += "HL";
						TextLabels.Data[ZigZagIndex] = 3;
					}
					else //LowerLow
					{
						BarLabelText += "LL";
						TextLabels.Data[ZigZagIndex] = 4;
					}
				}
			}
		}
	}
	else
	{
		if (ZigZagLine[BackRefIndex] > ZigZagLine[ZigZagIndex])
			TextLabels.Data[ZigZagIndex] = 11;  // first pivot low
		else
			TextLabels.Data[ZigZagIndex] = 12;  // first pivot high
	}
}

/*==========================================================================*/
SCSFExport scsf_ZigZagStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ZigZagLine        = sc.Subgraph[0];
	SCSubgraphRef TextLabels        = sc.Subgraph[1];
	SCSubgraphRef ReversalPrice     = sc.Subgraph[2];
	SCSubgraphRef ZigzagLength      = sc.Subgraph[3];
	SCSubgraphRef AccumulatedVolume = sc.Subgraph[4];
	SCSubgraphRef ZigzagNumBars     = sc.Subgraph[5];
	SCSubgraphRef ZigzagMidPoint    = sc.Subgraph[6];
	SCSubgraphRef ExtensionLinesSubgraph = sc.Subgraph[7];
	SCSubgraphRef ZigZagOscillator = sc.Subgraph[8];
	SCSubgraphRef ZigZagLineDuration = sc.Subgraph[9];

	SCInputRef VolumeToAccumulate = sc.Input[0];
	SCInputRef LabelOffsetType = sc.Input[1];
	SCInputRef FormatVolumeWithLargeNumberSuffix = sc.Input[2];
	SCInputRef ColorZigZagLine = sc.Input[3];
	SCInputRef VersionUpdate = sc.Input[14];
	SCInputRef InputDataHigh = sc.Input[15];
	SCInputRef InputDataLow = sc.Input[16];
	SCInputRef CalculationMode = sc.Input[17];
	SCInputRef ReversalPercent = sc.Input[18];
	SCInputRef ReversalAmount = sc.Input[19];
	SCInputRef NumberBarsForReversal = sc.Input[20];
	SCInputRef ShowRevPrice = sc.Input[21];
	SCInputRef DisplayHHHLLLLHLabels = sc.Input[22];
	SCInputRef LabelsOffset = sc.Input[23];
	SCInputRef AdditionalOutputForSpreadsheets = sc.Input[24];
	SCInputRef ShowTime = sc.Input[25];
	SCInputRef ShowLength = sc.Input[26];
	SCInputRef CalcOnBarClose = sc.Input[27];
	SCInputRef CalculateZigZagVolume_UpgradeOnly = sc.Input[28];
	SCInputRef ShowZigZagVolume = sc.Input[29];
	SCInputRef ShowZigZagDuration = sc.Input[30];
	SCInputRef UseMultiLineLabels = sc.Input[31];
	SCInputRef OmitLabelPrefixes = sc.Input[32];
	SCInputRef ShowZigZagNumBars = sc.Input[33];
	SCInputRef ResetAtStartOfSession = sc.Input[34];
	SCInputRef DisplayZigZagVolumeDividedByLengthInTicks = sc.Input[35];
	SCInputRef ExtendEndPointsUntilFutureIntersection = sc.Input[36];
	SCInputRef IncludeSecondsInTimeDisplay = sc.Input[37];
	SCInputRef DisplayValueLabel = sc.Input[38];

	if(sc.SetDefaults)
	{
		sc.GraphName="Zig Zag";

		//Manual looping
		sc.AutoLoop = 0;

		sc.GraphRegion = 0;

		sc.ValueFormat = VALUEFORMAT_INHERITED;

		ZigZagLine.Name = "Zig Zag";
		ZigZagLine.DrawStyle = DRAWSTYLE_LINE;
		ZigZagLine.LineStyle = LINESTYLE_DOT;
		ZigZagLine.PrimaryColor = RGB(255,0,255);
		ZigZagLine.SecondaryColorUsed = 1;
		ZigZagLine.SecondaryColor = RGB(255,0,0);
		ZigZagLine.LineWidth = 3;
		ZigZagLine.DrawZeros = false;

		TextLabels.Name = "Text Labels";
		TextLabels.SecondaryColorUsed = 1;
		TextLabels.PrimaryColor = RGB(0,255,0);
		TextLabels.SecondaryColor = RGB(255,0,0);
		TextLabels.LineWidth = 10;
		TextLabels.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		
		ReversalPrice.DrawStyle = DRAWSTYLE_IGNORE;
		ZigzagLength.DrawStyle = DRAWSTYLE_IGNORE;
		ZigzagNumBars.DrawStyle = DRAWSTYLE_IGNORE;
		AccumulatedVolume.DrawStyle = DRAWSTYLE_IGNORE;
		AccumulatedVolume.SecondaryColorUsed = 1;

		ZigzagMidPoint.Name = "Zig Zag Mid-Point";
		ZigzagMidPoint.DrawStyle = DRAWSTYLE_IGNORE;
		ZigzagMidPoint.PrimaryColor = RGB(128,128,255);
		ZigzagMidPoint.LineWidth = 1;
		ZigzagMidPoint.DrawZeros = false;

		ExtensionLinesSubgraph.Name = "Extension Lines";
		ExtensionLinesSubgraph.DrawStyle = DRAWSTYLE_LINE;
		ExtensionLinesSubgraph.PrimaryColor = RGB(128, 128, 255);
		ExtensionLinesSubgraph.SecondaryColorUsed = true;
		ExtensionLinesSubgraph.SecondaryColor = RGB(128, 0, 255);
		ExtensionLinesSubgraph.LineWidth = 1;
		ExtensionLinesSubgraph.DrawZeros = false;

		ZigZagOscillator.Name = "Zig Zag Oscillator";
		ZigZagOscillator.DrawStyle = DRAWSTYLE_IGNORE;
		ZigZagOscillator.PrimaryColor = RGB(0, 200, 0);
		ZigZagOscillator.SecondaryColor = RGB(200, 0, 0);
		ZigZagOscillator.AutoColoring = AUTOCOLOR_POSNEG;
		ZigZagOscillator.LineWidth = 3;
		ZigZagOscillator.DrawZeros = false;

		VersionUpdate.SetInt(3);

		unsigned short DisplayOrder = 1;

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);
		InputDataHigh.DisplayOrder = DisplayOrder++;

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);
		InputDataLow.DisplayOrder = DisplayOrder++;

		CalculationMode.Name = "Calculation Mode (1,2,3)";
		CalculationMode.SetInt(1);
		CalculationMode.SetIntLimits(1, 3);
		CalculationMode.DisplayOrder = DisplayOrder++;

		ReversalPercent.Name = "Reversal % for Calculation Mode 1";
		ReversalPercent.SetFloat(.5f);
		ReversalPercent.DisplayOrder = DisplayOrder++;

		ReversalAmount.Name = "Reversal Amount for Calculation Mode 2,3";
		ReversalAmount.SetFloat(0.01f);
		ReversalAmount.DisplayOrder = DisplayOrder++;

		NumberBarsForReversal.Name = "Number of Bars Required for Reversal (Calculation Mode 2)";
		NumberBarsForReversal.SetInt(5);
		NumberBarsForReversal.DisplayOrder = DisplayOrder++;

		VolumeToAccumulate.Name = "Volume To Accumulate";
		VolumeToAccumulate.SetCustomInputStrings("None;Total Volume;Bid Volume;Ask Volume;Ask Bid Volume Difference");
		VolumeToAccumulate.SetCustomInputIndex(0);
		VolumeToAccumulate.DisplayOrder = DisplayOrder++;

		ResetAtStartOfSession.Name = "Reset ZigZag At Start Of Trading Day";
		ResetAtStartOfSession.SetYesNo(0);
		ResetAtStartOfSession.DisplayOrder = DisplayOrder++;

		CalcOnBarClose.Name = "Calculate New Values On Bar Close";
		CalcOnBarClose.SetYesNo(0);
		CalcOnBarClose.DisplayOrder = DisplayOrder++;

		AdditionalOutputForSpreadsheets.Name = "Additional Output for Spreadsheets";
		AdditionalOutputForSpreadsheets.SetYesNo(false);
		AdditionalOutputForSpreadsheets.DisplayOrder = DisplayOrder++;

		DisplayHHHLLLLHLabels.Name = "Display HH,HL,LL,LH Labels";
		DisplayHHHLLLLHLabels.SetYesNo(0);
		DisplayHHHLLLLHLabels.DisplayOrder = DisplayOrder++;

		ShowRevPrice.Name = "Display Reversal Price";
		ShowRevPrice.SetYesNo(0);
		ShowRevPrice.DisplayOrder = DisplayOrder++;

		ShowLength.Name = "Display Length of Zig Zag Line";
		ShowLength.SetCustomInputStrings("None;Points;Ticks");
		ShowLength.SetCustomInputIndex(0);
		ShowLength.DisplayOrder = DisplayOrder++;

		ShowZigZagVolume.Name = "Display ZigZag Volume";
		ShowZigZagVolume.SetYesNo(0);
		ShowZigZagVolume.DisplayOrder = DisplayOrder++;

		FormatVolumeWithLargeNumberSuffix.Name = "Format Volume Using Large Number Suffix (K/M)";
		FormatVolumeWithLargeNumberSuffix.SetYesNo(0);
		FormatVolumeWithLargeNumberSuffix.DisplayOrder = DisplayOrder++;

		ShowTime.Name = "Display Time Labels";
		ShowTime.SetYesNo(0);
		ShowTime.DisplayOrder = DisplayOrder++;

		IncludeSecondsInTimeDisplay.Name = "Include Seconds in Time Display";
		IncludeSecondsInTimeDisplay.SetYesNo(1);
		IncludeSecondsInTimeDisplay.DisplayOrder = DisplayOrder++;

		ShowZigZagDuration.Name = "Display ZigZag Time Duration";
		ShowZigZagDuration.SetYesNo(0);
		ShowZigZagDuration.DisplayOrder = DisplayOrder++;

		ShowZigZagNumBars.Name = "Display ZigZag Number Of Bars";
		ShowZigZagNumBars.SetYesNo(0);
		ShowZigZagNumBars.DisplayOrder = DisplayOrder++;

		DisplayZigZagVolumeDividedByLengthInTicks.Name = "Display ZigZag Volume / Length in Ticks";
		DisplayZigZagVolumeDividedByLengthInTicks.SetYesNo(0);
		DisplayZigZagVolumeDividedByLengthInTicks.DisplayOrder = DisplayOrder++;

		UseMultiLineLabels.Name = "Use Multi-line Labels";
		UseMultiLineLabels.SetYesNo(0);
		UseMultiLineLabels.DisplayOrder = DisplayOrder++;

		OmitLabelPrefixes.Name = "Omit Label Prefixes";
		OmitLabelPrefixes.SetYesNo(0);
		OmitLabelPrefixes.DisplayOrder = DisplayOrder++;

		LabelOffsetType.Name = "Text Label Offset Specified As";
		LabelOffsetType.SetCustomInputStrings("Percentage Of Bar;Tick Offset");
		LabelOffsetType.SetCustomInputIndex(1);
		LabelOffsetType.DisplayOrder = DisplayOrder++;

		LabelsOffset.Name = "Text Label Offset";
		LabelsOffset.SetFloat(1.0f);
		LabelsOffset.DisplayOrder = DisplayOrder++;

		ColorZigZagLine.Name = "Color ZigZag Line Using";
		ColorZigZagLine.SetCustomInputStrings("None;Slope;Trend Confirmed Volume;Confirmed Volume");
		ColorZigZagLine.SetCustomInputIndex(0);
		ColorZigZagLine.DisplayOrder = DisplayOrder++;

		ExtendEndPointsUntilFutureIntersection.Name = "Extend End Points Until Future Intersection";
		ExtendEndPointsUntilFutureIntersection.SetYesNo(false);
		ExtendEndPointsUntilFutureIntersection.DisplayOrder = DisplayOrder++;

		DisplayValueLabel.Name = "Display Value Label for Extension Lines";
		DisplayValueLabel.SetYesNo(false);
		DisplayValueLabel.DisplayOrder = DisplayOrder++;

		return;
	}

	SCFloatArrayRef ZigZagPeakType  = ZigZagLine.Arrays[0];  // ZigZagPeakType : +1=high peak, -1=low peak, 0=not peak
	SCFloatArrayRef ZigZagPeakIndex = ZigZagLine.Arrays[1];  // ZigZagPeakIndex: index of current peak, -1=no peak yet
	SCFloatArrayRef ZigZagReset     = ZigZagLine.Arrays[2];  // ZigZagReset: 0=no reset, 1=reset
	SCFloatArrayRef ZigZagTrend     = ZigZagLine.Arrays[3];  // ZigZagTrend: 0=none, 1=confirmed up, -1=confirmed down

	int& LastCalcIndex	          = sc.GetPersistentInt(1);
	int& PriorLastZZEndpointIndex = sc.GetPersistentInt(2);
	int& ZigZagStartIndex         = sc.GetPersistentInt(3);
	int& LastUsedLabelDrawingNumber		  = sc.GetPersistentInt(4);
	int& LastUsedBarIndexForExtensionLine = sc.GetPersistentInt(5);

	if(sc.UpdateStartIndex == 0)
	{
		LastCalcIndex = -1;
		LastUsedLabelDrawingNumber = -1;
		LastUsedBarIndexForExtensionLine = -1;
		PriorLastZZEndpointIndex = -1;
		ZigZagStartIndex = 0;
		ZigZagReset[0] = 1.0f;
		TextLabels[0] = 10;  // first point

		if (VersionUpdate.GetInt() < 2)
		{
			ResetAtStartOfSession.SetYesNo(0);
			VersionUpdate.SetInt(2);
		}
		
		if (VersionUpdate.GetInt() < 3)
		{
			LabelOffsetType.SetCustomInputIndex(0);
			FormatVolumeWithLargeNumberSuffix.SetYesNo(0);
			ZigZagLine.SecondaryColorUsed = 1;
			ZigZagLine.SecondaryColor = RGB(255,0,0);
			ColorZigZagLine.SetCustomInputIndex(0);
			VolumeToAccumulate.SetCustomInputIndex(VolumeToAccumulate.GetIndex()+1);
			if (CalculateZigZagVolume_UpgradeOnly.GetYesNo() == 0)
				VolumeToAccumulate.SetCustomInputIndex(0);

			VersionUpdate.SetInt(3);
		}

		if (AdditionalOutputForSpreadsheets.GetYesNo())
		{
			ReversalPrice.Name = "Reversal Price";
			ZigzagLength.Name = "Zig Zag Line Length";
			ZigzagNumBars.Name = "Zig Zag Num Bars";
		}
		else
		{
			ReversalPrice.Name = "";
			ZigzagLength.Name = "";
			ZigzagNumBars.Name = "";
		}

		if (ShowZigZagVolume.GetYesNo() && VolumeToAccumulate.GetIndex() == 0)
			VolumeToAccumulate.SetCustomInputIndex(1);

		if(VolumeToAccumulate.GetIndex() != 0)
			AccumulatedVolume.Name= "AccumulatedVolume";
		else
			AccumulatedVolume.Name= "";
	}
	else
	{
		if (LastUsedLabelDrawingNumber != -1)
		{
			sc.DeleteACSChartDrawing(sc.ChartNumber, TOOL_DELETE_CHARTDRAWING, LastUsedLabelDrawingNumber);

			LastUsedLabelDrawingNumber = -1;
		}

		if (LastUsedBarIndexForExtensionLine != -1)
		{
			sc.DeleteLineUntilFutureIntersection(LastUsedBarIndexForExtensionLine, 0);

			LastUsedBarIndexForExtensionLine = -1;
		}

	}
	

	bool ShowLabelPrefixes = OmitLabelPrefixes.GetYesNo() == 0;

	SCDateTime NextSessionStart = 0;
	if (sc.UpdateStartIndex > 0)
		NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1*DAYS;

	sc.EarliestUpdateSubgraphDataArrayIndex = sc.UpdateStartIndex;

	// Standard Manual Looping loop
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; ++Index)
	{
		if (ResetAtStartOfSession.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];
		
			if (IndexDateTime >= NextSessionStart)
			{
				ZigZagStartIndex = Index;
				ZigZagReset[Index] = 1.0f;
				TextLabels[Index] = 10;  // first point

				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}

		int CalcIndex = Index;

		if (CalcOnBarClose.GetYesNo())
		{
			CalcIndex--;
			if (CalcIndex <= LastCalcIndex)
				continue;
			LastCalcIndex = CalcIndex;
		}

		//Calculate and set the zigzag lines
		switch (CalculationMode.GetInt())
		{
		case 1:
			sc.ZigZag(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				ReversalPercent.GetFloat() * 0.01f,
				ZigZagStartIndex);
			break;
		case 2:
			sc.ZigZag2(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				NumberBarsForReversal.GetInt(),
				ReversalAmount.GetFloat(),
				ZigZagStartIndex);
			break;
		case 3:
			sc.ZigZag(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				0.0f,
				ReversalAmount.GetFloat(),
				ZigZagStartIndex);
			break;
		}
	}
	
	AccumulateZigZagVolume(sc, AccumulatedVolume, ZigZagPeakType, VolumeToAccumulate.GetIndex(), ZigZagReset);

	if (PriorLastZZEndpointIndex != -1)
	{
		for (int ZeroIndex = PriorLastZZEndpointIndex; ZeroIndex < sc.ArraySize; ZeroIndex++)
		{
			TextLabels[ZeroIndex] = 0;
			ReversalPrice[ZeroIndex] = 0;
			ZigzagLength[ZeroIndex] = 0;
			ZigzagNumBars[ZeroIndex] = 0;
		}
	}

	if (PriorLastZZEndpointIndex == -1)
		PriorLastZZEndpointIndex = 0;

	// Get the index of the last endpoint
	int IndexOfLastEndPoint = 0;
	if (CalcOnBarClose.GetYesNo() && sc.ArraySize > 1)
		IndexOfLastEndPoint = (int)ZigZagPeakIndex[sc.ArraySize-2];
	else
		IndexOfLastEndPoint = (int)ZigZagPeakIndex[sc.ArraySize-1];


	for (int ZigZagIndex = PriorLastZZEndpointIndex; ZigZagIndex < sc.ArraySize; ZigZagIndex++)
	{
		if (ZigZagLine[ZigZagIndex] != 0 && ColorZigZagLine.GetIndex() != 0)
		{
			if (ColorZigZagLine.GetIndex() == 1 && ZigZagIndex > 0)
			{
				if (ZigZagLine[ZigZagIndex] > ZigZagLine[ZigZagIndex-1])
					ZigZagLine.DataColor[ZigZagIndex] = ZigZagLine.PrimaryColor;
				else
					ZigZagLine.DataColor[ZigZagIndex] = ZigZagLine.SecondaryColor;
			}
			else if (ColorZigZagLine.GetIndex() == 2 || ColorZigZagLine.GetIndex() == 3)
			{
				if (ZigZagTrend[ZigZagIndex] > 0)
					ZigZagLine.DataColor[ZigZagIndex] = ZigZagLine.PrimaryColor;
				else if (ZigZagTrend[ZigZagIndex] < 0)
					ZigZagLine.DataColor[ZigZagIndex] = ZigZagLine.SecondaryColor;
			}
		}

		int CurrentPeakIndex = (int)ZigZagPeakIndex[ZigZagIndex];

		ZigZagOscillator[ZigZagIndex] = sc.Close[ZigZagIndex] - ZigZagLine[CurrentPeakIndex];

		if (ZigZagPeakType[ZigZagIndex] == 0.0f) //not a peak, nothing to be drawn
			continue;

		if (ZigZagReset[ZigZagIndex] != 0)//Continue processing if this is a reset index
		{			
			continue;
		}

		int PrevEndPointIndex = -1;
		if (ZigZagIndex > 0)
			PrevEndPointIndex = (int)ZigZagPeakIndex[ZigZagIndex-1];

		if (PrevEndPointIndex >= 0)
		{
			float MidPoint = (ZigZagLine[ZigZagIndex] + ZigZagLine[PrevEndPointIndex]) / 2.0f;
			if (ZigzagMidPoint[ZigZagIndex] != MidPoint)
			{
				for (int UpdateIndex=PrevEndPointIndex+1; UpdateIndex<=ZigZagIndex; UpdateIndex++)
					ZigzagMidPoint[UpdateIndex] = MidPoint;
			}
		}

		if (ColorZigZagLine.GetIndex() == 2 || ColorZigZagLine.GetIndex() == 3) // determine zig zag trend color with confirmed volume
		{
			if (PrevEndPointIndex >= 0)
			{
				float Trend = ZigZagPeakType[ZigZagIndex];  // default trend is current direction

				if (PrevEndPointIndex > 0 && ZigZagReset[PrevEndPointIndex] == 0)	// have more than one swing
				{
					if (((ColorZigZagLine.GetIndex() == 2 && Trend != ZigZagTrend[PrevEndPointIndex]) || // use current trend and volume
						ColorZigZagLine.GetIndex() == 3)  // only use volume
						&& AccumulatedVolume[ZigZagIndex] < AccumulatedVolume[PrevEndPointIndex])
						Trend = -Trend;
				}

				if (ZigZagTrend[ZigZagIndex] != Trend)
				{
					for (int UpdateIndex=PrevEndPointIndex+1; UpdateIndex<=ZigZagIndex; UpdateIndex++)
					{
						ZigZagTrend[UpdateIndex] = Trend;

						if (Trend > 0)
							ZigZagLine.DataColor[UpdateIndex] = ZigZagLine.PrimaryColor;
						else
							ZigZagLine.DataColor[UpdateIndex] = ZigZagLine.SecondaryColor;
					}
				}
			}
		}

		//At this point we have a label to draw

		PriorLastZZEndpointIndex = ZigZagIndex;

		SCString BarLabelText;

		int VerticalTextAlignment = ZigZagPeakType[ZigZagIndex] == 1.0 ? DT_BOTTOM : DT_TOP;

		double ZigzagLineLength = 0;
		int CurrentZigZagNumBars = 0;

		CreateZigZagLineLabel(BarLabelText, ZigZagPeakType, ZigZagIndex, ZigZagPeakIndex, ShowRevPrice, UseMultiLineLabels, sc, ZigZagLine, ShowLength, ShowLabelPrefixes, ShowTime, IncludeSecondsInTimeDisplay, ShowZigZagVolume, AccumulatedVolume, FormatVolumeWithLargeNumberSuffix, ShowZigZagDuration, ShowZigZagNumBars, DisplayZigZagVolumeDividedByLengthInTicks, ZigZagReset, DisplayHHHLLLLHLabels, TextLabels, ZigzagLineLength, CurrentZigZagNumBars);

		if (BarLabelText.GetLength() > 0 && !sc.HideStudy)
		{
			s_UseTool Tool; 
			Tool.Clear();
			Tool.ChartNumber = sc.ChartNumber;
			Tool.Region = sc.GraphRegion;
			Tool.DrawingType = DRAWING_TEXT;
			Tool.ReverseTextColor = 0;
			Tool.BeginIndex = ZigZagIndex;
	
			float Offset;
			if (LabelOffsetType.GetIndex() == 0)
				Offset = LabelsOffset.GetFloat()*0.01f*(sc.High[ZigZagIndex]-sc.Low[ZigZagIndex]);
			else
				Offset = LabelsOffset.GetFloat()*sc.TickSize;

			if(ZigZagPeakType[ZigZagIndex] == 1.0f)
				Tool.BeginValue = ZigZagLine[ZigZagIndex] + Offset;
			else
				Tool.BeginValue = ZigZagLine[ZigZagIndex] - Offset;				

			if (ZigZagPeakType[ZigZagIndex] == 1.0f)
				Tool.Color =  TextLabels.PrimaryColor;
			else
				Tool.Color = TextLabels.SecondaryColor;

			//Tool.FontFace = sc.GetChartTextFontFaceName();
			Tool.FontSize = TextLabels.LineWidth;
			Tool.FontBold = true;
			Tool.Text = BarLabelText;
			Tool.AddMethod = UTAM_ADD_ALWAYS;
			Tool.LineNumber = -1;

			if (UseMultiLineLabels.GetYesNo())
				Tool.TextAlignment	= DT_LEFT | VerticalTextAlignment;
			else
				Tool.TextAlignment	= DT_CENTER | VerticalTextAlignment;

			sc.UseTool(Tool);
			LastUsedLabelDrawingNumber = Tool.LineNumber;
		}

		if (ExtendEndPointsUntilFutureIntersection.GetYesNo())
		{
			sc.AddLineUntilFutureIntersection
			(ZigZagIndex
				, 0  // LineIDForBar
				, ZigZagLine[ZigZagIndex]
				, ZigZagPeakType[ZigZagIndex] == 1.0f ? ExtensionLinesSubgraph.PrimaryColor : ExtensionLinesSubgraph.SecondaryColor
				, ExtensionLinesSubgraph.LineWidth
				, ExtensionLinesSubgraph.LineStyle
				, DisplayValueLabel.GetBoolean()
				, false
				, ""
			);

			LastUsedBarIndexForExtensionLine = ZigZagIndex;
		}

		ReversalPrice[ZigZagIndex] = ZigZagLine[ZigZagIndex];
		ZigzagLength[ZigZagIndex] = (float)ZigzagLineLength;
		ZigzagNumBars[ZigZagIndex] = (float)CurrentZigZagNumBars;
	}

	//Determine the beginning of the last line
	int Count = 0; //To count the number of zigzag pivots
	int StartingIndex = sc.ArraySize;
	while (StartingIndex > 0 && Count < 2)
	{
		StartingIndex--;
		if (ZigZagPeakType[StartingIndex] != 0.0f)
			Count ++;
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = StartingIndex;
}

/*==========================================================================*/
SCSFExport scsf_ZigZagCumulativeVolumeStudy(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ZigZagLine = sc.Subgraph[0];
	SCSubgraphRef AccumulatedVolume = sc.Subgraph[4];

	SCFloatArrayRef ZigZagPeakType  = ZigZagLine.Arrays[0];
	SCFloatArrayRef ZigZagReset     = ZigZagLine.Arrays[2];  // ZigZagReset: 0=no reset, 1=reset

	SCInputRef VolumeToAccumulate = sc.Input [0];
	SCInputRef VersionUpdate = sc.Input[14];
	SCInputRef InputDataHigh = sc.Input[15];
	SCInputRef InputDataLow = sc.Input[16];
	SCInputRef CalculationMode = sc.Input[17];
	SCInputRef ReversalPercent = sc.Input[18];
	SCInputRef ReversalAmount = sc.Input[19];
	SCInputRef NumberBarsForReversal = sc.Input[20];
	SCInputRef ResetAtStartOfSession = sc.Input[26];
	SCInputRef CalcOnBarClose = sc.Input[27];
	
	
	if(sc.SetDefaults)
	{
		sc.GraphName="Zig Zag Cumulative Volume";
	
		//Manual looping
		sc.AutoLoop = 0;
		sc.ValueFormat= 0;
		sc.GraphRegion = 1;

		ZigZagLine.Name = "Zig Zag";
		ZigZagLine.DrawStyle = DRAWSTYLE_IGNORE;

		AccumulatedVolume.Name= "AccumulatedVolume";
		AccumulatedVolume.DrawStyle = DRAWSTYLE_BAR;
		AccumulatedVolume.SecondaryColorUsed = 1;

		VolumeToAccumulate.Name= "Volume To Accumulate";
		VolumeToAccumulate.SetCustomInputStrings("Total Volume;Bid Volume;Ask Volume;Ask Bid Volume Difference");
		VolumeToAccumulate.SetCustomInputIndex(0);

		VersionUpdate.SetInt(2);

		InputDataHigh.Name = "Input Data for High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data for Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		CalculationMode.Name = "Calculation Mode (1,2,3)";
		CalculationMode.SetInt(1);
		CalculationMode.SetIntLimits(1, 3);

		ReversalPercent.Name = "Reversal % for Calculation Mode 1";
		ReversalPercent.SetFloat(.5f);

		ReversalAmount.Name = "Reversal Amount for Calculation Mode 2,3";
		ReversalAmount.SetFloat(0.01f);

		NumberBarsForReversal.Name = "Number of Bars Required for Reversal (Calculation Mode 2)";
		NumberBarsForReversal.SetInt(5);

		ResetAtStartOfSession.Name = "Reset ZigZag At Start Of Trading Day";
		ResetAtStartOfSession.SetYesNo(0);

		CalcOnBarClose.Name = "Calculate New Values On Bar Close";
		CalcOnBarClose.SetYesNo(0);

		return;
	}

	int &LastCalcIndex	  = sc.GetPersistentInt(1);
	int &ZigZagStartIndex = sc.GetPersistentInt(2);

	if (sc.UpdateStartIndex == 0)
	{
		LastCalcIndex = -1;
		ZigZagStartIndex = 0;
		ZigZagReset[0] = 1.0f;

		if (VersionUpdate.GetInt() < 2)
		{
			ResetAtStartOfSession.SetYesNo(0);
			VersionUpdate.SetInt(2);
		}
	}

	int VolumeTypeToAccumulate = VolumeToAccumulate.GetIndex() + 1;  // adjust for no "none" option

	SCDateTime NextSessionStart = 0;
	if (sc.UpdateStartIndex > 0)
		NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(sc.BaseDateTimeIn[sc.UpdateStartIndex - 1]) + 1*DAYS;

	sc.EarliestUpdateSubgraphDataArrayIndex = sc.UpdateStartIndex;

	// Standard Manual Looping loop
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		if (ResetAtStartOfSession.GetYesNo() != 0)
		{
			SCDateTime IndexDateTime = sc.BaseDateTimeIn[Index];

			if (IndexDateTime >= NextSessionStart)
			{
				ZigZagStartIndex = Index;
				ZigZagReset[Index] = 1.0f;

				NextSessionStart = sc.GetTradingDayStartDateTimeOfBar(IndexDateTime) + 1*DAYS;
			}
		}

		int CalcIndex = Index;

		if (CalcOnBarClose.GetYesNo())
		{
			CalcIndex--;
			if (CalcIndex <= LastCalcIndex)
				continue;
			LastCalcIndex = CalcIndex;
		}

		switch (CalculationMode.GetInt())
		{
		case 1:
			sc.ZigZag(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				ReversalPercent.GetFloat() * 0.01f,
				ZigZagStartIndex);
			break;
		case 2:
			sc.ZigZag2(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				NumberBarsForReversal.GetInt(),
				ReversalAmount.GetFloat(),
				ZigZagStartIndex);
			break;
		case 3:
			sc.ZigZag(
				sc.BaseData[InputDataHigh.GetInputDataIndex()],
				sc.BaseData[InputDataLow.GetInputDataIndex()],
				ZigZagLine,
				CalcIndex,
				0.0f,
				ReversalAmount.GetFloat(),
				ZigZagStartIndex);
			break;
		}

	}

	AccumulateZigZagVolume(sc, AccumulatedVolume, ZigZagPeakType, VolumeTypeToAccumulate, ZigZagReset);

	//Determine the beginning of the last line
	int Count = 0; //To count the number of zigzag pivots
	int StartingIndex = sc.ArraySize;
	while (StartingIndex > 0 && Count < 2)
	{
		StartingIndex--;
		if (ZigZagPeakType[StartingIndex] != 0.0f)
			Count ++;
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = StartingIndex;
}

/*==========================================================================*/
SCSFExport scsf_VolumeColoredBasedOnVolume2(SCStudyInterfaceRef sc)
{

	SCSubgraphRef Volume = sc.Subgraph[0];
	SCSubgraphRef Volume3rdColor = sc.Subgraph[1];
	SCSubgraphRef Volume4thColor = sc.Subgraph[2];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Volume - Colored Based on Volume 2";

		sc.StudyDescription = "This study displays the Volume per bar. Each volume bar is colored based upon whether the volume is up or down compared to the previous bar volume. It uses a 3rd color when the Volume is lower than the 2 previous bars.";

		

		sc.AutoLoop = 1;  // true

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.SecondaryColorUsed = 1;
		Volume.PrimaryColor = RGB(0,255,0);
		Volume.SecondaryColor = RGB(255,0,0);

		Volume3rdColor.Name = "Volume 3rd Color";
		Volume3rdColor.PrimaryColor = RGB(255,128,64);

		Volume4thColor.Name = "Default Color";
		Volume4thColor.PrimaryColor = RGB(0,128,255);

		return;
	}

	// Section 2 - Do data processing here

	Volume[sc.Index] = sc.Volume[sc.Index];

	if(sc.Volume[sc.Index] < sc.Volume[sc.Index-1]
	&& sc.Volume[sc.Index] < sc.Volume[sc.Index-2])
		Volume.DataColor[sc.Index] = Volume3rdColor.PrimaryColor;
	else if(sc.Volume[sc.Index-1] > sc.Volume[sc.Index])
		Volume.DataColor[sc.Index] = Volume.SecondaryColor;
	else if(sc.Volume[sc.Index-1] < sc.Volume[sc.Index])
		Volume.DataColor[sc.Index] = Volume.PrimaryColor;
	else //If volume is equal
		Volume.DataColor[sc.Index] = Volume4thColor.PrimaryColor;

}

/*==========================================================================*/
SCSFExport scsf_BackgroundColoring(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Top = sc.Subgraph[0];
	SCSubgraphRef Bottom = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Background Coloring";

		sc.AutoLoop = 1;
		sc.GraphRegion = 0;

		Top.Name ="Top";
		Top.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_TOP;
		Top.PrimaryColor = RGB(0,255,0);
		Top.DrawZeros = false;

		Bottom.Name ="Bottom";
		Bottom.DrawStyle = DRAWSTYLE_FILL_RECTANGLE_BOTTOM;
		Bottom.DrawZeros = RGB(255,0,255);
		Bottom.DrawZeros = false;

		sc.ScaleRangeType = SCALE_INDEPENDENT;

		
		

		return;
	}

	// Do data processing

	Top[sc.ArraySize - 1] = 2;
	Bottom[sc.ArraySize - 1] = 1;
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageSimple(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Simple";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0, 255, 0);
		Avg.LineWidth = 1;
		Avg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg,  Length.GetInt());
}


/*==========================================================================*/
SCSFExport scsf_MovingAverageSimpleSkipZeros(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Simple Skip Zeros";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0, 255, 0);
		Avg.LineWidth = 1;
		Avg.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		

		return;
	}

	int LengthInput = Length.GetInt();

	sc.DataStartIndex = LengthInput - 1;

	SCFloatArrayRef InputArray = sc.BaseDataIn[InputData.GetInputDataIndex()];

	if (LengthInput <= 0)
		return;

	float Sum = 0;
	int Count = 0;
	if (sc.Index < LengthInput - 1)
		LengthInput = sc.Index + 1;

	for(int BarIndex = sc.Index - LengthInput + 1; BarIndex <= sc.Index; BarIndex++)
	{
		if (InputArray[BarIndex] == 0.0f)
			continue;

		Sum  += InputArray[BarIndex]; 
		Count ++;
	}

	if (Count > 0)
		Avg[sc.Index] = Sum / Count;
	else
		Avg[sc.Index] = 0;

	return;
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageTimePeriod(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_Average = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef TimePeriodType = sc.Input[1];
	SCInputRef TimePeriodLength = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Time Period";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 0;

		Subgraph_Average.Name = "Avg";
		Subgraph_Average.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Average.PrimaryColor = RGB(0, 255, 0);
		Subgraph_Average.LineWidth = 1;
		Subgraph_Average.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		TimePeriodType.Name = "Time Period Type";
		TimePeriodType.SetTimePeriodType(TIME_PERIOD_LENGTH_UNIT_DAYS);

		TimePeriodLength.Name = "Time Period Length";
		TimePeriodLength.SetInt(1);

		return;
	}

	SCDateTime TimePeriodLengthAsDateTime = sc.TimePeriodSpan(TimePeriodType.GetIndex(), TimePeriodLength.GetInt());
	int LengthInput = 0;

	int LookbackStartIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, (sc.BaseDateTimeIn[sc.UpdateStartIndex+1] - TimePeriodLengthAsDateTime));
	

	SCFloatArrayRef InputArray = sc.BaseDataIn[InputData.GetInputDataIndex()];

	for(int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		float Sum = 0;
		int Count = 0;

		for (int LookbackIndex = LookbackStartIndex; LookbackIndex <= BarIndex; LookbackIndex++)
		{
			if (InputArray[LookbackIndex] == 0.0f)
				continue;

			Sum += InputArray[LookbackIndex];
			Count++;
		}

		if (Count > 0)
			Subgraph_Average[BarIndex] = Sum / Count;
		else
			Subgraph_Average[BarIndex] = 0;

		if (BarIndex < sc.ArraySize - 1)
		{
			SCDateTime NewLookbackStartDateTime = sc.BaseDateTimeIn[BarIndex + 1] - TimePeriodLengthAsDateTime;

			while (true)
			{
				SCDateTime CurrentAbsoluteTimeDifference = SCDateTime(sc.BaseDateTimeIn[LookbackStartIndex] - NewLookbackStartDateTime).GetAbsoluteValue();
				SCDateTime NextAbsoluteTimeDifference = SCDateTime(sc.BaseDateTimeIn[LookbackStartIndex + 1] - NewLookbackStartDateTime).GetAbsoluteValue();

				if (NextAbsoluteTimeDifference > CurrentAbsoluteTimeDifference)
					break;

				LookbackStartIndex++;

			}				
		}
	}

	return;
}

/*==========================================================================*/

SCSFExport scsf_AverageDailyRange(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ADR = sc.Subgraph[0];

	SCInputRef Length = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Average Daily Range";

		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.AutoLoop = 1;

		ADR.Name = "ADR";
		ADR.DrawStyle = DRAWSTYLE_LINE;
		ADR.PrimaryColor = RGB(0, 255, 0);
		ADR.LineWidth = 2;
		ADR.DrawZeros = true;


		Length.Name = "Length";
		Length.SetInt(30);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;
	ADR.Arrays[0][sc.Index] = sc.High[sc.Index] - sc.Low[sc.Index];


	sc.SimpleMovAvg(ADR.Arrays[0], ADR,  Length.GetInt());
}
/*==========================================================================*/
SCSFExport scsf_OBVWithMovAvg(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBV = sc.Subgraph[0];
	SCSubgraphRef Avg = sc.Subgraph[1];
	SCInputRef OBVLength = sc.Input[3];
	SCInputRef MovAvgLength = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Volume with Mov Avg";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;
		
		OBV.Name = "OBV";
		OBV.DrawStyle = DRAWSTYLE_LINE;
		OBV.PrimaryColor = RGB(0,255,0);
		OBV.DrawZeros = true;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,0,255);
		Avg.DrawZeros = true;

		//OBVLength.Name = "OBV Length";
		OBVLength.SetInt(10);
		OBVLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		MovAvgLength.Name = "Mov Avg Length";
		MovAvgLength.SetInt(10);
		MovAvgLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = MovAvgLength.GetInt();

	int i = (sc.Index >=1 ? sc.Index : 1);

	if(sc.Close[i] > sc.Close[i - 1])
	{
		OBV[i] = OBV[i - 1] + sc.Volume[i];
	}
	else if(sc.Close[i] < sc.Close[i - 1])
	{
		OBV[i] = OBV[i - 1] - sc.Volume[i];
	}
	else 
		OBV[i] = OBV[i - 1]; 

	sc.SimpleMovAvg(OBV, Avg, MovAvgLength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_OpenInterest(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OI = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Open Interest";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		OI.Name = "OI";
		OI.PrimaryColor = RGB(0,255,0);
		OI.DrawStyle = DRAWSTYLE_BAR;
		OI.DrawZeros = false;

		return;
	}

	OI[sc.Index]= sc.OpenInterest[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_NumberOfTrades(SCStudyInterfaceRef sc)
{
	SCSubgraphRef NumTrades = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Number of Trades";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		NumTrades.Name = "NumTrades";
		NumTrades.PrimaryColor = RGB(0,255,0);
		NumTrades.DrawStyle = DRAWSTYLE_BAR;
		NumTrades.LineWidth= 2;
		NumTrades.DrawZeros = false;

		return;
	}

	NumTrades[sc.Index]= sc.NumberOfTrades[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_VolumeDividedByNumberOfTrades(SCStudyInterfaceRef sc)
{
	SCSubgraphRef NumTrades = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume / NumberTrades";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		NumTrades.Name = "NumTrades";
		NumTrades.PrimaryColor = RGB(0,255,0);
		NumTrades.DrawStyle = DRAWSTYLE_BAR;
		NumTrades.LineWidth= 2;
		NumTrades.DrawZeros = false;

		return;
	}

	NumTrades[sc.Index] = sc.Volume[sc.Index] / sc.NumberOfTrades[sc.Index];
}
/*==========================================================================*/
SCSFExport scsf_SumBar(SCStudyInterfaceRef sc)
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

	SCInputRef InChart2Number = sc.Input[3];
	SCInputRef InChart1Multiplier = sc.Input[4];
	SCInputRef InChart2Multiplier = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Sum (Bar)";
		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;
		sc.UseGlobalChartColors = 0;
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
		OHLCAvg.DrawZeros = false;
		OHLCAvg.PrimaryColor = RGB(127,0,255);

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.DrawZeros = false;
		HLCAvg.PrimaryColor = RGB(0,255,255);

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.DrawZeros = false;
		HLAvg.PrimaryColor = RGB(0,127,255);
		
		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.DrawZeros = false;
		BidVol.PrimaryColor = RGB(0,255,0);
		
		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.DrawZeros = false;
		AskVol.PrimaryColor = RGB(0,255,0);


		InChart2Number.Name = "Chart 2 Number";
		InChart2Number.SetChartNumber(1);

		InChart1Multiplier.Name = "Chart 1 Multiplier";
		InChart1Multiplier.SetFloat(1.0f);

		InChart2Multiplier.Name = "Chart 2 Multiplier";
		InChart2Multiplier.SetFloat(1.0f);

		return;
	}
	
	// Obtain a reference to the Base Data in the specified chart.  This call
	// is relatively efficient, but it should be called as minimally as
	// possible.  To reduce the number of calls we have it outside of the
	// primary "for" loop in this study function.  And we also use Manual
	// Looping by not defining sc.AutoLoop = 1.  In this way,
	// sc.GetChartBaseData is called only once per call to this study
	// function.  sc.GetChartBaseData is a new function to get all of the Base
	// Data arrays with one efficient call.
	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-InChart2Number.GetChartNumber(), Chart2BaseData);
	
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(InChart2Number.GetChartNumber(), Index);
		
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
				= (sc.BaseDataIn[SubgraphIndex][Index] * InChart1Multiplier.GetFloat())
				+ (Chart2BaseData[SubgraphIndex][Chart2Index] * InChart2Multiplier.GetFloat());
		}
		
		sc.Subgraph[SC_HIGH][Index]
			= max(sc.Subgraph[SC_OPEN][Index],
				max(sc.Subgraph[SC_HIGH][Index],
					max(sc.Subgraph[SC_LOW][Index], sc.Subgraph[SC_LAST][Index])
				)
			);
		
		sc.Subgraph[SC_LOW][Index]
			= min(sc.Subgraph[SC_OPEN][Index],
				min(sc.Subgraph[SC_HIGH][Index],
					min(sc.Subgraph[SC_LOW][Index], sc.Subgraph[SC_LAST][Index])
				)
			);
		
		sc.CalculateOHLCAverages(Index);
	}
	
	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(InChart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Sum %s + %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}


/*==========================================================================*/
SCSFExport scsf_RatioBar(SCStudyInterfaceRef sc)
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
	SCSubgraphRef Unused11 = sc.Subgraph[11];
	SCSubgraphRef Unused12 = sc.Subgraph[12];

	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];
	SCInputRef ZeroOutputWhenNonExactDateMatch = sc.Input[6];
	SCInputRef UseLatestSourceDataForLastBar = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ratio (Bar)";

		sc.UseGlobalChartColors = 0;
		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		sc.GraphUsesChartColors = 1;

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

		Unused11.DrawStyle = DRAWSTYLE_IGNORE;
		Unused11.DrawZeros = false;

		Unused12.DrawStyle = DRAWSTYLE_IGNORE;
		Unused12.DrawZeros = false;


		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		ZeroOutputWhenNonExactDateMatch.Name = "Zero Output When Non Exact Date Match";
		ZeroOutputWhenNonExactDateMatch.SetYesNo(false);

		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(0);

		return;
	}
	
	SCGraphData Chart2Arrays;
	sc.GetChartBaseData(-Chart2Number.GetChartNumber(), Chart2Arrays);
	
	if (Chart2Arrays[0].GetArraySize() == 0)
		return;
	
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = 0;
		if(ZeroOutputWhenNonExactDateMatch.GetYesNo() == false)
		{
			Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);

			//When 'use latest source data for last bar' is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source chart.
			if(UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() && Index == sc.ArraySize - 1)
				Chart2Index = Chart2Arrays[0].GetArraySize() - 1;

		}
		else
		{

			Chart2Index = sc.GetExactMatchForSCDateTime(Chart2Number.GetChartNumber(), sc.BaseDateTimeIn[Index]);
			if(Chart2Index == -1)
			{
				for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_HL_AVG; SubgraphIndex++)
				{
					sc.Subgraph[SubgraphIndex][Index] = 0.0;
				}

				continue;
			}
		}


		
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
		{
			if (sc.BaseDataIn[SubgraphIndex][Index] == 0 || Chart2Arrays[SubgraphIndex][Chart2Index] == 0)
			{
				sc.Subgraph[SubgraphIndex][Index] = 0;
				continue;
			}
			
			sc.Subgraph[SubgraphIndex][Index] = (sc.BaseDataIn[SubgraphIndex][Index] * Chart1Multiplier.GetFloat())
				/ (Chart2Arrays[SubgraphIndex][Chart2Index] * Chart2Multiplier.GetFloat());
		}

		sc.Subgraph[SC_HIGH][Index]
		= max(sc.Subgraph[0][Index],
			max(sc.Subgraph[1][Index],
			max(sc.Subgraph[2][Index],sc.Subgraph[3][Index])
			)
			);

		sc.Subgraph[SC_LOW][Index]
		= min(sc.Subgraph[0][Index],
			min(sc.Subgraph[1][Index],
			min(sc.Subgraph[2][Index],sc.Subgraph[3][Index])
			)
			);

		sc.CalculateOHLCAverages(Index);
	}

	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Ratio %s / %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_MultiplyBar(SCStudyInterfaceRef sc)
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
	SCSubgraphRef Unused11 = sc.Subgraph[11];
	SCSubgraphRef Unused12 = sc.Subgraph[12];

	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Multiply Charts - Bar";

		sc.GraphUsesChartColors = true;

		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 1;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

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

		Unused11.DrawStyle = DRAWSTYLE_IGNORE;
		Unused11.DrawZeros = false;

		Unused12.DrawStyle = DRAWSTYLE_IGNORE;
		Unused12.DrawZeros = false;

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		return;
	}

	SCGraphData Chart2Arrays;
	sc.GetChartBaseData(-Chart2Number.GetChartNumber(), Chart2Arrays);

	if (Chart2Arrays[0].GetArraySize() == 0)
		return;

	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Index);

		for (int i2 = SC_OPEN; i2 <= SC_NUM_TRADES; i2++)
		{
			

			sc.Subgraph[i2][Index] = (sc.BaseDataIn[i2][Index] * Chart1Multiplier.GetFloat())
				* (Chart2Arrays[i2][i1] * Chart2Multiplier.GetFloat());
		}

		sc.Subgraph[1][Index]
		= max(sc.Subgraph[0][Index],
			max(sc.Subgraph[1][Index],
			max(sc.Subgraph[2][Index],sc.Subgraph[3][Index])
			)
			);

		sc.Subgraph[2][Index]
		= min(sc.Subgraph[0][Index],
			min(sc.Subgraph[1][Index],
			min(sc.Subgraph[2][Index],sc.Subgraph[3][Index])
			)
			);

		sc.CalculateOHLCAverages(Index);
	}

	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Multiply %s * %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_OnBalanceVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBV = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Volume";
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		OBV.Name = "OBV";
		OBV.PrimaryColor = RGB(0,255,0);
		OBV.DrawStyle = DRAWSTYLE_LINE;
		OBV.DrawZeros = true;

		return;
	}

	sc.DataStartIndex = 1;

	sc.OnBalanceVolume(sc.BaseDataIn, OBV);
}

/*==========================================================================*/
SCSFExport scsf_PriceOpenInterestVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_POIV = sc.Subgraph[0];
	SCFloatArrayRef Array_TrueHigh = sc.Subgraph[0].Arrays[0];
	SCFloatArrayRef Array_TrueLow = sc.Subgraph[0].Arrays[1];
	SCFloatArrayRef Array_IntermediateCalculation = sc.Subgraph[0].Arrays[2];
	SCFloatArrayRef Array_OBV = sc.Subgraph[0].Arrays[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Price, Open Interest, Volume";
		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		Subgraph_POIV.Name = "POIV";
		Subgraph_POIV.PrimaryColor = RGB(0, 255, 0);
		Subgraph_POIV.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_POIV.DrawZeros = true;

		return;
	}

	sc.DataStartIndex = 1;

	if (sc.Index < 1)
		return;

	//CumulativeSum (Open Interest * (Close - Close.1) / (True High - True Low)) + OBV 

	sc.OnBalanceVolume(sc.BaseDataIn, Array_OBV);

	float TrueHighLowDifference = sc.GetTrueHigh(sc.BaseDataIn) - sc.GetTrueLow(sc.BaseDataIn);

	if (TrueHighLowDifference != 0)
	{
		Array_IntermediateCalculation[sc.Index]
			=  ( 
			(sc.BaseDataIn[SC_OPEN_INTEREST][sc.Index] 
				* (sc.BaseDataIn[SC_LAST][sc.Index] - sc.BaseDataIn[SC_LAST][sc.Index - 1])
				) 
				/ TrueHighLowDifference
				) 
				+ Array_OBV[sc.Index];

		sc.CumulativeSummation(Array_IntermediateCalculation, Subgraph_POIV);
	}
	else
	{
		Subgraph_POIV[sc.Index] = Subgraph_POIV[sc.Index - 1];
	}
}

/*==========================================================================*/
SCSFExport scsf_Line(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Line = sc.Subgraph[0];
	SCInputRef Value = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Line";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Line.Name = "Line";
		Line.DrawStyle = DRAWSTYLE_LINE;
		Line.PrimaryColor = RGB(0,255,0);
		Line.DrawZeros = 1;

		Value.Name = "Value";
		Value.SetFloat(1.0f);

		return;
	}

	sc.DataStartIndex = 0;

	Line[sc.Index] = Value.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_AverageTrueRange(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ATR = sc.Subgraph[0];
	SCInputRef MAType = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Average True Range";

		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;

		ATR.Name = "ATR";
		ATR.DrawZeros = false;
		ATR.PrimaryColor = RGB(0,255,0);
		ATR.DrawStyle = DRAWSTYLE_LINE;

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);


		Length.Name = "Moving Average Length";
		Length.SetInt(14);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;
	sc.ATR(sc.BaseDataIn, ATR, Length.GetInt(), MAType.GetMovAvgType());
}


/*==========================================================================*/
SCSFExport scsf_TrueRange(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TR = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "True Range";

		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;

		TR.Name = "TR";
		TR.DrawZeros = false;
		TR.PrimaryColor = RGB(0,255,0);
		TR.DrawStyle = DRAWSTYLE_LINE;


		return;
	}

	sc.TrueRange(sc.BaseDataIn, TR);
}

/*==========================================================================*/
SCSFExport scsf_NormalizedAverageTrueRange(SCStudyInterfaceRef sc)
{

	SCSubgraphRef Subgraph_ATR = sc.Subgraph[0];
	SCSubgraphRef Subgraph_NATR = sc.Subgraph[1];

	SCInputRef Input_MAType = sc.Input[0];
	SCInputRef Input_Length = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Average True Range - Normalized";

		sc.GraphRegion = 1;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;

		Subgraph_ATR.Name = "ATR";
		Subgraph_ATR.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_NATR.Name = "NATR";
		Subgraph_NATR.DrawZeros = false;
		Subgraph_NATR.PrimaryColor = RGB(0, 255, 0);
		Subgraph_NATR.DrawStyle = DRAWSTYLE_LINE;

		Input_MAType.Name = "Moving Average Type";
		Input_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);


		Input_Length.Name = "Moving Average Length";
		Input_Length.SetInt(14);
		Input_Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Input_Length.GetInt() - 1;
	sc.ATR(sc.BaseDataIn, Subgraph_ATR, Input_Length.GetInt(), Input_MAType.GetMovAvgType());

	if (sc.BaseData[SC_LAST][sc.Index] == 0)
		Subgraph_NATR[sc.Index] = 0;
	else
		Subgraph_NATR[sc.Index] = 100 * Subgraph_ATR[sc.Index] / sc.BaseData[SC_LAST][sc.Index];
}
/*==========================================================================*/
SCSFExport scsf_Vortex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef VMPlus  = sc.Subgraph[0];
	SCSubgraphRef VMMinus = sc.Subgraph[1];
	
	SCInputRef VortexLength = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Vortex";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		VMPlus.Name = "VM+";
		VMPlus.DrawZeros = false;
		VMPlus.PrimaryColor = RGB(0,128,255);
		VMPlus.DrawStyle = DRAWSTYLE_LINE;

		VMMinus.Name = "VM-";
		VMMinus.DrawZeros = false;
		VMMinus.PrimaryColor = COLOR_CRIMSON;
		VMMinus.DrawStyle = DRAWSTYLE_LINE;

		VortexLength.Name = "Vortex Length";
		VortexLength.SetInt(21);
		VortexLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.Vortex(sc.BaseDataIn, VMPlus, VMMinus, VortexLength.GetInt());
}


/*==========================================================================*/
SCSFExport scsf_DailyRangeBand(SCStudyInterfaceRef sc)
{
	SCSubgraphRef UpperBand = sc.Subgraph[0];
	SCSubgraphRef LowerBand = sc.Subgraph[1];

	SCInputRef DailyRangeStudy        = sc.Input[0];
	SCInputRef GapAdjustWithYesterdaysClose = sc.Input[1];
	SCInputRef NumDaysToDisplay       = sc.Input[2];
	SCInputRef BandMultiplier		  = sc.Input[3];
	SCInputRef UseDailyHighLow		  = sc.Input[4];
	SCInputRef DailyRangeDisplacement = sc.Input[5];
	SCInputRef StopUpdatingOnRangeMet = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Daily Range Bands";

		sc.GraphRegion = 0;
		sc.AutoLoop = 0; // Manual Looping
		sc.DrawZeros = 0;

		// Subgraphs
		UpperBand.Name = "Upper Range Band";
		UpperBand.DrawZeros = false;
		UpperBand.PrimaryColor = RGB(0,128,192);
		UpperBand.DrawStyle = DRAWSTYLE_STAIR_STEP;

		LowerBand.Name = "Lower Range Band";
		LowerBand.DrawZeros = false;
		LowerBand.PrimaryColor = RGB(192,0,0);
		LowerBand.DrawStyle = DRAWSTYLE_STAIR_STEP;

		// Inputs
		DailyRangeStudy.Name = "Range Study Reference Subgraph";
		DailyRangeStudy.SetChartStudySubgraphValues(1, 1, 0);

		GapAdjustWithYesterdaysClose.Name = "Gap Adjust With Yesterdays Close";
		GapAdjustWithYesterdaysClose.SetYesNo(1);

		NumDaysToDisplay.Name = "Number of Days To Display (0=All)";
		NumDaysToDisplay.SetInt(0);
		NumDaysToDisplay.SetIntLimits(0,MAX_STUDY_LENGTH);

		BandMultiplier.Name = "Band Multiplier";
		BandMultiplier.SetFloat(1.0f);
		BandMultiplier.SetFloatLimits(0, FLT_MAX);

		UseDailyHighLow.Name = "Use High/Low From Historical Daily Chart";
		UseDailyHighLow.SetYesNo(0);

		DailyRangeDisplacement.Name = "Daily Range Displacement";
		DailyRangeDisplacement.SetInt(0);
		DailyRangeDisplacement.SetIntLimits(0, 1000);

		StopUpdatingOnRangeMet.Name = "Stop Updating High/Low When Projected Range Met";
		StopUpdatingOnRangeMet.SetYesNo(0);

		return;
	}

	float& TodaysHigh = sc.GetPersistentFloat(1);
	float& TodaysLow  = sc.GetPersistentFloat(2);
	SCDateTime& CurrentSessionStartDateTime = sc.GetPersistentSCDateTime(1);


	int DailyChartNumber = DailyRangeStudy.GetChartNumber();

	SCGraphData BaseData;
	sc.GetChartBaseData(DailyChartNumber, BaseData);

	SCFloatArrayRef DailyClose = BaseData[SC_LAST];
	SCFloatArrayRef DailyHigh = BaseData[SC_HIGH];
	SCFloatArrayRef DailyLow = BaseData[SC_LOW];

	SCFloatArray DailyBandRange;
	sc.GetStudyArrayFromChartUsingID(DailyRangeStudy.GetChartStudySubgraphValues(), DailyBandRange);

	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++ )
	{
		int TradeDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[BarIndex]);

		bool NewDay = BarIndex == 0 || sc.BaseDateTimeIn[BarIndex] >= CurrentSessionStartDateTime + 1*DAYS;

		if (NewDay)
		{
			TodaysHigh = sc.High[BarIndex];
			TodaysLow = sc.Low[BarIndex];
			CurrentSessionStartDateTime = sc.GetStartDateTimeForTradingDate(TradeDate);

			if (BarIndex != 0 && NumDaysToDisplay.GetInt() != 0)
			{
				SCDateTime FirstCalcDateTime = CurrentSessionStartDateTime - (NumDaysToDisplay.GetInt() - 1)*DAYS;
				int FirstCalcIndex = sc.GetContainingIndexForSCDateTime(sc.ChartNumber, FirstCalcDateTime);
				for (int Index=0; Index<FirstCalcIndex; ++Index)
				{
					UpperBand[Index] = 0;
					LowerBand[Index] = 0;
				}
			}
		}

		int DailyIndex = sc.GetFirstIndexForDate(DailyChartNumber, TradeDate);

		if (UseDailyHighLow.GetYesNo())
		{
			TodaysHigh = DailyHigh[DailyIndex];
			TodaysLow = DailyLow[DailyIndex];
		}
		else
		{
			if (sc.High[BarIndex] > TodaysHigh)
				TodaysHigh = sc.High[BarIndex];
			if (sc.Low[BarIndex] < TodaysLow)
				TodaysLow = sc.Low[BarIndex];
		}

		float RangeStudyValue = 0;
		if (DailyIndex >= DailyRangeDisplacement.GetInt())
			RangeStudyValue = DailyBandRange[DailyIndex-DailyRangeDisplacement.GetInt()];

		if (BandMultiplier.GetFloat() != 0)
			RangeStudyValue *= BandMultiplier.GetFloat();

		float GapAdjustedHigh = TodaysHigh;
		float GapAdjustedLow  = TodaysLow;

		if (GapAdjustWithYesterdaysClose.GetYesNo())
		{
			if (DailyIndex > 0)
			{
				float PriorClose = DailyClose[DailyIndex-1];
				if (PriorClose != 0)
				{
					GapAdjustedLow  = min(TodaysLow, PriorClose);
					GapAdjustedHigh = max(TodaysHigh, PriorClose);
				}
			}
		}

		if (StopUpdatingOnRangeMet.GetYesNo() && !NewDay)
		{
			if ((UpperBand[BarIndex-1] - LowerBand[BarIndex-1]) <= RangeStudyValue)
			{
				UpperBand[BarIndex] = UpperBand[BarIndex-1];
				LowerBand[BarIndex] = LowerBand[BarIndex-1];
			}
			else 
			{
				if (TodaysHigh >= UpperBand[BarIndex-1])
					LowerBand[BarIndex] = UpperBand[BarIndex-1] - RangeStudyValue;
				else
					LowerBand[BarIndex] = GapAdjustedHigh - RangeStudyValue;

				if (TodaysLow <= LowerBand[BarIndex-1])
					UpperBand[BarIndex] = LowerBand[BarIndex-1] + RangeStudyValue;
				else
					UpperBand[BarIndex] = GapAdjustedLow + RangeStudyValue;
			}
		}
		else
		{
			UpperBand[BarIndex] = GapAdjustedLow  + RangeStudyValue;
			LowerBand[BarIndex] = GapAdjustedHigh - RangeStudyValue;
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageExponential(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Exponential";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,255,0);
		Avg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.ExponentialMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageWeighted(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Weighted";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,255,0);
		Avg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	//sc.DataStartIndex = Length.GetInt() - 1;

	sc.WeightedMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageLeastSquares(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Linear Regression";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		Avg.Name = "Avg";
		Avg.DrawStyle = DRAWSTYLE_LINE;
		Avg.PrimaryColor = RGB(0,255,0);
		Avg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() - 1;

	sc.LinearRegressionIndicator(sc.BaseDataIn[InputData.GetInputDataIndex()], Avg, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageEnvelope(SCStudyInterfaceRef sc)
{
	SCSubgraphRef TopBand = sc.Subgraph[0];
	SCSubgraphRef BottomBand = sc.Subgraph[1];
	SCSubgraphRef Average = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef PercentageOrFixed = sc.Input[1];
	SCInputRef Percentage = sc.Input[3];
	SCInputRef MAType = sc.Input[4];
	SCInputRef MALength = sc.Input[5];
	SCInputRef FixedValue = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average Envelope";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		TopBand.Name = "TopBand";
		TopBand.DrawStyle = DRAWSTYLE_LINE;
		TopBand.PrimaryColor = RGB(0,255,0);
		TopBand.DrawZeros = true;

		BottomBand.Name = "BottomBand";
		BottomBand.DrawStyle = DRAWSTYLE_LINE;
		BottomBand.PrimaryColor = RGB(255,0,255);
		BottomBand.DrawZeros = true;

		Average.Name = "Average";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(255,255,0);
		Average.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		InputData.DisplayOrder = 1;

		PercentageOrFixed.Name = "Percentage or Fixed Value";
		PercentageOrFixed.SetCustomInputStrings( "Percentage;Fixed Value");
		PercentageOrFixed.SetCustomInputIndex( 0);
		PercentageOrFixed.DisplayOrder = 2;

		Percentage.Name = "Percentage (.01 = 1%)";
		Percentage.SetFloat(0.005f);
		Percentage.DisplayOrder = 3;

		FixedValue.Name = "Fixed Value";
		FixedValue.SetFloat(10.0f);
		FixedValue.DisplayOrder = 4;

		MAType.Name = "Moving Average Type";
		MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		MAType.DisplayOrder = 5;

		MALength.Name = "Moving Average Length";
		MALength.SetInt(10);
		MALength.SetIntLimits(1, MAX_STUDY_LENGTH);
		MALength.DisplayOrder = 6;

		return;
	}

	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], Average, MAType.GetMovAvgType(), MALength.GetInt());

	sc.DataStartIndex = MALength.GetInt() - 1;

	if (sc.Index < sc.DataStartIndex)
		return;

	if (PercentageOrFixed.GetIndex()  ==  1)//fixed value
	{
		TopBand[sc.Index] = Average[sc.Index] + FixedValue.GetFloat();
		BottomBand[sc.Index] = Average[sc.Index] - FixedValue.GetFloat();
	}
	else//percentage
	{
		TopBand[sc.Index] = Average[sc.Index] * (1 + Percentage.GetFloat());
		BottomBand[sc.Index] = Average[sc.Index] * (1 - Percentage.GetFloat());
	}
}

/*==========================================================================*/
SCSFExport scsf_MarketFacilitationIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MFI = sc.Subgraph[0];
	SCInputRef Multiplier = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Market Facilitation Index";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;
		
		MFI.Name = "MFI";
		MFI.DrawStyle = DRAWSTYLE_LINE;
		MFI.PrimaryColor = RGB(0,255,0);
		MFI.DrawZeros = true;

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		return;
	}

	MFI[sc.Index] = Multiplier.GetFloat() * (sc.High[sc.Index] - sc.Low[sc.Index]) / 
					sc.Volume[sc.Index];
}

/*==========================================================================*/
SCSFExport scsf_Momentum(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Momentum = sc.Subgraph[0];
	SCSubgraphRef CenterLine = sc.Subgraph[1];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];
	SCInputRef MomentumType = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Momentum";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Momentum.Name = "Momentum";
		Momentum.DrawStyle = DRAWSTYLE_LINE;
		Momentum.PrimaryColor = RGB(0,255,0);
		Momentum.DrawZeros = true;
		
		CenterLine.Name = "Line";
		CenterLine.DrawStyle = DRAWSTYLE_LINE;
		CenterLine.PrimaryColor = RGB(128,128,128);
		CenterLine.DrawZeros = true;


		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MomentumType.Name = "Momentum Type";
		MomentumType.SetCustomInputStrings("Difference;Quotient");
		MomentumType.SetCustomInputIndex(0);

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	if (sc.Index < sc.DataStartIndex)
		return;

// 	Momentum[sc.Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] 
// 						/ sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()]) * 100;

	const int SelectedIndex = MomentumType.GetIndex();
	switch (SelectedIndex)
	{
		case 0:
		{
			Momentum[sc.Index] = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] - sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()];
			CenterLine[sc.Index] = 0.0;
		}
		break;

		case 1:
		{
			Momentum[sc.Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] / sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()]) * 100;
			CenterLine[sc.Index] = 100.0;
		}
		break;
	}
}

/*==========================================================================*/
SCSFExport scsf_OnBalanceOpenInterest(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBOI = sc.Subgraph[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Open Interest";

		sc.AutoLoop = 1;

		OBOI.Name = "OBOI";
		OBOI.DrawStyle = DRAWSTYLE_LINE;
		OBOI.PrimaryColor = RGB(0,255,0);
		OBOI.DrawZeros = true;

		//Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = 1;

	if (sc.Index == 0)
		return;

	if (sc.Close[sc.Index] > sc.Close[sc.Index - 1])
		OBOI[sc.Index] = OBOI[sc.Index - 1] + sc.OpenInterest[sc.Index];
	else if (sc.Close[sc.Index] < sc.Close[sc.Index - 1])
		OBOI[sc.Index] = OBOI[sc.Index - 1] - sc.OpenInterest[sc.Index];
	else
		OBOI[sc.Index] = OBOI[sc.Index - 1];
}

/*==========================================================================*/
SCSFExport scsf_RateOfChangePercentage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ROC = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];
	SCInputRef MultFactor = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Rate Of Change - Percentage";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		ROC.Name = "ROC";
		ROC.DrawStyle = DRAWSTYLE_LINE;
		ROC.PrimaryColor = RGB(0,255,0);
		ROC.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MultFactor.Name = "Multiplication Factor";
		MultFactor.SetFloat(100.0f);
		MultFactor.SetFloatLimits(1.0f, (float)MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt();

	if (sc.Index < sc.DataStartIndex)
		return;

	ROC[sc.Index] = (sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] - 
					sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()]) /
					sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()] * MultFactor.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_KeltnerChannel(SCStudyInterfaceRef sc)
{
	SCSubgraphRef KeltnerAverage = sc.Subgraph[0];
	SCSubgraphRef TopBand = sc.Subgraph[1];
	SCSubgraphRef BottomBand = sc.Subgraph[2];
	SCSubgraphRef AtrTemp3 = sc.Subgraph[3];
	SCSubgraphRef AtrTemp4 = sc.Subgraph[4];

	SCInputRef InputData = sc.Input[0];
	SCInputRef KeltnerMALength = sc.Input[3];
	SCInputRef TrueRangeAvgLength = sc.Input[4];
	SCInputRef TopBandMult = sc.Input[5];
	SCInputRef BottomBandMult = sc.Input[6];
	SCInputRef KeltnerMAType = sc.Input[7];
	SCInputRef ATR_MAType = sc.Input[8];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Keltner Channel";

		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		KeltnerAverage.Name = "Keltner Average";
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
		InputData.SetInputDataIndex(SC_OHLC_AVG);

		KeltnerMALength.Name = "Keltner Mov Avg Length";
		KeltnerMALength.SetInt(10);
		KeltnerMALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		TrueRangeAvgLength.Name = "True Range Avg Length";
		TrueRangeAvgLength.SetInt(10);
		TrueRangeAvgLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		TopBandMult.Name = "Top Band Multiplier";
		TopBandMult.SetFloat(1.6f);

		BottomBandMult.Name="Bottom Band Multiplier";
		BottomBandMult.SetFloat(1.6f);

		KeltnerMAType.Name = "Keltner Mov Avg Type (Center line)";
		KeltnerMAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		ATR_MAType.Name = "ATR Mov Avg Type";
		ATR_MAType.SetMovAvgType(MOVAVGTYPE_WILDERS);

		return;
	}

	sc.DataStartIndex = max(KeltnerMALength.GetInt(), TrueRangeAvgLength.GetInt());

	sc.MovingAverage(sc.BaseDataIn[(int)InputData.GetInputDataIndex()], KeltnerAverage, KeltnerMAType.GetMovAvgType(), KeltnerMALength.GetInt());
	
	sc.ATR(sc.BaseDataIn, AtrTemp3, TrueRangeAvgLength.GetInt(), ATR_MAType.GetMovAvgType());
	sc.ATR(sc.BaseDataIn, AtrTemp4, TrueRangeAvgLength.GetInt(), ATR_MAType.GetMovAvgType());

	TopBand[sc.Index] = AtrTemp3[sc.Index] * TopBandMult.GetFloat() + KeltnerAverage[sc.Index];
	BottomBand[sc.Index] = KeltnerAverage[sc.Index] - (AtrTemp4[sc.Index] * BottomBandMult.GetFloat());
}

/*==========================================================================*/
SCSFExport scsf_OnBalanceVolumeShortTerm(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBV = sc.Subgraph[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Volume - Short Term";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		OBV.Name = "OBV Length";
		OBV.DrawStyle = DRAWSTYLE_LINE;
		OBV.PrimaryColor = RGB(0,255,0);
		OBV.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.OnBalanceVolumeShortTerm(sc.BaseDataIn, OBV, Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_RSI(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSI = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef RSIAvg = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef RSILength = sc.Input[3];
	SCInputRef RSI_MALength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef AvgType = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "RSI";
		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		RSI.Name = "RSI";
		RSI.DrawStyle = DRAWSTYLE_LINE;
		RSI.PrimaryColor = RGB(0,255,0);
		RSI.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros = true;
		
		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = true;
		
		RSIAvg.Name = "RSI Avg";
		RSIAvg.DrawStyle = DRAWSTYLE_LINE;
		RSIAvg.PrimaryColor = RGB(255,127,0);
		RSIAvg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		RSILength.Name = "RSI Length";
		RSILength.SetInt(10);
		RSILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RSI_MALength.Name = "RSI Mov Avg Length";
		RSI_MALength.SetInt(3);
		RSI_MALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70.0f);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30.0f);

	
		AvgType.Name = "Average Type";
		AvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		return;
	}

	sc.DataStartIndex = (RSILength.GetInt() + RSI_MALength.GetInt());

	sc.RSI(sc.BaseDataIn[InputData.GetInputDataIndex()], RSI, AvgType.GetMovAvgType(), RSILength.GetInt());
	sc.MovingAverage(RSI, RSIAvg, AvgType.GetMovAvgType(), RSI_MALength.GetInt());

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_AdaptiveRSIMovingAverage(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ARSI = sc.Subgraph[0];
	SCSubgraphRef RSI  = sc.Subgraph[1];  // subgraph and arrays used for base RSI calc

	SCFloatArrayRef Price       = ARSI.Arrays[0];
	SCFloatArrayRef RSISmoothed = ARSI.Arrays[1];

	SCInputRef In_ARSIPeriod           = sc.Input[0];
	SCInputRef In_ARSIMAType           = sc.Input[1];
	SCInputRef In_InputData            = sc.Input[2];
	SCInputRef In_PriceSetSmoothing    = sc.Input[3];
	SCInputRef In_PriceSmoothingPeriod = sc.Input[4];
	SCInputRef In_PriceSmoothingMAType = sc.Input[5];
	SCInputRef In_RSISetSmoothing      = sc.Input[6];
	SCInputRef In_RSISmoothingPeriod   = sc.Input[7];
	SCInputRef In_RSISmoothingMAType   = sc.Input[8];
	SCInputRef In_ARSIScaleFactor      = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Adaptive RSI Moving Average With Smoothing";

		// Data for the "Display Study Documentation" button
		sc.StudyDescription = "Adaptive RSI Moving Average With Smoothing. Traditional Adaptive RSI (ARSI) uses straight price and then RSI as the scaling factor without smoothing. The \"Set Price Smoothing?\" option enables cleaning up the price before calculations. If price is really noisy, recommended settings are EMA 3-10. Otherwise leave price smoothing off. The \"Set RSI Smoothing?\" option enables cleaning up some of the RSI noise before ARSI is calculated. It is recommended to smooth RSI out with an EMA 3-7. This allows for less volatility and more consistent movements.<p>To use traditional ARSI without any enhancements, turn off both smoothing options.<p>Observations: When there's a strong reversal, ARSI will go flat and then bend to follow the price. On weaker events, ARSI will go flat while price bounces around above or below and then returns to the ARSI line. Check indicators and wave counts to verify final motion.<p>To visualize what the scaling factor is doing, a separate RSI graph can be added with the selected smoothing values. When RSI is close to 50, the ARSI line will go flat and move less. Further away from 50 will increase the line movement.";

		sc.StudyVersion = 1;
		sc.GraphRegion = 0;	// main price graph
		
		sc.AutoLoop = 1;	// auto looping is enabled

		// subgraphs
		ARSI.Name = "ARSI";
		ARSI.PrimaryColor = RGB(0, 0, 255);
		ARSI.DrawStyle = DRAWSTYLE_LINE;
		ARSI.LineStyle = LINESTYLE_SOLID;
		ARSI.LineWidth = 2;
		ARSI.DrawZeros = 0;

		// inputs
		In_ARSIPeriod.Name = "ARSI Period";
		In_ARSIPeriod.SetInt(35);
		In_ARSIPeriod.SetIntLimits(3, 300);
		In_ARSIPeriod.SetDescription("Number of bars for the period. Useful values are 25 and higher.");

		In_ARSIMAType.Name = "ARSI Moving Average Type";
		In_ARSIMAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		In_ARSIMAType.SetDescription("Moving average for the base RSI calculation.");

		In_ARSIScaleFactor.Name = "ARSI Scale Factor";
		In_ARSIScaleFactor.SetFloat(1.0f);
		In_ARSIScaleFactor.SetFloatLimits(0.1f, 200.0f);
		In_ARSIScaleFactor.SetDescription("Additional Scale Factor for Adaptive calculation.");

		In_InputData.Name = "Input Data";
		In_InputData.SetInputDataIndex(SC_LAST); //default to bar close.
		In_InputData.SetDescription("Usually \"Last\" for bar close when viewing stock prices. One of the averages can be used for forex and other less time synchronized data.");

		In_PriceSetSmoothing.Name = "Set Price Smoothing";
		In_PriceSetSmoothing.SetYesNo(0);
		In_PriceSetSmoothing.SetDescription("Enables extended price smoothing options.");

		In_PriceSmoothingPeriod.Name = "Price Smoothing Period";
		In_PriceSmoothingPeriod.SetInt(3);
		In_PriceSmoothingPeriod.SetIntLimits(2, 50);
		In_PriceSmoothingPeriod.SetDescription("Number of bars for the period. Usually 3-10.");

		In_PriceSmoothingMAType.Name = "Price Smoothing Moving Average Type";
		In_PriceSmoothingMAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		In_PriceSmoothingMAType.SetDescription("Usually EMA, but others can be experimented with.");

		In_RSISetSmoothing.Name = "Set RSI Smoothing";
		In_RSISetSmoothing.SetYesNo(1);
		In_RSISetSmoothing.SetDescription("Enables extended RSI smoothing options.");

		In_RSISmoothingPeriod.Name = "RSI Smoothing Period";
		In_RSISmoothingPeriod.SetInt(5);
		In_RSISmoothingPeriod.SetIntLimits(2, 50);
		In_RSISmoothingPeriod.SetDescription("Number of bars for the period. Usually 3-7.");

		In_RSISmoothingMAType.Name = "RSI Smoothing Moving Average Type";
		In_RSISmoothingMAType.SetMovAvgType(MOVAVGTYPE_EXPONENTIAL);
		In_RSISmoothingMAType.SetDescription("Usually EMA, but others can be experimented with.");
		
		//Set printed output value format. Number is the index of the selected format.
		sc.ValueFormat = sc.BaseGraphValueFormat;

		return;
	}

	if (sc.Index == 0) 
	{
		// Set the index of the first array element to begin drawing at
		sc.DataStartIndex = In_ARSIPeriod.GetInt() - 1;
	}

	// Handle price smoothing.
	if (In_PriceSetSmoothing.GetYesNo()) 
	{
		sc.MovingAverage(sc.BaseDataIn[In_InputData.GetInputDataIndex()], Price, In_PriceSmoothingMAType.GetMovAvgType(), In_PriceSmoothingPeriod.GetInt() );
	}
	else 
	{
		Price[sc.Index] = sc.BaseDataIn[In_InputData.GetInputDataIndex()][sc.Index];
	}

	// Not enough data yet. Pre-load existing data into array for later calculations.
	if (sc.Index < In_ARSIPeriod.GetInt()) 
	{
		ARSI[sc.Index] = Price[sc.Index];
		return;
	}

	// Do the ARSI calculations.

	sc.RSI(Price, RSI, In_ARSIMAType.GetMovAvgType(), In_ARSIPeriod.GetInt());

	float ScalingFactor=0;

	if (In_RSISetSmoothing.GetYesNo()) 
	{
		sc.MovingAverage(RSI, RSISmoothed, In_RSISmoothingMAType.GetMovAvgType(), In_RSISmoothingPeriod.GetInt());
		ScalingFactor = ((RSISmoothed[sc.Index] / 100.0f) - 0.5f) * 2.0f; //range -1.0 to 1.0
		//Note that a MA may overshoot the range a little. For this it doesn't matter.
	}
	else 
	{
		ScalingFactor = ((RSI[sc.Index] / 100.0f) - 0.5f) * 2.0f; //range -1.0 to 1.0
	}

	ScalingFactor *= In_ARSIScaleFactor.GetFloat();

	if (ScalingFactor < 0.0f) 
	{
		ScalingFactor *= -1.0f;	//range must be positive for the next calculation
	}

	ARSI[sc.Index] = ((Price[sc.Index] - ARSI[sc.Index-1]) * ScalingFactor) + ARSI[sc.Index-1];
}

/*==========================================================================*/
SCSFExport scsf_RSI_W(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RSIW = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef RSIWAvg = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef RSILength = sc.Input[3];
	SCInputRef RSI_MALength = sc.Input[4];
	SCInputRef Line1Value = sc.Input[5];
	SCInputRef Line2Value = sc.Input[6];
	SCInputRef AvgType = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "RSI-W";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		RSIW.Name = "RSI-W";
		RSIW.DrawStyle = DRAWSTYLE_LINE;
		RSIW.PrimaryColor = RGB(0,255,0);
		RSIW.DrawZeros = true;

		Line1.Name = "Line1";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(255,0,255);
		Line1.DrawZeros = true;
		
		Line2.Name = "Line2";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(255,255,0);
		Line2.DrawZeros = true;
		
		RSIWAvg.Name = "RSI-W Avg";
		RSIWAvg.DrawStyle = DRAWSTYLE_LINE;
		RSIWAvg.PrimaryColor = RGB(255,127,0);
		RSIWAvg.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		RSILength.Name = "RSI Length";
		RSILength.SetInt(10);
		RSILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RSI_MALength.Name = "RSI Mov Avg Length";
		RSI_MALength.SetInt(3);
		RSI_MALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		Line1Value.Name = "Line1 Value";
		Line1Value.SetFloat(70.0f);

		Line2Value.Name = "Line2 Value";
		Line2Value.SetFloat(30.0f);

		AvgType.Name = "Average Type";
		AvgType.SetMovAvgType(MOVAVGTYPE_WILDERS);

		return;
	}

	sc.DataStartIndex = (RSILength.GetInt() + RSI_MALength.GetInt());


	sc.RSI(sc.BaseDataIn[InputData.GetInputDataIndex()], RSIW, AvgType.GetMovAvgType(), RSILength.GetInt());

	sc.MovingAverage(RSIW, RSIWAvg, AvgType.GetMovAvgType(), RSI_MALength.GetInt());

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_AccumulationDistributionFlow(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AccumulationDistribution = sc.Subgraph[0];
	SCSubgraphRef ADFlowAvg = sc.Subgraph[1];
	SCInputRef MALength = sc.Input[3];
	SCInputRef UsePrevClose = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Accumulation/Distribution Flow";

		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		AccumulationDistribution.Name = "A/D Flow";
		AccumulationDistribution.DrawStyle = DRAWSTYLE_LINE;
		AccumulationDistribution.PrimaryColor = RGB(0,255,0);
		AccumulationDistribution.DrawZeros = true;

		ADFlowAvg.Name = "A/D Flow Avg";
		ADFlowAvg.DrawStyle = DRAWSTYLE_LINE;
		ADFlowAvg.PrimaryColor = RGB(255,0,255);
		ADFlowAvg.DrawZeros = true;

		MALength.Name = "Moving Average Length";
		MALength.SetInt(10);
		MALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		UsePrevClose.Name = "Use Previous Close";
		UsePrevClose.SetYesNo(true);

		return;
	}

	sc.DataStartIndex = MALength.GetInt();

	if (sc.Index == 0)
	{
		AccumulationDistribution[sc.Index] = 5000.0f;
		return;
	}

	float OpenOrClose = 0.0f;
	if (UsePrevClose.GetYesNo())
	{
		OpenOrClose = sc.Close[sc.Index - 1];
	}
	else
	{
		OpenOrClose = sc.Open[sc.Index];
	}

	if (sc.High[sc.Index] - sc.Low[sc.Index] == 0.0f)
	{
		AccumulationDistribution[sc.Index] = AccumulationDistribution[sc.Index - 1];
	}
	else
	{
		AccumulationDistribution[sc.Index] = ((sc.Close[sc.Index] - OpenOrClose) / (sc.High[sc.Index] - sc.Low[sc.Index]) *
			sc.Volume[sc.Index]) + AccumulationDistribution[sc.Index - 1];
	}

	sc.SimpleMovAvg(AccumulationDistribution, ADFlowAvg, MALength.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_AccumulationDistribution(SCStudyInterfaceRef sc)
{


	//Subgraph references
	SCSubgraphRef AccumulationDistribution = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Accumulation/Distribution";

		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		AccumulationDistribution.Name = "A/D";
		AccumulationDistribution.DrawStyle = DRAWSTYLE_LINE;
		AccumulationDistribution.PrimaryColor = RGB(0,255,0);
		AccumulationDistribution.DrawZeros = true;

		return;
	}

	sc.DataStartIndex = 0;

	float RangeVolume = (sc.High[sc.Index] - sc.Low[sc.Index]) * sc.Volume[sc.Index];

	//In the case where RangeVolume equals 0 at sc.Index = 0, then AccumulationDistribution[sc.Index-1] is going to equal 0. Otherwise, it is the prior AccumulationDistribution value.
	if(RangeVolume == 0)
		AccumulationDistribution[sc.Index] = AccumulationDistribution[sc.Index - 1];
	else
		// Accumulation/Distribution = ((Close – Low) – (High – Close)) / (High – Low) * Bar Volume
		AccumulationDistribution[sc.Index] = ((sc.Close[sc.Index] - sc.Low[sc.Index]) - (sc.High[sc.Index] - sc.Close[sc.Index])) /  RangeVolume;



}


/*==========================================================================*/
SCSFExport scsf_VolumeBarRangeRatio(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Ratio = sc.Subgraph[0];

	SCFloatArrayRef High = sc.High;
	SCFloatArrayRef Low = sc.Low;
	SCFloatArrayRef Volume = sc.Volume;

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume Bar Range Ratio";

		sc.StudyDescription = "Formula: Volume / (High - Low)";
		sc.AutoLoop = 1;

		Ratio.Name = "Ratio";
		Ratio.DrawStyle = DRAWSTYLE_BAR;
		Ratio.PrimaryColor = RGB(0,255,0);

		return;
	}

	float Range = High[sc.Index] - Low[sc.Index];

	if (Range>0.0)
		Ratio[sc.Index] = Volume[sc.Index] / Range;
	else//If we have no range, then use the prior calculated value.
		Ratio[sc.Index] = Ratio[sc.Index-1];
	
}

/*==========================================================================*/
SCSFExport scsf_MovingAverages(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg1 = sc.Subgraph[0];
	SCSubgraphRef Avg2 = sc.Subgraph[1];
	SCSubgraphRef Avg3 = sc.Subgraph[2];
	
	SCInputRef MAType1 = sc.Input[0];
	SCInputRef InputData1 = sc.Input[1];
	SCInputRef Length1 = sc.Input[2];

	SCInputRef MAType2 = sc.Input[3];
	SCInputRef InputData2 = sc.Input[4];
	SCInputRef Length2 = sc.Input[5];

	SCInputRef MAType3 = sc.Input[6];
	SCInputRef InputData3 = sc.Input[7];
	SCInputRef Length3 = sc.Input[8];

	SCInputRef Version = sc.Input[12];
	
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Averages";
		
		sc.GraphRegion = 0;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;
		
		Avg1.Name = "Avg1";
		Avg1.DrawStyle = DRAWSTYLE_LINE;
		Avg1.PrimaryColor = RGB(0,255,0);

		Avg2.Name = "Avg2";
		Avg2.DrawStyle = DRAWSTYLE_LINE;
		Avg2.PrimaryColor = RGB(255,0,255);

		Avg3.Name = "Avg3";
		Avg3.DrawStyle = DRAWSTYLE_LINE;
		Avg3.PrimaryColor = RGB(255,255,0);
		
		MAType1.Name = "Moving Average Type 1";
		MAType1.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		InputData1.Name = "Input Data 1";
		InputData1.SetInputDataIndex(SC_LAST);
		
		Length1.Name = "Length 1";
		Length1.SetInt(5);
		Length1.SetIntLimits(1, INT_MAX);
		
		MAType2.Name = "Moving Average Type 2";
		MAType2.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		InputData2.Name = "Input Data 2";
		InputData2.SetInputDataIndex(SC_LAST);
		
		Length2.Name = "Length 2";
		Length2.SetInt(10);
		Length2.SetIntLimits(1, INT_MAX);
		
		MAType3.Name = "Moving Average Type 3";
		MAType3.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		
		InputData3.Name = "Input Data 3";
		InputData3.SetInputDataIndex(SC_LAST);
		
		Length3.Name = "Length 3";
		Length3.SetInt(15);
		Length3.SetIntLimits(1, INT_MAX);
		
		Version.SetInt(2);
		
		return;
	}


	if (Version.GetInt() < 2)
	{
		uint32_t OldInputData = sc.Input[0].GetInputDataIndex();
		int OldLength1 = sc.Input[3].GetInt();
		int OldLength2 = sc.Input[4].GetInt();
		int OldLength3 = sc.Input[5].GetInt();
		uint32_t OldMAtype = sc.Input[7].GetMovAvgType();

		MAType1.SetMovAvgType(OldMAtype);
		InputData1.SetInputDataIndex(OldInputData);
		Length1.SetInt(OldLength1);

		MAType2.SetMovAvgType(OldMAtype);
		InputData2.SetInputDataIndex(OldInputData);
		Length2.SetInt(OldLength2);

		MAType3.SetMovAvgType(OldMAtype);
		InputData3.SetInputDataIndex(OldInputData);
		Length3.SetInt(OldLength3);

		Version.SetInt(2);
	}

	// 
	if(Length1.GetInt()  <= 0)
	{
		Length1.SetInt(10);
		MAType1.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		InputData1.SetInputDataIndex(SC_LAST);
	}

	if(Length2.GetInt() <= 0)
	{
		Length2.SetInt(20);
		MAType2.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		InputData2.SetInputDataIndex(SC_LAST);
	}

	if(Length3.GetInt()  <= 0)
	{
		Length3.SetInt(50);
		MAType3.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		InputData3.SetInputDataIndex(SC_LAST);
	}

	

	
	sc.DataStartIndex = max(Length3.GetInt(), max(Length1.GetInt(), Length2.GetInt()));
	
	sc.MovingAverage(sc.BaseDataIn[InputData1.GetInputDataIndex()], Avg1, MAType1.GetMovAvgType(), Length1.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData2.GetInputDataIndex()], Avg2, MAType2.GetMovAvgType(), Length2.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData3.GetInputDataIndex()], Avg3, MAType3.GetMovAvgType(), Length3.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageCrossover(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Avg1 = sc.Subgraph[0];
	SCSubgraphRef Avg2 = sc.Subgraph[1];
	SCSubgraphRef CrossFromBottom = sc.Subgraph[2];
	SCSubgraphRef CrossFromTop = sc.Subgraph[3];

	SCInputRef MAType1 = sc.Input[0];
	SCInputRef InputData1 = sc.Input[1];
	SCInputRef Length1 = sc.Input[2];

	SCInputRef MAType2 = sc.Input[3];
	SCInputRef InputData2 = sc.Input[4];
	SCInputRef Length2 = sc.Input[5];


	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average Crossover";

		sc.GraphRegion = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.AutoLoop = 1;

		Avg1.Name = "Avg1";
		Avg1.DrawStyle = DRAWSTYLE_LINE;
		Avg1.PrimaryColor = RGB(0,255,0);

		Avg2.Name = "Avg2";
		Avg2.DrawStyle = DRAWSTYLE_LINE;
		Avg2.PrimaryColor = RGB(255,0,255);

		CrossFromBottom.Name = "Cross From Bottom";
		CrossFromBottom.DrawStyle = DRAWSTYLE_ARROW_UP;
		CrossFromBottom.PrimaryColor = RGB(0,255,0);
		CrossFromBottom.LineWidth = 2;

		CrossFromTop.Name = "Cross From Top";
		CrossFromTop.DrawStyle = DRAWSTYLE_ARROW_DOWN;
		CrossFromTop.PrimaryColor = RGB(255,0,255);
		CrossFromTop.LineWidth = 2;

		MAType1.Name = "Moving Average Type 1";
		MAType1.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputData1.Name = "Input Data 1";
		InputData1.SetInputDataIndex(SC_LAST);

		Length1.Name = "Length 1";
		Length1.SetInt(10);
		Length1.SetIntLimits(1, MAX_STUDY_LENGTH);

		MAType2.Name = "Moving Average Type 2";
		MAType2.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		InputData2.Name = "Input Data 2";
		InputData2.SetInputDataIndex(SC_LAST);

		Length2.Name = "Length 2";
		Length2.SetInt(20);
		Length2.SetIntLimits(1, MAX_STUDY_LENGTH);
		return;
	}


	sc.DataStartIndex = max(Length1.GetInt(), Length2.GetInt());

	sc.MovingAverage(sc.BaseDataIn[InputData1.GetInputDataIndex()], Avg1, MAType1.GetMovAvgType(), Length1.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData2.GetInputDataIndex()], Avg2, MAType2.GetMovAvgType(), Length2.GetInt());

	if(Length1.GetInt() <= Length2.GetInt() && sc.CrossOver(Avg1, Avg2) == CROSS_FROM_TOP)//Sell
	{
		CrossFromBottom[sc.Index] = 0;
		CrossFromTop[sc.Index] = sc.High[sc.Index];
	}
	else if(Length1.GetInt() <= Length2.GetInt() && sc.CrossOver(Avg1, Avg2) == CROSS_FROM_BOTTOM)//Buy
	{
		CrossFromBottom[sc.Index] = sc.Low[sc.Index];
		CrossFromTop[sc.Index] = 0;
	}
	else if(Length1.GetInt() > Length2.GetInt() && sc.CrossOver(Avg1, Avg2) == CROSS_FROM_TOP)//Buy
	{
		CrossFromBottom[sc.Index] = sc.Low[sc.Index];
		CrossFromTop[sc.Index] = 0;
	}
	else if(Length1.GetInt() > Length2.GetInt() && sc.CrossOver(Avg1, Avg2) == CROSS_FROM_BOTTOM)//Sell
	{
		CrossFromBottom[sc.Index] = 0;
		CrossFromTop[sc.Index] = sc.High[sc.Index];
	}	
	else 
	{
		CrossFromBottom[sc.Index] = 0;
		CrossFromTop[sc.Index] = 0;
	}
}
/*==========================================================================*/
SCSFExport scsf_STIX(SCStudyInterfaceRef sc)
{
	SCSubgraphRef STIX = sc.Subgraph[0];
	SCInputRef DecliningIssuesChartNumber = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "STIX";
		sc.GraphRegion = 1;
		sc.ValueFormat = 0;

		STIX.Name = "STIX";
		STIX.DrawStyle = DRAWSTYLE_LINE;
		STIX.PrimaryColor = RGB(0,255,0);
		STIX.DrawZeros = true;

		DecliningIssuesChartNumber.Name = "Declining Issues Chart #";
		DecliningIssuesChartNumber.SetChartNumber(1);

		return;
	}

	SCGraphData ChartBaseData;
	sc.GetChartBaseData(-DecliningIssuesChartNumber.GetChartNumber(), ChartBaseData);
	
	SCFloatArrayRef Chart2Array = ChartBaseData[SC_LAST];
	
	if (Chart2Array.GetArraySize() == 0)
		return;

	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		int NearestMatchIndex = sc.GetNearestMatchForDateTimeIndex(DecliningIssuesChartNumber.GetChartNumber(), BarIndex);

		if (sc.Close[BarIndex] == 0 || Chart2Array[NearestMatchIndex] == 0)
		{
			STIX[BarIndex] = STIX[BarIndex - 1];
		}
		else
		{
			STIX[BarIndex] = sc.Close[BarIndex] / (sc.Close[BarIndex] + Chart2Array[NearestMatchIndex]) * 100.0f * 0.09f + 
				STIX[BarIndex - 1] * 0.91f;
		}
	}
}

/*==========================================================================*/
SCSFExport scsf_AwesomeOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef AO = sc.Subgraph[0];
	SCSubgraphRef ZeroLine = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef MA1Length = sc.Input[2];
	SCInputRef MA2Length = sc.Input[3];
	SCInputRef MAType = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Awesome Oscillator";

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

	sc.DataStartIndex = max(MA1Length.GetInt(), MA2Length.GetInt()) - 1;



	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], AO.Arrays[0], MAType.GetMovAvgType(), MA1Length.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], AO.Arrays[1], MAType.GetMovAvgType(), MA2Length.GetInt());

	AO.Data[sc.Index] = AO.Arrays[1][sc.Index] - AO.Arrays[0][sc.Index];

}

/*==========================================================================*/
SCSFExport scsf_MovingAverageDifference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MovAvgDiff = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef MA1Length = sc.Input[2];
	SCInputRef MA2Length = sc.Input[3];
	SCInputRef MovAvgType = sc.Input[4];
	SCInputRef Version = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average Difference";
	
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;

		MovAvgDiff.Name = "MovAvg Diff";
		MovAvgDiff.DrawStyle = DRAWSTYLE_BAR;
		MovAvgDiff.SecondaryColor = RGB(255,0,0);
		MovAvgDiff.AutoColoring = AUTOCOLOR_SLOPE;
		MovAvgDiff.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_HL_AVG);

		MA1Length.Name = "Moving Average 1 Length";
		MA1Length.SetInt(30);
		MA1Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA2Length.Name = "Moving Average 2 Length";
		MA2Length.SetInt(10);
		MA2Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MovAvgType.Name = "Moving Average Type";
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Version.SetInt(1);

		return;
	}

	if(Version.GetInt() < 1)
	{
		MovAvgType.SetMovAvgType(MOVAVGTYPE_SIMPLE);
		Version.SetInt(1);
	}

	sc.DataStartIndex = max(MA1Length.GetInt(), MA2Length.GetInt());


	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], MovAvgDiff.Arrays[0], MovAvgType.GetMovAvgType(), MA1Length.GetInt());
	sc.MovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], MovAvgDiff.Arrays[1], MovAvgType.GetMovAvgType(), MA2Length.GetInt());
	MovAvgDiff[sc.Index] = MovAvgDiff.Arrays[0][sc.Index] - MovAvgDiff.Arrays[1][sc.Index];
}

/*==========================================================================*/

SCSFExport scsf_Volume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume";

		sc.ValueFormat = 0;
		sc.AutoLoop = true;
		sc.ScaleRangeType = SCALE_ZEROBASED;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.AutoColoring = AUTOCOLOR_BASEGRAPH;
		Volume.PrimaryColor = RGB(0, 255, 0);
		Volume.SecondaryColor = RGB(255, 0, 0);
		Volume.DrawZeros = false;
		Volume.LineWidth = 2;


		sc.DisplayStudyInputValues = false;

		return;
	}


	Volume[sc.Index] = sc.Volume[sc.Index];

}


/*==========================================================================*/

SCSFExport scsf_VolumeWithZeroColor(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];
	SCSubgraphRef NoPriceChangeColor = sc.Subgraph[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume with 0 Color";

		sc.ValueFormat = 0;
		sc.AutoLoop = true;
		sc.ScaleRangeType = SCALE_ZEROBASED;

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_BAR;
		Volume.PrimaryColor = RGB(0, 255, 0);
		Volume.SecondaryColor = RGB(255, 0, 0);
		Volume.SecondaryColorUsed = true;
		Volume.DrawZeros = false;
		Volume.LineWidth = 2;

		NoPriceChangeColor.Name = "No Price Change Color";
		NoPriceChangeColor.DrawStyle = DRAWSTYLE_CUSTOM_TEXT;
		NoPriceChangeColor.PrimaryColor = RGB(0, 255, 0);
		NoPriceChangeColor.DrawZeros = false;
		NoPriceChangeColor.DisplayNameValueInWindowsFlags = 0;
		sc.DisplayStudyInputValues = false;


		return;
	}


	Volume[sc.Index] = sc.Volume[sc.Index]; 

	if(sc.FormattedEvaluate(sc.BaseData[SC_OPEN][sc.Index], sc.BaseGraphValueFormat, LESS_OPERATOR, sc.BaseData[SC_LAST][sc.Index], sc.BaseGraphValueFormat) )
		Volume.DataColor[sc.Index] =	Volume.PrimaryColor;
	else if(sc.FormattedEvaluate(sc.BaseData[SC_OPEN][sc.Index], sc.BaseGraphValueFormat, GREATER_OPERATOR, sc.BaseData[SC_LAST][sc.Index], sc.BaseGraphValueFormat) )
		Volume.DataColor[sc.Index] =	Volume.SecondaryColor;
	else 
		Volume.DataColor[sc.Index] =	NoPriceChangeColor.PrimaryColor;
}

/*==========================================================================*/

SCSFExport scsf_VolumeColoredBasedOnBarCloses(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume-Colored Based on Bar Closes";

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


	if(sc.FormattedEvaluate( sc.Close[sc.Index], sc.BaseGraphValueFormat, GREATER_EQUAL_OPERATOR,  sc.Close[sc.Index - 1], sc.BaseGraphValueFormat))
		Volume.DataColor[sc.Index] = Volume.PrimaryColor;
	else
		Volume.DataColor[sc.Index] = Volume.SecondaryColor;

}

/*==========================================================================*/

SCSFExport scsf_VolumeWithNegativeDownVolume(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Volume = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Volume - With Negative Down Volume";

		sc.ValueFormat = 0;

		sc.AutoLoop = true;
		sc.ScaleRangeType = SCALE_AUTO;

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


	if(sc.Close[sc.Index] >=  sc.Open[sc.Index])
	{
		Volume[sc.Index] = sc.Volume[sc.Index];
		Volume.DataColor[sc.Index] = Volume.PrimaryColor;
	}
	else
	{
		Volume[sc.Index] = sc.Volume[sc.Index] * -1;
		Volume.DataColor[sc.Index] = Volume.SecondaryColor;
	}

}

/*==========================================================================*/

SCSFExport scsf_Spread3Chart(SCStudyInterfaceRef sc)
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

	SCInputRef InChart2Number = sc.Input[3];
	SCInputRef InChart3Number = sc.Input[4];
	SCInputRef InChart1Multiplier = sc.Input[5];
	SCInputRef InChart2Multiplier = sc.Input[6];
	SCInputRef InChart3Multiplier = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Spread - 3 Chart";

		sc.AutoLoop = 0;

		sc.ValueFormat = 2;
		sc.GraphRegion = 1;
		
		sc.UseGlobalChartColors = 0;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;
		
		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.SecondaryColor = RGB(0,255,0);

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.SecondaryColor = RGB(0,255,0);

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.SecondaryColor = RGB(0,255,0);

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);

		
		InChart2Number.Name = "Chart 2 Number";
		InChart2Number.SetChartNumber(1);

		InChart3Number.Name = "Chart 3 Number";
		InChart3Number.SetChartNumber(1);
		
		InChart1Multiplier.Name = "Chart 1 Multiplier";
		InChart1Multiplier.SetFloat(1.0f);
		
		InChart2Multiplier.Name = "Chart 2 Multiplier";
		InChart2Multiplier.SetFloat(1.0f);

		InChart3Multiplier.Name = "Chart 3 Multiplier";
		InChart3Multiplier.SetFloat(1.0f);

		return;
	}
	
	if(sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
	{
		SCString Chart1Name = sc.GetChartName(sc.ChartNumber);
		SCString Chart2Name = sc.GetChartName(InChart2Number.GetChartNumber());
		SCString Chart3Name = sc.GetChartName(InChart3Number.GetChartNumber());
		sc.GraphName.Format("Spread - 3 Chart: %s + %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars(), Chart3Name.GetChars());
	}

	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-InChart2Number.GetChartNumber(), Chart2BaseData);
	
	SCGraphData Chart3BaseData;
	sc.GetChartBaseData(-InChart3Number.GetChartNumber(), Chart3BaseData);
	
	// Iterate through the bars
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(InChart2Number.GetChartNumber(), Index);
		int Chart3Index = sc.GetNearestMatchForDateTimeIndex(InChart3Number.GetChartNumber(), Index);
		
		// Iterate through the subgraphs
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
				= (sc.BaseDataIn[SubgraphIndex][Index] * InChart1Multiplier.GetFloat())
				+ (Chart2BaseData[SubgraphIndex][Chart2Index] * InChart2Multiplier.GetFloat())
				- (Chart3BaseData[SubgraphIndex][Chart3Index] * InChart3Multiplier.GetFloat())
				;
		}
		
		sc.Subgraph[SC_HIGH][Index]
			= max(
				sc.Subgraph[SC_OPEN][Index],
				max(
					sc.Subgraph[SC_HIGH][Index],
					max(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);
		
		sc.Subgraph[SC_LOW][Index]
			= min(
				sc.Subgraph[SC_OPEN][Index],
				min(
					sc.Subgraph[SC_HIGH][Index],
					min(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);
		
		sc.CalculateOHLCAverages(Index);
	}
	
}

/*==========================================================================*/
SCSFExport scsf_SpreadButterfly(SCStudyInterfaceRef sc)
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


	SCInputRef UseLatestSourceDataForLastBar = sc.Input[2];
	SCInputRef InChart2Number = sc.Input[3];
	SCInputRef InChart3Number = sc.Input[4];
	SCInputRef InChart1Multiplier = sc.Input[5];
	SCInputRef InChart2Multiplier = sc.Input[6];
	SCInputRef InChart3Multiplier = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Spread - Butterfly";

		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		sc.UseGlobalChartColors = 0;
		sc.GraphDrawType		= GDT_OHLCBAR;
		sc.StandardChartHeader	= 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(0,255,0);
		High.SecondaryColor = RGB(0,255,0);

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(0,255,0);
		Low.SecondaryColor = RGB(0,255,0);

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(0,255,0);
		Last.SecondaryColor = RGB(0,255,0);

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);

		OpenInterest.Name = "# of Trades / OI";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);


		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(0);

		InChart2Number.Name = "Chart 2 Number";
		InChart2Number.SetChartNumber(1);

		InChart3Number.Name = "Chart 3 Number";
		InChart3Number.SetChartNumber(1);

		InChart1Multiplier.Name = "Chart 1 Multiplier";
		InChart1Multiplier.SetFloat(1.0f);

		InChart2Multiplier.Name = "Chart 2 Multiplier";
		InChart2Multiplier.SetFloat(2.0f);

		InChart3Multiplier.Name = "Chart 3 Multiplier";
		InChart3Multiplier.SetFloat(1.0f);

		return;
	}


	//Formula: Buy1 Sell2 Buy1

	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-InChart2Number.GetChartNumber(), Chart2BaseData);
	
	SCGraphData Chart3BaseData;
	sc.GetChartBaseData(-InChart3Number.GetChartNumber(), Chart3BaseData);
	
	// Iterate through the bars
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(InChart2Number.GetChartNumber(), Index);
		int Chart3Index = sc.GetNearestMatchForDateTimeIndex(InChart3Number.GetChartNumber(), Index);



		//When use latest source data for last bar is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source charts.
		if(UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() &&  Index == sc.ArraySize - 1)
		{
			Chart2Index = Chart2BaseData[SC_OPEN].GetArraySize() - 1;
			Chart3Index = Chart3BaseData[SC_OPEN].GetArraySize() - 1;
		}

		
		// Iterate through the subgraphs
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
				= (sc.BaseDataIn[SubgraphIndex][Index] * InChart1Multiplier.GetFloat())
				- (Chart2BaseData[SubgraphIndex][Chart2Index] * InChart2Multiplier.GetFloat())
				+ (Chart3BaseData[SubgraphIndex][Chart3Index] * InChart3Multiplier.GetFloat())
				;
		}
		
		sc.Subgraph[SC_HIGH][Index]
			= max(
				sc.Subgraph[SC_OPEN][Index],
				max(
					sc.Subgraph[SC_HIGH][Index],
					max(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);
		
		sc.Subgraph[SC_LOW][Index]
			= min(
				sc.Subgraph[SC_OPEN][Index],
				min(
					sc.Subgraph[SC_HIGH][Index],
					min(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);
		
		sc.CalculateOHLCAverages(Index);
	}



	sc.GraphName.Format("Butterfly Spread: +%s - %s + %s", 
		sc.GetChartName(sc.ChartNumber).GetChars(), 
		sc.GetChartName(InChart2Number.GetChartNumber()).GetChars(), 
		sc.GetChartName(InChart3Number.GetChartNumber()).GetChars());
}

/*==========================================================================*/

SCSFExport scsf_Sum3Chart(SCStudyInterfaceRef sc)
{

	SCInputRef InChart2Number = sc.Input[3];
	SCInputRef InChart3Number = sc.Input[4];
	SCInputRef InChart1Multiplier = sc.Input[5];
	SCInputRef InChart2Multiplier = sc.Input[6];
	SCInputRef InChart3Multiplier = sc.Input[7];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Sum - 3 Chart";

		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 1;
		sc.AutoLoop = 0;
		sc.GraphUsesChartColors = 1;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		InChart2Number.Name = "Chart 2 Number";
		InChart2Number.SetChartNumber(1);

		InChart3Number.Name = "Chart 3 Number";
		InChart3Number.SetChartNumber(1);

		InChart1Multiplier.Name = "Chart 1 Multiplier";
		InChart1Multiplier.SetFloat(1.0f);

		InChart2Multiplier.Name = "Chart 2 Multiplier";
		InChart2Multiplier.SetFloat(1.0f);

		InChart3Multiplier.Name = "Chart 3 Multiplier";
		InChart3Multiplier.SetFloat(1.0f);

		return;
	}

	if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
	{
		for (int SubgraphIndex = 0; SubgraphIndex <= NUM_BASE_GRAPH_ARRAYS; ++SubgraphIndex)
		{
			sc.Subgraph[SubgraphIndex].Name = sc.GetStudySubgraphName(0, SubgraphIndex);
		}

		SCString Chart1Name = sc.GetChartName(sc.ChartNumber);
		SCString Chart2Name = sc.GetChartName(InChart2Number.GetChartNumber());
		SCString Chart3Name = sc.GetChartName(InChart3Number.GetChartNumber());
		sc.GraphName.Format("Sum - 3 Chart: %s + %s + %s", Chart1Name.GetChars(), Chart2Name.GetChars(), Chart3Name.GetChars());

	}

	SCGraphData Chart2BaseData;
	sc.GetChartBaseData(-InChart2Number.GetChartNumber(), Chart2BaseData);

	SCGraphData Chart3BaseData;
	sc.GetChartBaseData(-InChart3Number.GetChartNumber(), Chart3BaseData);

	// Iterate through the bars
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(InChart2Number.GetChartNumber(), Index);
		int Chart3Index = sc.GetNearestMatchForDateTimeIndex(InChart3Number.GetChartNumber(), Index);

		// Iterate through the subgraphs
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index]
				= (sc.BaseDataIn[SubgraphIndex][Index] * InChart1Multiplier.GetFloat())
				+ (Chart2BaseData[SubgraphIndex][Chart2Index] * InChart2Multiplier.GetFloat())
				+ (Chart3BaseData[SubgraphIndex][Chart3Index] * InChart3Multiplier.GetFloat())
				;
		}

		sc.Subgraph[SC_HIGH][Index]
			= max(
				sc.Subgraph[SC_OPEN][Index],
				max(
					sc.Subgraph[SC_HIGH][Index],
					max(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);

		sc.Subgraph[SC_LOW][Index]
			= min(
				sc.Subgraph[SC_OPEN][Index],
				min(
					sc.Subgraph[SC_HIGH][Index],
					min(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);

		sc.CalculateOHLCAverages(Index);
	}

}
/*==========================================================================*/
SCSFExport scsf_HighestHighLowestLowOverNBars(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighestHigh = sc.Subgraph[0];
	SCSubgraphRef LowestLow = sc.Subgraph[1];

	SCInputRef Length = sc.Input[3];
	SCInputRef InputDataHigh = sc.Input[5];
	SCInputRef InputDataLow = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Highest High / Lowest Low Over N Bars";

		sc.GraphRegion = 0;
		sc.AutoLoop = 0;

		HighestHigh.Name= "Highest High";
		HighestHigh.DrawStyle = DRAWSTYLE_LINE;
		HighestHigh.PrimaryColor = RGB(0,255,0);
		HighestHigh.DrawZeros = true;

		LowestLow.Name = "Lowest Low";
		LowestLow.DrawStyle = DRAWSTYLE_LINE;
		LowestLow.PrimaryColor = RGB(255,0,255);
		LowestLow.DrawZeros = true;

		Length.Name = "Length";
		Length.SetInt(5);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		InputDataHigh.Name = "Input Data High";
		InputDataHigh.SetInputDataIndex(SC_HIGH);

		InputDataLow.Name = "Input Data Low";
		InputDataLow.SetInputDataIndex(SC_LOW);

		return;
	}

	sc.DataStartIndex = Length.GetInt()  - 1;


	int & HighestIndex = sc.GetPersistentInt(1);
	int & LowestIndex = sc.GetPersistentInt(2);
	float & CurrentHighest = sc.GetPersistentFloat(1);
	float & CurrentLowest = sc.GetPersistentFloat(2);
	if (sc.UpdateStartIndex== 0)
	{
		HighestIndex = -1;
		LowestIndex = -1;
		CurrentHighest = -FLT_MAX;
		CurrentLowest = FLT_MAX;
	}

	int HighDataIndex =  InputDataHigh.GetInputDataIndex();
	int LowDataIndex = InputDataLow.GetInputDataIndex();
	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		int EarliestIndex = BarIndex - (Length.GetInt() - 1);
		bool NeedToDetermineHigh =
			(HighestIndex < EarliestIndex || sc.BaseDataIn[HighDataIndex][BarIndex] > CurrentHighest);

		bool NeedToDetermineLow =
			(LowestIndex < EarliestIndex || sc.BaseDataIn[LowDataIndex][BarIndex] < CurrentLowest);

		if (NeedToDetermineHigh || NeedToDetermineLow)
		{
			if (NeedToDetermineHigh)
				CurrentHighest = -FLT_MAX;

			if (NeedToDetermineLow)
				CurrentLowest = FLT_MAX;

			for (int PriorBarIndex = EarliestIndex; PriorBarIndex <= BarIndex; PriorBarIndex++)
			{
				if (NeedToDetermineHigh)
				{
					if (CurrentHighest < sc.BaseDataIn[HighDataIndex][PriorBarIndex])
					{
						CurrentHighest = sc.BaseDataIn[HighDataIndex][PriorBarIndex];
						HighestIndex = PriorBarIndex;
					}
				}

				if (NeedToDetermineLow)
				{
					if (CurrentLowest > sc.BaseDataIn[LowDataIndex][PriorBarIndex])
					{
						CurrentLowest = sc.BaseDataIn[LowDataIndex][PriorBarIndex];
						LowestIndex = PriorBarIndex;
					}
				}
			}

		}
		HighestHigh[BarIndex] = CurrentHighest;
		LowestLow[BarIndex] = CurrentLowest;
	}
}

/*==========================================================================*/
SCSFExport scsf_DonchianChannel(SCStudyInterfaceRef sc)
{
	SCSubgraphRef HighestHigh = sc.Subgraph[0];
	SCSubgraphRef LowestLow = sc.Subgraph[1];
	SCSubgraphRef Midline = sc.Subgraph[2];
	SCInputRef Length = sc.Input[3];
	SCInputRef UseClose = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Donchian Channel";

		sc.GraphRegion = 0;
		sc.AutoLoop = true;

		HighestHigh.Name= "Highest High";
		HighestHigh.DrawStyle=  DRAWSTYLE_LINE;
		HighestHigh.DrawZeros= true;
		HighestHigh.GraphicalDisplacement= 1;

		LowestLow.Name = "Lowest Low";
		LowestLow.DrawStyle=  DRAWSTYLE_LINE;
		LowestLow.DrawZeros= true;
		LowestLow.GraphicalDisplacement= 1;

		Midline.Name = "Mid Line";
		Midline.DrawStyle=  DRAWSTYLE_LINE;
		Midline.LineStyle= LINESTYLE_DOT;
		Midline.DrawZeros= true;
		Midline.GraphicalDisplacement= 1;

		Length.Name = "Length ";
		Length.SetInt(5);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		UseClose.Name = "Use Close instead of High and Low";
		UseClose.SetYesNo(false);

		return;
	}

	float Lowest = FLT_MAX;
	float Highest = -FLT_MAX;

	if (UseClose.GetYesNo())
	{
		for (int BarIndex = sc.Index - (Length.GetInt() - 1); BarIndex <= sc.Index; BarIndex++)
		{
			Highest = (Highest < sc.Close[BarIndex]) ? sc.Close[BarIndex] : Highest;

			Lowest = (Lowest > sc.Close[BarIndex]) ? sc.Close[BarIndex] : Lowest;
		}
	}
	else
	{
		for (int BarIndex = sc.Index - (Length.GetInt() - 1); BarIndex <= sc.Index; BarIndex++)
		{
			if (Highest < sc.High[BarIndex])
				Highest = sc.High[BarIndex];

			if (Lowest > sc.Low[BarIndex])
				Lowest = sc.Low[BarIndex];
		}

	}

	HighestHigh[sc.Index] = Highest;
	LowestLow[sc.Index] = Lowest;
	Midline[sc.Index] = (Highest + Lowest)/2.0f;
}

/*==========================================================================*/
SCSFExport scsf_McClellanOscillator(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MO = sc.Subgraph[0];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];

	SCInputRef DecliningIssuesChartNumber = sc.Input[3];
	SCInputRef UseAbsValue = sc.Input[4];
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "McClellan Oscillator" ;
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;

		MO.Name = "MO";
		MO.DrawStyle = DRAWSTYLE_LINE;
		MO.PrimaryColor = RGB(0,255,0);
		MO.DrawZeros = true;

		DecliningIssuesChartNumber.Name = "Declining Issues Chart #";
		DecliningIssuesChartNumber.SetChartNumber(1);

		UseAbsValue.Name = "Use ABS Value";
		UseAbsValue.SetYesNo(0);

		return;
	}
	
	SCGraphData ChartBaseData;
	sc.GetChartBaseData(-DecliningIssuesChartNumber.GetChartNumber(), ChartBaseData);

	SCFloatArrayRef Chart2Array = ChartBaseData[SC_LAST];
	
	if (Chart2Array.GetArraySize() == 0)
		return;

	int i1 = 0;
	int i = 0;
	sc.DataStartIndex = 1;

	Temp3[i] = Temp2[i] = sc.Close[i] - Chart2Array[i1];
	
	for (i = max(1, sc.UpdateStartIndex); i < sc.ArraySize; i++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(DecliningIssuesChartNumber.GetChartNumber(), i);

		Temp3[i] = Temp3[i - 1] * 0.9f +
			((sc.Close[i] - Chart2Array[i1]) * 0.1f);

		if (UseAbsValue.GetYesNo())
		{
			Temp2[i] = Temp2[i - 1] * 0.95f +
				(fabs(
				(sc.Close[i] - Chart2Array[i1]) *0.05f));
		}
		else
		{
			Temp2[i] = Temp2[i - 1] * 0.95f +
				((
				(sc.Close[i] - Chart2Array[i1]) *0.05f));
		}

		MO[i] = Temp3[i] - Temp2[i];
	}
}

/*==========================================================================*/
SCSFExport scsf_BarDifference(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Difference = sc.Subgraph[0];
	SCInputRef InputData1 = sc.Input[0];
	SCInputRef InputData2 = sc.Input[1];
	SCInputRef InputData2Offset = sc.Input[3];
	SCInputRef InputCalculateDifferenceInPriceTicks = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Bar Difference";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Difference.Name = "Difference";
		Difference.DrawStyle = DRAWSTYLE_LINE;
		Difference.PrimaryColor = RGB(0,255,0);
		Difference.DrawZeros = true;

		InputData1.Name = "Input Data 1";
		InputData1.SetInputDataIndex(SC_HIGH);

		InputData2.Name = "Input Data 2";
		InputData2.SetInputDataIndex(SC_LOW);

		InputData2Offset.Name = "Input Data 2 Offset";
		InputData2Offset.SetInt(0);
		InputData2Offset.SetIntLimits(0, MAX_STUDY_LENGTH);

		InputCalculateDifferenceInPriceTicks.Name = "Calculate Difference in Price Ticks";
		InputCalculateDifferenceInPriceTicks.SetYesNo(false);

		return;
	}

	if (sc.Index < InputData2Offset.GetInt())
		return;

	Difference[sc.Index] = sc.BaseDataIn[InputData1.GetInputDataIndex()][sc.Index] -
		sc.BaseDataIn[InputData2.GetInputDataIndex()][sc.Index - InputData2Offset.GetInt()];

	if (InputCalculateDifferenceInPriceTicks.GetYesNo() && sc.TickSize != 0)
		Difference[sc.Index] /= sc.TickSize;
}

/*==========================================================================*/
SCSFExport scsf_McClellanSummationIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Sum = sc.Subgraph[0];
	SCSubgraphRef Temp2 = sc.Subgraph[2];
	SCSubgraphRef Temp3 = sc.Subgraph[3];

	SCInputRef DecliningIssuesChartNumber = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "McClellan Summation Index";

		sc.GraphRegion = 1;
		sc.ValueFormat = 0;


		Sum.Name = "Sum";
		Sum.DrawStyle = DRAWSTYLE_LINE;
		Sum.PrimaryColor = RGB(0,255,0);
		Sum.DrawZeros = true;

		DecliningIssuesChartNumber.Name = "Declining Issues Chart #";
		DecliningIssuesChartNumber.SetChartNumber(1);

		return;
	}

	SCGraphData ChartBaseData;
	sc.GetChartBaseData(-DecliningIssuesChartNumber.GetChartNumber(), ChartBaseData);
	
	SCFloatArrayRef Chart2Array = ChartBaseData[SC_LAST];
	if (Chart2Array.GetArraySize() == 0)
		return;

	sc.DataStartIndex = 1;

	Temp3[0] = Temp2[0] = sc.Close[0] - Chart2Array[0];

	for (int i = max(1, sc.UpdateStartIndex); i < sc.ArraySize; i++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(DecliningIssuesChartNumber.GetChartNumber(), i);

		if (sc.Close[i] == 0 || Chart2Array[i1] == 0)
		{
			Sum[i] = Sum[i - 1];
			continue;
		}

		Temp3[i] = Temp3[i - 1] * 0.9f +
			((sc.Close[i] - Chart2Array[i1]) * 0.1f);

		Temp2[i] = Temp2[i - 1] * 0.95f +
			((
			(sc.Close[i] - Chart2Array[i1]) * 0.05f));

		Sum[i] = Sum[i - 1] + (Temp3[i] - Temp2[i]);
	}
}

/*==========================================================================*/
SCSFExport scsf_MomentumTrend(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MomentumUp = sc.Subgraph[0];
	SCSubgraphRef MomentumDown = sc.Subgraph[1];
	SCSubgraphRef Temp2 = sc.Subgraph[2];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Momentum Trend";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		MomentumUp.Name = "Momentum Up";
		MomentumUp.DrawStyle = DRAWSTYLE_POINT;
		MomentumUp.PrimaryColor = RGB(0,255,0);
		MomentumUp.LineWidth = 5;
		MomentumUp.DrawZeros = false;

		MomentumDown.Name = "Momentum Down";
		MomentumDown.DrawStyle = DRAWSTYLE_POINT;
		MomentumDown.PrimaryColor = RGB(255,0,255);
		MomentumDown.LineWidth = 5;
		MomentumDown.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() + 1;

	if (sc.Index < sc.DataStartIndex - 1)
		return;

	Temp2[sc.Index] = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index] / sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - Length.GetInt()] * 100.0f;

	if (sc.Index < sc.DataStartIndex)
		return;

	if (Temp2[sc.Index] > Temp2[sc.Index - 1])
	{
		MomentumUp[sc.Index] = sc.High[sc.Index];
		MomentumDown[sc.Index] = 0;
	}
	else
	{
		MomentumDown[sc.Index] = sc.Low[sc.Index];
		MomentumUp[sc.Index] = 0;
	}
}

/*==========================================================================*/
SCSFExport scsf_SumAllCharts(SCStudyInterfaceRef sc)
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
		sc.GraphName = "Sum All Charts";
		

		sc.ValueFormat			= 2;
		sc.GraphRegion			= 1;
		sc.UseGlobalChartColors = 0;
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
	
	// Initialize elements to 0
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
	{
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index] = 0;
		}
	}
	
	// Sum charts
	const int MaxCharts = 200;
	for (int ChartIndex = 1; ChartIndex <= MaxCharts; ChartIndex++)
	{
		SCGraphData RefChartBaseData;
		sc.GetChartBaseData(-ChartIndex, RefChartBaseData);
		
		if (RefChartBaseData[SC_OPEN].GetArraySize() == 0)
			continue;
		
		for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
		{
			int RefChartIndex = sc.GetNearestMatchForDateTimeIndex(ChartIndex, Index);
			
			for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
			{
				sc.Subgraph[SubgraphIndex][Index] += RefChartBaseData[SubgraphIndex][RefChartIndex];
			}
			
			sc.Subgraph[SC_HIGH][Index]
				= max(sc.Subgraph[SC_OPEN][Index],
					max(sc.Subgraph[SC_HIGH][Index],
						max(sc.Subgraph[SC_LOW][Index], sc.Subgraph[SC_LAST][Index])
					)
				);
			
			sc.Subgraph[SC_LOW][Index]
				= min(sc.Subgraph[SC_OPEN][Index],
					min(sc.Subgraph[SC_HIGH][Index],
						min(sc.Subgraph[SC_LOW][Index], sc.Subgraph[SC_LAST][Index])
					)
				);
		}
	}
	
	// Calculate averages
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++)
		sc.CalculateOHLCAverages(Index);
}

/*==========================================================================*/
SCSFExport scsf_Kurtosis(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_SecondCentralMoment = sc.Subgraph[0];
	SCSubgraphRef Subgraph_FourthCentralMoment = sc.Subgraph[1];
	SCSubgraphRef Subgraph_Kurtosis = sc.Subgraph[2];
	SCSubgraphRef Subgraph_Line = sc.Subgraph[3];

	SCFloatArrayRef Array_Avg = Subgraph_Kurtosis.Arrays[0];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_Length = sc.Input[1];
	SCInputRef Input_KurtosisType = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Kurtosis";

		sc.AutoLoop = 1;

		Subgraph_SecondCentralMoment.Name = "Second Central Moment";
		Subgraph_SecondCentralMoment.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_FourthCentralMoment.Name = "Fourth Central Moment";
		Subgraph_FourthCentralMoment.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_Kurtosis.Name = "Kurtosis";
		Subgraph_Kurtosis.DrawStyle = DRAWSTYLE_BAR;
		Subgraph_Kurtosis.PrimaryColor = RGB(0, 255, 0);
		Subgraph_Kurtosis.SecondaryColor = RGB(255, 0, 0);
		Subgraph_Kurtosis.SecondaryColorUsed = true;
		Subgraph_Kurtosis.DrawZeros = false;

		Subgraph_Line.Name = "Line";
		Subgraph_Line.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_Line.PrimaryColor = RGB(128, 0, 128);
		Subgraph_Line.DrawZeros = true;

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_LAST);

		Input_Length.Name = "Length";
		Input_Length.SetInt(10);
		Input_Length.SetIntLimits(4, INT_MAX);

		Input_KurtosisType.Name = "Kurtosis Type";
		Input_KurtosisType.SetCustomInputStrings("Population;" "Sample");
		Input_KurtosisType.SetCustomInputIndex(0);

		return;
	}

	int Length = Input_Length.GetInt();

	sc.DataStartIndex = Length - 1;

	sc.SimpleMovAvg(sc.BaseDataIn[Input_InputData.GetInputDataIndex()], Array_Avg, Length);

	// Compute second and fourth central moments.
	float Deviation = 0.0f;
	float DeviationSquared = 0.0f;
	float SumDeviationSquared = 0.0f;
	float DeviationFourthPower = 0.0f;
	float SumDeviationFourthPower = 0.0f;

	for (int i = sc.Index - Length + 1; i <= sc.Index; i++)
	{
		Deviation = sc.BaseDataIn[Input_InputData.GetInputDataIndex()][i] - Array_Avg[sc.Index];
		DeviationSquared = Deviation * Deviation;
		SumDeviationSquared += DeviationSquared;
		DeviationFourthPower = DeviationSquared * DeviationSquared;
		SumDeviationFourthPower += DeviationFourthPower;
	}

	Subgraph_SecondCentralMoment[sc.Index] = SumDeviationSquared / Length;
	Subgraph_FourthCentralMoment[sc.Index] = SumDeviationFourthPower / Length;

	// Two cases for Kurtosis Type:
	const int SelectedIndex = Input_KurtosisType.GetIndex();

	switch (SelectedIndex)
	{
	case 0:
	{
		Subgraph_Kurtosis[sc.Index] = Subgraph_FourthCentralMoment[sc.Index] / (Subgraph_SecondCentralMoment[sc.Index] * Subgraph_SecondCentralMoment[sc.Index]) - 3.0f;

		if (Subgraph_Kurtosis[sc.Index] > 0)
			Subgraph_Kurtosis.DataColor[sc.Index] = Subgraph_Kurtosis.PrimaryColor;
		else
			Subgraph_Kurtosis.DataColor[sc.Index] = Subgraph_Kurtosis.SecondaryColor;
	}
	break;

	case 1:
	{
		Subgraph_Kurtosis[sc.Index] = ((Length - 1.0f)*(Length + 1.0f)) / ((Length - 2.0f)*(Length - 3.0f))*Subgraph_FourthCentralMoment[sc.Index] / (Subgraph_SecondCentralMoment[sc.Index] * Subgraph_SecondCentralMoment[sc.Index]) - 3.0f*(Length - 1)*(Length - 1) / ((Length - 2)*(Length - 3));

		if (Subgraph_Kurtosis[sc.Index] > 0)
			Subgraph_Kurtosis.DataColor[sc.Index] = Subgraph_Kurtosis.PrimaryColor;
		else
			Subgraph_Kurtosis.DataColor[sc.Index] = Subgraph_Kurtosis.SecondaryColor;
	}
	break;
	}

	Subgraph_Line[sc.Index] = 0.0f;

}

/*==========================================================================*/
SCSFExport scsf_AC_DC_Histogram(SCStudyInterfaceRef sc)
{
	SCSubgraphRef ACDC = sc.Subgraph[0];
	SCSubgraphRef Temp3 = sc.Subgraph[3];
	SCSubgraphRef Temp4 = sc.Subgraph[4];
	SCSubgraphRef Temp5 = sc.Subgraph[5];
	SCSubgraphRef Temp6 = sc.Subgraph[6];
	SCSubgraphRef Temp7 = sc.Subgraph[7];

	SCInputRef InputData = sc.Input[0];	
	SCInputRef MA1Length = sc.Input[2];
	SCInputRef MA2Length = sc.Input[3];
	SCInputRef MA3Length = sc.Input[4];
	SCInputRef MA4Length = sc.Input[5];

	if (sc.SetDefaults)
	{
		sc.GraphName = "AC/DC Histogram";
		
		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 1;
		

		ACDC.Name = "AC/DC";
		ACDC.DrawStyle = DRAWSTYLE_BAR;
		ACDC.SecondaryColor = RGB(255,0,0);
		ACDC.AutoColoring = AUTOCOLOR_SLOPE;
		ACDC.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MA1Length.Name = "Moving Average 1 Length";
		MA1Length.SetInt(34);
		MA1Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA2Length.Name = "Moving Average 2 Length";
		MA2Length.SetInt(5);
		MA2Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA3Length.Name = "Moving Average 3 Length";
		MA3Length.SetInt(34);
		MA3Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		MA4Length.Name = "Moving Average 4 Length";
		MA4Length.SetInt(5);
		MA4Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = max(MA1Length.GetInt(), MA2Length.GetInt()) + MA3Length.GetInt() + MA4Length.GetInt();

	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Temp3, MA1Length.GetInt());
	sc.SimpleMovAvg(sc.BaseDataIn[InputData.GetInputDataIndex()], Temp4, MA2Length.GetInt());

	if (sc.Index > 0)
		Temp5[sc.Index] = Temp4[sc.Index] - Temp3[sc.Index];
	
	sc.SimpleMovAvg(Temp5, Temp6, MA4Length.GetInt());
	Temp7[sc.Index]  = Temp5[sc.Index] - Temp6[sc.Index];
	sc.SimpleMovAvg(Temp7, ACDC, MA3Length.GetInt());
}

/*==========================================================================*/
SCSFExport scsf_MovingAverageTriangular(SCStudyInterfaceRef sc)
{
	SCSubgraphRef MA = sc.Subgraph[0];
	SCSubgraphRef TempMA = sc.Subgraph[1];

	SCInputRef InputData = sc.Input[0];
	SCInputRef Length = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Triangular";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		MA.Name = "MA";
		MA.DrawStyle = DRAWSTYLE_LINE;
		MA.PrimaryColor = RGB(0,255,0);
		MA.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Length.Name = "Length";
		Length.SetInt(9);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	sc.DataStartIndex = Length.GetInt() ;


	sc.TriangularMovingAverage(sc.BaseDataIn[InputData.GetInputDataIndex()], MA, Length.GetInt());

}

/*==========================================================================*/
SCSFExport scsf_1DividedByPrice(SCStudyInterfaceRef sc)
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
		sc.GraphName = "1 Divided by Price";

		sc.GraphUsesChartColors= true;
		
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 0;
		sc.DisplayAsMainPriceGraph= true;
		sc.AutoLoop = 1;
		
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

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

		OpenInterest.Name = "Num Trades / OpenInterest";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);
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

		
		if (sc.BaseDataIn[SC_OPEN][sc.Index] == 0.0f)
			sc.Subgraph[SC_OPEN][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_OPEN][sc.Index] = 1.0f / sc.BaseDataIn[SC_OPEN][sc.Index];

		if (sc.BaseDataIn[SC_HIGH][sc.Index] == 0.0f)
			sc.Subgraph[SC_LOW][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_LOW][sc.Index] = 1.0f / sc.BaseDataIn[SC_HIGH][sc.Index];

		if (sc.BaseDataIn[SC_LOW][sc.Index] == 0.0f)
			sc.Subgraph[SC_HIGH][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_HIGH][sc.Index] = 1.0f / sc.BaseDataIn[SC_LOW][sc.Index];

		if (sc.BaseDataIn[SC_LAST][sc.Index] == 0.0f)
			sc.Subgraph[SC_LAST][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_LAST][sc.Index] = 1.0f / sc.BaseDataIn[SC_LAST][sc.Index];


		sc.Subgraph[SC_VOLUME][sc.Index] =sc.BaseDataIn[SC_VOLUME][sc.Index];
		sc.Subgraph[SC_NUM_TRADES][sc.Index] =sc.BaseDataIn[SC_NUM_TRADES][sc.Index];

		if (sc.BaseDataIn[SC_OHLC_AVG][sc.Index] == 0.0f)
			sc.Subgraph[SC_OHLC_AVG][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_OHLC_AVG][sc.Index] = 1.0f / sc.BaseDataIn[SC_OHLC_AVG][sc.Index];

		if (sc.BaseDataIn[SC_HLC_AVG][sc.Index] == 0.0f)
			sc.Subgraph[SC_HLC_AVG][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_HLC_AVG][sc.Index] = 1.0f / sc.BaseDataIn[SC_HLC_AVG][sc.Index];

		if (sc.BaseDataIn[SC_HL_AVG][sc.Index] == 0.0f)
			sc.Subgraph[SC_HL_AVG][sc.Index] = 0.0f;
		else
			sc.Subgraph[SC_HL_AVG][sc.Index] = 1.0f / sc.BaseDataIn[SC_HL_AVG][sc.Index];


		sc.Subgraph[SC_ASKVOL][sc.Index] = sc.BaseDataIn[SC_ASKVOL][sc.Index];
		sc.Subgraph[SC_BIDVOL][sc.Index] = sc.BaseDataIn[SC_BIDVOL][sc.Index];



	SCString ChartName = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	sc.GraphName.Format("1/P %s", ChartName.GetChars());
}

/*==========================================================================*/

SCSFExport scsf_RoundPriceBarsToTickSize(SCStudyInterfaceRef sc)
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
		sc.GraphName = "Round Price Bars To Tick Size";

		sc.StudyDescription= "This study when applied to a chart will round the bar values to the Tick Size set in Chart >> Chart Settings.";

		sc.GraphUsesChartColors= true;

		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 0;
		sc.DisplayAsMainPriceGraph= true;
		sc.AutoLoop = 1;
		
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(255, 255,255);
		
		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(255, 255,255);
		
		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255, 255,255);
		
		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255, 255,255);

		Volume.Name = "Volume";
		Volume.DrawStyle = DRAWSTYLE_IGNORE;
		Volume.PrimaryColor = RGB(255,0,0);

		OpenInterest.Name = "Num Trades / OpenInterest";
		OpenInterest.DrawStyle = DRAWSTYLE_IGNORE;
		OpenInterest.PrimaryColor = RGB(0,0,255);

		OHLCAvg.Name = "OHLC Avg";
		OHLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		OHLCAvg.PrimaryColor = RGB(0,255,0);

		HLCAvg.Name = "HLC Avg";
		HLCAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLCAvg.PrimaryColor = RGB(0,255,255);

		HLAvg.Name = "HL Avg";
		HLAvg.DrawStyle = DRAWSTYLE_IGNORE;
		HLAvg.PrimaryColor = RGB(0,127,255);

		BidVol.Name = "Bid Vol";
		BidVol.DrawStyle = DRAWSTYLE_IGNORE;
		BidVol.PrimaryColor = RGB(0,255,0);

		AskVol.Name = "Ask Vol";
		AskVol.DrawStyle = DRAWSTYLE_IGNORE;
		AskVol.PrimaryColor = RGB(0,255,0);

		return;
	}


	
		sc.Subgraph[SC_OPEN][sc.Index] = sc.RoundToTickSize(sc.BaseDataIn[SC_OPEN][sc.Index] , sc.TickSize);
		sc.Subgraph[SC_HIGH][sc.Index] = sc.RoundToTickSize(sc.BaseDataIn[SC_HIGH][sc.Index] , sc.TickSize);
		sc.Subgraph[SC_LOW][sc.Index] = sc.RoundToTickSize(sc.BaseDataIn[SC_LOW][sc.Index] , sc.TickSize);
		sc.Subgraph[SC_LAST][sc.Index] = sc.RoundToTickSize(sc.BaseDataIn[SC_LAST][sc.Index] , sc.TickSize);


		for(int SubgraphIndex =SC_VOLUME; SubgraphIndex <=SC_ASKVOL;SubgraphIndex++)
			sc.Subgraph[SubgraphIndex][sc.Index] =sc.BaseDataIn[SubgraphIndex][sc.Index];




	
	sc.GraphName= sc.GetStudyNameFromChart(sc.ChartNumber, 0);
}

/*==========================================================================*/

SCSFExport scsf_DifferenceSingleLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Difference = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef UseLatestSourceDataForLastBar = sc.Input[1];
	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];
	SCInputRef Chart1Addition = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Difference (Single Line)";

		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 1;

		Difference.Name = "Difference";
		Difference.DrawStyle = DRAWSTYLE_LINE;
		Difference.PrimaryColor = RGB(0,255,0);
		Difference.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);
		
		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(0);

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);

		Chart1Addition.Name = "Chart 1 Addition";
		Chart1Addition.SetFloat(0.0f);

		return;
	}

	if (sc.IsFullRecalculation)
	{
		SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
		SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
		sc.GraphName.Format("Diff %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars());
	}

	SCFloatArray Chart2Array;
	sc.GetChartArray(Chart2Number.GetChartNumber()*-1, InputData.GetInputDataIndex(), Chart2Array); 

	if (Chart2Array.GetArraySize() == 0)
		return;

	sc.DataStartIndex = 0;

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	for (int DestinationIndex = CalculationStartIndex; DestinationIndex < sc.ArraySize; DestinationIndex++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), DestinationIndex);

		//When use latest source data for last bar is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source chart.
		if(UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() && DestinationIndex == sc.ArraySize - 1)
			Chart2Index = Chart2Array.GetArraySize() - 1;

		Difference[DestinationIndex] = (sc.BaseDataIn[InputData.GetInputDataIndex()][DestinationIndex] * Chart1Multiplier.GetFloat() + Chart1Addition.GetFloat()) -
			(Chart2Array[Chart2Index] * Chart2Multiplier.GetFloat());
	}
	
	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;

}
/*==========================================================================*/

SCSFExport scsf_DifferenceNetChange(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Difference = sc.Subgraph[0];
	SCInputRef Chart2Number = sc.Input[3];

	if (sc.SetDefaults)
	{
		//Calculates the difference of the bar to bar price change between two charts
		sc.GraphName = "Bar Price Change Difference - 2 Chart";

		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 1;

		Difference.Name = "Difference";
		Difference.DrawStyle = DRAWSTYLE_LINE;
		Difference.PrimaryColor = RGB(0,255,0);
		Difference.DrawZeros = false;

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);
	
		return;
	}

	SCFloatArray Chart2Array;
	sc.GetChartArray(Chart2Number.GetChartNumber()*-1, SC_LAST, Chart2Array); 

	if (Chart2Array.GetArraySize() == 0)
		return;

	sc.DataStartIndex = 1;

	for (int Chart1BarIndex = max(1, sc.UpdateStartIndex); Chart1BarIndex < sc.ArraySize; Chart1BarIndex++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), Chart1BarIndex);

		Difference[Chart1BarIndex] = (sc.BaseDataIn[SC_LAST][Chart1BarIndex] - sc.BaseDataIn[SC_LAST][Chart1BarIndex - 1] ) - ( Chart2Array[Chart2Index] - Chart2Array[Chart2Index - 1]);
	}

	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Bar Change Difference %s - %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_RatioSingleLine(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Ratio = sc.Subgraph[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef Chart2Number = sc.Input[3];
	SCInputRef Chart1Multiplier = sc.Input[4];
	SCInputRef Chart2Multiplier = sc.Input[5];
	SCInputRef UseLatestSourceDataForLastBar = sc.Input[6];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Ratio (Single Line)";

		
		sc.ValueFormat = 2;
		sc.GraphRegion = 1;

		Ratio.Name = "Ratio";
		Ratio.DrawStyle = DRAWSTYLE_LINE;
		Ratio.PrimaryColor = RGB(0,255,0);
		Ratio.DrawZeros = false;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		Chart2Number.Name = "Chart 2 Number";
		Chart2Number.SetChartNumber(1);

		Chart1Multiplier.Name = "Chart 1 Multiplier";
		Chart1Multiplier.SetFloat(1.0f);

		Chart2Multiplier.Name = "Chart 2 Multiplier";
		Chart2Multiplier.SetFloat(1.0f);


		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(0);
		
		return;
	}
	
	
	SCFloatArray Chart2Array;
	sc.GetChartArray(Chart2Number.GetChartNumber()*-1, InputData.GetInputDataIndex(), Chart2Array); 

	if (Chart2Array.GetArraySize() == 0)
		return;

	for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
	{
		int Chart2Index = sc.GetNearestMatchForDateTimeIndex(Chart2Number.GetChartNumber(), BarIndex);


		//When 'use latest source data for last bar' is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source chart.
		if(UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() && BarIndex == sc.ArraySize - 1)
			Chart2Index = Chart2Array.GetArraySize() - 1;


		if (sc.BaseDataIn[InputData.GetInputDataIndex()][BarIndex] == 0 
			|| Chart2Array[Chart2Index] == 0)
		{
			Ratio[BarIndex] = 0;
			continue;
		}

		Ratio[BarIndex] = (sc.BaseDataIn[InputData.GetInputDataIndex()][BarIndex] * Chart1Multiplier.GetFloat())
					/ (Chart2Array[Chart2Index] * Chart2Multiplier.GetFloat());
	}


	SCString Chart1Name = sc.GetStudyNameFromChart(sc.ChartNumber, 0);
	SCString Chart2Name = sc.GetStudyNameFromChart(Chart2Number.GetChartNumber(), 0);
	sc.GraphName.Format("Ratio %s / %s", Chart1Name.GetChars(), Chart2Name.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_OverlayBar(SCStudyInterfaceRef sc)
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
	SCSubgraphRef Unused11 = sc.Subgraph[11];
	SCSubgraphRef Unused12 = sc.Subgraph[12];

	SCInputRef Input_OverlayFromSameChartAsStudy = sc.Input[0];
	SCInputRef Input_ChartOverlayNumber = sc.Input[3];
	SCInputRef Input_Multiplier = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Overlay (Bar)";

		sc.UseGlobalChartColors = 0;
		
		//When the ability to access all the variables of another chart is supported, 
		//this should be changed to get the value format from the source chart
		sc.ValueFormat = VALUEFORMAT_INHERITED;

		sc.GraphRegion =1;
		sc.ScaleRangeType = SCALE_INDEPENDENT;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = 1;
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(128,255,128);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = 1;
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,128,128);
		Last.DrawZeros = false;
		Last.LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_VALUES_SCALE | LL_VALUE_ALIGN_CENTER;

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


		Unused11.DrawStyle = DRAWSTYLE_IGNORE;
		Unused12.DrawStyle = DRAWSTYLE_IGNORE;

		Input_OverlayFromSameChartAsStudy.Name = "Overlay from Same Chart as Study";
		Input_OverlayFromSameChartAsStudy.SetYesNo(false);

		Input_ChartOverlayNumber.Name = "Chart Number To Overlay";
		Input_ChartOverlayNumber.SetChartNumber(1);

		Input_Multiplier.Name = "Multiplier";
		Input_Multiplier.SetFloat(1.0f);

		return;
	}

	if (Input_OverlayFromSameChartAsStudy.GetYesNo())
		Input_ChartOverlayNumber.SetChartNumber(sc.ChartNumber);	
	
	SCGraphData ChartOverlayArrays;
	sc.GetChartBaseData(-Input_ChartOverlayNumber.GetChartNumber(), ChartOverlayArrays);
	
	if (ChartOverlayArrays[SC_OPEN].GetArraySize() == 0)
		return;
	
	for (int DataIndex = sc.UpdateStartIndex; DataIndex < sc.ArraySize; DataIndex++)
	{
		int i1 = sc.GetNearestMatchForDateTimeIndex(Input_ChartOverlayNumber.GetChartNumber(), DataIndex);
		
		for (int SubgraphIndex = 0; SubgraphIndex < ChartOverlayArrays.GetArraySize(); SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][DataIndex] = ChartOverlayArrays[SubgraphIndex][i1] * Input_Multiplier.GetFloat();
		}
	}
	
	SCString ChartOverlayName = sc.GetStudyNameFromChart(Input_ChartOverlayNumber.GetChartNumber(), 0);
	sc.GraphName.Format("%s Overlay", ChartOverlayName.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_HorizontalLines(SCStudyInterfaceRef sc)
{
	const int NumberOfLines = SC_SUBGRAPHS_AVAILABLE - 1;
	SCInputRef SetAllSubgraphsToBeSameAsFirst = sc.Input[SC_SUBGRAPHS_AVAILABLE - 1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Horizontal Lines";

		sc.ValueFormat = 3;
		sc.AutoLoop = 1;
		sc.DisplayStudyInputValues = false;

		for (int SubgraphIndex = 0; SubgraphIndex < NumberOfLines; SubgraphIndex++)
		{

			sc.Subgraph[SubgraphIndex].Name.Format("Line%d", SubgraphIndex + 1);
			sc.Subgraph[SubgraphIndex].DrawStyle = DRAWSTYLE_IGNORE;
			sc.Subgraph[SubgraphIndex].PrimaryColor = RGB(0,255,0);
			sc.Subgraph[SubgraphIndex].DrawZeros = true;
			
				
			sc.Input[SubgraphIndex].Name.Format("Line%d Value", SubgraphIndex + 1);
			sc.Input[SubgraphIndex].SetFloat(1.0f);
			sc.Input[SubgraphIndex].SetFloatLimits(-FLT_MAX, FLT_MAX);
		}

		sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;
		
		SetAllSubgraphsToBeSameAsFirst.Name = "Set All Subgraph Display Settings to Be Same as First";
		SetAllSubgraphsToBeSameAsFirst.SetYesNo(false);

		return;
	}



	for (int SubgraphIndex = 0; SubgraphIndex < NumberOfLines; SubgraphIndex++)
	{
		if (sc.Subgraph[SubgraphIndex].DrawStyle == DRAWSTYLE_IGNORE)
			continue;

		if (sc.IsFullRecalculation && sc.Index == 0 && SetAllSubgraphsToBeSameAsFirst.GetYesNo()
			&& SubgraphIndex > 0)
		{
			sc.Subgraph[SubgraphIndex].DrawStyle = sc.Subgraph[0].DrawStyle;
			sc.Subgraph[SubgraphIndex].PrimaryColor = sc.Subgraph[0].PrimaryColor;
			sc.Subgraph[SubgraphIndex].LineStyle = sc.Subgraph[0].LineStyle;
			sc.Subgraph[SubgraphIndex].LineWidth = sc.Subgraph[0].LineWidth;
			sc.Subgraph[SubgraphIndex].LineLabel = sc.Subgraph[0].LineLabel;
		}

		//Only do this on the first iteration for efficiency
		if (sc.IsFullRecalculation == 0)
			sc.Input[SubgraphIndex].Name.Format("%s Value",sc.Subgraph[SubgraphIndex].Name.GetChars()); 

		sc.Subgraph[SubgraphIndex][sc.Index] = sc.Input[SubgraphIndex].GetFloat();
	}
}

/*==========================================================================*/
SCSFExport scsf_RelativeMomentumIndex(SCStudyInterfaceRef sc)
{
	SCSubgraphRef RMI = sc.Subgraph[0];
	SCSubgraphRef Line1 = sc.Subgraph[1];
	SCSubgraphRef Line2 = sc.Subgraph[2];
	SCSubgraphRef Midline = sc.Subgraph[3];

	SCInputRef InputData = sc.Input[0];
	SCInputRef RMILength = sc.Input[3];
	SCInputRef RMI_MALength = sc.Input[4];
	SCInputRef RMI_MAType = sc.Input[5];
	SCInputRef Line1Value = sc.Input[6];
	SCInputRef Line2Value = sc.Input[7];
	SCInputRef MidlineValue = sc.Input[8];

	SCFloatArrayRef UpTempArray = RMI.Arrays[0];
	SCFloatArrayRef DownTempArray = RMI.Arrays[1];
	SCFloatArrayRef SmoothedUpTempArray = RMI.Arrays[2];
	SCFloatArrayRef SmoothedDownTempArray = RMI.Arrays[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Relative Momentum Index";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		RMI.Name = "RMI";
		RMI.DrawStyle = DRAWSTYLE_LINE;
		RMI.PrimaryColor = RGB(0, 255, 0);
		RMI.DrawZeros = true;

		Line1.Name = "Overbought";
		Line1.DrawStyle = DRAWSTYLE_LINE;
		Line1.PrimaryColor = RGB(128, 128, 128);
		Line1.DrawZeros = true;

		Line2.Name = "Oversold";
		Line2.DrawStyle = DRAWSTYLE_LINE;
		Line2.PrimaryColor = RGB(128, 128, 128);
		Line2.DrawZeros = true;

		Midline.Name = "Midline";
		Midline.DrawStyle = DRAWSTYLE_LINE;
		Midline.PrimaryColor = RGB(128, 128, 128);
		Midline.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		RMILength.Name = "RMI Length";
		RMILength.SetInt(5);
		RMILength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RMI_MALength.Name = "RMI Moving Average Length";
		RMI_MALength.SetInt(9);
		RMI_MALength.SetIntLimits(1, MAX_STUDY_LENGTH);

		RMI_MAType.Name = "RMI Moving Average Type";
		RMI_MAType.SetMovAvgType(MOVAVGTYPE_SIMPLE);

		Line1Value.Name = "Overbought Value";
		Line1Value.SetFloat(70.0f);
		

		Line2Value.Name = "Oversold Value";
		Line2Value.SetFloat(30.0f);

		MidlineValue.Name = "Midline Value";
		MidlineValue.SetFloat(50.0f);
		

		return;
	}

	sc.DataStartIndex = max(RMILength.GetInt(), RMI_MALength.GetInt());

	float previousValue = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index - RMILength.GetInt()];
	float currentValue = sc.BaseDataIn[InputData.GetInputDataIndex()][sc.Index];

	if (currentValue > previousValue) 
	{
		UpTempArray[sc.Index] = currentValue - previousValue;
		DownTempArray[sc.Index] = 0.0f;
	}
	else
	{
		UpTempArray[sc.Index] = 0;
		DownTempArray[sc.Index] = previousValue - currentValue;
	}

	sc.MovingAverage(UpTempArray, SmoothedUpTempArray, RMI_MAType.GetMovAvgType(), RMI_MALength.GetInt());
	sc.MovingAverage(DownTempArray, SmoothedDownTempArray, RMI_MAType.GetMovAvgType(), RMI_MALength.GetInt());

	if (SmoothedUpTempArray[sc.Index] + SmoothedDownTempArray[sc.Index] != 0.0f)
	{
		RMI[sc.Index] = 100 * SmoothedUpTempArray[sc.Index] / (SmoothedUpTempArray[sc.Index] + SmoothedDownTempArray[sc.Index]);
	}
	else
	{
		RMI[sc.Index] = RMI[sc.Index - 1];
	}

	Line1[sc.Index] = Line1Value.GetFloat();
	Line2[sc.Index] = Line2Value.GetFloat();
	Midline[sc.Index] = MidlineValue.GetFloat();
}

/*==========================================================================*/
SCSFExport scsf_OverlayNonSync(SCStudyInterfaceRef sc)
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
	SCSubgraphRef Unused11 = sc.Subgraph[11];
	SCSubgraphRef Unused12 = sc.Subgraph[12];

	SCInputRef NumberOfBars = sc.Input[2];
	SCInputRef ChartOverlayNumber = sc.Input[3];
	SCInputRef Multiplier = sc.Input[4];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Overlay (Non-Sync)";

		sc.UseGlobalChartColors = 0;
		
		sc.ValueFormat = 2;
		sc.GraphRegion = 0;
		sc.ScaleRangeType = SCALE_INDEPENDENT;
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		Open.Name = "Open";
		Open.DrawStyle = DRAWSTYLE_LINE;
		Open.PrimaryColor = RGB(0,255,0);
		Open.SecondaryColor = RGB(0,255,0);
		Open.SecondaryColorUsed = 1;
		Open.DrawZeros = false;

		High.Name = "High";
		High.DrawStyle = DRAWSTYLE_LINE;
		High.PrimaryColor = RGB(128,255,128);
		High.DrawZeros = false;

		Low.Name = "Low";
		Low.DrawStyle = DRAWSTYLE_LINE;
		Low.PrimaryColor = RGB(255,0,0);
		Low.SecondaryColor = RGB(255,0,0);
		Low.SecondaryColorUsed = 1;
		Low.DrawZeros = false;

		Last.Name = "Last";
		Last.DrawStyle = DRAWSTYLE_LINE;
		Last.PrimaryColor = RGB(255,128,128);
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


		Unused11.DrawStyle = DRAWSTYLE_IGNORE;
		Unused12.DrawStyle = DRAWSTYLE_IGNORE;

		NumberOfBars.Name = "Number of bars to Overlay";
		NumberOfBars.SetInt(500);
		NumberOfBars.SetIntLimits(1,MAX_STUDY_LENGTH);

		ChartOverlayNumber.Name = "Chart Number To Overlay";
		ChartOverlayNumber.SetChartNumber(1);

		Multiplier.Name = "Multiplier";
		Multiplier.SetFloat(1.0f);

		return;
	}

	SCGraphData ChartOverlayArrays;
	sc.GetChartBaseData(ChartOverlayNumber.GetChartNumber(), ChartOverlayArrays);
	
	int OverlayArraySize = ChartOverlayArrays[0].GetArraySize();
	if (OverlayArraySize == 0)
		return;
	
	for (int Index = sc.ArraySize - 1; (Index >= sc.ArraySize - NumberOfBars.GetInt()) && (Index >= 0); Index--)
	{
		OverlayArraySize--;
		for (int i2 = 0; i2 < ChartOverlayArrays.GetArraySize(); i2++)
		{
			if (OverlayArraySize >= 0)
			{
				sc.Subgraph[i2][Index] = ChartOverlayArrays[i2][OverlayArraySize] * Multiplier.GetFloat();
			}
			else
				sc.Subgraph[i2][Index] = 0.0f;
		}
	}

	for (int Index = sc.ArraySize - NumberOfBars.GetInt() - 1; (Index >= sc.ArraySize - sc.UpdateStartIndex) && (Index >= 0); Index--)
	{
		for (int i2 = 0; i2 < ChartOverlayArrays.GetArraySize(); i2++)
			sc.Subgraph[i2][Index] = 0.0f;
	}

	SCString ChartOverlayName = sc.GetStudyNameFromChart(ChartOverlayNumber.GetChartNumber(), 0);
	sc.GraphName.Format("%s Overlay", ChartOverlayName.GetChars());
}

/*==========================================================================*/
SCSFExport scsf_OnBalanceOpenInterestShortTerm(SCStudyInterfaceRef sc)
{
	SCSubgraphRef OBOI = sc.Subgraph[0];
	SCSubgraphRef Temp = sc.Subgraph[1];
	SCInputRef Length = sc.Input[0];

	if (sc.SetDefaults)
	{
		sc.GraphName = "On Balance Open Interest - Short Term";

		sc.ValueFormat = 0;
		sc.AutoLoop = 1;

		OBOI.Name = "OBOI";
		OBOI.PrimaryColor = RGB(0,255,0);
		OBOI.DrawStyle= DRAWSTYLE_LINE;
		OBOI.DrawZeros= true;

		Length.Name = "Length";
		Length.SetInt(10);
		Length.SetIntLimits(1, MAX_STUDY_LENGTH);
		
		return;
	}


	sc.DataStartIndex= Length.GetInt();
	if (sc.Index < 1)
		return;

	if(sc.Close[sc.Index ] > sc.Close[sc.Index-1])
	{
		Temp[sc.Index] = sc.OpenInterest[sc.Index];
	}
	else if(sc.Close[sc.Index ] < sc.Close[sc.Index-1])
	{
		Temp[sc.Index] = -sc.OpenInterest[sc.Index];
	}
	else
	{
		Temp[sc.Index] = 0.0f;
	}

	if(sc.Index < Length.GetInt())
	{
		OBOI[sc.Index] = OBOI[sc.Index - 1] + Temp[sc.Index];
	}
	else
	{
		OBOI[sc.Index] = OBOI[sc.Index - 1] + Temp[sc.Index] - Temp[sc.Index - Length.GetInt()];
	}
}



/*==========================================================================*/
/*

Coppock Curve = 10-period WMA of 14-period RoC + 11-period RoC

WMA = Weighted moving average
RoC = Rate-of-Change

ROC = [(Close - Close n periods ago) / (Close n periods ago)] * 100

*/
SCSFExport scsf_CoppockCurve(SCStudyInterfaceRef sc)
{
	SCSubgraphRef CoppockCurve = sc.Subgraph[0];
	SCFloatArrayRef RateOfChangeSum = sc.Subgraph[0].Arrays[0];
	SCInputRef InputData = sc.Input[0];
	SCInputRef MovingAverageLength = sc.Input[1];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Coppock Curve";

		sc.GraphRegion = 1;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		CoppockCurve.Name = "CC";
		CoppockCurve.DrawStyle = DRAWSTYLE_LINE;
		CoppockCurve.PrimaryColor = RGB(0, 255, 0);
		CoppockCurve.DrawZeros = true;


		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		MovingAverageLength.Name = "Moving Average Length";
		MovingAverageLength.SetInt(10);
		MovingAverageLength.SetIntLimits(1, MAX_STUDY_LENGTH);

		return;
	}

	//sc.DataStartIndex = MovingAverageLength.GetInt();

	if (sc.Index < sc.DataStartIndex)
		return;

	float ROC1 = 0.0;
	float ROC2 = 0.0;
	int InputDataIndex = InputData.GetInputDataIndex();

	ROC1 = ((sc.BaseData[InputDataIndex][sc.Index] - sc.BaseData[InputDataIndex][sc.Index - 14]) / sc.BaseData[InputDataIndex][sc.Index - 14]) * 100;

	ROC2 = ((sc.BaseData[InputDataIndex][sc.Index] - sc.BaseData[InputDataIndex][sc.Index - 11]) / sc.BaseData[InputDataIndex][sc.Index - 11]) * 100;

	RateOfChangeSum[sc.Index] = ROC1 + ROC2;

	sc.WeightedMovingAverage(RateOfChangeSum, CoppockCurve, MovingAverageLength.GetInt());

}

/*==========================================================================*/
SCSFExport scsf_HurstExponent(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_HurstExponent = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	SCInputRef LengthIndex = sc.Input[1];
	SCInputRef PerformCumulativeCalculation = sc.Input[2];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Hurst Exponent";

		sc.GraphRegion = 1;
		sc.ValueFormat = 3;
		sc.AutoLoop = 0;

		Subgraph_HurstExponent.Name = "HE";
		Subgraph_HurstExponent.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_HurstExponent.PrimaryColor = RGB(0, 255, 0);
		Subgraph_HurstExponent.LineWidth = 1;
		Subgraph_HurstExponent.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		LengthIndex.Name = "Length";
		LengthIndex.SetCustomInputStrings("32;64;128");
		LengthIndex.SetCustomInputIndex(0);

		PerformCumulativeCalculation.Name = "Perform Cumulative Calculation Across All Chart Bars";
		PerformCumulativeCalculation.SetYesNo(false);
		
		

		return;
	}

	sc.DataStartIndex = atoi(LengthIndex.GetSelectedCustomString().GetChars())-1;


	for (int BarIndex = max(1,sc.UpdateStartIndex); BarIndex < sc.ArraySize; BarIndex++)
	{
		if(PerformCumulativeCalculation.GetYesNo())
		{

			CumulativeHurstExponent_S(sc.BaseData[InputData.GetInputDataIndex()],
				Subgraph_HurstExponent, 
				Subgraph_HurstExponent.Arrays[0], 
				Subgraph_HurstExponent.Arrays[1], 
				Subgraph_HurstExponent.Arrays[2], 
				Subgraph_HurstExponent.Arrays[3],
				Subgraph_HurstExponent.Arrays[4], 
				Subgraph_HurstExponent.Arrays[5],
				Subgraph_HurstExponent.Arrays[6], 
				Subgraph_HurstExponent.Arrays[7],
				Subgraph_HurstExponent.Arrays[8],
				Subgraph_HurstExponent.Arrays[9], 
				Subgraph_HurstExponent.Arrays[10],
				Subgraph_HurstExponent.Arrays[11], 
				sc.Subgraph[1].Arrays[0],
				sc.Subgraph[1].Arrays[1], 
				BarIndex);


		}
		else
		{
			SCFloatArrayRef InputDataArray = sc.BaseData[InputData.GetInputDataIndex()];

			//Calculate Logarithmic Return 
			sc.Subgraph[0].Arrays[0][BarIndex] = (float)(log(InputDataArray[BarIndex]/InputDataArray[BarIndex - 1]));

			sc.HurstExponent(sc.Subgraph[0].Arrays[0], Subgraph_HurstExponent, BarIndex, LengthIndex.GetIndex());
		}

	}


}

/*==========================================================================*/
SCSFExport scsf_MovingAverageCumulative(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Average = sc.Subgraph[0];

	SCInputRef InputData = sc.Input[0];
	

	if (sc.SetDefaults)
	{
		sc.GraphName = "Moving Average - Cumulative";

		sc.GraphRegion = 0;
		sc.ValueFormat = 2;
		sc.AutoLoop = 1;

		Average.Name = "Avg";
		Average.DrawStyle = DRAWSTYLE_LINE;
		Average.PrimaryColor = RGB(0, 255, 0);
		Average.LineWidth = 1;
		Average.DrawZeros = true;

		InputData.Name = "Input Data";
		InputData.SetInputDataIndex(SC_LAST);

		

		return;
	}

	sc.DataStartIndex = 1;

	sc.MovingAverageCumulative(sc.BaseData[InputData.GetInputDataIndex()], Average);

}

/*==========================================================================*/
SCSFExport scsf_CombinationSymbolChart(SCStudyInterfaceRef sc)
{

	SCInputRef UseLatestSourceDataForLastBar = sc.Input[0];

	SCInputRef InChart1Multiplier = sc.Input[1];

	SCInputRef InUseChart2 = sc.Input[2];
	SCInputRef InChart2Number = sc.Input[3];
	SCInputRef InChart2Multiplier = sc.Input[4];	
	
	SCInputRef InUseChart3 = sc.Input[5];
	SCInputRef InChart3Number = sc.Input[6];
	SCInputRef InChart3Multiplier = sc.Input[7];

	SCInputRef InUseChart4 = sc.Input[8];
	SCInputRef InChart4Number = sc.Input[9];
	SCInputRef InChart4Multiplier = sc.Input[10];	

	SCInputRef InUseChart5 = sc.Input[11];
	SCInputRef InChart5Number = sc.Input[12];
	SCInputRef InChart5Multiplier = sc.Input[13];

	SCInputRef InUseChart6 = sc.Input[14];
	SCInputRef InChart6Number = sc.Input[15];
	SCInputRef InChart6Multiplier = sc.Input[16];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Combination Symbol Chart";
		sc.AutoLoop = 0;
		sc.ValueFormat = VALUEFORMAT_INHERITED;
		sc.GraphRegion = 1;

		sc.GraphUsesChartColors = 1;//Use the global color settings
		sc.GraphDrawType = GDT_OHLCBAR;
		sc.StandardChartHeader = 1;

		UseLatestSourceDataForLastBar.Name = "Use Latest Source Data For Last Bar";
		UseLatestSourceDataForLastBar.SetYesNo(false);

		InChart1Multiplier.Name = "Chart 1 Multiplier";
		InChart1Multiplier.SetFloat(1.0f);

		InUseChart2.Name = "Use Chart 2";
		InUseChart2.SetYesNo(true);
		
		InChart2Number.Name = "Chart 2 Number";
		InChart2Number.SetChartNumber(1);

		InChart2Multiplier.Name = "Chart 2 Multiplier";
		InChart2Multiplier.SetFloat(1.0f);
		
		InUseChart3.Name = "Use Chart 3";
		InUseChart3.SetYesNo(true);

		InChart3Number.Name = "Chart 3 Number";
		InChart3Number.SetChartNumber(1);

		InChart3Multiplier.Name = "Chart 3 Multiplier";
		InChart3Multiplier.SetFloat(1.0f);

		InUseChart4.Name = "Use Chart 4";
		InUseChart4.SetYesNo(true);

		InChart4Number.Name = "Chart 4 Number";
		InChart4Number.SetChartNumber(1);

		InChart4Multiplier.Name = "Chart 4 Multiplier";
		InChart4Multiplier.SetFloat(1.0f);

		InUseChart5.Name = "Use Chart 5";
		InUseChart5.SetYesNo(true);

		InChart5Number.Name = "Chart 5 Number";
		InChart5Number.SetChartNumber(1);

		InChart5Multiplier.Name = "Chart 5 Multiplier";
		InChart5Multiplier.SetFloat(1.0f);
		
		InUseChart6.Name = "Use Chart 6";
		InUseChart6.SetYesNo(true);

		InChart6Number.Name = "Chart 6 Number";
		InChart6Number.SetChartNumber(1);

		InChart6Multiplier.Name = "Chart 6 Multiplier";
		InChart6Multiplier.SetFloat(1.0f);
		return;
	}

	if (sc.IsFullRecalculation && sc.UpdateStartIndex == 0)
	{
		for (int SubgraphIndex = 0; SubgraphIndex <= NUM_BASE_GRAPH_ARRAYS; ++SubgraphIndex)
		{
			sc.Subgraph[SubgraphIndex].Name = sc.GetStudySubgraphName(0, SubgraphIndex);
		}

		sc.GraphName = "Combination Chart: ";
		sc.GraphName += sc.GetChartName(sc.ChartNumber);

		if (InUseChart2.GetYesNo())
		{
			sc.GraphName += ", ";
			sc.GraphName += sc.GetChartName(InChart2Number.GetChartNumber());
		}

		if (InUseChart3.GetYesNo())
		{
			sc.GraphName += ", ";
			sc.GraphName += sc.GetChartName(InChart3Number.GetChartNumber());
		}

		if (InUseChart4.GetYesNo())
		{
			sc.GraphName += ", ";
			sc.GraphName += sc.GetChartName(InChart4Number.GetChartNumber());
		}

		if (InUseChart5.GetYesNo())
		{
			sc.GraphName += ", ";
			sc.GraphName += sc.GetChartName(InChart5Number.GetChartNumber());
		}

		if (InUseChart6.GetYesNo())
		{
			sc.GraphName += ", ";
			sc.GraphName += sc.GetChartName(InChart6Number.GetChartNumber());
		}
	}

	SCGraphData Chart2BaseData;
	if(InUseChart2.GetYesNo())
		sc.GetChartBaseData(-InChart2Number.GetChartNumber(), Chart2BaseData);

	SCGraphData Chart3BaseData;
	if (InUseChart3.GetYesNo())
		sc.GetChartBaseData(-InChart3Number.GetChartNumber(), Chart3BaseData);

	SCGraphData Chart4BaseData;
	if (InUseChart4.GetYesNo())
		sc.GetChartBaseData(-InChart4Number.GetChartNumber(), Chart4BaseData);

	SCGraphData Chart5BaseData;
	if (InUseChart5.GetYesNo())
		sc.GetChartBaseData(-InChart5Number.GetChartNumber(), Chart5BaseData);

	SCGraphData Chart6BaseData;
	if (InUseChart6.GetYesNo())
		sc.GetChartBaseData(-InChart6Number.GetChartNumber(), Chart6BaseData);

	int CalculationStartIndex = sc.GetCalculationStartIndexForStudy();

	// Iterate through the bars
	for (int Index = CalculationStartIndex; Index < sc.ArraySize; ++Index)
	{
		int Chart2Index = 0;
		if (InUseChart2.GetYesNo())
			Chart2Index = sc.GetNearestMatchForDateTimeIndex(InChart2Number.GetChartNumber(), Index);

		int Chart3Index = 0;
		if (InUseChart3.GetYesNo())
			Chart3Index = sc.GetNearestMatchForDateTimeIndex(InChart3Number.GetChartNumber(), Index);

		int Chart4Index = 0;
		if (InUseChart4.GetYesNo())
			Chart4Index = sc.GetNearestMatchForDateTimeIndex(InChart4Number.GetChartNumber(), Index);

		int Chart5Index = 0;
		if (InUseChart5.GetYesNo())
			Chart5Index = sc.GetNearestMatchForDateTimeIndex(InChart5Number.GetChartNumber(), Index);

		int Chart6Index = 0;
		if (InUseChart6.GetYesNo())
			Chart6Index = sc.GetNearestMatchForDateTimeIndex(InChart6Number.GetChartNumber(), Index);


		//When use latest source data for last bar is set to Yes, replay is not running and at last bar in the destination chart, then use the data from the very latest bar in the source charts.
		if (UseLatestSourceDataForLastBar.GetYesNo() && !sc.IsReplayRunning() 
			&& Index == sc.ArraySize - 1)
		{
			if(InUseChart2.GetYesNo())
				Chart2Index = Chart2BaseData[SC_OPEN].GetArraySize() - 1;

			if (InUseChart3.GetYesNo())
				Chart3Index = Chart3BaseData[SC_OPEN].GetArraySize() - 1;

			if (InUseChart4.GetYesNo())
				Chart4Index = Chart4BaseData[SC_OPEN].GetArraySize() - 1;

			if (InUseChart5.GetYesNo())
				Chart5Index = Chart5BaseData[SC_OPEN].GetArraySize() - 1;

			if (InUseChart6.GetYesNo())
				Chart6Index = Chart6BaseData[SC_OPEN].GetArraySize() - 1;
		}


		// Iterate through the subgraphs
		for (int SubgraphIndex = SC_OPEN; SubgraphIndex <= SC_NUM_TRADES; SubgraphIndex++)
		{
			sc.Subgraph[SubgraphIndex][Index] = sc.BaseDataIn[SubgraphIndex][Index] * InChart1Multiplier.GetFloat();

			if(InUseChart2.GetYesNo())
				sc.Subgraph[SubgraphIndex][Index] += Chart2BaseData[SubgraphIndex][Chart2Index] * InChart2Multiplier.GetFloat();

			if (InUseChart3.GetYesNo())
				sc.Subgraph[SubgraphIndex][Index] += Chart3BaseData[SubgraphIndex][Chart3Index] * InChart3Multiplier.GetFloat();

			if (InUseChart4.GetYesNo())
				sc.Subgraph[SubgraphIndex][Index] += Chart4BaseData[SubgraphIndex][Chart4Index] * InChart4Multiplier.GetFloat();

			if (InUseChart5.GetYesNo())
				sc.Subgraph[SubgraphIndex][Index] += Chart5BaseData[SubgraphIndex][Chart5Index] * InChart5Multiplier.GetFloat();

			if (InUseChart6.GetYesNo())
				sc.Subgraph[SubgraphIndex][Index] += Chart6BaseData[SubgraphIndex][Chart6Index] * InChart6Multiplier.GetFloat();

		}

		sc.Subgraph[SC_HIGH][Index]
			= max(
				sc.Subgraph[SC_OPEN][Index],
				max(
					sc.Subgraph[SC_HIGH][Index],
					max(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);

		sc.Subgraph[SC_LOW][Index]
			= min(
				sc.Subgraph[SC_OPEN][Index],
				min(
					sc.Subgraph[SC_HIGH][Index],
					min(
						sc.Subgraph[SC_LOW][Index],
						sc.Subgraph[SC_LAST][Index]
					)
				)
			);

		sc.CalculateOHLCAverages(Index);
	}

	sc.EarliestUpdateSubgraphDataArrayIndex = CalculationStartIndex;
}
