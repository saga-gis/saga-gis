// PathHull.h: interface for the CPathHull class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHHULL_H__50C639BA_585B_4272_9AF4_4632128D8938__INCLUDED_)
#define AFX_PATHHULL_H__50C639BA_585B_4272_9AF4_4632128D8938__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LineApproximator.h"

namespace hull
{


#define DP_SGN(a) (a >= 0)

/*! \brief A path
	\ingroup 
*/
template<typename T, typename TPointContainer, typename TKeyContainer>
class TPathHull  
{
public:
	enum EStackOp
	{
		StackPushOp=0,
		StackTopOp=1,
		StackBotOp=2
	};

	typedef std::vector<signed char> OpContainer;
	typedef std::vector<TPointContainer::const_iterator> PCItContainer;

	TPathHull(): m_iHullMax(0)
	{};
	virtual ~TPathHull()
	{};

	void SetMaxSize(int iHullMax);

	int GetHp() const												{	return m_iHp;};
	int GetBot() const												{	return m_iBot;};
	int GetTop() const												{	return m_iTop;};
	const TPointContainer::const_iterator& GetpElt(int i)	const	{	ASSERT(i<m_ppElt.size()); return m_ppElt[i];};
	const TPointContainer::const_iterator& GetpHelt(int i)	const	{	ASSERT(i<m_ppHelt.size()); return m_ppHelt[i];};
	OpContainer& GetOps()											{	return m_pOp;};

	void SetHp(int hp)												{	m_iHp=hp;};

	void UpHp()		{	m_iHp++;};
	void UpTop()	{	m_iTop++;};
	void UpBot()	{	m_iBot++;};
	void DownHp()	{	m_iHp--;};
	void DownTop()	{	m_iTop--;};
	void DownBot()	{	m_iBot--;};

	void SetTopElt(const TPointContainer::const_iterator& p)		{	ASSERT(m_iTop>=0); ASSERT(m_iTop<m_ppElt.size()); m_ppElt[m_iTop]=p;};
	void SetBotElt(const TPointContainer::const_iterator& p)		{	ASSERT(m_iBot>=0); ASSERT(m_iBot<m_ppElt.size()); m_ppElt[m_iBot]=p;};

	void Split(const TPointContainer::const_iterator& e)
	{
		TPointContainer::const_iterator tmpe;
		int tmpo;
		
		ASSERT(m_iHp<m_ppHelt.size());
		ASSERT(m_iHp<m_pOp.size());
		while ((m_iHp >= 0) 
			&& ((tmpo = m_pOp[m_iHp]), 
			((tmpe = m_ppHelt[m_iHp]) != e) || (tmpo != StackPushOp)))
		{
			m_iHp--;
			switch (tmpo)
			{
			case StackPushOp:
				m_iTop--;
				m_iBot++;
				break;
			case StackTopOp:
				ASSERT(m_iTop-1>=0);
				ASSERT(m_iTop+1<m_ppElt.size());
				m_ppElt[++m_iTop] = tmpe;
				break;
			case StackBotOp:
				ASSERT(m_iBot-1>=0);
				ASSERT(m_iBot-1<m_ppElt.size());
				m_ppElt[--m_iBot] = tmpe;
				break;
			}
		}
	}

	void FindExtreme(const TLine<T,TPointContainer,TKeyContainer>::SHomog& line, TPointContainer::const_iterator* e, T& dist)
	{
	int sbase, sbrk, mid,lo, m1, brk, m2, hi;
	T d1, d2;
	
	if ((m_iTop - m_iBot) > 8) 
    {
		lo = m_iBot; hi = m_iTop - 1;
		sbase = SlopeSign(hi, lo, line);
		do
		{
			brk = (lo + hi) / 2;
			if (sbase == (sbrk = SlopeSign(brk, brk+1, line)))
				if (sbase == (SlopeSign(lo, brk+1, line)))
					lo = brk + 1;
				else
					hi = brk;
		}
		while (sbase == sbrk);
		
		m1 = brk;
		while (lo < m1)
		{
			mid = (lo + m1) / 2;
			if (sbase == (SlopeSign(mid, mid+1, line)))
				lo = mid + 1;
			else
				m1 = mid;
		}
		
		m2 = brk;
		while (m2 < hi) 
		{
			mid = (m2 + hi) / 2;
			if (sbase == (SlopeSign(mid, mid+1, line)))
				hi = mid;
			else
				m2 = mid + 1;
		}
		
		ASSERT(lo>=0);
		ASSERT(lo <m_ppElt.size());
		if ((d1 = TLine<T,TPointContainer,TKeyContainer>::DotProduct(*m_ppElt[lo], line)) < 0) 
			d1 = - d1;

		ASSERT(m2>=0);
		ASSERT(m2 <m_ppElt.size());
		if ((d2 = TLine<T,TPointContainer,TKeyContainer>::DotProduct(*m_ppElt[m2], line)) < 0) 
			d2 = - d2;
		
		dist = (d1 > d2 ? (*e = m_ppElt[lo], d1) : (*e = m_ppElt[m2], d2));
    }
	else				/* Few DP_POINTs in hull */
    {
		dist = 0.0;
		for (mid = m_iBot; mid < m_iTop; mid++)
		{
			
			ASSERT(mid>=0);
			ASSERT(mid<m_ppElt->size());
			if ((d1 = TLine<T,TPointContainer,TKeyContainer>::::DotProduct(*m_ppElt[mid], line)) < 0) 
				d1 = - d1;
			if (d1 > *dist)
			{
				dist = d1;
				*e = m_ppElt[mid];
			}
		}
    }
}

	void Init(const TPointContainer::const_iterator& e1, const TPointContainer::const_iterator& e2)
	{
		/* Initialize path hull and history  */
		ASSERT(m_iHullMax>=0);
		ASSERT(m_iHullMax+1<m_ppElt.size());
		m_ppElt[m_iHullMax] = e1;
		m_ppElt[m_iTop = m_iHullMax + 1] = 
			m_ppElt[m_iBot = m_iHullMax - 1] = 
			m_ppHelt[m_iHp = 0] = e2;
		m_pOp[0] = StackPushOp;
	}

	void Push(const TPointContainer::const_iterator& e)
	{
		ASSERT(m_iTop+1 >= 0);
		ASSERT(m_iTop+1 < m_ppElt.size());
		ASSERT(m_iBot-1 >= 0);
		ASSERT(m_iBot-1 < m_ppElt.size());
		ASSERT(m_iHp+1 >= 0);
		ASSERT(m_iHp+1 < m_ppHelt.size());
		ASSERT(m_iHp+1 < m_pOp.size());

		/* Push element $e$ onto path hull $h$ */
		m_ppElt[++m_iTop] = m_ppElt[--m_iBot] = m_ppHelt[++m_iHp] = e;
		m_pOp[m_iHp] = StackPushOp;
	}

	void PopTop()
	{	
		ASSERT(m_iTop >= 0);
		ASSERT(m_iTop < m_ppElt.size());
		ASSERT(m_iHp+1 >= 0);
		ASSERT(m_iHp+1 < m_ppHelt.size());
		ASSERT(m_iHp+1 < m_pOp.size());

		m_ppHelt[++m_iHp] = m_ppElt[m_iTop--];
		m_pOp[m_iHp] = StackTopOp;
	}

	void PopBot()
	{
		ASSERT(m_iBot >= 0);
		ASSERT(m_iBot < m_ppElt.size());
		ASSERT(m_iHp+1 >= 0);
		ASSERT(m_iHp+1 < m_ppHelt.size());
		ASSERT(m_iHp+1 < m_pOp.size());

		/* Pop from bottom */
		m_ppHelt[++m_iHp] = m_ppElt[m_iBot++];
		m_pOp[m_iHp] = StackBotOp;
	}
	
	void Add(const TPointContainer::const_iterator& p)
	{
		int topflag, botflag;
		
		topflag = LeftOfTop(p);
		botflag = LeftOfBot(p);
		
		if (topflag || botflag)
		{
			while (topflag)
			{
				PopTop();
				topflag = LeftOfTop(p);
			}
			while (botflag)
			{
				PopBot();
				botflag = LeftOfBot(p);
			}
			Push(p);
		}
	}

	int LeftOfTop(const TPointContainer::const_iterator& c)
	{
		ASSERT(m_iTop >= 1);
		ASSERT(m_iTop < m_ppElt.size());

		/* Determine if point c is left of line a to b */
		return (((*m_ppElt[m_iTop]).x - (*c).x)*((*m_ppElt[m_iTop-1]).y - (*c).y) 
			>= ((*m_ppElt[m_iTop-1]).x - (*c).x)*((*m_ppElt[m_iTop]).y - (*c).y));
	}

	int LeftOfBot(const TPointContainer::const_iterator& c)
	{
		ASSERT(m_iBot >= 0);
		ASSERT(m_iBot+1 < m_ppElt.size());

		/* Determine if point c is left of line a to b */
		return (((*m_ppElt[m_iBot+1]).x - (*c).x)*((*m_ppElt[m_iBot]).y - (*c).y) 
			>= ((*m_ppElt[m_iBot]).x - (*c).x)*((*m_ppElt[m_iBot+1]).y - (*c).y));
	}


	int SlopeSign(int p, int q, const TLine<T,TPointContainer,TKeyContainer>::SHomog& l)
	{
		ASSERT(p >= 0);
		ASSERT(p < m_ppElt.size());
		ASSERT(q >= 0);
		ASSERT(q < m_ppElt.size());

		/* Return the sign of the projection 
				   of $h[q] - h[p]$ onto the normal 
				   to line $l$ */ 
		return (int) (DP_SGN( 
			(l.x)*((*m_ppElt[q]).x - (*m_ppElt[p]).x) 
			+ (l.y)*((*m_ppElt[q]).y - (*m_ppElt[p]).y) ) ) ;
	};

protected:
	/// Maxium number of elements in hull
	int m_iHullMax;
	
	/// internal values
	int m_iTop;
	int m_iBot; 
	int m_iHp;
	OpContainer m_pOp;	
	PCItContainer m_ppElt; 
	PCItContainer m_ppHelt;
};

template <typename T,typename TPointContainer,typename TKeyContainer>
void TPathHull<T,TPointContainer,TKeyContainer>::SetMaxSize(int iHullMax)
{
	if (m_iHullMax == iHullMax)
		return;

	m_iHullMax=iHullMax;
	
	m_pOp.resize(3*m_iHullMax);
	m_ppElt.resize(2*m_iHullMax);
	m_ppHelt.resize(3*m_iHullMax);
}


};

#endif // !defined(AFX_PATHHULL_H__50C639BA_585B_4272_9AF4_4632128D8938__INCLUDED_)
