
// The top of every source code file must include this line
#include "sierrachart.h"

SCDLLName("Generic Study")

/**********************
 *  Helper Functions  *
 **********************/

// Put any helper functions here

/***********************
 *  Exported Function  *
 ***********************/

SCSFExport scsf_GenericStudy(SCStudyInterfaceRef sc)
{
  SCSubgraphRef Normalized = sc.Subgraph[0];

  SCInputRef Price1 = sc.Input[0];
  SCInputRef Price2 = sc.Input[1];

  // Section 1 - Set the configuration variables and defaults
  if (sc.SetDefaults) {
    sc.GraphName = "Normalize Two Prices v1.0";
    sc.GraphRegion = 1;
    sc.AutoLoop = 1;

    Normalized.Name = "Normalized Price";
    Normalized.DrawStyle = DRAWSTYLE_LINE;
    Normalized.PrimaryColor = RGB(0, 255, 0);
    Normalized.LineWidth = 2;

    Price1.Name = "Price 1";
    Price1.SetStudySubgraphValues(0, SC_LAST);

    Price2.Name = "Price 2";
    Price2.SetStudySubgraphValues(1, SC_LAST);

    sc.IncludeInSpreadsheet = 0;
    sc.IncludeInStudySummary = 1;

    return;
  }

  // Section 2 - Do data processing here

  if (!sc.IsFullRecalculation && sc.Index != sc.ArraySize-1) {
    return;
  }

  SCFloatArray price1;
  sc.GetStudyArrayUsingID(Price1.GetStudyID(), Price1.GetSubgraphIndex(), price1);
  SCFloatArray price2;
  sc.GetStudyArrayUsingID(Price2.GetStudyID(), Price2.GetSubgraphIndex(), price2);

  if (price1.GetArraySize() == 0 || price2.GetArraySize() == 0) {
    return;
  }

  Normalized[sc.Index] = price2[sc.Index] / price1[sc.Index];
}
