#define FILE_EXTENSION ".txt"
#define MAX_CHAR 26
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <filesystem>
#include <map>
#include <queue>

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
	
			TrieNode() : isEndOfWord(false), word("") {
				for(int i = 0; i < MAX_CHAR; i++) children[i] = NULL;
			}

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

		void buildPrefixTree(string word){
			buildPrefixTree(prefixRoot, word);
		}

		void buildSuffixTree(string word){
			buildSuffixTree(suffixRoot, word, 0);
		}

		bool search(TrieNode* node, string target, int mode){
			// cout << "searching " << target << endl;
			// cout << "mode = " << mode << endl;
			TrieNode* cur = node;

			if(mode == EXACT){
				for(auto &c : target){
					int alpha = tolower(c) - 'a';
					if(cur->children[alpha] != NULL){
						cur = cur->children[alpha];
					}
					else{
						return false;
					}
				}
				return cur->isEndOfWord == true;
			}
			else if(mode == PREFIX){
				cout << "prefix search: " << target << endl;
				for(auto &c : target){
					int alpha = tolower(c) - 'a';
					if(cur->children[alpha] != NULL){
						cur = cur->children[alpha];
					}
					else{
						return false;
					}
				}
				return true;
			}
			else if(mode == SUFFIX){
				for(int i = 0; i < target.length(); i++){
					int alpha = target[target.length() - i - 1] - 'a';
					if(cur->children[alpha] != NULL){
						cur = cur->children[alpha];
						if( i == target.length() - 1 ) return true;
					}
					else{
						return false;
					}
				}
				return false;
			}
			else if(mode == WILDCARD){
				
				return false;
			}
			return false;
			// int alpha;
			// for(int i = 0; i < target.length(); i++){
			// 	if( mode == SUFFIX ) alpha = target[target.length() - i - 1] - 'a';
			// 	else alpha = target[i] - 'a';
				
			// 	if(cur->children[alpha] != NULL){
			// 		cur = cur->children[alpha];
			// 		if( i == target.length() - 1 && mode == EXACT ) return cur->isEndOfWord == true;
			// 		else if( i == target.length() - 1 && mode != EXACT ) return true;  
			// 	}
			// 	else{
			// 		break;
			// 	}
			// }
			// return false;
		}

		
	private:
		void buildPrefixTree(TrieNode* node, string word){
			for(auto &c : word){
				int alpha = tolower(c) - 'a';
				if( node->children[alpha] == NULL ){
					node->children[alpha] = new TrieNode(word.substr(0, 1));
				}
				node = node->children[alpha];
			}
			node->isEndOfWord = true;
			// if( index >= word.length() ) return;
			// int alpha = word[index] - 'a';
			// if( node->children[alpha] == NULL ){
			// 	node->children[alpha] = new TrieNode(word.substr(0, index + 1));
			// }
			// if( index == word.length() - 1 ){
			// 	node->children[alpha]->isEndOfWord = true;
			// 	return;
			// }
			// buildPrefixTree(node->children[alpha], word, index + 1);
			return;
		}

		void buildSuffixTree(TrieNode* node, string word, int index){
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
};


// -------------- Data File -------------- //
struct Data{
	bool isResult;
	string title;
	Tree tree;
};

// -------------- Utility Func -------------- //

// string parser : output vector of strings (words) after parsing
vector<string> word_parse(vector<string> tmp_string){
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

vector<pair<string, int>> query_parse(vector<string> tmp_string, bool &flag){
	vector<pair<string, int>> parse_string;
	for(auto& token : tmp_string){
        if(token == "*"){
            parse_string.push_back(make_pair(token, -1)); // -1 表示運算子
        }
		else{
            parse_string.push_back(make_pair(token, 1)); // 1 表示運算元
        }
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
		res.push_back(s);
		p = strtok(NULL, d);
	}

	return res;
}

