#ifndef __ENGINE_H
#define __ENGINE_H

#include <windows.h>
#include <ddraw.h>
#include <math.h>

#define _8BITRGB(r,g,b)  5
#define _16BITRGB(r,g,b) (((r&255%32)<<11)+((g&255%32)<<6)+(b&255%32))
#define _24BITRGB(r,g,b) (((r&255)<<16)+((g&255)<<8)+(b&255))
#define _32BITRGB(r,g,b) (((r&255)<<16)+((g&255)<<8)+(b&255))

#define MODE_FULLSCREEN 0
#define MODE_WINDOWED	1

// Color Storage Structure
struct RGBVAL
{
	int r;
	int g;
	int b;
};

struct ENGINEDESC {

	// Surface Pointers And Variables
	ULONG *surface_buffer;

	// Window Information
	int WINDOW_WIDTH;
	int WINDOW_HEIGHT;
	int WINDOW_BPP;
	int WINDOW_MODE;
	RECT CLIENT_RECT;
	HWND hWnd;

	// Frame Status
	BOOL IS_BEGIN;
	BOOL IS_LOCKED;

	// Device Capabilities
	DDCAPS ddscaps_HAL;
	DDCAPS ddscaps_HEL;
};

// GRAPHICS ENGINE CLASS DEFINITION //
class ENGINE
{
	friend class PIXEL_OBJECT;
	friend class LINE_OBJECT;
	friend class IMAGE_OBJECT;
	friend class POLYGON_OBJECT;
	friend class SPRITE_OBJECT;

private:
	ENGINEDESC   enginedesc;
	ENGINE	     *engine;

	BOOL GetDisplayCaps();
	
public:
	// Global Surface Pointers and Variables this allows user access to DD
	LPDIRECTDRAW         lpDD;
	LPDIRECTDRAWSURFACE  lpDDPrimary, lpDDSecondary;
	DDSURFACEDESC		 ddsd;
	DDSCAPS 			 ddscaps;

	LPDIRECTDRAWCLIPPER  lpDDClipper;
	
	// Graphics Engine Initialization and Shutdown Utility Functions
	BOOL Startup(HWND hWnd, int MODE, int width, int height, int depth);
	BOOL ShutDown();
	
	// Frame Functions
	BOOL Begin_Frame();
	BOOL End_Frame();
	BOOL Lock_Frame();
	BOOL Unlock_Frame();
	BOOL Show_Frame();

	// Hardware Enumeration Calls
	BOOL GetDisplayDevice();
	BOOL SetDisplayDevice();

	// OBJECT CREATION FUNCTIONS
	PIXEL_OBJECT *CreatePixelObject();
	BOOL DestroyPixelObject(PIXEL_OBJECT *pixel_obj);

	LINE_OBJECT *CreateLineObject();
	BOOL DestroyLineObject(LINE_OBJECT *line_obj);

	IMAGE_OBJECT *CreateImageObject();
	BOOL DestroyImageObject(IMAGE_OBJECT *image_obj);

	SPRITE_OBJECT *CreateSpriteObject(IMAGE_OBJECT *image_obj);
	BOOL DestroySpriteObject(SPRITE_OBJECT *sprite_obj);

	POLYGON_OBJECT *CreatePolygonObject();
	BOOL DestroyPolygonObject(POLYGON_OBJECT *polygon_obj);
};

// PIXEL OBJECT CLASS DEFINITION //
struct PIXELDESC {

	// Screen Coordinates
	int x;
	int y;

	// Color Value After Conversion
	ULONG Color;
	// RGB Value of Color
	RGBVAL rgb;
};

// PIXEL OBJECT CLASS DEFINITION //
class PIXEL_OBJECT
{
	friend class ENGINE;

private:

	PIXELDESC pixeldesc;
	ENGINE    *engine;

	BOOL __fastcall ClipPixel();

public:
	// Graphic Routines
	BOOL __fastcall DrawPixel(int x, int y, int r, int g, int b);
	BOOL __fastcall DrawPixel(int x, int y);
	BOOL __fastcall DrawPixel();

	void GetPixelPosition(int *x, int *y);
	void GetPixelColor(int *r, int *g, int *b);

	void SetPixelColor(int r, int g, int b);
	void SetPixelPosition(int x, int y);
	void MovePixel(int dx, int dy);

	void SetClippingRectangle(int x, int y, int x1, int y1);
};

// LINE OBJECT CLASS DEFINITION /
struct LINEDESC
{
	ULONG Color;
	int x;
	int y;
	int x1;
	int y1;

	int r;
	int g;
	int b;
};

class LINE_OBJECT
{
	friend class ENGINE;

private:

	BOOL ClipLine();

	LINEDESC linedesc;
	ENGINE   *engine;

public:
	BOOL __fastcall DrawLine(int x, int y, int x1, int y1, int r, int g, int b);
	BOOL __fastcall DrawLine(int x, int y, int x1, int y1);
	BOOL __fastcall DrawLine();

	void SetLinePosition(int x, int y, int x1, int y1);
	void GetLinePosition(int *x, int *y, int *x1, int *y1);

	void SetLineColor(int r, int g, int b);
	void GetLineColor(int *r, int *g, int *b);

	void MoveLine(int dx, int dy);
	void StretchLine(int dx, int dy);

	void ScaleLine(float scale_factor);

	void SetClippingRectangle(int x, int y, int x1, int y1);
};

// POLYGON OBJECT CLASS DESCRIPTION //
struct POLYGONDESC
{
	struct VERTEX
	{
		float x, y;  // 2d vertex
	};

	struct POLYGON
	{
		ULONG b_color;
		ULONG i_color;
		int closed;
		int filled;
		float lxo, lyo;
		int num_vertices;
		VERTEX vertices[50];
	};
	POLYGON polygon;  // Polygon Object
	LINE_OBJECT edge; // Edge Object
};

class POLYGON_OBJECT
{
	friend class ENGINE;
private:

	POLYGONDESC polygondesc;
	ENGINE *engine;

public:
	void FillPolygon();
	void DrawPoly();
	void TranslatePolygon(float dx, float dy);
	void ScalePolygon(float scale);
	void RotatePolygon(float rotate_angle);
	void AddVertex(float x, float y);
	void DefinePolygon(int closed, int filled);
	void SetPolyEdgeColor(int r, int g, int b);
	void SetPolyFillColor(int r, int g, int b);
	void SetOrigin(float x, float y);
	void GetOrigin(float *x, float *y);
	void GetVertex(float *x, float *y, int vertex);
	BOOL SetVertex(float x, float y, int vertex);
};

// BITMAP OBJECT CLASS DEFINITION //
struct IMAGEDESC
{

	// Image Variables
	RECT Dest_rect;
	RECT Src_rect;
	RECT Clip_rect;
	
	// Status Flags
	BOOL IS_STRECHED;
	BOOL IS_LOCKED;

	float angle;
	int dx, dy;
	int x, y;
	int width, height;
	
	// Blitting Effects
	DDBLTFX bltfx;

	// Bitmap Load and storage
	LPCTSTR filename;
	HBITMAP hbm;
	BITMAP bm;

	LPDIRECTDRAWSURFACE lpDDS; // Bitmap surface
    DDSURFACEDESC      ddsd;
};

class IMAGE_OBJECT
{
	friend class ENGINE;

private:

	IMAGEDESC imagedesc;
	ENGINE     *engine;

	LPDIRECTDRAWSURFACE DDLoadBitmap(LPDIRECTDRAW pdd, LPCTSTR tszBitmap, int dx, int dy);
	HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE pdds, HBITMAP hbm, int x, int y, int dx, int dy);
	HRESULT DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb);
	DWORD DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb);

	BOOL ClipImage();

public:
	BOOL __fastcall DrawImageFullScreen();
	BOOL __fastcall DrawImageStreched(int x, int y, int dx, int dy);
	BOOL __fastcall DrawImageStreched(int dx, int dy);
	BOOL __fastcall DrawImage(int x, int y);
	BOOL __fastcall DrawImage();

	BOOL ALoadImage(LPCTSTR filename, int width, int height);
	BOOL RestoreImage(void);

	void SetImagePosition(int x, int y);
	void StretchImage(int dx, int dy);
	void SetFrameRectangle(int left, int top, int right, int bottom);
	BOOL SetTransparentColor(int r, int g, int b);
};

// SPRITE OBJECT CLASS DEFINITION //
struct SPRITEDESC
{
	// Relative Screen location of sprite (upper lefthand corner pixel)
	int x;
	int y;

	// Sprite Scaling Factors
	int dx;
	int dy;
	BOOL IS_SCALED;

	// Center of Sprite (center of bounding square "default")
	int x_origin;
	int y_origin;

	// Frame Area 
	RECT frame_area[50];
	int  frame_width[50];
	int  frame_height[50];

	// Frame Screen Clipping Information
	int Screen_width;
	int Screen_height;

	// Custom Clipping Area Information
	RECT clipping_region;
	BOOL IS_CLIPPED;

	// Frame Rate Information
	int FRAME_MSPF;  // milliseconds per frame
	int FRAME_TIMER;
	int FRAME_INDEX;
	int FRAME_COUNT;
	DWORD timer;
	float mspf;  // milliseconds per frame

	// Frame Rate Timer Defs
	float milliseconds;
	LARGE_INTEGER timerfreq;
	LARGE_INTEGER starttime;
	LARGE_INTEGER currenttime;

	// Animation Flags
	BOOL AUTO_ANIM;


	//Settings
	DWORD anim_mode;
};

class SPRITE_OBJECT
{
	friend class ENGINE;

private:
	SPRITEDESC	 spritedesc;

	IMAGE_OBJECT *frames_image; 
	ENGINE		 *engine;

public:
	
	// Drawing Functions
	BOOL DrawSprite(int x, int y);
	BOOL DrawSprite();

	// Animation Controls
	BOOL AddAnimationFrame(int left, int top, int right, int bottom);

	// User Controlled Animation
	void DeleteFrame(int frame);
	void JumpToFrame(int frame);
	void StepFrame(void);
	void SetFrameRate(float mspf);

	// Auto Animation
	void AutoAnimateOn(void);
	void AutoAnimateOff(void);
	void RepeatFrame(void);

	// Get Current Frame
	void GetCurrentFrame(int *frame);

	// Animation Modes
	void SetAnimationMode(int MODE);
	void AddAnimationMode(void);
	void RemoveAnimationMode(int MODE);

	// Auto TileSet Handling
	BOOL CreateSpriteTileSet(int CellWidth, int CellHeight, int NumCellsPerRow, int NumRows);
	
	BOOL SetFrameImage(IMAGE_OBJECT *IMAGE_OBJECT);

	void SetClippingRectangle(int left, int top, int right, int bottom);
	void SetCollisionRectangle(void);
	void SetCollisionPolygon(POLYGON_OBJECT *polygon);

	void ScaleSprite(int dx, int dy);
	void MoveSprite(int dx, int dy);
	void SetSpritePosition(int x, int y);
	void RotateSprite(float rotate_angle);

	void GetCollisionRectangle(void);
	void GetSpritePosition(int *x, int *y);
	void GetSpriteArea(int *x, int *y, int *x1, int *y1);
	void GetSpriteHotspot(void);

	BOOL SetSpriteCenter(int x, int y);
	BOOL AddHotSpot(int x, int y);
	void DefineSprite();

};

// LAYER OBJECT CLASS DEFINITION//
struct LAYERDESC
{
};

class LAYER_OBJECT
{
private:

};

// MAP OBJECT CLASS DEFINITION //
struct MAPDESC
{
};

class MAP_OBJECT
{
private:
};

// TEXT OBJECT CLASS DEFINITION //
struct TEXT_DESC
{
};

class TEXT_OBJECT
{
private:
public:
	char string[50];
	void PrintMessage(float x, float y);
};

#endif