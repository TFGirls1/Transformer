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
    clock_t start_time = clock();
    json tokens;
    read_token(tokens);
    BPE bpe;
    bpe.import_token(tokens);
    std::ifstream punc_f("materials/punc.txt");
    std::set<Char> Punc;
    Punc.insert(U' ');      // ASCII 空格
    Punc.insert(U'\t');     // 制表符
    Punc.insert(U'\n');     // 换行（如果需要）
    Punc.insert(U'\r');     // 回车
    Punc.insert(U'\u00A0'); // NBSP
    Punc.insert(U'\u3000'); // 全角空格
    String punc;
    getline(punc_f, punc);
    // std::cerr << punc << "\n";
    std::cerr << punc.size() << " punctuation characters loaded.\n";
    for(auto& i : punc){
        Punc.insert(i);
    }
    punc_f.close();
    std::ifstream text_f("materials/text.txt");
    if(!text_f.is_open()){
        std::cerr << "无法打开 materials/text.txt\n";
        return 1;
    }

    int t = 3000;
    String para;
    // 使用 getline 的返回值作为循环条件，避免在流状态为 fail/EOF 时误用 continue
    while(t-- && getline(text_f, para)) {
        if(para.empty()) {t ++; continue;}
        std::cerr << "Reading paragraph...\n";
        std::cerr << "Training on a paragraph of size " << para.size() << ".\n";
        
        // 重新读取 tokens（read_token 会自己打开文件）
        json current_tokens;
        read_token(current_tokens);
        bpe.import_token(current_tokens);

        std::ofstream o("data/tokens.json", std::ios::out | std::ios::trunc);
        if(!o.is_open()){
            std::cerr << "无法打开 data/tokens.json 进行写入\n";
            continue; // 无法写入时跳过此段的训练保存
        }
        /*删*/
        bool Continue = bpe.train_BPE(para, Punc);
        std::cerr << bpe.Amount_Subword() << " subwords trained.\n";    
        bpe.token_encode();
        json a = bpe.export_token();
        o << std::setw(4) << a << std::endl;
        o.close();
        /*输出json */
        if(text_f.eof()) {
            break;
        }
        if(!Continue) {
            std::cerr << "Vocabulary size limit reached. Stopping training.\n";
            break;
        }
    }
    text_f.close();
    std::cerr << "Training completed in " << double(clock() - start_time) / CLOCKS_PER_SEC << " seconds.\n";
    return 0;
}