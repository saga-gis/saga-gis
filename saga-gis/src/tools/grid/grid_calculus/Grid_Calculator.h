
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Grid_Calculator.h                   //
//                                                       //
//                 Olaf Conrad (C) 2023                  //
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
#ifndef HEADER_INCLUDED__Grid_Calculator_H
#define HEADER_INCLUDED__Grid_Calculator_H


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
class CGrid_Calculator_Base : public CSG_Tool_Grid
{
public:
	CGrid_Calculator_Base(void);


protected:

	bool						m_bUseNoData, m_bPosition[4];

	int							m_nValues;

	CSG_Formula					m_Formula;

	TSG_Grid_Resampling			m_Resampling;


	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool						Preprocess_Formula		(CSG_String &Formula);
	bool						Preprocess_Find			(const CSG_String &Formula, const CSG_String &Function, CSG_String &Head, CSG_String &Argument, CSG_String &Tail);
	CSG_Data_Object *			Preprocess_Get_Object	(const CSG_String &Argument);

	bool						Initialize				(int nGrids, int nGrids_X);

	TSG_Data_Type				Get_Result_Type			(void);

	bool						Get_Result				(const CSG_Vector &Values, double &Result);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Calculator : public CGrid_Calculator_Base
{
public:
	CGrid_Calculator(void);


protected:

	virtual bool				On_Execute				(void);


private:

	CSG_Parameter_Grid_List		*m_pGrids, *m_pGrids_X;


	bool						Get_Values				(int x, int y, CSG_Vector &Values);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrids_Calculator : public CGrid_Calculator_Base
{
public:
	CGrids_Calculator(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Grid Collection|Calculus") );	}


protected:

	virtual bool				On_Execute				(void);


private:

	CSG_Parameter_Grids_List	*m_pGrids, *m_pGrids_X;


	virtual bool				Preprocess_Formula		(CSG_String &Formula);

	bool						Get_Values				(int x, int y, int z, CSG_Vector &Values);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Calculator_H
