#include <iostream>
#include <stdlib.h>
#include <queue>
using namespace std;

#define MAXNODE 4

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
			prev = pr;
			next = nx;
		}
		void addPkt(PACKET* pkt) {
			// fill here
			link_queue.push(pkt);
		}
		PACKET* popPkt() {
			// fill here
			PACKET *tp;
			tp = 0;
			if (!link_queue.empty())
			{
				tp = link_queue.front();
				link_queue.pop();
			}
			return tp;
		}
		bool isEmpty() {
			return link_queue.empty();
		}
		int getLength() {
			return link_queue.size();
		}
};
class NODE {
	private :
		int node_id;
		LINK* link[4];
		queue<PACKET*> node_queue;
	public :
		void setLk(int idx, LINK* lk) {
			// fill here
			link[idx] = lk;
		}
		void GenPkt(int li_idx, int pkt_num, int timeslot) {
			// fill here
			link[li_idx]->addPkt(new PACKET(this, pkt_num, timeslot));
		}
		void recvPkt(PACKET* pkt) {
			node_queue.push(pkt);
		}
		int getLength() {
			return node_queue.size();
		}
};

int main() {
	int maxtime = 1000000;
	double lk_prob;
	int chan_idx;
	
	for(double R=0.2; R<=1; R+=0.02)
	{
		NODE* AP = new NODE();
		NODE* STA[4];
		LINK* lk[4];
		double ret = 0;
		int current_chan = 0;

		for (int i=0; i<4; i++)
		{
			STA[i] = new NODE();
			lk[i] = new LINK(AP, STA[i]);
			AP->setLk(i, lk[i]);
			STA[i]->setLk(0, lk[i]);
		}
		
		int pkt_num = 1;
		ret = 0;
		chan_idx = 0;
		for(int timeslot=1; timeslot<=maxtime; timeslot++) {
		
			double rangen = 1.0 * rand() / RAND_MAX;
			if(rangen < (0.15 * R))
			{
				AP->GenPkt(0, pkt_num, timeslot);
				pkt_num++;
			}
			rangen = 1.0 * rand() / RAND_MAX;
			if(rangen < (0.2 * R))
			{
				AP->GenPkt(1, pkt_num, timeslot);
				pkt_num++;
			}
			rangen = 1.0 * rand() / RAND_MAX;
			if(rangen < (0.25 * R))
			{
				AP->GenPkt(2, pkt_num, timeslot);
				pkt_num++;
			}
			rangen = 1.0 * rand() / RAND_MAX;
			if(rangen < (0.3 * R))
			{
				AP->GenPkt(3, pkt_num, timeslot);
				pkt_num++;
			}

			// int active_chan = -1;
			// int	attemp = 0;
			// for (int i=0; i<4; i++)
			// {
			// 	lk_prob = 1.0 * rand() / RAND_MAX;
			// 	if (lk_prob < 0.7 && !lk[i]->isEmpty())
			// 	{
			// 		active_chan = i;
			// 		attemp++;
			// 	}
			// }
			// if (attemp == 1 && !lk[active_chan]->isEmpty())
			// {
			// 	PACKET *packet = lk[active_chan]->popPkt();

			// 	if (packet != 0)
			// 		STA[active_chan]->recvPkt(packet);
			// }
			bool transmission_success = false;
    		int attempts = 0;
    
			for (int i=0; i<4; i++) {
				current_chan = (current_chan + 1) % 4; 
				
				if (!lk[current_chan]->isEmpty()) {
					lk_prob = 1.0 * rand() / RAND_MAX;
					if (lk_prob < 0.7) { 
						attempts++; 
						
						transmission_success = true;
						PACKET *packet = lk[current_chan]->popPkt();
						if (packet != 0)
							STA[current_chan]->recvPkt(packet);
						
						break;
					}
				}
			}

			if (timeslot > 999000)
			{
				for (int node_idx=0; node_idx<4; node_idx++)
					ret += lk[node_idx]->getLength();
			}
		}
		delete AP;
		for (int i=0; i<4; i++)
		{
			delete STA[i];
			delete lk[i];
			STA[i] = 0;
			lk[i] = 0;
		}
		AP=0;
		cout << R << "," << ret / 1000 << endl;
	}
}
