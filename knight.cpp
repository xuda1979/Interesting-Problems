// Author: Da Xu 
// Date: July 2, 2017

#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <algorithm>
#include <stdlib.h>
using namespace std;

// defines a structure for chess moves
struct Position {
     int x, y;   
};

Position operator+(const Position& p1, const Position& p2) {
     Position p{p1.x+p2.x, p1.y+p2.y};
     return p;
}

Position operator-(const Position& p1, const Position& p2) {
     Position p{p1.x-p2.x, p1.y-p2.y};
     return p;
}

bool operator==(const Position& a, const Position& b)
{
     return a.x == b.x && a.y == b.y;
}
     
void print(const Position& pos) {
     cout<<"{"<<pos.x<<","<<pos.y<<"} ";
}     
// all possible moves that knight can take
Position move_KT[8] = { {2,1},{1,2},{-1,2},{-2,1},
                               {-2,-1},{-1,-2},{1,-2},{2,-1} };
 

bool isInside(int x, int y, int N)
{
     if (x >= 0 && x < N && y >= 0 && y < N)
         return true;
     return false;
}

bool isInside(const Position& pos, int N)
{
     if (pos.x >= 0 && pos.x < N && pos.y >= 0 && pos.y < N)
         return true;
     return false;
}

//level 1

// The sequence is the sequence of the positions of the knight
bool isSeqValid(const vector<Position>& moveSequence, int N) {
     int len =(int)  moveSequence.size();
     for(int i = 0; i<len; i++) {
         if( moveSequence[i].x <0 || moveSequence[i].x >=N
             || moveSequence[i].y<0 || moveSequence[i].y>=N)
             return false;
         if(i+1< len)
         {
             for(int j=0; j<N; j++) {
                 Position move = {moveSequence[i+1].x-moveSequence[i].x,
                                     moveSequence[i+1].y-moveSequence[i].y};
                 if( move == move_KT[j])
                     return true;
             }
             return false;    
         }           
     }        
}


void printCurrentPosition(const Position& currentPosition, int N)
{
     for(int i = 0; i<N; i++) {
         for(int j = 0; j<N; j++) {
             if( i == currentPosition.x && j == currentPosition.y)
                 cout<<"K";
             else
                 cout<<"*";
         }
         cout<<endl;
     }        
     
}   

// level 2
vector<Position> findPath(const Position& start, const Position& end, int N) {
     // static 2d array to record if a position has been visited or not
     static int findPathTour[32][32] = {{0}};
     vector<Position> v;
     if(!isInside(start,N) || !isInside(end,N)) {
         cout<<"start or end position is out of the board!"<<endl;
         return v;
     }
     v.push_back(move(start));
  
     findPathTour[start.x][start.y] =1;
     
     if(v.back() == end)
         return v;
     
     for(int i = 0; i<8; i++){
         
         Position next = v.back()+move_KT[i];
         if(isInside(next,N))
         {
             if(findPathTour[next.x][next.y] == 0){    
                 findPathTour[next.x][next.y] = 1;      
                 vector<Position> v1 = findPath(next, end, N);
                 if(v1.back() == end) {
                     int s = v.size();
                     v.reserve(s+v1.size()); // preallocate memory
                     v.insert(v.end(),v1.begin(), v1.end());
                     return v;
                 }
             }
          }
         
     }
}


// level 3

struct Node{
    
     Node(const Position& pos,  Node* par) {
         current = pos;       
         parent = par;
         if(parent !=nullptr && parent->path.size()>0)
             path = parent->path;
         path.push_back(pos);
         
     }
     
     Position current;
     vector<Position> path;
     Node* parent;
     set<Node*> nextMoves;
        
};

// to print all the positions of a sequence from the starting point to the end 
// point
void print(const vector<Position>& v) {
     int s = 0;
     for(auto e:v) {
          s++;
          print(e);
          if(s%10 ==0)
              cout<<endl;
     }
     cout<<endl<<endl;
}

void print(const vector<vector<Position>>& v, int less) {
     int i = 0;
     for(auto path: v){
         if(i<less)
             print(path);
         i++;
     }
}

// the following function construct a search tree which will give the optimal solution
// If N= 8, it is pretty fast but it is very slow when N= 32. Therefore for large N,
// we will use another method to simulate paths, which is "Monte Carlo Tree Search"
// we also write a iterative search tree which is much better but still slow for long
// distance points and large N. It seems the working method in practice should be
// Monte Carlo Tree Search

Node* searchTreeShortest(const Position& start, const Position& end, Node* parent, int N) {
     static int shortest = 2*N;
     if(!isInside(start,N) || !isInside(end,N)) {
         cout<<"start or end position is out of the board!"<<endl;
         return nullptr;
     }
     Node* root = new Node(start, parent);
     
     if(start == end) {
         if(shortest >= (int) root->path.size()-1)
             shortest = (int)root->path.size()-1;
         return root;
     }
     
     // we only continue the next step of the path if its current length is smaller
     // than shortest 
     if(root->path.size()-1 < shortest){
         for(int i = 0; i<8; i++) {
             Position next = root->current+move_KT[i];
             if(isInside(next,N) &&
                find(root->path.begin(),root->path.end(), next) == root->path.end()) {                       
                     root->nextMoves.insert(searchTreeShortest(next, end, root, N));                                      
             }        
             
         }
     } 
     
     return root;    
}

// this searchTree is to use iteration method. The space complexity is much smaller than the recursive version above
// we use limit to denote the numbers of the paths we want to collet. If limit is set to 0, the result will include
// all shortest paths and will be time consuming when N is large. 

vector<vector<Position>> searchTreeShortestIteration(const Position& start, 
                                                     const Position& end, 
                                                     Node* parent, 
                                                     int N, 
                                                     int limit = 0) {

     int shortest = max(abs(start.x-end.x) ,abs(start.y-end.y));
     vector<vector<Position>> result;
     if(!isInside(start,N) || !isInside(end,N)) {
         cout<<"start or end position is out of the board!"<<endl;
         return result;
     }
     stack<Node> s;
    
     Node node(start, parent);
     s.push(node);
     
     while(!s.empty()){
         Node p = s.top();
         s.pop();
         
         if(p.current == end && p.path.size()-1 <=shortest){
             shortest = p.path.size()-1;
             result.push_back(p.path); 
             if(limit>0 && result.size() >=limit)
                 return result;           
         }
         
         if(!(p.current ==end) && p.path.size()-1 < shortest){
             for(int i = 0; i<8; i++) {
                 Position next = p.current+move_KT[i];
                 if(isInside(next,N) &&
                    find(p.path.begin(),p.path.end(), next) == p.path.end()) {                       
                     Node node(next, &p);
                     s.push(node);                                       
                 }        
             
              }
         }
     } 
         return result;
}


// this function is to get all the paths from the search tree
vector<vector<Position>>  getAllPaths(Node* root, const Position& end) {
     vector<vector<Position>> allPaths;
     if(root->nextMoves.size() == 0 && root->current == end)
     {     
         allPaths.push_back(move(root->path));
         return allPaths;
     }
     
     for(auto& p : root->nextMoves) {
         vector<vector<Position>> subAllPaths = getAllPaths(p,end);
         for(auto& path: subAllPaths) {         
             allPaths.push_back(move(path));
         }    
     }
     
     return allPaths;
}

// this function is to computed exactly all the shortest paths
vector<vector<Position>> shortestPaths(const vector<vector<Position>>& allPaths, const Position& start, const Position& end, int N) {
     int shortest = N*N;
     for(int i = 0; i< allPaths.size(); i++) {
         if((int)allPaths[i].size()-1 <= shortest
             && allPaths[i][0] == start
             && allPaths[i].back() == end) {
             shortest = (int) allPaths[i].size() -1;  
         }
     }
 
     vector<vector<Position>> shortPaths;
     for(auto& path:allPaths)
         if((path.size()-1) == shortest
             && path[0]== start
             && path.back()== end)
             shortPaths.push_back(move(path));
     
     return shortPaths;    
}

// level 4

// The technique is that we make a weight matrix for the cell on the board to 
// give the weight of each cell. If a cell is forbidden, we set the weight at that 
// cell as -1; and we return the length of that path to  be -1 ,which means that path 
// is not allowed.


// initialize a weight matrix with all ones( the trivial case). Then we can change
// the value to anyvalue
int weightedLength(const vector<Position>& path, const vector<vector<int>>& weight) {
     int s = 0;
     for(int i =1; i< path.size(); i++) {
         if(weight[path[i].x][path[i].y] != -1)
             s += weight[path[i].x][path[i].y];
         else
             return -1;
     }
     return s;
}

 

// In the description of the problem, different weights are assigned to cells 
// of the board. So we can define a weight on the board and compute the weighted 
// lenght of each path.

vector<vector<Position>> weightedShortestPaths(const vector<vector<Position>>& allPaths, 
                                               const Position& start,
                                               const Position& end, 
                                               int N,
                                               const vector<vector<int>>& weight) {
     int shortest = 5*N*N;
     for(int i = 0; i< allPaths.size(); i++) {
         if( weightedLength(allPaths[i],weight)<= shortest
             && weightedLength(allPaths[i],weight) >=0 // we set forbidden path's weighted length -1
             && allPaths[i][0] == start
             && allPaths[i].back() == end) {
             shortest = (int) allPaths[i].size() -1;  
         }
     }
 
     vector<vector<Position>> shortPaths;
     for(auto& path:allPaths)
         if( weightedLength(path,weight) == shortest
             && path[0]== start
             && path.back()== end)
             shortPaths.push_back(move(path));
     
     return shortPaths;    
}

// this function is to use Monte Carlo Tree Search to simulate the paths and 
// find the best effort solution
vector<Position> randomWalkWeightedShortestPath(const Position& start, 
                                                const Position& end,
                                                int N , 
                                                int limit, 
                                                const vector<vector<int>>& weight){
     int tour[N][N] = {0};
     vector<Position> v;
     bool found = false;
     if(!isInside(start,N) || !isInside(end,N)) {
         cout<<" bad input of start or end positions or board scale"<<endl;
         return v;
     }

     int shortest =5*N*N;

     for(int i=0; i<limit; i++) {

         for(int i= 0;i<N; i++)
             for(int j=0;j<N; j++)
                tour[i][j] = 0;

         vector<Position> vv;
         tour[start.x][start.y] = 1;
         vv.push_back(move(start));
         while(find(vv.begin(), vv.end(),end) == vv.end()) {
             bool moveable = false;
             Position next;
             for(int i=0;i<8;i++) {
                 next = vv.back()+move_KT[i];
                 if(isInside(next,N) && tour[next.x][next.y] ==0)
                     moveable = true;
             }
             if(!moveable)
                 break; 
             
             int r = rand() % 8;
             next = vv.back()+move_KT[r];
             if(isInside(next,N) && tour[next.x][next.y] ==0){
                 tour[next.x][next.y] = 1;
                 vv.push_back(move(next));
             }

             if(weightedLength(vv,weight) >shortest)
                 break;
         }

         if(weightedLength(vv,weight) <=shortest && vv.back()==end)
         {  
             v = vv;
             shortest = weightedLength(vv, weight);
             
         }
        
     }
     return v;

}

//level 5

// We use Monte Carlo Tree Search for this problem. We simulate each step untill arrive at the destination.
// There is no duplicat step in a path.

vector<Position> randomWalkLongestPath(const Position& start, const Position& end, int N, int limit=10000)
{
     int tour[N][N] = {0};
     vector<Position> v;
     bool found = false;
     if(!isInside(start,N) || !isInside(end,N)) {
         cout<<" bad input of start or end positions or board scale"<<endl;
         return v;
     }

     int longest = 0;

     for(int i=0; i<limit; i++) {

         for(int i= 0;i<N; i++)
             for(int j=0;j<N; j++)
                tour[i][j] = 0;

         vector<Position> vv;
         tour[start.x][start.y] = 1;
         vv.push_back(move(start));
         while(find(vv.begin(), vv.end(),end) == vv.end()) {
             bool moveable = false;
             Position next;
             for(int i=0;i<8;i++) {
                 next = vv.back()+move_KT[i];
                 if(isInside(next,N) && tour[next.x][next.y] ==0)
                     moveable = true;
             }
             if(!moveable)
                 break; 
             
             int r = rand() % 8;
             next = vv.back()+move_KT[r];
             if(isInside(next,N) && tour[next.x][next.y] ==0){
                 tour[next.x][next.y] = 1;
                 vv.push_back(move(next));
             }
         }
         if((vv.size()-1) >=longest && vv.back()==end)
         {  
             v = vv;
             longest =vv.size()-1;
             
         }
        
     }
     return v;
}

int main() {
    // the scale of the board
    int N = 8;
    //level 1 test
    cout<<"level 1 test result:"<<endl;
    vector<Position> s1 = {{1,1}, {3,2}, {5,3},{6,1},{4,0},{2,1}};
    vector<Position> s2 = {{9,0}};
    vector<Position> s3 = {{-2,1}};
    vector<Position> s4 = {{3,4},{5,7}};
    
    //expect 1, 0, 0, 0(1 means true, 0 means false)
    print(s1);
    string valid = isSeqValid(s1,8)?"valid":"invalid";
    cout<<valid<<endl<<endl;
    print(s2);
    valid = isSeqValid(s2,8)?"valid":"invalid";
    cout<<valid<<endl<<endl;
    print(s3);
    valid = isSeqValid(s3,8)?"valid":"invalid";
    cout<<valid<<endl<<endl;
    print(s4);
    valid = isSeqValid(s4,8)?"valid":"invalid";
    cout<<valid<<endl<<endl;
     
    // assume current position is at (1,1)
    Position currentPosition = {1,1};
    printCurrentPosition(currentPosition,8);
    cout<<endl<<endl;
    
    
    // level 2
    // we set the board sacle to be 32x32 for this problem.
    N = 32;
    cout<<"level 2 test result:"<<endl;
    Position start = {0,0};
    Position end = {7,7};
    cout<<"This is one of the paths from ";
    print(start);
    cout<<" to ";
    print(end);
    cout<<" in a "<<N<<"x"<<N<<" board";
    cout<<endl;
    vector<Position> path = findPath(start,end, N);
    print(path);  
    cout<<endl<<endl;
    
    //level 3
    N = 32;
    end= {10,10};
    cout<<"level 3 test result:"<<endl;   
    
    // The following three lines is to use recursive search tree which is memory consuming.
    // Node* root = searchTreeShortest(start, end, nullptr, N); 
    // vector<vector<Position>>  allPaths = getAllPaths(root, end);    
    // Node* root = searchTreeShortest(start, end, nullptr, N); 
   
    // we use iterative search tree. My computer feels much relaxed.
    vector<vector<Position>>  allPaths = searchTreeShortestIteration(start, end , nullptr, N); 
    cout<<endl;
    vector<vector<Position>> vv = shortestPaths(allPaths,start,end, N);
    cout<<"We use iterative search tree to get optimal solution for this problem the optimal solution can be"<<endl;
    cout<<"computed for N = "<<N<<endl;  
    cout<<"Shortest paths in a "<<N<<"x"<<N<<" board from ";
    print(start);
    cout<<" to ";
    print(end);
    cout<<endl;
    cout<<"Number of steps is "<<vv[0].size()-1;
    cout<<"(only show 10 of all the shortest paths)"<<endl;
    print(vv,10);
    cout<<endl<<endl;
    
    //level 4
    end = {31,31}; // now end position is at the corner of the board
    N = 32;
    int n_samples = 1000;
    vector<vector<int>> weight(N, vector<int>(N,1));
    cout<<"level 4 test result:"<<endl;
    cout<<"we can set the weight to be any value as we want. Here we show an example."<<endl;
    cout<<"See the code for the definition of the weights on the board"<<endl;
    //this weight is an example. We can manually set it to be anyting we want.
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            weight[i][j]=1;
 
    cout<<endl;
  
    vector<Position> weightedv = randomWalkWeightedShortestPath(start,end, N, n_samples,weight);
    
    cout<<"Weighted Shortest paths in a 32x32 board from ";
    print(start);
    cout<<" to ";
    print(end);
    cout<<"Using random walk "<<n_samples<<" sample paths"<<endl;
    cout<<endl;
    print(weightedv);
    cout<<endl<<endl;

    //level 5
    N = 32;
    n_samples = 1000; // number of the samples which can be increased if computing resources are adequate
    cout<<"level 5 test result:"<<endl;
    cout<<"Longest paths in a 32x32 board from ";
    print(start);
    cout<<" to ";
    print(end);
    cout<<"Using random walk from "<<n_samples<<" sample paths"<<endl;
    cout<<"Using random walk "<<n_samples<<" sample paths"<<endl;
    vector<Position> vrandom = randomWalkLongestPath(start,end, N, n_samples);
    if(!vrandom.empty())
        cout<<"Total number of steps "<<vrandom.size()-1<<endl;
    print(vrandom);
    cout<<endl;

    cout<<"For level 5, it seems that we have to use Monte Carlo Tree Search or some simulation. The optimal solution"<<endl;
    cout<<" will be too expensive "<<endl;
    return 0;
}
