// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include <afxsock.h>
#include <vector>
#include <string>

using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct Tower {
	int plate[3][6];
	int num;
	int numplate[3];
};

CSocket client;
Tower tower;

int typePlate;
char s_nickname[100];
char r_str[100];
int len;
string str;
bool check;


// The one and only application object
void outputTower(Tower tower);
bool checkNickname(char* str);
bool checkMove(Tower &plate, string str);

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			if (AfxSocketInit() == FALSE)
			{
				cout << "Khong the khoi tao Socket";
				return FALSE;
			}

			client.Create();
			if (client.Connect(_T("127.0.0.1"), 1234) != 0) {
				cout << "Ket noi toi Server thanh cong !!!" << endl << endl;

				//Xử lý nickname
				do {
					cout << "\nNhap nickname : ";
					gets(s_nickname);

					if (!checkNickname(s_nickname))
						cout << "\tNickname bi sai dinh dang";
					else {
						client.Send(s_nickname, 11, 0);
						strcpy(r_str, "");
						client.Receive(r_str, 18, 0);
						if (!strcmp(r_str, "Nickname bi trung")) {
							cout << "\tNickname bi trung lap";
						}
						else{
							cout << "\tDang nhap thanh cong\n";
						}
					}
				} while (!checkNickname(s_nickname) || !strcmp(r_str, "Nickname bi trung"));
				
				//Trò chơi bắt đầu
				len = client.Receive(r_str, 18, 0);
				r_str[len] = 0;
				cout << r_str << endl;

				client.Receive((char*)&tower, sizeof(tower), 0);
				outputTower(tower);

				while (1)
				{
					cout << "\nNhap : ";
					fflush(stdin);
					getline(cin, str);

					client.Send(str.c_str(), str.length());
					
					if (str.compare("end") == 0)
					{
						cout << "\tBo cuoc";
						break;
					}

					len = client.Receive(r_str, 30);
					r_str[len] = 0;
					str = r_str;
					if (str == "sai")
						cout << "\tBan da nhap sai. Vui long nhap lai\n";
					else
					{
						checkMove(tower, str);
						outputTower(tower);
					
						if (tower.numplate[0] == tower.num
							|| tower.numplate[1] == tower.num
							|| tower.numplate[2] == tower.num)
						{
							cout << "\nBan da dung. Doi nguoi choi khac thi xong" << endl;
							break;
						}
					}
				}

				/*
				int temp2;
				client.Receive((char*)&temp2, sizeof(temp2), 0);
				cout << "\nHang : " << temp2;
				
				client.Receive((char*)&temp2, sizeof(temp2), 0);
				cout << "\nDiem : " << temp2;
				*/

				len = client.Receive(r_str, 100, 0);
				r_str[len] = 0;
				cout << r_str;
	
				client.Close();
			}
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

bool checkNickname(char* str) {
	int n = strlen(str);

	if (n > 10)
		return false;

	for (int i = 0; i < n; i++) {
		if (!((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= '0' && str[i] <= '9')))
			return false;
	}
	return true;
}

void outputTower(Tower tower) {
	cout << "A: ";
	for (int i = 0; i < tower.numplate[0]; i++) {
		cout << to_string(tower.plate[0][i]) << " ";
	}
	cout << "\nB: ";
	for (int i = 0; i < tower.numplate[1]; i++) {
		cout << to_string(tower.plate[1][i]) << " ";
	}
	cout << "\nC: ";
	for (int i = 0; i < tower.numplate[2]; i++) {
		cout << to_string(tower.plate[2][i]) << " ";
	}
	cout << "\n";
}

bool checkMove(Tower &tower, string str) {
	if (str.length() > 3 || str[1] != '-')
		return false;

	typePlate = str[0] - 48;
	int column_new = str[2] - 65;

	if (column_new < 0 || column_new > 2)
		return false;

	if (tower.numplate[column_new] > 0 && typePlate >= tower.plate[column_new][tower.numplate[column_new] - 1])
		return false;

	int column_old = -1;
	for (int i = 0; i < 3; i++) {
		if (tower.numplate[i] > 0 && typePlate == tower.plate[i][tower.numplate[i] - 1])
			column_old = i;
	}

	if (column_old == -1)
		return false;

	tower.plate[column_new][tower.numplate[column_new]] = typePlate;
	tower.numplate[column_new]++;
	tower.plate[column_old][tower.numplate[column_old]] = 0;
	tower.numplate[column_old]--;

	return true;
}
