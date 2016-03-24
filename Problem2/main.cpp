//
//  main.cpp
//  Problem1
//
//  Created by TangZekun on 5/7/15.
//  Copyright (c) 2015 TangZekun. All rights reserved.
//

#include <iostream>
#include <queue>
#include "math.h"
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <utility>      // std::pair, std::make_pair
#include <map>
#include <math.h>



using namespace std;
int num_machine =10;
int num_turn = 100;
int goal = num_machine*num_turn;
int total_turn =0;
//vector<pair<double,int> > machine(num_machine);
//int waittingline = 0;


//---------For Calculation -----
double k=num_machine;
double aver_ser_time_rate= 0.2;
double aver_run_time_rate = 0.05;
double p = aver_run_time_rate/aver_ser_time_rate;

//---------For Calculation -----


// Seed the random number generator one time only.
std::random_device rd;


double milltime()
{
    std::default_random_engine generator(rd());
    std::exponential_distribution<double> distribution(20);
    auto dice= std::bind(distribution,generator);
    cout << fixed << setprecision(2);
    
    double probability = dice();
    double mt = (1-probability)*20;
    return mt;
}


double servicetime()
{
    std::default_random_engine generator(rd());
    std::exponential_distribution<double> distribution(5);
    auto dice= std::bind(distribution,generator);
    cout << fixed << setprecision(2);
    
    
    double probability = dice();
    double mt = (1-probability)*5;
    return mt;
}


//-------formulas for calculation---------

int factorial(int n)
{
    int result =0;
    if (n == 0)
    {
        result =1;
    }
    else
    {
        result = factorial (n-1) * n;
    }
    return result;
    
}

double p0_0summation (int c, int k)
{
    double result =0;
    for(int n =0; n<= c-1; n++)
    {
        result += (factorial(k)/((factorial(k-n))*(factorial(n)))) * pow(p,n);
    }
    return result;
    
}


double p0_1summation (int c, int k)
{
    double result =0;
    for(int n=c; n<=k; n++)
    {
        result += (factorial(k)/((factorial(k-n)) * (factorial(c))* (pow(c,n-c)))) * (pow(p,n));
        
    }
    return result;
}


double p0 (int c, int k)
{
    double  result =0;
    result =  pow((p0_0summation ( c,  k) +p0_1summation (c,k)),-1);
    
    return result;
}




double pn(int c, int k, int n)
{
    double result =0;
    if(n >=0 && n<=c-1 )
    {
        result = ((factorial(k)/((factorial(k-n))*(factorial(n)))) * pow(p,n)) * p0(c,k);
    }
    else if  (n >= c && n<=k)
    {
        result = ((factorial(k)/((factorial(k-n)) * (factorial(c))* (pow(c,n-c)))) * (pow(p,n))) *p0(c,k);
    }
    return result;
}

double effect (int c, int k)
{
    double result = 0;
    for(int n =0; n<=k; n++)
    {
        result += (k-n)*(aver_run_time_rate)*pn(c,k,n);
    }
    return result;
}


double server_utilization (int c, int k)
{
    return (effect(c,k)/(c*(aver_ser_time_rate)));
    
}





//-------formulas for calculation---------



int main()
{
    int num_server=0;
    cout << setprecision(3);
    cout << "Enter the number of servers (from 1 to 3):\n>";
    cin>>num_server;
    double c = num_server;
    
    
    double current_time =0;
    double end_worktime =0;
    double end_servicetime =0;
    double this_servicetime =0;
    
    
    // The queue of machines that are running.
    typedef pair <double,int> P;
    priority_queue< P, vector<P>, greater<P> > runningqueue;
    
    // The queue of machines that are waiting.
    queue< pair <double,int> > waittingqueue;
    
    // The queue of machines that are being serviced.
    priority_queue< P, vector<P>, greater<P> > serverqueue;
    
    // A vector of the time that each server is next available.  (one element per server)
    priority_queue<double,vector<double>, greater<double>> endservicevector;
    for(int i = 0; i < num_server; i++) {
        endservicevector.push(0);
    }
    
    vector<int> counter (num_machine);
    
    // A vector of machines that is used to initialize all of the machines
    vector<pair<double,int> > machine(num_machine);
    
    // A vector of machines that totally done.
    vector<pair<double,int> > finished(num_machine);
    
    // Initialize the 10 machines.
    for (int i =0; i<num_machine; i++)
    {
        machine[i].first = milltime();
        machine[i].second = i;
        runningqueue.push(machine[i]);
        std::cout<<"< "<<current_time<<" > : Machine < " <<i+1<<" > starts running. "<<std::endl;
    }
    
    
    while (!runningqueue.empty() )
    {
        // Each of these pairs has a time and a machine number.
        // This is a machine that ...
        pair<double,int> workinglinepopped (0.0, 0);  // ... was running and then finished its run.
        pair<double,int> waittinglinepopped (0.0, 0); // ... was waiting for service, and then got service.
        pair<double,int> servinglinepopped (0.0, 0);  // ... was being serviced and it is ready to run again.
        
        workinglinepopped = runningqueue.top();
        end_worktime = workinglinepopped.first;
        //cout<<"Mahcine--"<<workinglinepopped.second+1<<endl;
        //cout<<"end_worktime---" <<end_worktime<<endl;
        runningqueue.pop();
        
        waittingqueue.push(workinglinepopped);
        
        // If the server is idle
        if(total_turn < goal)
        {
            waittinglinepopped = waittingqueue.front();
            
            if(total_turn<num_server)// for the first two machines for servers
            {
                // The first time that each server is asked for service.
                //cout<<"Server initial service begin at--"<<waittinglinepopped.first<<endl;
                this_servicetime =servicetime();// generate service time
                waittinglinepopped.first +=this_servicetime;
                //cout<<"this_servicetime--"<<this_servicetime<<endl;
                //cout<<"service ends at--"<<waittinglinepopped.first<<endl;
                end_servicetime =waittinglinepopped.first;
                endservicevector.push(end_servicetime);
                endservicevector.pop();
                waittingqueue.pop();
            }
            else// for the rest machines that a server is asked for service
            {   // All subsequent times that a server is asked for service
                this_servicetime =servicetime();
                if(waittinglinepopped.first>endservicevector.top())
                {
                    //cout<<"service begin at--"<<waittinglinepopped.first<<endl;
                    waittinglinepopped.first =waittinglinepopped.first+this_servicetime;
                    //cout<<"this_servicetime--"<<this_servicetime<<endl;
                    //cout<<"service ends at--"<<waittinglinepopped.first<<endl;
                    end_servicetime =waittinglinepopped.first;
                    endservicevector.push(end_servicetime);
                    endservicevector.pop();
                    waittingqueue.pop();
                }
                else{
                    //cout<<"service begin at--"<<endservicevector.top()<<endl;
                    waittinglinepopped.first =endservicevector.top()+this_servicetime;
                    //cout<<"this_servicetime--"<<this_servicetime<<endl;
                    //cout<<"service ends at--"<<waittinglinepopped.first<<endl;
                    end_servicetime =waittinglinepopped.first;
                    endservicevector.push(end_servicetime);
                    endservicevector.pop();
                    waittingqueue.pop();
                }
            }
            
            
            serverqueue.push(waittinglinepopped);
            
            servinglinepopped = serverqueue.top();
            //cout<<" for check "<<machine[servinglinepopped.second].second<<endl;
            ++counter[machine[servinglinepopped.second].second];
            ++total_turn;
            //cout<<"total turn --"<<total_turn<<endl;
            //++machine[servinglinepopped.second].second;
            end_servicetime =servinglinepopped.first;
            serverqueue.pop();
            //end_servicetime =servinglinepopped.first;
            
            cout<<"< "<<end_servicetime<<" > : Machine < " <<servinglinepopped.second+1<<" > starts running. "<<endl;
            cout<<endl;
            
            if(counter[machine[servinglinepopped.second].second] <num_turn)
            {
                servinglinepopped.first +=milltime();
                runningqueue.push(servinglinepopped);
                
                
            }
            else
            {
                finished.push_back(servinglinepopped);
            }
            
        }
        
        
    }
    
    //------------------Calculation---------------------
    
    
    
    /*cout<<p0_0summation(c,k)<<endl;
     cout<<p0_1summation(c,k)<<endl;
     cout<<p0(c,k)<<endl;
     cout<<effect(c,k)<<endl;*/
    cout<<"Server Utilization" <<server_utilization(c, k)<<endl;
    
    
    
}
