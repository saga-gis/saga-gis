
///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "bsl_interpreter.h"
#include "basistypen.h"
#include "pars_all.h"
#include "auswert_anweisung.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBSL_Interpreter	*g_pInterpreter	= NULL;

void	g_Add_Grid		(CSG_Grid *pGrid)
{
	if( g_pInterpreter )
	{
		CSG_Grid	*p	= SG_Create_Grid(*pGrid);

		p->Set_Name(pGrid->Get_Name());

		g_pInterpreter->Get_Parameters()->Get_Parameter("OUTPUT")->asGridList()->Add_Item(p);
	}
}

//---------------------------------------------------------
bool				g_bProgress		= true;

bool	g_Set_Progress	(int i, int n)
{
	return( g_bProgress ? SG_UI_Process_Set_Progress(i, n) : SG_UI_Process_Get_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBSL_Interpreter::CBSL_Interpreter(bool bFile)
{
	m_bFile			= bFile;

	//-----------------------------------------------------
	Set_Name		(m_bFile ? _TL("BSL from File") : _TL("BSL"));

	Set_Author		(SG_T("SAGA User Group Associaton (c) 2009"));

	Set_Description	(_TW(
		"Boehner's Simple Language (BSL) is a macro script language for grid cell based calculations. "
		"BSL has been developed by C. Trachinow and J. Boehner originally as part of the grid analysis "
		"software SADO, 'System fuer die Analyse Diskreter Oberflaechen'. \n"
		"\n"
		"References:\n"
		"Boehner, J., Koethe, R., Trachinow, C. (1997): "
		"Weiterentwicklung der automatischen Reliefanalyse auf der Basis von digitalen Gelaendemodellen. – "
		"Göttinger Geogr. Abh. 100: 3-21."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	if( m_bFile )
	{
		Parameters.Add_FilePath(
			NULL, "BSL"		, _TL("BSL Script"),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s"),
				_TL("BSL Files (*.bsl)")	, SG_T("*.bsl"),
				_TL("Text Files (*.txt)")	, SG_T("*.txt"),
				_TL("All Files")			, SG_T("*.*")
			)
		);
	}
	else
	{
		Parameters.Add_String(
			NULL, "BSL"		, _TL("BSL Script"),
			_TL(""),
			_TW(
				"Matrix R(), NIR(), NDVI, RANGE;\n"
				"Point p;\n"
				"\n"
				"NDVI  = R;\n"
				"RANGE = R;\n"
				"\n"
				"foreach p in R do\n"
				"{\n"
				"  NDVI[p]  = (NIR[p] - R[p]) / (NIR[p] + R[p]);\n"
				"  RANGE[p] = max8(p, R) - min8(p, R);\n"
				"}\n"
				"\n"
				"showMatrix(NDVI);\n"
				"showMatrix(RANGE);\n"
			), true
		);
	}

	Parameters.Add_Value(
		NULL, "PROGRESS"	, _TL("Show Progress"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}

//---------------------------------------------------------
CBSL_Interpreter::~CBSL_Interpreter(void)
{
	g_pInterpreter	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBSL_Interpreter::On_Execute(void)
{
	//-----------------------------------------------------
	Parameters("OUTPUT")->asGridList()->Del_Items();

	g_bProgress	= Parameters("PROGRESS")->asBool();

	if( m_bFile )
	{
		CSG_File	Stream;
		
		if( !Stream.Open(Parameters("BSL")->asString(), SG_FILE_R, false) )
		{
			return( false );
		}

		Stream.Read(m_BSL, Stream.Length());
	}
	else
	{
		m_BSL	= Parameters("BSL")->asString();
	}

	//-----------------------------------------------------
	if( !Parse_Vars(false) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameters	Input(this, _TL("Input"), _TL(""), SG_T("INPUT"), true);

	FindMemoryGrids();

	for(T_InputText::iterator it=InputGrids.begin(); it!=InputGrids.end(); it++)
	{
		CSG_String	sName(it->c_str());

		Input.Add_Grid(NULL, sName, sName, _TL(""), PARAMETER_INPUT, true);
	}

	DeleteVarList();
	DeleteAnweisungList(AnweisungList);

	if( Dlg_Parameters(&Input, _TL("Input")) == false )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !Parse_Vars(true) )
	{
		return( false );
	}

	//-----------------------------------------------------
	g_pInterpreter	= this;

	if( GetMemoryGrids(&Input) )
	{
		try
		{
			ausfuehren_anweisung(AnweisungList);
		}
		catch(BBFehlerAusfuehren x)
		{
			if( x.Text == "" )
				Message_Add(_TL("unknown error: execution"));
			else
				Message_Add(CSG_String::Format(SG_T("error: %s\n"), CSG_String(x.Text.c_str()).c_str()));
		}
		catch(BBFehlerUserbreak x)
		{
			if( x.Text == "" )
				Message_Add(_TL("unknown error: user break"));
			else
				Message_Add(CSG_String::Format(SG_T("error: %s\n"), CSG_String(x.Text.c_str()).c_str()));
		}
	}

	g_pInterpreter	= NULL;

	DeleteVarList();
	DeleteAnweisungList(AnweisungList);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CBSL_Interpreter::Parse_Vars(bool bFlag)
{
	InputText.clear();

	CSG_String	s(m_BSL);

	while( s.Length() > 0 )
	{
		InputText.push_back(s.BeforeFirst('\n').b_str());

		s	= s.AfterFirst('\n');
	}

	InputText.push_back("\t\n\n");

	//-----------------------------------------------------
	try
	{
		int		zeile	= 0;
		int		p		= 0;
		isSyntaxCheck	= true;

		ParseVars		(zeile, p);
		AddMatrixPointVariables(bFlag);
		pars_ausdruck	(zeile, p);

		return( true );
	}
	catch (BBFehlerException)
	{
		Message_Add(CSG_String::Format(SG_T("error in line %d: %s\n"), FehlerZeile, CSG_String(FehlerString.c_str()).c_str()));

		DeleteVarList();
		DeleteAnweisungList(AnweisungList);

		return( false );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
