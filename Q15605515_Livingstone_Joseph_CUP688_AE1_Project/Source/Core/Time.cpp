#include "Time.h"

std::chrono::high_resolution_clock::time_point Time::m_current_time;
std::chrono::high_resolution_clock::time_point Time::m_previous_time;
float Time::m_delta_time = 0.0f;

void Time::Update()
{
	m_previous_time = m_current_time;
	m_current_time = std::chrono::high_resolution_clock::now();
	m_delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(m_current_time - m_previous_time).count();
}