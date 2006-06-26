
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     PROJ4_Base.h                      //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#ifndef HEADER_INCLUDED__PROJ4_Base_H
#define HEADER_INCLUDED__PROJ4_Base_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <projects.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class pj_proj4_EXPORT CPROJ4_Base : public CModule  
{
public:
	CPROJ4_Base(void);
	virtual ~CPROJ4_Base(void);

	bool				Initialize					(void);


protected:

	virtual bool		On_Execute					(void);
	virtual bool		On_Execute_Conversion		(void)	= 0;

	bool				Set_Transformation			(bool bHistory);
	bool				Set_Transformation_Inverse	(void);

	bool				Get_Converted				(double &x, double &y);
	bool				Get_Converted				(TGEO_Point &Point);


private:

	bool				bInitialized, m_Inverse, m_Reverse;

	PJ					*m_Projection;


	bool				Initialize_ExtraParms		(struct PJ_LIST *pProjection, const char *sName);

	bool				Get_ExtraParms				(int &pargc, char ***p_pargv, char *id);

};

#endif // #ifndef HEADER_INCLUDED__PROJ4_Base_H
