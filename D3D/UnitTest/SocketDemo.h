#pragma once
#include "Systems/IExecute.h"

#define MAX_MESSAGE_CHARACTERS 1024

class SocketDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override {};
	virtual void Update() override {};
	virtual void PreRender() override {};
	virtual void Render() override {};
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void StartWinSock();
	void InputUserID();
	void ConnetToServer();

private:
	void SendMessageToServer();

private:
	sockaddr_in serverInfo;
	SOCKET serverSocket;

	char UserID[10];
	char buffer[MAX_MESSAGE_CHARACTERS + 1]; //엔터를 치기 전 작성중인 메세지
	char message[MAX_MESSAGE_CHARACTERS]; //엔터치면 저장되는 최종 메세지
	
	const char* escape = "/quit";
	bool bQuit = false;
};