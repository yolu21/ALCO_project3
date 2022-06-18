#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include<vector>
using namespace std;


bool done = true;							//done = true�N����
int writeback = 0;							//��write�X���A�p�G��inst�Ƥ@�˴N����
int cur_issue = 0;							//�ݲ{�bissue��ĴX��inst
const int iss_Lat = 1;						//�n�j�󵥩�iss_Lat�~��iexe
const int wr_Lat = 1;						//�n����wr_Lat�~��iwrite
const int ADD = 2;							//exe�n�h��cycle�~��write
const int MUL = 4;							//
const int DIV = 10;							//

int cycle = 0;								//�]�X��cycle

class RS {									//RS
public:										//
	bool busy = false, disp = false;		//busy�N��rs�̬O�_���F��Adisp�P�_�ण��write
	string rs1, rs2, OP, RD;				//RD = rs1 OP rs2
	int result;								//result = ��X�Ӫ���
	int iss_lat = 0, wr_lat = 0,			//��iss_Lat�Bwr_Lat�����
		inst = 1000, op, lat;				//inst�O�ĴX��instruction�Alat�]�F�X��exe �n����ADD MUL DIV
};

class RAT {									//RAT
public:										//
	bool busy = false;						//busy�N��rs�̬O�_���F��
	string rat;								//rat ��Ȧs
};

class INST {								//instruction
public:										//
	string rd, rs, rt, op;					//��J���� -> op rd rs rt �Ard = rs op rt
	int issClock, exeBegin,					//�����ĴX��cycle����issue�B�i�Jexe�B
		exeEnd, wrBack;						//����exe�B����write
};


string rf[6] = { "0","0","2","4","6","8" }; //reg����
int issue(vector<INST> &inst, vector<RS> &rs, vector<RAT>& rat, int k);
void execute(vector<INST>&inst, vector<RS>& rs, vector<RAT>& rat, int k);
void write(vector<INST>& inst, vector<RS>& rs, vector<RAT>& rat, int k);
void print(vector<RS>& rs, int cycle, string buffer[2], vector<RAT>& rat);

bool is = false, wr = false;
int c = 0;
int pos = 0;
int main()
{
	vector<INST> INST(50);
	int k = 0;														//�����X��instrution
	while (true)
	{
		cin >> INST[k].op;											//��Joperator
		if (INST[k].op == "0")										//�p�G��J0�N������J�A���X�j��
		{
			break;
		}
		cin>> INST[k].rd >> INST[k].rs >> INST[k].rt;				
		k++;
	}
	
	
	string buffer[2] = { "0","0" };
	vector<RS> RS(5);
	vector<RAT> RAT(6);
	
	int n = 0;
	do {
		
		cycle++;
		is = false;
		wr = false;
		issue(INST, RS, RAT, k);
		execute(INST, RS, RAT, k);
		write(INST, RS, RAT, k);
		if (is || wr)												//�Ψ��bool�������ǭȦ����ܡA�����ܤ~��X
			print(RS, cycle, buffer, RAT);

		done = false;
		if (writeback == k)											// #writeback == k(#inst) �����Ҧ�inst
			done = true;
	} while (!done);
}
int issue(vector<INST>& inst, vector<RS>& rs, vector<RAT>& rat, int k)
{
	bool rsFree = false;
	if (cur_issue >= k)
		return 0;

	switch (inst[cur_issue].op[0])									//��op���Ĥ@�Ӧr���P�_�O����operator
	{
	case 'A':														//'A'=ADD or ADDI
		for (int i = 0; i < 3; i++)
		{
			if (!rs[i].busy) {
				pos = i;
				if (inst[cur_issue].op[3] == 'I')
					rs[i].op = 1;									//ADDI �� ADD�� op ���}�A��K�᭱��Jrs
				else
					rs[i].op = 2;
				rs[i].OP = '+';
				cur_issue++;										//�U�@��inst�i�Hissue
				rsFree = true;
				is = true;											//������issue
				break;
			}
		}
		if (!rsFree)
			return 1;
		break;
	case 'S':
		for (int i = 0; i < 3; i++)
		{
			if (!rs[i].busy) {
				pos = i;
				rs[i].op = 3;
				rs[i].OP = '-';
				cur_issue++;
				rsFree = true;
				is = true;
				break;
			}
		}
		if (!rsFree)
			return 1;
		break;
	case 'M':
		for (int i = 3; i < 5; i++)
		{
			if (!rs[i].busy) {
				pos = i;
				rs[i].op = 4;
				rs[i].OP = '*';
				cur_issue++;
				rsFree = true;
				is = true;
				break;
			}
		}
		if (!rsFree)
			return 1;
		break;
	case 'D':
		for (int i = 3; i < 5; i++)
		{
			if (!rs[i].busy) {
				pos = i;
				rs[i].op = 5;
				rs[i].OP = '/';
				cur_issue++;
				rsFree = true;
				is = true;
				break;
			}
		}
		if (!rsFree)
			return 1;
		break;
	default:
		break;

	}
	int ch = 0;
	ch = inst[cur_issue - 1].rs[1] - '0';				//inst��rs�OF�h��
	for (int i = 1; i < 6; i++)
	{
		if (ch == i && rat[i].busy)						//���ˬdrat�̬O�_���F��
		{
			rs[pos].rs1 = rat[i].rat;
			break;
		}
		else if (ch == i && !rat[i].busy)
		{
			rs[pos].rs1 = rf[i];						//�qreg����
			break;
		}
	}
	ch = inst[cur_issue - 1].rt[1] - '0';
	for (int i = 1; i < 6; i++)
	{
		if (ch == i && rat[i].busy)
		{
			rs[pos].rs2 = rat[i].rat;
			break;
		}
		else
		{
			if (rs[pos].op == 1)
			{
				rs[pos].rs2 = inst[cur_issue - 1].rt;
				break;
			}
			if (ch == i && !rat[i].busy)
			{
				rs[pos].rs2 = rf[i];
				break;
			}
		}
	}
	string buffer[2];
	rs[pos].busy = true;						//rs�̦��F��
	rs[pos].iss_lat = 0;
	rs[pos].inst = cur_issue - 1;				//�O���o��rs�O�s�ĴX��instruction
	inst[cur_issue - 1].issClock = cycle;		//�ĴX��cycle issue


	ch = inst[cur_issue - 1].rd[1] - '0';
	if (rat[ch].busy)							//�p�G������rat�w�g���F�A�����л\�W�h
	{
		rat[ch].rat.pop_back();
	}
	else
	{											
		char s = 'R';
		rat[ch].rat.push_back(s);
		s = 'S';
		rat[ch].rat.push_back(s);
	}
	char str = pos + '1';
	rat[ch].rat.push_back(str);



	rs[pos].RD = rat[ch].rat;					//�������G
	rat[ch].busy = true;						//rat�������F��F
	return 2;
}
bool add = false, mul = false;					//�O��alu���S���b��
void execute(vector<INST>& inst, vector<RS>& rs, vector<RAT>& rat, int k)
{
	for (int i = 0; i < rs.size(); i++)
	{
		if (rs[i].busy)
		{
			if (rs[i].iss_lat >= iss_Lat && ((i < 3 && add == false) || (i >= 3 && mul == false)))
			{									//ALU�n�Sinst�b���~��i�AADD MUL ALU�O���}��
				if (rs[i].rs1[0] != 'R' && rs[i].rs2[0] != 'R')			//�ݦ��S��ready
				{
					if (inst[rs[i].inst].exeBegin == 0)
						inst[rs[i].inst].exeBegin = cycle;				//�ĴX��cycle�iexe
					rs[i].lat++;
					stringstream ss;
					stringstream ss1;
					int a1, a2, ans;
					switch (rs[i].op)
					{
					case 1:

						ss << rs[i].rs1;
						ss >> a1;
						ss1 << rs[i].rs2;
						ss1 >> a2;
						ans = a1 + a2;
						if (rs[i].lat == ADD) {
							rs[i].result = ans;
							rs[i].disp = true;
							rs[i].lat = 0;
							inst[rs[i].inst].exeEnd = cycle;
							rs[i].iss_lat = 0;
							add = true;
						}
					case 2:
						ss << rs[i].rs1;
						ss >> a1;

						ss1 << rs[i].rs2;
						ss1 >> a2;
						ans = a1 + a2;
						if (rs[i].lat == ADD) {
							rs[i].result = ans;
							rs[i].disp = true;
							rs[i].lat = 0;
							inst[rs[i].inst].exeEnd = cycle;
							rs[i].iss_lat = 0;
							add = true;
						}
					case 3:
						ss << rs[i].rs1;
						ss >> a1;

						ss1 << rs[i].rs2;
						ss1 >> a2;
						ans = a1 - a2;
						if (rs[i].lat == ADD) {
							rs[i].result = ans;
							rs[i].disp = true;
							rs[i].lat = 0;
							inst[rs[i].inst].exeEnd = cycle;
							rs[i].iss_lat = 0;
							add = true;
						}
					case 4:
						ss << rs[i].rs1;
						ss >> a1;

						ss1 << rs[i].rs2;
						ss1 >> a2;
						ans = a1 * a2;
						if (rs[i].lat == MUL) {
							rs[i].result = ans;
							rs[i].disp = true;
							rs[i].lat = 0;
							inst[rs[i].inst].exeEnd = cycle;
							rs[i].iss_lat = 0;
							mul = true;
						}
					case 5:
						ss << rs[i].rs1;
						ss >> a1;

						ss1 << rs[i].rs2;
						ss1 >> a2;
						ans = a1 / a2;
						if (rs[i].lat == DIV) {
							rs[i].result = ans;
							rs[i].disp = true;
							rs[i].lat = 0;
							inst[rs[i].inst].exeEnd = cycle;
							rs[i].iss_lat = 0;
							mul = true;
						}
					default:
						break;
					}
				}

			}
			else
				rs[i].iss_lat++;
		}
	}
}
void write(vector<INST>& inst, vector<RS>& rs, vector<RAT>& rat, int k)
{
	for (int i = 0; i < rs.size(); i++)
	{
		if (rs[i].disp)
		{
			if (rs[i].wr_lat == wr_Lat)
			{
				if (inst[rs[i].inst].wrBack == 0)
					inst[rs[i].inst].wrBack = cycle;						//�ĴX��cycle�iwrite
				for (int j = 1; j < 6; j++)
				{
					if (rat[j].busy)
					{
						if (rs[i].RD == rat[j].rat)
						{
							stringstream ss;
							string str;
							ss << rs[i].result;
							ss >> str;
							rf[j] = str;									//�N�ȩ�^rf
							for (int h = 0; h <= rat[j].rat.size()+1; h++)
								rat[j].rat.pop_back();						//�R��rat
							rat[j].busy = false;							
						}
						
					}
				}
				if (i < 3)
					add = false;											//write �� ALU�ŤF�N�i�H����U�@��
				else
					mul = false;
				stringstream ss1;
				string str1;
				
				for (int y = 0; y < rs.size(); y++)
				{
					if (rs[y].rs1== rs[i].RD)								//rs�̼Ȧs��write���A����
					{
						ss1 << rs[i].result;
						ss1 >> str1;
						rs[y].rs1 = str1;
					}
					if (rs[y].rs2 == rs[i].RD)
					{
						ss1 << rs[i].result;
						ss1 >> str1;
						rs[y].rs2 = str1;
					}
				}
				rs[i].busy = false;
				rs[i].disp = false;
				for (int h = 0; h < rs[i].rs1.size(); h++)				//�R��rs
					rs[i].rs1.pop_back();								//
				for (int h = 0; h <=rs[i].rs2.size(); h++)				//	
					rs[i].rs2.pop_back();								//
				for (int h = 0; h < rs[i].OP.size(); h++)				//
					rs[i].OP.pop_back();								//�R��rs
				rs[i].wr_lat = 0;
				wr = true;												//������write
				writeback++;
			}
			else rs[i].wr_lat++;
		}
	}
}
void print( vector<RS> &rs, int cycle, string buffer[2], vector<RAT>&rat)
{
	cout << "------------------------------------\n";
	cout << "\nCycle: " << cycle << endl;
	cout << setw(3) << "\n--RF--" << endl;
	for (int i = 1; i < 6; i++)
		cout << "F" << i << " |" << setw(5) << rf[i] << " |" << endl;
	cout << "------------------\n" << endl;

	cout << setw(3) << "--RAT--" << endl;
	for (int i = 1; i < 6; i++)
	{
		cout << "F" << i << " |" << setw(5) << rat[i].rat << " |" << endl;
	}
	cout << "------------------\n" << endl;

	cout << setw(3) << "--RS--" << endl;
	for (int i = 0; i < 3; i++)
	{
		cout << "RS" << i + 1 << " |" << setw(5) << rs[i].OP << " |" << setw(5);
		cout << rs[i].rs1 << " |" << setw(5) << rs[i].rs2 << " |" << endl;
	}
	cout << "------------------\n";
	cout << "BUFFER: ";
	if (buffer[0] == "0")
		cout << "empty\n" << endl;
	else
		cout << buffer[0] << endl;

	cout << "------------------\n";
	for (int i = 3; i < 5; i++)
	{
		cout << "RS" << i + 1 << " |" << setw(5) << rs[i].OP << " |" << setw(5);
		cout << rs[i].rs1 << " |" << setw(5) << rs[i].rs2 << " |" << endl;
	}
	cout << "------------------\n";
	cout << "BUFFER: ";
	if (buffer[1] == "0")
		cout << "empty\n" << endl;
	else
		cout << buffer[1] << endl;


}
