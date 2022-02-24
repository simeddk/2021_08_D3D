#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"
#include "FrameworkDemo.h"
#include "LightingDemo.h"
#include "NormalMapDemo.h"
#include "AreaLightingDemo.h"
#include "ObbDemo.h"
#include "BillboardDemo.h"
#include "WeatherDemo.h"
#include "RtvDemo.h"
#include "PostProcessDemo.h"
#include "MrtDemo.h"
#include "BlurDemo.h"
#include "BloomDemo.h"
#include "EnvCubeDemo.h"
#include "ProjectionTextureDemo.h"
#include "DepthDemo.h"
#include "ShadowDemo.h"
#include "TessellationTriangleDemo.h"
#include "TessellationRectDemo.h"
#include "TessellationLodDemo.h"
#include "TerrainLODDemo.h"
#include "FrustumDemo.h"


void Main::Initialize()
{
	Push(new TerrainLODDemo());
}


void Main::Destroy()
{
	for (IExecute* exe : executes)
	{
		exe->Destroy();
		SafeDelete(exe);
	}
}

void Main::Update()
{
	for (IExecute* exe : executes)
		exe->Update();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->PreRender();
}

void Main::Render()
{
	for (IExecute* exe : executes)
		exe->Render();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
		exe->PostRender();
}

void Main::ResizeScreen()
{
	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
{
	executes.push_back(execute);

	execute->Initialize();
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);
	D3D::SetDesc(desc);

	Main* main = new Main();
	WPARAM wParam = Window::Run(main);

	SafeDelete(main);

	return wParam;
}