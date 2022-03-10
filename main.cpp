#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <map>

#ifdef _WIN32
#include <windows.h>

void sleep(int milliseconds) {
	Sleep(milliseconds);
}

void clear() {
	system("cls");
}

#elif defined(__linux__)
#include <unistd.h>

/*
void sleep(int milliseconds) {
	sleep(milliseconds);
}*/

void clear() {
	system("clear");
}
#endif

std::streamsize get_flength(std::ifstream& file) {
	if(!file.is_open()) {
		return 0;
	}
	std::streampos temp_1 = file.tellg();
	file.seekg(0, std::fstream::end);
	std::streampos temp_2 = file.tellg();
	file.seekg(temp_1, std::fstream::beg);

	return temp_2;
}

std::string read(std::string path) {
	std::ifstream ifile;
	ifile.open(path, std::ios::binary);
	std::streamsize len = get_flength(ifile);
	char* dummy = new char[len];

	try {
		ifile.read(dummy, len);
	}
	catch(std::exception& err) {
		ifile.close();
        return "";
	}
	if (dummy == nullptr || strlen(dummy) == 0) {
		ifile.close();
        return "";
	}
	ifile.close();
	//dummy += '\0';
	std::string re;
	re.assign(dummy, len);

	delete[] dummy;
	dummy = nullptr;

	return re;
}

std::vector<std::string> intoLines(std::string src) {
	std::vector<std::string> ret;
	std::string tmp;

	for(size_t i = 0; i < src.size(); ++i) {
		if(src[i] == '\n') {
			if(tmp != "") {
				ret.push_back(tmp);
			}
			tmp = "";
		}
		else {
			tmp += src[i];
		}
	}
	if(tmp != "") {
		ret.push_back(tmp);
	}

	/*
	for(size_t i = 0; i < ret.size(); ++i) {
		std::cout << i << " : \"" << ret[i] << "\"\n";
	}
	*/

	return ret;
}

struct Answer {
	std::string name;
	std::string jumpTo;
};

struct Question {
	std::string name;
	std::string text;
	std::vector<Answer> answers;
	bool breakAfterExecute = false;
	bool jumpAfterExecute = false;
	std::string jumpTo = "";

	void clear() {
		name = "";
		text = "";
		answers.clear();
		breakAfterExecute = false;
		jumpAfterExecute = false;
		jumpTo = "";
	}
};

namespace Global {
	std::map<std::string,std::string> vars;
	bool last_if_result = true;
}

std::string getInput() {
	std::string inp;
	std::getline(std::cin,inp);
	return inp;
}

std::vector<std::string> splitBySpaces(std::string str) {
	std::vector<std::string> ret;
	std::string tmp;

	for(size_t i = 0; i < str.size(); ++i) {
		if(str[i] == ' ') {
			if(tmp != "") {
				ret.push_back(tmp);
			}
			tmp = "";
		}
		else {
			tmp += str[i];
		}
	}
	if(tmp != "") {
		ret.push_back(tmp);
	}

	return ret;
}

std::string checkVar(std::string str) {
	if(str[0] == '$') {
		str.erase(str.begin());
		if(Global::vars.count(str) > 0) {
			return Global::vars[str];
		}
		return "$" + str;
	}
	return str;
}

void handleSystemCommand(std::string str, size_t line) {
	auto code = splitBySpaces(str);


	if(code[0] == "empty") {
		return;
	}
	else if(code[0] == "var") {
		if(code.size() < 4) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		if(code[2] != "=" && code[2] != "is") {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		Global::vars[code[1]] = checkVar(code[3]);
		return;
	}
	else if(code[0] == "input") {
		if(code.size() < 3) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		if(code[1] != "->" && code[1] != "in" && code[1] != "to") {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		std::cout << ":";
		Global::vars[code[2]] = getInput();
		
		return;
	}
	else if(code[0] == "if") {
		if(code.size() < 3) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		if(code[1][0] != '(' && code[1][0] != '$') {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}

		std::string check;
		int pos = 0;
		if(code[1][0] == '(') {
			std::string expr;
			pos = 1;
			while(code[pos].back() != ')') {
				expr += code[pos] + ' ';
				if(pos+1 >= code.size()) {
					std::cout << "Error, no closing brace in line " << line << "\n";
					std::exit(0);
				}
				++pos;
			}
			expr += code[pos];
			expr.pop_back();
			expr.erase(expr.begin());

			auto tokens = splitBySpaces(expr);
			if(tokens.size() != 3) {
				std::cout << "Error, too long expression in line " << line << "\n";
				std::exit(0);
			}

			std::string left = checkVar(tokens[0]);
			std::string right = checkVar(tokens[2]);
			std::string operat = checkVar(tokens[1]);

			if(operat == "==") {
				check = left == right ? "true" : "false";
			}
			else if(operat == "!=") {
				check = left != right ? "true" : "false";
			}
			else {
				std::cout << "Error, unknown operator " << operat << " in line " << line << "\n";
				std::exit(0);
			}
		}
		else if(code[1][0] == '$') {
			check = checkVar(code[1]);
			pos = 1;
		}
		++pos;
		if(code[pos][0] == '<') {
			std::string cd;
			while(code[pos].back() != '>') {
				cd += code[pos] + ' ';
				if(pos+1 >= code.size()) {
					std::cout << "Error, no closing \">\" in line " << line << "\n";
					std::exit(0);
				}
				++pos;
			}
			cd += code[pos];
			cd.pop_back();
			cd.erase(cd.begin());

			if(check == "true" || check == "TRUE" || check == "True" || check == "1") {
				handleSystemCommand(cd,line);
			}


		}
		else {
			std::cout << "Error, no copund component for if statment in line " << line << "\n";
		}

		
		return;
	}
	else if(code[0] == "sleep") {
		if(code.size() < 2) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		std::string v = checkVar(code[1]);
		try {
			std::stoi(v);
		}
		catch(std::exception e) {
			std::cout << "Error, not int given at sleep statement! Got \" " << v << "\" in line " << line << "\n";
			std::exit(0);
		}
		sleep(std::stoi(v));

		return;
	}
	else if(code[0] == "sleep") {
		if(code.size() < 1) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
			
		clear();

		return;
	}
	else {
		std::cout << "Error, unknown command \"" << code[0] << "\" in line " << line << "\n";
	}
}

std::vector<Question> questions;

void parse(std::string fileName) {
	bool inQuestion = false;
    std::string source = read(fileName);
	source += "\n"; //savety
	auto vec = intoLines(source);
	Question qes;
	for(size_t i = 0; i < vec.size(); ++i) {
		if(vec[i][0] == '[') {
			if(qes.name != "") {
				questions.push_back(qes);
			}
			qes.clear();
			for(size_t j = 1; vec[i][j] != ']'; ++j) {
				if(j+1 >= vec[i].size()) {
					std::cout << "Error, missing \"]\" (line:" << i << "\n";
					std::exit(0);
				}
				qes.name += vec[i][j];
			}
			inQuestion = true;
			continue;
		}
		else if(vec[i][0] == '{') {
			if(!inQuestion) {
				std::cout << "Error, found \"{\" outside of a question (line:" << i << ")\n";
				std::exit(0);
			}
			Answer tmpAnswer;
			size_t toSkip = 0;
			for(size_t j = 1; vec[i][j] != '}'; ++j) {
				if(vec[i].size() < j+1) {
					std::cout << "Error, missing \"}\" (line:" << i << "\n";
					std::exit(0);
				}
				if(vec[i][j+1] == '}') {
					tmpAnswer.name += vec[i][j];
					toSkip = j+2;
					break;
				}

				tmpAnswer.name += vec[i][j];
			}
			for(size_t j = toSkip+1; j < vec[i].size() ; ++j) {
				tmpAnswer.jumpTo += vec[i][j];
			}

			/*
			if(tmpAnswer.name == "respawn") {
				std::cout << "[/" << tmpAnswer.jumpTo << "]\n";
			}
			*/


			if(tmpAnswer.name == "<empty>" || tmpAnswer.name == "<none>" || tmpAnswer.name == "<void>" || tmpAnswer.name == "<>" || tmpAnswer.name == "") {
				if(tmpAnswer.jumpTo == "") {
					//std::cout << "[" << qes.name << " = breakAfterExecute]\n";
					qes.breakAfterExecute = true;
				}
				else {
					qes.jumpAfterExecute = true;
					qes.jumpTo = tmpAnswer.jumpTo;
				}
			}

			if(tmpAnswer.name == "respawn") {
				//std::cout << "[/" << tmpAnswer.jumpTo << "]\n";
			}

			qes.answers.push_back(tmpAnswer);
		}
		else {
			if(qes.name != "" && !inQuestion) {
				std::cout << "Error, Unknown text in line " << i << "\n";
				std::exit(0);
			}
			qes.text += vec[i] + "\n";
		}

		if(i+1 >= vec.size() && qes.name != "") {
			questions.push_back(qes);
		}
	}
	
}



Question& find(std::string name) {
	if(name == "") {
		std::exit(0);
	}
	for(size_t i = 0; i < questions.size(); ++i) {
		if(questions[i].name == name) {
			return questions[i];
		}
	}
	std::cout << "Error, failed to find \"" << name << "\"\n";
	std::exit(0);
}

bool is_special_key(char ch) {
	const char spk[] = {
		' ', '\n', '+', '~', '*','#','\'','\\','!','"','^','#','-','_','.',';',':',',','<','>','|','/','%','&','(',')','[',']','{','}','`','?'
	};

	for(size_t i = 0; i < sizeof(spk); ++i) {
		if(ch == spk[i]) {
			return true;
		}
	}
	return false;
}

std::string replaceVars(std::string str) {
	int beg = -1;
	std::string tmp;
	std::string ret;
	for(size_t i = 0; i < str.size(); ++i) {
		if(str[i] == '$') {
			beg = i;
		}
		else if(is_special_key(str[i]) && beg != -1) {
			ret += Global::vars[tmp] + str[i];
			tmp = "";
			beg = -1;
		}
		else if(beg != -1) {
			tmp += str[i];
		}
		else {
			ret += str[i];
		}
	}
	return ret;
}

void run() {
	Question current = questions[0];

	for(;;) {
		//std::cout << "[//" << current.name << "]\n";
		std::string out;

		auto split = intoLines(current.text);
		for(size_t i = 0; i < split.size(); ++i) {
			std::string with = replaceVars(split[i]);
			if(with[0] == '<' && with.back() == '>') {
				with.erase(with.begin());
				with.pop_back();
				handleSystemCommand(with,i); // i is not the line but eh who cares about good error messages.... right?
			}
			else {
				std::cout << with << "\n";
			}
		}

		if(current.jumpAfterExecute) {
			current = find(current.jumpTo);
			continue;
		}
		else if(current.breakAfterExecute) {
			//std::cout << "[quit]\n";
			std::exit(0);
		}

		for(size_t i = 0; i < current.answers.size(); ++i) {
			std::string o = replaceVars(current.answers[i].name);
			std::cout << "[" << i+1 << "] " << o << "\n";
		}

		std::string inp = getInput();

		try {
			std::stoi(inp);
		}
		catch(std::exception& e) {
			inp = "1";
		}

		if(inp == "" && std::stoi(inp) > current.answers.size()) {
			//std::cout << "[again]\n";
		}
		else {
			//std::cout << "[" << current.answers[std::stoi(inp)-1].jumpTo << "]\n";
			current = find(current.answers[std::stoi(inp)-1].jumpTo);
		}
	}
}

int main(int argc, char** argv) {
	if(argc == 1) {
		std::cout << "A file must be given!\n";
		return 1;
	}
	parse(std::string(argv[1]));
	run();
}