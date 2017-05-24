/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     grid analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_IMCORR.cpp                    //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                      Magnus Bremer                    //
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
//    e-mail:     magnus.bremer@uibk.ac.at               //
//                                                       //
//    contact:    Magnus Bremer                          //
//                Institute of Geography                 //
//                University of Innsbruck                //
//                Innrain 52                             //
//                6020 Innsbruck                         //
//                Austria                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_IMCORR.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

/*	This implementation is based on the original FORTRAN code
	available from <http://nsidc.org/data/velmap/imcorr.html>.
	This implementation extends the functionality by introducing
	the possibility to derive 3D displacement vectors once DTM
	datasets are provided.
*/

//---------------------------------------------------------
CGrid_IMCORR::CGrid_IMCORR(void)
{
	Set_Name		(_TL("IMCORR - Feature Tracking"));

	Set_Author		(SG_T("Magnus Bremer (c) 2012"));

	Set_Description	(_TW(
		"The tool performs an image correlation "
		"based on two raster data sets.\n"
		"Additionally, two DTMs can be given and used to optain 3D displacement vectors.\n\n"
		"This is a SAGA implementation of the standalone "
		"IMCORR software provided by the "
		"National Snow and Ice Data Center in Boulder, Colorado / US.\n\n"
		"The standalone software and documentation is available from:\n"
		"<a href=\"http://nsidc.org/data/velmap/imcorr.html\">http://nsidc.org/data/velmap/imcorr.html</a>\n\n"
		"References:\n"
		"Scambos, T. A., Dutkiewicz, M. J., Wilson, J. C., and R. A. Bindschadler (1992): "
		"Application of image cross-correlation to the measurement of glacier velocity "
		"using satellite image data. Remote Sensing Environ., 42(3), 177-186.\n\n"
		"Fahnestock, M. A., Scambos, T.A., and R. A. Bindschadler (1992): "
		"Semi-automated ice velocity determination from satellite imagery. Eos, 73, 493.\n\n"
	));

	Parameters.Add_Grid(
		NULL	, "GRID_1", _TL("Grid 1"),
		_TL("The first grid to correlate"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "GRID_2"	, _TL("Grid 2"),
		_TL("The second grid to correlate"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "DTM_1", _TL("DTM 1"),
		_TL("The first DTM used to assign height information to grid 1"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "DTM_2"	, _TL("DTM 2"),
		_TL("The second DTM used to assign height information to grid 2"),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL, "CORRPOINTS"	, _TL("Correlated Points"),
		_TL("Correlated points with displacement and correlation information"),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL, "CORRLINES"	, _TL("Displacement Vector"),
		_TL("Displacement vectors between correlated points"),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		NULL	, "SEARCH_CHIPSIZE"		, _TL("Search Chip Size (Cells)"),
		_TL("Chip size of search chip, used to find correlating reference chip"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("16x16"),
			_TL("32x32"),
			_TL("64x64"),
			_TL("128x128"),
			_TL("256x256")
			), 2	
		);

	Parameters.Add_Choice(
		NULL	, "REF_CHIPSIZE"		, _TL("Reference Chip Size (Cells)"),
		_TL("Chip size of reference chip to be found in search chip"),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("16x16"),
			_TL("32x32"),
			_TL("64x64"),
			_TL("128x128")
			), 1	
		);


	Parameters.Add_Value(
		NULL	, "GRID_SPACING"	, _TL("Grid Spacing (Map Units)"),
		_TL("Grid spacing used for the construction of correlated points [map units]"),
		PARAMETER_TYPE_Double, 10.0, 0.1, true, 256.0, true
	);


}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_IMCORR::On_Execute(void)
{
	CSG_Grid		*pGrid1, *pGrid2, *pDTM1, *pDTM2;
	CSG_Shapes		*pCorrPts, *pCorrLines;
	CSG_Shape		*pCorrPt, *pCorrLine;
	int				Search_Chipsize, Ref_Chipsize, Grid_Spacing;
	double			SpacingMetrics;

	pGrid1			= Parameters("GRID_1")->asGrid();
	pGrid2			= Parameters("GRID_2")->asGrid();
	pDTM1			= Parameters("DTM_1")->asGrid();
	pDTM2			= Parameters("DTM_2")->asGrid();
	pCorrPts		= Parameters("CORRPOINTS")->asShapes();
	pCorrLines		= Parameters("CORRLINES")->asShapes();
	Search_Chipsize	= Parameters("SEARCH_CHIPSIZE")->asInt();
	Ref_Chipsize	= Parameters("REF_CHIPSIZE")->asInt();
	SpacingMetrics	= Parameters("GRID_SPACING")->asDouble();

	Search_Chipsize = (int)(pow(2.0,4+Search_Chipsize));
	Ref_Chipsize = (int)(pow(2.0,4+Ref_Chipsize));
	if (Search_Chipsize < Ref_Chipsize)
		Search_Chipsize = Ref_Chipsize;

	CSG_String	Message = CSG_String::Format(_TL("Search chip size set to %d"), Search_Chipsize);
	SG_UI_Msg_Add(Message,true);
	Message = CSG_String::Format(_TL("Reference chip size set to %d"), Ref_Chipsize);
	SG_UI_Msg_Add(Message,true);


	if (pDTM1 == NULL || pDTM2 == NULL)
	{
		CSG_String	name = CSG_String::Format(_TL("%s_CORRPOINTS"), pGrid1->Get_Name());
		pCorrPts->Create(SHAPE_TYPE_Point, name.c_str(), pCorrPts);
		pCorrPts->Add_Field(SG_T("ID"), SG_DATATYPE_Int);
		pCorrPts->Add_Field(SG_T("GX"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("GY"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("REALX"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("REALY"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("DISP"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("STRENGTH"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("FLAG"), SG_DATATYPE_Int);
		pCorrPts->Add_Field(SG_T("XDISP"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("YDISP"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("XDISP_UNIT"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("YDISP_UNIT"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("XTARG"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("YTARG"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("XERR"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("YERR"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("ASPECT"), SG_DATATYPE_Double);

		CSG_String	name2 = CSG_String::Format(_TL("%s_DISP_VEC"), pGrid1->Get_Name());
		pCorrLines->Create(SHAPE_TYPE_Line, name2.c_str(), pCorrLines);
		pCorrLines->Add_Field(SG_T("ID"), SG_DATATYPE_Int);
		pCorrLines->Add_Field(SG_T("GX"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("GY"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("REALX"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("REALY"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("DISP"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("STRENGTH"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("FLAG"), SG_DATATYPE_Int);
		pCorrLines->Add_Field(SG_T("XDISP"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("YDISP"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("XDISP_UNIT"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("YDISP_UNIT"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("XTARG"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("YTARG"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("XERR"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("YERR"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("ASPECT"), SG_DATATYPE_Double);
	}
	else // If DTM is given the Output gets 3D
	{
		CSG_String	name = CSG_String::Format(_TL("%s_CORRPOINTS"), pGrid1->Get_Name());
		pCorrPts->Create(SHAPE_TYPE_Point, name.c_str(), pCorrPts, SG_VERTEX_TYPE_XYZ);
		pCorrPts->Add_Field(SG_T("ID"), SG_DATATYPE_Int);
		pCorrPts->Add_Field(SG_T("GX"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("GY"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("REALX"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("REALY"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("REALZ"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("DISP"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("DISP_REAL"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("STRENGTH"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("FLAG"), SG_DATATYPE_Int);
		pCorrPts->Add_Field(SG_T("XDISP"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("YDISP"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("ZDISP"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("XDISP_UNIT"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("YDISP_UNIT"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("XTARG"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("YTARG"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("ZTARG"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("XERR"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("YERR"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("ASPECT"), SG_DATATYPE_Double);
		pCorrPts->Add_Field(SG_T("SLOPE"), SG_DATATYPE_Double);

		CSG_String	name2 = CSG_String::Format(_TL("%s_DISP_VEC"), pGrid1->Get_Name());
		pCorrLines->Create(SHAPE_TYPE_Line, name2.c_str(), pCorrLines, SG_VERTEX_TYPE_XYZ);
		pCorrLines->Add_Field(SG_T("ID"), SG_DATATYPE_Int);
		pCorrLines->Add_Field(SG_T("GX"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("GY"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("REALX"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("REALY"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("REALZ"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("DISP"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("DISP_REAL"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("STRENGTH"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("FLAG"), SG_DATATYPE_Int);
		pCorrLines->Add_Field(SG_T("XDISP"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("YDISP"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("ZDISP"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("XDISP_UNIT"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("YDISP_UNIT"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("XTARG"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("YTARG"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("ZTARG"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("XERR"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("YERR"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("ASPECT"), SG_DATATYPE_Double);
		pCorrLines->Add_Field(SG_T("SLOPE"), SG_DATATYPE_Double);
	}


	// create containers
	std::vector<std::vector<double> > SearchChip;
	SearchChip.resize(Search_Chipsize);
	for(int i = 0; i < Search_Chipsize; i++)
		SearchChip[i].resize(Search_Chipsize);

	std::vector<std::vector<double> > RefChip;
	RefChip.resize(Ref_Chipsize);
	for(int i = 0; i < Ref_Chipsize; i++)
		RefChip[i].resize(Ref_Chipsize);

	// defaults for values;
	double mincorr = 2.0;
	int fitmeth = 1, okparam = 1;
	double maxdis = -1.0;

	std::vector<double>ioffrq,nomoff;
	nomoff.push_back(0.0);
	nomoff.push_back((Search_Chipsize-Ref_Chipsize)*0.5);
	nomoff.push_back((Search_Chipsize-Ref_Chipsize)*0.5);

	ioffrq.push_back(0.0);
	ioffrq.push_back(Search_Chipsize/2);
	ioffrq.push_back(Search_Chipsize/2);

	
	double disp=0.0;
	double strength=0.0;
	std::vector<double>best_fit, est_err;

	Grid_Spacing = (int)(SpacingMetrics / (pGrid1->Get_Cellsize()));


	// enshures that chips are always in grid
	int ID = 0;
	for(int gx1 = (Search_Chipsize/2-1); gx1 < pGrid1->Get_NX()-(Search_Chipsize/2) && Set_Progress(gx1, pGrid1->Get_NX()-(Search_Chipsize/2)); gx1 += Grid_Spacing)
	{
		for(int gy1 = (Search_Chipsize/2-1); gy1 < pGrid1->Get_NY()-(Search_Chipsize/2); gy1 += Grid_Spacing)
		{
			// get ref_chip
			Get_This_Chip(RefChip, pGrid1, gx1, gy1, Ref_Chipsize);

			// get search chip
			Get_This_Chip(SearchChip, pGrid2, gx1, gy1, Search_Chipsize);
			gcorr(SearchChip, RefChip, mincorr, fitmeth, maxdis, ioffrq, nomoff, okparam, strength, best_fit, est_err, disp);
			
			if (okparam ==1)
			{
				disp = sqrt(best_fit[1]*best_fit[1] + best_fit[2]*best_fit[2]) * Get_Cellsize();
				double DirNormX = (best_fit[2] * Get_Cellsize()) / disp;
				double DirNormY = (best_fit[1] * Get_Cellsize()) / disp;
				double Aspect;
				

				if((DirNormY > 0.0 && DirNormX > 0))
					Aspect = (atan(fabs(DirNormX)/fabs(DirNormY)))*M_RAD_TO_DEG;
				else if ((DirNormY <= 0.0 && DirNormX > 0))
					Aspect = 90+((atan(fabs(DirNormY)/fabs(DirNormX)))*M_RAD_TO_DEG);
				else if ((DirNormY <= 0.0 && DirNormX <= 0))
					Aspect = 180+((atan(fabs(DirNormX)/fabs(DirNormY)))*M_RAD_TO_DEG);
				else
					Aspect = 270+((atan(fabs(DirNormY)/fabs(DirNormX)))*M_RAD_TO_DEG);

				double xReal = pGrid1->Get_System().Get_xGrid_to_World(gx1);
				double yReal = pGrid1->Get_System().Get_yGrid_to_World(gy1);
				
				double xReal2	= xReal + best_fit[2] * Get_Cellsize();
				double yReal2	= yReal + best_fit[1] * Get_Cellsize();

				if (pDTM1 == NULL || pDTM2 == NULL)
				{
					pCorrPt = pCorrPts->Add_Shape();
					pCorrPt->Add_Point(xReal, yReal);
					pCorrPt->Set_Value(0, ID);
					pCorrPt->Set_Value(1, gx1);
					pCorrPt->Set_Value(2, gy1);
					pCorrPt->Set_Value(3, xReal);
					pCorrPt->Set_Value(4, yReal);
					pCorrPt->Set_Value(5, disp);
					pCorrPt->Set_Value(6, strength);
					pCorrPt->Set_Value(7, okparam);
					pCorrPt->Set_Value(8, best_fit[2] * Get_Cellsize());
					pCorrPt->Set_Value(9, best_fit[1] * Get_Cellsize());
					pCorrPt->Set_Value(10, DirNormX);
					pCorrPt->Set_Value(11, DirNormY);
					pCorrPt->Set_Value(12, xReal2);
					pCorrPt->Set_Value(13, yReal2);
					pCorrPt->Set_Value(14, est_err[2]);
					pCorrPt->Set_Value(15, est_err[1]);
					pCorrPt->Set_Value(16, Aspect);

					pCorrLine = pCorrLines->Add_Shape();
					pCorrLine->Add_Point(xReal, yReal);
					pCorrLine->Add_Point(xReal2, yReal2);
					pCorrLine->Set_Value(0, ID);
					pCorrLine->Set_Value(1, gx1);
					pCorrLine->Set_Value(2, gy1);
					pCorrLine->Set_Value(3, xReal);
					pCorrLine->Set_Value(4, yReal);
					pCorrLine->Set_Value(5, disp);
					pCorrLine->Set_Value(6, strength);
					pCorrLine->Set_Value(7, okparam);
					pCorrLine->Set_Value(8, best_fit[2] * Get_Cellsize());
					pCorrLine->Set_Value(9, best_fit[1] * Get_Cellsize());
					pCorrLine->Set_Value(10, DirNormX);
					pCorrLine->Set_Value(11, DirNormY);
					pCorrLine->Set_Value(12, xReal2);
					pCorrLine->Set_Value(13, yReal2);
					pCorrLine->Set_Value(14, est_err[2]);
					pCorrLine->Set_Value(15, est_err[1]);
					pCorrLine->Set_Value(16, Aspect);
				}
				else
				{
					double zReal2 = pDTM2->Get_Value(xReal2, yReal2);
					double zReal = pDTM1->asDouble(gx1, gy1);
					double Slope = (atan((zReal2-zReal)/fabs(disp)))*M_RAD_TO_DEG;
					double dispReal = sqrt(pow(zReal2-zReal,2) + disp*disp);
					pCorrPt = pCorrPts->Add_Shape();
					pCorrPt->Add_Point(xReal,yReal);
					pCorrPt->Set_Z(zReal,ID,0);
					pCorrPt->Set_Value(0, ID);
					pCorrPt->Set_Value(1, gx1);
					pCorrPt->Set_Value(2, gy1);
					pCorrPt->Set_Value(3, xReal);
					pCorrPt->Set_Value(4, yReal);
					pCorrPt->Set_Value(5, zReal);
					pCorrPt->Set_Value(6, disp);
					pCorrPt->Set_Value(7, dispReal);
					pCorrPt->Set_Value(8, strength);
					pCorrPt->Set_Value(9, okparam);
					pCorrPt->Set_Value(10, best_fit[2] * Get_Cellsize());
					pCorrPt->Set_Value(11, best_fit[1] * Get_Cellsize());
					pCorrPt->Set_Value(12, zReal2-zReal);
					pCorrPt->Set_Value(13, DirNormX);
					pCorrPt->Set_Value(14, DirNormY);
					pCorrPt->Set_Value(15, xReal2);
					pCorrPt->Set_Value(16, yReal2);
					pCorrPt->Set_Value(17, zReal2);
					pCorrPt->Set_Value(18, est_err[2]);
					pCorrPt->Set_Value(19, est_err[1]);
					pCorrPt->Set_Value(20, Aspect);
					pCorrPt->Set_Value(21, Slope);

					pCorrLine = pCorrLines->Add_Shape();
					pCorrLine->Add_Point(xReal, yReal);
					pCorrLine->Set_Z(zReal,0,0);
					pCorrLine->Add_Point(xReal2, yReal2);
					pCorrLine->Set_Z(zReal2,1,0);
					pCorrLine->Set_Value(0, ID);
					pCorrLine->Set_Value(1, gx1);
					pCorrLine->Set_Value(2, gy1);
					pCorrLine->Set_Value(3, xReal);
					pCorrLine->Set_Value(4, yReal);
					pCorrLine->Set_Value(5, zReal);
					pCorrLine->Set_Value(6, disp);
					pCorrLine->Set_Value(7, dispReal);
					pCorrLine->Set_Value(8, strength);
					pCorrLine->Set_Value(9, okparam);
					pCorrLine->Set_Value(10, best_fit[2] * Get_Cellsize());
					pCorrLine->Set_Value(11, best_fit[1] * Get_Cellsize());
					pCorrLine->Set_Value(12, zReal2-zReal);
					pCorrLine->Set_Value(13, DirNormX);
					pCorrLine->Set_Value(14, DirNormY);
					pCorrLine->Set_Value(15, xReal2);
					pCorrLine->Set_Value(16, yReal2);
					pCorrLine->Set_Value(17, zReal2);
					pCorrLine->Set_Value(18, est_err[2]);
					pCorrLine->Set_Value(19, est_err[1]);
					pCorrLine->Set_Value(20, Aspect);
					pCorrLine->Set_Value(21, Slope);
				}
			ID++;
			}
		}
	}

	return( true );
}



///////////////////////////////////////////////////////////
//														 //
//						Methods						     //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_IMCORR::gcorr(std::vector<std::vector<double> >ChipSearch, std::vector<std::vector<double> >ChipRef, double csmin, int mfit, double ddmx, std::vector<double> ioffrq, std::vector<double> nomoff,    int& iacrej, double& streng, std::vector<double>& bfoffs,  std::vector<double>& tlerrs, double ddact)
{
	bfoffs.resize(3);
	std::vector<double>unormc;
	// compute raw cross-product sums
	cross(unormc,ChipSearch,ChipRef);

	// compute normalized cross-correlation values and compile statistics
	std::vector<double>ccnorm;
	std::vector<double>pkval;
	std::vector<int>ipkcol;
	std::vector<int>ipkrow;
	std::vector<double>sums;
	gnorm(ccnorm,pkval, ipkcol, ipkrow,sums,ChipSearch, ChipRef,unormc);

	int ncol = (int)(ChipSearch[0].size()-ChipRef[0].size()+1);
	int nrow = (int)(ChipSearch.size()-ChipRef.size()+1);
	// Evaluate Strength of correlation peak
	std::vector<double>cpval;
	eval(ncol, nrow, ccnorm,pkval, ipkcol, ipkrow,sums,csmin, streng, iacrej,cpval);

	std::vector<double>pkoffs;
	// determine offsets of peak relative to nominal location
	if (iacrej==1) 
	{
		if (mfit != 4)
		{
			fitreg(cpval, mfit, pkoffs, tlerrs);
			bfoffs[1] = (ipkcol[1] - 1) - nomoff[1] + pkoffs[1];
			bfoffs[2] = (ipkrow[1] - 1) - nomoff[2] + pkoffs[2];
		}
		else
		{
			bfoffs[1] = (ipkcol[1] - 1) - nomoff[1];
			bfoffs[2] = (ipkrow[1] - 1) - nomoff[2];
			tlerrs[1] = 0.5;
			tlerrs[2] = 0.5;
		}
		//
		//  Determine diagonal displacement from nominal and check against maximum 
		//  acceptable value
		ddact = sqrt(bfoffs[1]*bfoffs[1] + bfoffs[2]*bfoffs[2]);
		if (ddmx > 0.0)
		{
	  		if (ddact > ddmx)
	      		iacrej = 5;
		}
		else
		{
	  		if ( (bfoffs[1]*bfoffs[1] > ioffrq[1]*ioffrq[1])|| (bfoffs[2]*bfoffs[2] >  ioffrq[2]*ioffrq[2]) )
	      		iacrej = 5;
		}
	}
	return;
}


//---------------------------------------------------------
void CGrid_IMCORR::fitreg(std::vector<double> cpval, int mfit, std::vector<double>& pkoffs, std::vector<double>& tlerrs)
{
	pkoffs.resize(3);
	tlerrs.resize(3);
	
	int i, j;
	std::vector<std::vector<float> >b;
	std::vector<double> coeffs, vector, wghts, z;
	coeffs.resize(7);
	double denom;

	// compute elements of matrix and column vector;
	sums(cpval, mfit, z, wghts, b, vector);

	// invert matrix to get best-fit peak offsets
	kvert(b);
	i = 1;
	while (i <= 6)
	{
		coeffs[i] = 0.0;
		j = 1;
		while(j <= 6)
		{
			coeffs[i] += (b[i-1][j-1]*vector[j]);
			j++;
		}
		i++;
	}

	denom = 4.0*coeffs[4]*coeffs[6] - coeffs[5]*coeffs[5];
	pkoffs[1] = (coeffs[3] * coeffs[5] - 2.0 * coeffs[2] * coeffs[6])/denom;
	pkoffs[2] = (coeffs[2]*coeffs[5] - 2.0 * coeffs[3] * coeffs[4])/denom;

	// estimate errors in best fit offsets;
	esterr(z, wghts, b, coeffs, pkoffs, tlerrs);

	return;
}


//---------------------------------------------------------
void CGrid_IMCORR::esterr(std::vector<double> z, std::vector<double> wghts,std::vector<std::vector<float> > bnvrs, std::vector<double> coeffs, std::vector<double>& pkoffs, std::vector<double>& tlerrs)
{
	pkoffs.resize(3);
	tlerrs.resize(4);

	int i, ivalpt, j;
	std::vector<double>du, dv;
	du.resize(7);
	dv.resize(7);
	double c, denom, f, usum, var, vsum, x, xsum, y;

	//Compute residual variance for elements of peak array;
	ivalpt = 1;
	var = 0.0;
	y = -2.0;
	while (y <= 2.0)
	{
		x= -2.0;
		while(x <= 2.0)
		{
			f = coeffs[1] + coeffs[2]*x + coeffs[3]*y+ coeffs[4]*x+x + coeffs[5]*x*y + coeffs[6]*y*y;
			var+= (wghts[ivalpt]*pow((f-z[ivalpt]),2));
			ivalpt+=1;
			x+=1.0;
		}
		y+=1.0;
	}

	// compute constants to use the weights
	c = var/19.0;

	// compute partial derivatives of peak offsets with respect to polynomal coefficients
	denom = 4.0 * coeffs[4] * coeffs[6] - (coeffs[5] * coeffs[5]);
	du[1] = 0.0;
	du[2] = -2.0 * coeffs[6]/denom;
	du[3] = coeffs[5]/denom;
	du[4] = -4.0 * coeffs[6] * pkoffs[1]/denom;
	du[5] = (coeffs[3]+2.0*coeffs[5]*pkoffs[1])/denom;
	du[6] = (-2.0 * coeffs[2]-4.0*coeffs[4]*pkoffs[1])/denom;

	dv[1] = 0.0;
	dv[2] = du[3];
	dv[3] = -2.0 * coeffs[4]/denom;
	dv[4] = (-2.0*coeffs[3] - 4.0*coeffs[6]*pkoffs[2])/denom;
	dv[5] = (coeffs[2] + 2.0*coeffs[5] * pkoffs[2])/denom;
	dv[6] = -4.0 * coeffs[4] * pkoffs[2]/denom;

	// compute estimated errors in offsets
	usum = 0.0;
	vsum = 0.0;
	xsum = 0.0;
	i=1;
	while (i <= 6)
	{
		j=1;
		while(j <= 6)
		{
			usum += (du[i] * du[j] * bnvrs[i-1][j-1]);
			vsum += (dv[i] * dv[j] * bnvrs[i-1][j-1]);
			xsum += (du[i] * dv[j] * bnvrs[i-1][j-1]);
			j++;
		}
		i++;
	}
	tlerrs[1] = sqrt(abs(c*usum));
	tlerrs[2] = sqrt(abs(c*vsum));
	tlerrs[3] = c*xsum;

	return;
}


//---------------------------------------------------------
void CGrid_IMCORR::sums(std::vector<double> cpval, int mfit, std::vector<double>& z, std::vector<double>& wghts,std::vector<std::vector<float> > & b, std::vector<double>& vector)
{
	b.resize(6);
	for(int i = 0; i <b.size(); i++)
		b[i].resize(6);

	vector.resize(26);
	wghts.resize(26);
	z.resize(26);

	int i, ic, ir, ivalpt, j;

	std::vector<double>term;
	term.resize(7);

	double val;

	//initialize arrays and constants
	i = 1;
	while (i  <= 6)
	{
		j=1;
		while(j <= 6)
		{
			b[i-1][j-1] = 0.0;
			j++;
		}
		vector[i] = 0.0;
		i++;
	}
	term[1] = 1.0;
	ivalpt = 0;

	//compute function of correlation coefficient and assign weight for
	// each location in neighbourhood of peak

	ir = 1;
	while(ir <= 5)
	{
		ic = 1;
		while (ic <= 5)
		{
			ivalpt++;
			
			if (cpval[ivalpt] > 1.0)
				val = cpval[ivalpt];
			else
				val = 1.0;

			if (mfit == 1) // eliptical paraboloid
			{
				z[ivalpt] = val;
				wghts[ivalpt] = 1.0;
			}
			else if (mfit == 2) // eliptical gaussian
			{
				z[ivalpt] = log(val);
				wghts[ivalpt] = pow(val,2);
			}
			else // reciprocal paraboloid
			{
				z[ivalpt] = 1.0/val;
				wghts[ivalpt] = pow(val,4);
			}

			// compute matrix and vector elements in linear equations for best-fit coefficients
			term[2] = ic -3;
			term[3] = ir -3;
			term[4] = term[2] * term[2];
			term[5] = term[2] * term[3];
			term[6] = term[3] * term[3];
			i = 1;
			while (i <= 6)
			{	
				vector[i] += (wghts[ivalpt]*term[i]*z[ivalpt]);
				j=1;
				while (j <= 6)
				{	
					b[i-1][j-1] += (float)(wghts[ivalpt]*term[i]*term[j]);
					j++;
				}
				i++;
			}
			ic++;
		}
		ir++;
	}

	return;	
}


//---------------------------------------------------------
void  CGrid_IMCORR::cofact( float num[ 25 ][ 25 ], float f, std::vector<std::vector<float> >& INV )
{
    float b[ 25 ][ 25 ], fac[ 25 ][ 25 ];
    int p, q, m, n, i, j;
 
    for ( q = 0;q < f;q++ )
    {
        for ( p = 0;p < f;p++ )
        {
            m = 0;
            n = 0;
 
            for ( i = 0;i < f;i++ )
            {
                for ( j = 0;j < f;j++ )
                {
                    b[ i ][ j ] = 0;
 
                    if ( i != q && j != p )
                    {
                        b[ m ][ n ] = num[ i ][ j ];
 
                        if ( n < ( f-2.0 ) )
                            n++;
                        else
                        {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
 
            fac[ q ][ p ] = (float)(pow( -1.0, (q + p) ) * detrm( b, (f-1) ));
        }
    }
 
    trans( num, fac, f, INV );

	return;
}


//---------------------------------------------------------
float  CGrid_IMCORR::detrm( float a[ 25 ][ 25 ], float k )
{
    float s = 1, det = 0, b[ 25 ][ 25 ];
    int i, j, m, n, c;
 
    if ( k == 1 )
    {
        return ( a[ 0 ][ 0 ] );
    }
    else
    {
        det = 0;
 
        for ( c = 0;c < k;c++ )
        {
            m = 0;
            n = 0;
 
            for ( i = 0;i < k;i++ )
            {
                for ( j = 0;j < k;j++ )
                {
                    b[ i ][ j ] = 0;
 
                    if ( i != 0 && j != c )
                    {
                        b[ m ][ n ] = a[ i ][ j ];
 
                        if ( n < ( k-2 ) )
                            n++;
                        else
                        {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
 
            det = det + s * ( a[ 0 ][ c ] * detrm( b, k-1 ) );
            s = -1 * s;
        }
    }
 
    return ( det );
}


//---------------------------------------------------------
void  CGrid_IMCORR::trans( float num[ 25 ][ 25 ], float fac[ 25 ][ 25 ], float r , std::vector<std::vector<float> >& INV)
{
    int i, j;
    float b[ 25 ][ 25 ], inv[ 25 ][ 25 ], d;
 
    for ( i = 0;i < r;i++ )
    {
        for ( j = 0;j < r;j++ )
        {
            b[ i ][ j ] = fac[ j ][ i ];
        }
    }
 
    d = detrm( num, r );
    inv[ i ][ j ] = 0;
 
    for ( i = 0;i < r;i++ )
    {
        for ( j = 0;j < r;j++ )
        {
            inv[ i ][ j ] = b[ i ][ j ] / d;
        }
    }
 
	INV.resize((int)r);
	for (int ii = 0; ii < r; ii++)
	{
		INV[ii].resize((int)r);
	}
    for ( i = 0;i < r;i++ )
    {
        for ( j = 0;j < r;j++ )
        {
            INV[ i ][ j ] =  inv[ i ][ j ];
        }
    }

	return;
}


//---------------------------------------------------------
void CGrid_IMCORR::kvert(std::vector<std::vector<float> >& V)
{
	const int k = (int)V[0].size();
	float b[25][25];
	for ( int i = 0;i < k;i++ )
    {
        for (int  j = 0;j < k;j++ )
        {
             b[ i ][ j ] = V[ i ][ j ];
        }
    }
	float d = detrm( b, (float)k );
    if ( d != 0 )
        cofact( b, (float)k, V);

	return;
}


//---------------------------------------------------------
// code from original FORTRAN version
//void CGrid_IMCORR::kvert(std::vector<std::vector<double> >& V, std::vector<double>W)
//{
//	int LV = V[0].size()-1;
//	int N = V.size()-1;
//	int H,I,J,K,L,M,N,O,P,Q;
//	double S, T;
//
//
//	if (N != 1)
//	{
//		O = N+1;
//		L = 0;
//		M = 1;
//		if (L != N)
//		{
//			K = L;
//			L = M;
//			M = M + 1;
//			// find pivot and start row swap
//			P = L;
//			Q = L;
//			S = abs(V[L][L]);
//			H = L;
//			bool GoOut = false;
//			while (H <= N)
//			{
//				I = L;
//				while(I <= N)
//				{	
//					T = abs(V[I][H]);
//					if (T <= S)
//						break;
//					P = I;
//					Q = H;
//					S = T;
//					if ( I>N)
//						break;
//					I++;
//				}
//				H++;
//			}
//			W[N+L] = P;
//			W[O-L] = Q;
//			I = 1;
//			while(I <= N)
//			{
//				T = V[I][L];
//				V[I][L] = V[I][Q];
//				V[I][Q] = T;
//				I++;
//			}
//			S = V[P][L];
//			V[P][L] = V[L][L];
//			if (S != 0.0)
//			{
//				// compute multipliers
//				V[L][L] = -1;
//				S = 1.0/S;
//				I=1;
//				while(I <= N)
//				{
//					V[I][L] = -S * V[I][L];
//					I++;
//				}
//				J=L;
//				J++;
//				if(J > N)
//					J = 1;
//
//					
//			}
//
//		}
//	}
//	return;
//}


//---------------------------------------------------------
void CGrid_IMCORR::eval(int ncol, int nrow, std::vector<double> CCNORM , std::vector<double> pkval, std::vector<int> ipkcol, std::vector<int> ipkrow, std::vector<double> sums, double& csmin, double& streng, int& iacrej, std::vector<double>& cpval)
{
	cpval.resize(26,0.0);// shift to rebuild fortran;
	std::vector<int>ipt5(3);
	int i, icol, idist, iptr, ipt7, irow, krbase, lcol, lrow, npts, n5, n7, j;
	double bmean, bsigma;
	ipt5[0] = 0;
	ipt5[1] = 32;
	ipt5[2] = 32;
	iacrej = 1;
	streng = 0.0;
	ipt7 =1;

	// check for peak value within two rows or columns of edge
	if ((ipkcol[1] <= 2) || (ipkcol[1] >= ncol-1) || (ipkrow[1] <= 2) || (ipkrow[1] >= nrow-1))
	{
		iacrej = 0;
		return;
	}

	// Find largest values outside 5 by 5 and 7 by 7 neoghbourhoods of peak
	n5 = 0;
	n7 = 0;
	i=2;
	while ((n5 < 2) && (i <= 32))
	{
		if (abs(ipkcol[1]-ipkcol[i]) > abs(ipkrow[1]-ipkrow[i]))
			idist = abs(ipkcol[1]-ipkcol[i]);
		else
			idist = abs(ipkrow[1]-ipkrow[i]);
		if (idist > 2)
		{
			n5 = n5 +1;
			ipt5[n5] = i;
			if ((n7 == 0) && (idist < 3))
			{
				ipt7 = i;
				n7 = 1;
			}
		}
		i++;
	}

	if((ipt5[1] <= 3) || ipt5[2] <=5)
	{
		iacrej = 3;
		return;
	}

	//Find edges of 9 by 9 array centred on peak
	if (1 > ipkcol[1] -4)
		icol = 1;
	else
		icol = ipkcol[1] -4;

	if (1 > ipkcol[1] -4)
		irow = 1;
	else
		irow = ipkcol[1] -4;

	if (ncol < ipkcol[1] -4)
		lcol = ncol;
	else
		lcol = ipkcol[1] -4;

	if (nrow < ipkcol[1] -4)
		lrow = nrow;
	else
		lrow = ipkcol[1] -4;

	// eliminate points within 9 by 9 array from background statistics
	krbase = ncol*(nrow-1);
	i = irow;
	while (i <= lrow)
	{
		j=icol;
		while (j <= lcol)
		{
			sums[0] -= CCNORM[krbase+j];
			sums[1] -= (CCNORM[krbase+j]*CCNORM[krbase+j]);
			j++;
		}
		krbase+= ncol;
		i++;
	}

	//Compute background mean and standard deviation
	npts = ncol*nrow - (lcol - icol +1) * (lrow - irow +1);
	bmean = sums[0]/npts;
	bsigma = sqrt(sums[1]/npts - bmean*bmean);

	// Compute correlation strength and check against minimum. Note that
	// the LAS 4.x Version of the code contains an error... it often tries to
	// access pkval array element zero when there are no significant subsidiary peaks, resulting in
	// unpredictable results

	if (n7 == 0)
		streng = 2 * ((pkval[1] -bmean)/bsigma) -0.2;
	else
	{
		streng = (pkval[1] -bmean)/bsigma + (pkval[0] - pkval[ipt7])/bsigma + 0.2*(n7-1.0);
	}

	if (streng < csmin)
	{
		iacrej = 4;
		return;
	}

	//convert 5 by 5 neighbourhood of peak to standard deviations above mean
	krbase = ncol*(ipkrow[1]-3);
	iptr = 1;
	int I = 1;
	while (I <= 5)
	{
		j = ipkcol[1]-2;
		while (j <= ipkcol[1]+2)
		{
			cpval[iptr] = ((CCNORM[krbase+j] - bmean)/bsigma);
			iptr++;
			j++;
		}
		krbase+= ncol;
		I++;
	}

	return;
}


//---------------------------------------------------------
void CGrid_IMCORR::gnorm(std::vector<double>& CCNORM , std::vector<double>& pkval, std::vector<int>& ipkcol, std::vector<int>& ipkrow, std::vector<double>& sums,       std::vector<std::vector<double> >ChipSearch, std::vector<std::vector<double> >ChipRef, std::vector<double> UNORMC )
{
	std::vector<double>ser, ref;
	//create arrays
	ser.push_back(0.0); // dummy index to allow fortran conform indexing 
	for(int i = 0; i<ChipSearch.size(); i++)
	{
		for(int ii = 0; ii<ChipSearch[0].size(); ii++)
		{
			ser.push_back(ChipSearch[i][ii]);
		}
	}

	ref.push_back(0.0); // dummy index to allow fortran conform indexing 
	for(int i = 0; i<ChipRef.size(); i++)
	{
		for(int ii = 0; ii<ChipRef[0].size(); ii++)
		{
			ref.push_back(ChipRef[i][ii]);
		}
	}


	CCNORM.push_back(0.0);

	// double vectors
	std::vector<double >xval, colsum, colsqr;
	xval.resize(33);
	colsum.resize(257,0.0);
	colsqr.resize(257,0.0);

	// integer vectors
	std::vector<int>iptr, ixcol, ixrow;
	iptr.resize(33);
	ixcol.resize(33);
	ixrow.resize(33);

	// in most cases search is bigger than ref (default = 64x64)
	std::vector<int>nsnew;
	nsnew.push_back((int)ChipSearch[0].size());
	nsnew.push_back((int)ChipSearch.size());

	// in most cases ref is smaller than search (default = 32x32)
	std::vector<int>nrnew;
	nrnew.push_back((int)ChipRef[0].size());
	nrnew.push_back((int)ChipRef.size()); 

	int ipfree, ipt, jstart, jstop, k, kol, koladd, kolsub, line, lnadd, lnsub, ncol, nrow, nrtot;
	double refsum, refsqr, rho, rmean, rsigma, rtotal, sigma1, sigmas, srchsm, srchsq, temp, tempmn, igl, iglnew, iglold;
	refsum = 0;
	refsqr = 0;
	nrtot = nrnew[0]*nrnew[1];

	ipt =1;
	while (ipt <= nrtot)
	{
		igl = ref[ipt];
		refsum = refsum + igl;
		refsqr = refsqr +igl*igl;
		ipt++;
	}
	rtotal = nrtot;
	tempmn = 0.01 * (rtotal*rtotal);
	rmean = refsum/rtotal;
	if ((refsqr/rtotal - rmean*rmean) > 0.01)
		rsigma = sqrt(refsqr/rtotal - rmean*rmean);
	else
		rsigma = sqrt(0.01);

	// clear sums and sums of squares of normalized correlation values
	sums.push_back(0.0);
	sums.push_back(0.0);
	k =1;
	while (k <= 32)
	{
		xval[k] = -1.0;
		iptr[k] = k;
		k++;
	}
	ncol = nsnew[0] - nrnew[0] +1;
	nrow = nsnew[1] - nrnew[1] +1;

	// compute normalized cross-corr values for one row of alignments at a time
	jstart = 1;
	jstop = ncol;
	int i = 1;
	while (i <= nrow)
	{
		// Get column sums and sums of squares for portion of search
		// subimage overlain by reference in current row of alignments
		if (i == 1)
		{
			kol = 1;
			while(kol <= nsnew[0])
			{
				colsum[kol] = 0.0;
				colsqr[kol] = 0.0;
				kol++;
			}
			ipt = 1;
			line = 1;
			while (line <= nrnew[1])
			{
				kol = 1;
				while (kol <= nsnew[0])
				{
					igl = ser[ipt];
					colsum[kol]+=igl;
					colsqr[kol]+=(igl*igl);
					ipt++;
					kol++;
				}
				line++;
			}
		}
		else // if i != 1
		{
			lnsub = (i-2)*nsnew[0];
			lnadd = lnsub + nsnew[0] * nrnew[1];
			kol = 1;
			while (kol <= nsnew[0])
			{
				iglnew = ser[lnadd+kol];
				iglold = ser[lnsub+kol];
				colsum[kol]+= (iglnew-iglold);
				colsqr[kol]+= ((iglnew*iglnew)-(iglold*iglold));
				kol++;
			}

		} // end if

		// complete computation of search-subarea pixel statistics
		int j = jstart;
		while(j <= jstop)
		{
			
			if(j == jstart)
			{
				srchsm = 0.0;
				srchsq = 0.0;
				kol = 1;
				while (kol <= nrnew[0])
				{
					srchsm+= colsum[kol];
					srchsq+= colsqr[kol];
					kol++;
				}

				if (tempmn > (rtotal*srchsq - srchsm*srchsm))
					temp = tempmn;
				else
					temp = (rtotal*srchsq - srchsm*srchsm);
				sigmas = sqrt(temp);
			}
			else
			{
				kolsub = j -jstart;
				koladd = kolsub + nrnew[0];
				srchsm+= (colsum[koladd] - colsum[kolsub]);
				srchsq+= (colsqr[koladd] - colsqr[kolsub]);

				if (tempmn > (rtotal*srchsq - srchsm*srchsm))
					temp = tempmn;
				else
					temp = (rtotal*srchsq - srchsm*srchsm);

				sigma1 = 0.5 * (sigmas + temp/sigmas);
				sigmas = 0.5 *(sigma1 + temp/sigma1);
			}

			// compute normalized cross-correlation value
			rho = (UNORMC[j] - rmean*srchsm)/(rsigma * sigmas);
			CCNORM.push_back(rho);
			sums[0]+= rho;
			sums[1]+= (rho*rho);

			// check whether value is among top 32
			if (rho > xval[iptr[32]])
			{
				k = 32;
				ipfree = iptr[32];
				while ( k > 1 && rho > xval[iptr[k-1]])
				{
					iptr[k] = iptr[k-1];
					k = k-1;
				}
				iptr[k] = ipfree;
				xval[ipfree] = rho;
				ixcol[ipfree] = j - jstart + 1;
				ixrow[ipfree] = i;
			}
			j++;
		}
		jstart+= ncol;
		jstop+= ncol;
		i++;
	}

	// copy peak values and coordinates in correct sequence
	k= 1;
	pkval.push_back(0.0);
	ipkcol.push_back(0);
	ipkrow.push_back(0);
	while (k <= 32)
	{
		pkval.push_back(xval[iptr[k]]);
		ipkcol.push_back(ixcol[iptr[k]]);
		ipkrow.push_back(ixrow[iptr[k]]);
		k++;
	}

	return;
}


//---------------------------------------------------------
void CGrid_IMCORR::cross(std::vector<double>& UNORMC , std::vector<std::vector<double> >ChipSearch, std::vector<std::vector<double> >ChipRef)
{
	// in most cases search is bigger than ref (default = 64x64)
	std::vector<int>nsnew;
	nsnew.push_back((int)ChipSearch[0].size());
	nsnew.push_back((int)ChipSearch.size());

	// in most cases ref is smaller than search (default = 32x32)
	std::vector<int>nrnew;
	nrnew.push_back((int)ChipRef[0].size());
	nrnew.push_back((int)ChipRef.size());  

	std::vector<std::vector<double> > ChipRef2;
	ChipRef2.resize(ChipSearch[0].size());
	for(int i = 0; i < ChipSearch[0].size(); i++)
		ChipRef2[i].resize(ChipSearch.size(),0.0);

	// zero extent chipref to search chip size
	for(int i = 0; i<ChipRef.size(); i++)
	{
		for(int ii = 0; ii<ChipRef[0].size(); ii++)
		{
			ChipRef2[i][ii] = ChipRef[i][ii];
		}
	}


	std::vector<double>ser, ref;
	//create arrays
	ser.push_back(0.0); // dummy index to allow fortran conform indexing 
	for(int i = 0; i<ChipSearch.size(); i++)
	{
		for(int ii = 0; ii<ChipSearch[0].size(); ii++)
		{
			ser.push_back(ChipSearch[i][ii]);
		}
	}

	ref.push_back(0.0); // dummy index to allow fortran conform indexing 
	for(int i = 0; i<ChipRef2.size(); i++)
	{
		for(int ii = 0; ii<ChipRef2[0].size(); ii++)
		{
			ref.push_back(ChipRef2[i][ii]);
		}
	}

	int lnstrt = 1;
	int imgptr = 1;
	int line = 1;
	int lncont = 0;

	// pseudo complex arrays
	std::vector<double>cser, cref;
	cser.push_back(0.0);
	cref.push_back(0.0);
	for (int i = 1; i < ser.size(); i++)
	{
		cser.push_back(ser[i]);
		cser.push_back(0.0);
		cref.push_back(ref[i]);
		cref.push_back(0.0);
	}

	// take fast fourier transform of search and reference data
	fft2(cser,nsnew,1); // from "house" format to frequency format
	fft2(cref,nsnew,1);

	// make point by multiplication of ft of search ft with conjugate of reference image
	for(int i = 1; i<cser.size(); i+=2)
	{
		// cser[i] = cser[i] * conjg(cref[i])
		double temp_cser_real = cser[i];
		cser[i] = cser[i]*cref[i] - cser[i+1]*(-cref[i+1]); 
		cser[i+1] = temp_cser_real*(-cref[i+1]) + cref[i]*(cser[i+1]); 
	}

	// take inverse fft of cser
	fft2(cser,nsnew,-1); // real signal format (house) again

	//Extract useful valid correlation
	int ncol = nsnew[0] - nrnew[0] +1;
	int nrow = nsnew[1] - nrnew[1] +1;
	int denom = (int)(ChipSearch[0].size() * ChipSearch.size());
	int ndxout = 1;
	int i =1;

	UNORMC.push_back(0.0); // shift for fortran compatibility;
	std::vector<int>WhichValues;
	while(i <= nrow)
	{
		int j = 1;
		while(j <= ncol*2)
		{
			
			//UNORMC.push_back((double)(cser[(j-1)*nsnew[1]+i])/(double) (denom));
			UNORMC.push_back((double)(cser[(i-1)*(nsnew[1]*2)+j])/(double) (denom));
			WhichValues.push_back((i-1)*(nsnew[1]*2)+j);
			ndxout++;
			j+=2; // because of pseudo complex configuration
		}
		i+=1; // I think rows are not influenced by complex padding 
	}

	return;
}


//---------------------------------------------------------
void CGrid_IMCORR::fft2(std::vector<double>& data, std::vector<int>nel, int isign)
{
	double wr,wi,wpr,wpi,wtemp,theta;
	int ntot = nel[0]*nel[1];

	int nprev = 1;
	int idim = 1;
	while(idim <= 2)
	{
		int n = nel[idim-1];
		int nrem = ntot/(n*nprev);
		int ip1 = 2*nprev;
		int ip2 = ip1*n;
		int ip3 = ip2 * nrem;
		int i2rev = 1;

		// perform bit reversal
		int i2 = 1;
		while(i2 <= ip2)
		{
			if(i2 < i2rev)
			{
				int i1= i2;
				while (i1 <= (i2+ip1-2))
				{
					int i3=i1;
					while (i3 <= ip3)
					{
						int i3rev = i2rev+i3-i2;
						double tempr = data[i3];
						double tempi = data[i3+1];

						data[i3] = data[i3rev];
						data[i3+1] = data[i3rev+1];
						data[i3rev] = tempr;
						data[i3rev+1] = tempi;
						i3+=ip2;
					}
					i1+=2;
				}
			}
			int ibit = ip2/2;
			while (ibit >= ip1 && i2rev > ibit)
			{
				i2rev = i2rev-ibit;
				ibit = ibit/2;
			}
			i2rev = i2rev+ibit;
			i2+=ip1;
		}
		// end of bit reversal


		// Danielson-Lanczos Section
		int ifp1 = ip1;
		while(ifp1 < ip2)
		{
			int ifp2 = 2* ifp1;
			// Initialize for the trigonometric recurrence
			theta = isign *(M_PI*2) / (ifp2/ip1);
			wtemp = sin(0.5*theta);
			wpr = -2.0 * (wtemp*wtemp);
			wpi = sin(theta);
			wr = 1.0;
			wi = 0.0;
			int i3=1;
			while (i3 <= ifp1)
			{
				int i1 = i3;
				while (i1 <= (i3+ip1-2))
				{
					int i2 = i1;
					while (i2 <= ip3)
					{
						int k1 = i2;
						int k2 = k1+ifp1;
						double tempr = wr * data[k2] - wi * data[k2+1];
						double tempi = wr * data[k2+1] + wi * data[k2];
						data[k2] = data[k1]-tempr;
						data[k2+1] = data[k1+1]-tempi;
						data[k1] = data[k1]+tempr;
						data[k1+1] = data[k1+1]+tempi;
						i2+=ifp2;
					}
					i1+=2;
				}
				// the trigonometric recurrence
				wtemp = wr;
				wr = wr*wpr-wi*wpi+wr;
				wi=wi*wpr+wtemp*wpi+wi;
				i3+=ip1;
			}
			ifp1=ifp2;
		}
		nprev = n*nprev;
		idim++;
	}
	
	return;
}


//---------------------------------------------------------
int CGrid_IMCORR::decimal(std::vector<int> Bin) 
{
	int Sum = 0;
	for (int i = 0; i < Bin.size(); i++)
	{	
		int Exponent = (int)(Bin.size()-1)-i;
		Sum+= Bin[i] * (int)pow(2.0, Exponent);
	}
	return Sum;
}


//---------------------------------------------------------
void CGrid_IMCORR::binary(std::vector<int>& Bin, int number) 
{
	int remainder;

	if(number <= 1)
	{
		Bin.push_back(number);
		return;
	}
	remainder = number%2;
	binary(Bin, number >> 1);    
	Bin.push_back(remainder);
}


//---------------------------------------------------------
void CGrid_IMCORR::Get_This_Chip(std::vector<std::vector<double> >& Chip, CSG_Grid *pGrid, int gx, int gy, int Ref_Chipsize)
{
	int ref_chipX = 0;
	int ref_chipY = 0;
	for (int refx = gx - (Ref_Chipsize/2-1); refx < gx - (Ref_Chipsize/2-1) + Ref_Chipsize; refx++)
	{
		ref_chipY = 0;
		for (int refy = gy - (Ref_Chipsize/2-1); refy < gy - (Ref_Chipsize/2-1) + Ref_Chipsize; refy++)
		{
			Chip[ref_chipX][ref_chipY] = pGrid->asDouble(refx,refy);
			ref_chipY++;
		}
		ref_chipX++;
	}

	return;
}

//---------------------------------------------------------
