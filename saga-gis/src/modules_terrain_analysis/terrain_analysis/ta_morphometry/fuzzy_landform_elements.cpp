/**********************************************************
 * Version $Id: felement.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                fuzzy_landform_elements.h              //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
#include "fuzzy_landform_elements.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	SLOPE	= 0,
	MAXCURV,
	MINCURV,
	PCURV,
	TCURV,
	IN_COUNT
};

//---------------------------------------------------------
const CSG_String	IN_Type[IN_COUNT][2]	= 
{
	{	"SLOPE"     , _TL("Slope"               )	},
	{	"MINCURV"   , _TL("Minimum Curvature"   )	},
	{	"MAXCURV"   , _TL("Maximum Curvature"   )	},
	{	"PCURV"     , _TL("Profile Curvature"   )	},
	{	"TCURV"     , _TL("Tangential Curvature")	}
};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
struct SForm_Def
{
	CSG_String	ID, Name;

	long		Color;

	int			Value;
};

//---------------------------------------------------------
enum
{
	PLAIN	= 0,
	PIT,
	PEAK,
	RIDGE,
	CHANNEL,
	SADDLE,
	BSLOPE,
	FSLOPE,
	SSLOPE,
	HOLLOW,
	FHOLLOW,
	SHOLLOW,
	SPUR,
	FSPUR,
	SSPUR,
	FE_COUNT
};

//---------------------------------------------------------
#define MAKE_RGB(r, g, b)	SG_GET_RGB((int)(r * 255.0), (int)(g * 255.0), (int)(b * 255.0))

//---------------------------------------------------------
const struct SForm_Def Form_Def[FE_COUNT]	= 
{
	{	"PLAIN"     , _TL("Plain"          ), MAKE_RGB(0.820, 0.820, 0.820), 100	},
	{	"PIT"       , _TL("Pit"            ), MAKE_RGB(0.824, 0.706, 0.549), 111	},
	{	"PEAK"      , _TL("Peak"           ), MAKE_RGB(0.647, 0.165, 0.165), 122	},
	{	"RIDGE"     , _TL("Ridge"          ), MAKE_RGB(0.627, 0.125, 0.941), 120	},
	{	"CHANNEL"   , _TL("Channel"        ), MAKE_RGB(0.933, 0.510, 0.933), 101	},
	{	"SADDLE"    , _TL("Saddle"         ), MAKE_RGB(0.000, 0.000, 0.000), 121	},
	{	"BSLOPE"    , _TL("Back Slope"     ), MAKE_RGB(0.000, 1.000, 0.000),   0	},
	{	"FSLOPE"    , _TL("Foot Slope"     ), MAKE_RGB(0.596, 0.984, 0.596),  10	},
	{	"SSLOPE"    , _TL("Shoulder Slope" ), MAKE_RGB(0.000, 0.392, 0.000),  20	},
	{	"HOLLOW"    , _TL("Hollow"         ), MAKE_RGB(0.000, 0.000, 1.000),   1	},
	{	"FHOLLOW"   , _TL("Foot Hollow"    ), MAKE_RGB(0.000, 1.000, 1.000),  11	},
	{	"SHOLLOW"   , _TL("Shoulder Hollow"), MAKE_RGB(0.000, 0.000, 0.392),  21	},
	{	"SPUR"      , _TL("Spur"           ), MAKE_RGB(1.000, 0.000, 0.000),   2	},
	{	"FSPUR"     , _TL("Foot Spur"      ), MAKE_RGB(1.000, 0.714, 0.757),  12	},
	{	"SSPUR"     , _TL("Shoulder Spur"  ), MAKE_RGB(0.392, 0.000, 0.000),  22	}
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFuzzy_Landform_Elements::CFuzzy_Landform_Elements(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Fuzzy Landform Element Classification"));

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Algorithm for derivation of form elements acording to slope, maximum curvature, "
		"minimum curvature, profile curvature, tangential curvature, "
		"based on a linear semantic import model for slope and curvature and a fuzzy classification "
		"Based on the AML script \'felementf\' by Jochen Schmidt, Landcare Research. "
		"\n"
	));

	//-----------------------------------------------------
	int		i;

	for(i=0; i<IN_COUNT; i++)
	{
		Parameters.Add_Grid(NULL, IN_Type[i][0], IN_Type[i][1], _TL(""), PARAMETER_INPUT);
	}

	for(i=0; i<FE_COUNT; i++)
	{
		Parameters.Add_Grid(NULL, Form_Def[i].ID, Form_Def[i].Name, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}

	Parameters.Add_Grid(NULL, "FORM"      , _TL("Landform"            ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "MEM"       , _TL("Maximum Membership"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "ENTROPY"   , _TL("Entropy"             ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "CI"        , _TL("Confusion Index"     ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice(
		NULL	, "SLOPETODEG"	, _TL("Slope Grid Units"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("degree"),
			_TL("radians")
		), 0
	);

	Parameters.Add_Range(
		NULL	, "T_SLOPE"		, _TL("Slope Thresholds [Degree]"),
		_TL("lower and upper thresholds for semantic import model, planar vs. sloped areas"),
		5.0, 15.0, 0.0, true, 90.0, true
	);

	Parameters.Add_Range(
		NULL	, "T_CURVE"		, _TL("Curvature Thresholds [1 / m]"),
		_TL("lower and upper thresholds for semantic import model, straight vs. curved areas"),
		1.0 / 500000.0, 1.0 / 20000.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFuzzy_Landform_Elements::On_Execute(void)
{
	int			i;
	CSG_Grid	*pInput[IN_COUNT], *pMembership[FE_COUNT], *pForm, *pMem, *pEntropy, *pCI;

	//-----------------------------------------------------
	pForm			= Parameters("FORM"   )->asGrid();
	pMem			= Parameters("MEM"    )->asGrid();
	pEntropy		= Parameters("ENTROPY")->asGrid();
	pCI				= Parameters("CI"     )->asGrid();

	m_loSlope		= Parameters("T_SLOPE")->asRange()->Get_LoVal();// * M_DEG_TO_RAD;
	m_hiSlope		= Parameters("T_SLOPE")->asRange()->Get_HiVal();// * M_DEG_TO_RAD;
	m_loCurve		= Parameters("T_CURVE")->asRange()->Get_LoVal();
	m_hiCurve		= Parameters("T_CURVE")->asRange()->Get_HiVal();

	m_bSlopeToDeg	= Parameters("BSLOPE" )->asInt() == 1;

	for(i=0; i<IN_COUNT; i++)
	{
		pInput[i]		= Parameters(IN_Type[i][0])->asGrid();
	}

	for(i=0; i<FE_COUNT; i++)
	{
		pMembership[i]	= Parameters(Form_Def[i].ID)->asGrid();

		DataObject_Set_Colors(pMembership[i], 11, SG_COLORS_WHITE_RED);
	}

	//-----------------------------------------------------
	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pForm, "LUT");

	if( pLUT && pLUT->asTable() )
	{
		pLUT->asTable()->Del_Records();

		for(i=0; i<FE_COUNT; i++)
		{
			CSG_Table_Record	*pRecord	= pLUT->asTable()->Add_Record();

			pRecord->Set_Value(0, Form_Def[i].Color);
			pRecord->Set_Value(1, Form_Def[i].Name);
			pRecord->Set_Value(3, Form_Def[i].Value);
			pRecord->Set_Value(4, Form_Def[i].Value);
		}

		DataObject_Set_Parameter(pForm, pLUT);

		DataObject_Set_Parameter(pForm, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(i)
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bOkay;
			int		Element;
			double	MaxMem, Entropy, CI, Input[IN_COUNT], Membership[FE_COUNT];

			for(i=0, bOkay=true; i<IN_COUNT && bOkay; i++)
			{
				if( pInput[i]->is_NoData(x, y) )
				{
					bOkay	= false;
				}
				else
				{
					Input[i]	= pInput[i]->asDouble(x, y);
				}
			}

			if( bOkay && Get_Memberships(Input, Membership, Element, MaxMem, Entropy, CI) )
			{
				for(i=0; i<FE_COUNT; i++)
				{
					if( pMembership[i] )
					{
						pMembership[i]->Set_Value(x, y, Membership[i]);
					}
				}

				pForm   ->Set_Value(x, y, Element);	// hard classification according to highest membership value
				pMem    ->Set_Value(x, y, MaxMem );
				pEntropy->Set_Value(x, y, Entropy);
				pCI     ->Set_Value(x, y, CI     );	// confusion index
			}
			else
			{
				for(i=0; i<FE_COUNT; i++)
				{
					if( pMembership[i] )
					{
						pMembership[i]->Set_NoData(x, y);
					}
				}

				pForm   ->Set_NoData(x, y);
				pMem    ->Set_NoData(x, y);
				pEntropy->Set_NoData(x, y);
				pCI     ->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_MINIMUM(a, b, c)	M_GET_MIN(a, M_GET_MIN(b, c))

//---------------------------------------------------------
bool CFuzzy_Landform_Elements::Get_Memberships(double Input[], double M[], int &Element, double &MaxMem, double &Entropy, double &CI)
{
	double	bFlat, bSlope, MaxMem2,
			bPCurve  , bPCurveX  , bPCurveV,
			bTCurve  , bTCurveX  , bTCurveV,
			bMaxCurve, bMaxCurveX, bMaxCurveV,
			bMinCurve, bMinCurveX, bMinCurveV;

	//-----------------------------------------------------
	if( m_bSlopeToDeg )
	{
		Input[SLOPE]	*= M_RAD_TO_DEG;
	}

	//-----------------------------------------------------
	// membership slope flat
	bFlat		= Input[SLOPE] <= m_loSlope ? 1.0 : (Input[SLOPE] >= m_hiSlope ? 0.0
				: (m_hiSlope - Input[SLOPE]) / (m_hiSlope - m_loSlope));			// linear membership fct
	bSlope		= 1 - bFlat;

	// membership pcurv straight
	bPCurve		= fabs(Input[PCURV]) <= m_loCurve ? 1.0 : (fabs(Input[PCURV]) >= m_hiCurve ? 0.0
				: (m_hiCurve - fabs(Input[PCURV])) / (m_hiCurve - m_loCurve));		// linear membership fct
	bPCurveX	= Input[PCURV] >= 0.0 ? 1.0 - bPCurve : 0.0;
	bPCurveV	= Input[PCURV] <= 0.0 ? 1.0 - bPCurve : 0.0;

	// membership tcurv straight
	bTCurve		= fabs(Input[TCURV]) <= m_loCurve ? 1.0 : (fabs(Input[TCURV]) >= m_hiCurve ? 0.0
				: (m_hiCurve - fabs(Input[TCURV])) / (m_hiCurve - m_loCurve));		// linear membership fct
	bTCurveX	= Input[TCURV] >= 0.0 ? 1.0 - bTCurve : 0.0;
	bTCurveV	= Input[TCURV] <= 0.0 ? 1.0 - bTCurve : 0.0;

	// membership maxcurv straight
	bMaxCurve	= fabs(Input[MAXCURV]) <= m_loCurve ? 1.0 : (fabs(Input[MAXCURV]) >= m_hiCurve ? 0.0
				: (m_hiCurve - fabs(Input[MAXCURV])) / (m_hiCurve - m_loCurve));	// linear membership fct
	bMaxCurveX	= Input[MAXCURV] >= 0.0 ? 1.0 - bMaxCurve : 0.0;
	bMaxCurveV	= Input[MAXCURV] <= 0.0 ? 1.0 - bMaxCurve : 0.0;

	// membership mincurv straight
	bMinCurve	= fabs(Input[MINCURV]) <= m_loCurve ? 1.0 : (fabs(Input[MINCURV]) >= m_hiCurve ? 0.0
				: (m_hiCurve - fabs(Input[MINCURV])) / (m_hiCurve - m_loCurve));	// linear membership fct
	bMinCurveX	= Input[MINCURV] >= 0.0 ? 1.0 - bMinCurve : 0.0;
	bMinCurveV	= Input[MINCURV] <= 0.0 ? 1.0 - bMinCurve : 0.0;

	//-----------------------------------------------------
	M[PLAIN]	= GET_MINIMUM(bFlat, bMaxCurve, bMinCurve);
	Element		= Form_Def[PLAIN].Value;
	MaxMem		= M[PLAIN];

	M[PIT]		= GET_MINIMUM(bFlat, bMaxCurveV, bMinCurveV);
	if( M[PIT] > MaxMem ) 
	{ 
		Element	= Form_Def[PIT].Value;
		MaxMem	= M[PIT]; 
		MaxMem2	= M[PLAIN];
	}
	else
	{
		MaxMem2	= M[PIT];
	}

	M[PEAK]		= GET_MINIMUM(bFlat, bMaxCurveX, bMinCurveX);
	if( M[PEAK] > MaxMem )
	{
		Element	= Form_Def[PEAK].Value;
		MaxMem	= M[PEAK];
	}
	else if( M[PEAK] > MaxMem2 )
	{
		MaxMem2	= M[PEAK];
	}

	M[RIDGE]	= GET_MINIMUM(bFlat, bMaxCurveX, bMinCurve);
	if( M[RIDGE] > MaxMem )
	{ 
		Element	= Form_Def[RIDGE].Value;
		MaxMem	= M[RIDGE];
	}
	else if( M[RIDGE] > MaxMem2 )
	{
		MaxMem2	= M[RIDGE];
	}

	M[CHANNEL]   = GET_MINIMUM(bFlat, bMaxCurve, bMinCurveV);
	if( M[CHANNEL] > MaxMem )
	{ 
		Element	= Form_Def[CHANNEL].Value;
		MaxMem	= M[CHANNEL];
	}
	else if( M[CHANNEL] > MaxMem2 )
	{
		MaxMem2	= M[CHANNEL];
	}

	M[SADDLE]   = GET_MINIMUM(bFlat, bMaxCurveX, bMinCurveV);
	if( M[SADDLE] > MaxMem )
	{ 
		Element	= Form_Def[SADDLE].Value;
		MaxMem	= M[SADDLE];
	}
	else if( M[SADDLE] > MaxMem2 )
	{
		MaxMem2	= M[SADDLE];
	}

	M[BSLOPE]   = GET_MINIMUM(bSlope, bPCurve, bTCurve);
	if( M[BSLOPE] > MaxMem )
	{ 
		Element	= Form_Def[BSLOPE].Value;
		MaxMem	= M[BSLOPE];
	}
	else if( M[BSLOPE] > MaxMem2 )
	{
		MaxMem2	= M[BSLOPE];
	}

	M[FSLOPE]   = GET_MINIMUM(bSlope, bPCurveV, bTCurve);
	if( M[FSLOPE] > MaxMem )
	{ 
		Element	= Form_Def[FSLOPE].Value;
		MaxMem	= M[FSLOPE];
	}
	else if( M[FSLOPE] > MaxMem2 )
	{
		MaxMem2	= M[FSLOPE];
	}

	M[SSLOPE]   = GET_MINIMUM(bSlope, bPCurveX, bTCurve);
	if( M[SSLOPE] > MaxMem )
	{ 
		Element	= Form_Def[SSLOPE].Value;
		MaxMem	= M[SSLOPE];
	}
	else if( M[SSLOPE] > MaxMem2 )
	{
		MaxMem2	= M[SSLOPE];
	}

	M[HOLLOW]   = GET_MINIMUM(bSlope, bPCurve, bTCurveV);
	if( M[HOLLOW] > MaxMem )
	{ 
		Element	= Form_Def[HOLLOW].Value;
		MaxMem	= M[HOLLOW];
	}
	else if( M[HOLLOW] > MaxMem2 )
	{
		MaxMem2	= M[HOLLOW];
	}

	M[FHOLLOW]   = GET_MINIMUM(bSlope, bPCurveV, bTCurveV);
	if( M[FHOLLOW] > MaxMem )
	{ 
		Element	= Form_Def[FHOLLOW].Value;
		MaxMem	= M[FHOLLOW];
	}
	else if( M[FHOLLOW] > MaxMem2 )
	{
		MaxMem2	= M[FHOLLOW];
	}

	M[SHOLLOW]   = GET_MINIMUM(bSlope, bPCurveX, bTCurveV);
	if( M[SHOLLOW] > MaxMem )
	{ 
		Element	= Form_Def[SHOLLOW].Value;
		MaxMem	= M[SHOLLOW];
	}
	else if( M[SHOLLOW] > MaxMem2 )
	{
		MaxMem2	= M[SHOLLOW];
	}

	M[SPUR]   = GET_MINIMUM(bSlope, bPCurve, bTCurveX);
	if( M[SPUR] > MaxMem )
	{ 
		Element	= Form_Def[SPUR].Value;
		MaxMem	= M[SPUR];
	}
	else if( M[SPUR] > MaxMem2 )
	{
		MaxMem2	= M[SPUR];
	}

	M[FSPUR] = GET_MINIMUM(bSlope, bPCurveV, bTCurveX);
	if( M[FSPUR] > MaxMem )
	{ 
		Element	= Form_Def[FSPUR].Value;
		MaxMem	= M[FSPUR];
	}
	else if( M[FSPUR] > MaxMem2 )
	{
		MaxMem2	= M[FSPUR];
	}

	M[SSPUR] = GET_MINIMUM(bSlope, bPCurveX, bTCurveX);
	if( M[SSPUR] > MaxMem )
	{ 
		Element	= Form_Def[SSPUR].Value;
		MaxMem	= M[SSPUR];
	}
	else if( M[SSPUR] > MaxMem2 )
	{
		MaxMem2	= M[SSPUR];
	}

	//-----------------------------------------------------
	double	summem	= 0.0;	// = %PLAIN% + %PIT% + %PEAK% + %RIDGE% + %CHANNEL% + %SADDLE% + %BSLOPE% + %FSLOPE% + %SSLOPE% + %HOLLOW% + %FHOLLOW% + %SHOLLOW% + %SPUR% + %FSPUR% + %SSPUR%

	for(int i=0; i<FE_COUNT; i++)
	{
		summem	+= M[i];
	}

	// entropy: / ln (15 elements)
	Entropy	= -0.3692693 *
		( (M[PLAIN]   <= 0 || summem <= 0 ? 0 : M[PLAIN]   * log(M[PLAIN]  ) / summem)
		+ (M[PIT]     <= 0 || summem <= 0 ? 0 : M[PIT]     * log(M[PIT]    ) / summem)
		+ (M[PEAK]    <= 0 || summem <= 0 ? 0 : M[PEAK]    * log(M[PEAK]   ) / summem)
		+ (M[RIDGE]   <= 0 || summem <= 0 ? 0 : M[RIDGE]   * log(M[RIDGE]  ) / summem)
		+ (M[CHANNEL] <= 0 || summem <= 0 ? 0 : M[CHANNEL] * log(M[CHANNEL]) / summem)
		+ (M[SADDLE]  <= 0 || summem <= 0 ? 0 : M[SADDLE]  * log(M[SADDLE] ) / summem)
		+ (M[BSLOPE]  <= 0 || summem <= 0 ? 0 : M[BSLOPE]  * log(M[BSLOPE] ) / summem)
		+ (M[FSLOPE]  <= 0 || summem <= 0 ? 0 : M[FSLOPE]  * log(M[FSLOPE] ) / summem)
		+ (M[SSLOPE]  <= 0 || summem <= 0 ? 0 : M[SSLOPE]  * log(M[SSLOPE] ) / summem)
		+ (M[HOLLOW]  <= 0 || summem <= 0 ? 0 : M[HOLLOW]  * log(M[HOLLOW] ) / summem)
		+ (M[FHOLLOW] <= 0 || summem <= 0 ? 0 : M[FHOLLOW] * log(M[FHOLLOW]) / summem)
		+ (M[SHOLLOW] <= 0 || summem <= 0 ? 0 : M[SHOLLOW] * log(M[SHOLLOW]) / summem)
		+ (M[SPUR]    <= 0 || summem <= 0 ? 0 : M[SPUR]    * log(M[SPUR]   ) / summem)
		+ (M[FSPUR]   <= 0 || summem <= 0 ? 0 : M[FSPUR]   * log(M[FSPUR]  ) / summem)
		+ (M[SSPUR]   <= 0 || summem <= 0 ? 0 : M[SSPUR]   * log(M[SSPUR]  ) / summem)
		- (summem <= 0 ? 0 : log(summem))
	);

	CI	= MaxMem2 / MaxMem;	// confusion index

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
