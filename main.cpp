#include <iostream>
#include <fstream>
#include "lsOutput.h"

using namespace std;

bool readAll(const string& filename, string& output)
{
	char ch;
	ifstream ifs;
	ifs.open(filename.c_str(), ios::binary);

	if (ifs.is_open())
	{
		do
		{
			ifs.get(ch);
			if (!ifs.eof() && !ifs.bad()) {output += ch;}
		} while(!ifs.eof() && !ifs.bad());

		return true;
	}
	else
	{
		cerr << "Error: could not read from file: " << filename << endl;
		return false;
	}
}

int main(int argc, char ** argv)
{
	int i;
	string output;
	vector<string> list;

	if (argc > 1)
	{
		readAll(argv[1], output);
	}

	list = lsOutput::getPieces(output, '\n');

	for (i = 0; i < (int)list.size(); i++)
	{
		if (lsOutput::totalRow(list[i])) { cout << '"' << list[i] << "\"\n"; }
	}

	return 0;
}
