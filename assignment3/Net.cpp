/*
 * Net.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: chentuju
 */

#include "Net.h"

Net::Net() {

	m_id = -1;
}

Net::~Net() {
}

void Net::setId(int id) {

	m_id = id;
}

int Net::getId() {

	return m_id;
}

void Net::insertBlock(Block *block) {

	m_blocks.push_back(block);
}

std::vector<Block*> &Net::getBlocks() {

	return m_blocks;
}

bool Net::containBlock(Block *block) {

	for(unsigned int i=0; i<m_blocks.size(); i++)
		if(m_blocks[i] == block)
			return true;
	return false;
}

void Net::printNet() {

	std::cout << "Net ID: " << m_id << "\tConnected Blocks:";
	for(unsigned int i=0; i<m_blocks.size(); i++)
		std::cout << "\t" << m_blocks[i]->getId();
}
