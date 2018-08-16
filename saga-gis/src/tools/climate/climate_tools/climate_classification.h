/**********************************************************
 * Version $Id: climate_classification.h 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                climate_classification.h               //
//                                                       //
//                 Copyright (C) 2018 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__climate_classification_H
#define HEADER_INCLUDED__climate_classification_H


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
class CClimate_Classification : public CSG_Tool_Grid
{
public:
	CClimate_Classification(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Bioclimatology") );	}

	typedef struct SClassInfo
	{
		int	ID, Color;	CSG_String	Name, Description;
	}
	TClassInfo;


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool				On_Execute				(void);


private:

	bool						Set_Classified			(CSG_Grid *pClasses, int Method);
	bool						Set_Classified			(CSG_Grid *pClasses, const TClassInfo Info[], int nClasses);

	bool						Get_Values				(int x, int y, CSG_Parameter_Grid_List *pValues, CSG_Simple_Statistics &Values);

	bool						is_North				(double T[]);

	bool						Get_PSeasonal			(bool bNorth, double P[], CSG_Simple_Statistics &PWinter, CSG_Simple_Statistics &PSummer);

	int							Get_GrowingDegreeDays	(double T[], double Tmin);
	double						Get_HumidMonths			(double T[], double P[]);

	int 						Get_KoppenGeiger		(int Method, CSG_Simple_Statistics &T, CSG_Simple_Statistics &P);

	int 						Get_TrollPaffen			(int Method, CSG_Simple_Statistics &T, CSG_Simple_Statistics &P);

	int 						Get_Thornthwaite		(int Method, CSG_Simple_Statistics &T, CSG_Simple_Statistics &P);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__climate_classification_H
