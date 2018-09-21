﻿#include <iostream>
#include <thread>
#include <mutex>
#include <iomanip>
#include "NetCenter.h"
#include "QuoteHandler.h"
#include "Common.h"

#ifdef OM_Win32
#	include <sys/timeb.h>
#else
#	include <sys/time.h>
#	include <sys/select.h>
#   include <unistd.h>
#endif

using namespace ftq;
using namespace std;

vector<int> vTicker(2001, 0);
vector<int> vBasic(2001, 0);
vector<int> vObook(2001, 0);

mutex mtx;

ofstream fout("../CntDly.log");

void DlyCount(i32_t nTime)
{
	while (true)
	{
		// Sleep单位是ms，sleep的单位是s
		#ifdef OM_Win32
			Sleep(nTime * 60 * 1000);
		#else
			sleep(nTime * 60);
		#endif
		lock_guard<mutex> lck(mtx);

		fout << "Ticker Delay : " << endl;
		for (int i = 0; i < vTicker.size(); ++i)
		{
			fout << fixed << setprecision(6) << 0.000010 * i << "s : " << vTicker[i] << endl;
		}

		fout << "Basic Delay : " << endl;
		for (int i = 0; i < vBasic.size(); ++i)
		{
			fout << fixed << setprecision(6) << 0.000010 * i << "s : " << vBasic[i] << endl;
		}

		fout << "OrderBook Delay : " << endl;
		for (int i = 0; i < vObook.size(); ++i)
		{
			fout << fixed << setprecision(6) << 0.000010 * i << "s : " << vObook[i] << endl;
		}
		break;
	}
}

int main(int argc, const char * argv[]) {
    // insert code here...
	
	if (argc != 4)
	{
		cout << "input error!" << endl;
		return 0;
	}

	bool bIsUsTime = (string(argv[1]) == "US") ? true : false;	cout << "Area:\t" << (bIsUsTime ? "US" : "HK") << endl;
	i32_t nSubNum = atoi(argv[2]);								cout << "SubNum:\t" << nSubNum << endl;
	i32_t nPort = 8000 + nSubNum;								cout << "Port:\t" << nPort << endl;
	i32_t nTime = atoi(argv[3]);								cout << "Sleep:\t" << nTime << endl;

	QuoteHandler *pQuoteHandler = new QuoteHandler(vTicker, vBasic, vObook, mtx, bIsUsTime, nSubNum);
	NetCenter::Default()->Init(uv_default_loop());
	NetCenter::Default()->SetProtoHandler(pQuoteHandler);
	NetCenter::Default()->Connect("127.0.0.1", nPort);

	thread t(DlyCount, nTime);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	t.join();

    return 0;
}
