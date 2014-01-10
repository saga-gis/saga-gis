/**********************************************************
 * Version $Id: My_Module.h 1246 2011-11-25 13:42:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
//    e-mail:     3piras@informatik.uni-hamburg.de       //
//                                                       //
//    contact:    Luca Piras                             //
//                Rellinger Str. 39                      //
//                20257 Hamburg                          //
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
#ifndef HEADER_INCLUDED__OpenCV_NNet_H
#define HEADER_INCLUDED__OpenCV_NNet_H

//---------------------------------------------------------
#include "MLB_Interface.h"
#include "opencv\cxcore.h"
#include "opencv\ml.h"

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Use the 'My_Module_EXPORT' macro as defined in
// 'MLB_Interface.h' to export this class to allow other
// programs/libraries to use its functions:
//
// class My_Module_EXPORT CMy_Module : public CSG_Module
// ...
//

class COpenCV_NNet : public CSG_Module_Grid
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
#endif // #ifndef HEADER_INCLUDED__OpenCV_NNet_H
