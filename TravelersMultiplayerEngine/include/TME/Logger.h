#pragma once

#include <windows.h>
#include <cstring>

namespace tme
{
	constexpr int LOGGER_MSG_SIZE = 512;
	constexpr int LOGGER_QUEUE_SIZE = 1024;

	class Logger
	{
	public:

	private:
		struct LogMessage
		{
			char m_data[LOGGER_MSG_SIZE];
			size_t lenght = 0;

			void set(const char* msg);
		};

		static HANDLE m_file;
		static HANDLE m_event;
		static HANDLE m_thread;
		static bool m_running;
		static bool consoleEnable;

		static CRITICAL_SECTION m_csQueue;
		static LogMessage m_messageQueue[LOGGER_QUEUE_SIZE];
		static size_t m_head;
		static size_t m_tail;

		static size_t increment(size_t idx);
		static DWORD WINAPI threadFunc(LPVOID);
	};
}
