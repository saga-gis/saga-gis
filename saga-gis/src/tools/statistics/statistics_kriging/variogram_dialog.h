/**********************************************************
 * Version $Id: variogram_dialog.h 1922 2014-01-09 10:28:46Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       image_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Variogram_Dialog.h                   //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
//    contact:    SAGA User Group Association            //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Variogram_Dialog_H
#define HEADER_INCLUDED__Variogram_Dialog_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

#include <saga_gdi/saga_gdi.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Variogram
{
public:
	enum ESG_Variogram_Field
	{
		FIELD_CLASS	= 0,
		FIELD_DISTANCE,
		FIELD_COUNT,
		FIELD_VAR_EXP,
		FIELD_VAR_CUM,
		FIELD_VAR_MODEL
	};

	CSG_Variogram(void);

	static bool		Calculate			(CSG_Shapes *pPoints, int Attribute, bool bLog, CSG_Table *pVariogram, int nClasses = 25, double maxDistance = 0.0, int nSkip = 1);

	static double	Get_Lag_Distance	(CSG_Shapes *pPoints, int Method, int nSkip = 1);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVariogram_Dialog : public CSGDI_Dialog
{
public:
	CVariogram_Dialog(void);

	bool						Execute		(CSG_Shapes *pPoints, int Attribute, bool bLog, CSG_Table *pVariogram, CSG_Trend *pModel);


private:

	wxButton					*m_pSettings;

	wxCheckBox					*m_pPairs;

	wxChoice					*m_pFormulas;

	wxTextCtrl					*m_pFormula, *m_pParameters;

	CSGDI_Slider				*m_pDistance;

	class CVariogram_Diagram	*m_pDiagram;

	CSG_Parameters				m_Settings;

	CSG_Trend					*m_pModel;

	CSG_Table					*m_pVariogram;

	CSG_Shapes					*m_pPoints;

	CSG_Rect					m_Extent;

	bool						m_bLog;

	int							m_Attribute, m_nPoints;

	double						m_Distance;


	void						On_Update_Control		(wxCommandEvent &event);
	void						On_Update_Choices		(wxCommandEvent &event);
	void						On_Button				(wxCommandEvent &event);

	void						Set_Variogram			(void);
	void						Set_Model				(void);


	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Variogram_Dialog_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
