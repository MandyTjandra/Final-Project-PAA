#pragma GCC optimize("Ofast,unroll-loops,omit-frame-pointer")

#include <cstdio>

static char in_buf[1 << 19];
static char *p = in_buf;

#define GET_INT(n) { \
    while (*p < '0') p++; \
    n = *p++ - '0'; \
    while (*p >= '0') n = n * 10 + (*p++ - '0'); \
}

static int top_arr[105], l_arr[105], r_arr[105], b_arr[105];
static int stk_id[420], stk_pos[420], stk_dir[420];

static char _pad1[64];
static unsigned char grid[16384];
static char _pad2[64];
static unsigned char used[16384];

static const int D_POS[] = {-128, 1, 128, -1};
static const int B_DIR[] = {2, 3, 0, 1};
static const int L_DIR[] = {3, 0, 1, 2};
static const int R_DIR[] = {1, 2, 3, 0};

int main() {
    int len = fread(in_buf, 1, sizeof(in_buf) - 1, stdin);
    in_buf[len] = 0;

    int X, Y;
    GET_INT(X);
    if (X <= 0) return 0;
    GET_INT(Y);

    register int i, j;
    for (i = 0; i < Y; ++i) GET_INT(top_arr[i]);
    for (i = 0; i < X; ++i) { GET_INT(l_arr[i]); GET_INT(r_arr[i]); }
    for (i = 0; i < Y; ++i) GET_INT(b_arr[i]);

    for (i = 0; i < 16384; ++i) { grid[i] = 255; used[i] = 0; }
    for (i = 1; i <= X; ++i) {
        int row = i << 7;
        for (j = 1; j <= Y; ++j) grid[row + j] = 0;
    }

    int stk_sz = 0;
    for (int side = 0; side < 4; ++side) {
        int cnt = (side & 1) ? X : Y;
        for (int k = 0; k < cnt; ++k) {
            int idx = (side > 1) ? (cnt - 1 - k) : k;
            int id, pos, dir;
            
            if (side == 0) { id = top_arr[idx]; pos = 128 + idx + 1; dir = 2; }
            else if (side == 1) { id = r_arr[idx]; pos = ((idx + 1) << 7) + Y; dir = 3; }
            else if (side == 2) { id = b_arr[idx]; pos = (X << 7) + idx + 1; dir = 0; }
            else { id = l_arr[idx]; pos = ((idx + 1) << 7) + 1; dir = 1; }

            if (stk_sz > 0 && stk_id[stk_sz - 1] == id) {
                stk_sz--;
                register int cp = stk_pos[stk_sz];
                register int cd = stk_dir[stk_sz];
                int tp = pos;
                int td = B_DIR[dir];

                while (1) {
                    register unsigned char cell = grid[cp];
                    if (!cell) {
                        int ld = L_DIR[cd];
                        if ((cp == tp && ld == td) || (grid[cp + D_POS[ld]] != 255 && !(used[cp] & (1 << ld)))) {
                            grid[cp] = (cd & 1) ? 1 : 3;
                            cd = ld;
                        } else {
                            int rd = R_DIR[cd];
                            if ((cp == tp && rd == td) || (grid[cp + D_POS[rd]] != 255 && !(used[cp] & (1 << rd)))) {
                                grid[cp] = (cd & 1) ? 3 : 1;
                                cd = rd;
                            } else { puts("-1"); return 0; }
                        }
                    } else {
                        cd ^= cell;
                    }
                    
                    used[cp] |= (1 << cd);
                    if (cp == tp && cd == td) break;
                    cp += D_POS[cd];
                    if (grid[cp] == 255) { puts("-1"); return 0; }
                    used[cp] |= (1 << B_DIR[cd]);
                }
            } else {
                stk_id[stk_sz] = id; stk_pos[stk_sz] = pos; stk_dir[stk_sz] = dir;
                stk_sz++;
            }
        }
    }

    if (stk_sz) { puts("-1"); return 0; }

    static char out[16384];
    char *o = out;
    for (i = 1; i <= X; ++i) {
        int r = i << 7;
        for (j = 1; j <= Y; ++j) {
            *o++ = (grid[r + j] == 3) ? '\\' : '/';
        }
        *o++ = '\n';
    }
    fwrite(out, 1, o - out, stdout);

    return 0;
}
