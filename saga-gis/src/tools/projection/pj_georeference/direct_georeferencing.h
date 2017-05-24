/**********************************************************
 * Version $Id: direct_georeferencing.h 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    georeferencing                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 direct_georeferencing.h               //
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
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__direct_georeferencing_H
#define HEADER_INCLUDED__direct_georeferencing_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Direct_Georeferencer
{
public:
	CSG_Direct_Georeferencer(void);

	bool						Add_Parameters			(CSG_Parameters &Parameters);

	bool						Set_Transformation		(CSG_Parameters &Parameters, int nCols, int nRows);
	const CSG_Matrix &			Get_Transformation		(void)	const	{	return( m_R );	}

	TSG_Point					World_to_Image			(double x_w, double y_w, double z_w = 0.0);
	TSG_Point					Image_to_World			(double x_i, double y_i, double z_w = 0.0);


private:

	double						m_f, m_s;

	CSG_Vector					m_T, m_O;

	CSG_Matrix					m_R, m_Rinv;

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDirect_Georeferencing : public CSG_Tool_Grid
{
public:
	CDirect_Georeferencing(void);


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	CSG_Parameters_Grid_Target	m_Grid_Target;

	CSG_Direct_Georeferencer	m_Georeferencer;

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDirect_Georeferencing_WorldFile : public CSG_Tool
{
public:
	CDirect_Georeferencing_WorldFile(void);


protected:

	virtual bool				On_Execute				(void);


private:

	CSG_Direct_Georeferencer	m_Georeferencer;

};


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__direct_georeferencing_H
