/*
 * Solution.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: chentuju
 */

#include "Solution.h"

// global variables from Display.cpp
extern std::vector<int> displayBlocks;
extern float xWindowSize;
extern float yWindowSize;
extern float yOffset;
extern int drawDepth;

std::vector<std::vector<std::vector<int>>> partitionAdjMatrix;
std::vector<Net*> partitionNetList;

Solution::Solution() {

	m_numBlock = 12;
	m_numNet = 18;
	m_maxBlockPerPartition = m_numBlock / Solution::NUMPARTITION;
	m_lastBlockIndex = -1;
	m_lastBlockId = -1;
	m_cutSize = 0;
	m_drawNode.order = 0;
	m_drawNode.id = -1;
	m_drawNode.x = xWindowSize / 2.0;
	m_drawNode.y = 0.0;
}

Solution::Solution(int numBlock, int numNet) {

	m_numBlock = numBlock;
	m_numNet = numNet;
	m_maxBlockPerPartition = m_numBlock / Solution::NUMPARTITION;
	m_partitions.resize(Solution::NUMPARTITION);
	m_blocks.resize(m_numBlock+1);
	for(unsigned int i=0; i<m_blocks.size(); i++)
		m_blocks[i] = Solution::UNSET;
	m_lastBlockIndex = -1;
	m_lastBlockId = -1;
	m_cutSize = 0;
	m_drawNode.order = 0;
	m_drawNode.id = -1;
	m_drawNode.x = xWindowSize / 2.0;
	m_drawNode.y = 0.0;
}

Solution::Solution(const Solution &solution) {

	m_numBlock = solution.m_numBlock;
	m_numNet = solution.m_numNet;
	m_maxBlockPerPartition = solution.m_maxBlockPerPartition;
	m_partitions.resize(Solution::NUMPARTITION);
	for(unsigned int i=0; i<m_partitions.size(); i++)
		m_partitions[i] = solution.m_partitions[i];
	m_blocks.resize(solution.m_blocks.size());
	for(unsigned int i=0; i<m_blocks.size(); i++)
		m_blocks[i] = solution.m_blocks[i];
	m_lastBlockIndex = solution.m_lastBlockIndex;
	m_lastBlockId = solution.m_lastBlockId;
	m_cutSize = solution.m_cutSize;
	m_cutNets = solution.m_cutNets;
	m_drawNode.order = solution.m_drawNode.order;
	m_drawNode.id = solution.m_drawNode.id;
	m_drawNode.x = solution.m_drawNode.x;
	m_drawNode.y = solution.m_drawNode.y;
}

Solution::~Solution() {
}

void Solution::pushBlock(int block, int region) {

	// exit if block is already partitioned
	if(m_blocks[block] != Solution::UNSET) {
		if(m_blocks[block] == region)
			std::cout << "[ERROR Solution::pushBlock] Block " << block
				<< " is already partitioned to same partition "
				<< region << std::endl;
		else
			std::cout << "[ERROR Solution::pushBlock] Block " << block
				<< " is already partitioned to partition "
				<< region << std::endl;
		exit(-1);
	}

	// exit if partition already contains block
	if(m_partitions[region].find(block) != m_partitions[region].end()) {
		std::cout << "[ERROR Solution::pushBlock] Partition " << region
				<< "already contains block " << block << std::endl;
		exit(-1);
	}

	// update m_cutSize and m_cutNets by comparing with existing blocks in different region
	for(int i=0; i<(int)m_partitions.size(); i++) {
		if(i != region) {
			for(auto it=m_partitions[i].begin(); it!=m_partitions[i].end(); it++) {
				int block2 = *it;
				for(unsigned int j=0; j<partitionAdjMatrix[block][block2].size(); j++) {
					int net = partitionAdjMatrix[block][block2][j];
					// insert cut net and increment cut size if current net is new
					if(m_cutNets.find(net) == m_cutNets.end()) {
						m_cutNets.insert(net);
						m_cutSize++;
					}
				}
			}
		}
	}

	// update m_partitions and m_blocks and keep track of last added block
	m_lastBlockIndex++;
	m_lastBlockId = block;
	m_blocks[block] = region;
	m_partitions[region].insert(block);
}

std::vector<int> &Solution::getPartitionBlocks() {

	return m_blocks;
}

int Solution::getLowerBound() {

	int lowerBound = 0;
	// simply return m_cutSize if solution is complete
	if(!isSolutionComplete()) {
		std::vector<std::vector<int>> lastBlockConnectedUnset;
		for(unsigned int i=0; i<partitionAdjMatrix[m_lastBlockId].size(); i++)
			if(partitionAdjMatrix[m_lastBlockId][i].size() > 0 && m_blocks[i] == Solution::UNSET)
				lastBlockConnectedUnset.push_back(partitionAdjMatrix[m_lastBlockId][i]);

		int lastPartitionAvail = m_maxBlockPerPartition -
				m_partitions[m_blocks[m_lastBlockId]].size();
		int top = (int)lastBlockConnectedUnset.size() - lastPartitionAvail;

		if(top > 0) {
			std::vector<int> temp;
			for(unsigned int i=0; i<lastBlockConnectedUnset.size(); i++) {
				int count = 0;
				for(unsigned int j=0; j<lastBlockConnectedUnset[i].size(); j++)
					// if the net connects last block and any unset block is uncut
					if(m_cutNets.find(lastBlockConnectedUnset[i][j]) == m_cutNets.end())
						count++;
				temp.push_back(count);
			}
			// sort potential cut cost in ascending order
			sort(temp.begin(), temp.end());
			for(int i=0; i<top; i++)
				lowerBound += temp[i];
		}
	}
	return m_cutSize + lowerBound;
}

int Solution::getUpperBound() {

	int upperBound = 0;
	// simply return m_cutSize if solution is complete
	if(!isSolutionComplete()) {
		std::unordered_set<int> tempNetSet;
		// go through netList, for each uncut net if any block is not partitioned, upperBound++
		for(unsigned int i=0; i<partitionNetList.size(); i++) {
			if(m_cutNets.find(partitionNetList[i]->getId()) == m_cutNets.end() &&
					tempNetSet.find(partitionNetList[i]->getId()) == tempNetSet.end())
				for(unsigned int j=0; j<partitionNetList[i]->getBlocks().size(); j++) {
					if(m_blocks[partitionNetList[i]->getBlocks()[j]->getId()] == Solution::UNSET) {
						upperBound++;
						tempNetSet.insert(partitionNetList[i]->getId());
						break;
					}
				}
		}
	}
	return m_cutSize + upperBound;
}

int Solution::getLastBlockIndex() {

	return m_lastBlockIndex;
}

void Solution::sanityCheck() {

	// check if each block has valid partition region
	for(unsigned int i=1; i<m_blocks.size(); i++)
		if(m_blocks[i] > Solution::RIGHT) {
			std::cout << "[ERROR Solution::sanityCheck] Block " << i
					<< " having invalid partition region " << m_blocks[i] << std::endl;
			exit(-1);
		}

	// check if each partition has size equal to m_maxBlockPerPartition
	for(unsigned int i=0; i<m_partitions.size(); i++) {
		int partitionSize = (int)m_partitions[i].size();
		if(partitionSize != m_maxBlockPerPartition) {
			std::cout << "[ERROR Solution::sanityCheck] Partition region " << i
					<< " have invalid size " << partitionSize
					<< " (should be " << m_maxBlockPerPartition << ")\n";
			exit(-1);
		}
	}

	// check if m_cutSize has exceeded number of nets
	if(m_cutSize > m_numNet) {
		std::cout << "[ERROR Solution::sanityCheck] Partition cut size of "
				<< m_cutSize << " has exceeded total number of nets "
				<< m_numNet << std::endl;
		exit(-1);
	}

	// check if m_cutNets has unequal size to m_cutSize
	if((int)m_cutNets.size() != m_cutSize) {
		std::cout << "[ERROR Solution::sanityCheck] Partition cut nets have "
				<< m_cutNets.size() << " nets unequal to current cut size"
				<< m_cutSize << std::endl;
		exit(-1);
	}
}

void Solution::updateDrawNode(int region) {

	m_drawNode.id = displayBlocks[m_drawNode.order];
	int depth = m_drawNode.order + 1;
	float numGap = pow(3, depth);
	float xOffset = xWindowSize / numGap;
	float x;
	float y = m_drawNode.y + yOffset;

	if(region == Solution::LEFT)
		x = m_drawNode.x - xOffset;
	else if(region == Solution::MIDDLE)
		x = m_drawNode.x;
	else if(region == Solution::RIGHT)
		x = m_drawNode.x + xOffset;
	else {
		std::cout << "[ERROR Solution::updateDrawNode] "
				<< "Partition number " << region
				<< " is invalid" << std::endl;
		exit(-1);
	}

	// draw LEFT, MIDDLE, RIGHT branches and cutsize
	char str[8];
	setcolor(RED);
	if(m_drawNode.order <= drawDepth) {
		drawline(m_drawNode.x, m_drawNode.y, x, y);
		sprintf(str, "%d", m_cutSize);
		drawtext((m_drawNode.x+x)/2.0-7.0, (m_drawNode.y+y)/2.0, str, 100.0);
	}

	// push 3 branched nodes onto queue
	m_drawNode.order = depth;
	m_drawNode.id = displayBlocks[m_drawNode.order];
	m_drawNode.y = y;
	m_drawNode.x = x;
}

bool Solution::isBlockPartitioned(int block) {

	return (m_blocks[block] <= Solution::RIGHT);
}

bool Solution::isPartitionFull(int region) {

	if(region > Solution::RIGHT) {
		std::cout << "[ERROR Solution::isPartitionFull] Input region "
				<< region << " is invalid" << std::endl;
		exit(-1);
	}

	int avail = m_maxBlockPerPartition - (int)m_partitions[region].size();
	if(avail > 0)
		return false;
	else if(avail == 0)
		return true;
	else {
		std::cout << "[ERROR Solution::isPartitionFull] Solution has previously "
				<< "exceeded maximum allowed blocks " << m_maxBlockPerPartition
				<< " by " << abs(avail) << " in partition " << region << std::endl;
		exit(-1);
	}
}

bool Solution::isSolutionComplete() {

	int currentNumBlock = 0;
	for(unsigned int i=0; i<m_partitions.size(); i++)
		currentNumBlock += m_partitions[i].size();
	return (currentNumBlock == m_numBlock);
}

void Solution::printBlocks() {

	for(unsigned int i=1; i<m_blocks.size(); i++)
		std::cout << "Block " << i << " is in partition region "
			<< m_blocks[i] << std::endl;
}

void Solution::printPartitions(std::unordered_map<int,Block*> &blockMap) {

	for(unsigned int i=0; i<m_partitions.size(); i++) {
		std::cout << "Partition region " << i << " contains:";
		for(auto it=m_partitions[i].begin(); it!=m_partitions[i].end(); it++)
			std::cout << "\t" << *it;
		std::cout << std::endl;
	}
	std::cout << "Solution cut size = " << m_cutSize << std::endl;
}
