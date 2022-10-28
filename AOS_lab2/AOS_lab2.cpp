

#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

#define max_bits 14
#define max_regs 6
#define max_com 2
#define word bitset<max_bits>
#define FILE "input.txt"
#define EX return cout << "Invalid file\n", 1

const string commands[] = { "mov", "sub" };

word toWord(int x) {
	word res;
	if (x < 0) {
		x = -x;
		res[max_bits - 1] = 1;
	}
	for (int i = 0; i < max_bits - 1; i++)
		res[i] = (x >> i) & 1;
	return res;
}
int toInt(word x) {
	int ans = 0;
	for (int i = 0; i < max_bits - 1; i++)
		if (x[i])
			ans |= (1 << i);
	if (x[max_bits - 1]) ans = -ans;
	return ans;
}

word& operator++(word& a, int) {
	int carry = 1;
	for (int i = 0; i < max_bits; i++) {
		int z = (int)a[i] + carry;
		carry = 0;
		if (z > 1) {
			carry = 1; z -= 2;
		}
		a[i] = z;
	}
	return a;
}
word toAdd(word x) {
	if (x[max_bits - 1] == 0)return x;
	x[max_bits - 1] = 0;
	x = ~x;
	x++;
	return x;
}
word toNor(word x) {
	if (x[max_bits - 1] == 0)return x;
	x = ~x;
	x[max_bits - 1].flip();
	x++;
	return x;
}
word operator+(word a, word b) {
	word res;
	int sgn = a[max_bits - 1] != b[max_bits - 1];
	int carry = 0;
	for (int i = 0; i < max_bits; i++) {
		int z = (int)a[i] + (int)b[i] + carry;
		carry = 0;
		if (z > 1) {
			carry = 1; z -= 2;
		}
		res[i] = z;
	}
	return (res);

}
word operator-(word a, word b) {
	b = toNor(b);
	b[max_bits - 1].flip();
	b = toAdd(b);
	return a + b;
}

vector<string> splitter(string s) {
	vector<string> ans;
	string x = "";
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == ' ' || s[i] == ',') {
			if (x != "")ans.push_back(x);
			x = "";
		}
		else
			x += s[i];
	}
	if (x != "")ans.push_back(x);
	return ans;
}
bool isDigit(string line){
	if (line.size() > 9) return false;
	char* p;
	strtol(line.c_str(), &p, 10);
	return *p == 0;
}
bool isRegister(string line) {
	if (line.size() != 2)return false;
	return line[0] == 'R' && '1' <= line[1] && line[1] < '1' + max_regs;
}
bool isCommand(string line) {
	for (int i = 0; i < max_com; i++)
		if (line == commands[i])return true;
	return false;
}

struct processor {
	word regs[max_regs];
	string last_command;
	int takt = 0;
	int commands = 0;
	int status = 0;
	void mov_val(int op1, int value) {
		regs[op1] = toAdd(toWord(value));
	}
	void mov_reg(int op1, int op2) {
		regs[op1] = regs[op2];
	}
	void sub_val(int op1, int value) {
		auto w = toWord(value);
		regs[op1] = regs[op1] - w;
	}
	void sub_reg(int op1, int op2) {
		auto w = regs[op2];
		regs[op1] = regs[op1] - w;
	}
	void proccess_command(vector<string> operands) {
		last_command = operands[0] + " " + operands[1] + "," + operands[2];
		commands++;
		takt += 1;
		takt &= 1;		
		print_info();

		if (operands[0] == "mov") mov(operands[1], operands[2]);
		else sub(operands[1], operands[2]);
		status = regs[operands[1][1] - '1'][max_bits - 1];

		takt += 1;
		takt &= 1;
		print_info();
	}
	void mov(string op1, string op2) {
		if (isDigit(op2)) mov_val(op1[1] - '1', stoi(op2));
		else mov_reg(op1[1] - '1', op2[1] - '1');
	}
	void sub(string op1, string op2) {
		if (isDigit(op2)) sub_val(op1[1] - '1', stoi(op2));
		else sub_reg(op1[1] - '1', op2[1] - '1');
	}
	void print_info() {
		cout << "IR: " << last_command << '\n';
		for (int i = 0; i < max_regs; i++) 
			cout <<"R" << char('1' + i) << " " << regs[i].to_string() << ' ' << toInt(toNor(regs[i])) << '\n';

		cout << "PS: " << status << '\n';
		cout << "PC: " << commands << '\n';
		cout << "TC: " << takt << '\n';

		cout << '\n';
	}
};
int main()
{
	freopen(FILE, "r", stdin);
	string s;
	auto proc = processor();
	while (getline(cin, s)) {
		auto parts = splitter(s);
		if (parts.size() != 3) EX;
		if (!isCommand(parts[0])) EX;
		if (!isRegister(parts[1])) EX;
		if (!isRegister(parts[2]) && !isDigit(parts[2])) EX;
		
		proc.proccess_command(parts);
	}
}
