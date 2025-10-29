#include <iostream>
#include <string>
#include <stack>
#include <cmath>
#include <cctype>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

bool isDigitChar(char c) {
    return (c >= '0' && c <= '9') || c == '.';
}

bool isIdentStart(char c) {
    return isalpha((unsigned char)c) || c == '_';
}

bool isIdentChar(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

double applyBinary(double a, double b, char op, bool& ok) {
    ok = true;
    if (op == '+') return a + b;
    if (op == '-') return a - b;
    if (op == '*') return a * b;
    if (op == '/') {
        if (b == 0) { ok = false; return 0; }
        return a / b;
    }
    if (op == '^') return pow(a, b);
    ok = false; return 0;
}

double applyFunction(const string& name, double x, bool& ok) {
    ok = true;
    if (name == "sin") return sin(x);
    if (name == "cos") return cos(x);
    if (name == "tan") return tan(x);
    if (name == "asin") return asin(x);
    if (name == "acos") return acos(x);
    if (name == "atan") return atan(x);
    if (name == "sqrt") {
        if (x < 0) { ok = false; return 0; }
        return sqrt(x);
    }
    if (name == "log") {
        if (x <= 0) { ok = false; return 0; }
        return log10(x);
    }
    if (name == "ln" || name == "loge") {
        if (x <= 0) { ok = false; return 0; }
        return log(x);
    }
    if (name == "exp") return exp(x);
    ok = false; return 0;
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

bool isOperatorChar(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool applyTopOperator(stack<double>& nums, stack<string>& ops, string& err) {
    if (ops.empty()) { err = "运算符栈空"; return false; }
    string top = ops.top(); ops.pop();
    if (top == "(") { err = "左括号错误"; return false; }
    if (isOperatorChar(top[0]) && top.size() == 1) {
        char op = top[0];
        if (nums.size() < 2) { err = "操作数不足"; return false; }
        double b = nums.top(); nums.pop();
        double a = nums.top(); nums.pop();
        bool ok;
        double r = applyBinary(a, b, op, ok);
        if (!ok) { err = (op == '/' ? "除以零" : "二元操作失败"); return false; }
        nums.push(r);
        return true;
    }
    else {
        string fname = top;
        if (nums.empty()) { err = "函数缺少参数"; return false; }
        double x = nums.top(); nums.pop();
        bool ok;
        double r = applyFunction(fname, x, ok);
        if (!ok) { err = "函数计算域错误或未知函数：" + fname; return false; }
        nums.push(r);
        return true;
    }
}

double strToDouble(const string& s) {
    try { return stod(s); }
    catch (...) { return 0; }
}

bool handleOperatorPush(stack<double>& nums, stack<string>& ops, const string& op, string& err) {
    if (op == "(") {
        ops.push(op);
        return true;
    }
    if (op == ")") {
        while (!ops.empty() && ops.top() != "(") {
            if (!applyTopOperator(nums, ops, err)) return false;
        }
        if (ops.empty()) { err = "括号不匹配"; return false; }
        ops.pop();
        if (!ops.empty()) {
            string maybeFunc = ops.top();
            if (maybeFunc != "(" && !isOperatorChar(maybeFunc[0])) {
                if (!applyTopOperator(nums, ops, err)) return false;
            }
        }
        return true;
    }
    char opc = op[0];
    while (!ops.empty()) {
        string top = ops.top();
        if (top == "(") break;
        if (!isOperatorChar(top[0]) || top.size() != 1) break;
        char topc = top[0];
        int ptop = precedence(topc);
        int pop = precedence(opc);
        if ((opc != '^' && ptop >= pop) || (opc == '^' && ptop > pop)) {
            if (!applyTopOperator(nums, ops, err)) return false;
        }
        else break;
    }
    ops.push(op);
    return true;
}

bool evaluateExpression(const string& expr, double& result, string& err) {
    stack<double> nums;
    stack<string> ops;
    map<string, double> constants;
    constants["pi"] = acos(-1.0);
    constants["e"] = exp(1.0);
    int n = expr.size();
    int i = 0;
    bool expectUnary = true;
    while (i < n) {
        char c = expr[i];
        if (isspace((unsigned char)c)) { i++; continue; }
        if (isDigitChar(c)) {
            int j = i;
            while (j < n && isDigitChar(expr[j])) j++;
            string numstr = expr.substr(i, j - i);
            double v = strToDouble(numstr);
            nums.push(v);
            i = j;
            expectUnary = false;
            continue;
        }
        if (isIdentStart(c)) {
            int j = i + 1;
            while (j < n && isIdentChar(expr[j])) j++;
            string name = expr.substr(i, j - i);
            string name_l = name;
            for (char& ch : name_l) ch = tolower((unsigned char)ch);
            if (constants.find(name_l) != constants.end()) {
                nums.push(constants[name_l]);
            }
            else {
                ops.push(name_l);
            }
            i = j;
            expectUnary = false;
            continue;
        }
        if (c == '(') {
            ops.push(string(1, '('));
            i++;
            expectUnary = true;
            continue;
        }
        if (c == ')') {
            if (!handleOperatorPush(nums, ops, string(1, ')'), err)) return false;
            i++;
            expectUnary = false;
            continue;
        }
        if (isOperatorChar(c)) {
            if (c == '-' && expectUnary) {
                nums.push(0);
            }
            string op(1, c);
            if (!handleOperatorPush(nums, ops, op, err)) return false;
            i++;
            expectUnary = true;
            continue;
        }
        err = string("不支持的字符：") + c;
        return false;
    }
    while (!ops.empty()) {
        if (ops.top() == "(" || ops.top() == ")") { err = "括号不匹配"; return false; }
        if (!applyTopOperator(nums, ops, err)) return false;
    }
    if (nums.size() != 1) { err = "表达式错误"; return false; }
    result = nums.top();
    return true;
}

int main() {
    cout.setf(std::ios::fixed);
    cout.precision(10);
    string line;
    cout << "请输入表达式，支持 + - * / ^ 括号，以及函数 sin cos tan log ln sqrt exp 和常量 pi e，输入 quit 退出。" << endl;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        if (line.empty()) continue;
        if (line == "quit" || line == "exit") break;
        double ans;
        string err;
        bool ok = evaluateExpression(line, ans, err);
        if (ok) cout << "计算结果是：" << ans << endl;
        else cout << "表达式无效：" << err << endl;
    }
    return 0;
}
