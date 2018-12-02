/*
 * Display.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: chentuju
 */


#include "Display.h"

std::vector<int> displayBlocks;
float xWindowSize = 1200.0;
float yWindowSize = 600.0;
float yOffset = 0.0;
int drawDepth;
bool drawn = false;

void openDisplay() {

//	drawDepth = (int)displayBlocks.size();
	drawDepth = 13;
	yOffset = yWindowSize / (float)drawDepth;
    init_graphics("ECE1387 Assignment 3 - B&B Partitioner (developed by Louis Chen)", WHITE);
    init_world (0.0, 0.0, xWindowSize, yWindowSize);
    update_message("Press \"Proceed\" to start branch-and-bound partitioner.");
    event_loop(act_on_button_press, NULL, NULL, drawScreen);
    drawn = true;
}

void closeDisplay() {

    update_message("Press \"Proceed\" to exit.");
    event_loop(act_on_button_press, NULL, NULL, drawScreen);
}

void drawScreen() {

	if(drawn)
		return;

	set_draw_mode(DRAW_NORMAL);
	clearscreen();

	std::queue<Node> q;
	Node a;
	a.order = 0;
	a.id = displayBlocks[a.order];
	a.x = xWindowSize / 2.0;
	a.y = 0.0;
	q.push(a);

	setfontsize(15);
	setcolor(BLACK);

	while(!q.empty()) {

		Node node = q.front();
		q.pop();

		if(node.order == drawDepth)
			continue;

		// get LEFT, MIDDLE, RIGHT points
		int depth = node.order + 1;
		float numGap = pow(3, depth);
		float xOffset = xWindowSize / numGap;
		float x_left = node.x - xOffset;
		float x_right = node.x + xOffset;
		float y = node.y + yOffset;

		// draw node
		char str[8];
		if(depth == 1) {
			sprintf(str, "%d in LEFT", node.id);
			drawtext(node.x, node.y-7.0, str, 100.0);
		}
		else {
			sprintf(str, "%d", node.id);
			drawtext(node.x-7.0, node.y, str, 100.0);
		}

		// draw LEFT, MIDDLE, RIGHT branches
		drawline(node.x, node.y, x_left, y);
		drawline(node.x, node.y, node.x, y);
		drawline(node.x, node.y, x_right, y);

		// push 3 branched nodes onto queue
		node.order = depth;
		node.id = displayBlocks[node.order];
		node.y = y;
		q.push(node);
		node.x = x_left;
		q.push(node);
		node.x = x_right;
		q.push(node);
	}
}

void act_on_button_press(float x, float y) {
/* Called whenever event_loop gets a button press in the graphics *
 * area.  Allows the user to do whatever he/she wants with button *
 * clicks.                                                        */
//   printf("User clicked a button at coordinates (%f, %f)\n", x, y);
}

void act_on_mouse_move (float x, float y) {
/* function to handle mouse move event, the current mouse position  *
 * in the current world coordinate as defined as MAX_X and MAX_Y in *
 * init_world is returned.											*/
//   printf ("Mouse move at (%f,%f)\n", x, y);
}

void act_on_key_press (char c) {
/* function to handle keyboard press event, the ASCII character is returned */
//   printf ("Key press: %c\n", c);
}
