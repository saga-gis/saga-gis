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
class CPolygon_Classify_Supervised : public CSG_Module
{
public:
	CPolygon_Classify_Supervised(void);


protected:

	virtual bool				On_Execute				(void);

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

	bool						m_bNormalise;

	int							m_Class_ID, *m_Features, m_nFeatures;

	CSG_Classifier_Supervised	m_Classifier;

	CSG_Shapes					*m_pPolygons, *m_pClasses;


	bool						Finalize				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__classify_supervised_polygons_H
