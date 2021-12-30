#include "stdafx.h"
#include "SocketDemo.h"

void SocketDemo::Initialize()
{
	//자원 준비
	StartWinSock();
	InputUserID();
	ConnetToServer();

	//송신(스레드)
	thread t(bind(&SocketDemo::SendMessageToServer, this));

	//수신
	int size = 0;
	while (bQuit == false)
	{
		ZeroMemory(buffer, sizeof(buffer));

		if ((size = recv(serverSocket, buffer, MAX_MESSAGE_CHARACTERS, 0)) == INVALID_SOCKET)
		{
			printf("메세지 수신 에러\n");
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

	//종료
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
	printf("ID 입력 : ");
	gets_s(UserID);
}

void SocketDemo::ConnetToServer()
{
	//소켓
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("소켓 생성 실패");
		exit(0);
	}

	//서버 연결
	ZeroMemory(&serverInfo, sizeof(sockaddr_in));
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.S_un.S_addr = inet_addr("27.96.131.171");
	serverInfo.sin_port = htons(9999);

	if (connect(serverSocket, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) < 0)
	{
		printf("서버 연결 실패\n");
		exit(0);
	}
	else
		printf("서버에 연결됨\n");
}

void SocketDemo::SendMessageToServer()
{
	while (true)
	{
		if (fgets(buffer, MAX_MESSAGE_CHARACTERS, stdin))
		{
			//메세지 저장
			buffer[strlen(buffer)] = '\0';
			sprintf(message, "[%s] : %s", UserID, buffer);

			//실제 송신
			if (send(serverSocket, message, strlen(message), 0) < 0)
				printf("메세지 송신 에러\n");

			//종료 명령
			if (strstr(message, escape) != 0)
			{
				printf("갈려구? 안녕~~~~~~\n");
				bQuit = true;
				exit(0);
			}
		}
	}
}
