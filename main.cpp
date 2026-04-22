#include <bits/stdc++.h>
using namespace std;

struct Node {
    long long k;
    int l, r;
    int sz;
    uint32_t pr;
    Node() : k(0), l(0), r(0), sz(1), pr(0) {}
    Node(long long _k, uint32_t _pr) : k(_k), l(0), r(0), sz(1), pr(_pr) {}
};

static inline int getsz(const vector<Node>& t, int x){ return x? t[x].sz : 0; }
static inline void pull(vector<Node>& t, int x){ if(x) t[x].sz = 1 + getsz(t, t[x].l) + getsz(t, t[x].r); }

static thread_local uint64_t seed64 = 88172645463393265ull;
static inline uint32_t rng32(){ seed64 ^= seed64 << 7; seed64 ^= seed64 >> 9; seed64 ^= seed64 << 8; return (uint32_t)seed64; }

static vector<Node> pool;
static inline int clone_node(int x){
    pool.push_back(pool[x]);
    return (int)pool.size()-1;
}
static inline int make_node(long long k){
    pool.emplace_back(k, rng32());
    return (int)pool.size()-1;
}

static int merge(int a, int b){
    if(!a) return b; if(!b) return a;
    if(pool[a].pr < pool[b].pr){
        int na = clone_node(a);
        pool[na].r = merge(pool[a].r, b);
        pull(pool, na);
        return na;
    }else{
        int nb = clone_node(b);
        pool[nb].l = merge(a, pool[b].l);
        pull(pool, nb);
        return nb;
    }
}

static pair<int,int> split_le(int t, long long key){
    if(!t) return {0,0};
    if(pool[t].k <= key){
        int nt = clone_node(t);
        auto pr = split_le(pool[t].r, key);
        pool[nt].r = pr.first;
        pull(pool, nt);
        return {nt, pr.second};
    }else{
        int nt = clone_node(t);
        auto pr = split_le(pool[t].l, key);
        pool[nt].l = pr.second;
        pull(pool, nt);
        return {pr.first, nt};
    }
}

static bool contains(int t, long long key){
    while(t){
        if(key < pool[t].k) t = pool[t].l;
        else if(key > pool[t].k) t = pool[t].r;
        else return true;
    }
    return false;
}

static int insert_key(int t, long long key, bool &inserted){
    if(contains(t, key)) { inserted = false; return t; }
    inserted = true;
    int n = make_node(key);
    auto pr = split_le(t, key);
    return merge( merge(pr.first, n), pr.second );
}

static int erase_key(int t, long long key, bool &erased){
    auto p1 = split_le(t, key-1);
    auto p2 = split_le(p1.second, key); // p2.first == key
    if(p2.first){
        erased = true;
        return merge(p1.first, p2.second);
    }else{
        erased = false;
        return merge(p1.first, p2.second);
    }
}

static int cnt_le(int t, long long key){
    int ans = 0;
    while(t){
        if(key < pool[t].k){
            t = pool[t].l;
        }else{
            ans += 1 + getsz(pool, pool[t].l);
            t = pool[t].r;
        }
    }
    return ans;
}

static bool predecessor(int t, long long key, long long &res){
    bool ok = false; long long best = 0;
    while(t){
        if(key <= pool[t].k){
            t = pool[t].l;
        }else{
            ok = true; best = pool[t].k; t = pool[t].r;
        }
    }
    if(ok) res = best; return ok;
}

static bool successor(int t, long long key, long long &res){
    bool ok = false; long long best = 0;
    while(t){
        if(key >= pool[t].k){
            t = pool[t].r;
        }else{
            ok = true; best = pool[t].k; t = pool[t].l;
        }
    }
    if(ok) res = best; return ok;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<int> roots(1, 0);
    bool valid = false;
    int it_a = -1;
    long long it_key = 0;

    int op;
    while ( (cin >> op) ){
        long long a,b,c;
        switch(op){
            case 0: { // insert
                cin >> a >> b;
                if (a >= (long long)roots.size()) roots.resize(a+1, 0);
                bool inserted;
                int nr = insert_key(roots[(size_t)a], b, inserted);
                roots[(size_t)a] = nr;
                if(inserted){ it_a = (int)a; it_key = b; valid = true; }
                break;
            }
            case 1: { // erase
                cin >> a >> b;
                if (a < (long long)roots.size()){
                    if (valid && it_a == (int)a && it_key == b) valid = false;
                    bool erased;
                    roots[(size_t)a] = erase_key(roots[(size_t)a], b, erased);
                }
                break;
            }
            case 2: { // copy
                cin >> a;
                if (a >= (long long)roots.size()) roots.resize(a+1, 0);
                roots.push_back(roots[(size_t)a]);
                break;
            }
            case 3: { // find
                cin >> a >> b;
                bool found = false;
                if (a < (long long)roots.size()){
                    found = contains(roots[(size_t)a], b);
                    if(found){ it_a = (int)a; it_key = b; valid = true; }
                }
                cout << (found?"true":"false") << '\n';
                break;
            }
            case 4: { // range count [b, c]
                cin >> a >> b >> c;
                long long ans = 0;
                if (a < (long long)roots.size()){
                    int right = cnt_le(roots[(size_t)a], c);
                    int left = cnt_le(roots[(size_t)a], b-1);
                    ans = (long long)right - (long long)left;
                }
                cout << ans << '\n';
                break;
            }
            case 5: { // prev
                if (valid && it_a < (int)roots.size()){
                    long long res;
                    if(predecessor(roots[(size_t)it_a], it_key, res)){
                        it_key = res;
                        cout << it_key << '\n';
                        break;
                    }else{
                        valid = false;
                    }
                }
                cout << -1 << '\n';
                break;
            }
            case 6: { // next
                if (valid && it_a < (int)roots.size()){
                    long long res;
                    if(successor(roots[(size_t)it_a], it_key, res)){
                        it_key = res;
                        cout << it_key << '\n';
                        break;
                    }else{
                        valid = false;
                    }
                }
                cout << -1 << '\n';
                break;
            }
            default:
                break;
        }
    }
    return 0;
}
