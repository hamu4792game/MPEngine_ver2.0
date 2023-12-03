#include "TimeBaseLoopExecuter.h"

double TimeBaseLoopExecuter::TimeAdjustment()
{
	// 今の時間を取得
	QueryPerformanceCounter(&mTimeEnd);
	// (今の時間 - 前フレームの時間) / 周波数 = 経過時間(秒単位)
	double frameTime = static_cast<double>(mTimeEnd.QuadPart - mTimeStart.QuadPart) / static_cast<double>(mTimeFreq.QuadPart);
	double fps = 0.0;
	//処理時間に余裕がある場合はその分待つ
	if (frameTime < FRAME_TIME)
	{
		//Sleepの時間を計算
		DWORD sleepTime = static_cast<DWORD>((FRAME_TIME - frameTime) * 1000.0);
		timeBeginPeriod(1);
		//寝る
		Sleep(sleepTime);
		timeEndPeriod(1);
	}
	fps = 1 / frameTime;

	//代入大事
	mTimeStart = mTimeEnd;
	
	return fps;
}
