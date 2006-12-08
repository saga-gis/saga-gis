
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    mat_matrix.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <memory.h>

#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Matrix_LU_Decomposition	(int n, int *Permutation, double **Matrix, bool bSilent);
bool		SG_Matrix_LU_Solve			(int n, int *Permutation, double **Matrix, double *Vector, bool bSilent);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Vector::CSG_Vector(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Vector::CSG_Vector(const CSG_Vector &Vector)
{
	_On_Construction();

	Create(Vector);
}

bool CSG_Vector::Create(const CSG_Vector &Vector)
{
	return( Assign(Vector) );
}

//---------------------------------------------------------
CSG_Vector::CSG_Vector(int n, double *Data)
{
	_On_Construction();

	Create(n, Data);
}

bool CSG_Vector::Create(int n, double *Data)
{
	if( n > 0 )
	{
		if( n != m_n )
		{
			Destroy();

			m_n	= n;
			m_z	= (double *)SG_Malloc(m_n * sizeof(double));
		}

		if( Data )
		{
			memcpy(m_z, Data, m_n * sizeof(double));
		}
		else
		{
			memset(m_z,    0, m_n * sizeof(double));
		}

		return( true );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
CSG_Vector::~CSG_Vector(void)
{
	Destroy();
}

bool CSG_Vector::Destroy(void)
{
	if( m_z )
	{
		SG_Free(m_z);
		m_z	= NULL;
		m_n	= 0;
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Vector::_On_Construction(void)
{
	m_z	= NULL;
	m_n	= 0;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Vector::asString(void)
{
	CSG_String	s;

	for(int i=0; i<m_n; i++)
	{
		s.Append(CSG_String::Format(SG_T("%f\n"), m_z[i]));
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Vector::is_Equal(const CSG_Vector &Vector) const
{
	if( m_n == Vector.m_n )
	{
		for(int i=0; i<m_n; i++)
		{
			if( m_z[i] != Vector.m_z[i] )
			{
				return( false );
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Vector::Assign(double Scalar)
{
	if( m_n > 0 )
	{
		for(int i=0; i<m_n; i++)
		{
			m_z[i]	= Scalar;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Vector::Assign(const CSG_Vector &Vector)
{
	if( Create(Vector.m_n) )
	{
		memcpy(m_z, Vector.m_z, m_n * sizeof(double));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Vector::Add(double Scalar)
{
	if( m_n > 0 )
	{
		for(int i=0; i<m_n; i++)
		{
			m_z[i]	+= Scalar;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Vector::Add(const class CSG_Vector &Vector)
{
	if( m_n == Vector.m_n && m_n > 0 )
	{
		for(int i=0; i<m_n; i++)
		{
			m_z[i]	+= Vector.m_z[i];
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Vector::Subtract(const class CSG_Vector &Vector)
{
	if( m_n == Vector.m_n && m_n > 0 )
	{
		for(int i=0; i<m_n; i++)
		{
			m_z[i]	-= Vector.m_z[i];
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Vector::Multiply(double Scalar)
{
	if( m_n > 0 )
	{
		for(int i=0; i<m_n; i++)
		{
			m_z[i]	*= Scalar;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Vector::Multiply(const class CSG_Vector &Vector)
{
	if( m_n == Vector.m_n && m_n == 3 )
	{
		CSG_Vector	v(*this);

		m_z[0]	= v[1] * Vector.m_z[2] - v[2] * Vector.m_z[1];
		m_z[1]	= v[2] * Vector.m_z[0] - v[0] * Vector.m_z[2];
		m_z[2]	= v[0] * Vector.m_z[1] - v[1] * Vector.m_z[0];

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
double CSG_Vector::Multiply_Scalar(const class CSG_Vector &Vector) const
{
	double	z	= 0.0;

	if( m_n == Vector.m_n )
	{
		for(int i=0; i<m_n; i++)
		{
			z	+= m_z[i] * Vector.m_z[i];
		}
	}

	return( z );
}

//---------------------------------------------------------
bool CSG_Vector::Multiply(const CSG_Matrix &Matrix)
{
	return( Assign(Matrix.Multiply(*this)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Vector::operator == (const CSG_Vector &Vector) const
{
	return( is_Equal(Vector) );
}

//---------------------------------------------------------
CSG_Vector & CSG_Vector::operator = (double Scalar)
{
	Assign(Scalar);

	return( *this );
}

CSG_Vector & CSG_Vector::operator = (const CSG_Vector &Vector)
{
	Assign(Vector);

	return( *this );
}

//---------------------------------------------------------
CSG_Vector & CSG_Vector::operator += (double Scalar)
{
	Add(Scalar);

	return( *this );
}

CSG_Vector & CSG_Vector::operator += (const class CSG_Vector &Vector)
{
	Add(Vector);

	return( *this );
}

//---------------------------------------------------------
CSG_Vector & CSG_Vector::operator -= (double Scalar)
{
	Add(-Scalar);

	return( *this );
}

CSG_Vector & CSG_Vector::operator -= (const class CSG_Vector &Vector)
{
	Subtract(Vector);

	return( *this );
}

//---------------------------------------------------------
CSG_Vector & CSG_Vector::operator *= (double Scalar)
{
	Multiply(Scalar);

	return( *this );
}

CSG_Vector & CSG_Vector::operator *= (const class CSG_Vector &Vector)
{
	Multiply(Vector);

	return( *this );
}

CSG_Vector & CSG_Vector::operator *= (const CSG_Matrix &Matrix)
{
	Multiply(Matrix);

	return( *this );
}

//---------------------------------------------------------
CSG_Vector CSG_Vector::operator + (double Scalar) const
{
	CSG_Vector	v(*this);

	v.Add(Scalar);

	return( v );
}

CSG_Vector CSG_Vector::operator + (const class CSG_Vector &Vector) const
{
	CSG_Vector	v(*this);

	v.Add(Vector);

	return( v );
}

//---------------------------------------------------------
CSG_Vector CSG_Vector::operator - (double Scalar) const
{
	CSG_Vector	v(*this);

	v.Add(-Scalar);

	return( v );
}

CSG_Vector CSG_Vector::operator - (const class CSG_Vector &Vector) const
{
	CSG_Vector	v(*this);

	v.Subtract(Vector);

	return( v );
}

//---------------------------------------------------------
CSG_Vector CSG_Vector::operator * (double Scalar) const
{
	CSG_Vector	v(*this);

	v.Multiply(Scalar);

	return( v );
}

double CSG_Vector::operator * (const class CSG_Vector &Vector) const
{
	return( Multiply_Scalar(Vector) );
}

CSG_Vector operator * (double Scalar, const CSG_Vector &Vector)
{
	return( Vector * Scalar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Vector::Set_Zero(void)
{
	return( Create(m_n) );
}

//---------------------------------------------------------
bool CSG_Vector::Set_Unity(void)
{
	double	Length;

	if( (Length = Get_Length()) > 0.0 )
	{
		for(int i=0; i<m_n; i++)
		{
			m_z[i]	/= Length;
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Vector::Get_Length(void) const
{
	if( m_n > 0 )
	{
		double	z	= 0.0;

		for(int i=0; i<m_n; i++)
		{
			z	+= m_z[i] * m_z[i];
		}

		return( sqrt(z) );
	}

	return( 0.0 );
}

//---------------------------------------------------------
double CSG_Vector::Get_Angle(const CSG_Vector &Vector) const
{
	if( m_n > Vector.m_n )
	{
		return( Vector.Get_Angle(*this) );
	}

	int		i;
	double	A, B, z;

	if( (A = Get_Length()) > 0.0 && (B = Vector.Get_Length()) > 0.0 )
	{
		for(i=0, z=0.0; i<m_n; i++)
		{
			z	+= Vector.m_z[i] * m_z[i];
		}

		for(i=m_n; i<Vector.m_n; i++)
		{
			z	+= Vector.m_z[i];
		}

		return( acos(z / (A * B)) );
	}

	return( 0.0 );
}

//---------------------------------------------------------
CSG_Vector CSG_Vector::Get_Unity(void) const
{
	CSG_Vector	v(*this);

	v.Set_Unity();

	return( v );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Matrix::CSG_Matrix(void)
{
	_On_Construction();
}

//---------------------------------------------------------
CSG_Matrix::CSG_Matrix(const CSG_Matrix &Matrix)
{
	_On_Construction();

	Create(Matrix);
}

bool CSG_Matrix::Create(const CSG_Matrix &Matrix)
{
	return( Assign(Matrix) );
}

//---------------------------------------------------------
CSG_Matrix::CSG_Matrix(int nx, int ny, double *Data)
{
	_On_Construction();

	Create(nx, ny, Data);
}

bool CSG_Matrix::Create(int nx, int ny, double *Data)
{
	if( nx > 0 && ny > 0 )
	{
		if( nx != m_nx || ny != m_ny )
		{
			m_nx	= nx;
			m_ny	= ny;
			m_z		= (double **)SG_Malloc(m_ny        * sizeof(double *));
			m_z[0]	= (double  *)SG_Malloc(m_ny * m_nx * sizeof(double  ));

			for(ny=1; ny<m_ny; ny++)
			{
				m_z[ny]	= m_z[ny - 1] + nx;
			}
		}

		if( Data )
		{
			memcpy(m_z[0], Data, m_ny * m_nx * sizeof(double));
		}
		else
		{
			memset(m_z[0],    0, m_ny * m_nx * sizeof(double));
		}

		return( true );
	}

	Destroy();

	return( false );
}

//---------------------------------------------------------
CSG_Matrix::~CSG_Matrix(void)
{
	Destroy();
}

bool CSG_Matrix::Destroy(void)
{
	if( m_z )
	{
		SG_Free(m_z[0]);
		SG_Free(m_z);
		m_z		= NULL;
		m_nx	= 0;
		m_ny	= 0;
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Matrix::_On_Construction(void)
{
	m_z		= NULL;
	m_nx	= 0;
	m_ny	= 0;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Matrix::asString(void)
{
	CSG_String	s;

	for(int y=0; y<m_ny; y++)
	{
		for(int x=0; x<m_nx; x++)
		{
			s.Append(CSG_String::Format(SG_T("%f\t"), m_z[y][x]));
		}

		s.Append(SG_T("\n"));
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Matrix::is_Equal(const CSG_Matrix &Matrix) const
{
	if( m_nx == Matrix.m_nx && m_ny == Matrix.m_ny )
	{
		for(int y=0; y<m_ny; y++)
		{
			for(int x=0; x<m_nx; x++)
			{
				if( m_z[y][x] != Matrix.m_z[y][x] )
				{
					return( false );
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Matrix::Assign(double Scalar)
{
	if( m_nx > 0 && m_ny > 0 )
	{
		for(int y=0; y<m_ny; y++)
		{
			for(int x=0; x<m_nx; x++)
			{
				m_z[y][x]	= Scalar;
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Matrix::Assign(const CSG_Matrix &Matrix)
{
	if( Create(Matrix.m_nx, Matrix.m_ny) )
	{
		memcpy(m_z[0], Matrix.m_z[0], m_nx * m_ny * sizeof(double));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Matrix::Add(double Scalar)
{
	if( m_nx > 0 && m_ny > 0 )
	{
		for(int y=0; y<m_ny; y++)
		{
			for(int x=0; x<m_nx; x++)
			{
				m_z[y][x]	+= Scalar;
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Matrix::Add(const CSG_Matrix &Matrix)
{
	if( m_nx == Matrix.m_nx && m_ny == Matrix.m_ny )
	{
		for(int y=0; y<m_ny; y++)
		{
			for(int x=0; x<m_nx; x++)
			{
				m_z[y][x]	+= Matrix.m_z[y][x];
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Matrix::Subtract(const CSG_Matrix &Matrix)
{
	if( m_nx == Matrix.m_nx && m_ny == Matrix.m_ny )
	{
		for(int y=0; y<m_ny; y++)
		{
			for(int x=0; x<m_nx; x++)
			{
				m_z[y][x]	-= Matrix.m_z[y][x];
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Matrix::Multiply(double Scalar)
{
	if( m_nx > 0 && m_ny > 0 )
	{
		for(int y=0; y<m_ny; y++)
		{
			for(int x=0; x<m_nx; x++)
			{
				m_z[y][x]	*= Scalar;
			}
		}

		return( true );
	}

	return( false );
}

CSG_Vector CSG_Matrix::Multiply(const CSG_Vector &Vector) const
{
	CSG_Vector	v;

	if( m_nx == Vector.Get_N() && v.Create(m_ny) )
	{
		for(int y=0; y<m_ny; y++)
		{
			double	z	= 0.0;

			for(int x=0; x<m_nx; x++)
			{
				z	+= m_z[y][x] * Vector(x);
			}

			v[y]	= z;
		}
	}

	return( v );
}

CSG_Matrix CSG_Matrix::Multiply(const CSG_Matrix &Matrix) const
{
	CSG_Matrix	m;

	if( m_nx == Matrix.m_ny && m.Create(Matrix.m_nx, m_ny) )
	{
		for(int y=0; y<m.m_ny; y++)
		{
			for(int x=0; x<m.m_nx; x++)
			{
				double	z	= 0.0;

				for(int n=0; n<m_nx; n++)
				{
					z	+= m_z[y][n] * Matrix.m_z[n][x];
				}

				m.m_z[y][x]	= z;
			}
		}
	}

	return( m );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Matrix::operator == (const CSG_Matrix &Matrix) const
{
	return( is_Equal(Matrix) );
}

//---------------------------------------------------------
CSG_Matrix & CSG_Matrix::operator = (double Scalar)
{
	Assign(Scalar);

	return( *this );
}

CSG_Matrix & CSG_Matrix::operator = (const CSG_Matrix &Matrix)
{
	Assign(Matrix);

	return( *this );
}

//---------------------------------------------------------
CSG_Matrix & CSG_Matrix::operator += (double Scalar)
{
	Add(Scalar);

	return( *this );
}

CSG_Matrix & CSG_Matrix::operator += (const CSG_Matrix &Matrix)
{
	Add(Matrix);

	return( *this );
}

//---------------------------------------------------------
CSG_Matrix & CSG_Matrix::operator -= (double Scalar)
{
	Add(-Scalar);

	return( *this );
}

CSG_Matrix & CSG_Matrix::operator -= (const CSG_Matrix &Matrix)
{
	Subtract(Matrix);

	return( *this );
}

//---------------------------------------------------------
CSG_Matrix & CSG_Matrix::operator *= (double Scalar)
{
	Multiply(Scalar);

	return( *this );
}

CSG_Matrix & CSG_Matrix::operator *= (const CSG_Matrix &Matrix)
{
	Multiply(Matrix);

	return( *this );
}

//---------------------------------------------------------
CSG_Matrix CSG_Matrix::operator + (double Scalar) const
{
	CSG_Matrix	m(*this);

	m.Add(Scalar);

	return( m );
}

CSG_Matrix CSG_Matrix::operator + (const CSG_Matrix &Matrix) const
{
	CSG_Matrix	m(*this);

	m.Add(Matrix);

	return( m );
}

//---------------------------------------------------------
CSG_Matrix CSG_Matrix::operator - (double Scalar) const
{
	CSG_Matrix	m(*this);

	m.Add(-Scalar);

	return( m );
}

CSG_Matrix CSG_Matrix::operator - (const CSG_Matrix &Matrix) const
{
	CSG_Matrix	m(*this);

	m.Subtract(Matrix);

	return( m );
}

//---------------------------------------------------------
CSG_Matrix CSG_Matrix::operator * (double Scalar) const
{
	CSG_Matrix	m(*this);

	m.Multiply(Scalar);

	return( m );
}

CSG_Vector CSG_Matrix::operator * (const CSG_Vector &Vector) const
{
	return( Multiply(Vector) );
}

CSG_Matrix CSG_Matrix::operator * (const CSG_Matrix &Matrix) const
{
	return( Multiply(Matrix) );
}

CSG_Matrix	operator * (double Scalar, const CSG_Matrix &Matrix)
{
	return( Matrix * Scalar );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Matrix::Set_Zero(void)
{
	return( Create(m_nx, m_ny) );
}

//---------------------------------------------------------
bool CSG_Matrix::Set_Identity(void)
{
	if( m_nx > 0 && m_ny > 0 )
	{
		for(int y=0; y<m_ny; y++)
		{
			for(int x=0; x<m_nx; x++)
			{
				m_z[y][x]	= x == y ? 1.0 : 0.0;
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Matrix::Set_Transpose(void)
{
	CSG_Matrix	m;
	
	if( m.Create(*this) && Create(m_ny, m_nx) )
	{
		for(int y=0; y<m_ny; y++)
		{
			for(int x=0; x<m_nx; x++)
			{
				m_z[y][x]	= m.m_z[y][x];
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Matrix::Set_Inverse(bool bSilent, int nSubSquare)
{
	bool	bResult	= false;
	int		n		= 0;

	//-----------------------------------------------------
	if( nSubSquare > 0 )
	{
		if( nSubSquare <= m_nx && nSubSquare <= m_ny )
		{
			n	= nSubSquare;
		}
	}
	else if( is_Square() )
	{
		n	= m_nx;
	}

	//-----------------------------------------------------
	if( n > 0 )
	{
		CSG_Matrix	m(*this);
		int		*Permutation	= (int *)SG_Malloc(n * sizeof(int));

		if( SG_Matrix_LU_Decomposition(n, Permutation, m.Get_Data(), bSilent) )
		{
			CSG_Vector	v(n);

			for(int j=0; j<n && (bSilent || SG_UI_Process_Set_Progress(j, n)); j++)
			{
				v.Set_Zero();
				v[j]	= 1.0;

				SG_Matrix_LU_Solve(n, Permutation, m.Get_Data(), v.Get_Data(), true);

				for(int i=0; i<n; i++)
				{
					m_z[i][j]	= v[i];
				}
			}

			bResult	= true;
		}

		SG_Free(Permutation);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Matrix::Get_Determinant(void) const
{
	double	d	= 0.0;

	for(int y=0; y<m_ny; y++)
	{
		for(int x=0; x<m_nx; x++)
		{
		}
	}

	return( d );
}

//---------------------------------------------------------
CSG_Matrix CSG_Matrix::Get_Transpose(void) const
{
	CSG_Matrix	m(m_ny, m_nx);

	for(int y=0; y<m_ny; y++)
	{
		for(int x=0; x<m_nx; x++)
		{
			m.m_z[x][y]	= m_z[y][x];
		}
	}

	return( m );
}

//---------------------------------------------------------
CSG_Matrix CSG_Matrix::Get_Inverse(bool bSilent, int nSubSquare) const
{
	CSG_Matrix	m(*this);

	m.Set_Inverse(bSilent, nSubSquare);

	return( m );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Matrix_Solve(CSG_Matrix &Matrix, CSG_Vector &Vector, bool bSilent)
{
	bool	bResult	= false;
	int		n		= Vector.Get_N();

	if( n > 0 && n == Matrix.Get_NX() && n == Matrix.Get_NY() )
	{
		int	*Permutation	= (int *)SG_Malloc(n * sizeof(int));

		if( SG_Matrix_LU_Decomposition(n, Permutation, Matrix.Get_Data(), bSilent) )
		{
			SG_Matrix_LU_Solve(n, Permutation, Matrix.Get_Data(), Vector.Get_Data(), bSilent);

			bResult	= true;
		}

		SG_Free(Permutation);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool		SG_Matrix_LU_Decomposition(int n, int *Permutation, double **Matrix, bool bSilent)
{
	int			i, j, k, iMax;
	double		dMax, d, Sum;
	CSG_Vector	Vector;
	
	Vector.Create(n);

	for(i=0, iMax=0; i<n && (bSilent || SG_UI_Process_Set_Progress(i, n)); i++)
	{
		dMax	= 0.0;

		for(j=0; j<n; j++)
		{
			if( (d = fabs(Matrix[i][j])) > dMax )
			{
				dMax	= d;
			}
		}

		if( dMax <= 0.0 )	// singular matrix !!!...
		{
			return( false );
		}

		Vector[i]	= 1.0 / dMax;
	}

	for(j=0; j<n && (bSilent || SG_UI_Process_Set_Progress(j, n)); j++)
	{
		for(i=0; i<j; i++)
		{
			Sum		= Matrix[i][j];

			for(k=0; k<i; k++)
			{
				Sum		-= Matrix[i][k] * Matrix[k][j];
			}

			Matrix[i][j]	= Sum;
		}

		for(i=j, dMax=0.0; i<n; i++)
		{
			Sum		= Matrix[i][j];

			for(k=0; k<j; k++)
			{
				Sum		-= Matrix[i][k] * Matrix[k][j];
			}

			Matrix[i][j]	= Sum;

			if( (d = Vector[i] * fabs(Sum)) >= dMax )
			{
				dMax	= d;
				iMax	= i;
			}
		}

		if( j != iMax )
		{
			for(k=0; k<n; k++)
			{
				d				= Matrix[iMax][k];
				Matrix[iMax][k]	= Matrix[j   ][k];
				Matrix[j   ][k]	= d;
			}

			Vector[iMax]	= Vector[j];
		}

		Permutation[j]	= iMax;

		if( Matrix[j][j] == 0.0 )
		{
			Matrix[j][j]	= M_TINY;
		}

		if( j != n )
		{
			d	= 1.0 / (Matrix[j][j]);

			for(i=j+1; i<n; i++)
			{
				Matrix[i][j]	*= d;
			}
		}
	}

	return( bSilent || SG_UI_Process_Get_Okay(false) );
}

//---------------------------------------------------------
bool		SG_Matrix_LU_Solve(int n, int *Permutation, double **Matrix, double *Vector, bool bSilent)
{
	int		i, j, k;
	double	Sum;

	for(i=0, k=-1; i<n && (bSilent || SG_UI_Process_Set_Progress(i, n)); i++)
	{
		Sum						= Vector[Permutation[i]];
		Vector[Permutation[i]]	= Vector[i];

		if( k >= 0 )
		{
			for(j=k; j<=i-1; j++)
			{
				Sum	-= Matrix[i][j] * Vector[j];
			}
		}
		else if( Sum )
		{
			k		= i;
		}

		Vector[i]	= Sum;
	}

	for(i=n-1; i>=0 && (bSilent || SG_UI_Process_Set_Progress(n-i, n)); i--)
	{
		Sum			= Vector[i];

		for(j=i+1; j<n; j++)
		{
			Sum		-= Matrix[i][j] * Vector[j];
		}

		Vector[i]	= Sum / Matrix[i][i];
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
