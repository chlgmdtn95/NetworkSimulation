#include <iostream>
#include <stdlib.h>
#include <queue>
using namespace std;

#define MAXNODE 2

class NODE;
class LINK;
class PACKET {
	private :
		NODE* source;
		int id;
		int gen_time;
		int dest_time;
	public :
		PACKET(NODE* src, int iid, int time) {
			source = src;
			id = iid;
			gen_time = time;
		}
};
class LINK {
	private :
		NODE* prev;
		NODE* next;
		queue<PACKET*> link_queue;
	public :
		LINK(NODE* pr, NODE* nx) {
			// fill here
		}
		void addPkt(PACKET* pkt) {
			// fill here
		}
		PACKET* popPkt() {
			// fill here
			return tp;
		}
		bool isEmpty() {
			return link_queue.empty();
		}
		void getLength() {
			cout << "Link 1 (queue) has: " << link_queue.size() << " pkt(s)\n";
		}
};
class NODE {
	private :
		int node_id;
		LINK* link;
		queue<PACKET*> node_queue;
	public :
		void setLk(LINK* lk) {
			// fill here
		}
		void GenPkt(int pkt_num, int timeslot) {
			// fill here
		}
		void recvPkt(PACKET* pkt) {
			node_queue.push(pkt);
		}
		int getLength() {
			return node_queue.size();
		}
};

int main() {
	NODE* AP = new NODE();
	NODE* STA = new NODE();
	LINK* lk = new LINK(AP, STA);
	AP->setLk(lk);
	STA->setLk(lk);
	int maxtime = 3;
	int pkt_num = 1;

	for(int timeslot=1; timeslot<=maxtime; timeslot++) {
		cout << "time=" << timeslot << endl;
		lk->getLength();
		double rangen = 1.0 * rand() / RAND_MAX;
		int lp;
		if(rangen < 0.25)
			lp = 0;
		else if(rangen < 0.5)
			lp = 1;
		else if(rangen < 0.75)
			lp = 2;
		else
			lp = 3;
		for(int i=0; i<lp; i++)
			AP->GenPkt(pkt_num++, timeslot);
		cout << "Node 1 creates:	" << lp << " pkt(s)\n";
		lk->getLength();
		if(!lk->isEmpty()) {
			PACKET* packet = lk->popPkt();
			STA->recvPkt(packet);
			cout << "Node 2 receives:	1 pkt(s)\n";
		}
		lk->getLength();
		cout << endl;
	}
}
