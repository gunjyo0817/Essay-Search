#define FILE_EXTENSION ".txt"
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <filesystem>
#include <map>

using namespace std;
namespace fs = std::filesystem;

// Trie : for prefix search, exact search
// Suffix Tree : for suffix search, wildcard search
class Tree{
	public:
		// Trie Node
		struct TrieNode{
			TrieNode *children[26];
			bool isEndOfWord;

			TrieNode(){
				isEndOfWord = false;
				memset(children, 0, sizeof(children));
			}
		};

		// Suffix Tree Node
		struct SuffixNode{
			int a, b;
			SuffixNode *l[128], *suffix;
			SuffixNode(int a, int b) : a(a), b(b), suffix(0){
				memset(l, 0, sizeof(l));
			}
		};

		TrieNode *trieRoot;
		SuffixNode *suffixRoot;
		Tree(){
			trieRoot = new TrieNode();
			suffixRoot = new SuffixNode(0, 0);
		}

		void addWordToTrie(const string& word) {
			// cout<<"add word to trie: "<<word<<endl;
			TrieNode *p = trieRoot;

			for (int i = 0; i < word.size(); i++) {
				int index = word[i] - 'a';
				if (!p->children[index])
					try{
						p->children[index] = new TrieNode();
					}
					 catch (const std::bad_alloc& e) {
						std::cerr << "記憶體不足: " << e.what() << '\n';
					}

				p = p->children[index];
			}

			p->isEndOfWord = true;
			// cout<<"finish add word to trie: "<<word<<endl;
		}

		void addWordToSuffixTree(const string& word) {
			cout<<"add word to suffix tree: "<<word<<endl;
			SuffixNode *p = suffixRoot;

			int N = word.size();
			for (int a = 0, i = 0; i < N + 1; ++i){
				SuffixNode *ant = suffixRoot;
				while (a <= i){
					while (true){
						if (a == i) break;
						SuffixNode *q = p->l[word[a]];
						if (a + q->b - q->a > i) break;
						a += q->b - q->a;
						p = q;
					}

					SuffixNode *q = p;
					if (a == i){
						if (p->l[word[i]]) break;
					}
					else if (a < i){
						SuffixNode *r = p->l[word[a]];
						int k = r->a + i - a;
						if (k >= word.size()) break;
						if (word[i] == word[k]) break;

						p->l[word[a]] = q = new SuffixNode(r->a, k);
						q->l[word[k]] = r;
						r->a = k;
					}

					try {
						q->l[word[i]] = new SuffixNode(i, N);
					}
					catch (const std::bad_alloc& e) {
						std::cerr << "記憶體不足: " << e.what() << '\n';
					}

					if (ant != suffixRoot) ant->suffix = q;
					ant = q;

					if (p->suffix) p = p->suffix;
					else a++;
				}
				if (ant != suffixRoot) ant->suffix = p;
			}
			cout<<"finish add word to suffix tree: "<<word<<endl;
		}

		// Search
		// bool exactSearch(const string& word) {
		// 	// Implement exact search using TrieNode

		// }

		// bool prefixSearch(const string& prefix) {
		// 	// Implement prefix search using TrieNode
		// }

		// bool suffixSearch(const string& suffix) {
		// 	// Implement suffix search using SuffixNode
		// }

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

	int file_num = 0;
	for (const auto &entry : fs::directory_iterator(data_dir))
		file_num++;
	cout << file_num << endl;


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

		// Add each word in the title to the Trie and Suffix Tree
		for(auto &word : title){
			tree.addWordToTrie(word);
			tree.addWordToSuffixTree(word);
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
				tree.addWordToTrie(word);
				tree.addWordToSuffixTree(word);
			}
		}

		// SAVE TREE
		tree_map[title_name] = tree;

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
		// GET QUERY WORD VECTOR
		tmp_string = split(query_line, " ");

		// PARSE QUERY
		vector<string> query = word_parse(tmp_string);

		for(auto &word : query){
			cout << word << endl;
		}

		// Process each query
		/*
		for (const string& word : query) {
			// Iterate over all trees
			for (auto& pair : tree_map) {
				const string& file_name = pair.first;
				Tree& tree = pair.second;

				if (word[0] == '"' && word[word.size() - 1] == '"') {
					// Exact search
					bool found = tree.exactSearch(word.substr(1, word.size() - 2));
				} else if (word[0] == '*') {
					// Suffix search
					bool found = tree.suffixSearch(word.substr(1));
				} else if (word[word.size() - 1] == '*') {
					// Prefix search
					bool found = tree.prefixSearch(word.substr(0, word.size() - 1));
				} else if (word[0] == '<' && word[word.size() - 1] == '>') {
					// Wildcard search
					bool found = tree.wildcardSearch(word.substr(1, word.size() - 2));
				}

				// Handle operators (+, /, -)
			}
		}
		*/
	
	}
}

// 1. UPPERCASE CHARACTER & LOWERCASE CHARACTER ARE SEEN AS SAME.
// 2. FOR SPECIAL CHARACTER OR DIGITS IN CONTENT OR TITLE -> PLEASE JUST IGNORE, YOU WONT NEED TO CONSIDER IT.
//    EG : "AB?AB" WILL BE SEEN AS "ABAB", "I AM SO SURPRISE!" WILL BE SEEN AS WORD ARRAY AS ["I", "AM", "SO", "SURPRISE"].
// 3. THE OPERATOR IN "QUERY.TXT" IS LEFT ASSOCIATIVE
//    EG : A + B / C == (A + B) / C

//

//////////////////////////////////////////////////////////
