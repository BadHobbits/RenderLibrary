#pragma once
#ifdef D_EXPORT
#define D_EXP __declspec(dllexport)
#else
#define D_EXP __declspec(dllimport)
#endif
extern "C" D_EXP void Hello();


extern "C" D_EXP bool ToogleFullscreen(uint ScreenWidth=800,uint ScreenHeigh=600);
extern "C" D_EXP void InitRenderLibrary(HWND hwnd);
extern "C" D_EXP bool InitRendererDevice(uint backBufferCount = 3, uint MSAA_samples_count = 1, BOOL windowed = TRUE, uint ScreenWidth=800,uint ScreenHeigh=600);
extern "C" D_EXP bool SetViewport(float width, float height);
extern "C" D_EXP bool PrepareRender();
extern "C" D_EXP bool StartRenderScene();
extern "C" D_EXP bool EndRenderScene();
extern "C" D_EXP bool Render();
extern "C" D_EXP void CleanUpRender();
