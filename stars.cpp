// stars.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "resource.h"
#include <winreg.h>
#include "display.h"
#include "starobject.h"
#include "mmengine.h"
#include<string>
using namespace std;
#include"planets.h"

MMENGINE engine;
DISPLAY_ENGINE *dengine;

#define start_none	0
#define start_set	1
#define start_saver 2
#define but_ok 0
#define but_cancel 1
#define but_apply 2
#define but_help 3
#define but_det1 4
#define but_det2 5
#define but_det3 6
#define but_speed 8
#define but_plan 7
#define but_fps 9
#define but_timer 10

HWND hwnd;
HINSTANCE hinstance;
HWND buttons[16];
bool planetsyn;
int detail;	//1 = low,2 = med,3 = high
int speed; // 7 to 25
LARGE_INTEGER timerfreq;
LARGE_INTEGER starttime;
LARGE_INTEGER currenttime;	
unsigned int numfstars;
forestar *fstar;
int nummoves = 0;	
int width,height;
TEXTURE_OBJECT *starpic;	
bool done = false;
TEXT_OBJECT *frametext;
vector<TEXTURE_OBJECT*> planet_textures;
planets *planet;
bool fpsyn;
bool timeryn;
bool dont_iterate = false;
int mousexi = 0;
int mouseyi = 0;
bool mouse_first = true;

void setbuttons()
{
	if(planetsyn) SendMessage(buttons[but_plan],BM_SETCHECK,1,0);
	else SendMessage(buttons[but_plan],BM_SETCHECK,0,0);
	
	if(fpsyn) SendMessage(buttons[but_fps],BM_SETCHECK,1,0);
	else SendMessage(buttons[but_fps],BM_SETCHECK,0,0);

	if(timeryn) SendMessage(buttons[but_timer],BM_SETCHECK,1,0);
	else SendMessage(buttons[but_timer],BM_SETCHECK,0,0);
	
	if(detail == 1) 
	{
		SendMessage(buttons[but_det1],BM_SETCHECK,1,0);
		SendMessage(buttons[but_det2],BM_SETCHECK,0,0);
		SendMessage(buttons[but_det3],BM_SETCHECK,0,0);
	}
	if(detail == 2) 
	{
		SendMessage(buttons[but_det1],BM_SETCHECK,0,0);
		SendMessage(buttons[but_det2],BM_SETCHECK,1,0);
		SendMessage(buttons[but_det3],BM_SETCHECK,0,0);
	}
	if(detail == 3) 
	{
		SendMessage(buttons[but_det1],BM_SETCHECK,0,0);
		SendMessage(buttons[but_det2],BM_SETCHECK,0,0);
		SendMessage(buttons[but_det3],BM_SETCHECK,1,0);
	}


	SetScrollPos(buttons[but_speed],SB_CTL,speed,true);
}

void saveregistry()
{
	HKEY skey;
	unsigned long disp;
	RegCreateKeyEx(HKEY_CURRENT_USER,"Software",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&skey,&disp);
	RegCreateKeyEx(skey,"hpt_interactive",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&skey,&disp);
	RegCreateKeyEx(skey,"stars",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&skey,&disp);
	RegSetValueEx(skey,"planets",0,REG_DWORD,(unsigned char*)(&planetsyn),sizeof(int));
	RegSetValueEx(skey,"fps",0,REG_DWORD,(unsigned char*)(&fpsyn),sizeof(int));
	RegSetValueEx(skey,"timer",0,REG_DWORD,(unsigned char*)(&timeryn),sizeof(int));
	RegSetValueEx(skey,"detail",0,REG_DWORD,(unsigned char*)(&detail),sizeof(int));
	RegSetValueEx(skey,"speed",0,REG_DWORD,(unsigned char*)(&speed),sizeof(int));
	RegCloseKey(skey);
}

void loadregistry()
{
	HKEY skey;
	unsigned long size,type;
	size = sizeof(int);
	if((RegOpenKeyEx(HKEY_CURRENT_USER,"Software",0,KEY_ALL_ACCESS,&skey)) != ERROR_SUCCESS)
		return;
	RegOpenKeyEx(skey,"hpt_interactive",0,KEY_ALL_ACCESS,&skey);
	if((RegOpenKeyEx(skey,"stars",0,KEY_ALL_ACCESS,&skey)) != ERROR_SUCCESS)
		return;
	RegQueryValueEx(skey,"planets",NULL,NULL,(unsigned char*)(&planetsyn),&size);
	RegQueryValueEx(skey,"fps",NULL,NULL,(unsigned char*)(&fpsyn),&size);
	RegQueryValueEx(skey,"timer",NULL,NULL,(unsigned char*)(&timeryn),&size);
	RegQueryValueEx(skey,"detail",NULL,&type,(unsigned char*)(&detail),&size);
	RegQueryValueEx(skey,"speed",NULL,&type,(unsigned char*)(&speed),&size);
	type = REG_SZ; size = 11;
	RegCloseKey(skey);
}





BOOL CALLBACK aboutboxproc(HWND hdlg,unsigned int message,WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
	case WM_INITDIALOG:
		return true;
	case WM_COMMAND:
		switch(LOWORD(wparam))
		{
		case IDOK:
			EndDialog(hdlg,0);
			return true;
		}
	}
	return false;
}


LRESULT CALLBACK WndProcset(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static int xwidth,ywidth;
	TEXTMETRIC tm;
	switch(message)
	{
		case WM_CREATE:
			RECT temp;
			GetWindowRect(hwnd,&temp);
			hdc = GetDC(hwnd);
			GetTextMetrics(hdc,&tm);
			xwidth = tm.tmAveCharWidth;
			ywidth = tm.tmHeight + tm.tmExternalLeading;
			ReleaseDC(hwnd,hdc);
			MoveWindow(hwnd,temp.left,temp.top,xwidth*50,ywidth*19,true);
			CreateWindow("STATIC","Starfield Density",WS_CHILD | WS_VISIBLE | SS_LEFT,xwidth*2,ywidth*8,xwidth*17,ywidth,hwnd,NULL,hinstance,NULL);
			CreateWindow("STATIC","SPEED",WS_CHILD | WS_VISIBLE | SS_LEFT,xwidth*6,ywidth*3,xwidth*8,ywidth,hwnd,NULL,hinstance,NULL);
			CreateWindow("STATIC","RESOLUTION",WS_CHILD | WS_VISIBLE | SS_LEFT,xwidth*26,(int)(ywidth*0.5),xwidth*14,ywidth,hwnd,NULL,hinstance,NULL);
			buttons[but_ok] = CreateWindow("BUTTON","OK",WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,xwidth*2,ywidth*15,xwidth*7,(int)(ywidth*1.5),hwnd,(HMENU)but_ok,hinstance,NULL);
			buttons[but_cancel] = CreateWindow("BUTTON","CANCEL",WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,xwidth*11,ywidth*15,xwidth*10,(int)(ywidth*1.5),hwnd,(HMENU)but_cancel,hinstance,NULL);
			buttons[but_apply] = CreateWindow("BUTTON","APPLY",WS_DISABLED | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,xwidth*24,ywidth*15,xwidth*10,(int)(ywidth*1.5),hwnd,(HMENU)but_apply,hinstance,NULL);
			buttons[but_plan] = CreateWindow("BUTTON","Enable Planets",WS_CHILD | WS_VISIBLE | BS_CHECKBOX,xwidth*2,(int)(ywidth*0.5),xwidth*18,ywidth,hwnd,(HMENU)but_plan,hinstance,NULL);
			buttons[but_fps] = CreateWindow("BUTTON","Enable Frame Counter",WS_CHILD | WS_VISIBLE | BS_CHECKBOX,xwidth*22,(int)(ywidth*0.5),xwidth*26,ywidth,hwnd,(HMENU)but_fps,hinstance,NULL);
			buttons[but_timer] = CreateWindow("BUTTON","Save CPU Power",WS_CHILD | WS_VISIBLE | BS_CHECKBOX,xwidth*22,(int)(ywidth*2.5),xwidth*26,ywidth,hwnd,(HMENU)but_timer,hinstance,NULL);
			buttons[but_help] = CreateWindow("BUTTON","ABOUT",WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,xwidth*37,ywidth*15,xwidth*9,(int)(ywidth*1.5),hwnd,(HMENU)but_help,hinstance,NULL);
			buttons[but_det1] = CreateWindow("BUTTON","Low",WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,xwidth*2,ywidth*9,xwidth*10,ywidth,hwnd,(HMENU)but_det1,hinstance,NULL);
			buttons[but_det2] = CreateWindow("BUTTON","Medium",WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,xwidth*2,ywidth*10,xwidth*10,ywidth,hwnd,(HMENU)but_det2,hinstance,NULL);
			buttons[but_det3] = CreateWindow("BUTTON","High",WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,xwidth*2,ywidth*11,xwidth*10,ywidth,hwnd,(HMENU)but_det3,hinstance,NULL);
			buttons[but_speed] = CreateWindow("SCROLLBAR",NULL,WS_CHILD | WS_VISIBLE | SBS_HORZ,xwidth*2,(int)(ywidth*4.5),xwidth*15,ywidth*2,hwnd,(HMENU)but_speed,hinstance,NULL);
			SetScrollRange(buttons[but_speed],SB_CTL,7,25,true);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			InvalidateRect(hwnd,NULL,TRUE);
			hdc = BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
			return 0;
		case WM_HSCROLL:
			switch(LOWORD(wParam))
			{
			case SB_TOP:
				EnableWindow(buttons[but_apply],true);
				speed = 7;
				setbuttons();
				return 0;
			case SB_BOTTOM:
				EnableWindow(buttons[but_apply],true);
				speed = 25;
				setbuttons();
				return 0;
			case SB_LINELEFT:
				EnableWindow(buttons[but_apply],true);
				speed--;
				if(speed < 7) speed = 7;
				setbuttons();
				return 0;
			case SB_LINERIGHT:
				EnableWindow(buttons[but_apply],true);
				speed++;
				if(speed > 25) speed = 25;
				setbuttons();
				return 0;
			case SB_PAGELEFT:
				EnableWindow(buttons[but_apply],true);
				speed -= 5;
				if(speed < 7) speed = 7;
				setbuttons();
				return 0;
			case SB_PAGERIGHT:
				EnableWindow(buttons[but_apply],true);
				speed += 5;
				if(speed > 25) speed = 25;
				setbuttons();
				return 0;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				EnableWindow(buttons[but_apply],true);
				speed = HIWORD(wParam);
				setbuttons();
				return 0;
			}

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case but_ok:
				saveregistry();
				PostQuitMessage(0);
				return 0;
			case but_cancel:
				PostQuitMessage(0);
				return 0;
			case but_apply:
				EnableWindow(buttons[but_apply],false);
				saveregistry();
				return 0;
			case but_help:
				DialogBox(hinstance,"about_box",hwnd,aboutboxproc);
				return 0;
			case but_plan:
				EnableWindow(buttons[but_apply],true);
				planetsyn = !planetsyn;
				setbuttons();
				return 0;
			case but_fps:
				EnableWindow(buttons[but_apply],true);
				fpsyn = !fpsyn;
				setbuttons();
				return 0;
			case but_timer:
				EnableWindow(buttons[but_apply],true);
				timeryn = !timeryn;
				setbuttons();
				return 0;
			case but_det1:
				EnableWindow(buttons[but_apply],true);
				detail = 1;
				setbuttons();
				return 0;
			case but_det2:
				EnableWindow(buttons[but_apply],true);
				detail = 2;
				setbuttons();
				return 0;
			case but_det3:
				EnableWindow(buttons[but_apply],true);
				detail = 3;
				setbuttons();
				return 0;
	
			}

	}

	
	return DefWindowProc(hwnd,message,wParam,lParam);
}

void iterate()
{
	if(dont_iterate) return;
	static float frameratef = 0;
	static int numframes = 0;
	static string framerate = "0 fps";
	static bool planetyn = false;
	static float planet_timeout = 5;
	static float speed_factor = 2000;
			float timepassed;
			int count;
			QueryPerformanceCounter(&currenttime);
			timepassed = (currenttime.QuadPart - starttime.QuadPart)/(float)timerfreq.QuadPart;
			starttime.QuadPart = currenttime.QuadPart;
			frameratef += timepassed;
			if(!planetyn) planet_timeout -= timepassed;
			if(planet_timeout < 0) 
			{
				planetyn = true;
				planet->createnewplanet(false);
				planet_timeout = (rand() % 60000)/1000.0f + 60;
			}

			if(planetyn)
			{
				if(speed_factor > 100) speed_factor -= timepassed*6.5f*speed;
			}
			else
			{
				if(speed_factor < 2000) speed_factor += timepassed*6.5f*speed;
			}
			timepassed *= speed*speed_factor;

			numframes++;
			if(numframes >= 10)
			{
				char temp[50];
				numframes = 0;
				if(frameratef != 0.0f) frameratef = 10 / frameratef;
				else frameratef = 9999.0f;
				sprintf(temp,"%.3f fps",frameratef);
				frametext->SetText(temp);
				frameratef = 0;

			
			}
			
		for(count = 0;count<numfstars;count+=10)
			{
				fstar[count].movestar(timepassed);
				fstar[count].project();
				fstar[count+1].movestar(timepassed);
				fstar[count+1].project();
				fstar[count+2].movestar(timepassed);
				fstar[count+2].project();
				fstar[count+3].movestar(timepassed);
				fstar[count+3].project();
				fstar[count+4].movestar(timepassed);
				fstar[count+4].project();
				fstar[count+5].movestar(timepassed);
				fstar[count+5].project();
				fstar[count+6].movestar(timepassed);
				fstar[count+6].project();
				fstar[count+7].movestar(timepassed);
				fstar[count+7].project();
				fstar[count+8].movestar(timepassed);
				fstar[count+8].project();
				fstar[count+9].movestar(timepassed);
				fstar[count+9].project();
			}
			if(planetyn)
			{
				planet->movestar(timepassed);
				planet->project();
			}
		dengine->Begin_Frame();

	
				for(count = 0;count<numfstars;count+=10)
			{
				fstar[count].drawstar();
				fstar[count+1].drawstar();
				fstar[count+2].drawstar();
				fstar[count+3].drawstar();
				fstar[count+4].drawstar();
				fstar[count+5].drawstar();
				fstar[count+6].drawstar();
				fstar[count+7].drawstar();
				fstar[count+8].drawstar();
				fstar[count+9].drawstar();
			}
			if(planetyn)	planet->drawstar();

			if(fpsyn) frametext->Render();
			dengine->End_Frame();
			if(planet->IsDone()) planetyn = false;
}


LRESULT CALLBACK WndProcsaver(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			done = true;
			return false;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN: 
		case WM_RBUTTONDOWN: 
		case WM_KEYDOWN:
			PostQuitMessage(0);
			done = true;
			return false;
		case WM_TIMER:
			iterate();
			return false;
		case WM_MOUSEMOVE:
			if(mouse_first)
			{
				mouse_first = false;
				mousexi = LOWORD(lParam);
				mouseyi = HIWORD(lParam);
			}
			else
			{
				if((abs(LOWORD(lParam) - mousexi) > 150) || (abs(HIWORD(lParam) - mouseyi) > 150))			
				{
					PostQuitMessage(0);
					done = true;
				}
			}
			//else nummoves++;
			return false;
		case WM_SYSCOMMAND:
			if(wParam == SC_MONITORPOWER) 
			{
				dont_iterate = true;
				return false;
			}
		
			
}
return DefWindowProc(hwnd,message,wParam,lParam);
}


int howstart(char *startparam)
{
	if((*startparam) == NULL) return start_set;
	int temp = start_none;
	while((*startparam) != NULL)
	{
		switch (tolower(*startparam))
		{
		case 's':
			temp = start_saver;
			break;
		case 'c':
			temp = start_set;
			break;
		}
		startparam++;
	}


	return temp;
}



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
//	MessageBox(NULL,lpCmdLine,"error",MB_OK);
 	HANDLE mutex;
 char name[6] = "stars";
 name[5] = 0;
 mutex = CreateMutex(NULL,TRUE,name);
 if(GetLastError() == ERROR_ALREADY_EXISTS) return 0;
  char tempsd[270];
  GetSystemDirectory(tempsd,270);
  SetCurrentDirectory(tempsd);
	 planetsyn = false;
	hinstance = hInstance;
	int saverstart;
	saverstart = howstart(lpCmdLine);
	if(saverstart == start_saver)
	{
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING,true,NULL,0);
		loadregistry();
		speed *= 2;
		width = GetSystemMetrics(SM_CXSCREEN); 
		height = GetSystemMetrics(SM_CYSCREEN);

		HDC hdc;
		hdc = GetDC(hwnd);
		ReleaseDC(hwnd,hdc);
		MSG msg;
		char appname[6];
		strcpy(appname,"stars");
		WNDCLASS wc;
		wc.style = CS_OWNDC | CS_NOCLOSE;
		wc.lpfnWndProc = WndProcsaver;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(NULL,IDC_ARROW);
		wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = appname;
		RegisterClass(&wc);
		hwnd = CreateWindowEx(WS_EX_TOPMOST,appname,NULL,WS_POPUP,0,0,width,height,NULL,NULL,hinstance,NULL);
		ShowWindow(hwnd,SW_SHOW);
		SetFocus(hwnd);
		ShowCursor(false);
		dengine = engine.CreateDisplayEngine();
	    if(dengine->Startup(hwnd, 75,true) == FALSE)
			MessageBox(hwnd, "UNABLE TO CREATE D3D WINDOW", "ERROR", MB_OK);
		
		frametext = dengine->CreateTextObject();
		frametext->SetFont(.1f,.1f);
		frametext->SetRect(1.2f,-.5f,1.2f,1.2f);
		frametext->SetColor(1,1,1,1);
		frametext->SetText("test");
		if(detail == 1) 
		{
			numfstars = 2000;
		}
		if(detail == 2) 
		{
			numfstars = 5000;
		}
		if(detail == 3) 
		{
			numfstars = 10000;
		}
		loadstarimages();
	
		fstar = new forestar[numfstars];
		planet = new planets();
		fstar[0].setres(width,height);
		unsigned int count,count2;
		for(count = 0;count<numfstars;count++)
			{
				fstar[count].setres(width,height);
			}
		planet->setres(width,height);
		for(count2 = 0;count2 < (numfstars);count2++)
		{
			for(count = 0;count<(count2/100);count++)
			{
				fstar[count].movestar(speed*0.001f);
				fstar[count].project();
			}
		}
		QueryPerformanceFrequency(&timerfreq);
		QueryPerformanceCounter(&starttime);
		while(PeekMessage(&msg,NULL,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_REMOVE) != 0)
		{
		}
		if(timeryn) SetTimer(hwnd,1,30,NULL);
		while(!done)
			{
				if(timeryn)
				{
					if(GetMessage(&msg,NULL,0,0))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				else
				{
					if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					iterate();
				}
			}
			if(timeryn) KillTimer(hwnd,1);
			delete [] fstar;
			delete planet;
			destroystarimages();
			dengine->Shutdown();
		ShowCursor(true);
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING,false,NULL,0);
	}
	if(saverstart == start_set)
	{
		MSG msg;
		char appname[6];
		loadregistry();
		strcpy(appname,"stars");
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProcset;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));
		wc.hCursor = LoadCursor(NULL,IDC_ARROW);
		wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = appname;
		RegisterClass(&wc);
		hwnd = CreateWindowEx(WS_EX_TOPMOST,appname,"Stars Setup",WS_OVERLAPPEDWINDOW ,CW_USEDEFAULT,CW_USEDEFAULT,350,300,NULL,NULL,hinstance,NULL);
		ShowWindow(hwnd,SW_SHOW);
		SetFocus(hwnd);
		UpdateWindow(hwnd);
		setbuttons();
		while(GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	
	}
	ReleaseMutex(mutex);
	CloseHandle(mutex);
	return 0;
}



