#include<bits/stdc++.h>
using namespace std;
#define SIZE 4096
#define MAX_KEY 32
#define MAX_VALUE 256
#define THREADS 1

// error numbers
#define ENOSPACE -1
#define ENOWORD -2

struct node
{
  string data;
  node * next;
  
  node(string d){
    data = d;
    next = NULL;
  }
};


class conQueue
{
    volatile atomic<node*> head, tail;
    volatile atomic<int> available;
    
    
  public:
    conQueue(){
     available = 0;
      node* newNode = new node("0");
      head.store(newNode);
      tail.store(newNode);
    }
    
    
    void enqueue(string i)
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
    
    
    string dequeue()
    {
      node *first,*next,*last;
      string value;
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
      return "-1";
      
    }
    
    // void dump_queue()
    // {
    //   cout<<endl;
    //   cout<<"head:"<<head<<", tail:"<<tail<<", available:"<<available<<endl;
      
      
    //   node * temp=head.load();
    //   temp = temp->next;
    //   while(temp!=NULL)
    //   {
    //     cout<<temp->data<<"  ";
    //     temp=temp->next;
    //   }
    //   cout<<endl;
    // }
    
};





long BUCKET = SIZE;    // No. of buckets
  
    // Pointer to an array containing buckets
conQueue table[SIZE];
// list<string> *table = new list<string>[BUCKET];
    // hash function to map values to key
unsigned long hashFunction(string str) {
   unsigned long hash = 5381;
   int n = str.length();
   int c;
   for(int i=0;i<n;i++){
     c =(int)str[i];
       hash = ((hash << 5) + hash) + c;
      hash%=SIZE;
   }
 // cout<<hash<<" "<<SIZE<<endl;
  return hash;
}


bool isWordPresent(string sentence, string word)
{
    // To break the sentence in words
    stringstream s(sentence);
 
    // To temporarily store each individual word
    string temp;
 
    while (s >> temp) {
 
        // Comparing the current word
        // with the word to be searched
        if (temp.compare(word) == 0) {
            return true;
        }
    }
    return false;
}  

void insert(string key,string data)
{ 
    int index = hashFunction(key);
  // cout<<index<<endl;
    table[index].enqueue(data);
}
  
void remove_delete(string key)
{ 
  //   int index = hashFunction(key);
  
  // // find the key in (inex)th list

   int index = hashFunction(key);

       vector<string> all_sentence;
       while(true){
           string current = table[index].dequeue();
           if(current=="-1")
           break;
          if(!isWordPresent(current,key))
          all_sentence.push_back(current);
       }
       for (auto x : all_sentence){
           table[index].enqueue(x);
       }

  //   cout<<index<<" ";
  //  list <string> :: iterator i;
  // for (i = table[index].begin(); i != table[index].end(); i++) {
  //   if (isWordPresent(*i, key)){
  //     cout<<*i<<" ";
  //    // table[index].erase(i);
  //   }
  // }  

  // if key is found in hash table, remove it
}

void find(string key){
   int index = hashFunction(key);
 
     vector<string> all_sentence;
       while(true){
           string current = table[index].dequeue();
           if(current=="-1")
           break;
          if(isWordPresent(current,key))
          cout<<current<<" ";
          all_sentence.push_back(current);
       }
       cout<<endl;
       for (auto x : all_sentence){
           table[index].enqueue(x);
       }

  
}
  
// function to display hash table
// void displayHash() {
//   for (int i = 0; i < BUCKET; i++) {
//     cout << i<<" ";
//     for (auto x : table[i])
//       cout << " --> " << x;
//   }
// }

void processfile(char* fname)
{
  char operation, *wptr,*temp;
  const int LINE_SIZE = MAX_KEY+MAX_VALUE+5;
  char line[LINE_SIZE];
  char word[MAX_KEY+1]; //= (char *)"big";
  char sentence[MAX_VALUE+1];  //= (char *)"At one end of the big barn";
  fstream fh;
  
  fh.open(fname,ios::in);
  
  if(!fh)
    cout<<fname<<" file can't be opened\n";
  else{
    cout<<fname<<" file successfully opened\n";
    
    //fh.seekg(0,ios::beg);
    int lc=1;
    while(fh.getline(line,LINE_SIZE)){
     
     operation = line[0];
     if(operation=='I'){
     
      //line parsing start
      wptr = strchr(line+2, ',');
      temp = line+2;
      int i=0;
      while(temp!=wptr)
      {
      
        word[i++]=*temp;
        ++temp;
      }
      word[i]='\0';
      strcpy(sentence,wptr+1);
      //line parsing end
     
      insert(word,sentence);
      
     } 
     else if(operation=='R'){
      temp = line+2;
      int i=0;
      while(*temp!='\0')
      {
      
        word[i++]=*temp;
        ++temp;
      }
      word[i]='\0';
      
      remove_delete(word);
      
     }
     else{
      temp = line+2;
      int i=0;
      while(*temp!='\0')
      {
      
        word[i++]=*temp;
        ++temp;
      }
      word[i]='\0';
      
      find(word);
     }
     
    }
    fh.close();
  }
}

  
// Driver program 
int main(int argc, char *argv[])
{
 thread thd[THREADS];
  
 for(int i=0;i<THREADS;i++){
  thd[i] = thread(processfile,argv[i+1]);
 }
 
 
 for(int i=0;i<THREADS;i++){
  thd[i].join();
 }   
 //   displayHash();
  return 0;
}

