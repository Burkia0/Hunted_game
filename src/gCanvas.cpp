/*
* gCanvas.cpp
*
*  Created on: April 11, 2026
*      Author: Burak Eroglu
*/

#include "gCanvas.h"


gCanvas::gCanvas(gApp* root) : gBaseCanvas(root) {
	this->root = root;
}

gCanvas::~gCanvas() {
}

void gCanvas::setup() {
	grasstile.loadImage("oyun/tiles/grass.png");
	watertile.loadImage("oyun/tiles/water.png");
	buildings[0].loadImage("oyun/buildings/binaa.png");
	buildings[1].loadImage("oyun/buildings/binab.png");
	buildings[2].loadImage("oyun/buildings/binac.png");
	buildings[3].loadImage("oyun/buildings/binad.png");
	buildings[4].loadImage("oyun/buildings/binae.png");
	buildings[5].loadImage("oyun/buildings/binaf.png");
	buildings[6].loadImage("oyun/buildings/binag.png");
	buildings[7].loadImage("oyun/buildings/binah.png");
	buildings[8].loadImage("oyun/buildings/binai.png");
	buildings[9].loadImage("oyun/buildings/binaj.png");
	buildings[10].loadImage("oyun/buildings/binak.png");
	buildings[11].loadImage("oyun/buildings/binal.png");
	character[0].loadImage("oyun/characters/player/karakter_kuzey.png");
	character[1].loadImage("oyun/characters/player/karakter_kuzeydogu.png");
	character[2].loadImage("oyun/characters/player/karakter_dogu.png");
	character[3].loadImage("oyun/characters/player/karakter_guneydogu.png");
	character[4].loadImage("oyun/characters/player/karakter_guney.png");
	character[5].loadImage("oyun/characters/player/karakter_guneybati.png");
	character[6].loadImage("oyun/characters/player/karakter_bati.png");
	character[7].loadImage("oyun/characters/player/karakter_kuzeybati.png");
	ammo[0].loadImage("oyun/ammos/ammo_kuzey.png");
	ammo[1].loadImage("oyun/ammos/ammo_kuzeydogu.png");
	ammo[2].loadImage("oyun/ammos/ammo_dogu.png");
	ammo[3].loadImage("oyun/ammos/ammo_guneydogu.png");
	ammo[4].loadImage("oyun/ammos/ammo_guney.png");
	ammo[5].loadImage("oyun/ammos/ammo_guneybati.png");
	ammo[6].loadImage("oyun/ammos/ammo_bati.png");
	ammo[7].loadImage("oyun/ammos/ammo_kuzeybati.png");
	charw = character[0].getWidth() / 4.0f;
	charh = character[0].getHeight() / 4.0f;
	cx = (getWidth() - charw) / 2;
	cy = (getHeight() - charh) / 2;
	crot = 270.0f;
	characterdir = 0;
	keystate = KEY_NONE;

	// Compute map world origin so the leftmost pixel is at world x=0
	float halfw = TILE_W / 2.0f;
	float halfh = TILE_H / 2.0f;
	mapbasex = TILE_OFFSET_X + (MAP_ROWS - 1) * halfw;
	mapbasey = 100.0f;

	// Compute map world extent for camera limits
	float mapwidth = mapbasex + (MAP_COLS - 1) * halfw - TILE_OFFSET_X + grasstile.getWidth();
	float mapheight = mapbasey + ((MAP_COLS - 1) + (MAP_ROWS - 1)) * halfh - TILE_OFFSET_Y + grasstile.getHeight();

	// Start camera centered horizontally on map, top of map visible
	camx = mapbasex - getWidth() / 2.0f;
	camy = 0.0f;

	camleftmargin = getWidth() * 3 / 10;
	camrightmargin = getWidth() * 7 / 10;
	camtopmargin = getHeight() * 3 / 10;
	cambottommargin = getHeight() * 3 / 5;
	mincamx = 0.0f;
	mincamy = 0.0f;
	maxcamx = mapwidth - getWidth();
	maxcamy = mapheight - getHeight();

	placeBuildings();
}

void gCanvas::update() {
	moveCharacter();
	moveCamera();
	updateBullets();

	// Calculate direction index from mouse angle
	float normalizedrot = crot;
	while (normalizedrot < 0.0f) normalizedrot += 360.0f;
	while (normalizedrot >= 360.0f) normalizedrot -= 360.0f;
	characterdir = ((int)((normalizedrot + 22.5f) / 45.0f)) % 8;
}

void gCanvas::draw() {
	drawWater();
	drawMap();
	drawEntities();
}

void gCanvas::moveCharacter() {
	cdx = 0.0f;
	cdy = 0.0f;
	float crotrad = gDegToRad(crot);
	if(keystate & KEY_W) {
		cdx = std::sin(crotrad) * 4.0f;
		cdy = -std::cos(crotrad) * 4.0f;
	} else if(keystate & KEY_S) {
		cdx = -std::sin(crotrad) * 2.0f;
		cdy = std::cos(crotrad) * 2.0f;
	}
	if(keystate & KEY_A) {
		cdx = -std::cos(crotrad) * 2.5f;
		cdy = -std::sin(crotrad) * 2.5f;
	} else if(keystate & KEY_D) {
		cdx = std::cos(crotrad) * 2.5f;
		cdy = std::sin(crotrad) * 2.5f;
	}
	float newcx = cx + cdx;
	if (canMoveTo(newcx, cy)) cx = newcx;

	float newcy = cy + cdy;
	if (canMoveTo(cx, newcy)) cy = newcy;
}

void gCanvas::moveCamera() {
    if (cx < camleftmargin && cdx < 0) {
        float overflow = cx - camleftmargin;
        cx = camleftmargin;
        camx += overflow;
    }
    if (camx < mincamx) {
        cx += camx;
        camx = mincamx;
    }

    if (cx > camrightmargin && cdx > 0) {
        float overflow = cx - camrightmargin;
        cx = camrightmargin;
        camx += overflow;
    }
    if (camx > maxcamx) {
        cx += (camx - maxcamx);
        camx = maxcamx;
    }

    if (cy < camtopmargin && cdy < 0) {
        float overflow = cy - camtopmargin;
        cy = camtopmargin;
        camy += overflow;
    }
    if (camy < mincamy) {
        cy += camy;
        camy = mincamy;
    }

    if (cy > cambottommargin && cdy > 0) {
        float overflow = cy - cambottommargin;
        cy = cambottommargin;
        camy += overflow;
    }
    if (camy > maxcamy) {
        cy += (camy - maxcamy);
        camy = maxcamy;
    }
}

void gCanvas::placeBuildings() {
    float halfw = TILE_W / 2.0f;
    float halfh = TILE_H / 2.0f;

    int positions[BUILDING_COUNT][3] = {
        { 0, 40, 30 },
        { 1, 47, 27 },
        { 2, 10, 30 },
		{ 2, 10, 38 },
		{ 2, 10, 46 },
		{ 2, 85, 20 },
		{ 2, 85, 28 },
		{ 2, 85, 36 },
		{ 2, 82, 12 },
		{ 2, 88, 10 },
		{ 0, 32, 88 },
		{ 7, 50, 60 },
		{ 1, 46, 48 },
		{ 1, 40, 62 },
        { 3, 67, 33 },
        { 4, 72, 44 },
        { 5, 54, 24 },
        { 6, 87, 78 },
		{ 6, 84, 70 },
        { 7, 80, 81 },
        { 8, 28, 76 },
        { 9, 70, 85 },
        { 10, 18, 80 },
        { 11, 63, 21 },
    };

    for (int i = 0; i < BUILDING_COUNT; i++) {
        int imgidx = positions[i][0];
        int col = positions[i][1];
        int row = positions[i][2];

        BuildingInstance bi;
        bi.imageIndex = imgidx;
        bi.col = col;
        bi.row = row;
        bi.worldx = mapbasex + (col - row) * halfw;
        bi.worldy = mapbasey + (col + row) * halfh;
        buildingInstances.push_back(bi);

        float bw = buildings[imgidx].getWidth() / 2.0f;
        float bh = buildings[imgidx].getHeight() / 2.0f;
        float obstacletop = bi.worldy - bh + (TILE_H / 2.0f) + (bh * 3.0f / 5.0f);
        Obstacle o;
        o.x = bi.worldx - (bw / 2.0f);
        o.y = obstacletop;
        o.w = bw;
        o.h = bh * 2.0f / 5.0f;
        obstacles.push_back(o);
    }
}

void gCanvas::drawEntities() {
	struct DrawItem {
		float sortY;
		int type;
		int index;
	};

	std::vector<DrawItem> items;

	// Add buildings to draw list
	for (size_t i = 0; i < buildingInstances.size(); i++) {
		DrawItem item;
		item.sortY = buildingInstances[i].worldy;
		item.type = 0;
		item.index = (int)i;
		items.push_back(item);
	}

	// Add character to draw list
	DrawItem charitem;
	charitem.sortY = cy + charh + camy;
	charitem.type = 1;
	charitem.index = 0;
	items.push_back(charitem);

	// Sort by world Y, lower Y drawn first (further from camera)
	std::sort(items.begin(), items.end(), [](const DrawItem& a, const DrawItem& b) {
		return a.sortY < b.sortY;
	});

	// Draw in sorted order
	for (size_t i = 0; i < items.size(); i++) {
		if (items[i].type == 0) {
			BuildingInstance& bi = buildingInstances[items[i].index];
			float bw = buildings[bi.imageIndex].getWidth() / 2.0f;
			float bh = buildings[bi.imageIndex].getHeight() / 2.0f;
			float drawx = bi.worldx - (bw / 2.0f) - camx;
			float drawy = bi.worldy - bh + (TILE_H / 2.0f) - camy;
			buildings[bi.imageIndex].draw(drawx, drawy, bw, bh);
		} else {
			drawCharacter();
		}
	}

	// Draw bullets on top
	for (size_t i = 0; i < bullets.size(); i++) {
		float drawx = bullets[i].worldx - BULLET_SIZE / 2.0f - camx;
		float drawy = bullets[i].worldy - BULLET_SIZE / 2.0f - camy;
		if (bullets[i].alpha < 1.0f) {
			renderer->setColor(255, 255, 255, (int)(bullets[i].alpha * 255));
		}
		ammo[bullets[i].dirIndex].draw(drawx, drawy, BULLET_SIZE, BULLET_SIZE);
		if (bullets[i].alpha < 1.0f) {
			renderer->setColor(255, 255, 255, 255);
		}
	}
}

bool gCanvas::canMoveTo(float testcx, float testcy) {
	// Foot position in world coordinates (bottom-center of drawn sprite)
	float footworldx = testcx + charw / 2.0f + camx;
	float footworldy = testcy + charh + camy;

	// Check map bounds: reverse isometric transform to (col, row)
	float relx = footworldx - mapbasex;
	float rely = footworldy - mapbasey;
	float halfw = TILE_W / 2.0f;
	float halfh = TILE_H / 2.0f;
	float fcol = (relx / halfw + rely / halfh) / 2.0f;
	float frow = (rely / halfh - relx / halfw) / 2.0f;

	if (fcol < 0.0f || fcol >= MAP_COLS || frow < 0.0f || frow >= MAP_ROWS) {
		return false;
	}

	// Check obstacle collision (point vs AABB)
	for (size_t i = 0; i < obstacles.size(); i++) {
		if (footworldx >= obstacles[i].x && footworldx <= obstacles[i].x + obstacles[i].w &&
			footworldy >= obstacles[i].y && footworldy <= obstacles[i].y + obstacles[i].h) {
			return false;
		}
	}

	return true;
}

void gCanvas::drawWater() {
	float halfw = WATER_TILE_W / 2.0f;
	float halfh = WATER_TILE_H / 2.0f;
	int pngw = watertile.getWidth();
	int pngh = watertile.getHeight();

	// Screen bounds in world coordinates with one-tile margin
	float worldleft = camx - pngw;
	float worldright = camx + getWidth() + pngw;
	float worldtop = camy - pngh;
	float worldbottom = camy + getHeight() + pngh;

	// Convert to isometric u,v space (u = col-row, v = col+row)
	// centerX = u * halfw, centerY = v * halfh
	int minu = (int)std::floor(worldleft / halfw) - 1;
	int maxu = (int)std::ceil(worldright / halfw) + 1;
	int minv = (int)std::floor(worldtop / halfh) - 1;
	int maxv = (int)std::ceil(worldbottom / halfh) + 1;

	for (int v = minv; v <= maxv; v++) {
		for (int u = minu; u <= maxu; u++) {
			// Only draw when u+v is even (ensures integer col/row)
			if ((u + v) % 2 != 0) continue;

			float centerx = u * halfw;
			float centery = v * halfh;
			float drawx = centerx - WATER_OFFSET_X - camx;
			float drawy = centery - WATER_OFFSET_Y - camy;
			watertile.draw(drawx, drawy);
		}
	}
}

void gCanvas::drawMap() {
	float halfw = TILE_W / 2.0f;
	float halfh = TILE_H / 2.0f;
	int pngw = grasstile.getWidth();
	int pngh = grasstile.getHeight();
	float screenw = getWidth();
	float screenh = getHeight();

	for (int row = 0; row < MAP_ROWS; row++) {
		for (int col = 0; col < MAP_COLS; col++) {
			float centerx = mapbasex + (col - row) * halfw;
			float centery = mapbasey + (col + row) * halfh;
			float drawx = centerx - TILE_OFFSET_X - camx;
			float drawy = centery - TILE_OFFSET_Y - camy;
			if (drawx + pngw < 0 || drawx > screenw || drawy + pngh < 0 || drawy > screenh) continue;
			grasstile.draw(drawx, drawy);
		}
	}
}

void gCanvas::drawCharacter() {
	character[characterdir].draw(cx, cy, charw, charh);
}

void gCanvas::fireBullet(int mousex, int mousey) {
	float charcenterx = cx + charw / 2.0f + camx;
	float charcentery = cy + charh / 2.0f + camy;

	float targetx = mousex + camx;
	float targety = mousey + camy;

	float dx = targetx - charcenterx;
	float dy = targety - charcentery;
	float len = std::sqrt(dx * dx + dy * dy);
	if (len < 0.001f) return;
	dx /= len;
	dy /= len;

	Bullet b;
	b.worldx = charcenterx;
	b.worldy = charcentery;
	b.velx = dx * BULLET_SPEED;
	b.vely = dy * BULLET_SPEED;
	b.alive = true;
	b.alpha = 1.0f;

	float angle = gRadToDeg(std::atan2(dy, dx)) + 90.0f;
	while (angle < 0.0f) angle += 360.0f;
	while (angle >= 360.0f) angle -= 360.0f;
	b.dirIndex = ((int)((angle + 22.5f) / 45.0f)) % 8;

	bullets.push_back(b);
}

void gCanvas::updateBullets() {
	for (size_t i = 0; i < bullets.size(); i++) {
		if (!bullets[i].alive) continue;

		bullets[i].worldx += bullets[i].velx;
		bullets[i].worldy += bullets[i].vely;

		// Harita siniri kontrolu — disarida ise kademeli solma
		float relx = bullets[i].worldx - mapbasex;
		float rely = bullets[i].worldy - mapbasey;
		float halfw = TILE_W / 2.0f;
		float halfh = TILE_H / 2.0f;
		float fcol = (relx / halfw + rely / halfh) / 2.0f;
		float frow = (rely / halfh - relx / halfw) / 2.0f;

		// Harita disindan ne kadar uzakta (piksel cinsinden)
		float outsidedist = 0.0f;
		if (fcol < 0.0f) outsidedist = std::max(outsidedist, -fcol * halfw);
		if (fcol >= MAP_COLS) outsidedist = std::max(outsidedist, (fcol - MAP_COLS) * halfw);
		if (frow < 0.0f) outsidedist = std::max(outsidedist, -frow * halfh);
		if (frow >= MAP_ROWS) outsidedist = std::max(outsidedist, (frow - MAP_ROWS) * halfh);

		if (outsidedist > 0.0f) {
			bullets[i].alpha = 1.0f - (outsidedist / BULLET_FADE_DIST);
			if (bullets[i].alpha <= 0.0f) {
				bullets[i].alive = false;
				continue;
			}
		}

		// Bina collision
		for (size_t j = 0; j < obstacles.size(); j++) {
			if (bullets[i].worldx >= obstacles[j].x &&
				bullets[i].worldx <= obstacles[j].x + obstacles[j].w &&
				bullets[i].worldy >= obstacles[j].y &&
				bullets[i].worldy <= obstacles[j].y + obstacles[j].h) {
				bullets[i].alive = false;
				break;
			}
		}
	}

	bullets.erase(
		std::remove_if(bullets.begin(), bullets.end(),
			[](const Bullet& b) { return !b.alive; }),
		bullets.end()
	);
}

void gCanvas::keyPressed(int key) {
	int pressedkey = KEY_NONE;
	switch(key) {
	case G_KEY_W:
		pressedkey = KEY_W;
		break;
	case G_KEY_S:
		pressedkey = KEY_S;
		break;
	case G_KEY_A:
		pressedkey = KEY_A;
		break;
	case G_KEY_D:
		pressedkey = KEY_D;
		break;
	default:
		break;
	}
	keystate |= pressedkey;
}

void gCanvas::keyReleased(int key) {
	int pressedkey = KEY_NONE;
	switch(key) {
	case G_KEY_W:
		pressedkey = KEY_W;
		break;
	case G_KEY_S:
		pressedkey = KEY_S;
		break;
	case G_KEY_A:
		pressedkey = KEY_A;
		break;
	case G_KEY_D:
		pressedkey = KEY_D;
		break;
	default:
		break;
	}
	keystate &= ~pressedkey;
}

void gCanvas::charPressed(unsigned int codepoint) {
}

void gCanvas::mouseMoved(int x, int y) {
	crot = gRadToDeg(std::atan2((y - (cy + charh / 2)), x - (cx + charw / 2))) + 90.0f;
}

void gCanvas::mouseDragged(int x, int y, int button) {
}

void gCanvas::mousePressed(int x, int y, int button) {
	if (button == 0) {
		fireBullet(x, y);
	}
}

void gCanvas::mouseReleased(int x, int y, int button) {
}

void gCanvas::mouseScrolled(int x, int y) {
}

void gCanvas::mouseEntered() {
}

void gCanvas::mouseExited() {
}

void gCanvas::windowResized(int w, int h) {
}

void gCanvas::showNotify() {
}

void gCanvas::hideNotify() {
}
