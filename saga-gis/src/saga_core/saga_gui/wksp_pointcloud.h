
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  WKSP_PointCloud.h                    //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_PointCloud_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_PointCloud_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_layer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_PointCloud : public CWKSP_Layer
{
public:
	CWKSP_PointCloud(CSG_PointCloud *pPointCloud);
	virtual ~CWKSP_PointCloud(void);

	virtual TWKSP_Item			Get_Type				(void)	{	return( WKSP_ITEM_PointCloud );	}

	virtual wxString			Get_Description			(void);

	virtual wxMenu *			Get_Menu				(void);

	virtual bool				On_Command				(int Cmd_ID);
	virtual bool				On_Command_UI			(wxUpdateUIEvent &event);

	CSG_PointCloud *			Get_PointCloud			(void)	{	return( m_pPointCloud );	}

	virtual wxString			Get_Value				(CSG_Point ptWorld, double Epsilon);
	virtual double				Get_Value_Range			(void);

	bool						asImage					(CSG_Grid *pImage);


protected:

	int							m_Color_Field, m_PointSize, m_Aggregation;

	wxColour					m_Color_Pen;

	CSG_PointCloud				*m_pPointCloud;

	CSG_Grid					m_Z, m_N;


	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);
	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual wxMenu *			On_Edit_Get_Menu		(void);
	virtual bool				On_Edit_On_Mouse_Up		(CSG_Point Point, double ClientToWorld, int Key);
	virtual bool				On_Edit_Set_Attributes	(void);
	virtual TSG_Rect			On_Edit_Get_Extent		(void);

	virtual void				On_Draw					(CWKSP_Map_DC &dc_Map, bool bEdit);

	void						_Draw_Point				(CWKSP_Map_DC &dc_Map, int x, int y, double z, int Color);
	void						_Draw_Point				(CWKSP_Map_DC &dc_Map, int x, int y, double z, int Color, int Radius);
	void						_Draw_Points			(CWKSP_Map_DC &dc_Map);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_PointCloud_H
