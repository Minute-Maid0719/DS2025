#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
using namespace std;

class Complex {
public:
    double real;
    double imag;

    Complex(double r = 0, double i = 0) {
        real = r;
        imag = i;
    }

    double magnitude() const {
        return sqrt(real * real + imag * imag);
    }

    bool operator==(const Complex& c) const {
        return (real == c.real && imag == c.imag);
    }

    void print() const {
        cout << "(" << real << "," << imag << ")";
    }
};

void bubbleSort(vector<Complex>& v) {
    int n = v.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (v[j].magnitude() > v[j + 1].magnitude() ||
                (v[j].magnitude() == v[j + 1].magnitude() && v[j].real > v[j + 1].real)) {
                swap(v[j], v[j + 1]);
            }
        }
    }
}

void merge(vector<Complex>& v, int left, int mid, int right) {
    int i = left, j = mid + 1;
    vector<Complex> temp;
    while (i <= mid && j <= right) {
        double mag1 = v[i].magnitude();
        double mag2 = v[j].magnitude();
        if (mag1 < mag2 || (mag1 == mag2 && v[i].real < v[j].real)) {
            temp.push_back(v[i]);
            i++;
        }
        else {
            temp.push_back(v[j]);
            j++;
        }
    }
    while (i <= mid) temp.push_back(v[i++]);
    while (j <= right) temp.push_back(v[j++]);
    for (int k = 0; k < temp.size(); k++) {
        v[left + k] = temp[k];
    }
}

void mergeSort(vector<Complex>& v, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    mergeSort(v, left, mid);
    mergeSort(v, mid + 1, right);
    merge(v, left, mid, right);
}

int findComplex(const vector<Complex>& v, const Complex& target) {
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == target) return i;
    }
    return -1;
}

void deleteComplex(vector<Complex>& v, const Complex& target) {
    for (auto it = v.begin(); it != v.end(); it++) {
        if (*it == target) {
            v.erase(it);
            break;
        }
    }
}

void uniqueVector(vector<Complex>& v) {
    sort(v.begin(), v.end(), [](Complex a, Complex b) {
        if (a.real == b.real) return a.imag < b.imag;
        return a.real < b.real;
        });
    v.erase(unique(v.begin(), v.end()), v.end());
}

vector<Complex> rangeSearch(const vector<Complex>& v, double m1, double m2) {
    vector<Complex> result;
    for (auto& c : v) {
        double m = c.magnitude();
        if (m >= m1 && m < m2) {
            result.push_back(c);
        }
    }
    return result;
}

int main() {
    srand(time(0));
    vector<Complex> v;
    for (int i = 0; i < 10; i++) {
        double r = rand() % 10;
        double im = rand() % 10;
        v.push_back(Complex(r, im));
    }

    cout << "初始复数向量:" << endl;
    for (auto& c : v) c.print(), cout << " ";
    cout << endl;

    random_shuffle(v.begin(), v.end());
    cout << "\n置乱后的向量:" << endl;
    for (auto& c : v) c.print(), cout << " ";
    cout << endl;

    Complex target(5, 5);
    int idx = findComplex(v, target);
    if (idx != -1)
        cout << "\n找到复数(5,5)，下标为：" << idx << endl;
    else
        cout << "\n未找到复数(5,5)" << endl;

    v.push_back(Complex(8, 2));
    cout << "\n插入(8,2)后的向量:" << endl;
    for (auto& c : v) c.print(), cout << " ";
    cout << endl;

    deleteComplex(v, Complex(5, 5));
    cout << "\n删除(5,5)后的向量:" << endl;
    for (auto& c : v) c.print(), cout << " ";
    cout << endl;

    uniqueVector(v);
    cout << "\n唯一化后的向量:" << endl;
    for (auto& c : v) c.print(), cout << " ";
    cout << endl;

    vector<Complex> v1 = v;
    vector<Complex> v2 = v;

    clock_t start, end;
    start = clock();
    bubbleSort(v1);
    end = clock();
    cout << "\n冒泡排序耗时：" << (double)(end - start) / CLOCKS_PER_SEC << " 秒" << endl;

    start = clock();
    mergeSort(v2, 0, v2.size() - 1);
    end = clock();
    cout << "归并排序耗时：" << (double)(end - start) / CLOCKS_PER_SEC << " 秒" << endl;

    cout << "\n排序结果（按模排序）:" << endl;
    for (auto& c : v2) c.print(), cout << " ";
    cout << endl;

    double m1 = 3.0, m2 = 8.0;
    vector<Complex> result = rangeSearch(v2, m1, m2);
    cout << "\n模在 [" << m1 << "," << m2 << ") 的复数有:" << endl;
    for (auto& c : result) c.print(), cout << " ";
    cout << endl;

    return 0;
}
