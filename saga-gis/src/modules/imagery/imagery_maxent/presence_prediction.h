/**********************************************************
 * Version $Id: presence_prediction.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        maxent                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 presence_prediction.h                 //
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
#ifndef HEADER_INCLUDED__presence_prediction_H
#define HEADER_INCLUDED__presence_prediction_H


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
class CPresence_Prediction : public CSG_Module_Grid
{
public:
	CPresence_Prediction(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Grid|Analysis|Macroecology") );	}


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

	ME_Model					m_YT_Model;

	class EventSet				*m_DL_Events;

	class GISTrainer			*m_DL_Trainer;

	class MaxEntModel			*m_DL_Model;


	CSG_String					Get_Feature		(int x, int y, int i);
	bool						Get_Features	(CSG_Array &Features);

	bool						Get_Training	(void);
	bool						Get_Training	(int x, int y, const char *ID);

	bool						Get_File		(const CSG_String &File);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__presence_prediction_H
