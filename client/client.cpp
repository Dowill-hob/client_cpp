// client.cpp
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>  // InetPton 사용을 위해 필요
#include <thread>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void clientHandler(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            cout << "Server: " << buffer << endl;
        }
        else {
            break; // 서버가 종료되면 루프 종료
        }
    }
}
void Start_Page() {
    cout << "= = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl;
    cout << "=                         M E N U                     =" << endl;
    cout << "=                                                     =" << endl;
    cout << "=                      <1. Register>                  =" << endl;
    cout << "=                      <2. Login   >                  =" << endl;
    cout << "=                      <3. Exit    >                  =" << endl;
    cout << "=                                                     =" << endl;
    cout << "= = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl;
}
int main() {
    WSADATA wsa;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (result != 0) {
        cerr << "WSAStartup 실패! 오류 코드: " << result << endl;
        return 1; // 프로그램 종료
    }
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    InetPtonA(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "서버와 연결할 수 없습니다." << endl;
        return 1;
    }

    thread(clientHandler, clientSocket).detach();

    int choice = 0;
    string request, username, password, input;
    bool loggedIn = false;
    Start_Page();
    while (!loggedIn)
    {
        cout << "Choice(숫자만 입력) : ";
        if (!(cin >> choice))
        {
            cout << "잘못된 입력입니다. 숫자를 입려하세요." << endl;
            cin.clear();
            cin.ignore(100, '\n'); // 입력 버퍼 비우기
            continue;
        }
        ;
        switch (choice)
        {
        case 1: { // 회원가입
            cin.ignore();
            cout << "USERNAME: ";
            getline(cin, username);
            cout << "PASSWORD: ";
            getline(cin, password);

            request = "REGISTER " + username + " " + password;
            send(clientSocket, request.c_str(), request.size(), 0);

            char response[1024] = { 0 };
            int bytesReceived = recv(clientSocket, response, sizeof(response) - 1, 0); // ✅ 최대 1023 바이트만 받음
            if (bytesReceived > 0) {
                response[bytesReceived] = '\0'; // ✅ 안전하게 문자열 종료 처리
                cout << "서버 응답: " << response << endl;
            }
            else {
                cout << "서버 응답 없음" << endl;
            }
            Start_Page();
            break;
        }
        case 2:
            cout << "USERNAME: ";
            getline(cin, username);
            cout << "PASSWORD: ";
            getline(cin, password);
            request = "LOGIN " + username + " " + password;
            send(clientSocket, request.c_str(), request.size(), 0);

            // 서버로부터 로그인 응답 받기
            char response[1024];
            memset(response, 0, sizeof(response));
            recv(clientSocket, response, sizeof(response), 0);
            if (string(response) == "LOGIN_SUCCESS") {
                cout << "로그인 성공!" << endl;
                loggedIn = true;
            }
            else {
                cout << "로그인 실패. 다시 시도하세요." << endl;
            }
            break;
        case 3:
            cout << "프로그램을 종료합니다." << endl;
            closesocket(clientSocket);
            WSACleanup();
            return 0;
        default:
            cout << "잘못된 입력입니다. 다시 선택하세요." << endl;
            break;
        }

    }

    while (true)
    {
        cout << "메시지 입력 (종료하려면 'exit'): ";
        getline(cin, input);
        if (input == "exit") break;
        send(clientSocket, input.c_str(), request.size(), 0);
        
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}