/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    FillSinks_WL.h                     //
//                                                       //
//                 Copyright (C) 2007 by                 //
//                    Volker Wichmann                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     reklovw@web.de					     //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__FillSinks_WL_H
#define HEADER_INCLUDED__FillSinks_WL_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CFillSinks_WL_Node
{
public:
	CFillSinks_WL_Node() : spill( 0 ) {}
		~CFillSinks_WL_Node(){}

	struct Greater : public binary_function< CFillSinks_WL_Node, CFillSinks_WL_Node, bool >
	{
		bool operator()(const CFillSinks_WL_Node n1, const CFillSinks_WL_Node n2) const
		{
			return n1.spill > n2.spill;
		}
	};

	int		x;
	int		y;
	double	spill;
};



//---------------------------------------------------------
class ta_preprocessor_EXPORT CFillSinks_WL : public CSG_Module_Grid
{
public:
	CFillSinks_WL(void);
	virtual ~CFillSinks_WL(void);


protected:

	virtual bool		On_Execute(void);


private:

	typedef				vector< CFillSinks_WL_Node > nodeVector;
	typedef				priority_queue< CFillSinks_WL_Node, nodeVector, CFillSinks_WL_Node::Greater > PriorityQ;

	CSG_Grid			*pFilled;

	int					Get_Dir(int x, int y, double z);

};

//---------------------------------------------------------
class ta_preprocessor_EXPORT CFillSinks_WL_XXL : public CSG_Module_Grid
{
public:
	CFillSinks_WL_XXL(void);
	virtual ~CFillSinks_WL_XXL(void);


protected:

	virtual bool		On_Execute(void);


private:

	typedef		vector< CFillSinks_WL_Node > nodeVector;
	typedef		priority_queue< CFillSinks_WL_Node, nodeVector, CFillSinks_WL_Node::Greater > PriorityQ;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__FillSinks_WL_H
