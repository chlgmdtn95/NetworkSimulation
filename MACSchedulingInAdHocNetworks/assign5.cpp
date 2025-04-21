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
class PACKET;
void GreedyQ(deque<LINK*> LinkContainer, int* schedule);

PACKET *list_empty_pkt = NULL;

class PACKET {
	public:
		PACKET *next;
		int pkt_id;
		int gen_time;
		PACKET(int id, int time) {
			pkt_id = id;
			gen_time = time;
		}
};

void	discard(PACKET *p)
{
	p->next = list_empty_pkt;
	list_empty_pkt = p;
}

PACKET *creat_pkt (int id, int time)
{
	PACKET *new_pkt;
	if (list_empty_pkt != NULL)
	{
		new_pkt = list_empty_pkt;
		list_empty_pkt = new_pkt->next;
	}
	else
		new_pkt = new PACKET(id, time);
	new_pkt->pkt_id = id;
	new_pkt->gen_time = time;
	return new_pkt;
}

class LINK {
	// fill
	public:
		queue<PACKET *> q;
		int	link_id;
		double arrival;
		NODE *prev;
		NODE *next;

		LINK(int id, NODE *prev, NODE *next)
		{
			link_id = id;
			this->prev = prev;
			this->next = next;
		}

		PACKET	*pop()
		{
			PACKET	*ret;

			if (!q.empty())
			{
				ret = q.front();
				q.pop();
			}
			return ret;
		}
};


class NODE {
	// fill
	private:
		queue <LINK *> linkcon;

	public:
		int	node_id;
		double x_pos;
		double y_pos;
		
		NODE(int idx, double x_pos, double y_pos)
		{
			node_id = idx;
			this->x_pos = x_pos;
			this->y_pos = y_pos;
		}

		void push(LINK *link)
		{
			linkcon.push(link);
		}
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
	int	qmax = 0;
	int	max_idx = 0;
	int	left_node_id, right_node_id;

	bool de;

	int	tem = LinkContainer.size();

	for (int i=0; i<MAXLINK; i++)
		schedule[i] = 0;

	while (!LinkContainer.empty())
	{
		qmax = 0;
		max_idx = 0;
		for (deque<LINK*>::iterator iter=LinkContainer.begin(); iter!=LinkContainer.end(); iter++)
		{
			if ((*iter)->q.size() >= qmax)
			{
				qmax = (*iter)->q.size();
				max_idx = (*iter)->link_id;
				left_node_id = (*iter)->prev->node_id;
				right_node_id = (*iter)->next->node_id;
			}
		}
		schedule[max_idx] = 1;
		for (deque<LINK*>::iterator iter=LinkContainer.begin(); iter!=LinkContainer.end(); )
		{
			if ((*iter)->link_id == max_idx || (*iter)->prev->node_id == left_node_id || (*iter)->next->node_id == left_node_id ||\
			(*iter)->prev->node_id == right_node_id || (*iter)->next->node_id == right_node_id)
			{
				LinkContainer.erase(iter);
				if (LinkContainer.empty())
					break;
			}
			else
				iter++;
		}
	}
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

		for (deque<LINK *>::iterator iter=LinkContainer.begin(); iter!=LinkContainer.end(); iter++)
		{
			double random = 1.0 * rand() / RAND_MAX;
			if (random < ((*iter)->arrival * rate))
			{
				(*iter)->q.push(creat_pkt(pkt_num++, time));
			}
		}

		// scheduling
		GreedyQ(LinkContainer, schedule);
		
		// actual pkt transmission
		for(int i=0; i<MAXLINK; i++) {
			if(schedule[i] == 1 && !LinkContainer[i]->q.empty()) {
				PACKET* pkt = LinkContainer[i]->pop();
				if(time>=999000)
				delay += time - pkt->gen_time;
				discard(pkt);
			}
		}
		
		// queue length evaluation (for 1000 time slots)
		if(time>=999000) {
			for(int i=0; i<MAXLINK; i++)
			queue_length += LinkContainer[i]->q.size();
		}
	}
	cout << rate << "	" << queue_length / 1000.0 << "	" << delay / 1000.0 << endl;
}
