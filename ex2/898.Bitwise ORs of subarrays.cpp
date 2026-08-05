class Solution {
public:
    int subarrayBitwiseORs(vector<int>& arr) {
        unordered_set<int> res;
        unordered_set<int> cur;
        for (int x : arr) {
            unordered_set<int> next_cur = {x};
            for (int y : cur) {
                next_cur.insert(y | x);
            }
            cur = move(next_cur);
            for (int y : cur) {
                res.insert(y);
            }
        }
        return res.size();
    }
};
