/*
 * Solution.h
 *
 *  Created on: Nov 23, 2018
 *      Author: chentuju
 */

#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include "Block.h"
#include "graphics.h"

class Solution {
public:
	Solution();
	Solution(int numBlock, int numNet);
	Solution(const Solution &solution);
	~Solution();

	// functional methods
	void pushBlock(int block, int region);
	std::vector<int> &getPartitionBlocks();
	int getLowerBound();
	int getUpperBound();
	int getLastBlockIndex();
	void sanityCheck();
	void updateDrawNode(int region);

	// query methods
	bool isBlockPartitioned(int block);
	bool isPartitionFull(int region);
	bool isSolutionComplete();

	// debugging methods
	void printBlocks();
	void printPartitions(std::unordered_map<int,Block*> &blockMap);

	// enums for partition regions
	enum {
		LEFT,
		MIDDLE,
		RIGHT,
		NUMPARTITION,
		UNSET
	};

private:
	int m_numBlock;
	int m_numNet;
	int m_maxBlockPerPartition;
	int m_lastBlockIndex;
	int m_lastBlockId;

	std::vector<int> m_blocks;
	std::vector<std::unordered_set<int>> m_partitions;

	int m_cutSize;
	std::unordered_set<int> m_cutNets;

	struct _Node {
		int order;
		int id;
		float x;
		float y;
	} m_drawNode;
};

#endif /* SOLUTION_H_ */
