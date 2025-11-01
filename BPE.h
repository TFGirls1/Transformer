#include<iostream>
#include<string>
#include<map>
#include<set>
#include "TFdef.h"
class BPE{
private:
	std::map<String, std::pair<int, int> > Freq; //subword的频率
	
public:
	const int vocab_size = 200010;
	const Char Emp = 998244353;
	std::set<Char> Punc;
	
	void get_vocab(String text, std::set<Char> Punc) { //从文本开始生成单个word
		int siz = (int)text.size();
		int t = 0;
		for(int i = 0; i < siz; i ++) {
			if(!Punc.count(text[i])) {
				if(i != siz - 1 && !Punc[text[i + 1]]) {
					String tmp;
					tmp += text[i];
					Freq[tmp].first ++;	
					Freq[tmp].second = ++ t;
				}else {
					String tmp;
					tmp += text[i];
					tmp += Emp;
					Freq[tmp].first ++;
					Freq[tmp].second = ++ t;
				}
			}
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
		std::priority_queue<std::map<String, std::pair<int, int> >, std::vector<std::map<String, std::pair<int, int> > >, compare> pq;
		for(auto& [word, freq] : Freq) {
			pq.push({word, freq});
		}
	}

};
struct compare{
		bool operator()(const std::pair<String, std::pair<int, int> >& a, const std::pair<String, std::pair<int, int> >& b) {
			if(a.second.second != b.second.second) return a.second.second < b.second.second;
			return a.second.first > b.second.first;
		}
	};