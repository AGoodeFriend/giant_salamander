#include "type.h"
#include <winsock2.h>
#include <windows.h>   

namespace LPPCDEVICE
{
	class HPETTimer
	{
	public:
		HPETTimer() : m_elapsed(0)
		{
			QueryPerformanceFrequency(&m_freq);
		}
		~HPETTimer() {}
	public:
		void start()
		{
			QueryPerformanceFrequency(&m_freq);
			if (!QueryPerformanceCounter(&m_begin_time))
			{
				std::cout << " get begin_time error" << std::endl;
			}
		}
		void stop()
		{
			LARGE_INTEGER end_time;
			QueryPerformanceFrequency(&m_freq);
			if (!QueryPerformanceCounter(&end_time))
			{
				std::cout << " get end_time error" << std::endl;
			}
			m_elapsed = ((end_time.QuadPart - m_begin_time.QuadPart) * 1000000) / m_freq.QuadPart;
		}
		void restart()
		{
			m_elapsed = 0;
			start();
		}
		long long getDuration()
		{
			stop();
			return m_elapsed;
		}
		double elapsed_ms()
		{
			return m_elapsed / 1000.0;
		}
		double elapsed_second()
		{
			return m_elapsed / 1000000.0;
		}

	private:
		LARGE_INTEGER m_freq;
		LARGE_INTEGER m_begin_time;
		long long m_elapsed;
	};
}