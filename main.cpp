#include <bits/stdc++.h>
using namespace std;

struct Node {
    long long k;
    uint32_t pr;
    int sz;
    Node *l, *r;
    Node(long long _k, uint32_t _pr) : k(_k), pr(_pr), sz(1), l(nullptr), r(nullptr) {}
};

static inline int getsz(Node* t){ return t? t->sz : 0; }
static inline void pull(Node* t){ if(t) t->sz = 1 + getsz(t->l) + getsz(t->r); }

static thread_local uint64_t seed64 = 88172645463393265ull;
static inline uint32_t rng32(){
    seed64 ^= seed64 << 7; seed64 ^= seed64 >> 9; seed64 ^= seed64 << 8; return (uint32_t)seed64;
}

// Persistent merge: returns new root
static Node* merge(Node* a, Node* b){
    if(!a) return b; if(!b) return a;
    if(a->pr < b->pr){
        Node* na = new Node(*a);
        na->r = merge(a->r, b);
        pull(na);
        return na;
    }else{
        Node* nb = new Node(*b);
        nb->l = merge(a, b->l);
        pull(nb);
        return nb;
    }
}

// split by key: <= key goes to a, > key to b
static pair<Node*,Node*> split_le(Node* t, long long key){
    if(!t) return {nullptr,nullptr};
    if(t->k <= key){
        Node* nt = new Node(*t);
        auto pr = split_le(t->r, key);
        nt->r = pr.first;
        pull(nt);
        return {nt, pr.second};
    }else{
        Node* nt = new Node(*t);
        auto pr = split_le(t->l, key);
        nt->l = pr.second;
        pull(nt);
        return {pr.first, nt};
    }
}

static bool contains(Node* t, long long key){
    while(t){
        if(key < t->k) t = t->l;
        else if(key > t->k) t = t->r;
        else return true;
    }
    return false;
}

static Node* insert_key(Node* t, long long key, bool &inserted){
    if(contains(t, key)) { inserted = false; return t; }
    inserted = true;
    Node* n = new Node(key, rng32());
    auto pr = split_le(t, key);
    return merge( merge(pr.first, n), pr.second );
}

static Node* erase_key(Node* t, long long key, bool &erased){
    // split into <key, >=key
    auto p1 = split_le(t, key-1);
    auto p2 = split_le(p1.second, key); // p2.first are ==key
    if(p2.first){
        erased = true;
        // drop p2.first
        return merge(p1.first, p2.second);
    }else{
        erased = false;
        return merge(p1.first, p2.second);
    }
}

static int cnt_le(Node* t, long long key){
    int ans = 0;
    while(t){
        if(key < t->k){
            t = t->l;
        }else{
            ans += 1 + getsz(t->l);
            t = t->r;
        }
    }
    return ans;
}

static bool predecessor(Node* t, long long key, long long &res){
    bool ok = false; long long best = 0;
    while(t){
        if(key <= t->k){
            t = t->l;
        }else{
            ok = true; best = t->k; t = t->r;
        }
    }
    if(ok) res = best; return ok;
}

static bool successor(Node* t, long long key, long long &res){
    bool ok = false; long long best = 0;
    while(t){
        if(key >= t->k){
            t = t->r;
        }else{
            ok = true; best = t->k; t = t->l;
        }
    }
    if(ok) res = best; return ok;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<Node*> roots(1, nullptr);
    int lst = 0;
    bool valid = false;
    int it_a = -1;
    long long it_key = 0;

    int op;
    while ( (cin >> op) ){
        long long a,b,c;
        switch(op){
            case 0: { // insert
                cin >> a >> b;
                if (a >= (long long)roots.size()) roots.resize(a+1, nullptr);
                bool inserted;
                Node* nr = insert_key(roots[a], b, inserted);
                roots[a] = nr;
                if(inserted){
                    it_a = (int)a; it_key = b; valid = true;
                }
                break;
            }
            case 1: { // erase
                cin >> a >> b;
                if (a < (long long)roots.size()){
                    if (valid && it_a == (int)a && it_key == b) valid = false;
                    bool erased;
                    roots[a] = erase_key(roots[a], b, erased);
                }
                break;
            }
            case 2: { // copy
                cin >> a;
                if (a >= (long long)roots.size()) roots.resize(a+1, nullptr);
                roots.push_back(roots[a]);
                lst++;
                break;
            }
            case 3: { // find
                cin >> a >> b;
                bool found = false;
                if (a < (long long)roots.size()){
                    found = contains(roots[a], b);
                    if(found){ it_a = (int)a; it_key = b; valid = true; }
                }
                cout << (found?"true":"false") << '\n';
                break;
            }
            case 4: { // range count [b, c]
                cin >> a >> b >> c;
                long long ans = 0;
                if (a < (long long)roots.size()){
                    int right = cnt_le(roots[a], c);
                    int left = cnt_le(roots[a], b-1);
                    ans = (long long)right - (long long)left;
                }
                cout << ans << '\n';
                break;
            }
            case 5: { // prev
                if (valid && it_a < (int)roots.size()){
                    long long res;
                    if(predecessor(roots[it_a], it_key, res)){
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
                    if(successor(roots[it_a], it_key, res)){
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
