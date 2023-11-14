#ifndef MINENETWORKS_LOADBALANCER_H
#define MINENETWORKS_LOADBALANCER_H
#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <numeric>


using namespace std;

typedef struct request{
    double QArrivalTime;
    double QExitTime;
    double ProssesTime;
}Request;

Request createRequest(double QArrivalTime,double QExitTime, double ProssesTime){
    Request req;
    req.QArrivalTime  = QArrivalTime;
    req.QExitTime = QExitTime;
    req.ProssesTime = ProssesTime;
    return req;

}

 typedef struct serverstats{
    int queue_size;
    double service_rate;
    std::queue<Request> queue;
}Serverstats;


class LoadBalancer {

    double TotalTime;
    int ServersNumber;
    double Lambda;
    int totalRequestNum;
    int droppedRequestsNum;
    double lastReqExitTime;
    vector<double> m_probabilities;
    vector<Serverstats> servers;

    double avgWaitTime;
    double avgProssessTime;
    double totalWaitTime;
    double totalProssesTime;

    static int selectRandomServer(vector<double>& probabilities);
    void insertEvent(Request& event ,Serverstats& selectedServer);
    void loadBalancerStimulateEvents();
    void calculateStats();


public:
    void stimulate();

    LoadBalancer(double TotalTime ,int ServersNumber ,double Lambda , vector<double> & server_dist_prob,
    vector<int> server_queue_size, vector<double> server_rate):TotalTime(TotalTime),ServersNumber(ServersNumber),
    Lambda(Lambda),totalRequestNum(0),droppedRequestsNum(0),lastReqExitTime(0),servers(vector<Serverstats>(ServersNumber)),
    avgWaitTime(0),avgProssessTime(0),totalWaitTime(0),totalProssesTime(0){
        for(int index=0;index<ServersNumber;++index){
            Serverstats& server = servers[index];
            server.queue_size = server_queue_size[index];
            server.service_rate = server_rate[index];
            server.queue = std::queue<Request>();
        }
         m_probabilities = server_dist_prob;
    }

    ~LoadBalancer()= default;

};


    void LoadBalancer::stimulate(){
        loadBalancerStimulateEvents();
        calculateStats();
        cout << totalRequestNum-droppedRequestsNum << " " << droppedRequestsNum << " " << lastReqExitTime
        << " " << avgWaitTime << " " << avgProssessTime << endl ;

    }

    static inline double generatePoissian(double rate) {
        std::random_device generator;
        std::exponential_distribution<double> exp_var(rate);
        return exp_var(generator);
    }


    void LoadBalancer::loadBalancerStimulateEvents(){

        double latest_event_time = 0.0;
        int selected_server = -1;

            while(latest_event_time < TotalTime){
                latest_event_time = generatePoissian(Lambda) + latest_event_time;
                selected_server = LoadBalancer::selectRandomServer(m_probabilities);
                totalRequestNum++;
                if(servers[selected_server].queue.front().QExitTime > latest_event_time &&
                    servers[selected_server].queue.size() > servers[selected_server].queue_size){
                    droppedRequestsNum++;
                }else{
                      Request req = createRequest(latest_event_time,0,0);
                      insertEvent(req,servers[selected_server]);
                }
            }
    }

    void LoadBalancer::insertEvent(Request& event,Serverstats& selectedServer){

         if(selectedServer.queue.empty()){
               event.ProssesTime = generatePoissian(selectedServer.service_rate);
               event.QExitTime = event.QArrivalTime + event.ProssesTime;
               totalProssesTime+=event.ProssesTime;
               selectedServer.queue.push(event);
               return;
            }

            if(selectedServer.queue.size() > selectedServer.queue_size){
                selectedServer.queue.pop();
            }

        event.ProssesTime = generatePoissian(selectedServer.service_rate);
        event.QExitTime = std::max(selectedServer.queue.back().QExitTime, event.QArrivalTime) + event.ProssesTime;
        totalWaitTime+= std::max(selectedServer.queue.back().QExitTime - event.QArrivalTime,0.0) ;

        totalProssesTime+=event.ProssesTime;
        selectedServer.queue.push(event);

        if(event.QExitTime > lastReqExitTime){
            lastReqExitTime = event.QExitTime;
        }


    }

    inline int LoadBalancer::selectRandomServer(vector<double>& probabilities){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<> dis(probabilities.begin(), probabilities.end());
        int index = dis(gen);
        return index;
    }

    inline void LoadBalancer::calculateStats(){
        avgProssessTime = totalProssesTime /(double) (totalRequestNum - droppedRequestsNum);
        avgWaitTime = totalWaitTime /(double) (totalRequestNum - droppedRequestsNum);
    }


#endif








