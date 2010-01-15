
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Discretisation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_Classify_Supervised.h              //
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
#ifndef HEADER_INCLUDED__Grid_Classify_Supervised_H
#define HEADER_INCLUDED__Grid_Classify_Supervised_H


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
class CClass_Info
{
public:
	CClass_Info(void);
	virtual ~CClass_Info(void);

	void						Create						(int nFeatures);
	void						Destroy						(void);

	int							Get_Feature_Count			(void)		{	return( m_nFeatures );			}

	int							Get_Count					(void)		{	return( m_IDs.Get_Count() );	}
	const CSG_String &			Get_ID						(int Index)	{	return( m_IDs[Index] );			}

	CSG_Simple_Statistics *		Get_Statistics				(const CSG_String &ID);
	CSG_Simple_Statistics *		Get_Statistics				(int Index)	{	return( m_Statistics[Index] );	}
	CSG_Simple_Statistics *		operator []					(int Index)	{	return( m_Statistics[Index] );	}

	int							Get_Element_Count			(int Index)	{	return( m_nElements[Index] );	}
	void						Inc_Element_Count			(int Index)	{	m_nElements[Index]++;			}


private:

	int							m_nFeatures, *m_nElements;

	CSG_Strings					m_IDs;

	CSG_Simple_Statistics		**m_Statistics;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Classify_Supervised : public CSG_Module_Grid
{
public:
	CGrid_Classify_Supervised(void);

	virtual const SG_Char *		Get_MenuPath				(void)	{	return( _TL("R:Classification") );	}


protected:

	virtual bool				On_Execute					(void);


private:

	bool						m_bNormalise;

	CClass_Info					m_Class_Info;

	CSG_String					m_Name_Method, m_Name_Quality;

	CSG_Grid					*m_pClasses, *m_pQuality;

	CSG_Parameter_Grid_List		*m_pGrids;


	bool						Initialise					(void);
	bool						Finalise					(void);

	double						Get_Value					(int x, int y, int iGrid);
	bool						Set_Class					(int x, int y, int iClass, double Quality);

	bool						Set_Parallel_Epiped			(void);
	bool						Set_Minimum_Distance		(void);
	bool						Set_Mahalanobis_Distance	(void);
	bool						Set_Maximum_Likelihood		(void);
	bool						Set_Spectral_Angle_Mapping	(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Grid_Classify_Supervised_H
