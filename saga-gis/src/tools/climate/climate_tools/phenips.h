
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       ips-pro                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      phenips.h                        //
//                                                       //
//                 Copyrights (C) 2019                   //
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
#ifndef HEADER_INCLUDED__phenips_H
#define HEADER_INCLUDED__phenips_H


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
#define NGENERATIONS	3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPhenIps_Table : public CSG_Tool
{
public:
	CPhenIps_Table(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Bioclimatology|Phenology") );	}


protected:

	virtual bool			On_Execute				(void);


};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPhenIps_Grids : public CSG_Tool_Grid
{
public:
	CPhenIps_Grids(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Bioclimatology|Phenology") );	}


protected:

	CSG_Grid				*m_pOnset, *m_pGenerations, *m_pOnsets[NGENERATIONS][2], *m_pStates[NGENERATIONS][2];

	CSG_Parameter_Grid_List	*m_pATmean, *m_pATmax, *m_pSIrel;


	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool					Initialize				(bool bReset);
	bool					Finalize				(void);

	double					Get_DayLength			(int x, int y, int Day);

	bool					Set_NoData				(int x, int y);
	bool					Set_Values				(int x, int y, const class CPhenIps &PhenIps);


private:

	double					m_Lat_const;

	CSG_Grid				m_Lat_Grid, *m_pLat_Grid;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPhenIps_Grids_Annual : public CPhenIps_Grids
{
public:
	CPhenIps_Grids_Annual	(void);


protected:

	virtual bool			On_Execute				(void);


private:

	bool					Get_Daily				(int x, int y, CSG_Parameter_Grid_List *pValues, CSG_Vector &Values);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPhenIps_Grids_Days : public CPhenIps_Grids
{
public:
	CPhenIps_Grids_Days(void);


protected:

	virtual bool			On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__phenips_H
