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
//                        VIGRA                          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      vigra.cpp                        //
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
#include "vigra_watershed.h"

//---------------------------------------------------------
#include <iostream>

#include <vigra/stdimagefunctions.hxx>
#include <vigra/localminmax.hxx>
#include <vigra/labelimage.hxx>
#include <vigra/seededregiongrowing.hxx>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CViGrA_Watershed::CViGrA_Watershed(void)
{
	Set_Name		(_TL("Watershed Segmentation (ViGrA)"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		"Note that the watershed algorithm usually results in an "
		"oversegmentation (i.e., too many regions), but its boundary "
		"localization is quite good.\n"
		"Based on the example code \"watershed.cxx\" by Ullrich Koethe.\n"
		"References:\n"
		"ViGrA - Vision with Generic Algorithms\n"
		"<a target=\"_blank\" href=\"http://hci.iwr.uni-heidelberg.de/vigra\">http://hci.iwr.uni-heidelberg.de</a>"
	));

	Parameters.Add_Grid(
		NULL	, "INPUT"	, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"	, _TL("Segmentation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "SCALE"	, _TL("Width of gradient filter"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "RGB"		, _TL("RGB coded data"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "EDGES"	, _TL("Edges"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_Watershed::On_Execute(void)
{
	bool		bEdges, bRGB;
	double		Scale;
	CSG_Grid	*pInput, *pOutput;

	pInput	= Parameters("INPUT")	->asGrid();
	pOutput	= Parameters("OUTPUT")	->asGrid();
	Scale	= Parameters("SCALE")	->asDouble();
	bEdges	= Parameters("EDGES")	->asBool();
	bRGB	= Parameters("RGB")		->asBool();

	//-----------------------------------------------------
	if( !bRGB )
	{
		vigra::FImage	Input, Output(Get_NX(), Get_NY());

		Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

		Segmentation(Input, Output, Scale, bEdges);

		Copy_Grid_VIGRA_to_SAGA(*pOutput, Output, false);
	}

	//-----------------------------------------------------
	else	// perform watershed segmentation on color image
	{
		vigra::BRGBImage	Input, Output(Get_NX(), Get_NY());

		Copy_RGBGrid_SAGA_to_VIGRA(*pInput, Input, true);

		Segmentation(Input, Output, Scale, bEdges);

		Copy_RGBGrid_VIGRA_to_SAGA(*pOutput, Output, false);
	}

	//-----------------------------------------------------
	pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), Get_Name().c_str()));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// define a functor that calsulates the squared magnitude
// of the gradient given the x/y components of the gradient
//---------------------------------------------------------
struct GradientSquaredMagnitudeFunctor
{
	float operator()(float const & g1, float const & g2) const
	{
		return g1 * g1 + g2 * g2;
	}

	float operator()(vigra::RGBValue<float> const & rg1, vigra::RGBValue<float> const & rg2) const
	{
		float g1 = rg1.squaredMagnitude();
		float g2 = rg2.squaredMagnitude();

		return g1 + g2;
	}
};

//---------------------------------------------------------
// generic implementation of the watershed algorithm
//---------------------------------------------------------
template <class TImage_In, class TImage_Out>
void CViGrA_Watershed::Segmentation(TImage_In &Input, TImage_Out &Output, double Scale, bool bEdges)
{
	typedef typename vigra::NumericTraits<typename TImage_In::value_type>::RealPromote	TmpType;

	vigra::BasicImage<TmpType>	gradientx	(Get_NX(), Get_NY());
	vigra::BasicImage<TmpType>	gradienty	(Get_NX(), Get_NY());
	vigra::FImage				gradientmag	(Get_NX(), Get_NY());
	vigra::IImage				labels		(Get_NX(), Get_NY());

	//-----------------------------------------------------
	// calculate the x- and y-components of the image gradient at given scale
	Process_Set_Text(_TL("calculate gradients"));

	recursiveFirstDerivativeX	(srcImageRange(Input)		, destImage(gradientx), Scale);
	recursiveSmoothY			(srcImageRange(gradientx)	, destImage(gradientx), Scale);

	recursiveFirstDerivativeY	(srcImageRange(Input)		, destImage(gradienty), Scale);
	recursiveSmoothX			(srcImageRange(gradienty)	, destImage(gradienty), Scale);

	//-----------------------------------------------------
	// transform components into gradient magnitude
	Process_Set_Text(_TL("calculate gradient magnitude"));

	combineTwoImages(
		srcImageRange(gradientx),
		srcImage(gradienty),
		destImage(gradientmag),
		GradientSquaredMagnitudeFunctor()
	);

	//-----------------------------------------------------
	// find the local minima of the gradient magnitude (might be larger than one pixel)
	Process_Set_Text(_TL("find local minima"));

	labels	= 0;

	extendedLocalMinima(srcImageRange(gradientmag), destImage(labels), 1);

	//-----------------------------------------------------
	// label the minima just found
	Process_Set_Text(_TL("label minima"));

	int max_region_label	= labelImageWithBackground(srcImageRange(labels), destImage(labels), false, 0);

	//-----------------------------------------------------
	// create a statistics functor for region growing
	vigra::ArrayOfRegionStatistics<vigra::SeedRgDirectValueFunctor<float> >gradstat(max_region_label);

	//-----------------------------------------------------
	// perform region growing, starting from the minima of the gradient magnitude;
	// as the feature (first input) image contains the gradient magnitude,
	// this calculates the catchment basin of each minimum
	Process_Set_Text(_TL("perform region growing"));

	seededRegionGrowing(srcImageRange(gradientmag), srcImage(labels), destImage(labels), gradstat);

	//-----------------------------------------------------
	// initialize a functor to determine the average gray-value or color for each region (catchment basin) just found
	vigra::ArrayOfRegionStatistics<vigra::FindAverage<TmpType> >averages(max_region_label);

	//-----------------------------------------------------
	// calculate the averages
	Process_Set_Text(_TL("calculate averages"));

	inspectTwoImages(srcImageRange(Input), srcImage(labels), averages);

	//-----------------------------------------------------
	// write the averages into the destination image (the functor 'averages' acts as a look-up table)
	transformImage(srcImageRange(labels), destImage(Output), averages);

	//-----------------------------------------------------
	// mark the watersheds (region boundaries) black
	if( bEdges )
	{
		regionImageToEdgeImage(srcImageRange(labels), destImage(Output), vigra::NumericTraits<typename TImage_Out::value_type>::zero());
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
