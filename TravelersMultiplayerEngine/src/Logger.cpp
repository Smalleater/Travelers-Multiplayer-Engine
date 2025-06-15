#include "TME/Logger.h"

namespace tme
{
	void Logger::LogMessage::set(const char* msg)
	{
		size_t len = strlen(msg);
		if (len >= LOGGER_MSG_SIZE - 2)
		{
			len = LOGGER_MSG_SIZE - 3;
		}

		memcpy(m_data, msg, len);
		m_data[len++] = '\r';
		m_data[len++] = '\n';
		lenght = len;
	}

	HANDLE Logger::m_file = INVALID_HANDLE_VALUE;
	HANDLE Logger::m_event = NULL;
	HANDLE Logger::m_thread = NULL;
	bool Logger::m_running = false;
	bool Logger::consoleEnable = false;

	CRITICAL_SECTION Logger::m_csQueue;
	Logger::LogMessage Logger::m_messageQueue[LOGGER_QUEUE_SIZE];
	size_t Logger::m_head = 0;
	size_t Logger::m_tail = 0;

	size_t Logger::increment(size_t idx)
	{
		return (idx + 1) % LOGGER_QUEUE_SIZE;
	}

	DWORD __stdcall Logger::threadFunc(LPVOID)
	{
		while (m_running)
		{
			DWORD waitResult = WaitForSingleObject(m_event, 1000);

			while (true)
			{
				EnterCriticalSection(&m_csQueue);
				if (m_head == m_tail)
				{
					LeaveCriticalSection(&m_csQueue);
					break;
				}

				LogMessage& msg = m_messageQueue[m_tail];
				m_tail = increment(m_tail);
				LeaveCriticalSection(&m_csQueue);

				if (m_file != INVALID_HANDLE_VALUE)
				{
					DWORD written;
					WriteFile(m_file, msg.m_data, (DWORD)msg.lenght, &written, NULL);
				}

				if (consoleEnable)
				{
					DWORD consoleOut;
					WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), msg.m_data, (DWORD)msg.lenght, &consoleOut, NULL);
				}
			}

			if (waitResult == WAIT_OBJECT_0)
			{
				ResetEvent(m_event);
			}
		}

		return 0;
	}
}
