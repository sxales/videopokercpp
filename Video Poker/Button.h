#pragma once

#include <string>
#include "Rect.h"
#include "LWindow.h"
#include "ResourceManager.h"
#include "BitmapFont.h"

class Button : public Rect {
public:
	std::string txt = "Error";
	bool over = false;
	bool enabled = true;

	Button() {}
	Button(std::string _txt, int _x, int _y, int _width, int _height) {
		txt = _txt;
		over = false;
		enabled = true;

		x = _x;
		y = _y;
		width = _width;
		height = _height;
	}

	bool check(int _x, int _y) {
		if (overlaps(Rect(_x, _y, 1, 1))) over = true;
		else over = false;

		return over;
	}
};