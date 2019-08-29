
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
//                  GSGrid_Residuals.h                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__GSGrid_Residuals_H
#define HEADER_INCLUDED__GSGrid_Residuals_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	MEAN	= 0,
	MIN,
	MAX,
	RANGE,
	STDDEV,
	VARIANCE,
	SUM,
	DIFF,
	DEVMEAN,
	PERCENT,
	MEDIAN,
	MINORITY,
	MAJORITY,
	COUNT
};

//---------------------------------------------------------
const CSG_String	Results[COUNT][2]	=
{
	{	"MEAN"    , _TL("Mean Value"                )	},
	{	"MIN"     , _TL("Minimum Value"             )	},
	{	"MAX"     , _TL("Maximum Value"             )	},
	{	"RANGE"   , _TL("Value Range"               )	},
	{	"STDDEV"  , _TL("Standard Deviation"        )	},
	{	"VARIANCE", _TL("Variance"                  )	},
	{	"SUM"     , _TL("Sum"                       )	},
	{	"DIFF"    , _TL("Difference from Mean Value")	},
	{	"DEVMEAN" , _TL("Deviation from Mean Value" )	},
	{	"PERCENT" , _TL("Percentile"                )	},
	{	"MEDIAN"  , _TL("Median"                    )	},
	{	"MINORITY", _TL("Minority"                  )	},
	{	"MAJORITY", _TL("Majority"                  )	}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGSGrid_Residuals : public CSG_Tool_Grid
{
public:
	CGSGrid_Residuals(void);


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	CSG_Grid_Cell_Addressor	m_Kernel;

	CSG_Grid				*m_pGrid, *m_pResult[COUNT];


	bool					Get_Statistics			(int x, int y, bool bCenter);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__GSGrid_Residuals_H
