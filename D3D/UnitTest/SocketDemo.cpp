#include "stdafx.h"
#include "SocketDemo.h"

void SocketDemo::Initialize()
{
	//�ڿ� �غ�
	StartWinSock();
	InputUserID();
	ConnetToServer();

	//�۽�(������)
	thread t(bind(&SocketDemo::SendMessageToServer, this));

	//����
	int size = 0;
	while (bQuit == false)
	{
		ZeroMemory(buffer, sizeof(buffer));

		if ((size = recv(serverSocket, buffer, MAX_MESSAGE_CHARACTERS, 0)) == INVALID_SOCKET)
		{
			printf("�޼��� ���� ����\n");
			exit(0);
		}
		else
		{
			buffer[size] = '\0';
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN);
			printf("%s\n", buffer);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		}

	}

	//����
	t.join();
	closesocket(serverSocket);
	WSACleanup();
}

void SocketDemo::StartWinSock()
{
	WSADATA wsa;
	int message = WSAStartup(MAKEWORD(2, 2), &wsa);
	assert(message == 0);
}

void SocketDemo::InputUserID()
{
	printf("ID �Է� : ");
	gets_s(UserID);
}

void SocketDemo::ConnetToServer()
{
	//����
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("���� ���� ����");
		exit(0);
	}

	//���� ����
	ZeroMemory(&serverInfo, sizeof(sockaddr_in));
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.S_un.S_addr = inet_addr("27.96.131.171");
	serverInfo.sin_port = htons(9999);

	if (connect(serverSocket, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) < 0)
	{
		printf("���� ���� ����\n");
		exit(0);
	}
	else
		printf("������ �����\n");
}

void SocketDemo::SendMessageToServer()
{
	while (true)
	{
		if (fgets(buffer, MAX_MESSAGE_CHARACTERS, stdin))
		{
			//�޼��� ����
			buffer[strlen(buffer)] = '\0';
			sprintf(message, "[%s] : %s", UserID, buffer);

			//���� �۽�
			if (send(serverSocket, message, strlen(message), 0) < 0)
				printf("�޼��� �۽� ����\n");

			//���� ���
			if (strstr(message, escape) != 0)
			{
				printf("������? �ȳ�~~~~~~\n");
				bQuit = true;
				exit(0);
			}
		}
	}
}
