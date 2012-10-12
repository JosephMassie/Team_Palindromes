#include "Gem.h"

Gem::Gem(void) {
	m_count = 0;
}

Gem::~Gem(void) {
	release();
}

void Gem::initialize() {
	engine = DX2DEngine::getInstance();
	m_tex.initialize(L"images/gem.png");
	m_pos = V2DF(UI_LEFT, UI_TOP+GRID_SIZE);
	m_angle = 0.0f;
	m_scale = 0.8f;
}

void Gem::render() {
	m_tex.draw(m_pos, m_angle, m_scale);
}

void Gem::drawText() {
	char buffer[500] = {0};
	sprintf(buffer, "x%d", m_count);
	RECT rect;
	rect.top = UI_TOP+20;
	rect.left = UI_LEFT+20;
	rect.bottom = 600;	
	rect.right = 800;
	engine->writeText(buffer, rect);
}
