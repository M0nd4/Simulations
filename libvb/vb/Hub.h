#pragma once /// \file
#include <cstdlib>
#include <map>
#include <string>

namespace vb {
	class Value : public std::string {
	public:
		Value (const std::string & s = "") : std::string (s) {}
		Value operator= (const std::string & s) { std::string::operator=(s); return *this; }

		operator bool()    	const { return atoi(c_str()); }
		operator int()     	const { return atoi(c_str()); }
		operator unsigned()	const { return atoi(c_str()); }
		operator long()    	const { return atoi(c_str()); }
		operator double()  	const { return atof(c_str()); }
	};

	class Hub : public std::map <char,Value> {
	public:
		Hub ();

		void init (std::string t, int argc, char ** argv, std::string c = "");

		std::string title="undefined", prog="undefined", dir="output/", help="undefined";
		std::map <char,bool> has_arg;
	};

	extern Hub H;
}
