#include<bits/stdc++.h>
#include "TFdef.h"
class BPE{
private:
	std::map<String, int> Freq; //subword的频率
	std::set<String> Words;
	std::map<String, std::vector<String> > Subword;
public:
	int Vocab_Size = 200010;
	const Char Emp = 998244353;
	std::map<String, std::pair<int, int> > Token;
	void get_vocab(const String& text, const std::set<Char>& Punc) { //从文本开始生成单个word
		int siz = (int)text.size();
		String cur;
		for(int i = 0; i < siz; i ++) {
			cur += text[i];
			if(!Punc.count(text[i])) {
				cur.pop_back();
				Words.insert(cur);
				cur.clear();
			}
		}
		if(cur.size()) {
			if(Punc.count(cur.back())) cur.pop_back();
			Words.insert(cur);
		}
	}
	/*
		subword:[位置，token（编号），原本字符串是啥]
		while true until 没有频次高于2的：
		遍历位置，合并频次最高的
		原本字符串拼接，删除之前的版本
		（第一次在后面有标点、空格的词后面加上分隔符Emp）
		[1,2,3,1,4,2,3]
		->
		[1,5,1,4,5]
	*/
	/*
		[1,2,3,1,2,2,3,2,3]
		[1,4,1,2,4,4]
		
	*/
	void get_stats() {
		for(auto word: Words) {
			int siz = (int)word.size();
			for(int i = 0; i < siz; i ++) {
				String tmp;
				tmp += word[i];
				Subword[word].push_back(tmp);
				Freq[tmp] ++;
			}
		}
	}
	int Amount_Subword() {
		return (int)Freq.size();
	}
	void set_size(int size) {
		Vocab_Size = size;
	}
	std::map< std::pair<String, String>, int> get_pairs() {
		std::map< std::pair<String, String>, int> pairs;
		for(auto word: Words) {
			int siz = (int)Subword[word].size();
			for(int i = 0; i < siz - 1; i ++) {
				pairs[{Subword[word][i], Subword[word][i + 1]}] ++;
			}
		}
		return pairs;
	}
	std::pair< std::pair<String, String>, int> get_max_pair(const std::map< std::pair<String, String>, int>& pairs) {
		std::pair< std::pair<String, String>, int> max_pair;
		max_pair.second = -1;
		for(auto p: pairs) {
			if(p.second > max_pair.second) {
				max_pair = p;
			}
		}
		return max_pair;
	}
	void merge_pair(const std::pair<String, String>& Subpair) {
		//记录合并操作
		for(auto word: Words) {
			for(int i = 0; i < (int)Subword[word].size() - 1; i ++) {
				if(std::make_pair(Subword[word][i], Subword[word][i + 1]) == Subpair) {
					String New_Subword = Subpair.first + Subpair.second;
					Subword[word][i] = New_Subword;
					Subword[word].erase(Subword[word].begin() + i + 1);
					Freq[New_Subword] ++;
					Freq[Subpair.first] --;
					Freq[Subpair.second] --;
					if(!Freq[Subpair.first]) Freq.erase(Subpair.first);
					if(!Freq[Subpair.second]) Freq.erase(Subpair.second);
				}
			}
		}
	}
	void train_BPE(const String& text, const std::set<Char>& Punc) {
		if(text.empty()) return;
		get_vocab(text, Punc);
		get_stats();
		while((int)Freq.size() < Vocab_Size) {
			std::map< std::pair<String, String>, int> pairs = get_pairs();
			std::pair< std::pair<String, String>, int> max_pair = get_max_pair(pairs);
			if(max_pair.second < 2) {
				break;
			}
			std::pair<String, String> Subpair = max_pair.first;
			merge_pair(Subpair);
		}
	}
	/*
	接下来我们考虑去做token映射
	*/
	void token_encode() {
		int idx = (int)Token.size();
		for(auto& p: Freq){
			Token[p.first] = {idx++, p.second};
		}
	}
	void token_decode(const String& text, std::vector<int>& token_text, const std::set<Char>& Punc) {
		//将文本词汇变为token序列
		int siz = (int)text.size();
		String cur;
		for(int i = 0; i < siz; i ++) {
			cur += text[i];
			if(Punc.count(text[i]))
				cur.clear();
			if(Token.count(cur)) {
				token_text.push_back(Token[cur].first);
				cur.clear();
			}
		}
	}
};
