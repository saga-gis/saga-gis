
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    pj_Georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Georef_Engine.cpp                   //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//    e-mail:     oconrad@gwdg.de                        //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

// cminpak usage is based on earlier codings of A.Ringeler...

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Georef_Engine.h"

#include "dpmpar.h"
#include "cminpak.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoref_Engine::CGeoref_Engine(void)
{
	m_nParms	= 6;
}

//---------------------------------------------------------
CGeoref_Engine::~CGeoref_Engine(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Engine::Get_Converted(TSG_Point &Point, bool bInverse)
{
	return( Get_Converted(Point.x, Point.y, bInverse) );
}

//---------------------------------------------------------
bool CGeoref_Engine::Get_Converted(double &x, double &y, bool bInverse)
{
	double	x_;

	if( !bInverse )
	{
		x_	= x * m_x    [0] + y * m_x    [1] + m_x    [2]; 
		y	= x * m_x    [3] + y * m_x    [4] + m_x    [5];
	}
	else
	{
		x_	= x * m_x_inv[0] + y * m_x_inv[1] + m_x_inv[2]; 
		y	= x * m_x_inv[3] + y * m_x_inv[4] + m_x_inv[5];
	}

	x	= x_;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Engine::Set_Engine(CSG_Shapes *pSource, CSG_Shapes *pTarget)
{
	int				iShape, iPart, iPoint;
	TSG_Point		Point;
	CSG_Points	Pts_Source, Pts_Target;
	CSG_Shape			*pShape;

	if( pSource && pTarget )
	{
		for(iShape=0; iShape<pSource->Get_Count(); iShape++)
		{
			pShape	= pSource->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Point	= pShape->Get_Point(iPoint, iPart);
					Pts_Source.Add(Point.x, Point.y);
				}
			}
		}

		for(iShape=0; iShape<pTarget->Get_Count(); iShape++)
		{
			pShape	= pTarget->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					Point	= pShape->Get_Point(iPoint, iPart);
					Pts_Target.Add(Point.x, Point.y);
				}
			}
		}

		return( _Set_Engine(&Pts_Source, &Pts_Target) );
	}

	return( false );
}

//---------------------------------------------------------
bool CGeoref_Engine::Set_Engine(CSG_Shapes *pSource, int xField, int yField)
{
	int				iShape;
	TSG_Point		Point;
	CSG_Points	Pts_Source, Pts_Target;
	CSG_Shape			*pShape;

	if( pSource && pSource->Get_Type() == SHAPE_TYPE_Point
	&&	xField >= 0 && xField < pSource->Get_Field_Count()
	&&	yField >= 0 && yField < pSource->Get_Field_Count()	)
	{
		for(iShape=0; iShape<pSource->Get_Count(); iShape++)
		{
			pShape	= pSource->Get_Shape(iShape);
			Point	= pShape->Get_Point(0);
			Pts_Source.Add(Point.x, Point.y);
			Pts_Target.Add(
				pShape->asDouble(xField),
				pShape->asDouble(yField)
			);
		}

		return( _Set_Engine(&Pts_Source, &Pts_Target) );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Points	*g_pPts_Source, *g_pPts_Target;

//---------------------------------------------------------
void fcn_linear(int m, int n, double x[], double fv[], int *iflag)
{
	for(int i=0; i<m/2; i++) 
	{
		fv[i * 2 + 0]	= g_pPts_Source->Get_X(i) * x[0] + g_pPts_Source->Get_Y(i) * x[1] + x[2] - g_pPts_Target->Get_X(i);
		fv[i * 2 + 1]	= g_pPts_Source->Get_X(i) * x[3] + g_pPts_Source->Get_Y(i) * x[4] + x[5] - g_pPts_Target->Get_Y(i);
    }
}

//---------------------------------------------------------
void fcn_linear_inverse(int m, int n, double x[], double fv[], int *iflag)
{
	for(int i=0; i<m/2; i++) 
	{
		fv[i * 2 + 0]	= g_pPts_Target->Get_X(i) * x[0] + g_pPts_Target->Get_Y(i) * x[1] + x[2] - g_pPts_Source->Get_X(i);
		fv[i * 2 + 1]	= g_pPts_Target->Get_X(i) * x[3] + g_pPts_Target->Get_Y(i) * x[4] + x[5] - g_pPts_Source->Get_Y(i);
	}
}

//---------------------------------------------------------
void fcn_linear_2(int m, int n, double x[], double fv[], int *iflag)
{
	for(int i=0; i<m/2; i++) 
	{
		fv[i * 2 + 0]	= (g_pPts_Source->Get_X(i) * x[0] + g_pPts_Source->Get_Y(i) * x[1] + x[2]) 
						/ (g_pPts_Source->Get_X(i) * x[3] + g_pPts_Source->Get_Y(i) * x[4]) - g_pPts_Target->Get_X(i);

		fv[i * 2 + 1]	= (g_pPts_Source->Get_X(i) * x[5] + g_pPts_Source->Get_Y(i) * x[6] + x[7])
						/ (g_pPts_Source->Get_X(i) * x[8] + g_pPts_Source->Get_Y(i) * x[9] + 1) - g_pPts_Target->Get_Y(i);
	}
}

//---------------------------------------------------------
void fcn_linear_2_inverse(int m, int n, double x[], double fv[], int *iflag)
{
	for(int i=0; i<m/2; i++) 
	{
		fv[i * 2 + 0]	= (g_pPts_Target->Get_X(i) * x[0] + g_pPts_Target->Get_Y(i) * x[1] + x[2]) 
						/ (g_pPts_Target->Get_X(i) * x[3] + g_pPts_Target->Get_Y(i) * x[4]) - g_pPts_Source->Get_X(i);

		fv[i * 2 + 1]	= (g_pPts_Target->Get_X(i) * x[5] + g_pPts_Target->Get_Y(i) * x[6] + x[7])
						/ (g_pPts_Target->Get_X(i) * x[8] + g_pPts_Target->Get_Y(i) * x[9] + 1) - g_pPts_Source->Get_Y(i);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Engine::_Set_Engine(CSG_Points *pPts_Source, CSG_Points *pPts_Target)
{
	int			*msk, info, ecode, nfev, nData, i;
	double		*fvec;

	//-----------------------------------------------------
	m_Message.Clear();

	g_pPts_Source	= pPts_Source;
	g_pPts_Target	= pPts_Target;

	if( g_pPts_Source == NULL || g_pPts_Target == NULL )
	{
		m_Message.Printf(_TL("Error: invalid source and target references.") );

		return( false );
	}

	if( g_pPts_Source->Get_Count() != g_pPts_Target->Get_Count() )
	{
		m_Message.Printf(_TL("Error: source and target references differ in number of points.") );

		return( false );
	}

	if( g_pPts_Source->Get_Count() < 3 )
	{
		m_Message.Printf(_TL("Error: not enough reference points. The transformation requires at least 3 reference points."));

		return( false );
	}

	//-----------------------------------------------------
	nData	= 2 * pPts_Source->Get_Count();
	fvec	= (double *)SG_Calloc(nData, sizeof(double));

	for(i=0; i<nData; i++)
	{
		fvec[i]	= 0.0;
	}

	msk		= (int    *)SG_Malloc(m_nParms * sizeof(int));

	for(i=0; i<m_nParms; i++)
	{
		msk[i]	= 1;
		m_x[i]	= m_x_inv[i]	= 0.0;
	}

	//-----------------------------------------------------
	ecode	= lmdif0(fcn_linear        , nData, m_nParms, m_x    , msk, fvec, sqrt(2.220446049250313e-16), &info, &nfev);

	m_Message.Append(CSG_String::Format(SG_T("\n%d %s\n"), nfev, _TL("function evaluations")));
	m_Message.Append(CSG_String::Format(SG_T("x\n")));
	m_Message.Append(CSG_String::Format(SG_T("%lf %lf %lf %lf %lf %lf\n"), m_x[0], m_x[1], m_x[2], m_x[3], m_x[4], m_x[5]));
	m_Message.Append(CSG_String::Format(SG_T("%s\n"), _TL("fvec")));
	m_Message.Append(CSG_String::Format(SG_T("%lg %lg %lg %lg %lg %lg\n"), fvec[0], fvec[1], fvec[2], fvec[3], fvec[4], fvec[5]));
	m_Message.Append(CSG_String::Format(SG_T("%s = %.15e\n"), _TL("function norm"), enorm(nData, fvec)));

	//-----------------------------------------------------
	ecode	= lmdif0(fcn_linear_inverse, nData, m_nParms, m_x_inv, msk, fvec, sqrt(2.220446049250313e-16), &info, &nfev);

	m_Message.Append(CSG_String::Format(SG_T("\n%d inverse function evaluations\n"), nfev));
	m_Message.Append(CSG_String::Format(SG_T("x\n")));
	m_Message.Append(CSG_String::Format(SG_T("%lf %lf %lf %lf %lf %lf\n"), m_x_inv[0], m_x_inv[1], m_x_inv[2], m_x_inv[3], m_x_inv[4], m_x_inv[5]));
	m_Message.Append(CSG_String::Format(SG_T("%s\n"), _TL("fvec")));
	m_Message.Append(CSG_String::Format(SG_T("%lg %lg %lg %lg %lg %lg\n"), fvec[0], fvec[1], fvec[2], fvec[3], fvec[4], fvec[5]));
	m_Message.Append(CSG_String::Format(SG_T("%s = %.15e\n"), _TL("function norm"), enorm(nData, fvec)));

	//-----------------------------------------------------
	SG_Free(fvec);
	SG_Free(msk);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
