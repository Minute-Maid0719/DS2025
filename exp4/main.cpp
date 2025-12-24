#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;

// ======================= 边界框结构 =======================
struct Box {
    float x1, y1, x2, y2;
    float score;
};

// ======================= IoU 计算 =======================
float IoU(const Box& a, const Box& b) {
    float xx1 = max(a.x1, b.x1);
    float yy1 = max(a.y1, b.y1);
    float xx2 = min(a.x2, b.x2);
    float yy2 = min(a.y2, b.y2);

    float w = max(0.0f, xx2 - xx1);
    float h = max(0.0f, yy2 - yy1);

    float inter = w * h;
    float areaA = (a.x2 - a.x1) * (a.y2 - a.y1);
    float areaB = (b.x2 - b.x1) * (b.y2 - b.y1);

    return inter / (areaA + areaB - inter);
}

// ======================= NMS =======================
vector<Box> NMS(vector<Box>& boxes, float threshold) {
    vector<Box> result;
    vector<bool> suppressed(boxes.size(), false);

    for (size_t i = 0; i < boxes.size(); ++i) {
        if (suppressed[i]) continue;
        result.push_back(boxes[i]);

        for (size_t j = i + 1; j < boxes.size(); ++j) {
            if (IoU(boxes[i], boxes[j]) > threshold) {
                suppressed[j] = true;
            }
        }
    }
    return result;
}

// ======================= 排序算法 =======================

// 1. 冒泡排序
void bubbleSort(vector<Box>& a) {
    for (size_t i = 0; i < a.size(); ++i) {
        for (size_t j = 0; j + 1 < a.size() - i; ++j) {
            if (a[j].score < a[j + 1].score)
                swap(a[j], a[j + 1]);
        }
    }
}

// 2. 插入排序
void insertionSort(vector<Box>& a) {
    for (size_t i = 1; i < a.size(); ++i) {
        Box key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j].score < key.score) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

// 3. 归并排序
void merge(vector<Box>& a, int l, int m, int r) {
    vector<Box> temp;
    int i = l, j = m + 1;

    while (i <= m && j <= r) {
        if (a[i].score > a[j].score)
            temp.push_back(a[i++]);
        else
            temp.push_back(a[j++]);
    }
    while (i <= m) temp.push_back(a[i++]);
    while (j <= r) temp.push_back(a[j++]);

    for (int k = 0; k < temp.size(); ++k)
        a[l + k] = temp[k];
}

void mergeSort(vector<Box>& a, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSort(a, l, m);
    mergeSort(a, m + 1, r);
    merge(a, l, m, r);
}

// 4. 快速排序
int partition(vector<Box>& a, int l, int r) {
    float pivot = a[r].score;
    int i = l;
    for (int j = l; j < r; ++j) {
        if (a[j].score > pivot) {
            swap(a[i], a[j]);
            i++;
        }
    }
    swap(a[i], a[r]);
    return i;
}

void quickSort(vector<Box>& a, int l, int r) {
    if (l < r) {
        int p = partition(a, l, r);
        quickSort(a, l, p - 1);
        quickSort(a, p + 1, r);
    }
}

// ======================= 数据生成 =======================

// 随机分布
vector<Box> randomBoxes(int n) {
    vector<Box> boxes;
    for (int i = 0; i < n; ++i) {
        float x = rand() % 500;
        float y = rand() % 500;
        float w = 20 + rand() % 100;
        float h = 20 + rand() % 100;
        float s = (float)rand() / RAND_MAX;
        boxes.push_back({ x, y, x + w, y + h, s });
    }
    return boxes;
}

// 聚集分布
vector<Box> clusteredBoxes(int n) {
    vector<Box> boxes;
    float cx = 250, cy = 250;

    for (int i = 0; i < n; ++i) {
        float x = cx + rand() % 50;
        float y = cy + rand() % 50;
        float w = 30 + rand() % 50;
        float h = 30 + rand() % 50;
        float s = (float)rand() / RAND_MAX;
        boxes.push_back({ x, y, x + w, y + h, s });
    }
    return boxes;
}

// ======================= 性能测试 =======================
void testSort(const string& name, vector<Box> boxes, int type) {
    auto start = chrono::high_resolution_clock::now();

    if (type == 1) bubbleSort(boxes);
    else if (type == 2) insertionSort(boxes);
    else if (type == 3) mergeSort(boxes, 0, boxes.size() - 1);
    else if (type == 4) quickSort(boxes, 0, boxes.size() - 1);

    auto afterSort = chrono::high_resolution_clock::now();
    NMS(boxes, 0.5f);
    auto end = chrono::high_resolution_clock::now();

    double sortTime = chrono::duration<double, milli>(afterSort - start).count();
    double totalTime = chrono::duration<double, milli>(end - start).count();

    cout << name
        << " | 排序(ms): " << sortTime
        << " | 总时间(ms): " << totalTime << endl;
}

// ======================= 主函数 =======================
int main() {
    srand((unsigned int)time(nullptr));
    int sizes[] = { 100, 500, 1000, 5000, 10000 };

    for (int n : sizes) {
        cout << "\n==============================" << endl;
        cout << "数据规模: " << n << endl;

        cout << "\n--- 随机分布 ---" << endl;
        auto randomData = randomBoxes(n);
        testSort("Bubble", randomData, 1);
        testSort("Insertion", randomData, 2);
        testSort("Merge", randomData, 3);
        testSort("Quick", randomData, 4);

        cout << "\n--- 聚集分布 ---" << endl;
        auto clusterData = clusteredBoxes(n);
        testSort("Bubble", clusterData, 1);
        testSort("Insertion", clusterData, 2);
        testSort("Merge", clusterData, 3);
        testSort("Quick", clusterData, 4);
    }

    return 0;
}
