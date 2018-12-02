/*
 * Block.h
 *
 *  Created on: Nov 23, 2018
 *      Author: chentuju
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include <iostream>
#include <iomanip>
#include <vector>
#include "Net.h"

class Net;

class Block {
public:
	Block();
	~Block();

	// functional methods
	void setId(int id);
	int getId();
	void insertNet(Net* net);
	std::vector<Net*> &getNets();
	std::vector<int> isConnected(Block *block);

	// debugging methods
	void printBlock();

private:
	int m_id;
	std::vector<Net*> m_nets;
};

#endif /* BLOCK_H_ */
