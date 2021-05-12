#define SEATS 80
#define PRODUCER 1
#define CONSUMERS 50

#include<bits/stdc++.h>
using namespace std;

struct node
{
  int data;
  node * next;
  
  node(int d){
    data = d;
    next = NULL;
  }
};

mutex m;

class conQueue
{
    volatile atomic<node*> head, tail;
    volatile atomic<int> available;
    
    
  public:
    conQueue(){
     available = 0;
      node* newNode = new node(0);
      head.store(newNode);
      tail.store(newNode);
    }
    
    
    void enqueue(int i)
    {
        node * last,* newNode, *next;
        
        newNode = new node(i);
        while(true){
        last=tail.load();
        next = last->next;
         if(last == tail.load()){
             if(next == NULL){
               if(last->next = newNode){
                  available.fetch_add(1);
                  break;
                }
                else{
                  tail.compare_exchange_strong(last,next);
                }
             }
          }

      }
      tail.compare_exchange_strong(last,newNode);

      
       
    }
    
    
    int dequeue()
    {
      node *first,*next,*last;
      int value;
      while(available.load())
      {
        first=head.load();
        next=first->next;
        last=tail.load();
      
        if(first==head.load())
        {
          if(first==last)
          {
            if(next==NULL)
              continue;
            
            tail.compare_exchange_weak(last,next);
          }
          else
          {
            value = next->data;
            if(head.compare_exchange_weak(first,next))
            {
              free(first);
              available.fetch_add(-1);
              return value;
            }
          }
          
        }
 
      }
      //free(first);
      return -1;
      
    }
    
    void dump_queue()
    {
      cout<<endl;
      cout<<"head:"<<head<<", tail:"<<tail<<", available:"<<available<<endl;
      
      
      node * temp=head.load();
      temp = temp->next;
      while(temp!=NULL)
      {
        cout<<temp->data<<"  ";
        temp=temp->next;
      }
      cout<<endl;
    }
    
};

conQueue q;

void producer()
{
   for(int i=1;i<=SEATS;i++){
     q.enqueue(i); 
   }   
}

void consumer(int a)
{
  int n,no_of_tickets;
  no_of_tickets = rand()%4+1;
  n = 0;
  int seats[no_of_tickets];
   for(int i=0;i<no_of_tickets;i++){
       seats[i]= q.dequeue();
       if(seats[i]==-1)
       n++;
   }
    
         m.lock();
        cout<<"\nConsumer with ID:"<<a<<" has booked "<<no_of_tickets-n<<" seats out of total requested "<<no_of_tickets<<" with seat numbers ";
        for(int i=0;i<no_of_tickets-n;i++){
          if(seats[i]!=-1)
          cout<<"S"<<seats[i]<<", ";
        }
        
      m.unlock(); 


}

int main()
{
 
 thread deqThread[CONSUMERS+1];

 //producer(q);
 thread enqThread(producer);
 //q.enqueue();
 
 //q.dump_queue();
 for(int i=1;i<=CONSUMERS;i++){
 deqThread[i] = thread(consumer,i);
 

 }
 
 enqThread.join();
 for(int i=1;i<=CONSUMERS;i++){
 deqThread[i].join();
 }
   q.dump_queue();

 
 return 0;
 
}