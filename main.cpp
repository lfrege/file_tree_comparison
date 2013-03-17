#include <iostream>
#include <fstream>
#include "lsOutput.h"
#include "execLs.h"

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
	string dir1, dir2;
	vector<string> list1, list2;

	if (argc != 3)
	{
		cerr << "Error: 2 arguments required." << endl;
		return -1;
	}

	lsOutput ls1(execLs::CWD(), argv[1]);
	lsOutput ls2(execLs::CWD(), argv[2]);

	dir1 = execLs::dirContents(argv[1]);
	dir2 = execLs::dirContents(argv[2]);

	list1 = lsOutput::getPieces(dir1, '\n');

	for (i = 0; i < (int)list1.size(); i++)
	{
		if (lsOutput::isDir(list1[i]))
		{
			ls1.addDirectory(list1[i]);
		}
		else if (lsOutput::isMod(list1[i]))
		{
			ls1.addFile(list1[i]);
		}
	}

	list2 = lsOutput::getPieces(dir2, '\n');
	for (i = 0; i < (int)list2.size(); i++)
	{
		if (lsOutput::isDir(list2[i]))
		{
			ls2.addDirectory(list2[i]);
		}
		else if (lsOutput::isMod(list2[i]))
		{
			ls2.addFile(list2[i]);
		}
	}

	cout << ls1 - ls2 << endl;

	return 0;
}
