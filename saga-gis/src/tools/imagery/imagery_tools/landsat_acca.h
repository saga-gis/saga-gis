
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     imagery_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    landsat_acca.h                     //
//                                                       //
//                 Copyright (C) 2012 by                 //
//            Benjamin Bechtel & Olaf Conrad             //
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
#ifndef HEADER_INCLUDED__landsat_acca_H
#define HEADER_INCLUDED__landsat_acca_H


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CACCA
{
public:
	CACCA(void) {}

	bool						m_bCelsius { false };

	enum
	{
		NO_DEFINED = 0, IS_COLD_CLOUD = 1, IS_WARM_CLOUD = 2, IS_SHADOW = 3
	};


	void	acca_algorithm		(CSG_Grid *pCloud, CSG_Grid *band[], int single_pass, int with_shadow, int cloud_signature, int n_hist = 100);

	void	filter_holes		(CSG_Grid *pGrid);


private:

	int		hist_n { 100 };

	void	acca_first			(CSG_Grid *pCloud, CSG_Grid *band[], int with_shadow, int count[], int cold[], int warm[], double stats[]);
	void	acca_second			(CSG_Grid *pCloud, CSG_Grid *band, int review_warm, double upper, double lower);

	int		shadow_algorithm	(double pixel[]);

	void	hist_put			(double t, int hist[]);
	double	quantile			(double q, int hist[]);
	double	moment				(int n, int hist[], int k);

};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLandsat_ACCA : public CSG_Tool_Grid
{
public:
	CLandsat_ACCA(void);

	virtual CSG_String	Get_MenuPath	(void)	{	return( _TL("Landsat") );	}


protected:

	virtual bool		On_Execute		(void);

};


///////////////////////////////////////////////////////////
//                                                       //												
//                                                       //												
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__landsat_acca_H
