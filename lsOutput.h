

#ifndef LF_LSOUTPUT_H
#define LF_LSOUTPUT_H

#include <vector>


class lsOutput
{
	public:
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
			if (input[i] == delim)
			{
				output.push_back(input.substr(last, i - last));
				last = i + 1;
			}
		}

		output.push_back(input.substr(last, i - last));

		return output;
	}

	class dirHeader
	{
		public:
		std::vector<std::string> path;

		void load(const std::string& cwd, const std::string& path)
		{
			
		}
	};
};

#endif

