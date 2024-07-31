#include "Time.h"

std::chrono::steady_clock::time_point Time::m_current_time = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Time::m_previous_time = std::chrono::steady_clock::now();
double Time::m_delta_time = 0.0f;

void Time::Update()
{
	m_previous_time = m_current_time;
	m_current_time = std::chrono::steady_clock::now();
	m_delta_time = std::chrono::duration_cast<std::chrono::duration<double>>(m_current_time - m_previous_time).count();
}