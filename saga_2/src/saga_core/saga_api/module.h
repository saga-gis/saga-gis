
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       module.h                        //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__module_H
#define HEADER_INCLUDED__SAGA_API__module_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum EModule_Type
{
	MODULE_TYPE_Base			= 0,
	MODULE_TYPE_Interactive,
	MODULE_TYPE_Grid,
	MODULE_TYPE_Grid_Interactive
}
TModule_Type;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum EModule_Error
{
	MODULE_ERROR_Unknown		= 0,
	MODULE_ERROR_Calculation
}
TModule_Error;


///////////////////////////////////////////////////////////
//														 //
//						CModule							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CModule
{
	friend class CModule_Interactive_Base;

public:

	CModule(void);
	virtual ~CModule(void);

	virtual void				Destroy						(void);

	virtual TModule_Type		Get_Type					(void)	{	return( MODULE_TYPE_Base );	}

	const char *				Get_Name					(void);
	const char *				Get_Description				(void);
	const char *				Get_Author					(void);
	const char *				Get_Icon					(void)	{	return( NULL );	}

	virtual const char *		Get_MenuPath				(void)	{	return( NULL );	}

	CParameters *				Get_Parameters				(void)	{	return( &Parameters );	}
	CParameters *				Get_Extra_Parameters		(int i)	{	return( i >= 0 && i < m_npParameters ? m_pParameters[i] : NULL );	}
	int							Get_Extra_Parameters_Count	(void)	{	return( m_npParameters );	}

	virtual bool				is_Interactive				(void)	{	return( false );	}
	bool						is_Progress					(void)	{	return( SG_Callback_Process_Get_Okay(false) );	}
	bool						is_Executing				(void)	{	return( m_bExecutes );	}

	void						Set_Translation				(CSG_Translator &Translator);
	void						Set_Managed					(bool bOn = true);
	void						Set_Show_Progress			(bool bOn = true);

	bool						Execute						(void);


protected:

	CParameters					Parameters;

	CSG_History					History_Supplement;


	//-----------------------------------------------------
	void						Set_Name					(const char *String);
	void						Set_Description				(const char *String);
	void						Set_Author					(const char *String);

	//-----------------------------------------------------
	virtual bool				On_Execute					(void)		= 0;

	virtual int					On_Parameter_Changed		(CParameters *pParameters, CParameter *pParameter);


	//-----------------------------------------------------
	bool						Dlg_Parameters				(CParameters *pParameters, const char *Caption);

	CParameters *				Add_Extra_Parameters		(const char *Identifier, const char *Name, const char *Description);
	CParameters *				Get_Extra_Parameters		(const char *Identifier);
	int							Dlg_Extra_Parameters		(const char *Identifier);


	//-----------------------------------------------------
	virtual bool				Process_Get_Okay			(bool bBlink);
	virtual void				Process_Set_Text			(const char *Text);

	virtual bool				Set_Progress				(int Position);
	virtual bool				Set_Progress				(double Position, double Range);

	void						Message_Add					(const char *Text, bool bNewLine = true);
	void						Message_Dlg					(const char *Text, const char *Caption = NULL);

	bool						Error_Set					(TModule_Error Error_ID = MODULE_ERROR_Unknown);
	bool						Error_Set					(const char *Error_Text);


	//-----------------------------------------------------
	bool						DataObject_Add				(CDataObject *pDataObject, bool bUpdate = false);
	bool						DataObject_Update			(CDataObject *pDataObject, bool bShow = false);
	bool						DataObject_Update			(CDataObject *pDataObject, double Parm_1, double Parm_2, bool bShow = false);

	void						DataObject_Update_All		(void);

	bool						DataObject_Get_Colors		(CDataObject *pDataObject, CSG_Colors &Colors);
	bool						DataObject_Set_Colors		(CDataObject *pDataObject, CSG_Colors &Colors);


private:

	bool						m_bExecutes, m_bError_Ignore, m_bManaged, m_bShow_Progress;

	int							m_npParameters;

	CParameters					**m_pParameters;

	CSG_String					m_Author;


	void						_Set_Output_History			(void);

	static int					_On_Parameter_Changed		(CParameter *pParameter);

};


///////////////////////////////////////////////////////////
//														 //
//					CModule_Grid						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CModule_Grid : public CModule
{
public:
	CModule_Grid(void);
	virtual ~CModule_Grid(void);

	virtual TModule_Type		Get_Type				(void)			{	return( MODULE_TYPE_Grid );	}

	CGrid_System *				Get_System				(void)			{	return( Parameters.Get_Grid_System() );	}


protected:

	virtual bool				Set_Progress			(int iRow);
	virtual bool				Set_Progress			(double Position, double Range);
	virtual bool				Set_Progress_NCells		(int iCell);

	//-----------------------------------------------------
	int							Get_NX					(void)						{	return( Get_System()->Get_NX() );				}
	int							Get_NY					(void)						{	return( Get_System()->Get_NY() );				}
	int							Get_NCells				(void)						{	return( Get_System()->Get_NCells() );			}
	double						Get_Cellsize			(void)						{	return( Get_System()->Get_Cellsize() );			}
	double						Get_XMin				(void)						{	return( Get_System()->Get_XMin() );				}
	double						Get_YMin				(void)						{	return( Get_System()->Get_YMin() );				}
	int							Get_xTo					(int Dir, int x = 0)		{	return( Get_System()->Get_xTo(Dir, x) );		}
	int							Get_yTo					(int Dir, int y = 0)		{	return( Get_System()->Get_yTo(Dir, y) );		}
	int							Get_xFrom				(int Dir, int x = 0)		{	return( Get_System()->Get_xFrom(Dir, x) );		}
	int							Get_yFrom				(int Dir, int y = 0)		{	return( Get_System()->Get_yFrom(Dir, y) );		}
	double						Get_Length				(int Dir)					{	return( Get_System()->Get_Length(Dir) );		}
	double						Get_UnitLength			(int Dir)					{	return( Get_System()->Get_UnitLength(Dir) );	}
	bool						is_InGrid				(int x, int y)				{	return(	Get_System()->is_InGrid(x, y) );		}
	bool						is_InGrid				(int x, int y, int Rand)	{	return(	Get_System()->is_InGrid(x, y, Rand) );	}

	//-----------------------------------------------------
	void						Lock_Create				(void);
	void						Lock_Destroy			(void);

	bool						is_Locked				(int x, int y)	{	return( Lock_Get(x, y) != 0 );	}
	char						Lock_Get				(int x, int y)	{	return( m_pLock && x >= 0 && x < Get_System()->Get_NX() && y >= 0 && y < Get_System()->Get_NY() ? m_pLock->asChar(x, y) : 0 );	}

	void						Lock_Set				(int x, int y, char Value = 1)
	{
		if( m_pLock && x >= 0 && x < Get_System()->Get_NX() && y >= 0 && y < Get_System()->Get_NY() )
		{
			m_pLock->Set_Value(x, y, Value);
		}
	}


private:

	CGrid						*m_pLock;

};


///////////////////////////////////////////////////////////
//														 //
//				CModule_Interactive_Base				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum EModule_Interactive_Mode
{
	MODULE_INTERACTIVE_UNDEFINED		= 0,
	MODULE_INTERACTIVE_LDOWN,
	MODULE_INTERACTIVE_LUP,
	MODULE_INTERACTIVE_LDCLICK,
	MODULE_INTERACTIVE_MDOWN,
	MODULE_INTERACTIVE_MUP,
	MODULE_INTERACTIVE_MDCLICK,
	MODULE_INTERACTIVE_RDOWN,
	MODULE_INTERACTIVE_RUP,
	MODULE_INTERACTIVE_RDCLICK,
	MODULE_INTERACTIVE_MOVE,
	MODULE_INTERACTIVE_MOVE_LDOWN,
	MODULE_INTERACTIVE_MOVE_MDOWN,
	MODULE_INTERACTIVE_MOVE_RDOWN
}
TModule_Interactive_Mode;

//---------------------------------------------------------
typedef enum EModule_Interactive_DragMode
{
	MODULE_INTERACTIVE_DRAG_NONE		= 0,
	MODULE_INTERACTIVE_DRAG_LINE,
	MODULE_INTERACTIVE_DRAG_BOX,
	MODULE_INTERACTIVE_DRAG_CIRCLE
}
TModule_Interactive_DragMode;

//---------------------------------------------------------
#define MODULE_INTERACTIVE_KEY_LEFT		0x01
#define MODULE_INTERACTIVE_KEY_MIDDLE	0x02
#define MODULE_INTERACTIVE_KEY_RIGHT	0x04
#define MODULE_INTERACTIVE_KEY_SHIFT	0x08
#define MODULE_INTERACTIVE_KEY_ALT		0x10
#define MODULE_INTERACTIVE_KEY_CTRL		0x20

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CModule_Interactive_Base
{
	friend class CModule_Interactive;
	friend class CModule_Grid_Interactive;

public:
	CModule_Interactive_Base(void);
	virtual ~CModule_Interactive_Base(void);

	bool						Execute_Position		(CSG_Point ptWorld, TModule_Interactive_Mode Mode, int Keys);
	bool						Execute_Keyboard		(int Character, int Keys);
	bool						Execute_Finish			(void);

	int							Get_Drag_Mode			(void)	{	return( m_Drag_Mode );	}


protected:

	virtual bool				On_Execute_Position		(CSG_Point ptWorld, TModule_Interactive_Mode Mode);
	virtual bool				On_Execute_Keyboard		(int Character);
	virtual bool				On_Execute_Finish		(void);

	CSG_Point &					Get_Position			(void)	{	return( m_Point );					}
	double						Get_xPosition			(void)	{	return( m_Point.m_point.x );		}
	double						Get_yPosition			(void)	{	return( m_Point.m_point.y );		}

	CSG_Point &					Get_Position_Last		(void)	{	return( m_Point_Last );				}
	double						Get_xPosition_Last		(void)	{	return( m_Point_Last.m_point.x );	}
	double						Get_yPosition_Last		(void)	{	return( m_Point_Last.m_point.y );	}

	bool						is_Shift				(void)	{	return( (m_Keys & MODULE_INTERACTIVE_KEY_SHIFT) != 0 );	}
	bool						is_Alt					(void)	{	return( (m_Keys & MODULE_INTERACTIVE_KEY_ALT)   != 0 );	}
	bool						is_Ctrl					(void)	{	return( (m_Keys & MODULE_INTERACTIVE_KEY_CTRL)  != 0 );	}

	void						Set_Drag_Mode			(int Drag_Mode);


private:

	int							m_Keys, m_Drag_Mode;

	CSG_Point					m_Point, m_Point_Last;

	CModule						*m_pModule;

};


///////////////////////////////////////////////////////////
//														 //
//					CModule_Interactive					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CModule_Interactive : public CModule_Interactive_Base, public CModule
{
public:
	CModule_Interactive(void);
	virtual ~CModule_Interactive(void);

	virtual TModule_Type		Get_Type				(void)	{	return( MODULE_TYPE_Interactive );	}

	virtual bool				is_Interactive			(void)	{	return( true );	}

};


///////////////////////////////////////////////////////////
//														 //
//				CModule_Grid_Interactive				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CModule_Grid_Interactive : public CModule_Interactive_Base, public CModule_Grid
{
public:
	CModule_Grid_Interactive(void);
	virtual ~CModule_Grid_Interactive(void);

	virtual TModule_Type		Get_Type				(void)	{	return( MODULE_TYPE_Grid_Interactive );	}

	virtual bool				is_Interactive			(void)	{	return( true );	}


protected:

	bool						Get_Grid_Pos			(int &x, int &y);

	int							Get_xGrid				(void);
	int							Get_yGrid				(void);

};


///////////////////////////////////////////////////////////
//														 //
//			Module Library Interface Definitions		 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum EMLB_Info
{
	MLB_INFO_Name	= 0,
	MLB_INFO_Description,
	MLB_INFO_Author,
	MLB_INFO_Version,
	MLB_INFO_Menu_Path,
	MLB_INFO_User,
	MLB_INFO_Count
}
TMLB_Info;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CModule_Library_Interface
{
public:
	CModule_Library_Interface(void);
	virtual ~CModule_Library_Interface(void);

	void						Set_Info				(int ID, const char *Info);
	const char *				Get_Info				(int ID);

	int							Get_Count				(void);
	bool						Add_Module				(CModule *pModule);
	CModule *					Get_Module				(int iModule);

	void						Set_File_Name			(const char *File_Name);
	const char *				Get_File_Name			(void);

	const char *				Get_Translation			(const char *Text);


private:

	const char					*m_Info[MLB_INFO_Count];

	int							m_nModules;

	CModule						**m_Modules;

	CSG_String					m_File_Name;

	CSG_Translator				m_Translator;

};

//---------------------------------------------------------
#define SYMBOL_MLB_Initialize		"MLB_Initialize"
typedef bool						(* PFNC_MLB_Initialize)		(const char *);

//---------------------------------------------------------
#define SYMBOL_MLB_Get_Interface	"MLB_Get_Interface"
typedef CModule_Library_Interface *	(* PFNC_MLB_Get_Interface)	(void);

//---------------------------------------------------------
#define MLB_INTERFACE	CModule_Library_Interface		MLB_Interface;\
\
extern "C" _SAGA_DLL_EXPORT bool						MLB_Initialize		(const char *File_Name)\
{\
	MLB_Interface.Set_File_Name(File_Name);\
\
	int		i	= 0;\
\
	while( MLB_Interface.Add_Module(Create_Module(i++)) );\
\
	for(i=0; i<MLB_INFO_Count; i++)\
	{\
		MLB_Interface.Set_Info(i, Get_Info(i));\
	}\
\
	return( MLB_Interface.Get_Count() > 0 );\
}\
\
extern "C" _SAGA_DLL_EXPORT CModule_Library_Interface *	MLB_Get_Interface   (void)\
{\
	return( &MLB_Interface );\
}\
\
extern "C" _SAGA_DLL_EXPORT const char *				Get_API_Version		(void)\
{\
	return( SAGA_API_VERSION );\
}\

//---------------------------------------------------------
extern CModule_Library_Interface	MLB_Interface;

#define _TL			MLB_Interface.Get_Translation


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__module_H
