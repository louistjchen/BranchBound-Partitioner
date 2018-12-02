/*
 * Display.h
 *
 *  Created on: Nov 23, 2018
 *      Author: chentuju
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <cmath>
#include <queue>
#include "graphics.h"
#include "Solution.h"

typedef struct _Node {
	int order;
	int id;
	float x;
	float y;
} Node;

void openDisplay();
void closeDisplay();
void drawScreen();

void act_on_button_press(float x, float y);
void act_on_mouse_move(float x, float y);
void act_on_key_press(char c);

#endif /* DISPLAY_H_ */
