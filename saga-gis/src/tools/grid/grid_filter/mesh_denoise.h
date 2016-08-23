/**********************************************************
 * Version $Id: mesh_denoise.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     mesh_denoise.h                    //
//                                                       //
//                 Copyright (C) 2011 by                 //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#ifndef HEADER_INCLUDED__mesh_denoise_H
#define HEADER_INCLUDED__mesh_denoise_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CMesh_Denoise_Grid : public CSG_Tool_Grid
{
public:
	CMesh_Denoise_Grid(void);


protected:

	virtual bool				On_Execute		(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// mdenoise.h: header for feature-preserving mesh denoising.
// Copyright (C) 2007 Cardiff University, UK
//
// Version: 1.0
//
// Author: Xianfang Sun
//


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// mathematical constants

#define FLT_MAX         3.402823466e+38F
#define FLT_EPSILON     1.192092896e-07F

//---------------------------------------------------------
// type definitions

typedef double			FVECTOR3[3];
typedef int				NVECTOR3[3];
typedef int				NVECTOR2[2];

//---------------------------------------------------------
// macro definitions

#define FMAX(x,y)		((x)>(y) ? (x) : (y))

#define VEC3_ZERO(vec)	{ (vec)[0]=(vec)[1]=(vec)[2]=0; }

#define VEC3_EQ(a,b)	(((a)[0]==(b)[0]) && ((a)[1]==(b)[1]) && ((a)[2]==(b)[2]))

#define VEC3_V_OP_S(a,b,op,c)	{\
				(a)[0] = (b)[0] op (c); \
				(a)[1] = (b)[1] op (c); \
				(a)[2] = (b)[2] op (c); }

#define VEC3_V_OP_V(a,b,op,c)	{\
				(a)[0] = (b)[0] op (c)[0]; \
				(a)[1] = (b)[1] op (c)[1]; \
				(a)[2] = (b)[2] op (c)[2]; }

#define VEC3_V_OP_V_OP_S(a,b,op1,c,op2,d)	{\
				(a)[0] = (b)[0] op1 (c)[0] op2 (d); \
				(a)[1] = (b)[1] op1 (c)[1] op2 (d); \
				(a)[2] = (b)[2] op1 (c)[2] op2 (d); }

#define VEC3_VOPV_OP_S(a,b,op1,c,op2,d)		{\
				(a)[0] = ((b)[0] op1 (c)[0]) op2 (d); \
				(a)[1] = ((b)[1] op1 (c)[1]) op2 (d); \
				(a)[2] = ((b)[2] op1 (c)[2]) op2 (d); }

#define VEC3_V_OP_V_OP_V(a,b,op1,c,op2,d)	{\
				(a)[0] = (b)[0] op1 (c)[0] op2 (d)[0]; \
				(a)[1] = (b)[1] op1 (c)[1] op2 (d)[1]; \
				(a)[2] = (b)[2] op1 (c)[2] op2 (d)[2]; }

#define VEC3_ASN_OP(a,op,b)		{	a[0] op b[0]; a[1] op b[1]; a[2] op b[2];	}

#define DOTPROD3(a, b)			((a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2])

#define CROSSPROD3(a,b,c)		{\
				(a)[0] = (b)[1]*(c)[2]-(b)[2]*(c)[1]; \
				(a)[1] = (b)[2]*(c)[0]-(b)[0]*(c)[2]; \
				(a)[2] = (b)[0]*(c)[1]-(b)[1]*(c)[0]; }


///////////////////////////////////////////////////////////
//                                                       //
//                   Original Mesh                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CMesh_Denoise
{
public:
	CMesh_Denoise(void);
	~CMesh_Denoise(void);

	void				Destroy			(void);

	bool				Denoise			(CSG_Grid *pInput, CSG_Grid *pOutput);

	void				Set_Sigma		(double d)	{	if( d >= 0.0 && d <= 1.0 )	m_fSigma	= d;	}
	void				Set_Iterations	(int    i)	{	if( i >= 1 )	m_nIterations	= i;	}
	void				Set_VIterations	(int    i)	{	if( i >= 1 )	m_nVIterations	= i;	}
	void				Set_NB_CV		(bool   b)	{	m_bNeighbourCV	= b;	}
	void				Set_ZOnly		(bool   b)	{	m_bZOnly		= b;	}


private:

	bool				m_bNeighbourCV, m_bZOnly;

	int					m_nIterations, m_nVIterations, m_nNumVertex, m_nNumFace, m_nNumVertexP, m_nNumFaceP;

	double				m_fSigma, m_fScale, m_f3Centre[3];

	int					**m_ppnVRing1V;		// 1-ring neighbouring vertices of each vertex  
	int					**m_ppnVRing1T;		// 1-ring neighbouring triangles of each vertex 
	int					**m_ppnTRing1TCV;	// 1-ring neighbouring triangles with common vertex of each triangle 
	int					**m_ppnTRing1TCE;	// 1-ring neighbouring triangles with common edge of each triangle 

	NVECTOR3			*m_pn3Face, *m_pn3FaceP;

	FVECTOR3			*m_pf3Vertex , *m_pf3FaceNormal , *m_pf3VertexNormal;
	FVECTOR3			*m_pf3VertexP, *m_pf3FaceNormalP, *m_pf3VertexNormalP;


	int					Set_Data		(CSG_Grid *pGrid, int *Index);
	void				Get_Data		(CSG_Grid *pGrid, int *Index);

	// Preprocessing Operations
	void				ScalingBox			(void);
	void				V3Normalize			(FVECTOR3 v);
	void				ComputeNormal		(bool bProduced);
	void				ComputeVRing1V		(void);
	void				ComputeVRing1T		(void);
	void				ComputeTRing1TCV	(void);
	void				ComputeTRing1TCE	(void);

	// Main Operations
	void				MeshDenoise			(bool bNeighbourCV, double fSigma, int nIterations, int nVIterations);
	void				VertexUpdate		(int** tRing, int nVIterations);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__mesh_denoise_H
