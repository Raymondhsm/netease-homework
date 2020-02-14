#include "pch.h"
#include "StepTimer.h"

StepTimer::StepTimer():
	m_elapsedTicks(0),
	m_totalTicks(0),
	m_leftOverTicks(0),
	m_frameCount(0),
	m_framesPerSecond(0),
	m_framesThisSecond(0),
	m_qpcSecondCounter(0)
{
	QueryPerformanceFrequency(&m_qpcFrequency);
	QueryPerformanceCounter(&m_qpcLastTime);

	// 将最大增量初始化为 1/10 秒。
	m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
}

void StepTimer::ResetElapsedTime()
{
	QueryPerformanceCounter(&m_qpcLastTime);

	m_leftOverTicks = 0;
	m_framesPerSecond = 0;
	m_framesThisSecond = 0;
	m_qpcSecondCounter = 0;
}
