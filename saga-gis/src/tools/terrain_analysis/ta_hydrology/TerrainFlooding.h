
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TerrainFlooding.h                   //
//                                                       //
//                 Copyright (C) 2022 by                 //
//                    Volker Wichmann                    //
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
#ifndef HEADER_INCLUDED__TerrainFlooding_H
#define HEADER_INCLUDED__TerrainFlooding_H


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
class CTerrainFloodingBase
{
protected:

	double				m_dWaterLevel;


	bool				Create				(CSG_Parameters &Parameters, bool bInteractive);

	bool				Initialize			(const CSG_Parameters &Parameters);
	bool				Finalize			(const CSG_Parameters &Parameters);

	bool				Set_Flooding		(double xWorld, double yWorld, double dWaterLevel, bool bShow, bool bReset = false);


private:

	bool				m_bConstantLevel;

	int					m_iLevelReference;

	CSG_Grid			*m_pDEM, *m_pWaterBody, *m_pFlooded, m_Flooded;
	
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTerrainFlooding : public CSG_Tool_Grid, CTerrainFloodingBase
{
public:
	CTerrainFlooding(void);

	virtual CSG_String	Get_MenuPath		(void)	{	return( _TL("Miscellaneous") );	}


protected:

	virtual int			On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool		On_Execute			(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTerrainFloodingInteractive : public CSG_Tool_Grid_Interactive, CTerrainFloodingBase
{
public:

	CTerrainFloodingInteractive(void);

	virtual CSG_String	Get_MenuPath		(void)	{	return( _TL("Miscellaneous") );	}


protected:

	virtual int			On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool		On_Execute			(void);
	virtual bool		On_Execute_Finish	(void);
	virtual bool		On_Execute_Position	(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__TerrainFlooding_H
