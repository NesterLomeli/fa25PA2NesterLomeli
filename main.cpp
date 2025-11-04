//
// Created by Manju Muralidharan on 10/19/25.
//
#include <iostream>  // cout
#include <fstream>   // ifstream
#include <stack>     // std::stack for iterative DFS
#include <string>    // std::string for codes
#include "heap.h"    // our min-heap header

using namespace std;

// global arrays to store the “tree” using parallel arrays
// each index is a node id. leaves have a letter + weight, internals use '#'
int  weightArr[MAX_NODES];
int  leftArr[MAX_NODES];
int  rightArr[MAX_NODES];
char charArr[MAX_NODES]; // if it's a leaf, this holds the character

// tiny helpers to keep things simple and fast
static inline bool isUpper(char c) { return (c >= 'A' && c <= 'Z'); }
static inline bool isLower(char c) { return (c >= 'a' && c <= 'z'); }
static inline char toLowerFast(char c) {
    if (isUpper(c)) return char(c - 'A' + 'a');
    return c;
}

// read the file and count lowercase a-z letters (we ignore everything else)
// also we normalize uppercase to lowercase
void buildFrequency(const string& filename, int freq[26]) {
    for (int i = 0; i < 26; ++i) freq[i] = 0;

    ifstream fin(filename.c_str());
    if (!fin) {
        cout << "Failed to open input file: " << filename << "\n";
        return;
    }
    char c;
    while (fin.get(c)) {
        c = toLowerFast(c);
        if (isLower(c)) freq[c - 'a']++;
    }
    fin.close();
}

// create leaf nodes for all letters that actually appear (freq > 0)
// returns nextFree = how many nodes we used so far
int createLeafNodes(const int freq[26]) {
    int nextFree = 0;

    // reset everything first (not fancy; just clear)
    for (int i = 0; i < MAX_NODES; ++i) {
        weightArr[i] = 0;
        leftArr[i] = -1;
        rightArr[i] = -1;
        charArr[i] = '\0';
    }

    // make leaves
    for (int i = 0; i < 26; ++i) {
        if (freq[i] > 0) {
            charArr[nextFree] = char('a' + i);
            weightArr[nextFree] = freq[i];
            // left/right already -1, meaning “no children” (leaf)
            nextFree++;
            if (nextFree >= MAX_NODES) break; // just in case
        }
    }
    return nextFree;
}

// build the Huffman-like tree using our heap:
// keep popping the two smallest nodes and combine them into a parent
// push the parent back; repeat until one node (root) remains
int buildEncodingTree(int nextFree) {
    MinHeap heap;

    // start with all leaves
    for (int i = 0; i < nextFree; ++i) {
        heap.push(i);
    }

    // no letters? nothing to do
    if (heap.getSize() == 0) {
        return -1;
    }

    // only one letter? root is just that leaf (still need a code)
    if (heap.getSize() == 1) {
        return heap.pop();
    }

    // we build internal nodes after the leaves
    int curr = nextFree;

    while (heap.getSize() > 1 && curr < MAX_NODES) {
        int a = heap.pop();
        int b = heap.pop();

        // parent node: weight = sum, children = a and b
        leftArr[curr] = a;
        rightArr[curr] = b;
        charArr[curr] = '#'; // mark as internal
        weightArr[curr] = weightArr[a] + weightArr[b];

        heap.push(curr);
        curr++;
    }

    // the last one in the heap is the root
    if (heap.getSize() == 1) {
        return heap.pop();
    }
    // if something went wrong (shouldn’t happen), bail
    return -1;
}

// generate the actual bit codes for each letter using an iterative DFS (stack)
// left edge = '0', right edge = '1'
// if there’s only one symbol total, we give it "0" so it’s still encodable
void generateCodes(int root, string codes[26]) {
    for (int i = 0; i < 26; ++i) codes[i].clear();
    if (root < 0) return;

    // simple frame with node id + the path bits we took to get here
    struct Item {
        int node;
        string path;
    };

    stack<Item> st;
    st.push({root, ""});

    while (!st.empty()) {
        Item cur = st.top();
        st.pop();

        int n = cur.node;
        int L = leftArr[n];
        int R = rightArr[n];

        bool isLeaf = (L == -1 && R == -1);

        if (isLeaf) {
            char ch = charArr[n];
            if (ch >= 'a' && ch <= 'z') {
                // edge case: only one letter total → give it "0"
                if (cur.path.empty()) codes[ch - 'a'] = "0";
                else codes[ch - 'a'] = cur.path;
            }
        } else {
            // push right first so left comes off stack next (order doesn't really matter)
            if (R != -1) st.push({R, cur.path + "1"});
            if (L != -1) st.push({L, cur.path + "0"});
        }
    }
}

// print the code table and then the encoded message
// we re-open the file to go through all chars and spit out bits
void encodeMessage(const string& filename, string codes[26]) {
    cout << "Character : Code\n";
    for (int i = 0; i < 26; ++i) {
        if (!codes[i].empty()) {
            cout << char('a' + i) << " : " << codes[i] << "\n";
        }
    }

    cout << "\nEncoded message:\n";
    ifstream fin(filename.c_str());
    if (!fin) {
        cout << "Failed to open input file: " << filename << "\n";
        return;
    }
    char c;
    while (fin.get(c)) {
        c = toLowerFast(c);
        if (isLower(c)) {
            const string& bitstr = codes[c - 'a'];
            // skip if somehow missing (shouldn’t be)
            if (!bitstr.empty()) cout << bitstr;
        }
        // ignore non-letters
    }
    fin.close();
    cout << "\n";
}

int main() {
    const string filename = "input.txt";

    // Step 1: get letter counts
    int freq[26];
    buildFrequency(filename, freq);

    // Step 2: make leaf nodes for letters that show up
    int nextFree = createLeafNodes(freq);

    if (nextFree == 0) {
        cout << "No lowercase letters found in input. Nothing to encode.\n";
        return 0;
    }

    // Step 3: build encoding tree from heap
    int root = buildEncodingTree(nextFree);

    // Step 4: walk tree (iteratively) to get codes
    string codes[26];
    generateCodes(root, codes);

    // Step 5: show table and the final bitstring
    encodeMessage(filename, codes);

    return 0;
}