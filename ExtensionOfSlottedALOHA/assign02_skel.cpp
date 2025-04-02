#include <iostream>
#include <queue>
#include <stdlib.h>
using namespace std;

#define NUM_NODE 10

class Packet {
	private :
		int packet_ID;
		int gen_time;
		int dest_time;
	public :
		Packet() {}
		Packet(int id, int time) {
			// ... 

		}
		void setDT(int time) {	// record the arrival time 
			// ... 

		}
		int getDelay() {
			// ... 

			return 0;	// remove this
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
			// ...

			return 0.0; // remove this
		}
};

void tx_success(int time, Node* former, Node* next) {
	// ...

}

int main() {

	int MAXTIME = 100000;
	int pkt_num;
	bool attempt[NUM_NODE];

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
			// stations attempt transmission with prob. (attemp_prob)
			// ....


			// check if transmission(s) are successful 
			// ....


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
