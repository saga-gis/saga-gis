/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      io_virtual                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  pc_get_grid_spcvf.h                  //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    LASERDATA GmbH                         //
//                Management and Analysis of             //
//                Laserscanning Data                     //
//                Technikerstr. 21a                      //
//                6020 Innsbruck                         //
//                Austria                                //
//                www.laserdata.at                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__pc_get_grid_spcvf_H
#define HEADER_INCLUDED__pc_get_grid_spcvf_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPointCloud_Get_Grid_SPCVF_Base
{
public:
	CPointCloud_Get_Grid_SPCVF_Base(void);
	virtual ~CPointCloud_Get_Grid_SPCVF_Base(void);

	void					Initialise	(int iOutputs, CSG_Rect	AOI, CSG_Shapes *pShapes, int iFieldName, bool bMultiple, bool bAddOverlap, double dOverlap,
										 CSG_String sFilename, CSG_Parameter_File_Name *pFilePath, CSG_Parameter_Grid_List *pGridList, double dCellsize,
										 bool bFitToCells, int iMethod, bool bConstrain, int iField, double dMinAttrRange, double dMaxAttrRange);
	void					Finalise	(void);
	bool					Get_Subset	(int iFieldToGrid);
	void					Write_Subset(CSG_Grid *pGrid, int iAOI, int iDatasets, double dPoints);

protected:


private:

	int						m_iOutputs;
	CSG_Rect				m_AOI;
	CSG_Shapes				*m_pShapes;
	int						m_iFieldName;
	bool					m_bMultiple, m_bAddOverlap;
	double					m_dOverlap;
	CSG_String				m_sFileName;
	CSG_Parameter_File_Name	*m_pFilePath;
	CSG_Parameter_Grid_List	*m_pGridList;
	double					m_dCellsize;
	bool					m_bFitToCells;
	int						m_iMethod;
	bool					m_bConstrain;
	int						m_iField;
	double					m_dMinAttrRange, m_dMaxAttrRange;

};


//---------------------------------------------------------
class CPointCloud_Get_Grid_SPCVF : public CSG_Tool
{
public:
	CPointCloud_Get_Grid_SPCVF(void);

	virtual CSG_String			Get_MenuPath		(void)	{	return( _TL("Point Cloud") );	}


protected:

	virtual bool				On_Execute			(void);

	virtual int					On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

	CPointCloud_Get_Grid_SPCVF_Base		m_Get_Grid_SPCVF;

};


//---------------------------------------------------------
class CPointCloud_Get_Grid_SPCVF_Interactive : public CSG_Tool_Interactive
{
public:
	CPointCloud_Get_Grid_SPCVF_Interactive(void);

	virtual CSG_String			Get_MenuPath		(void)	{	return( _TL("Point Cloud") );	}


protected:

	virtual bool				On_Execute			(void);
	virtual bool				On_Execute_Position	(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);

	virtual int					On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

	CSG_Point					m_ptDown;

	CPointCloud_Get_Grid_SPCVF_Base		m_Get_Grid_SPCVF;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__pc_get_grid_spcvf_H
