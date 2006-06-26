
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Discretisation                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Segmentation.h                  //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Grid_Segmentation_H
#define HEADER_INCLUDED__Grid_Segmentation_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSegment
{
public:
	CSegment(int Segment, double Value, int xSeed, int ySeed);
	~CSegment(void);

	int					Get_Segment(int jSegment);
	void				Set_Segment(int jSegment, int jConnect);

	double				Get_Value(void)	{	return( Value );	}
	int					Get_xSeed(void)	{	return( xSeed );	}
	int					Get_ySeed(void)	{	return( ySeed );	}


private:

	int					iSegment, xSeed, ySeed,
						nConnects, maxConnects,
						*Connect, *Segment;

	double				Value;
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Segmentation : public CModule_Grid
{
public:
	CGrid_Segmentation(void);
	virtual ~CGrid_Segmentation(void);


protected:

	virtual bool		On_Execute(void);


private:

	int					nSegments;

	CGrid				*pGrid, *pSegments;

	CSegment			**Segments;


	void				Do_Grid_Segmentation(double Threshold);

	bool				Get_Initials(void);
	void				Get_Junctions(void);

	void				Segment_Change(int iFrom, int iTo);
	bool				Segment_Change(int iFrom, int iTo, int x, int y);

	void				UnPrepareNoBorders(void);
	void				UnPrepareBorders(void);
};

#endif // #ifndef HEADER_INCLUDED__Grid_Segmentation_H
