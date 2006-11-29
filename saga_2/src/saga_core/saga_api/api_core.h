
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
#define SAGA_API_VERSION			"2.0.0"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef SWIG

#include <stdlib.h>
#include <stdio.h>

#endif	// #ifdef SWIG


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
//						UI Callback						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_UI_Callback_ID
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
TSG_UI_Callback_ID;

//---------------------------------------------------------
typedef int (* TSG_PFNC_UI_Callback) (TSG_UI_Callback_ID ID, long pParam_1, long pParam_2);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool					SG_Set_UI_Callback			(TSG_PFNC_UI_Callback Function);
SAGA_API_DLL_EXPORT TSG_PFNC_UI_Callback	SG_Get_UI_Callback			(void);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool					SG_UI_Process_Get_Okay		(bool bBlink);
SAGA_API_DLL_EXPORT bool					SG_UI_Process_Set_Okay		(bool bOkay = true);
SAGA_API_DLL_EXPORT bool					SG_UI_Process_Set_Progress	(double Position, double Range);
SAGA_API_DLL_EXPORT bool					SG_UI_Process_Set_Ready		(void);
SAGA_API_DLL_EXPORT void					SG_UI_Process_Set_Text		(const char *Text);

SAGA_API_DLL_EXPORT void					SG_UI_Dlg_Message			(const char *Message, const char *Caption);
SAGA_API_DLL_EXPORT bool					SG_UI_Dlg_Continue			(const char *Message, const char *Caption);
SAGA_API_DLL_EXPORT int						SG_UI_Dlg_Error				(const char *Message, const char *Caption);
SAGA_API_DLL_EXPORT bool					SG_UI_Dlg_Parameters		(class CSG_Parameters *pParameters, const char *Caption);

SAGA_API_DLL_EXPORT void					SG_UI_Msg_Add				(const char *Message, bool bNewLine);
SAGA_API_DLL_EXPORT void					SG_UI_Msg_Add_Error			(const char *Message);
SAGA_API_DLL_EXPORT void					SG_UI_Msg_Add_Execution		(const char *Message, bool bNewLine);

SAGA_API_DLL_EXPORT class CSG_Data_Object *	SG_UI_DataObject_Find		(const char *File_Name             , int Object_Type);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Check		(class CSG_Data_Object *pDataObject, int Object_Type);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Add		(class CSG_Data_Object *pDataObject, bool bShow);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Update		(class CSG_Data_Object *pDataObject, bool bShow, class CSG_Parameters *pParameters);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Show		(class CSG_Data_Object *pDataObject);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_asImage	(class CSG_Data_Object *pDataObject, class CSG_Grid *pGrid);

SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Colors_Get	(class CSG_Data_Object *pDataObject, class CSG_Colors *pColors);
SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Colors_Set	(class CSG_Data_Object *pDataObject, class CSG_Colors *pColors);

SAGA_API_DLL_EXPORT bool					SG_UI_DataObject_Get_All	(class CSG_Parameters *pParameters);

SAGA_API_DLL_EXPORT void *					SG_UI_Get_Window_Main		(void);


///////////////////////////////////////////////////////////
//														 //
//						Memory							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT void *			SG_Malloc			(size_t size);
SAGA_API_DLL_EXPORT void *			SG_Calloc			(size_t num, size_t size);
SAGA_API_DLL_EXPORT void *			SG_Realloc			(void *memblock, size_t size);
SAGA_API_DLL_EXPORT void			SG_Free				(void *memblock);

SAGA_API_DLL_EXPORT void			SG_Swap_Bytes		(void *Value, int nValueBytes);

//---------------------------------------------------------
#define SG_GET_LONG(b0, b1, b2, b3)	((long) (((BYTE)(b0) | ((WORD)(b1) << 8)) | (((DWORD)(BYTE)(b2)) << 16) | (((DWORD)(BYTE)(b3)) << 24)))

#define SG_GET_BYTE_0(vLong)		((BYTE) ((vLong)      ))
#define SG_GET_BYTE_1(vLong)		((BYTE) ((vLong) >>  8))
#define SG_GET_BYTE_2(vLong)		((BYTE) ((vLong) >> 16))
#define SG_GET_BYTE_3(vLong)		((BYTE) ((vLong) >> 24))


///////////////////////////////////////////////////////////
//														 //
//						String							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_String
{
public:
	CSG_String(void);
	CSG_String(const CSG_String &String);
	CSG_String(const char *String);
	CSG_String(char Character);

	virtual ~CSG_String(void);

	const char *					c_str				(void)	const;
	operator const char *								(void)	const	{	return( c_str() );	}

	size_t							Length				(void)	const;

	void							Clear				(void);
	int								Printf				(const char *Format, ...);
	static CSG_String				Format				(const char *Format, ...);

	CSG_String &					Append				(const char *String);
	CSG_String &					Append				(char Character);

	CSG_String &					operator =			(const CSG_String &String);
	CSG_String &					operator =			(const char *String);
	CSG_String &					operator =			(char Character);

	CSG_String						operator +			(const CSG_String &String)		const;
	CSG_String						operator +			(const char *String)			const;
	CSG_String						operator +			(char Character)				const;

	void							operator +=			(const CSG_String &String);
	void							operator +=			(const char *String);
	void							operator +=			(char Character);

	char &							operator []			(int i);

	int								Cmp					(const char *String)			const;
	int								CmpNoCase			(const char *String)			const;

	CSG_String &					Make_Lower			(void);
	CSG_String &					Make_Upper			(void);

	size_t							Replace				(const char *sOld, const char *sNew, bool replaceAll = true);

	CSG_String &					Remove				(size_t pos);
	CSG_String &					Remove				(size_t pos, size_t len);

	int								Remove_WhiteChars	(bool fromEnd = false);

	int								Find				(char Character, bool fromEnd = false);
	int								Find				(const char *String);
	bool							Contains			(const char *String);

	CSG_String						AfterFirst			(char Character)				const;
	CSG_String						AfterLast			(char Character)				const;
	CSG_String						BeforeFirst			(char Character)				const;
	CSG_String						BeforeLast			(char Character)				const;

	CSG_String						Right				(size_t count)					const;
	CSG_String						Mid					(size_t first, size_t count)	const;
	CSG_String						Left				(size_t count) const;

	int								asInt				(void)							const;
	bool							asInt				(int &Value)					const;
	double							asDouble			(void)							const;
	bool							asDouble			(double &Value)					const;


protected:

	class wxString					*m_pString;

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_String		operator +			(const char *A, const CSG_String &B);
SAGA_API_DLL_EXPORT CSG_String		operator +			(char        A, const CSG_String &B);

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Strings
{
public:
	CSG_Strings(void);
	CSG_Strings(const CSG_Strings &Strings);
	CSG_Strings(int nStrings, const char **Strings);

	virtual ~CSG_Strings(void);

	void							Clear				(void);

	CSG_Strings &					operator  =			(const CSG_Strings &Strings);
	bool							Assign				(const CSG_Strings &Strings);

	bool							Add					(const CSG_String &String);

	bool							Set_Count			(int nStrings);
	int								Get_Count			(void)				{	return( m_nStrings );	}

	CSG_String &					operator []			(int Index) const	{	return( *m_Strings[Index]   );	}
	CSG_String &					Get_String			(int Index) const	{	return( *m_Strings[Index]   );	}


protected:

	int								m_nStrings;

	CSG_String						**m_Strings;

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_String		SG_Get_CurrentTimeStr			(bool bWithDate = true);

SAGA_API_DLL_EXPORT double			SG_Degree_To_Double				(const char *String);
SAGA_API_DLL_EXPORT CSG_String		SG_Double_To_Degree				(double Value);

SAGA_API_DLL_EXPORT double			SG_Date_To_Double				(const char *String);
SAGA_API_DLL_EXPORT CSG_String		SG_Double_To_Date				(double Value);

SAGA_API_DLL_EXPORT int				SG_Get_Significant_Decimals		(double Value, int maxDecimals = 6);

SAGA_API_DLL_EXPORT CSG_String		SG_Get_String					(double Value, int Precision = 2, bool bScientific = false);


///////////////////////////////////////////////////////////
//														 //
//						File							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool			SG_Dir_isValid			(const char *Directory);
SAGA_API_DLL_EXPORT bool			SG_Dir_Create			(const char *Directory);
SAGA_API_DLL_EXPORT CSG_String		SG_Dir_Get_Current		(void);

SAGA_API_DLL_EXPORT bool			SG_File_Exists			(const char *FileName);
SAGA_API_DLL_EXPORT bool			SG_File_Delete			(const char *FileName);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Get_TmpName		(const char *Prefix, const char *Directory);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Get_Name		(const char *full_Path, bool bExtension);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Get_Path		(const char *full_Path);
SAGA_API_DLL_EXPORT CSG_String		SG_File_Make_Path		(const char *Directory, const char *Name, const char *Extension = NULL);
SAGA_API_DLL_EXPORT bool			SG_File_Cmp_Extension	(const char *File_Name, const char *Extension);

SAGA_API_DLL_EXPORT bool			SG_Read_Line			(FILE *Stream, CSG_String &Line);

SAGA_API_DLL_EXPORT int				SG_Read_Int				(FILE *Stream				, bool bBig);
SAGA_API_DLL_EXPORT void			SG_Write_Int			(FILE *Stream, int Value	, bool bBig);
SAGA_API_DLL_EXPORT double			SG_Read_Double			(FILE *Stream				, bool bBig);
SAGA_API_DLL_EXPORT void			SG_Write_Double			(FILE *Stream, double Value	, bool bBig);
SAGA_API_DLL_EXPORT int				SG_Read_Int				(char *Buffer				, bool bBig);
SAGA_API_DLL_EXPORT void			SG_Write_Int			(char *Buffer, int Value	, bool bBig);
SAGA_API_DLL_EXPORT double			SG_Read_Double			(char *Buffer				, bool bBig);
SAGA_API_DLL_EXPORT void			SG_Write_Double			(char *Buffer, double Value	, bool bBig);


///////////////////////////////////////////////////////////
//														 //
//						Colors							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_GET_RGB(r, g, b)			((DWORD) (((BYTE)(r) | ((WORD)(g) << 8)) | (((DWORD)(BYTE)(b)) << 16)))
#define SG_GET_RGBA(r, g, b, a)		((DWORD) (((BYTE)(r) | ((WORD)(g) << 8)) | (((DWORD)(BYTE)(b)) << 16) | (((DWORD)(BYTE)(a)) << 24)))

#define SG_GET_R(rgb)				((BYTE) ((rgb)      ))
#define SG_GET_G(rgb)				((BYTE) ((rgb) >>  8))
#define SG_GET_B(rgb)				((BYTE) ((rgb) >> 16))
#define SG_GET_A(rgb)				((BYTE) ((rgb) >> 24))

//---------------------------------------------------------
#define SG_COLOR_BLACK				SG_GET_RGB(  0,   0,   0)
#define SG_COLOR_GREY				SG_GET_RGB(128, 128, 128)
#define SG_COLOR_GREY_LIGHT			SG_GET_RGB(192, 192, 192)
#define SG_COLOR_WHITE				SG_GET_RGB(255, 255, 255)
#define SG_COLOR_RED				SG_GET_RGB(255,   0,   0)
#define SG_COLOR_RED_DARK			SG_GET_RGB(128,   0,   0)
#define SG_COLOR_YELLOW				SG_GET_RGB(255, 255,   0)
#define SG_COLOR_YELLOW_DARK		SG_GET_RGB(128, 128,   0)
#define SG_COLOR_GREEN				SG_GET_RGB(  0, 255,   0)
#define SG_COLOR_GREEN_DARK			SG_GET_RGB(  0, 128,   0)
#define SG_COLOR_GREEN_LIGHT		SG_GET_RGB(  0, 255,   0)
#define SG_COLOR_BLUE				SG_GET_RGB(  0,   0, 255)
#define SG_COLOR_BLUE_DARK			SG_GET_RGB(  0,   0, 128)
#define SG_COLOR_BLUE_LIGHT			SG_GET_RGB(  0, 255, 255)
#define SG_COLOR_BLUE_GREEN			SG_GET_RGB(  0, 128, 128)
#define SG_COLOR_PURPLE				SG_GET_RGB(128,   0, 128)
#define SG_COLOR_PINK				SG_GET_RGB(255,   0, 255)
#define SG_COLOR_NONE				-1
#define SG_COLOR_RANDOM				-2

//---------------------------------------------------------
enum ESG_Colors
{
	SG_COLORS_DEFAULT			= 0,
	SG_COLORS_DEFAULT_BRIGHT,
	SG_COLORS_BLACK_WHITE,
	SG_COLORS_BLACK_RED,
	SG_COLORS_BLACK_GREEN,
	SG_COLORS_BLACK_BLUE,
	SG_COLORS_WHITE_RED,
	SG_COLORS_WHITE_GREEN,
	SG_COLORS_WHITE_BLUE,
	SG_COLORS_YELLOW_RED,
	SG_COLORS_YELLOW_GREEN,
	SG_COLORS_YELLOW_BLUE,
	SG_COLORS_RED_GREEN,
	SG_COLORS_RED_BLUE,
	SG_COLORS_GREEN_BLUE,
	SG_COLORS_RED_GREY_BLUE,
	SG_COLORS_RED_GREY_GREEN,
	SG_COLORS_GREEN_GREY_BLUE,
	SG_COLORS_RED_GREEN_BLUE,
	SG_COLORS_RED_BLUE_GREEN,
	SG_COLORS_GREEN_RED_BLUE,
	SG_COLORS_RAINBOW,
	SG_COLORS_NEON,
	SG_COLORS_COUNT
};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Colors
{
public:
	CSG_Colors(void);
	CSG_Colors(const CSG_Colors &Colors);
	CSG_Colors(int nColors, int Palette = SG_COLORS_DEFAULT, bool bRevert = false);
	virtual ~CSG_Colors(void);

	void							Destroy				(void);

	bool							Set_Count			(int nColors);
	int								Get_Count			(void) const		{	return( m_nColors );	}

	CSG_Colors &					operator  =			(const CSG_Colors &Colors);
	long &							operator []			(int Index)			{	return( m_Colors[Index] );	}

	bool							Set_Color			(int Index, long Color);
	bool							Set_Color			(int Index, int Red, int Green, int Blue);
	bool							Set_Red				(int Index, int Value);
	bool							Set_Green			(int Index, int Value);
	bool							Set_Blue			(int Index, int Value);
	bool							Set_Brightness		(int Index, int Value);

	long							Get_Color			(int Index) const	{	return( Index >= 0 && Index < m_nColors ? m_Colors[Index] : 0 );	}
	long							Get_Red				(int Index) const	{	return( SG_GET_R(Get_Color(Index)) );	}
	long							Get_Green			(int Index) const	{	return( SG_GET_G(Get_Color(Index)) );	}
	long							Get_Blue			(int Index) const	{	return( SG_GET_B(Get_Color(Index)) );	}
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

	bool							Assign				(const CSG_Colors &Colors);
	bool							Assign				(CSG_Colors *pSource);

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
//						Translator						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Translator
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

		CSG_String					m_Text, m_Translation;
	};


public:
	CSG_Translator(void);
	CSG_Translator(const char *File_Name, bool bSetExtension = true);

	virtual ~CSG_Translator(void);

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
SAGA_API_DLL_EXPORT CSG_Translator *	SG_Get_Translator	(void);

SAGA_API_DLL_EXPORT const char *		LNG					(const char *Text);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__api_core_H
