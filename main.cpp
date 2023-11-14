#include <iostream>
#include "LoadBalancer.h"

vector<vector<double>> setParameters(int argc , char *argv[]);

int main(int argc, char *argv[]) {

    double service_time = stod(string(argv[1]));
    int servers_number = stoi(string(argv[2]));
    double request_rate = stod(string(argv[3+servers_number]));


    vector<double> distributeProb(servers_number);

            for(int index=0 ; index < servers_number; ++index){
                distributeProb[index] = (stod(argv[index+3]));
            }

            

    vector<int> serversQueueSizes(servers_number);

             for(int index=0 ; index < servers_number; ++index){
                serversQueueSizes[index] = (stoi(argv[4+servers_number+index]));
            }

     vector<double> processRate(servers_number);

            for (int index=0; index<servers_number; ++index){
                processRate[index] = stod(string(argv[4+2*servers_number+index]));
            }

    

    LoadBalancer load_balancer(service_time,servers_number,request_rate,distributeProb,serversQueueSizes
    ,processRate);

    load_balancer.stimulate();
}



