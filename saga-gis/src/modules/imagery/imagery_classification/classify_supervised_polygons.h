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
//                imagery_classification                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             classify_supervised_polygons.h            //
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
//                University of Hamburg                  //
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
#ifndef HEADER_INCLUDED__classify_supervised_polygons_H
#define HEADER_INCLUDED__classify_supervised_polygons_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPolygon_Classify_Supervised : public CSG_Tool
{
public:
	CPolygon_Classify_Supervised(bool bShapes);

	virtual CSG_String			Get_MenuPath			(void)	{	return( m_bShapes ? _TL("A:Shapes|Table") : _TL("A:Table|Tools") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	bool						m_bShapes, m_bNormalise;

	int							*m_Features, m_nFeatures;

	CSG_Table					*m_pTable;


	bool						Get_Features			(void);
	bool						Get_Features			(int iRecord, CSG_Vector &Features);

	bool						Set_Classifier			(CSG_Classifier_Supervised &Classifier);
	bool						Set_Classifier			(CSG_Classifier_Supervised &Classifier, int Training);

	bool						Set_Classification		(CSG_Classifier_Supervised &Classifier);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__classify_supervised_polygons_H
