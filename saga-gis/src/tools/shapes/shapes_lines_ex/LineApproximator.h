/**********************************************************
 * Version $Id$
 *********************************************************/
/* Plot Graphic Library

	Copyright (C) 2002 Pelikhan, Inc. All rights reserved
	Go to http://eauminerale.syldavie.csam.ucl.ac.be/peli/pgl/pgl.html for the latest PGL binaries
	This software is NOT freeware.

	This software is provided "as is", with no warranty.
	Read the license agreement provided with the files

	This software, and all accompanying files, data and materials, are distributed "AS IS" and with no warranties of any kind, 
	whether express or implied.  his disclaimer of warranty constitutes an essential part of the agreement.  
	In no event shall Pelikhan, or its principals, shareholders, officers, employees, affiliates, contractors, subsidiaries, 
	or parent organizations, be liable for any incidental, consequential, or punitive damages whatsoever relating to the use of PGL, 
	or your relationship with Pelikhan.

	Author: Jonathan de Halleux, dehalleux@auto.ucl.ac.be
*/
#if !defined(AFX_LINEAPPROXIMATOR_H__F5E6E8DC_1185_4AC0_A061_7B3309700E9D__INCLUDED_)
#define AFX_LINEAPPROXIMATOR_H__F5E6E8DC_1185_4AC0_A061_7B3309700E9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <crtdbg.h>
#ifndef ASSERT
	#ifdef _DEBUG
		#define ASSERT(a)	_ASSERT(a);
	#else
		#define ASSERT(a)
	#endif
#endif
#ifdef min
	#undef min
#endif
#ifdef max
	#undef max
#endif
	
#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>

/*! 
  \defgroup LAGroup Line approximation algorithms
*/
namespace hull
{

/*! \brief A point (x,y)

\param T float or double

  A pair of (x,y) values.
\ingroup LAGroup
*/
template<typename T>
class TPoint
{
public:
	//! Default constructor
	TPoint( T _x = 0, T _y=0):x(_x), y(_y){};
	//! Assignement constructor
	TPoint& operator = ( const TPoint<T>& t)	{	if (&t != this){ x=t.x; y=t.y;} return *this;};

	//! x value
	T x;
	//! y value
	T y;
};

/*! \brief A point (x,y) with references

\param T float or double

  \ingroup LAGroup
*/
template<typename T>
class TPointRef
{
public:
	//! Default contructor
	TPointRef():x(xDummy),y(yDummy){};
	/*! \brief Constructor with 2 values

	\param _x value to get reference to.
	\param _y value to get reference to.
	*/
	TPointRef(T& _x, T& _y):x(_x),y(_y){};
	//! Assignement constructor
	TPointRef<T>& operator = (const TPointRef<T>& t)	{	if (this != &t){ x=t.x;y=t.y;} return *this;};
	//! Assignement constructor with point
	TPointRef<T>& operator = (TPoint<T> t)		{	x=t.x;y=t.y; return *this;};
	//! x, reference to a value
	T& x;
	//! y, reference to a value
	T& y;
private:
	static T xDummy;
	static T yDummy;
};

template<typename T> T TPointRef<T>::xDummy=0;
template<typename T> T TPointRef<T>::yDummy=0;

/*! \brief Virtual base class for Line approximator classes

\par Template arguments

\param T float or double
\param TPointContainer a container of points (structure with x,y) with random access iterators
\param TKeyContainer a container of TPointContainer::const_iterator (structure with x,y) with single direction iterators


\par Notations:

  - points : data to interpolate
  - keys : selected points from available set that interpolate within the desired tolerance

\par Containers:

  - Points are stored in a #PointContainer. #PointContainer is a container such that
	#- has random access iterator,
	#- value_type is a structure/class with x,y members
  - Keys are stored in a #KeyContainer. #KeyContainer is a container such that:
	#- has single directional iterator,
	#- value_type is PointContainer::const_iterator 

\par Data normalization:

  To avoid numerical instability when computing cross product and so, data is normalized ( see #NormalizePoints ).
  To enable/disable normalization, use #SetNormalization.

  \ingroup LAGroup
*/
template<typename T, typename TPointContainer, typename TKeyContainer>
class TLine
{
public:

	//! \name Structures and typedefs
	//@{
	//! Point container
	typedef TPointContainer PointContainer;
	//! Key container
	typedef TKeyContainer KeyContainer;
	//! 2D homogenous point
	struct SHomog
	{
	public:
		//! Default constructor
		SHomog(T _x=0, T _y=0, T _w=1)	{ x=_x; y=_y; w=_w;};	

		T x;
		T y;
		T w;
	};

	/*! \brief returns square of euclidian distance
	
	*/
	static T SqrDist( const TPointContainer::const_iterator& p, const TPointContainer::const_iterator& q)
	{
		T dx=p->x-q->x;
		T dy=p->y-q->y;
		return dx*dx+dy*dy;
	}

	/*! \brief Cross product

	\param p an iterator with x,y members
	\param q an iterator with x,y members
	\result r cross product of p,q

  	The euclidian cross product of p,q:
	\f[ \vec r = \vec p \times \vec q = \left( \begin{array}{c} p_x q_y - p_y q_x \\ -q_y + p_y \\ q_x - p_x \end{array}\right)\f]
	*/
	static void CrossProduct( const TPointContainer::const_iterator& p, const TPointContainer::const_iterator& q, SHomog& r)
	{
			r.w = p->x * q->y - p->y * q->x;
			r.x = - q->y + p->y;
			r.y = q->x - p->x;
	};
		
	/*! \brief Dot product

	\param p an iterator with x,y members
	\param q an 2D homogenous point
	\result dot product of p,q

	The euclidian dot product of p,q:
	\f[ <\vec p, \vec q> = q_w + p_x q_x + p_y q_y \f]
	*/
	static T DotProduct( const TPointContainer::const_iterator& p, const SHomog& q)
	{
		return q.w + p->x*q.x + p->y*q.y;
	};
	
	/*! \brief Dot product

	\param p an iterator with x,y members
	\param q an iterator with x,y members
	\result dot product of p,q 

	The euclidian dot product of p,q:
	\f[ < \vec p,\vec q> = p_x q_x + p_y q_y \f]
	*/
	static T DotProduct( const TPointContainer::const_iterator& p, const TPointContainer::const_iterator& q)
	{
		return p->x*q->x + p->y*q->y;
	};

	/*! \brief Linear combination of points

	\param a p multiplier
	\param p a point
	\param b q multiplier
	\param q a point
	\param r linear combination of p,q

	The linear combination is:
	\f[ \vec r = a \vec p + b \vec q \f]
	*/
	static void LinComb( T a, const TPointContainer::const_iterator& p, T b, const TPointContainer::const_iterator& q, const TPointContainer::const_iterator& r)
	{
		r->x = a * p->x + b * q->x;
		r->y = a * p->y + b * q->y;
	};

	//! Internal limit structure
	struct SLimits
	{
		T dMinX;
		T dMaxX;
		T dMinY;
		T dMaxY;
		T GetCenterX()	{	return static_cast<T>((dMaxX+dMinX)/2.0);};
		T GetCenterY()	{	return static_cast<T>((dMaxY+dMinY)/2.0);};
		T GetWidth()	{	return static_cast<T>(dMaxX-dMinX);};
		T GetHeight()	{	return static_cast<T>(dMaxY-dMinY);};
	};

	//! T container
	typedef std::vector<T> TVector;
	//@}

	//! Default constructor
	TLine():m_bNormalization(true){};

	//! \name Point handling
	//@{
	//! returns number of points
	size_t GetPointSize() const				{	return m_cPoints.size();};
	//! return vector of points
	PointContainer& GetPoints()				{	return m_cPoints;};
	//! return vector of points, const
	const PointContainer& GetPoints() const	{	return m_cPoints;};
	//@}

	//! \name Key handling
	//@{
	//! returns number of keys
	size_t GetKeySize() const					{	return m_cKeys.size();};
	//! return keys
	KeyContainer& GetKeys()					{	return m_cKeys;};
	//! return keys, const
	const KeyContainer& GetKeys() const		{	return m_cKeys;};
	//@}

	//! \name Helpers
	//@{
	//! Setting points from vectors
	void SetPoints( const std::vector<T>& vX, const std::vector<T>& vY);
	//! Returning keys to vectors
	void GetKeys( std::vector<T>& vX, std::vector<T>& vY) const;
	//@}

	//! \name Bounding box
	//@{
	//! compute the bounding box
	void ComputeBoundingBox();
	//! return the point bounding box
	const SLimits& GetBoundingBox() const		{	return m_limits;};
	//@}

	//! \name Normalization
	//@{
	/*! \brief Point normalization
	
	 Let \f$(x_i,y_i)\f$, the original points and \f$(\hat x_x, \hat y_i)\f$ the normalized points:
	 \f[
	 \hat x_i = \frac{x_i - \bar x]}{\max_i (x_i-x_j)}
	 \f]
	where \f$\bar x\f$, \f$\bar y\f$ denote respectively the mean value of the \f$x_i\f$ and \f$y_i\f$.

	\sa DeNormalizePoints
	*/
	void NormalizePoints();

	/*! \brief Roll back normalization

	\sa NormalizePoints
	*/
	void DeNormalizePoints();
	//! enabled, disable normalization
	void SetNormalization( bool bEnabled = true)	{	m_bNormalization = true;};
	//! returns true if normalizing
	bool IsNormalization() const					{	return m_bNormalization;};
	//@}

	//! \name Double points checking and loop...
	//@{
	/*! \brief Discard double points

	For each pair of points \f$p_i, p_{i+1}\f$, discards \f$p_{i+1}\f$ if 
	\f[ \| p_i - p_{i+1} \| < \varepsilon \f]
	*/
	void DiscardDoublePoints();
	//! Test for loops
	void FindLoop(size_t uStartPoint, size_t& uEndPoint);
	//@}

protected:
	//! \name Attributes
	//@{
	TPointContainer m_cPoints;
	TKeyContainer m_cKeys;
	SLimits m_limits;
	bool m_bNormalization;
	//@}
};

namespace priv
{
	template<class Container, class Pred>
	struct PredX : public std::binary_function< Container::iterator, Container::iterator, bool>
	{
		bool operator()( const Container::value_type& p1, const Container::value_type& p2)	
		{	return m_pred(p1.x, p2.x); };
	protected:
		Pred m_pred;
	};

	template<class Container, class Pred>
	struct PredY : public std::binary_function< Container::iterator, Container::iterator, bool>
	{
		bool operator()( const Container::value_type& p1, const Container::value_type& p2)	
		{	return m_pred(p1.y, p2.y); };
	protected:
		Pred m_pred;
	};
};

template <typename T, typename TPointContainer, typename TKeyContainer>
void TLine<T, TPointContainer,TKeyContainer>::ComputeBoundingBox()
{
	if (m_cPoints.size() < 2)
		return;

	PointContainer::const_iterator it = (*((const TPointContainer*)&m_cPoints)).begin();


	//finding minimum and maximum...
	m_limits.dMinX=std::min_element( m_cPoints.begin(), m_cPoints.end(), priv::PredX<TPointContainer, std::less<T> >() )->x ;
	m_limits.dMaxX=std::max_element( m_cPoints.begin(), m_cPoints.end(), priv::PredX<TPointContainer, std::less<T> >() )->x ;
	m_limits.dMinY=std::min_element( m_cPoints.begin(), m_cPoints.end(), priv::PredY<TPointContainer, std::less<T> >() )->y ;
	m_limits.dMaxY=std::max_element( m_cPoints.begin(), m_cPoints.end(), priv::PredY<TPointContainer, std::less<T> >() )->y ;

	if ( fabs( m_limits.GetWidth() ) < std::numeric_limits<T>::epsilon() )
	{
		m_limits.dMaxX = m_limits.dMinX+1;
	}
	if ( fabs( m_limits.GetHeight() ) < std::numeric_limits<T>::epsilon() )
	{
		m_limits.dMaxY = m_limits.dMinY+1;
	}
}

namespace priv
{
	template<typename T>
	struct Rect
	{
		Rect( T xm, T ym, T dx, T dy)
			:m_xm(xm),m_ym(ym),m_dx(dx),m_dy(dy){};
	protected:
		T m_xm;
		T m_ym;
		T m_dx;
		T m_dy;
	};

	template<typename T>
	struct NormalizePoint : public std::unary_function< TPoint<T>& , int>, public Rect<T>
	{
		NormalizePoint( T xm, T ym, T dx, T dy)
			: Rect<T>(xm,ym,dx,dy){};
		int operator() ( TPoint<T>& point)
		{	
			point.x=(point.x-m_xm)/m_dx;
			point.y=(point.y-m_ym)/m_dy;
			return 0;
		};
	};

	template<typename T>
	struct DeNormalizePoint : public std::unary_function< TPoint<T>& , int>, public Rect<T>
	{
		DeNormalizePoint( T xm, T ym, T dx, T dy)
			: Rect<T>(xm,ym,dx,dy){};
		int operator() ( TPoint<T>& point)
		{	
			point.x=m_xm+point.x*m_dx;
			point.y=m_ym+point.y*m_dy;
			return 0;
		};
	};
};

template <typename T, typename TPointContainer, typename TKeyContainer>
void TLine<T, TPointContainer, TKeyContainer>::NormalizePoints()
{
	T xm,ym,dx,dy;
	// normalizing...
	xm=m_limits.GetCenterX();
	ym=m_limits.GetCenterY();
	dx=m_limits.GetWidth();
	dy=m_limits.GetHeight();

	std::for_each( 
		m_cPoints.begin(), 
		m_cPoints.end(), 
		priv::NormalizePoint<T>( xm, ym, dx, dy) 
		);
}

template <typename T, typename TPointContainer, typename TKeyContainer>
void TLine<T, TPointContainer, TKeyContainer>::DeNormalizePoints()
{
	T xm,ym,dx,dy;
	// normalizing...
	xm=m_limits.GetCenterX();
	ym=m_limits.GetCenterY();
	dx=m_limits.GetWidth();
	dy=m_limits.GetHeight();

	std::for_each( 
		m_cPoints.begin(), 
		m_cPoints.end(), 
		priv::DeNormalizePoint<T>( xm, ym, dx, dy) 
		);
}

template <typename T, typename TPointContainer, typename TKeyContainer>
void TLine<T, TPointContainer, TKeyContainer>::DiscardDoublePoints()
{
	// creating a list...
	TPointContainer& pc=GetPoints();
	TPointContainer::iterator it, it1;
	T epsilon2=std::numeric_limits<T>::epsilon();

#ifdef _DEBUG
	size_t count=0;
#endif
	
	it1=it=pc.begin();
	++it1;
	while (it !=pc.end() && it1!=pc.end())
	{
		if ( SqrDist(it, it1) < epsilon2 )
		{
			it1=pc.erase(it1);
		}
		else
		{
			++it; ++it1;
		}
	}

#ifdef _DEBUG
	TRACE( _T("Numer of (double) points erased: %d\n"), count);
#endif
};

template <typename T, typename TPointContainer, typename TKeyContainer>
void TLine<T, TPointContainer, TKeyContainer>::SetPoints( const std::vector<T>& vX, const std::vector<T>& vY)
{
	TPointContainer& pc=GetPoints();
	const size_t n = __min( vX.size(), vY.size());

	pc.resize(n);
	for (size_t i=0;i<n;i++)
	{
		pc[i]= TPoint<T>( vX[i], vY[i]);
	}
};

template <typename T, typename TPointContainer, typename TKeyContainer>
void TLine<T, TPointContainer, TKeyContainer>::GetKeys( std::vector<T>& vX, std::vector<T>& vY) const
{
	const TKeyContainer& kc=GetKeys();
	TKeyContainer::const_iterator it;
	size_t i;

	vX.resize(kc.size());
	vY.resize(kc.size());

	for (it=kc.begin(), i=0;it!=kc.end();it++, i++)
	{
		vX[i]=(*it)->x;
		vY[i]=(*it)->y;
	}
};

template <typename T, typename TPointContainer, typename TKeyContainer>
void TLine<T, TPointContainer, TKeyContainer>::FindLoop(size_t uStartPoint, size_t& uEndPoint)
{
};


/*! \brief Base class for line approximators

	

  \ingroup LAGroup
*/
template <typename T, typename TPointContainer, typename TKeyContainer>
class TLineApproximator : virtual public TLine<T,TPointContainer, TKeyContainer>
{
public:

	//! \name Constructor
	//@{
	TLineApproximator(): m_dTol(0)
	{m_limits.dMinX=m_limits.dMinY=0;m_limits.dMaxX=m_limits.dMaxY=1;};
	~TLineApproximator(){};
	//@}


	//! \name Tolerance
	//@{
	//! sets the tolerance
	void SetTol( double dTol)				{	m_dTol = __max( dTol, 0);};
	//! return current tolerance
	double GetTol() const					{	return m_dTol;};
	//@}

	//! \name Simplification functions
	//@{
	//! Initialize simplification
	void ClearKeys()								{	m_cKeys.clear();};
	//! Compute the keys
	void Simplify();
	/*! Shrink to compression level 
	
	\param dScale scaling to apply [0...1]
	\param dScaleTol [optional] tolerance with respect to dScale, default is 0.05
	\param eTolRight [optional] first estimate on right tolerance
	\param nMaxIter [optional] maximum number of iterations, default is 250
	\return number of estimations
	*/
	size_t ShrinkNorm( T dScale, T dScaleTol = 0.05, T eTolRight = -1, size_t nMaxIter = 250);

	/*! Shrink to a specified number of points
	
	\param n desired number of points in the approximate curve
	\param nTol [optional] tolerance with respect to n, default is 10
	\param eTolRight [optional] first estimate on right tolerance
	\param nMaxIter [optional] maximum number of iterations, default is 250
	\return number of estimations
	*/
	size_t Shrink( size_t nDesiredPoints, size_t nTol = 10, T eTolRight = -1, size_t nMaxIter = 250);
	//@}

protected:
	//! \name Virtual functions
	//@{
	/*! \brief Virtual approximation function

	This function must be overriden in inherited classes. To implement your own algorithm,
	override this function.
	*/
	virtual void ComputeKeys()		{	ClearKeys();};
	//@}

private:
	T m_dTol;
};


template <typename T, typename TPointContainer, typename TKeyContainer>
size_t TLineApproximator<T,TPointContainer,TKeyContainer>::ShrinkNorm( T dScale, T dScaleTol, T eTolRight ,size_t nMaxIter)
{
	// number of points wanted...
	size_t uWantedPoints= __min(m_cPoints.size(), __max(2, static_cast<size_t>(floor(m_cPoints.size()*dScale))));
	size_t uTol = __min(m_cPoints.size(), __max(0, static_cast<size_t>(floor(m_cPoints.size()*dScaleTol)) ));

	return Shrink( uWantedPoints, uTol, eTolRight, nMaxIter);
}

namespace priv
{
//  (C) Copyright Gennadiy Rozental 2001-2002.
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied warranty,
//  and with no claim as to its suitability for any purpose.

//  See http://www.boost.org for most recent version including documentation.
//
//  File        : $RCSfile: LineApproximator.h,v $
//
//  Version     : $Id$
//
//  Description : defines algoirthms for comparing 2 floating point values
// ***************************************************************************
	template<typename FPT>
	inline FPT
	fpt_abs( FPT arg ) 
	{
	    return arg < 0 ? -arg : arg;
	}

	// both f1 and f2 are unsigned here
	template<typename FPT>
	inline FPT 
	safe_fpt_division( FPT uf1, FPT uf2 )
	{
	    return  ( uf1 < 1 && uf1 > uf2 * std::numeric_limits<FPT>::max())   
			? std::numeric_limits<FPT>::max() :
	           ((uf2 > 1 && uf1 < uf2 * std::numeric_limits<FPT>::min() || 
	             uf1 == 0)                                               ? 0                               :
	                                                                      uf1/uf2 );
	}

	template<typename FPT>
	class close_at_tolerance 
	{
	public:
	    explicit close_at_tolerance( FPT tolerance, bool strong_or_weak = true ) 
			: p_tolerance( tolerance ),m_strong_or_weak( strong_or_weak ) { };
	
	 explicit    close_at_tolerance( int number_of_rounding_errors, bool strong_or_weak = true ) 
	    : p_tolerance( std::numeric_limits<FPT>::epsilon() * number_of_rounding_errors/2 ), 
	   m_strong_or_weak( strong_or_weak ) {}
	
	    bool        operator()( FPT left, FPT right ) const
	    {
	        FPT diff = fpt_abs( left - right );
	     FPT d1   = safe_fpt_division( diff, fpt_abs( right ) );
	     FPT d2   = safe_fpt_division( diff, fpt_abs( left ) );
	        
	        return m_strong_or_weak ? (d1 <= p_tolerance.get() && d2 <= p_tolerance.get()) 
	                             : (d1 <= p_tolerance.get() || d2 <= p_tolerance.get());
	    }
	
	    // Data members
		class p_tolerance_class
		{
		private:
			FPT f;
		public:
			p_tolerance_class(FPT _f=0):f(_f){};
			FPT  get() const{	return f;};
		};
		p_tolerance_class p_tolerance;	
	private:
	bool        m_strong_or_weak;
	};

	template <typename T>
	inline bool IsEqual(T x, T y)		
	{ 
	  static close_at_tolerance<T> comp( std::numeric_limits<T>::epsilon()/2*10);
	  return comp(fpt_abs(x),fpt_abs(y));
	};
  
	template <typename T>
	inline bool IsEmptyInterval(T x, T y)
	{
		return ( x>=y || IsEqual(x,y) );
	}

};

template<typename T, typename TPointContainer, typename TKeyContainer>
size_t TLineApproximator<T,TPointContainer,TKeyContainer>::Shrink( size_t nDesiredPoints, size_t nTol, T eTolRight, size_t nMaxIter)
{
	if (m_cPoints.size()<2)
		return 0;
	
	// number of points wanted...
	T dWantedPoints= __min(m_cPoints.size(), __max(2, nDesiredPoints));
	T uMinWantedPoints = __min(m_cPoints.size(), __max(2, nDesiredPoints-nTol ));
	T uMaxWantedPoints = __min(m_cPoints.size(), __max(2, nDesiredPoints+nTol ));

	T eLeft, eRight, eMiddle;
	T dResultLeft, dResultRight;
	size_t iter=0;

	// compute limits
	ComputeBoundingBox();

	// normalize if needed
	if (m_bNormalization)
		NormalizePoints();

	// first estimation
	eLeft = 0;
	SetTol(eLeft);

	ComputeKeys();

	dResultLeft =  m_cKeys.size();
	iter++;
	// test if success
	if ( (m_cKeys.size()<=uMaxWantedPoints) && (m_cKeys.size() >= uMinWantedPoints)  )
		goto PostProcess;

	// second estimation
	if (eTolRight<=0)
		eRight=__max( m_limits.GetWidth(), m_limits.GetHeight()); 
	else
		eRight=eTolRight;
	SetTol(eRight);

	ComputeKeys();

	dResultRight =  m_cKeys.size();

	// test if optimization possible
//	if (dResultLeft<uMinWantedPoints ||  dResultRight>uMaxWantedPoints)
//		throw _T("TLineApproximator<T>::Shrink failed: Desired compression ratio not possible in the tolerance domain.");

	iter++;
	// test if success
	if ( ((m_cKeys.size()<=uMaxWantedPoints) && (m_cKeys.size() >= uMinWantedPoints)) || (dResultLeft == dResultRight) )
		goto PostProcess;

	// main loop, dichotomy
	do
	{ 
		// test middle
		eMiddle=(eLeft +eRight)/2;
		SetTol(eMiddle);

		// computing new DP...
		ComputeKeys();
		
		// updating...
		if ( (m_cKeys.size()-dWantedPoints)*( dResultLeft-dResultRight) < 0 )
		{
			eRight=eMiddle;
			dResultRight=m_cKeys.size();
		}
		else 
		{
			eLeft=eMiddle;
			dResultLeft=m_cKeys.size();
		}

		iter++;
	} while ( ((m_cKeys.size()>uMaxWantedPoints) || (m_cKeys.size() < uMinWantedPoints)) /* checking that we are in the acceptable compression */
		&& !priv::IsEmptyInterval(eLeft,eRight) /* interval is non empty */
		&& (dResultRight != dResultLeft)
		&& iter<nMaxIter /* checking for maximum number of iterations */);

PostProcess:
	if (m_bNormalization)
		DeNormalizePoints();

	return iter;
}


template <typename T, typename TPointContainer, typename TKeyContainer>
void TLineApproximator<T,TPointContainer, TKeyContainer>::Simplify()
{
	if (m_cPoints.size()<2)
		return;

	// compute limits
	ComputeBoundingBox();
	
	// preprocess...
	if (m_bNormalization)
		NormalizePoints();

	ComputeKeys();

	if (m_bNormalization)
		DeNormalizePoints();
}

}; // namespace hull

#endif // !defined(AFX_LINEAPPROXIMATOR_H__F5E6E8DC_1185_4AC0_A061_7B3309700E9D__INCLUDED_)
