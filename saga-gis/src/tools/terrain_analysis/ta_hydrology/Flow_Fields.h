#include "MLB_Interface.h"

#ifndef HEADER_INCLUDED__Flow_Fields_H
#define HEADER_INCLUDED__Flow_Fields_H

class CFlow_Fields :
	public CSG_Tool_Grid
{
public:
	CFlow_Fields();
	~CFlow_Fields();

   
	double Get_Flow(int, int, double [8]);
	bool On_Execute();
private:
	CSG_Grid * DEM;
	CSG_Grid * fields;
	CSG_Grid * UpArea;
	bool m_bStopAtEdge;
};

#endif