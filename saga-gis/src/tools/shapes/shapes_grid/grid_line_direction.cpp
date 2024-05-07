///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                       Network                         //
//                     Line_Direction                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  LineDirection.cpp                    //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                  Johan Van de Wauw                    //
//               based on Shapes2grid.cpp by             //
//                  Olaf Conrad (c) 2003                 //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_line_direction.h"

#include <algorithm>

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define X_WORLD_TO_GRID(X)	(((X) - m_pGrid->Get_XMin()) / m_pGrid->Get_Cellsize())
#define Y_WORLD_TO_GRID(Y)	(((Y) - m_pGrid->Get_YMin()) / m_pGrid->Get_Cellsize())

//---------------------------------------------------------

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGridLineDirection::CGridLineDirection(void)
{
    //-----------------------------------------------------
    Set_Name		(_TL("Line Direction"));

    Set_Author		("Johan Van de Wauw (c) 2019");

    Set_Description	(_TW(
                         "Creates a grid of the direction (fat) of a line shapes. "
                     ));

    //-----------------------------------------------------
    Parameters.Add_Shapes("",
                          "INPUT"		, _TL("Shapes"),
                          _TL(""),
                          PARAMETER_INPUT
                         );

    Parameters.Add_Table_Field("INPUT", "ORDER_FIELD", "Order Field", "Field for order in which the shape will be sorted prior to rasterization.", false);

    //-----------------------------------------------------
    m_Grid_Target.Create(&Parameters, false, NULL, "TARGET_");

    m_Grid_Target.Add_Grid("GRID", _TL("Grid"), false);
}


//---------------------------------------------------------

int CGridLineDirection::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
    if (!SG_STR_CMP(pParameter->Get_Identifier(), "INPUT"))
    {
        m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());
    }

    m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

    return(CSG_Tool::On_Parameter_Changed(pParameters, pParameter));
}


int CGridLineDirection::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
    if (!SG_STR_CMP(pParameter->Get_Identifier(), "INPUT"))
    {
        pParameters->Set_Enabled("LINE_TYPE", pParameter->asShapes() && pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Line);
        pParameters->Set_Enabled("POLY_TYPE", pParameter->asShapes() && pParameter->asShapes()->Get_Type() == SHAPE_TYPE_Polygon);
    }

    if (!SG_STR_CMP(pParameter->Get_Identifier(), "OUTPUT"))
    {
        pParameters->Set_Enabled("FIELD", pParameter->asInt() == 2);
        pParameters->Set_Enabled("MULTIPLE", pParameter->asInt() != 0);
        pParameters->Set_Enabled("GRID_TYPE", pParameter->asInt() == 2);
    }

    m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

    return(CSG_Tool::On_Parameters_Enable(pParameters, pParameter));
}


bool CGridLineDirection::On_Execute(void)
{
    //-----------------------------------------------------
    CSG_Shapes	*pShapes	= Parameters("INPUT")->asShapes();

    //-----------------------------------------------------

    if( (m_pGrid = m_Grid_Target.Get_Grid("GRID", SG_DATATYPE_Byte)) == NULL )
    {
        return( false );
    }
    m_pGrid->Set_NoData_Value(255);

    if( !pShapes->Get_Extent().Intersects(m_pGrid->Get_Extent()) )
    {
        Error_Set(_TL("Lines' and target grid's extent do not intersect."));

        return( false );
    }

    m_pGrid->Set_Name(CSG_String::Format("%s [Direction]", pShapes->Get_Name()));
    m_pGrid->Assign_NoData();

    // sort shape if necessary
    int sort_field=-1;
    sort_field = Parameters("ORDER_FIELD")->asInt();

    pShapes->Set_Index(sort_field, TABLE_INDEX_Ascending);

    //-----------------------------------------------------
    for(sLong i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
    {
        CSG_Shape	*pShape;
        pShape	= sort_field>-1?pShapes->Get_Shape_byIndex(i):pShapes->Get_Shape(i);

        if( pShape->Intersects(m_pGrid->Get_Extent()) )
        {
            Set_Line	(pShape);
        }

    }



    //-----------------------------------------------------
    return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CGridLineDirection::Set_Value(int x, int y, double Value)
{
    TSG_Point_Int p = prev;
    prev.x = x;
    prev.y = y;
    x = p.x;
    y = p.y;


    if( m_pGrid->is_InGrid(x, y, false) )
    {
        m_pGrid->Set_Value(x, y, Value);
    }
}



///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGridLineDirection::Set_Line(CSG_Shape *pShape)
{
    for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
    {
        int	iPoint	= pShape->Get_Type() == SHAPE_TYPE_Polygon ? 0 : 1;

        TSG_Point	a, b	= pShape->Get_Point(0, iPart, iPoint != 0);

        b.x	= X_WORLD_TO_GRID(b.x);
        b.y	= Y_WORLD_TO_GRID(b.y);

        for( ; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
        {
            a	= b;
            b	= pShape->Get_Point(iPoint, iPart);

            b.x	= X_WORLD_TO_GRID(b.x);
            b.y	= Y_WORLD_TO_GRID(b.y);

            Set_Line_Fat(a, b);

        }
    }
}

//---------------------------------------------------------
void CGridLineDirection::Set_Line_Fat(TSG_Point a, TSG_Point b)
{
    double Value;

    TSG_Point_Int	A;
    A.x	= (int)(a.x	+= 0.5);
    A.y	= (int)(a.y	+= 0.5);
    TSG_Point_Int	B;
    B.x	= (int)(b.x	+= 0.5);
    B.y	= (int)(b.y	+= 0.5);

    prev = A;
    //-----------------------------------------------------
    if( A.x != B.x || A.y != B.y )
    {
        double	dx	= b.x - a.x;
        double	dy	= b.y - a.y;

        a.x	= a.x > 0. ? a.x - (int)a.x : 1. + (a.x - (int)a.x);
        a.y	= a.y > 0. ? a.y - (int)a.y : 1. + (a.y - (int)a.y);

        //-------------------------------------------------
        if( fabs(dx) > fabs(dy) )
        {
            int	ix	= dx > 0. ? 1 : -1;
            int	iy	= dy > 0. ? 1 : -1;

            double	d, e;

            d	= fabs(dy / dx);
            dx	= ix < 0 ? a.x : 1. - a.x;
            e	= iy > 0 ? a.y : 1. - a.y;
            e	+= d * dx;

            while( e > 1. )
            {
                Value = iy>0? 0 : 4;
                Set_Value(A.x, A.y += iy, Value);
                e--;
            }

            while( A.x != B.x )
            {
                Value = ix>0 ? 2 : 6;
                Set_Value(A.x += ix, A.y, Value);
                e += d;

                if( A.x != B.x )
                {
                    while( e > 1. )
                    {
                        Value = iy>0 ? 0 : 4;
                        Set_Value(A.x, A.y += iy, Value);
                        e--;
                    }
                }
            }

            if( A.y != B.y )
            {
                iy	= A.y < B.y ? 1 : -1;

                while( A.y != B.y )
                {
                    Value = iy>0 ? 0 : 4;
                    Set_Value(A.x, A.y += iy, Value);
                }
            }
        }

        //-------------------------------------------------
        else // if( fabs(dy) > fabs(dx) )
        {
            int	ix	= dx > 0.0 ? 1 : -1;
            int	iy	= dy > 0.0 ? 1 : -1;

            double	d, e;

            d	= fabs(dx / dy);
            dy	= iy < 0 ? a.y : 1.0 - a.y;
            e	= ix > 0 ? a.x : 1.0 - a.x;
            e	+= d * dy;

            while( e > 1. )
            {
                Value = ix>0 ? 2 : 6;
                Set_Value(A.x += ix, A.y, Value);
                e--;
            }

            while( A.y != B.y )
            {
                Value = iy>0 ? 0 : 4;
                Set_Value(A.x, A.y += iy, Value);
                e += d;

                if( A.y != B.y )
                {
                    while( e > 1. )
                    {
                        Value = ix>0 ? 2 : 6;
                        Set_Value(A.x += ix, A.y, Value);
                        e--;
                    }
                }
            }

            if( A.x != B.x )
            {
                ix	= A.x < B.x ? 1 : -1;

                while( A.x != B.x )
                {
                    Value = ix>0 ? 2 : 6;
                    Set_Value(A.x += ix, A.y, Value);
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////
