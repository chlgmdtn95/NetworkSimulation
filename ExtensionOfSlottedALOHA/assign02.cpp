#include <iostream>
#include <queue>
#include <stdlib.h>
#include <random>
#include <iterator>
using namespace std;

#define NUM_NODE 10

class Packet {
	private :
		int packet_ID;
		int gen_time;
		int dest_time;
		Packet() {}

	public :
		Packet(int id, int time) {
			packet_ID = id;
			gen_time = time;
		}
		void setDT(int time) {	// record the arrival time 
			dest_time = time;
		}
		int getDelay() {
			return dest_time - gen_time;	// remove this
		}
};

class Node {
	private :
		int Node_ID;
		queue<Packet*> q;
		
		public :
		Node() {}

		Node(int id) {
			Node_ID = id;
		}
		bool isEmpty() {
			if(q.empty())
				return true;
			else
				return false;
		}
		void addPacket(Packet* pkt) {
			q.push(pkt);
		}
		int qlen() {
			return q.size();
		}
		Packet* popPacket() {
			Packet* p = q.front();
			q.pop();
			return p;
		}
		double avg_delay() {
			double ret = 0;
			int len = qlen();
			Packet	*tem;

			while (!q.empty())
			{
				tem = q.front();
				q.pop();
				if (tem != 0)
					ret += (double)(tem->getDelay());
			}

			return ret / len;
		}
};

void tx_success(int time, Node* former, Node* next) {
	// ...
	Packet	*tem;

	tem = former->popPacket();
	tem->setDT(time);
	next->addPacket(tem);
}

int main() {

	int MAXTIME = 100000;
	int pkt_num;
	bool attempt[NUM_NODE];
	int collision;

	for(double prob = 0.01; prob < 0.25; prob = prob + 0.01) {

		Node* AP = new Node();
		Node* NodeContainer[NUM_NODE];
		for(int i=0; i<NUM_NODE; i++) {
			attempt[i] = false;
			Node* temp = new Node(i);
			NodeContainer[i] = temp;
		}
		pkt_num = 0;

		for(int i=0; i<MAXTIME; i++) {
			for (int idx=0; idx<NUM_NODE; idx++)
			{
				if (NodeContainer[idx]->isEmpty())
					NodeContainer[idx]->addPacket(new Packet(pkt_num, i));
				pkt_num++;
			}
			
			// stations attempt transmission with prob. (attemp_prob)
			// ....
			for (int idx=0; idx<NUM_NODE; idx++)
			{
				// std::cout << "rand: " << 1.0 * rand() / RAND_MAX << std::endl;
				if ((1.0 * rand() / RAND_MAX) < prob)
					attempt[idx] = true;
				else
					attempt[idx] = false;
			}


			// check if transmission(s) are successful 
			// ....
			collision = 0;
			for(int idx=0; idx<NUM_NODE; idx++)
			{
				if (attempt[idx] == true)
					collision++;
				if (collision > 1)
					break;
			}
			if (collision == 1)
			{
				for (int idx=0; idx<NUM_NODE; idx++)
				{
					if (attempt[idx] == 1)
					{
						tx_success(i, NodeContainer[idx], AP);
						// std::cout << "	i: " << i << " prob: " << prob << std::endl;
					}
					break;
				}
			}
			// if successful, 
			//   move the packet from source node to AP
			//   and record the delay 
			// ....
		}
		cout << prob << "	" \
			<< 1.0* AP->qlen() / MAXTIME << "	" \
			<< AP->avg_delay() << endl;
	}
}
