 /*
 * gCanvas.h
 *
 *  Created on: April 11, 2026
 *      Author: Burak Eroglu
 */

#ifndef GCANVAS_H_
#define GCANVAS_H_

#include "gBaseCanvas.h"
#include "gApp.h"
#include "gImage.h"
#include <vector>
#include <algorithm>
#include <cstdlib>


struct Obstacle {
	float x, y, w, h;
};

struct BuildingInstance {
	int imageIndex;
	int col, row;
	float worldx, worldy;
};


class gCanvas : public gBaseCanvas {
public:
	gCanvas(gApp* root);
	virtual ~gCanvas();

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void charPressed(unsigned int codepoint);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int x, int y);
	void mouseEntered();
	void mouseExited();
	void windowResized(int w, int h);

	void showNotify();
	void hideNotify();

private:
	static const int KEY_NONE = 0, KEY_W = 1, KEY_S = 2, KEY_A = 4, KEY_D = 8;

	// Character direction constants
	static const int DIR_COUNT = 8;

	// Grass tile constants
	static const int MAP_ROWS = 100;
	static const int MAP_COLS = 100;
	static const int TILE_W = 37;
	static const int TILE_H = 17;
	static const int TILE_OFFSET_X = 31;
	static const int TILE_OFFSET_Y = 17;

	// Water tile constants
	static const int WATER_TILE_W = 100;
	static const int WATER_TILE_H = 50;
	static const int WATER_OFFSET_X = 50;
	static const int WATER_OFFSET_Y = 25;

	// Building constants
	static const int BUILDING_COUNT = 24;

	void moveCharacter();
	void moveCamera();
	bool canMoveTo(float testcx, float testcy);
	void placeBuildings();
	void drawWater();
	void drawMap();
	void drawEntities();
	void drawCharacter();

	gApp* root;
	gImage grasstile;
	gImage watertile;
	gImage buildings[BUILDING_COUNT];
	gImage character[DIR_COUNT];
	std::vector<BuildingInstance> buildingInstances;
	int characterdir;
	float charw, charh;
	float mapbasex, mapbasey;
	float cx, cy;
	float cdx, cdy;
	float crot;
	int keystate;
	float camx, camy;
	int camtopmargin, cambottommargin;
	float camleftmargin, camrightmargin;
	float mincamx, mincamy, maxcamx, maxcamy;
	std::vector<Obstacle> obstacles;
};

#endif /* GCANVAS_H_ */
