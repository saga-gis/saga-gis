/**********************************************************
 * Version $Id: 3d_viewer_shapes.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       3d_viewer                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 3d_viewer_shapes.cpp                  //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
#include "3d_viewer_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_Shapes_Panel : public CSG_3DView_Panel
{
public:
	C3D_Viewer_Shapes_Panel(wxWindow *pParent, CSG_Shapes *pShapes, int Field_Color);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual void				Update_Statistics		(void);
	virtual void				Update_Parent			(void);

	virtual void				On_Key_Down				(wxKeyEvent   &event);

	virtual bool				On_Before_Draw			(void);
	virtual bool				On_Draw					(void);

	virtual int					Get_Color				(double Value);


private:

	bool						m_Color_bGrad;

	double						m_Color_Min, m_Color_Scale;

	CSG_Colors					m_Colors;

	CSG_Shapes					*m_pShapes;


	void						Draw_Shape				(CSG_Shape *pShape, int Field_Color);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_Shapes_Panel, CSG_3DView_Panel)
	EVT_KEY_DOWN	(C3D_Viewer_Shapes_Panel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Shapes_Panel::C3D_Viewer_Shapes_Panel(wxWindow *pParent, CSG_Shapes *pShapes, int cField)
	: CSG_3DView_Panel(pParent)
{
	m_pShapes	= pShapes;

	//-----------------------------------------------------
	CSG_String	Attributes;

	for(int i=0; i<m_pShapes->Get_Field_Count(); i++)
	{
		Attributes	+= m_pShapes->Get_Field_Name(i);
		Attributes	+= "|";
	}

	//-----------------------------------------------------
	CSG_Parameter	*pNode, *pNode_1;

	//-----------------------------------------------------
	pNode	= m_Parameters("NODE_GENERAL");

	m_Parameters.Add_Value(
		pNode	, "Z_SCALE"			, _TL("Exaggeration"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(
		NULL	, "NODE_VIEW"		, _TL("Shapes View Settings"),
		_TL("")
	);

	pNode_1	= m_Parameters.Add_Choice(
		pNode	, "COLORS_ATTR"		, _TL("Colour Attribute"),
		_TL(""),
		Attributes, cField
	);

	m_Parameters.Add_Colors(
		pNode_1	, "COLORS"			, _TL("Colours"),
		_TL("")
	);

	m_Parameters.Add_Value(
		pNode_1	, "COLORS_GRAD"		, _TL("Graduated"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		pNode_1	, "COLORS_RANGE"	, _TL("Value Range"),
		_TL("")
	);

	//-----------------------------------------------------
	Update_Statistics();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Shapes_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_3DView_Panel::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_Shapes_Panel::Update_Statistics(void)
{
	int	Field	= m_Parameters("COLORS_ATTR")->asInt();

	m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
		m_pShapes->Get_Mean(Field) - 1.5 * m_pShapes->Get_StdDev(Field),
		m_pShapes->Get_Mean(Field) + 1.5 * m_pShapes->Get_StdDev(Field)
	);

	m_Data_Min.x	= m_pShapes->Get_Extent().Get_XMin();
	m_Data_Max.x	= m_pShapes->Get_Extent().Get_XMax();

	m_Data_Min.y	= m_pShapes->Get_Extent().Get_YMin();
	m_Data_Max.y	= m_pShapes->Get_Extent().Get_YMax();

	m_Data_Min.z	= m_pShapes->Get_ZMin();
	m_Data_Max.z	= m_pShapes->Get_ZMax();

	//-----------------------------------------------------
	Update_View();
}

//---------------------------------------------------------
void C3D_Viewer_Shapes_Panel::Update_Parent(void)
{
	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_Shapes_Panel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:	CSG_3DView_Panel::On_Key_Down(event);	return;

	case WXK_F1:	m_Parameters("Z_SCALE")->Set_Value(m_Parameters("Z_SCALE")->asDouble() -  0.5);	break;
	case WXK_F2:	m_Parameters("Z_SCALE")->Set_Value(m_Parameters("Z_SCALE")->asDouble() +  0.5);	break;
	}

	//-----------------------------------------------------
	Update_View();
	Update_Parent();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Shapes_Panel::On_Before_Draw(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Projector.Set_zScaling(m_Projector.Get_xScaling() * m_Parameters("Z_SCALE")->asDouble());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_Shapes_Panel::Get_Color(double Value)
{
	if( m_Color_Scale <= 0.0 )
	{
		return( (int)Value );
	}

	double	c	= m_Color_Scale * (Value - m_Color_Min);

	return( m_Color_bGrad ? m_Colors.Get_Interpolated(c) : m_Colors[(int)c] );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Shapes_Panel::On_Draw(void)
{
	//-----------------------------------------------------
	int		cField	= m_Parameters("COLORS_ATTR")->asInt();

	if( m_Parameters("COLORS_RANGE")->asRange()->Get_LoVal()
	>=  m_Parameters("COLORS_RANGE")->asRange()->Get_HiVal() )
	{
		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			m_pShapes->Get_Mean(cField) - 1.5 * m_pShapes->Get_StdDev(cField),
			m_pShapes->Get_Mean(cField) + 1.5 * m_pShapes->Get_StdDev(cField)
		);
	}

	m_Colors		= *m_Parameters("COLORS")->asColors();
	m_Color_bGrad	= m_Parameters("COLORS_GRAD")->asBool();
	m_Color_Min		= m_Parameters("COLORS_RANGE")->asRange()->Get_LoVal();
	m_Color_Scale	= m_Colors.Get_Count() / (m_Parameters("COLORS_RANGE")->asRange()->Get_HiVal() - m_Color_Min);

	//-------------------------------------------------
	for(int iShape=0; iShape<m_pShapes->Get_Count(); iShape++)
	{
		Draw_Shape(m_pShapes->Get_Shape(iShape), cField);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_Shapes_Panel::Draw_Shape(CSG_Shape *pShape, int Field_Color)
{
	int	Color	= Get_Color(pShape->asDouble(Field_Color));

	for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		switch( pShape->Get_Type() )
		{
		//-------------------------------------------------
		case SHAPE_TYPE_Point:
		case SHAPE_TYPE_Points:
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point_Z	a;
					TSG_Point	p	= pShape->Get_Point(iPoint, iPart);

					a.x	= p.x;
					a.y	= p.y;
					a.z	= pShape->Get_Z(iPoint, iPart);

					m_Projector.Get_Projection(a);

					Draw_Point(a.x, a.y, a.z, Color, 2);
				}
			}
			break;

		//-------------------------------------------------
		case SHAPE_TYPE_Line:
			{
				TSG_Point_Z	a, b;
				TSG_Point	p	= pShape->Get_Point(0, iPart);

				a.x	= p.x;
				a.y	= p.y;
				a.z	= pShape->Get_Z(0, iPart);

				m_Projector.Get_Projection(a);

				for(int iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					b	= a;
					p	= pShape->Get_Point(iPoint, iPart);
					a.x	= p.x;
					a.y	= p.y;
					a.z	= pShape->Get_Z(iPoint, iPart);

					m_Projector.Get_Projection(a);

					Draw_Line(a, b, Color);
				}
			}
			break;

		//-------------------------------------------------
		case SHAPE_TYPE_Polygon:
			{
				TSG_Point_Z	a, b;
				TSG_Point	p	= pShape->Get_Point(0, iPart, false);

				a.x	= p.x;
				a.y	= p.y;
				a.z	= pShape->Get_Z(0, iPart, false);

				m_Projector.Get_Projection(a);

				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					b	= a;	p	= pShape->Get_Point(iPoint, iPart);

					a.x	= p.x;
					a.y	= p.y;
					a.z	= pShape->Get_Z(iPoint, iPart);

					m_Projector.Get_Projection(a);

					Draw_Line(a, b, Color);
				}
			}
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_Shapes_Dialog : public CSG_3DView_Dialog
{
public:
	C3D_Viewer_Shapes_Dialog(CSG_Shapes *pShapes, int Field_Color)
		: CSG_3DView_Dialog(_TL("3D Shapes Viewer"))
	{
		Create(new C3D_Viewer_Shapes_Panel(this, pShapes, Field_Color));
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_Shapes::C3D_Viewer_Shapes(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("3D Shapes Viewer"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		"3D viewer for 3D Shapes."
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "COLOR"	, _TL("Colour"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_Shapes::On_Execute(void)
{
	CSG_Shapes	*pShapes	= Parameters("SHAPES")->asShapes();

	if( !pShapes->is_Valid() )
	{
		Error_Set(_TL("invalid input"));

		return( false );
	}

	C3D_Viewer_Shapes_Dialog	dlg(pShapes, Parameters("COLOR")->asInt());

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
