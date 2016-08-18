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
//                     shapes_lines                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  line_smoothing.cpp                   //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "line_smoothing.h"

#define MIN_WEIGHT	0.0005
#define EPSILON		1.0e-07


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLine_Smoothing::CLine_Smoothing(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Line Smoothing"));

	Set_Author		(SG_T("V. Wichmann, J. Engels (c) 2014"));

	Set_Description	(_TW(
		"The tool provides methods for line smoothing including iterative averaging "
		"(SIA) and Gaussian filtering.\n\n"
		
		"Iterative averaging (SIA) is described by Mansouryar & Hedayati (2012). "
		"A higher smoothing sensitivity results in a "
		"stronger smoothing in less iterations and vice versa. The 'improved SIA "
		"model' simply applies a preservation factor in the first iteration and "
		"then runs the 'basic SIA model' for the following iterations.\n\n"

		"Gaussian filtering with shrinkage correction is described by Lowe (1989).\n\n"

		"In case the density of line vertices is too high, the 'Line Simplification' "
		"tool can be applied first. If the density of line vertices is too low, "
		"additional vertices can be inserted by applying the 'Convert Lines to Points' "
		"and the 'Convert Points to Line(s)' tools prior to smoothing.\n\n"

		"References:\n"
		"Lowe, D. (1989): Organization of Smooth Image Curves at Multiple Scales. "
		"International Journal of Computer Vision, 3: 119-130. "
		"(<a target=\"_blank\" href=\"http://www.cs.ubc.ca/~lowe/papers/iccv88.pdf\">pdf</a>)\n\n"

		"Mansouryar, M. & Hedayati, A. (2012): Smoothing Via Iterative Averaging (SIA) "
		"- A Basic Technique for Line Smoothing. International Journal of Computer and "
		"Electrical Engineering Vol. 4, No. 3: 307-311. "
		"(<a target=\"_blank\" href=\"http://www.ijcee.org/papers/501-P063.pdf\">pdf</a>)\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "LINES_IN"		, _TL("Lines"),
		_TL("The input line shapefile to smooth."),
		PARAMETER_INPUT, SHAPE_TYPE_Line
	);
	
	Parameters.Add_Shapes(
		NULL	, "LINES_OUT"	, _TL("Smoothed Lines"),
		_TL("The smoothed output line shapefile."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL("Choose the method to apply."),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("basic SIA model"),
			_TL("improved SIA model"),
			_TL("Gaussian Filtering")
		), 2
	);
	
	Parameters.Add_Value(
		NULL	, "SENSITIVITY"	, _TL("Sensitivity"),
		_TL("Half the size of the moving window [vertex count], controls smoothing sensitivity."),
		PARAMETER_TYPE_Int, 3, 1, true
	);

	Parameters.Add_Value(
		NULL	, "ITERATIONS"	, _TL("Iterations"),
		_TL("The number of smoothing iterations [-]."),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	Parameters.Add_Value(
		NULL	, "PRESERVATION"	, _TL("Preservation"),
		_TL("The smoothing preservation factor [-]."),
		PARAMETER_TYPE_Double, 10.0, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "SIGMA"	, _TL("Sigma"),
		_TL("Standard deviation of the Gaussian filter [-]."),
		PARAMETER_TYPE_Double, 2.0, 0.5, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CLine_Smoothing::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("SENSITIVITY",		pParameter->asInt() <= 1);
		pParameters->Set_Enabled("ITERATIONS",		pParameter->asInt() <= 1);
		pParameters->Set_Enabled("PRESERVATION",	pParameter->asInt() == 1);
		pParameters->Set_Enabled("SIGMA",			pParameter->asInt() == 2);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLine_Smoothing::On_Execute(void)
{
	CSG_Shapes	*pLines, *pSmooth;
	int			iMethod, iSS, iSI;
	double		dSP, dSigma;
	bool		bResult;


	pLines		= Parameters("LINES_IN")->asShapes();
	pSmooth		= Parameters("LINES_OUT")->asShapes();
	iMethod		= Parameters("METHOD")->asInt();
	iSS			= Parameters("SENSITIVITY")->asInt();
	iSI			= Parameters("ITERATIONS")->asInt();
	dSP			= Parameters("PRESERVATION")->asDouble();
	dSigma		= Parameters("SIGMA")->asDouble();


	//--------------------------------------------------------
	pSmooth->Create(SHAPE_TYPE_Line, CSG_String::Format(SG_T("%s_smoothed"), pLines->Get_Name()), pLines, pLines->Get_Vertex_Type());


	if( iMethod <= 1 )
	{
		bResult = Calc_SIA(pLines, pSmooth, iMethod, iSS, iSI, dSP);
	}
	else
	{
		bResult = Calc_Gaussian(pLines, pSmooth, dSigma);
	}


	return( bResult );
}


//---------------------------------------------------------
bool CLine_Smoothing::Calc_SIA(CSG_Shapes *pLines, CSG_Shapes *pSmooth, int iMethod, int iSS, int iSI, double dSP)
{
	int		iOffset;


	if(	iSS > iSI )
	{
		SG_UI_Msg_Add(_TL("WARNING: smoothing sensitivity is greater than the number of iterations!"), true);
	}

	if(	iMethod == 1 && iSI < 2 )
	{
		SG_UI_Msg_Add_Error(_TL("The improved SIA model requires more than one smoothing iteration!"));
		return( false );
	}


	//--------------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
	{
		CSG_Shape *pLine = pLines->Get_Shape(iLine);
		
		CSG_Shape *pSmoothed = pSmooth->Add_Shape(pLine, SHAPE_COPY_ATTR);


		//--------------------------------------------------------
		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			std::vector<TSG_Point>	verticesA(pLine->Get_Point_Count(iPart));
			std::vector<TSG_Point>	verticesB(pLine->Get_Point_Count(iPart));
			TSG_Point				p;

			for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				p.x	= pLine->Get_Point(iPoint, iPart).x;
				p.y	= pLine->Get_Point(iPoint, iPart).y;

				verticesB.at(iPoint) = p;
			}


			//--------------------------------------------------------
			for(int i=0; i<iSI; i++)
			{
				verticesA = verticesB;

				for(int iPoint=0; iPoint<(int)verticesA.size(); iPoint++)
				{
					if( iPoint < iSS )
					{
						iOffset = iPoint;
					}
					else if( iSS <= iPoint && iPoint < (int)verticesA.size() - iSS )
					{
						iOffset = iSS;
					}
					else // if( iPoint >= (int)verticesA.size() - iSS )
					{
						iOffset = (int)verticesA.size() - 1 - iPoint;
					}

					if( iPoint - iOffset < 0 )	// validate lower ...
					{
						iOffset = 0;
					}
					if( iPoint + iOffset >= verticesA.size() )	// ... and upper limit
					{
						iOffset = (int)verticesA.size() - 1 - iPoint;
					}


					if( iMethod == 1 && i == 0 )	// improved SIA, apply preservation in first iteration
					{
						p.x = (verticesA.at(iPoint - iOffset).x + verticesA.at(iPoint).x + verticesA.at(iPoint + iOffset).x) / 3.0;
						p.y = (verticesA.at(iPoint - iOffset).y + verticesA.at(iPoint).y + verticesA.at(iPoint + iOffset).y) / 3.0;

						TSG_Point	p_GA;	// vector GA (triangle centroid - point)

						p_GA.x = verticesA.at(iPoint).x - p.x;
						p_GA.y = verticesA.at(iPoint).y - p.y;

						double dLength = sqrt(p_GA.x * p_GA.x + p_GA.y * p_GA.y);

						if( dLength > 0.0 )
						{
							double dScaled = dLength * dSP;	// scale by preservation

							p.x = verticesA.at(iPoint).x + (p_GA.x / dLength) * dScaled;	// shift point
							p.y = verticesA.at(iPoint).y + (p_GA.y / dLength) * dScaled;
						}
						else
						{
							p = verticesA.at(iPoint);
						}
					}
					else	// basic SIA
					{
						p.x = (verticesA.at(iPoint - iOffset).x + verticesA.at(iPoint).x + verticesA.at(iPoint + iOffset).x) / 3.0;
						p.y = (verticesA.at(iPoint - iOffset).y + verticesA.at(iPoint).y + verticesA.at(iPoint + iOffset).y) / 3.0;
					}

					verticesB.at(iPoint) = p;
				} // iPoint
			} // iIteration


			//--------------------------------------------------------
			for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
			{
				pSmoothed->Add_Point(verticesB.at(iPoint).x, verticesB.at(iPoint).y, iPart);
				
				if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
				{
					pSmoothed->Set_Z(pLine->Get_Z(iPoint, iPart), iPoint, iPart);

					if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
					{
						pSmoothed->Set_M(pLine->Get_M(iPoint, iPart), iPoint, iPart);
					}
				}
			}	
		} // iPart
	} // iLine


	return( true );
}


//---------------------------------------------------------
bool CLine_Smoothing::Calc_Gaussian(CSG_Shapes *pLines, CSG_Shapes *pSmooth, double dSigma)
{
	int			half_window_width;
	bool		bClosed;
	double      variance;

	variance			= dSigma * dSigma;
	double t0d			= sqrt (-2.0 * variance * log ( MIN_WEIGHT ));
	half_window_width	= (int) floor (t0d);
	double nfac			= 1.0 / dSigma / sqrt (2.0 * M_PI);
	
	if(	2 * half_window_width + 1 < 3 )
	{
		SG_UI_Msg_Add_Error(_TL("The length of the Gaussian window is too small, please provide a larger sigma."));
		return( false );
	}

	std::vector<double>	gauss_window(2 * half_window_width + 1);
	std::vector<double>	first_deriv_gauss_window(2 * half_window_width + 1);
	std::vector<double>	snd_deriv_gauss_window(2 * half_window_width + 1);
	std::vector<double>	gauss_window_corr(2 * half_window_width + 1);
	std::vector<double>	snd_deriv_gauss_window_corr(2 * half_window_width + 1);

	double zero_sum_weights	= 0.0 ;
	double snd_sum_weights	= 0.0 ;


	SG_UI_Msg_Add(CSG_String::Format(_TL("Length of the Gaussian window:\t %d"), 2 * half_window_width + 1), true);

	for (int k=-half_window_width, l=0; k<=half_window_width; k++, l++)
	{
		double kd = (double)k;

		gauss_window [l] = nfac * exp (-kd * kd / (2.0 * variance));
		zero_sum_weights += gauss_window [l];

		first_deriv_gauss_window [l] = -nfac * kd * exp (-kd * kd / (2.0 * variance)) / variance;

		snd_deriv_gauss_window [l] = nfac * (kd * kd / variance - 1.0) * exp (-kd * kd / (2.0 * variance)) / variance;
		snd_sum_weights += snd_deriv_gauss_window [l];
	}

	// The filter kernels have to be corrected because of the discretization
	// The sum of the elements of the first-derivative-kernel is zero due to antisymmetry
	
	for (int k=-half_window_width, l=0; k<=half_window_width; k++, l++)
	{
		double kd = (double) k ;

		gauss_window_corr [l] = gauss_window [l] / zero_sum_weights ;

		snd_deriv_gauss_window_corr [l] = snd_deriv_gauss_window [l] - snd_sum_weights * gauss_window_corr [l] ;
	}


	//--------------------------------------------------------
	for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress (iLine, pLines->Get_Count ()); iLine++)
	{
		CSG_Shape *pLine = pLines->Get_Shape(iLine);
		
		CSG_Shape *pSmoothed = pSmooth->Add_Shape(pLine, SHAPE_COPY_ATTR);

		//--------------------------------------------------------
		for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
		{
			int curve_length = pLine -> Get_Point_Count(iPart);

			std::vector<TSG_Point>	verticesA (curve_length) ;  // the original points
			std::vector<TSG_Point>	verticesB (curve_length) ;  // the intermediate points (after smoothing)
			std::vector<TSG_Point>	verticesC (curve_length) ;  // the points after smoothing and shrinkage correction
			TSG_Point				p;

			if (pLine->Get_Point(0, iPart).x == pLine->Get_Point(curve_length - 1, iPart).x &&
				pLine->Get_Point(0, iPart).y == pLine->Get_Point(curve_length - 1, iPart).y )
			{
				bClosed = true;
				curve_length--;
			}
			else
				bClosed = false;


			double mean_X = 0.0;
			double mean_Y = 0.0;

			for(int iPoint=0; iPoint<curve_length; iPoint++)
			{
				mean_X += (pLine->Get_Point(iPoint, iPart).x);
				mean_Y += (pLine->Get_Point(iPoint, iPart).y);
			}

			mean_X /= curve_length;
			mean_Y /= curve_length;

			for(int iPoint=0; iPoint<curve_length; iPoint++)
			{
				p.x	= pLine->Get_Point(iPoint, iPart).x - mean_X;
				p.y	= pLine->Get_Point(iPoint, iPart).y - mean_Y;

				verticesA.at(iPoint) = p;
			}

			if( curve_length < 2 * half_window_width + 1 )
			{
				SG_UI_Msg_Add(_TL("WARNING: unable to smooth line because Gaussian window too large (number of line vertices too small or sigma too big)."), true);

				for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
				{
					pSmoothed->Add_Point(pLine->Get_Point(iPoint, iPart).x, pLine->Get_Point(iPoint, iPart).y, iPart);
				
					if( pLines->Get_Vertex_Type() != SG_VERTEX_TYPE_XY )
					{
						pSmoothed->Set_Z(pLine->Get_Z(iPoint, iPart), iPoint, iPart);

						if( pLines->Get_Vertex_Type() == SG_VERTEX_TYPE_XYZM )
						{
							pSmoothed->Set_M(pLine->Get_M(iPoint, iPart), iPoint, iPart);
						}
					}
				}	

				continue;
			}

			for(int iPoint=0; iPoint<curve_length; iPoint++)
			{
				double value_x = 0.0;
				double value_y = 0.0;

				double first_deriv_x = 0.0;
				double first_deriv_y = 0.0;

				double snd_deriv_x = 0.0;
				double snd_deriv_y = 0.0;

				double mean_loc_x = verticesA.at(iPoint).x;
				double mean_loc_y = verticesA.at(iPoint).y;
				int loc_hw;

			    if( (iPoint == 0 || iPoint == curve_length - 1) && bClosed == false)
				{
					verticesB.at(iPoint) = verticesA.at(iPoint);
					verticesC.at(iPoint) = verticesA.at(iPoint);
					continue;
				}
				else if( (iPoint < half_window_width || iPoint > curve_length - half_window_width - 1) && bClosed == false)
				{
				    if( iPoint < half_window_width )
						loc_hw = iPoint ;
					else
						loc_hw = curve_length - 1 - iPoint ;

					// same story as before, but with different window length

					std::vector<double>	gwloc(2 * loc_hw + 1);
					std::vector<double>	first_deriv_gwloc(2 * loc_hw + 1);
					std::vector<double>	snd_deriv_gwloc(2 * loc_hw + 1);

					std::vector<double>	gwloc_corr(2 * loc_hw + 1);
					std::vector<double>	first_deriv_gwloc_corr(2 * loc_hw + 1);
					std::vector<double>	snd_deriv_gwloc_corr(2 * loc_hw + 1);

					double zero_sum_loc = 0.0;
					double snd_sum_loc = 0.0;

					for(int k=-loc_hw, l=0; k<=loc_hw; k++, l++)
					{
						double kd = (double)k;
						gwloc [l] = nfac * exp (-kd * kd / (2.0 * variance));
						first_deriv_gwloc [l] = -kd * nfac * exp (-kd * kd / (2.0 * variance)) / variance;
						snd_deriv_gwloc [l] = nfac * (kd * kd / variance - 1.0) * exp (-kd * kd / (2.0 * variance)) / variance;

						zero_sum_loc += gwloc [l];
						snd_sum_loc += snd_deriv_gwloc [l];
					}


					// The filter kernels have to be corrected because of the discretization

					double first_k_sum_loc = 0.0 ;

					for(int k=-loc_hw, l=0; k<=loc_hw; k++, l++)
					{
						double kd = (double)k;
						gwloc_corr [l] = gwloc [l] / zero_sum_loc;

						snd_deriv_gwloc_corr [l] = snd_deriv_gwloc [l] - snd_sum_loc * gwloc_corr [l];

						// attention: it is convolution not correlation, therefore the minus sign
						first_k_sum_loc -= kd * first_deriv_gwloc [l];
					}

					double snd_kk_sum_loc = 0.0;

					for(int k=-loc_hw, l=0; k<=loc_hw; k++, l++)
					{
						double kd = (double)k;
						first_deriv_gwloc_corr [l] = first_deriv_gwloc [l] / first_k_sum_loc;

						snd_kk_sum_loc += kd * kd * snd_deriv_gwloc_corr [l];
					}


					for(int k=-loc_hw, l=0; k<=loc_hw; k++, l++)
					{
						snd_deriv_gwloc_corr [l] *= (2.0 / snd_kk_sum_loc);
					}


					std::vector<double>	values_red_x(2 * half_window_width + 1);
					std::vector<double>	values_red_y(2 * half_window_width + 1);


					for(int l=iPoint - loc_hw, k=0; l<=iPoint + loc_hw; k++, l++)
					{
						values_red_x [k] = verticesA.at(l).x - mean_loc_x;
						values_red_y [k] = verticesA.at(l).y - mean_loc_y;
					}

					
					int lstart = iPoint - loc_hw;
					int lend   = iPoint + loc_hw;

					for(int i=0, l=lstart; l<=lend; i++, l++)
					{
						value_x += gwloc_corr [i] * values_red_x [i];
						value_y += gwloc_corr [i] * values_red_y [i];

						// attention: it is convolution not correlation, and the first-derivative-kernel is antisymmetric

						first_deriv_x -= first_deriv_gwloc [i] * values_red_x [i];
						first_deriv_y -= first_deriv_gwloc [i] * values_red_y [i];

						snd_deriv_x += snd_deriv_gwloc_corr [i] * values_red_x [i];
						snd_deriv_y += snd_deriv_gwloc_corr [i] * values_red_y [i];
					}

					value_x += mean_loc_x;
					value_y += mean_loc_y;

					if( loc_hw <= 3 )
					{
						p.x = value_x;
						p.y = value_y;
						verticesB.at(iPoint) = p;
						verticesC.at(iPoint) = p;
						continue;
					}
				}				
				else     // if( (half_window_width <= iPoint && iPoint < curve_length - half_window_width) || bClosed == true )
				{
					std::vector<double>	values_red_x(2 * half_window_width + 1);
					std::vector<double>	values_red_y(2 * half_window_width + 1);

					loc_hw = half_window_width;

					for(int l=iPoint - half_window_width, k=0; l<=iPoint + half_window_width; k++, l++)
					{
						int leff = l;

						if( l < 0 ) 
							leff = curve_length + l ;
						else if( l >= curve_length ) 
							leff = l - curve_length ;

						values_red_x [k] = verticesA.at(leff).x - mean_loc_x;
						values_red_y [k] = verticesA.at(leff).y - mean_loc_y;
					}

					for(int l=iPoint - half_window_width, k=0; l<=iPoint + half_window_width; k++, l++)
					{
						value_x += gauss_window [k] * values_red_x [k];
						value_y += gauss_window [k] * values_red_y [k];

						first_deriv_x -= first_deriv_gauss_window [k] * values_red_x [k];
						first_deriv_y -= first_deriv_gauss_window [k] * values_red_y [k];

						snd_deriv_x += snd_deriv_gauss_window_corr [k] * values_red_x [k];
						snd_deriv_y += snd_deriv_gauss_window_corr [k] * values_red_y [k];
					}

					value_x += mean_loc_x;
					value_y += mean_loc_y;
				}
		

				double denom = first_deriv_x * first_deriv_x + first_deriv_y * first_deriv_y;

				if( abs(denom) < EPSILON )
				{
					Error_Set(_TL("First Derivative is singular! Probably the shape containes consecutive identical points.\n\n"));

					return( false );
				}

				double sqrt_denom = sqrt (denom);
				double kappa = (first_deriv_x * snd_deriv_y - first_deriv_y * snd_deriv_x) / (denom * sqrt_denom);

				p.x = value_x;
				p.y = value_y;
				verticesB.at(iPoint) = p;


				if( abs(kappa) < EPSILON )
				{
					verticesC.at(iPoint) = verticesB.at(iPoint);
					continue;
				}

				// Newton's method for the determination of the (original) curvature radius

				double rsmooth = 1.0 / kappa;
				double r = rsmooth;

				for(int k=0; ; k++)
				{
					double efunc = exp (- denom * variance / (2.0 * r * r));
					double f = r * efunc - rsmooth;
					double fs = efunc * (1.0 + denom * variance / r / r);

					if( abs(fs) < EPSILON )
					{
						r = rsmooth;
						break;
					}

					r = r - f / fs;

					if( k == 5 || abs(f) < EPSILON )
					{
						break;
					}
				}


				first_deriv_x /= sqrt_denom;
				first_deriv_y /= sqrt_denom;

				value_x  += first_deriv_y * (r - rsmooth);
				value_y  += first_deriv_x * (rsmooth - r);

				p.x = value_x;
				p.y = value_y;
				verticesC.at(iPoint) = p;
			} // iPoint


			//--------------------------------------------------------
			for(int iPoint = 0 ; iPoint<curve_length; iPoint++)
			{
				pSmoothed->Add_Point(verticesC.at(iPoint).x + mean_X, verticesC.at(iPoint).y + mean_Y, iPart);
				
				if( pLines -> Get_Vertex_Type () != SG_VERTEX_TYPE_XY )
				{
					pSmoothed-> Set_Z (pLine->Get_Z (iPoint, iPart), iPoint, iPart);

					if( pLines -> Get_Vertex_Type () == SG_VERTEX_TYPE_XYZM )
					{
						pSmoothed -> Set_M (pLine -> Get_M (iPoint, iPart), iPoint, iPart);
					}
				}
			}	
		} // iPart
	} // iLine

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
