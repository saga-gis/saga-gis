
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      grid_pca.h                       //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#ifndef HEADER_INCLUDED__grid_pca_H
#define HEADER_INCLUDED__grid_pca_H

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_PCA : public CSG_Tool_Grid
{
public:
	CGrid_PCA(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("Principle Components") );	}


protected:

	virtual bool			On_Execute			(void);


private:

	int						m_Method, m_nFeatures;

	CSG_Parameter_Grid_List	*m_pGrids;


	bool					is_NoData			(sLong iCell);
	double					Get_Value			(sLong iCell, int iFeature);
	
	bool					Get_Matrix			(CSG_Matrix &Matrix);

	void					Print_Eigen_Values	(CSG_Vector &Eigen_Values);
	void					Print_Eigen_Vectors	(CSG_Matrix &Eigen_Vectors);

	bool					Get_Components		(CSG_Matrix &Eigen_Vectors);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_PCA_Inverse : public CSG_Tool_Grid
{
public:
	CGrid_PCA_Inverse(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("Principle Components") );	}


protected:

	virtual bool			On_Execute			(void);


private:

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__grid_pca_H
