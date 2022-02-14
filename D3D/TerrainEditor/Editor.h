#pragma once
#include "Systems/IExecute.h"

class Editor : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void ToDataMapFile(); //���̸� �ҷ����� -> ���̰� ����� ä�� ���� -> *.dds�� ����
	//Todo

private:
	Shader* shader;
	Shadow* shadow;

	CubeSky* sky;

private:
	wstring heightMapFileDirectory;
	wstring heightMapFileName;
	Texture* heightMapFile = nullptr;

private:
	enum class EChannelType
	{
		Red, Green, Blue, Alpha, Max
	};
	int selectedChannel = 0;

	vector<wstring> dataMapFileList;
};