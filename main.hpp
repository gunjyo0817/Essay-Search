#define FILE_EXTENSION ".txt"
#define MAX_CHAR 26
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// -------------- Trie -------------- //

enum SearchMode {
    EXACT,
    PREFIX,
    SUFFIX,
	WILDCARD
};

class Tree{
	public:
		struct TrieNode {
			TrieNode* children[MAX_CHAR];
			bool isEndOfWord;
			string word;

			TrieNode(string s) : isEndOfWord(false), word(s) {
				for(int i = 0; i < MAX_CHAR; i++) children[i] = NULL;
			}
		};

		TrieNode* prefixRoot;
		TrieNode* suffixRoot;

		Tree() {
			prefixRoot = new TrieNode("");
			suffixRoot = new TrieNode("");
		}

		~Tree() {
		}

		void buildPrefixTree(string &word){
			buildPrefixTree(prefixRoot, word, 0);
		}

		void buildSuffixTree(string &word){
			buildSuffixTree(suffixRoot, word, 0);
		}

		bool search(TrieNode* &node, string target, int mode){
			if (mode == WILDCARD) {
				return wildcardSearch(node, target, 0);
			}
			TrieNode* cur = node;
			int alpha;
			for(int i = 0; i < target.length(); i++){
				if( mode == SUFFIX ) alpha = tolower(target[target.length() - i - 1]) - 'a';
				else alpha = tolower(target[i]) - 'a';
				
				if(cur->children[alpha] != NULL){
					cur = cur->children[alpha];
					if( i == target.length() - 1 && mode == EXACT ) return cur->isEndOfWord == true;
					else if( i == target.length() - 1 && mode != EXACT ) return true;  
				}
				else return false;
			}
			return false;
		}

		
	private:
		void buildPrefixTree(TrieNode* &node, string &word, int index = 0){
			if( index >= word.length() ) return;
			int alpha = word[index] - 'a';
			if( node->children[alpha] == NULL ){
				node->children[alpha] = new TrieNode(word.substr(0, index + 1));
			}

			if( index == word.length() - 1 ){
				node->children[alpha]->isEndOfWord = true;
				return;
			}
			buildPrefixTree(node->children[alpha], word, index + 1);
			return;
			
		}

		void buildSuffixTree(TrieNode* &node, string &word, int index){
			if( index >= word.length() ) return;
			int alpha = word[word.length() - index - 1] - 'a';
			if( node->children[alpha] == NULL ){
				node->children[alpha] = new TrieNode(word.substr(0, word.length() - index - 1));
			}

			if( index == word.length() - 1 ){
				node->children[alpha]->isEndOfWord = true;
				return;
			}
			buildSuffixTree(node->children[alpha], word, index + 1);
			return;
		}

		bool wildcardSearch(TrieNode* &node, string target, int index) {
			if (node == nullptr) return false;
			if (index == target.size()) return node->isEndOfWord;
			if (target[index] == '*') {
				if (wildcardSearch(node, target, index + 1)) {
					return true;
				}
				for (int i = 0; i < MAX_CHAR; i++) {
					if (node->children[i] != nullptr) {
						if (wildcardSearch(node->children[i], target, index)) {
							return true;
						}
					}
				}
			} 
			else {
				int alpha = tolower(target[index]) - 'a';
				if (node->children[alpha] != nullptr) {
					if (wildcardSearch(node->children[alpha], target, index + 1)) {
						return true;
					}
				}
			}
			return false;
		}
};


// -------------- Data File -------------- //
struct Data{
	bool isResult;
	string title;
	Tree tree;
};

// -------------- Utility Func -------------- //

// string parser : output vector of strings (words) after parsing
vector<string> word_parse(vector<string> &tmp_string){
	vector<string> parse_string;
	for (auto &word : tmp_string){
		string new_str;
		for (auto &ch : word){
			if (isalpha(ch)) new_str.push_back(tolower(ch));
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<string> split(const string &str, const string &delim){
	vector<string> res;
	if ("" == str) return res;

	char *strs = new char[str.length() + 1];
	strcpy(strs, str.c_str());

	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p){
		string s = p;
		res.emplace_back(s);
		p = strtok(NULL, d);
	}

	return res;
}

