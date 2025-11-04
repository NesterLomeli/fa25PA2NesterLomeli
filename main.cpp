//
// Created by Manju Muralidharan on 10/19/25.
//
#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include "heap.h"

using namespace std;

// ====== Global node arrays (parallel arrays) ======
int  weightArr[MAX_NODES];
int  leftArr[MAX_NODES];
int  rightArr[MAX_NODES];
char charArr[MAX_NODES];   // valid letter at leaves; '#' (or '\0') for internals

// ====== Helpers ======
static inline bool isUpper(char c) { return (c >= 'A' && c <= 'Z'); }
static inline bool isLower(char c) { return (c >= 'a' && c <= 'z'); }
static inline char toLowerFast(char c) {
    if (isUpper(c)) return char(c - 'A' + 'a');
    return c;
}

// 1) Read file and compute lowercase a-z frequency
// TODO [Ness-PA2-Freq-01]: Normalize to lowercase; ignore non-letters per spec.
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

// 2) Create leaf nodes for nonzero freqs. Returns nextFree = number of nodes used so far.
// TODO [Ness-PA2-Nodes-01]: Each nonzero frequency becomes a leaf; internals use '#'.
int createLeafNodes(const int freq[26]) {
    int nextFree = 0;
    for (int i = 0; i < MAX_NODES; ++i) {
        weightArr[i] = 0;
        leftArr[i] = -1;
        rightArr[i] = -1;
        charArr[i] = '\0';
    }

    for (int i = 0; i < 26; ++i) {
        if (freq[i] > 0) {
            charArr[nextFree] = char('a' + i);
            weightArr[nextFree] = freq[i];
            leftArr[nextFree] = -1;
            rightArr[nextFree] = -1;
            nextFree++;
            if (nextFree >= MAX_NODES) break; // safety guard
        }
    }
    return nextFree;
}

// 3) Build the Huffman-like encoding tree using our array heap; return root index
// TODO [Ness-PA2-Tree-01]: Repeatedly pop two mins, create parent, push back, until one root remains.
int buildEncodingTree(int nextFree) {
    MinHeap heap;

    // Push leaves into heap
    for (int i = 0; i < nextFree; ++i) {
        heap.push(i);
    }

    // Edge case: no symbols
    if (heap.getSize() == 0) {
        return -1;
    }

    // Edge case: only one symbol → tree is just that leaf
    if (heap.getSize() == 1) {
        return heap.pop();
    }

    int curr = nextFree; // next available slot for internal nodes

    while (heap.getSize() > 1 && curr < MAX_NODES) {
        int a = heap.pop();
        int b = heap.pop();

        // Create parent
        leftArr[curr] = a;
        rightArr[curr] = b;
        charArr[curr] = '#'; // internal
        weightArr[curr] = weightArr[a] + weightArr[b];

        heap.push(curr);
        curr++;
    }

    if (heap.getSize() == 1) {
        return heap.pop();
    }
    // Fallback (shouldn't happen with valid input sizes)
    return -1;
}

// 4) Iterative traversal to generate codes (no recursion)
// TODO [Ness-PA2-Codes-01]: Use stack frames {node, path}; left = '0', right = '1'.
// TODO [Ness-PA2-Codes-02]: Single-symbol input must still get a code (use "0").
void generateCodes(int root, string codes[26]) {
    for (int i = 0; i < 26; ++i) codes[i].clear();
    if (root < 0) return;

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
                // Handle single-symbol input: assign "0" if path is empty
                if (cur.path.empty()) codes[ch - 'a'] = "0";
                else codes[ch - 'a'] = cur.path;
            }
        } else {
            // Push right first so left is processed first (optional order)
            if (R != -1) st.push({R, cur.path + "1"});
            if (L != -1) st.push({L, cur.path + "0"});
        }
    }
}

// 5) Print code table and encoded message
// TODO [Ness-PA2-Output-01]: First print Character:Code pairs, then the bitstring.
// TODO [Ness-PA2-Output-02]: Ignore non-letters when emitting encoded bits.
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
            if (!bitstr.empty()) cout << bitstr;
        }
        // Non-letters are ignored for encoding
    }
    fin.close();
    cout << "\n";
}

int main() {
    const string filename = "input.txt";

    // Step 1: frequency
    int freq[26];
    buildFrequency(filename, freq);

    // Step 2: leaf nodes
    int nextFree = createLeafNodes(freq);

    if (nextFree == 0) {
        cout << "No lowercase letters found in input. Nothing to encode.\n";
        return 0;
    }

    // Step 3: build encoding tree
    int root = buildEncodingTree(nextFree);

    // Step 4: iterative traversal to assign codes
    string codes[26];
    generateCodes(root, codes);

    // Step 5: print table and encoded message
    encodeMessage(filename, codes);

    return 0;
}