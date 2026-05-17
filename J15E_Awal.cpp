#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <cstring>
using namespace std;

int x, y;
vector<int> top_arr, bottom_arr, left_arr, right_arr;
char grid[105][105];

struct Endpoint {
    int type, idx, id, r, c, dir;
};

struct State {
    int dist, r, c, dir;
    
    bool operator>(const State& other) const {
        return dist > other.dist; 
    }
};

struct ParentInfo {
    short r, c;
    char dir, mirror;
};

ParentInfo parent_info[105][105][4];
int dist[105][105][4];

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> x >> y)) return 0;

    top_arr.resize(y); bottom_arr.resize(y); left_arr.resize(x); right_arr.resize(x);
    for (int i = 0; i < y; ++i) cin >> top_arr[i];
    for (int i = 0; i < x; ++i) cin >> left_arr[i] >> right_arr[i];
    for (int i = 0; i < y; ++i) cin >> bottom_arr[i];

    for (int r = 0; r < x; ++r) {
        for (int c = 0; c < y; ++c) grid[r][c] = '.';
        grid[r][y] = '\0';
    }

    vector<Endpoint> perim;
    for (int i = 0; i < y; ++i) perim.push_back({0, i, top_arr[i], 0, i, 2});
    for (int i = 0; i < x; ++i) perim.push_back({1, i, right_arr[i], i, y - 1, 3});
    for (int i = y - 1; i >= 0; --i) perim.push_back({2, i, bottom_arr[i], x - 1, i, 0});
    for (int i = x - 1; i >= 0; --i) perim.push_back({3, i, left_arr[i], i, 0, 1});

    vector<pair<Endpoint, Endpoint>> pairs_to_route;
    stack<Endpoint> s;
    for (auto ep : perim) {
        if (!s.empty() && s.top().id == ep.id) {
            pairs_to_route.push_back({s.top(), ep});
            s.pop();
        } else {
            s.push(ep);
        }
    }

    if (!s.empty()) {
        cout << -1 << "\n";
        return 0;
    }

    for (auto p : pairs_to_route) {
        Endpoint start_ep = p.first;
        Endpoint target_ep = p.second;

        memset(dist, 0x3f, sizeof(dist)); 
        priority_queue<State, vector<State>, greater<State>> pq;

        pq.push({0, start_ep.r, start_ep.c, start_ep.dir});
        dist[start_ep.r][start_ep.c][start_ep.dir] = 0;

        bool found = false;

        while (!pq.empty()) {
            State curr = pq.top();
            pq.pop();

            if (curr.dist > dist[curr.r][curr.c][curr.dir]) continue;

            int r = curr.r, c = curr.c, dir = curr.dir;

            char possible_mirrors[2];
            int num_m = 0;
            
            if (grid[r][c] == '.') {
                possible_mirrors[0] = '/'; possible_mirrors[1] = '\\'; num_m = 2;
            } else {
                possible_mirrors[0] = grid[r][c]; num_m = 1;
            }

            for (int i = 0; i < num_m; ++i) {
                char m = possible_mirrors[i];
                int ndir = -1;
                
                if (m == '\\') { 
                    int next_dir[] = {3, 2, 1, 0}; ndir = next_dir[dir];
                } else {         
                    int next_dir[] = {1, 0, 3, 2}; ndir = next_dir[dir];
                }

                int nr = r, nc = c;
                if (ndir == 0) nr--; else if (ndir == 1) nc++; else if (ndir == 2) nr++; else if (ndir == 3) nc--;

                if (nr < 0 || nr >= x || nc < 0 || nc >= y) {
                    bool is_target = false;
                    if (nr < 0 && target_ep.type == 0 && target_ep.idx == nc) is_target = true;
                    else if (nr >= x && target_ep.type == 2 && target_ep.idx == nc) is_target = true;
                    else if (nc < 0 && target_ep.type == 3 && target_ep.idx == nr) is_target = true;
                    else if (nc >= y && target_ep.type == 1 && target_ep.idx == nr) is_target = true;

                    if (is_target) {
                        int br = r, bc = c, bdir = dir;
                        char bm = m;
                        while (true) {
                            if (grid[br][bc] == '.') grid[br][bc] = bm;
                            if (br == start_ep.r && bc == start_ep.c && bdir == start_ep.dir) break;
                            ParentInfo pi = parent_info[br][bc][bdir];
                            br = pi.r; bc = pi.c; bdir = pi.dir; bm = pi.mirror;
                        }
                        found = true;
                        break;
                    }
                } else {
                    
                    int weight;
                    if (grid[nr][nc] != '.') {
                        weight = 0;
                    } else {
                        weight = (m == '/') ? 10 : 11; 
                    }
                    
                    if (dist[r][c][dir] + weight < dist[nr][nc][ndir]) {
                        dist[nr][nc][ndir] = dist[r][c][dir] + weight;
                        parent_info[nr][nc][ndir] = {(short)r, (short)c, (char)dir, m};
                        pq.push({dist[nr][nc][ndir], nr, nc, ndir});
                    }
                }
            }
            if (found) break;
        }

        if (!found) {
            cout << -1 << "\n";
            return 0;
        }
    }

    for (int r = 0; r < x; ++r) {
        for (int c = 0; c < y; ++c) {
            if (grid[r][c] == '.') grid[r][c] = '/';
        }
        cout << grid[r] << "\n";
    }

    return 0;
}