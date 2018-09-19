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
//                    Table_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        Fit.cpp                        //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

#include "LMFit.h"
#include <vector>
#include <math.h>
#include <string.h>

#include "Fit.h"

#define EPS 0.001

CSG_Formula Formel;

char vars[26]; 

double NUG(double x)
{
	if (x > 0)
		return 1.0;
	else return 0.0;
}

double SPH(double x, double a)
{
	if (x < 0)
		return 0.0;
	
	if (x > a)
		return 1.0;
	
	double val = x/a;
	
	return (1.5 - 0.5*val*val)*val;
}

double EXP(double x, double a)
{
	if (x < 0)
		return 0.0;
	return 1.0 - exp(-x/a);
}

double LIN(double x, double a)
{
	if (a == 0.0)
		return x;
	
	if (x < a)
		return x/a;
	return x;
}


CFit::CFit(void)
{
	Set_Name(_TL("Function Fit"));
	
	Set_Description(_TL("CFit\n(created by SAGA Wizard)."));
	
	CSG_Parameter	*pNode;
	
	pNode	= Parameters.Add_Table(NULL	, "SOURCE"		, _TL("Source")			, _TL(""), PARAMETER_INPUT);
	
	Parameters.Add_Table_Field(pNode	, "YFIELD"		, _TL("y - Values")				, _TL(""));
	
	Parameters.Add_Choice(pNode, "USE_X", _TL("Use x -Values"), _TL(""), _TL("No|Yes|"));
	Parameters.Add_Table_Field(pNode	, "XFIELD"		, _TL("x - Values")				, _TL(""));
	
	Parameters.Add_String(NULL,	"FORMEL", _TL("Formula"), 
		_TW(
		"The following operators are available for the formula definition:\n"
		"+ Addition\n"
		"- Subtraction\n"
		"* Multiplication\n"
		"/ Division\n"
		"^ power\n"
		"sin(x)\n"
		"cos(x)\n"
		"tan(x)\n"
		"asin(x)\n"
		"acos(x)\n"
		"atan(x)\n"
		"abs(x)\n"
		"sqrt(x)\n\n"

		"For Variogram - Fitting you can use the following Variogram - Models:\n"
		"NUG(x)\n"
		"SPH(x,a)\n"
		"EXP(x,a)\n"
		"LIN(x,a)\n"
		
		"The Fitting variables are single characters like a,b,m .. "
		"alphabetical order with the grid list order ('a'= first var, 'b' = second grid, ...)\n"
		"Example: m*x+a \n"),
				
		SG_T("m*x+c"));
	
	Parameters.Add_Value(NULL, "ITER", _TL("Iterationen"), _TL(""), PARAMETER_TYPE_Int, 1000, 1, true);
	
	Parameters.Add_Value(NULL, "LAMDA", _TL("Max Lamda"), _TL(""), PARAMETER_TYPE_Double, 10000, 1, true);

	Formel.Add_Function(SG_T("NUG"), (TSG_PFNC_Formula_1) NUG, 1, 0);
	Formel.Add_Function(SG_T("SPH"), (TSG_PFNC_Formula_1) SPH, 2, 0);
	Formel.Add_Function(SG_T("EXP"), (TSG_PFNC_Formula_1) EXP, 2, 0);
	Formel.Add_Function(SG_T("LIN"), (TSG_PFNC_Formula_1) LIN, 2, 0);
}

int CFit::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if (pParameter->Cmp_Identifier(SG_T("FORMEL")) )
	{
		CSG_String Msg;

		Formel.Set_Formula(pParameters->Get_Parameter("FORMEL")->asString());
	
		if( Formel.Get_Error(Msg) )
		{
			Error_Set  (Msg);
			Message_Dlg(Msg);

			return( -1 );
		}
	}

	return( 0 );
}

CFit::~CFit(void)
{}

void FitFunc(double x, vector < double> ca, double &y, vector < double> &dyda, int na)
{
	int		i;

	for(i = 0; i < na; i++)
	{
		Formel.Set_Variable(vars[i], ca[i]);
	}
	
	y= Formel.Get_Value(x);
	
	for (i = 0; i < na; i++)
	{
		Formel.Set_Variable(vars[i], ca[i] + EPS);
		
		dyda[i] = Formel.Get_Value(x);
		dyda[i] -= y;
		dyda[i] /= EPS;
		
		Formel.Set_Variable(vars[i], ca[i] - EPS);
	}
}
// MinGW Error !!!
// Fit.cpp:199: error: name lookup of `i' changed for new ISO `for' scoping
// Fit.cpp:192: error:   using obsolete binding at `i'

bool CFit::On_Execute(void)
{
	int i, j,  NrVars;
	vector < double> x, y, StartValue, Result;
	CSG_String	msg;	
	
	CSG_Parameters StartParameters;

	const SG_Char *formel	=	Parameters("FORMEL")->asString();

	Formel.Set_Formula(formel);
	
	
	if (Formel.Get_Error(msg))
	{
		Message_Add(msg);

		return false;
	}
	
	const SG_Char *uservars = NULL;
	
	uservars = Formel.Get_Used_Variables();
	

	NrVars	=	0;
	for (i = 0; i < SG_STR_LEN(uservars); i++)
	{
		if (uservars[i] >='a' && uservars[i] <= 'z')
		{
			if (uservars[i] != 'x')
				vars[NrVars++] = (char)(uservars[i]);
		}
	}
	
	vars[NrVars] =(char) 0;
	
	StartParameters.Add_Info_String(NULL, _TL(""), _TL("Formula"), _TL("Formula"), formel);
	
	for (i = 0; i < strlen(vars); i++)
	{
		CSG_String	c(vars[i]);
		StartParameters.Add_Value(NULL, c, c, _TL("Start Value"), PARAMETER_TYPE_Double, 1.0);
	}
	
	Dlg_Parameters(&StartParameters, _TL("Start Values"));
	
	for (i = 0; i < strlen(vars); i++)
	{
		char c[3];
		sprintf(c, "%c", vars[i]);
		StartValue.push_back(StartParameters(c)->asDouble());
	}
	
	CSG_Table	*pTable	= Parameters("SOURCE")->asTable();
	int Record_Count = pTable->Get_Record_Count();
	
	int	yField		= Parameters("YFIELD")->asInt();
	int	xField		= Parameters("XFIELD")->asInt();
	bool Use_X		= Parameters("USE_X")->asBool();
	
	pTable->Add_Field(_TL("Fit")				, SG_DATATYPE_Double);	
	
	for (i = 0; i < Record_Count; i++)
	{
		CSG_Table_Record *	Record = pTable->Get_Record(i);
		if (Use_X)
		{
			x.push_back(Record->asDouble(xField));
		}
		else
		{
			x.push_back(i);
		}
		
		y.push_back(Record->asDouble(yField));
	}
	
	TLMFit *Fit;
	
	Fit = new TLMFit(x, y, StartValue,  FitFunc);
	
	int max_iter = Parameters("ITER")->asInt();
	double Max_lamda = Parameters("LAMDA")->asInt();
	
	int iter = 0; 
	
	try
	{
		Fit->Fit();
		
		while ((Fit->Alamda() < Max_lamda) &&(iter < max_iter) &&Process_Get_Okay(true))
		{
			Fit->Fit();
			iter++;
		}
	}
	catch (ESingularMatrix &E)
	{
		if (E.Type == 1 || E.Type == 2)
		{
			msg.Printf(_TL("Matrix signular\n"));
			
			Message_Add(msg);
			
			return false;
		}
	}
	
	Result    = Fit->Param();
	
	for (i = 0; i < NrVars; i++)
	{
		Formel.Set_Variable(vars[i], (double) Result[i]);
	}
	
	msg.Printf(_TL("Model Parameters:"));
	Message_Add(msg);
	for (i = 0; i < NrVars; i++)
	{
		msg.Printf(SG_T("%c = %f\n"), vars[i], Result[i]);
		Message_Add(msg);
	}
	
	msg.Printf(_TL("\nRMS  of Residuals (stdfit):\t%f\n"), sqrt(Fit->Chisq()/x.size()));
	Message_Add(msg);
	
	msg.Printf(_TL("Correlation Matrix of the Fit Parameters:\n"));
	Message_Add(msg);
	
	vector< vector < double> > covar = Fit->Covar();
	
	msg.Printf(_TL(""));
	for (j = 0; j < NrVars; j++)
		msg.Printf(SG_T("%s\t%c"), msg.c_str(), vars[j]);
	
	msg.Printf(SG_T("%s\n"), msg.c_str());
	
	Message_Add(msg);
	
	for (i = 0; i < NrVars; i++)
	{
		msg.Printf(SG_T("%c"), vars[i]);
		for (j = 0; j <= i; j++)
		{	
			msg.Printf(SG_T("%s\t%f"), msg.c_str(), covar[i][j]/covar[i][i]);
		}
		msg.Printf(SG_T("%s\n"), msg.c_str());
		
		Message_Add(msg);
	}
	
	int Field_Count  = pTable->Get_Field_Count();
	
	for (i = 0; i < Record_Count; i++)
	{
		CSG_Table_Record *	Record = pTable->Get_Record(i);
		
		Record->Set_Value(Field_Count - 1, Formel.Get_Value(x[i]));
	}

//	API_FREE (uservars);
	return (true);
}
