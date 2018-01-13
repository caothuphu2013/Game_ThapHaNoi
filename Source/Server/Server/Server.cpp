// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include <afxsock.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct Tower {
	int plate[3][6];
	int num;
	int numplate[3];

	Tower(bool isRand = false) {
		if (isRand)
		{
			srand(time(NULL));
			num = rand() % 3 + 4;
			for (int i = num; i >= 1; i--)
			{
				int vitri = rand() % 3;
				if (vitri == 0)
					plate[0][numplate[0]++] = i;
				if (vitri == 1)
					plate[1][numplate[1]++] = i;
				if (vitri == 2)
					plate[2][numplate[2]++] = i;
			}
		}
	
	}
};


struct Player
{
	CSocket client;
	SOCKET activate;
	string nickname;
	Tower tower;
	int point = 0;
	int rank = 0;
};

Player player[3];

int numpeople = 0;
bool check;
string str;
vector<string> nickname;
int len, typePlate;
char temp[30];
int stop = 0;
// The one and only application object
bool checkMove(Tower &plate, string str);
CWinApp theApp;

char r_nickname[30];

using namespace std;

Tower tower(true);

DWORD WINAPI process(void* para) {
	Player *player = (Player*)para;
	player->client.Attach(player->activate);

	do {
		player->client.Receive(r_nickname, 11, 0);
		check = true;
		nickname.push_back(r_nickname);
		for (int j = 0; j < nickname.size() - 1; j++) {
			if (!strcmp(r_nickname, nickname[j].c_str())) {
				player->client.Send("Nickname bi trung", 18, 0);
				nickname.pop_back();
				check = false;
			}
		}
	} while (check == false);
	player->nickname = r_nickname;
	cout << "\nNickname : " << numpeople + 1 << " : " << player->nickname;
	numpeople++;
	player->client.Send("A", 2, 0);
	player->activate = player->client.Detach();

	while (numpeople < 3);
	player->client.Attach(player->activate);
	char temp[18] = "Tro choi bat dau";
	player->client.Send(temp, 18, 0);

	player->client.Send((char*)&player->tower, sizeof(player->tower), 0);
	numpeople = 0;
	
	str = "";
	while (1)
	{
		len = player->client.Receive(temp, 30, 0);
		temp[len] = 0;
		str = temp;

		if (str.compare("end") == 0) {
			numpeople++;
			player->rank = 3;
			break;
		}

		if (checkMove(player->tower, str))
		{
			player->client.Send(str.c_str(), str.length(), 0);
			player->point++;

			if (player->tower.numplate[0] == player->tower.num
				|| player->tower.numplate[1] == player->tower.num
				|| player->tower.numplate[2] == player->tower.num)
			{
				numpeople++;
				break;
			}
		}
		else
		{
			str = "sai";
			str.push_back(0);
			player->client.Send(str.c_str(), str.length());
		}
	}
	player->activate = player->client.Detach();
	stop++;
	return 0;
}

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
			CSocket server;
			
			if (AfxSocketInit() == FALSE) {
				cout << "Khong the khoi tao Socket Libraray";
				return FALSE;
			}

			if (server.Create(1234, SOCK_STREAM, NULL) == 0) { //SOCK_STREAM or SOCK_DGRAM.
				cout << "Khoi tao that bai !!!" << endl;
				cout << server.GetLastError();
				return FALSE;
			}
			else {
				cout << "Server khoi tao thanh cong !!!" << endl;
				
				server.Listen();
				CSocket * client = new CSocket[3];
				int numPlayer = 0;

				while (1) {
					if (numPlayer < 3) {
						server.Accept(player[numPlayer].client);
						player[numPlayer].tower = tower;
						player[numPlayer].activate = player[numPlayer].client.Detach();
						CreateThread(0, 0, process, &player[numPlayer], 0, 0);
						numPlayer++;
					}

					if (stop == 3)
						break;
				}

				int point_temp[3];
				for (int i = 0; i < 3; i++) {
					point_temp[i] = player[i].point;
					if (player[i].rank == 3) {
						point_temp[i] = 1000;
					}
				}

				int *max = max_element(point_temp, point_temp + 3);
				int *min = min_element(point_temp, point_temp + 3);

				for (int i = 0; i < 3; i++) {
					if (point_temp[i] == *max) {
						player[i].rank = 3;
					}
					else if (point_temp[i] == *min) {
						player[i].rank = 1;
					}
					else {
						player[i].rank = 2;
					}
				}

				
				for (int i = 0; i < 3; i++) {
					player[i].client.Attach(player[i].activate);
					str = "\nHang : " + to_string(player[i].rank) + "\nDiem : " + to_string(player[i].point) + "\n\nTro choi ket thuc\n";
					player[i].client.Send(str.c_str(), str.length(), 0);
				}

				for (int i = 0; i < 3; i++) {
					client[i].Close();
				}
				server.Close();
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

bool checkMove(Tower &tower,string str) {
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
