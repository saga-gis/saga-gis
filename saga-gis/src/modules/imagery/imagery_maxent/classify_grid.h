/**********************************************************
 * Version $Id: classify_grid.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        maxent                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    classify_grid.h                    //
//                                                       //
//                 Copyright (C) 2015 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#ifndef HEADER_INCLUDED__classify_grid_H
#define HEADER_INCLUDED__classify_grid_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
#include "maxent.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CClassify_Grid : public CSG_Tool_Grid
{
public:
	CClassify_Grid(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Imagery|Classification") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	typedef struct
	{
		bool					bNumeric;

		char					Name[256];

		CSG_Grid				*pGrid;
	}
	TFeature;

	bool						m_bYT_Weights;

	int							m_Method, m_nNumClasses, m_nFeatures;

	TFeature					*m_Features;

	CSG_Parameter_Grid_List		*m_pProbs;

	ME_Model					m_YT_Model;

	class EventSet				*m_DL_Events;

	class GISTrainer			*m_DL_Trainer;

	class MaxEntModel			*m_DL_Model;


	CSG_String					Get_Feature		(int x, int y, int i);
	bool						Get_Features	(CSG_Array &Features);

	bool						Get_Training	(void);
	void						Get_Training	(const CSG_String &ID, CSG_Shape_Polygon *pArea);

	bool						Get_File		(const CSG_String &File);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__classify_grid_H
