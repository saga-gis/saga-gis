
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Tool Library                       //
//                Geostatistics_Kriging                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 semivariogram.cpp                     //
//                                                       //
//                Copyright (C) 2009 by                  //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Variogram (Dialog)"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"ATTRIBUTE"		, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Table("",
		"VARIOGRAM"		, _TL("Variogram"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("",
		"LOG"			, _TL("Logarithmic Transformation"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"VAR_MAXDIST"	, _TL("Maximum Distance"),
		_TL(""),
		-1.0
	)->Set_UseInGUI(false);

	Parameters.Add_Int("",
		"VAR_NCLASSES"	, _TL("Lag Distance Classes"),
		_TL("initial number of lag distance classes"),
		100, 1, true
	)->Set_UseInGUI(false);

	Parameters.Add_Int("",
		"VAR_NSKIP"		, _TL("Skip"),
		_TL(""),
		1, 1, true
	)->Set_UseInGUI(false);

	Parameters.Add_String("",
		"VAR_MODEL"		, _TL("Model"),
		_TL(""),
		"a + b * x"
	)->Set_UseInGUI(false);

	//-----------------------------------------------------
	m_pVariogram	= SG_UI_Get_Window_Main() ? new CVariogram_Dialog : NULL;
}

//---------------------------------------------------------
CSemiVariogram::~CSemiVariogram(void)
{
	if( m_pVariogram && SG_UI_Get_Window_Main() )	// don't destroy dialog, if gui is closing (i.e. main window == NULL)
	{
		m_pVariogram->Destroy();

		delete(m_pVariogram);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSemiVariogram::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pVariogram	= Parameters("VARIOGRAM")->asTable();

	CSG_Trend	Model;

	bool	bResult	= false;

	//-----------------------------------------------------
	if( m_pVariogram )
	{
		if( m_pVariogram->Execute(
			Parameters("POINTS"      )->asShapes(),
			Parameters("ATTRIBUTE"   )->asInt   (),
			Parameters("LOG"         )->asBool  (),
			pVariogram, &Model) )
		{
			bResult	= true;
		}
	}

	//-----------------------------------------------------
	else
	{
		Model.Set_Formula(Parameters("VAR_MODEL")->asString());

		if( CSG_Variogram::Calculate(
			Parameters("POINTS"      )->asShapes(),
			Parameters("ATTRIBUTE"   )->asInt   (),
			Parameters("LOG"         )->asBool  (), pVariogram,
			Parameters("VAR_NCLASSES")->asInt   (),
			Parameters("VAR_MAXDIST" )->asDouble(),
			Parameters("VAR_NSKIP"   )->asInt   ()) )
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
