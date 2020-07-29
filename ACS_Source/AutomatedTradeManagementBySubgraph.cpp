#include "sierrachart.h"


SCSFExport scsf_TradeManagementByStudy(SCStudyInterfaceRef sc)
{
	// Setup References
	SCInputRef Enabled                  = sc.Input[0];
	SCInputRef OrderType                = sc.Input[1];
	SCInputRef ControlSubgraphRef       = sc.Input[2];
	SCInputRef AdjustmentTiming         = sc.Input[3]; 
	SCInputRef AdjustmentFrequency      = sc.Input[4];
	SCInputRef OrderOffset              = sc.Input[5];
	SCInputRef ACSButtonNumber                   = sc.Input[6];
	SCInputRef PositionType             = sc.Input[7];
	SCInputRef Logging                  = sc.Input[8];
	SCInputRef OnlyModifyInOneDirection = sc.Input[9];
	SCInputRef Version = sc.Input[10];
	SCInputRef AllowZeroValues = sc.Input[11];
	SCInputRef AllowNegativeValues= sc.Input[12];
	SCInputRef AllowCustomPropertiesForControlBarButton = sc.Input[13];

	if (sc.SetDefaults)	
	{
		sc.GraphName = "Trade Management by Study";
		sc.AutoLoop = 0;//We are using Manual looping for efficiency since historical data is not relevant with this study
		sc.GraphRegion = 0;
		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		
		Enabled.Name = "Enabled";
		Enabled.SetYesNo(0);

		OrderType.Name = "Order Type To Manage";
		OrderType.SetCustomInputStrings("Stops;Targets");
		OrderType.SetCustomInputIndex(0);

		ControlSubgraphRef.Name = "Controlling Subgraph Reference";
		ControlSubgraphRef.SetStudySubgraphValues(0, 0);

		AdjustmentTiming.Name = "Order Adjustment Timing";
		AdjustmentTiming.SetCustomInputStrings("Every N Seconds;On Bar Close");
		AdjustmentTiming.SetCustomInputIndex(1);

		AdjustmentFrequency.Name = "Adjustment Frequency (in Seconds)";
		AdjustmentFrequency.SetInt(5);

		OrderOffset.Name = "Trailing Offset (in Ticks)";
		OrderOffset.SetInt(2);

		ACSButtonNumber.Name = "ACS Control Bar Button # for Enable/Disable";
		ACSButtonNumber.SetInt(1);
		ACSButtonNumber.SetIntLimits(1,50);

		PositionType.  Name = "Position Type";
		PositionType.SetCustomInputStrings("All Positions;Long Only;Short Only");
		PositionType.SetCustomInputIndex(0);

		Logging.Name = "Detailed Logging (for debugging)";
		Logging.SetYesNo(0);

		OnlyModifyInOneDirection.Name = "Only Modify Stops In One Direction";
		OnlyModifyInOneDirection.SetYesNo(false);

		Version.SetInt(2);

		AllowZeroValues.Name = "Allow Zero Values";
		AllowZeroValues.SetYesNo(false);

		AllowNegativeValues.Name = "Allow Negative Values";
		AllowNegativeValues.SetYesNo(false);

		AllowCustomPropertiesForControlBarButton.Name = "Allow Custom 'Properties' for Control Bar Button";
		AllowCustomPropertiesForControlBarButton.SetYesNo(false);

		// Do not receive mouse pointer events when ACS Tool Control Bar button is pressed in/enabled
		sc.ReceivePointerEvents = false;

		//sc.CancelAllOrdersOnEntries = false;


		return;
	}



	//References to persistent variables
	SCDateTime& LastAdjustmentDateTime = sc.GetPersistentSCDateTime(1);

	int& MenuID = sc.GetPersistentInt(1);

	int & LastLogMessageIdentifier =sc.GetPersistentInt(2);

	/****************************************
	* First, handle mouse events
	*****************************************/

	if (sc.UpdateStartIndex == 0)
	{
		// set ACS Tool Control Bar tool tip
		sc.SetCustomStudyControlBarButtonHoverText(ACSButtonNumber.GetInt(), "Enable/Disable Trade Management by Study");

		// set Custom Study Control Bar button text if the input to allow custom properties is not true. Otherwise, rely upon what the user sets.
		if (!AllowCustomPropertiesForControlBarButton.GetYesNo())
			sc.SetCustomStudyControlBarButtonText(ACSButtonNumber.GetInt(), "TMS");

		sc.SetCustomStudyControlBarButtonEnable(ACSButtonNumber.GetInt(), Enabled.GetBoolean() );

		LastLogMessageIdentifier = 0;
	}

	// Wait for an event to enable study if not already enabled
	if (sc.MenuEventID == ACSButtonNumber.GetInt())
	{
		int state = (sc.PointerEventType == SC_ACS_TOOL_ON) ? 1 : 0;
		Enabled.SetYesNo(state);
	}

	/****************************************
	* next, execute the order management
	*****************************************/

	// If study is not enabled, exit without doing anything
	if (Enabled.GetYesNo() == 0)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 1)
		{
			LastLogMessageIdentifier = 1;
			sc.AddMessageToLog("Study not enabled.", 0);
		}

		return;
	}
	

	//For safety we must never do any order management while historical data is being downloaded.
	if (sc.ChartIsDownloadingHistoricalData(sc.ChartNumber))
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 2)
		{
			LastLogMessageIdentifier = 2;
			sc.AddMessageToLog("No order adjustments performed because historical data is being downloaded for chart.", 0);
		}
		return;
	}

	sc.SendOrdersToTradeService = !sc.GlobalTradeSimulationIsOn;

	// Flat state is a good time to reset LastAdjustmentDateTime
	s_SCPositionData PositionData;
	sc.GetTradePosition (PositionData);

	if (PositionData.PositionQuantity == 0)
	{
		LastAdjustmentDateTime = 0.0;

		if (Logging.GetYesNo() && LastLogMessageIdentifier != 3)
		{
			LastLogMessageIdentifier = 3;
			sc.AddMessageToLog("Position does not exist.", 0);
		}
		
		return;
	}

	if (PositionType.GetIndex() == 1 && PositionData.PositionQuantity < 0)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 4)
		{
			LastLogMessageIdentifier = 4;
			SCString  LogText("Position Type input is set to 'Long Only' and Position is Short. Position: ");
			SCString QuantityString;
			sc.OrderQuantityToString(PositionData.PositionQuantity, QuantityString);
			LogText += QuantityString;
			sc.AddMessageToLog(LogText, 0);
		}

		return;
	}

	if (PositionType.GetIndex() == 2 && PositionData.PositionQuantity > 0)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 5)
		{
			LastLogMessageIdentifier = 5;
			SCString  LogText("Position Type input is set to 'Short Only' and Position is Long. Position: ");
			SCString QuantityString;
			sc.OrderQuantityToString(PositionData.PositionQuantity, QuantityString);
			LogText += QuantityString;
			sc.AddMessageToLog(LogText, 0);
		}

		return;
	}

	// If the timing is every N seconds, check that we have surpassed the set frequency
	if (AdjustmentTiming.GetInt() == 0 && (sc.LatestDateTimeForLastBar - LastAdjustmentDateTime) < AdjustmentFrequency.GetFloat() * SECONDS)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 6)
		{
			LastLogMessageIdentifier = 6;
			sc.AddMessageToLog("Time interval has not elapsed yet for 'Every N Seconds'.", 0);
		}
		return;		
	}


	//  Update the LastAdjustmentDateTime

	LastAdjustmentDateTime = sc.LatestDateTimeForLastBar;

	// Retrieve the controlling subgraph array
	SCFloatArray ControllingSubgraph;
	sc.GetStudyArrayUsingID(ControlSubgraphRef.GetStudyID(), ControlSubgraphRef.GetSubgraphIndex(), ControllingSubgraph);



	// Based on Input settings, retrieve nearest stop/target order
	s_SCTradeOrder ExistingTargetOrStopOrder;
	int Result;

	if (OrderType.IntValue == 0)
		Result = sc.GetNearestStopOrder(ExistingTargetOrStopOrder);
	else
		Result = sc.GetNearestTargetOrder(ExistingTargetOrStopOrder);

	if (Result == 0)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 7)
		{
			LastLogMessageIdentifier = 7;
			sc.AddMessageToLog("There is no Stop/Target Attached Order found.", 0);
		}
		return;
	}
	
	// only process open orders
	if (ExistingTargetOrStopOrder.OrderStatusCode != SCT_OSC_OPEN)
	{	
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 8)
		{
			LastLogMessageIdentifier = 8;
			sc.AddMessageToLog("The found Stop/Target order is not in an Open state.", 0);
		}
		return;
	}
	
	// Modify the order
	int BarIndex = sc.ArraySize - 1;
	
	if(AdjustmentTiming.GetIndex() == 1 )
		BarIndex = sc.ArraySize - 2;


	if (!AllowZeroValues.GetYesNo() && ControllingSubgraph[BarIndex]== 0.0)
	{	
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 9)
		{
			LastLogMessageIdentifier = 9;
			sc.AddMessageToLog("The 'Controlling Subgraph Reference' value is 0 at the last bar.", 0);
		}
		return;
	}


	if (!AllowNegativeValues.GetYesNo() && ControllingSubgraph[BarIndex] < 0)
	{	
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 15)
		{
			LastLogMessageIdentifier = 15;
			sc.AddMessageToLog("The 'Controlling Subgraph Reference' value is < 0 at the last bar.", 0);
		}
		return;
	}


	double NewPrice = 0.0;
	
	if (ExistingTargetOrStopOrder.BuySell == BSE_SELL)
	{
		NewPrice = ControllingSubgraph[BarIndex] - OrderOffset.GetInt() * sc.TickSize;
	}
	else
	{
		NewPrice = ControllingSubgraph[BarIndex] + OrderOffset.GetInt() * sc.TickSize;
	}

	//Round the price to an actual order price.
	NewPrice= sc.RoundToTickSize(NewPrice);

	//If the price has not changed, then do not modify.
	if (sc.FormattedEvaluateUsingDoubles(ExistingTargetOrStopOrder.Price1, sc.BaseGraphValueFormat, EQUAL_OPERATOR, NewPrice, sc.BaseGraphValueFormat ) 
		|| sc.FormattedEvaluateUsingDoubles(ExistingTargetOrStopOrder.LastModifyPrice1, sc.BaseGraphValueFormat, EQUAL_OPERATOR, NewPrice, sc.BaseGraphValueFormat)
		)
	{	
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 10)
		{
			LastLogMessageIdentifier = 10;

			SCString LogMessage;
			LogMessage.Format("There is not a new price to modify the order to. Existing order: Price1=%f, LastModifyPrice1 =%f. New study subgraph price with offset=%f.", ExistingTargetOrStopOrder.Price1, ExistingTargetOrStopOrder.LastModifyPrice1, NewPrice);

			sc.AddMessageToLog(LogMessage, 0);
		}
		return;
	}

	// If modify stop price is in wrong direction, then do not modify.
	if (OrderType.IntValue == 0 //Stops
		&&  OnlyModifyInOneDirection.GetYesNo() &&
		((PositionData.PositionQuantity < 0 && sc.FormattedEvaluateUsingDoubles(NewPrice, sc.BaseGraphValueFormat, GREATER_OPERATOR, ExistingTargetOrStopOrder.Price1, sc.BaseGraphValueFormat)) ||
		(PositionData.PositionQuantity > 0 && sc.FormattedEvaluateUsingDoubles(NewPrice, sc.BaseGraphValueFormat, LESS_OPERATOR, ExistingTargetOrStopOrder.Price1, sc.BaseGraphValueFormat)) )	)
	{
		if (Logging.GetYesNo() && LastLogMessageIdentifier != 14)
		{
			LastLogMessageIdentifier = 14;
			sc.AddMessageToLog("Modify stop price is in wrong direction.", 0);
		}
		return;
	}
	
	s_SCNewOrder ModifyOrder;
	ModifyOrder.InternalOrderID = ExistingTargetOrStopOrder.InternalOrderID;
	ModifyOrder.Price1 = NewPrice;

	Result = sc.ModifyOrder(ModifyOrder);

	if (Result == 1 && Logging.GetYesNo() && LastLogMessageIdentifier != 11)
	{
		LastLogMessageIdentifier = 11;
		SCString  LogText;
		LogText.Format("The Stop/Target order has been modified. Internal Order ID: %d. New Price: %f", ExistingTargetOrStopOrder.InternalOrderID, NewPrice);

		sc.AddMessageToLog( LogText , 0);
	}
	else if (Result == SCT_SKIPPED_AUTO_TRADING_DISABLED && Logging.GetYesNo() && LastLogMessageIdentifier != 13)
	{
		LastLogMessageIdentifier = 13;
		SCString  LogText;
		LogText.Format("'Trade >> Auto Trading Enabled' is disabled.");

		sc.AddMessageToLog( LogText , 0);
	}
	else if (Result != 1 && Logging.GetYesNo() && LastLogMessageIdentifier != 12)
	{
		LastLogMessageIdentifier = 12;
		SCString  LogText;
		LogText.Format("There was an error modifying the Stop/Target order. Internal Order ID: %d. New Price: %f. Error Code %d.", ExistingTargetOrStopOrder.InternalOrderID, NewPrice, Result);

		sc.AddMessageToLog( LogText , 0);
	}


}


