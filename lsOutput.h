

#ifndef LF_LSOUTPUT_H
#define LF_LSOUTPUT_H

#include <vector>
#include <sstream>

class lsOutput
{
	public:
	class dirHeader
	{
		public:
		std::vector<std::string> path;

		void load(const std::string& cwd, const std::string& dir)
		{
			int i;
			std::vector<std::string> cwdv = getPieces(cwd, '/');
			std::vector<std::string> dirv = getPieces(dir, '/');

			path.clear();

			if (dir.length() > 0 && dir[0] != '/')
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
			std::string output;

			for (i = 0; i < (int)path.size(); i++)
			{
				output += "/" + path[i];
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
					name = input.substr(namestart, input.length() - namestart);
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

		bool isFile() const
		{
			if (permissions.length() == 0 || permissions[0] == 'd')
			{
				return false;
			}
			return true;
		}

		std::string toString() const
		{
			return date + ", " + size +  ", " + name;
		}
	};

	class dirBody
	{
		dirHeader head;
		std::string cwd;
		std::vector<fileRow> files;

		public:

		dirBody(const std::string& incwd, const std::string& dir)
		{
			cwd = incwd;
			head.load(cwd, dir);
		}

		void addFile(const std::string& newfile)
		{
			files.push_back(fileRow(newfile));
		}

		std::string toString() const
		{
			int i;
			std::string output = head.toString() + "\n";

			for (i = 0; i < (int)files.size(); i++)
			{
				if (files[i].isFile())
				{
					output += "\t" + files[i].toString() + "\n";
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
		else if (input[0] != 'd' && input[0] != '-') { return false; }
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
		directory.push_back(dirBody(cwd, newdir));
	}

	void addFile(const std::string& newfile)
	{
		directory.back().addFile(newfile);
	}

	lsOutput(const std::string& incwd, const std::string& inroot)
	{
		root = inroot;
		cwd = incwd;
		addDirectory("");
	}

	protected:
	std::string cwd;
	std::string root;
	std::vector<dirBody> directory;
};

#endif

