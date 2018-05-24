#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#define intmax 10000000

using namespace std;

typedef struct Edge {
    int weight;
    int beg, end;
} Edge;

int programs;
int computers;
int vertices;
vector <Edge> edges;
int** initialMatrix;
int cost = 0;
int* matchingFunction;

int** init(){

    fstream inFile;
    inFile.open("in.txt");
    inFile >> programs >> computers;

    vector <Edge> lineVector;
    vector < vector<Edge> > adjacencyList;
    vertices = 2*(programs + 1);

    for(int i = 1; i <= programs; i++){
        Edge* src = new Edge();
        src->beg = 0;
        src->end = i;
        src->weight = 0;
        lineVector.push_back(*src);
        delete src;
    }

    adjacencyList.push_back(lineVector);
    lineVector.clear();
    lineVector.shrink_to_fit();

    for(int i = 1; i <= programs; i++){
        for(int j = 1; j <= computers; j++){
            Edge* e = new Edge();
            inFile >> e->weight;
            e->beg = i;
            e->end = j + programs;
            lineVector.push_back(*e);
            delete e;
        }
    }
    adjacencyList.push_back(lineVector);
    lineVector.clear();
    lineVector.shrink_to_fit();


    for(int i = programs + 1; i <= 2 * programs; i++){
        Edge* t = new Edge();
        t->beg = i;
        t->end = 2 * programs + 1;
        t->weight = 0;
        lineVector.push_back(*t);
        delete t;
    }
    adjacencyList.push_back(lineVector);
    lineVector.clear();
    lineVector.shrink_to_fit();

    inFile.close();

    int x, y;

    int** matrix = new int* [vertices];
    for(int i = 0 ; i < vertices; i++){
        matrix[i] = new int[vertices];
    }

    for(int i = 0; i < vertices; i++){
        for(int j = 0; j < vertices; j++){
            matrix[i][j] = intmax;
        }
    }

    for(unsigned int i = 0; i < adjacencyList.size(); i++){
        for(unsigned int j = 0; j < adjacencyList[i].size(); j++){
            x = adjacencyList[i][j].beg;
            y = adjacencyList[i][j].end;
            matrix[x][y] = adjacencyList[i][j].weight;
        }
    }

    initialMatrix = new int* [vertices];
    for(int i = 0 ; i < vertices; i++){
        initialMatrix[i] = new int[vertices];
    }

    for(int i = 0; i < vertices; i++){
        for(int j = 0; j < vertices; j++){
            initialMatrix[i][j] = matrix[i][j];
        }
    }

    matchingFunction = new int[programs];
    for(int i = 0; i < programs; i++){
        matchingFunction[i] = -1;
    }

    for(unsigned int i = 0; i < adjacencyList.size(); i++){
        adjacencyList[i].clear();
        adjacencyList[i].shrink_to_fit();
    }
    adjacencyList.clear();
    adjacencyList.shrink_to_fit();


    return matrix;

}

int minDistance(int* distDijkstra, bool* finished){
   int min = intmax;
   int min_index;

    for (int v = 0; v < vertices; v++){
        if (finished[v] == false && distDijkstra[v] <= min){
                min = distDijkstra[v];
                min_index = v;
        }
    }

   return min_index;
}

void changeWeights(int** graph, int* dist){
    int newWeight;
    for(int i = 0; i < vertices; i++){
        for(int j = 0; j < vertices; j++){
            if(graph[i][j] != intmax){
                newWeight = graph[i][j] - (dist[j] - dist[i]);
                if(newWeight >= 0){
                    graph[i][j] = newWeight;
                } else {
                    graph[i][j] = 0;
                }
            }
        }
    }

}

void getPath(int* pred, int j){
    if (pred[j] == -1){
        return;
    }

    getPath(pred, pred[j]);
    Edge *e = new Edge();
    e->beg = pred[j];
    e->end = j;
    edges.push_back(*e);
    delete e;
}

void reverseEdges(int** graph){
    int x, y;

    for(unsigned int i = 0; i < edges.size(); i++){
        x = edges[i].beg;
        y = edges[i].end;
        swap(graph[x][y], graph[y][x]);
    }

}

bool succesiveShortestPath(int** graph, int source){

    bool match = false;
    bool* finished = new bool[vertices];
    int* distDijkstra = new int[vertices];
    int* pred = new int[vertices];

    for(int i = 0; i < vertices; i++){
        distDijkstra[i] = intmax;
        finished[i] = false;
        pred[i] = -1;
    }

    distDijkstra[source] = 0;
    int u;

    //Dijkstra = n^2
    for(int i = 0; i < vertices; i++){
        u = minDistance(distDijkstra, finished);
        finished[u] = true;
        for(int v = 0; v < vertices; v++){
            if(distDijkstra[v] > distDijkstra[u] + graph[u][v] && graph[u][v] != intmax){
                distDijkstra[v] = distDijkstra[u] + graph[u][v];
                pred[v] = u;
            }
        }
    }

    getPath(pred, vertices - 1); // n
    changeWeights(graph, distDijkstra); // n^2

    int x, y;

    //p^2
    for(unsigned int i = 0; i < edges.size(); i++){
        x = edges[i].beg;
        y = edges[i].end;
        for(int j = 1; j <= programs; j++){
            if(x == j){
                for(int k = programs; k <= 2*programs; k++){
                    if(y == k){
                        matchingFunction[x-1] = y;
                    }
                }
            }
        }
    }

    int flag = 0;

    for(int i = 0; i < programs; i++){
        if(matchingFunction[i] != -1){
            flag++;
        }
    }

    if(flag == programs){
        fstream outFile;
        outFile.open("out.txt", ofstream::out | ofstream::trunc);
        for(int i = 0; i < programs; i++){
            outFile << matchingFunction[i] - programs << " ";
            cost += initialMatrix[i+1][matchingFunction[i]];
        }
        outFile << endl << cost;
        outFile.close();
        match = true;
    } else if(flag == 0){
        match = true;
    }

    //n
    reverseEdges(graph);
    edges.clear();
    edges.shrink_to_fit();
    delete[] distDijkstra;
    delete[] finished;
    delete[] pred;

    return match;

}



int main(){

    int** graph = init();

    while(!succesiveShortestPath(graph, 0));

    for(int i = 0; i < vertices; i++) {
        delete[] graph[i];
    }
    delete[] graph;

    for(int i = 0 ; i < vertices; i++){
        delete[] initialMatrix[i];
    }
    delete[] initialMatrix;
    delete[] matchingFunction;

    return 0;

}
