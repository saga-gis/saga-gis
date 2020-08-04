
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                         svm                           //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     svm_grids.h                       //
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
#ifndef HEADER_INCLUDED__svm_grids_H
#define HEADER_INCLUDED__svm_grids_H


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

//---------------------------------------------------------
#if defined(SYSTEM_SVM)
	#include <libsvm/svm.h>
#else
	#include "svm/svm.h"
#endif


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSVM_Grids : public CSG_Tool_Grid
{
public:
	CSVM_Grids(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("A:Imagery|Classification|Machine Learning") );	}


protected:

	virtual bool				On_Execute				(void);

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


private:

	int							m_Scaling;

	CSG_Table					m_Classes;

	CSG_Parameter_Grid_List		*m_pGrids;

	CSG_Grid					*m_pClasses;

	struct svm_problem			m_Problem;

	struct svm_node				*m_Nodes;

	struct svm_model			*m_pModel;


	double						Get_Value				(int x, int y, int iGrid);

	bool						Load					(void);

	bool						Training				(void);
	bool						Training_Get_Parameters	(struct svm_parameter &param);
	bool						Training_Get_Elements	(CSG_Table &Elements);

	bool						Predict					(void);

	bool						Finalize				(void);

};


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__svm_grids_H
