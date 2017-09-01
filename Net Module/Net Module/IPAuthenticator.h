#pragma once
#include "UDPSocket.h"
#include "EPacket.h"
#include <map>
#include <chrono>
#include <atomic>
#include <future>

#define CONNECTION_KEY (uint32_t)5274

using namespace std::chrono;

class IPAuthenticator
{
public:
	IPAuthenticator();
	~IPAuthenticator();
	bool IsTimedOut(uint32_t IP);
private:

	void CountdownTimeouts();
	float m_timeoutTime = 1.f * 60.f;
	high_resolution_clock::time_point t1;
	high_resolution_clock::time_point t2;
	std::map<uint32_t, std::atomic<float>> m_secondsLeft;
	std::map<uint32_t, std::atomic<uint16_t>> m_attemptsLeft;

	std::atomic<bool> m_closeThread;
	std::future<void> m_asyncThread;
};
