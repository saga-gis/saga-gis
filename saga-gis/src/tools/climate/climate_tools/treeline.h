
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      treeline.h                       //
//                                                       //
//                 Copyright (C) 2016 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version >=2 of the License. //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__treeline_H
#define HEADER_INCLUDED__treeline_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "climate_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCT_Growing_Season : public CCT_Water_Balance
{
public:
	CCT_Growing_Season(void);
	CCT_Growing_Season(const CCT_Growing_Season &Copy);

	bool						Set_DT_min				(double Value);
	double						Get_DT_min				(void)	const	{	return( m_DT_min  );	}

	bool						Set_LGS_min				(int    Value);
	int							Get_LGS_min				(void)	const	{	return( m_LGS_min );	}

	bool						Set_SMT_min				(double Value);
	double						Get_SMT_min				(void)	const	{	return( m_SMT_min );	}

	bool						Set_SW_min				(double Value);
	double						Get_SW_min				(void)	const	{	return( m_SW_min  );	}

	virtual bool				Calculate				(double TSWC, double Latitude);
	virtual bool				Calculate				(double TSWC, double Latitude, double &Height, double maxDiff = 1000.0);

	int							Get_LGS					(void)		{	return( (int)m_T_Season.Get_Count() );	}
	double						Get_SMT					(void)		{	return(      m_T_Season.Get_Mean () );	}


private:

	int							m_LGS_min;

	double						m_DT_min, m_SMT_min, m_SW_min;

	CSG_Simple_Statistics		m_T_Season;


	bool						is_Growing				(double SWC, double Latitude, double Height);

	bool						Get_T_Season			(const double *T, const double *Snow = NULL, const double *S0 = NULL, const double *S1 = NULL);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTree_Growth : public CSG_Tool_Grid
{
public:
	CTree_Growth(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Bioclimatology") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	CCT_Growing_Season			m_Model;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWater_Balance_Interactive : public CSG_Tool_Grid_Interactive
{
public:
	CWater_Balance_Interactive(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Soils") );	}


protected:

	double						m_SWC_Def, m_SWC_Surface, m_Lat_Def;

	CSG_Grid					*m_pSWC, m_Lat, *m_pLat;

	CSG_Parameter_Grid_List		*m_pT, *m_pTmin, *m_pTmax, *m_pP;

	CSG_Table					*m_pSummary, *m_pDaily;

	CCT_Water_Balance			m_Model;


	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);
	virtual bool				On_Execute_Finish		(void);
	virtual bool				On_Execute_Position		(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__treeline_H
