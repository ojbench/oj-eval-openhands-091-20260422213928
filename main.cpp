#include <bits/stdc++.h>
using namespace std;

/*
 We don't have the original ESet implementation here. The README suggests speedtest with operations:
 0 a b : insert b into set a
 1 a b : erase b from set a
 2 a   : copy set a to a new set with index ++lst
 3 a b : find b in set a -> print true/false and set iterator to that element if found
 4 a l r : print count of elements in [l, r] in set a
 5     : print previous element of current iterator if valid else -1
 6     : print next element of current iterator if valid else -1

 We'll implement functionality using std::set<long long>. Keep memory efficient.
 We will maintain a vector< set<long long> > s. Indexes used by inputs are non-negative and small; op 2 creates new copy
 appended to the vector. We'll start with some initial capacity, and grow as needed.

 We'll also track an iterator to the last found element along with which set it belongs to and a validity flag.
 The tricky part is semantics of op 5 and 6 according to the provided sample tester in ESet repo:
 - op 5: if valid, attempt to move to previous element (predecessor). If currently at begin, becomes invalid and output -1. If valid after moving, output the value.
 - op 6: if valid, attempt to move to next element (successor). If currently at last element, becomes invalid and output -1; otherwise move and output value. If not valid initially, output -1.
 The sample tester had some peculiar code to detect border using two-iterator comparisons; here we can do directly with std::set ops.
*/

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector< set<long long> > s(1); // s[0] exists
    int lst = 0; // highest index
    bool valid = false;
    int it_a = -1; // which set
    set<long long>::iterator it; // current iterator

    int op;
    while ( (cin >> op) ){
        long long a,b,c;
        switch(op){
            case 0: { // insert
                cin >> a >> b;
                if (a >= (long long)s.size()) s.resize(a+1);
                auto res = s[a].insert(b);
                if (res.second) {
                    it_a = (int)a;
                    it = res.first;
                    valid = true;
                }
                break;
            }
            case 1: { // erase
                cin >> a >> b;
                if (a < (long long)s.size()){
                    if (valid && it_a == (int)a && s[a].find(b) != s[a].end() && *it == b){
                        // if iterator points to the element that's being erased, invalidate
                        valid = false;
                    }
                    s[a].erase(b);
                }
                break;
            }
            case 2: { // copy
                cin >> a;
                if (a >= (long long)s.size()) s.resize(a+1);
                s.push_back(s[a]);
                lst++;
                break;
            }
            case 3: { // find
                cin >> a >> b;
                bool found = false;
                if (a < (long long)s.size()){
                    auto it2 = s[a].find(b);
                    if (it2 != s[a].end()){
                        found = true;
                        it_a = (int)a;
                        it = it2;
                        valid = true;
                    }
                }
                cout << (found?"true":"false") << '\n';
                break;
            }
            case 4: { // range count [b, c]
                cin >> a >> b >> c;
                long long ans = 0;
                if (a < (long long)s.size()){
                    auto &st = s[a];
                    auto L = st.lower_bound(b);
                    auto R = st.upper_bound(c);
                    ans = distance(L, R);
                }
                cout << ans << '\n';
                break;
            }
            case 5: { // prev
                if (valid && it_a < (int)s.size()){
                    auto &st = s[it_a];
                    if (it == st.begin()){
                        valid = false;
                    } else {
                        --it;
                        cout << *it << '\n';
                        break;
                    }
                }
                cout << -1 << '\n';
                break;
            }
            case 6: { // next
                if (valid && it_a < (int)s.size()){
                    auto &st = s[it_a];
                    auto nxt = it; ++nxt;
                    if (nxt == st.end()){
                        valid = false;
                    } else {
                        it = nxt;
                        cout << *it << '\n';
                        break;
                    }
                }
                cout << -1 << '\n';
                break;
            }
            default:
                // ignore unknown ops
                break;
        }
    }
    return 0;
}
