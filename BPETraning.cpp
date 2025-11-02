#include"BPE.h"
int main() {
    std::ifstream f("data/tokens.json");
    json tokens;
    f >> tokens;
    f.close();
    BPE bpe;
    bpe.import_token(tokens);
    std:: ifstream f("materials/punc.txt");
    std::set<Char> Punc;
    String punc;
    getline(f, punc);
    for(auto& i : punc){
        Punc.insert(i);
    }
    for(auto c: punc) {
        Punc.insert(c);
    }
    f.close();
    std:: ifstream f("materials/text.txt");
    while(1) {
        String para;
        if(!getline(f, para)) break;
        
        std::ofstream o("data/tokens.json", std::ios::trunc);
        /*删*/
        bpe.train_BPE(para, Punc);
        o << std::setw(4) << bpe.export_token() << std::endl;
        /*输出json */
    }
    f.close();
    return 0;
}