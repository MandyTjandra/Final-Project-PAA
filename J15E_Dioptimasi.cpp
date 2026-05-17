#include <cstdio>
#include <cstring>

typedef unsigned short u16;
typedef unsigned int u32;

char in_buf[1 << 17];
int in_pos = 0;
int in_len = 0;

inline void init_io() {
    in_len = fread(in_buf, 1, sizeof(in_buf) - 1, stdin);
    in_buf[in_len] = '\0';
}

inline void read_int(int &val) {
    while (in_pos < in_len && in_buf[in_pos] < '0') in_pos++;
    if (in_pos >= in_len) { val = -1; return; }
    val = 0;
    while (in_pos < in_len && in_buf[in_pos] >= '0') {
        val = val * 10 + (in_buf[in_pos++] - '0');
    }
}

u16 dist_arr[65536];
u16 vis_id[65536];
u16 parent_state[65536];
char parent_mirror[65536];
u16 current_route_id = 0;

u32 dq[262144]; 
char grid[16384];
static const int DELTA[] = {-128, 1, 128, -1};

struct Endpoint { 
    int type, idx, id, r, c, dir; 
    Endpoint() {}
    Endpoint(int _t, int _i, int _id, int _r, int _c, int _d) 
        : type(_t), idx(_i), id(_id), r(_r), c(_c), dir(_d) {}
};

struct PairEP { 
    Endpoint s, t; 
    PairEP() {}
    PairEP(Endpoint _s, Endpoint _t) : s(_s), t(_t) {}
};

int top_arr[105], bottom_arr[105], left_arr[105], right_arr[105];
Endpoint perim[420];
Endpoint stk[420];
PairEP pairs[420];
int X, Y;

#define PROCESS(ndir, m_char) do { \
    int npos = pos + DELTA[ndir]; \
    char ncell = grid[npos]; \
    if (ncell == 'T') { \
        u16 bstate = state; \
        char bm = m_char; \
        while (bstate != start_state) { \
            int bpos = bstate >> 2; \
            if (grid[bpos] == '.') grid[bpos] = bm; \
            bm = parent_mirror[bstate]; \
            bstate = parent_state[bstate]; \
        } \
        if (grid[bstate >> 2] == '.') grid[bstate >> 2] = bm; \
        found = true; \
    } else if (ncell != '#') { \
        u16 nstate = (npos << 2) | ndir; \
        int w = (ncell == '.') ? 1 : 0; \
        int new_dist = cdist + w; \
        if (vis_id[nstate] != current_route_id || new_dist < dist_arr[nstate]) { \
            vis_id[nstate] = current_route_id; \
            dist_arr[nstate] = new_dist; \
            parent_state[nstate] = state; \
            parent_mirror[nstate] = m_char; \
            u32 n_item = (new_dist << 16) | nstate; \
            if (w == 0) { \
                dq_head = (dq_head - 1) & 262143; \
                dq[dq_head] = n_item; \
            } else { \
                dq[dq_tail] = n_item; \
                dq_tail = (dq_tail + 1) & 262143; \
            } \
        } \
    } \
} while(0)

int main() {
    init_io();
    read_int(X); 
    if (X <= 0) return 0;
    read_int(Y);

    if (X > 100 || Y > 100) { puts("-1"); return 0; }

    for (int i = 0; i < Y; ++i) read_int(top_arr[i]);
    for (int i = 0; i < X; ++i) { read_int(left_arr[i]); read_int(right_arr[i]); }
    for (int i = 0; i < Y; ++i) read_int(bottom_arr[i]);

    for (int c = 0; c <= Y + 1; ++c) { grid[c] = '#'; grid[((X + 1) << 7) | c] = '#'; }
    for (int r = 1; r <= X; ++r) {
        int row_base = r << 7;
        grid[row_base] = '#';
        for (int c = 1; c <= Y; ++c) grid[row_base | c] = '.';
        grid[row_base | (Y + 1)] = '#';
    }

    int perim_sz = 0;
    for (int i = 0; i < Y; ++i) perim[perim_sz++] = Endpoint(0, i, top_arr[i], 1, i+1, 2);
    for (int i = 0; i < X; ++i) perim[perim_sz++] = Endpoint(1, i, right_arr[i], i+1, Y, 3);
    for (int i = Y-1; i >= 0; --i) perim[perim_sz++] = Endpoint(2, i, bottom_arr[i], X, i+1, 0);
    for (int i = X-1; i >= 0; --i) perim[perim_sz++] = Endpoint(3, i, left_arr[i], i+1, 1, 1);

    int stk_sz = 0, pairs_sz = 0;
    for (int i = 0; i < perim_sz; ++i) {
        if (stk_sz > 0 && stk[stk_sz - 1].id == perim[i].id) {
            pairs[pairs_sz++] = PairEP(stk[stk_sz - 1], perim[i]);
            stk_sz--;
        } else {
            stk[stk_sz++] = perim[i];
        }
    }
    
    if (stk_sz > 0) { puts("-1"); return 0; }

    for (int p = 0; p < pairs_sz; ++p) {
        const Endpoint& start_ep = pairs[p].s;
        const Endpoint& target_ep = pairs[p].t;
        
        if (++current_route_id >= 60000) {
            current_route_id = 1;
            for(int i = 0; i < 65536; ++i) vis_id[i] = 0;
        }

        int target_r = -1, target_c = -1;
        if (target_ep.type == 0) { target_r = 0; target_c = target_ep.idx + 1; }
        else if (target_ep.type == 1) { target_r = target_ep.idx + 1; target_c = Y + 1; }
        else if (target_ep.type == 2) { target_r = X + 1; target_c = target_ep.idx + 1; }
        else if (target_ep.type == 3) { target_r = target_ep.idx + 1; target_c = 0; }

        int target_pos = (target_r << 7) | target_c;
        grid[target_pos] = 'T';

        u32 dq_head = 0;
        u32 dq_tail = 0;

        u16 start_state = (((start_ep.r << 7) | start_ep.c) << 2) | start_ep.dir;
        
        vis_id[start_state] = current_route_id;
        dist_arr[start_state] = 0;
        
        dq[dq_tail] = (0 << 16) | start_state;
        dq_tail = (dq_tail + 1) & 262143;

        bool found = false;

        while (dq_head != dq_tail) {
            u32 item = dq[dq_head];
            dq_head = (dq_head + 1) & 262143;
            
            int cdist = item >> 16;
            u16 state = item & 0xFFFF;

            if (cdist > dist_arr[state]) continue;

            int pos = state >> 2;
            int dir = state & 3;
            char cell = grid[pos];

            if (cell != '.') {
                int ndir = dir ^ ((cell == '/') ? 1 : 3);
                PROCESS(ndir, cell);
                if (found) break;
            } else {
                PROCESS(dir ^ 1, '/');
                if (found) break;
                PROCESS(dir ^ 3, '\\');
                if (found) break;
            }
        }

        grid[target_pos] = '#';
        if (!found) { puts("-1"); return 0; }
    }

    char out_buf[16384];
    char * out_ptr = out_buf;
    
    for (int r = 1; r <= X; ++r) {
        int row_base = r << 7;
        for (int c = 1; c <= Y; ++c) {
            char cell = grid[row_base | c];
            *out_ptr++ = (cell == '.') ? '/' : cell;
        }
        *out_ptr++ = '\n';
    }
    fwrite(out_buf, 1, out_ptr - out_buf, stdout);
    
    return 0;
}