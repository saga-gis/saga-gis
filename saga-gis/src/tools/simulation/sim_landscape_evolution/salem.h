
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//               sim_landscape_evolution                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       salem.h                         //
//                                                       //
//              Michael Bock, Olaf Conrad                //
//                      (C) 2017                         //
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
#ifndef HEADER_INCLUDED__salem_H
#define HEADER_INCLUDED__salem_H


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
class CSaLEM_Climate
{
public:
	CSaLEM_Climate(void);

	void					Destroy					(void);

	bool					Add_Parameters			(CSG_Parameters &Parameters, const CSG_String &Parent = "");
	bool					Set_Parameters			(CSG_Parameters &Parameters);

	bool					Set_Year				(int YearBP);
	bool					Set_Month				(int Month);

	double					Get_T					(void)	const	{	return( m_T             );	}
	double					Get_Tmin				(void)	const	{	return( m_Tmin          );	}
	double					Get_Tmax				(void)	const	{	return( m_Tmax          );	}
	double					Get_Tamp				(void)	const	{	return( m_Tmax - m_Tmin );	}
	double					Get_P					(void)	const	{	return( m_P             );	}

	double					Get_TLapse				(void)	const	{	return( m_TLapse        );	}
	bool					Get_TLapse_Cellwise		(void)	const	{	return( m_TLapse_bCell  );	}


private:

	bool					m_TLapse_bCell;

	int						m_Scenario, m_fTrend_Year, m_fTrend_T, m_fAnnual_T, m_fAnnual_Tmin, m_fAnnual_Tmax, m_fAnnual_P;

	double					m_T, m_Tmin, m_Tmax, m_P, m_Pmax, m_TTrend, m_T_Offset, m_TLapse;


	CSG_Table				*m_pTrend, *m_pAnnual;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSaLEM_Bedrock
{
public:
	CSaLEM_Bedrock(void);

	void					Destroy					(void);

	bool					Add_Parameters			(CSG_Parameters &Parameters, const CSG_String &Parent = "");
	bool					Set_Parameters			(CSG_Parameters &Parameters);

	int						Get_Bedrock_Index		(int x, int y, double z)	const;
	CSG_String				Get_Bedrock_Name		(int x, int y, double z)	const;

	bool					Set_Weathering			(double dTime, CSaLEM_Climate &Climate, CSG_Grid &Surface, CSG_Grid &Slope, CSG_Grid &Regolith);


private:

	CSG_Formula				*m_Weathering[2];

	CSG_Parameter_Grid_List	*m_pRocks;


	bool					Get_Weathering_Formulas	(CSG_Parameters &Parameters);
	CSG_String				Get_Weathering_Formula	(const CSG_String &Formula, const CSG_String &Default);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSaLEM_Tracers
{
public:
	CSaLEM_Tracers(void);

	void					Destroy					(void);

	bool					Add_Parameters			(CSG_Parameters &Parameters, const CSG_String &Parent = "");
	bool					Set_Parameters			(CSG_Parameters &Parameters, CSG_Grid *pSurface);
	int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	int						Add_Tracers				(double Time          , CSG_Grid &Surface, CSG_Grid &Regolith, const CSaLEM_Bedrock &Bedrock);
	bool					Set_Tracers				(double Time, double k, CSG_Grid &Surface, CSG_Grid Gradient[3], CSG_Grid &dHin, CSG_Grid &dHout);

	CSG_Shapes *			Get_Tracers				(void)	{	return( m_pPoints );	}
	CSG_Shapes *			Get_Paths				(void)	{	return( m_pLines  );	}


private:

	bool					m_hRandom, m_vRandom;

	int						m_hDensity, m_Trim;

	double					m_vDensity, m_dDirection;

	CSG_Grid				m_Bedrock;

	CSG_Shapes				m_Candidates, *m_pPoints, *m_pLines, m_Trim_Points, m_Trim_Lines;


	bool					Do_Move					(double Depth, double dActive);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSaLEM : public CSG_Tool_Grid
{
public:
	CSaLEM(void);


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	int						m_Time, m_dTime;

	CSG_Grid				*m_pSurface, *m_pRegolith, m_Gradient[3];

	CSaLEM_Climate			m_Climate;

	CSaLEM_Bedrock			m_Bedrock;

	CSaLEM_Tracers			m_Tracers;


	bool					Initialize				(void);
	bool					Finalize				(void);

	bool					Set_Gradient			(void);
	bool					Set_Allochthone			(void);
	bool					Set_Weathering			(void);
	bool					Set_Diffusive			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__salem_H

