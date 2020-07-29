#include "sierrachart.h"
#include <vector>

SCSFExport scsf_POC(SCStudyInterfaceRef sc)
{
 if (sc.SetDefaults)
 {
 sc.GraphName = "Identifying POC Values";
 sc.GraphRegion = 0;
 sc.Subgraph[0].Name = "POC Value";
 sc.Subgraph[1].Name = "POC Price";

 sc.MaintainVolumeAtPriceData = 1;

 sc.AutoLoop = 1;
 sc.FreeDLL = 1;

 return;
 }

 int poc_value = 0; 
 float poc_price = 0;
 std::vector<int> v_poc_values = {};
 std::vector<float> v_poc_prices = {};
 const s_VolumeAtPriceV2 *p_volume_at_price = NULL;

 int number_of_levels = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(sc.Index);
 for (int i = 0; i < number_of_levels; i++)
 {
 if (!sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.Index, i, &p_volume_at_price))
 break;

 int last_volume_value = p_volume_at_price->Volume;
 if (last_volume_value > poc_value)
 {
 poc_value = last_volume_value;
 poc_price = p_volume_at_price->PriceInTicks * sc.TickSize;
 v_poc_values.push_back(poc_value);
 v_poc_prices.push_back(poc_price);
 }
 }
 
 sc.Subgraph[0][sc.Index] = (float)poc_value;
 sc.Subgraph[1][sc.Index] = (float)poc_price;

 //log 
 SCString log_prices;
 SCString log_values;
 
 if (v_poc_prices.size() != v_poc_values.size())
 return;

 for (int i = 0; i < v_poc_prices.size(); i++)
 {
 log_prices.Format("%f", v_poc_prices.at(i));
 log_values.Format("%d", v_poc_values.at(i));
 }
 
 sc.AddMessageToTradeServiceLog(log_prices, 0); 
 sc.AddMessageToTradeServiceLog(log_values, 1);
}