#ifndef LF_EXECLS_H
#define LF_EXECLS_H

#include <iostream>
#include <stdio.h>

class execLs
{
	public:
	static std::string escapify(const std::string& input)
	{
		int i;
		std::string output;

		for (i = 0; i < (int)input.length(); i++)
		{
			if (input[i] == ' ') output += "\\ ";
			else if (input[i] == '{') output += "\\{";
			else if (input[i] == '}') output += "\\}";
			else if (input[i] == '(') output += "\\(";
			else if (input[i] == ')') output += "\\)";
			else if (input[i] == '\'') output += "\\\'";
			else if (input[i] == '"') output += "\\\"";
			else if (input[i] == '&') output += "\\&";
			else if (input[i] == ',') output += "\\,";
			else if (input[i] == '$') output += "\\$";
			else if (input[i] == '`') output += "\\`";
			else if (input[i] == ';') output += "\\;";
			else output += input[i];
		}

		return output;
	}

	static std::string exec(const std::string& cmd)
	{
		FILE* pipe = popen(cmd.c_str(), "r");
		char buffer[128];
		std::string output = "";

		if (!pipe)
		{
			return "ERROR";
		}

		while(!feof(pipe))
		{
			if(fgets(buffer, 128, pipe))
			{
				output += buffer;
			}
		}

		pclose(pipe);
		return output;
	}

	static std::string CWD()
	{
		return exec("pwd");
	}

	static std::string dirContents(const std::string& dir)
	{
		return exec("ls -lAhR " + escapify(dir));
	} 
};

#endif

