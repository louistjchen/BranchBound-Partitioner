/*
 * Block.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: chentuju
 */

#include "Block.h"

Block::Block() {

	m_id = -1;
}

Block::~Block() {
}

void Block::setId(int id) {

	m_id = id;
}

int Block::getId() {

	return m_id;
}

void Block::insertNet(Net *net) {

	m_nets.push_back(net);
}

std::vector<Net*> &Block::getNets() {

	return m_nets;
}

std::vector<int> Block::isConnected(Block *block) {

	std::vector<int> connectedNets;
	for(unsigned int i=0; i<m_nets.size(); i++)
		if(m_nets[i]->containBlock(block))
			connectedNets.push_back(m_nets[i]->getId());
	return connectedNets;
}

void Block::printBlock() {

	std::cout << "Block ID: " << m_id << "\tConnected Nets:";
	for(unsigned int i=0; i<m_nets.size(); i++)
		std::cout << "\t" << m_nets[i]->getId();
}
