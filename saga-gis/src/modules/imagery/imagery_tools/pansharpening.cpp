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
TSG_Grid_Resampling	Get_Interpolation(int Interpolation)
{
	switch( Interpolation )
	{
	case 0:				return( GRID_RESAMPLING_NearestNeighbour );
	case 1:				return( GRID_RESAMPLING_Bilinear );
	case 2: default:	return( GRID_RESAMPLING_BSpline );
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("IHS Sharpening"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Intensity, hue, saturation (IHS) sharpening.\n"
		"\nReferences:\n"
		"Haydn, R., Dalke, G. W., Henkel, J., Bare, J. E. (1982): "
		"Application of the IHS color transform to the processing of multisensor data and image enhancement. "
		"Proceedings of the International Symposium on Remote Sensing of Arid and Semi-Arid Lands, "
		"Cairo, Egypt (Environmental Research Institute, Ann Arbor, Mich., 1982), pp. 599–616.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL	, "R"		, _TL("Red")	, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL	, "G"		, _TL("Green")	, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL	, "B"		, _TL("Blue")	, _TL(""), PARAMETER_INPUT);

	pNode	= Parameters.Add_Grid_System(
		NULL	, "PAN_SYSTEM"	, _TL("High Resolution Grid System"),
		_TL("")
	);

	Parameters.Add_Grid(
		pNode	, "PAN"			, _TL("Panchromatic Channel"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(pNode	, "R_SHARP"	, _TL("Red")	, _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid(pNode	, "G_SHARP"	, _TL("Green")	, _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid(pNode	, "B_SHARP"	, _TL("Blue")	, _TL(""), PARAMETER_OUTPUT, false);

	Parameters.Add_Grid_List(
		NULL	, "SHARPEN"		, _TL("Sharpened Channels"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("nearest neighbour"),
			_TL("bilinear"),
			_TL("cubic convolution")
		), 2
	);

	Parameters.Add_Choice(
		NULL	, "PAN_MATCH"	, _TL("Panchromatic Channel Matching"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
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
	TSG_Grid_Resampling	Interpolation	= Get_Interpolation(Parameters("RESAMPLING")->asInt());

	//-----------------------------------------------------
	int			y;
	double		r, g, b, i, h, s;
	CSG_Grid	*pPan, *pR, *pG, *pB;

	pPan	= Parameters("PAN")->asGrid();

	//-----------------------------------------------------
	Process_Set_Text(CSG_String::Format(SG_T("%s: %s ..."), _TL("Resampling"), Parameters("R")->asGrid()->Get_Name()));
	pR	= Parameters("R_SHARP")->asGrid();
	pR	->Assign  (Parameters("R")->asGrid(), Interpolation);
	pR	->Set_Name(Parameters("R")->asGrid()->Get_Name());

	Process_Set_Text(CSG_String::Format(SG_T("%s: %s ..."), _TL("Resampling"), Parameters("G")->asGrid()->Get_Name()));
	pG	= Parameters("G_SHARP")->asGrid();
	pG	->Assign  (Parameters("G")->asGrid(), Interpolation);
	pG	->Set_Name(Parameters("G")->asGrid()->Get_Name());

	Process_Set_Text(CSG_String::Format(SG_T("%s: %s ..."), _TL("Resampling"), Parameters("B")->asGrid()->Get_Name()));
	pB	= Parameters("B_SHARP")->asGrid();
	pB	->Assign  (Parameters("B")->asGrid(), Interpolation);
	pB	->Set_Name(Parameters("B")->asGrid()->Get_Name());

	//-----------------------------------------------------
	Process_Set_Text(_TL("RGB to IHS"));

	double	rMin	= pR->Get_ZMin(),	rRange	= pR->Get_ZRange();
	double	gMin	= pG->Get_ZMin(),	gRange	= pG->Get_ZRange();
	double	bMin	= pB->Get_ZMin(),	bRange	= pB->Get_ZRange();

	for(y=0; y<pPan->Get_NY() && Set_Progress(y, pPan->Get_NY()); y++)
	{
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
				r	= (pR->asDouble(x, y) - rMin) / rRange;	if( r < 0.0 ) r = 0.0; else if( r > 1.0 ) r = 1.0;
				g	= (pG->asDouble(x, y) - gMin) / gRange;	if( g < 0.0 ) g = 0.0; else if( g > 1.0 ) g = 1.0;
				b	= (pB->asDouble(x, y) - bMin) / bRange;	if( b < 0.0 ) b = 0.0; else if( b > 1.0 ) b = 1.0;

				i	= r + g + b;
				
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
		Offset_Pan	= pPan->Get_ZMin();
		Offset		= pR->Get_ZMin();
		Scale		= pR->Get_ZRange() / pPan->Get_ZRange();
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
		for(int x=0; x<pPan->Get_NX(); x++)
		{
			if( !pR->is_NoData(x, y) )
			{
				i	= Offset + Scale * (pPan->asDouble(x, y) - Offset_Pan);
				s	= pG  ->asDouble(x, y);
				h	= pB  ->asDouble(x, y);

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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Colour Normalized Brovey Sharpening"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Colour normalized (Brovey) sharpening.\n"
		"\n"
		"References:\n"
		"Vrabel, J. (1996): Multispectral Imagery Band Sharpening Study. "
		"Photogrammetric Engineering & Remote Sensing, Vol. 62, No. 9, pp. 1075-1083.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(NULL	, "R"		, _TL("Red")	, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL	, "G"		, _TL("Green")	, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL	, "B"		, _TL("Blue")	, _TL(""), PARAMETER_INPUT);

	pNode	= Parameters.Add_Grid_System(
		NULL	, "PAN_SYSTEM"	, _TL("High Resolution Grid System"),
		_TL("")
	);

	Parameters.Add_Grid(
		pNode	, "PAN"			, _TL("Panchromatic Channel"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(pNode	, "R_SHARP"	, _TL("Red")	, _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid(pNode	, "G_SHARP"	, _TL("Green")	, _TL(""), PARAMETER_OUTPUT, false);
	Parameters.Add_Grid(pNode	, "B_SHARP"	, _TL("Blue")	, _TL(""), PARAMETER_OUTPUT, false);

	Parameters.Add_Grid_List(
		NULL	, "SHARPEN"		, _TL("Sharpened Channels"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
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
	TSG_Grid_Resampling	Interpolation	= Get_Interpolation(Parameters("RESAMPLING")->asInt());

	//-----------------------------------------------------
	CSG_Grid	*pPan, *pR, *pG, *pB;

	pPan	= Parameters("PAN")->asGrid();

	//-----------------------------------------------------
	Process_Set_Text(CSG_String::Format(SG_T("%s: %s ..."), _TL("Resampling"), Parameters("R")->asGrid()->Get_Name()));
	pR	= Parameters("R_SHARP")->asGrid();
	pR	->Assign  (Parameters("R")->asGrid(), Interpolation);
	pR	->Set_Name(Parameters("R")->asGrid()->Get_Name());

	Process_Set_Text(CSG_String::Format(SG_T("%s: %s ..."), _TL("Resampling"), Parameters("G")->asGrid()->Get_Name()));
	pG	= Parameters("G_SHARP")->asGrid();
	pG	->Assign  (Parameters("G")->asGrid(), Interpolation);
	pG	->Set_Name(Parameters("G")->asGrid()->Get_Name());

	Process_Set_Text(CSG_String::Format(SG_T("%s: %s ..."), _TL("Resampling"), Parameters("B")->asGrid()->Get_Name()));
	pB	= Parameters("B_SHARP")->asGrid();
	pB	->Assign  (Parameters("B")->asGrid(), Interpolation);
	pB	->Set_Name(Parameters("B")->asGrid()->Get_Name());

	//-----------------------------------------------------
	Process_Set_Text(_TL("Sharpening"));

	for(int y=0; y<pPan->Get_NY() && Set_Progress(y, pPan->Get_NY()); y++)
	{
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Colour Normalized Spectral Sharpening"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Colour normalized spectral sharpening.\n"
		"\n"
		"References:\n"
		"Vrabel, J., Doraiswamy, P., McMurtrey, J., Stern, A. (2002): "
		"Demonstration of the Accuracy of Improved Resolution Hyperspectral Imagery. "
		"SPIE Symposium Proceedings.\n"
		"\n"
		"Vrabel, J., Doraiswamy, P., Stern, A. (2002): "
		"Application of Hyperspectral Imagery Resolution Improvement for Site-Specific Farming. "
		"ASPRS 2002 Conference Proceedings.\n"
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid_System(
		NULL	, "LO_RES"		, _TL("Low Resolution Grid System"),
		_TL("")
	);

	Parameters.Add_Grid_List(
		pNode	, "GRIDS"		, _TL("Original Channels"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(
		NULL	, "PAN"			, _TL("Panchromatic Channel"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "SHARPEN"		, _TL("Sharpened Channels"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
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
	TSG_Grid_Resampling	Interpolation	= Get_Interpolation(Parameters("RESAMPLING")->asInt());

	//-----------------------------------------------------
	int						i;
	CSG_Grid				*pPan;
	CSG_Parameter_Grid_List	*pGrids, *pSharp;
	
	pPan	= Parameters("PAN"    )->asGrid();
	pGrids	= Parameters("GRIDS"  )->asGridList();
	pSharp	= Parameters("SHARPEN")->asGridList();

	//-----------------------------------------------------
	pSharp->Del_Items();

	for(i=0; i<pGrids->Get_Count(); i++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s: %s ..."), _TL("Resampling"), pGrids->asGrid(i)->Get_Name()));

		CSG_Grid	*pGrid	= SG_Create_Grid(*Get_System());

		pGrid->Set_Name (pGrids->asGrid(i)->Get_Name());
		pGrid->Assign   (pGrids->asGrid(i), Interpolation);

		pSharp->Add_Item(pGrid);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	Sum	= 0.0;

			if( !pPan->is_NoData(x, y) )
			{
				for(i=0; i<pSharp->Get_Count(); i++)
				{
					if( !pSharp->asGrid(i)->is_NoData(x, y) )
					{
						Sum	+= pSharp->asGrid(i)->asDouble (x, y);
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
				Sum	= pPan->asDouble(x, y) * pSharp->Get_Count() / (Sum + pSharp->Get_Count());

				for(i=0; i<pSharp->Get_Count(); i++)
				{
					pSharp->asGrid(i)->Mul_Value(x, y, Sum);
				}
			}
			else
			{
				for(i=0; i<pSharp->Get_Count(); i++)
				{
					pSharp->asGrid(i)->Set_NoData(x, y);
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Principle Components Based Image Sharpening"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Principle components based image sharpening."
	));

	//-----------------------------------------------------
	pNode	= Parameters.Add_Grid_System(
		NULL	, "LO_RES"		, _TL("Low Resolution Grid System"),
		_TL("")
	);

	Parameters.Add_Grid_List(
		pNode	, "GRIDS"		, _TL("Original Channels"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(
		NULL	, "PAN"			, _TL("Panchromatic Channel"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "SHARPEN"		, _TL("Sharpened Channels"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("correlation matrix"),
			_TL("variance-covariance matrix"),
			_TL("sums-of-squares-and-cross-products matrix")
		), 2
	);

	Parameters.Add_Choice(
		NULL	, "RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("nearest neighbour"),
			_TL("bilinear"),
			_TL("cubic convolution")
		), 2
	);

	Parameters.Add_Choice(
		NULL	, "PAN_MATCH"	, _TL("Panchromatic Channel Matching"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("normalized"),
			_TL("standardized")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "OVERWRITE"	, _TL("Overwrite"),
		_TL("overwrite previous output if adequate"),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RUN_TOOL(BRETVAL, LIBRARY, TOOL, PARAMETERS, GRIDSYSTEM, CONDITION)	{\
	\
	BRETVAL	= false;\
	\
	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool(SG_T(LIBRARY), TOOL);\
	\
	if(	pTool == NULL )\
	{\
		Error_Fmt("%s: %s", _TL("could not find tool"), SG_T(LIBRARY));\
	}\
	else if( pTool->is_Grid() )\
	{\
		Process_Set_Text(pTool->Get_Name());\
		\
		pTool->Settings_Push();\
		\
		((CSG_Tool_Grid *)pTool)->Get_System()->Assign(GRIDSYSTEM);\
		\
		if( !(CONDITION) )\
		{\
			Error_Fmt("%s: %s.%s", _TL("could not initialize tool"), SG_T(LIBRARY), pTool->Get_Name().c_str());\
		}\
		else if( !pTool->Execute() )\
		{\
			Error_Fmt("%s: %s.%s", _TL("could not execute tool"   ), SG_T(LIBRARY), pTool->Get_Name().c_str());\
		}\
		else\
		{\
			BRETVAL	= true;\
		}\
		\
		PARAMETERS.Assign(pTool->Get_Parameters());\
		\
		pTool->Settings_Pop();\
	}\
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
	// get the principle components for the low resolution bands

	RUN_TOOL(bResult, "statistics_grid", 8, Tool_Parms, *Parameters("GRIDS")->Get_Parent()->asGrid_System(),
			SG_TOOL_PARAMETER_SET("GRIDS" , Parameters("GRIDS"))
		&&	SG_TOOL_PARAMETER_SET("METHOD", Parameters("METHOD"))
		&&	SG_TOOL_PARAMETER_SET("EIGEN" , &Eigen)
		&&	SG_TOOL_PARAMETER_SET("NFIRST", 0)	// get all components
	);

	if( !bResult )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pPCA	= Tool_Parms.Get_Parameter("PCA")->asGridList();

	int			i, n	= pPCA->Get_Count();

	CSG_Grid	*PCA	= new CSG_Grid[n];
	CSG_Grid	*pPan	= Parameters("PAN")->asGrid();

	//-----------------------------------------------------
	// replace first principle component with the high resolution panchromatic band

	Process_Set_Text(_TL("Replace first PC with PAN"));

	double	Offset_Pan, Offset, Scale;

	if( Parameters("PAN_MATCH")->asInt() == 0 )	// scale PAN band to fit first PC histogram
	{
		Offset_Pan	= pPan->Get_ZMin();
		Offset		= pPCA->asGrid(0)->Get_ZMin();
		Scale		= pPCA->asGrid(0)->Get_ZRange() / pPan->Get_ZRange();
	}
	else
	{
		Offset_Pan	= pPan->Get_Mean();
		Offset		= pPCA->asGrid(0)->Get_Mean();
		Scale		= pPCA->asGrid(0)->Get_StdDev() / pPan->Get_StdDev();
	}

	PCA[0].Create(*Get_System());

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

	TSG_Grid_Resampling	Interpolation	= Get_Interpolation(Parameters("RESAMPLING")->asInt());

	for(i=1; i<n; i++)
	{
		Process_Set_Text(CSG_String::Format("%s: %s ...", _TL("Resampling"), pPCA->asGrid(i)->Get_Name()));

		PCA[i].Create(*Get_System());
		PCA[i].Assign(pPCA->asGrid(i), Interpolation);

		delete(pPCA->asGrid(i));	// PCA tool was unmanaged, so we have to delete the output
	}

	delete(pPCA->asGrid(0));

	pPCA->Del_Items();

	for(i=0; i<n; i++)
	{
		pPCA->Add_Item(&PCA[i]);
	}

	//-----------------------------------------------------
	// inverse principle component rotation for the high resolution bands

	RUN_TOOL(bResult, "statistics_grid", 10, Tool_Parms, *Get_System(),
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

	for(i=0; i<pLoRes->Get_Count() && i<pGrids->Get_Count(); i++)
	{
		if( pHiRes->asGrid(i) )
		{
			pHiRes->asGrid(i)->Assign(pGrids->asGrid(i));

			delete(pGrids->asGrid(i));
		}
		else
		{
			pHiRes->Add_Item(pGrids->asGrid(i));
		}

		pHiRes->asGrid(i)->Set_Name(pLoRes->asGrid(i)->Get_Name());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
