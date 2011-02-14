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
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Swath_Profile.cpp                //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include "Grid_Swath_Profile.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define VALUE_OFFSET	10


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Swath_Profile::CGrid_Swath_Profile(void)
{
	Set_Name(_TL("Swath Profile"));

	Set_Author		(SG_T("(c) 2005 by O.Conrad"));

	Set_Description	(_TW(
		"Create interactively swath profiles from a grid based DEM\n"
		"Use left mouse button clicks into a map window to add profile points."
		"A right mouse button click will finish the profile.\n"
		"Generated outputs for the swath profile are arithmetic mean, "
		"minimum, maximum values and the standard deviation.\n")
	);

	Parameters.Add_Grid(
		NULL, "DEM"			, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL, "VALUES"		, _TL("Values"),
		_TL("Additional values that shall be saved to the output table."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL, "POINTS"		, _TL("Profile Points"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, "LINE"		, _TL("Swath Profile"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Value(
		NULL, "WIDTH"		, _TL("Swath Width"),
		_TL("Swath width measured in map units."),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);
}

//---------------------------------------------------------
CGrid_Swath_Profile::~CGrid_Swath_Profile(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Swath_Profile::On_Execute(void)
{
	m_pDEM		= Parameters("DEM")		->asGrid();
	m_pValues	= Parameters("VALUES")	->asGridList();
	m_pPoints	= Parameters("POINTS")	->asShapes();
	m_pLine		= Parameters("LINE")	->asShapes();
	m_Width		= Parameters("WIDTH")	->asDouble() / 2.0;

	m_bAdd		= false;

	DataObject_Update(m_pDEM , SG_UI_DATAOBJECT_SHOW_NEW_MAP);
	DataObject_Update(m_pLine, SG_UI_DATAOBJECT_SHOW_LAST_MAP);

	return( true );
}

//---------------------------------------------------------
bool CGrid_Swath_Profile::On_Execute_Finish(void)
{
	if( m_bAdd )
	{
		Set_Profile();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Swath_Profile::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	switch( Mode )
	{
	default:
		break;

	case MODULE_INTERACTIVE_LDOWN:
		if( !m_bAdd )
		{
			m_bAdd	= true;
			m_pLine->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s [%s]"), _TL("Profile"), m_pDEM->Get_Name()));
			m_pLine->Add_Field("ID"	, SG_DATATYPE_Int);
			m_pLine->Add_Shape()->Set_Value(0, 1);
		}

		m_pLine->Get_Shape(0)->Add_Point(Get_System()->Fit_to_Grid_System(ptWorld));

		DataObject_Update(m_pLine);
		break;

	case MODULE_INTERACTIVE_RDOWN:
		Set_Profile();
		m_bAdd	= false;
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Swath_Profile::Set_Profile(void)
{
	int			i;
	double		d;
	CSG_Point	A, B, P, Left, Right;
	CSG_Shape	*pLine, *pLeft, *pRight;

	//-----------------------------------------------------
	m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), _TL("Profile"), m_pDEM->Get_Name()));

	m_pPoints->Add_Field("ID"				, SG_DATATYPE_Int);
	m_pPoints->Add_Field("D"				, SG_DATATYPE_Double);
	m_pPoints->Add_Field("X"				, SG_DATATYPE_Double);
	m_pPoints->Add_Field("Y"				, SG_DATATYPE_Double);
	m_pPoints->Add_Field("Z"				, SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("Z [mean]")	, SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("Z [min]")		, SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("Z [max]")		, SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("Z [min_sd]")	, SG_DATATYPE_Double);
	m_pPoints->Add_Field(_TL("Z [max_sd]")	, SG_DATATYPE_Double);

	for(i=0; i<m_pValues->Get_Count(); i++)
	{
		m_pPoints->Add_Field(m_pValues->asGrid(i)->Get_Name(), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format(SG_T("%s [%s]"), _TL("mean")	, m_pValues->asGrid(i)->Get_Name()), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format(SG_T("%s [%s]"), _TL("min"	)	, m_pValues->asGrid(i)->Get_Name()), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format(SG_T("%s [%s]"), _TL("max"	)	, m_pValues->asGrid(i)->Get_Name()), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format(SG_T("%s [%s]"), _TL("min_sd")	, m_pValues->asGrid(i)->Get_Name()), SG_DATATYPE_Double);
		m_pPoints->Add_Field(CSG_String::Format(SG_T("%s [%s]"), _TL("max_sd")	, m_pValues->asGrid(i)->Get_Name()), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	if( (pLine = m_pLine->Get_Shape(0)) != NULL && pLine->Get_Point_Count(0) > 1 )
	{
		if( (pLeft	= m_pLine->Get_Shape(1)) == NULL )
			pLeft	= m_pLine->Add_Shape();
		else
			pLeft	->Del_Parts();

		if( (pRight	= m_pLine->Get_Shape(2)) == NULL )
			pRight	= m_pLine->Add_Shape();
		else
			pRight	->Del_Parts();

		B	= pLine->Get_Point(0);

		for(i=1; i<pLine->Get_Point_Count(0); i++)
		{
			A		= B;
			B		= pLine->Get_Point(i);

			P		= B - A;
			d		= m_Width / sqrt(P.Get_X()*P.Get_X() + P.Get_Y()*P.Get_Y());
			P		.Assign(-d * P.Get_Y(), d * P.Get_X());
			Left	= A - P;
			Right	= A + P;

			Set_Profile(A, B, Left, Right);

			pLeft	->Add_Point(Left , i - 1);
			Left	= B - P;
			pLeft	->Add_Point(Left , i - 1);

			pRight	->Add_Point(Right, i - 1);
			Right	= B + P;
			pRight	->Add_Point(Right, i - 1);
		}
	}

	//-----------------------------------------------------
	DataObject_Update(m_pLine);
	DataObject_Update(m_pPoints);

	return( m_pPoints->Get_Count() > 0 );
}

//---------------------------------------------------------
bool CGrid_Swath_Profile::Set_Profile(CSG_Point A, CSG_Point B, CSG_Point Left, CSG_Point Right)
{
	double		dx, dy, d, n;
	CSG_Point	p, dStep, Step;

	//-----------------------------------------------------
	dx	= fabs(B.Get_X() - A.Get_X());
	dy	= fabs(B.Get_Y() - A.Get_Y());

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

		dx		= A.Get_X() < B.Get_X() ? dx : -dx;
		dy		= A.Get_Y() < B.Get_Y() ? dy : -dy;

		dStep	.Assign( dx, dy);
		Step	.Assign(-dy, dx);

		if( fabs(Step.Get_X()) > fabs(Step.Get_Y()) )
		{
			if( Left.Get_X() > Right.Get_X() )
			{
				p	= Left;	Left	= Right;	Right	= p;
			}

			if( Step.Get_X() < 0.0 )
			{
				Step.Assign(-Step.Get_X(), -Step.Get_Y());
			}
		}
		else
		{
			if( Left.Get_Y() > Right.Get_Y() )
			{
				p	= Left;	Left	= Right;	Right	= p;
			}

			if( Step.Get_Y() < 0.0 )
			{
				Step.Assign(-Step.Get_X(), -Step.Get_Y());
			}
		}

		//-------------------------------------------------
		for(d=0.0; d<=n; d++, A+=dStep, Left+=dStep, Right+=dStep)
		{
			Add_Point(A, Left, Right, Step);
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CGrid_Swath_Profile::Add_Point(CSG_Point Point, CSG_Point Left, CSG_Point Right, CSG_Point Step)
{
	int		i, j;
	double	Distance;
	CSG_Shape	*pPoint, *pLast;

	if( m_pDEM->is_InGrid_byPos(Point) )
	{
		if( m_pPoints->Get_Count() == 0 )
		{
			Distance	= 0.0;
		}
		else
		{
			pLast		= m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);
			Distance	= SG_Get_Distance(Point, pLast->Get_Point(0));

			if( Distance == 0.0 )
			{
				return( false );
			}

			Distance	+= pLast->asDouble(1);
		}

		pPoint	= m_pPoints->Add_Shape();
		pPoint->Add_Point(Point);

		pPoint->Set_Value(0, m_pPoints->Get_Count());
		pPoint->Set_Value(1, Distance);
		pPoint->Set_Value(2, Point.Get_X());
		pPoint->Set_Value(3, Point.Get_Y());

		pPoint->Set_Value(4, m_pDEM->Get_Value(Point, GRID_INTERPOLATION_BSpline, true));
		Add_Swath(pPoint, 4, m_pDEM, Left, Right, Step);

		for(i=0, j=VALUE_OFFSET; i<m_pValues->Get_Count(); i++, j+=6)
		{
			pPoint->Set_Value(j, m_pValues->asGrid(i)->Get_Value(Point, GRID_INTERPOLATION_BSpline, true));
			Add_Swath(pPoint, j, m_pValues->asGrid(i), Left, Right, Step);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Swath_Profile::Add_Swath(CSG_Table_Record *pRecord, int iEntry, CSG_Grid *pGrid, CSG_Point Left, CSG_Point Right, CSG_Point Step)
{
	int		n;
	double	z, Sum, Sum2, Min, Max, iRun, dRun, nRun;

	if( Step.Get_X() > Step.Get_Y() )
	{
		iRun	= Left	.Get_X();
		dRun	= Step	.Get_X();
		nRun	= Right	.Get_X();
	}
	else
	{
		iRun	= Left	.Get_Y();
		dRun	= Step	.Get_Y();
		nRun	= Right	.Get_Y();
	}

	for(n=0, Sum=0.0, Sum2=0.0; iRun<=nRun; iRun+=dRun, Left+=Step)
	{
		if( pGrid->Get_Value(Left, z, GRID_INTERPOLATION_BSpline, true) )
		{
			if( n == 0 )
				Min	= Max	= z;
			else if( z < Min )
				Min	= z;
			else if( z > Max )
				Max	= z;

			Sum		+= z;
			Sum2	+= z * z;

			n++;
		}
	}

	if( n > 0 )
	{
		Sum		/= (double)n;							// arithmetic mean
		Sum2	= sqrt(Sum2 / (double)n - Sum * Sum);	// standard deviation

		pRecord->Set_Value(iEntry + 1, Sum);
		pRecord->Set_Value(iEntry + 2, Min);
		pRecord->Set_Value(iEntry + 3, Max);
		pRecord->Set_Value(iEntry + 4, Sum - Sum2);
		pRecord->Set_Value(iEntry + 5, Sum + Sum2);

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
