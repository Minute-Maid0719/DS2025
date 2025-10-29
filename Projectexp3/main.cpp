#include <iostream>
#include <vector>
#include <stack>

using namespace std;

long long getMaxArea(const vector<int>& h) {
    int n = h.size();
    stack<int> s;
    long long maxArea = 0;
    for (int i = 0; i <= n; i++) {
        long long now = (i == n ? 0 : h[i]);
        while (!s.empty() && now < h[s.top()]) {
            int top = s.top();
            s.pop();
            long long height = h[top];
            int left = s.empty() ? -1 : s.top();
            long long width = i - left - 1;
            long long area = height * width;
            if (area > maxArea) maxArea = area;
        }
        s.push(i);
    }
    return maxArea;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cout << "请输入测试组数: ";
    if (!(cin >> T)) return 0;
    for (int t = 1; t <= T; t++) {
        int n;
        cout << "第" << t << "组, 请输入柱子数量 n: ";
        cin >> n;
        if (n <= 0) {
            cout << "n 必须大于0，跳过本组\n";
            continue;
        }
        vector<int> h;
        h.reserve(n);
        cout << "请输入 " << n << " 个高度（用空格或回车分隔）: ";
        for (int i = 0; i < n; i++) {
            int x;
            cin >> x;
            if (x < 0) x = 0;
            if (x > 10000) x = 10000;
            h.push_back(x);
        }
        cout << "输入的高度数组为: [";
        for (int i = 0; i < n; i++) {
            cout << h[i];
            if (i != n - 1) cout << ", ";
        }
        cout << "]\n";
        long long ans = getMaxArea(h);
        cout << "最大矩形面积=" << ans << "\n";
        cout << "-----------------------\n";
    }
    return 0;
}
