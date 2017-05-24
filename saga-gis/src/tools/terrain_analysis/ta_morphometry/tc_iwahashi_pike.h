/**********************************************************
 * Version $Id: tpi.h 911 2011-02-14 16:38:15Z reklov_w $
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
//                  tc_iwahashi_pike.h                   //
//                                                       //
//                 Copyright (C) 2012 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__tc_iwahashi_pike_H
#define HEADER_INCLUDED__tc_iwahashi_pike_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTC_Parameter_Base : public CSG_Tool_Grid
{
public:
	CTC_Parameter_Base(void);


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	void					On_Construction			(void);

	bool					Get_Parameter			(CSG_Grid *pValues, CSG_Grid *pParameter);


private:

	CSG_Grid_Cell_Addressor	m_Kernel;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTC_Texture : public CTC_Parameter_Base
{
public:
	CTC_Texture(void);


protected:

	virtual bool			On_Execute				(void);


private:

	CSG_Grid				*m_pDEM;


	int						Get_Noise				(int x, int y, double Epsilon = 0.0);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTC_Convexity : public CTC_Parameter_Base
{
public:
	CTC_Convexity(void);


protected:

	virtual bool			On_Execute				(void);


private:

	CSG_Grid				*m_pDEM;


	int						Get_Laplace				(int x, int y, const double Kernel[2], int Type, double Epsilon);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTC_Classification : public CSG_Tool_Grid
{
public:
	CTC_Classification(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("A:Terrain Analysis|Terrain Classification" ));	}


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	double					m_Mean_Slope, m_Mean_Convexity, m_Mean_Texture;

	CSG_Simple_Statistics	m_Stat_Slope, m_Stat_Convexity, m_Stat_Texture;

	CSG_Grid				*m_pSlope, *m_pConvexity, *m_pTexture;


	bool					Get_Classes				(void);
	int						Get_Class				(int Level, int x, int y, bool bLastLevel);

	void					Set_LUT					(CSG_Grid *pLandforms, int nLevels);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__tc_iwahashi_pike_H
