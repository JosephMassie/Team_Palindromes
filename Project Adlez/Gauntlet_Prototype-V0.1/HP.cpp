#include "HP.h"


HP::HP(void) {
	m_health = MAX_HEARTS;
	for(int i=0; i<MAX_HEARTS; ++i) {
		m_index[i] = 2;
	}
}

HP::~HP(void) {
	release();
}

// Texture notes
// count = total number of sprites in sheet
// rows = number of rows in the sheet
// columns = number of columns in the sheet
// size = the uniform size of each sprite in the sheet
void HP::initialize() {
	m_tex.initialize(L"images/health2.png", 3, 1, 4, 32);
	m_pos = V2DF(UI_LEFT, UI_TOP);
	m_angle = 0.0f;
	m_scale = 0.8f;
}

void HP::update(float hp) {
	m_health = hp/2;
	setIndex();
}

void HP::render() {
	for(int i=1; i<MAX_HEARTS+1; ++i) {
		m_tex.draw(m_index[i], V2DF(m_pos.x+((i-1)*24.0f), m_pos.y), m_angle, m_scale);
	}
}

void HP::setIndex() {
	for(int i=1; i<MAX_HEARTS+1; ++i) {
		if(m_health>=i) {
			m_index[i] = 0;
		} else {
			if(m_health<i && m_health>(i-1)) {
				m_index[i] = 1;
			} else {
				if(m_health<i) {
					m_index[i] = 2;
				}
			}
		}
	}
}