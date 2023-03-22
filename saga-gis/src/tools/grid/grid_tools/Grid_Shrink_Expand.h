
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Shrink_Expand.h                  //
//                                                       //
//                 Copyright (C) 2011 by                 //
//            Volker Wichmann and Olaf Conrad            //
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
#ifndef HEADER_INCLUDED__Grid_Shrink_Expand_H
#define HEADER_INCLUDED__Grid_Shrink_Expand_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Shrink_Expand : public CSG_Tool_Grid
{
public:
	CGrid_Shrink_Expand(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("A:Grid|Gaps") );	}


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);

	bool					Set_Kernel				(bool bInitialize = true);

	bool					Do_Operation			(CSG_Grid *pInput, CSG_Grid *pResult);


private:

	CSG_Grid_Cell_Addressor	m_Kernel;


	bool					Do_Shrink				(CSG_Grid *pInput, CSG_Grid *pResult);
	bool					Do_Expand				(CSG_Grid *pInput, CSG_Grid *pResult);
	bool					Do_Expand				(CSG_Grid *pInput, CSG_Grid *pResult, const CSG_Grid_Cell_Addressor &Kernel);

	bool					Get_Expand_Value		(CSG_Grid *pInput, int x, int y, int Method, double &Value, const CSG_Grid_Cell_Addressor &Kernel);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrids_Shrink_Expand : public CGrid_Shrink_Expand
{
public:
	CGrids_Shrink_Expand(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("A:Grid Collection|Gaps") );	}


protected:

	virtual bool			On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Shrink_Expand_H
