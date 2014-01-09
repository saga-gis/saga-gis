/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_profiles                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_Cross_Profiles.cpp                //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Cross_Profiles.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define OFFSET	3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Cross_Profiles::CGrid_Cross_Profiles(void)
{
	Set_Name		(_TL("Cross Profiles"));

	Set_Author		(SG_T("O.Conrad (c) 2006"));

	Set_Description	(_TW(
		"Create cross profiles from a grid based DEM for given lines.\n"
	));

	Parameters.Add_Grid(
		NULL, "DEM"			, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "LINES"		, _TL("Lines"),
		_TL(""),
		PARAMETER_INPUT		, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL, "PROFILES"	, _TL("Cross Profiles"),
		_TL(""),
		PARAMETER_OUTPUT	, SHAPE_TYPE_Line
	);

	Parameters.Add_Value(
		NULL, "DIST_LINE"	, _TL("Profile Distance"),
		_TL(""),
		PARAMETER_TYPE_Double, 10.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "DIST_PROFILE", _TL("Profile Length"),
		_TL(""),
		PARAMETER_TYPE_Double, 10.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL, "NUM_PROFILE"	, _TL("Profile Samples"),
		_TL(""),
		PARAMETER_TYPE_Int	, 10.0, 3.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cross_Profiles::On_Execute(void)
{
	int			iLine, iPart, iPoint, nSamples;
	double		Distance, Length, dLine, dist, dx, dy;
	TSG_Point	iPt, jPt, dPt, aPt, bPt;
	CSG_Shapes	*pLines, *pProfiles;
	CSG_Shape	*pLine, *pProfile;

	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM")			->asGrid();
	pProfiles	= Parameters("PROFILES")	->asShapes();
	pLines		= Parameters("LINES")		->asShapes();
	Distance	= Parameters("DIST_LINE")	->asDouble();
	Length		= Parameters("DIST_PROFILE")->asDouble();
	nSamples	= Parameters("NUM_PROFILE")	->asInt();

	//-----------------------------------------------------
	pProfiles->Create(SHAPE_TYPE_Line, _TL("Profiles"));
	pProfiles->Add_Field("ID"	, SG_DATATYPE_Int);
	pProfiles->Add_Field("LINE"	, SG_DATATYPE_Int);
	pProfiles->Add_Field("PART"	, SG_DATATYPE_Int);

	for(iPoint=0; iPoint<nSamples; iPoint++)
	{
		pProfiles->Add_Field(CSG_String::Format(SG_T("X%03d"), iPoint), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
	for(iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		pLine	= pLines->Get_Shape(iLine);

		for(iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			if( pLine->Get_Point_Count(iPart) > 1 )
			{
				dist	= 0.0;
				iPt		= pLine->Get_Point(0, iPart);

				for(iPoint=1; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					jPt		= iPt;
					iPt		= pLine->Get_Point(iPoint, iPart);
					dx		= iPt.x - jPt.x;
					dy		= iPt.y - jPt.y;
					dLine	= sqrt(dx*dx + dy*dy);
					dx		/= dLine;
					dy		/= dLine;

					while( dist < dLine )
					{
						dPt.x	= jPt.x + dist * dx;
						dPt.y	= jPt.y + dist * dy;

						if( m_pDEM->is_InGrid_byPos(dPt) )
						{
							aPt.x	= dPt.x + dy * Length;
							aPt.y	= dPt.y - dx * Length;
							bPt.x	= dPt.x - dy * Length;
							bPt.y	= dPt.y + dx * Length;

							pProfile	= pProfiles->Add_Shape();
							pProfile->Add_Point(aPt);
							pProfile->Add_Point(bPt);
							pProfile->Set_Value(0, pProfiles->Get_Count());
							pProfile->Set_Value(1, iLine);
							pProfile->Set_Value(2, iPart);

							Get_Profile(pProfile, aPt, bPt, nSamples);
						}

						dist	+= Distance;
					}

					dist	-= dLine;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( pProfiles->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Cross_Profiles::Get_Profile(CSG_Shape *pProfile, TSG_Point A, TSG_Point B, int nSamples)
{
	if( 1 )
	{
		double	d, dx, dy, z;

		dx	= B.x - A.x;
		dy	= B.y - A.y;
	//	d	= sqrt(dx*dx + dy*dy) / (nSamples - 1);
		d	= (nSamples - 1);
		dx	/= d;
		dy	/= d;

		for(int i=0; i<nSamples; i++)
		{
			if( m_pDEM->Get_Value(A, z) )
			{
				pProfile->Set_Value	(OFFSET + i, z);
			}
			else
			{
				pProfile->Set_NoData	(OFFSET + i);
			}

			A.x	+= dx;
			A.y	+= dy;
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/*/---------------------------------------------------------
#include <saga_api/doc_pdf.h>

//---------------------------------------------------------
#define NBOXES	4

//---------------------------------------------------------
void CGrid_Cross_Profiles::Make_Report(const SG_Char *FileName, CSG_Grid *pDEM, CSG_Shapes *pLines, CSG_Shapes *pProfiles, double Distance)
{
	if( FileName )
	{
		int				iProfile, iPoint, nSamples, iBox;
		CSG_Rect		r;
		CSG_Shape			*pProfile, *pLine;
		CSG_Shapes			Profile;
		CSG_Doc_PDF	pdf;

		pdf.Open(PDF_PAGE_SIZE_A4, PDF_PAGE_ORIENTATION_PORTRAIT, _TL("Cross Profiles"));
		pdf.Layout_Add_Box(5,  5, 95, 20);
		pdf.Layout_Add_Box(5, 25, 95, 45);
		pdf.Layout_Add_Box(5, 50, 95, 70);
		pdf.Layout_Add_Box(5, 75, 95, 90);

		nSamples	= pProfiles->Get_Field_Count() - OFFSET;
		Distance	= Distance / (nSamples - 1);
		iBox		= 0;

		for(iProfile=0; iProfile<pProfiles->Get_Count() && Set_Progress(iProfile, pProfiles->Get_Count()); iProfile++)
		{
			pProfile	= pProfiles->Get_Shape(iProfile);

			Profile.Create(SHAPE_TYPE_Line);
			pLine		= Profile.Add_Shape();

			for(iPoint=0; iPoint<nSamples; iPoint++)
			{
				if( !pProfile->is_NoData(OFFSET + iPoint) )
				{
					pLine->Add_Point(
						(iPoint - nSamples / 2) * Distance,
						pProfile->asDouble(OFFSET + iPoint)
					);
				}
			}

			if( pLine->Get_Point_Count(0) > 1 )
			{
				if( iBox >= NBOXES )
				{
					pdf.Add_Page();
					iBox	= 0;
				}

//				pdf.Draw_Graticule	(pdf.Layout_Get_Box(iBox),  Profile.Get_Extent(), 20);
//				pdf.Draw_Shapes		(pdf.Layout_Get_Box(iBox), &Profile);
				r	= pLine->Get_Extent();	r.Inflate(10);
				pdf.Draw_Graticule	(pdf.Layout_Get_Box(iBox), r, 15);
				pdf.Draw_Shape		(pdf.Layout_Get_Box(iBox), pLine, PDF_STYLE_POLYGON_FILLSTROKE, SG_COLOR_GREEN, SG_COLOR_BLACK, 1, &r);

				iBox++;
			}
		}

		pdf.Save(Parameters("DOCUMENT")->asString());
	}
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
