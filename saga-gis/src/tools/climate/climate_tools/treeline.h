
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
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ETGS_Climate
{
	PARM_T	= 0,
	PARM_TMIN,
	PARM_TMAX,
	PARM_P,
	PARM_CLDS
};

//---------------------------------------------------------
#define TGS_COLD	0x01
#define TGS_SNOW	0x02
#define TGS_DRY		0x04

//---------------------------------------------------------
class CTreeGrowingSeason
{
public:

	//-----------------------------------------------------
	typedef struct TGS_Parameters
	{
		int		SW_MaxIter;

		double	DT_min, SMT_min, LGS_min, SWC_Surface, SW_min, SW_Resist;
	}
	TTGS_Parameters;


public:
	CTreeGrowingSeason(const TTGS_Parameters &Parms, double TSWC, double Latitude);

	double *					Get_Monthly				(int Parameter)	{	return( m_Monthly[Parameter] );	}

	bool						Calculate				(void);

	double						Get_SMT					(void)	{	return(      m_TGS.Get_Mean () );	}
	int							Get_LGS					(void)	{	return( (int)m_TGS.Get_Count() );	}

	double						Get_T					(int iDay)	{	return( m_Daily[PARM_T][iDay] );	}
	double						Get_P					(int iDay)	{	return( m_Daily[PARM_P][iDay] );	}
	double						Get_Snow				(int iDay)	{	return( m_Snow         [iDay] );	}
	double						Get_ET					(int iDay)	{	return( m_ET           [iDay] );	}
	double						Get_SW_0				(int iDay)	{	return( m_SW[0]        [iDay] );	}
	double						Get_SW_1				(int iDay)	{	return( m_SW[1]        [iDay] );	}


private:

	int							m_Growing[365];

	double						m_Monthly[4][12], m_SWC[2], m_Lat;

	TTGS_Parameters				m_Parms;

	CSG_Vector					m_Daily[4], m_Snow, m_ET, m_SW[2];

	CSG_Simple_Statistics		m_TGS;


	void						Reset					(void);

	bool						Get_TGS					(void);

	void						Set_Daily_Spline		(CSG_Vector &Daily  , const double *Monthly);
	void						Set_Daily_Events		(CSG_Vector &Daily_P, const double *Monthly_P, const double *Monthly_T);

	bool						Set_Temperature			(void);

	bool						Set_Snow_Depth			(void);
	int							Snow_Get_Start			(const double *T);
	double						Snow_Get_SnowMelt		(double Snow, double T, double P);

	bool						Set_Soil_Water			(void);
	int							Soil_Get_Start			(const double *P, const double *ET);
	void						Soil_Set_ET				(double *ET);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTreeLine_Base
{
public:
	CTreeLine_Base(void)	{}


protected:

	double						m_TSWC, m_Lat_Default;

	CSG_Grid					*m_pTSWC, m_Lat, *m_pLat;

	CSG_Parameter_Grid_List		*m_pT, *m_pTmin, *m_pTmax, *m_pP;
	
	CTreeGrowingSeason::TTGS_Parameters	m_Parms;


	void						Add_Parameters			(CSG_Parameters &Parameters);
	bool						Set_Parameters			(CSG_Parameters &Parameters);

	bool						Get_Monthly				(double Monthly[12], int x, int y, CSG_Parameter_Grid_List *pMonthly, double Default = -1.0);;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTreeLine : public CSG_Tool_Grid, CTreeLine_Base
{
public:
	CTreeLine(void);

//	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTreeLine_Interactive : public CSG_Tool_Grid_Interactive, CTreeLine_Base
{
public:
	CTreeLine_Interactive(void);

//	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	CSG_Table					*m_pSummary, *m_pDaily;


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
