
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
//                       AvWiMa2.h                       //
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
#ifndef HEADER_INCLUDED__AvWiMa2_H
#define HEADER_INCLUDED__AvWiMa2_H

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
class CAvWiMa2 : public CSG_Module_Grid
{
public: ////// public members and functions: //////////////

	CAvWiMa2(void);							// constructor
	virtual ~CAvWiMa2(void);				// destructor

	virtual const SG_Char *	Get_MenuPath			(void)	{	return( _TL("R:Filter for Grids") );	}


protected: /// protected members and functions: ///////////

	virtual bool		On_Execute(void);		// always override this function


private: ///// private members and functions: /////////////


};


//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__AvWiMa2_H
