
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      ESRI_E00.h                       //
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
//						ESRI_E00.h						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__ESRI_E00_H
#define HEADER_INCLUDED__ESRI_E00_H

//---------------------------------------------------------
#include "MLB_Interface.h"

#include "./e00compr/e00compr.h"

//---------------------------------------------------------
class CESRI_E00_Import : public CSG_Module
{
public:
	CESRI_E00_Import(void);
	virtual ~CESRI_E00_Import(void);


protected:

	virtual bool			On_Execute(void);


private:

	E00ReadPtr				hReadPtr;

	CSG_String				e00_Name;

	CSG_Table				*pPAT, *pAAT;


	bool					Open(const SG_Char *FileName);
	bool					Load(void);

	bool					E00GotoLine(int iLine);

	CSG_Grid *				getraster	(int prec, double scale);
	CSG_Shapes *			getarcs		(int prec, double scale, TSG_Shape_Type &shape_type);
	CSG_Shapes *			getlabels	(int prec, double scale);
	CSG_Shapes *			getsites	(int prec, double scale);
	double					getproj		(void);

	int						info_Get_Tables	(void);
	CSG_Table *				info_Get_Table	(struct info_Table info);
	void					info_Skip_Table	(struct info_Table info);
	void					info_Get_Record	(char *buffer, int buffer_length);

	bool					Assign_Attributes(CSG_Shapes *pShapes);

	CSG_Shapes *			Arcs2Polygons(CSG_Shapes *pArcs);
	void					Arcs2Polygon (CSG_Shapes *pArcs, CSG_Shapes *pPolygons, int id);

	void					skip	(char *end);
	void					skip_dat(void);
	void					skip_msk(void);
	void					skip_arc(int prec);
	void					skip_lab(int prec);
	void					skip_pal(int prec);
	void					skip_txt(int prec);

};

#endif // #ifndef HEADER_INCLUDED__ESRI_E00_H
