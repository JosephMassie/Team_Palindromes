#pragma once
#include "DX2DEngine.h"
#include "v2d.h"
#include "HelpfulData.h"

// contains the necessary information to store for a single d3d texture
// ...and draw it to the screen
class Texture
{
private:
	// graphics engine reference
	DX2DEngine* engine;
	// Texture reference
	int textureID;
	// tint of the texture
	int m_R;
	int m_G;
	int m_B;
	// values for sprite sheet
	int m_count;
	TemplateVector<RECT> m_rects;
public:
	Texture();
	~Texture();
	void initialize(LPCWSTR fileName);
	void initialize(LPCWSTR fileName, int R, int G, int B);
	void initialize(LPCWSTR fileName, int count, int rows, int columns, int size);
	void setTint(int R, int G, int B);
	void release();
	void draw(V2DF pos, float rotAngle, float scale);
	void draw(int index, V2DF pos, float rotAngle, float scale);
	int getImageCount();
};