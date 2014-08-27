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
//                   Cost Analysis                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              LeastCostPathProfile.cpp                 //
//                                                       //
//                 Copyright (C) 2004 by                 //
//               Olaf Conrad & Victor Olaya              //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "LeastCostPathProfile.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define VALUE_OFFSET	5


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLeastCostPathProfile::CLeastCostPathProfile(void)
{
	Parameters.Set_Name(_TL("Least Cost Path"));
	Parameters.Set_Description(_TW(
		"Creates a least cost past profile using an accumulated cost surface."
		"\n(c) 2004 Victor Olaya, Goettingen.\nemail: oconrad@gwdg.de\n")
	);

	Parameters.Add_Grid(
		NULL, "DEM"		, 
		_TL("Accumulated cost"),
		_TL("Accumulated cost"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL, 
		"VALUES", 
		_TL("Values"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL, 
		"POINTS", 
		_TL("Profile (points)"),
		_TL(""),
		PARAMETER_OUTPUT, 
		SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, 
		"LINE", 
		_TL("Profile (lines)"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);
}

//---------------------------------------------------------
CLeastCostPathProfile::~CLeastCostPathProfile(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLeastCostPathProfile::On_Execute(void)
{
	m_pDEM		= Parameters("DEM")		->asGrid();
	m_pValues	= Parameters("VALUES")	->asGridList();
	m_pPoints	= Parameters("POINTS")	->asShapes();
	m_pLine		= Parameters("LINE")	->asShapes();

	DataObject_Update(m_pDEM, true);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLeastCostPathProfile::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	switch( Mode )
	{
	case MODULE_INTERACTIVE_LDOWN:
		Set_Profile(Get_System()->Fit_to_Grid_System(ptWorld));
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
bool CLeastCostPathProfile::Set_Profile(TSG_Point ptWorld)
{
	int			x, y, i;

	//-----------------------------------------------------
	if( Get_System()->Get_World_to_Grid(x, y, ptWorld) && m_pDEM->is_InGrid(x, y) )
	{
		m_pPoints->Create(SHAPE_TYPE_Point, CSG_String::Format(_TL("Profile [%s]"), m_pDEM->Get_Name()));

		m_pPoints->Add_Field("ID"	, SG_DATATYPE_Int);
		m_pPoints->Add_Field("D"	, SG_DATATYPE_Double);
		m_pPoints->Add_Field("X"	, SG_DATATYPE_Double);
		m_pPoints->Add_Field("Y"	, SG_DATATYPE_Double);
		m_pPoints->Add_Field("Z"	, SG_DATATYPE_Double);

		for(i=0; i<m_pValues->Get_Count(); i++)
		{
			m_pPoints->Add_Field(m_pValues->asGrid(i)->Get_Name(), SG_DATATYPE_Double);
		}

		//-----------------------------------------------------
		m_pLine->Create(SHAPE_TYPE_Line, CSG_String::Format(_TL("Profile [%s]"), m_pDEM->Get_Name()));
		m_pLine->Add_Field("ID", SG_DATATYPE_Int);
		m_pLine->Add_Shape()->Set_Value(0, 1);

		//-----------------------------------------------------
		Set_Profile(x, y);

		//-----------------------------------------------------
		DataObject_Update(m_pLine	, false);
		DataObject_Update(m_pPoints	, false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CLeastCostPathProfile::Set_Profile(int iX, int iY)
{
	int iNextX, iNextY;

	iNextX = iX;
	iNextY = iY;
	do {
		iX = iNextX;
		iY = iNextY;
		getNextCell(m_pDEM, iX, iY, iNextX, iNextY);
	
	}while (Add_Point(iX, iY) 
			&& (iX != iNextX || iY != iNextY));
	
}


void CLeastCostPathProfile::getNextCell(CSG_Grid *g,
										int iX,
										int iY,
										int &iNextX,
										int &iNextY) {

    float fMaxSlope;
    float fSlope;

    fMaxSlope = 0;
    fSlope = 0;

    if (iX < 1 || iX >= g->Get_NX()-1 || iY < 1 || iY >= g->Get_NY()-1
            || g->is_NoData(iX,iY)) {
        iNextX = iX;
		iNextY = iY;
		return;
    }// if

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {                	
            if (!g->is_NoData(iX+i,iY+j)){
                fSlope = (g->asFloat(iX+i,iY+j)
                         - g->asFloat(iX,iY));                                				
                if (fSlope <= fMaxSlope) {
                    iNextX = iX+i;
					iNextY = iY+j;                        
                    fMaxSlope = fSlope;
                }// if
            }//if                    
        }// for
    }// for

}// method

//---------------------------------------------------------
bool CLeastCostPathProfile::Add_Point(int x, int y)
{
	int			i;
	double		Distance;
	TSG_Point	Point;
	CSG_Shape		*pPoint, *pLast;

	if( m_pDEM->is_InGrid(x, y) )
	{
		Point	= Get_System()->Get_Grid_to_World(x, y);

		if( m_pPoints->Get_Count() == 0 )
		{
			Distance	= 0.0;
		}
		else
		{
			pLast		= m_pPoints->Get_Shape(m_pPoints->Get_Count() - 1);
			Distance	= SG_Get_Distance(Point, pLast->Get_Point(0));
			Distance	+= pLast->asDouble(1);
		}

		pPoint	= m_pPoints->Add_Shape();
		pPoint->Add_Point(Point);

		pPoint->Set_Value(0, m_pPoints->Get_Count());
		pPoint->Set_Value(1, Distance);
		pPoint->Set_Value(2, Point.x);
		pPoint->Set_Value(3, Point.y);
		pPoint->Set_Value(4, m_pDEM->asDouble(x, y));

		for(i=0; i<m_pValues->Get_Count(); i++)
		{
			pPoint->Set_Value(VALUE_OFFSET + i, m_pValues->asGrid(i)->asDouble(x, y));
		}

		m_pLine->Get_Shape(0)->Add_Point(Point);

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