#pragma once
struct strCoords {
	int x;
	int y;

	friend bool operator < (strCoords a, strCoords b) { return std::make_pair(a.x, a.y) < std::make_pair(b.x, b.y); }
};