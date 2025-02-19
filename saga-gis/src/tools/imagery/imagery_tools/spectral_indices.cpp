
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     image_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 spectral_indices.cpp                  //
//                                                       //
//                 Olaf Conrad (C) 2025                  //
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
#include "spectral_indices.h"
#include "spectral_indices_data.h"

//---------------------------------------------------------
#ifdef _SAGA_LINUX
bool _finite(double val)
{
	return( true );
}
#endif


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CASI_Indices
{
public:
	CASI_Indices(void)
	{
		Create_Bands(); Create_Constants(); Create_Domains(); Create_Indices();
	}

	int					Get_Band_Count        (void)  const { return( (int)m_Bands    .Get_Count() ); }
	const SG_Char *		Get_Band_ID           (int i) const { return( m_Bands    [i].asString("id"         ) ); }
	const SG_Char *		Get_Band_Name         (int i) const { return( m_Bands    [i].asString("name"       ) ); }
	const SG_Char *		Get_Band_System       (int i) const { return( m_Bands    [i].asString("system"     ) ); }
	const SG_Char *		Get_Band_Description  (int i) const { return( m_Bands    [i].asString("description") ); }

	int					Get_Constant_Count    (void)  const { return( (int)m_Constants.Get_Count() ); }
	const SG_Char *		Get_Constant_ID       (int i) const { return( m_Constants[i].asString("id"         ) ); }
	const SG_Char *		Get_Constant_Name     (int i) const { return( m_Constants[i].asString("name"       ) ); }
	double				Get_Constant          (int i) const { return( m_Constants[i].asDouble("value"      ) ); }

	int					Get_Domain_Count      (void)  const { return( (int)m_Domains  .Get_Count() ); }
	const SG_Char *		Get_Domain_ID         (int i) const { return( m_Domains  [i].asString("id"         ) ); }
	const SG_Char *		Get_Domain_Name       (int i) const { return( m_Domains  [i].asString("name"       ) ); }
	CSG_String			Get_Domain_Choices    (void)  const { CSG_String s; for(int i=0; i<Get_Domain_Count(); i++) { s += Get_Domain_Name(i); s += "|"; } return( s ); }

	int					Get_Count             (void)  const { return( (int)m_Indices  .Get_Count() ); }
	const SG_Char *		Get_ID                (int i) const { return( m_Indices  [i].asString("id"         ) ); }
	const SG_Char *		Get_Name              (int i) const { return( m_Indices  [i].asString("name"       ) ); }
	const SG_Char *		Get_Domain            (int i) const { return( m_Indices  [i].asString("domain"     ) ); }
	const SG_Char *		Get_Formula           (int i) const { return( m_Indices  [i].asString("formula"    ) ); }
	const SG_Char *		Get_Bands             (int i) const { return( m_Indices  [i].asString("bands"      ) ); }
	const SG_Char *		Get_Reference         (int i) const { return( m_Indices  [i].asString("reference"  ) ); }
	const SG_Char *		Get_Contributor       (int i) const { return( m_Indices  [i].asString("contributor") ); }
	const SG_Char *		Get_Date              (int i) const { return( m_Indices  [i].asString("date"       ) ); }

	//---------------------------------------------------------
	bool				Get_Variables(int Index, CSG_Strings &Variables) const
	{
		if( Index >= 0 && Index < Get_Count() )
		{
			Variables = SG_String_Tokenize(Get_Bands(Index), ",");

			return( Variables.Get_Count() > 0 );
		}

		return( false );
	}

private:

	CSG_Table			m_Bands, m_Constants, m_Domains, m_Indices;


	//-----------------------------------------------------
	bool				Create_Bands(void)
	{
		struct SBands { const char *id, *common, *name; double wave_max, wave_min; const char *system; };

		const struct SBands Bands[] =
		{
			{ "A"  , "coastal"   , "Aersols"                     ,   455,   400 , "AUX"     },
			{ "B"  , "blue"      , "Blue"                        ,   530,   450 , "VNIR"    },
			{ "G"  , "green"     , "Green"                       ,   600,   510 , "VNIR"    },
			{ "G1" , "green05"   , "Green 1"                     ,   550,   510 , "VNIR"    },
			{ "N"  , "nir"       , "Near-Infrared (NIR)"         ,   900,   760 , "VNIR"    },
			{ "N2" , "nir08"     , "Near-Infrared (NIR) 2"       ,   880,   850 , "REDEDGE" },
			{ "R"  , "red"       , "Red"                         ,   690,   620 , "VNIR"    },
			{ "RE1", "rededge071", "Red Edge 1"                  ,   715,   695 , "REDEDGE" },
			{ "RE2", "rededge075", "Red Edge 2"                  ,   750,   730 , "REDEDGE" },
			{ "RE3", "rededge078", "Red Edge 3"                  ,   795,   765 , "REDEDGE" },
			{ "S1" , "swir16"    , "Short-wave Infrared (SWIR) 1",  1750,  1550 , "SWIR"    },
			{ "S2" , "swir22"    , "Short-wave Infrared (SWIR) 2",  2350,  2080 , "SWIR"    },
			{ "T"  , "lwir"      , "Thermal Infrared"            , 12500, 10400 , "THERMAL" },
			{ "T1" , "lwir11"    , "Thermal Infrared 1"          , 11190, 10600 , "THERMAL" },
			{ "T2" , "lwir12"    , "Thermal Infrared 2"          , 12510, 11500 , "THERMAL" },
			{ "WV" , "nir09"     , "Water Vapour"                ,   960,   930 , "AUX"     },
			{ "Y"  , "yellow"    , "Yellow"                      ,   625,   585 , "VNIR"    },
			{ "VV" , "VV"        , "VV"                          ,    -1,    -1 , ""        },
			{ "VH" , "VH"        , "VH"                          ,    -1,    -1 , ""        },
			{ "HH" , "HH"        , "HH"                          ,    -1,    -1 , ""        },
			{ "HV" , "HV"        , "HV"                          ,    -1,    -1 , ""        },
			{ NULL , NULL        , NULL                          ,     0,     0 , NULL      }
		};

		m_Bands.Destroy();

		m_Bands.Add_Field("id"         , SG_DATATYPE_String);
		m_Bands.Add_Field("common"     , SG_DATATYPE_String);
		m_Bands.Add_Field("name"       , SG_DATATYPE_String);
		m_Bands.Add_Field("wave_min"   , SG_DATATYPE_Double);
		m_Bands.Add_Field("wave_max"   , SG_DATATYPE_Double);
		m_Bands.Add_Field("system"     , SG_DATATYPE_String);
		m_Bands.Add_Field("description", SG_DATATYPE_String);

		for(int i=0; Bands[i].id; i++)
		{
			CSG_Table_Record &Record = *m_Bands.Add_Record();

			Record.Set_Value("id"         , Bands[i].id      );
			Record.Set_Value("common"     , Bands[i].common  );
			Record.Set_Value("name"       , Bands[i].name    );
			Record.Set_Value("wave_min"   , Bands[i].wave_min);
			Record.Set_Value("wave_max"   , Bands[i].wave_max);
			Record.Set_Value("system"     , Bands[i].system  );

			if( Bands[i].wave_min > 0 )
			{
				Record.Set_Value("description", CSG_String::Format("Wavelengths %d - %d nm",
					(int)Bands[i].wave_min, (int)Bands[i].wave_max)
				);
			}
		}

		m_Bands.Sort("wave_min");

		return( true );
	}

	//-----------------------------------------------------
	bool				Create_Constants(void)
	{
		struct SConstants { const char *id; double value; const char *name; };

		struct SConstants Constants[] =
		{
			{ "C1"      , 6.0  , "Coefficient 1 for the aerosol resistance term" },
			{ "C2"      , 7.5  , "Coefficient 2 for the aerosol resistance term" },
			{ "L"       , 1.0  , "Canopy background adjustment"                  },
			{ "PAR"     , 0.0  , "Photosynthetically Active Radiation"           }, // null
			{ "alpha"   , 0.1  , "Weighting coefficient used for WDRVI"          },
			{ "beta"    , 0.05 , "Calibration parameter used for NDSInw"         },
			{ "c"       , 1.0  , "Trade-off parameter in the polynomial kernel"  },
			{ "cexp"    , 1.16 , "Exponent used for OCVI"                        },
			{ "epsilon" , 1.0  , "Adjustment constant used for EBI"              },
			{ "fdelta"  , 0.581, "Adjustment factor used for SEVI"               },
			{ "g"       , 2.5  , "Gain factor"                                   },
			{ "gamma"   , 1.0  , "Weighting coefficient used for ARVI"           },
			{ "k"       , 0.0  , "Slope parameter by soil used for NIRvH2"       },
			{ "lambdaG" , 0.0  , "Green central wavelength (nm)"                 }, // null
			{ "lambdaN" , 0.0  , "NIR central wavelength (nm)"                   }, // null
			{ "lambdaR" , 0.0  , "Red central wavelength (nm)"                   }, // null
			{ "lambdaS1", 0.0  , "SWIR1 central wavelength (nm)"                 }, // null
			{ "nexp"    , 2.0  , "Exponent used for GDVI"                        },
			{ "omega"   , 2.0  , "Weighting coefficient used for MBWI"           },
			{ "p"       , 2.0  , "Kernel degree in the polynomial kernel"        },
			{ "sigma"   , 0.5  , "Length-scale parameter in the RBF kernel"      },
			{ "sla"     , 1.0  , "Soil line slope"                               },
			{ "slb"     , 0.0  , "Soil line intercept"                           },
			{ NULL      , 0.0  , NULL                                            }
		};

		m_Constants.Destroy();

		m_Constants.Add_Field("id"   , SG_DATATYPE_String);
		m_Constants.Add_Field("name" , SG_DATATYPE_String);
		m_Constants.Add_Field("value", SG_DATATYPE_Double);

		for(int i=0; Constants[i].id; i++)
		{
			CSG_Table_Record &Record = *m_Constants.Add_Record();

			Record.Set_Value("id"   , Constants[i].id   );
			Record.Set_Value("name" , Constants[i].name );
			Record.Set_Value("value", Constants[i].value);
		}

		return( true );
	}

	//-----------------------------------------------------
	bool				Create_Domains(void)
	{
		m_Domains.Destroy();

		m_Domains.Add_Field("id"   , SG_DATATYPE_String);
		m_Domains.Add_Field("name" , SG_DATATYPE_String);

		#define Add_Domain(id, name) { CSG_Table_Record &r = *m_Domains.Add_Record(); r.Set_Value(0, id); r.Set_Value(1, name); }

		Add_Domain("burn"      , _TL("Burn"      ));
		Add_Domain("radar"     , _TL("Radar"     ));
		Add_Domain("snow"      , _TL("Snow"      ));
		Add_Domain("soil"      , _TL("Soil"      ));
		Add_Domain("urban"     , _TL("Urban"     ));
		Add_Domain("vegetation", _TL("Vegetation"));
		Add_Domain("water"     , _TL("Water"     ));

		return( true );
	}

	//-----------------------------------------------------
	bool				Create_Indices(void)
	{
		enum { id = 0, name, domain, formula, bands, reference, contributor, date };

		m_Indices.Destroy();

		m_Indices.Add_Field("id"         , SG_DATATYPE_String);
		m_Indices.Add_Field("name"       , SG_DATATYPE_String);
		m_Indices.Add_Field("domain"     , SG_DATATYPE_String);
		m_Indices.Add_Field("formula"    , SG_DATATYPE_String);
		m_Indices.Add_Field("bands"      , SG_DATATYPE_String);
		m_Indices.Add_Field("reference"  , SG_DATATYPE_String);
		m_Indices.Add_Field("contributor", SG_DATATYPE_String);
		m_Indices.Add_Field("date"       , SG_DATATYPE_String);

		for(int i=0; Awesome_Indices[i][0]; i++)
		{
			CSG_Table_Record &Record = *m_Indices.Add_Record();

			Record.Set_Value("id"         , Awesome_Indices[i][id         ]);
			Record.Set_Value("name"       , Awesome_Indices[i][name       ]);
			Record.Set_Value("domain"     , Awesome_Indices[i][domain     ]);
			Record.Set_Value("formula"    , Awesome_Indices[i][formula    ]);
			Record.Set_Value("reference"  , Awesome_Indices[i][reference  ]);
			Record.Set_Value("contributor", Awesome_Indices[i][contributor]);
			Record.Set_Value("date"       , Awesome_Indices[i][date       ]);

			CSG_String Bands(Awesome_Indices[i][bands]);
			Bands.Replace(" ", ""); Bands.Replace("[", ""); Bands.Replace("]", ""); Bands.Replace("'", ""); Bands.Replace("**", "^");
			Record.Set_Value("bands"      , Bands);
		}

		CSG_Index Index; int Fields[2] = { domain, name };
		m_Indices.Set_Index(Index, Fields, 2, true);
		m_Indices.Sort(Index);

		return( true );
	}
};

//---------------------------------------------------------
static CASI_Indices m_Indices;


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSpectral_Indices::CSpectral_Indices(void)
{
	Set_Name		(_TL("\'Awesome\' Spectral Indices"));

	Set_Author		("O.Conrad (c) 2025");

	CSG_String Description(_TW(
		"Calculate a spectral index. Select the index of your choice "
		"from the index definitions originally provided by the "
		"<i>Awesome-Spectral-Indices</i> project. "
		"See <b>Montero et al. (2023)</b> for details "
		"and the following listing of provided spectral index definitions. "
	));

	Description += CSG_String::Format("<hr><h4>%s</h4><table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>", _TL("Spectral Indices"), _TL("Domain"), _TL("ID"), _TL("Name"));

	for(int i=0; i<m_Indices.Get_Count(); i++)
	{
		if( SG_STR_CMP(m_Indices.Get_Domain(i), "kernel") )
		{
			Description += CSG_String::Format("<tr><td>%s</td><td><a href=\"%s\">%s</a></td><td>%s</td></tr>",
				m_Indices.Get_Domain(i), m_Indices.Get_Reference(i), m_Indices.Get_ID(i), m_Indices.Get_Name(i)
			);
		}
	}

	Description += "</table>";

	Set_Description(Description);

	Add_Reference("https://github.com/awesome-spectral-indices/awesome-spectral-indices", SG_T("Awesome Spectral Indices"));

	Add_Reference(SG_T("Montero, D., Aybar, C., Mahecha, M.D., Martinuzzi, F., Söchting, M. & Wieneke, S."), "2023",
		"A standardized catalogue of spectral indices to advance the use of remote sensing in Earth system research",
		"Nature Scientific Data, 10:197, 1-20.",
		SG_T("https://doi.org/10.1038/s41597-023-02096-0"), SG_T("doi:10.1038/s41597-023-02096-0")
	);	

	//-----------------------------------------------------
	Parameters.Add_Grid("", "INDEX", _TL("Spectral Index"), _TL(""), PARAMETER_OUTPUT);

	for(int i=0; i<m_Indices.Get_Band_Count(); i++)
	{
		CSG_String System;

		if( *m_Indices.Get_Band_System(i) )
		{
			System.Printf("GRID_SYSTEM_%s", m_Indices.Get_Band_System(i));

			if( !Parameters(System) )
			{
				if( System.Find("VNIR"   ) > 0 ) { Parameters.Add_Grid_System("", System, _TL("Visible & Near-Infrared"), _TL("")); }
				if( System.Find("SWIR"   ) > 0 ) { Parameters.Add_Grid_System("", System, _TL("Short-wave Infrared"    ), _TL("")); }
				if( System.Find("REDEDGE") > 0 ) { Parameters.Add_Grid_System("", System, _TL("Red Edge"               ), _TL("")); }
				if( System.Find("THERMAL") > 0 ) { Parameters.Add_Grid_System("", System, _TL("Thermal Infrared"       ), _TL("")); }
				if( System.Find("AUX"    ) > 0 ) { Parameters.Add_Grid_System("", System, _TL("Auxiliary Bands"        ), _TL("")); }
			}
		}
		
		(m_Indices.Get_Band_System(i));

		Parameters.Add_Grid(System, m_Indices.Get_Band_ID(i), m_Indices.Get_Band_Name(i), m_Indices.Get_Band_Description(i), PARAMETER_INPUT);//, false);
	}

	//-----------------------------------------------------
	Parameters.Add_Choice("", "DOMAIN", _TL("Application Domain"), _TL(""), m_Indices.Get_Domain_Choices(), 0);

	for(int iDomain=0; iDomain<m_Indices.Get_Domain_Count(); iDomain++)
	{
		CSG_String Indices, Domain(m_Indices.Get_Domain_ID(iDomain));

		for(int i=0; i<m_Indices.Get_Count(); i++)
		{
			if( Domain.Cmp(m_Indices.Get_Domain(i)) == 0 )
			{
				Indices += CSG_String::Format("{%d}%s|", i, m_Indices.Get_Name(i));
			}
		}

		if( !Indices.is_Empty() )
		{
			Parameters.Add_Choice("DOMAIN", Domain, _TL("Spectral Index"), _TL(""), Indices);
		}
	}

	for(int i=0; i<m_Indices.Get_Constant_Count(); i++)
	{
		Parameters.Add_Double("", m_Indices.Get_Constant_ID(i), m_Indices.Get_Constant_ID(i), m_Indices.Get_Constant_Name(i), m_Indices.Get_Constant(i));
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSpectral_Indices::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_String Domain(m_Indices.Get_Domain_ID((*pParameters)("DOMAIN")->asInt()));

	if( pParameter->Cmp_Identifier("DOMAIN") )
	{
		for(int iDomain=0; iDomain<m_Indices.Get_Domain_Count(); iDomain++)
		{
			pParameters->Set_Enabled(m_Indices.Get_Domain_ID(iDomain), Domain.Cmp(m_Indices.Get_Domain_ID(iDomain)) == 0);
		}
	}

	if( pParameter->Cmp_Identifier("DOMAIN") || pParameter->Cmp_Identifier(Domain) )
	{
		for(int i=0; i<m_Indices.Get_Band_Count    (); i++) { pParameters->Set_Enabled(m_Indices.Get_Band_ID    (i), false); }
		for(int i=0; i<m_Indices.Get_Constant_Count(); i++) { pParameters->Set_Enabled(m_Indices.Get_Constant_ID(i), false); }

		int Index; CSG_Strings Variables;

		if( (*pParameters)(Domain)->asChoice()->Get_Data(Index) && m_Indices.Get_Variables(Index, Variables) )
		{
			for(int i=0; i< Variables.Get_Count(); i++) { pParameters->Set_Enabled(Variables[i], true); }
		}

		for(int i=0; i<pParameters->Get_Count(); i++)
		{
			CSG_Parameter *pP = pParameters->Get_Parameter(i);

			if( pP->Get_Type() == PARAMETER_TYPE_Grid_System )
			{
				bool bEnable = false; pP->Set_Enabled(true);

				for(int j=0; !bEnable && j<pP->Get_Children_Count(); j++)
				{
					bEnable = pP->Get_Child(j)->is_Enabled();
				}

				pP->Set_Enabled(bEnable);
			}
		}
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpectral_Indices::On_Execute(void)
{
	CSG_String Domain(m_Indices.Get_Domain_ID(Parameters("DOMAIN")->asInt()));

	int Index; CSG_Strings Variables;

	if( !Parameters(Domain)->asChoice()->Get_Data(Index) && m_Indices.Get_Variables(Index, Variables) )
	{
		Error_Set(_TL("spectral index selection error"));

		return( false );
	}

	//-----------------------------------------------------
	if( !Set_Formula(Index) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid *pIndex = Parameters("INDEX")->asGrid();

	pIndex->Fmt_Name("%s [%s - %s]", _TL("Spectral Index"), m_Indices.Get_Domain_Name(Parameters("DOMAIN")->asInt()), m_Indices.Get_ID(Index));

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif // !_DEBUG
		for(int x=0; x<Get_NX(); x++)
		{
			double Value;

			if( !Get_Result(x, y, Value) )
			{
				pIndex->Set_NoData(x, y);
			}
			else
			{
				pIndex->Set_Value(x, y, Value);
			}
		}
	}

	m_Bands.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSpectral_Indices::Set_Formula(int Index)
{
	#define ReplaceID(old, new) if( old.Length() != 1 ) { Formula.Replace(old, new); } else { Formula.Replace_Single_Char(old[0], new); }

	const int nVars = 27; const SG_Char Vars[nVars] = SG_T("abcdefghijklmnopqrstuvwxyz");

	CSG_Strings Variables;

	if( !m_Indices.Get_Variables(Index, Variables) )
	{
		Error_Set(_TL("formula initialization error"));

		return( false );
	}

	CSG_String Formula(m_Indices.Get_Formula(Index)); Formula.Replace("**", "^");

	for(int i=0, n=0; i<Variables.Get_Count(); i++)
	{
		CSG_Parameter *pVariable = Parameters(Variables[i]);

		if( !pVariable )
		{
			Error_Fmt("%s: %s", _TL("unknown variable"), Variables[i].c_str());

			return( false );
		}

		if( pVariable->asGrid() )
		{
			if( n >= nVars )
			{
				Error_Set(_TL("to many bands in formula"));

				return( false );
			}

			m_Bands += pVariable->asGrid();

			ReplaceID(Variables[i], Vars[n]); n++;
		}
		else if( pVariable->Get_Type() == PARAMETER_TYPE_Double ) // constant
		{
			ReplaceID(Variables[i], pVariable->asString());
		}
	}

	if( !m_Formula.Set_Formula(Formula) )
	{
		CSG_String Message;

		if( !m_Formula.Get_Error(Message) )
		{
			Message.Printf("%s: %s", _TL("error in formula"), Formula.c_str());
		}

		Error_Set(Message);

		return( false );
	}

	Message_Fmt("\n%s: %s", _TL("Formula"), Formula.c_str());

	return( true );
}

//---------------------------------------------------------
bool CSpectral_Indices::Get_Result(int x, int y, double &Value)
{
	CSG_Vector Values(m_Bands.Get_Size());

	for(sLong i=0; i<m_Bands.Get_Size(); i++)
	{
		CSG_Grid *pBand = (CSG_Grid *)m_Bands[i];

		if( Get_System() == pBand->Get_System() )
		{
			if( pBand->is_NoData(x, y) )
			{
				return( false );
			}

			Values[i] = pBand->asDouble(x, y);
		}
		else if( !pBand->Get_Value(Get_System().Get_Grid_to_World(x, y), Values[i], GRID_RESAMPLING_BSpline) )
		{
			return( false );
		}
	}

	return( _finite(Value = m_Formula.Get_Value(Values)) != 0 );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
