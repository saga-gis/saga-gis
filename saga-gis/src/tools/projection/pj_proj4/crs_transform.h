
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

	bool					Create						(const CSG_CRSProjector &Projector);
	bool					Destroy						(void);

	bool					Set_Copies					(int nCopies = 0);
	CSG_CRSProjector &		operator []					(int iCopy) { return( iCopy > 0 && iCopy <= m_nCopies ? m_Copies[iCopy - 1] : *this ); }

	static CSG_String		Get_Version					(void);
	static CSG_String		Get_Description				(void);

	enum class TCRS_Format {
		PROJ, JSON, ESRI, WKT1, WKT2, WKT2015, WKT2018, WKT2019
	};

	static CSG_String		Convert_CRS_To_PROJ			(const CSG_String &Definition);
	static CSG_String		Convert_CRS_To_WKT1			(const CSG_String &Definition, bool bMultiLine);
	static CSG_String		Convert_CRS_To_WKT2			(const CSG_String &Definition, bool bMultiLine, bool bSimplified);
	static CSG_String		Convert_CRS_To_JSON			(const CSG_String &Definition, bool bMultiLine);
	static CSG_String		Convert_CRS_To_ESRI			(const CSG_String &Definition);
	static CSG_String		Convert_CRS_Format			(const CSG_String &Definition, TCRS_Format Format, bool bMultiLine = true, bool bSimplified = true);
	static bool				Convert_CRS_Format			(const CSG_String &Definition, CSG_String *PROJ, CSG_String *WKT1, CSG_String *WKT2, CSG_String *JSON, CSG_String *ESRI, bool bMultiLine = true, bool bSimplified = true);


	bool					Set_Source					(const CSG_Projection &Projection, bool bSetTransformation = false);
	const CSG_Projection &	Get_Source					(void) const { return( m_Source ); }

	bool					Set_Target					(const CSG_Projection &Projection, bool bSetTransformation = false);
	const CSG_Projection &	Get_Target					(void) const { return( m_Target ); }

	bool					Set_Transformation			(void);
	bool					Set_Transformation			(const CSG_Projection &Source, const CSG_Projection &Target);

	bool					Set_Forward					(bool bOn = true);
	bool					Get_Forward					(void) const { return( !m_bInverse ); }
	bool					Set_Inverse					(bool bOn = true);
	bool					Get_Inverse					(void) const { return(  m_bInverse ); }
	bool					Has_Inverse					(void) const;

	bool					Get_Projection				(double &x, double &y           ) const;
	bool					Get_Projection				(double &x, double &y, double &z) const;
	bool					Get_Projection				(TSG_Point    &Point)             const;
	bool					Get_Projection				(CSG_Point    &Point)             const;
	bool					Get_Projection				(TSG_Point_3D &Point)             const;
	bool					Get_Projection				(CSG_Point_3D &Point)             const;


private:

	bool					m_bInverse = false;

	void					*m_pContext = NULL, *m_pSource = NULL, *m_pTarget = NULL;

	CSG_Projection			m_Source, m_Target;

	int						m_nCopies = 0;

	CSG_CRSProjector		*m_Copies = NULL;


	void					_On_Construction			(void);

	bool					_Set_Projection				(const CSG_Projection &Projection, void **ppProjection) const;


//	void					*m_pTransformation = NULL;
//	bool					_Set_Transformation			(void                *pSource, void                *pTarget, void **ppTransformation) const;
//	bool					_Set_Transformation			(const CSG_Projection &Source, const CSG_Projection &Target, void **ppTransformation) const;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__crs_transform_H
