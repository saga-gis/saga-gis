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
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Shapes_Cut_Interactive.cpp              //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <string.h>

#include "shapes_cut_interactive.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes_Cut_Interactive::CShapes_Cut_Interactive(void)
{
	Set_Name		(_TL("Cut Shapes Layer"));

	Set_Author		(SG_T("(c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		""
	));

	Set_Drag_Mode	(MODULE_INTERACTIVE_DRAG_BOX);

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "CUT"			, _TL("Cut"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "EXTENT"		, _TL("Extent"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		Cut_Methods_Str(), 0
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("CUT", _TL("Extent"), _TL(""));

	pParameters->Add_Value(
		NULL, "AX"	, _TL("Left")				, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "BX"	, _TL("Right")				, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "AY"	, _TL("Bottom")				, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "BY"	, _TL("Top")				, _TL(""), PARAMETER_TYPE_Double
	);

	pParameters->Add_Value(
		NULL, "DX"	, _TL("Horizontal Range")	, _TL(""), PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	pParameters->Add_Value(
		NULL, "DY"	, _TL("Vertical Range")		, _TL(""), PARAMETER_TYPE_Double, 1.0, 0.0, true
	);
}

//---------------------------------------------------------
CShapes_Cut_Interactive::~CShapes_Cut_Interactive(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes_Cut_Interactive::On_Execute(void)
{
	m_bDown		= false;

	m_pShapes	= Parameters("SHAPES")	->asShapes();
	m_pCut		= Parameters("CUT")		->asShapes();
	m_pExtent	= Parameters("EXTENT")	->asShapes();
	m_Method	= Parameters("METHOD")	->asInt();

	m_pCut->Create(m_pShapes->Get_Type());

	return( true );
}

//---------------------------------------------------------
bool CShapes_Cut_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	switch( Mode )
	{
	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LDOWN:
		if( m_bDown == false )
		{
			m_bDown	= true;
			m_pDown	= ptWorld;
		}

		return( true );

	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LUP:
		if( m_bDown == true )
		{
			m_bDown	= false;

			CSG_Rect	r(m_pDown, ptWorld);

			if( Get_Extent(r) )
			{
				if( Cut_Set_Extent(r, m_pExtent, true) )
				{
					DataObject_Update(m_pExtent);
				}

				if( Cut_Shapes(r, m_Method, m_pShapes, m_pCut) )
				{
					DataObject_Update(m_pCut);
				}
				else
				{
					Message_Add(_TL("No shapes in selection"));
				}
			}
		}

		return( true );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CShapes_Cut_Interactive::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameters->Get_Identifier(), SG_T("CUT")) )
	{
		double	ax, ay, bx, by, dx, dy, d;

		ax	= pParameters->Get_Parameter("AX")->asDouble();
		ay	= pParameters->Get_Parameter("AY")->asDouble();
		bx	= pParameters->Get_Parameter("BX")->asDouble();
		by	= pParameters->Get_Parameter("BY")->asDouble();
		dx	= pParameters->Get_Parameter("DX")->asDouble();
		dy	= pParameters->Get_Parameter("DY")->asDouble();

		if( ax > bx )	{	d	= ax;	ax	= bx;	bx	= d;	}
		if( ay > by )	{	d	= ay;	ay	= by;	by	= d;	}

		if     ( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DX")) )
		{
			bx	= ax + dx;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("AX"))
			||	 !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("BX")) )
		{
			dx	= bx - ax;
		}

		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DY")) )
		{
			by	= ay + dy;
		}
		else if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("AY"))
			||	 !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("BY")) )
		{
			dy	= by - ay;
		}

		pParameters->Get_Parameter("AX")->Set_Value(ax);
		pParameters->Get_Parameter("AY")->Set_Value(ay);
		pParameters->Get_Parameter("BX")->Set_Value(bx);
		pParameters->Get_Parameter("BY")->Set_Value(by);
		pParameters->Get_Parameter("DX")->Set_Value(dx);
		pParameters->Get_Parameter("DY")->Set_Value(dy);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CShapes_Cut_Interactive::Get_Extent(CSG_Rect &r)
{
	CSG_Parameters	*pParameters	= Get_Parameters("CUT");

	pParameters->Get_Parameter("AX")->Set_Value(r.Get_XMin());
	pParameters->Get_Parameter("AY")->Set_Value(r.Get_YMin());
	pParameters->Get_Parameter("BX")->Set_Value(r.Get_XMax());
	pParameters->Get_Parameter("BY")->Set_Value(r.Get_YMax());
	pParameters->Get_Parameter("DX")->Set_Value(r.Get_XRange());
	pParameters->Get_Parameter("DY")->Set_Value(r.Get_YRange());

	if( Dlg_Parameters("CUT") )
	{
		r.Assign(
			pParameters->Get_Parameter("AX")->asDouble(),
			pParameters->Get_Parameter("AY")->asDouble(),
			pParameters->Get_Parameter("BX")->asDouble(),
			pParameters->Get_Parameter("BY")->asDouble()
		);

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
