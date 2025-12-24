#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <map>
#include <set>
using namespace std;

/* ===================== 图1：带权无向图 ===================== */

const int INF = 1e9;
vector<char> v1 = { 'A','B','C','D','E','F','G','H' };
map<char, int> idx1;

/* ---------- 构建图1邻接矩阵 ---------- */
vector<vector<int>> buildGraph1() {
    int n = v1.size();
    vector<vector<int>> g(n, vector<int>(n, INF));
    for (int i = 0; i < n; i++) g[i][i] = 0;

    auto addEdge = [&](char a, char b, int w) {
        int i = idx1[a], j = idx1[b];
        g[i][j] = g[j][i] = w;
        };

    addEdge('A', 'B', 4);
    addEdge('A', 'D', 6);
    addEdge('A', 'G', 7);
    addEdge('B', 'C', 12);
    addEdge('C', 'D', 9);
    addEdge('C', 'E', 1);
    addEdge('C', 'F', 2);
    addEdge('C', 'H', 10);
    addEdge('D', 'E', 13);
    addEdge('D', 'G', 2);
    addEdge('E', 'F', 5);
    addEdge('E', 'G', 11);
    addEdge('E', 'H', 8);
    addEdge('F', 'H', 3);
    addEdge('G', 'H', 14);

    return g;
}

/* ---------- 输出邻接矩阵 ---------- */
void printMatrix(const vector<vector<int>>& g) {
    cout << "邻接矩阵：\n  ";
    for (char c : v1) cout << c << " ";
    cout << endl;

    for (int i = 0; i < g.size(); i++) {
        cout << v1[i] << " ";
        for (int j = 0; j < g.size(); j++) {
            if (g[i][j] == INF) cout << "∞ ";
            else cout << g[i][j] << " ";
        }
        cout << endl;
    }
}

/* ---------- BFS ---------- */
void BFS(const vector<vector<int>>& g, int start) {
    int n = g.size();
    vector<bool> vis(n, false);
    queue<int> q;

    q.push(start);
    vis[start] = true;

    cout << "BFS: ";
    while (!q.empty()) {
        int u = q.front(); q.pop();
        cout << v1[u] << " ";
        for (int v = 0; v < n; v++) {
            if (g[u][v] != INF && !vis[v]) {
                vis[v] = true;
                q.push(v);
            }
        }
    }
    cout << endl;
}

/* ---------- DFS ---------- */
void DFSUtil(const vector<vector<int>>& g, int u, vector<bool>& vis) {
    vis[u] = true;
    cout << v1[u] << " ";
    for (int v = 0; v < g.size(); v++) {
        if (g[u][v] != INF && !vis[v])
            DFSUtil(g, v, vis);
    }
}

void DFS(const vector<vector<int>>& g, int start) {
    vector<bool> vis(g.size(), false);
    cout << "DFS: ";
    DFSUtil(g, start, vis);
    cout << endl;
}

/* ---------- Dijkstra ---------- */
void Dijkstra(const vector<vector<int>>& g, int s) {
    int n = g.size();
    vector<int> dist(n, INF);
    vector<bool> used(n, false);
    dist[s] = 0;

    for (int i = 0; i < n; i++) {
        int u = -1, minD = INF;
        for (int j = 0; j < n; j++)
            if (!used[j] && dist[j] < minD)
                minD = dist[j], u = j;

        if (u == -1) break;
        used[u] = true;

        for (int v = 0; v < n; v++)
            if (g[u][v] != INF && dist[v] > dist[u] + g[u][v])
                dist[v] = dist[u] + g[u][v];
    }

    cout << "A 到各点最短路径：\n";
    for (int i = 0; i < n; i++)
        cout << "A -> " << v1[i] << " = " << dist[i] << endl;
}

/* ---------- Prim ---------- */
void Prim(const vector<vector<int>>& g) {
    int n = g.size();
    vector<int> low(n, INF);
    vector<bool> used(n, false);
    low[0] = 0;
    int sum = 0;

    for (int i = 0; i < n; i++) {
        int u = -1, minW = INF;
        for (int j = 0; j < n; j++)
            if (!used[j] && low[j] < minW)
                minW = low[j], u = j;

        used[u] = true;
        sum += minW;

        for (int v = 0; v < n; v++)
            if (!used[v] && g[u][v] < low[v])
                low[v] = g[u][v];
    }

    cout << "最小生成树总权值: " << sum << endl;
}

/* ===================== 图2：双连通分量 ===================== */

vector<vector<int>> g2(12);
vector<int> dfn(12), low2(12);
set<int> cut;
int t = 0;

void add2(int a, int b) {
    g2[a].push_back(b);
    g2[b].push_back(a);
}

void tarjan(int u, int fa) {
    dfn[u] = low2[u] = ++t;
    int child = 0;

    for (int v : g2[u]) {
        if (!dfn[v]) {
            child++;
            tarjan(v, u);
            low2[u] = min(low2[u], low2[v]);
            if (fa != -1 && low2[v] >= dfn[u])
                cut.insert(u);
        }
        else if (v != fa)
            low2[u] = min(low2[u], dfn[v]);
    }

    if (fa == -1 && child > 1)
        cut.insert(u);
}

/* ===================== 主函数 ===================== */

int main() {
    for (int i = 0; i < v1.size(); i++) idx1[v1[i]] = i;

    auto g1 = buildGraph1();

    /* (1) 邻接矩阵 */
    printMatrix(g1);

    /* (2) BFS / DFS */
    BFS(g1, idx1['A']);
    DFS(g1, idx1['A']);

    /* (3) 最短路径 + 最小生成树 */
    Dijkstra(g1, idx1['A']);
    Prim(g1);

    /* (4) 图2双连通分量（关节点） */
    add2(0, 1); add2(1, 2); add2(2, 3);
    add2(0, 4); add2(4, 5); add2(5, 6); add2(6, 2);
    add2(2, 7); add2(5, 9); add2(9, 10);
    add2(10, 11); add2(6, 10); add2(4, 8);

    tarjan(0, -1);

    cout << "图2关节点：";
    for (int x : cut)
        cout << char('A' + x) << " ";
    cout << endl;

    return 0;
}
