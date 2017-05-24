
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    Bioclimatology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   thermal_belts.cpp                   //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                  Dirk Nikolaus Karger                 //
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
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     dirk.karger@systbot.uzh.ch             //
//                                                       //
//    contact:    Dirk Nikolaus Karger                   //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "thermal_belts.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CThermal_Belts::CThermal_Belts(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Thermic Belt Classification"));

	Set_Author		("Dirk Nikolaus Karger");

	Set_Description	(_TW(
		"Calculates the thermal belts based on mean temperature and length of the growing season."
	));

	Add_Reference("Körner, C. / Paulsen, J. / Spehn, E.M.", "2011",
		"A definition of mountains and their bioclimatic belts for global comparisons of biodiversity data",
		"Alpine Botany, 121, 73–78. doi:10.1007/s00035-011-0094-4",
		SG_T("http://link.springer.com/article/10.1007/s00035-011-0094-4"), _TL("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL,
		"GSL"	, _TL("Growing Season Length"),
		_TL("Growing season length given as number of days of a year."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"GST"	, _TL("Mean Temperature"),
		_TL("Mean temperature of the growing season."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL,
		"FROST", _TL("Frost occurence"),
		_TL("Frost occurence as binary factor."),
		PARAMETER_INPUT
		);

	Parameters.Add_Value(
		NULL, "NIVAL_TEMP", "Minimum Temperature Nival",
		"Minimum Temperature for nival belt.",
		PARAMETER_TYPE_Double,
		3.5
		);

	Parameters.Add_Value(
		NULL, "TREE_TEMP", "Minimum Temperature Treeline",
		"Minimum Temperature for treeline.",
		PARAMETER_TYPE_Double,
		6.4
		);

	Parameters.Add_Grid(NULL,
		"ATB"	, _TL("Thermal Belts"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CThermal_Belts::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pL		= Parameters("GSL")->asGrid();
	CSG_Grid	*pT		= Parameters("GST")->asGrid();
	CSG_Grid	*pF		= Parameters("FROST")->asGrid();

	CSG_Grid	*pBelt	= Parameters("ATB")->asGrid();

	double TH,NT;
	NT = Parameters("NIVAL_TEMP")->asDouble();
	TH = Parameters("TREE_TEMP")->asDouble();
	//-----------------------------------------------------
	pBelt->Set_NoData_Value(0.0);

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pBelt, "LUT");	// get GUI parameter with id 'LUT' (look-up table)

	if( pLUT && pLUT->asTable() )	// parameter and associated look-up table exist only in GUI!!!
	{
		pLUT->asTable()->Del_Records();

		#define LUT_ADD_CLASS(id, r, g, b, name)	{ CSG_Table_Record *pClass = pLUT->asTable()->Add_Record();\
			pClass->Set_Value(0, SG_GET_RGB(r, g, b));\
			pClass->Set_Value(1, name);\
			pClass->Set_Value(3, id);\
		}

		LUT_ADD_CLASS(1, 202, 202, 255, _TL("Nival"        ));
		LUT_ADD_CLASS(2, 104, 180, 255, _TL("Upper Alpine" ));
		LUT_ADD_CLASS(3, 128,   0, 255, _TL("Lower Alpine" ));
		LUT_ADD_CLASS(4,   0, 255, 128, _TL("Upper Montane"));
		LUT_ADD_CLASS(5,   0, 128,  64, _TL("Lower Montane"));
		LUT_ADD_CLASS(6, 225, 225,   0, _TL("Freezing"));
		LUT_ADD_CLASS(7, 225, 102,   0, _TL("No Freezing"));

		LUT_ADD_CLASS(8, 225, 225, 225, _TL("other"));
		
		DataObject_Set_Parameter(pBelt, pLUT);
		DataObject_Set_Parameter(pBelt, "COLORS_TYPE", 1);	// set color classification type to 'Look-up Table'
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int	Belt	= 0;	// no data

			if( !pL->is_NoData(x, y) )
			{
				Belt	= 8;	// no growing season days

				if( !pT->is_NoData(x, y) )
				{
					double	T	= pT->asDouble(x, y);
					double	F   = pF->asInt(x, y);

					if     ( T <   NT )
					{
						Belt	= pL->asInt(x, y) <= 10 ? 1 : 2;
					}
					else if (T <   TH)
					{
						Belt	= 3;
					}
					else if( T <= 10.0 )
					{
						Belt	= 4;
					}
					else if( T <  15.0 )
					{
						Belt	= 5;
					}
					else if (F == 1)
					{
						Belt	= 6;
					}
					else if (F == 0)
					{
						Belt	= 7;
					}

				}
			}

			pBelt->Set_Value(x, y, Belt);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
