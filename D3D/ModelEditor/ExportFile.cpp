#include "stdafx.h"
#include "ExportFile.h"
#include "Converter.h"

void ExportFile::Initialize()
{
	//Tank();
	//Kachujin();
	//Tower();
	//Airplane();
	//Weapons();
}

void ExportFile::Tank()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tank/Tank.fbx");
	conv->ExportMesh(L"Tank/Tank");
	conv->ExportMaterial(L"Tank/Tank", false);

	SafeDelete(conv);
}

void ExportFile::Kachujin()
{
	Converter* conv = nullptr;

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Mesh.fbx");
	conv->ExportMesh(L"Kachujin/Mesh");
	conv->ExportMaterial(L"Kachujin/Mesh", false);
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Idle.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Idle");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Walk.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Walk");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Run.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Run");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Slash.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Slash");
	SafeDelete(conv);

	conv = new Converter();
	conv->ReadFile(L"Kachujin/Bboy Uprock.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Uprock");
	SafeDelete(conv);
}

void ExportFile::Tower()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tower/Tower.fbx");
	conv->ExportMesh(L"Tower/Tower");
	conv->ExportMaterial(L"Tower/Tower");

	SafeDelete(conv);
}

void ExportFile::Airplane()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"B787/Airplane.fbx");
	conv->ExportMesh(L"B787/Airplane");
	conv->ExportMaterial(L"B787/Airplane", false);

	SafeDelete(conv);
}

void ExportFile::Weapons()
{
	vector<wstring> names;
	names.push_back(L"Cutter.FBX");
	names.push_back(L"Cutter2.FBX");
	names.push_back(L"Dagger_epic.FBX");
	names.push_back(L"Dagger_small.FBX");
	names.push_back(L"Katana.FBX");
	names.push_back(L"LongArrow.obj");
	names.push_back(L"LongBow.obj");
	names.push_back(L"Rapier.FBX");
	names.push_back(L"Sword.FBX");
	names.push_back(L"Sword_epic.FBX");
	names.push_back(L"Sword2.FBX");

	for (wstring name : names)
	{
		Converter* conv = new Converter();
		conv->ReadFile(L"Weapon/" + name);

		String::Replace(&name, L".FBX", L"");
		String::Replace(&name, L".obj", L"");

		conv->ExportMesh(L"Weapon/" + name);
		conv->ExportMaterial(L"Weapon/" + name, false);

		SafeDelete(conv);
	}
}
