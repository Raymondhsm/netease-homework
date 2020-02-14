#pragma once
#include "pch.h"

class StepTimer
{
public:
	StepTimer();

	void ResetElapsedTime();

	// 更新计时器状态，调用适当次数的指定 Update 函数。
	template<typename TUpdate>
	inline void Tick(const TUpdate& update)
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

	// 获取上一次 Update 调用后的运行时间。
	__int64 GetElapsedTicks() const { return m_elapsedTicks; }
	double GetElapsedSeconds() const { return TicksToSeconds(m_elapsedTicks); }

	// 获取程序启动之后的总时间。
	__int64 GetTotalTicks() const { return m_totalTicks; }
	double GetTotalSeconds() const { return TicksToSeconds(m_totalTicks); }

	// 获取自程序启动之后的更新次数。
	__int32 GetFrameCount() const { return m_frameCount; }

	// 获取当前帧速率。
	__int32 GetFramesPerSecond() const { return m_framesPerSecond; }

	// 整数格式使用每秒 10,000,000 次滴答来表示时间。
	static const __int64 TicksPerSecond = 10000000;

	static double TicksToSeconds(__int64 ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
	static __int64 SecondsToTicks(double seconds) { return static_cast<__int64>(seconds * TicksPerSecond); }

private:
	// 源计时数据使用 QPC 单位。
	LARGE_INTEGER m_qpcFrequency;
	LARGE_INTEGER m_qpcLastTime;
	__int64 m_qpcMaxDelta;

	// 派生的计时数据使用规范的计时格式。
	__int64 m_elapsedTicks;
	__int64 m_totalTicks;
	__int64 m_leftOverTicks;

	// 用于跟踪帧速率的成员。
	__int32 m_frameCount;
	__int32 m_framesPerSecond;
	__int32 m_framesThisSecond;
	__int64 m_qpcSecondCounter;
};


