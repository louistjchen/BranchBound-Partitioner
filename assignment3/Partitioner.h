/*
 * Partitioner.h
 *
 *  Created on: Nov 22, 2018
 *      Author: chentuju
 */

#ifndef PARTITIONER_H_
#define PARTITIONER_H_

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include "Block.h"
#include "Net.h"
#include "Solution.h"
#include "Display.h"

class Partitioner {
public:
	Partitioner();
	 ~Partitioner();

	 // main methods
	 void init(const char *inputFile, const char *traversal,
			 const char *display);
	 void run();

protected:
	 // functional methods - init()
	 void constructBlockNet(const char *inputFile);
	 void constructAdjMatrix();

	 // functional methods - run()
	 void initialSolution();
	 void branchBoundDFS();
	 void branchBoundLBF();
	 int computeCutSize(Solution &solution);

	 // sort methods
	 static bool sortBlockList(Block *a, Block *b);
	 static bool sortNetList(Net *a, Net *b);

	 // debugging methods
	 void printBlockList();
	 void printNetList();
	 void printAdjMatrix();

private:
	 // block-net connectivity info
	 std::vector<Block*> m_blockList;
	 std::vector<Net*> m_netList;
	 std::unordered_map<int,Block*> m_blockMap;
	 std::unordered_map<int,Net*> m_netMap;
	 std::vector<std::vector<std::vector<int>>> m_adjMatrix;

	 // partition info
	 Solution m_bestSolution;
	 int m_bestCost;
	 long long m_visitCount;

	 // flag to select depth-first traversal or lowest-bound-first traversal
	 bool m_useDFSNotLBF;

	 // flag for display
	 bool m_display;
};

#endif /* PARTITIONER_H_ */
