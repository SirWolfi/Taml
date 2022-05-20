#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <map>
#include <stdio.h>
#include <filesystem>
#include <chrono>
#include <stack>
#include <optional>

#include "InI++/Inipp.hpp"


#ifdef _WIN32
#define OS_ "Windows"
#include <windows.h>

#define DP "\\"

void tsleep(int milliseconds) {
	Sleep(milliseconds); // Windows users often have problems with std::thread so give them their API instead...
}

void clear() {
	system("cls");
}
#elif defined(__linux__)
#define OS_ "Linux"
#include <thread> 

#define DP "/"

void tsleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


void clear() {
	system("clear");
}
#else
#define OS_ "Unknown"

#define DP "/" // hopefully this will work? idk

void tsleep(int milliseconds) {
	// Heh, hello mac/BSD/wtf users
}

void clear() {

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
	Question current;
	std::vector<Question> questions;
	int text_speed = 0;
	std::stack<std::filesystem::path> current_path;
}

std::string getInput() {
	std::string inp;
	std::getline(std::cin,inp);
	return inp;
}

std::vector<std::string> splitBySpaces(std::string str, bool nline = false, bool keep = false) {
	std::vector<std::string> ret;
	std::string tmp;

	for(size_t i = 0; i < str.size(); ++i) {
		if(str[i] == ' ' || str[i] == '\t' || (str[i] == '\n' && nline)) {
			if(tmp != "") {
				if(keep) {
					ret.push_back(tmp + str[i]);
				}
				else {
					ret.push_back(tmp);
				}
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

Question& find(std::string name) {
	if(name == "") {
		std::exit(0);
	}
	for(size_t i = 0; i < Global::questions.size(); ++i) {
		if(Global::questions[i].name == name) {
			return Global::questions[i];
		}
	}
	std::cout << "Error, failed to find \"" << name << "\"\n";
	std::exit(0);
}

bool isInt(std::string str) {
	try {
		std::stoi(str);
	}
	catch(...) {
		return false;
	}
	return true;
}

char first_char(std::string str) {
	if(str.size() == 0) {
		return ' ';
	}
	int p = 0;
	while(str[p] == ' ' || str[p] == '\t') {
		++p;
		if(p == str.size()) {
			return ' ';
		}
	}
	return str[p];
}
std::string rm_begin_end_spaces(std::string str) {
	if(str.size() == 0) {
		return str;
	}
	int p = 0;
	while(str[p] == ' ' || str[p] == '\t') {
		++p;
		if(p == str.size()) {
			return "";
		}
	}

	str.erase(str.begin(),str.begin()+p); // remove spaces/tabs an the front

	p = str.size()-1;
	while(str[p] == ' ' || str[p] == '\t') {
		str.pop_back(); // removes spaces/tabs on the back
		p = str.size()-1;
	}

	return str;
}

bool handleSystemCommand(std::string str, size_t line) {
	auto code = splitBySpaces(str);


	if(code[0] == "empty") {
		return false;
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
		return false;
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
		
		return false;
	}
	else if(code[0] == "if") {
		bool jmp = false;
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
				if(left == right)
					check = "true";
				else
					check = "false";
			}
			else if(operat == "!=") {
				if(left != right)
					check = "true";
				else
					check = "false";
			}
			else if(operat == ">") {
				if(!isInt(left)) {
					std::cout << "Error, integer operator on an string: \"" << left << "\" in line " << line << "\n";
					std::exit(0);
				}
				if(!isInt(right)) {
					std::cout << "Error, integer operator on an string: \"" << right << "\" in line " << line << "\n";
					std::exit(0);
				}
				if(std::stoi(left) > std::stoi(right))
					check = "true";
				else
					check = "false";
			}
			else if(operat == "<") {
				if(!isInt(left)) {
					std::cout << "Error, integer operator on an string: \"" << left << "\" in line " << line << "\n";
					std::exit(0);
				}
				if(!isInt(right)) {
					std::cout << "Error, integer operator on an string: \"" << right << "\" in line " << line << "\n";
					std::exit(0);
				}
				if(std::stoi(left) < std::stoi(right))
					check = "true";
				else
					check = "false";
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
				jmp = handleSystemCommand(cd,line);
				Global::last_if_result = true;
			}
			else {
				Global::last_if_result = false;
			}


		}
		else {
			std::cout << "Error, no copound component for if statment in line " << line << "\n";
		}

		
		return jmp;
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
		tsleep(std::stoi(v));

		return false;
	}
	else if(code[0] == "clear") {
		if(code.size() < 1) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
			
		clear();

		return false;
	}
	else if(code[0] == "else") {
		if(code.size() < 2) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		if(code[1][0] != '<') {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		std::string cd;
		int pos = 1;
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
		bool jmp = false;
		if(!Global::last_if_result) {
			jmp = handleSystemCommand(cd,line);
		}
		return jmp;
	}
	else if(code[0] == "save") {
		if(code.size() < 6) {
			std::cout << "Error, wrong args in line " << line << "\n"; // <save var as hello -> hi.tamlsave>
			std::exit(0);
		}
		if(code[2] != "as" && code[2] != "=>") {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		if(code[4] != "to" && code[4] != "->" && code[4] != "in") {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}


		std::string name = checkVar(code[1]);
		std::string in_name = checkVar(code[3]);
		std::string file = checkVar(code[5]);

		if(!std::filesystem::exists(file)) {
			std::ofstream os;
			os.open(file, std::ios::trunc | std::ios::app);
			os.close();
		}

		IniFile inf = IniFile::from_file(file);
		if(!inf) {
			if(inf.error_msg() != "Empty file\n") {
				std::cout << inf.error_msg();
				std::exit(0);
			}
		}
		inf.set(in_name,IniHelper::to_element(Global::vars[name]),"Tamlsave");
		inf.to_file(file);
		return false;
	}
	else if(code[0] == "get") { // <get <key> from <file> to <var>>
		if(code.size() < 6) {
			std::cout << "Error, wrong args in line " << line << "\n"; // <save var as hello -> hi.tamlsave>
			std::exit(0);
		}
		if(code[2] != "from" && code[2] != "<-") {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		if(code[4] != "to" && code[4] != "=>" && code[4] != "as") {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}


		std::string key = checkVar(code[1]);
		std::string file = checkVar(code[3]);
		std::string var = checkVar(code[5]);
		IniFile inf = IniFile::from_file(file);
		if(!inf) {
			std::cout << inf.error_msg() << "\n";
			std::exit(0);
		}
		Global::vars[var] = inf.get(key,"Tamlsave").to_string();
		inf.to_file(file); // just in case
		return false;
	}
	else if(code[0] == "jump") {
		if(code.size() < 2) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}
		std::string to_question = "";

		for(size_t i = 1; i < code.size(); ++i) {
			to_question = code[i] + " ";
		}
		to_question.pop_back();

		Global::current = find(to_question);

		return true;
	}
	else if(code[0] == "textspeed") {
		if(code.size() < 2) {
			std::cout << "Error, wrong args in line " << line << "\n";
			std::exit(0);
		}

		try {
			Global::text_speed = std::stoi(checkVar(code[1]));
		}
		catch(std::exception e) {
			std::cout << "Error, not an int given as text speed!\n";
			std::exit(0);
		}
		return false;
	}
	else {
		std::cout << "Error, unknown command \"" << code[0] << "\" in line " << line << "\n";
		std::exit(0);
	}
}

// std::vector<Question> questions; <- moved to Global namespace!

std::vector<Question> parse(std::string fileName, std::filesystem::path ph) {
	Global::current_path.push(ph);
	std::vector<Question> ret;
	bool inQuestion = false;
    std::string source = read(ph.string() + DP + fileName);
	source += "\n"; //savety
	auto vec = intoLines(source);
	Question qes;
	for(size_t i = 0; i < vec.size(); ++i) {
		if(first_char(vec[i]) == '[') {
			vec[i] = rm_begin_end_spaces(vec[i]);
			if(qes.name != "") {
				ret.push_back(qes);
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
		else if(first_char(vec[i]) == '{') {
			vec[i] = rm_begin_end_spaces(vec[i]);
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
		else if(first_char(vec[i]) == '%') {
			vec[i] = rm_begin_end_spaces(vec[i]);
			std::string file = vec[i];
			file.erase(file.begin());
			if(!std::filesystem::exists(Global::current_path.top().string() + file)) {
				std::cout << "Error, unknown file \"" << file << "\" to include in line " << i << "\n";
				std::exit(0);
			}

			if(qes.name != "") {
				ret.push_back(qes);
			}
			qes.clear();
			inQuestion = false;

			auto add = parse(file,Global::current_path.top());
			for(auto i : add) {
				ret.push_back(i);
			}
		}
		else if(first_char(vec[i]) == '#') {
			; // comment!
		}
		else {
			if(qes.name != "" && !inQuestion) {
				std::cout << "Error, unexpected text in line " << i << "\n";
				std::exit(0);
			}
			qes.text += vec[i] + "\n";
		}
		if(i+1 >= vec.size() && qes.name != "") {
			ret.push_back(qes);
		}
	}
	Global::current_path.pop();
	return ret;
	
}

bool is_special_key(char ch) {
	const char spk[] = {
		' ', '\n', '+', '~', '*','#','\'','\\','!','"','^','#','-',/*'_','.',*/';',/*':',*/',','<','>','|','/','%','&','(',')','[',']','{','}','`','?'
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
	if(tmp != "" && beg != -1) {
		ret += Global::vars[tmp];
	}
	return ret;
}

// Question current; <- moved to Global namespace!
void run() {
	Global::current = Global::questions[0];

	for(;;) {
		std::string out;

		//Yes, they are useless, yes i like them, yes they'll stay ;3
		Global::vars["__TEXT_SPEED__"] = std::to_string(Global::text_speed);
		Global::vars["__QUESTION__"] = Global::current.name;
		Global::vars["__QUESTIONS__"] = Global::questions.size();
		Global::vars["__START__"] = Global::questions.front().name;
		Global::vars["__PATH__"] = std::filesystem::current_path().string();
		Global::vars["__ROOT__"] = std::filesystem::current_path().root_directory();
		Global::vars["__OS__"] = OS_;
		// Global::vars["__LAST__"] <- will be set before jumping

		auto split = intoLines(Global::current.text);
		bool cont = false;
		for(size_t i = 0; i < split.size(); ++i) {
			std::string with = replaceVars(split[i]);
			if(with[0] == '<' && with.back() == '>') {
				with.erase(with.begin());
				with.pop_back();
				bool result = handleSystemCommand(with,i); // i is not the line but eh who cares about good error messages.... right?
				if(result) {
					cont = true;
					break;
				}
			}
			else {
				if(Global::text_speed == 0) {
					std::cout << with << "\n";
				}
				else {
					for(auto i : with) {
						std::cout << i;
						std::cout.flush();
						tsleep(Global::text_speed);
					}
					std::cout << "\n";
				}
			}

		}
		if(cont) {
			cont = false;
			continue;
		}

		if(Global::current.jumpAfterExecute) {
			Global::vars["__LAST__"] = Global::current.name;
			Global::current = find(checkVar(Global::current.jumpTo));
			continue;
		}
		else if(Global::current.breakAfterExecute) {
			std::exit(0);
		}

		for(size_t i = 0; i < Global::current.answers.size(); ++i) {
			std::string o = replaceVars(Global::current.answers[i].name);
			std::cout << "[" << i+1 << "] " << o << "\n";
		}

		std::string inp = getInput();

		try {
			std::stoi(inp);
		}
		catch(std::exception& e) {
			inp = "1";
		}

		if(inp == "" && std::stoi(inp) > Global::current.answers.size()) {
			// something was here but I forgot, eh
		}
		else {
			Global::vars["__LAST__"] = Global::current.name;
			Global::current = find(checkVar(Global::current.answers[std::stoi(inp)-1].jumpTo));
		}
	}
}

int main(int argc, char** argv) {
	if(argc == 1) {
		std::cout << "A file must be given!\n";
		return 1;
	}

	std::string path = std::filesystem::current_path().string() + DP + std::filesystem::path(std::string(argv[1])).remove_filename().string();
	std::string file = std::filesystem::path(std::string(argv[1])).filename();
	Global::questions = parse(file,path);
	
	run();
}
