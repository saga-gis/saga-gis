
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                 imagery_segmentation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        slic.h                         //
//                                                       //
//                  Olaf Conrad (C) 2019                 //
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
#ifndef HEADER_INCLUDED__slic_H
#define HEADER_INCLUDED__slic_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSLIC : public CSG_Tool_Grid
{
public:
	CSLIC(void);

//	virtual CSG_String				Get_MenuPath			(void)	{	return( _TL("Segmentation") );	}


protected:

	virtual int						On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int						On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool					On_Execute				(void);


private:

	bool							m_bNormalize;

	CSG_Parameter_Grid_List			*m_pGrids;

	CSG_Grid						*m_Centroid;


	int								Get_Feature_Count		(void);
	double							Get_Feature				(int k, int x, int y);

	int								Fit_To_Grid_System		(double Value, int Coordinate);


	bool							Get_Polygons			(CSG_Grid &Segments);
	bool							Get_Grids				(CSG_Grid &Segments);

	bool							Get_Segments			(CSG_Grid &Segments);

	bool							Get_Edge				(CSG_Grid &Edge);

	bool							Get_Centroids			(int Size);
	bool							Del_Centroids			(void);

	bool							Get_Generalized			(CSG_Grid &Segments);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__slic_H

