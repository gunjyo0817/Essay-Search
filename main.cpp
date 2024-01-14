/*
Query Format
Exact Search: “search-word”
Eg: we want to search essay with graph, we use query - “graph”
Prefix Search: search-word
Eg: we want to search essay with prefix graph, we use query - graph
Suffix Search: *search-word*
Eg: we want to search essay with suffix graph, we use query - *graph*
Wildcard Search: <search-pattern>
Eg: we want to search essay with word pattern gr*h, we use query - <gr*h>. “*” can be empty, single or multiple characters, so gr*h should match words like graph, growth…etc.
And operator: “+”
Eg: we want to search essay with graph and sparsity, we use query – “graph” + “sparsity”
Or operator: “/”
Eg: we want to search essay with graph or quantum, we use query – “graph” / “quantum”
Exclude operator: “-”
Eg: we want to search essay with graph but without deep, we use query – “graph” - “deep”
*/

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

// Trie -> AC Algo
class Tree{
	public:
		struct TrieNode {
			TrieNode* suffixLink;
			TrieNode* outputLink;
			TrieNode* children[MAX_CHAR];
			bool isEndOfWord;
			vector<string> words;
	
			TrieNode(){
				for (int i = 0; i < MAX_CHAR; i++) {
					children[i] = nullptr;
				}
				suffixLink = nullptr;
				outputLink = nullptr;
				isEndOfWord = false;
			}
		};

		TrieNode* root;

		Tree() {
			root = new TrieNode();
		}

		~Tree() {
		}

		void addWords(const vector<string>& words) {
            for (const auto& word : words) {
                addPattern(root, word);
				// addPattern(reversedRoot, string(word.rbegin(), word.rend()));
            }
			cout<<"finish adding patterns"<<endl;
			cout<<"start building suffix links"<<endl;
            buildSuffixLinks();
			cout<<"finish building suffix links"<<endl;
			cout<<"start building output links"<<endl;
            buildOutputLinks();
			cout<<"finish building output links"<<endl;
			cout<<"finish adding words"<<endl;
        }

		// 添加模式到 Trie
		void addPattern(TrieNode* trieRoot, const string& word) {
			// cout<<"add pattern: "<<word<<endl;
			TrieNode* node = trieRoot;
			for (auto &ch : word) {
				if (node->children[ch - 'a'] == nullptr) {
					node->children[ch - 'a'] = new TrieNode();
				}
				node = node->children[ch - 'a'];
			}
			node->isEndOfWord = true;
			node->words.push_back(word);
		}

		// 建立後綴鏈接
		void buildSuffixLinks() {
			queue<TrieNode*> q;
			root->suffixLink = root;
			q.push(root);
			while (!q.empty()) {
				TrieNode* curNode = q.front();
				q.pop();
				int index = 0;
				for (auto& child : curNode->children) {
					if(child == nullptr) continue;
					TrieNode* fallback = curNode->suffixLink;
					TrieNode* fallbackChild = fallback->children[index++];
					while (fallback != root &&  fallbackChild == nullptr) {
						fallback = fallback->suffixLink;
					}
					if (fallback != curNode && fallbackChild) {
						child->suffixLink = fallbackChild;
					}
					else {
						child->suffixLink = root;
					}
					q.push(child);
				}
			}
		}

		// 建立輸出鏈接
		void buildOutputLinks() {
			queue<TrieNode*> q;
			root->outputLink = nullptr;
			q.push(root);
			while (!q.empty()) {
				TrieNode* curNode = q.front();
				q.pop();
				TrieNode* fallback = curNode->suffixLink;
				while(fallback != root && !fallback->isEndOfWord) {
					fallback = fallback->suffixLink;
				}
				if(fallback != curNode && fallback->isEndOfWord) {
					curNode->outputLink = fallback;
				}
				for (auto& child : curNode->children) {
					if(child == nullptr) continue;
					q.push(child);
				}
			}
		}

		// Search
		bool exactSearch(const string& word) {
			// cout<<"exact search: "<<word<<endl;
			TrieNode* node = root;
			for (auto &ch : word) {
				node = node->children[ch - 'a'];
				if (node == nullptr) return false;
			}
			return node->isEndOfWord;
		}

		bool prefixSearch(const string& prefix) {
			// cout<<"prefix search: "<<prefix<<endl;
			TrieNode* node = root;
			for (auto &ch : prefix) {
				node = node->children[ch - 'a'];
				if (node == nullptr) return false;
			}
			return true;
		}

		bool suffixSearch(const string& suffix) {
			TrieNode* node = root;
			for (auto it = suffix.rbegin(); it != suffix.rend(); ++it) {
				char ch = *it;
				if (node->children[ch - 'a'] == nullptr) {
					return false;
				}
				node = node->children[ch - 'a'];
			}
			if (node->isEndOfWord) {
				// 检查所有存储的单词是否包含给定的后缀
				for (const auto& word : node->words) {
					if (word.size() >= suffix.size() && 
						word.substr(word.size() - suffix.size()) == suffix) {
						return true;
					}
				}
			}
			return false;
		}






		// bool wildcardSearch(const string& pattern) {
		// 	// Implement wildcard search using SuffixNode
		// }
};


// Utility Func

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




int main(int argc, char *argv[]){

	// INPUT :
	// 1. data directory in data folder
	// 2. number of txt files
	// 3. output route

	string data_dir = argv[1] + string("/");
	string query = string(argv[2]);
	string output = string(argv[3]);

	// Read File & Parser Example
	// Read Data File
	string file_name, title_name, tmp;
	fstream fi;
	vector<string> tmp_string;

	map<string, Tree> tree_map;

	// from data_dir get file ....
	// go through all files, build trie and suffix trie
	cout<<"start building tree"<<endl;
	for (const auto &file_path : fs::directory_iterator(data_dir)){
		// cout<<file_num--<<endl;
		// OPEN FILE
		file_name = file_path.path().string();
		cout << file_name << endl;
		fi.open(file_name, ios::in);

		// Create Tree
		Tree tree;

		// GET TITLENAME
		getline(fi, title_name);
		// cout << title_name << endl;

		// GET TITLENAME WORD ARRAY
		tmp_string = split(title_name, " ");

		vector<string> title = word_parse(tmp_string);

		vector<string> words;

		// Add each word in the title to the Trie and Suffix Tree
		for(auto &word : title){
			words.push_back(word);
		}

		// GET CONTENT LINE BY LINE
		cout << "start reading content" << endl;
		while (getline(fi, tmp)){

			// GET CONTENT WORD VECTOR
			tmp_string = split(tmp, " ");

			// PARSE CONTENT
			vector<string> content = word_parse(tmp_string);

			// Add each word in the content to the Trie and Suffix Tree
			for(auto &word : content){
				words.push_back(word);
			}
		}
		cout<<"finish reading content"<<endl;

		// BUILD TREE
		tree.addWords(words);
		cout<<"finish building tree"<<endl;

		// SAVE TREE
		tree_map[title_name] = tree;
		cout<<"finish saving tree"<<endl;

		// CLOSE FILE
		fi.close();
	}

	cout<<"finish building tree"<<endl;
	cout<<"start query"<<endl;

	// Read Query File
	fstream query_file;
	query_file.open(query, ios::in);

	string query_line;
	while(getline(query_file, query_line)){
		// cout << query_line << endl;
		// GET QUERY WORD VECTOR
		tmp_string = split(query_line, " ");

		// PARSE QUERY
		// vector<string> query = word_parse(tmp_string);
		vector<string> query = tmp_string;
		for(auto &word : query){
			cout << word << endl;
		}

		// Process each query
		
		for (const string& word : query) {
			cout<<"query: "<<word<<endl;
			// Iterate over all trees
			for (auto& pair : tree_map) {
				const string& file_name = pair.first;
				Tree& tree = pair.second;
				// cout<<"file name: "<<file_name<<endl;

				bool found = false;

				if (word[0] == '"' && word[word.size() - 1] == '"') {
					// Exact search
					found = tree.exactSearch(word.substr(1, word.size() - 2));
				} 
				else if (word[0] == '*' && word[word.size() - 1] == '*') {
					//suffix search
					found = tree.suffixSearch(word.substr(1, word.size() - 2));
				}
				else if (word[0] == '<' && word[word.size() - 1] == '>') {
					//wildcard search
				}
				else {
					// Prefix search
					found = tree.prefixSearch(word.substr(0, word.size()));
				}

				// cout<<found<<endl;
				if(found){
					cout << "found " <<file_name << endl;
				}
			}
			// Handle operators (+, /, -)
		}
		
	
	}
}

// 1. UPPERCASE CHARACTER & LOWERCASE CHARACTER ARE SEEN AS SAME.
// 2. FOR SPECIAL CHARACTER OR DIGITS IN CONTENT OR TITLE -> PLEASE JUST IGNORE, YOU WONT NEED TO CONSIDER IT.
//    EG : "AB?AB" WILL BE SEEN AS "ABAB", "I AM SO SURPRISE!" WILL BE SEEN AS WORD ARRAY AS ["I", "AM", "SO", "SURPRISE"].
// 3. THE OPERATOR IN "QUERY.TXT" IS LEFT ASSOCIATIVE
//    EG : A + B / C == (A + B) / C

//

//////////////////////////////////////////////////////////
