/*
 * Net.h
 *
 *  Created on: Nov 23, 2018
 *      Author: chentuju
 */

#ifndef NET_H_
#define NET_H_

#include <iostream>
#include <iomanip>
#include <vector>
#include "Block.h"

class Block;

class Net {
public:
	Net();
	~Net();

	// functional methods
	void setId(int id);
	int getId();
	void insertBlock(Block* block);
	std::vector<Block*> &getBlocks();
	bool containBlock(Block *block);

	// debugging methods
	void printNet();

private:
	int m_id;
	std::vector<Block*> m_blocks;
};

#endif /* NET_H_ */
