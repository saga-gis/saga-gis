/**********************************************************
 * Version $Id: opencv_ml.h 0001 2016-05-24
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        OpenCV                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Copyright (C) 2016 by                 //
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
#ifndef HEADER_INCLUDED__OpenCV_ML_H
#define HEADER_INCLUDED__OpenCV_ML_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

#include "opencv2/core/version.hpp"

#if CV_MAJOR_VERSION >= 3

#include <opencv2/ml.hpp>

//---------------------------------------------------------
using namespace cv;
using namespace cv::ml;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COpenCV_ML : public CSG_Module_Grid
{
public:
	COpenCV_ML(bool bProbability);

	virtual CSG_String			Get_MenuPath	(void)	{	return( _TL("A:Imagery|Classification|Machine Learning (OpenCV)") );	}


protected:

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);

	int							Get_Feature_Count		(void)	{	return( m_pFeatures->Get_Count() );	}
	int							Get_Class_Count			(void)	{	return( m_Classes   .Get_Count() );	}

	virtual Ptr<StatModel>		Get_Model				(void)	= 0;
	virtual Ptr<TrainData>		Get_Training			(const CSG_Matrix &Data);

	virtual double				Get_Probability			(const Ptr<StatModel> &Model, const Mat &Sample)	{	return( 0.0 );	}


private:

	bool						m_bNormalize;

	CSG_Parameter_Grid_List		*m_pFeatures;

	CSG_Grid					*m_pClasses, *m_pProbability;

	CSG_Table					m_Classes;


	double						_Get_Feature			(int x, int y, int iFeature);

	bool						_Initialize				(void);
	bool						_Finalize				(void);

	bool						_Get_Training			(CSG_Matrix &Data);
	bool						_Get_Training			(CSG_Matrix &Data, CSG_Table_Record *pClass, CSG_Shape_Polygon *pPolygon);

	bool						_Get_Prediction			(const Ptr<StatModel> &Model);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COpenCV_ML_NBayes : public COpenCV_ML
{
public:
	COpenCV_ML_NBayes(void);


protected:

	virtual Ptr<StatModel>		Get_Model				(void);

	virtual double				Get_Probability			(const Ptr<StatModel> &Model, const Mat &Sample);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COpenCV_ML_KNN : public COpenCV_ML
{
public:
	COpenCV_ML_KNN(void);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual Ptr<StatModel>		Get_Model				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COpenCV_ML_SVM : public COpenCV_ML
{
public:
	COpenCV_ML_SVM(void);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual Ptr<StatModel>		Get_Model				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COpenCV_ML_DTrees : public COpenCV_ML
{
public:
	COpenCV_ML_DTrees(void);


protected:

	virtual Ptr<StatModel>		Get_Model				(void);

	virtual Ptr<DTrees>			Get_Trees				(void);

};

//---------------------------------------------------------
class COpenCV_ML_Boost : public COpenCV_ML_DTrees
{
public:
	COpenCV_ML_Boost(void);


protected:

	virtual Ptr<DTrees>			Get_Trees				(void);

};

//---------------------------------------------------------
class COpenCV_ML_RTrees : public COpenCV_ML_DTrees
{
public:
	COpenCV_ML_RTrees(void);


protected:

	virtual Ptr<DTrees>			Get_Trees				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class COpenCV_ML_ANN : public COpenCV_ML
{
public:
	COpenCV_ML_ANN(void);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual Ptr<StatModel>		Get_Model				(void);

	virtual Ptr<TrainData>		Get_Training			(const CSG_Matrix &Data);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define new_COpenCV_ML_NBayes	new COpenCV_ML_NBayes
#define new_COpenCV_ML_KNN		new COpenCV_ML_KNN
#define new_COpenCV_ML_SVM		new COpenCV_ML_SVM
#define new_COpenCV_ML_DTrees	new COpenCV_ML_DTrees
#define new_COpenCV_ML_Boost	new COpenCV_ML_Boost
#define new_COpenCV_ML_RTrees	new COpenCV_ML_RTrees
#define new_COpenCV_ML_ANN		new COpenCV_ML_ANN

#else // CV_MAJOR_VERSION == 3

#define new_COpenCV_ML_NBayes	MLB_INTERFACE_SKIP_MODULE
#define new_COpenCV_ML_KNN		MLB_INTERFACE_SKIP_MODULE
#define new_COpenCV_ML_SVM		MLB_INTERFACE_SKIP_MODULE
#define new_COpenCV_ML_DTrees	MLB_INTERFACE_SKIP_MODULE
#define new_COpenCV_ML_Boost	MLB_INTERFACE_SKIP_MODULE
#define new_COpenCV_ML_RTrees	MLB_INTERFACE_SKIP_MODULE
#define new_COpenCV_ML_ANN		MLB_INTERFACE_SKIP_MODULE

#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__OpenCV_ML_H
