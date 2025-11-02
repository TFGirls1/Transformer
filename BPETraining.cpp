#include"BPE.h"
void read_token(json& x){
    std::ifstream f("data/tokens.json");
    std::string temp;
    f >> temp;
    f.close();
    f.open("data/tokens.json");
    if(temp.empty()) {
        x = json::object();
    }else {
        f >> x;
    }
    f.close();
}
int main() {
    json tokens;
    read_token(tokens);
    BPE bpe;
    bpe.import_token(tokens);
    std::ifstream punc_f("materials/punc.txt");
    std::set<Char> Punc;
    String punc;
    getline(punc_f, punc);
    // std::cerr << punc << "\n";
    std::cerr << punc.size() << " punctuation characters loaded.\n";
    for(auto& i : punc){
        Punc.insert(i);
    }
    punc_f.close();
    std::ifstream text_f("materials/text.txt");
    while(1) {
        String para;
        if(!getline(text_f, para)) break;
        // std::cerr << para.to_utf8() << "\n";
        text_f.open("data/tokens.json", std::ios::in);
        json tokens;
        read_token(tokens);
        bpe.import_token(tokens);
        std::ofstream o("data/tokens.json", std::ios::out | std::ios::trunc);
        /*删*/
        bpe.train_BPE(para, Punc);
        std::cerr << bpe.Amount_Subword() << " subwords trained.\n";
        bpe.token_encode();
        json a = bpe.export_token();
        // for(auto& [key, value] : a.items()) {
        //     std::cerr << key << ": " << value[0]<< " " << value[1] << "\n";
        // }
        o << std::setw(4) << bpe.export_token() << std::endl;
        /*输出json */
    }
    text_f.close();
    return 0;
}