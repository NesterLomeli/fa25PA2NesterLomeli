//
// Created by Manju Muralidharan on 10/19/25.
//

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <string>

// ===== Shared capacity across files =====
#ifndef MAX_NODES
#define MAX_NODES 128   // Enough for <= 2*26-1 nodes; raise if you need.
#endif

// Compares node indices by referring to this global array.
// DEFINED in main.cpp and DECLARED here.
extern int weightArr[MAX_NODES];

// ===== Array-based min-heap storing NODE INDICES =====
struct MinHeap {
    int data[MAX_NODES];
    int size;

    MinHeap() : size(0) {}

    bool empty() const { return size == 0; }
    int  getSize() const { return size; }

    void swapIdx(int i, int j) {
        int t = data[i];
        data[i] = data[j];
        data[j] = t;
    }

    // Compare two heap positions by node weights (tie-break on index for stability)
    bool lessAt(int i, int j) const {
        int a = data[i], b = data[j];
        if (weightArr[a] != weightArr[b]) return weightArr[a] < weightArr[b];
        return a < b;
    }

    // TODO [Ness-PA2-Heap-01]: Bubble the new node up until heap property holds.
    void upheap(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (!lessAt(i, p)) break;
            swapIdx(i, p);
            i = p;
        }
    }

    // TODO [Ness-PA2-Heap-02]: Sift the root down after pop to restore order.
    void downheap(int i) {
        while (true) {
            int l = 2 * i + 1;
            int r = 2 * i + 2;
            int smallest = i;

            if (l < size && lessAt(l, smallest)) smallest = l;
            if (r < size && lessAt(r, smallest)) smallest = r;

            if (smallest == i) break;
            swapIdx(i, smallest);
            i = smallest;
        }
    }

    // TODO [Ness-PA2-Heap-03]: Insert new node index, then upheap.
    void push(int nodeIdx) {
        data[size] = nodeIdx;
        size++;
        upheap(size - 1);
    }

    // TODO [Ness-PA2-Heap-04]: Remove min (root), move last to root, then downheap.
    int pop() {
        int ret = data[0];
        data[0] = data[size - 1];
        size--;
        if (size > 0) downheap(0);
        return ret;
    }

    int top() const { return data[0]; }
};

#endif // MIN_HEAP_H