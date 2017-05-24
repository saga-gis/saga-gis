/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Transect                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Polygon_Transect.cpp                //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                   Johan Van de Wauw                   //
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
#include "Polygon_Transect.h"
#include <map>
#include <list>

using std::map;
using std::list;
using std::pair;
///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPolygon_Transect::CPolygon_Transect(void)
{
    //-----------------------------------------------------
    // Give some information about your tool...

    Set_Name	(_TL("Transect through polygon shapefile"));

    Set_Author	(_TL("Copyrights (c) 2009 Johan Van de Wauw"));

    Set_Description	(_TW(
                    "Transect for lines and polygon shapefiles\n"
                    "\n"
                    "The goal of this tool is to create a transect along a line through a polygon map.\n"
                    "Eg\n"
                    "\n"
                    "|____ST1_____!_ST2_!__ST1__!_______ST#_____|\n"
                    "\n"
                    "(Soil type 1 etc...)\n"
                    "\n"
                    "This is done by creating a table with the ID of each line, the distance \n"
                    "to the starting point and the different transects:\n"
                    "\n"
                    "<pre>|  line_id  |  start  |  end  |  code/field  |\n"
                    "|    0      |    0    |  124  |     ST1      |\n"
                    "|    0      |   124   |  300  |     ST2      |\n"
                    "|    0      |   300   | 1223  |     ST1      |\n"
                    "|    0      |  1223   | 2504  |     ST3      |\n"
                    "|    1      |    0    |  200  |     ST4      |\n"
                    "|   ...     |   ...   |  ...  |     ...      |</pre>\n"
                    "\n"
                    "\n"
                    "The tool requires an input shape with all the line transects [Transect_Line] \n"
                    "and a polygon theme [Theme]. You also have to select which field you want to have in \n"
                    "the resulting table [Transect_Result]. This can be an ID of the polygon theme if you \n"
                    "want to link the tables later on, or any other field [Theme_Field].\n")
                    );


    //-----------------------------------------------------
    // Define your parameters list...
    CSG_Parameter *pTheme;
    Parameters.Add_Shapes(
        NULL, "TRANSECT", _TL("Line Transect(s)"),
        _TL(""),
        PARAMETER_INPUT, SHAPE_TYPE_Line
    );

    pTheme = Parameters.Add_Shapes(
                 NULL, "THEME", _TL("Theme"),
                 _TL(""),
                 PARAMETER_INPUT, SHAPE_TYPE_Polygon
             );

    Parameters.Add_Table_Field(
        pTheme	, "THEME_FIELD"		, _TL("Theme Field"),
        _TL(""),
        PARAMETER_INPUT
    );
    Parameters.Add_Table(
        NULL, "TRANSECT_RESULT", _TL("Result table"),
        _TL(""),
        PARAMETER_OUTPUT
    );
}

//---------------------------------------------------------
CPolygon_Transect::~CPolygon_Transect(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPolygon_Transect::On_Execute(void)
{
    CSG_Table   *pTransect_Result;
    CSG_Shapes  *pTheme, *pTransect;

    CSG_Shape_Line    *pLine;
    CSG_Shape_Polygon *pTheme_Shape;
    int Theme_Field;

    pTransect   = Parameters("TRANSECT")->asShapes();
    pTheme      = Parameters("THEME")->asShapes();

    pTransect_Result = Parameters("TRANSECT_RESULT")->asTable();
    Theme_Field = Parameters("THEME_FIELD")->asInt();

    //-----------------------------------------------------
    // Check for valid parameter settings...
    //-----------------------------------------------------
    if (pTheme->Get_Type() != SHAPE_TYPE_Polygon){
        Error_Set(CSG_String("[THEME] is not a polygon file"));
        return(false);
    }
    if (pTransect->Get_Type() != SHAPE_TYPE_Line){
        Error_Set(CSG_String("[TRANSECT] is not a line shapefile"));
        return(false);
    }
    if (pTheme->Get_Count() ==0 || pTransect->Get_Count()==0)
    {
        Error_Set(CSG_String("[TRANSECT] or [THEME] is empty"));
        return(false);
    }
    if (!(pTheme->Get_Extent().Intersects(pTransect->Get_Extent())))
    {
        Error_Set(CSG_String("[TRANSECT] and [THEME] do not intersect"));
        return(false);
    }

    const int LINE_ID = pTransect_Result->Get_Field_Count();
    pTransect_Result->Add_Field(SG_T("line_id"), SG_DATATYPE_Int);
    const int START = pTransect_Result->Get_Field_Count();
    pTransect_Result->Add_Field(SG_T("start"), SG_DATATYPE_Double);
    const int END = pTransect_Result->Get_Field_Count();
    pTransect_Result->Add_Field(SG_T("end"), SG_DATATYPE_Double);
    const int POLY_ID = pTransect_Result->Get_Field_Count();
    pTransect_Result->Add_Field(SG_T("poly_id"), SG_DATATYPE_Int);
    const int FIELD = pTransect_Result->Get_Field_Count();
    pTransect_Result->Add_Field(SG_T("field"), pTheme->Get_Field_Type(Theme_Field));
    
    // Short description of the algorithm:
    // For every line it is checked whether it crosses a polygon. If it does, 
    // the points where it crosses are recorded in the map LineBorders, with 
    // the polygon_id as key.
    
    for (int iLine=0; iLine<pTransect->Get_Count() && Set_Progress(iLine, pTransect->Get_Count());
                iLine++)
    {
        pLine =(CSG_Shape_Line *) pTransect->Get_Shape(iLine);
        for (int iLinePart=0;iLinePart<pLine->Get_Part_Count();iLinePart++)
        {
            CSG_Shape_Part *pLinePart =pLine->Get_Part(iLinePart);
            CSG_Rect LinePartExtent = pLinePart->Get_Extent();
            map<int,list<double> > LineBorders;
            for (int iShape=0; iShape<pTheme->Get_Count();iShape++)
            {
                pTheme_Shape = (CSG_Shape_Polygon *) pTheme->Get_Shape(iShape);
                if (pLinePart->Get_Extent().Intersects(pTheme_Shape->Get_Extent())>0)
                {
                    for (int iPart=0; iPart<pTheme_Shape->Get_Part_Count(); iPart++)
                    {
                        CSG_Shape_Polygon_Part *pPart = (CSG_Shape_Polygon_Part *) pTheme_Shape->Get_Part(iPart);
                        if (pPart->Contains(pLinePart->Get_Point(0)))
                            LineBorders[iShape].push_back(0);
                        if (pPart->Contains(pLinePart->Get_Point(pLinePart->Get_Count())))
                            LineBorders[iShape].push_back(pLine->Get_Length(iLinePart));
                        for (int iPoint=0; iPoint<pPart->Get_Count();iPoint++)
                        {
                            int iPoint2 = (iPoint!=pPart->Get_Count()-1)?iPoint+1:0;
                            TSG_Point Crossing;
                            double Length=0;
                            for (int iLinePartPoint=0; iLinePartPoint<pLinePart->Get_Count(); iLinePartPoint++)
                            {
                                if (SG_Get_Crossing(Crossing,pPart->Get_Point(iPoint),pPart->Get_Point(iPoint2),
                                                    pLinePart->Get_Point(iLinePartPoint), pLinePart->Get_Point(iLinePartPoint+1)))
                                {
                                    LineBorders[iShape].push_back(Length+SG_Get_Distance(Crossing, pLinePart->Get_Point(iLinePartPoint)));
                                }
                                Length+=SG_Get_Distance(pLinePart->Get_Point(iLinePartPoint), pLinePart->Get_Point(iLinePartPoint+1));
                            }
                        }
                    }
                }
            }
            // convert LineBorders to the result table
            // the table contains the lineids and the distance to the origin of the line,
            // and it is sorted by lineid, polygonid
            CSG_Table_Record *pRecord;
            for (map<int,list<double> >::iterator shapeit=LineBorders.begin();shapeit!=LineBorders.end();++shapeit)
            {
                //shapeit->second.sort();
                bool start=1;
                for (list<double>::iterator i=shapeit->second.begin(); i!=shapeit->second.end();++i)
                {
                    if (start){
                        pRecord =pTransect_Result->Add_Record();
                        pRecord->Set_Value(LINE_ID, iLine);
                        pRecord->Set_Value(START, *i);
                        pRecord->Set_Value(POLY_ID, shapeit->first);
                        CSG_Table_Record *pTheme_Record =pTheme->Get_Shape(shapeit->first) ;
                        if(pTheme->Get_Field_Type(Theme_Field)== SG_DATATYPE_String )
                            pRecord->Set_Value(FIELD, pTheme_Record->asString(Theme_Field));
                        else
                            pRecord->Set_Value(FIELD, pTheme_Record->asDouble(Theme_Field));
                        start = 0;
                    }
                    else
                    {
                        pRecord->Set_Value(END, *i);
                        start=1;
                    }
                }
            }
        }
    }
    return( true );
}
