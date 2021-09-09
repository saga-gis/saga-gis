
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_hydrology                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    overland_flow.h                    //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#ifndef HEADER_INCLUDED__overland_flow_H
#define HEADER_INCLUDED__overland_flow_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COverland_Flow : public CSG_Tool_Grid
{
public:
	COverland_Flow(void);


protected:

	virtual int				On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	bool					m_bStrickler, m_bFlow_Out;

	double					m_dTime, m_vMax, m_vMin, m_Flow_Out;

	CSG_Grid				*m_pDEM, m_Flow, *m_pFlow, *m_pVelocity, *m_pIntercept, *m_pPonding, *m_pInfiltrat;

	double					  m_Roughness,   m_Precipitation,   m_ETpot,   m_Intercept_max,   m_Ponding_max,   m_Infiltrat_max;
	CSG_Grid				*m_pRoughness, *m_pPrecipitation, *m_pETpot, *m_pIntercept_max, *m_pPonding_max, *m_pInfiltrat_max;

	CSG_Grids				m_v;


	bool					Initialize				(void);
	bool					Finalize				(void);

	bool					Do_Updates				(void);

	bool					Set_Time_Stamp			(double Time);

	bool					Do_Time_Step			(void);

	double					Get_Precipitation		(int x, int y);
	double					Get_ETpot				(int x, int y);

	double					Get_Roughness			(int x, int y);
	double					Get_Intercept_max		(int x, int y);
	double					Get_Ponding				(int x, int y);
	double					Get_Infiltration		(int x, int y);

	double					Get_Surface				(int x, int y);
	bool					Get_Neighbour			(int x, int y, int i, int &ix, int &iy);
	double					Get_Slope				(int x, int y, int i);

	double					Get_Velocity			(double Flow, double Slope, double Roughness);
	bool					Get_Velocity			(int x, int y);

	double					Get_Flow_Lateral		(int x, int y, int i, bool bInverse);
	bool					Set_Flow_Lateral		(int x, int y);

	bool					Set_Flow_Vertical		(int x, int y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__overland_flow_H
