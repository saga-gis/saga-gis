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
//                 Geostatistics_Points                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  semivariogram.cpp                    //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
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
#include "semivariogram.h"

#include "variogram_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSemiVariogram::CSemiVariogram(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Variogram (Dialog))"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description(
		_TL("")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "VARIOGRAM"	, _TL("Variogram"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "LOG"			, _TL("Logarithmic Transformation"),
		_TL(""),
		PARAMETER_TYPE_Bool
	);

	//-----------------------------------------------------
	if( !SG_UI_Get_Window_Main() )
	{
		Parameters.Add_Value(
			NULL	, "VAR_MAXDIST"		, _TL("Maximum Distance"),
			_TL(""),
			PARAMETER_TYPE_Double	, -1.0
		);

		Parameters.Add_Value(
			NULL	, "VAR_NCLASSES"	, _TL("Lag Distance Classes"),
			_TL("initial number of lag distance classes"),
			PARAMETER_TYPE_Int		, 100, 1, true
		);

		Parameters.Add_Value(
			NULL	, "VAR_NSKIP"		, _TL("Skip"),
			_TL(""),
			PARAMETER_TYPE_Int, 1, 1, true
		);

		Parameters.Add_String(
			NULL	, "VAR_MODEL"		, _TL("Model"),
			_TL(""),
			SG_T("a + b * x")
		);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSemiVariogram::On_Execute(void)
{
	bool		bLog, bResult	= false;
	int			Attribute;
	CSG_Trend	Model;
	CSG_Shapes	*pPoints;
	CSG_Table	*pVariogram;

	//-----------------------------------------------------
	pPoints		= Parameters("POINTS")		->asShapes();
	Attribute	= Parameters("ATTRIBUTE")	->asInt();
	bLog		= Parameters("LOG")			->asBool();
	pVariogram	= Parameters("VARIOGRAM")	->asTable();

	//-----------------------------------------------------
	if( SG_UI_Get_Window_Main() )
	{
		static CVariogram_Dialog	dlg;

		if( dlg.Execute(pPoints, Attribute, bLog, pVariogram, &Model) )
		{
			bResult	= true;
		}
	}

	//-----------------------------------------------------
	else
	{
		int		nSkip		= Parameters("VAR_NSKIP")		->asInt();
		int		nClasses	= Parameters("VAR_NCLASSES")	->asInt();
		double	maxDistance	= Parameters("VAR_MAXDIST")		->asDouble();

		Model.Set_Formula(Parameters("VAR_MODEL")->asString());

		if( CSG_Variogram::Calculate(pPoints, Attribute, bLog, pVariogram, nClasses, maxDistance, nSkip) )
		{
			Model.Clr_Data();

			for(int i=0; i<pVariogram->Get_Count(); i++)
			{
				CSG_Table_Record	*pRecord	= pVariogram->Get_Record(i);

				Model.Add_Data(pRecord->asDouble(CSG_Variogram::FIELD_DISTANCE), pRecord->asDouble(CSG_Variogram::FIELD_VAR_EXP));
			}

			bResult	= Model.Get_Trend() || Model.Get_Parameter_Count() == 0;
		}
	}

	//-----------------------------------------------------
	if( bResult )
	{
		Message_Add(Model.Get_Formula(), false);

		for(int i=0; i<pVariogram->Get_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= pVariogram->Get_Record(i);

			pRecord->Set_Value(CSG_Variogram::FIELD_VAR_MODEL, Model.Get_Value(pRecord->asDouble(CSG_Variogram::FIELD_DISTANCE)));
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
