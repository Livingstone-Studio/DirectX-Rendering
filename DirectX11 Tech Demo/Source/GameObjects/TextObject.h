#pragma once

#include <SimpleMath.h>
#include <string>

using namespace DirectX;

class TextObject
{
public:
	TextObject(std::wstring text, SimpleMath::Vector2 screen_position, float text_scale);
	~TextObject();

	void SetText(std::wstring text) { m_text = text; }
	const std::wstring& GetText() { return m_text; }

	void SetScreenPosition(SimpleMath::Vector2 screen_position) { m_screen_position = screen_position; }
	const SimpleMath::Vector2& GetScreenPosition() { return m_screen_position; }

	void SetScale(float text_scale) { m_text_scale = text_scale; }
	const float& GetScale() { return m_text_scale; }

protected:
	std::wstring m_text = L"Placeholder Text";
	SimpleMath::Vector2 m_screen_position = { 0,0 };
	float m_text_scale = 10.0f;
};

