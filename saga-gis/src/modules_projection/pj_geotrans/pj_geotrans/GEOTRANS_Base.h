/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Projection_GeoTRANS                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    GEOTRANS_Base.h                    //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#ifndef HEADER_INCLUDED__GEOTRANS_Base_H
#define HEADER_INCLUDED__GEOTRANS_Base_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include "geotrans/geotrans.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class pj_geotrans_EXPORT CGEOTRANS_Base : public CSG_Module  
{
public:
	CGEOTRANS_Base(void);
	virtual ~CGEOTRANS_Base(void);

	bool				Initialize					(void);

	virtual bool		do_Sync_Projections			(void)	{	return( false  );	}


protected:

	virtual bool		On_Execute					(void);
	virtual bool		On_Execute_Conversion		(void)	= 0;

	bool				Set_Transformation			(bool bShow_Dialog);
	bool				Set_Transformation_Inverse	(void);

	bool				Get_Converted				(double &x, double &y, double &z);
	bool				Get_Converted				(double &x, double &y);
	bool				Get_Converted				(TSG_Point &Point);


private:

	bool				bInitialized;

	char				flag_Hemisphere;

	int					UTM_Zone;

	Coordinate_Type		Type_Input, Type_Output;

	CSG_String			fn_Ellipsoid, fn_Datum3, fn_Datum7, fn_Geoid;

	CSG_Parameters		*pProjection;


	CSG_Parameters *	Get_Projection_Parameters	(bool bSource, Coordinate_Type Type, bool bShow_Dialog);

	bool				Set_Projection_Parameters	(Input_Output dir, Coordinate_Type Type, CSG_Parameters *pParms);

	bool				Convert_Set					(double  x, double  y, double  z);
	bool				Convert_Set					(double  x, double  y);

	bool				Convert_Get					(double &x, double &y, double &z);
	bool				Convert_Get					(double &x, double &y);

};

#endif // #ifndef HEADER_INCLUDED__GEOTRANS_Base_H
