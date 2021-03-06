//################################
//Course: Operating Systems
//Semester: 2, 2019
//Name: Luoyang Xing
//ID: a1756195
//Assignment 1
//################################
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;

const int DEBUG_PRINT_QUEUE = false;

//define a customer class, each line in input file
//is a string line contains the customer object's info
class Customer
{
	public:
		string ID;
		int arTime;
		int narTime;//the arrtime when re-enter a queue
		int priority;
		int quantum;
		int age;
		int tickets;
		int ctkt;//trace the current tickets remaining to buy 
		int processCount;//priority++ when it is 3, in q1
		int waitRuns; //9 runs, priority-- in q2
		int readytime;
		int endtime;
		int runningtime;
		int cr;//current runningtime in one run
		int promote;//0 or 1
		int demote;//0 or 1

		Customer(){
			this->ID = "none";
			this->arTime = 999;
			this->narTime = 0;
			this->priority = 999;
			this->quantum = 0;
			this->age = 999;
			this->tickets = 999;
			this->ctkt=0;
			this->processCount = 0;
			this->waitRuns = 0; 
			this->readytime = 999;
			this->endtime = 0;
			this->runningtime = 0;
			this->cr=0;
			this->promote = 0;
			this->demote = 0;


		}
		
};

struct lessSort{
	bool operator () (Customer a, Customer b){
		//if (a.priority==b.priority) return ;
		return (a.priority > b.priority);
	}
};



int main(int argc, char const *argv[])
{
	fstream inFile;
	string line;
	//input the customer information file
	inFile.open("input.txt");

	//create a vector of string cus to store each customer's info
	vector<string> cus;
	string delimiter = " ";
	while (!inFile.eof())
	{
		getline(inFile, line);
		size_t pos = 0;
		string token;
		while ((pos = line.find(delimiter)) != string::npos) {
   		token = line.substr(0, pos);
   		cus.push_back(token);
    	line.erase(0, pos + delimiter.length());
		}
		cus.push_back(line);
	}
	inFile.close();

	//next we store these info in a vector of customer objects
	std::vector<Customer> cus0;
	for (int i = 0; i < cus.size()/5; ++i)
	{
		Customer c;
		c.ID = cus[(5*i)];
		stringstream iss (cus[5*i+1]);
		int x;
		iss >> x;
		c.arTime = x;
		c.narTime = x;
		stringstream isx(cus[5*i+2]);
		int y;
		isx>>y;
		c.priority = y;
		stringstream isy(cus[5*i+3]);
		int z;
		isy>>z;
		c.age = z;
		stringstream isw(cus[5*i+4]);
		int b;
		isw>>b;
		c.tickets = b;
		c.ctkt = b;
		cus0.push_back(c);
	}

	//we have a timer integer frm 0 and increment 5 each time
	int timer = 0;
	int total_times=0;//the total number of runs
	std::vector<Customer> readyq;//output

	priority_queue<Customer,vector<Customer>,lessSort> q1;
	vector<Customer> q2;

	std::vector<Customer> p;//for the customer object currently being procesed

	//get the last customer
	int last=0;
	int t=0;
	int Ftime;
	for (int i = 0; i < cus0.size(); ++i)
	{
		if (cus0[i].arTime>last)
		{
			last = cus0[i].arTime;
		}
	}
	for (int i = 0; i < cus0.size(); ++i)
	{
		if (cus0[i].arTime==last)
		{
			t = cus0[i].tickets;
		}
	}
	Ftime = last+t*5;
	while(timer<Ftime)
	{
		//every time the timer is at a time point, loop the customers' arrival time
		//to know if the customers have arrived, and put them in the corresponding queue
		for (size_t i = 0; i < cus0.size(); ++i)
		{
			if (cus0[i].narTime == timer)//for new arrivals
			{
				if (cus0[i].priority <= 3)
				{
					q1.push(cus0[i]); //q1 has the current time arrived customers in ascending priority
				}
				else
				{	
					q2.push_back(cus0[i]);
				}
			}
		}
		//q2 should be sorted by arrTime!!!!! not yet!


		//after the customers arrived being put to their queue
		//we check from high priority queue to low priority queue
		//and pick the correct customer to process
		if (p.empty())//no customer is being processed now
		{
			if (!q1.empty()) //at this time, process q1.top() 
			{
				p.push_back(q1.top()); 
				q1.pop();	//move 1st object in q1 to vector p , put back to q1 or delete after processing

				Customer c = p[0];
				int quantum = (8-c.priority)*10; //quantum time units
				int N = quantum/5;//quota
				
				//modify the object in the queue to store info
					p[0].quantum = quantum;
					if (c.readytime==999)
					{
						p[0].readytime = timer;//1st time, record the ready time
					}	
				
				//after , we increment timer and the total runs number
				total_times++;
				timer = timer +5;

			}
			
			else if (q1.empty()&&!q2.empty())
			{
				p.push_back(q2[0]); 
				q2.erase(q2.begin());	//move 1st object in q1 to vector p , put back to q1 or delete after processing
				Customer c = p[0];//q2 has highest priority

				//modify the object in the queue to store info
				p[0].quantum = 100;
				if (c.readytime ==999)
				{
					p[0].readytime = timer;
				}
				
				//after , we increment timer and total runs
				p[0].waitRuns = 0;//just processed, wait be 0 again for q2 item
				total_times++;
				timer = timer +5;
			}

			else if (q1.empty()&&q2.empty())//all queues are empty
			{
				//no elements, just increment timer
				timer = timer +5;	
			}
		}


		else//one customer is being processed in p vector
		{
			//check which customer is being processed and if there is pre-emptive
			if (!q1.empty()&&p[0].priority>3)
			{//q1 is non-preemptive q2 is preemptive
				//process p[0] one time , and put it back to q2, put q1 item in p initialize
				Customer c = p[0];
				if (c.ctkt>1&&c.cr<=95)// check the remaining tickets required, put back to q2
				{
					p[0].ctkt--;
					p[0].runningtime+=5;
					p[0].cr=0; //preempted, current runningtime be 0 again
					q2.push_back(p[0]);
					//erase vector p
					p.clear();
				}
					
				else if (c.ctkt==1)// end, 1 to 0, start new customer
				{
					p[0].endtime = timer;
					p[0].runningtime+=5;
					p[0].cr=0;
					//push_back to readyq
					readyq.push_back(p[0]);
					p.clear();
				}

				//loop q2 to age++
					for (int i = 0; i < q2.size(); ++i)
					{
						q2[i].waitRuns++;
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].waitRuns==8)
						{
							q2[i].waitRuns=0;
							q2[i].priority--;
						}
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].priority==3)
						{
							q1.push(q2[i]);
							q2.erase(q2.begin()+i);
						}
					}

				//put q1 item in p , initialize
				p.push_back(q1.top()); 
				q1.pop();//move 1st object in q1 to vector p , put back to q1 or delete after processing

				c = p[0];
				int quantum = (8-c.priority)*10; //quantum time units
				
				//modify the object in the queue to store info
				p[0].quantum = quantum;
				if (c.readytime==999)
				{
					p[0].readytime = timer;//1st time, record the ready time
				}	
				
				//after , we increment timer and the total runs number
				total_times++;
				timer = timer +5;
			}
			else if (q1.empty()&&p[0].priority>3)
			{
				//process p[0] one time
				Customer c = p[0];
				if (c.ctkt>1&&c.cr<95)// check the remaining tickets required
				{
					p[0].ctkt--;
					p[0].runningtime+=5;
					p[0].cr+=5;
				}
					
				else if (c.ctkt==1)// end, 1 to 0, start new customer
				{
					p[0].endtime = timer;
					p[0].runningtime+=5;
					p[0].cr=0;
					//push_back to readyq
					readyq.push_back(p[0]);
					p.clear();
					//loop q2 to age++
					for (int i = 0; i < q2.size(); ++i)
					{
						q2[i].waitRuns++;
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].waitRuns==8)
						{
							q2[i].waitRuns=0;
							q2[i].priority--;
						}
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].priority==3)
						{
							q1.push(q2[i]);
							q2.erase(q2.begin()+i);
						}
					}

					if (!q2.empty())
					{
						//put q2 item in p , initialize
						p.push_back(q2[0]); 
						q2.erase(q2.begin());//move 1st object in q1 to vector p , put back to q1 or delete after processing

						c = p[0];
						int quantum = 100; //quantum time units
				
						//modify the object in the queue to store info
						p[0].quantum = quantum;
						if (c.readytime==999)
						{
							p[0].readytime = timer;//1st time, record the ready time
						}
					}
					
				}
				else if (c.ctkt>1&&c.cr==95)//this quantum ends, need to go back to q2 to wait
				{
					p[0].ctkt--;
					p[0].runningtime+=5;
					p[0].cr=0;
					q2.push_back(p[0]);
					p.clear();
					//loop q2 to age++
					for (int i = 0; i < q2.size(); ++i)
					{
						q2[i].waitRuns++;
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].waitRuns==8)
						{
							q2[i].waitRuns=0;
							q2[i].priority--;
						}
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].priority==3)
						{
							q1.push(q2[i]);
							q2.erase(q2.begin()+i);
						}
					}

					//put q2 item in p , initialize
						p.push_back(q2[0]); 
						q2.erase(q2.begin());//move 1st object in q1 to vector p , put back to q1 or delete after processing

						c = p[0];
						int quantum = 100; //quantum time units
				
						//modify the object in the queue to store info
						p[0].quantum = quantum;
						if (c.readytime==999)
						{
							p[0].readytime = timer;//1st time, record the ready time
						}
				}



				timer+=5;
			}
			else if(p[0].priority<=3)
			{//current processing item is from q1
				//process p[0] one time
				Customer c = p[0];
				if (c.ctkt>1&&c.cr<c.quantum-5)// check the remaining tickets required
				{
					p[0].ctkt--;
					p[0].runningtime+=5;
					p[0].cr+=5;
				}
				else if (c.ctkt>1&&c.cr==c.quantum-5)//this quantum is not enough
				{
					p[0].ctkt--;
					p[0].runningtime+=5;
					p[0].cr=0;
					//increase processCount of q1 item
					p[0].processCount++;
					if (p[0].processCount==3)
					{
						p[0].processCount=0;
						p[0].priority++;
					}
					if (p[0].priority>3)
					{
						q2.push_back(p[0]);
					}else{
						q1.push(p[0]);
					}
					p.clear();
					//loop q2 to age++
					for (int i = 0; i < q2.size(); ++i)
					{
						q2[i].waitRuns++;
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].waitRuns==8)
						{
							q2[i].waitRuns=0;
							q2[i].priority--;
						}
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].priority==3)
						{
							q1.push(q2[i]);
							q2.erase(q2.begin()+i);
						}
					}
					//put new element
					if (!q1.empty())
					{
						//put q2 item in p , initialize
						p.push_back(q1.top()); 
						q1.pop();//move 1st object in q1 to vector p , put back to q1 or delete after processing

						c = p[0];
						int quantum = 100; //quantum time units
				
						//modify the object in the queue to store info
						p[0].quantum = quantum;
						if (c.readytime==999)
						{
							p[0].readytime = timer;//1st time, record the ready time
						}

					}
					else{
						//put q2 item in p , initialize
						p.push_back(q2[0]); 
						q2.erase(q2.begin());//move 1st object in q1 to vector p , put back to q1 or delete after processing

						c = p[0];
						int quantum = 100; //quantum time units
				
						//modify the object in the queue to store info
						p[0].quantum = quantum;
						if (c.readytime==999)
						{
							p[0].readytime = timer;//1st time, record the ready time
						}
					}

				}
				else if(c.ctkt==1)
				{
					p[0].endtime = timer;
					p[0].runningtime+=5;
					p[0].cr=0;
					//push_back to readyq
					readyq.push_back(p[0]);
					p.clear();
					//loop q2 to age++
					for (int i = 0; i < q2.size(); ++i)
					{
						q2[i].waitRuns++;
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].waitRuns==8)
						{
							q2[i].waitRuns=0;
							q2[i].priority--;
						}
					}
					for (int i = 0; i < q2.size(); ++i)
					{
						if (q2[i].priority==3)
						{
							q1.push(q2[i]);
							q2.erase(q2.begin()+i);
						}
					}


					//put new element
					if (!q1.empty())
					{
						//put q2 item in p , initialize
						p.push_back(q1.top()); 
						q1.pop();//move 1st object in q1 to vector p , put back to q1 or delete after processing

						c = p[0];
						int quantum = 100; //quantum time units
				
						//modify the object in the queue to store info
						p[0].quantum = quantum;
						if (c.readytime==999)
						{
							p[0].readytime = timer;//1st time, record the ready time
						}

					}
					else{
						//put q2 item in p , initialize
						p.push_back(q2[0]); 
						q2.erase(q2.begin());//move 1st object in q1 to vector p , put back to q1 or delete after processing

						c = p[0];
						int quantum = 100; //quantum time units
				
						//modify the object in the queue to store info
						p[0].quantum = quantum;
						if (c.readytime==999)
						{
							p[0].readytime = timer;//1st time, record the ready time
						}
					}
				}
				timer+=5; //after initialize a new customer in p, timer increments
			}
		}	
	}


	//output 
	string s = "name   arrival   end   ready   running   waiting";
	cout<<s<<endl;

	for (int i = 0; i < readyq.size(); ++i)
	{
		int ss = readyq[i].endtime-readyq[i].readytime-readyq[i].runningtime;
		cout<<std::setiosflags(std::ios::right)<<readyq[i].ID<<setw(10)<<readyq[i].arTime<<setw(10)<<readyq[i].endtime<<setw(10)<<readyq[i].readytime<<setw(10)<<readyq[i].runningtime<<setw(10)<<ss<<endl;
	}



	return 0;
}