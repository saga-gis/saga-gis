
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Grid_Profile_From_Lines.cpp              //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
#include "Grid_Profile_From_Lines.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Profile_From_Lines::CGrid_Profile_From_Lines(void)
{
	Set_Name		(_TL("Profiles from Lines"));

	Set_Author		(_TL("Copyrights (c) 2006 by Olaf Conrad"));

	Set_Description	(_TL(
		"Create profiles from a grid based DEM for each line of a lines layer. "
	));

	Parameters.Add_Grid(
		NULL, "DEM"			, _TL("DEM"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL, "VALUES"		, _TL("Values"),
		_TL("Additional values that shall be saved to the output table."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL, "LINES"		, _TL("Lines"),
		"",
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL, "PROFILE"		, _TL("Profiles"),
		"",
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes_List(
		NULL, "PROFILES"	, _TL("Profiles"),
		"",
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL, "SPLIT"		, _TL("Each Line as new Profile"),
		"",
		PARAMETER_TYPE_Bool, false
	);
}

//---------------------------------------------------------
CGrid_Profile_From_Lines::~CGrid_Profile_From_Lines(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::On_Execute(void)
{
	int			iLine;
	CSG_String	Name;

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM")		->asGrid();
	m_pValues	= Parameters("VALUES")	->asGridList();
	m_pLines	= Parameters("LINES")	->asShapes();

	//-----------------------------------------------------
	if( Parameters("SPLIT")->asBool() == false )
	{
		if( (m_pProfile = Parameters("PROFILE")->asShapes()) == NULL )
		{
			Parameters("PROFILE")->Set_Value(m_pProfile = SG_Create_Shapes(SHAPE_TYPE_Point));
		}

		Name.Printf("%s [%s]", _TL("Profile"), m_pDEM->Get_Name());
		Init_Profile(m_pProfile, Name);

		for(iLine=0; iLine<m_pLines->Get_Count() && Set_Progress(iLine, m_pLines->Get_Count()); iLine++)
		{
			Set_Profile(iLine, m_pLines->Get_Shape(iLine));
		}

		return( true );
	}

	//-----------------------------------------------------
	else
	{
		Parameters("PROFILES")->asShapesList()->Del_Items();

		for(iLine=0; iLine<m_pLines->Get_Count() && Set_Progress(iLine, m_pLines->Get_Count()); iLine++)
		{
			Name.Printf("%s [%d, %s]", _TL("Profile"), iLine, m_pDEM->Get_Name());
			Init_Profile(m_pProfile = SG_Create_Shapes(), Name);

			Set_Profile(iLine, m_pLines->Get_Shape(iLine));

			Parameters("PROFILES")->asShapesList()->Add_Item(m_pProfile);
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	F_LINE_ID	= 0,
	F_ID,
	F_DIST,
	F_DIST_SURF,
	F_X,
	F_Y,
	F_Z,
	F_VALUES
};

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Init_Profile(CSG_Shapes *pPoints, const char *Name)
{
	if( pPoints )
	{
		pPoints->Create(SHAPE_TYPE_Point, Name);

		pPoints->Get_Table().Add_Field("LINE_ID"	, TABLE_FIELDTYPE_Int);
		pPoints->Get_Table().Add_Field("ID"			, TABLE_FIELDTYPE_Int);
		pPoints->Get_Table().Add_Field("DIST"		, TABLE_FIELDTYPE_Double);
		pPoints->Get_Table().Add_Field("DIST_SURF"	, TABLE_FIELDTYPE_Double);
		pPoints->Get_Table().Add_Field("X"			, TABLE_FIELDTYPE_Double);
		pPoints->Get_Table().Add_Field("Y"			, TABLE_FIELDTYPE_Double);
		pPoints->Get_Table().Add_Field("Z"			, TABLE_FIELDTYPE_Double);

		for(int i=0; i<m_pValues->Get_Count(); i++)
		{
			pPoints->Get_Table().Add_Field(m_pValues->asGrid(i)->Get_Name(), TABLE_FIELDTYPE_Double);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Set_Profile(int Line_ID, CSG_Shape *pLine)
{
	if( pLine && pLine->Get_Point_Count(0) > 1 )
	{
		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			TSG_Point	A, B;

			B	= pLine->Get_Point(0, iPart);

			for(int iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				A	= B;
				B	= pLine->Get_Point(iPoint, iPart);

				Set_Profile(Line_ID, iPoint == 1, A, B);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Set_Profile(int Line_ID, bool bStart, const TSG_Point &A, const TSG_Point &B)
{
	double		dx, dy, d, n;
	TSG_Point	p;

	//-----------------------------------------------------
	dx	= fabs(B.x - A.x);
	dy	= fabs(B.y - A.y);

	if( dx > 0.0 || dy > 0.0 )
	{
		if( dx > dy )
		{
			dx	/= Get_Cellsize();
			n	 = dx;
			dy	/= dx;
			dx	 = Get_Cellsize();
		}
		else
		{
			dy	/= Get_Cellsize();
			n	 = dy;
			dx	/= dy;
			dy	 = Get_Cellsize();
		}

		if( B.x < A.x )
		{
			dx	= -dx;
		}

		if( B.y < A.y )
		{
			dy	= -dy;
		}

		//-------------------------------------------------
		for(d=0.0, p.x=A.x, p.y=A.y; d<=n; d++, p.x+=dx, p.y+=dy)
		{
			Add_Point(Line_ID, bStart, p);

			bStart	= false;
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_Profile_From_Lines::Add_Point(int Line_ID, bool bStart, const TSG_Point &Point)
{
	int			x, y, i;
	double		z, Distance, Distance_2;
	CSG_Shape	*pPoint, *pLast;

	if( Get_System()->Get_World_to_Grid(x, y, Point) && m_pDEM->is_InGrid(x, y) )
	{
		z	= m_pDEM->asDouble(x, y);

		if( bStart || m_pProfile->Get_Count() == 0 )
		{
			Distance	= 0.0;
			Distance_2	= 0.0;
		}
		else
		{
			pLast		= m_pProfile->Get_Shape(m_pProfile->Get_Count() - 1);
			Distance	= SG_Get_Distance(Point, pLast->Get_Point(0));

			if( Distance == 0.0 )
			{
				return( false );
			}

			Distance_2	= pLast->Get_Record()->asDouble(F_Z) - z;
			Distance_2	= sqrt(Distance*Distance + Distance_2*Distance_2);

			Distance	+= pLast->Get_Record()->asDouble(F_DIST);
			Distance_2	+= pLast->Get_Record()->asDouble(F_DIST_SURF);
		}

		pPoint	= m_pProfile->Add_Shape();
		pPoint->Add_Point(Point);

		pPoint->Get_Record()->Set_Value(F_LINE_ID	, Line_ID);
		pPoint->Get_Record()->Set_Value(F_ID		, m_pProfile->Get_Count());
		pPoint->Get_Record()->Set_Value(F_DIST		, Distance);
		pPoint->Get_Record()->Set_Value(F_DIST_SURF	, Distance_2);
		pPoint->Get_Record()->Set_Value(F_X			, Point.x);
		pPoint->Get_Record()->Set_Value(F_Y			, Point.y);
		pPoint->Get_Record()->Set_Value(F_Z			, z);

		for(i=0; i<m_pValues->Get_Count(); i++)
		{
			pPoint->Get_Record()->Set_Value(F_VALUES + i, m_pValues->asGrid(i)->asDouble(x, y, true));
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
