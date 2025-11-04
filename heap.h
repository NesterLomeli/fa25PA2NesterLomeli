//
// Created by Manju Muralidharan on 10/19/25.
//

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <string>

// just a simple cap for how many nodes we might have
// 26 letters -> at most 2*26-1 nodes, so 128 is plenty
#ifndef MAX_NODES
#define MAX_NODES 128
#endif

// weights live in main.cpp; heap only stores indices into those arrays
extern int weightArr[MAX_NODES];

// basic array-based min-heap storing NODE INDICES (not weights directly)
struct MinHeap {
    int data[MAX_NODES]; // holds indices to the node arrays
    int size;            // how many items are in the heap

    MinHeap() : size(0) {}

    bool empty() const { return size == 0; }
    int  getSize() const { return size; }

    // tiny swap helper
    void swapIdx(int i, int j) {
        int t = data[i];
        data[i] = data[j];
        data[j] = t;
    }

    // compare two heap positions using their node weights
    // if tie, break by index so it’s stable
    bool lessAt(int i, int j) const {
        int a = data[i], b = data[j];
        if (weightArr[a] != weightArr[b]) return weightArr[a] < weightArr[b];
        return a < b;
    }

    // move a new item up until heap property is good
    void upheap(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (!lessAt(i, p)) break;
            swapIdx(i, p);
            i = p;
        }
    }

    // push the root down after a pop to restore order
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

    // insert a node index and fix the heap
    void push(int nodeIdx) {
        data[size] = nodeIdx;
        size++;
        upheap(size - 1);
    }

    // pop the smallest (root), move last to root, then sift down
    int pop() {
        int ret = data[0];
        data[0] = data[size - 1];
        size--;
        if (size > 0) downheap(0);
        return ret;
    }

    int top() const { return data[0]; } // just peek (not used here but nice to have)
};

#endif // MIN_HEAP_H