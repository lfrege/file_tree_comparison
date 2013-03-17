

#ifndef LF_LSOUTPUT_H
#define LF_LSOUTPUT_H

#include <vector>
#include <sstream>
#include "execLs.h"

class lsOutput
{
	public:
	class dirHeader
	{
		std::string wd;

		public:
		std::vector<std::string> path;

		void load(const std::string& cwd, const std::string& indir)
		{
			int i;
			std::string dir;

			if (indir[indir.length()-1] == ':') { dir = indir.substr(0,indir.length()-1); }
			else { dir = indir; }

			std::vector<std::string> cwdv = getPieces(cwd, '/');
			std::vector<std::string> dirv = getPieces(dir, '/');
			wd = cwd;

			path.clear();

			if (dir.length() == 0 || dir[0] != '/')
			{
				for (i = 0; i < (int)cwdv.size(); i++)
				{
					if (cwdv[i] != "")
					{
						path.push_back(cwdv[i]);
					}
				}
			}

			for (i = 0; i < (int)dirv.size(); i++)
			{
				if (dirv[i] == "" || dirv[i] == ".") {	}
				else if (dirv[i] == ".." && path.size() > 0) { path.pop_back(); }
				else { path.push_back(dirv[i]); }
			}
		}

		std::string toString() const
		{
			int i;
			std::string output = "";

			for (i = 0; i < (int)path.size(); i++)
			{
				output += "/" + path[i];
			}

			return output;
		}

		std::string operator-(const std::string& rhs) const
		{
			int i;
			std::string output;
			dirHeader temp;
			temp.load(wd, rhs);

			for (i = 0; i < (int)path.size() && i < (int)temp.path.size(); i++)
			{
				if (path[i] != temp.path[i])
				{
					return toString();
				}
			}

			for (; i < (int)path.size(); i++)
			{
				output += path[i];
				if (i+1 < (int)path.size())
				{
					output += '/';
				}
			}

			return output;
		} 
	};


	class fileRow
	{
		static const int MODE_PERMISSIONS = 0;
		static const int MODE_LINKS= 1;
		static const int MODE_OWNER = 2;
		static const int MODE_GROUP = 3;
		static const int MODE_SIZE= 4;
		static const int MODE_DATE0 = 5;
		static const int MODE_DATE1 = 6;
		static const int MODE_DATE2 = 7;
		static const int MODE_NAME = 8;

		std::string permissions;
		int links;
		std::string owner;
		std::string group;
		std::string size;
		std::string date;
		std::string name;

		void parse(const std::string& input)
		{
			int i;
			int namestart = 0;
			int mode = MODE_PERMISSIONS;
			std::vector<std::string> pieces = getPieces(input, ' ');

			for (i = 0; i < (int)pieces.size() && mode <= MODE_NAME; i++)
			{
				if (pieces[i] == "")
				{
				}
				else if (mode == MODE_PERMISSIONS)
				{
					permissions = pieces[i];
					mode++;
				}
				else if (mode == MODE_LINKS)
				{
					links = toInt(pieces[i]);
					mode++;
				}
				else if (mode == MODE_OWNER)
				{
					owner = pieces[i];
					mode++;
				}
				else if (mode == MODE_GROUP)
				{
					group = pieces[i];
					mode++;
				}
				else if (mode == MODE_SIZE)
				{
					size = pieces[i];
					mode++;
				}
				else if (mode == MODE_DATE0)
				{
					date = pieces[i];
					mode++;
				}
				else if (mode == MODE_DATE1)
				{
					date += " " + pieces[i];
					mode++;
				}
				else if (mode == MODE_DATE2)
				{
					namestart = find(input, size);
					namestart = find(input, pieces[i], namestart + size.length());
					namestart += pieces[i].size();
					date += " " + pieces[i];
					mode++;
				}
				else if (mode == MODE_NAME)
				{
					name = trim(input.substr(namestart, input.length() - namestart));
					mode++;
				}
			}
		}

		public:
		fileRow() { }

		fileRow(const std::string& input)
		{
			parse(input);
		}

		std::string getName() const
		{
			return name;
		}

		bool isFile() const
		{
			if (permissions.length() > 0 && permissions[0] == '-')
			{
				return true;
			}
			return false;
		}

		std::string toString() const
		{
			return date + ", " + size +  ", " + name;
		}

		std::string diff(const fileRow& rhs) const
		{
			std::string output = name + "=";

			if (date != rhs.date) { output += "(" +date + "-" + rhs.date + ")"; }
			if (size != rhs.size) { output += "(" +size + "-" + rhs.size + ")"; }
			return output;
		}

		bool operator<(const fileRow& rhs) const { return name < rhs.name; }
		bool operator>(const fileRow& rhs) const { return name > rhs.name; }
		bool operator==(const fileRow& rhs) const { return name == rhs.name && size == rhs.size && date == rhs.date; }
	};

	class dirBody
	{
		dirHeader head;
		std::string cwd;
		std::string root;
		std::vector<fileRow> files;

		public:

		dirBody(const std::string& incwd, const std::string& inroot, const std::string& dir)
		{
			cwd = incwd;
			root = inroot;
			if (dir != "") { head.load(cwd, dir); }
			else { head.load(cwd, root); }
		}

		void addFile(const std::string& newfile)
		{
			files.push_back(fileRow(newfile));
		}

		std::string toString() const
		{
			int i;
			std::string output = absPath() + "\n";

			for (i = 0; i < (int)files.size(); i++)
			{
				if (files[i].isFile())
				{
					output += "\t" + files[i].toString() + "\n";
				}
			}

			return output;
		}

		std::string relPath() const { return head-root; }
		std::string absPath() const { return head.toString(); }

		bool operator<(const dirBody& rhs) const
		{
			return relPath() < rhs.relPath();
		}

		bool operator==(const dirBody& rhs) const
		{
			return relPath() == rhs.relPath();
		}

		std::string fullCopy(const std::string& dest) const
		{
			int i;
			std::string output;

			for (i = 0; i < (int)files.size(); i++)
			{
				if (files[i].isFile())
				{
					output += "cp -p " + execLs::escapify(absPath() + "/" + files[i].getName()) + " " + execLs::escapify(dest + "/" + files[i].getName()) + "\n";
				}
			}

			return output;
		}

		std::string operator-(const dirBody& rhs) const
		{
			int i = 0, j = 0;
			std::string output;

			while (i < (int)files.size() || j < (int)rhs.files.size())
			{
				if (i == (int) files.size() && rhs.files[j].isFile())
				{
					output += "#rm -f " + execLs::escapify(rhs.absPath() + "/" + rhs.files[j].getName()) + "\n";
					j++;
				}
				else if (j == (int) rhs.files.size() && files[i].isFile())
				{
					output += "cp -p " + execLs::escapify(absPath() + "/" + files[i].getName()) + " " + execLs::escapify(rhs.absPath() + "/" + files[i].getName()) + "\n";
					i++;
				}
				else if (i != (int)files.size() && !files[i].isFile()) { i++; }
				else if (j != (int)rhs.files.size() && !rhs.files[j].isFile()) { j++; }
				else
				{
					if (files[i] < rhs.files[j])
					{
						output += "cp -p " + execLs::escapify(absPath() + "/" + files[i].getName()) + " " + execLs::escapify(rhs.absPath() + "/" + files[i].getName()) + "\n";
						i++;
					}
					else if (files[i] > rhs.files[j])
					{
						output += "#rm -f " + execLs::escapify(rhs.absPath() + "/" + rhs.files[j].getName()) + "\n";
						j++;
					}
					else if (files[i] == rhs.files[j])
					{
						i++;
						j++;
					}
					else
					{
						std::cerr << files[i].diff(rhs.files[j]) << std::endl;
						output += "cp -p " + execLs::escapify(absPath() + "/" + files[i].getName()) + " " + execLs::escapify(rhs.absPath() + "/" + files[i].getName()) + "\n";
						i++;
						j++;
					}
				}
			}

			return output;
		}
	};


	static bool isDir(const std::string& input)
	{
		return (input.length() > 0 && !isMod(input) && !totalRow(input));
	}

	static bool isMod(const std::string& input)
	{
		if (input.length() < 10) { return false; }
		else if (input[0] != 'd' && input[0] != 'l' && input[0] != '-') { return false; }
		else if (input[1] != 'r' && input[1] != '-') { return false; }
		else if (input[2] != 'w' && input[2] != '-') { return false; }
		else if (input[3] != 'x' && input[3] != '-') { return false; }
		else if (input[4] != 'r' && input[4] != '-') { return false; }
		else if (input[5] != 'w' && input[5] != '-') { return false; }
		else if (input[6] != 'x' && input[6] != '-') { return false; }
		else if (input[7] != 'r' && input[7] != '-') { return false; }
		else if (input[8] != 'w' && input[8] != '-') { return false; }
		else if (input[9] != 'x' && input[9] != '-') { return false; }
		return true;
	}

	static bool whitespace(char input)
	{
		if (input == ' ' || input == '\t' || input == '\r' || input == '\n')
		{
			return true;
		}
		return false;
	}

	static std::string trim(const std::string& input)
	{
		int i, first = -1, last = -1;

		for (i = 0; i < (int)input.length(); i++)
		{
			if (first == -1 && !whitespace(input[i])) { first = i; }
			if (!whitespace(input[i])) { last = i; }
			if (first == -1 && input[i] == '\\') { first = i; }
			if (input[i] == '\\') { last = i + 1;  i++; }
		}

		if (first == -1) { return ""; }
		if (last == -1) { last = input.length() + 1; }
		return input.substr(first, last - first + 1);
	}

	static bool numeric(char input)
	{
		if (input <= '9' && input >= '0')
		{
			return true;
		}
		return false;
	}

	static bool numeric(const std::string input)
	{
		int i;
		for (i = 0; i < (int)input.length(); i++)
		{
			if (!numeric(input[i])) { return false; }
		}

		return true;
	}

	static bool totalRow(const std::string& input)
	{
		if (input.length() < 5) { return false; }
		else if (input.substr(0, 5) != "total") { return false; }
		else if (!numeric(trim(input.substr(5, input.length() - 5)))) { return false; }

		return true;
	}

	static std::vector<std::string> getPieces(const std::string& input, char delim)
	{
		int i, last = 0;
		std::vector<std::string> output;

		for (i = 0; i < (int)input.length(); i++)
		{
			if (input[i] == '\\') { i++; }
			else if (input[i] == delim)
			{
				output.push_back(input.substr(last, i - last));
				last = i + 1;
			}
		}

		output.push_back(input.substr(last, i - last));

		return output;
	}

	static int find(const std::string input, const std::string key, int start = 0)
	{
		int i,j;

		if (key.size() > input.size()) { return -1; }

		for (i = start; i < (int)input.size(); i++)
		{
			for (j = 0; i+j < (int)input.size() && j < (int)key.size() && input[i+j] == key[j]; j++) { }
			if (j == (int)key.size()) { return i; }
		}

		return -1;
	}

	static int toInt(const std::string input)
	{
		int output;
		std::stringstream ss(input);
		ss >> output;
		return output; 
	}

	std::string toString() const
	{
		int i;
		std::string output;

		for (i = 0; i < (int)directory.size(); i++)
		{
			output += directory[i].toString() + "\n";
		}

		return output;
	}

	void addDirectory(const std::string& newdir)
	{
		directory.push_back(dirBody(cwd, root, newdir));
	}

	void addFile(const std::string& newfile)
	{
		directory.back().addFile(newfile);
	}

	std::string operator-(const lsOutput& rhs) const
	{
		std::string output;
		int i = 0, j = 0;

		while (i < (int)directory.size() || j < (int)rhs.directory.size())
		{
			if (i == (int)directory.size())
			{
				j++;
			}
			else if (j == (int)rhs.directory.size())
			{
				output += "mkdir " + execLs::escapify(rhs.directory[0].absPath() + "/" + directory[i].relPath()) + "\n";
				output += directory[i].fullCopy(rhs.directory[0].absPath() + "/" + directory[i].relPath());
				i++;
			}
			else if (directory[i] == rhs.directory[j])
			{
				output += directory[i] - rhs.directory[j];
				i++;
				j++;
			}
			else if (directory[i] < rhs.directory[j])
			{
				output += "mkdir " + execLs::escapify(rhs.directory[0].absPath() + "/" + directory[i].relPath()) + "\n";
				output += directory[i].fullCopy(rhs.directory[0].absPath() + "/" + directory[i].relPath());
				i++;
			}
			else { j++; }
		}

		return output;
	}

	lsOutput(const std::string& incwd, const std::string& inroot)
	{
		root = trim(inroot);
		cwd = trim(incwd);
		addDirectory("");
	}

	protected:
	std::string cwd;
	std::string root;
	std::vector<dirBody> directory;
};

#endif

