/**********************************************************
 * Version $Id: tpi.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
class CTC_Texture : public CSG_Module_Grid
{
public:
	CTC_Texture(void);


protected:

	virtual bool			On_Execute		(void);


private:

	CSG_Grid_Cell_Addressor	m_Kernel;

	CSG_Grid				*m_pDEM, *m_pTexture, *m_pNoise;


	int						Get_MedianDiff	(int x, int y);

	int						Get_Texture		(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTC_Convexity : public CSG_Module_Grid
{
public:
	CTC_Convexity(void);


protected:

	virtual bool			On_Execute		(void);


private:

	int						m_dLaplace[2];

	CSG_Grid_Cell_Addressor	m_Kernel;

	CSG_Grid				*m_pDEM, *m_pConvexity, *m_pLaplace;


	int						Get_Laplace		(int x, int y);

	int						Get_Convexity	(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTC_Classification : public CSG_Module_Grid
{
public:
	CTC_Classification(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("A:Terrain Analysis|Terrain Classification" ));	}


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	CSG_Simple_Statistics	stat_Slope, stat_Convexity, stat_Texture;

	double					m_Mean_Slope, m_Mean_Convexity, m_Mean_Texture;

	CSG_Grid				*m_pSlope, *m_pConvexity, *m_pTexture, *m_pLandforms;


	void					Set_LUT					(int nLevels);

	bool					Get_Classes				(void);

	void					Get_Class				(int Level, int x, int y, bool bLastLevel);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__tc_iwahashi_pike_H
