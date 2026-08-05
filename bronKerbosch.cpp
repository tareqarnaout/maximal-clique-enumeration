#include <fstream>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;


using VertexSet = unordered_set<int>;
int maximalCount = 0;
vector<VertexSet> graph;
vector<int> maximumClique;



struct Task{
    VertexSet R;
    VertexSet P;
    VertexSet X;
};
// thisd is used as for intersectins calculation in the bk algorithm ex (N(v) ∩ P )
VertexSet IntersectSets(VertexSet &first, VertexSet &Second)
{
   VertexSet resluts;

  for (int i : first)
  {
      if (Second.find(i) != Second.end())
      {
          resluts.insert(i);
      }
  }

  return resluts;
}


void bronKerbosch(VertexSet R, VertexSet P, VertexSet X)
{
    if (P.empty() && X.empty())
    {
        maximumClique.assign(R.begin(), R.end());
         ++maximalCount;
    }


    VertexSet candidate = P;
    for (int v : candidate)
    {
        VertexSet newR = R;
        newR.insert(v);
        VertexSet newP = IntersectSets(graph[v], P);
        VertexSet newX = IntersectSets(graph[v], X);
        bronKerbosch(newR, newP, newX);
        P.erase(v);
        X.insert(v);

    }

}



vector<Task> createTopLevelTasks(int graphSize)
{
    vector<Task> tasks;
    VertexSet P;
    VertexSet X;
    for (int i = 0; i < graphSize; i++)
    {
        P.insert(i);
    }

    VertexSet candidates = P;

    for (int v : candidates)
    {
        Task t;
        t.P = IntersectSets(graph[v], P);
        t.X = IntersectSets(graph[v], X);
        t.R.insert(v);
        tasks.push_back(move(t));
        P.erase(v);
        X.insert(v);
    }

    return  tasks;
}

int main()
{
    string filePath = "9_985_30%.txt";

    // Get only the filename without its extension:
    // graphs/1_100_25.txt -> 1_100_25
    string filename = filesystem::path(filePath).stem().string();

    int id;
    int graphSize;
    int percentage;

    char underscore1;
    char underscore2;

    stringstream parser(filename);

    parser >> id
            >> underscore1
            >> graphSize
            >> underscore2
            >> percentage;

    if (parser.fail() || underscore1 != '_' || underscore2 != '_')
    {
        cerr << "Invalid filename format\n";
        return 1;
    }




    ////////////////////////////////////////////////////////////////////
    ifstream inputFile("9_985_30%.txt");
    int n = graphSize;
    if (!inputFile)
    {
        cout << "error opening file" << endl;
        return 0;
    }

    int vertex;
    int neighbor;
    graph.resize(n);
    while (inputFile >> vertex >> neighbor)
    {
        graph[vertex].insert(neighbor);
    }


    vector<Task> tasks = createTopLevelTasks(n);
    // VertexSet R;
    // VertexSet P;
    // VertexSet X;

    // for (int i = 0 ; i < n; i++)
    // {
    //     P.insert(i);
    // }

    //bronKerbosch(R, P, X);

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(tasks.size()); i++)
    {
        bronKerbosch(tasks[i].R, tasks[i].P, tasks[i].X);
    }
    cout << maximalCount << "****" << endl;



}
