#ifndef __EXECUTOR_FOR_API__
#define __EXECUTOR_FOR_API__

#include <string>
#include <set>
#include <vector>

using std::string;
using std::set;
using std::vector;

namespace ourapi
{

class Executor
{   
public:
    enum outputType {
        TYPE_JSON, TYPE_XML   
    };
    Executor();
    bool qdiscconfig(vector<string>& args, outputType type,  string& response);
    bool classconfig(vector<string>& args, outputType type, string& response);
    bool filterconfig(vector<string>& args, outputType type, string& response);


	bool diskinfo(const set<string>& args, outputType type, string& response);
	bool procinfo(const set<string>& args, outputType type, string& response);
	bool sysinfo(const set<string>& args, outputType type, string& response);
	bool tcinfo(const set<string>& args, outputType type, string& response);

private:
    void _generateOutput(void *data, outputType type, string& output);

};



}  // namespace ourapi

#endif
