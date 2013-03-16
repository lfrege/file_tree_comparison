

#ifndef LF_LSOUTPUT_H
#define LF_LSOUTPUT_H

#include <vector>


class lsOutput
{
	public:
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

