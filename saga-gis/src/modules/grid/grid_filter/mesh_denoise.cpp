/**********************************************************
 * Version $Id: template.cpp 911 2011-11-11 11:11:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  m_denoise_grid.cpp                   //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "mesh_denoise.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMesh_Denoise_Grid::CMesh_Denoise_Grid(void)
{
	Set_Name		(_TL("Mesh Denoise"));

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		"Mesh denoising for grids, using the algorithm of Sun et al. (2007).\n"

		"References:\n"

		"Cardiff University: Filtering and Processing of Irregular Meshes with Uncertainties. "
		"<a target=\"_blank\" href=\"http://www.cs.cf.ac.uk/meshfiltering/\">online</a>.\n"

		"Stevenson, J.A., Sun, X., Mitchell, N.C. (2010): "
		"Despeckling SRTM and other topographic data with a denoising algorithm, "
		"Geomorphology, Vol.114, No.3, pp.238-252.\n"

		"Sun, X., Rosin, P.L., Martin, R.R., Langbein, F.C. (2007): "
		"Fast and effective feature-preserving mesh denoising. "
		"IEEE Transactions on Visualization and Computer Graphics, Vol.13, No.5, pp.925-938.\n"
	));

	Parameters.Add_Grid(
		NULL	, "INPUT"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"	, _TL("Denoised Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "SIGMA"	, _TL("Threshold"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.9, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "ITER"	, _TL("Number of Iterations for Normal Updating"),
		_TL(""),
		PARAMETER_TYPE_Int, 5, 1, true
	);

	Parameters.Add_Value(
		NULL	, "VITER"	, _TL("Number of Iterations for Vertex Updating"),
		_TL(""),
		PARAMETER_TYPE_Int, 50, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "NB_CV"	, _TL("Common Edge Type of Face Neighbourhood"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Common Vertex"),
			_TL("Common Edge")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "ZONLY"	, _TL("Only Z-Direction Position is Updated"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMesh_Denoise_Grid::On_Execute(void)
{
	CMesh_Denoise	Denoise;

	Denoise.Set_Sigma		(Parameters("SIGMA")->asDouble());
	Denoise.Set_Iterations	(Parameters("ITER" )->asInt   ());
	Denoise.Set_VIterations	(Parameters("VITER")->asInt   ());
	Denoise.Set_NB_CV		(Parameters("NB_CV")->asBool  ());
	Denoise.Set_ZOnly		(Parameters("ZONLY")->asBool  ());

	return( Denoise.Denoise(Parameters("INPUT")->asGrid(), Parameters("OUTPUT")->asGrid()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// MDenoise.cpp: Feature-Preserving Mesh Denoising.
// Copyright (C) 2007 Cardiff University, UK
//
// Version: 1.0
//
// Author: Xianfang Sun
// 
// Reference:
// @article{SRML071,
//   author = "Xianfang Sun and Paul L. Rosin and Ralph R. Martin and Frank C. Langbein",
//   title = "Fast and effective feature-preserving mesh denoising",
//   journal = "IEEE Transactions on Visualization and Computer Graphics",
//   volume = "13",
//   number = "5",
//   pages  = "925--938",
//   year = "2007",
// }


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMesh_Denoise::CMesh_Denoise(void)
{
    m_bNeighbourCV		= true;
	m_bZOnly			= false;	// only z-direction position is updated
    m_fSigma			= 0.4;
    m_nIterations		= 20;
    m_nVIterations		= 50;

	m_ppnVRing1V		= NULL;
	m_ppnVRing1T		= NULL;
	m_ppnTRing1TCV		= NULL;
	m_ppnTRing1TCE		= NULL;

	m_pf3Vertex			= NULL;
	m_pn3Face			= NULL;
	m_pf3FaceNormal		= NULL;
	m_pf3VertexNormal	= NULL;

	m_pf3VertexP		= NULL;
	m_pn3FaceP			= NULL;
	m_pf3FaceNormalP	= NULL;
	m_pf3VertexNormalP	= NULL;
}

//---------------------------------------------------------
CMesh_Denoise::~CMesh_Denoise(void)
{
	Destroy();
}

//---------------------------------------------------------
#define FREE_ARRAY(A, N)	if( A ) { for(int i=0; i<N; i++) SG_FREE_SAFE(A[i]) SG_FREE_SAFE(A) }

//---------------------------------------------------------
void CMesh_Denoise::Destroy(void)
{
	FREE_ARRAY(m_ppnVRing1V  , m_nNumVertex);
	FREE_ARRAY(m_ppnVRing1T  , m_nNumVertex);
	FREE_ARRAY(m_ppnTRing1TCV, m_nNumFace);
	FREE_ARRAY(m_ppnTRing1TCE, m_nNumFace);

	SG_FREE_SAFE(m_pf3Vertex);
	SG_FREE_SAFE(m_pn3Face);
	SG_FREE_SAFE(m_pf3FaceNormal);
	SG_FREE_SAFE(m_pf3VertexNormal);

	SG_FREE_SAFE(m_pf3VertexP);
	SG_FREE_SAFE(m_pn3FaceP);
	SG_FREE_SAFE(m_pf3FaceNormalP);
	SG_FREE_SAFE(m_pf3VertexNormalP);
}

//---------------------------------------------------------
bool CMesh_Denoise::Denoise(CSG_Grid *pInput, CSG_Grid *pOutput)
{
	int	*index	= (int *)SG_Malloc(pInput->Get_NCells() * sizeof(int));

	m_nNumFace	= Set_Data(pInput, index);

	//-----------------------------------------------------
    //Denoising Model...
    MeshDenoise(m_bNeighbourCV, m_fSigma, m_nIterations, m_nVIterations);

	//-----------------------------------------------------
    //Saving Model...
	pOutput->Create(pInput);
	pOutput->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pInput->Get_Name(), _TL("Denoised")));

	Get_Data(pOutput, index);

	SG_Free(index);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CMesh_Denoise::Set_Data(CSG_Grid *pGrid, int *index)
{
	int		i, x, y;

	//-----------------------------------------------------
	m_nNumFace		= 0;
	m_nNumVertex	= 0;

	m_pf3Vertex		= (FVECTOR3 *)SG_Malloc(pGrid->Get_NCells() * sizeof(FVECTOR3));
	m_pn3Face		= (NVECTOR3 *)SG_Malloc(2 * (pGrid->Get_NX() - 1) * (pGrid->Get_NY() - 1) * sizeof(NVECTOR3));

	//-----------------------------------------------------
	for(y=0; y<pGrid->Get_NY(); y++)
	{
		for(x=0; x<pGrid->Get_NX(); x++)
		{
			int	k	= x + y * pGrid->Get_NX();

			if( pGrid->is_NoData(x, y) )
			{
				index[k]	= -1;
			}
			else
			{
				m_pf3Vertex[m_nNumVertex][0]	= y * pGrid->Get_Cellsize();
				m_pf3Vertex[m_nNumVertex][1]	= x * pGrid->Get_Cellsize();
				m_pf3Vertex[m_nNumVertex][2]	= pGrid->asDouble(x, y);

				index[k]	= m_nNumVertex++;
			}
		}
	}

	//-----------------------------------------------------
	m_pf3Vertex		= (FVECTOR3 *)SG_Realloc(m_pf3Vertex, m_nNumVertex * sizeof(FVECTOR3));

	for(y=0; y<pGrid->Get_NY()-1; y++)
	{
		for(x=0; x<pGrid->Get_NX()-1; x++)
		{
			int	k	= 4, kk[4];

			kk[0]	= x + y * pGrid->Get_NX();
			kk[1]	= kk[0] + 1;
			kk[2]	= kk[0] + pGrid->Get_NX();
			kk[3]	= kk[2] + 1;

			for(i=0; i<4; i++)
			{
				if( index[kk[i]] < 0 )
				{
					if( k < i )
					{
						k	= 5;

						break;
					}
					else
					{
						k	= i;
					}
				}
			}

			switch( k )
			{
			case 0:
				m_pn3Face[m_nNumFace][0] = index[kk[1]];
				m_pn3Face[m_nNumFace][1] = index[kk[3]];
				m_pn3Face[m_nNumFace][2] = index[kk[2]];
				m_nNumFace++;
				break;

			case 1:
				m_pn3Face[m_nNumFace][0] = index[kk[0]];
				m_pn3Face[m_nNumFace][1] = index[kk[3]];
				m_pn3Face[m_nNumFace][2] = index[kk[2]];
				m_nNumFace++;
				break;

			case 2:
				m_pn3Face[m_nNumFace][0] = index[kk[1]];
				m_pn3Face[m_nNumFace][1] = index[kk[3]];
				m_pn3Face[m_nNumFace][2] = index[kk[0]];
				m_nNumFace++;
				break;

			case 3:
				m_pn3Face[m_nNumFace][0] = index[kk[0]];
				m_pn3Face[m_nNumFace][1] = index[kk[1]];
				m_pn3Face[m_nNumFace][2] = index[kk[2]];
				m_nNumFace++;
				break;

			case 4:	//generate two triangles with minimum total area 
				if(	(	fabs(pGrid->asDouble(index[kk[2]]) - pGrid->asDouble(index[kk[0]])) >
						fabs(pGrid->asDouble(index[kk[3]]) - pGrid->asDouble(index[kk[1]])) )
				&&	(	fabs(pGrid->asDouble(index[kk[1]]) - pGrid->asDouble(index[kk[0]])) >
						fabs(pGrid->asDouble(index[kk[3]]) - pGrid->asDouble(index[kk[2]])) ) )
				{
					m_pn3Face[m_nNumFace][0] = index[kk[0]];
					m_pn3Face[m_nNumFace][1] = index[kk[1]];
					m_pn3Face[m_nNumFace][2] = index[kk[2]];
					m_nNumFace++;

					m_pn3Face[m_nNumFace][0] = index[kk[1]];
					m_pn3Face[m_nNumFace][1] = index[kk[3]];
					m_pn3Face[m_nNumFace][2] = index[kk[2]];
					m_nNumFace++;
				}
				else
				{
					m_pn3Face[m_nNumFace][0] = index[kk[1]];
					m_pn3Face[m_nNumFace][1] = index[kk[3]];
					m_pn3Face[m_nNumFace][2] = index[kk[0]];
					m_nNumFace++;

					m_pn3Face[m_nNumFace][0] = index[kk[0]];
					m_pn3Face[m_nNumFace][1] = index[kk[3]];
					m_pn3Face[m_nNumFace][2] = index[kk[2]];
					m_nNumFace++;
				}
			}
		}
	}

	m_pn3Face = (NVECTOR3 *)SG_Realloc(m_pn3Face, m_nNumFace*sizeof(NVECTOR3));

	//-----------------------------------------------------
	ScalingBox(); // scale to a box
	ComputeNormal(false);

	m_nNumVertexP		= m_nNumVertex;
	m_nNumFaceP			= m_nNumFace;
	m_pf3VertexP		= (FVECTOR3 *)SG_Malloc(m_nNumVertexP * sizeof(FVECTOR3));
	m_pn3FaceP			= (NVECTOR3 *)SG_Malloc(m_nNumFaceP   * sizeof(NVECTOR3));
	m_pf3VertexNormalP	= (FVECTOR3 *)SG_Malloc(m_nNumVertexP * sizeof(FVECTOR3));
	m_pf3FaceNormalP	= (FVECTOR3 *)SG_Malloc(m_nNumFaceP   * sizeof(FVECTOR3));

	for(int i=0; i<m_nNumVertex; i++)
	{
		VEC3_ASN_OP(m_pf3VertexP[i],=,m_pf3Vertex[i]);
		VEC3_ASN_OP(m_pf3VertexNormalP[i],=,m_pf3VertexNormal[i]);
	}

	for (int i=0;i<m_nNumFace;i++)
	{
		VEC3_ASN_OP(m_pn3FaceP[i],=,m_pn3Face[i]);
		VEC3_ASN_OP(m_pf3FaceNormalP[i],=,m_pf3FaceNormal[i]);
	}

	return m_nNumFace;
}

//---------------------------------------------------------
void CMesh_Denoise::Get_Data(CSG_Grid *pGrid, int *index)
{
	for(int i=0; i<m_nNumVertexP; i++)
    {
        VEC3_V_OP_V_OP_S(m_pf3VertexP[i], m_f3Centre, +, m_pf3VertexP[i], *, m_fScale);
    }

	for(int y=0; y<pGrid->Get_NY(); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)		
		{
			int	k	= index[x + y * pGrid->Get_NX()];

			if( k < 0 )
			{
				pGrid->Set_NoData(x, y);
			}
			else
			{
				pGrid->Set_Value(x, y, m_pf3VertexP[k][2]);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CMesh_Denoise::ScalingBox(void)
{
    int i,j;
    double box[2][3];

    box[0][0] = box[0][1] = box[0][2] = FLT_MAX;
    box[1][0] = box[1][1] = box[1][2] = -FLT_MAX;
    for (i=0;i<m_nNumVertex;i++)
    {
        for (j=0;j<3;j++)
        {
            if (box[0][j]>m_pf3Vertex[i][j])
                box[0][j] = m_pf3Vertex[i][j];
            if (box[1][j]<m_pf3Vertex[i][j])
                box[1][j] = m_pf3Vertex[i][j];
        }
    }
    m_f3Centre[0] = (box[0][0]+box[1][0])/2.0;
    m_f3Centre[1] = (box[0][1]+box[1][1])/2.0;
    m_f3Centre[2] = (box[0][2]+box[1][2])/2.0;

    m_fScale = FMAX(box[1][0]-box[0][0],FMAX(box[1][1]-box[0][1],box[1][2]-box[0][2]));
    m_fScale /=2.0;
    for (i=0;i<m_nNumVertex;i++)
    {
        VEC3_VOPV_OP_S(m_pf3Vertex[i],m_pf3Vertex[i],-,m_f3Centre,/,m_fScale);
    }
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CMesh_Denoise::ComputeNormal(bool bProduced)
{
    int i, j;
    FVECTOR3 vect[3];
    double fArea;
   
    if(bProduced)
    {
        SG_FREE_SAFE(m_pf3VertexNormalP);
        SG_FREE_SAFE(m_pf3FaceNormalP);

        m_pf3VertexNormalP	= (FVECTOR3 *)SG_Malloc(m_nNumVertexP * sizeof(FVECTOR3));
        m_pf3FaceNormalP	= (FVECTOR3 *)SG_Malloc(m_nNumFaceP   * sizeof(FVECTOR3));
       
        for (i=0;i<m_nNumVertexP;i++)
        {
            VEC3_ZERO(m_pf3VertexNormalP[i]);
        }
        for (i=0;i<m_nNumFaceP;i++) // compute each triangle normal and vertex normal
        {
            VEC3_V_OP_V(vect[0],m_pf3VertexP[m_pn3FaceP[i][1]],-,m_pf3VertexP[m_pn3FaceP[i][0]]);
            VEC3_V_OP_V(vect[1],m_pf3VertexP[m_pn3FaceP[i][2]],-,m_pf3VertexP[m_pn3FaceP[i][0]]);
            CROSSPROD3(vect[2],vect[0],vect[1]);
            fArea = sqrt(DOTPROD3(vect[2], vect[2]))/2.0f; // Area of the face
            V3Normalize(vect[2]);
            VEC3_ASN_OP(m_pf3FaceNormalP[i],=,vect[2]);
            for (j=0;j<3;j++)
            {
                VEC3_V_OP_V_OP_S(m_pf3VertexNormalP[m_pn3FaceP[i][j]], \
                    m_pf3VertexNormalP[m_pn3FaceP[i][j]], +, vect[2], *, fArea);
            }
        }
        for (i=0;i<m_nNumVertexP;i++)
            V3Normalize(m_pf3VertexNormalP[i]);
    }
    else
    {   
        SG_FREE_SAFE(m_pf3VertexNormal);
        SG_FREE_SAFE(m_pf3FaceNormal);

        m_pf3VertexNormal	= (FVECTOR3 *)SG_Malloc(m_nNumVertex * sizeof(FVECTOR3));
        m_pf3FaceNormal		= (FVECTOR3 *)SG_Malloc(m_nNumFace   * sizeof(FVECTOR3));
       
        for (i=0;i<m_nNumVertex;i++)
        {
            VEC3_ZERO(m_pf3VertexNormal[i]);
        }
        for (i=0;i<m_nNumFace;i++) // compute each triangle normal and vertex normal
        {
            VEC3_V_OP_V(vect[0],m_pf3Vertex[m_pn3Face[i][1]],-,m_pf3Vertex[m_pn3Face[i][0]]);
            VEC3_V_OP_V(vect[1],m_pf3Vertex[m_pn3Face[i][2]],-,m_pf3Vertex[m_pn3Face[i][0]]);
            CROSSPROD3(vect[2],vect[0],vect[1]);
            fArea = sqrt(DOTPROD3(vect[2], vect[2]))/2.0f; // Area of the face
            V3Normalize(vect[2]);
            VEC3_ASN_OP(m_pf3FaceNormal[i],=,vect[2]);
            for (j=0;j<3;j++)
            {
                VEC3_V_OP_V_OP_S(m_pf3VertexNormal[m_pn3Face[i][j]], \
                    m_pf3VertexNormal[m_pn3Face[i][j]], +, vect[2], *, fArea);
            }
        }
        for (i=0;i<m_nNumVertex;i++)
            V3Normalize(m_pf3VertexNormal[i]);
    }
}

//---------------------------------------------------------
void CMesh_Denoise::V3Normalize(FVECTOR3 v)
{
    double len;
    len=sqrt(DOTPROD3(v,v));
    if (len!=0.0)
    {
        v[0]=v[0]/len;
        v[1]=v[1]/len;
        v[2]=v[2]/len;
    }
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CMesh_Denoise::ComputeVRing1V(void)
{
    int i,j,k;
    int tmp0, tmp1, tmp2;

    if(m_ppnVRing1V != NULL)
        return;

    m_ppnVRing1V=(int **)SG_Malloc(m_nNumVertex*sizeof(int *));

    for (i=0;i<m_nNumVertex;i++) {
        m_ppnVRing1V[i]=(int *)SG_Malloc(6*sizeof(int));
        m_ppnVRing1V[i][0]=0; // m_ppnVRing1V[i][0] stores the number of vertex neighbours
    }

    for (k=0; k<m_nNumFace; k++)
    {
        for (i=0;i<3;i++)
        {
            tmp0=m_pn3Face[k][i];
            tmp2=m_pn3Face[k][(i+2)%3];
            for (j=1;j<m_ppnVRing1V[tmp0][0]+1;j++)
                if (m_ppnVRing1V[tmp0][j] == tmp2)
                    break;
            if (j==m_ppnVRing1V[tmp0][0]+1)
            {
                m_ppnVRing1V[tmp0][j]=tmp2;
                m_ppnVRing1V[tmp0][0] += 1;
                if (!(m_ppnVRing1V[tmp0][0]%5))
                    m_ppnVRing1V[tmp0] = (int *)SG_Realloc(m_ppnVRing1V[tmp0],(m_ppnVRing1V[tmp0][0]+6)*sizeof(int));
            }
            tmp1=m_pn3Face[k][(i+1)%3];
            for (j=1;j<m_ppnVRing1V[tmp0][0]+1;j++)
                if (m_ppnVRing1V[tmp0][j] == tmp1)
                    break;
            if (j==m_ppnVRing1V[tmp0][0]+1)
            {
                m_ppnVRing1V[tmp0][j]=tmp1;
                m_ppnVRing1V[tmp0][0] += 1;
                if (!(m_ppnVRing1V[tmp0][0]%5))
                    m_ppnVRing1V[tmp0] = (int *)SG_Realloc(m_ppnVRing1V[tmp0],(m_ppnVRing1V[tmp0][0]+6)*sizeof(int));
            }   
        }
    }
    for (i=0;i<m_nNumVertex;i++) {
        m_ppnVRing1V[i]=(int *)SG_Realloc(m_ppnVRing1V[i],(m_ppnVRing1V[i][0]+1)*sizeof(int));
    }
}

//---------------------------------------------------------
void CMesh_Denoise::ComputeVRing1T(void)
{
        int i,k;
    int tmp;

    if(m_ppnVRing1T != NULL)
        return;

    m_ppnVRing1T=(int **)SG_Malloc(m_nNumVertex*sizeof(int *));
    for (i=0;i<m_nNumVertex;i++) {
        m_ppnVRing1T[i]=(int *)SG_Malloc(6*sizeof(int));
        m_ppnVRing1T[i][0]=0; // m_ppnVRing1T[i][0] stores the number of triangle neighbours
    }

    for (k=0; k<m_nNumFace; k++)
    {
        for (i=0;i<3;i++)
        {
            tmp = m_pn3Face[k][i]; //the vertex incident to the k-th triangle
            m_ppnVRing1T[tmp][0] += 1;
            m_ppnVRing1T[tmp][m_ppnVRing1T[tmp][0]]=k;
            if (!(m_ppnVRing1T[tmp][0]%5))
            {
                m_ppnVRing1T[tmp] = (int *)SG_Realloc(m_ppnVRing1T[tmp],(m_ppnVRing1T[tmp][0]+6)*sizeof(int));
            }
        }
    }
    for (i=0;i<m_nNumVertex;i++) {
        m_ppnVRing1T[i]=(int *)SG_Realloc(m_ppnVRing1T[i],(m_ppnVRing1T[i][0]+1)*sizeof(int));
    }
}

//---------------------------------------------------------
void CMesh_Denoise::ComputeTRing1TCV(void)
{
    int i,j,k;
    int tmp,tmp0,tmp1,tmp2;

    if(m_ppnTRing1TCV != NULL)
        return;

    m_ppnTRing1TCV=(int **)SG_Malloc(m_nNumFace*sizeof(int *));
    for (k=0; k<m_nNumFace; k++)
    {
        tmp0 = m_pn3Face[k][0]; 
        tmp1 = m_pn3Face[k][1];
        tmp2 = m_pn3Face[k][2];
        tmp = m_ppnVRing1T[tmp0][0] + m_ppnVRing1T[tmp1][0] +  m_ppnVRing1T[tmp2][0];
        m_ppnTRing1TCV[k] =(int *)SG_Malloc(tmp*sizeof(int));
       
        m_ppnTRing1TCV[k][0] = m_ppnVRing1T[tmp0][0];
        for (i=1; i<m_ppnVRing1T[tmp0][0]+1; i++)
        {
            m_ppnTRing1TCV[k][i] = m_ppnVRing1T[tmp0][i];
        }

        for (i=1; i<m_ppnVRing1T[tmp1][0]+1; i++)
        {
            if((m_pn3Face[m_ppnVRing1T[tmp1][i]][0] != tmp0) && (m_pn3Face[m_ppnVRing1T[tmp1][i]][1] != tmp0)\
                && (m_pn3Face[m_ppnVRing1T[tmp1][i]][2] != tmp0))
            {
                ++m_ppnTRing1TCV[k][0];
                m_ppnTRing1TCV[k][m_ppnTRing1TCV[k][0]]= m_ppnVRing1T[tmp1][i];
            }
            else
            {
                for(j=1; j<m_ppnTRing1TCV[k][0]+1; j++)
                {
                    if(m_ppnTRing1TCV[k][j]==m_ppnVRing1T[tmp1][i])
                    {
                        break;
                    }
                }
            }
        }

        for (i=1; i<m_ppnVRing1T[tmp2][0]+1; i++)
        {
            if((m_pn3Face[m_ppnVRing1T[tmp2][i]][0] != tmp0) && (m_pn3Face[m_ppnVRing1T[tmp2][i]][1] != tmp0)\
                && (m_pn3Face[m_ppnVRing1T[tmp2][i]][2] != tmp0) && (m_pn3Face[m_ppnVRing1T[tmp2][i]][0] != tmp1)\
                && (m_pn3Face[m_ppnVRing1T[tmp2][i]][1] != tmp1) && (m_pn3Face[m_ppnVRing1T[tmp2][i]][2] != tmp1))
            {
                ++m_ppnTRing1TCV[k][0];
                m_ppnTRing1TCV[k][m_ppnTRing1TCV[k][0]]= m_ppnVRing1T[tmp2][i];
            }
            else
            {
                for(j=1; j<m_ppnTRing1TCV[k][0]+1; j++)
                {
                    if(m_ppnTRing1TCV[k][j]==m_ppnVRing1T[tmp2][i])
                    {
                        break;
                    }
                }
            }
        }
    }
    for (i=0;i<m_nNumFace;i++) {
        m_ppnTRing1TCV[i]=(int *)SG_Realloc(m_ppnTRing1TCV[i],(m_ppnTRing1TCV[i][0]+1)*sizeof(int));
    }
}

//---------------------------------------------------------
void CMesh_Denoise::ComputeTRing1TCE(void)
{
    int i,k;
    int tmp,tmp0,tmp1,tmp2;

    if(m_ppnTRing1TCE != NULL)
        return;

    m_ppnTRing1TCE=(int **)SG_Malloc(m_nNumFace*sizeof(int *));
    for (k=0; k<m_nNumFace; k++)
    {
        tmp0 = m_pn3Face[k][0]; 
        tmp1 = m_pn3Face[k][1];
        tmp2 = m_pn3Face[k][2];

        m_ppnTRing1TCE[k] = (int *)SG_Malloc(5*sizeof(int));

        tmp = 0;
        for (i=1; i<m_ppnVRing1T[tmp0][0]+1; i++)
        {
            if ((m_pn3Face[m_ppnVRing1T[tmp0][i]][0] == tmp1)||(m_pn3Face[m_ppnVRing1T[tmp0][i]][0] == tmp2)||\
                    (m_pn3Face[m_ppnVRing1T[tmp0][i]][1] == tmp1)||(m_pn3Face[m_ppnVRing1T[tmp0][i]][1] == tmp2)||\
                    (m_pn3Face[m_ppnVRing1T[tmp0][i]][2] == tmp1)||(m_pn3Face[m_ppnVRing1T[tmp0][i]][2] == tmp2))
            {
                tmp++;
                if (tmp>4)
                {
                    tmp--;
                    break;
                }
                m_ppnTRing1TCE[k][tmp] = m_ppnVRing1T[tmp0][i];
            }
        }

        for (i=1; i<m_ppnVRing1T[tmp1][0]+1; i++)
        {
            if ((m_pn3Face[m_ppnVRing1T[tmp1][i]][0] == tmp1)&&\
                ((m_pn3Face[m_ppnVRing1T[tmp1][i]][1] == tmp2)||(m_pn3Face[m_ppnVRing1T[tmp1][i]][2] == tmp2)))
            {
                tmp++;
                if (tmp>4)
                {
                    tmp--;
                    break;
                }
                m_ppnTRing1TCE[k][tmp] = m_ppnVRing1T[tmp1][i];
                break;   
            }
            else if((m_pn3Face[m_ppnVRing1T[tmp1][i]][0] == tmp2)&&\
                ((m_pn3Face[m_ppnVRing1T[tmp1][i]][1] == tmp1)||(m_pn3Face[m_ppnVRing1T[tmp1][i]][2] == tmp1)))
            {
                tmp++;
                if (tmp>4)
                {
                    tmp--;
                    break;
                }
                m_ppnTRing1TCE[k][tmp] = m_ppnVRing1T[tmp1][i];
                break;   
            }
            else if((m_pn3Face[m_ppnVRing1T[tmp1][i]][1] == tmp2)&&(m_pn3Face[m_ppnVRing1T[tmp1][i]][2] == tmp1))
            {
                tmp++;
                if (tmp>4)
                {
                    tmp--;
                    break;
                }
                m_ppnTRing1TCE[k][tmp] = m_ppnVRing1T[tmp1][i];
                break;   
            }
            else if((m_pn3Face[m_ppnVRing1T[tmp1][i]][1] == tmp1)&&\
                (m_pn3Face[m_ppnVRing1T[tmp1][i]][2] == tmp2)&&(m_pn3Face[m_ppnVRing1T[tmp1][i]][0] != tmp0))
            {
                tmp++;
                if (tmp>4)
                {
                    tmp--;
                    break;
                }
                m_ppnTRing1TCE[k][tmp] = m_ppnVRing1T[tmp1][i];
                break;   
            }
        }
        m_ppnTRing1TCE[k][0] = tmp;
    }
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CMesh_Denoise::MeshDenoise(bool bNeighbourCV, double fSigma, int nIterations, int nVIterations)
{
    int **ttRing; //store the list of triangle neighbours of a triangle

    FVECTOR3 *Vertex;
    FVECTOR3 *TNormal;

    int i,k,m;
    double tmp3;

    if (m_nNumFace == 0)
        return;

    SG_FREE_SAFE(m_pf3VertexP);
    SG_FREE_SAFE(m_pf3VertexNormalP);
    SG_FREE_SAFE(m_pf3FaceNormalP);
    ComputeVRing1V(); //find the neighbouring vertices of each vertex
    ComputeVRing1T();     //find the neighbouring triangles of each vertex

    //find out the neighbouring triangles of each triangle
    if (bNeighbourCV)
    {
        ComputeTRing1TCV();
        ttRing = m_ppnTRing1TCV;
        for (k=0; k<m_nNumFace; k++)
        {
            ttRing[k] = m_ppnTRing1TCV[k];       
        }
    }
    else
    {
        ComputeTRing1TCE();
        ttRing = m_ppnTRing1TCE;
        for (k=0; k<m_nNumFace; k++)
        {
            ttRing[k] = m_ppnTRing1TCE[k];       
        }
    }

    //begin filter
    m_nNumVertexP		= m_nNumVertex;
    m_nNumFaceP			= m_nNumFace;
    m_pf3VertexP		= (FVECTOR3 *)SG_Malloc(m_nNumVertexP * sizeof(FVECTOR3));
    m_pf3FaceNormalP	= (FVECTOR3 *)SG_Malloc(m_nNumFaceP   * sizeof(FVECTOR3));
    m_pf3VertexNormalP	= (FVECTOR3 *)SG_Malloc(m_nNumVertexP * sizeof(FVECTOR3));
    Vertex				= (FVECTOR3 *)SG_Malloc(m_nNumVertexP * sizeof(FVECTOR3));
    TNormal				= (FVECTOR3 *)SG_Malloc(m_nNumFace    * sizeof(FVECTOR3));

    for(i=0; i<m_nNumFace; i++)
    {
        VEC3_ASN_OP(m_pf3FaceNormalP[i], =, m_pf3FaceNormal[i]);
    }
    for(i=0; i<m_nNumVertex; i++)
    {
        VEC3_ASN_OP(m_pf3VertexP[i], =, m_pf3Vertex[i]);
    }

    for(i=0; i<m_nNumVertex; i++)
    {
        VEC3_ASN_OP(Vertex[i], =, m_pf3VertexP[i]);
    }

	SG_UI_Process_Set_Text(_TL("Normal Updating"));

	for(m=0; m<nIterations && SG_UI_Process_Set_Progress(m, nIterations); m++)
    {
        //initialization
        for(i=0; i<m_nNumFace && SG_UI_Process_Get_Okay(); i++)
        {
            VEC3_ASN_OP(TNormal[i], =, m_pf3FaceNormalP[i]);
        }

        //modify triangle normal
        for(k=0; k<m_nNumFace && SG_UI_Process_Get_Okay(); k++)
        {
            VEC3_ZERO(m_pf3FaceNormalP[k]);
            for(i=1; i<ttRing[k][0]+1; i++)
            {
                tmp3 = DOTPROD3(TNormal[ttRing[k][i]],TNormal[k])-fSigma;
                if( tmp3 > 0.0)
                {
                    VEC3_V_OP_V_OP_S(m_pf3FaceNormalP[k],m_pf3FaceNormalP[k], +, TNormal[ttRing[k][i]], *, tmp3*tmp3);
                }
            }
            V3Normalize(m_pf3FaceNormalP[k]);
        }

        for(k=0; k<m_nNumFace && SG_UI_Process_Get_Okay(); k++)
        {
            VEC3_ASN_OP(TNormal[k], =, m_pf3FaceNormalP[k]);
        }
    }

    //modify vertex coordinates
    VertexUpdate(m_ppnVRing1T, nVIterations);
    //m_L2Error = L2Error();

    SG_FREE_SAFE(Vertex);
    SG_FREE_SAFE(TNormal);

    return;
}

//---------------------------------------------------------
void CMesh_Denoise::VertexUpdate(int** tRing, int nVIterations)
{
    int i, j, m;
    int nTmp0, nTmp1, nTmp2;
    double fTmp1;

    FVECTOR3 vect[3];
   
	SG_UI_Process_Set_Text(_TL("Vertex Updating"));

    for(m=0; m<nVIterations && SG_UI_Process_Set_Progress(m, nVIterations); m++)
    {   
        for(i=0; i<m_nNumVertex; i++)
        {
            VEC3_ZERO(vect[1]);
            for(j=1; j<tRing[i][0]+1; j++)
            {
                nTmp0 = m_pn3Face[tRing[i][j]][0]; // the vertex number of triangle tRing[i][j]
                nTmp1 = m_pn3Face[tRing[i][j]][1];
                nTmp2 = m_pn3Face[tRing[i][j]][2];
                VEC3_V_OP_V_OP_V(vect[0], m_pf3VertexP[nTmp0],+, m_pf3VertexP[nTmp1],+, m_pf3VertexP[nTmp2]);
                VEC3_V_OP_S(vect[0], vect[0], /, 3.0); //vect[0] is the centr of the triangle.
                VEC3_V_OP_V(vect[0], vect[0], -, m_pf3VertexP[i]); //vect[0] is now vector PC.
                fTmp1 = DOTPROD3(vect[0], m_pf3FaceNormalP[tRing[i][j]]);
				if(m_bZOnly)
					vect[1][2] = vect[1][2] + m_pf3FaceNormalP[tRing[i][j]][2] * fTmp1;
				else
					VEC3_V_OP_V_OP_S(vect[1], vect[1], +, m_pf3FaceNormalP[tRing[i][j]],*, fTmp1);                   
            }
            if (tRing[i][0]!=0)
            {
				if(m_bZOnly)
					m_pf3VertexP[i][2] = m_pf3VertexP[i][2] + vect[1][2]/tRing[i][0];
				else
					VEC3_V_OP_V_OP_S(m_pf3VertexP[i], m_pf3VertexP[i],+, vect[1], /, tRing[i][0]);
            }
        }
    }
    ComputeNormal(true);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
