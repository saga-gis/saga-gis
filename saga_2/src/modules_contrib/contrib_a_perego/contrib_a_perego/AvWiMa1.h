
///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                        AvWiMa                         //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       AvWiMa1.h                       //
//                                                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                                                       //
//    by Alessandro Perego (Italy)                       //
//                                                       //
//    e-mail:     alper78@alice.it                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
#ifndef HEADER_INCLUDED__AvWiMa1_H
#define HEADER_INCLUDED__AvWiMa1_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CAvWiMa1 : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CAvWiMa1(void);							// constructor
	virtual ~CAvWiMa1(void);				// destructor

	virtual const SG_Char *	Get_MenuPath			(void)	{	return( _TL("R:Filter for Grids") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////


};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__AvWiMa1_H
