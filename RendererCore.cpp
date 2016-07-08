#include <SDKDDKVer.h>
#define PONGO_EXPORT
#define D_EXPORT
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:


#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <DxErr.h>

#include <fstream>



#include "Utils.h"
#include "Core.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxerr.lib")

#ifdef _DEBUG
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#endif

#pragma comment(lib,"Core.lib")
#pragma comment(lib,"Utils.lib")


#define HANDLE_D3D_HR(x){\
	if(FAILED(x))\
	{\
		handleError(x);\
		return false;\
	}\
}


using namespace std;
using namespace PongoEngine;
#include "RenderLibrary.h"

PongoCoreHelper helper;
HWND						m_d3dxWindowDescriptor;
// global declarations
LPDIRECT3D9					m_d3d9;				// the pointer to our Direct3D interface
LPDIRECT3DDEVICE9			m_d3d9device;		// the pointer to the device class

bool						m_SceneStarted=false;
bool						m_RenderStarted=false;
bool						m_windowed=false;

uint						m_backbufCount=3;
uint						m_msaa=0;

void handleError(HRESULT hr)
{
	wstr err = L"Error: ";
	err.append(DXGetErrorStringW(hr)).append(L" error description: ").append(DXGetErrorDescriptionW(hr));
	err.append(L"\n");
	throw std::exception("render error");
	helper.Warning(err);
}

extern "C" D_EXP void InitRenderLibrary(HWND hwnd){ 
	m_d3dxWindowDescriptor=hwnd;
	m_d3d9=0;
	m_d3d9device=0;

}

extern "C" D_EXP bool ToogleFullscreen(uint ScreenWidth,uint ScreenHeigh)
{
	if(!m_d3d9)
		return false;
	PONGO_SAFE_RELEASE(m_d3d9device);
	if(m_d3dxWindowDescriptor==0)
		return false;
	m_windowed = !m_windowed;

	D3DPRESENT_PARAMETERS d;
	ZeroMemory(&d,sizeof(d));
	d.BackBufferCount = m_backbufCount;
	d.Windowed = m_windowed;
	d.hDeviceWindow = m_d3dxWindowDescriptor;
	d.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d.MultiSampleType = (D3DMULTISAMPLE_TYPE)m_msaa;
	d.BackBufferHeight = ScreenHeigh;
	d.BackBufferWidth = ScreenWidth;
	d.BackBufferFormat = D3DFMT_X8R8G8B8;

	auto hr = m_d3d9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_d3dxWindowDescriptor,D3DCREATE_HARDWARE_VERTEXPROCESSING,&d,&m_d3d9device);
	HANDLE_D3D_HR(hr);

	return true;
}


extern "C" D_EXP	bool InitRendererDevice(uint backBufferCount, uint MSAA_samples_count, BOOL windowed, uint ScreenWidth,uint ScreenHeigh)
{

	if(m_d3dxWindowDescriptor==0)
		return false;
	m_windowed = windowed;
	m_backbufCount = backBufferCount;
	m_msaa = MSAA_samples_count;

	D3DPRESENT_PARAMETERS d;
	ZeroMemory(&d,sizeof(d));
	d.BackBufferCount = backBufferCount;
	d.Windowed = windowed;
	d.hDeviceWindow = m_d3dxWindowDescriptor;
	d.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d.MultiSampleType = (D3DMULTISAMPLE_TYPE)MSAA_samples_count;
	d.BackBufferHeight = ScreenHeigh;
	d.BackBufferWidth = ScreenWidth;
	d.BackBufferFormat = D3DFMT_X8R8G8B8;

	m_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_d3d9)
		return false;

	auto hr = m_d3d9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_d3dxWindowDescriptor,D3DCREATE_HARDWARE_VERTEXPROCESSING,&d,&m_d3d9device);
	HANDLE_D3D_HR(hr);

	return true;
}

extern "C" D_EXP bool SetViewport(float width, float height)
{
	if (!m_d3d9||!m_d3d9device)
		return false;
	D3DVIEWPORT9 view;
	view.Height = height;
	view.Width = width;
	view.X = view.Y = 0;
	view.MinZ = 0.0f;
	view.MaxZ = 1.0f;
	auto hr = m_d3d9device->SetViewport(&view);
	HANDLE_D3D_HR(hr);
	return true;
}

extern "C" D_EXP bool StartRenderScene()
{
	if(!m_RenderStarted||m_SceneStarted)
		return false;
	m_SceneStarted = true;
	auto hr = m_d3d9device->BeginScene();
	HANDLE_D3D_HR(hr);
	return true;
}

extern "C" D_EXP bool EndRenderScene()
{
	if(!m_RenderStarted||!m_SceneStarted)
		return false;
	m_SceneStarted = false;
	auto hr = m_d3d9device->EndScene();
	HANDLE_D3D_HR(hr);
	return true;
}

extern "C" D_EXP bool PrepareRender()
{
	if (!m_d3d9device || !m_d3d9)
		return false;
		// this is the function used to render a single frame
			// clear the back buffer to a deep blue
	if(m_RenderStarted)
		if(!EndRenderScene())
			return false;
	auto hr = m_d3d9device->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,40,100),1,0);
	m_RenderStarted = true;
	m_SceneStarted = false;
		// do 3D rendering on the back buffer here
	HANDLE_D3D_HR(hr);

	return true;
}

extern "C" D_EXP bool Render()
{
	if (!m_d3d9device || !m_d3d9)
		return false;
	if(!m_RenderStarted)
		if(!PrepareRender())
			return false;
	if(m_SceneStarted)
		if(!EndRenderScene())
			return false;
		// switch the back buffer and the front buffer
	auto hr = m_d3d9device->Present(0,0,0,0);
	m_RenderStarted = false;
	m_SceneStarted = false;
	HANDLE_D3D_HR(hr);
	return true;
}

	// this is the function that cleans up Direct3D and COM
extern "C" D_EXP void CleanUpRender()
{
		// close and release all existing COM objects
	PONGO_SAFE_RELEASE(m_d3d9device);
	PONGO_SAFE_RELEASE(m_d3d9);
}


extern "C" D_EXP void Hello()
{
	MessageBoxW(0,L"Привет",L"Большой привет",0);
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
