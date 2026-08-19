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
vector<VertexSet> cliques;



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


void bronKerbosch(VertexSet R, VertexSet P, VertexSet X, int depth, ofstream& out, long &maximumClique)
{
    if (P.empty() && X.empty())
    {
        //maximumClique.assign(R.begin(), R.end());
        #pragma omp atomic
        ++maximalCount;
        #pragma omp critical
        {
            if (R.size() > maximumClique)
            {
                maximumClique = R.size();
            }
            for (int i : R)
            {
                out << i << " ";
            }
            out << "\n";
        }
    }


    VertexSet candidate = P;
    for (int v : candidate)
    {
        VertexSet newR = R;
        newR.insert(v);
        
        VertexSet newP = IntersectSets(graph[v], P);
        VertexSet newX = IntersectSets(graph[v], X);
        if (depth < 2 && newP.size() > 50)
        {
            #pragma omp task firstprivate(newR,newP,newX, depth) shared(out)
            {
                bronKerbosch(newR, newP, newX, depth + 1, out, maximumClique);

            }
        }
        else
        {
            bronKerbosch(newR, newP, newX, depth + 1, out, maximumClique);
        }
        P.erase(v);
        X.insert(v);

    }
    #pragma omp taskwait

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
    string filePath = "7_87_75%.txt";

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
    ifstream inputFile("7_87_75%.txt");
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

    // #pragma omp parallel for schedule(dynamic)
    // for (int i = 0; i < static_cast<int>(tasks.size()); i++)
    // {
    //     bronKerbosch(tasks[i].R, tasks[i].P, tasks[i].X, 0);
    // }
    //
    std::filesystem::path path(filePath);
    string fileName = path.stem().string(); 
    string folder = fileName+ "_MaximalCliques";
	std::filesystem::create_directories(folder);
	ofstream out(folder + "/" + fileName + "_MaximalCliques");

	long maximumClique = 0;
    #pragma omp parallel
    {
        #pragma omp single
        {
            for (const Task& task : tasks)
            {
                #pragma omp task firstprivate(task)
                {
                    bronKerbosch(task.R, task.P, task.X, 0, out, maximumClique);
                }
            }
            #pragma omp taskwait
        }
    }
    cout << maximalCount <<endl;
    cout << maximumClique << endl;
    for (VertexSet i : cliques){
        for (int j : i)
        {
            cout << j << " ";
        }
        cout  << endl;
    }



}
