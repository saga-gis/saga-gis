
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    sgdi_classify.h                    //
//                                                       //
//                 Copyright (C) 2025 by                 //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GDI__sgdi_classify_H
#define _HEADER_INCLUDED__SAGA_GDI__sgdi_classify_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgdi_core.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SGDI_API_DLL_EXPORT CSGDI_Classify
{
public:

	enum class Classifier
	{
		Unique = 0, Equal, Defined, Quantile, Geometric, Natural, StdDev
	};

	enum class LUT_Fields
	{
		Color = 0, Name, Description, Minimum, Maximum
	};

	//-----------------------------------------------------
	CSGDI_Classify(void);
	virtual ~CSGDI_Classify(void);

								CSGDI_Classify			(CSG_Data_Object *pObject);
	bool						Create					(CSG_Data_Object *pObject);

								CSGDI_Classify			(CSG_Table *pTable, int Field, int Normalize = -1);
	bool						Create					(CSG_Table *pTable, int Field, int Normalize = -1);

	bool						Destroy					(void);

	//-----------------------------------------------------
	const CSG_String &			Get_Name				(Classifier Classifier) const;

	bool						is_Okay					(void) const;

	bool						Set_LUT					(CSG_Table &LUT, CSG_Colors Colors) const;

	bool						Classify_Unique			(int maxCount = 1024);
	bool						Classify_Equal			(int Count);
	bool						Classify_Defined		(double Interval);
	bool						Classify_Quantile		(int Count, bool bHistogram = true);
	bool						Classify_Geometric		(int Count, bool bIncreasing = true);
	bool						Classify_Natural		(int Count);
	bool						Classify_StdDev			(double StdDev);

	bool						m_Show_Progress = false;


private:

	int							m_Field = -1;

	sLong						m_nValues = 0;

	CSG_Data_Object				*m_pObject = NULL;

	CSG_Table					m_Normalized, m_Classes;


	bool						_is_Numeric				(void) const;
	bool						_Create_Classes			(void);

	bool						_Set_Progress			(sLong i) const;

	bool						_Get_Value				(sLong i, double     &Value, bool bIndexed = false) const;
	bool						_Get_Value				(sLong i, CSG_String &Value, bool bIndexed = false) const;

	bool						_Get_Statistics			(CSG_Simple_Statistics &Statistics) const;
	bool						_Get_Histogram			(CSG_Histogram         &Histogram ) const;
	bool						_Set_Index				(void);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GDI__sgdi_classify_H
