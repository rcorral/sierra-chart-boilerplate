#ifndef _SCSYMBOLDATA_H_
#define _SCSYMBOLDATA_H_

typedef unsigned int t_Volume;

#pragma pack(push, 8)


enum TickDirectionEnum
{ DOWN_TICK = -1
, NO_TICK_DIRECTION = 0
, UP_TICK = 1
};

// This structure is used in ACSIL studies.  Therefore, back compatibility
// needs to be maintained.
struct s_SCBasicSymbolData
{
	int DailyNumberOfTrades;
	float DailyHigh;
	float DailyLow;
	int LastTradeVolume;
	unsigned int AskQuantity;
	unsigned int BidQuantity;
	float Bid;
	float Ask;
	float LastTradePrice;
	float DailyOpen;
	float CurrencyValuePerTick;
	float SettlementPrice;
	unsigned int OpenInterest;
	unsigned int DailyVolume;
	unsigned int SharesOutstanding;
	float EarningsPerShare;
	int TickDirection;  // TickDirectionEnum
	int LastTradeAtSamePrice;
	float StrikePrice;
	char Description[256];
	// The last date-time that historical Intraday data was modified,
	// requiring a reload of the Intraday data file.
	SCDateTime LastDateTimeHistoricalIntradayDataModified;
	char Symbol[128];
	float SellRolloverInterest;

	// Standard Sierra Chart price format code.  This must correspond to the
	// pricing format actually used after the price values are adjusted by
	// the DisplayPriceMultiplier.
	int PriceFormat;
	char Unused_2[796];
	float ContractSize;
	float BuyRolloverInterest;
	char TradeIndicator;
	char LastTradeAtBidAsk;// can be SC_TS_BID or SC_TS_ASK 

	char Unused_1;
	
	char VolumeValueFormat;

	// This must correspond to the pricing format actually used after the
	// price values are adjusted by the DisplayPriceMultiplier.
	float TickSize;

	SCDateTime LastTradeDateTime;
	int AccumulatedLastTradeVolume; 
	SCDateTime LastTradingDateForFutures;
	
	char TradingStatus;

	char Unused_3[3];

	// The Date value (SCDateTime) that the current values in the
	// BasicSymbolData object are for.  This is going to be what is
	// considered the trading day date.  So this will be the following day in
	// the case when the trading begins in the prior day at the beginning of
	// the evening session.
	int TradingDayDate;

	SCDateTime LastMarketDepthUpdateDateTime;

	float DisplayPriceMultiplier;

	SCDateTime SettlementPriceDate;
	SCDateTime DailyOpenPriceDate;
	SCDateTime DailyHighPriceDate;
	SCDateTime DailyLowPriceDate;
	SCDateTime DailyVolumeDate;

	int NumberOfTradesAtCurrentPrice;

	/*======================================================================*/
	s_SCBasicSymbolData()
	{
		Clear();
	};

	/*======================================================================*/
	void Clear()
	{
		memset(this, 0, sizeof(s_SCBasicSymbolData));
		PriceFormat = -1;
		TickDirection=NO_TICK_DIRECTION;		
		DisplayPriceMultiplier = 1.0;
	}

	/*======================================================================*/
	s_SCBasicSymbolData(const s_SCBasicSymbolData& Src)
	{
		Copy(Src);
	}
	
	/*======================================================================*/
	s_SCBasicSymbolData& operator = (const s_SCBasicSymbolData& Src)
	{
		Copy(Src);
		return *this;
	}
	
	/*======================================================================*/
	void Copy(const s_SCBasicSymbolData& Src)
	{
		if (&Src == this)
			return;
		
		memcpy(this, &Src, sizeof(s_SCBasicSymbolData));
	}
	
	/*======================================================================*/
	bool operator == (const s_SCBasicSymbolData& That)
	{
		return (memcmp(this, &That, sizeof(s_SCBasicSymbolData)) == 0);
	}

	/*======================================================================*/
	void MultiplyData(float Multiplier, bool InvertPrices)
	{
		if (Multiplier != 1.0)
		{
			Ask *= Multiplier;
			Bid *= Multiplier;
			DailyHigh *= Multiplier;
			LastTradePrice *= Multiplier;
			DailyLow *= Multiplier;
			DailyOpen *= Multiplier;
			SettlementPrice *= Multiplier;
		}

		if (InvertPrices)
		{
			Ask = SafeInverse(Ask);
			Bid = SafeInverse(Bid);
			DailyHigh = SafeInverse(DailyHigh);
			LastTradePrice = SafeInverse(LastTradePrice);
			DailyLow = SafeInverse(DailyLow);
			DailyOpen = SafeInverse(DailyOpen);
			SettlementPrice = SafeInverse(SettlementPrice);

		}
	}

	float GetDailyPriceChange() const
	{
		float DailyPriceChange = 0.0;
		if (SettlementPrice != 0.0)
		{
			DailyPriceChange = LastTradePrice - SettlementPrice;
		}
		return DailyPriceChange;
	}



	/*======================================================================*/
	static float SafeInverse(float Number)
	{
		if (Number == 0.0f)
			return 0.0f;

		return 1.0f / Number;
	}
};

#pragma pack(pop)

#endif
