
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  clouds_and_shadows.h                 //
//                                                       //
//                  Copyrights (c) 2023                  //
//                  Justus Spitzmüller                   //
//                     Olaf Conrad                       //
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
#ifndef HEADER_INCLUDED__clouds_and_shadows_H
#define HEADER_INCLUDED__clouds_and_shadows_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>
#include <bitset>
#include <functional>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////
//typedef enum SpectralBand
//{
//	RED = 0, GREEN, BLUE, NIR, SWIR1, SWIR2, TIR, QARAD
//} SpectralBand;

//---------------------------------------------------------
class CDetect_Clouds : public CSG_Tool
{
public:
	CDetect_Clouds(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Imagery|Feature Extraction") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	bool						m_bCelsius { false };

	CSG_Grid					*m_pBand[8];
	
	CSG_Grid					*m_pResults[6];

	CSG_Grid_System 			m_pSystem;


	bool						Get_Brightness			(int x, int y, int Band, double &Value);
	double 						Get_Brightness			(int x, int y, int Band, bool &Eval );
	bool 						Is_Saturated			(int x, int y, int Band );


	int							Get_Fmask				(int x, int y);
	bool						Set_Fmask				(CSG_Grid *pClouds);
	bool						Set_Fmask_Pass_One_Two	();

	bool						Set_ACCA				(CSG_Grid *pClouds);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDetect_CloudShadows : public CSG_Tool_Grid
{
public:
	CDetect_CloudShadows(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Imagery|Feature Extraction") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	CSG_Table					*m_pInfo { NULL };

	CSG_Grid					m_Correction;


	CSG_Grid *					Get_Target				(void);

	bool						Get_Candidates			(CSG_Grid &Candidates);

	double						Get_Correction			(double dx, double dy, double dz, CSG_Grid *pDEM, int x, int y);
	bool						Get_Correction			(double dx, double dy, double dz);
	bool						Get_Correction			(double dx, double dy, int &x, int &y);

	bool						Get_Cloud				(CSG_Grid_Stack &Cloud, const CSG_Grid *pClouds);
	bool						Get_Cloud				(CSG_Grid_Stack &Cloud,       CSG_Grid *pClouds, int x, int y, CSG_Grid *pID);

	bool						Find_Shadow				(CSG_Grid *pShadows, CSG_Grid &Candidates, const CSG_Grid_Stack &Cloud, double zRange[2], double dx, double dy, double dz);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__clouds_and_shadows_H
