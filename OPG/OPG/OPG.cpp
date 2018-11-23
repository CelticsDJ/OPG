#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <iomanip>
using namespace std;

string blank = ""; //用于通过字符凑字符串

vector<string> grammarList; //文法集合
vector<char> Vn; //非终结符集合Vn
vector<char> Vt; //终结符集合Vt
vector<vector<char>> firstVT; //firstVT集
vector<vector<char>> lastVT; //LastVT集
char table[255][255]; //算符优先顺序表

bool F[30][255]; //firstVT集的布尔数组
stack<pair<char, char>> STACK; //firstVT集计算用栈

void printLine (unsigned int n) {
	for (unsigned int i = 0; i < n; ++i) {
		cout << '-';
	}
	cout << endl;
}

void printTable(vector<char> symbolStack, vector<char> inputString, int step, char left, char priority, char right, string handle) {
	string symbolStr = blank+" ", inputStr = blank+" ";
	for (unsigned int i = 0; i < symbolStack.size(); ++i) symbolStr += symbolStack[i];
	for (unsigned int i = 0; i < inputString.size(); ++i) inputStr += inputString[i];
	cout << "| " << setw(4) << step << '|' << setw(10) << symbolStr << '|' << setw(8)
		<< (" " + blank + left + " " + priority + " " + right) << "|" << setw(12) << " "+handle << '|' << setw(10) << inputStr << "|\n";
}

void printV() { //打印集合和算符优先表
	cout << endl;
	/*cout << "文法集合：\n";
	for (unsigned int i = 0; i < grammarList.size(); ++i) {
		cout << grammarList[i] << endl;
	}

	cout << endl << "Vn集: ";
	for (unsigned int i = 0; i < Vn.size(); ++i) {
		cout << Vn[i] << " ";
	}
	cout << endl;

	cout << endl << "Vt集: ";
	for (unsigned int i = 0; i < Vt.size(); ++i) {
		cout << Vt[i] << " ";
	}
	cout << endl<< endl;

	cout << "FIRSTVT集:\n";
	for (unsigned int i = 0; i < firstVT.size(); ++i) { //打印firstVT集
		cout << Vn[i] << ": ";
		for (unsigned int j = 0; j < firstVT[i].size(); ++j) {
			cout << firstVT[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	cout << "LASTVT集:\n";
	for (unsigned int i = 0; i < lastVT.size(); ++i) { //打印lastVT集
		cout << Vn[i] << ": ";
		for (unsigned int j = 0; j < lastVT[i].size(); ++j) {
			cout << lastVT[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	*/
	cout << "算符优先顺序表:\n";
	cout << endl;
	cout << "     ";
	for (unsigned int i = 0; i < Vt.size(); ++i) {
		cout << " " << Vt[i] << "  ";
	}
	cout << " " << "#" << "  \n";
	for (unsigned int i = 0; i < Vt.size() + 1; ++i) { //输出table
		cout << endl;
		cout << "  ";
		if (i == Vt.size()) cout << '#';
		else cout << Vt[i];
		cout << "  ";
		for (unsigned int j = 0; j < Vt.size() + 1; ++j) {
			cout << " " <<table[i][j] << "  ";
		}
		cout << endl;
	}
	cout << endl;
}

int get_VnNum(char c) {
	for (unsigned int i = 0; i < Vn.size(); ++i) {
		if (c == Vn[i])
			return i;
	}
	return -1;
}

int get_VtNum(char c) {
	if (c == '#') //规约句柄时用于查找算符优先顺序表
		return Vt.size();

	for (unsigned int i = 0; i < Vt.size(); ++i) {
		if (c == Vt[i])
			return i;
	}
	return -1;
}

bool search_List(char c, vector<char> List) {
	for (unsigned int i = 0; i < List.size(); ++i) {
		if (c == List[i])
			return true;
	}
	return false;
}

char search_Grammar(string handle) { //归约句柄
	for (unsigned int i = 0; i < grammarList.size(); ++i) {
		if (grammarList[i].size() == 4 && grammarList[i][3] >= 'A' && grammarList[i][3] <= 'Z') continue; //忽略非终结符相互归约
		bool flag = false;
		if (handle.size() == grammarList[i].size() - 3) { //句柄长度和文法右端长度比较
			for (unsigned int j = 3; j < grammarList[i].size(); ++j) { //比较每一个字符
				//cout << grammarList[i] << endl;
				int k = j - 3;
				if (handle[k] >= 'A' && handle[k] <= 'Z' && grammarList[i][j] >= 'A'&& grammarList[i][j] <= 'Z') { //非终结符不重要
					if (j == grammarList[i].size() - 1) {
						//cout << grammarList[i] << endl;
						flag = true;
					}
					continue;
				}
				if (handle[k] != grammarList[i][j]) { //有字符不匹配则跳出
					flag = false;
					break;
				}
				//cout << j << endl;
				if (j == grammarList[i].size() - 1) {
					//cout << grammarList[i] << endl;
					flag = true;
				}
			}
			//if (flag) break;
		}
		if (flag) { //如果匹配成功，返回该文法任意非终结符
			//cout << grammarList[i] << endl;
			return grammarList[0][0];
		}
	}
	//没有文法匹配，返回-1，说明输入串不属于该文法
	return -1;
}

bool check_grammar(string grammar) { //检查输入的每条文法是不是算符优先文法
	if (grammar.size() < 4) { //每条文法长度至少为4
		cout << "每条文法长度至少为4\n";
		return false;
	}

	if (grammar[0] < 'A' || grammar[0] > 'Z') { //检查最左非终结符是不是大写字母
		cout << "该条文法最左非终结符不是大写字母\n";
		return false;
	}

	if (grammar[1] != '-' || grammar[2] != '>') { //检查有没有推导符号->
		cout << "文法没有推导符号->\n";
		return false;
	}
	
	for (unsigned int i = 3; i < grammar.size() - 1; ++i) { //检查右侧是否有连续的非终结符
		if (grammar[i] >= 'A' && grammar[i] <= 'Z'&&grammar[i + 1] >= 'A' && grammar[i + 1] <= 'Z') {
			cout << "算符优先文法右侧不能连续出现两个非终结符\n";
			return false;
		}
	}

	return true;
}

bool chack_grammarList() { //检查文法集合是否合法,同时生成Vt集
	for (unsigned int i = 0; i < grammarList.size(); ++i) {
		for (unsigned int j = 3; j < grammarList[i].size(); ++j) {
			if (grammarList[i][j] >= 'A' && grammarList[i][j] <= 'Z') {
				if (search_List(grammarList[i][j], Vn)) //检查右端非终结符是否在左端都有出现
					continue;
				else {
					cout << "文法左端终结符" << grammarList[i][j] << "未在右端出现过\n";
					return false;
				}
			}
			else {
				if (!search_List(grammarList[i][j],Vt)) {
					Vt.push_back(grammarList[i][j]); //记录非终结符
				}
			}
		}
	}
	return true;
}

void cut_grammar() { //将带有 '|' 的文法拆分成多个文法
	for (unsigned int i = 0; i < grammarList.size(); ++i) { //检查每一条文法
		string tmp = blank + grammarList[i][0] + "->";
		string tmp_left = blank + grammarList[i][0] + "->"; //文法左端
		for (unsigned int j = 3; j < grammarList[i].size(); ++j) {
			if (grammarList[i][j] == '|') { //遇到'|'
				string tmp_right = blank;
				for (unsigned int k = j + 1; k < grammarList[i].size(); ++k) //截取'|'后全部字符
					tmp_right += grammarList[i][k];
				grammarList.push_back(tmp_left+tmp_right); //记录为新的文法
				grammarList[i] = tmp; //更新原先文法
				break;
			}
			else {
				tmp += grammarList[i][j];
			}
		}
	}
}

void INSERT(char U, char b) {
	int x = get_VnNum(U), y = get_VtNum(b);
	if (F[x][y] == 0) {
		F[x][y] = 1;
		STACK.push(pair<char, char>(U, b));
	}
}

void get_firstVt() { //计算firstVT集
	for (unsigned int i = 0; i < Vn.size(); ++i) { //初始化
		for (unsigned int j = 0; j < Vt.size(); ++j) {
			F[i][j] = 0;
		}
	}
	
	for (unsigned int i = 0; i < grammarList.size(); ++i) { //赋初值
		char U = grammarList[i][0];
		for (unsigned int j = 3; j < grammarList[i].size(); ++j) {
			char b = grammarList[i][j];
			if (search_List(b, Vt)) { //是非终结符
				INSERT(U, b); //形如U->b...或U->Vb...
				break;
			}
		}
	}

	while (!STACK.empty()) {
		char V = STACK.top().first, b = STACK.top().second;
		STACK.pop();
		pair<char, char> tmp = pair<char, char>(V, b);
		for (unsigned int i = 0; i < grammarList.size(); ++i) {
			char U = grammarList[i][0];
			if (U != V && grammarList[i][3] == V) { //形如U->Vb...
				INSERT(U, b);
			}
		}
	}

	for (unsigned int i = 0; i < Vn.size(); ++i) {
		vector<char> tmp;
		firstVT.push_back(tmp);
	}

	for (unsigned int i = 0; i < Vn.size(); ++i) {
		for (unsigned int j = 0; j < Vt.size(); ++j) {
			if (F[i][j] == 1)
				firstVT[i].push_back(Vt[j]);
		}
	}
	return;
}

void get_lastVt() { //计算lastVT集
	for (unsigned int i = 0; i < Vn.size(); ++i) { //初始化
		for (unsigned int j = 0; j < Vt.size(); ++j) {
			F[i][j] = 0;
		}
	}

	for (unsigned int i = 0; i < grammarList.size(); ++i) { //赋初值
		char U = grammarList[i][0];
		for (unsigned int j = grammarList[i].size() - 1; j > 2; --j) {
			char b = grammarList[i][j];
			if (search_List(b, Vt)) { //是非终结符
				INSERT(U, b); //形如U->...b或U->...bV
				break;
			}
		}
	}

	while (!STACK.empty()) {
		char V = STACK.top().first, b = STACK.top().second;
		STACK.pop();
		pair<char, char> tmp = pair<char, char>(V, b);
		for (unsigned int i = 0; i < grammarList.size(); ++i) {
			char U = grammarList[i][0];
			int tmp = grammarList[i].size() - 1;
			if (U != V && grammarList[i][tmp] == V) { //形如U->...bV
				INSERT(U, b);
			}
		}
	}

	for (unsigned int i = 0; i < Vn.size(); ++i) {
		vector<char> tmp;
		lastVT.push_back(tmp);
	}

	for (unsigned int i = 0; i < Vn.size(); ++i) {
		for (unsigned int j = 0; j < Vt.size(); ++j) {
			if (F[i][j] == 1)
				lastVT[i].push_back(Vt[j]);
		}
	}
	return;
}

void get_table() { //计算算符优先顺序表
	for (unsigned int i = 0; i < Vt.size() + 1; ++i) { //table初始化
		for (unsigned int j = 0; j < Vt.size() + 1; ++j) {
			table[i][j] = '/';
		}
	}

	for (unsigned int i = 0; i < grammarList.size(); ++i) {

		for (unsigned int j = 3; j < grammarList[i].size() - 1; ++j) {

			char x0 = grammarList[i][j], x1 = grammarList[i][j + 1];

			if (search_List(x0, Vt) && search_List(x1, Vt)) {
				int x = get_VtNum(x0), y = get_VtNum(x1);
				table[x][y] = '=';
			}

			if (j < grammarList.size() - 2) {
				char x2 = grammarList[i][j + 2];
				if (search_List(x0, Vt) && search_List(x2, Vt) && search_List(x1,Vn)) {
					int x = get_VtNum(x0), y = get_VtNum(x2);
					table[x][y] = '=';
				}
			}

			if (search_List(x0, Vt) && search_List(x1, Vn)) {
				int tmp = get_VnNum(x1);
				for (unsigned int k = 0; k < firstVT[tmp].size(); ++k) {
					char b = firstVT[tmp][k];
					int x = get_VtNum(x0), y = get_VtNum(b);
					table[x][y] = '<';
				}
			}

			if (search_List(x0, Vn) && search_List(x1, Vt)) {
				int tmp = get_VnNum(x0);
				for (unsigned int k = 0; k < lastVT[tmp].size(); ++k) {
					char a = lastVT[tmp][k];
					int x = get_VtNum(x1), y = get_VtNum(a);
					table[y][x] = '>';
				}
			}
		}
	}

	for (unsigned int i = 0; i < firstVT.size(); ++i) { //处理'#'优先级
		for (unsigned j = 0; j < firstVT[i].size(); ++j) {
			int y = get_VtNum(firstVT[i][j]);
			table[Vt.size()][y] = '<';
		}
	}
	
	for (unsigned int i = 0; i < lastVT.size(); ++i) {
		for (unsigned j = 0; j < lastVT[i].size(); ++j) {
			int x = get_VtNum(lastVT[i][j]);
			table[x][Vt.size()] = '>';
		}
	}


	return;
}

bool check_sentence(string sentence) {
	bool flag = true;
	if (sentence[sentence.size() - 1] != '#') {
		cout << "输入串应以#结尾\n";
		flag = false;
	}

	if (sentence.size() == 1) {
		cout << "输入串长度至少为2\n";
		flag = false;
	}

	for (unsigned int i = 0; i < sentence.size() - 1; ++i) {
		if (sentence[i] == '#') {
			cout << "输入串只有结尾符号可以为#\n";
			flag = false;
		}

		if (!search_List(sentence[i], Vn) && !search_List(sentence[i], Vt)) {
			cout << sentence[i] << "在文法中不存在\n";
			flag = false;
		}
	}

	return flag;
}

void analysis(string sentence) {
	vector<char> symbolStack; //分析栈
	vector<char> inputString; //剩余输入串
	symbolStack.push_back('#');
	for (unsigned int i = 0; i < sentence.size(); ++i) {
		inputString.push_back(sentence[i]);
	}

	cout << setw(30) << "分析过程\n";
	printLine(51);
	cout << '|' << setw(5) << left << "步骤" << '|' << setw(10)<< left <<"分析栈" << '|' << setw(8) << left 
		<< "关系标志" << "|" << setw(12) << "当前输入符" << '|' << setw(10) << "剩余输入串" << "|\n";

	char left = '#', right = '\0', priority = '/'; //符号栈栈顶符号,读入符号,顺序
	int x, y,step = 0; //算符优先顺序表横纵坐标
	bool flag; //规约是否成功
	
	while (!inputString.empty() || symbolStack.size() > 3) {
		for (int i = symbolStack.size() - 1; i >= 0; --i) {
			if (!search_List(symbolStack[i], Vn)) {
				left = symbolStack[i];
				break;
			}
		}
		if (right == '\0') {
			right = inputString.front();
			inputString.erase(inputString.begin());
		}
		x = get_VtNum(left);
		y = get_VtNum(right);
		priority = table[x][y];

		printLine(51);

		if (priority == '/') { //归约失败
			printTable(symbolStack, inputString, step, left, priority, right, "归约失败");
			printLine(51);
			return;
		}

		else if (priority == '>') {
			flag = false;
			string handle;
			for (int i = symbolStack.size() - 1; i > 0; --i){ //寻找最左素短语
				handle = blank;
				for (int j = i; j < symbolStack.size(); ++j) { //句柄
					handle += symbolStack[j];
				}
				int res = search_Grammar(handle);
				if (res != -1) {
					flag = true;
					printTable(symbolStack, inputString, step, left, priority, right, ("归约" + handle));
					for (int j = 0; j < handle.size(); ++j) { //句柄
						symbolStack.erase(symbolStack.begin() + i);
					}
					
					symbolStack.push_back((char)res); //归约成功
					break;
				}
			}

			if (flag) { //归约成功
				//printTable(symbolStack, inputString, step, left, priority, right, ("归约"+handle));
			}

			else { //归约失败
				printTable(symbolStack, inputString, step, left, priority, right, "归约失败");
				printLine(51);
				return;
			}
		}

		else { //移进
			printTable(symbolStack, inputString, step, left, priority, right, "移进"+(blank+right));
			symbolStack.push_back(right);
			right = '\0';
		}
		step++;
	}

	/*分析成功*/
	left = priority = right = ' ';
	printLine(51);
	printTable(symbolStack, inputString, step, left, priority, right, "分析成功");
	printLine(51);

	return;
}

int main()
{
	cout << setw(20) << " " << "算 符 优 先 分 析 器" << endl;
	while (1) { //对输入的文法进行处理和检验，得到文法集合，Vn集，Vt集
		cout << "请输入文法，形如E->E+T（不含空格），输入end表示输入结束\n";
		string tmp;
		while (cin >> tmp) {
			if (tmp.compare("end") != 0) {
				if (check_grammar(tmp)) {//检查输入的是不是算符优先文法
					grammarList.push_back(tmp); //记录该文法
					if (!search_List(tmp[0], Vn)) //如果没出现过，记录该终结符
						Vn.push_back(tmp[0]);
				}
			}
			else break;
		}

		cut_grammar();

		if (chack_grammarList()) break; //合法，继续

		else { //不合法，清空重来
			grammarList.clear();
			Vn.clear();
			Vt.clear();
			continue;
		}
	}
	

	get_firstVt(); //求firstVT集
	get_lastVt(); //求lastVT集
	get_table(); //求算符优先顺序表

	printV();

	cout << "请输入要识别的语句(以#结尾)：\n";
	string sentence;
	while (1) {
		cin >> sentence;
		if (check_sentence(sentence)) break;
	}

	analysis(sentence);

	return 0;
}
