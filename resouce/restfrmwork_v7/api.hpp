#ifndef __API_FOR_REST_OR_WHATEVER__
#define __API_FOR_REST_OR_WHATEVER__

#include <map>
#include <string>
#include <set>
#include <executor.hpp>
#include <vector>

using std::map;
using std::string;
using std::set;
using std::vector;

namespace  ourapi
{

class api
{
public:
    api();
    bool executeAPI(const string& url, const map<string, string>& argval, 
            string& response);
private:
    Executor _executor;
    bool _validate_key(const void*  key);
    bool _executeAPI(const string& url, vector<string>& argvals, 
            Executor::outputType type, string& response);
    void _getInvalidResponse(string& response);
    void _getInvalidResponse_keys(string& response);
	map<string, set<string> > _apikeys;

};

} // namespace ourapi ends

#endif
