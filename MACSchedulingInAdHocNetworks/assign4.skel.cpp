#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <deque>
#include <queue>
#include <list>
using namespace std;

#define MAXNODE 20
#define MAXTIME 1000000
#define SEED 1
#define DEBUG false
//#define DEBUG true

int MAXLINK = 0;

class NODE;
class LINK;
void GreedyQ(deque<LINK*> LinkContainer, int* schedule);

class PACKET {
	public:
		int pkt_id;
		int gen_time;
		PACKET(int id, int time) {
			pkt_id = id;
			gen_time = time;
		}
};

class LINK {
	// fill
};


class NODE {
	// fill
};

bool isClose(NODE* n1, NODE* n2) {
	double x_diff = pow(n1->x_pos - n2->x_pos, 2);
	double y_diff = pow(n1->y_pos - n2->y_pos, 2);
	double distance = sqrt(x_diff + y_diff);
	if(distance < 0.35)
		return true;
	else
		return false;
}

void GreedyQ(deque<LINK*> LinkContainer, int* schedule) {
	// fill
}


int main(int argc, char* argv[]) {
	if(argc != 2) {
		cout << "Usage : a.out <rho>\n";
		exit(1);
	}
	double rate = atof(argv[1]);


	NODE* NodeContainer[MAXNODE];
	deque<LINK*> LinkContainer;

	// node generation (at a random location in 1x1 area)
	srand(SEED);	// this is REQUIRED to generate the SAME topology
					// across the simulation runs. 
					// You can change SEED, e.g., to 2 or 3 ...
					// But this should remain fixed for all simulations.
	for(int i=0; i<MAXNODE; i++) {
		NODE* n = new NODE(i, 1.0*rand()/RAND_MAX, 1.0*rand()/RAND_MAX);
		NodeContainer[i] = n;
	}

	// link generation (based on the proximity)
	for(int i=0; i<MAXNODE; i++) {
		for(int j=i+1; j<MAXNODE; j++) {
			// link(i,j)?
			if(isClose(NodeContainer[i], NodeContainer[j])) {
				LINK* tl = new LINK(MAXLINK++, NodeContainer[i], NodeContainer[j]);

				// setting (relative) pkt arrival rate at link(i,j)
				double ran = 1.0 * rand() / RAND_MAX;
				if(ran < 0.4)
					tl->arrival = 0.0;
				else if(ran < 0.7)
					tl->arrival = 0.1;
				else if(ran < 0.9)
					tl->arrival = 0.2;
				else
					tl->arrival = 0.3;
				LinkContainer.push_back(tl);
				NodeContainer[i]->push(tl);
				NodeContainer[j]->push(tl);
			}
		}
	}
	if(MAXLINK != LinkContainer.size()) {
		cout << "Error: Wrong Size - " << MAXLINK << " | " << LinkContainer.size() << endl;
		exit(1);
	}

	if(DEBUG) {
		for(int i=0; i<MAXNODE; i++) {
			cout << NodeContainer[i]->node_id << ": (" << NodeContainer[i]->x_pos << ", " << \
			NodeContainer[i]->y_pos << ")\n";
			cout << "# of links: " << MAXLINK << endl;
		}
	}

	int schedule[MAXLINK];
	int pkt_num = 0;
	double queue_length = 0;
	double delay = 0;
	for(int time=0; time<MAXTIME; time++) {

		// fill
		//   You should handle the interference. 
		//   Since the GreedyQ is a centralized scheduler, 
		//   it may be better to handle the interference
		//   within the GreedyQ() function call
		//   i.e., the result of GreedyQ() should yield
		//   an interference-free schedule.


		// scheduling
		GreedyQ(LinkContainer, schedule);

		// actual pkt transmission
		for(int i=0; i<MAXLINK; i++) {
			if(schedule[i] == 1) {
				PACKET* pkt = LinkContainer[i]->pop();
				if(time>=999000)
					delay += time - pkt->gen_time;
				delete pkt;
			}
		}

		// queue length evaluation (for 1000 time slots)
		if(time>=999000) {
			for(int i=0; i<MAXLINK; i++)
				queue_length += LinkContainer[i]->q.size();
		}
	}
	cout << queue_length / 1000.0 << "	" << delay / 1000.0 << endl;
}
