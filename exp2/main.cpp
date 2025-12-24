#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <cctype>
using namespace std;

// 位图类 Bitmap
class Bitmap {
private:
    unsigned char* M;  // 位图空间
    int N;             // 容量（字节数）
    int _sz;           // 有效位数

    void expand(int k) { // 扩容
        if (k < 8 * N) return; // 仍在界内，无需扩容

        int oldN = N;
        unsigned char* oldM = M;

        // 加倍策略扩容
        N = (k + 7) / 8 * 2;
        M = new unsigned char[N];
        memset(M, 0, N);
        memcpy(M, oldM, oldN);

        delete[] oldM;
    }

public:
    Bitmap(int n = 8) { // 构造函数
        N = (n + 7) / 8;
        M = new unsigned char[N];
        memset(M, 0, N);
        _sz = 0;
    }

    Bitmap(const Bitmap& other) { // 拷贝构造函数
        N = other.N;
        _sz = other._sz;
        M = new unsigned char[N];
        memcpy(M, other.M, N);
    }

    Bitmap& operator=(const Bitmap& other) { // 赋值运算符
        if (this != &other) {
            delete[] M;
            N = other.N;
            _sz = other._sz;
            M = new unsigned char[N];
            memcpy(M, other.M, N);
        }
        return *this;
    }

    ~Bitmap() { // 析构函数
        delete[] M;
    }

    int size() const { return _sz; }

    void set(int k) { // 设置第k位为1
        // 移除expand(k)的const问题
        if (k >= 8 * N) {
            expand(k);
        }
        if (!test(k)) _sz++;
        M[k >> 3] |= (0x80 >> (k & 0x07));
    }

    void clear(int k) { // 清除第k位
        if (k >= 8 * N) {
            expand(k);
        }
        if (test(k)) _sz--;
        M[k >> 3] &= ~(0x80 >> (k & 0x07));
    }

    bool test(int k) const { // 测试第k位
        // 直接实现测试逻辑，不调用expand
        if (k >= 8 * N) return false; // 超出范围返回false
        return (M[k >> 3] & (0x80 >> (k & 0x07))) != 0;
    }

    string toString() const { // 转换为字符串
        if (_sz == 0) return "";
        string s;
        for (int i = 0; i < _sz; i++) {
            s += test(i) ? '1' : '0';
        }
        return s;
    }

    void append(bool bit) { // 追加一位
        int pos = _sz;
        set(pos); // 先设置为1
        if (!bit) clear(pos); // 如果是0，再清除
    }
};

// Huffman编码类型
class HuffCode {
private:
    Bitmap code;

public:
    HuffCode() = default;

    void append(bool bit) {
        code.append(bit);
    }

    int size() const {
        return code.size();
    }

    string toString() const {
        return code.toString();
    }

    bool getBit(int pos) const {
        return code.test(pos);
    }
};

// 二叉树节点
struct BinNode {
    char data;
    int weight;
    BinNode* left;
    BinNode* right;

    BinNode(char d, int w) : data(d), weight(w), left(nullptr), right(nullptr) {}
};

// 二叉树类
class BinTree {
protected:
    BinNode* root;

    void clear(BinNode* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

public:
    BinTree() : root(nullptr) {}

    virtual ~BinTree() {
        clear(root);
    }

    BinNode* getRoot() const { return root; }

    void setRoot(BinNode* r) { root = r; }

    bool empty() const { return root == nullptr; }
};

// Huffman树节点（用于优先队列）
struct HuffmanNode {
    BinNode* node;
    int weight;

    HuffmanNode(BinNode* n, int w) : node(n), weight(w) {}

    // 重载<运算符用于优先队列（最小堆）
    bool operator<(const HuffmanNode& other) const {
        return weight > other.weight;
    }
};

// Huffman树类
class HuffTree : public BinTree {
private:
    map<char, HuffCode> codeTable;

    void buildCodeTable(BinNode* node, HuffCode code) {
        if (!node) return;

        // 叶子节点
        if (!node->left && !node->right) {
            codeTable[node->data] = code;
            return;
        }

        // 左子树编码追加0
        if (node->left) {
            HuffCode leftCode = code;
            leftCode.append(false);
            buildCodeTable(node->left, leftCode);
        }

        // 右子树编码追加1
        if (node->right) {
            HuffCode rightCode = code;
            rightCode.append(true);
            buildCodeTable(node->right, rightCode);
        }
    }

public:
    void build(const string& text) {
        // 统计字符频率
        map<char, int> freq;
        for (char c : text) {
            if (isalpha(c)) {
                c = tolower(c);
                freq[c]++;
            }
        }

        // 如果文本中没有字母，直接返回
        if (freq.empty()) {
            root = nullptr;
            return;
        }

        priority_queue<HuffmanNode> pq;

        for (auto& pair : freq) {
            BinNode* node = new BinNode(pair.first, pair.second);
            pq.push(HuffmanNode(node, pair.second));
        }

        // 构建Huffman树
        while (pq.size() > 1) {
            HuffmanNode node1 = pq.top();
            pq.pop();
            HuffmanNode node2 = pq.top();
            pq.pop();

            BinNode* newNode = new BinNode('\0', node1.weight + node2.weight);
            newNode->left = node1.node;
            newNode->right = node2.node;

            pq.push(HuffmanNode(newNode, newNode->weight));
        }

        if (!pq.empty()) {
            root = pq.top().node;
        }

        // 构建编码表
        HuffCode initialCode;
        buildCodeTable(root, initialCode);
    }

    string getCode(char c) {
        c = tolower(c);
        auto it = codeTable.find(c);
        if (it != codeTable.end()) {
            return it->second.toString();
        }
        return "";
    }

    string encode(const string& text) {
        string result;
        for (char c : text) {
            if (isalpha(c)) {
                string code = getCode(tolower(c));
                if (!code.empty()) {
                    result += code;
                }
            }
        }
        return result;
    }

    void displayCodeTable() {
        cout << "Huffman编码表：" << endl;
        vector<char> letters;
        for (char c = 'a'; c <= 'z'; c++) {
            letters.push_back(c);
        }

        // 按编码长度排序显示
        vector<pair<char, string>> codes;
        for (char c : letters) {
            string code = getCode(c);
            if (!code.empty()) {
                codes.push_back({ c, code });
            }
        }

        // 按编码长度排序
        sort(codes.begin(), codes.end(),
            [](const pair<char, string>& a, const pair<char, string>& b) {
                if (a.second.length() != b.second.length()) {
                    return a.second.length() < b.second.length();
                }
                return a.second < b.second;
            });

        for (auto& pair : codes) {
            cout << pair.first << ": " << pair.second << " (长度: " << pair.second.length() << ")" << endl;
        }
    }

    // 显示字符频率统计
    void displayFrequency(const string& text) {
        map<char, int> freq;
        for (char c : text) {
            if (isalpha(c)) {
                c = tolower(c);
                freq[c]++;
            }
        }

        cout << "\n字符频率统计：" << endl;
        int total = 0;
        for (auto& pair : freq) {
            total += pair.second;
        }

        vector<pair<char, int>> freqVec(freq.begin(), freq.end());
        sort(freqVec.begin(), freqVec.end(),
            [](const pair<char, int>& a, const pair<char, int>& b) {
                return a.second > b.second;
            });

        for (auto& pair : freqVec) {
            double percentage = (double)pair.second / total * 100;
            cout << pair.first << ": " << pair.second << "次 (" << percentage << "%)" << endl;
        }
    }
};

// 读取演讲文本
string readSpeechText() {
    return "I have a dream that one day this nation will rise up and live out the true meaning of its creed we hold these truths to be self evident that all men are created equal I have a dream that one day on the red hills of Georgia the sons of former slaves and the sons of former slave owners will be able to sit down together at the table of brotherhood I have a dream that one day even the state of Mississippi a state sweltering with the heat of injustice sweltering with the heat of oppression will be transformed into an oasis of freedom and justice I have a dream that my four little children will one day live in a nation where they will not be judged by the color of their skin but by the content of their character I have a dream today";
}

int main() {
    cout << "=== Huffman编码实验 ===" << endl;
    cout << "文本: 马丁路德金《I have a dream》" << endl;

    // 读取文本
    string text = readSpeechText();
    cout << "\n演讲文本长度: " << text.length() << " 字符" << endl;

    // 构建Huffman树
    HuffTree huffTree;
    huffTree.build(text);

    // 显示字符频率
    huffTree.displayFrequency(text);

    // 显示编码表
    cout << "\n";
    huffTree.displayCodeTable();

    // 编码测试单词
    vector<string> testWords = { "dream", "freedom", "justice", "equality", "brotherhood", "have", "nation" };

    cout << "\n=== 单词编码结果 ===" << endl;
    for (const string& word : testWords) {
        string encoded = huffTree.encode(word);
        if (!encoded.empty()) {
            cout << word << ": " << encoded << " (长度: " << encoded.length() << "位)" << endl;

            // 计算压缩比
            int originalBits = word.length() * 8; // ASCII需要8位/字符
            double compressionRatio = (1.0 - (double)encoded.length() / originalBits) * 100.0;
            cout << "  压缩率: " << compressionRatio << "% (原始: " << originalBits << "位)" << endl;
        }
    }

    // 测试整个短语
    string phrase = "i have a dream";
    string encodedPhrase = huffTree.encode(phrase);
    if (!encodedPhrase.empty()) {
        cout << "\n短语编码结果:" << endl;
        cout << "\"" << phrase << "\": " << encodedPhrase << endl;
        cout << "编码长度: " << encodedPhrase.length() << "位" << endl;

        int originalBits = phrase.length() * 8;
        double compressionRatio = (1.0 - (double)encodedPhrase.length() / originalBits) * 100.0;
        cout << "压缩率: " << compressionRatio << "%" << endl;
    }

    return 0;
}