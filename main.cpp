//Shameer Khan
//CS-323
//project 7.

#include <iostream>
#include <fstream>
#include <stdlib.h>

static int print=0;//kept this one for printing scheduling table (this basically gives me the upper limit of all loops)
using namespace std;

class graphNode{

friend class scheduling;
friend class hashNode;
friend class graphList;

private:
    int jobId;
    graphNode *next;
    int jobTime;
    int jobChild;

public:
    graphNode(int x){
        jobId=x;
        next=NULL;
    }

    graphNode(int x, int y, int z){
        jobId=x;
        jobTime=y;
        jobChild=z;
        next=NULL;
    }

};

class graphList{

friend class scheduling;

private:
    graphNode *head;
    int size;

public:

    graphList(){
        head= new graphNode(-999,-999,-999);
        size=0;
    }

    graphList(int x){
        head = new graphNode(x,-999,-999);
        size=0;
    }

       void insert(graphNode *n){
        graphNode *current = head;
        //graphNode *spot=NULL;
        if(head==NULL){
            head=n;
            size++;
        }
        else{
            while(current->next!=NULL && (current->next->jobTime<=n->jobTime) ){
                if(current->next->jobTime==n->jobTime){
                           if(current->next->jobChild<n->jobChild){
                            current=current->next;
                            break;
                        }
                        else
                            break;
                }
                current=current->next;
            }
        }
        n->next=current->next;
        current->next=n;
        size++;
    }

    bool isEmpty(){
    if(size==0)return true;
    else
        return false;
    }

    void print(){
    graphNode *current=head->next;
    while(current!=NULL){
        cout<<current->jobId<<" ";
        current=current->next;
        }
    }

    graphNode* remove(){
    graphNode *current=head->next;
    head->next=head->next->next;
    size--;
    return current;
    }
};


class hashNode {

friend class scheduling;

private:
    int jobTime;
    int fatherCount;
    int childCount;
    graphNode *stackTop;
    hashNode *graphHashTable;
    graphNode *OPEN;

public:
    hashNode(){
    };

    hashNode(int x){
        graphHashTable = new hashNode[x+1];
        jobTime=0;
        fatherCount=0;
        childCount=0;
        print=x;
        stackTop=NULL;
        OPEN=NULL;
    }
    void push(graphNode *g){
    if(stackTop==NULL){
        stackTop=g;
        }
    else{
        g->next =stackTop;
        stackTop=g;
        }
    }

    void printTable(){
    cout<<"***HashTable***\n";
    for(int i=0;i<print+1;i++){
        graphNode *current= graphHashTable[i].stackTop;
        cout<<i<<" ";
        while(current!=NULL){
            cout<<current->jobId<<" ";
            current=current->next;
        }
        cout<<endl;
    }
    }

   /* void showME_F_C(){
    cout<<"***Father and Child count***"<<endl;
    for(int i=0;i<16;i++){
        cout<<i<<" "<<graphHashTable[i].fatherCount<<" "<<graphHashTable[i].childCount<<endl;
        }
    }*/

    ~hashNode(){
    delete []graphHashTable;
    }
};

class scheduling{
friend class hashNode;
private:
    int numJobs;
    int numProcessors;
    int totalJobtimes;
    int time;
    int **processorSchduler;
    fstream infile1, infile2;
    ofstream outfile;
    graphList OPEN;
    int slot;
    bool *mark;

public:

    scheduling(char *file, char *file2, char* file3, char *file4){
        infile1.open(file);
        infile2.open(file2);
        outfile.open(file4);
        infile1>>numJobs;
        numProcessors= atoi(file3);
        hashNode *table= new hashNode(numJobs);
        if(numProcessors>numJobs){
            numProcessors=numJobs;
        }
        mark = new bool[numJobs+1];

        for(int i=0;i<numJobs+1;i++){
            if(i==0)
                mark[i]=true;
            else{
                mark[i]=false;
            }
        }
        time=1;
        slot=time;
        totalJobtimes=getTotalTime(table);

        createTable(table);
    }//end of constructors

   int getTotalTime(hashNode *ta){
    int x=0;
    int i=1,t=0,job=0;
    infile2>>x;
    while(infile2>>x){
        if(i%2==0){
            ta->graphHashTable[job].jobTime=x;
           t+=x;
        }
        else{
            job=x;
        }
    i++;
    }
    return t;
   }//end of getTotalTime

   void createTable(hashNode *table){

        while(!infile1.eof()){
            int job,child;
            infile1>>job;
            infile1>>child;
            graphNode *newNode = new graphNode(child);
            table->graphHashTable[job].push(newNode);
            table->graphHashTable[job].childCount++;
            table->graphHashTable[child].fatherCount++;
        }
//        table->printTable();
  //      table->showME_F_C();

        processorSchduler = new int *[numProcessors+1];
        for(int i=0;i<numProcessors+1;i++){
            processorSchduler[i]= new int[totalJobtimes+1];
        }

        for(int i=0; i<numProcessors+1;i++){
            for(int j=0; j<totalJobtimes+1;j++){
                processorSchduler[i][j]=0;
               // cout<<processorSchduler[i][j]<<" ";
            }
           // cout<<endl;
        }
        while(!isMarked()){
        int job=1;
        while(job<=numJobs){
            if(table->graphHashTable[job].fatherCount==0 && mark[job]==false){
                int orfen=job;
                mark[orfen]=true;
                graphNode *newNode = new graphNode(orfen, table->graphHashTable[job].jobTime,table->graphHashTable[job].childCount);
               OPEN.insert(newNode);
              // OPEN.append(newNode);
            }//end of if
        job++;
        }//end of  while(job<=numJobs)
        int processor=1;
        int jobTime=0;
        int availablePrcoessor=0;
        while(!OPEN.isEmpty() && processor<=numProcessors){
            if(OPEN.isEmpty()==false && processorSchduler[processor][0]<=0){
                availablePrcoessor=processor;
                processorSchduler[availablePrcoessor][0]++;
                job=OPEN.remove()->jobId;
                jobTime= table->graphHashTable[job].jobTime;
            }
            slot=time;
            while(slot<time+jobTime){
              processorSchduler[availablePrcoessor][slot] =job;
              slot++;
            }//end of while
            processor++;
        }//end of while
         time++;
         processor=1;
         cout<<time;
         while(processor<=numProcessors){

            if(processorSchduler[processor][0]>0 && processorSchduler[processor][time]<=0){

                int doneJob = processorSchduler[processor][time-1];
                cout<<doneJob<<endl;
                table->graphHashTable[doneJob].fatherCount--;
                processorSchduler[processor][0]=0;
                graphNode *current= table->graphHashTable[doneJob].stackTop;
                while(current!=NULL){
                    table->graphHashTable[current->jobId].fatherCount--;
                    current=current->next;
                }
            }//end of if
            processor++;
         }//end of while
         for(int i=0; i<numProcessors+1;i++){
            for(int j=0; j<totalJobtimes+1;j++){
               // if(processorSchduler[i][j]!=0)
                cout<<processorSchduler[i][j]<<" ";
            }
            cout<<endl;
        }
         for(int i=0;i<numJobs+1;i++){
           cout<<i+1<<" "<< table->graphHashTable[i].fatherCount<<endl;
         }
         table->printTable();
        }//end of it

        printTable(outfile);

   }//end of create Table

   bool isMarked(){
   for(int i=0;i<numJobs+1; i++){
    if(mark[i]==false)
        return false;
    }
    return true;
   }

   void printTable(ofstream &out){
       outfile<<"Scheduling table for "<<numJobs<<" jobs and we have "<<numProcessors<<" processors available.";
       for(int i=0; i<numProcessors+1;i++){
            for(int j=0; j<totalJobtimes+1;j++){
               if(processorSchduler[i][j]!=0)
                out<<processorSchduler[i][j]<<" ";
            }
            out<<endl;
        }
   }

    ~scheduling(){
    infile1.close();
    infile2.close();
    for(int i=0;i<numProcessors;i++){
        delete []processorSchduler[i];
    }
    delete [] processorSchduler;
    delete [] mark;
    }

};


int main(int argc, char* argv[]){

scheduling s(argv[1],argv[2],argv[3],argv[4]);

}
