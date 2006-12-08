
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description	(_TW(
		"Calculates the hypsometric curve for a given DEM.\n\n"
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
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "SORTING"		, _TL("Sort"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("up"),
			_TL("down")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Classification Constant"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("height"),
			_TL("area")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "BZRANGE"		, _TL("Use Z-Range"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Range(
		NULL	, "ZRANGE"		, _TL("Z-Range"),
		_TL(""),
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
	bool	bDown;
	int		nClasses;
	double	zMin, zMax;
	CSG_Grid	*pDEM;
	CSG_Table	*pTable;

	pDEM		= Parameters("ELEVATION")	->asGrid();
	pTable		= Parameters("TABLE")		->asTable();
	bDown		= Parameters("SORTING")		->asInt() == 1;
	nClasses	= Parameters("COUNT")		->asInt();
	zMin		= Parameters("BZRANGE")		->asBool() ? Parameters("ZRANGE")->asRange()->Get_LoVal() : 0.0;
	zMax		= Parameters("BZRANGE")		->asBool() ? Parameters("ZRANGE")->asRange()->Get_HiVal() : 0.0;
	
	pTable->Destroy();
	pTable->Set_Name(CSG_String::Format(SG_T("%s: %s"), _TL("Hypsometric Curve"), pDEM->Get_Name()));
	pTable->Add_Field(_TL("Relative Height"), TABLE_FIELDTYPE_Double);
	pTable->Add_Field(_TL("Relative Area")	, TABLE_FIELDTYPE_Double);
	pTable->Add_Field(_TL("Absolute Height"), TABLE_FIELDTYPE_Double);
	pTable->Add_Field(_TL("Absolute Area")	, TABLE_FIELDTYPE_Double);

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
	long	n, *Cells_Count;
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
		Cells_Count	= (long *)SG_Calloc(nClasses + 1, sizeof(long));

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

		return( false );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHypsometry::Calculate_B(CSG_Grid *pDEM, CSG_Table *pTable, bool bDown, int nClasses, double zMin, double zMax)
{
	int		x, y, i, n, nStep, nMin, nMax, nRange;
	double	z, zRange;

	//-----------------------------------------------------
	if( zMin < zMax && zMin < pDEM->Get_ZMax() && zMax > pDEM->Get_ZMin() )
	{
		for(nMin=0; nMin<pDEM->Get_NCells() && Set_Progress_NCells(nMin); nMin++)
		{
			pDEM->Get_Sorted(nMin, x, y, !bDown);

			if( zMin <= pDEM->asDouble(x, y) )
			{
				zMin	= pDEM->asDouble(x, y);
				break;
			}
		}

		for(nMax=pDEM->Get_NCells() - 1; nMax > nMin && Set_Progress_NCells(nMax); nMax--)
		{
			pDEM->Get_Sorted(nMax, x, y, !bDown);

			if( zMax >= pDEM->asDouble(x, y) )
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

		pTable->Add_Field(_TL("Relative Height"), TABLE_FIELDTYPE_Double);
		pTable->Add_Field(_TL("Relative Area")	, TABLE_FIELDTYPE_Double);
		pTable->Add_Field(_TL("Absolute Height"), TABLE_FIELDTYPE_Double);
		pTable->Add_Field(_TL("Absolute Area")	, TABLE_FIELDTYPE_Double);

		pTable->Set_Name(CSG_String::Format(SG_T("%s: %s"), _TL("Hypsometric Curve"), pDEM->Get_Name()));

		//-------------------------------------------------
		nStep	= nRange / nClasses;

		for(i=0, n=nMax; i<=nClasses; i++, n-=nStep)
		{
			pDEM->Get_Sorted(n, x, y, !bDown);

			z	= pDEM->asDouble(x, y);

			pRecord	= pTable->Add_Record();
			pRecord->Set_Value(0, 100.0 * i / nClasses);				// Relative Area
			pRecord->Set_Value(1, 100.0 * (z - zMin) / zRange);			// Relative Height
			pRecord->Set_Value(2, z);									// Absolute Height
			pRecord->Set_Value(3, i * nStep * pDEM->Get_Cellarea());	// Absolute Area
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
