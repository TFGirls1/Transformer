#include<iostream>
#include<string>
#include<map>
#include<set>
#include "TFdef.h"
class BPE{
private:
	std::map<String, int> Freq; //subword的频率
	
public:
	const int vocab_size = 200010;
	const Char Emp = 998244353;
	std::set<Char> Punc;
	
	void get_vocab(String text, std::set<Char> Punc) { //从文本开始生成单个word
		int siz = (int)text.size();
		int t = 0;
		for(int i = 0; i < siz; i ++) {
			if(!Punc.count(text[i]) && i != siz && !Punc.count(text[i + 1])){
				String tmp;
				tmp += text[i];
				tmp += text[i + 1];
				Freq[tmp] ++;
			}
			if(!Punc.count(text[i])) {
				String tmp;
				tmp += text[i];
				Freq[tmp] ++;
			}
		}
	}
	/*
		[1,2,3,1,4,2,3]
		->
		[1,5,1,4,5]
	*/
	void get_stats() {
		for(auto& [word, freq] : Freq) {
			int siz = (int)word.size();
			for(int i = 0; i < siz; i ++) {
				
			}

		}
	}

};
