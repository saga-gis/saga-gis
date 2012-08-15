/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_Aspect_Slope_Map.cpp               //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                    Volker Wichmann                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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
#include "Grid_Aspect_Slope_Map.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Aspect_Slope_Map::CGrid_Aspect_Slope_Map(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Aspect-Slope Grid"));

	Set_Author(_TL("Copyrights (c) 2012 by Volker Wichmann"));

	Set_Description	(_TW(
		"This module creates an aspect-slope map which shows both the aspect "
		"and the slope of the terrain. Aspect is symbolized by different hues, "
		"while slope is mapped with saturation.\n\n"
		"References:\n"
		"Brewer, C.A. & Marlow, K.A. (1993): Color Representation of Aspect and "
		"Slope simultaneously. Proceedings, Eleventh International Symposium on "
		"Computer-Assisted Cartography (Auto-Carto-11), Minneapolis, "
		"October/November 1993, pp. 328-337.\n" 
		"<a href=\"http://www.personal.psu.edu/cab38/Terrain/AutoCarto.html\">http://www.personal.psu.edu/cab38/Terrain/AutoCarto.html</a>\n"
		"\n\n"
		"<a href=\"http://blogs.esri.com/esri/arcgis/2008/05/23/aspect-slope-map/\">http://blogs.esri.com/esri/arcgis/2008/05/23/aspect-slope-map/</a>\n")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "ASPECT"		,_TL("Aspect"),
		_TL("Aspect grid, in radians and 360 degree from north."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SLOPE"		,_TL("Slope"),
		_TL("Slope grid, in radians."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "ASPECT_SLOPE", _TL("Aspect-Slope"),
		_TL("Final aspect-slope grid."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		NULL	, "LUT"		, _TL("Lookup Table"),
		_TL("Lookup table."),
		PARAMETER_OUTPUT_OPTIONAL
	);
}

//---------------------------------------------------------
CGrid_Aspect_Slope_Map::~CGrid_Aspect_Slope_Map(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
long            LUT_COLOR[25]    =
    {
        SG_GET_RGB(153, 153, 153),
        SG_GET_RGB(147, 166,  89),
        SG_GET_RGB(102, 153, 102),
        SG_GET_RGB(102, 153, 136),
        SG_GET_RGB( 89,  89, 166),
        SG_GET_RGB(128, 108, 147),
        SG_GET_RGB(166,  89,  89),
        SG_GET_RGB(166, 134,  89),
        SG_GET_RGB(166, 166,  89),
        SG_GET_RGB(172, 217,  38),
        SG_GET_RGB( 77, 179,  77),
        SG_GET_RGB( 73, 182, 146),
        SG_GET_RGB( 51,  51, 204),
        SG_GET_RGB(128,  89, 166),
		SG_GET_RGB(217,  38,  38),
		SG_GET_RGB(217, 142,  38),
		SG_GET_RGB(217, 217,  38),
		SG_GET_RGB(191, 255,   0),
		SG_GET_RGB( 51, 204,  51),
		SG_GET_RGB( 51, 204, 153),
		SG_GET_RGB( 26,  26, 230),
		SG_GET_RGB(128,  51, 204),
		SG_GET_RGB(255,   0,   0),
		SG_GET_RGB(255, 149,   0),
		SG_GET_RGB(255, 255,   0)
    };

//---------------------------------------------------------
CSG_String		LUT_NAME[25]    =
    {
        SG_T("near-flat slopes"),
        SG_T("N, low slopes"),
        SG_T("NE, low slopes"),
        SG_T("E, low slopes"),
        SG_T("SE, low slopes"),
        SG_T("S, low slopes"),
        SG_T("SW, low slopes"),
        SG_T("W, low slopes"),
        SG_T("NW, low slopes"),
        SG_T("N, moderate slopes"),
        SG_T("NE, moderate slopes"),
        SG_T("E, moderate slopes"),
        SG_T("SE, moderate slopes"),
        SG_T("S, moderate slopes"),
		SG_T("SW, moderate slopes"),
		SG_T("W, moderate slopes"),
		SG_T("NW, moderate slopes"),
		SG_T("N, steep slopes"),
		SG_T("NE, steep slopes"),
		SG_T("E, steep slopes"),
		SG_T("SE, steep slopes"),
		SG_T("S, steep slopes"),
		SG_T("SW, steep slopes"),
		SG_T("W, steep slopes"),
		SG_T("NW, steep slopes")
    };

//---------------------------------------------------------
int				LUT_BREAK[26]		=
	{11, 20,
	 22, 23, 24, 25, 26, 27, 28, 29,
	 32, 33, 34, 35, 36, 37, 38, 39,
	 42, 43, 44, 45, 46, 47, 48, 49
	}; 


//---------------------------------------------------------
bool CGrid_Aspect_Slope_Map::On_Execute(void)
{
	CSG_Grid	*pAspect, *pSlope, *pAspectSlope;
	CSG_Table	*pLUT;

	int						iAspectCount	= 9;
	static const double		AspectBreaks[]	= {0.000, 0.393, 1.178, 1.963, 2.749, 3.534, 4.320, 5.105, 5.890, 6.283};
	static const int		AspectClass[]	= {1, 2, 3, 4, 5, 6, 7, 8, 1};

	int						iSlopeCount		= 4;
	static const double		SlopeBreaks[]	= {0.000, 0.087, 0.349, 0.698, 1.571};
	static const int		SlopeClass[]	= {10, 20, 30, 40};


	pAspect			= Parameters("ASPECT")->asGrid();
	pSlope			= Parameters("SLOPE")->asGrid();
	pAspectSlope	= Parameters("ASPECT_SLOPE")->asGrid();
	pLUT			= Parameters("LUT")->asTable();


	//-----------------------------------------------------
	if( pLUT == NULL )
		pLUT = new CSG_Table();
	else
		pLUT->Destroy();

	pLUT->Set_Name(SG_T("LUT_Aspect-Slope"));

	pLUT->Add_Field(SG_T("COLOR"),			SG_DATATYPE_Int);
	pLUT->Add_Field(SG_T("NAME"),			SG_DATATYPE_String);
	pLUT->Add_Field(SG_T("DESCRIPTION"),	SG_DATATYPE_String);
	pLUT->Add_Field(SG_T("MINIMUM"),		SG_DATATYPE_Int);
	pLUT->Add_Field(SG_T("MAXIMUM"),		SG_DATATYPE_Int);

	for(int i=0; i<25; i++)
	{
		CSG_Table_Record	*pRecord = pLUT->Add_Record();

		pRecord->Set_Value(0, LUT_COLOR[i]);
		pRecord->Set_Value(1, LUT_NAME[i]);
		pRecord->Set_Value(2, SG_T(""));
		pRecord->Set_Value(3, LUT_BREAK[i]);
		pRecord->Set_Value(4, LUT_BREAK[i+1]);
	}


	//-----------------------------------------------------
	#pragma omp parallel for
	for(long n=0; n<Get_NCells(); n++)
	{
		int		iAspectClass, iSlopeClass;

		if( pAspect->is_NoData(n) && pSlope->is_NoData(n) )
		{
			pAspectSlope->Set_NoData(n);
		}
		else
		{
			iAspectClass	= Get_Class(pAspect->asDouble(n), iAspectCount, AspectBreaks, AspectClass);
			iSlopeClass		= Get_Class(pSlope->asDouble(n), iSlopeCount, SlopeBreaks, SlopeClass);

			pAspectSlope->Set_Value(n, iAspectClass + iSlopeClass);
		}
	}


	//-----------------------------------------------------
	CSG_Parameters	Parms;

	if( DataObject_Get_Parameters(pAspectSlope, Parms) && Parms("COLORS_TYPE") && Parms("LUT") )
	{
		Parms("LUT")->asTable()->Assign(pLUT);
		Parms("COLORS_TYPE")->Set_Value(1);

		DataObject_Set_Parameters(pAspectSlope, Parms);
	}

	if( Parameters("LUT")->asTable() == NULL )
	{
		delete pLUT;
	}


	//-----------------------------------------------------
	return( true );
}


//---------------------------------------------------------
int CGrid_Aspect_Slope_Map::Get_Class(double dValue, int iCount, const double *pBreaks, const int *pClass)
{
	for(int i=0; i<iCount; i++)
	{
		if( dValue >= pBreaks[i] && dValue < pBreaks[i+1] )
			return( pClass[i] );
	}

	return( -1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
