
#include "sierrachart.h"
#include "scstudyfunctions.h"

#include <vector>

/*============================================================================*/
int TradingLevelsStudy_RequestValuesFromServer
	( SCStudyInterfaceRef sc
	, const SCString& BaseWebsiteURL
	, int& r_RequestState
	, const char* AlternateSymbol
	);

void TradingLevelsStudy_ResetStateForNextRequest
	( SCStudyInterfaceRef sc
	, int& r_RequestState
	, SCDateTime& r_RequestDateTime
	, std::vector<SCString>* p_ValuesLineForDates
	, std::vector<std::vector<char*>>* p_PointersToValuesForDates
	);

unsigned int TradingLevelsStudy_GetValuesForDate
	( SCStudyInterfaceRef sc
	, SCDateTime BarDate
	, float* Values
	, unsigned int ArraySize
	, std::vector<std::vector<char*>>* p_PointersToValuesForDates
	);

void TradingLevelsStudyCore
	( SCStudyInterfaceRef sc
	, const SCString& RequestURL
	, const int RequestIntervalInMinutes
	, std::vector<SCString>* p_ValuesLineForDates
	, std::vector<std::vector<char*>>* p_PointersToValuesForDates
	, int& r_RequestState
	, SCDateTime& r_RequestDateTime
	, int& r_ClearAtMidnight
	, bool AllowRequestLevelsFromServer
	, int& r_AwaitingNextRequestForLevels
	, const char* InAlternateSymbol
	);

/*============================================================================*/
SCSFExport scsf_TradingLevelsStudy(SCStudyInterfaceRef sc)
{
	SCInputRef InVersion = sc.Input[0];
	SCInputRef InRequestIntervalInMinutes = sc.Input[1];
	SCInputRef InUseTimeRangeForLevelsRequest = sc.Input[2];
	SCInputRef InTimeRangeTimeZone = sc.Input[3];
	SCInputRef InRequestStartTime = sc.Input[4];
	SCInputRef InRequestEndTime = sc.Input[5];
	SCInputRef InAlternateSymbol = sc.Input[6];

	const SCString RequestURL = "https://www.sierrachart.com/API.php?Service=PriceLevels";
	
	if (sc.SetDefaults)
	{
		sc.GraphName = "Trading Levels";

		sc.GraphRegion = 0;
		sc.ScaleRangeType = SCALE_SAMEASREGION;
		sc.ValueFormat =  VALUEFORMAT_INHERITED;
		sc.AutoLoop = 0;


		sc.StudyDescription = "Requests line level values from Web server in the format 'YYYY-MM-DD, [level_value_1], [level_value_2], [level_value_3], [level_value_4], and so on, where the level_value_# is equal to the value of the line. There can be up to SC_SUBGRAPHS_AVAILABLE level values.";


		for (int Index = 0; Index < SC_SUBGRAPHS_AVAILABLE; Index++)
		{
			SCString SubgraphName;
			SubgraphName.Format("Level %d", Index+1);
			sc.Subgraph[Index].Name = SubgraphName;
			sc.Subgraph[Index].DrawStyle = DRAWSTYLE_DASH;
			sc.Subgraph[Index].LineWidth = 2;
			sc.Subgraph[Index].PrimaryColor = RGB(0, 255, 0);
			sc.Subgraph[Index].DrawZeros = 0;  // false
			sc.Subgraph[Index].LineLabel = LL_DISPLAY_VALUE | LL_VALUE_ALIGN_CENTER | LL_VALUE_ALIGN_VALUES_SCALE;
		}

		InRequestIntervalInMinutes.Name = "Request Interval in Minutes";
		InRequestIntervalInMinutes.SetInt(60);
		InRequestIntervalInMinutes.SetIntLimits(1, MINUTES_PER_DAY);

		InUseTimeRangeForLevelsRequest.Name = "Use Time Range For Levels Request";
		InUseTimeRangeForLevelsRequest.SetYesNo(false);

		InTimeRangeTimeZone.Name = "Time Range Time Zone";
		InTimeRangeTimeZone.SetTimeZone(TIMEZONE_NEW_YORK);

		InRequestStartTime.Name = "Request Start Time";
		InRequestStartTime.SetTime(0);

		InRequestEndTime.Name = "Request End Time";
		InRequestEndTime.SetTime(SECONDS_PER_DAY - 1);

		InAlternateSymbol.Name = "Alternate Symbol";
		InAlternateSymbol.SetString("");

		sc.TextInputName = "Request Identifier";

		return;
	}


	std::vector<SCString>* p_ValuesLineForDates = (std::vector<SCString>*)sc.GetPersistentPointer(1);
	std::vector<std::vector<char*>>* p_PointersToValuesForDates = (std::vector<std::vector<char*>>*)sc.GetPersistentPointer(2);

	int& r_RequestState = sc.GetPersistentInt(1);
	int& r_ClearAtMidnight = sc.GetPersistentInt(2);
	int& r_AwaitingNextRequestForLevels = sc.GetPersistentInt(3);

	SCDateTime& r_RequestDateTime = sc.GetPersistentSCDateTime(2);

	SCDateTime CurrentDateTimeInRequestTimeZone;

	bool AllowRequestLevelsFromServer = true;

	if (InUseTimeRangeForLevelsRequest.GetYesNo())
	{
		CurrentDateTimeInRequestTimeZone = sc.CurrentSystemDateTime;
		CurrentDateTimeInRequestTimeZone = sc.ConvertDateTimeFromChartTimeZone(CurrentDateTimeInRequestTimeZone, InTimeRangeTimeZone.GetTimeZone());

		if (CurrentDateTimeInRequestTimeZone.GetTimeInSeconds() < InRequestStartTime.GetTime()
			|| CurrentDateTimeInRequestTimeZone.GetTimeInSeconds() > InRequestEndTime.GetTime())
			AllowRequestLevelsFromServer = false;
	}

	TradingLevelsStudyCore
		( sc
		, RequestURL
		, InRequestIntervalInMinutes.GetInt()
		, p_ValuesLineForDates
		, p_PointersToValuesForDates
		, r_RequestState
		, r_RequestDateTime
		, r_ClearAtMidnight
		, AllowRequestLevelsFromServer
		, r_AwaitingNextRequestForLevels
		, InAlternateSymbol.GetString()
		);
}

/*============================================================================*/
void TradingLevelsStudyCore
( SCStudyInterfaceRef sc
, const SCString& RequestURL
, const int RequestIntervalInMinutes
, std::vector<SCString>* p_ValuesLineForDates
, std::vector<std::vector<char*>>* p_PointersToValuesForDates
, int& r_RequestState
, SCDateTime& r_RequestDateTime
, int& r_ClearAtMidnight
, bool AllowRequestLevelsFromServer
, int& r_AwaitingNextRequestForLevels
, const char* InAlternateSymbol
)
{
	if(sc.LastCallToFunction)
	{
		if(p_ValuesLineForDates != NULL)
		{
			delete p_ValuesLineForDates;
			sc.SetPersistentPointer(1, NULL);
		}

		if(p_PointersToValuesForDates != NULL)
		{
			delete p_PointersToValuesForDates;
			sc.SetPersistentPointer(2, NULL);
		}

		return;
	}

	if (p_ValuesLineForDates == NULL)
	{
		p_ValuesLineForDates = new std::vector<SCString>;

		if(p_ValuesLineForDates != NULL)
			sc.SetPersistentPointer(1, p_ValuesLineForDates);
		else
		{
			sc.AddMessageToLog("Memory allocation error.", 1);
			return;
		}
	}

	if (p_PointersToValuesForDates == NULL)
	{
		p_PointersToValuesForDates = new std::vector<std::vector< char*> >;

		if(p_PointersToValuesForDates != NULL)
			sc.SetPersistentPointer(2, p_PointersToValuesForDates);
		else
		{
			sc.AddMessageToLog("Memory allocation error.", 1);
			return;
		}
	}

	if (sc.IsFullRecalculation)
		r_ClearAtMidnight = false;

	if (AllowRequestLevelsFromServer || r_RequestState == HTTP_REQUEST_MADE)
	{
		//Request data on a full recalculation and also at the specified interval

		// This is to solve a condition when Apply is used from the Chart Studies window, and the initial state of the RequestState is remembered
		if (sc.UpdateStartIndex == 0  && r_RequestState == HTTP_REQUEST_MADE)
			TradingLevelsStudy_ResetStateForNextRequest(sc, r_RequestState, r_RequestDateTime, p_ValuesLineForDates, p_PointersToValuesForDates);
		else if (sc.UpdateStartIndex == 0  && r_RequestState == HTTP_REQUEST_RECEIVED)
			TradingLevelsStudy_ResetStateForNextRequest(sc, r_RequestState, r_RequestDateTime, p_ValuesLineForDates, p_PointersToValuesForDates);
		else if (r_RequestDateTime.IsUnset()
			|| ( (sc.CurrentSystemDateTime - r_RequestDateTime) >= RequestIntervalInMinutes*MINUTES)
			)// Request interval has elapsed
			TradingLevelsStudy_ResetStateForNextRequest(sc, r_RequestState, r_RequestDateTime, p_ValuesLineForDates, p_PointersToValuesForDates);

		if (TradingLevelsStudy_RequestValuesFromServer(sc, RequestURL, r_RequestState, InAlternateSymbol))
		{
			if (r_RequestState == HTTP_REQUEST_MADE)
				r_AwaitingNextRequestForLevels = false;

			return;//Return here since we need to wait for the response
		}
	}


	if (sc.HTTPRequestID != 0)//response received
	{
		r_RequestState = HTTP_REQUEST_RECEIVED;

		if (sc.HTTPResponse == ACSIL_HTTP_REQUEST_ERROR_TEXT
			|| sc.HTTPResponse == ACSIL_HTTP_EMPTY_RESPONSE_TEXT)
		{
			sc.AddMessageToLog("There was an error requesting data from the server.", true);
		}

		if (sc.UpdateStartIndex == 0 && sc.HTTPResponse == "NO_DATA")
			sc.AddMessageToLog("There are no price levels for the given parameters.", false);

		if (strstr(sc.HTTPResponse.GetChars(), "CLEAR_AT_MIDNIGHT") != NULL)
		{
			sc.AddMessageToLog("Received clear at midnight command.", false);
			r_ClearAtMidnight = true;
		}
	}

	if (r_RequestState != HTTP_REQUEST_RECEIVED)
		return;

	SCDateTime StartIndexDate = sc.BaseDateTimeIn[sc.UpdateStartIndex].GetDate();
	SCDateTime LastBarDate = sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate();

	if (r_ClearAtMidnight && StartIndexDate != LastBarDate)
	{
		//clear existing levels
		p_ValuesLineForDates->clear();
		p_PointersToValuesForDates->clear();

		for (int BarIndex = sc.ArraySize - 1; BarIndex >= 0; --BarIndex)
		{
			bool FoundNotZeroValue = false;

			for (unsigned int LevelIndex = 0; LevelIndex < SC_SUBGRAPHS_AVAILABLE; LevelIndex++)
			{
				if (sc.Subgraph[LevelIndex][BarIndex] != 0.0)
					FoundNotZeroValue = true;

				sc.Subgraph[LevelIndex][BarIndex] = 0;
			}
		}

		r_AwaitingNextRequestForLevels = true;

	}

	if (r_AwaitingNextRequestForLevels)
		return;

	bool FullRecalculate = false;

	if(p_ValuesLineForDates->size() == 0)
	{
		sc.HTTPResponse.ParseLines(*p_ValuesLineForDates);

		SCString Message;
		Message.Format("Received %d price level lines of data from server.", p_ValuesLineForDates->size());
		sc.AddMessageToLog(Message, false);

		std::vector<char *> EmptyVector;

		for (int Index = 0; Index < (int)p_ValuesLineForDates->size(); Index++)
		{
			p_PointersToValuesForDates->push_back(EmptyVector);
			p_ValuesLineForDates->at(Index).Tokenize(",", p_PointersToValuesForDates->back());
		}

		FullRecalculate = true;
	}
	
	if(p_PointersToValuesForDates->empty())
		return;

	if (FullRecalculate)
		sc.UpdateStartIndex = 0;

	float LevelValues[SC_SUBGRAPHS_AVAILABLE] = { 0 };

	SCDateTime PriorDate;

	int DataLineType = 0;//Default: Date, RepeatingValues
	if (!p_ValuesLineForDates->empty()) 
	{
		if (p_ValuesLineForDates->at(0).CompareNoCase("Date, Label, Color, Width, Value") == 0)
		{
			DataLineType = 1;
		}
		else if (p_ValuesLineForDates->at(0).CompareNoCase("Date, Label, Color, Width, TopValue, BottomValue") == 0)
		{
			DataLineType = 2;
		}
	}

	struct s_LineConfig
	{
		SCDateTime Date;
		SCString Label;
		uint32_t Color;
		uint32_t Width;
		float Value1;
		float Value2;
	};

	if (DataLineType == 0)
	{
		unsigned int NumberUsedLevels = 0;
		for (int BarIndex = sc.UpdateStartIndex; BarIndex < sc.ArraySize; BarIndex++)
		{
			SCDateTime IndexDate = sc.GetTradingDayDate(sc.BaseDateTimeIn[BarIndex]);

			if (PriorDate != IndexDate)
			{
				NumberUsedLevels = TradingLevelsStudy_GetValuesForDate(sc, IndexDate, LevelValues, SC_SUBGRAPHS_AVAILABLE, p_PointersToValuesForDates);
			}

			PriorDate = IndexDate;

			for (unsigned int LevelIndex = 0; LevelIndex < NumberUsedLevels; LevelIndex++)
				sc.Subgraph[LevelIndex][BarIndex] = LevelValues[LevelIndex];

		}
	}
	else if (DataLineType == 1)
	{

	}
}

/*============================================================================*/
//Returns 1 if request has been made. Returns 0 if request has not been made.
int TradingLevelsStudy_RequestValuesFromServer
( SCStudyInterfaceRef sc
, const SCString& BaseWebsiteURL
, int& r_RequestState
, const char* AlternateSymbol
)
{
	SCString FullURL;

	if (r_RequestState == HTTP_REQUEST_NOT_SENT)
	{
		if (sc.TextInput.IsEmpty())
			return 0;

		const char* RequestSymbol = NULL;

		if (AlternateSymbol != NULL && AlternateSymbol[0] != '\0')
			RequestSymbol = AlternateSymbol;
		else
			RequestSymbol = sc.Symbol.GetChars();

		FullURL.Format
			( "%s&Username=%s&Symbol=%s&SCDLLName=%s"
			, BaseWebsiteURL.GetChars()
			, sc.UserName.GetChars()
			, RequestSymbol
			, sc.TextInput.GetChars()
			);

		if (!sc.MakeHTTPRequest(FullURL))
		{
			sc.AddMessageToLog("Error making HTTP Request.", true);
			r_RequestState = HTTP_REQUEST_ERROR;
		}
		else
		{
			r_RequestState = HTTP_REQUEST_MADE;
			SCString LogMessage("Requesting data from Trading Levels server: ");
			LogMessage += FullURL;
			sc.AddMessageToLog(LogMessage, false);
		}

		return 1;
	}

	return 0;
}

/*============================================================================*/
void TradingLevelsStudy_ResetStateForNextRequest
( SCStudyInterfaceRef sc
, int& r_RequestState
, SCDateTime& r_RequestDateTime
, std::vector<SCString>* p_ValuesLineForDates
, std::vector<std::vector<char*>>* p_PointersToValuesForDates
)
{
	p_ValuesLineForDates->clear();
	p_PointersToValuesForDates->clear();

	r_RequestState = HTTP_REQUEST_NOT_SENT;
	r_RequestDateTime = sc.CurrentSystemDateTime;
}
/*============================================================================*/
unsigned int TradingLevelsStudy_GetValuesForDate
( SCStudyInterfaceRef sc
, SCDateTime BarDate
, float* Values
, unsigned int ArraySize
, std::vector<std::vector<char*>>* p_PointersToValuesForDates
)
{
	//Clean array
	for(unsigned int Index = 0; Index < ArraySize; Index++)
	{
		Values[Index] = 0.0;
	}

	if (p_PointersToValuesForDates->empty())
		return 0;

	unsigned int NumberLevels = 0;
	for(unsigned int Index = 0; Index < p_PointersToValuesForDates->size(); Index++)
	{
		SCDateTime DataDate =sc.DateStringToSCDateTime(p_PointersToValuesForDates->at(Index)[0]);
		if (DataDate != BarDate)
			continue;

		unsigned int Size = min(ArraySize + 1, static_cast<unsigned int>(p_PointersToValuesForDates->at(Index).size()));

		for (unsigned int ItemIndex = 1; ItemIndex <  Size; ItemIndex++)
		{
			Values[ItemIndex - 1] = (float)sc.StringToDouble(p_PointersToValuesForDates->at(Index)[ItemIndex]);

		}
		NumberLevels = Size;
		break;
	}

	return NumberLevels;
}
/*============================================================================*/

