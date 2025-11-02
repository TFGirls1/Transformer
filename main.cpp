#include"BPE.h"
int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    BPE bpe;
    String text;
    std::set<Char> Punc;
    bpe.train_BPE(text, Punc);
    return 0;
}