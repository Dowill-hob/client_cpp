#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <ws2tcpip.h>  
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

SOCKET client_socket;

// 서버로부터 메시지를 수신하는 함수
void receiveMessages() {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            cout << buffer << endl;
        }
    }
}

// 초기 메뉴 화면
void Start_Page() {
    cout << "= = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl;
    cout << "=                         M E N U                     =" << endl;
    cout << "=                                                     =" << endl;
    cout << "=                     <1. 회원 가입 >                 =" << endl;
    cout << "=                     <2. 로그인    >                 =" << endl;
    cout << "=                     <3. 회원 탈퇴 >                 =" << endl;
    cout << "=                     <4. 채팅 내역 >                 =" << endl;
    cout << "=                     <5. 종료      >                 =" << endl;
    cout << "=                                                     =" << endl;
    cout << "= = = = = = = = = = = = = = = = = = = = = = = = = = = =" << endl;
}

int main() {
    WSADATA wsaData;
    sockaddr_in server_addr;
    char buffer[1024];
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup 실패! 오류 코드: " << result << endl;
        return 1; // 프로그램 종료
    }
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cout << "Connection failed!" << endl;
        return 1;
    }

    cout << "Connected to server!" << endl;
    Start_Page();

    thread receiveThread(receiveMessages); // 서버로부터 메시지를 수신하는 스레드 실행

    while (true) {
        memset(buffer, 0, sizeof(buffer));  // 버퍼 초기화
        string choice;
        cout << "Choose an option: ";
        cin >> choice;
        send(client_socket, choice.c_str(), choice.size(), 0);

        if (choice == "1") { // 회원가입
            string username, password;
            cin >> username;
            send(client_socket, username.c_str(), username.size(), 0);

            cin >> password;
            send(client_socket, password.c_str(), password.size(), 0);
            Sleep(1000);
        }
        else if (choice == "2") { // 로그인
            string username, password;
            char buffer[1024] = { 0 };  // 버퍼 초기화
            cin >> username;
            send(client_socket, username.c_str(), username.size(), 0);

            // 비밀번호 입력 및 전송
            cin >> password;
            send(client_socket, password.c_str(), password.size(), 0);

            // 서버 응답 수신
            memset(buffer, 0, sizeof(buffer));  // 버퍼 초기화
            int bytes_received = recv(client_socket, buffer, sizeof(buffer) -1 , 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0'; // 문자열로 변환
                string server_response(buffer);
                /*cout << "[DEBUG] : " << server_response << endl;*/
                if (server_response.find("You can now start chatting") != string::npos) {
                    cout << "Logged in successfully! You can now start chatting." << endl;
                    string message;
                    cin.ignore();  // 입력 버퍼 비우기

                    while (true) {
                        cout << "[Enter message]: ";
                        getline(cin, message);

                        send(client_socket, message.c_str(), message.size(), 0);
                        if (message == "exit") { Sleep(1000); break; }
                        
                    }
                }
                else {
                    cout << "Login failed. Please try again." << endl;
                }


            }
        }

        else if (choice == "3") { // 회원 탈퇴
            string username;
            cin >> username;
            send(client_socket, username.c_str(), username.size(), 0);
            Sleep(1000);
        }
        else if (choice == "4") { // 채팅 내역 검색
            string username;
            cin >> username;
            send(client_socket, username.c_str(), username.size(), 0);
            Sleep(1000);
        }
        else if (choice == "5") { // 종료
            cout << "System down good bye!" << endl;
            receiveThread.join();
            break;
        }
    }
    closesocket(client_socket);
    WSACleanup();
    return 0;
}