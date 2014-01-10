/**********************************************************
 * Version $Id: mrvbf.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        MRVBF.h                        //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__MRVBF_H
#define HEADER_INCLUDED__MRVBF_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CMRVBF : public CSG_Module_Grid
{
public:
	CMRVBF(void);
	virtual ~CMRVBF(void);


protected:

	virtual bool				On_Execute				(void);


private:

	double						m_P_Slope, m_P_Pctl, m_T_Pctl_V, m_T_Pctl_R;

	CSG_Grid_Radius				m_Radius;


	double						Get_Transformation		(double x, double t, double p);

	bool						Get_Percentile			(CSG_Grid *pDEM, int x, int y, double &Percentile);
	bool						Get_Percentiles			(CSG_Grid *pDEM, CSG_Grid *pPercentile, int Radius);
	bool						Get_Slopes				(CSG_Grid *pDEM, CSG_Grid *pSlope);
	bool						Get_Smoothed			(CSG_Grid *pDEM, CSG_Grid *pSmoothed, int Radius, double Smoothing);
	bool						Get_Values				(CSG_Grid *pDEM, CSG_Grid *pSlope, CSG_Grid *pPercentiles, double Resolution);

	bool						Get_Flatness			(CSG_Grid *pSlope, CSG_Grid *pPctl, CSG_Grid *pF, CSG_Grid *pVB, CSG_Grid *pRT, double T_Slope);

	bool						Get_MRVBF				(int Level, CSG_Grid *pMRVBF, CSG_Grid *pVF, CSG_Grid *pMRRTF, CSG_Grid *pRF);

	bool						Get_Classified			(CSG_Grid *pMRF);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__MRVBF_H
