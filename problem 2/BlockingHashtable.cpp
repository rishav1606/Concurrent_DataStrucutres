#include<bits/stdc++.h>
using namespace std;
#define SIZE 4096
#define MAX_KEY 32
#define MAX_VALUE 256
#define THREADS 1

// error numbers
#define ENOSPACE -1
#define ENOWORD -2

long BUCKET = SIZE;    // No. of buckets
  
    // Pointer to an array containing buckets
list<string> *table = new list<string>[BUCKET];
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

mutex m[SIZE],g;

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
     m[index].lock();
    table[index].push_back(data);
    m[index].unlock(); 
}
  
void remove_delete(string key)
{ 
  //   int index = hashFunction(key);
  
  // // find the key in (inex)th list

   int index = hashFunction(key);
   m[index].lock();
       vector<string> all_sentence;
       for (auto x : table[index]){
         if(isWordPresent(x, key))
           all_sentence.push_back(x);
       }
       for(auto x:all_sentence){
      table[index].remove(x);
       }
  m[index].unlock();
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
   m[index].lock();
   g.lock();
       vector<string> all_sentence;
       for (auto x : table[index]){
         if(isWordPresent(x, key))
           cout<<x<<" ";
       }
       cout<<endl;
  g.unlock();
  m[index].unlock();
  
}
  
// function to display hash table
void displayHash() {
  for (int i = 0; i < BUCKET; i++) {
    cout << i<<" ";
    for (auto x : table[i])
      cout << " --> " << x;
  }
}

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

