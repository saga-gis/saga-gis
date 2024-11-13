
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              GSGrid_Zonal_Statistics.cpp              //
//                                                       //
//              Copyright (C) 2005-2022 by               //
//                    Volker Wichmann                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GSGrid_Zonal_Statistics.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Zonal_Statistics::CGSGrid_Zonal_Statistics(void)
{
    Set_Name		(_TL("Zonal Grid Statistics"));

	Set_Author		(_TL("Volker Wichmann (c) 2005-2022"));

    Set_Version     ("2.0");

	Set_Description	(_TW(
		"The tool allows one to calculate zonal statistics over a set of input grids and reports the "
        "statistics in a table.\n"
		"The tool first creates a contingency table of unique condition units (UCUs) on which the "
        "statistics are calculated. These UCUs are delineated from a zonal grid (e.g. sub catchments) "
        "and optional categorical grids (e.g. landcover, soil, ...). The derived UCUs can be output "
        "as a grid dataset.\n"
        "The tool then calculates descriptive statistics (n, min, max, mean, standard "
		"deviation and sum) for each UCU from (optional) grids with continuous data (e.g. slope). A grid "
        "storing aspect must be treated specially (circular statistics), please use the \"Aspect\" "
        "input parameter for such a grid.\n\n"
		"The tool has four different modes of operation:\n"
		"(1) only a zonal grid is used as input. This results in a simple contingency table with "
		"the number of grid cells in each zone.\n"
		"(2) a zonal grid and additional categorical grids are used as "
		"input. This results in a contingency table with the number of cells in each UCU.\n"
		"(3) a zonal grid "
		"and additional grids with continuous data are used as input. This results in a contingency table "
		"with the number of cells in each zone and the corresponding statistics for each continuous grid.\n"
		"(4) a zonal grid, additional categorical grids and additional "
		"grids with continuous data are used as input. This results in a contingency table with the number "
		"of cells in each UCU and the corresponding statistics for each continuous grid.\n"
		"\n"
		"Depending on the mode of operation, the output table contains information about the categorical "
		"combination of each UCU, the number of cells in each UCU and the statistics for each UCU. A "
		"typical output table may look like this:\n"
		"<table border=\"1\">"
		"<tr><td>ID UCU</td><td>ID Zone</td><td>ID 1stCat</td><td>ID 2ndCat</td><td>Count UCU</td><td>N 1stCont</td><td>MIN 1stCont</td><td>MAX 1stCont</td><td>MEAN 1stCont</td><td>STDDEV 1stCont</td><td>SUM 1stCont</td></tr>"
		"<tr><td>1      </td><td>0      </td><td>2        </td><td>6        </td><td>6        </td><td>6        </td><td>708.5      </td><td>862.0      </td><td>734.5       </td><td>62.5          </td><td>4406.8     </td></tr>"
		"<tr><td>2      </td><td>0      </td><td>3        </td><td>4        </td><td>106      </td><td>106      </td><td>829.1      </td><td>910.1      </td><td>848.8       </td><td>28.5          </td><td>89969.0    </td></tr>"
		"</table>"
        "\n\n"
        "Note: in the case you like to convert some one of the statistics back to a grid dataset, you "
        "can use the following procedure. Run the tool and output the UCU grid. Then edit the output "
        "table to match your needs (delete all fields besides the UCU identifier and the fields you like "
        "to create grids from). Then use both datasets in the \"Grids from Classified Grid and Table\" "
        "tool.\n\n"
	));

	Parameters.Add_Grid(
		"", "ZONES", _TL("Zone Grid"),
		_TL("The grid defining the zones to analyze [NoData;categorical values]."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		"", "CATLIST", _TL("Categorical Grids"),
		_TL("Additional grids used to delineate the UCUs [NoData;categorical values]."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid_List(
		"", "STATLIST", _TL("Grids to Analyse"),
		_TL("The grids with continuous data for which the statistics are calculated [NoData;continuous values]."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "ASPECT", _TL("Aspect"),
		_TL("A grid encoding the aspect of each cell [radians]."),
		PARAMETER_INPUT_OPTIONAL
	);

    Parameters.Add_Grid(
        "", "UCU", _TL("UCUs"),
        _TL("The derived unique condition areas (UCU)."),
        PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Long
    );

	Parameters.Add_Table(
		"", "OUTTAB", _TL("Zonal Statistics"),
		_TL("The summary table with the statistics for each UCU."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		"", "SHORTNAMES", _TL("Short Field Names"),
		_TL("Shorten the field names to ten characters (as this is the limit for field names in shapefiles)."),
		true
	);
}

//---------------------------------------------------------
CGSGrid_Zonal_Statistics::~CGSGrid_Zonal_Statistics(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//							  							 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Zonal_Statistics::On_Execute(void)
{
    CSG_Grid                *pZones		= Parameters("ZONES")->asGrid();
    CSG_Parameter_Grid_List *pCatList	= Parameters("CATLIST")->asGridList();
    CSG_Parameter_Grid_List *pStatList	= Parameters("STATLIST")->asGridList();
	CSG_Grid                *pAspect	= Parameters("ASPECT")->asGrid();
    CSG_Grid                *pUCU       = Parameters("UCU")->asGrid();
	CSG_Table               *pOutTab    = Parameters("OUTTAB")->asTable();
	bool                    bShortNames	= Parameters("SHORTNAMES")->asBool();


    //---------------------------------------------------------
    int iStats = pStatList->Get_Grid_Count();
    
    if (pAspect != NULL)
    {
        iStats++;
    }
    
    if (iStats == 0)
    {
        iStats++; // only UCUs, no stats (besides cell count)
    }

    if( pUCU != NULL )
    {
        pUCU->Assign_NoData();
    }


    //---------------------------------------------------------
    std::map<std::vector<int>, std::vector<STATS> > mapUCUs;  // key = vector of categories describing the UCU, value = vector of statistics for each grid
    
    sLong iNoDataCount = 0;

    for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
    {
        for(int x=0; x<Get_NX(); x++)
        {
            std::vector<int>    vCategories;

            vCategories.push_back(pZones->asInt(x, y));

            for(int i=0; i<pCatList->Get_Grid_Count(); i++)
            {
                vCategories.push_back(pCatList->Get_Grid(i)->asInt(x, y));
            }

            std::map<std::vector<int>, std::vector<STATS> >::iterator it = mapUCUs.find(vCategories);

            if( it == mapUCUs.end() )
            {
                mapUCUs.insert(std::pair<std::vector<int>, std::vector<STATS> >(vCategories, std::vector<STATS>()));
                mapUCUs[vCategories].resize(iStats);
            }

            mapUCUs[vCategories][0].cells.push_back(pZones->Get_System().Get_IndexFromRowCol(x, y));
            
            for(int i=0; i<pStatList->Get_Grid_Count(); i++)
            {
                if( pStatList->Get_Grid(i)->is_NoData(x, y) )
                {
                    iNoDataCount++;
                }
                else
                {
                    _Set_Stats(mapUCUs, vCategories, i, pStatList->Get_Grid(i)->asDouble(x, y), false);
                }
            }

            if( pAspect != NULL )
            {
                if( pAspect->is_NoData(x, y) )
                {
                    iNoDataCount++;
                }
                else
                {
                    _Set_Stats(mapUCUs, vCategories, pStatList->Get_Grid_Count(), pAspect->asDouble(x, y), true);
                }
            }
        }
    }


    //---------------------------------------------------------
    pOutTab->Destroy();
    pOutTab->Set_Name(CSG_String::Format("%s_zonal_stats", pZones->Get_Name()));

    _Create_Field(pOutTab, SG_T("UCU")                              , SG_T("")      , SG_DATATYPE_Long  , bShortNames);
    _Create_Field(pOutTab, pZones->Get_Name()                       , SG_T("")      , SG_DATATYPE_Long  , bShortNames);

    for(int i=0; i<pCatList->Get_Grid_Count(); i++)
    {
        _Create_Field(pOutTab, pCatList->Get_Grid(i)->Get_Name()    , SG_T("")      , SG_DATATYPE_Long  , bShortNames);
    }

    _Create_Field(pOutTab, SG_T("Count_UCU")                        , SG_T("")      , SG_DATATYPE_Long  , bShortNames);

    for(int i=0; i<pStatList->Get_Grid_Count(); i++)
    {
        _Create_Field(pOutTab, pStatList->Get_Grid(i)->Get_Name()   , SG_T("N")     , SG_DATATYPE_Long  , bShortNames);
        _Create_Field(pOutTab, pStatList->Get_Grid(i)->Get_Name()   , SG_T("MIN")   , SG_DATATYPE_Double, bShortNames);
        _Create_Field(pOutTab, pStatList->Get_Grid(i)->Get_Name()   , SG_T("MAX")   , SG_DATATYPE_Double, bShortNames);
        _Create_Field(pOutTab, pStatList->Get_Grid(i)->Get_Name()   , SG_T("MEAN")  , SG_DATATYPE_Double, bShortNames);
        _Create_Field(pOutTab, pStatList->Get_Grid(i)->Get_Name()   , SG_T("STD")   , SG_DATATYPE_Double, bShortNames);
        _Create_Field(pOutTab, pStatList->Get_Grid(i)->Get_Name()   , SG_T("SUM")   , SG_DATATYPE_Double, bShortNames);
    }

    if( pAspect != NULL )
    {
        _Create_Field(pOutTab, pAspect->Get_Name()                  , SG_T("N")     , SG_DATATYPE_Long  , bShortNames);
        _Create_Field(pOutTab, pAspect->Get_Name()                  , SG_T("MIN")   , SG_DATATYPE_Double, bShortNames);
        _Create_Field(pOutTab, pAspect->Get_Name()                  , SG_T("MAX")   , SG_DATATYPE_Double, bShortNames);
        _Create_Field(pOutTab, pAspect->Get_Name()                  , SG_T("MEAN")  , SG_DATATYPE_Double, bShortNames);
    }


    //---------------------------------------------------------
    sLong iUCU = 1;

    for(std::map<std::vector<int>, std::vector<STATS> >::iterator it=mapUCUs.begin(); it!=mapUCUs.end(); ++it)
    {
        CSG_Table_Record *pRecord = pOutTab->Add_Record();
        int iField = 0;

        pRecord->Set_Value(iField++     , iUCU);                    // UCU identifier

        for(size_t i=0; i<it->first.size(); i++)
        {
            pRecord->Set_Value(iField++ , it->first.at(i));         // categories making up this UCU
        }

        pRecord->Set_Value(iField++     , it->second.at(0).cells.size());   // count UCU

        for(int i=0; i<pStatList->Get_Grid_Count(); i++)
        {
            pRecord->Set_Value(iField++ , it->second.at(i).n);      // statistics
            pRecord->Set_Value(iField++ , it->second.at(i).min);
            pRecord->Set_Value(iField++ , it->second.at(i).max);
            pRecord->Set_Value(iField++ , it->second.at(i).sum / it->second.at(i).n);
            pRecord->Set_Value(iField++ , sqrt((it->second.at(i).sum_2 - it->second.at(i).n * pow(it->second.at(i).sum / it->second.at(i).n, 2)) / (it->second.at(i).n - 1)));
            pRecord->Set_Value(iField++ , it->second.at(i).sum);
        }

        if( pAspect != NULL )
        {
            pRecord->Set_Value(iField++ , it->second.at(iStats - 1).n);
            pRecord->Set_Value(iField++ , it->second.at(iStats - 1).min * M_RAD_TO_DEG);
            pRecord->Set_Value(iField++ , it->second.at(iStats - 1).max * M_RAD_TO_DEG);

            double dX       = it->second.at(iStats - 1).sum   / it->second.at(iStats - 1).n;
            double dY       = it->second.at(iStats - 1).sum_2 / it->second.at(iStats - 1).n;
            double dMean    = dX ? fmod(M_PI_270 + atan2(dY, dX), M_PI_360) : (dY > 0 ? M_PI_270 : (dY < 0 ? M_PI_090 : -1));
            dMean           = fmod(M_PI_360 - dMean, M_PI_360);

            pRecord->Set_Value(iField++ , dMean * M_RAD_TO_DEG);
        }

        if( pUCU != NULL )
        {
            for(size_t i=0; i<it->second.at(0).cells.size(); i++)
            {
                pUCU->Set_Value(it->second.at(0).cells.at(i), iUCU);
            }
        }

        iUCU++;
    }


    //---------------------------------------------------------
	if( iNoDataCount > 0 )
	{
		Message_Fmt("\n%s: %lld %s", _TL("Warning"), iNoDataCount, _TL("NoData value(s) in statistic grid(s)!"));
	}
    
	return (true);
}


//---------------------------------------------------------
void CGSGrid_Zonal_Statistics::_Set_Stats(std::map<std::vector<int>, std::vector<STATS> > &mapUCUs, std::vector<int> &vCategories, int i, double val, bool bAspect)
{
    mapUCUs[vCategories][i].n   += 1;

    if( !bAspect )
    {
        mapUCUs[vCategories][i].sum    += val;
        mapUCUs[vCategories][i].sum_2  += pow(val, 2);
    }
    else
    {
        mapUCUs[vCategories][i].sum    += sin(val);
        mapUCUs[vCategories][i].sum_2  += cos(val);
    }

    if( mapUCUs[vCategories][i].min > val )
    {
        mapUCUs[vCategories][i].min = val;
    }
    
    if( mapUCUs[vCategories][i].max < val )
    {
        mapUCUs[vCategories][i].max = val;
    }

    return;
}


//---------------------------------------------------------
void CGSGrid_Zonal_Statistics::_Create_Field(CSG_Table *pTable, CSG_String sFieldName, CSG_String sSuffix, TSG_Data_Type Type, bool bShortNames)
{
    if( bShortNames )
    {
        sFieldName = sFieldName.Left(10 - sSuffix.Length());    
    }

    sFieldName += sSuffix;

    pTable->Add_Field(sFieldName, Type);

    return;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
