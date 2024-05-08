#ifndef HEADER_INCLUDED__topologize_H
#define HEADER_INCLUDED__topologize_H
#include "MLB_Interface.h"
#include <list>
///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLine_Topology : public CSG_Tool
{
public:
    CLine_Topology(void);
    virtual ~CLine_Topology(void);
    static bool Topologize(CSG_Shapes *pInLines, CSG_Shapes *pOutPoints, CSG_Shapes *pOutLines, double tolerance);
    virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Topology") );	}
protected:
    virtual bool			On_Execute(void);

};

struct Vertex : TSG_Point
{
    friend bool operator < (const Vertex& v1, const Vertex& v2)
    {
        if (v1.x<v2.x)
            return true;
        else if (v1.x == v2.x)
            return v1.y<v2.y;
        else
            return false;
    }
    void operator = (const TSG_Point& v)
    {
        x = v.x;
        y = v.y;
    }

};
#endif
