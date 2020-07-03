
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       OpenCVNNet                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     OpenCVNNet.h						 //
//                                                       //
//														 //
//                      Luca Piras                       //
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
//    e-mail:     3piras@informatik.uni-hamburg.de       //
//                                                       //
//    contact:    Luca Piras                             //
//                Rellinger Str. 39                      //
//                20257 Hamburg                          //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__OpenCV_NNet_H
#define HEADER_INCLUDED__OpenCV_NNet_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include "opencv2/core/version.hpp"

#if CV_MAJOR_VERSION == 2

#include <opencv/cxcore.h>
#include <opencv/ml.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COpenCV_NNet : public CSG_Tool_Grid
{
public:
	COpenCV_NNet(void);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Imagery|Classification") );	}

	enum DATA_TYPE { TABLE = 0, GRID = 1 };
	enum ACTIVATION_FUNCTION { IDENTITY = 0, SIGMOID = 1, GAUSSIAN = 2 };
	enum TRAINING_METHOD { RPROP = 0, BPROP = 1	};

protected:

	virtual bool				On_Execute		(void);
	virtual int					On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

private:
	virtual CvMat*				GetEvalMatrix(CSG_Table* pTable, int type);
	virtual CvMat*				GetEvalMatrix(CSG_Parameter_Grid_List *gl_grids, int type);
	virtual CvMat**				GetTrainAndOutputMatrix(CSG_Table *t_trainData, int type, int *i_outputFeatureIdxs, int i_outputFeatureCount);
	virtual CvMat**				GetTrainAndOutputMatrix(CSG_Parameter_Grid_List *gl_grids, int type, CSG_Shapes *s_areas, int i_classId, CSG_Grid *g_evalOut, CSG_Grid *g_evalOutCert);
	virtual CSG_Vector			GetClassVectorByName(CSG_Shapes *s_areas, const SG_Char *s_class, int i_classId); 
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define new_COpenCV_NNet	new COpenCV_NNet

#else // CV_MAJOR_VERSION == 2

#define new_COpenCV_NNet	TLB_INTERFACE_SKIP_TOOL

#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__OpenCV_NNet_H
