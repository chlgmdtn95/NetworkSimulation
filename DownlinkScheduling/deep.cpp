#include <iostream>
#include <cstdlib>
#include <queue>
#include <ctime>
#include <vector>
using namespace std;

// 패킷 클래스
class PACKET {
public:
    int id;             // 패킷 ID
    int gen_time;       // 생성 시간
    int retry_count;    // 재전송 횟수

    PACKET(int _id, int _gen_time) : id(_id), gen_time(_gen_time), retry_count(0) {}
};

// 링크 클래스 (AP-STA 간 연결)
class LINK {
private:
    queue<PACKET*> tx_queue;    // 전송 대기 큐
    int collisions;             // 충돌 횟수
    int max_queue_size;         // 최대 큐 크기 (오버플로 방지)

public:
    LINK() : collisions(0), max_queue_size(10000) {}

    // 패킷 추가 (재전송 여부에 따라 처리)
    bool addPacket(PACKET* pkt, bool is_retransmit = false) {
        if (tx_queue.size() >= max_queue_size) {
            delete pkt;  // 큐가 가득 차면 패킷 폐기
            return false;
        }
        if (is_retransmit) {
            pkt->retry_count++;
            collisions++;
        }
        tx_queue.push(pkt);
        return true;
    }

    // 패킷 추출 (전송 시도)
    PACKET* popPacket() {
        if (tx_queue.empty()) return nullptr;
        PACKET* pkt = tx_queue.front();
        tx_queue.pop();
        return pkt;
    }

    bool isEmpty() const { return tx_queue.empty(); }
    int getQueueSize() const { return tx_queue.size(); }
    int getCollisions() const { return collisions; }
};

// 노드 클래스 (AP 또는 STA)
class NODE {
private:
    int id;
    vector<LINK*> links;    // 연결된 링크들

public:
    NODE() {}
    NODE(int _id) : id(_id) {}
    void addLink(LINK* link) { links.push_back(link); }

    // 패킷 생성 (AP에서 호출)
    void generatePacket(int ch, int pkt_id, int time) {
        if (ch < 0 || ch >= links.size()) return;
        links[ch]->addPacket(new PACKET(pkt_id, time));
    }

    // 패킷 수신 (STA에서 호출)
    void receivePacket(PACKET* pkt) {
        delete pkt;  // 실제로는 처리 로직이 필요하지만, 여기서는 단순 삭제
    }
};

// 메인 시뮬레이션
int main() {
    srand(time(0));
    const int MAX_TIME = 1000000;    // 총 슬롯 수
    const int WARMUP = 990000;       // 초기 안정화 기간
    const int N_CHANNELS = 4;        // 채널 수
    const double P_TX = 0.7;         // 전송 시도 확률

    cout << "R,Avg Queue Length" << endl;

    // R 값 변화 (0.1 ~ 1.0)
    for (double R = 0.1; R <= 1.0; R += 0.02) {
        // 네트워크 초기화
        NODE AP(0);                     // AP 노드
        NODE STA[N_CHANNELS];            // STA 노드들
        LINK links[N_CHANNELS];          // AP-STA 링크들

        for (int i = 0; i < N_CHANNELS; i++) {
            STA[i] = NODE(i + 1);
            AP.addLink(&links[i]);
            STA[i].addLink(&links[i]);
        }

        int pkt_id = 1;
        double total_queue_length = 0;
        int active_slots = 0;

        // 슬롯 단위 시뮬레이션
        for (int slot = 1; slot <= MAX_TIME; slot++) {

            if ((rand() / (double)RAND_MAX) < (R * 0.15)) {
                AP.generatePacket(0, pkt_id++, slot);
            }
            if ((rand() / (double)RAND_MAX) < (R * 0.2)) {
                AP.generatePacket(1, pkt_id++, slot);
            }
            if ((rand() / (double)RAND_MAX) < (R * 0.25)) {
                AP.generatePacket(2, pkt_id++, slot);
            }
            if ((rand() / (double)RAND_MAX) < (R * 0.3)) {
                AP.generatePacket(3, pkt_id++, slot);
            }

            

            // 2. 채널 액세스 및 충돌 검사
            vector<int> tx_channels;  // 전송 시도한 채널들

            for (int ch = 0; ch < N_CHANNELS; ch++) {
                if (!links[ch].isEmpty() && (rand() / (double)RAND_MAX) < P_TX) {
                    tx_channels.push_back(ch);
                }
            }

            // 3. 충돌 처리
            if (tx_channels.size() == 1) {
                // 성공적 전송
                PACKET* pkt = links[tx_channels[0]].popPacket();
                STA[tx_channels[0]].receivePacket(pkt);
            } else if (tx_channels.size() > 1) {
                // 충돌 발생 → 모든 패킷 재전송 대기열로
                for (int ch : tx_channels) {
                    PACKET* pkt = links[ch].popPacket();
                    if (pkt) links[ch].addPacket(pkt, true);
                }
            }

            // 4. 성능 측정 (WARMUP 이후만)
            if (slot > WARMUP) {
                for (int ch = 0; ch < N_CHANNELS; ch++) {
                    total_queue_length += links[ch].getQueueSize();
                }
                active_slots++;
            }
        }

        // 결과 출력 (평균 큐 길이)
        cout << R << "," << total_queue_length / 1000 << endl;
    }

    return 0;
}