#include "stdafx.h"
#include "Editor.h"
#include "Brush.h"

void Editor::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(126, 35, 39);
	((Freedom*)Context::Get()->GetCamera())->Speed(100.f);
	
	heightMapFileDirectory = L"Terrain/";
	heightMapFileName = L"Gray256.png";

	shader = new Shader(L"44_Brush.fxo");
	shadow = new Shadow(shader, Vector3(128, 0, 128), 256);

	sky = new CubeSky(L"Environment/Mountain1024.dds", shader);
	brush = new Brush(shader, terrain);

	openFunc = bind(&Editor::OpenComplete, this, placeholders::_1);

	wstring path = L"Terrain/Gray256.dds";
	terrain = new Terrain(shader, path);
	terrain->BaseMap(L"Terrain/Dirt.png");
	terrain->LayerMap(L"Terrain/Grass (Lawn).jpg");
}

void Editor::Destroy()
{
	SafeDelete(shader);
	SafeDelete(shadow);
	SafeDelete(sky);
	SafeDelete(terrain);
	SafeDelete(brush);
}

void Editor::Update()
{
	//램버트 테스트
	ImGui::SliderFloat3("LightDirection", Lighting::Get()->Direction(), -1, +1);
	
	ImGui::Separator();

	//파일 다이알로그
	{
		if (ImGui::Button("Open HeightMap") == true)
		{
			Path::OpenFileDialog(heightMapFileName, Path::ImageFilter, heightMapFileDirectory, openFunc);

			SafeDelete(heightMapFile);
			heightMapFile = new Texture(heightMapFileDirectory + heightMapFileName);
		}
	}

	//콤보박스 - 채널 선택
	const char* channels[] = { "Red", "Green", "Blue", "Alpha" };
	ImGui::Combo("Channel", &selectedChannel, channels, 4);
	
	//*.dds로 저장
	if (ImGui::Button("Convert To DDS"))
	{
		ToDataMapFile();
	}

	//선택된 높이맵을 임구이 이미지 뷰어로 띄우기
	if (heightMapFile != nullptr)
		ImGui::Image(heightMapFile->SRV(), ImVec2(128, 128));

	ImGui::Separator();
	if (ImGui::CollapsingHeader("DDS Map List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (UINT i = 0; i < dataMapFileList.size(); i++)
		{
			if (ImGui::Button(String::ToString(dataMapFileList[i]).c_str(), ImVec2(200, 0)))
			{
				SafeDelete(terrain);
				SafeDelete(brush);

				wstring path = L"Terrain/" + dataMapFileList[i] + L".dds";
				terrain = new Terrain(shader, path);
				terrain->BaseMap(L"Terrain/Dirt.png");
				terrain->LayerMap(L"Terrain/Grass (Lawn).jpg");

				brush = new Brush(shader, terrain);
			}
		}
	}
	ImGui::Separator();

	if (terrain != nullptr)
	{
		brush->Update();
		terrain->Update();
	}

	sky->Update();
	UpdateDataMapFileList();
}

void Editor::Render()
{
	sky->Pass(4);
	sky->Render();

	if (terrain != nullptr)
	{
		brush->Render();

		terrain->Pass(8);
		terrain->Render();
	}
}

void Editor::OpenComplete(wstring fileName)
{
	heightMapFileName = Path::GetFileName(fileName);
}

void Editor::ToDataMapFile()
{
	if (heightMapFile == nullptr) return;

	//한번 생성된 Texture의 desc은 수정이 불가하므로 desc을 복사해줌
	ID3D11Texture2D* srcTexture = heightMapFile->GetTexture();
	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);

	//원본 높이맵 -> readTexture로 복사
	ID3D11Texture2D* readTexture;
	D3D11_TEXTURE2D_DESC readDesc;
	ZeroMemory(&readDesc, sizeof(D3D11_TEXTURE2D_DESC));
	readDesc.Width = srcDesc.Width;
	readDesc.Height = srcDesc.Height;
	readDesc.ArraySize = 1;
	readDesc.Format = srcDesc.Format;
	readDesc.MipLevels = 1;
	readDesc.SampleDesc = srcDesc.SampleDesc;
	readDesc.Usage = D3D11_USAGE_STAGING;
	readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Check(D3D::GetDevice()->CreateTexture2D(&readDesc, nullptr, &readTexture));
	D3D::GetDC()->CopyResource(readTexture, srcTexture);

	UINT* pixels = new UINT[readDesc.Width * readDesc.Height];
	D3D11_MAPPED_SUBRESOURCE subResouce;
	D3D::GetDC()->Map(readTexture, 0, D3D11_MAP_READ, 0, &subResouce);
	{
		memcpy(pixels, subResouce.pData, sizeof(UINT) * readDesc.Width * readDesc.Height);
	}
	D3D::GetDC()->Unmap(readTexture, 0);

	UINT* heights = new UINT[readDesc.Width * readDesc.Height];
	for (UINT i = 0; i < readDesc.Width * readDesc.Height; i++)
	{
		UINT pixel = pixels[i];
		UINT result;

		EChannelType channel = (EChannelType)selectedChannel;
		switch (channel)
		{
			case Editor::EChannelType::Alpha:	result = (pixel & 0xFF000000) >> 24;	break;
			case Editor::EChannelType::Blue:	result = (pixel & 0x00FF0000) >> 16;	break;
			case Editor::EChannelType::Green:	result = (pixel & 0x0000FF00) >> 8;		break;
			case Editor::EChannelType::Red:		result = (pixel & 0x000000FF) >> 0;		break;
		}

		heights[i] = result << 24;
	}

	ID3D11Texture2D* saveTexture;
	D3D11_TEXTURE2D_DESC saveDesc;
	ZeroMemory(&saveDesc, sizeof(D3D11_TEXTURE2D_DESC));
	saveDesc.Width = readDesc.Width;
	saveDesc.Height = readDesc.Height;
	saveDesc.ArraySize = 1;
	saveDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	saveDesc.MipLevels = 1;
	saveDesc.SampleDesc = readDesc.SampleDesc;
	saveDesc.Usage = D3D11_USAGE_STAGING;
	saveDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA saveSubResource;
	saveSubResource.pSysMem = heights;
	saveSubResource.SysMemPitch = sizeof(UINT) * readDesc.Width;
	saveSubResource.SysMemSlicePitch = sizeof(UINT) * readDesc.Width * readDesc.Height;
	Check(D3D::GetDevice()->CreateTexture2D(&saveDesc, &saveSubResource, &saveTexture));

	wstring fileName = Path::GetFileNameWithoutExtension(heightMapFile->GetFile());
	fileName = L"../../_Textures/Terrain/" + fileName + L".dds";
	Check(D3DX11SaveTextureToFile(D3D::GetDC(), saveTexture, D3DX11_IFF_DDS, fileName.c_str()));

	SafeRelease(srcTexture);
	SafeRelease(readTexture);
	SafeRelease(saveTexture);

	SafeDeleteArray(heights);
	SafeDeleteArray(pixels);
}

void Editor::UpdateDataMapFileList()
{
	dataMapFileList.clear();
	Path::GetFiles(&dataMapFileList, L"../../_Textures/Terrain/", L"*.dds", false);

	for (wstring& file : dataMapFileList)
		file = Path::GetFileNameWithoutExtension(file);

}

