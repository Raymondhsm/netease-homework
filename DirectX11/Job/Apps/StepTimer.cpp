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

template<typename TUpdate>
inline void StepTimer::Tick(const TUpdate & update)
{
	// 查询当前时间。
	LARGE_INTEGER currentTime;

	QueryPerformanceCounter(&currentTime);

	__int64 timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;

	m_qpcLastTime = currentTime;
	m_qpcSecondCounter += timeDelta;

	// 固定过大的时间增量(例如， 在调试器中暂停之后)。
	if (timeDelta > m_qpcMaxDelta)
	{
		timeDelta = m_qpcMaxDelta;
	}

	// 将 QPC 单位转换为规范的计时格式。这会因为上一次固定而无法溢出。
	timeDelta *= TicksPerSecond;
	timeDelta /= m_qpcFrequency.QuadPart;

	__int32 lastFrameCount = m_frameCount;

	
	m_elapsedTicks = timeDelta;
	m_totalTicks += timeDelta;
	m_leftOverTicks = 0;
	m_frameCount++;

	update();
	

	// 跟踪当前帧速率。
	if (m_frameCount != lastFrameCount)
	{
		m_framesThisSecond++;
	}

	if (m_qpcSecondCounter >= static_cast<__int64>(m_qpcFrequency.QuadPart))
	{
		m_framesPerSecond = m_framesThisSecond;
		m_framesThisSecond = 0;
		m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
	}
}