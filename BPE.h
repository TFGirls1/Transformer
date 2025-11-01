#include<bits/stdc++.h>
using String = std::u32string;
using Char = char32_t;
class BPE{
private:
	std::map<String, int> Freq; //subword的频率
	
public:
	const int vocab_size = 200010;
	const Char Emp = 998244353;
	std::set<String> Punc;
	void get_vocab(String text, std::set<String> Punc) { // 从文本开始生成单个word
		int siz = (int)text.size();
		for(int i = 0,t = 0; i < siz; i ++) {
			t ++;
			if(Punc.count(text[i])){
				if(t != 1)
					Freq[text.substring(i - t + 1, t - 1) + Emp] ++, t = 0;
			}
		}
	}
	void get_stats() {
		for(auto& [word, freq] : Freq) {
			int siz = (int)word.size();
			for(int i = 0; i < siz; i ++) {

			}

		}
	}

};