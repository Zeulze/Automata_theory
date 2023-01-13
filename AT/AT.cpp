#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

const string PROGRAM_FILE = "C:/Users/Zeulze/Desktop/AT/AT/program.txt";

const string names[35] = {
		"name", "number", "realNumber", "+", "-",
		"(", ")", "[", "]",
		"*", "/", "=", "<",
		"<=", ">", ">=", ";",
		",", ".", "begin", "end",
		"if", "then", "while", "read",
		"print", "int", "float", "else",
		"intArr", "floatArr", "intDoubleArr",
		"floatDoubleArr", "{", "}" 
};

class LexemsAlphabet {
public:
	const char singleSymbols[15] = {
		'+', '-', '(', ')',
		'[', ']', '*', '/',
		'=', '<', '>', ';',
		',', '{', '}'
	};

	const string compositeSymbols[2] = {
		"<=", ">="
	};

	const string serviceWords[14] = {
		"begin", "end",
		"if","then","while","read",
		"print","int","float","else",
		"intArr", "floatArr", "intDoubleArr",
		"floatDoubleArr"
	};

	string program;

};

class SingleLexem {
public:
	string Name = "";
	string ValueSymbol = "";
	
	vector<SingleLexem> vectorLex;

	vector<SingleLexem> parent;

	int size = 0;
	int index = -1;

	double ValueNumber = INFINITY;
	int X = -1;
	int Y = -1;
	bool Mark = false;
};

class Lexer {
private:
	int pos = 0;
	int row = 1;
	vector<SingleLexem> Token;
public:

	vector<SingleLexem> Analyzer(LexemsAlphabet lexems)
	{
		string s;
		string program;
		ifstream in(PROGRAM_FILE);
		if (in.is_open())
		{
			while (getline(in, s)) {
				program = s;
				if (!AnalyzeComponents(program, lexems)) break;
				pos = 0;
				row += 1;
			}

			int helperY = 1;
			int helperX = 1;
			for (int i = 0; i < Token.size(); i++) {
				while (Token[i].Y > helperY) {
					helperY += 1;
					helperX = 1;
				}

				Token[i].X = helperX;
				helperX += 1;
			}

		}
		return Token;
	}

	int AnalyzeComponents(string program, LexemsAlphabet lexems) {
		bool trigger = false;
		while (pos < program.length()) {
			SingleLexem newLexem;
			newLexem.Y = row;
			//NUMBER HANDLER
			while (program[pos] < 58 && program[pos] > 47) {
				newLexem.Name = names[1];
				if (newLexem.ValueNumber == INFINITY) newLexem.ValueNumber = program[pos] - '0';
				else {
					newLexem.ValueNumber = newLexem.ValueNumber * 10 + (program[pos] - '0');
				}
				pos++;
			}

			//FLOAT NUMBER HANDLER
			if (program[pos] == '.') {
				if (newLexem.Name == names[1]) {
					pos++;
					if (program[pos] < 58 && program[pos] > 47) {
						double d = 1;
						while (program[pos] < 58 && program[pos] > 47) {
							newLexem.Name = names[2];
							d *= 0.1;
							newLexem.ValueNumber = newLexem.ValueNumber + (program[pos] - '0') * d;
							pos++;
						}
					}
					else {
						cout << "Error: " << row << ":" << pos + 1 << endl;
						break;
					}
				}
				else {
					cout << "* can be placed only behind a number: " << row << ":" << pos + 1 << endl;
					break;
				}

			}

			//SINGLE SYMBOLS HANDLER
			for (int i = 0; i < 15; i++) {
				if (program[pos] == lexems.singleSymbols[i]) {
					if (newLexem.Name == "") {
						newLexem.Name = lexems.singleSymbols[i];
						newLexem.ValueSymbol = lexems.singleSymbols[i];
						pos++;
					}
				}
			}

			//COMPOSITE SYMBOLS HANDLER
			for (int i = 0; i < 2; i++) {
				string word = newLexem.ValueSymbol;
				word.push_back(program[pos]);
				if (word == lexems.compositeSymbols[i]) {
					newLexem.Name = word;
					newLexem.ValueSymbol = word;
					pos++;
				}
			}

			//NAMES HANDLER
			if (newLexem.Name != names[1]) {
				if (newLexem.Name == "" && !(program[pos] < 58 && program[pos] > 47)) {
					while ((program[pos] < 91 && program[pos] > 64) ||
						(program[pos] < 123 && program[pos] > 96) ||
						(program[pos] < 58 && program[pos] > 47))
					{
						newLexem.Name = names[0];
						newLexem.ValueSymbol.push_back(program[pos]);
						pos++;
					}
				}
			}

			//SERVICE WORDS HANDLER
			if (newLexem.Name == "name") {
				for (int i = 0; i < 14; i++) {
					if (newLexem.ValueSymbol == lexems.serviceWords[i]) {
						newLexem.Name = lexems.serviceWords[i];
						newLexem.ValueSymbol = lexems.serviceWords[i];
					}
				}
			}

			//COMMS HANDLER
			if (newLexem.Name == "/") {
				if (program[pos] == '/') {
					newLexem.Name = "";
					while (pos < program.length()) {
						pos++;
					}
				}
			}

			//space handler
			while (program[pos] == ' ') {
				pos++;
				trigger = true;
			}

			//wrong lexeme handler
			if (newLexem.Name != "") {
				Token.push_back(newLexem);
				trigger = false;
			}
			else if (!trigger) {
				cout << "Wrong lexem: " << row << ":" << pos + 1;
				return 0;
			}
		}
		return 1;

	}
};

class Ops {
private:
	void generationOperations() {
		while (Stack[pos].Name != ";") {
			if (Stack[pos].Name == "name" ||
				Stack[pos].Name == "number" ||
				Stack[pos].Name == "realNumber")
			{
				programOPS.push_back(Stack[pos]);
			}
			if (Stack[pos].Name == "mass") {
				programOPS.push_back(Stack[pos]);
				pos++;
				programOPS.push_back(Stack[pos]);
			}
			if (Stack[pos].Name == "(") {
				opStack.push(Stack[pos]);
			}
			if (Stack[pos].Name == ")") {
				while (opStack.top().Name != "(") {
					programOPS.push_back(opStack.top());
					opStack.pop();
				}
				opStack.pop();
			}
			if (Stack[pos].Name == "+" ||
				Stack[pos].Name == "-" ||
				Stack[pos].Name == "*" ||
				Stack[pos].Name == "/")
			{
				if (opStack.empty() ||
					(opStack.top().ValueNumber < Stack[pos].ValueNumber))
				{
					opStack.push(Stack[pos]);

				}
				else {
					while (!(opStack.empty() || (opStack.top().ValueNumber < Stack[pos].ValueNumber)))
					{
						programOPS.push_back(opStack.top());
						opStack.pop();
					}
					opStack.push(Stack[pos]);

				}
			}
			pos++;
		}
	}
	void generationPrint() {
		while (Stack[pos].ValueSymbol != ";") {
			if (Stack[pos].Name == "name" ||
				Stack[pos].Name == "number" ||
				Stack[pos].Name == "realNumber")
			{
				programOPS.push_back(Stack[pos]);
			}
			if (Stack[pos].Name == "mass") {
				programOPS.push_back(Stack[pos]);;
				pos++;
				programOPS.push_back(Stack[pos]);
			}
			if (Stack[pos].Name == "(") {
				opStack.push(Stack[pos]);
			}
			if (Stack[pos].Name == ")") {
				while (opStack.top().Name != "(") {
					programOPS.push_back(opStack.top());
					opStack.pop();
				}
				opStack.pop();
			}
			if (Stack[pos].Name == "+" ||
				Stack[pos].Name == "-" ||
				Stack[pos].Name == "*" ||
				Stack[pos].Name == "/")
			{
				if (opStack.empty() ||
					(opStack.top().ValueNumber < Stack[pos].ValueNumber))
				{
					opStack.push(Stack[pos]);

				}
				else {
					while (!(opStack.empty() || (opStack.top().ValueNumber < Stack[pos].ValueNumber)))
					{
						programOPS.push_back(opStack.top());
						opStack.pop();
					}
					opStack.push(Stack[pos]);

				}
			}
			pos++;
		}
	}
	void markZeroHandler() {
		programOPS.push_back(Stack[pos]);
		while (Stack[pos].Name != "m1") {
			if (Stack[pos].Name == "name" ||
				Stack[pos].Name == "number" ||
				Stack[pos].Name == "floatNumber")
			{
				programOPS.push_back(Stack[pos]);
			}
			if (Stack[pos].Name == "mass") {
				programOPS.push_back(Stack[pos]);
				programOPS.push_back(Stack[pos + 1]);
				pos++;

			}
			if (Stack[pos].Name == "<" ||
				Stack[pos].Name == ">" ||
				Stack[pos].Name == ">=" ||
				Stack[pos].Name == "<=") {
				opStack.push(Stack[pos]);
			}
			pos++;
		}
	}
	void markOneHandler() {
		while (Stack[pos].Name != "m1") {
			if (Stack[pos].Name == "name" ||
				Stack[pos].Name == "number" ||
				Stack[pos].Name == "floatNumber")
			{
				programOPS.push_back(Stack[pos]);
			}
			if (Stack[pos].Name == "mass") {
				programOPS.push_back(Stack[pos]);
				programOPS.push_back(Stack[pos + 1]);
				pos++;
			}
			if (Stack[pos].Name == "<" ||
				Stack[pos].Name == ">" ||
				Stack[pos].Name == ">=" ||
				Stack[pos].Name == "<=") {
				opStack.push(Stack[pos]);
			}
			pos++;
		}
		if (!opStack.empty()) {
			programOPS.push_back(opStack.top());
			opStack.pop();
		}
		programOPS.push_back(Stack[pos]);
		createJump("jf");
		pos++;
	}
	void createJump(string name) {
		SingleLexem jump;
		jump.Name = name;
		jump.ValueSymbol = name;
		programOPS.push_back(jump);
	}
public:
	vector<SingleLexem> intNames;
	vector<SingleLexem> floatNames;
	vector<SingleLexem> floatArrNames;
	vector<SingleLexem> intArrNames;
	vector<SingleLexem> Stack;
	stack<SingleLexem> opStack;
	vector<SingleLexem> programOPS;
	string nameFlag = "";
	int pos = 0;

	void generationOPS() {
		
		while (pos < Stack.size()) {
			if (Stack[pos].Name == "m0") {
				markZeroHandler();
				while (!opStack.empty()) {
					programOPS.push_back(opStack.top());
					opStack.pop();
				}
				programOPS.push_back(Stack[pos]);
				createJump("jf");
				pos++;
			}
			if (!Stack[pos].Mark) {
				markOneHandler();
			}
			if (Stack[pos].Name == "m2") {
				programOPS.push_back(Stack[pos]);
				createJump("j");
			}
			if (Stack[pos].Name == "m3") {
				programOPS.push_back(Stack[pos]);
			}
			if (Stack[pos].Name == "m4") {
				programOPS.push_back(Stack[pos]);
			}
			if (Stack[pos].Name == "r") {
				if (programOPS[pos + 1].Name != "mass") {
					programOPS.push_back(Stack[pos + 1]);
					programOPS.push_back(Stack[pos]);
					pos += 1;
				}
				else {
					programOPS.push_back(Stack[pos + 1]);
					programOPS.push_back(Stack[pos + 2]);
					programOPS.push_back(Stack[pos]);
					pos += 2;
				}
				
			}
			if (Stack[pos].Name == "w") {
				SingleLexem w = Stack[pos];
				generationPrint();
				while (!opStack.empty()) {
					programOPS.push_back(opStack.top());
					opStack.pop();
				}
				programOPS.push_back(w);
			}
			if (Stack[pos].Name == "mass") {
				programOPS.push_back(Stack[pos]);
				pos++;
				programOPS.push_back(Stack[pos]);
				pos++;
				generationOperations();
				while (!opStack.empty()) {
					programOPS.push_back(opStack.top());
					opStack.pop();
				}
				SingleLexem equality;
				equality.Name = "=";
				equality.ValueSymbol = "=";
				programOPS.push_back(equality);
			}
			if (Stack[pos].Mark && Stack[pos].Name == "name") {
				generationOperations();
				while (!opStack.empty()) {
					programOPS.push_back(opStack.top());
					opStack.pop();
				}
				SingleLexem equality;
				equality.Name = "=";
				equality.ValueSymbol = "=";
				programOPS.push_back(equality);
			}
			pos++;
		}
	}
	
};

class Parser {
private:
	vector<SingleLexem> Token;
	
	int pos = 0;
	bool trigger = false;
	

	void P() {
		if (Token[pos].Name != "{") {

			enterError(Token[pos].X, Token[pos].Y);
		}
		pos++;
		A();
		if (Token[pos].Name != "}") {
			enterError(Token[pos].X, Token[pos].Y);
		}
		createAMark("}");
	}

	void A() {
		if (Token[pos].Name == "name") {
			Token[pos].Mark = true;
			prog.Stack.push_back(Token[pos]);
			pos++;
			H();
			if (Token[pos].Name != "=") {
				
				enterError(Token[pos].X, Token[pos].Y);
			}
			pos++;
			S();
			Z();
			return;
		}
		if (Token[pos].Name == "if") {
			createAMark("m4");
			pos++;
			C();
			if (Token[pos].Name != "then") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			pos++;
			B();
			E();
			Z();
			return;
		}
		if (Token[pos].Name == "while") {
			createAMark("m0");
			pos++;
			C();
			B();
			Z();
			return;
		}
		if (Token[pos].Name == "read") {
			createAMark("r");
			pos++;
			if (Token[pos].Name != "(") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			pos++;
			if (Token[pos].Name != "name") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			prog.Stack.push_back(Token[pos]);
			pos++;
			H();
			if (Token[pos].Name != ")") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			pos++;
			return;
		}
		if (Token[pos].Name == "print") {
			createAMark("w");
			pos++;
			if (Token[pos].Name != "(") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			pos++;
			S();
			if (Token[pos].Name != ")") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			pos++;
			if (Token[pos].Name != ";") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			prog.Stack.push_back(Token[pos]);
			pos++;
			return;
		}
		if (Token[pos].Name == "int") {
			pos++;
			prog.nameFlag = "int";
			R();
			A();
			return;
		}
		if (Token[pos].Name == "float") {
			pos++;
			prog.nameFlag = "float";
			R();
			A();
			return;
		}
		if (Token[pos].Name == "intArr") {
			pos++;
			prog.nameFlag = "intArr";
			R();
			A();
			return;
		}
		if (Token[pos].Name == "floatArr") {
			pos++;
			prog.nameFlag = "floatArr";
			R();
			A();
			return;
		}
	}

	void R() {
		if (Token[pos].Name != "name") {
			enterError(Token[pos].X, Token[pos].Y);
		}
		addVariable();
		pos++;
		N();
		M();
		if (Token[pos].Name == ";") {
			pos++;
			return;
		}
		else {
			enterError(Token[pos].X, Token[pos].Y);
		}
	}

	void M() {
		if (Token[pos].Name == ",") {
			pos++;
			if (Token[pos].Name != "name") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			addVariable();
			pos++;
			N();
			M();
			return;
		}
	}

	void N() {
		if (Token[pos].Name == "[") {
			pos++;
			if (Token[pos].Name != "number") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			initMass();
			pos++;
			if (Token[pos].Name != "]") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			pos++;
			return;
		}
	}

	void Q() {
		if (Token[pos].Name == ";") {
			pos++;
			A();
			Q();
			return;
		}
	}

	void C() {
		if (Token[pos].Name == "(") {
			Token[pos].ValueNumber = 0;
			prog.Stack.push_back(Token[pos]);
			pos++;
			S();
			if (Token[pos].Name != ")") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			pos++;
			V();
			U();
			D();
			return;
		}
		if (Token[pos].Name == ";") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			A();
			Q();
			V();
			U();
			D();
			return;
		}
		if (Token[pos].Name == "name") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			H();
			V();
			U();
			D();
			return;
		}
		if (Token[pos].Name == "number" || Token[pos].Name == "realNumber") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			V();
			U();
			D();
			return;
		}
		else {
			enterError(Token[pos].X, Token[pos].Y);
		}
	}

	void D() {
		if (Token[pos].Name == "<") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			S();
			Z();
			return;
		}
		if (Token[pos].Name == ">") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			S();
			Z();
			return;
		}
		if (Token[pos].Name == "<=") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			S();
			Z();
			return;
		}
		if (Token[pos].Name == ">=") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			S();
			Z();
			return;
		}
	}

	void S() {
		if (Token[pos].Name == "(") {
			Token[pos].ValueNumber = 0;
			prog.Stack.push_back(Token[pos]);
			pos++;
			S();
			if (Token[pos].Name != ")") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			prog.Stack.push_back(Token[pos]);
			pos++;
			V();
			U();
			return;
		}
		if (Token[pos].Name == ";") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			A();
			Q();
			V();
			U();
			return;
		}
		if (Token[pos].Name == "name") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			H();
			V();
			U();
			return;
		}
		if (Token[pos].Name == "number" || Token[pos].Name == "realNumber") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			V();
			U();
			return;
		}
		else {
			enterError(Token[pos].X, Token[pos].Y);
		}

	}

	void B() {
		if (Token[pos].Name == "begin") {
			createAMark("m1");
			pos++;
			A();
			Q();
			if (Token[pos].Name != "end") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			createAMark("m2");
			pos++;
			A();
			return;
		}
	}

	void U() {
		if (Token[pos].Name == "+") {	
			Token[pos].ValueNumber = 1;
			prog.Stack.push_back(Token[pos]);
			pos++;
			T();
			U();
			return;
		}
		if (Token[pos].Name == "-") {
			Token[pos].ValueNumber = 1;
			prog.Stack.push_back(Token[pos]);
			pos++;
			T();
			U();
			return;
		}
	}

	void T() {
		if (Token[pos].Name == "(") {
			Token[pos].ValueNumber = 0;
			prog.Stack.push_back(Token[pos]);
			pos++;
			S();
			if (Token[pos].Name != ")") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			prog.Stack.push_back(Token[pos]);
			pos++;
			V();
			return;
		}
		if (Token[pos].Name == ";") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			A();
			Q();
			V();
			return;
		}
		if (Token[pos].Name == "name") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			H();
			V();
			return;
		}
		if (Token[pos].Name == "number" || Token[pos].Name == "realNumber") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			V();
			return;
		}
		else {
			enterError(Token[pos].X, Token[pos].Y);
		}
	}

	void V() {
		if (Token[pos].Name == "*") {
			Token[pos].ValueNumber = 2;
			prog.Stack.push_back(Token[pos]);
			pos++;
			Z();
			V();
			return;
		}
		if (Token[pos].Name == "/") {
			Token[pos].ValueNumber = 2;
			prog.Stack.push_back(Token[pos]);
			pos++;
			Z();
			V();
			return;
		}
	}

	void Z() {
		if (Token[pos].Name == "(") {
			Token[pos].ValueNumber = 0;
			prog.Stack.push_back(Token[pos]);
			pos++;
			S();
			if (Token[pos].Name != ")") {
				enterError(Token[pos].X, Token[pos].Y);
			}
			prog.Stack.push_back(Token[pos]);
			pos++;
			return;
		}
		if (Token[pos].Name == ";") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			A();
			Q();
			return;
		}
		if (Token[pos].Name == "name") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			H();
			return;
		}
		if (Token[pos].Name == "number") {
			prog.Stack.push_back(Token[pos]);
			pos++;
			return;
		}
	}

	void E() {
		if (Token[pos].Name == "else") {
			createAMark("m3");
			pos++;
			B();
			return;
		}
	}

	void H() {
		if (Token[pos].Name == "[") {
			prog.Stack[prog.Stack.size() - 1].Name = "mass";
			pos++;
			S();
			L();
			return;
		}
	}

	void L() {
		if (Token[pos].Name == "]") {
			pos++;
			return;
		}
	}

	void initMass() {
		if (prog.nameFlag == "intArr") {
			for (int i = 0; i < prog.intArrNames.size(); i++) {
				if (Token[pos - 2].ValueSymbol == prog.intArrNames[i].ValueSymbol) {
					prog.intArrNames[i].size = Token[pos].ValueNumber;
					for (int j = 0; j < Token[pos].ValueNumber; j++) {
						SingleLexem newLex;
						newLex.ValueNumber = INFINITY;
						prog.intArrNames[i].vectorLex.push_back(newLex);
					}
				}
			}
		}
		if (prog.nameFlag == "floatArr") {
			for (int i = 0; i < prog.floatArrNames.size(); i++) {
				if (Token[pos - 2].ValueSymbol == prog.floatArrNames[i].ValueSymbol) {
					prog.floatArrNames[i].size = Token[pos].ValueNumber;
					for (int j = 0; j < Token[pos].ValueNumber; j++) {
						SingleLexem newLex;
						newLex.ValueNumber = INFINITY;
						prog.floatArrNames[i].vectorLex.push_back(newLex);
					}
				}
			}
		}
	}
	
	void addVariable() {
		checkArrName();
		if (prog.nameFlag == "int") prog.intNames.push_back(Token[pos]);
		if (prog.nameFlag == "float") prog.floatNames.push_back(Token[pos]);
		if (prog.nameFlag == "intArr") prog.intArrNames.push_back(Token[pos]);
		if (prog.nameFlag == "floatArr") prog.floatArrNames.push_back(Token[pos]);
	}

	void checkArrName() {
		for (int i = 0; i < prog.intNames.size(); i++) {
			if (Token[pos].ValueSymbol == prog.intNames[i].ValueSymbol) enterError(Token[pos].X, Token[pos].Y);
		}

		for (int i = 0; i < prog.floatArrNames.size(); i++) {
			if (Token[pos].ValueSymbol == prog.floatArrNames[i].ValueSymbol) enterError(Token[pos].X, Token[pos].Y);
		}

		for (int i = 0; i < prog.floatNames.size(); i++) {
			if (Token[pos].ValueSymbol == prog.floatNames[i].ValueSymbol) enterError(Token[pos].X, Token[pos].Y);
		}

		for (int i = 0; i < prog.intArrNames.size(); i++) {
			if (Token[pos].ValueSymbol == prog.intArrNames[i].ValueSymbol) enterError(Token[pos].X, Token[pos].Y);
		}
	}

	void enterError(int X, int Y) {
		cout << "Wrong lexem at position: " << Y << " " << X << endl;
		exit(1);
	}

	void createAMark(string name) {
		Token[pos].Name = name;
		Token[pos].ValueSymbol = name;
		Token[pos].Mark = true;
		prog.Stack.push_back(Token[pos]);
	}


public:
	Ops prog;
	Parser(vector<SingleLexem> Token) {
		this->Token = Token;
	}

	void analyze() {
		P();

	}

};

class Interpretator {
private:
	Ops ops;
	vector<SingleLexem> program;
	stack<SingleLexem> summator;
	stack<SingleLexem> stack;
	SingleLexem first;
	SingleLexem second;
	int pos = 0;
	bool helper = false;
public:
	Interpretator(vector<SingleLexem> program, Ops ops) {
		this->program = program;
		this->ops = ops;
		
	}

	SingleLexem findNumber(SingleLexem number) {
		for (int i = 0; i < ops.intNames.size(); i++) {
			if (number.ValueSymbol == ops.intNames[i].ValueSymbol) return ops.intNames[i];
		}

		for (int i = 0; i < ops.floatArrNames.size(); i++) {
			if (number.ValueSymbol == ops.floatArrNames[i].ValueSymbol) {
				if (number.Mark) ops.floatArrNames[i].Mark = true;
				return ops.floatArrNames[i];
			}
		}

		for (int i = 0; i < ops.floatNames.size(); i++) {
			if (number.ValueSymbol == ops.floatNames[i].ValueSymbol) return ops.floatNames[i];
		}

		for (int i = 0; i < ops.intArrNames.size(); i++) {
			if (number.ValueSymbol == ops.intArrNames[i].ValueSymbol) {
				if (number.Mark) ops.intArrNames[i].Mark = true;
				return ops.intArrNames[i];
			}
		}
		cout << "This variable is undefined" << " " << number.ValueSymbol << " " << number.Y << " " << number.X;
		exit(1);
		return number;
	}
	void setNumber(SingleLexem number) {
		for (int i = 0; i < ops.intNames.size(); i++) {
			if (number.ValueSymbol == ops.intNames[i].ValueSymbol) 
			{ 
				ops.intNames[i].ValueNumber = number.ValueNumber;
				return; 
			}
		}

		for (int i = 0; i < ops.floatArrNames.size(); i++) {
			if (number.ValueSymbol == ops.floatArrNames[i].ValueSymbol) 
			{
				ops.floatArrNames[i].ValueNumber = number.ValueNumber;
				return;
			}
		}

		for (int i = 0; i < ops.floatNames.size(); i++) {
			if (number.ValueSymbol == ops.floatNames[i].ValueSymbol) 
			{
				ops.floatNames[i].ValueNumber = number.ValueNumber;
				return;
			}
		}

		for (int i = 0; i < ops.intArrNames.size(); i++) {
			if (number.ValueSymbol == ops.intArrNames[i].ValueSymbol) 
			{
				ops.intArrNames[i].ValueNumber = number.ValueNumber;
				return;
			}
		}

		cout << "This variable is undefined " << " " << number.ValueNumber << " " << number.Y << " " << number.X;
		exit(1);
	}
	void setArray(SingleLexem arr) {
		for (int i = 0; i < ops.intNames.size(); i++) {
			if (arr.ValueSymbol == ops.intNames[i].ValueSymbol)
			{
				ops.intNames[i] = arr;
				return;
			}
		}

		for (int i = 0; i < ops.floatArrNames.size(); i++) {
			if (arr.ValueSymbol == ops.floatArrNames[i].ValueSymbol)
			{
				ops.floatArrNames[i] = arr;
				return;
			}
		}

		for (int i = 0; i < ops.floatNames.size(); i++) {
			if (arr.ValueSymbol == ops.floatNames[i].ValueSymbol)
			{
				ops.floatNames[i] = arr;
				return;
			}
		}

		for (int i = 0; i < ops.intArrNames.size(); i++) {
			if (arr.ValueSymbol == ops.intArrNames[i].ValueSymbol)
			{
				ops.intArrNames[i] = arr;
				return;
			}
		}

		cout << "This variable is undefined " << " " << arr.ValueNumber << " " << arr.Y << " " << arr.X;
		exit(1);
	}
	void baseOperations() {
		if (program[pos].Name == "name") {
			if (program[pos].Name == "name") {
				stack.push(findNumber(program[pos]));
			}
		}
		if (program[pos].Name == "number" ||
			program[pos].Name == "floatNumber") {
			stack.push(program[pos]);
		}
		if (program[pos].Name == "mass" && program[pos].Mark) {
			stack.push(findNumber(program[pos]));
			pos++;
			if (program[pos].Name == "name") {
				if (findNumber(program[pos]).ValueNumber < stack.top().size) {
					stack.top().index = findNumber(program[pos]).ValueNumber;
					
					
				}
				else {
					cout << program[pos].ValueNumber << " " << stack.top().size;
					cout << "Out of index of array: " << stack.top().Name;
					exit(1);
				}
			}
			else if (program[pos].Name == "number" ||
				program[pos].Name == "realNumber")
			{
				if (program[pos].ValueNumber < stack.top().size) {
					stack.top().index = program[pos].ValueNumber;
					
				}
				else {
					cout << program[pos].ValueNumber << " " << stack.top().size;
					cout << "Out of index of array: " << stack.top().Name;
					exit(1);
				}
			}	
		}
		if (program[pos].Name == "mass" && !program[pos].Mark) {
			SingleLexem helperLex = findNumber(program[pos]);
			pos++;
			if (program[pos].Name == "name") {
				if (findNumber(program[pos]).ValueNumber < helperLex.size) {
					int positionX = findNumber(program[pos]).ValueNumber;
					stack.push(helperLex.vectorLex[positionX]);
				}
				else {
					cout << program[pos].ValueNumber << " " << stack.top().size;
					cout << "Out of index of array: " << stack.top().Name;
					exit(1);
				}
			}
			else {
				if (program[pos].ValueNumber < helperLex.size) {
					stack.push(helperLex.vectorLex[program[pos].ValueNumber]);
				}
				else {
					cout << program[pos].ValueNumber << " " << stack.top().size;
					cout << "Out of index of array: " << stack.top().Name;
					exit(1);
				}
			}
			
		}
		if (program[pos].Name == "/") {
			second = stack.top();
			stack.pop();
			stack.top().ValueNumber /= second.ValueNumber;
		}
		if (program[pos].Name == "*") {
			second = stack.top();
			stack.pop();
			stack.top().ValueNumber *= second.ValueNumber;
		}
		if (program[pos].Name == "+") {
			second = stack.top();
			stack.pop();
			stack.top().ValueNumber += second.ValueNumber;
		}
		if (program[pos].Name == "-") {
			second = stack.top();
			stack.pop();
			stack.top().ValueNumber -= second.ValueNumber;
		}
		if (program[pos].Name == "=") {				
				second = stack.top();
				stack.pop();
				if (stack.top().index == -1) {
					stack.top().ValueNumber = second.ValueNumber;
					setNumber(stack.top());
					stack.pop();
				}
				else {

					stack.top().vectorLex[stack.top().index].ValueNumber = second.ValueNumber;
					setArray(stack.top());
					stack.pop();
				}
		}
		if (program[pos].Name == "w") {
			cout << findNumber(stack.top()).ValueNumber << endl;
			stack.pop();
		}
		if (program[pos].Name == "r") {
			double value;
			cout << "Enter the value ";
			cin >> value;
			stack.top().ValueNumber = value;
			setNumber(stack.top());
			stack.pop();
		}
	}
	void whileExecution() {
		int markZero = pos - 1;
		bool result = false;
		while (program[pos].ValueSymbol != "m1")
		{
			if (program[pos].Name == "name") {
				stack.push(findNumber(program[pos]));
			}
			if (program[pos].Name == "number" ||
				program[pos].Name == "floatNumber") {
				stack.push(program[pos]);
			}
			if (program[pos].Name == "mass") {

				SingleLexem massHelper;
				massHelper = findNumber(program[pos]);
				pos++;
				if (program[pos].Name == "name") {
					stack.push(massHelper.vectorLex[findNumber(program[pos]).ValueNumber]);
				}
				else {
					stack.push(massHelper.vectorLex[program[pos].ValueNumber]);
				}
			}
			if (program[pos].ValueSymbol == ">") {
				second = stack.top();
				stack.pop();
				result = (stack.top().ValueNumber > second.ValueNumber);
				stack.pop();
			}
			if (program[pos].ValueSymbol == "<") {
				second = stack.top();
				stack.pop();
				result = (stack.top().ValueNumber < second.ValueNumber);
				stack.pop();
			}
			if (program[pos].ValueSymbol == ">=") {
				second = stack.top();
				stack.pop();
				result = (stack.top().ValueNumber >= second.ValueNumber);
				stack.pop();
			}
			if (program[pos].ValueSymbol == "<=") {
				second = stack.top();
				stack.pop();
				result = (stack.top().ValueNumber <= second.ValueNumber);
				stack.pop();
			}
			pos++;
		}
		if (!stack.empty()) {
			result = 1 && stack.top().ValueNumber;
			stack.pop();
		}
		if (!result) {
			while (program[pos].Name != "j") {
				pos++;
			}
		}
		else {
			while (program[pos].ValueSymbol != "m2") {
				baseOperations();
				pos++;
			}
			pos = markZero;
		}
	}
	void ifExecution() {
		bool result = false;
		while (program[pos].ValueSymbol != "m1")
		{
			if (program[pos].Name == "name") {
				stack.push(findNumber(program[pos]));
			}
			if (program[pos].Name == "number" ||
				program[pos].Name == "floatNumber") {
				stack.push(program[pos]);
			}
			if (program[pos].Name == "mass") {
				SingleLexem massHelper;
				massHelper = findNumber(program[pos]);
				pos++;
				if (program[pos].Name == "name") {
					stack.push(massHelper.vectorLex[findNumber(program[pos]).ValueNumber]);
				}
				else {
					stack.push(massHelper.vectorLex[program[pos].ValueNumber]);
				}
			}
			if (program[pos].ValueSymbol == ">") {
				second = stack.top();
				stack.pop();
				result = (stack.top().ValueNumber > second.ValueNumber);
				stack.pop();
			}
			if (program[pos].ValueSymbol == "<") {
				second = stack.top();
				stack.pop();
				result = (stack.top().ValueNumber < second.ValueNumber);
				stack.pop();
			}
			if (program[pos].ValueSymbol == ">=") {
				second = stack.top();
				stack.pop();
				result = (stack.top().ValueNumber >= second.ValueNumber);
				stack.pop();
			}
			if (program[pos].ValueSymbol == "<=") {
				second = stack.top();
				stack.pop();
				result = (stack.top().ValueNumber <= second.ValueNumber);
				stack.pop();
			}
			pos++;
		}
		if (!stack.empty()) {
			result = 1 && stack.top().ValueNumber;
			stack.pop();
		}
		if (result) {
			while (program[pos].ValueSymbol != "j") {
				baseOperations();
				pos++;
			}
			if (program[pos + 1].ValueSymbol == "m3") {
				pos++;
				while (program[pos].ValueSymbol != "j") {
					pos++;
				}
			}
		}
		else {
			while (program[pos].ValueSymbol != "j") {
				pos++;
			}
			if (program[pos + 1].ValueSymbol == "m3") {
				pos++;
				while (program[pos].ValueSymbol != "j") {
					baseOperations();
					pos++;
				}
			}
		}
	}

	void execution() {
		while (pos < program.size()) {
			baseOperations();
			if (program[pos].Name == "m0") {
				whileExecution();
			}
			if (program[pos].Name == "m4") {
				ifExecution();
			}
			pos++;
		}
	}

};

int main() {
	LexemsAlphabet mainLexems;
	Lexer lexer;

	vector<SingleLexem> Token = lexer.Analyzer(mainLexems);
	cout << "\n\n---------DEBUG HAS STARTED------------\n\n";
	for (int i = 0; i < Token.size(); i++) {
		if (Token[i].ValueNumber == INFINITY) cout << Token[i].Name << " " << Token[i].ValueSymbol << ": " << Token[i].Y << " " << Token[i].X << endl;
		else cout << Token[i].Name << " " << Token[i].ValueNumber << ": " << Token[i].Y << " " << Token[i].X << endl;
	}

	cout << endl << endl;
	Parser parser(Token);
	parser.analyze();

	cout << endl << endl;

	Token = parser.prog.Stack;
	for (int i = 0; i < Token.size(); i++) {
		if (Token[i].ValueNumber == INFINITY) cout << Token[i].Name << " " << Token[i].ValueSymbol << ": " << Token[i].Y << " " << Token[i].X << endl;
		else cout << Token[i].Name << " " << Token[i].ValueNumber << ": " << Token[i].Y << " " << Token[i].X << endl;
	}

	cout << "GENERATION OPS \n\n";

	parser.prog.generationOPS();
	cout << endl << endl;

	Token = parser.prog.programOPS;
	for (int i = 0; i < Token.size(); i++) {
		if (Token[i].ValueNumber == INFINITY) cout << Token[i].Name << " " << Token[i].ValueSymbol << ": " << Token[i].Y << " " << Token[i].X << endl;
		else cout << Token[i].Name << " " << Token[i].ValueNumber << ": " << Token[i].Y << " " << Token[i].X << endl;
	}

	cout << "\n\n---------DEBUG HAS ENDED------------\n\n-------PROGRAM HAS STARTED----------\n\n";

	Interpretator intepretator(Token, parser.prog);
	intepretator.execution();
}

//if (!result) {
//	while (program[pos].Name != "j") {
//		pos++;
//	}
//	if (program[pos + 1].ValueSymbol == "m3") {
//		pos++;
//		while (program[pos].ValueSymbol != "j") {
//			baseOperations();
//			pos++;
//		}
//	}
//}
//else {
//	while (program[pos].ValueSymbol != "j") {
//		baseOperations();
//		pos++;
//	}
//	if (program[pos + 1].ValueSymbol == "m3") {
//		pos++;
//		while (program[pos].ValueSymbol != "j") {
//			baseOperations();
//			pos++;
//		}
//	}
//}

//while (program[pos].Name != "=") {
//	if (program[pos].Name == "name")
//	{
//		stack.push(findNumber(program[pos]));
//	}
//	if (program[pos].Name == "number" ||
//		program[pos].Name == "floatNumber")
//	{
//		stack.push(program[pos]);
//	}
//	if (program[pos].Name == "+") {
//		second = stack.top();
//		stack.pop();
//		stack.top().ValueNumber += second.ValueNumber;
//	}
//	if (program[pos].Name == "-") {
//		second = stack.top();
//		stack.pop();
//		stack.top().ValueNumber -= second.ValueNumber;
//	}
//	if (program[pos].Name == "*") {
//		second = stack.top();
//		stack.pop();
//		stack.top().ValueNumber *= second.ValueNumber;
//	}
//	if (program[pos].Name == "/") {
//		second = stack.top();
//		stack.pop();
//		stack.top().ValueNumber /= second.ValueNumber;
//	}
//	pos++;
//}
//
//if (stack.top().Name == "name") {
//	setNumber(stack.top());
//	cout << stack.top().ValueNumber << endl;
//	pos++;
//}
//else {
//	second = stack.top();
//	stack.pop();
//	stack.top().ValueNumber = second.ValueNumber;
//	setNumber(stack.top());
//	cout << stack.top().ValueNumber << endl;
//	pos++;
//}