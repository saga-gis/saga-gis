
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       image_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 points_view_dialog.h                  //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#ifndef HEADER_INCLUDED__points_view_dialog_H
#define HEADER_INCLUDED__points_view_dialog_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_gdi/saga_gdi.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPoints_View_Dialog : public CSGDI_Dialog
{
public:
	CPoints_View_Dialog(CSG_PointCloud *pPoints);

	void						Update_Rotation			(void);
	void						Update_Extent			(void);


private:

	wxButton					*m_pBtn_Prop;

	wxCheckBox					*m_pCheck_Dist, *m_pCheck_Stereo, *m_pCheck_Bold;

	wxChoice					*m_pField_Color, *m_pField_Z;

	wxTextCtrl					*m_pFormula, *m_pParameters;

	CSGDI_Slider				*m_pSlide_xRotate, *m_pSlide_yRotate, *m_pSlide_zRotate, *m_pSlide_Dist, *m_pSlide_Detail;

	class CPoints_View_Control	*m_pView;

	class CPoints_View_Extent	*m_pExtent;


	void						On_Update_Control		(wxCommandEvent &event);
	void						On_Update_Choices		(wxCommandEvent &event);
	void						On_Button				(wxCommandEvent &event);

	void						On_Key_Down				(wxKeyEvent     &event);


	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__points_view_dialog_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
