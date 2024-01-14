#define FILE_EXTENSION ".txt"
#define MAX_CHAR 26
#include "main.hpp"
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <filesystem>
#include <map>
#include <queue>
#include <stack>
#include <set>

using namespace std;
namespace fs = std::filesystem;

bool isOperator(const string &token){
    return token == "+" || token == "/" || token == "-";
}

vector<string> intersectionSet(const vector<string>& vec1, const vector<string>& vec2){
    // 實現兩個向量的交集操作
    vector<string> result;
    for(auto& i : vec1){
        if(std::find(vec2.begin(), vec2.end(), i) != vec2.end()){
            result.push_back(i);
        }
    }
    return result;
}

vector<string> unionSet(const vector<string>& vec1, const vector<string>& vec2, vector<Data>& data_list){
    // 實現兩個向量的聯集操作
    vector<string> result = vec1;
    for(auto& i : data_list){
        if(std::find(vec1.begin(), vec1.end(), i.title) != vec1.end() || std::find(vec2.begin(), vec2.end(), i.title) != vec2.end()){
            if(std::find(result.begin(), result.end(), i.title) == result.end()) result.push_back(i.title);
        }
    }
    return result;
}

vector<string> differenceSet(const vector<string>& vec1, const vector<string>& vec2){
    // 實現兩個向量的差集操作
    vector<string> result;
    for(auto& i : vec1){
        if(std::find(vec2.begin(), vec2.end(), i) == vec2.end()){
            result.push_back(i);
        }
    }
    return result;
}

int main(int argc, char *argv[]){
	vector<string> out;

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

	vector<pair<string, int>> query_mode; 
	// map<string, Tree> tree_map;
	vector<Data> data_list;

	// -------------- Read Datas -------------- //
	vector<string> paths;
	for (const auto &file_path : fs::directory_iterator(data_dir)) paths.push_back(file_path.path().string());
	// sort by file order
	sort(paths.begin(), paths.end(), [](const string& str1, const string& str2) {
		unsigned int i = 0;
		if (str1.length() != str2.length()) return (str1.length() < str2.length());
		while ((i < str1.length()) && (i < str2.length())) {
			if (str1[i] < str2[i]) return true;
			else if (str1[i] > str2[i]) return false;
			++i;
		}
		return false;
	});

	for (const auto &file_path : paths) {
		// OPEN FILE
		std::cout << file_path << std::endl;

		fi.open(file_path, std::ios::in);

		// GET TITLENAME
		std::getline(fi, title_name);
		// cout << title_name << endl;

		// GET TITLENAME WORD ARRAY
		tmp_string = split(title_name, " ");

		std::vector<std::string> title = word_parse(tmp_string);

		// Create Tree
		Tree tree;

		// Add each word in the title to the Trie and Suffix Tree
		for(auto &word : title){
			if(word == "") continue;
			tree.buildPrefixTree(word);
			tree.buildSuffixTree(word);
		}

		// GET CONTENT LINE BY LINE
		// std::cout << "start reading content" << endl;
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

		// std::cout<<"finish reading content"<<endl;

		// SAVE TREE
		Data data = {true, title_name, tree};
		
		data_list.push_back(data);
		std::cout<<"finish saving tree"<<endl;

		// CLOSE FILE
		fi.close();
	}

	
	// -------------- Read Query File -------------- //
	fstream query_file;
	query_file.open(query, ios::in);

	vector<vector<string>> query_list;

	string query_line;
	while(getline(query_file, query_line)){
		cout << query_line << endl;
		// GET QUERY WORD VECTOR
		tmp_string = split(query_line, " ");

		// PROCESS QUERY
		vector<string> resultSet;
		string oper = "X";

		for(auto& token : tmp_string){
			out.push_back(token);
			if(isOperator(token)){
				oper = token;
			}
			else{
				cout << "token: " << token << endl;
				vector<string> currentSet;

				// Iterate over all trees
				for(auto& data : data_list){
					// cout<<"file name: "<<file_name<<endl;

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
					}
					else {
						// Prefix search
						found = data.tree.search(data.tree.prefixRoot, token, PREFIX);
					}

					// cout<<found<<endl;
					if(found){
						cout << "found " << token << " in " << data.title << endl;
						currentSet.push_back(data.title);
					}
				}
				if(oper == "X"){
					resultSet = currentSet;
				}
				else if(oper == "+"){
					resultSet = intersectionSet(resultSet, currentSet);
				}
				else if(oper == "/"){
					resultSet = unionSet(resultSet, currentSet, data_list);
				}
				else if(oper == "-"){
					resultSet = differenceSet(resultSet, currentSet);
				}
				cout << "resultSet: " << endl;
				for(auto& i : resultSet){
					cout << i << endl;
				}
			}
		}

		if(resultSet.size() == 0){
			cout << "Not Found!" << endl;
			out.push_back("Not Found!");
		}
		else{
			for(auto& i : resultSet){
				out.push_back(i);
			}
		}		
	}
	query_file.close();

	// -------------- Write output -------------- //
	fstream fo;
	fo.open(output, ios::out);
	for(auto &data: out){
		fo << data << endl;
	}
	fo.close();
}