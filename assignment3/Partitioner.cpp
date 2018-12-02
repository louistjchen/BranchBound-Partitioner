/*
 * Partitioner.cpp
 *
 *  Created on: Nov 22, 2018
 *      Author: chentuju
 */

#include "Partitioner.h"

extern std::vector<std::vector<std::vector<int>>> partitionAdjMatrix;
extern std::vector<Net*> partitionNetList;
extern std::vector<int> displayBlocks;

Partitioner::Partitioner() {

	m_visitCount = 0;
	m_useDFSNotLBF = true;
	m_display = false;
}

Partitioner::~Partitioner() {

	// free all blocks
	for(unsigned int i=0; i<m_blockList.size(); i++)
		if(m_blockList[i])
			delete m_blockList[i];

	// free all nets
	for(unsigned int i=0; i<m_netList.size(); i++)
		if(m_netList[i])
			delete m_netList[i];
}

void Partitioner::init(const char *inputFile, const char *traversal,
		const char *display) {

	if(strcmp(traversal, "-traversal=dfs") == 0)
		m_useDFSNotLBF = true;
	else if(strcmp(traversal, "-traversal=lbf") == 0)
		m_useDFSNotLBF = false;
	else {
		std::cout << "[ERROR Partitioner::init] Invalid traversal order <"
				<< traversal << ">" << std::endl;
		exit(-1);
	}
	if(strcmp(display, "-display=on") == 0)
		m_display = true;
	else if(strcmp(display, "-display=off") == 0)
		m_display = false;
	else {
		std::cout << "[ERROR Partitioner::init] Invalid display mode <"
				<< display << ">" << std::endl;
		exit(-1);
	}
	constructBlockNet(inputFile);
	constructAdjMatrix();
}

void Partitioner::run() {

	// sort m_blockList in descending fan-out order (for best pruning)
	std::sort(m_blockList.begin(), m_blockList.end(), Partitioner::sortBlockList);

	// sort m_netList in ascending fan-out order (for best initial solution)
	std::sort(m_netList.begin(), m_netList.end(), Partitioner::sortNetList);

	// find initial solution and compute best cost
	initialSolution();
	std::cout << "-------------------- Initial Partition --------------------" << std::endl;
	m_bestSolution.printPartitions(m_blockMap);
	std::cout << "-----------------------------------------------------------" << std::endl;

	// invoke graphics
	if(m_display) {
		for(unsigned int i=0; i<m_blockList.size(); i++)
			displayBlocks.push_back(m_blockList[i]->getId());
		openDisplay();
	}

	// start timer
	struct timeval t1, t2;
	gettimeofday(&t1, NULL);

	// use iterative rather than recursive traversal
	if(m_useDFSNotLBF)
		branchBoundDFS();
	else
		branchBoundLBF();

	// stop timer
	gettimeofday(&t2, NULL);
	double time1 = t1.tv_sec * 1000.0 + t1.tv_usec / 1000.0;
	double time2 = t2.tv_sec * 1000.0 + t2.tv_usec / 1000.0;

	std::cout << "--------------------- Final Partition ---------------------" << std::endl;
	m_bestSolution.printPartitions(m_blockMap);
	std::cout << "-----------------------------------------------------------" << std::endl;
	std::cout << "---> Verified cut size = " << computeCutSize(m_bestSolution) << std::endl;
	std::cout << "---> # nodes expanded = " << m_visitCount << std::endl;

	// print total run-time
	std::cout << "---> Partitioner took " << (time2-time1) << " ms to finish" << std::endl;

	// close graphics
	if(m_display)
		closeDisplay();
}

void Partitioner::constructBlockNet(const char *inputFile) {

	char buf[8];
	std::ifstream file;
	file.open(inputFile, std::ifstream::in);

	// loop through every line in the input file
	while(file >> buf) {

		int num = atoi(buf);
		if(num == -1)
			break;

		// allocate, push, and hash a new block
		Block *block = new Block();
		block->setId(num);
		m_blockList.push_back(block);
		m_blockMap[num] = block;

		// loop through every netnum in a line
		while(file >> buf) {

			num = atoi(buf);
			if(num == -1)
				break;

			// allocate, push, and hash the net if it does not exist
			Net *net = NULL;
			if(m_netMap.find(num) == m_netMap.end()) {
				net = new Net();
				net->setId(num);
				m_netList.push_back(net);
				m_netMap[num] = net;
			}
			else
				net = m_netMap[num];

			// link block and net
			net->insertBlock(block);
			block->insertNet(net);
		}
	}
	file.close();
	partitionNetList = m_netList;
}

void Partitioner::constructAdjMatrix() {

	m_adjMatrix.resize(m_blockList.size()+1);
	for(unsigned int i=0; i<m_adjMatrix.size(); i++)
		m_adjMatrix[i].resize(m_blockList.size()+1);

	for(unsigned int i=1; i<m_adjMatrix.size(); i++) {
		for(unsigned int j=1; j<=i; j++) {
			if(i != j && i != 0 && j != 0) {
				Block *block1 = m_blockMap[i];
				Block *block2 = m_blockMap[j];
				std::vector<int> connectedNets = block1->isConnected(block2);
				for(unsigned int k=0; k<connectedNets.size(); k++) {
					m_adjMatrix[i][j].push_back(connectedNets[k]);
					m_adjMatrix[j][i].push_back(connectedNets[k]);
				}
			}
		}
	}
	partitionAdjMatrix = m_adjMatrix;
}

void Partitioner::initialSolution() {

	m_bestSolution = Solution(m_blockList.size(), m_netList.size());

	// traverse through all nets in descending fan-out order
	// try to group as many as possible within one net
	for(unsigned int i=0; i<m_netList.size(); i++) {

		std::vector<Block*> blockList = m_netList[i]->getBlocks();
		for(unsigned int j=0; j<blockList.size(); j++) {

			Block *block = blockList[j];
			int blockId = block->getId();
			// if current block not partitioned yet
			if(!m_bestSolution.isBlockPartitioned(blockId)) {
				bool done = false;
				for(int k=0; k<Solution::NUMPARTITION && !done; k++) {
					if(!m_bestSolution.isPartitionFull(k)) {
						m_bestSolution.pushBlock(blockId, k);
						done = true;
					}
				}
				if(!done) {
					std::cout << "[ERROR Partitioner::initialSolution] "
							<< "Solution::blockPartitioned shows unpartitioned "
							<< "but Solution::partitionFull gives unavailability"
							<< std::endl;
					exit(-1);
				}
			}
		}
	}
	m_bestSolution.sanityCheck();
	m_bestCost = m_bestSolution.getLowerBound();
}

void Partitioner::branchBoundDFS() {

	// create a stack to perform depth-first traversal
	std::stack<Solution*> st;
	Solution *startSolution = new Solution(m_blockList.size(), m_netList.size());

	// lock 1st block to only 1 partition by symmetry
	int nextBlockIndex = startSolution->getLastBlockIndex() + 1;
	int nextBlockId = m_blockList[nextBlockIndex]->getId();
	startSolution->pushBlock(nextBlockId, Solution::LEFT);

	// lock 2nd block to only 2 partitions by symmetry
	Solution *startSolution2 = new Solution(*startSolution);
	nextBlockIndex = startSolution->getLastBlockIndex() + 1;
	nextBlockId = m_blockList[nextBlockIndex]->getId();

	startSolution->pushBlock(nextBlockId, Solution::LEFT);
	startSolution2->pushBlock(nextBlockId, Solution::MIDDLE);

	if(m_display) {
		startSolution->updateDrawNode(Solution::LEFT);
		startSolution2->updateDrawNode(Solution::MIDDLE);
	}

	st.push(startSolution);
	st.push(startSolution2);

	while(!st.empty()) {

		Solution *solution = st.top();
		st.pop();
		m_visitCount++;

		int solutionLowerBound = solution->getLowerBound();
//		int solutionUpperBound = solution->getUpperBound();

		// stop expansion and update best if current solution is complete
		if(solution->isSolutionComplete() && solutionLowerBound < m_bestCost) {
			m_bestSolution = *solution;
			m_bestCost = solutionLowerBound;
			delete solution;
			continue;
		}

		// update m_bestCost if upper bound of current solution < m_bestCost
//		if(solutionUpperBound < m_bestCost) {
//			std::cout << "[DEBUG Partitioner::branchBoundDFS] Upper bound pruned"
//					<< std::endl;
//			m_bestCost = solutionUpperBound;
//		}

		// prune if lower bound of current solution >= best solution
		if(solutionLowerBound >= m_bestCost) {
			delete solution;
			continue;
		}

		// create and push solution object while preserving balance constraint
		nextBlockIndex = solution->getLastBlockIndex() + 1;
		if(nextBlockIndex < (int)m_blockList.size()) {
			nextBlockId = m_blockList[nextBlockIndex]->getId();
			for(int i=0; i<Solution::NUMPARTITION; i++) {
				if(!solution->isPartitionFull(i)) {
					Solution *nextSolution = new Solution(*solution);
					nextSolution->pushBlock(nextBlockId, i);
					if(m_display)
						nextSolution->updateDrawNode(i);
					st.push(nextSolution);
				}
			}
		}
		delete solution;
	}
}

class Compare {
public:
	bool operator()(Solution *&a, Solution*&b) {
		return a->getLowerBound() > b->getLowerBound();
	}
};

void Partitioner::branchBoundLBF() {

	// create a priority queue to perform lowest-bound-first traversal
	std::priority_queue<Solution*,std::vector<Solution*>,Compare> pq;
	Solution *startSolution = new Solution(m_blockList.size(), m_netList.size());

	// lock 1st block to only 1 partition by symmetry
	int nextBlockIndex = startSolution->getLastBlockIndex() + 1;
	int nextBlockId = m_blockList[nextBlockIndex]->getId();
	startSolution->pushBlock(nextBlockId, Solution::LEFT);

	// lock 2nd block to only 2 partitions by symmetry
	Solution *startSolution2 = new Solution(*startSolution);
	nextBlockIndex = startSolution->getLastBlockIndex() + 1;
	nextBlockId = m_blockList[nextBlockIndex]->getId();

	startSolution->pushBlock(nextBlockId, Solution::LEFT);
	startSolution2->pushBlock(nextBlockId, Solution::MIDDLE);

	if(m_display) {
		startSolution2->updateDrawNode(Solution::MIDDLE);
		startSolution->updateDrawNode(Solution::LEFT);
	}

	pq.push(startSolution);
	pq.push(startSolution2);

	while(!pq.empty()) {

		Solution *solution = pq.top();
		pq.pop();
		m_visitCount++;

		int solutionLowerBound = solution->getLowerBound();
//		int solutionUpperBound = solution->getUpperBound();

		// stop expansion and update best if current solution is complete
		if(solution->isSolutionComplete() && solutionLowerBound < m_bestCost) {
			m_bestSolution = *solution;
			m_bestCost = solutionLowerBound;
			delete solution;
			continue;
		}

		// update m_bestCost if upper bound of current solution < m_bestCost
//		if(solutionUpperBound < m_bestCost) {
//			std::cout << "[DEBUG Partitioner::branchBoundLBF] Upper bound pruned"
//					<< std::endl;
//			m_bestCost = solutionUpperBound;
//		}

		// prune if lower bound of current solution >= best solution
		if(solutionLowerBound >= m_bestCost) {
			delete solution;
			continue;
		}

		// create and push solution object while preserving balance constraint
		nextBlockIndex = solution->getLastBlockIndex() + 1;
		if(nextBlockIndex < (int)m_blockList.size()) {
			nextBlockId = m_blockList[nextBlockIndex]->getId();
			for(int i=0; i<Solution::NUMPARTITION; i++) {
				if(!solution->isPartitionFull(i)) {
					Solution *nextSolution = new Solution(*solution);
					nextSolution->pushBlock(nextBlockId, i);
					if(m_display)
						nextSolution->updateDrawNode(i);
					pq.push(nextSolution);
				}
			}
		}
		delete solution;
	}
}

int Partitioner::computeCutSize(Solution &solution) {

	int cutSize = 0;
	std::vector<int> partitionBlocks = solution.getPartitionBlocks();

	// loop through all nets
	for(unsigned int i=0; i<m_netList.size(); i++) {

		Net *net = m_netList[i];
		int partition[Solution::NUMPARTITION] = {0};
		std::vector<Block*> blocks = net->getBlocks();

		// for each net see if it contains blocks in different partitions
		for(unsigned int j=0; j<blocks.size(); j++) {

			int region = partitionBlocks[blocks[j]->getId()];
			if(partition[region] == 0)
				partition[region] = 1;
			int count = 0;
			for(int k=0; k<Solution::NUMPARTITION && count<=1; k++)
				count += partition[k];
			if(count > 1) {
				cutSize++;
				break;
			}
		}
	}
	return cutSize;
}

bool Partitioner::sortBlockList(Block *a, Block *b) {

	return a->getNets().size() > b->getNets().size();
}

bool Partitioner::sortNetList(Net *a, Net *b) {

	return a->getBlocks().size() < b->getBlocks().size();
}

void Partitioner::printBlockList() {

	std::cout << "# Blocks = " << m_blockList.size()
			<< "; # Nets = " << m_netList.size() << std::endl;
	for(unsigned int i=0; i<m_blockList.size(); i++) {
		m_blockList[i]->printBlock();
		std::cout << std::endl;
	}
}

void Partitioner::printNetList() {

	std::cout << "# Blocks = " << m_blockList.size()
			<< "; # Nets = " << m_netList.size() << std::endl;
	for(unsigned int i=0; i<m_netList.size(); i++) {
		m_netList[i]->printNet();
		std::cout << std::endl;
	}
}

void Partitioner::printAdjMatrix() {

	for(unsigned int i=1; i<m_adjMatrix.size(); i++)
		for(unsigned int j=1; j<m_adjMatrix[i].size(); j++)
			if(i != j) {
				std::cout << "Blocks " << std::setw(3) << i
						<< " and " << std::setw(3) << j
						<< " are connected via nets: ";
				for(unsigned int k=0; k<m_adjMatrix[i][j].size(); k++)
					std::cout << std::setw(3) << m_adjMatrix[i][j][k] << " ";
				std::cout << std::endl;
			}
}
