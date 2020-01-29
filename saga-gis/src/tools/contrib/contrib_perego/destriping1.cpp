
///////////////////////////////////////////////////////////
//                                                       //
//                        Tool:                          //
//                      destriping                       //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    destriping1.cpp                    //
//                                                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                                                       //
//    by Alessandro Perego (Italy)                       //
//                                                       //
//    e-mail:     alper78@alice.it                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "destriping1.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
Cdestriping1::Cdestriping1(void)
{
	Set_Name		(_TL("Destriping"));

	Set_Author		("Alessandro Perego");

	Set_Description	(_TW(
		"Destriping filter removes straight parallel stripes in raster data. "
		"It uses two low-pass filters elongated in the stripes direction; "
		"the first one is 1 pixel unit wide while the second one is wide as the striping wavelength. "
		"Their difference is the striping error which is removed from the original data to obtain the destriped DEM. "
		"This method is equivalent to that proposed by Oimoen (2000). "
	));

	Add_Reference("Oimoen, M.J.", "2000",
		"An Effective Filter For Removal Of Production Artifacts",
		"In U.S. Geological Survey 7.5-Minute Digital Elevation Models. "
		"Proceedings of the Fourteenth International Conference on Applied Geologic Remote Sensing, "
		"6-8 November, Las Vegas, NV.",
		SG_T("http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.470.4960&rep=rep1&type=pdf"), SG_T("online")
	);

	Add_Reference("Perego, A.", "2009",
		"SRTM DEM destriping with SAGA GIS: consequences on drainage network extraction",
		"alsperGIS - Dati geografici e software per lo studio e la rappresentazione del territorio.",
		SG_T("http://www.alspergis.altervista.org/software/destriping.html"), SG_T("online")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "INPUT"  , _TL("Grid"            ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid  ("", "MASK"   , _TL("Mask"            ), _TL("Mask's no-data cells will be excluded from processing"), PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Grid  ("", "RESULT3", _TL("Destriped Grid"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "RESULT1", _TL("Low-pass 1"      ), _TL("Step 1: low-pass of stripe"                           ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid  ("", "RESULT2", _TL("Low-pass 2"      ), _TL("Step 2: low-pass between stripe and its surroundings" ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid  ("", "STRIPES", _TL("Stripes"         ), _TL("The difference between destriped and original grid"   ), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Double("", "ANG"    , _TL("Angle"           ), _TL("[Degree], 0 = horizontal, 90 = vertical"));
	Parameters.Add_Double("", "R"      , _TL("Radius"          ), _TL("[Cells]"), 20., 1., true);
	Parameters.Add_Double("", "D"      , _TL("Stripes Distance"), _TL("[Cells]"),  2., 2., true);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool Cdestriping1::is_Between(double Value, double Center, double Tolerance)
{
	return( Value >= (Center - Tolerance) && Value <= (Center + Tolerance) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Cdestriping1::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("INPUT")->asGrid();
	CSG_Grid	*pMask	= Parameters("MASK" )->asGrid();

	CSG_Grid	*pLowPass[2], *pStripes, *pDestriped = Parameters("RESULT3")->asGrid();

	DataObject_Set_Parameters(pDestriped , pGrid);

	pDestriped->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Destriped"));

	if( (pLowPass[0] = Parameters("RESULT1")->asGrid()) != NULL )
	{
		DataObject_Set_Parameters(pLowPass[0], pGrid);

		pLowPass[0]->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Low-pass 1"));
	}

	if( (pLowPass[1] = Parameters("RESULT2")->asGrid()) != NULL )
	{
		DataObject_Set_Parameters(pLowPass[1], pGrid);

		pLowPass[1]->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Low-pass 2"));
	}

	if( (pStripes   = Parameters("STRIPES" )->asGrid()) != NULL )
	{
		DataObject_Set_Colors(pStripes, 11, SG_COLORS_RED_GREY_BLUE);

		pStripes->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Stripes"));
	}

	//-----------------------------------------------------
	double	D	= Parameters("D")->asDouble();

	double	sin_a	= sin(Parameters("ANG")->asDouble() * M_DEG_TO_RAD);
	double	cos_a	= cos(Parameters("ANG")->asDouble() * M_DEG_TO_RAD);

	int		r_nx	= (int)(Parameters("R")->asDouble() * fabs(cos_a) + 0.5 * D * fabs(sin_a));
	int		r_ny	= (int)(Parameters("R")->asDouble() * fabs(sin_a) + 0.5 * D * fabs(cos_a));

	double	t[2];

	if( fabs(sin_a) >= fabs(cos_a) )
	{
		t[0]	= fabs(    0.5 / sin_a);
		t[1]	= fabs(D * 0.5 / sin_a);
	}
	else // if( fabs(sin_a) >= fabs(cos_a) )
	{
		t[0]	= fabs(    0.5 / cos_a);
		t[1]	= fabs(D * 0.5 / cos_a);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Simple_Statistics	s[2];

			if( !pGrid->is_NoData(x, y) && (!pMask || !pMask->is_NoData(x, y)) )
			{
				int	rx[2], ry[2];

				rx[0]	= x - r_nx; if( rx[0] <         0 ) { rx[0] =            0; }
				rx[1]	= x + r_nx; if( rx[1] >= Get_NX() ) { rx[1] = Get_NX() - 1; }
				ry[0]	= y - r_ny; if( ry[0] <         0 ) { ry[0] =            0; }
				ry[1]	= y + r_ny; if( ry[1] >= Get_NY() ) { ry[1] = Get_NY() - 1; }

				//-----------------------------------------
				if( fabs(sin_a) >= fabs(cos_a) )
				{
					for(int iy=ry[0]; iy<=ry[1]; iy++)
					{
						double	dy	= (y - iy) * cos_a / sin_a;

						for(int ix=rx[0]; ix<=rx[1]; ix++)
						{
							if( !pGrid->is_NoData(ix, iy) && (!pMask || !pMask->is_NoData(ix, iy)) )
							{
								double	dx	= (x - ix);

								if( is_Between(dx, dy, t[0]) ) { s[0] += pGrid->asDouble(ix, iy); }
								if( is_Between(dx, dy, t[1]) ) { s[1] += pGrid->asDouble(ix, iy); }
							}
						}
					}
				}
				else // if( fabs(sin_a) >= fabs(cos_a) )
				{
					for(int ix=rx[0]; ix<=rx[1]; ix++)
					{
						double	dx	= (x - ix) * sin_a / cos_a;

						for(int iy=ry[0], dy=y-ry[0]; iy<=ry[1]; iy++, dy++)
						{
							if( !pGrid->is_NoData(ix, iy) && (!pMask || !pMask->is_NoData(ix, iy)) )
							{
								double	dy	= (y - iy);

								if( is_Between(dy, dx, t[0]) ) { s[0] += pGrid->asDouble(ix, iy); }
								if( is_Between(dy, dx, t[1]) ) { s[1] += pGrid->asDouble(ix, iy); }
							}
						}
					}
				}
			}

			//---------------------------------------------
			if( s[0].Get_Count() > 0 && s[1].Get_Count() > 0 )
			{
				if( pLowPass[0] ) pLowPass[0]->Set_Value(x, y, s[0].Get_Mean());
				if( pLowPass[1] ) pLowPass[1]->Set_Value(x, y, s[1].Get_Mean());
				if( pStripes    ) pStripes   ->Set_Value(x, y, s[1].Get_Mean() - s[0].Get_Mean());
				if( pDestriped  ) pDestriped ->Set_Value(x, y, s[1].Get_Mean() - s[0].Get_Mean() + pGrid->asDouble(x, y));
			}
			else
			{
				if( pLowPass[0] ) pLowPass[0]->Set_NoData(x, y);
				if( pLowPass[1] ) pLowPass[1]->Set_NoData(x, y);
				if( pStripes    ) pStripes   ->Set_NoData(x, y);
				if( pDestriped  ) pDestriped ->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
