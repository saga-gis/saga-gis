
///////////////////////////////////////////////////////////
//                                                       //
//                        SAGA                           //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Tool Library                       //
//                    sim_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                soilwater_glugla.cpp                   //
//                                                       //
//                Olaf Conrad (C) 2020                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 3 of the     //
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
#include "soilwater_glugla.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSoilWater_Model
{
public:
	CSoilWater_Model(void)
	{
		Create();
	}

	virtual ~CSoilWater_Model(void)
	{
		Destroy();
	}

	virtual bool	Create				(void)
	{
		return( Destroy() );
	}

	virtual bool	Destroy				(void)
	{
		m_bRooting	= false;
		m_Depth		= 0.;

		m_Layers.Create(sizeof(TLayer));

		Set_Storage_Leaf  ();
		Set_Storage_Litter();

		return( true );
	}

	//-----------------------------------------------------
	virtual bool	Set_Storage_Leaf	(double I_max = 0., double LAI_min = 0., double LAI_max = 0.)
	{
		m_Leaf[0]	= LAI_min;
		m_Leaf[1]	= LAI_max;
		m_Leaf[2]	=   I_max;

		return( true );
	}

	//-----------------------------------------------------
	virtual bool	Set_Storage_Litter	(double Litter_max = 0., double CF = 1., double Water = 0.)
	{
		if( CF > 0. )
		{
			m_Litter[0]	=   Water;
			m_Litter[1]	=   Litter_max;
			m_Litter[2]	=   CF;

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool	Set_Glugla_Coeff	(double Glugla)
	{
		if( Glugla > 0. )
		{
			m_Glugla	= Glugla;

			return( true );
		}

		return( false );
	}

	bool			Set_Rooting			(bool bOn = true)	{	m_bRooting = bOn; return( bOn );	}

	//-----------------------------------------------------
	bool			Add_Layer			(double Depth, double Water, double FC, double PWP, double ETmax, double Rooting)
	{
		if( Depth > 0. && m_Layers.Inc_Array() )
		{
			TLayer	&Layer	= *(TLayer *)m_Layers[m_Layers.Get_Size() - 1];

			Layer.Depth   = Depth;
			Layer.Water   = Depth * Water / 100.;	// [vol.%] -> [mm]
			Layer.FC      = Depth * FC    / 100.;	// [vol.%] -> [mm]
			Layer.PWP     = Depth * PWP   / 100.;	// [vol.%] -> [mm]
			Layer.ETmax   = Depth * ETmax / 100.;	// [vol.%] -> [mm]
			Layer.Rooting = Rooting       / 100.;

			m_Depth	+= Depth;

			return( true );
		}

		return( false );
	}

	size_t			Get_nLayers			(void)		const	{	return( m_Layers.Get_Size() );	}

	virtual double	Get_Litter			(void)		const	{	return( m_Litter[0] );	}

	virtual double	Get_Water			(size_t i)	const	{	return( Get_Layer(i).Water  );	}

	//-----------------------------------------------------
	virtual bool	Set_Balance			(double &P, double &ETp, double LAI = 0.)
	{
		if( m_Glugla <= 0. || m_Depth <= 0. )
		{
			return( false );
		}

		Set_Interception_Leaf  (P, ETp, LAI, m_Leaf[0], m_Leaf[1], m_Leaf[2]);
		Set_Interception_Litter(P, ETp, m_Litter[0], m_Litter[1], m_Litter[2]);

		double	Lambda	= m_Glugla / (m_Depth*m_Depth);

		for(size_t i=0; i<Get_nLayers(); i++)
		{
			if( !Set_Soil_Water(P, ETp, Get_Layer(i).Water, Get_FC(i), Get_PWP(i), Get_ETmax(i), m_bRooting ? Get_Rooting(i) : -1., Lambda) )
			{
				return( false );
			}
		}

		return( true );
	}


protected:

	bool			m_bRooting;

	double			m_Depth, m_Glugla, m_Leaf[3], m_Litter[3];

	typedef struct SLayer
	{
		double	Depth, Water, FC, PWP, ETmax, Rooting;
	}
	TLayer;

	CSG_Array		m_Layers;

	TLayer &		Get_Layer		(size_t i)			{	return( *(TLayer *)m_Layers[i] );	}
	const TLayer &	Get_Layer		(size_t i)	const	{	return( *(TLayer *)m_Layers[i] );	}

	virtual double	Get_Depth		(void)		const	{	return( m_Depth );	}
	virtual double	Get_Depth		(size_t i)	const	{	return( Get_Layer(i).Depth    );	}
	virtual double	Get_FC			(size_t i)	const	{	return( Get_Layer(i).FC       );	}
	virtual double	Get_PWP			(size_t i)	const	{	return( Get_Layer(i).PWP      );	}
	virtual double	Get_uFC			(size_t i)	const	{	return( Get_FC(i) - Get_PWP(i));	}
	virtual double	Get_ETmax		(size_t i)	const	{	return( Get_Layer(i).ETmax    );	}
	virtual double	Get_Rooting		(size_t i)	const	{	return( m_bRooting ? Get_Layer(i).Rooting : -1. );	}

	//-----------------------------------------------------
	static bool		Set_Interception_Leaf	(double &P, double &ETp, double LAI, double LAI_min, double LAI_max, double I_max)
	{
		if( LAI > 0. && LAI_max > 0. && I_max > 0. )
		{
			double	I	= LAI_min + LAI * I_max / LAI_max;

			if( I > ETp )
			{
				I	= ETp;
			}

			if( P > I )
			{
				ETp	-= I;
				P	-= I;
			}
			else // P <= I
			{
				ETp	-= P;
				P	 = 0.;
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	static bool		Set_Interception_Litter	(double &P, double &ETp, double &WC, double WC_max, double CF)
	{
		if( WC_max > 0. && CF > 0. )
		{
			double	I	= M_GET_MIN(WC_max, WC + P) / CF;

			if( I > ETp )
			{
				I	= ETp;
			}

			double	_WC	= WC + P - I;

			WC	 = _WC > WC_max ? WC_max : _WC;
			P	 = _WC > WC ? _WC - WC : 0.;
			ETp	-= I - (_WC < 0. ? _WC : 0.);
		}

		return( true );
	}

	//-----------------------------------------------------
	static bool		Set_Soil_Water	(double &P, double &ETp, double &Water, double FC, double PWP, double ETmax, double Rooting, double Lambda)
	{
		Water		+= P;

		double	ETa	 = Water > ETmax ? ETp : ETp * (Water - PWP) / (ETmax - PWP);

		if( Rooting >= 0. && Water <= ETmax )
		{
			double	d	= ETp * Rooting;

			if( ETa > d )
			{
				ETa	= d;
			}
		}

		Water		-= ETa;

		if( Water > FC )
		{
			P		 = Water - FC;
			Water	 =         FC;
		}
		else
		{
			double g = Lambda * SG_Get_Square(Water - PWP);	// Glugla

			P		 = g;
			Water	-= g;
		}

		ETp			-= ETa;

		return( true );
	}

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_REFERENCES	\
	Add_Reference("Glugla, G.", "1969",\
		"Berechnungsverfahren zur Ermittlung des aktuellen Wassergehalts und Gravitationswasserabflusses im Boden",\
		"Archives of Agronomy and Soil Science, 13(4):371-376.",\
		SG_T("https://doi.org/10.1080/03650346909413005"), SG_T("doi:10.1080/03650346909413005")\
	);\
	\
	Add_Reference("Hoermann, G.", "1997",\
		"SIMPEL - ein einfaches, benutzerfreundliches Bodenwassermodell zum Einsatz in der Ausbildung",\
		"Deutsche Gewässerkundliche Mitteilungen 41(2):67-72.",\
		SG_T("https://www.hydrology.uni-kiel.de/download/projekte/simpel/englisch/low_end_hydrology.pdf"), SG_T("PDF (unpublished english translation)")\
	);\
	\
	Add_Reference(\
		"https://www.hydrology.uni-kiel.de/en/research/projects/simpel-en/simpel-models-en",\
		SG_T("Simpel Homepage")\
	);\

//	Add_Reference("Bonta, J.V., Mueller, M.", "1999",
//		"Evaluation of the Glugla method for estimating evapotranspiration and groundwater recharge",
//		"Hydrological Sciences Journal, 44(5):743-761.",
//		SG_T("https://doi.org/10.1080/02626669909492271"), SG_T("doi:10.1080/02626669909492271")
//	);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoilWater_Glugla_Table::CSoilWater_Glugla_Table(void)
{
	Set_Name	(CSG_String::Format("%s (%s)", _TL("Soil Water Simulation after Glugla"), _TL("Table")));

	Set_Author	("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"A simple model for daily soil water simulation based on the approach of Glugla (1969).\n"
		"\n"
		"This is a re-implementation of the formulas used in the 'Simpel' model developed by "
		"Hoermann (1998), \"...covering the lowest, serious end of hydrologic computing\". "
	));

	ADD_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Table("",
		"INPUT"			, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("INPUT",
		"INPUT_DAY"		, _TL("Day Identifier"),
		_TL("Day specifier, e.g. a date field."),
		true
	);

	Parameters.Add_Table_Field("INPUT",
		"INPUT_P"		, _TL("Precipitation"),
		_TL("[mm]")
	);

	Parameters.Add_Table_Field("INPUT",
		"INPUT_ETP"		, _TL("Potential Evapotranspiration"),
		_TL("[mm]")
	);

	Parameters.Add_Table_Field_or_Const("INPUT",
		"INPUT_LAI"		, _TL("Leaf Area Index"),
		_TL(""),
		0.
	);

	Parameters.Add_Table("",
		"SIMULATION"	, _TL("Simulation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"I_MAX"			, _TL("Interception Capacity"),
		_TL("Leaf interception capacity [mm] at maximum LAI."),
		2., 0., true
	);

	Parameters.Add_Double("I_MAX",
		"LAI_MIN"		, _TL("Minimum LAI"),
		_TL("Minimum leaf area index."),
		0.1, 0., true
	);

	Parameters.Add_Double("I_MAX",
		"LAI_MAX"		, _TL("Maximum LAI"),
		_TL("Maximum leaf area index."),
		4., 0., true
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"LITTER_MAX"	, _TL("Litter Capacity"),
		_TL("[mm]"),
		0., 0., true
	);

	Parameters.Add_Double("LITTER_MAX",
		"LITTER_CF"		, _TL("Litter Drying Factor"),
		_TL("Curvature factor determining the drying of the litter storage."),
		3., 0., true
	);

	Parameters.Add_Double("LITTER_MAX",
		"LITTER_0"		, _TL("Initial Litter Water Content"),
		_TL("[mm]"),
		0., 0., true
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"GLUGLA"		, _TL("Glugla Coefficient"),
		_TL("Empirical parameter [mm/day], depends on soil texture and structure."),
		150., 0., true
	);

	Parameters.Add_Bool("",
		"ROOTING"		, _TL("Rooting"),
		_TL(""),
		false
	);

	CSG_Table	&Layers	= *Parameters.Add_FixedTable("",
		"SOIL_LAYERS"	, _TL("Soil Layers"),
		_TL("Provide a row for each soil layer: depth [cm], field capacity [vol.%], permanent wilting point [vol.%], maximum ET [vol.%/day], rooting [% of total], initial water content [vol.%].")
	)->asTable();

	Layers.Add_Field("Depth"  , SG_DATATYPE_Double);
	Layers.Add_Field("FC"     , SG_DATATYPE_Double);
	Layers.Add_Field("PWP"    , SG_DATATYPE_Double);
	Layers.Add_Field("ETmax"  , SG_DATATYPE_Double);
	Layers.Add_Field("Rooting", SG_DATATYPE_Double);
	Layers.Add_Field("Water_0", SG_DATATYPE_Double);

	#define ADD_LAYER(FC, PWP, ETmax, Depth, Rooting, Water_0) { CSG_Table_Record &r = *Layers.Add_Record();\
		r.Set_Value("Depth"  , Depth  );\
		r.Set_Value("FC"     , FC     );\
		r.Set_Value("PWP"    , PWP    );\
		r.Set_Value("ETmax"  , ETmax  );\
		r.Set_Value("Rooting", Rooting);\
		r.Set_Value("Water_0", Water_0);\
	}

	ADD_LAYER(25, 12, 20, 30, 50, 20);
	ADD_LAYER(25, 12, 20, 20, 30, 20);
	ADD_LAYER(25, 12, 20, 20, 15, 20);
	ADD_LAYER(25, 12, 20, 10,  5, 20);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSoilWater_Glugla_Table::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("I_MAX") )
	{
		pParameters->Set_Enabled("LAI_MIN"  , pParameter->asDouble() > 0.);
		pParameters->Set_Enabled("LAI_MAX"  , pParameter->asDouble() > 0.);
	}

	if( pParameter->Cmp_Identifier("LITTER_MAX") )
	{
		pParameters->Set_Enabled("LITTER_CF", pParameter->asDouble() > 0.);
		pParameters->Set_Enabled("LITTER_0" , pParameter->asDouble() > 0.);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoilWater_Glugla_Table::On_Execute(void)
{
	CSG_Table &Input = *Parameters("INPUT")->asTable();

	int	field_Day = Parameters("INPUT_DAY")->asInt();
	int	field_P   = Parameters("INPUT_P"  )->asInt();
	int	field_ETp = Parameters("INPUT_ETP")->asInt();
	int	field_LAI = Parameters("INPUT_LAI")->asInt();

	double	LAI	= Parameters("INPUT_LAI")->asDouble();

	//-----------------------------------------------------
	CSG_Table &Simulation = *Parameters("SIMULATION")->asTable();

	Simulation.Destroy();
	Simulation.Set_Name(_TL("Soil Water Simulation"));

	if( field_Day >= 0 )
	{
		Simulation.Add_Field(Input.Get_Field_Name(field_Day), Input.Get_Field_Type(field_Day));
	}
	else
	{
		Simulation.Add_Field("Day", SG_DATATYPE_Int);
	}

	Simulation.Add_Field("LAI"     , SG_DATATYPE_Double);
	Simulation.Add_Field("P"       , SG_DATATYPE_Double);
	Simulation.Add_Field("ETpot"   , SG_DATATYPE_Double);
	Simulation.Add_Field("ETact"   , SG_DATATYPE_Double);
	Simulation.Add_Field("Litter"  , SG_DATATYPE_Double);
	Simulation.Add_Field("Recharge", SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSoilWater_Model	Model;

	Model.Set_Storage_Leaf(
		Parameters(  "I_MAX"   )->asDouble(),
		Parameters("LAI_MIN"   )->asDouble(),
		Parameters("LAI_MAX"   )->asDouble()
	);

	Model.Set_Storage_Litter(
		Parameters("LITTER_MAX")->asDouble(),
		Parameters("LITTER_CF" )->asDouble(),
		Parameters("LITTER_0"  )->asDouble()
	);

	Model.Set_Rooting(
		Parameters("ROOTING"   )->asBool  ()
	);

	Model.Set_Glugla_Coeff(
		Parameters("GLUGLA"    )->asDouble()
	);

	{
		CSG_Table	&Layers	= *Parameters("SOIL_LAYERS")->asTable();

		for(int i=0; i<Layers.Get_Count(); i++)
		{
			Model.Add_Layer(
				Layers[i].asDouble("Depth"  ) * 10.,	// [cm] -> [mm]
				Layers[i].asDouble("Water_0"),
				Layers[i].asDouble("FC"     ),
				Layers[i].asDouble("PWP"    ),
				Layers[i].asDouble("ETmax"  ),
				Layers[i].asDouble("Rooting")
			);

			Simulation.Add_Field(CSG_String::Format("SWC%d", i + 1), SG_DATATYPE_Double);
		}
	}

	//-----------------------------------------------------
	for(int i=0; i<Input.Get_Count() && Set_Progress(i, Input.Get_Count()); i++)
	{
		CSG_Table_Record	&Record	= *Simulation.Add_Record();

		if( field_Day >= 0 )
		{
			Record.Set_Value(0, Input[i].asString(field_Day));
		}
		else
		{
			Record.Set_Value(0, i + 1);
		}

		double	P   = Input[i].asDouble(field_P  );
		double	ETp = Input[i].asDouble(field_ETp);

		if( field_LAI >= 0 )
		{
			LAI     = Input[i].asDouble(field_LAI);
		}

		Record.Set_Value("P"       , P  );
		Record.Set_Value("ETpot"   , ETp);
		Record.Set_Value("LAI"     , LAI);

		Model.Set_Balance(P, ETp, LAI);

		Record.Set_Value("ETact"   , Input[i].asDouble(field_ETp) - ETp);
		Record.Set_Value("Litter"  , Model.Get_Litter());
		Record.Set_Value("Recharge", P  );

		for(size_t iLayer=0; iLayer<Model.Get_nLayers(); iLayer++)
		{
			Record.Set_Value(CSG_String::Format("SWC%d", iLayer + 1), Model.Get_Water(iLayer));
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
class CSoilWater_Model_Grid : public CSoilWater_Model
{
public:
	CSoilWater_Model_Grid(void)
	{
		m_pP       = NULL;
		m_pETp     = NULL;
		m_pLAI     = NULL;

		m_pGlugla  = NULL;
		m_pLitter  = NULL;

		m_pFC      = NULL;
		m_pPWP     = NULL;
		m_pETmax   = NULL;
		m_pRooting = NULL;

		m_pWater   = NULL;

		Create();
	}

	//-----------------------------------------------------
	void			Set_Dynamic_Input	(CSG_Grid *pP, CSG_Grid *pETp, CSG_Grid *pLAI, double LAI)
	{
		m_pP		= pP;
		m_pETp		= pETp;
		m_pLAI		= pLAI;
		m_LAI		= LAI;
	}

	void			Set_Static_Input	(CSG_Grid *pGlugla, double Glugla, CSG_Parameter_Grid_List *pFC, CSG_Parameter_Grid_List *pPWP, CSG_Parameter_Grid_List *pETmax, CSG_Parameter_Grid_List *pRooting)
	{
		m_pGlugla	= pGlugla;
		m_Glugla	= Glugla;
		m_pFC		= pFC;
		m_pPWP		= pPWP;
		m_pETmax	= pETmax;
		m_pRooting	= pRooting;
	}

	virtual void	Set_Storage			(CSG_Grid *pLitter, CSG_Parameter_Grid_List *pWater)
	{
		m_pLitter	= pLitter;
		m_pWater	= pWater;
	}

	//-----------------------------------------------------
	virtual double	Get_Litter			(int x, int y)	const	{	return( m_Litter[0] );	}

	virtual double	Get_Water			(int x, int y, size_t i)	const	{	return( Get_Layer(i).Water  );	}

	//-----------------------------------------------------
	virtual bool	Set_Balance			(int x, int y)
	{
		if( m_Depth <= 0. || m_Glugla <= 0. )
		{
			return( false );
		}

		TSG_Point	p	= m_pWater->Get_Grid(0)->Get_System().Get_Grid_to_World(x, y);

		double	P, ETp;
		
		if( !m_pP->Get_Value(p, P) || !m_pETp->Get_Value(p, ETp) )
		{
			for(int i=0; i<m_pWater->Get_Grid_Count(); i++)
			{
				m_pWater->Get_Grid(i)->Set_NoData(x, y);
			}

			return( false );
		}

		//-------------------------------------------------
		double	LAI;
		
		if( m_pLAI == NULL || m_pLAI->Get_Value(p, LAI) == false )
		{
			LAI	= m_LAI;
		}

		Set_Interception_Leaf(P, ETp, LAI, m_Leaf[0], m_Leaf[1], m_Leaf[2]);

		//-------------------------------------------------
		if( m_pLitter )
		{
			double	Litter	= m_pLitter->asDouble(x, y);

			Set_Interception_Litter(P, ETp, Litter, m_Litter[1], m_Litter[2]);

			m_pLitter->Set_Value(x, y, Litter);
		}

		//-------------------------------------------------
		double	Lambda;

		if( m_pGlugla == NULL || m_pGlugla->Get_Value(p, Lambda) == false )
		{
			Lambda	= m_Glugla;
		}

		Lambda	/= m_Depth*m_Depth;

		for(size_t i=0; i<Get_nLayers(); i++)
		{
			double	Water	= m_pWater->Get_Grid((int)i)->asDouble(x, y);

			if( !Set_Soil_Water(P, ETp, Water, Get_FC(p, i), Get_PWP(p, i), Get_ETmax(p, i), Get_Rooting(p, i), Lambda) )
			{
				return( false );
			}

			if( Water >= 0. )
				m_pWater->Get_Grid((int)i)->Set_Value(x, y, Water);
		}

		return( true );
	}


protected:

	double						m_LAI;

	CSG_Grid					*m_pP, *m_pETp, *m_pLAI, *m_pGlugla, *m_pLitter;

	CSG_Parameter_Grid_List		*m_pFC, *m_pPWP, *m_pETmax, *m_pRooting, *m_pWater;


	virtual double				_Get_Layer	(const TSG_Point &p, size_t i, CSG_Parameter_Grid_List *pGrids, double Default)	const
	{
		double	Value;

		if( pGrids && pGrids->Get_Grid((int)i) && pGrids->Get_Grid((int)i)->Get_Value(p, Value) )
		{
			return( Value );
		}

		return( Default );
	}

	virtual double	Get_FC		(const TSG_Point &p, size_t i)	const	{	return( _Get_Layer(p, i, m_pFC , CSoilWater_Model::Get_FC (i)) );	}
	virtual double	Get_PWP		(const TSG_Point &p, size_t i)	const	{	return( _Get_Layer(p, i, m_pPWP, CSoilWater_Model::Get_PWP(i)) );	}
	virtual double	Get_ETmax	(const TSG_Point &p, size_t i)	const	{	return( _Get_Layer(p, i, m_pPWP, CSoilWater_Model::Get_PWP(i)) );	}
	virtual double	Get_Rooting	(const TSG_Point &p, size_t i)	const	{	return( m_bRooting ? _Get_Layer(p, i, m_pPWP, CSoilWater_Model::Get_PWP(i)) : -1. );	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSoilWater_Glugla_Grid::CSoilWater_Glugla_Grid(void)
{
	Set_Name	(CSG_String::Format("%s (%s)", _TL("Soil Water Simulation after Glugla"), _TL("Grid")));

	Set_Author	("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"A simple model for daily soil water simulation based on the approach of Glugla (1969).\n"
		"\n"
		"This is a re-implementation of the formulas used in the 'Simpel' model developed by "
		"Hoermann (1998), \"...covering the lowest, serious end of hydrologic computing\". "
	));

	ADD_REFERENCES;

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "P"  , _TL("Precipitation"               ), _TL("[mm]"), PARAMETER_INPUT, false);
	Parameters.Add_Grid_List("", "ETP", _TL("Potential Evapotranspiration"), _TL("[mm]"), PARAMETER_INPUT, false);

	//-----------------------------------------------------
	Parameters.Add_Grid_or_Const("",
		"LAI"			, _TL("Leaf Area Index"),
		_TL(""),
		0., 0., true, 0., false, false
	);

	Parameters.Add_Grid_or_Const("",
		"GLUGLA"		, _TL("Glugla Coefficient"),
		_TL("Empirical parameter [mm/day], depends on soil texture and structure."),
		150., 0., true, 0., false, false
	);

	Parameters.Add_Grid_List("", "FC"     , _TL("Field Capacity"             ), _TL("[vol.%]"     ), PARAMETER_INPUT_OPTIONAL, false);
	Parameters.Add_Grid_List("", "PWP"    , _TL("Permanent Wilting Point"    ), _TL("[vol.%]"     ), PARAMETER_INPUT_OPTIONAL, false);
	Parameters.Add_Grid_List("", "ETMAX"  , _TL("Maxmimum Evapotranspiration"), _TL("[vol.%/day]" ), PARAMETER_INPUT_OPTIONAL, false);
//	Parameters.Add_Grid_List("", "ROOTING", _TL("Rooting"                    ), _TL("[% of total]"), PARAMETER_INPUT_OPTIONAL, false);

	Parameters.Add_Grid_List("",
		"SOIL_WATER"	, _TL("Soil Water Content"),
		_TL("[mm]" ),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"LITTER"		, _TL("Litter Water Content"),
		_TL("[mm]"),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"I_MAX"			, _TL("Interception Capacity"),
		_TL("Leaf interception capacity [mm] at maximum LAI."),
		2., 0., true
	);

	Parameters.Add_Double("I_MAX",
		"LAI_MIN"		, _TL("Minimum LAI"),
		_TL("Minimum leaf area index."),
		0.1, 0., true
	);

	Parameters.Add_Double("I_MAX",
		"LAI_MAX"		, _TL("Maximum LAI"),
		_TL("Maximum leaf area index."),
		4., 0., true
	);

	//-----------------------------------------------------
	Parameters.Add_Double("",
		"LITTER_MAX"	, _TL("Litter Capacity"),
		_TL("[mm]"),
		0., 0., true
	);

	Parameters.Add_Double("LITTER_MAX",
		"LITTER_CF"		, _TL("Litter Drying Factor"),
		_TL("Curvature factor determining the drying of the litter storage."),
		3., 0., true
	);

	Parameters.Add_Double("LITTER",
		"LITTER_0"		, _TL("Initial Litter Water Content"),
		_TL("[mm]"),
		0., 0., true
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"RESET"			, _TL("Reset"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"ROOTING"		, _TL("Rooting"),
		_TL(""),
		false
	);

	CSG_Table	&Layers	= *Parameters.Add_FixedTable("",
		"SOIL_LAYERS"	, _TL("Soil Layers"),
		_TL("Provide a row for each soil layer: depth [cm], field capacity [vol.%], permanent wilting point [vol.%], maximum ET [vol.%/day], rooting [% of total], initial water content [vol.%].")
	)->asTable();

	Layers.Add_Field("Depth"  , SG_DATATYPE_Double);
	Layers.Add_Field("FC"     , SG_DATATYPE_Double);
	Layers.Add_Field("PWP"    , SG_DATATYPE_Double);
	Layers.Add_Field("ETmax"  , SG_DATATYPE_Double);
	Layers.Add_Field("Rooting", SG_DATATYPE_Double);
	Layers.Add_Field("Water_0", SG_DATATYPE_Double);

	ADD_LAYER(25, 12, 20, 30, 50, 20);
	ADD_LAYER(25, 12, 20, 20, 30, 20);
	ADD_LAYER(25, 12, 20, 20, 15, 20);
	ADD_LAYER(25, 12, 20, 10,  5, 20);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSoilWater_Glugla_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("I_MAX") )
	{
		pParameters->Set_Enabled("LAI_MIN"  , pParameter->asDouble() > 0.);
		pParameters->Set_Enabled("LAI_MAX"  , pParameter->asDouble() > 0.);
	}

	if( pParameter->Cmp_Identifier("LITTER_MAX") )
	{
		pParameters->Set_Enabled("LITTER_CF", pParameter->asDouble() > 0.);
		pParameters->Set_Enabled("LITTER_0" , pParameter->asDouble() > 0. && (*pParameters)("RESET")->asBool());
		pParameters->Set_Enabled("LITTER"   , pParameter->asDouble() > 0.);
	}

	if( pParameter->Cmp_Identifier("RESET") )
	{
		pParameters->Set_Enabled("LITTER_0" , pParameter->asBool() && (*pParameters)("LITTER_MAX")->asDouble() > 0.);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSoilWater_Glugla_Grid::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pP   = Parameters("P"  )->asGridList();
	CSG_Parameter_Grid_List	*pETp = Parameters("ETP")->asGridList();

	int	nDays	= M_GET_MIN(pP->Get_Grid_Count(), pETp->Get_Grid_Count());

	if( nDays < 1 )
	{
		return( false );
	}

	CSG_Grid *pLAI = Parameters("LAI")->asGrid  ();
	double     LAI = Parameters("LAI")->asDouble();

	//-----------------------------------------------------
	CSoilWater_Model_Grid	Model;

	Model.Set_Storage_Leaf(
		Parameters(  "I_MAX"   )->asDouble(),
		Parameters("LAI_MIN"   )->asDouble(),
		Parameters("LAI_MAX"   )->asDouble()
	);

	Model.Set_Storage_Litter(
		Parameters("LITTER_MAX")->asDouble(),
		Parameters("LITTER_CF" )->asDouble(),
		Parameters("LITTER_0"  )->asDouble()
	);

	Model.Set_Rooting(
		Parameters("ROOTING"   )->asBool  ()
	);

	Model.Set_Static_Input(
		Parameters("GLUGLA"    )->asGrid    (),
		Parameters("GLUGLA"    )->asDouble  (),
		Parameters("FC"        )->asGridList(),
		Parameters("PWP"       )->asGridList(),
		Parameters("ETMAX"     )->asGridList(),
		Parameters("ROOTING"   )->asGridList()
	);

	CSG_Table	&Layers	= *Parameters("SOIL_LAYERS")->asTable();

	for(int i=0; i<Layers.Get_Count(); i++)
	{
		Model.Add_Layer(
			Layers[i].asDouble("Depth"  ) * 10.,	// [cm] -> [mm]
			Layers[i].asDouble("Water_0"),
			Layers[i].asDouble("FC"     ),
			Layers[i].asDouble("PWP"    ),
			Layers[i].asDouble("ETmax"  ),
			Layers[i].asDouble("Rooting")
		);
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pWater	= Parameters("SOIL_WATER")->asGridList();

	CSG_Grid	*pLitter	= Parameters("LITTER")->asGrid();

	if( Parameters("RESET")->asBool() )
	{
		if( Parameters("LITTER_MAX")->asDouble() > 0. )
		{
			pLitter->Assign(Parameters("LITTER_0")->asDouble());
		}

		for(int i=pWater->Get_Grid_Count(); i<(int)Model.Get_nLayers(); i++)
		{
			pWater->Add_Item(SG_Create_Grid(Get_System()));
		}

		for(int i=0; i<(int)Model.Get_nLayers(); i++)
		{
			pWater->Get_Grid(i)->Fmt_Name("SWC_%d", i + 1);
			pWater->Get_Grid(i)->Assign(Layers[i].asDouble("Depth") * Layers[i].asDouble("Water_0") * 0.1);

			DataObject_Add(pWater->Get_Grid(i));
		}
	}

	Model.Set_Storage(pLitter, pWater);

	if( pWater->Get_Grid_Count() < (int)Model.Get_nLayers() )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int iDay=0; iDay<nDays && Set_Progress(iDay, nDays); iDay++)
	{
		Process_Set_Text(CSG_String::Format("%d/%d", iDay + 1, nDays));

		Model.Set_Dynamic_Input(pP->Get_Grid(iDay), pETp->Get_Grid(iDay), pLAI, LAI);

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			Model.Set_Balance(x, y);
		}

		SG_UI_ProgressAndMsg_Lock(true);
		for(int i=0; i<pWater->Get_Item_Count(); i++)
		{
			DataObject_Update(pWater->Get_Item(i));
		}
		SG_UI_ProgressAndMsg_Lock(false);
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
