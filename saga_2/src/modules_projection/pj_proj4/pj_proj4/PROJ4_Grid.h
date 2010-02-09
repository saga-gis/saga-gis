
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     PROJ4_Grid.h                      //
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
#ifndef HEADER_INCLUDED__PROJ4_Grid_H
#define HEADER_INCLUDED__PROJ4_Grid_H

//---------------------------------------------------------
#include "PROJ4_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class pj_proj4_EXPORT CPROJ4_Grid : public CPROJ4_Base
{
public:
	CPROJ4_Grid(int Interface, bool bInputList);

	virtual const SG_Char *		Get_MenuPath			(void)	{	return( _TL("R:Grid") );	}


protected:

	virtual bool				On_Execute_Conversion	(void);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

	int							m_Interpolation;

	CSG_Parameters_Grid_Target	m_Grid_Target;


	bool						Set_Grids				(CSG_Parameter_Grid_List *pSources, CSG_Parameter_Grid_List *pTargets);
	bool						Set_Shapes				(CSG_Parameter_Grid_List *pSources, CSG_Shapes              *pTarget );
	bool						Set_Grid				(CSG_Grid *pSource, CSG_Grid   *pTarget);
	bool						Set_Shapes				(CSG_Grid *pSource, CSG_Shapes *pTarget);

	bool						Init_XY					(const CSG_Grid_System &System, CSG_Grid **ppX, CSG_Grid **ppY);
	bool						Init_Target				(CSG_Grid *pSource, CSG_Grid *pTarget);

	void						Get_MinMax				(TSG_Rect &r, double x, double y);
	bool						Get_Target_Extent		(CSG_Grid *pSource, TSG_Rect &Extent, bool bEdge);

	CSG_Grid *					Get_Target_Autofit		(CSG_Grid *pSource, TSG_Data_Type Type);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__PROJ4_Grid_H
