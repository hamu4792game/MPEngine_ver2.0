#pragma once
#include <fstream>
#include <sstream>
#include <windows.h>
#include <timeapi.h>
#pragma comment(lib,"winmm.lib")

class TimeBaseLoopExecuter
{
public:
	TimeBaseLoopExecuter(double frame = 60.0):FRAME_TIME(1.0f / frame)
	{
		//周波数取得
		QueryPerformanceFrequency(&mTimeFreq);
		//計測開始時間の初期化
		QueryPerformanceCounter(&mTimeStart);
		QueryPerformanceCounter(&mTimeEnd);
	}
	//FPSを固定するため処理時間に余裕がある場合その分待つ
	double TimeAdjustment();
private:
	const double FRAME_TIME = 0.0;
	//計測開始時間
	LARGE_INTEGER mTimeStart;
	//計測終了時間
	LARGE_INTEGER mTimeEnd;
	//計測周波数
	LARGE_INTEGER mTimeFreq;
};

