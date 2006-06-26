
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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

#include "Fit.h"

#define EPS 0.001

CMAT_Formula Formel;

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
	
	Set_Description("CFit\n" "(created by SAGA Wizard).");
	
	CParameter	*pNode;
	
	pNode	= Parameters.Add_Table(NULL	, "SOURCE"		, _TL("Source")			, "", PARAMETER_INPUT);
	
	Parameters.Add_Table_Field(pNode	, "YFIELD"		, _TL("y - Values")				, "");
	
	Parameters.Add_Choice(pNode, "USE_X", _TL("Use x -Values"), "", _TL("No|Yes|"));
	Parameters.Add_Table_Field(pNode	, "XFIELD"		, _TL("x - Values")				, "");
	
	Parameters.Add_String(NULL,	"FORMEL", _TL("Formula"), 
		_TL(
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

		"For Variogram - Fitting you can use the folowing Variogram - Models:\n"
		"NUG(x)\n"
		"SPH(x,a)\n"
		"EXP(x,a)\n"
		"LIN(x,a)\n"
		
		"The Fitting variables are single characters like a,b,m .. "
		"alphabetical order with the grid list order ('a'= first var, 'b' = second grid, ...)\n"
		"Example: m*x+a \n"),
				
		"m*x+c");
	
	Parameters.Add_Value(NULL, "ITER", _TL("Iterationen"), "", PARAMETER_TYPE_Int, 1000, 1, true);
	
	Parameters.Add_Value(NULL, "LAMDA", _TL("Max Lamda"), "", PARAMETER_TYPE_Double, 10000, 1, true);
}

int CFit::On_Parameter_Changed(CParameters *pParameters, CParameter *pParameter)
{
	bool retval = false;
	

	if (!strcmp(pParameter->Get_Identifier(), "FORMEL") )
	{
		const char * formel=pParameters->Get_Parameter("FORMEL")->asString();
		Formel.Set_Formula(formel);
	
		int Pos;
		const char * Msg;
		if (Formel.Get_Error(&Pos, &Msg))
		
		{
			char Str[1024];
			
			sprintf(Str, _TL("Error at character #%d of the function: \n%s\n%s\n"), Pos, formel);

			Error_Set(Str);			
		
			Message_Dlg(Str,Str);

			return false;
		}
	}
	return (retval);
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
	
	y= Formel.Val(x);
	
	for (i = 0; i < na; i++)
	{
		Formel.Set_Variable(vars[i], ca[i] + EPS);
		
		dyda[i] = Formel.Val(x);
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
	char msg[1024];	
	
	CParameters StartParameters;

	const char *formel	=	Parameters("FORMEL")->asString();

	Formel.Add_Function("NUG", (TMAT_Formula_Function_1) NUG, 1, 0);
	Formel.Add_Function("SPH", (TMAT_Formula_Function_1) SPH, 2, 0);
	Formel.Add_Function("EXP", (TMAT_Formula_Function_1) EXP, 2, 0);
    Formel.Add_Function("LIN", (TMAT_Formula_Function_1) LIN, 2, 0);
	
	
	Formel.Set_Formula(formel);
	
	
	int Pos;
	const char * ErrorMsg;
	if (Formel.Get_Error(&Pos, &ErrorMsg))
	{
		sprintf(msg, _TL("Error at character #%d of the function: \n%s\n"), Pos, formel);
		
		Message_Add(msg);
		
		sprintf(msg, "\n%s\n", ErrorMsg);
		
		Message_Add(msg);
		
		return false;
	}
	
	const char *uservars = NULL;
	
	uservars = Formel.Get_Used_Var();
	

	NrVars	=	0;
	for (i = 0; i < strlen(uservars); i++)
	{
		if (uservars[i] >='a' && uservars[i] <= 'z')
		{
			if (uservars[i] != 'x')
				vars[NrVars++] = uservars[i];
		}
	}
	
	vars[NrVars] =(char) 0;
	
	StartParameters.Add_Info_String(NULL, "", _TL("Formula"), _TL("Formula"), formel);
	
	for (i = 0; i < strlen(vars); i++)
	{
		char c[3];
		sprintf(c, "%c", vars[i]);
		StartParameters.Add_Value(NULL, c, c, _TL("Start Value"), PARAMETER_TYPE_Double, 1.0);
	}
	
	Dlg_Parameters(&StartParameters, _TL("Start Values"));
	
	for (i = 0; i < strlen(vars); i++)
	{
		char c[3];
		sprintf(c, "%c", vars[i]);
		StartValue.push_back(StartParameters(c)->asDouble());
	}
	
	CTable	*pTable	= Parameters("SOURCE")->asTable();
	int Record_Count = pTable->Get_Record_Count();
	
	int	yField		= Parameters("YFIELD")->asInt();
	int	xField		= Parameters("XFIELD")->asInt();
	bool Use_X		= Parameters("USE_X")->asBool();
	
	pTable->Add_Field(_TL("Fit")				, TABLE_FIELDTYPE_Double);	
	
	for (i = 0; i < Record_Count; i++)
	{
		CTable_Record *	Record = pTable->Get_Record(i);
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
			sprintf(msg, _TL("Matrix signular\n"));
			
			Message_Add(msg);
			
			return false;
		}
	}
	
	Result    = Fit->Param();
	
	for (i = 0; i < NrVars; i++)
	{
		Formel.Set_Variable(vars[i], (double) Result[i]);
	}
	
	sprintf(msg, _TL("Model Parameters:"));
	Message_Add(msg);
	for (i = 0; i < NrVars; i++)
	{
		sprintf(msg, "%c = %f\n", vars[i], Result[i]);
		Message_Add(msg);
	}
	
	sprintf(msg, _TL("\nRMS  of Residuals (stdfit):\t%f\n"), sqrt(Fit->Chisq()/x.size()));
	Message_Add(msg);
	
	sprintf(msg, _TL("Correlation Matrix of the Fit Parameters:\n"));
	Message_Add(msg);
	
	vector< vector < double> > covar = Fit->Covar();
	
	sprintf(msg, "");
	for (j = 0; j < NrVars; j++)
		sprintf(msg, "%s\t%c", msg, vars[j]);
	
	sprintf(msg, "%s\n", msg);
	
	Message_Add(msg);
	
	for (i = 0; i < NrVars; i++)
	{
		sprintf(msg, "%c", vars[i]);
		for (j = 0; j <= i; j++)
		{	
			sprintf(msg, "%s\t%f", msg, covar[i][j]/covar[i][i]);
		}
		sprintf(msg, "%s\n", msg);
		
		Message_Add(msg);
	}
	
	int Field_Count  = pTable->Get_Field_Count();
	
	for (i = 0; i < Record_Count; i++)
	{
		CTable_Record *	Record = pTable->Get_Record(i);
		
		Record->Set_Value(Field_Count - 1, Formel.Val(x[i]));
	}

//	API_FREE (uservars);
	return (true);
}
