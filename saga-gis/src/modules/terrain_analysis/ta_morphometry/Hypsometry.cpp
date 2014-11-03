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
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Hypsometry.cpp                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Hypsometry.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHypsometry::CHypsometry(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Hypsometry"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"Calculates the hypsometric curve for a given DEM.\n\n"
		"The hypsometric curve is an empirical cumulative distribution function of "
		"elevations in a catchment or of a whole planet. The module calculates both "
		"the relative (scaled from 0 to 100 percent) and absolute (minimum "
		"to maximum values) distributions. The former scales elevation and area "
		"by the maximum values. Such a non-dimensional curve allows one to asses the "
		"similarity of watersheds as differences in hypsometric curves arise from "
		"different geomorphic processes shaping a landscape.\n\n"
		"In case the hypsometric curve should not be calculated for the whole "
		"elevation range of the input dataset, a user-specified elevation range "
		"can be specified with the classification constant area.\n\n"
		"The output table has two attribute columns with relative height and area "
		"values, and two columns with absolute height and area values. In order to "
		"plot the non-dimensional hypsometric curve as diagram, use the relative "
		"area as x-axis values and the relative height for the y-axis. For a "
		"diagram with absolute values, use the absolute area as x-axis values "
		"and the absolute height for the y-axis.\n\n"
		"References:\n"
		"- Harlin, J.M (1978):\n"
		"    'Statistical moments of the hypsometric curve and its density function',\n"
		"    J. Int. Assoc. Math. Geol., Vol.10, p.59-72\n\n"
		"- Luo, W. (2000):\n"
		"    'Quantifying groundwater-sapping landforms with a hypsometric technique',\n"
		"    J. of Geophysical Research, Vol.105, No.E1, p.1685-1694\n\n")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Hypsometry"),
		_TL(""),
		PARAMETER_OUTPUT
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL	, "COUNT"		, _TL("Number of Classes"),
		_TL("Number of discrete intervals (bins) used for sampling"),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "SORTING"		, _TL("Sort"),
		_TL("Choose how to sort the elevation dataset before sampling"),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("up"),
			_TL("down")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Classification Constant"),
		_TL("Choose the classification constant to use"),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("height"),
			_TL("area")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "BZRANGE"		, _TL("Use Z-Range"),
		_TL("Use a user-specified elevation range instead of min/max of the input dataset"),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Range(
		NULL	, "ZRANGE"		, _TL("Z-Range"),
		_TL("User specified elevation range"),
		0.0, 1000.0
	);
}

//---------------------------------------------------------
CHypsometry::~CHypsometry(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHypsometry::On_Execute(void)
{
	bool		bDown;
	int			nClasses;
	double		zMin, zMax;
	CSG_Grid	*pDEM;
	CSG_Table	*pTable;

	pDEM		= Parameters("ELEVATION")->asGrid();
	pTable		= Parameters("TABLE"    )->asTable();
	bDown		= Parameters("SORTING"  )->asInt() == 1;
	nClasses	= Parameters("COUNT"    )->asInt();
	zMin		= Parameters("BZRANGE"  )->asBool() ? Parameters("ZRANGE")->asRange()->Get_LoVal() : 0.0;
	zMax		= Parameters("BZRANGE"  )->asBool() ? Parameters("ZRANGE")->asRange()->Get_HiVal() : 0.0;
	
	if( !bDown && Parameters("BZRANGE")->asBool() && Parameters("METHOD")->asInt() == 1 )
	{
		SG_UI_Msg_Add_Error(_TW(
			"The selected module parameter configuration (classification constant area, "
			"upward sorting and use of an user-specified elevation range) is not supported."
		));

		return( false );
	}

	if( !pDEM->Set_Index() )
	{
		Error_Set(_TL("index creation failed"));

		return( false );
	}

	pTable->Destroy();
	pTable->Set_Name(CSG_String::Format(SG_T("%s: %s"), _TL("Hypsometric Curve"), pDEM->Get_Name()));
	pTable->Add_Field(_TL("Relative Height"), SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Relative Area")	, SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Absolute Height"), SG_DATATYPE_Double);
	pTable->Add_Field(_TL("Absolute Area")	, SG_DATATYPE_Double);

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:				return( Calculate_A(pDEM, pTable, bDown, nClasses) );
	case 1:	default:	return( Calculate_B(pDEM, pTable, bDown, nClasses, zMin, zMax) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHypsometry::Calculate_A(CSG_Grid *pDEM, CSG_Table *pTable, bool bDown, int nClasses)
{
	int		i;
	sLong	n, *Cells_Count;
	double	z, dz, A, a, Min, Max, za, zb;

	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	// 1. Min, Max, Area...

	A	= 0;

	for(n=0; n<pDEM->Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( !pDEM->is_NoData(n) )
		{
			if( A <= 0 )
			{
				Min	= Max	= pDEM->asDouble(n);
			}
			else
			{
				z	= pDEM->asDouble(n);

				if( Min > z )
				{
					Min	= z;
				}
				else if( Max < z )
				{
					Max	= z;
				}
			}

			A++;
		}
	}


	//-----------------------------------------------------
	// 2. Hypsometric Curve...

	if( A > 0 && Min < Max )
	{
		Cells_Count	= (sLong *)SG_Calloc(nClasses + 1, sizeof(sLong));

		for(n=0; n<pDEM->Get_NCells() && Set_Progress_NCells(n); n++)
		{
			if( !pDEM->is_NoData(n) )
			{
				i	= (int)(nClasses * (Max - pDEM->asDouble(n)) / (Max - Min));

				Cells_Count[i]++;
			}
		}

		dz	= (Max - Min) / nClasses;
		a	= A;

		for(i=nClasses; i>=0; i--)
		{
			za	= a / A;
			a	-= Cells_Count[bDown ? i : nClasses - i];
			zb	= a / A;

			pRecord	= pTable->Add_Record();
			pRecord->Set_Value(0, 100.0 * i * dz / (Max - Min));	// Relative Height
			pRecord->Set_Value(1, 100.0 * za);						// Relative Area
			pRecord->Set_Value(2, Min +   i * dz);					// Absolute Height
			pRecord->Set_Value(3, a * pDEM->Get_Cellarea());		// Absolute Area
		}

		SG_Free(Cells_Count);

		return( true );
	}

	SG_UI_Msg_Add_Error(_TL("Total area is zero or minimum elevation is equal or lower than maximum elevation!"));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHypsometry::Calculate_B(CSG_Grid *pDEM, CSG_Table *pTable, bool bDown, int nClasses, double zMin, double zMax)
{
	int		x, y, i;
	sLong	n, nMin, nMax, nStep, nRange;
	double	z, zRange;

	//-----------------------------------------------------
	if( zMin < zMax && zMin < pDEM->Get_ZMax() && zMax > pDEM->Get_ZMin() )
	{
		for(nMin=0; nMin<pDEM->Get_NCells() && Set_Progress_NCells(nMin); nMin++)
		{
			if( pDEM->Get_Sorted(nMin, x, y, !bDown) && zMin <= pDEM->asDouble(x, y) )
			{
				zMin	= pDEM->asDouble(x, y);
				break;
			}
		}

		for(nMax=pDEM->Get_NCells() - 1; nMax > nMin && Set_Progress_NCells(nMax); nMax--)
		{
			if( pDEM->Get_Sorted(nMax, x, y, !bDown) && zMax >= pDEM->asDouble(x, y) )
			{
				zMax	= pDEM->asDouble(x, y);
				break;
			}
		}
	}
	else
	{
		zMin	= pDEM->Get_ZMin();
		zMax	= pDEM->Get_ZMax();
		nMin	= 0;
		nMax	= pDEM->Get_NCells() - 1;
	}

	//-----------------------------------------------------
	if( (nRange = nMax - nMin) > 0 && (zRange = zMax - zMin) > 0 )
	{
		CSG_Table_Record	*pRecord;

		pTable->Destroy();

		pTable->Add_Field(_TL("Relative Height"), SG_DATATYPE_Double);
		pTable->Add_Field(_TL("Relative Area")	, SG_DATATYPE_Double);
		pTable->Add_Field(_TL("Absolute Height"), SG_DATATYPE_Double);
		pTable->Add_Field(_TL("Absolute Area")	, SG_DATATYPE_Double);

		pTable->Set_Name(CSG_String::Format(SG_T("%s: %s"), _TL("Hypsometric Curve"), pDEM->Get_Name()));

		//-------------------------------------------------
		nStep	= nRange / nClasses;

		for(i=0, n=nMax; i<=nClasses; i++, n-=nStep)
		{
			if( pDEM->Get_Sorted(n, x, y, !bDown) )
			{
				z	= pDEM->asDouble(x, y);

				pRecord	= pTable->Add_Record();
				pRecord->Set_Value(0, 100.0 * i / nClasses);				// Relative Area
				pRecord->Set_Value(1, 100.0 * (z - zMin) / zRange);			// Relative Height
				pRecord->Set_Value(2, z);									// Absolute Height
				pRecord->Set_Value(3, i * nStep * pDEM->Get_Cellarea());	// Absolute Area
			}
		}

		return( true );
	}

	SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Elevation range (zMax (%.2f) - zMin (%.2f)) is equal or lower than zero!"), zMax, zMin));

	return( false );
}


//---------------------------------------------------------
int CHypsometry::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METHOD")) )
	{
		pParameters->Get_Parameter("BZRANGE")->Set_Enabled(pParameter->asInt() == 1);
		pParameters->Get_Parameter("ZRANGE")->Set_Enabled(pParameter->asInt() == 1);
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("BZRANGE")) )
	{
		pParameters->Get_Parameter("ZRANGE")->Set_Enabled(pParameter->asBool());
	}
	
	//-----------------------------------------------------
	return (1);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
