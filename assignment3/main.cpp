/*
 * main.cpp
 *
 *  Created on: Nov 22, 2018
 *      Author: chentuju
 */

#include <iostream>
#include "Partitioner.h"

int main(int argc, char **argv) {

	if(argc != 4) {
		std::cout << "[ERROR main] Number of input argument is not correct"
				<< std::endl;
		exit(-1);
	}

	Partitioner partitioner;
	partitioner.init(argv[1], argv[2], argv[3]);
	partitioner.run();

	return 0;
}
