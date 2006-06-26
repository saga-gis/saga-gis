
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
//                      api_core.h                       //
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
#ifndef HEADER_INCLUDED__SAGA_API__api_core_H
#define HEADER_INCLUDED__SAGA_API__api_core_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _SAGA_MSW
	#define	_SAGA_DLL_EXPORT		__declspec( dllexport )
	#define	_SAGA_DLL_IMPORT		__declspec( dllimport )
#else
	#define	_SAGA_DLL_EXPORT
	#define	_SAGA_DLL_IMPORT
#endif

//---------------------------------------------------------
#ifdef _SAGA_API_EXPORTS
	#define	SAGA_API_DLL_EXPORT		_SAGA_DLL_EXPORT
#else
	#define	SAGA_API_DLL_EXPORT		_SAGA_DLL_IMPORT
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SAGA_API_VERSION			"2.0.0b"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef _TYPEDEF_BOOL
	typedef unsigned int	bool;
	#define true			((bool)1)
	#define false			((bool)0)
#endif	// _TYPEDEF_BOOL

//---------------------------------------------------------
#ifdef _TYPEDEF_BYTE
	typedef unsigned char	BYTE;
#endif	// _TYPEDEF_BYTE

//---------------------------------------------------------
#ifdef _TYPEDEF_WORD
	typedef unsigned short	WORD;
	typedef unsigned long	DWORD;
#endif	// _TYPEDEF_WORD


///////////////////////////////////////////////////////////
//														 //
//						Callback						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	CALLBACK_PROCESS_GET_OKAY,
	CALLBACK_PROCESS_SET_OKAY,
	CALLBACK_PROCESS_SET_PROGRESS,
	CALLBACK_PROCESS_SET_READY,
	CALLBACK_PROCESS_SET_TEXT,

	CALLBACK_DLG_MESSAGE,
	CALLBACK_DLG_CONTINUE,
	CALLBACK_DLG_ERROR,

	CALLBACK_DLG_PARAMETERS,

	CALLBACK_MESSAGE_ADD,
	CALLBACK_MESSAGE_ADD_ERROR,
	CALLBACK_MESSAGE_ADD_EXECUTION,

	CALLBACK_DATAOBJECT_FIND_BY_FILE,
	CALLBACK_DATAOBJECT_CHECK,
	CALLBACK_DATAOBJECT_ADD,
	CALLBACK_DATAOBJECT_UPDATE,
	CALLBACK_DATAOBJECT_SHOW,
	CALLBACK_DATAOBJECT_ASIMAGE,

	CALLBACK_DATAOBJECT_COLORS_GET,
	CALLBACK_DATAOBJECT_COLORS_SET,

	CALLBACK_DATAOBJECT_GET_ALL,

	CALLBACK_GUI_GET_WINDOW
}
TAPI_Callback_ID;

//---------------------------------------------------------
typedef int (* PFNC_Callback) (TAPI_Callback_ID ID, long pParam_1, long pParam_2);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool				API_Set_Callback					(PFNC_Callback Callback);
SAGA_API_DLL_EXPORT PFNC_Callback		API_Get_Callback					(void);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool				API_Callback_Process_Get_Okay		(bool bBlink);
SAGA_API_DLL_EXPORT bool				API_Callback_Process_Set_Okay		(bool bOkay = true);
SAGA_API_DLL_EXPORT bool				API_Callback_Process_Set_Progress	(double Position, double Range);
SAGA_API_DLL_EXPORT bool				API_Callback_Process_Set_Ready		(void);
SAGA_API_DLL_EXPORT void				API_Callback_Process_Set_Text		(const char *Text);

SAGA_API_DLL_EXPORT void				API_Callback_Dlg_Message			(const char *Message, const char *Caption);
SAGA_API_DLL_EXPORT bool				API_Callback_Dlg_Continue			(const char *Message, const char *Caption);
SAGA_API_DLL_EXPORT int					API_Callback_Dlg_Error				(const char *Message, const char *Caption);

SAGA_API_DLL_EXPORT bool				API_Callback_Dlg_Parameters			(class CParameters *pParameters, const char *Caption);

SAGA_API_DLL_EXPORT void				API_Callback_Message_Add			(const char *Message, bool bNewLine);
SAGA_API_DLL_EXPORT void				API_Callback_Message_Add_Error		(const char *Message);
SAGA_API_DLL_EXPORT void				API_Callback_Message_Add_Execution	(const char *Message, bool bNewLine);

SAGA_API_DLL_EXPORT class CDataObject *	API_Callback_DataObject_Find		(const char *File_Name         , int Object_Type);
SAGA_API_DLL_EXPORT bool				API_Callback_DataObject_Check		(class CDataObject *pDataObject, int Object_Type);
SAGA_API_DLL_EXPORT bool				API_Callback_DataObject_Add			(class CDataObject *pDataObject, bool bShow);
SAGA_API_DLL_EXPORT bool				API_Callback_DataObject_Update		(class CDataObject *pDataObject, bool bShow, class CParameters *pParameters);
SAGA_API_DLL_EXPORT bool				API_Callback_DataObject_Show		(class CDataObject *pDataObject);
SAGA_API_DLL_EXPORT bool				API_Callback_DataObject_asImage		(class CDataObject *pDataObject, class CGrid *pGrid);

SAGA_API_DLL_EXPORT bool				API_Callback_DataObject_Colors_Get	(class CDataObject *pDataObject, class CColors *pColors);
SAGA_API_DLL_EXPORT bool				API_Callback_DataObject_Colors_Set	(class CDataObject *pDataObject, class CColors *pColors);

SAGA_API_DLL_EXPORT bool				API_Callback_DataObject_Get_All		(class CParameters *pParameters);

SAGA_API_DLL_EXPORT void *				API_Callback_Get_Window_Main		(void);


///////////////////////////////////////////////////////////
//														 //
//						Memory							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT void *			API_Malloc			(size_t size);
SAGA_API_DLL_EXPORT void *			API_Calloc			(size_t num, size_t size);
SAGA_API_DLL_EXPORT void *			API_Realloc			(void *memblock, size_t size);
SAGA_API_DLL_EXPORT void			API_Free			(void *memblock);

SAGA_API_DLL_EXPORT void			API_Swap_Bytes		(void *Value, int nValueBytes);


///////////////////////////////////////////////////////////
//														 //
//						Points							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef struct
{
	int								x, y;
}
TAPI_iPoint;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CAPI_iPoints
{
public:
	CAPI_iPoints(void);
	virtual ~CAPI_iPoints(void);

	void							Clear				(void);

	CAPI_iPoints &					operator  =			(const CAPI_iPoints &Points);
	bool							Assign				(const CAPI_iPoints &Points);

	bool							Add					(int x, int y);
	bool							Add					(const TAPI_iPoint &Point);

	bool							Set_Count			(int nPoints);
	int								Get_Count			(void) const		{	return( m_nPoints );	}

	TAPI_iPoint &					operator []			(int Index)			{	return( m_Points[Index]   );	}
	TAPI_iPoint &					Get_Point			(int Index)			{	return( m_Points[Index]   );	}
	int								Get_X				(int Index) const	{	return( m_Points[Index].x );	}
	int								Get_Y				(int Index) const	{	return( m_Points[Index].y );	}


private:

	int								m_nPoints;

	TAPI_iPoint						*m_Points;

};

//---------------------------------------------------------
typedef struct
{
	double							x, y;
}
TAPI_dPoint;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CAPI_dPoints
{
public:
	CAPI_dPoints(void);
	virtual ~CAPI_dPoints(void);

	void							Clear				(void);

	CAPI_dPoints &					operator  =			(const CAPI_dPoints &Points);
	bool							Assign				(const CAPI_dPoints &Points);

	bool							Add					(double x, double y);
	bool							Add					(const TAPI_dPoint &Point);

	bool							Set_Count			(int nPoints);
	int								Get_Count			(void) const		{	return( m_nPoints );	}

	TAPI_dPoint &					operator []			(int Index)			{	return( m_Points[Index]   );	}
	TAPI_dPoint &					Get_Point			(int Index)			{	return( m_Points[Index]   );	}
	double							Get_X				(int Index) const	{	return( m_Points[Index].x );	}
	double							Get_Y				(int Index) const	{	return( m_Points[Index].y );	}


private:

	int								m_nPoints;

	TAPI_dPoint						*m_Points;

};

//---------------------------------------------------------
typedef struct
{
	int								x, y, z;
}
TAPI_3D_iPoint;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CAPI_3D_iPoints
{
public:
	CAPI_3D_iPoints(void);
	virtual ~CAPI_3D_iPoints(void);

	void							Clear				(void);

	CAPI_3D_iPoints &				operator  =			(const CAPI_3D_iPoints &Points);
	bool							Assign				(const CAPI_3D_iPoints &Points);

	bool							Add					(int x, int y, int z);
	bool							Add					(const TAPI_3D_iPoint &Point);

	bool							Set_Count			(int nPoints);
	int								Get_Count			(void)		{	return( m_nPoints );	}

	TAPI_3D_iPoint &				operator []			(int Index)	{	return( m_Points[Index]   );	}
	TAPI_3D_iPoint &				Get_Point			(int Index)	{	return( m_Points[Index]   );	}
	int								Get_X				(int Index)	{	return( m_Points[Index].x );	}
	int								Get_Y				(int Index)	{	return( m_Points[Index].y );	}
	int								Get_Z				(int Index)	{	return( m_Points[Index].z );	}


private:

	int								m_nPoints;

	TAPI_3D_iPoint					*m_Points;

};

//---------------------------------------------------------
typedef struct
{
	double							x, y, z;
}
TAPI_3D_dPoint;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CAPI_3D_dPoints
{
public:
	CAPI_3D_dPoints(void);
	virtual ~CAPI_3D_dPoints(void);

	void							Clear				(void);

	CAPI_3D_dPoints &				operator  =			(const CAPI_3D_dPoints &Points);
	bool							Assign				(const CAPI_3D_dPoints &Points);

	bool							Add					(double x, double y, double z);
	bool							Add					(const TAPI_3D_dPoint &Point);

	bool							Set_Count			(int nPoints);
	int								Get_Count			(void)		{	return( m_nPoints );	}

	TAPI_3D_dPoint &				operator []			(int Index)	{	return( m_Points[Index]   );	}
	TAPI_3D_dPoint &				Get_Point			(int Index)	{	return( m_Points[Index]   );	}
	double							Get_X				(int Index)	{	return( m_Points[Index].x );	}
	double							Get_Y				(int Index)	{	return( m_Points[Index].y );	}
	double							Get_Z				(int Index)	{	return( m_Points[Index].z );	}


private:

	int								m_nPoints;

	TAPI_3D_dPoint					*m_Points;

};


///////////////////////////////////////////////////////////
//														 //
//						String							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CAPI_String
{
public:
	CAPI_String(void);
	CAPI_String(const CAPI_String &String);
	CAPI_String(const char *String);
	CAPI_String(char Character);

	virtual ~CAPI_String(void);

	const char *					c_str				(void);
	operator const char *								(void)	{	return( c_str() );	}

	size_t							Length				(void);

	void							Clear				(void);
	int								Printf				(const char *Format, ...);
	static CAPI_String				Format				(const char *Format, ...);

	CAPI_String &					Append				(const char *String);
	CAPI_String &					Append				(char Character);

	CAPI_String &					operator =			(const CAPI_String &String);
	CAPI_String &					operator =			(const char *String);
	CAPI_String &					operator =			(char Character);

	void							operator +=			(const CAPI_String &String);
	void							operator +=			(const char *String);
	void							operator +=			(char Character);

	char &							operator []			(int i);

	int								Cmp					(const char *String);
	int								CmpNoCase			(const char *String);

	CAPI_String &					Make_Lower			(void);
	CAPI_String &					Make_Upper			(void);

	size_t							Replace				(const char *sOld, const char *sNew, bool replaceAll = true);

	CAPI_String &					Remove				(size_t pos);
	CAPI_String &					Remove				(size_t pos, size_t len);

	int								Remove_WhiteChars	(bool fromEnd = false);

	int								Find				(char Character, bool fromEnd = false);
	int								Find				(const char *String);
	bool							Contains			(const char *String);

	CAPI_String						AfterFirst			(char Character) const;
	CAPI_String						AfterLast			(char Character) const;
	CAPI_String						BeforeFirst			(char Character) const;
	CAPI_String						BeforeLast			(char Character) const;

	CAPI_String						Right				(size_t count) const;
	CAPI_String						Mid					(size_t first, size_t count) const;
	CAPI_String						Left				(size_t count) const;

	int								asInt				(void);
	bool							asInt				(int &Value);
	double							asDouble			(void);
	bool							asDouble			(double &Value);


protected:

	class wxString					*m_pString;

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CAPI_String		operator +			(const CAPI_String &x, const CAPI_String &y);
SAGA_API_DLL_EXPORT CAPI_String		operator +			(const CAPI_String &x, const char *y);
SAGA_API_DLL_EXPORT CAPI_String		operator +			(const CAPI_String &x, char y);
SAGA_API_DLL_EXPORT CAPI_String		operator +			(const char *x, const CAPI_String &y);

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CAPI_Strings
{
public:
	CAPI_Strings(void);
	CAPI_Strings(const CAPI_Strings &Strings);
	CAPI_Strings(int nStrings, const char **Strings);

	virtual ~CAPI_Strings(void);

	void							Clear				(void);

	CAPI_Strings &					operator  =			(const CAPI_Strings &Strings);
	bool							Assign				(const CAPI_Strings &Strings);

	bool							Add					(const CAPI_String &String);

	bool							Set_Count			(int nStrings);
	int								Get_Count			(void)				{	return( m_nStrings );	}

	CAPI_String &					operator []			(int Index) const	{	return( *m_Strings[Index]   );	}
	CAPI_String &					Get_String			(int Index) const	{	return( *m_Strings[Index]   );	}


protected:

	int								m_nStrings;

	CAPI_String						**m_Strings;

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CAPI_String		API_Get_CurrentTimeStr			(bool bWithDate = true);

SAGA_API_DLL_EXPORT double			API_DegreeStr2Double			(const char *String);
SAGA_API_DLL_EXPORT CAPI_String		API_Double2DegreeStr			(double Value);

SAGA_API_DLL_EXPORT double			API_DateStr2Double				(const char *String);
SAGA_API_DLL_EXPORT CAPI_String		API_Double2DateStr				(double Value);

SAGA_API_DLL_EXPORT int				API_Get_Significant_Decimals	(double Value, int maxDecimals = 6);

SAGA_API_DLL_EXPORT CAPI_String		API_Get_String					(double Value, int Precision = 2, bool bScientific = false);


///////////////////////////////////////////////////////////
//														 //
//						File							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool			API_Directory_isValid			(const char *Directory);
SAGA_API_DLL_EXPORT bool			API_Directory_Make				(const char *Directory);

SAGA_API_DLL_EXPORT bool			API_Remove_File					(const char *FileName);
SAGA_API_DLL_EXPORT CAPI_String		API_Get_CWD						(void);

SAGA_API_DLL_EXPORT CAPI_String		API_Get_Temp_File_Name			(const char *Prefix, const char *Directory);

SAGA_API_DLL_EXPORT CAPI_String		API_Extract_File_Name			(const char *full_Path, bool bExtension);
SAGA_API_DLL_EXPORT CAPI_String		API_Extract_File_Path			(const char *full_Path);

SAGA_API_DLL_EXPORT CAPI_String		API_Make_File_Path				(const char *Directory, const char *Name, const char *Extension = NULL);

SAGA_API_DLL_EXPORT bool			API_Cmp_File_Extension			(const char *File_Name, const char *Extension);

SAGA_API_DLL_EXPORT bool			API_Read_Line					(FILE *Stream, CAPI_String &Line);

SAGA_API_DLL_EXPORT int				API_Read_Int					(FILE *Stream				, bool bBig);
SAGA_API_DLL_EXPORT void			API_Write_Int					(FILE *Stream, int Value	, bool bBig);
SAGA_API_DLL_EXPORT double			API_Read_Double					(FILE *Stream				, bool bBig);
SAGA_API_DLL_EXPORT void			API_Write_Double				(FILE *Stream, double Value	, bool bBig);
SAGA_API_DLL_EXPORT int				API_Read_Int					(char *Buffer				, bool bBig);
SAGA_API_DLL_EXPORT void			API_Write_Int					(char *Buffer, int Value	, bool bBig);
SAGA_API_DLL_EXPORT double			API_Read_Double					(char *Buffer				, bool bBig);
SAGA_API_DLL_EXPORT void			API_Write_Double				(char *Buffer, double Value	, bool bBig);


///////////////////////////////////////////////////////////
//														 //
//						Translator						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CTranslator
{
private:

	class CTranslation
	{
	public:
		CTranslation(const char *Text, const char *Translation)
		{
			m_Text					= Text;
			m_Translation			= Translation;
		}

		~CTranslation(void)	{}

		CAPI_String					m_Text, m_Translation;
	};


public:
	CTranslator(void);
	CTranslator(const char *File_Name, bool bSetExtension = true);

	virtual ~CTranslator(void);

	bool							Create				(const char *File_Name, bool bSetExtension = true);
	void							Destroy				(void);

	int								Get_Count			(void)	{	return( m_nTranslations );	}

	const char *					Get_Text			(int Index);
	const char *					Get_Translation		(int Index);

	const char *					Get_Translation		(const char *Text);


private:

	int								m_nTranslations;

	CTranslation					**m_Translations;


	void							_Add_Translation	(const char *Text, const char *Translation);
	int								_Get_Index			(const char *Text);

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CTranslator *	API_Get_Translator	(void);

SAGA_API_DLL_EXPORT const char *	LNG					(const char *Text);


///////////////////////////////////////////////////////////
//														 //
//						Colors							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define COLOR_GET_RGB(r, g, b)		((DWORD) (((BYTE)(r) | ((WORD)(g) << 8)) | (((DWORD)(BYTE)(b)) << 16)))
#define COLOR_GET_RGBA(r, g, b, a)	((DWORD) (((BYTE)(r) | ((WORD)(g) << 8)) | (((DWORD)(BYTE)(b)) << 16) | (((DWORD)(BYTE)(a)) << 24)))

#define COLOR_GET_R(rgb)			((BYTE) ((rgb)      ))
#define COLOR_GET_G(rgb)			((BYTE) ((rgb) >>  8))
#define COLOR_GET_B(rgb)			((BYTE) ((rgb) >> 16))
#define COLOR_GET_A(rgb)			((BYTE) ((rgb) >> 24))

//---------------------------------------------------------
#define COLOR_DEF_BLACK				COLOR_GET_RGB(  0,   0,   0)
#define COLOR_DEF_GREY				COLOR_GET_RGB(128, 128, 128)
#define COLOR_DEF_GREY_LIGHT		COLOR_GET_RGB(192, 192, 192)
#define COLOR_DEF_WHITE				COLOR_GET_RGB(255, 255, 255)
#define COLOR_DEF_RED				COLOR_GET_RGB(255,   0,   0)
#define COLOR_DEF_RED_DARK			COLOR_GET_RGB(128,   0,   0)
#define COLOR_DEF_YELLOW			COLOR_GET_RGB(255, 255,   0)
#define COLOR_DEF_YELLOW_DARK		COLOR_GET_RGB(128, 128,   0)
#define COLOR_DEF_GREEN				COLOR_GET_RGB(  0, 255,   0)
#define COLOR_DEF_GREEN_DARK		COLOR_GET_RGB(  0, 128,   0)
#define COLOR_DEF_GREEN_LIGHT		COLOR_GET_RGB(  0, 255,   0)
#define COLOR_DEF_BLUE				COLOR_GET_RGB(  0,   0, 255)
#define COLOR_DEF_BLUE_DARK			COLOR_GET_RGB(  0,   0, 128)
#define COLOR_DEF_BLUE_LIGHT		COLOR_GET_RGB(  0, 255, 255)
#define COLOR_DEF_BLUE_GREEN		COLOR_GET_RGB(  0, 128, 128)
#define COLOR_DEF_PURPLE			COLOR_GET_RGB(128,   0, 128)
#define COLOR_DEF_PINK				COLOR_GET_RGB(255,   0, 255)
#define COLOR_DEF_NONE				-1
#define COLOR_DEF_RANDOM			-2

//---------------------------------------------------------
enum
{
	COLORS_PALETTE_DEFAULT		= 0,
	COLORS_PALETTE_DEFAULT_BRIGHT,
	COLORS_PALETTE_BLACK_WHITE,
	COLORS_PALETTE_BLACK_RED,
	COLORS_PALETTE_BLACK_GREEN,
	COLORS_PALETTE_BLACK_BLUE,
	COLORS_PALETTE_WHITE_RED,
	COLORS_PALETTE_WHITE_GREEN,
	COLORS_PALETTE_WHITE_BLUE,
	COLORS_PALETTE_YELLOW_RED,
	COLORS_PALETTE_YELLOW_GREEN,
	COLORS_PALETTE_YELLOW_BLUE,
	COLORS_PALETTE_RED_GREEN,
	COLORS_PALETTE_RED_BLUE,
	COLORS_PALETTE_GREEN_BLUE,
	COLORS_PALETTE_RED_GREY_BLUE,
	COLORS_PALETTE_RED_GREY_GREEN,
	COLORS_PALETTE_GREEN_GREY_BLUE,
	COLORS_PALETTE_RED_GREEN_BLUE,
	COLORS_PALETTE_RED_BLUE_GREEN,
	COLORS_PALETTE_GREEN_RED_BLUE,
	COLORS_PALETTE_RAINBOW,
	COLORS_PALETTE_NEON,
	COLORS_PALETTE_COUNT
};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CColors
{
public:
	CColors(void);
	CColors(int nColors, int Palette = COLORS_PALETTE_DEFAULT, bool bRevert = false);
	virtual ~CColors(void);

	void							Destroy				(void);

	bool							Set_Count			(int nColors);
	int								Get_Count			(void) const		{	return( m_nColors );	}

	bool							Set_Color			(int Index, long Color);
	bool							Set_Color			(int Index, int Red, int Green, int Blue);
	bool							Set_Red				(int Index, int Value);
	bool							Set_Green			(int Index, int Value);
	bool							Set_Blue			(int Index, int Value);
	bool							Set_Brightness		(int Index, int Value);

	long							Get_Color			(int Index) const	{	return( Index >= 0 && Index < m_nColors ? m_Colors[Index] : 0 );	}
	long							Get_Red				(int Index) const	{	return( COLOR_GET_R(Get_Color(Index)) );	}
	long							Get_Green			(int Index) const	{	return( COLOR_GET_G(Get_Color(Index)) );	}
	long							Get_Blue			(int Index) const	{	return( COLOR_GET_B(Get_Color(Index)) );	}
	long							Get_Brightness		(int Index) const	{	return( (Get_Red(Index) + Get_Green(Index) + Get_Blue(Index)) / 3 );	}

	bool							Set_Default			(int nColors = 100);
	bool							Set_Palette			(int Index, bool bRevert = false, int nColors = 100);
	bool							Set_Ramp			(long Color_A, long Color_B);
	bool							Set_Ramp			(long Color_A, long Color_B, int iColor_A, int iColor_B);
	bool							Set_Ramp_Brighness	(int Brightness_A, int Brightness_B);
	bool							Set_Ramp_Brighness	(int Brightness_A, int Brightness_B, int iColor_A, int iColor_B);

	bool							Random				(void);
	bool							Invert				(void);
	bool							Revert				(void);

	bool							Assign				(CColors *pSource);

	bool							Load				(const char *File_Name);
	bool							Save				(const char *File_Name, bool bBinary);

	bool							Serialize			(FILE *Stream, bool bSave, bool bBinary);


private:

	int								m_nColors;

	long							*m_Colors;


	void							_Set_Brightness		(double &a, double &b, double &c, int Pass = 0);

};


///////////////////////////////////////////////////////////
//														 //
//						History							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CHistory_Entry
{
	friend class CHistory;

public:

	const char *					Get_Date			(void)	{	return( m_Date );		}
	const char *					Get_Name			(void)	{	return( m_Name );		}
	const char *					Get_Entry			(void)	{	return( m_Entry );		}
	class CHistory *				Get_History			(void)	{	return( m_pHistory );	}


private:

	CHistory_Entry(const char *Date, const char *Name, const char *Entry, class CHistory *pHistory);
	CHistory_Entry(const CHistory_Entry &Entry);
	virtual ~CHistory_Entry(void);

	CAPI_String						m_Date, m_Name, m_Entry;

	class CHistory					*m_pHistory;

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CHistory
{
public:
	CHistory(void);
	CHistory(const CHistory &History);
	virtual ~CHistory(void);

	void							Destroy				(void);
	void							Assign				(const CHistory &History, bool bAdd = false);

	int								Get_Count			(void)			{	return( m_nEntries );	}
	CHistory_Entry *				Get_Entry			(int iEntry)	{	return( iEntry >= 0 && iEntry < m_nEntries ? m_pEntries[iEntry] : NULL );	}
	void							Add_Entry			(const char *Name, const char *Entry, CHistory *pHistory = NULL);

	bool							Load				(const char *File_Name, const char *File_Extension);
	bool							Save				(const char *File_Name, const char *File_Extension);

	CAPI_String						Get_HTML			(void);


private:

	int								m_nEntries;

	CHistory_Entry					**m_pEntries;


	void							_Add_Entry			(CHistory_Entry *pEntry);

	bool							_Load				(FILE *Stream);
	bool							_Save				(FILE *Stream);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__api_core_H
