/**********************************************************
 * Version $Id$
 *********************************************************/

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
typedef enum ESG_Module_Type
{
	MODULE_TYPE_Base			= 0,
	MODULE_TYPE_Interactive,
	MODULE_TYPE_Grid,
	MODULE_TYPE_Grid_Interactive
}
TSG_Module_Type;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Module_Error
{
	MODULE_ERROR_Unknown		= 0,
	MODULE_ERROR_Calculation
}
TSG_Module_Error;


///////////////////////////////////////////////////////////
//														 //
//						CSG_Module						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Module is the base class for all executable SAGA modules.
  * @see CSG_Parameters
  * @see CSG_Module_Interactive
  * @see CSG_Module_Grid
  * @see CSG_Module_Grid_Interactive
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module
{
	friend class CSG_Module_Interactive_Base;

public:

	CSG_Module(void);
	virtual ~CSG_Module(void);

	virtual void				Destroy						(void);

	virtual TSG_Module_Type		Get_Type					(void)	{	return( MODULE_TYPE_Base );	}

	const SG_Char *				Get_Name					(void);
	const SG_Char *				Get_Description				(void);
	const SG_Char *				Get_Author					(void);
	const SG_Char *				Get_Icon					(void)	{	return( NULL );	}

	virtual const SG_Char *		Get_MenuPath				(void)	{	return( NULL );	}

	int							Get_Parameters_Count		(void)	{	return( m_npParameters );	}
	CSG_Parameters *			Get_Parameters				(void)	{	return( &Parameters );	}
	CSG_Parameters *			Get_Parameters				(int i)	{	return( i >= 0 && i < m_npParameters ? m_pParameters[i] : NULL );	}
	CSG_Parameters *			Get_Parameters				(const SG_Char *Identifier);
#ifdef _SAGA_UNICODE
	CSG_Parameters *			Get_Parameters				(const char    *Identifier);
#endif

	int							Garbage_Get_Count			(void)	{	return( m_nGarbage );		}
	CSG_Data_Object *			Garbage_Get_Item			(int i)	{	return( i >= 0 && i < m_nGarbage ? m_Garbage[i] : NULL );	}
	CSG_Data_Object *			Garbage_Del_Item			(int i, bool bFromListOnly = true);
	void						Garbage_Clear				(void);

	virtual bool				do_Sync_Projections			(void)	{	return( true  );	}

	virtual bool				is_Interactive				(void)	{	return( false );	}
	bool						is_Progress					(void)	{	return( SG_UI_Process_Get_Okay(false) );	}
	bool						is_Executing				(void)	{	return( m_bExecutes );	}

	void						Set_Translation				(CSG_Translator &Translator);
	void						Set_Managed					(bool bOn = true);
	void						Set_Show_Progress			(bool bOn = true);

	virtual bool				On_Before_Execution			(void)	{	return( true );	}
	virtual bool				On_After_Execution			(void)	{	return( true );	}

	bool						Execute						(void);


protected:

	CSG_Parameters				Parameters;

	CSG_MetaData				History_Supplement;


	//-----------------------------------------------------
	void						Set_Name					(const SG_Char *String);
	void						Set_Description				(const SG_Char *String);
	void						Set_Author					(const SG_Char *String);

	//-----------------------------------------------------
	virtual bool				On_Execute					(void)	= 0;

	virtual int					On_Parameter_Changed		(CSG_Parameters *pParameters, CSG_Parameter *pParameter);


	//-----------------------------------------------------
	CSG_Parameters *			Add_Parameters				(const SG_Char *Identifier, const SG_Char *Name, const SG_Char *Description);
	bool						Dlg_Parameters				(const SG_Char *Identifier);
	bool						Dlg_Parameters				(CSG_Parameters *pParameters, const SG_Char *Caption);
#ifdef _SAGA_UNICODE
	CSG_Parameters *			Add_Parameters				(const char    *Identifier, const SG_Char *Name, const SG_Char *Description);
	bool						Dlg_Parameters				(const char    *Identifier);
#endif


	//-----------------------------------------------------
	virtual bool				Process_Get_Okay			(bool bBlink = false);
	virtual void				Process_Set_Text			(const SG_Char *Text);

	virtual bool				Set_Progress				(int Position);
	virtual bool				Set_Progress				(double Position, double Range);

	bool						Stop_Execution				(bool bDialog = true);

	void						Message_Add					(const SG_Char *Text, bool bNewLine = true);
	void						Message_Dlg					(const SG_Char *Text, const SG_Char *Caption = NULL);
	bool						Message_Dlg_Confirm			(const SG_Char *Text, const SG_Char *Caption = NULL);

	bool						Error_Set					(TSG_Module_Error Error_ID = MODULE_ERROR_Unknown);
	bool						Error_Set					(const SG_Char *Error_Text);


	//-----------------------------------------------------
	bool						DataObject_Add				(CSG_Data_Object *pDataObject, bool bUpdate = false);
	bool						DataObject_Update			(CSG_Data_Object *pDataObject, int Show = SG_UI_DATAOBJECT_UPDATE_ONLY);
	bool						DataObject_Update			(CSG_Data_Object *pDataObject, double Parm_1, double Parm_2, int Show = SG_UI_DATAOBJECT_UPDATE_ONLY);

	void						DataObject_Update_All		(void);

	bool						DataObject_Get_Colors		(CSG_Data_Object *pDataObject, CSG_Colors &Colors);
	bool						DataObject_Set_Colors		(CSG_Data_Object *pDataObject, const CSG_Colors &Colors);
	bool						DataObject_Set_Colors		(CSG_Data_Object *pDataObject, int nColors, int Palette = SG_COLORS_DEFAULT, bool bRevert = false);

	bool						DataObject_Get_Parameters	(CSG_Data_Object *pDataObject, CSG_Parameters &Parameters);
	bool						DataObject_Set_Parameters	(CSG_Data_Object *pDataObject, CSG_Parameters &Parameters);

	CSG_Parameter *				DataObject_Get_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID);
	bool						DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, CSG_Parameter *pParameter);
	bool						DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, int            Value);
	bool						DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, double         Value);
	bool						DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, void          *Value);
	bool						DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, const SG_Char *Value);
	bool						DataObject_Set_Parameter	(CSG_Data_Object *pDataObject, const CSG_String &ID, double loVal, double hiVal);	// Range Parameter

	bool						Get_Projection				(CSG_Projection &Projection)	const;


private:

	bool						m_bExecutes, m_bError_Ignore, m_bManaged, m_bShow_Progress;

	int							m_npParameters, m_nGarbage;

	CSG_Data_Object				**m_Garbage;

	CSG_Parameters				**m_pParameters;

	CSG_String					m_Author;


	bool						_Garbage_Add_Item			(CSG_Data_Object *pDataObject);

	bool						_Synchronize_DataObjects	(void);

	void						_Set_Output_History			(void);

	static int					_On_Parameter_Changed		(CSG_Parameter *pParameter);

};


///////////////////////////////////////////////////////////
//														 //
//					CSG_Module_Grid						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Module_Grid.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Grid : public CSG_Module
{
public:
	CSG_Module_Grid(void);
	virtual ~CSG_Module_Grid(void);

	virtual TSG_Module_Type		Get_Type				(void)			{	return( MODULE_TYPE_Grid );	}

	CSG_Grid_System *			Get_System				(void)			{	return( Parameters.Get_Grid_System() );	}


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

	CSG_Grid					*m_pLock;

};


///////////////////////////////////////////////////////////
//														 //
//				CSG_Module_Interactive_Base				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Module_Interactive_Mode
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
TSG_Module_Interactive_Mode;

//---------------------------------------------------------
typedef enum ESG_Module_Interactive_DragMode
{
	MODULE_INTERACTIVE_DRAG_NONE		= 0,
	MODULE_INTERACTIVE_DRAG_LINE,
	MODULE_INTERACTIVE_DRAG_BOX,
	MODULE_INTERACTIVE_DRAG_CIRCLE
}
TSG_Module_Interactive_DragMode;

//---------------------------------------------------------
#define MODULE_INTERACTIVE_KEY_LEFT		0x01
#define MODULE_INTERACTIVE_KEY_MIDDLE	0x02
#define MODULE_INTERACTIVE_KEY_RIGHT	0x04
#define MODULE_INTERACTIVE_KEY_SHIFT	0x08
#define MODULE_INTERACTIVE_KEY_ALT		0x10
#define MODULE_INTERACTIVE_KEY_CTRL		0x20

//---------------------------------------------------------
/**
  * CSG_Module_Interactive_Base.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Interactive_Base
{
	friend class CSG_Module_Interactive;
	friend class CSG_Module_Grid_Interactive;

public:
	CSG_Module_Interactive_Base(void);
	virtual ~CSG_Module_Interactive_Base(void);

	bool						Execute_Position		(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode, int Keys);
	bool						Execute_Keyboard		(int Character, int Keys);
	bool						Execute_Finish			(void);

	int							Get_Drag_Mode			(void)	{	return( m_Drag_Mode );	}


protected:

	virtual bool				On_Execute_Position		(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);
	virtual bool				On_Execute_Keyboard		(int Character);
	virtual bool				On_Execute_Finish		(void);

	CSG_Point &					Get_Position			(void)	{	return( m_Point );				}
	double						Get_xPosition			(void)	{	return( m_Point.Get_X() );		}
	double						Get_yPosition			(void)	{	return( m_Point.Get_Y() );		}

	CSG_Point &					Get_Position_Last		(void)	{	return( m_Point_Last );			}
	double						Get_xPosition_Last		(void)	{	return( m_Point_Last.Get_X() );	}
	double						Get_yPosition_Last		(void)	{	return( m_Point_Last.Get_Y() );	}

	bool						is_Shift				(void)	{	return( (m_Keys & MODULE_INTERACTIVE_KEY_SHIFT) != 0 );	}
	bool						is_Alt					(void)	{	return( (m_Keys & MODULE_INTERACTIVE_KEY_ALT)   != 0 );	}
	bool						is_Ctrl					(void)	{	return( (m_Keys & MODULE_INTERACTIVE_KEY_CTRL)  != 0 );	}

	void						Set_Drag_Mode			(int Drag_Mode);


private:

	int							m_Keys, m_Drag_Mode;

	CSG_Point					m_Point, m_Point_Last;

	CSG_Module					*m_pModule;

};


///////////////////////////////////////////////////////////
//														 //
//				CSG_Module_Interactive					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Module_Interactive.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Interactive : public CSG_Module_Interactive_Base, public CSG_Module
{
public:
	CSG_Module_Interactive(void);
	virtual ~CSG_Module_Interactive(void);

	virtual TSG_Module_Type		Get_Type				(void)	{	return( MODULE_TYPE_Interactive );	}

	virtual bool				is_Interactive			(void)	{	return( true );	}

};


///////////////////////////////////////////////////////////
//														 //
//				CSG_Module_Grid_Interactive				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Module_Grid_Interactive.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Grid_Interactive : public CSG_Module_Interactive_Base, public CSG_Module_Grid
{
public:
	CSG_Module_Grid_Interactive(void);
	virtual ~CSG_Module_Grid_Interactive(void);

	virtual TSG_Module_Type		Get_Type				(void)	{	return( MODULE_TYPE_Grid_Interactive );	}

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
typedef enum ESG_MLB_Info
{
	MLB_INFO_Name	= 0,
	MLB_INFO_Description,
	MLB_INFO_Author,
	MLB_INFO_Version,
	MLB_INFO_Menu_Path,
	MLB_INFO_User,
	MLB_INFO_Count
}
TSG_MLB_Info;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Module_Library_Interface
{
public:
	CSG_Module_Library_Interface(void);
	virtual ~CSG_Module_Library_Interface(void);

	void						Set_Info				(int ID, const SG_Char *Info);
	const SG_Char *				Get_Info				(int ID);

	int							Get_Count				(void);
	bool						Add_Module				(CSG_Module *pModule);
	CSG_Module *				Get_Module				(int iModule);

	void						Set_File_Name			(const CSG_String &File_Name);
	const SG_Char *				Get_File_Name			(void);

	const SG_Char *				Get_Translation			(const SG_Char *Text);


private:

	const SG_Char				*m_Info[MLB_INFO_Count];

	int							m_nModules;

	CSG_Module					**m_Modules;

	CSG_String					m_File_Name;

	CSG_Translator				m_Translator;

};

//---------------------------------------------------------
#define SYMBOL_MLB_Initialize			SG_T("MLB_Initialize")
typedef bool							(* TSG_PFNC_MLB_Initialize)		(const SG_Char *);

#define SYMBOL_MLB_Finalize				SG_T("MLB_Finalize")
typedef bool							(* TSG_PFNC_MLB_Finalize)		(void);

#define SYMBOL_MLB_Get_Interface		SG_T("MLB_Get_Interface")
typedef CSG_Module_Library_Interface *	(* TSG_PFNC_MLB_Get_Interface)	(void);

//---------------------------------------------------------
#define MLB_INTERFACE_SKIP_MODULE		((CSG_Module *)0x1)

//---------------------------------------------------------
#define MLB_INTERFACE_CORE	CSG_Module_Library_Interface	MLB_Interface;\
\
extern "C" _SAGA_DLL_EXPORT CSG_Module_Library_Interface *	MLB_Get_Interface   (void)\
{\
	return( &MLB_Interface );\
}\
\
extern "C" _SAGA_DLL_EXPORT const SG_Char *					Get_API_Version		(void)\
{\
	return( SAGA_API_VERSION );\
}\

//---------------------------------------------------------
#define MLB_INTERFACE_INITIALIZE	extern "C" _SAGA_DLL_EXPORT bool MLB_Initialize	(const SG_Char *File_Name)\
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

//---------------------------------------------------------
#define MLB_INTERFACE_FINALIZE		extern "C" _SAGA_DLL_EXPORT bool MLB_Finalize	(void)\
{\
	return( true );\
}\

//---------------------------------------------------------
#define MLB_INTERFACE	MLB_INTERFACE_CORE MLB_INTERFACE_INITIALIZE MLB_INTERFACE_FINALIZE

//---------------------------------------------------------
#ifndef SWIG

extern CSG_Module_Library_Interface	MLB_Interface;

#endif	// #ifdef SWIG


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__module_H
