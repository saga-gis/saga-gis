// Containers.h: interface for the CContainers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTAINERS_H__33EAD324_F802_4BC8_BEC8_39E69C1C21A7__INCLUDED_)
#define AFX_CONTAINERS_H__33EAD324_F802_4BC8_BEC8_39E69C1C21A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <list>

namespace hull
{

/*!
	\defgroup DCGroup Double container groups
	\ingroup LAGroup
*/

/*! \brief Virtual base class for Point double container

\ingroup DCGroup	
*/
template<typename T, typename TVectorX, typename TVectorY>
class TPointDoubleContainerBase
{
public:
	//! \name Typedefs
	//@{
	typedef TPoint<T> value_type;
	typedef TPointRef<T> reference;
	typedef TVectorX container_x;
	typedef TVectorY container_y;
	//@}

	/*! \brief Constructor

	\param pX pointer to x coordinate container,
	\param pY pointer to y coordinate container
	*/
	TPointDoubleContainerBase(container_x* pX = NULL, container_y* pY =NULL):m_pX(pX),m_pY(pY){};
	virtual ~TPointDoubleContainerBase(){};
	
	//! return the size of the point container
	size_t size() const			{	ASSERT(m_pX); ASSERT(m_pY); return __min( m_pX->size(), m_pY->size());};
	/*! \brief resize the container
	
	\param size the new size of the vector.
	*/
	void resize(size_t size)	{	ASSERT(m_pX); ASSERT(m_pY); m_pX->resize(size); m_pY->resize(size);};

	//! return the value at position i
	value_type operator[](UINT i) const	{	return value_type((*m_pX)[i],(*m_pY)[i]);};
	//! return a reference to position i
	reference operator[](UINT i)		{	return reference((*m_pX)[i],(*m_pY)[i]);};

	//! Sets the container pointers
	void SetContainers( container_x* pX, container_y* pY)	{	m_pX=pX; m_pY=pY;};
	//! return the x coordinate pointer, const
	const container_x* GetXContainer() const		{	return m_pX;};
	//! return the y coordinate pointer, const
	const container_y* GetYContainer() const		{	return m_pY;};
	//! return the x coordinate pointer
	container_x* GetXContainer()					{	return m_pX;};
	//! return the y coordinate pointer
	container_y* GetYContainer()					{	return m_pY;};

protected:
	container_x* m_pX;
	container_y* m_pY;
};

/*! \brief Base class for point iterator

\param T float or double
\param TVectorX x coordinate container with random access iterator
\param TVectorY y coordinate container with random access iterator

	A random access iterator base class.

  \ingroup DCGroup
*/
template<typename T,typename TVectorX, typename TVectorY>
class TPointIt
{
public:
	TPointIt(const TPointDoubleContainerBase<T,TVectorX,TVectorY>* pV = NULL, UINT index = 0):m_pV(pV),m_index(index){};
	virtual ~TPointIt(){};

	TPointIt<T,TVectorX,TVectorY>& operator = (const TPointIt<T,TVectorX,TVectorY>& t)	
	{	
		if (&t!=this)
		{
			m_pV=t.m_pV;
			m_index=t.m_index;
		};
		return *this;
	};

	friend UINT operator - (const TPointIt<T,TVectorX,TVectorY>& t1, const TPointIt<T,TVectorX,TVectorY>& t2){	return t1.m_index-t2.m_index; };

	friend bool operator == ( const TPointIt<T,TVectorX,TVectorY>& t1, const TPointIt<T,TVectorX,TVectorY>& t2)	{	return t1.m_index==t2.m_index && t1.m_pV==t2.m_pV;};
	friend bool operator != ( const TPointIt<T,TVectorX,TVectorY>& t1, const TPointIt<T,TVectorX,TVectorY>& t2)	{	return  t1.m_index!=t2.m_index || t1.m_pV!=t2.m_pV;};
	friend bool operator < ( const TPointIt<T,TVectorX,TVectorY>& t1, const TPointIt<T,TVectorX,TVectorY>& t2)	{	return  t1.m_index<t2.m_index;};
	friend bool operator > ( const TPointIt<T,TVectorX,TVectorY>& t1, const TPointIt<T,TVectorX,TVectorY>& t2)	{	return  t1.m_index>t2.m_index;};
	friend bool operator <= ( const TPointIt<T,TVectorX,TVectorY>& t1, const TPointIt<T,TVectorX,TVectorY>& t2)	{	return  t1.m_index<=t2.m_index;};
	friend bool operator >= ( const TPointIt<T,TVectorX,TVectorY>& t1, const TPointIt<T,TVectorX,TVectorY>& t2)	{	return  t1.m_index>=t2.m_index;};

protected:
	long m_index;
	const TPointDoubleContainerBase<T,TVectorX,TVectorY>* m_pV;
};

/*! \brief PointDoubleContainer iterator

\param T float or double
\param TVectorX x coordinate container with random access iterator
\param TVectorY y coordinate container with random access iterator

  A random access iterator.

  \ingroup DCGroup
*/
template<typename T, typename TVectorX, typename TVectorY>
class TPointRandIt : public TPointIt<T,TVectorX,TVectorY>
{
public:
	TPointRandIt(const TPointDoubleContainerBase<T,TVectorX,TVectorY>* pV = NULL, UINT index = 0):TPointIt<T,TVectorX,TVectorY>(pV,index)
	{};
	TPointRandIt<T,TVectorX,TVectorY>& operator = (const TPointRandIt<T,TVectorX,TVectorY>& t)	
	{	
		if (&t!=this)
		{
			TPointIt<T,TVectorX,TVectorY>::operator=(t);
		};
		return *this;
	};


	TPoint<T> operator*() const			
	{	
		ASSERT(m_pV); 
		ASSERT(m_index<m_pV->size()); 
		return TPoint<T>((*m_pV->GetXContainer())[m_index], (*m_pV->GetYContainer())[m_index]);
	};

	TPointRef<T> operator*()						
	{	
		ASSERT(m_pV); 
		ASSERT(m_index<m_pV->size());
		return TPointRef<T>( (*m_pV->GetXContainer())[m_index], (*m_pV->GetYContainer())[m_index]);
	};

	TPointRandIt<T,TVectorX,TVectorY>& operator++(int)				{	return TPointRandIt<T,TVectorX,TVectorY>( m_pV, m_index+1); };
	TPointRandIt<T,TVectorX,TVectorY>& operator--(int)				{	return TPointRandIt<T,TVectorX,TVectorY>( m_pV, m_index-1); };
	TPointRandIt<T,TVectorX,TVectorY>& operator++()					{	m_index++; return *this; };
	TPointRandIt<T,TVectorX,TVectorY>& operator--()					{	m_index--; return *this; };
	TPointRandIt<T,TVectorX,TVectorY>& operator+=(UINT i)			{	m_index+=i; return *this; };
	TPointRandIt<T,TVectorX,TVectorY>& operator-=(UINT i)			{	m_index-=i;  return *this; };
	friend TPointRandIt<T,TVectorX,TVectorY> operator + (const TPointRandIt<T,TVectorX,TVectorY>& t, UINT i)	{	return TPointRandIt<T,TVectorX,TVectorY>( t.m_pV, t.m_index+i);};
	friend TPointRandIt<T,TVectorX,TVectorY> operator - (const TPointRandIt<T,TVectorX,TVectorY>& t, UINT i)	{	return TPointRandIt<T,TVectorX,TVectorY>( t.m_pV, t.m_index-i);};

};

/*! \brief PointDoubleContainer const_iterator

\param T float or double
\param TVectorX x coordinate container with random access iterator
\param TVectorY y coordinate container with random access iterator

	A const random access iterator.

  \ingroup DCGroup
*/
template<typename T, typename TVectorX, typename TVectorY>
class TPointConstRandIt : public TPointIt<T,TVectorX,TVectorY>
{
public:
	TPointConstRandIt(const TPointDoubleContainerBase<T,TVectorX,TVectorY>* pV = NULL, UINT index = 0):TPointIt<T,TVectorX,TVectorY>(pV,index){};

	TPointConstRandIt& operator = (const TPointConstRandIt<T,TVectorX,TVectorY>& t)	
	{	
		if (&t!=this)
		{
			TPointIt<T,TVectorX,TVectorY>::operator=(t);
		};
		return *this;
	};

	TPointConstRandIt& operator = (const TPointRandIt<T,TVectorX,TVectorY>& t)	
	{	
		TPointIt<T,TVectorX,TVectorY>::operator=(t);
		return *this;
	};

	TPoint<T> operator*() const							
	{			
		ASSERT(m_pV); 
		ASSERT(m_index<m_pV->size());	
		return TPoint<T>((*m_pV->GetXContainer())[m_index], (*m_pV->GetYContainer())[m_index]);
	};

	TPointConstRandIt<T,TVectorX,TVectorY>& operator++(int)				{	return TPointConstRandIt<T,TVectorX,TVectorY>( t.m_pV, m_index+1); };
	TPointConstRandIt<T,TVectorX,TVectorY>& operator--(int)				{	return TPointConstRandIt<T,TVectorX,TVectorY>( t.m_pV, m_index-1); };
	TPointConstRandIt<T,TVectorX,TVectorY>& operator++()					{	m_index++; return *this; };
	TPointConstRandIt<T,TVectorX,TVectorY>& operator--()					{	m_index--; return *this; };
	TPointConstRandIt<T,TVectorX,TVectorY>& operator+=(UINT i)			{	m_index+=i; return *this; };
	TPointConstRandIt<T,TVectorX,TVectorY>& operator-=(UINT i)			{	m_index-=i;  return *this; };
	friend TPointConstRandIt<T,TVectorX,TVectorY> operator + (const TPointConstRandIt<T,TVectorX,TVectorY>& t, UINT i)	{	return TPointConstRandIt<T,TVectorX,TVectorY>( t.m_pV, t.m_index+i);};
	friend TPointConstRandIt<T,TVectorX,TVectorY> operator - (const TPointConstRandIt<T,TVectorX,TVectorY>& t, UINT i)	{	return TPointConstRandIt<T,TVectorX,TVectorY>( t.m_pV, t.m_index-i);};
};

/*! \brief A container linking two separate containers into a point container

\param T float or double
\param TVectorX x coordinate container with random access iterator
\param TVectorY y coordinate container with random access iterator

  \ingroup DCGroup
*/
template<typename T, typename TVectorX, typename TVectorY>
class TPointDoubleContainer : virtual public TPointDoubleContainerBase<T,TVectorX,TVectorY>
{
public:
	TPointDoubleContainer(TVectorX* pX = NULL, TVectorY* pY =NULL):TPointDoubleContainerBase<T,TVectorX,TVectorY>(pX,pY){};
	virtual~TPointDoubleContainer(){};

	typedef TPointConstRandIt<T,TVectorX,TVectorY> const_iterator;
	typedef TPointRandIt<T,TVectorX,TVectorY> iterator;

	iterator begin()	{	return iterator(this,0); };
	iterator end()		{	return iterator(this,size()); };

	const_iterator begin() const	{	return const_iterator(this,0); };
	const_iterator end() const		{	return const_iterator(this,size()); };
};

/*! \brief A container to export point to two containers

\param T float or double
\param TListX x coordinate container with single direction iterator
\param TListY y coordinate container with single direction iterator
\param TPointContainer The point container

	\ingroup DCGroup
*/
template<typename T, typename TListX, typename TListY, typename TPointContainer>
class TKeyDoubleContainer
{
public:
	typedef TPointContainer::const_iterator value_type;
	typedef TListX container_x;
	typedef TListY container_y;

	TKeyDoubleContainer( container_x* pListX = NULL, container_y* pListY = NULL):m_pListX(pListX), m_pListY(pListY){};

	void SetContainers( container_x* pListX, container_y* pListY)		{	m_pListX=pListX; m_pListY=pListY;};
	container_x* GetXContainer()										{	return m_pListX;};
	container_y* GetYContainer()										{	return m_pListY;};
	const container_x* GetXContainer() const							{	return m_pListX;};
	const container_y* GetYContainer() const							{	return m_pListY;};

	void clear()		{	ASSERT(m_pListX); ASSERT(m_pListY); m_pListX->clear(); m_pListY->clear();};
	size_t size() const	{	ASSERT(m_pListX); ASSERT(m_pListY); return __min( m_pListX->size(), m_pListY->size());};

	void push_back( const value_type& p)	{ ASSERT(m_pListX); ASSERT(m_pListY); m_pListX->push_back((*p).x); m_pListY->push_back((*p).y);};
	void push_front( const value_type& p)	{ ASSERT(m_pListX); ASSERT(m_pListY); m_pListX->push_front((*p).x); m_pListY->push_front((*p).y);};

protected:
	container_x* m_pListX;
	container_y* m_pListY;
};



};

#endif // !defined(AFX_CONTAINERS_H__33EAD324_F802_4BC8_BEC8_39E69C1C21A7__INCLUDED_)
