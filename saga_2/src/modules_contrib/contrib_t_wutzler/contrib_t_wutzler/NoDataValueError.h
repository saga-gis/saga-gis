
#if !defined(AFX_NODATAVALUEERROR_H__F8600AD2_4EE3_4B0C_B30A_B4F2D9F3C127__INCLUDED_)
#define AFX_NODATAVALUEERROR_H__F8600AD2_4EE3_4B0C_B30A_B4F2D9F3C127__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCalcError.h"

class CNoDataValueError : public CGridCalcError  
{
public:
	CNoDataValueError(const std::string& what ) : CGridCalcError(what){}

};

#endif // !defined(AFX_NODATAVALUEERROR_H__F8600AD2_4EE3_4B0C_B30A_B4F2D9F3C127__INCLUDED_)
