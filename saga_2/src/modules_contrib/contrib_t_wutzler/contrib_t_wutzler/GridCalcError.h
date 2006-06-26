
#if !defined(AFX_GRIDCALCERROR_H__29182901_9324_4E00_8EB1_DA017C6F5401__INCLUDED_)
#define AFX_GRIDCALCERROR_H__29182901_9324_4E00_8EB1_DA017C6F5401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdexcept>

class CGridCalcError : public std::runtime_error
{
public:
	CGridCalcError(const std::string& what ) : std::runtime_error(what){}
};

#endif // !defined(AFX_GRIDCALCERROR_H__29182901_9324_4E00_8EB1_DA017C6F5401__INCLUDED_)
