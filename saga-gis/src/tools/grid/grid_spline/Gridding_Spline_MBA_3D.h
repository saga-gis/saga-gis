
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Gridding_Spline_MBA_3D.h               //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                      Olaf Conrad                      //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Gridding_Spline_MBA_3D_H
#define HEADER_INCLUDED__Gridding_Spline_MBA_3D_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Gridding_Spline_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGridding_Spline_MBA_3D : public CSG_Tool
{
public:
	CGridding_Spline_MBA_3D(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Grid|Gridding|Interpolation (3D)") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	int							m_zField;

	double						m_Epsilon, m_zCellsize;

	CSG_Matrix					m_Points;

	CSG_Grids					*m_pGrids;

	CSG_Parameters_Grid_Target	m_Grid_Target;


	bool						Initialize				(void);

	bool						_Set_MBA				(double dCell);

	bool						_Get_Difference			(const CSG_Grids &Phi, int Level);

	double						BA_Get_B				(int i, double d)	const;
	bool						BA_Set_Phi				(CSG_Grids &Phi, double Cellsize);
	double						BA_Get_Phi				(const CSG_Grids &Phi, double x, double y, double z)	const;
	void						BA_Set_Grids			(const CSG_Grids &Phi, bool bAdd = false);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Gridding_Spline_MBA_3D_H
