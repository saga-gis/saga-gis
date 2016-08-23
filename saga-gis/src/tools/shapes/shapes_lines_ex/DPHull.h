/**********************************************************
 * Version $Id$
 *********************************************************/

#if !defined(AFX_DPHULL1_H__6CE88E63_3AC7_4E18_87FB_CACF5BE62BE4__INCLUDED_)
#define AFX_DPHULL1_H__6CE88E63_3AC7_4E18_87FB_CACF5BE62BE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef UINT
	#define UINT unsigned int
#endif

#include <tchar.h>
#include <vector>
#include <stack>
#include "PathHull.h"
#include "DPHull.h"
#include "Containers.h"

namespace hull
{

/*! \brief Douglas-Peukler Appromixation algorithm

\ingroup LAGroup
*/
template <typename T, typename TPointContainer, typename TKeyContainer>
class TDPHull : public TLineApproximator<T,TPointContainer,TKeyContainer>
{
public:
	//! Build step types
	enum EBuildStep
	{
		//! Output to vertex
		BuildStepOutputVertex,
		//! Call DP
		BuildStepDP,
		//! Call Build
		BuildStepBuild,
		//! Is a return key
		BuildStepReturnKey
	};
	//! A build step structure
	struct SBuildStep
	{
		SBuildStep(TPointContainer::const_iterator _i,TPointContainer::const_iterator _j, EBuildStep _s):i(_i),j(_j),s(_s){};

		TPointContainer::const_iterator i;
		TPointContainer::const_iterator j;
		EBuildStep s;
	};

public:
	//! \name Constructors
	//@{
	//! Default constructor
	explicit TDPHull(){};
	//! Destructor
	virtual ~TDPHull(){};
	//@}

protected:
	//! A path hull
	typedef TPathHull<T,TPointContainer,TKeyContainer> PathHull;
	typedef std::stack< SBuildStep > BuildStack;

	//! Computes the keys
	virtual void ComputeKeys();

	//! \name Hull methods
	//@{
	/*! \brief Adds a function call to the stack

	\param i left point iterator
	\param j right point iterator
	\param buildStep step description
	*/
	void AddBuildStep( TPointContainer::const_iterator i, TPointContainer::const_iterator j, EBuildStep buildStep) 
	{	m_stack.push( BuildStack::value_type(i,j,buildStep) );	}
	/*! \brief Apply Douglas-Peucker algo

	\pre m_stack not empty
	\pre m_stack.top().s == BuildStepDP
	*/
	void DP();

	/*! \brief Builds the path hull

	\pre m_stack not empty
	\pre m_stack.top().s == BuildStepBuild
	*/
	void Build();

	/*! \brief Stores key

	\pre m_stack size = 2
	\pre m_stack.top().s == BuildStepReturnKey
	\pre m_stack.top()(twice).s == BuildStepOutputVertex
	*/
	void OutputVertex() 
	{ 
		ASSERT(!m_stack.empty());
		ASSERT(m_stack.top().s==BuildStepReturnKey);

		GetKeys().push_back(m_stack.top().i); 
		m_stack.pop();
		ASSERT(!m_stack.empty());
		ASSERT(m_stack.top().s==BuildStepOutputVertex);
		m_stack.pop();
	};
	//@}

protected:
	//! \name Attributes
	//@{
	TPathHull<T,TPointContainer,TKeyContainer> m_phRight;
	TPathHull<T,TPointContainer,TKeyContainer> m_phLeft;
	TPointContainer::const_iterator m_pPHtag;

	BuildStack m_stack;
	//@}
};

template<typename T, typename TPointContainer, typename TKeyContainer>
void TDPHull<T,TPointContainer,TKeyContainer>::DP()
	{
		T ld, rd, len_sq;
		SHomog l;
		register TPointContainer::const_iterator le;
		register TPointContainer::const_iterator re;
		
		ASSERT( !m_stack.empty() );
		ASSERT( m_stack.top().s == BuildStepDP );
		TPointContainer::const_iterator i(m_stack.top().i);
		TPointContainer::const_iterator j(m_stack.top().j);
		m_stack.pop();

		CrossProduct(i, j, l);
		len_sq = l.x * l.x + l.y * l.y;
		
		if (j - i < 8)
		{		/* chain small */
			rd  = 0.0;
			for (le = i + 1; le < j; ++le)
			{
				ld = DotProduct(le, l);
				if (ld < 0) ld = - ld;
				if (ld > rd) 
				{
					rd = ld;
					re = le;
				}
			}
			if (rd * rd > GetTol() * len_sq)
			{
				AddBuildStep( re, j, BuildStepDP );
				AddBuildStep( i, re, BuildStepOutputVertex );
				AddBuildStep( i, re, BuildStepDP );
				return;
//				OutputVertex(DP(i, re)); 
//				return(DP(re, j));
			}
			else
			{
				AddBuildStep(j,j,BuildStepReturnKey);
				return;
//				return j;
			}
		}
		else
		{	
			/* chain large */
			int sbase, sbrk, mid, lo, m1, brk, m2, hi;
			T d1, d2;
			if ((m_phLeft.GetTop() - m_phLeft.GetBot()) > 8) 
			{
				/* left hull large */
				lo = m_phLeft.GetBot(); 
				hi = m_phLeft.GetTop() - 1;
				sbase = m_phLeft.SlopeSign(hi, lo, l);
				do
				{
					brk = (lo + hi) / 2;
					if (sbase == (sbrk = m_phLeft.SlopeSign(brk, brk+1, l)))
						if (sbase == (m_phLeft.SlopeSign(lo, brk+1, l)))
							lo = brk + 1;
						else
							hi = brk;
				}
				while (sbase == sbrk && lo < hi);
				
				m1 = brk;
				while (lo < m1)
				{
					mid = (lo + m1) / 2;
					if (sbase == (m_phLeft.SlopeSign(mid, mid+1, l)))
						lo = mid + 1;
					else
						m1 = mid;
				}
				
				m2 = brk;
				while (m2 < hi) 
				{
					mid = (m2 + hi) / 2;
					if (sbase == (m_phLeft.SlopeSign(mid, mid+1, l)))
						hi = mid;
					else
						m2 = mid + 1;
				};
				
				
				if ((d1 = DotProduct(m_phLeft.GetpElt(lo), l)) < 0) d1 = - d1;
				if ((d2 = DotProduct(m_phLeft.GetpElt(m2), l)) < 0) d2 = - d2;
				ld = (d1 > d2 ? (le = m_phLeft.GetpElt(lo), d1) : (le = m_phLeft.GetpElt(m2), d2));
			}
			else
			{			/* Few SPoints in left hull */
				ld = 0.0;
				for (mid = m_phLeft.GetBot(); mid < m_phLeft.GetTop(); mid++)
				{
					if ((d1 = DotProduct(m_phLeft.GetpElt(mid), l)) < 0) d1 = - d1;
					if (d1 > ld)
					{
						ld = d1;
						le = m_phLeft.GetpElt(mid);
					}
				}
			}
			
			if ((m_phRight.GetTop() - m_phRight.GetBot()) > 8)
			{			/* right hull large */
				lo = m_phRight.GetBot(); hi = m_phRight.GetTop() - 1;
				sbase = m_phRight.SlopeSign(hi, lo, l);
				do
				{
					brk = (lo + hi) / 2;
					if (sbase == (sbrk = m_phRight.SlopeSign(brk, brk+1, l)))
						if (sbase == (m_phRight.SlopeSign(lo, brk+1, l)))
							lo = brk + 1;
						else
							hi = brk;
				}
				while (sbase == sbrk && lo < hi);
				
				m1 = brk;
				while (lo < m1)
				{
					mid = (lo + m1) / 2;
					if (sbase == (m_phRight.SlopeSign(mid, mid+1, l)))
						lo = mid + 1;
					else
						m1 = mid;
				}
				
				m2 = brk;
				while (m2 < hi) 
				{
					mid = (m2 + hi) / 2;
					if (sbase == (m_phRight.SlopeSign(mid, mid+1, l)))
						hi = mid;
					else
						m2 = mid + 1;
				};
				
				if ((d1 = DotProduct(m_phRight.GetpElt(lo), l)) < 0) d1 = - d1;
				if ((d2 = DotProduct(m_phRight.GetpElt(m2), l)) < 0) d2 = - d2;
				rd = (d1 > d2 ? (re = m_phRight.GetpElt(lo), d1) : (re = m_phRight.GetpElt(m2), d2));
			}
			else
			{			/* Few SPoints in righthull */
				rd = 0.0;
				for (mid = m_phRight.GetBot(); mid < m_phRight.GetTop(); mid++)
				{
					if ((d1 = DotProduct(m_phRight.GetpElt(mid), l)) < 0) d1 = - d1;
					if (d1 > rd)
					{
						rd = d1;
						re = m_phRight.GetpElt(mid);
					}
				}
			}
    }
	
	
	if (ld > rd)
		if (ld * ld > GetTol() * len_sq)
		{
			/* split left */
			register int tmpo; 
			
			while ((m_phLeft.GetHp() >= 0) 
				&& ( (tmpo = m_phLeft.GetOps()[m_phLeft.GetHp()] ), 
				((re = m_phLeft.GetpHelt(m_phLeft.GetHp())) != le) || (tmpo != PathHull::StackPushOp)))
			{
				m_phLeft.DownHp();
				switch (tmpo)
				{
				case PathHull::StackPushOp:
					m_phLeft.DownTop();
					m_phLeft.UpBot();
					break;
				case PathHull::StackTopOp:
					m_phLeft.UpTop();
					m_phLeft.SetTopElt(re);
					break;
				case PathHull::StackBotOp:
					m_phLeft.DownBot();
					m_phLeft.SetBotElt(re);
					break;
				}
			}
			
			AddBuildStep( le, j, BuildStepDP );
			AddBuildStep( le, j, BuildStepBuild);
			AddBuildStep( i, le, BuildStepOutputVertex);
			AddBuildStep( i, le, BuildStepDP);
			AddBuildStep( i, le, BuildStepBuild);
			return;
//			Build(i, le);
//			OutputVertex(DP(i, le));
//			Build(le, j);
//			return DP(le, j);
		}
		else
		{
			AddBuildStep(j,j,BuildStepReturnKey);
			return;
//			return(j);
		}
		else				/* extreme on right */
			if (rd * rd > GetTol() * len_sq)
			{				/* split right or both */
//				if (m_pPHtag == re)
//				{
//					AddBuildStep( i, re, BuildStepBuild);
//					Build(i, re);
//				}
//				else
				if (m_pPHtag != re)
				{	
					/* split right */
					register int tmpo;
					
					while ((m_phRight.GetHp() >= 0) 
						&& ((tmpo = m_phRight.GetOps()[m_phRight.GetHp()]), 
						((le = m_phRight.GetpHelt(m_phRight.GetHp())) != re) || (tmpo != PathHull::StackPushOp)))
					{
						m_phRight.DownHp();
						switch (tmpo)
						{
						case PathHull::StackPushOp:
							m_phRight.DownTop();
							m_phRight.UpBot();
							break;
						case PathHull::StackTopOp:
							m_phRight.UpTop();
							m_phRight.SetTopElt(le);
							break;
						case PathHull::StackBotOp:
							m_phRight.DownBot();
							m_phRight.SetBotElt(le);
							break;
						}
					}
				}

				AddBuildStep( re, j ,BuildStepDP );
				AddBuildStep( re, j, BuildStepBuild );
				AddBuildStep( i, re, BuildStepOutputVertex );
				AddBuildStep( i, re, BuildStepDP );
				if (m_pPHtag == re)
					AddBuildStep( i, re, BuildStepBuild);

				return;

//				OutputVertex(DP(i, re));
//				Build(re, j);
//				return(DP(re, j));
			}
			else
				AddBuildStep( j,j, BuildStepReturnKey);
//				return(j);	
	}

template<typename T, typename TPointContainer, typename TKeyContainer>
void TDPHull<T,TPointContainer,TKeyContainer>::Build()
{
		TPointContainer::const_iterator k;
		int topflag, botflag;
		
		ASSERT( !m_stack.empty() );
		ASSERT( m_stack.top().s == BuildStepBuild );
		TPointContainer::const_iterator i(m_stack.top().i);
		TPointContainer::const_iterator j(m_stack.top().j);
		m_stack.pop();

		m_pPHtag = i + (j - i) / 2;	/* Assign tag vertex */
		
		
		m_phLeft.Init(m_pPHtag, m_pPHtag - 1); /* \va{left} hull */
		for (k = m_pPHtag - 2; k >= i; --k)
		{
			topflag = m_phLeft.LeftOfTop(k);
			botflag = m_phLeft.LeftOfBot(k);
			if ((topflag || botflag) && !(topflag && botflag))
			{
				while (topflag)
				{
					m_phLeft.PopTop();
					topflag = m_phLeft.LeftOfTop(k);
				}
				while (botflag)
				{
					m_phLeft.PopBot();
					botflag = m_phLeft.LeftOfBot(k);
				}
				m_phLeft.Push(k);
			}
		}
		
		m_phRight.Init(m_pPHtag, m_pPHtag + 1); /* \va{right} hull */
		for (k = m_pPHtag + 2; k <= j; ++k)
		{
			topflag = m_phRight.LeftOfTop(k);
			botflag = m_phRight.LeftOfBot(k);
			if ((topflag || botflag) && !(topflag && botflag))
			{
				while (topflag)
				{
					m_phRight.PopTop();
					topflag = m_phRight.LeftOfTop(k);
				}
				while (botflag)
				{
					m_phRight.PopBot();
					botflag = m_phRight.LeftOfBot(k);
				}
				m_phRight.Push(k);
			}
		}
	};


template<typename T, typename TPointContainer, typename TKeyContainer>
void TDPHull<T,TPointContainer,TKeyContainer>::ComputeKeys()
{
	using namespace std;
	static const T epsilon2 = numeric_limits<T>::epsilon()*numeric_limits<T>::epsilon();
	TLineApproximator<T,TPointContainer,TKeyContainer>::ComputeKeys();
	const TPointContainer& pc=GetPoints();
	TPointContainer::const_iterator pcend=pc.end();
	--pcend;
	T len_sq;
	SHomog l;

	/////////////////////////////////////////////////////////////////////////////
	CrossProduct(pc.begin(), pcend, l);

	len_sq = l.x * l.x + l.y * l.y;
	if (len_sq < epsilon2)
		throw _T("TDPHull<T,TKeyExporter>::DP failed: Start and end points are equal or at a distance < epsilon\n");

	////////////////////////////////////////////////////////////////////////
	// prepraring path if needed...
	m_phLeft.SetMaxSize(pc.size()+1);
	m_phRight.SetMaxSize(pc.size()+1);

	/////////////////////////////////////////////////////////////////////////
	// building hull
//	Build(pc.begin(), pcend);	/* Build the initial path hull */	
//	OutputVertex( pc.begin() );
//	OutputVertex( DP(pc.begin(), pcend) ); /* Simplify */

	AddBuildStep( pc.begin(), pcend, BuildStepBuild );
	Build();
	AddBuildStep( pc.begin(), pc.begin(), BuildStepOutputVertex );
	AddBuildStep( pc.begin(), pc.begin(), BuildStepReturnKey );
	OutputVertex();

	AddBuildStep( pc.begin(), pc.begin(), BuildStepOutputVertex );
	AddBuildStep( pc.begin(), pcend, BuildStepDP );
	while (!m_stack.empty())
	{
//		std::cerr<<"stack size: "<<m_stack.size()<<std::endl;
		switch( m_stack.top().s)
		{
		case BuildStepDP:
			DP();
			break;
		case BuildStepBuild:
			Build();
			break;
		case BuildStepReturnKey:
			OutputVertex();	
			break;
		case BuildStepOutputVertex:
			ASSERT(false);
			break;
		default:
			ASSERT(false);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// cleaning path...
	m_phLeft.SetMaxSize(0);
	m_phRight.SetMaxSize(0);
};

/*! \brief A single precision DPHull

 The classical DPHull object:
	- PointContainer: vector<TPoint<float>>
	- KeyContainer: list<PointContainer::const_iterator>

  \ingroup LAGroup
*/
typedef TDPHull<float, std::vector< TPoint<float> >, std::vector< std::vector< TPoint< float > >::const_iterator > > CDPHullF;

/*! \brief A double precision DPHull

 The classical DPHull object:
	- PointContainer: vector<TPoint<double>>
	- KeyContainer: list<PointContainer::const_iterator>

  \ingroup LAGroup
*/
typedef TDPHull<double, std::vector< TPoint< double > >, std::vector< std::vector< TPoint< double  > >::const_iterator > > CDPHullD;

};

#endif // !defined(AFX_DPHULL1_H__6CE88E63_3AC7_4E18_87FB_CACF5BE62BE4__INCLUDED_)
