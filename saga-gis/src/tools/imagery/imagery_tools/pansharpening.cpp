
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    imagery_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   pansharpening.cpp                   //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "pansharpening.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Grid_Resampling	Get_Resampling(int Resampling)
{
	switch( Resampling )
	{
	case  0: return( GRID_RESAMPLING_NearestNeighbour );
	case  1: return( GRID_RESAMPLING_Bilinear         );
	default: return( GRID_RESAMPLING_BSpline          );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPanSharp_IHS::CPanSharp_IHS(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("IHS Sharpening"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Intensity, hue, saturation (IHS) sharpening."
	));

	Add_Reference("Haydn, R., Dalke, G. W., Henkel, J., Bare, J. E.", "1982",
		"Application of the IHS color transform to the processing of multisensor data and image enhancement",
		"Proceedings of the International Symposium on Remote Sensing of Arid and Semi-Arid Lands, "
		"Cairo, Egypt (Environmental Research Institute, Ann Arbor, Mich., 1982), pp. 599–616."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "R", _TL("Red"  ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "G", _TL("Green"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "B", _TL("Blue" ), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid_System("", "PAN_SYSTEM"	, _TL("High Resolution Grid System"),
		_TL("")
	);

	Parameters.Add_Grid("PAN_SYSTEM",
		"PAN"		, _TL("Panchromatic Channel"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid("PAN_SYSTEM", "R_SHARP", _TL("Red"  ), _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid("PAN_SYSTEM", "G_SHARP", _TL("Green"), _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid("PAN_SYSTEM", "B_SHARP", _TL("Blue" ), _TL(""), PARAMETER_OUTPUT, false);

	Parameters.Add_Grid_List("",
		"SHARPEN"	, _TL("Sharpened Channels"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("nearest neighbour"),
			_TL("bilinear"),
			_TL("cubic convolution")
		), 2
	);

	Parameters.Add_Choice("",
		"PAN_MATCH"	, _TL("Panchromatic Channel Matching"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("normalized"),
			_TL("standardized")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPanSharp_IHS::On_Execute(void)
{
	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling	= Get_Resampling(Parameters("RESAMPLING")->asInt());

	//-----------------------------------------------------
	int			y;

	CSG_Grid	*pPan	= Parameters("PAN")->asGrid();

	//-----------------------------------------------------
	Process_Set_Text("%s: %s ...", _TL("Resampling"), Parameters("R")->asGrid()->Get_Name());
	CSG_Grid	*pR	= Parameters("R_SHARP")->asGrid();
	pR->Assign  (Parameters("R")->asGrid(), Resampling);
	pR->Set_Name(Parameters("R")->asGrid()->Get_Name());

	Process_Set_Text("%s: %s ...", _TL("Resampling"), Parameters("G")->asGrid()->Get_Name());
	CSG_Grid	*pG	= Parameters("G_SHARP")->asGrid();
	pG->Assign  (Parameters("G")->asGrid(), Resampling);
	pG->Set_Name(Parameters("G")->asGrid()->Get_Name());

	Process_Set_Text("%s: %s ...", _TL("Resampling"), Parameters("B")->asGrid()->Get_Name());
	CSG_Grid	*pB	= Parameters("B_SHARP")->asGrid();
	pB->Assign  (Parameters("B")->asGrid(), Resampling);
	pB->Set_Name(Parameters("B")->asGrid()->Get_Name());

	//-----------------------------------------------------
	Process_Set_Text(_TL("RGB to IHS"));

	double	rMin	= pR->Get_Min(),	rRange	= pR->Get_Range();
	double	gMin	= pG->Get_Min(),	gRange	= pG->Get_Range();
	double	bMin	= pB->Get_Min(),	bRange	= pB->Get_Range();

	for(y=0; y<pPan->Get_NY() && Set_Progress(y, pPan->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pPan->Get_NX(); x++)
		{
			bool	bNoData	= true;

			if( pPan->is_NoData(x, y) || pR->is_NoData(x, y) || pG->is_NoData(x, y) || pB->is_NoData(x, y) )
			{
				pR->Set_NoData(x, y);
				pG->Set_NoData(x, y);
				pB->Set_NoData(x, y);
			}
			else
			{
				double	r	= (pR->asDouble(x, y) - rMin) / rRange;	if( r < 0.0 ) r = 0.0; else if( r > 1.0 ) r = 1.0;
				double	g	= (pG->asDouble(x, y) - gMin) / gRange;	if( g < 0.0 ) g = 0.0; else if( g > 1.0 ) g = 1.0;
				double	b	= (pB->asDouble(x, y) - bMin) / bRange;	if( b < 0.0 ) b = 0.0; else if( b > 1.0 ) b = 1.0;

				double	h, s, i	= r + g + b;
				
				if( i <= 0.0 )
				{
					h	= 0.0;
					s	= 0.0;
				}
				else
				{
					if( r == g && g == b )			{	h	= 0.0;	}
					else if( b < r && b < g )		{	h	= (g - b) / (i - 3 * b)    ;	}
					else if( r < g && r < b )		{	h	= (b - r) / (i - 3 * r) + 1;	}
					else							{	h	= (r - g) / (i - 3 * g) + 2;	}

					if     ( 0.0 <= h && h < 1.0 )	{	s	= (i - 3 * b) / i;	}
					else if( 1.0 <= h && h < 2.0 )	{	s	= (i - 3 * r) / i;	}
					else							{	s	= (i - 3 * g) / i;	}
				}

				pR->Set_Value(x, y, i);
				pG->Set_Value(x, y, s);
				pB->Set_Value(x, y, h);
			}
		}
	}

	//-----------------------------------------------------
	double	Offset_Pan, Offset, Scale;

	if( Parameters("PAN_MATCH")->asInt() == 0 )
	{
		Offset_Pan	= pPan->Get_Min();
		Offset		= pR->Get_Min();
		Scale		= pR->Get_Range() / pPan->Get_Range();
	}
	else
	{
		Offset_Pan	= pPan->Get_Mean();
		Offset		= pR->Get_Mean();
		Scale		= pR->Get_StdDev() / pPan->Get_StdDev();
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("IHS to RGB"));

	for(y=0; y<pPan->Get_NY() && Set_Progress(y, pPan->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pPan->Get_NX(); x++)
		{
			if( !pR->is_NoData(x, y) )
			{
				double	i	= Offset + Scale * (pPan->asDouble(x, y) - Offset_Pan);
				double	s	= pG  ->asDouble(x, y);
				double	h	= pB  ->asDouble(x, y);

				double	r, g, b;

				if     ( 0.0 <= h && h < 1.0 )
				{
					r	= i * (1 + 2 * s - 3 * s * h) / 3;
					g	= i * (1 -     s + 3 * s * h) / 3;
					b	= i * (1 -     s            ) / 3;
				}
				else if( 1.0 <= h && h < 2.0 )
				{
					r	= i * (1 -     s                  ) / 3;
					g	= i * (1 + 2 * s - 3 * s * (h - 1)) / 3;
					b	= i * (1 -     s + 3 * s * (h - 1)) / 3;
				}
				else
				{
					r	= i * (1 -     s + 3 * s * (h - 2)) / 3;
					g	= i * (1 -     s                  ) / 3;
					b	= i * (1 + 2 * s - 3 * s * (h - 2)) / 3;
				}

				pR->Set_Value(x, y, rMin + r * rRange);
				pG->Set_Value(x, y, gMin + g * gRange);
				pB->Set_Value(x, y, bMin + b * bRange);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPanSharp_Brovey::CPanSharp_Brovey(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Colour Normalized Brovey Sharpening"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Colour normalized (Brovey) sharpening."
	));

	Add_Reference("Vrabel, J.", "1996",
		"Multispectral Imagery Band Sharpening Study",
		"Photogrammetric Engineering & Remote Sensing, Vol. 62, No. 9, pp. 1075-1083."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "R", _TL("Red"  ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "G", _TL("Green"), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "B", _TL("Blue" ), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid_System("",
		"PAN_SYSTEM", _TL("High Resolution Grid System"),
		_TL("")
	);

	Parameters.Add_Grid("PAN_SYSTEM",
		"PAN"		, _TL("Panchromatic Channel"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid("PAN_SYSTEM", "R_SHARP", _TL("Red"  ), _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid("PAN_SYSTEM", "G_SHARP", _TL("Green"), _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid("PAN_SYSTEM", "B_SHARP", _TL("Blue" ), _TL(""), PARAMETER_OUTPUT, false);

	Parameters.Add_Grid_List("",
		"SHARPEN"	, _TL("Sharpened Channels"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("nearest neighbour"),
			_TL("bilinear"),
			_TL("cubic convolution")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPanSharp_Brovey::On_Execute(void)
{
	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling	= Get_Resampling(Parameters("RESAMPLING")->asInt());

	//-----------------------------------------------------
	CSG_Grid	*pPan	= Parameters("PAN")->asGrid();

	//-----------------------------------------------------
	Process_Set_Text("%s: %s ...", _TL("Resampling"), Parameters("R")->asGrid()->Get_Name());
	CSG_Grid	*pR	= Parameters("R_SHARP")->asGrid();
	pR->Assign  (Parameters("R")->asGrid(), Resampling);
	pR->Set_Name(Parameters("R")->asGrid()->Get_Name());

	Process_Set_Text("%s: %s ...", _TL("Resampling"), Parameters("G")->asGrid()->Get_Name());
	CSG_Grid	*pG	= Parameters("G_SHARP")->asGrid();
	pG->Assign  (Parameters("G")->asGrid(), Resampling);
	pG->Set_Name(Parameters("G")->asGrid()->Get_Name());

	Process_Set_Text("%s: %s ...", _TL("Resampling"), Parameters("B")->asGrid()->Get_Name());
	CSG_Grid	*pB	= Parameters("B_SHARP")->asGrid();
	pB->Assign  (Parameters("B")->asGrid(), Resampling);
	pB->Set_Name(Parameters("B")->asGrid()->Get_Name());

	//-----------------------------------------------------
	Process_Set_Text(_TL("Sharpening"));

	for(int y=0; y<pPan->Get_NY() && Set_Progress(y, pPan->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pPan->Get_NX(); x++)
		{
			if( !pPan->is_NoData(x, y) && !pR->is_NoData(x, y) && !pG->is_NoData(x, y) && !pB->is_NoData(x, y) )
			{
				double	k	= (pR->asDouble(x, y) + pG->asDouble(x, y) + pB->asDouble(x, y));

				if( k != 0.0 )
				{
					k	= pPan->asDouble(x, y) / k;
				}

				pR->Mul_Value(x, y, k);
				pG->Mul_Value(x, y, k);
				pB->Mul_Value(x, y, k);
			}
			else
			{
				pR->Set_NoData(x, y);
				pG->Set_NoData(x, y);
				pB->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPanSharp_CN::CPanSharp_CN(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Colour Normalized Spectral Sharpening"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Colour normalized spectral sharpening."
	));

	Add_Reference("Vrabel, J., Doraiswamy, P., McMurtrey, J., Stern, A.", "2002",
		"Demonstration of the Accuracy of Improved Resolution Hyperspectral Imagery",
		"SPIE Symposium Proceedings."
	);

	Add_Reference("Vrabel, J., Doraiswamy, P., Stern, A", "2002",
		"Application of Hyperspectral Imagery Resolution Improvement for Site-Specific Farming",
		"ASPRS 2002 Conference Proceedings."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_System("",
		"LO_RES"	, _TL("Low Resolution Grid System"),
		_TL("")
	);

	Parameters.Add_Grid_List("LO_RES",
		"GRIDS"		, _TL("Original Channels"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid("",
		"PAN"		, _TL("Panchromatic Channel"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"SHARPEN"	, _TL("Sharpened Channels"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("nearest neighbour"),
			_TL("bilinear"),
			_TL("cubic convolution")
		), 2
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPanSharp_CN::On_Execute(void)
{
	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling	= Get_Resampling(Parameters("RESAMPLING")->asInt());

	//-----------------------------------------------------
	int		i;

	CSG_Grid	*pPan	= Parameters("PAN")->asGrid();

	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS"  )->asGridList();
	CSG_Parameter_Grid_List	*pSharp	= Parameters("SHARPEN")->asGridList();

	//-----------------------------------------------------
	pSharp->Del_Items();

	for(i=0; i<pGrids->Get_Grid_Count(); i++)
	{
		Process_Set_Text("%s: %s ...", _TL("Resampling"), pGrids->Get_Grid(i)->Get_Name());

		CSG_Grid	*pGrid	= SG_Create_Grid(Get_System());

		pGrid->Set_Name (pGrids->Get_Grid(i)->Get_Name());
		pGrid->Assign   (pGrids->Get_Grid(i), Resampling);

		pSharp->Add_Item(pGrid);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(i)
		for(int x=0; x<Get_NX(); x++)
		{
			double	Sum	= 0.0;

			if( !pPan->is_NoData(x, y) )
			{
				for(i=0; i<pSharp->Get_Grid_Count(); i++)
				{
					if( !pSharp->Get_Grid(i)->is_NoData(x, y) )
					{
						Sum	+= pSharp->Get_Grid(i)->asDouble(x, y);
					}
					else
					{
						Sum	 = 0.0;

						break;
					}
				}
			}

			if( Sum )
			{
				Sum	= pPan->asDouble(x, y) * pSharp->Get_Grid_Count() / (Sum + pSharp->Get_Grid_Count());

				for(i=0; i<pSharp->Get_Grid_Count(); i++)
				{
					pSharp->Get_Grid(i)->Mul_Value(x, y, Sum);
				}
			}
			else
			{
				for(i=0; i<pSharp->Get_Grid_Count(); i++)
				{
					pSharp->Get_Grid(i)->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPanSharp_PCA::CPanSharp_PCA(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Principal Component Based Image Sharpening"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Principal component based image sharpening."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_System("",
		"LO_RES"	, _TL("Low Resolution"),
		_TL("")
	);

	Parameters.Add_Grid_List("LO_RES",
		"GRIDS"		, _TL("Original Channels"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid("",
		"PAN"		, _TL("Panchromatic Channel"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"SHARPEN"	, _TL("Sharpened Channels"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("correlation matrix"),
			_TL("variance-covariance matrix"),
			_TL("sums-of-squares-and-cross-products matrix")
		), 1
	);

	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("nearest neighbour"),
			_TL("bilinear"),
			_TL("cubic convolution")
		), 2
	);

	Parameters.Add_Choice("",
		"PAN_MATCH"	, _TL("Panchromatic Channel Matching"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("normalized"),
			_TL("standardized")
		), 1
	);

	Parameters.Add_Bool("",
		"OVERWRITE"	, _TL("Overwrite"),
		_TL("overwrite previous output if adequate"),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPanSharp_PCA::On_Execute(void)
{
	//-----------------------------------------------------
	bool			bResult;
	CSG_Parameters	Tool_Parms;
	CSG_Table		Eigen;

	//-----------------------------------------------------
	// get the principal components for the low resolution bands

	SG_RUN_TOOL_KEEP_PARMS(bResult, "statistics_grid", 8, Tool_Parms,
			SG_TOOL_PARAMETER_SET("GRIDS"     , Parameters("GRIDS" ))
		&&	SG_TOOL_PARAMETER_SET("METHOD"    , Parameters("METHOD"))
		&&	SG_TOOL_PARAMETER_SET("EIGEN"     , &Eigen)
		&&	SG_TOOL_PARAMETER_SET("COMPONENTS", 0)	// get all components
	);

	if( !bResult )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pPCA	= Tool_Parms.Get_Parameter("PCA")->asGridList();

	int			i, n	= pPCA->Get_Grid_Count();

	CSG_Grid	*PCA	= new CSG_Grid[n];
	CSG_Grid	*pPan	= Parameters("PAN")->asGrid();

	//-----------------------------------------------------
	// replace first principal component with the high resolution panchromatic band

	Process_Set_Text(_TL("Replace first PC with PAN"));

	double	Offset_Pan, Offset, Scale;

	if( Parameters("PAN_MATCH")->asInt() == 0 )	// scale PAN band to fit first PC histogram
	{
		Offset_Pan	= pPan->Get_Min();
		Offset		= pPCA->Get_Grid(0)->Get_Min();
		Scale		= pPCA->Get_Grid(0)->Get_Range() / pPan->Get_Range();
	}
	else
	{
		Offset_Pan	= pPan->Get_Mean();
		Offset		= pPCA->Get_Grid(0)->Get_Mean();
		Scale		= pPCA->Get_Grid(0)->Get_StdDev() / pPan->Get_StdDev();
	}

	PCA[0].Create(Get_System());

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pPan->is_NoData(x, y) )
			{
				PCA[0].Set_NoData(x, y);
			}
			else
			{
				PCA[0].Set_Value(x, y, Offset + Scale * (pPan->asDouble(x, y) - Offset_Pan));
			}
		}
	}

	//-----------------------------------------------------
	// resample all other PCs to match the high resolution of the PAN band

	TSG_Grid_Resampling	Resampling	= Get_Resampling(Parameters("RESAMPLING")->asInt());

	for(i=1; i<n; i++)
	{
		Process_Set_Text("%s: %s ...", _TL("Resampling"), pPCA->Get_Grid(i)->Get_Name());

		PCA[i].Create(Get_System());
		PCA[i].Assign(pPCA->Get_Grid(i), Resampling);

		delete(pPCA->Get_Grid(i));	// PCA tool was unmanaged, so we have to delete the output
	}

	delete(pPCA->Get_Grid(0));

	pPCA->Del_Items();

	for(i=0; i<n; i++)
	{
		pPCA->Add_Item(&PCA[i]);
	}

	//-----------------------------------------------------
	// inverse principal component rotation for the high resolution bands

	SG_RUN_TOOL_KEEP_PARMS(bResult, "statistics_grid", 10, Tool_Parms,
			SG_TOOL_PARAMETER_SET("PCA"  , Tool_Parms("PCA"))
		&&	SG_TOOL_PARAMETER_SET("GRIDS", Parameters("SHARPEN"))
		&&	SG_TOOL_PARAMETER_SET("EIGEN", &Eigen)
	);

	delete[](PCA);

	if( !bResult )
	{
		return( false );
	}

	CSG_Parameter_Grid_List	*pHiRes	= Parameters("SHARPEN")->asGridList();
	CSG_Parameter_Grid_List	*pLoRes	= Parameters("GRIDS"  )->asGridList();
	CSG_Parameter_Grid_List	*pGrids	= Tool_Parms("GRIDS"  )->asGridList();

	if( !Parameters("OVERWRITE")->asBool() )
	{
		pHiRes->Del_Items();
	}

	for(i=0; i<pLoRes->Get_Grid_Count() && i<pGrids->Get_Grid_Count(); i++)
	{
		if( pHiRes->Get_Grid(i) )
		{
			pHiRes->Get_Grid(i)->Assign(pGrids->Get_Grid(i));

			delete(pGrids->Get_Grid(i));
		}
		else
		{
			pHiRes->Add_Item(pGrids->Get_Grid(i));
		}

		pHiRes->Get_Grid(i)->Set_Name(pLoRes->Get_Grid(i)->Get_Name());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
