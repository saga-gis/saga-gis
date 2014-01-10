/**********************************************************
 * Version $Id: GSPoints_Distances.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   statistics_points                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 GSPoints_Distances.cpp                //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "GSPoints_Distances.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSPoints_Distances::CGSPoints_Distances(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Minimum Distance Analysis"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description(
		_TL("")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Minimum Distance Analysis"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SET_VALUE(s, v)	{ pRecord = pTable->Add_Record(); pRecord->Set_Value(0, s); pRecord->Set_Value(1, v); }

//---------------------------------------------------------
bool CGSPoints_Distances::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("POINTS")	->asShapes();
	CSG_Table	*pTable		= Parameters("TABLE")	->asTable();

	//-----------------------------------------------------
	CSG_PRQuadTree			QT(pPoints, 0);
	CSG_Simple_Statistics	s;

	double	x, y, z;

	for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		TSG_Point	p	= pPoints->Get_Shape(iPoint)->Get_Point(0);

		if( QT.Select_Nearest_Points(p.x, p.y, 2) && QT.Get_Selected_Point(1, x, y, z) && (x != p.x || y != p.y) )
		{
			s.Add_Value(SG_Get_Distance(x, y, p.x, p.y));
		}
	}

	//-----------------------------------------------------
	if( s.Get_Count() > 0 )
	{
		CSG_Table_Record	*pRecord;

		pTable->Destroy();
		pTable->Set_Name(CSG_String::Format(SG_T("%s [%s]"), _TL("Minimum Distance Analysis"), pPoints->Get_Name()));

		pTable->Add_Field(SG_T("NAME")	, SG_DATATYPE_String);
		pTable->Add_Field(SG_T("VALUE")	, SG_DATATYPE_Double);

		SET_VALUE(_TL("Mean Average")		, s.Get_Mean());
		SET_VALUE(_TL("Minimum")			, s.Get_Minimum());
		SET_VALUE(_TL("Maximum")			, s.Get_Maximum());
		SET_VALUE(_TL("Standard Deviation")	, s.Get_StdDev());
		SET_VALUE(_TL("Duplicates")			, pPoints->Get_Count() - s.Get_Count());

		DataObject_Update(pTable, SG_UI_DATAOBJECT_SHOW);

		return( true );
	}

	Message_Dlg(_TL("not enough observations"));

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
