#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

// config
const int THREAD_NUM = 1;
const string FILENAME = "test_data.txt";
const bool DEBUG = true;
const int N = 1000010;

// graph
int h[N];
int ne[N];
int e[N];
int idx;
bool st[N];

//global var
mutex m1;
condition_variable cv;
std::atomic<int> mapDoneCount;
vector<vector<int>> ans;

void print(string a) {
	if (DEBUG == true) {
		cout << a << endl;
	}
}

void add_edge(int a, int b, int weight)
{
	e[idx] = b;
	ne[idx] = h[a];
	h[a] = idx++;
}
void graph_init()
{
	idx = 0;
	memset(h, -1, sizeof h);
}
bool check_length(int x, int l = 2, int r = 8)
{
	return x > l && x < r;
}
void dfs(int u, int start, vector<int> &path)
{
	/*cout<<"path: ";
	for (auto x : path)
		cout << x << " ";
	cout << endl;
	*/
	if ((int)path.size() > 7)	return;
	
	for (int i = h[u]; i != -1; i = ne[i])
	{
		int j = e[i];
		// cout << "u: "<<u<<"   j: "<<j;
		if (start > j)
			continue;
		if (start == j)
		{
			if (check_length((int)path.size()))
			{
				ans.push_back(path);
			}
		}
		else if(st[j]==false){
			path.push_back(j);
			st[j] = true;
			dfs(j, start, path);
			st[j] = false;
			path.pop_back();

		}

	}


}

void run(int i)
{
	std::ifstream file(FILENAME);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			stringstream ss(line);
			int from, to, amount;
			char comma;
			ss >> from >>comma>> to >>comma>> amount;
			//printf("%d -> %d  weight %d\n", from, to, amount);
			add_edge(from, to, amount);
		}
		file.close();
	}
	else {
		print("file not open");
	}
	mapDoneCount.fetch_add(1);
	cout << i << "done!\n";
}
void merge()
{
	for (int i = 0; i < N; i++)
	{
		if (h[i] != -1)
		{
			int j = i;
			vector<int> path;
			st[j] = true;
			path.push_back(j);
			dfs(j, j, path);
			path.pop_back();
			st[j] = false;
		}
	}
}
bool cmp(vector<int> & a, vector<int> &b) {
	int asize = (int)a.size();
	int bsize = (int)b.size();
	if (asize < bsize)	return true;
	else if (asize > bsize) return false;
	else {
		for (int i = 0; i < asize; i++)
		{
			if (a[i] < b[i])
				return true;
			else if (a[i] > b[i])
				return false;
		}
		return true;
	}
}
void output()
{
	sort(ans.begin(), ans.end(),cmp);
	cout << ans.size() << endl;
	for (auto vec : ans)
	{
		for (auto x : vec)
		{
			cout << x << " ";
		}
		cout << endl;
	}

}
int main()
{
	graph_init();
	for (int i = 0; i < THREAD_NUM; i++)
	{
		thread t1(run, i);
		t1.join();
	}
	unique_lock<mutex> lock(m1);
	cv.wait(lock, [=] {return mapDoneCount == THREAD_NUM; });
	merge();
	output();
	return 0;
}