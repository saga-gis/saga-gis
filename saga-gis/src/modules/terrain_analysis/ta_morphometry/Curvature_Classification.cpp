/**********************************************************
 * Version $Id: Curvature_Classification.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             Curvature_Classification.cpp              //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#include "Curvature_Classification.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCurvature_Classification::CCurvature_Classification(void)
{
	Set_Name		(_TL("Curvature Classification"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Surface curvature based terrain classification.\n"
		"Reference:\n"
		"Dikau, R. (1988):\n'Entwurf einer geomorphographisch-analytischen Systematik von Reliefeinheiten',\n"
		"Heidelberger Geographische Bausteine, Heft 5\n\n"
		"0 - V  / V\n"
		"1 - GE / V\n"
		"2 - X  / V\n"
		"3 - V  / GR\n"
		"4 - GE / GR\n"
		"5 - X  / GR\n"
		"6 - V  / X\n"
		"7 - GE / X\n"
		"8 - X  / X\n"
	));

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CLASS"		, _TL("Curvature Classification"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Char
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Threshold for plane"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0005, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCurvature_Classification::On_Execute(void)
{
	//-----------------------------------------------------
	double		Threshold;
	CSG_Grid	*pClass;

	m_pDEM		= Parameters("DEM"      )->asGrid();
	pClass		= Parameters("CLASS"    )->asGrid();
	Threshold	= Parameters("THRESHOLD")->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	Plan, Prof;

			if( Get_Curvature(x, y, Plan, Prof) )
			{
				pClass->Set_Value(x, y,
						(fabs(Plan) < Threshold ? 3 : Plan < 0 ? 0 : 6)
					+	(fabs(Prof) < Threshold ? 1 : Prof < 0 ? 0 : 2)
				);
			}
			else
			{
				pClass->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Parameters	P;

	if( DataObject_Get_Parameters(pClass, P) && P("COLORS_TYPE") && P("LUT") )
	{
		int Color[9]	=
		{
			SG_GET_RGB(  0,   0, 127),	// V / V
			SG_GET_RGB(  0,  63, 200),	// G / V
			SG_GET_RGB(  0, 127, 255),	// X / V
			SG_GET_RGB(127, 200, 255),	// V / G
			SG_GET_RGB(245, 245, 245),	// G / G
			SG_GET_RGB(255, 200, 127),	// X / G
			SG_GET_RGB(255, 127,   0),	// V / X
			SG_GET_RGB(200,  63,   0),	// G / X
			SG_GET_RGB(127,   0,   0),	// X / X
		};

		//-------------------------------------------------
		CSG_Strings	Name, Desc;

		Name	+= _TL( "V / V" );	Desc	+= _TL( "V / V" );
		Name	+= _TL("GE / V" );	Desc	+= _TL("GE / V" );
		Name	+= _TL( "X / V" );	Desc	+= _TL( "X / V" );
		Name	+= _TL( "V / GR");	Desc	+= _TL( "V / GR");
		Name	+= _TL("GE / GR");	Desc	+= _TL("GE / GR");
		Name	+= _TL( "X / GR");	Desc	+= _TL( "X / GR");
		Name	+= _TL( "V / X" );	Desc	+= _TL( "V / X" );
		Name	+= _TL("GE / X" );	Desc	+= _TL("GE / X" );
		Name	+= _TL( "X / X" );	Desc	+= _TL( "X / X" );

		//-------------------------------------------------
		CSG_Table	*pTable	= P("LUT")->asTable();

		pTable->Del_Records();

		for(int i=0; i<9; i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, Color[i]);
			pRecord->Set_Value(1, Name [i].c_str());
			pRecord->Set_Value(2, Desc [i].c_str());
			pRecord->Set_Value(3, i);
			pRecord->Set_Value(4, i);
		}

		P("COLORS_TYPE")->Set_Value(1);	// Color Classification Type: Lookup Table

		DataObject_Set_Parameters(pClass, P);
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
bool CCurvature_Classification::Get_Curvature(int x, int y, double &Plan, double &Profile)
{
	static const int	Index[8]	=	{ 5, 8, 7, 6, 3, 0, 1, 2 };

	if( !m_pDEM->is_InGrid(x, y) )
	{
		return( false );
	}

	double	z	= m_pDEM->asDouble(x, y), Z[9];	Z[4]	= 0.0;

	for(int i=0, ix, iy; i<8; i++)
	{
		if( m_pDEM->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) )
		{
			Z[Index[i]]	= m_pDEM->asDouble(ix, iy) - z;
		}
		else if( m_pDEM->is_InGrid(ix = Get_xFrom(i, x), iy = Get_yFrom(i, y)) )
		{
			Z[Index[i]]	= z - m_pDEM->asDouble(ix, iy);
		}
		else
		{
			Z[Index[i]]	= 0.0;
		}
	}

	double	D	= ((Z[3] + Z[5]) / 2.0 - Z[4]) * 2.00 / Get_Cellarea();
	double	E	= ((Z[1] + Z[7]) / 2.0 - Z[4]) * 2.00 / Get_Cellarea();
	double	F	=  (Z[0] - Z[2] - Z[6] + Z[8]) * 0.25 / Get_Cellarea();
	double	G	=  (Z[5] - Z[3])               * 0.50 / Get_Cellsize();
    double	H	=  (Z[7] - Z[1])               * 0.50 / Get_Cellsize();

	Profile	= -2.0 * (D * G*G + E * H*H + F*G*H) / (G*G + H*H);
	Plan	= -2.0 * (E * G*G + D * H*H - F*G*H) / (G*G + H*H);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
