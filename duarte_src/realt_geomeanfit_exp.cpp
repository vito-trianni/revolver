
// with stimulus update per ant, or per timestep (see define)

//---------------------------------------------------------------------------

#include <vector>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include "random.h"
#include <fstream>
#include <cassert>
#include <algorithm>
#include "mygetch.h"
#include <sys/stat.h>
#include <sstream>
//#define DEBUG
//#define SIMULTANEOUS_UPDATE
//#define ACC_FITNESS
//#define STOPCODE
//---------------------------------------------------------------------------

using namespace std;


struct Params
{
    int N; //number of workers
    int Col; // number of colonies
    int maxtime; // time steps
    double p;  // quitting probability
    int tasks;
    double mutp;//mutation probability
    int maxgen;
    double beta_fit, gama_fit;
    int seed;
    double recomb;
    int timecost;
    double mutstep;
    double initStim; // initial stimulus value
    double p_wait; // probability that ant has to wait c time steps before switching
    int tau; // time step from which fitness is counted
    double fit_broadness;

    vector<double> meanT; // mean threshold 1
    vector<double> delta;
    vector<double> alfa;
    vector<double> beta;
    
    istream & InitParams(istream & inp);
};

struct Ant
{
// genome
vector < double > threshold;
// behaviour
//vector < bool > act; // active or not at task 1 or 2
vector < int > countacts;   // counter of acts done by this ant
vector < bool > want_task;
int last_act;
int curr_act;
int switches; // transitions to a different task
int workperiods; // number of working periods 
double F; // specialization value
bool mated;
double F_franjo;
int count_time; //COUNTER OF TIMESTEPS TO SWITCH TASK
};

typedef vector < Ant > Workers;
typedef vector <Ant> Sexuals;

struct Colony
{
Workers MyAnts;
Ant male, queen;
int ID;
vector<double> stim;
vector<double> newstim;
vector<double> workfor;  // number acts * eff each time step
vector < int > numacts; // number of acts performed per task
double idle; // proportion workers that _never_ worked in the simulation 
double inactive; // proportion workers that were idle each time step
vector <double>last_half_acts; //number of acts performed in the last half of simulation
double work_fitness; // product of number of acts for each task, every time step (of half of simulation)
double fitness;
double diff_fit;// fitness difference to minimal fitness
double rel_fit; // fitness relative to whole population
double cum_fit; //cumulative fitness
vector<double>HighF, LowF, CategF;
vector<double> HighT, LowT, CategT1, CategT2;
vector<double>mean_work_alloc;
double mean_F;
double var_F;
double mean_F_franjo;
double var_F_franjo;
int num_offspring;
double mean_switches;
double var_switches;
double mean_workperiods;
double var_workperiods;
};

typedef vector < Colony > Population;
vector <int> parentCol;
double sum_Fit;
Sexuals mySexuals;
int simstart_generation;
int simpart;

istream & Params::InitParams(istream & in)
{
    tasks = 2;    
    meanT.reserve(tasks);
    delta.reserve(tasks);
    alfa.reserve(tasks);
    beta.reserve(tasks);

    in >> N >>
        Col >>
        maxtime; 
    double tmp;
    for (int i=0; i<tasks; i++) 
        {
        in >> tmp ; 
        meanT.push_back(tmp);
        }
    for (int i=0; i<tasks; i++) 
        {    
        in >> tmp ; 
        delta.push_back(tmp);
        }
    for (int i=0; i<tasks; i++)
        {
        in >> tmp; 
        alfa.push_back(tmp);
        }
    for (int i=0; i<tasks; i++) 
        {
        in >> tmp;
        beta.push_back(tmp);
        }
    in >> p >>
        mutp >>
        maxgen >>
        beta_fit >>
        gama_fit >>
        seed >>
        recomb >>
        timecost>>
	mutstep >>
	initStim >>
	p_wait >>
	tau >>
	fit_broadness;
         

return in;
}

//=================================================================================================================
// Function to check if a file exists already
bool FileExists(string strFilename) 
{
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    blnReturn = false;
  }
  
  return(blnReturn);
}




//========================================================================================
// WORK IN PROGRESS
// Function that initializes founders from (previous) data 

void StartFromLast (istream & in, Params & Par, Population & Pop)
{

	double tmp;

	
	in >> simpart 
	   >> simstart_generation; 

	for (unsigned int i = 0; i < Pop.size(); i++)
		{
		for (int task = 0; task<Par.tasks; task++) 
			{
			in >> tmp;
			Pop[i].male.threshold[task] = tmp;
			}
		for (int task=0; task<Par.tasks; task++)
			{
			in >> tmp;
			Pop[i].queen.threshold[task] = tmp;
			}
		}
}




//----------------------------------------------------------------------------------
void ShowParams(Params & Par)
        {
         cout <<"Workers " << Par.N << endl;
         cout << "Colonies " << Par.Col << endl;
         cout << "Timesteps " << Par.maxtime << endl;
         
         for (int task=0; task<Par.tasks; task++)
            cout << "Initial T" <<task <<"\t" << Par.meanT[task] << endl;
         
         for (int task=0; task<Par.tasks; task++)
            cout << "Delta " <<task <<"\t"  << Par.delta[task] << endl;
         
         for (int task=0; task<Par.tasks; task++)
            cout << "effic " << task << "\t" << Par.alfa[task] << endl;
         
         for (int task=0; task<Par.tasks; task++)
            cout << "Decay " << task << "\t" << Par.beta[task] << endl;
         
         cout << "prob quit" << Par.p << endl;
         cout << "Task number " << Par.tasks << endl;
         cout << "mut prob " << Par.mutp << endl;
         cout << "Max gen " <<  Par.maxgen << endl;
         cout << "Exp task 1 " <<  Par.beta_fit << endl;
         cout << "Exp task 2 " <<  Par.gama_fit << endl;
         cout << "seed " << Par.seed << endl;
         cout << "recombination " << Par.recomb << endl;
         cout << "timecost " << Par.timecost << endl;
        }
//-----------------------------------------------------------------------------



void InitFounders(Population &Pop, Params &Par)
	{
#ifdef DEBUG  
        assert(Par.tasks==2);
        cout <<Par.Col << endl;
#endif
        Pop.resize(Par.Col);
        //cout << "Initializing founders!" << endl;
	for (unsigned int i = 0; i < Pop.size(); i++)
		{
		Pop[i].male.threshold.resize(Par.tasks);
		Pop[i].queen.threshold.resize(Par.tasks);
		for(int task=0; task<Par.tasks; task++)
		    {
		    Pop[i].male.threshold[task]= Par.meanT[task];
	            Pop[i].queen.threshold[task]= Par.meanT[task];
		    //Pop[i].male.threshold[task]= Normal(Par.meanT[task],1);
			    //Pop[i].queen.threshold[task]= Normal(Par.meanT[task],1);
		    }
		Pop[i].male.mated =true;
		Pop[i].queen.mated = true;
               // cout << Pop[i].male.threshold[0] << "\t" << Pop[i].male.threshold[1] << endl;
               // cout << Pop[i].queen.threshold[0] << "\t" << Pop[i].queen.threshold[1] << endl;
                //getch();
		}
	}
//----------------------------------------------------------------------------------------------------------------------

void Inherit(Ant &Daughter, Ant &Mom, Ant &Dad, Params &Par)
{
    double rec = Uniform();
	
    if (Par.recomb >= rec)
        {
        int who = RandomNumber(2);
           //each task will be inherited from different parents
        if (who ==0)
                {
                if (Par.mutp > Uniform())
                    Daughter.threshold[0] = Mom.threshold[0] + Normal(0,Par.mutstep);
                else Daughter.threshold[0] = Mom.threshold[0];
                
                if (Par.mutp > Uniform())
                    Daughter.threshold[1] = Dad.threshold[1] + Normal(0,Par.mutstep);
                else Daughter.threshold[1] = Dad.threshold[1];
                }

        else 
                {
                if (Par.mutp > Uniform())
                    Daughter.threshold[0] = Dad.threshold[0] + Normal(0,Par.mutstep);
                else Daughter.threshold[0] = Dad.threshold[0];
                
                if (Par.mutp > Uniform())
                    Daughter.threshold[1] = Mom.threshold[1] + Normal(0,Par.mutstep);
                else Daughter.threshold[1] = Mom.threshold[1];
                }
    
        for (int task = 0; task < Par.tasks; task++)
                {
                if (Daughter.threshold[task]<0) 
                    Daughter.threshold[task]=0.0;
                }
        } 
                //assert(Daughter.threshold[task]>0);
    else 
        {
       //both tasks will be inherited from the same parent 
        int who = RandomNumber(2);
        
        for (int task = 0; task < Par.tasks; task++)
		    {
            assert (task < 2);
                //cout << "For task " << task << endl;
            double anynumber = Uniform();
                //cout << anynumber << endl;
                  //  cout << "inherited from " << who << endl;
            if (who ==0)
                {
                if (Par.mutp > anynumber)
                    Daughter.threshold[task] = Mom.threshold[task] + Normal(0,Par.mutstep);
                else Daughter.threshold[task] = Mom.threshold[task];
                }

            else if (who==1)
                {
                if (Par.mutp > anynumber)
                    Daughter.threshold[task] = Dad.threshold[task] + Normal(0,Par.mutstep);
                else Daughter.threshold[task] = Dad.threshold[task];
                }
                    if (Daughter.threshold[task]<0) Daughter.threshold[task]=0.0;

            
		//mygetch();
                //assert(Daughter.threshold[task]>0);
            } 
        }


} // end of Inherit


//----------------------------------------------------------------------------------------------------------

void InitAnts(Ant & myAnt, Params & Par, Colony & myCol)
  {
  myAnt.threshold.resize(Par.tasks);

  if (Par.maxgen > 1) Inherit(myAnt, myCol.queen, myCol.male, Par);
  else 
    {
    for (int task=0; task<Par.tasks; task++)
        myAnt.threshold[task]= Par.meanT[task];
    }

  myAnt.countacts.resize(Par.tasks);
  myAnt.want_task.resize(Par.tasks);
  for (int task = 0; task < Par.tasks; task++)
    {
    myAnt.countacts[task]=0;
    myAnt.want_task[task]=false;
    //cout << myAnt.threshold[task] << "\t" ;
  //  if (myAnt.threshold[task]<=0)
    //   {
      // cout << myAnt.threshold[task] << "\t" ;
       //getch();
      // }
    //assert(myAnt.threshold[task]>0);
    }
    //other stuff
  //myAnt.act.resize(3);
 // myAnt.act[0] = false;  // doing task 1
 // myAnt.act[1] = false;  // doing task 2
 // myAnt.act[2] = true; // idle
  myAnt.last_act = 7; // initiate it at an impossible value for a task, because 0 is a task
  myAnt.curr_act = 2; // 0 = task 1, 1 = task 2, 2 = idle
  myAnt.switches = 0;
  myAnt.workperiods=0;
  myAnt.F = 10;
  myAnt.F_franjo = 10;
  myAnt.mated = false;
  myAnt.count_time=0;
  }


//------------------------------------------------------------------------------------

void Init(Population & Pop, Params & Par)
  {
#ifdef DEBUG
    cout << Pop.size() << endl;
    cout << Par.N << endl;
      assert(Pop.size()==Par.Col);
#endif
      
  //cout << " Initiating colonies" << endl;
  for (unsigned int i = 0; i< Pop.size(); i++)
    {
    Pop[i].MyAnts.resize(Par.N);
   // Pop[i].HighF.resize(21);
   // Pop[i].LowF.resize(21);
   // Pop[i].CategF.resize(21);
   // Pop[i].HighT.resize(21);
   // Pop[i].LowT.resize(21);
   // Pop[i].CategT1.resize(21);
   // Pop[i].CategT2.resize(21);
    Pop[i].ID = i;
    Pop[i].fitness = 0;
    Pop[i].rel_fit = 0;
    Pop[i].cum_fit = 0;
    Pop[i].diff_fit = 0;
    Pop[i].idle = 0;
    Pop[i].inactive = 0;
    Pop[i].mean_F = 10; // specialization value
    Pop[i].var_F=0;
    Pop[i].mean_F_franjo = 10;
    Pop[i].var_F_franjo = 0;
    Pop[i].mean_switches = 0;
    Pop[i].var_switches = 0;
    Pop[i].mean_workperiods=0;
    Pop[i].var_workperiods=0;
    Pop[i].work_fitness = 1;
    
    //cout << "Colony " << i << "inits switches at " << Pop[i].mean_switches <<endl;
    if(!Pop[i].workfor.empty()) Pop[i].workfor.erase(Pop[i].workfor.begin(),Pop[i].workfor.end());
    assert(Pop[i].workfor.empty());

    if(!Pop[i].last_half_acts.empty()) 
        Pop[i].last_half_acts.erase(Pop[i].last_half_acts.begin(),Pop[i].last_half_acts.end());
   assert(Pop[i].last_half_acts.empty()); 

    if(!Pop[i].numacts.empty()) 
        Pop[i].numacts.erase(Pop[i].numacts.begin(),Pop[i].numacts.end());
   assert(Pop[i].numacts.empty()); 
    
   if(!Pop[i].stim.empty()) 
        Pop[i].stim.erase(Pop[i].stim.begin(),Pop[i].stim.end());
    assert(Pop[i].stim.empty()); 
    
    if (!Pop[i].newstim.empty())
        Pop[i].newstim.erase(Pop[i].newstim.begin(),Pop[i].newstim.end());
    assert(Pop[i].newstim.empty());
   
    if(!Pop[i].mean_work_alloc.empty());
        Pop[i].mean_work_alloc.erase(Pop[i].mean_work_alloc.begin(),Pop[i].mean_work_alloc.end());
   assert( Pop[i].mean_work_alloc.empty());
    
    Pop[i].workfor.reserve(Par.tasks);
    Pop[i].last_half_acts.reserve(Par.tasks);
    Pop[i].numacts.reserve(Par.tasks);
    Pop[i].stim.reserve(Par.tasks);
    Pop[i].newstim.reserve(Par.tasks);
    Pop[i].mean_work_alloc.reserve(Par.tasks);

   //cout << Par.tasks << endl;
    for (int job=0; job<Par.tasks; job++)
        {
        Pop[i].workfor.push_back(0);
        Pop[i].last_half_acts.push_back(0);
        Pop[i].numacts.push_back(0);
        Pop[i].stim.push_back(Par.initStim);
        Pop[i].newstim.push_back(0);
        Pop[i].mean_work_alloc.push_back(0);
        //cout << "Blah" << endl;
        }
    
    //InitFounders(Pop[i].male, Par);
    //InitFounders(Pop[i].queen, Par);
     for (unsigned int j = 0; j<Pop[i].MyAnts.size(); j++)
       InitAnts (Pop[i].MyAnts[j], Par, Pop[i]);

    } // end of for Pop
  } // end of Init()

//-------------------------------------------------------------------------------
void ShowAnts(Colony & anyCol)
{
    cout << "Current values of:" << endl; 

    for (unsigned int ant=0; ant < anyCol.MyAnts.size(); ant++)
	{
	    cout << "ant " << ant << endl;
	    cout << "F " << anyCol.MyAnts[ant].F << endl; // specialization value
	    cout << "switches " << anyCol.MyAnts[ant].switches << endl;
	    cout << "workperiods " << anyCol.MyAnts[ant].workperiods << endl;
	    cout << "count acts " << anyCol.MyAnts[ant].countacts[0] << "\t" << anyCol.MyAnts[ant].countacts[1]  << endl;
	}
}

//--------------------------------------------------------------------------------
void ShowColony(Colony & anyCol)
{
    cout << "Current values of:" << endl; 
    cout << "fitness " << anyCol.fitness << endl;
    cout << "relative fitness " << anyCol.rel_fit << endl;
    cout << "cumulative fitness " << anyCol.cum_fit << endl;
    cout << "diff fit " << anyCol.diff_fit << endl;
    cout << "idle " << anyCol.idle << endl;
    cout << "inactive " << anyCol.inactive << endl;
    cout << "mean_F " << anyCol.mean_F << endl; // specialization value
    cout << "var_F" << anyCol.var_F << endl;
    cout << "mean_franjo" << anyCol.mean_F_franjo << endl;
    cout << "var_franjo " << anyCol.var_F_franjo << endl;
    cout << "mean_switches " << anyCol.mean_switches << endl;
    cout << "var_switches " <<anyCol.var_switches << endl;
    cout << "workperiods " << anyCol.mean_workperiods << endl;
    cout << "var workperiods " <<  anyCol.var_workperiods << endl;
	cout << "workfor 1 " <<  anyCol.workfor[0]/3 << endl;
	cout << "workfor 2" <<  anyCol.workfor[1]/3 << endl;
	ShowAnts(anyCol);
	mygetch();
}

//--------------------------------------------------------------------------------
void UpdateStimPerAnt(Params & Par, Colony & anyCol, Ant & anyAnt, int task)
{

    //cout << "Updating the stimulus per ant" << endl;
#ifdef DEBUG
cout << Par.alfa[task] << endl;
   
cout << Par.N << endl;
cout << anyCol.workfor[task] << endl;
cout <<anyCol.numacts[task]<< endl;
cout << anyCol.stim[task] << endl;
#endif
    //anyCol.stim[task]-=(Par.alfa[task]/Par.N); //update stimulus     
    //if (anyCol.stim[task] - (Par.alfa[task]/Par.N) >= 0)    
//	    { 
		anyCol.workfor[task]+=Par.alfa[task];   //update workdone (fitness) if stimulus is enough
		anyCol.stim[task]-=(Par.alfa[task]/Par.N); //update stimulus     
//	    }
    
    if(anyCol.stim[task]<0)
        anyCol.stim[task]=0;
}
//------------------------------------------------------------------------------


void QuitTask(Colony & anyCol, Ant & anyAnt, int job, Params & Par)
  {
#ifdef DEBUG
cout << "Quitting tasks" << endl;
cout << "chance to quit: " << Par.p << endl;

#endif
  double q = Uniform();


#ifdef SIMULTANEOUS_UPDATE

  if (q <= Par.p)
     {
     //anyAnt.act[job] = false;
     //anyAnt.act[2] = true; // she quits task and becomes idle
     anyAnt.want_task[anyAnt.curr_act]=false;
     anyAnt.count_time=0; // reset time to zero, she may choose the same or another task next
     anyAnt.curr_act = 2;
     //cout << "ant quits" << endl;
     }
#endif  

#ifndef SIMULTANEOUS_UPDATE  
  if (q <= Par.p)
     {
     //anyAnt.act[job] = false;
     //anyAnt.act[2] = true; // she quits task and becomes idle
     anyAnt.want_task[anyAnt.curr_act]=false;
     anyAnt.count_time=0; // reset time to zero, she may choose the same or another task next
     anyAnt.curr_act = 2;
     //cout << "ant quits" << endl;
     }
  else UpdateStimPerAnt(Par, anyCol, anyAnt, job);
#endif
  
  }

//------------------------------------------------------------------------------
double RPfunction (double t, double s) // response probability
  {
  double RP;
  
//  cout << "threshold is: " << t << endl;
  if (t > 0.00 )  RP = s*s / ((s*s)+(t*t));
  else if (s > 0.00000) RP = 1.0; // if threshold is zero, then probability of acting is 1.  
        else RP = 0.0;            // if stimulus is zero, do nothing.
  //cout << "RP is: " << RP << endl;

  return RP;
  }
//-------------------------------------------------------------------------------

void DoTask ( Params Par, Colony & anyCol, Ant & anyAnt,int job)
{
    

#ifdef SIMULTANEOUS_UPDATE 
                //    cout << "Blah" << endl;
                 anyAnt.curr_act = job; 
                 anyAnt.workperiods +=1; 
                 //cout << "Ant is doing " << anyAnt.curr_act << endl; 
#endif
        

#ifndef SIMULTANEOUS_UPDATE 
                 //cout << "Blah" << endl;
                 anyAnt.curr_act = job; 
                 anyAnt.workperiods +=1; 
                 UpdateStimPerAnt(Par, anyCol, anyAnt, job);
                 //cout << "Ant is doing " << anyAnt.curr_act << endl; 
#endif
}
//end DoTask
//---------------------------------------------------------------------------------
void WantTask ( Params Par, Colony & anyCol, Ant & anyAnt)
{
vector<int>counter;
counter.reserve(Par.tasks); 
   //double q = Uniform();

#ifdef SIMULTANEOUS_UPDATE 
for (int job=0; job<Par.tasks ; job++)
	{
	double stim_noise = anyCol.stim[job] + Normal(0,1);
	double t_noise =  anyAnt.threshold[job] + Normal(0,1);
	if(stim_noise < 0) stim_noise = 0;
	if (t_noise <0) t_noise = 0;

        if (stim_noise >= t_noise && stim_noise >0) 
            {
      		counter.push_back(job);  //which tasks are above the threshold
                //cout << stim_noise << " larger than " << t_noise << " --> ANT WANTS " << endl;
             } else 
                    {
                        anyAnt.want_task[job]=false; // if her threshold not high enough, then quit with wanting task
                    }
	}

if (counter.size()>1) // if more than one task is above threshold, do random task among those
	{
		int job = RandomNumber(counter.size());
		anyAnt.want_task[counter[job]]=true;
	}
else if(!counter.empty()) anyAnt.want_task[counter[0]]=true;
#endif
        

#ifndef SIMULTANEOUS_UPDATE 

for (int job=0; job<Par.tasks ; job++)
	{
	double stim_noise = anyCol.stim[job] + Normal(0,1);
	if (stim_noise < 0 ) stim_noise =0;
	double t_noise =  anyAnt.threshold[job] + Normal(0,1);
	if (t_noise < 0) t_noise =0;
        if (stim_noise >= t_noise && stim_noise >0) 
            {
      		counter.push_back(job);  //which tasks are above the threshold
                //cout << "stim : " << anyCol.stim[job] << " threshold : " << anyAnt.threshold[job] << endl; 
		//cout << stim_noise << " larger than " << t_noise << " --> ANT WANTS " << endl;
             } else 
                    {
                	//cout << "stim : " << anyCol.stim[job] << " threshold : " << anyAnt.threshold[job] << endl; 
			//cout << stim_noise << " smaller than " << t_noise << " --> NOPE " << endl;
                        anyAnt.want_task[job]=false; // if her threshold not high enough, then quit with wanting task
                    }
	}

if (counter.size()>1)
	{
		int job = RandomNumber(counter.size());
		anyAnt.want_task[counter[job]]=true;
	}
else if (!counter.empty()) anyAnt.want_task[counter[0]]=true;
#endif

}
//end WantTask
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EvalTaskSwitch (Params & Par, Colony & anyCol, Ant & anyAnt,int myjob )
{
//cout << "Evaluating switches " << endl;
if (myjob==anyAnt.last_act) 
    {
    DoTask(Par, anyCol, anyAnt, myjob);
    //cout << "Ant does her favorite!" << endl;    
    }
else 
    {
          if (Par.p_wait >= Uniform() && anyAnt.count_time < Par.timecost)    
                {
                //cout << "Ant wants to change task!" << endl;    
                anyAnt.curr_act=2; // stays idle for as long as count_time<timecost    
                anyAnt.count_time ++;
                }
            else
                {
                DoTask(Par, anyCol, anyAnt, myjob);
                }
    }
    
}
//end EvalTaskSwitch
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void TaskChoice(Params & Par, Colony & anyCol, Ant & anyAnt)
{ 
   //int job =  RandomNumber(Par.tasks);  // prob to meet one of two tasks is random
   

   //cout << "Choosing tasks" << endl; 
#ifdef DEBUG 
   //cout << "Choosing tasks" << endl; 
    assert(Par.tasks==2);
    //assert(anyCol.stim[job]>=0);
    //assert(anyAnt.threshold[job]>=0);
#endif
     // if she does not want a specific task 
    if(!anyAnt.want_task[0] && !anyAnt.want_task[1])
        {

	//cout << " ant has no preference yet " << endl;
        WantTask(Par, anyCol, anyAnt);
        for (int task=0; task<Par.tasks; task++)
		{
		if(anyAnt.want_task[task])
            		{
			assert(anyAnt.want_task[1-task]==false);
            		if(anyAnt.last_act==7) 
                		DoTask(Par, anyCol, anyAnt, task);
                
            		else  
                		EvalTaskSwitch(Par, anyCol, anyAnt, task); 
			}    
            
        	else 
			{
			//cout << " no task " << task << " for me" << endl;	
            		//anyAnt.curr_act =2; // if she doesn't want the task, just remain idle
        		}
		}
	}

    else 
	{   
	//cout << "Ant has a preference "  ;
        for (int job=0; job<Par.tasks; job++)
		{
			if (anyAnt.want_task[job] ) // if she wants a specific task 
                	{
			//cout << "for task " << job << endl;
			assert(anyAnt.want_task[1-job]==false);    
                	EvalTaskSwitch(Par, anyCol, anyAnt, job); 
			break;
                	}
		}
         }       
  } // end of TaskChoice()
//-------------------------------------------------------------------------------------------------

/*

void TaskChoice(Params & Par, Colony & anyCol, Ant & anyAnt)
{ 
   int job =  RandomNumber(Par.tasks);  // prob to meet one of two tasks is random
   double q = Uniform();
   

#ifdef DEBUG 
   cout << "Choosing tasks" << endl; 
    assert(Par.tasks==2);
    assert(anyCol.stim[job]>=0);
    assert(anyAnt.threshold[job]>=0);
#endif

#ifdef SIMULTANEOUS_UPDATE 
    if (q <= RPfunction(anyAnt.threshold[job],anyCol.stim[job])) 
        {
             //cout << "Blah" << endl;
		     anyAnt.curr_act = job; 
             anyAnt.workperiods +=1; 
             //cout << "Ant is doing " << anyAnt.curr_act << endl; 
         } else anyAnt.curr_act=2;
#endif

#ifndef SIMULTANEOUS_UPDATE 
    if (q <= RPfunction(anyAnt.threshold[job],anyCol.stim[job])) 
        {
             //cout << "Blah" << endl;
		     anyAnt.curr_act = job; 
             anyAnt.workperiods +=1; 
             UpdateStimPerAnt(Par, anyCol, anyAnt, job);
             //cout << "Ant is doing " << anyAnt.curr_act << endl; 
         } else anyAnt.curr_act=2;
#endif
    

  } // end of TaskChoice()

*/

//-------------------------------------------------------------------------------------------------

void UpdateAnts(Population & Pop, Params & Par)
  {
  //cout << "updating ants" << endl;
#ifdef DEBUG
cout << Pop.size() << endl;
#endif
   for (unsigned int i = 0; i<Pop.size(); i++)
     {
     Pop[i].inactive = 0;
     for (int task =0; task<Par.tasks; task++)
         Pop[i].workfor[task]=0; // reset work for tasks
     
     random_shuffle(Pop[i].MyAnts.begin(), Pop[i].MyAnts.end());
     
     for (unsigned int j = 0; j<Pop[i].MyAnts.size(); j++)  //actives may quit, idle may work
        {
        //cout << j << " of "<< Pop[i].MyAnts.size() << " workers " << endl;
        if(Pop[i].MyAnts[j].curr_act!=2)//if ant active 
            {
	    //cout << "Ant busy " << endl;
            Pop[i].MyAnts[j].last_act = Pop[i].MyAnts[j].curr_act; //record last act
            QuitTask(Pop[i], Pop[i].MyAnts[j], Pop[i].MyAnts[j].curr_act, Par); // wanna quit?
            }
        if(Pop[i].MyAnts[j].curr_act==2) TaskChoice(Par, Pop[i], Pop[i].MyAnts[j]); //if inactive, choose a task 
        
        //update number of switches after choosing tasks
        if(Pop[i].MyAnts[j].curr_act!=2)//if ant active 
            {
            
            Pop[i].numacts[Pop[i].MyAnts[j].curr_act] += 1;
            Pop[i].MyAnts[j].countacts[Pop[i].MyAnts[j].curr_act] += 1;
            
     //       if (Pop[i].stim[Pop[i].MyAnts[j].curr_act] - Par.alfa[Pop[i].MyAnts[j].curr_act]/Pop[i].MyAnts.size() >= 0)
            //if (Pop[i].stim[Pop[i].MyAnts[j].curr_act] > 0) // only count for work when stimulus is > 0
       //         Pop[i].workfor[Pop[i].MyAnts[j].curr_act] += Par.alfa[Pop[i].MyAnts[j].curr_act]; // update the work done for that task 
            
            if (Pop[i].MyAnts[j].last_act != 7 && Pop[i].MyAnts[j].last_act != Pop[i].MyAnts[j].curr_act)
                Pop[i].MyAnts[j].switches+=1;
            }
        else Pop[i].inactive ++;
     	//mygetch();
        } 
      
     
     Pop[i].inactive /= Pop[i].MyAnts.size(); // proportion inactive workers 
     }
  }  // end of UpdateAnts()
//------------------------------------------------------------------------------

void UpdateStim(Population & Pop, Params & Par)   // stimulus changes const increase
  {
  //cout << "updating stimulus" << endl;
  for (unsigned int i = 0; i<Pop.size(); i++)
     {

#ifdef SIMULTANEOUS_UPDATE
    for (int task=0; task<Par.tasks; task++)
        {
            
	//assert(Par.delta[task]==1);
        Pop[i].newstim[task] = Pop[i].stim[task] + Par.delta[task] - (Par.beta[task]*Pop[i].stim[task]) - (Pop[i].workfor[task]/Par.N) ; 
        Pop[i].stim[task] = Pop[i].newstim[task];

        if (Pop[i].stim[task] < 0) Pop[i].stim[task] =0;
        }
#endif

#ifndef SIMULTANEOUS_UPDATE
    for (int task=0; task<Par.tasks; task++)
        {
            
    	//assert(Par.delta[task]==1);
        Pop[i].newstim[task] = Pop[i].stim[task] + Par.delta[task] - (Par.beta[task]*Pop[i].stim[task]) ; 
        Pop[i].stim[task] = Pop[i].newstim[task];

        if (Pop[i].stim[task] < 0) Pop[i].stim[task] =0;
        }
#endif

    }
  } // end UpdateStim()
//------------------------------------------------------------------------------

void Calc_F(Population & Pop, Params & Par)
  {
	double C;
   
  for (unsigned int i = 0; i<Pop.size(); i++)
     {
     Pop[i].mean_F=-10;
     Pop[i].mean_F_franjo=-10; 
     Pop[i].mean_switches=0; 
     Pop[i].mean_workperiods=0; 
    // cout << Pop[i].mean_F << endl;
    // cout << Pop[i].mean_F_franjo << endl; 
    // cout << Pop[i].mean_switches << endl; 
    // cout << Pop[i].mean_workperiods << endl; 
     
     double sumF=0;
     double sumsquares_F=0;

     double sumF_franjo=0;
     double sumsquares_F_franjo=0;

     double sumsquares_switches=0;
     double sumsquares_workperiods=0;
     
     double activ=0;

     //cout << "colony " << i << " Switches start at " << "\t" << Pop[i].mean_switches << endl;
     for (unsigned int j = 0; j<Pop[i].MyAnts.size(); j++)
        {
	assert(Pop[i].MyAnts[j].workperiods <= 100);
        C = 0;

        //cout << "mean switches "<< "\t" << Pop[i].mean_switches << endl;
        if (Pop[i].MyAnts[j].workperiods > 0)
          {
          Pop[i].mean_switches += Pop[i].MyAnts[j].switches;
       //   cout << "Switches add up " << j << "\t" << Pop[i].mean_switches << endl;
         // cout << "Switches ant " << j << "\t" << Pop[i].MyAnts[j].switches << endl;
          
          Pop[i].mean_workperiods += Pop[i].MyAnts[j].workperiods;
          
          C = double(Pop[i].MyAnts[j].switches) / (Pop[i].MyAnts[j].workperiods - 1);
          
          Pop[i].MyAnts[j].F = 1 - 2*C;
          Pop[i].MyAnts[j].F_franjo = 1-C;
          
          sumF += Pop[i].MyAnts[j].F;
          activ +=1;
          sumF_franjo +=Pop[i].MyAnts[j].F_franjo;
          }
        }

      //cout << "in colony " << i << ", " << activ << " ants did something" << endl;
      //cout << "sum of switches " << Pop[i].mean_switches << endl;
     
        Pop[i].mean_switches=Pop[i].mean_switches/activ;

        //cout << "mean of switches " << Pop[i].mean_switches << endl;

        for (unsigned int j = 0; j<Pop[i].MyAnts.size(); j++)
            {
                if (Pop[i].MyAnts[j].workperiods > 0)
                    {
                    sumsquares_switches += ( Pop[i].MyAnts[j].switches - Pop[i].mean_switches)*( Pop[i].MyAnts[j].switches - Pop[i].mean_switches);

                    }
            }
        Pop[i].var_switches = sumsquares_switches / activ;

        

        Pop[i].mean_workperiods=Pop[i].mean_workperiods/Pop[i].MyAnts.size();

        //cout << "mean of switches " << Pop[i].mean_switches << endl;

        for (unsigned int j = 0; j<Pop[i].MyAnts.size(); j++)
            {

                    sumsquares_workperiods += (Pop[i].MyAnts[j].workperiods - Pop[i].mean_workperiods)*(Pop[i].MyAnts[j].workperiods - Pop[i].mean_workperiods);
            }

        Pop[i].var_workperiods = sumsquares_workperiods / Pop[i].MyAnts.size();

        

        Pop[i].mean_F = sumF/activ;
        Pop[i].mean_F_franjo = sumF_franjo/activ;
     

        for (unsigned int j = 0; j<Pop[i].MyAnts.size(); j++)
            {
                if (Pop[i].MyAnts[j].countacts[0]+Pop[i].MyAnts[j].countacts[1] > 0)
                    {
                    sumsquares_F += (Pop[i].MyAnts[j].F - Pop[i].mean_F)* (Pop[i].MyAnts[j].F - Pop[i].mean_F);

                    sumsquares_F_franjo += (Pop[i].MyAnts[j].F_franjo - Pop[i].mean_F_franjo)* (Pop[i].MyAnts[j].F_franjo - Pop[i].mean_F_franjo);

                    }
            } // end of for MyAnts
        Pop[i].var_F = sumsquares_F / activ;
        Pop[i].var_F_franjo = sumsquares_F_franjo / activ;
        

     } // end for Colonies

  } // end of Calc_F()
//==============================================================================================================================================

void CalcFitness(Population & Pop, Params & Par)
  {
  sum_Fit = 0;
  double total = 0;

  for (unsigned int i = 0; i<Pop.size(); i++)
     {
     Pop[i].idle = 0;
   
     total = Pop[i].last_half_acts[0] + Pop[i].last_half_acts[1];
     //cout << "Colony " << i << " total acts : " << total << " work_fitness : " << Pop[i].work_fitness << endl;
     	     
     if (total == 0)
	     Pop[i].fitness =0;
     else
	{ 
#ifdef ACC_FITNESS
		Pop[i].fitness= pow (Pop[i].work_fitness, 1.0/(Par.maxtime - Par.tau));
//		cout << "Fitness " << Pop[i].fitness << endl;
#endif

#ifndef ACC_FITNESS
             double p1 = Pop[i].last_half_acts[0] / total;
	    Pop[i].fitness = log(total) * exp( -Par.fit_broadness * pow((p1 - Par.beta_fit), 2));
#endif
      	}

     for (unsigned int j =0; j< Pop[i].MyAnts.size(); j++)
       {
       if (Pop[i].MyAnts[j].countacts [0] == 0 && Pop[i].MyAnts[j].countacts [1] == 0)
         Pop[i].idle +=1;
       }

     }

     int min_fit = Pop[0].ID;
     for (unsigned int col=1; col< Pop.size(); col++)
        {
        if (Pop[col].fitness < Pop[min_fit].fitness)
                min_fit = col;
        }

     for (unsigned int i = 0; i<Pop.size(); i++)
	{
        //cout << i <<"\t"<<  Pop[i].fitness << endl;
        Pop[i].diff_fit = Pop[i].fitness - Pop[min_fit].fitness;
        sum_Fit += Pop[i].diff_fit;
        }
        //cout << "min fit " << min_fit << "\t"<< Pop[min_fit].fitness << endl;
        //getch();

     for (unsigned int i = 0; i<Pop.size(); i++)
        {
	Pop[i].rel_fit = Pop[i].diff_fit / sum_Fit;
	if (i==0)
		Pop[i].cum_fit = Pop[i].rel_fit;
	else
		Pop[i].cum_fit = Pop[i-1].cum_fit + Pop[i].rel_fit;

    //    cout << "Colony " << i << " cum_fit = " << Pop[i].cum_fit << endl;
	}
 // assert(Pop.back().cum_fit==1);

  } // end of CalcFitness()
//-------------------------------------------------------------------------------

int drawParent(int nCol, Population & Pop)
{
const double draw = Uniform(); // random number
int cmin=-1, cmax=nCol-1;

while (cmax - cmin != 1)
	{
	int cmid = (cmax+cmin)/2 ;
	if (draw < Pop[cmid].cum_fit) cmax = cmid;
	else cmin = cmid;
	}
return cmax;
}// end of drawParent()
//----------------------------------------------------------------------------------------------------

void MakeSexuals(Population & Pop, Params & Par)
{
 mySexuals.resize(2 * Par.Col); // number of sexuals needed
 parentCol.resize(mySexuals.size());

 for (unsigned int ind = 0; ind < mySexuals.size(); ind ++)
	{
        //initialize sexuals
        mySexuals[ind].threshold.resize(Par.tasks);
        //mySexuals[ind].act.resize(NULL);
      //  mySexuals[ind].countacts.resize(NULL);
      //  mySexuals[ind].last_act = NULL;
      //  mySexuals[ind].curr_act = NULL;
     //   mySexuals[ind].switches = NULL;
     //   mySexuals[ind].F =NULL;
        mySexuals[ind].mated = false;
        // draw a parent colony for each sexual
	parentCol[ind] = drawParent(Pop.size(), Pop);
      //	cout << "Sexual " << ind << " is by col " << parentCol[ind] << endl;

	Inherit(mySexuals[ind], Pop[parentCol[ind]].queen, Pop[parentCol[ind]].male, Par);

	}
}
//-------------------------------------------------------------------------------------------
void MakeColonies(Population &Pop, Params &Par, int generation)
{
int mother, father;

static ofstream lastgen;

for (unsigned int col = 0; col < Pop.size(); col++)
	{
	do
	{
	mother = RandomNumber(mySexuals.size());
	father = RandomNumber(mySexuals.size()) ;
	}	while
		(mother == father || mySexuals[mother].mated==true || mySexuals[father].mated==true);
	mySexuals[mother].mated = true;
	mySexuals[father].mated = true;

	Pop[col].queen = mySexuals[mother]; Pop[col].male = mySexuals[father]; // copy new males and females

	} // end for Colonies
// if last generation, write out the founders to a file 
if (generation - simstart_generation == Par.maxgen -1) 
	{
		lastgen.open("lastgen.txt");
		lastgen << simpart + 1 << endl;
		lastgen << generation + 1 << endl;
		for (unsigned int col = 0; col < Pop.size(); col++)
			{
			for (int task = 0; task < Par.tasks; task++)
		 		{
					lastgen << Pop[col].male.threshold[task] << endl;	
				}
			for (int task = 0; task < Par.tasks; task++)
		 		{
					lastgen << Pop[col].queen.threshold[task] << endl;	
				}
			}


	}

} // end MakeColonies()
//-----------------------------------------------------------------------------------------------------

void NameDataFiles(string & data1, string &data2, string &data3, string &data4, string &dataant)
	{
		stringstream tmp;
		tmp << "data_work_alloc_" << simpart << ".txt";
		data1 = tmp.str();

		stringstream tmp2;
		tmp2 << "threshold_distribution_" << simpart << ".txt";
		data2 = tmp2.str();

		stringstream tmp3;
		tmp3 << "f_dist_" << simpart << ".txt";
		data3 = tmp3.str();
	
		stringstream tmp4;
		tmp4 << "branch.txt";
		data4 = tmp4.str();
	
		stringstream tmp5;
		tmp5 << "ant_beh_" << simpart << ".txt";
		dataant = tmp5.str();
	}
//=====================================================================================================

void START_SIM (Params & Par, Population & Pop) 
{
	if (FileExists("lastgen.txt"))
		{
		ifstream inp("lastgen.txt");
		StartFromLast(inp, Par, Pop);
		}
	else 
		{
		simpart = 1;
		simstart_generation = 0; 
		}
}
//=====================================================================================================

bool Check_Spec(Population & Pop)
{
	bool mybool=false;
	int count_cols=0; // count colonies with less than 0.75 average specialization 
	for (unsigned int col=0; col<Pop.size(); col++)
		{
	        	if(Pop[col].mean_F<0.75) 
				count_cols +=1;	

		}

        if (double(count_cols)/Pop.size() < 0.4) mybool=true; // if less than 40% of colonies have low specialization
	
	return mybool; 
} // end of Check_Spec

//-------------------------------------------------------------------------------------------------------

void Update_Col_Data(int step, Population & Pop, Params & Par, double & eq_steps)
{
	if (step >= Par.tau) 
		{
			#ifdef ACC_FITNESS // multiply each time step the acts for tasks, and sum it up over the last half of simulation
			for (unsigned int col = 0; col < Pop.size(); col++)
				{
					//cout << "Colony " << col << "workfor multiplied : " << (MyColonies[col].workfor[0]/myPars.alfa[0]) * (MyColonies[col].workfor[1]/myPars.alfa[1])<< endl;
					for (int task=0; task<Par.tasks; task++) 
						Pop[col].last_half_acts[task] += Pop[col].workfor[task]/Par.alfa[task];
					
			//		if (Pop[col].workfor[0]+ Pop[col].workfor[1] > 0)
			//			{
					
					double total = ((Pop[col].workfor[0]+ Pop[col].workfor[1]) / Par.alfa[0]) + 1;
					double p1;
					if(total==1)
						 p1 =1.0; 
					else 
						p1 = (Pop[col].workfor[0]/(Pop[col].workfor[0]+ Pop[col].workfor[1] )) + 1.0;

			//		cout << "p1: " << p1 <<  "   total: " << total << endl;
					Pop[col].work_fitness *= total * exp( -pow((p1 - (Par.beta_fit + 1.0)), 2) / (2 * pow(Par.fit_broadness,2)));
					//mygetch();
		//			}

			//		cout << Pop[col].work_fitness << endl;
					assert(Pop[col].work_fitness >=0);
				}	
			#endif

			#ifndef ACC_FITNESS
			for (unsigned int col = 0; col < Pop.size(); col++)
				{
				    for (int task=0; task<Par.tasks; task++) 
					Pop[col].last_half_acts[task] += Pop[col].workfor[task]/Par.alfa[task];
				}	

			#endif
		}
		
	    
	    if(step >= Par.tau) //calculate work allocation for the timesteps where fitness is calculated
		{
			eq_steps +=1;
         		 for (unsigned int col = 0; col < Pop.size(); col++)
				{
			        for (int task=0; task<Par.tasks; task++)
					Pop[col].mean_work_alloc[task]+=Pop[col].workfor[task]/Par.alfa[task];
				}

		    }
	/*	
	    for (unsigned int col=0; col < Pop.size(); col++)
	        ShowColony(Pop[col]);
	    mygetch();
	*/
}

//==========================================================================================================

void Write_Col_Data(ofstream & mydata, Params & Par, Population & Pop, int gen, int colony)
{

	mydata << gen << "\t" << colony << "\t";
	for (int task=0; task<Par.tasks; task++)
		    mydata << Pop[colony].numacts[task] << "\t";
	for (int task=0; task<Par.tasks; task++)
		    mydata << Pop[colony].mean_work_alloc[task] << "\t"; 				
	mydata << Pop[colony].idle 
	    << "\t" << Pop[colony].inactive 
            << "\t" << Pop[colony].fitness 
	    << "\t" << Pop[colony].stim[0] 
	    << "\t" << Pop[colony].stim[1] 
            << "\t" << Pop[colony].mean_switches 
            << "\t" << Pop[colony].mean_workperiods   
            << "\t" << Pop[colony].last_half_acts[0]
            << "\t" << Pop[colony].last_half_acts[1]
	    << endl;  
}
//------------------------------------------------------------------------------------------------------

void Write_Thresholds_Spec(ofstream & data_thresh,  ofstream & data_f, Params & Par, Population & Pop, int gen, int colony)
{

	 // output only thresholds of foundresses and mean specialization 
	data_thresh << gen << ";";
	for (int task=0; task<Par.tasks; task++)
		{
			if (task < Par.tasks-1)
			    data_thresh << Pop[colony].male.threshold[task] << ";"; 
			else 
			    data_thresh << Pop[colony].male.threshold[task] << endl; 
		}
	data_thresh << gen <<";";
	for (int task=0; task<Par.tasks; task++)
		{
			if (task < Par.tasks-1)
			    data_thresh << Pop[colony].queen.threshold[task] << ";"; 
			else 
			    data_thresh << Pop[colony].queen.threshold[task] << endl; 
		}
	assert(Pop[colony].numacts[0] > 0);
	assert(Pop[colony].numacts[1] > 0);
	double p1 = (double)Pop[colony].numacts[0] / (Pop[colony].numacts[0] + Pop[colony].numacts[1]);
	//cout << "p1 : " << p1 << endl;
	
	double p2 = (double)Pop[colony].numacts[1] / (Pop[colony].numacts[0] + Pop[colony].numacts[1]);
	//cout << "p2: " << p2 << endl;
	
	double denomin = p1*p1 + p2*p2;
	//cout << "denominator :" << denomin << endl;

	data_f << gen <<";" 
		<< Pop[colony].mean_F <<";"
		<< Pop[colony].mean_F_franjo/denomin << ";" 
	    	<< Pop[colony].mean_switches << ";" 
		<< Pop[colony].mean_workperiods << ";"
	    	<< Pop[colony].var_F << ";" 
		<< Pop[colony].var_F_franjo << ";"
	    	<< Pop[colony].var_switches <<";" 
		<< Pop[colony].var_workperiods << endl;
} 

//====================================================================================================
void Write_Branching(ofstream & afile, Params & Par, int yesno)
{
	afile << Par.mutp << ";" 
		<< Par.mutstep << ";" 
		<< Par.timecost << ";" 
		<< yesno << endl;	
}	


//=====================================================================================================

void StopIfSpec(Population & Pop, int generation, Params & Par, ofstream & file1, ofstream &file2, ofstream & file3, ofstream &file4, int eq_steps, string filename)
	{ 
	if (Check_Spec(Pop))
		{
		file4.open(filename.c_str());
		int branch = 1;
		for (unsigned int col = 0; col < Pop.size(); col++)
			{
			for (int task=0; task<Par.tasks; task++)
				Pop[col].mean_work_alloc[task]/=eq_steps;
		    
			Write_Col_Data(file1, Par, Pop, generation, col);
		  
			 // output only thresholds of foundresses and mean specialization 
			    
			Write_Thresholds_Spec(file2, file3, Par, Pop, generation, col);
			
			}
		
		Write_Branching(file4, Par, branch);	
		exit (1);
		}

	 else if (generation == simstart_generation + Par.maxgen-1)
		{
		file4.open(filename.c_str());
		
		int branch = 0;	
		for (unsigned int col = 0; col < Pop.size(); col++)
			{
			for (int task=0; task<Par.tasks; task++)
				Pop[col].mean_work_alloc[task]/=eq_steps;
			    
			Write_Col_Data(file1, Par, Pop, generation, col);
			  
				 // output only thresholds of foundresses and mean specialization 
				    
			Write_Thresholds_Spec(file2, file3, Par, Pop, generation, col);
				
			}				
		Write_Branching(file4, Par, branch);	
		}

	} // end StopIfSpec

//==============================================================================================================================================

int main(int argc, char* argv[])
{
	Params myPars;

	ifstream inp("params.txt");
	myPars.InitParams(inp);
	//ShowParams(myPars);
	//mygetch();
	SetSeed(myPars.seed);
	
	Population MyColonies;
	InitFounders(MyColonies, myPars);
	
	
	// code that checks which run this is
	// is lastgen.txt present y/n

        START_SIM(myPars, MyColonies);

	string datafile1, datafile2, datafile3, datafile4, dataants;

	NameDataFiles(datafile1, datafile2, datafile3, datafile4, dataants);

	    
	static ofstream out; 
	static ofstream out2;
	static ofstream out5;
	static ofstream out_f;
        static ofstream branching;
        static ofstream header1;
	static ofstream out_ants;
	
	if(myPars.Col>1) 
	    {
	    out.open(datafile1.c_str());
	    header1.open("header.txt");
	    header1 << "Gen" << "\t" 
		<< "Col"  << "\t" 
		<< "NumActs1" << "\t" 
		<< "NumActs2" << "\t" 
		<< "WorkAlloc1" << "\t" 
		<< "WorkAlloc2" <<"\t" 
		<< "Idle"<< "\t" 
		<< "Inactive" << "\t"
		<<"Fitness" << "\t" 
		<< "End_stim1" << "\t" 
		<< "End_stim2" << "\t"
		<< "mean_switches" << "\t"
		<< "mean_workperiods" << "\t" 
		<< "last_half_acts_1" << "\t"
		<< "last_half_acts_2" << endl; 
	    }
	    
	    
	    out5.open("data_1gen.txt");

	    out5 << "Time" << ";" 
		<< "Col" << ";" 
		<< "Stim1" << ";" 
		<< "Stim2" << ";" 
		<< "Workers1" << ";"
		<< "Workers2" << ";" 
		<< "Fitness" << ";" 
		<< "Mean_F"<< ";" 
		<< "Mean_F_franjo" << endl;
	     

	out2.open(datafile2.c_str());
	out_f.open(datafile3.c_str());    
	    
	out_ants.open(dataants.c_str()); 

	for (int g= simstart_generation; g < simstart_generation + myPars.maxgen; g++)
		{
//		cout << "Generation " << g << endl;

		Init(MyColonies, myPars);
		//cout<< "Colonies initialized" << endl;
		//mygetch();
		//
		double equil_steps=0;

	#ifdef DEBUG
		cout << MyColonies.size() <<endl;
		cout <<myPars.maxtime << endl;

	#endif
		for (int k = 0; k < myPars.maxtime; k++)
		    {
//		    cout << "Time step" << k << "\t";

		    UpdateAnts(MyColonies, myPars);

		    UpdateStim(MyColonies, myPars);

		    Calc_F(MyColonies, myPars); 
		    Update_Col_Data(k, MyColonies, myPars, equil_steps);	
		    
		    if (k == myPars.maxtime-1)
			 {
			 CalcFitness(MyColonies, myPars);

			#ifdef STOPCODE
			 StopIfSpec(MyColonies, g, myPars, out, out2, out_f, branching, equil_steps, datafile4);
		    	#endif 

		    #ifndef STOPCODE 
			 if ((g<=100 || g%100==0) && myPars.Col>1)
			    {
			    //cout <<  "Generation " << g << endl;
			    for (unsigned int col = 0; col < MyColonies.size(); col++)
				{

					for (int task=0; task<myPars.tasks; task++)
						MyColonies[col].mean_work_alloc[task]/=equil_steps;
					   
					Write_Col_Data(out, myPars, MyColonies, g, col);
					  
						 // output only thresholds of foundresses and mean specialization 
						    
					Write_Thresholds_Spec(out2, out_f, myPars, MyColonies, g, col);
					 
				} // end of for colonies
			    } // end if generations are right
		   #endif

			 } // end if k=maxtime


		  if(g== simstart_generation+myPars.maxgen-1) 
		    {
		    
		    for (unsigned int col = 0; col < MyColonies.size(); col++)
			    	{
				double p1 = (double) MyColonies[col].numacts[0] / (MyColonies[col].numacts[0] + MyColonies[col].numacts[1]);
				//cout << "p1 : " << p1 << endl;
				
				double p2 = (double)MyColonies[col].numacts[1] / (MyColonies[col].numacts[0] + MyColonies[col].numacts[1]);
				//cout << "p2: " << p2 << endl;
				
				double denomin = p1*p1 + p2*p2;
	//cout << "denominator :" << denomin << endl;
				out5 << k << ";" << col << ";"; 
				for (int task=0; task<myPars.tasks; task++)
				     out5 <<MyColonies[col].stim[task] << ";"; 
				for (int task=0; task<myPars.tasks; task++)
				     out5 << MyColonies[col].workfor[task]/myPars.alfa[task] << ";";
				out5 << MyColonies[col].fitness << ";" << 
						MyColonies[col].mean_F <<";"<< MyColonies[col].mean_F_franjo/denomin <<endl; 

				if(k == myPars.maxtime -1)
					{	
					for (unsigned int ant=0; ant < MyColonies[col].MyAnts.size(); ant++)
						{
						out_ants << col << ";" << ant << ";" << MyColonies[col].MyAnts[ant].countacts[0] << ";" 
								 << MyColonies[col].MyAnts[ant].countacts[1] << ";"
								<< MyColonies[col].MyAnts[ant].switches << ";"
								<< MyColonies[col].MyAnts[ant].workperiods << endl;  
						
					
						}
					}
				}
			}   
// one colony only


			 if (myPars.Col==1)
			    {
			    for (unsigned int col = 0; col < MyColonies.size(); col++)
				    {
					out5 << k << ";" << col << ";"; 
					for (int task=0; task<myPars.tasks; task++)
					     out5 <<MyColonies[col].stim[task] << ";"; 
					for (int task=0; task<myPars.tasks; task++)
					     out5 << MyColonies[col].workfor[task]/myPars.alfa[task] << ";";
					out5 << MyColonies[col].fitness << ";" << 
							MyColonies[col].mean_F <<";"<< MyColonies[col].mean_F_franjo <<endl;	     
					
					for (unsigned int ind = 0; ind < MyColonies[col].MyAnts.size(); ind++)
						{
							out2 << g << ";";
						    for (int task=0; task<myPars.tasks; task++)
								out2 << MyColonies[col].MyAnts[ind].threshold[task] << ";"; 
					    out2 << MyColonies[col].MyAnts[ind].F << ";" << MyColonies[col].MyAnts[ind].F_franjo << endl;
						}
				    }
			    }

		   /*  // average colonies
		    double mean_work1, mean_work2;
		    double mean_stim1, mean_stim2;
		    double sum_work1 = 0; double sum_work2 = 0;
		    double sum_stim1 = 0; double sum_stim2=0;
		    for (unsigned int i = 0; i<MyColonies.size(); i++)
			  {
			  sum_work1 += MyColonies[i].workfor1/myPars.alfa1;
			  sum_work2 += MyColonies[i].workfor2/myPars.alfa2;
			  sum_stim1 += MyColonies[i].stim1;
			  sum_stim2 += MyColonies[i].stim2;
			  }
		    mean_work1 = sum_work1/MyColonies.size();
		    mean_work2 = sum_work2/MyColonies.size();
		    mean_stim1 = sum_stim1/MyColonies.size();
		    mean_stim2 = sum_stim2/MyColonies.size();
		   */ 
		   /*
		    out << g << "\t" << k << "\t"  << mean_work1 << "\t"
			  << mean_work2 << "\t" << mean_stim1 << "\t"
			  << mean_stim2  << endl;
		    */
		    }

		MakeSexuals(MyColonies, myPars);

		MakeColonies(MyColonies, myPars, g);

		} // end for generations

	//cout << "done!" << endl;

}
//---------------------------------------------------------------------------



