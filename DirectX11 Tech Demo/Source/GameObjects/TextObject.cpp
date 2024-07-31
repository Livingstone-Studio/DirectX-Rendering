#include "TextObject.h"

#include "../Rendering/Renderer.h"

TextObject::TextObject(std::wstring text, SimpleMath::Vector2 screen_position, float text_scale)
	:m_text{ text }, m_screen_position{ screen_position }, m_text_scale { text_scale }
{
	Renderer::Instance->AddTextObject(this);
}

TextObject::~TextObject()
{
}
