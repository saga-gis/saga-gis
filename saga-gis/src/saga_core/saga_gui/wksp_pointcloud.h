/**********************************************************
 * Version $Id$
 *********************************************************/

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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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

	CSG_PointCloud *			Get_PointCloud			(void)	{	return( (CSG_PointCloud *)m_pObject );	}
	class CWKSP_Table *			Get_Table				(void)	{	return( m_pTable );		}

	virtual wxString			Get_Description			(void);

	virtual wxMenu *			Get_Menu				(void);

	virtual bool				On_Command				(int Cmd_ID);
	virtual bool				On_Command_UI			(wxUpdateUIEvent &event);

	virtual wxString			Get_Value				(CSG_Point ptWorld, double Epsilon);
	virtual double				Get_Value_Range			(void);

	int							Get_Color_Field			(void)	{	return( m_Color_Field );		}

	bool						asImage					(CSG_Grid *pImage);

	wxString					Get_Name_Attribute		(void);

	virtual wxMenu *			Edit_Get_Menu			(void);
	virtual TSG_Rect			Edit_Get_Extent			(void);
	virtual bool				Edit_On_Mouse_Up		(CSG_Point Point, double ClientToWorld, int Key);
	virtual bool				Edit_Set_Attributes		(void);


protected:

	int							m_Color_Field, m_PointSize, m_Aggregation;

	wxColour					m_Color_Pen;

	CSG_Grid					m_Z, m_N;

	class CWKSP_Table			*m_pTable;


	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);
	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);
	virtual void				On_Update_Views			(void);

	void						_LUT_Create				(void);

	virtual void				On_Draw					(CWKSP_Map_DC &dc_Map, int Flags);

	void						_Draw_Point				(CWKSP_Map_DC &dc_Map, int x, int y, double z, int Color);
	void						_Draw_Point				(CWKSP_Map_DC &dc_Map, int x, int y, double z, int Color, int Radius);
	void						_Draw_Points			(CWKSP_Map_DC &dc_Map);

	CSG_Parameter *				_AttributeList_Add		(CSG_Parameter *pNode, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	void						_AttributeList_Set		(CSG_Parameter *pFields, bool bAddNoField);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_PointCloud_H
