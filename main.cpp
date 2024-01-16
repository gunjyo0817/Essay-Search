#define FILE_EXTENSION ".txt"
#define MAX_CHAR 26
#include "main.hpp"
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <filesystem>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

enum OperMode {
    NONE,
    AND,
    OR,
	EXCLUDE
};


int main(int argc, char *argv[]){
	vector<string> out;

	// INPUT :
	string data_dir = argv[1] + string("/");
	string query = string(argv[2]);
	string output = string(argv[3]);

	// Read Data File
	string file_name, title_name, tmp;
	fstream fi;
	vector<string> tmp_string;

	vector<Data> data_list;

	// -------------- Read Datas -------------- //
	vector<string> paths;
	for (const auto &file_path : fs::directory_iterator(data_dir)) paths.emplace_back(file_path.path().string());
	// sort by file order
	sort(paths.begin(), paths.end(), [](const string& str1, const string& str2) {
		if (str1.length() != str2.length()) return str1.length() < str2.length();
		return lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end());
	});

	for (const auto &file_path : paths) {
		// OPEN FILE
		fi.open(file_path, ios::in);
		// GET TITLENAME
		getline(fi, title_name);
		// GET TITLENAME WORD ARRAY
		tmp_string = split(title_name, " ");
		vector<string> title = word_parse(tmp_string);

		// Create Tree
		Tree tree;

		// Add each word in the title to the Trie and Suffix Tree
		for(auto &word : title){
			if(word == "") continue;
			tree.buildPrefixTree(word);
			tree.buildSuffixTree(word);
		}

		// GET CONTENT LINE BY LINE
		while (getline(fi, tmp)){
			// GET CONTENT WORD VECTOR
			tmp_string = split(tmp, " ");
			// PARSE CONTENT
			vector<string> content = word_parse(tmp_string);

			// Add each word in the content to the Trie and Suffix Tree
			for(auto &word : content){
				if(word == "") continue;
				tree.buildPrefixTree(word);
				tree.buildSuffixTree(word);
			}
		}

		// SAVE TREE
		Data data = {false, title_name, tree};
		data_list.emplace_back(data);

		// CLOSE FILE
		fi.close();
	}

	
	// -------------- Read Query File -------------- //
	fstream query_file;
	query_file.open(query, ios::in);

	vector<vector<string>> query_list;

	string query_line;
	while(getline(query_file, query_line)){
		// GET QUERY WORD VECTOR
		// out.emplace_back(query_line);
		tmp_string = split(query_line, " ");

		int oper = NONE;

		for(auto& token : tmp_string){
			if(token == "+")      oper = AND;
			else if(token == "/") oper = OR;
			else if(token == "-") oper = EXCLUDE;
			else{
				// Iterate over all trees
				for(auto& data : data_list){
					if(oper == AND && data.isResult == false) continue;
					if(oper == OR && data.isResult == true) continue;
					if(oper == EXCLUDE && data.isResult == false) continue;

					bool found = false;

					if (token[0] == '"' && token[token.size() - 1] == '"') {
						// Exact search
						found = data.tree.search(data.tree.prefixRoot, token.substr(1, token.size() - 2), EXACT);
					} 
					else if (token[0] == '*' && token[token.size() - 1] == '*') {
						//suffix search
						found = data.tree.search(data.tree.suffixRoot, token.substr(1, token.size() - 2), SUFFIX);
					}
					else if (token[0] == '<' && token[token.size() - 1] == '>') {
						//wildcard search
						found = data.tree.search(data.tree.prefixRoot, token.substr(1, token.size() - 2), WILDCARD);
					}
					else {
						// Prefix search
						found = data.tree.search(data.tree.prefixRoot, token, PREFIX);
					}

					if(found){
						if(oper == AND) data.isResult = true;
						else if(oper == OR) data.isResult = true;
						else if(oper == EXCLUDE) data.isResult = false;
						else data.isResult = true;
					}
					else{
						if(oper == AND) data.isResult = false;
					}
				}
			}
		}

		bool isNotFound  = true;
		for(auto& data : data_list){
			if(data.isResult == true){
				out.emplace_back(data.title);
				data.isResult = false;
				isNotFound = false;
			}
		}
		if(isNotFound){
			out.emplace_back("Not Found!");
		}
	}
	query_file.close();

	// -------------- Write output -------------- //
	fstream fo;
	fo.open(output, ios::out | ios::binary);
	for(auto &data: out){
		fo << data << '\n';
	}
	fo.close();
    return 0;
}