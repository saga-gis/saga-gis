/**********************************************************
 * Version $Id$
 *********************************************************/
////////////////////////////////////////////////////////////////////////////
// Programm    :                                                            
// Bibliothek  :                                                            
// Header-Datei: Gr_def.h                                                   
//                                                                          
////////////////////////////////////////////////////////////////////////////
//                                                                          
// Autoren        :  Joerg Dolle                                            
// 		     Christian Trachinow                                    
// 		     Stephan Westphal                                       
// Telefon-Nr.    :                                                         
// Mail-Adresse   :  jdolle@gwdg.de                                         
//                                                                          
////////////////////////////////////////////////////////////////////////////
//                                                                          
// Compiler       :  Borland C++   V.: 4.00                                 
// Ersterstellung :  09.11.95                                               
// Ueberarbeitung :  22.04.96                                               
//                                                                          
//                                                                          
// Programmversion:  2.00                                                   
//                                                                          
////////////////////////////////////////////////////////////////////////////


/****************** Include-Dateien **************/
#ifndef __gr_def__
#define __gr_def__

#include "gr_syst.h"
#include "vector.h"

#undef  HEADER_INCLUDED__DATA
#define HEADER_INCLUDED__DATA 


//DO *Typkonstrukte***************************************************
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

typedef int G_Color;

typedef struct {
	unsigned char red, green, blue;
} G_RgbColor;

typedef struct {
	char *name;
	G_Color index;
} G_NamedColors;

typedef struct {
	int NumColors;
	G_RgbColor *RgbColor;
	int NumNamedColors;
	G_NamedColors *NamedColor;
} G_ColorDef;

typedef struct {
  double x, y;
} G_Point;

//DO * Papierformate und Ausrichtungen ******
typedef enum G_PaperFormat { A0P, A0L, A1P, A1L, A2P, A2L,
											A3P, A3L, A4P, A4L, A5P, A5L};
typedef enum G_PaperOrient { Portrait, Landscape };
typedef enum G_FillType { Hollow, Blank, Filled };


//DO *RGB-Felder********************
#define RGBMAX_DOS 16
#define RGBMAX_REGENBOGEN 256
#define RGBMAX_ATLAS 256
#define RGBMAX_ATLAS_SMALL 64
#define RGBMAX_REGULAR 128
#define RGBMAX_WINDOWS 16

extern  G_RgbColor RGBDos[RGBMAX_DOS];
extern  G_RgbColor RGBRegenbogen[RGBMAX_REGENBOGEN];
extern  G_RgbColor RGBAtlas[RGBMAX_ATLAS];
extern  G_RgbColor RGBAtlasSmall[RGBMAX_ATLAS_SMALL];
extern  G_RgbColor RGBRegular[RGBMAX_REGULAR];
extern  G_RgbColor RGBWindows[RGBMAX_WINDOWS];

#define NAMEDCOLORSMAX 16
extern  G_NamedColors NamedColorsDos[NAMEDCOLORSMAX];
extern  G_NamedColors NamedColorsRegenbogen[NAMEDCOLORSMAX];
extern  G_NamedColors NamedColorsAtlas[NAMEDCOLORSMAX];
extern  G_NamedColors NamedColorsAtlasSmall[NAMEDCOLORSMAX];
extern  G_NamedColors NamedColorsRegular[NAMEDCOLORSMAX];
extern  G_NamedColors NamedColorsWindows[NAMEDCOLORSMAX];


extern  G_ColorDef ColorDefDos;
extern  G_ColorDef ColorDefRegenbogen;
extern  G_ColorDef ColorDefAtlas;
extern  G_ColorDef ColorDefAtlasSmall;
extern  G_ColorDef ColorDefRegular;
extern  G_ColorDef ColorDefWindows;


//DO * Textdefinitionen *****************
typedef enum G_TextOrientX {Left,  HCenter, Right };
typedef enum G_TextOrientY {Bottom,VCenter, Top};
typedef enum G_TextFont { TimesRoman,
						  TimesBold,
						  TimesItalic,
						  TimesBoldItalic,
						  Helvetica,
						  HelveticaBold,
						  HelveticaOblique,
						  HelveticaBoldOblique,
						  Greek,
						  Symbol };

//DO *** Clipping Type ******************
typedef enum G_ClipType {NoClip,Clip};

#define CS_TEXT_COLOR 1
#define CS_TEXT_FONT 2
#define CS_TEXT_ORIENT 4
#define CS_TEXT_ANGLE 8
#define CS_TEXT_HEIGHT 16

#define CS_POLY_WIDTH 1
#define CS_POLY_COLOR 2
#define CS_POLY_FILL 4

#define CS_POLYLINE_WIDTH 1
#define CS_POLYLINE_COLOR 2

#define CS_RECT_WIDTH 1
#define CS_RECT_COLOR 2
#define CS_RECT_FILL 4

#define CS_CIRCLE_WIDTH 1
#define CS_CIRCLE_RADIUS 2
#define CS_CIRCLE_COLOR 4
#define CS_CIRCLE_FILL 8

#define CS_TRIANGLE_WIDTH 1
#define CS_TRIANGLE_COLOR 2
#define CS_TRIANGLE_FILL 4

#define CS_LINE_WIDTH 1
#define CS_LINE_COLOR 2

typedef struct 
{
	unsigned Text : 5;
	unsigned TextVector : 5;
	unsigned Poly : 3;
	unsigned Polyline : 2;
	unsigned Rect : 3;
	unsigned Circle : 4;
	unsigned Triangle : 3;
	unsigned Line : 2;
	unsigned Palette : 1;
	void setBits(void)
	{
		Text = 31;
		TextVector = 31;
		Poly = 7;
		Polyline = 3;
		Rect = 7;
		Circle = 15;
		Triangle = 7;
		Line = 3;
	}
} ContextStatus;


//DO * RGB-Farben in einer Klasse gekapselt

class  RGBModelle
{
public:

  RGBModelle();
  ~RGBModelle();

private:
  void init_dos_colors(void);
  void init_atlas_colors(void);
  void init_small_atlas_colors(void);
  void init_regenbogen_colors(void);
  void init_regular_colors(void);
  void init_windows_colors(void);
};


#undef HEADER_INCLUDED__DATA
#define HEADER_INCLUDED__DATA

#endif

