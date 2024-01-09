#include "Prop.h"

Prop::Prop(std::string meshPath, Transform transform)
	: GameObject(meshPath, "", transform)
{
}

Prop::~Prop()
{
}
