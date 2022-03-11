///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Grid_ProfileFromPoints.cpp               //
//                                                       //
//              Copyright (C) 2004 - 2022 by             //
//            Victor Olaya and Volker Wichmann           //
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
#include "Grid_ProfileFromPoints.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CProfileFromPoints::CProfileFromPoints(void)
{
	Set_Name        (_TL("Profile from Points"));

    Set_Author      (SG_T("V.Olaya, V. Wichmann (c) 2004-2022"));

	Set_Description (_TW("The tool allows one to query a profile from an input grid "
                         "(usually a DEM) for point coordinates stored in a table "
                         "or shapefile. The profile is traced from one point to "
                         "the next, sampling the grid values along each line segment. "
                         "Optionally, additional grids can be queried "
                         "whose values are added to the profile table.\n\n"));

    Parameters.Add_Grid("", 
        "GRID"  , _TL("Grid"), 						
		_TL("The input grid to query."), 
		PARAMETER_INPUT
    );

    Parameters.Add_Grid_List("",
        "VALUES"    , _TL("Values"),
        _TL("Additional grids whose values shall be saved to the output table."),
        PARAMETER_INPUT_OPTIONAL
    );

	Parameters.Add_Table("",
        "TABLE" ,   _TL("Input Table"),
        _TL("The input table with the point coordinates to query."),	
        PARAMETER_INPUT
    );
	
	Parameters.Add_Table_Field("TABLE",
        "X" , _TL("X"),
		_TL("The attribute field with the x-coordinate.")
    );

    Parameters.Add_Table_Field("TABLE",
        "Y" , _TL("Y"),
        _TL("The attribute field with the y-coordinate.")
    );

	Parameters.Add_Table("", 
        "RESULT"    , _TL("Result"), 
        _TL("The output table with the queried profile values."), 
		PARAMETER_OUTPUT
    );
}


//---------------------------------------------------------
CProfileFromPoints::~CProfileFromPoints(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CProfileFromPoints::On_Execute(void)
{
    CSG_Table   *pTable;
    int         iXField, iYField;

	m_pGrid     = Parameters("GRID")->asGrid();
    m_pValues   = Parameters("VALUES")->asGridList();
	pTable      = Parameters("TABLE")->asTable();
	m_pProfile  = Parameters("RESULT")->asTable();
	iXField     = Parameters("X")->asInt();
	iYField     = Parameters("Y")->asInt();	
	

    //---------------------------------------------------------
    m_pProfile->Destroy();
    m_pProfile->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pGrid->Get_Name(), _TL("Profile")));

    m_pProfile->Add_Field("ID"       , SG_DATATYPE_Int);
    m_pProfile->Add_Field("DIST"     , SG_DATATYPE_Double);
    m_pProfile->Add_Field("DIST_SURF", SG_DATATYPE_Double);
    m_pProfile->Add_Field("X"        , SG_DATATYPE_Double);
    m_pProfile->Add_Field("Y"        , SG_DATATYPE_Double);
    m_pProfile->Add_Field("Z"        , SG_DATATYPE_Double);

    for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
    {
        m_pProfile->Add_Field(m_pValues->Get_Grid(i)->Get_Name(), SG_DATATYPE_Double);
    }


    //---------------------------------------------------------
	for(int i=1; i<pTable->Get_Record_Count(); i++)
    {
        TSG_Point   A, B;

        A.x = pTable->Get_Record(i - 1)->asDouble(iXField);
        A.y = pTable->Get_Record(i - 1)->asDouble(iYField);
        B.x = pTable->Get_Record(i    )->asDouble(iXField);
        B.y = pTable->Get_Record(i    )->asDouble(iYField);

        Set_Profile(A, B, i == (pTable->Get_Record_Count() - 1));
	}


    //---------------------------------------------------------
	return( true );
}


//---------------------------------------------------------
bool CProfileFromPoints::Set_Profile(const TSG_Point &A, const TSG_Point &B, bool bLastPoint)
{
    double      dx, dy, d, n;
    TSG_Point   p;

    //-----------------------------------------------------
    dx  = fabs(B.x - A.x);
    dy  = fabs(B.y - A.y);

    if( dx > 0.0 || dy > 0.0 )
    {
        if( dx > dy )
        {
            dx  /= Get_Cellsize();
            n    = dx;
            dy  /= dx;
            dx   = Get_Cellsize();
        }
        else
        {
            dy  /= Get_Cellsize();
            n    = dy;
            dx  /= dy;
            dy   = Get_Cellsize();
        }

        if( B.x < A.x )
        {
            dx  = -dx;
        }

        if( B.y < A.y )
        {
            dy  = -dy;
        }

        //-------------------------------------------------
        for(d=0.0, p.x=A.x, p.y=A.y; d<=n; d++, p.x+=dx, p.y+=dy)
        {
            Add_Point(p);
        }

        if( bLastPoint && SG_Get_Distance(p, B) > M_ALMOST_ZERO )
        {
            Add_Point(B);
        }
    }

    //-----------------------------------------------------
    return( true );
}


//---------------------------------------------------------
bool CProfileFromPoints::Add_Point(const TSG_Point &Point)
{
    int			        x, y, i;
    double		        z, Distance, Distance_2;
    CSG_Table_Record    *pPoint, *pLast;

    if( Get_System().Get_World_to_Grid(x, y, Point) && m_pGrid->is_InGrid(x, y) )
    {
        z	= m_pGrid->asDouble(x, y);

        if( m_pProfile->Get_Count() == 0 )
        {
            Distance	= 0.0;
            Distance_2	= 0.0;
        }
        else
        {
            pLast		= m_pProfile->Get_Record(m_pProfile->Get_Count() - 1);
            Distance	= SG_Get_Distance(Point.x, Point.y, pLast->asDouble(F_X), pLast->asDouble(F_Y));

            if( Distance == 0.0 )
            {
                return( false );
            }

            Distance_2	= pLast->asDouble(F_Z) - z;
            Distance_2	= sqrt(Distance*Distance + Distance_2*Distance_2);

            Distance	+= pLast->asDouble(F_DIST);
            Distance_2	+= pLast->asDouble(F_DIST_SURF);
        }

        pPoint	= m_pProfile->Add_Record();

        pPoint->Set_Value(F_ID       , m_pProfile->Get_Count());
        pPoint->Set_Value(F_DIST     , Distance);
        pPoint->Set_Value(F_DIST_SURF, Distance_2);
        pPoint->Set_Value(F_X        , Point.x);
        pPoint->Set_Value(F_Y        , Point.y);
        pPoint->Set_Value(F_Z        , z);

        for(i=0; i<m_pValues->Get_Grid_Count(); i++)
        {
            pPoint->Set_Value(F_VALUES + i, m_pValues->Get_Grid(i)->asDouble(x, y));
        }

        return( true );
    }

    return( false );
}
