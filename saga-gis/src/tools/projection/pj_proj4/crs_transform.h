
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    crs_transform.h                    //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#ifndef HEADER_INCLUDED__crs_transform_H
#define HEADER_INCLUDED__crs_transform_H


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
class CSG_CRSProjector
{
public:
	CSG_CRSProjector(void);
	CSG_CRSProjector(const CSG_CRSProjector &Projector);
	virtual ~CSG_CRSProjector(void);

	bool					Destroy						(void);

	static CSG_String		Get_Version					(void);
	static CSG_String		Get_Description				(void);

	bool					Set_Source					(const CSG_Projection &Projection);
	const CSG_Projection &	Get_Source					(void)	const		{	return( m_Source );	}

	bool					Set_Target					(const CSG_Projection &Projection);
	const CSG_Projection &	Get_Target					(void)	const		{	return( m_Target );	}

	bool					Set_Inverse					(bool bOn = true);
	bool					Get_Inverse					(void)	const		{	return( m_bInverse );	}

	bool					Set_Precise_Mode			(bool bOn = true);
	bool					Get_Precise_Mode			(void)	const		{	return( m_pGCS != NULL );	}

	bool					Get_Projection				(double &x, double &y)	const;
	bool					Get_Projection				(TSG_Point &Point)		const;
	bool					Get_Projection				(CSG_Point &Point)		const;

	bool					Get_Projection				(double &x, double &y, double &z)	const;
	bool					Get_Projection				(TSG_Point_Z &Point)				const;
	bool					Get_Projection				(CSG_Point_Z &Point)				const;


private:

	bool					m_bInverse;

	void					*m_pContext, *m_pSource, *m_pTarget, *m_pGCS;

	CSG_Projection			m_Source, m_Target;


	void					_On_Construction			(void);

	bool					_Set_Projection				(const CSG_Projection &Projection, void **ppProjection, bool bInverse);


};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__crs_transform_H
