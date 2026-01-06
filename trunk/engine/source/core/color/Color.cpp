#include "Color.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Color);
Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);
Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
Color Color::Gray(0.5f, 0.5f, 0.5f, 1.0f);
Color Color::Yellow(1.0f, 0.92f, 0.016f, 1.0f);
Color Color::Clear(0.0f, 0.0f, 0.0f, 0.0f);
DC_END_NAMESPACE
