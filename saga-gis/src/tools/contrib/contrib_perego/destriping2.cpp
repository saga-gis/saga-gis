
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
//                    destriping2.cpp                    //
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
#include "destriping2.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
Cdestriping2::Cdestriping2(void)
{
	Set_Name		(_TL("Destriping with Mask"));

	Set_Author		("Alessandro Perego");

	Set_Description	(_TW(
		"Destriping filter removes straight parallel stripes in raster data. "
		"It uses two low-pass filters elongated in the stripes direction; "
		"the first one is 1 pixel unit wide while the second one is wide as the striping wavelength. "
		"Their difference is the striping error which is removed from the original data to obtain the destriped DEM. "
		"This method is equivalent to that proposed ry[1] Oimoen (2000). "
		"With destriping 2 you can choose a range of value (min-max) from the input grid "
		"and a range of value (Mask min - Mask max) from a mask grid to select the target cells. "
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
	Parameters.Add_Grid  ("", "MASK"   , _TL("Mask"            ), _TL("Mask used to select cells for processing"), PARAMETER_INPUT);

	Parameters.Add_Grid  ("", "RESULT3", _TL("Destriped Grid"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid  ("", "RESULT1", _TL("Low-pass 1"      ), _TL("Step 1: low-pass of stripe"                           ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid  ("", "RESULT2", _TL("Low-pass 2"      ), _TL("Step 2: low-pass between stripe and its surroundings" ), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid  ("", "STRIPES", _TL("Stripes"         ), _TL("The difference between destriped and original grid"   ), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Double("", "ANG"    , _TL("Angle"           ), _TL("[Degree], 0 = horizontal, 90 = vertical"));
	Parameters.Add_Double("", "R"      , _TL("Radius"          ), _TL("[Cells]"), 20., 1., true);
	Parameters.Add_Double("", "D"      , _TL("Stripes Distance"), _TL("[Cells]"),  2., 2., true);

	Parameters.Add_Double("INPUT", "MIN" , _TL("Minimum"         ), _TL(""),    -10.);
	Parameters.Add_Double("INPUT", "MAX" , _TL("Maximum"         ), _TL(""),     10.);
	Parameters.Add_Double("MASK" , "MMIN", _TL("Mask Minimum"    ), _TL(""), -10000.);
	Parameters.Add_Double("MASK" , "MMAX", _TL("Mask Maximum"    ), _TL(""),  10000.);	
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Cdestriping2::On_Execute(void)
{
	CSG_Grid *pZ	= Parameters("INPUT")->asGrid  ();
	double  zmin	= Parameters(  "MIN")->asDouble();
	double  zmax	= Parameters(  "MAX")->asDouble();

	CSG_Grid *pM	= Parameters( "MASK")->asGrid  ();
	double  mmin	= Parameters( "MMIN")->asDouble();
	double  mmax	= Parameters( "MMAX")->asDouble();

	//-----------------------------------------------------
	CSG_Grid	Mask(Get_System(), SG_DATATYPE_Char);

	Mask.Set_NoData_Value(0.);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Mask.Set_Value(x, y,
				!pZ->is_NoData(x, y) && SG_IS_BETWEEN(zmin, pZ->asDouble(x, y), zmax) &&
				!pM->is_NoData(x, y) && SG_IS_BETWEEN(mmin, pM->asDouble(x, y), mmax) ? 1. : 0.
			);
		}
	}

	//-----------------------------------------------------
	SG_RUN_TOOL_ExitOnError("contrib_perego", 5, // Destriping
			SG_TOOL_PARAMETER_SET("INPUT"  , Parameters("INPUT"  ))
		&&	SG_TOOL_PARAMETER_SET("RESULT1", Parameters("RESULT1"))
		&&	SG_TOOL_PARAMETER_SET("RESULT2", Parameters("RESULT2"))
		&&	SG_TOOL_PARAMETER_SET("RESULT3", Parameters("RESULT3"))
		&&	SG_TOOL_PARAMETER_SET("STRIPES", Parameters("STRIPES"))
		&&	SG_TOOL_PARAMETER_SET("ANG"    , Parameters("ANG"    ))
		&&	SG_TOOL_PARAMETER_SET("R"      , Parameters("R"      ))
		&&	SG_TOOL_PARAMETER_SET("D"      , Parameters("D"      ))
		&&	SG_TOOL_PARAMETER_SET("MASK"   , &Mask                )
	);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
