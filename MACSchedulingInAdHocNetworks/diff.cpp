#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <deque>
#include <queue>
#include <list>
#include <vector>
#include <iomanip> // for setprecision
using namespace std;

#define MAXNODE 20
#define MAXTIME 1000000
#define SEED 1

int MAXLINK = 0;

class NODE;
class LINK;
void GreedyQ(const deque<LINK*>& LinkContainer, int* schedule);

class PACKET {
public:
    int pkt_id;
    int gen_time;
    PACKET(int id, int time) : pkt_id(id), gen_time(time) {}
};

class LINK {
public:
    int link_id;
    NODE* n1;
    NODE* n2;
    double arrival;
    deque<PACKET*> q;

    LINK(int id, NODE* a, NODE* b)
        : link_id(id), n1(a), n2(b), arrival(0.0) {
    }

    PACKET* pop() {
        if (q.empty()) return nullptr;
        PACKET* pkt = q.front();
        q.pop_front();
        return pkt;
    }
};

class NODE {
public:
    int node_id;
    double x_pos, y_pos;
    vector<LINK*> connectedLinks;

    NODE(int id, double x, double y)
        : node_id(id), x_pos(x), y_pos(y) {
    }

    void push(LINK* l) {
        connectedLinks.push_back(l);
    }
};

bool isClose(NODE* n1, NODE* n2) {
    double dx = n1->x_pos - n2->x_pos;
    double dy = n1->y_pos - n2->y_pos;
    return sqrt(dx * dx + dy * dy) < 0.35;
}

bool isInterfere(LINK* L1, LINK* L2) {
    return (L1->n1 == L2->n1 || L1->n1 == L2->n2 || L1->n2 == L2->n1 || L1->n2 == L2->n2);
}

void GreedyQ(const deque<LINK*>& LinkContainer, int* schedule) {
    for (int i = 0; i < MAXLINK; i++) schedule[i] = 0;
    list<LINK*> candidate(LinkContainer.begin(), LinkContainer.end());

    while (!candidate.empty()) {
        LINK* best = nullptr;
        int maxQ = -1;
        for (auto& l : candidate) {
            if ((int)l->q.size() > maxQ) {
                maxQ = l->q.size();
                best = l;
            }
        }
        if (!best) break;
        schedule[best->link_id] = 1;
        for (auto it = candidate.begin(); it != candidate.end(); ) {
            if (*it == best || isInterfere(*it, best)) it = candidate.erase(it);
            else ++it;
        }
    }
}

int main() {
    cout << fixed << setprecision(3);

    for (int i = 70; i <= 99; i++) {
        double rho = i / 100.0;
        MAXLINK = 0;

        NODE* NodeContainer[MAXNODE];
        deque<LINK*> LinkContainer;
        srand(SEED);

        for (int i = 0; i < MAXNODE; i++)
            NodeContainer[i] = new NODE(i, 1.0 * rand() / RAND_MAX, 1.0 * rand() / RAND_MAX);

        for (int i = 0; i < MAXNODE; i++) {
            for (int j = i + 1; j < MAXNODE; j++) {
                if (isClose(NodeContainer[i], NodeContainer[j])) {
                    LINK* tl = new LINK(MAXLINK++, NodeContainer[i], NodeContainer[j]);
                    double ran = 1.0 * rand() / RAND_MAX;
                    if (ran < 0.4) tl->arrival = 0.0;
                    else if (ran < 0.7) tl->arrival = 0.1;
                    else if (ran < 0.9) tl->arrival = 0.2;
                    else tl->arrival = 0.3;
                    LinkContainer.push_back(tl);
                    NodeContainer[i]->push(tl);
                    NodeContainer[j]->push(tl);
                }
            }
        }

        int* schedule = new int[MAXLINK];
        int pkt_id = 0;
        double queue_sum = 0.0, delay_sum = 0.0;

        for (int time = 0; time < MAXTIME; time++) {
            for (int i = 0; i < MAXLINK; i++) {
                double prob = LinkContainer[i]->arrival * rho;
                double r = 1.0 * rand() / RAND_MAX;
                if (r < prob)
                    LinkContainer[i]->q.push_back(new PACKET(pkt_id++, time));
            }

            GreedyQ(LinkContainer, schedule);

            for (int i = 0; i < MAXLINK; i++) {
                if (schedule[i]) {
                    PACKET* pkt = LinkContainer[i]->pop();
                    if (pkt && time >= 999000)
                        delay_sum += (time - pkt->gen_time);
                    delete pkt;
                }
            }

            if (time >= 999000) {
                for (int i = 0; i < MAXLINK; i++)
                    queue_sum += LinkContainer[i]->q.size();
            }
        }

        cout << rho << "\t" << queue_sum / 1000.0 << "\t" << delay_sum / 1000.0 << endl;

        delete[] schedule;
        for (int i = 0; i < MAXNODE; i++) delete NodeContainer[i];
        for (auto l : LinkContainer) delete l;
    }

    return 0;
}