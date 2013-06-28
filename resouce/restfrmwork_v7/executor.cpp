#include <stdio.h>
#include <iostream>
#include <vector>
#include <sstream>

#include <stdint.h>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
/*
Boost.PropertyTree 提供了一种结构化的数据存储容器。
并且提供了一系列的解释器可以将内存中的结构与具体格式相互转换 (比如: INI, XML, JSON )。
至少可以用在:
1.进程间通讯或者跨语言的进程间的通讯
2.一些配置文件的存取
3.网络通讯协议的格式
基本用法有 2 种场景。第一种是从 Property Tree存储到具体格式。
第二种是从具体格式解析到具体的 Property Tree。
*/

#include <executor.hpp>
#include <strutil.hpp>

using namespace ourapi;
using std::vector;
using boost::property_tree::ptree;
using std::make_pair;
using boost::lexical_cast;
using boost::bad_lexical_cast;
using boost::format;
using boost::regex_search;
using boost::match_default;
using boost::match_results;
using boost::regex;


Executor::Executor()
{
}

bool Executor::diskinfo(const set<string>& args, outputType type, 
        string& response)
{
    const char *command = "df | sed 's/ \\+/ /g'  | tail -n +2 ";
    char line[255];
    vector<string> tokens;
    int i = 0,j;
    bool spaceinfo = false;
    bool totalparts = false;
    uint64_t totalspace = 0;
    uint64_t usedspace = 0;
    int32_t partnum = 0;

    FILE *fp = popen(command, "r");
    if (!fp){
        return false;
    }
    while (fgets(line, 255, fp) != 0){
        response += string(line);
    }
    fclose(fp);

    if (args.find("spaceinfo") != args.end()) {
        spaceinfo = true;
    }
    if (args.find("totalparts") != args.end()) {
        totalparts = true;
    }


    StrUtil::splitString( response, " \t\n", tokens); 
    
    j = tokens.size();
    ptree diskinforoot ;
    ptree diskinfo;

    ptree::iterator  ptit = diskinforoot.push_back(make_pair("diskinfo", diskinfo ));
    ptree::iterator pit ;
    while (i < j) {
        {
            ptree temp;
            pit = ptit->second.push_back(make_pair("FileSystem", temp));
        }
        pit->second.push_back(make_pair("Name", tokens[i++]));
        try {
            if (spaceinfo) {
                totalspace += lexical_cast<uint64_t>(tokens[i]);
            }
            pit->second.push_back(make_pair("Size", tokens[i++]));
            usedspace += lexical_cast<uint64_t>(tokens[i]);
            pit->second.push_back(make_pair("Used", tokens[i++]));

        } catch ( bad_lexical_cast& e) {
        }
        pit->second.push_back(make_pair("Avail", tokens[i++]));
        pit->second.push_back(make_pair("PercentUse", tokens[i++]));
        pit->second.push_back(make_pair("MountedOn", tokens[i++]));
        partnum++;
    }

    if (spaceinfo) {
        ptree temp;
        format fmter("%1%");
        pit = ptit->second.push_back(make_pair("SpaceInfo", temp));
        fmter % totalspace;
        pit->second.push_back(make_pair("TotalSpace", fmter.str()));
        fmter.clear();
        fmter % usedspace;
        pit->second.push_back(make_pair("UsedSpae", fmter.str()));
        fmter.clear();

    }

    if (totalparts) {
        ptree temp;
        format fmter("%1%");
        fmter % partnum;
        ptit->second.push_back(make_pair("TotalParts", fmter.str()));
        fmter.clear();
    }

    _generateOutput(&diskinforoot, type, response);
    std::cout << response << std::endl;
    return true;
}

bool Executor::procinfo(const set<string>& args, outputType type, 
        string& response)
{
    const char *command = "ps auxef | tail -n +2 |awk ' { printf \"%s %s %s %s \", $1, $2, $3, $3 ; for (i = 11; i <= NF; i++) {printf \"%s \", $i }  print \"\" }  ' ";
    char line[8096];
    FILE *fp = popen(command, "r");

    if (!fp) {
        return false;
    }

    string read_line;
    ptree prcinforoot ;
    ptree prcinfo;
    string::const_iterator start, end;
    match_results<string::const_iterator > what;
    ptree::iterator  ptit = prcinforoot.push_back(make_pair("prcinfo", prcinfo ));
    ptree::iterator pit;
    regex expression("(.*?) (.*?) (.*?) (.*?) (.*)");  
    ptree temp;
    bool percentcpu = false;
    bool percentmemory = false; 

    if (args.find("percentcpu") != args.end()) {
        percentcpu = true;
    }
    if (args.find("percentmemory") != args.end()) {
        percentmemory = true;
    }

    while (fgets(line, 8096, fp) != 0){
        read_line = line;
        start = read_line.begin();
        end = read_line.end();
        if (!regex_search(start, end, what, expression, match_default)){
            continue;
        }
        if (what.size() != 6){
            continue;
        }   
        pit = ptit->second.push_back(make_pair("process", temp));
        pit->second.push_back(make_pair("owner", string(what[1].first, what[1].second)));
        pit->second.push_back(make_pair("processid", string(what[2].first, what[2].second)));
        if (percentcpu)
            pit->second.push_back(make_pair("percentcpu", string(what[3].first, what[3].second)));
        if (percentmemory)
            pit->second.push_back(make_pair("percentmemory", string(what[4].first, what[4].second)));
        pit->second.push_back(make_pair("processcommand", string(what[5].first, what[5].second)));
    }
    fclose(fp);    
    _generateOutput(&prcinforoot, type, response);
    std::cout << response << std::endl;
    return true;
}

bool Executor::sysinfo(const set<string>& args, outputType type, 
        string& response)
{
    const char *commandcpu = "cat /proc/cpuinfo |  sed 's/\\s\\+: /:/g'";
    const char *commandmemory = "cat /proc/meminfo |  sed 's/:\\s\\+/:/g'";
    const char *commandos = "uname -a";
    FILE *fp;
    char commandout[1048];
    string line;
    ptree sysinforoot ;
    ptree sysinfo;
    ptree::iterator  ptit = sysinforoot.push_back(make_pair("sysinfo", sysinfo ));

    while (args.empty() || args.find("cpus") != args.end()) {
        fp = popen(commandcpu, "r");
        if (!fp)
            break;
        ptree temp;
        string field;
        string value;
        size_t index;
        ptree::iterator pit;
        while (fgets(commandout, 1048, fp) != 0){
            line = commandout;
            StrUtil::eraseAllChars(line, ")( \r\n\t");
            if (strncasecmp(line.c_str(),"processor:", 10) == 0) {
                pit = ptit->second.push_back(make_pair("cpus", temp));
            }
            index = line.find(":");
            if (string::npos == index)
                continue;
            field = line.substr(0, index);
            value = line.substr(index + 1);
            pit->second.push_back(make_pair(field, value));
        }
        fclose(fp);
        break;
    }
    
    while (args.empty()  ||  args.find("memory") != args.end()) {
        fp = popen(commandmemory, "r");
        if (!fp)
            break;
        ptree temp;
        string field;
        string value;
        size_t index;
        ptree::iterator pit = ptit->second.push_back(make_pair("memory", temp));
        while (fgets(commandout, 1048, fp) != 0){
            line = commandout;
            StrUtil::eraseAllChars(line, ")( \n\r\t");
            index = line.find(":");
            if (string::npos == index)
                continue;
            field = line.substr(0, index );
            value = line.substr(index + 1);
            pit->second.push_back(make_pair(field, value));
        }
        fclose(fp);
        break;
    }
    while (args.empty() || args.find("os") != args.end()) {
		/*popen() 函数通过创建一个管道，调用 fork 产生一个子进程，执行一个 shell 以运行命令来开启一个进程.
		向这个流写内容相当于写入该命令的标准输入；命令的标准输出和调用 popen 的进程相同。
		与之相反的，从流中读数据相当于读取命令的标准输出；
		*/
        fp = popen(commandos, "r");					
        if (!fp)
            break;
		/*char *fgets(char *buf, int bufsize, FILE *stream);
		从文件结构体指针stream中读取数据，每次读取一行。
		读取的数据保存在buf指向的字符数组中，每次最多读取bufsize-1个字符（第bufsize个字符赋'\0'），
		如果文件中的该行，不足bufsize个字符，则读完该行就结束。
		如果函数读取成功，则返回指针buf，失败则返回NULL。
		*/
        if (fgets(commandout, 1048, fp) == 0) {
            fclose(fp);
            break;
        }
        line = commandout;
        ptree temp;
        string field;
        string value;
        size_t index;
        ptree::iterator pit = ptit->second.push_back(make_pair("os", temp));
        pit->second.push_back(make_pair("osdetails", line));
        fclose(fp);
        break;
    }

    _generateOutput(&sysinforoot, type, response);
    std::cout << response << std::endl;			//把output/response中的内容输出

    return true;
}

//=======================加入tc的请求=========================
bool Executor::tcinfo(const set<string>& args, outputType type, 
        string& response)
{
	const char *commandos = "ls";
	FILE *fp;
	char commandout[1024];
	string line;
	ptree tcinforoot;
	ptree tcinfo;
	ptree::iterator ptit = tcinforoot.push_back(make_pair("tcinfo", tcinfo ));

	while (args.empty() || args.find("qdisc") != args.end()) {
        fp = popen(commandos, "r");
        if (!fp)
            break;
        if (fgets(commandout, 1048, fp) == 0) {
            fclose(fp);
            break;
        }
        line = commandout;
        ptree temp;
        string field;
        string value;
		//--------------显示自定义内容----------------------//
		//string show;
		//show = line + url;
		//--------------------------------------------------//
        size_t index;
        ptree::iterator pit = ptit->second.push_back(make_pair("tc", temp));
        pit->second.push_back(make_pair("tcdetails", line));
        fclose(fp);
        break;
    }

    _generateOutput(&tcinforoot, type, response);
    std::cout << response << std::endl;

    return true;

}
//============================================================

bool Executor::qdiscconfig(vector<string>& args, outputType type,  string& response)
{
	char cmd_qdisc_del[1024];
	char cmd_qdisc_add[1024];
//	char cmd_netem[1024];
	char cmd_qdisc_show[1024];
	char commandout[10240];
	FILE *fp;

	if(args[0] == "add")
	{
//		sprintf(cmd_qdisc_del, "tc qdisc del dev %s root", args[1].c_str());
//		fp = popen(cmd_qdisc_del, "r");
		sprintf(cmd_qdisc_add, "tc qdisc %s dev %s root handle 1: htb default %s", args[0].c_str(), args[1].c_str(), args[2].c_str());
		fp = popen(cmd_qdisc_add, "r");
//		sprintf(cmd_netem, "tc qdisc %s dev %s parent 1:1 netem delay %s", args[0].c_str(), args[1].c_str(), args[3].c_str());
//		fp = popen(cmd_netem, "r");
		sprintf(cmd_qdisc_show, "tc qdisc show dev %s", args[1].c_str());
		fp = popen(cmd_qdisc_show, "r");
	}
	else
	{
		sprintf(cmd_qdisc_del, "tc qdisc del dev %s root", args[1].c_str());
		fp = popen(cmd_qdisc_del, "r");
		sprintf(cmd_qdisc_show, "tc qdisc show dev %s", args[1].c_str());
		fp = popen(cmd_qdisc_show, "r");
	}

	string line;
	/*
	if(fgets(commandout, 1048, fp) == 0)
	{
		fclose(fp);
		return false;
	}
	else
	
	while (fgets(commandout, 1048, fp) != 0)
	{
		line = line + " " + commandout;
	}
*/
	if(fp)
	{
		int iterator = 0;
		int ch = 0;
		while (!feof(fp))
		{
			ch = fgetc(fp);
			if (ch == EOF)
				break;
			commandout[iterator] = ch;
			iterator++;
		}
		commandout[iterator] = 0;
	}

	ptree root;
	ptree qdiscinfo;
	ptree::iterator ptit = root.push_back(make_pair("tcinfo", qdiscinfo ));

	line = commandout;
    ptree temp;

	if(line.empty())
	{
		line = "The value of key is wrong";
	}

    ptree::iterator pit = ptit->second.push_back(make_pair("qdiscinfo", temp));
    pit->second.push_back(make_pair("qdiscdetals", line));
    fclose(fp);

	_generateOutput(&root, type, response);
    std::cout << response << std::endl;

    return true;

}

//针对class的配置信息
bool Executor::classconfig(vector<string>& args, outputType type,  string& response)
{
	char cmd_class_root_del[1024];
	char cmd_class_root_add[1024];
	char cmd_class_class1_add[1024];
	char cmd_class_class1_del[1024];
	char cmd_class_class2_add[1024];
	char cmd_class_class2_del[1024];
	char cmd_qdisc_netem1_add[1024];
	char cmd_qdisc_netem2_add[1024];
	char cmd_class_show[1024];
	char commandout[10240];
	FILE *fp;
	string line;

	if(args[0] == "add")
	{
		sprintf(cmd_class_root_add, 
			"tc class %s dev %s parent 1: classid 1:1 htb rate %skbit ceil %skbit", 
			args[0].c_str(), args[1].c_str(), args[2].c_str(), args[3].c_str());
		fp = popen(cmd_class_root_add, "r");
		
		sprintf(cmd_class_class1_add, 
			"tc class %s dev %s parent 1:1 classid 1:%s htb rate %skbit ceil %skbit prio %s", 
			args[0].c_str(), args[1].c_str(), args[4].c_str(), args[5].c_str(), args[6].c_str(), args[7].c_str());
		fp = popen(cmd_class_class1_add, "r");
		
		sprintf(cmd_class_class2_add, 
			"tc class %s dev %s parent 1:1 classid 1:%s htb rate %skbit ceil %skbit prio %s", 
			args[0].c_str(), args[1].c_str(), args[8].c_str(), args[9].c_str(), args[10].c_str(), args[11].c_str());
		fp = popen(cmd_class_class2_add, "r");

		sprintf(cmd_qdisc_netem1_add,
			"tc qdisc %s dev %s parent 1:%s handle %s:1 netem delay %sms",
			args[0].c_str(), args[1].c_str(), args[4].c_str(), args[4].c_str(), args[12].c_str());
		fp = popen(cmd_qdisc_netem1_add, "r");

		sprintf(cmd_qdisc_netem2_add,
			"tc qdisc %s dev %s parent 1:%s handle %s:1 netem delay %sms",
			args[0].c_str(), args[1].c_str(), args[8].c_str(), args[8].c_str(), args[12].c_str());
		fp = popen(cmd_qdisc_netem2_add, "r");

		sprintf(cmd_class_show, "tc class show dev %s", args[1].c_str());
		fp = popen(cmd_class_show, "r");
	}
	else
	{
		sprintf(cmd_class_class1_del,
			"tc class del dev %s parent 1:1 classid 1:%s htb rate %skbit",
			args[1].c_str(), args[4].c_str(), args[5].c_str());
		fp = popen(cmd_class_class1_del, "r");

		sprintf(cmd_class_class2_del,
			"tc class del dev %s parent 1:1 classid 1:%s htb rate %skbit",
			args[1].c_str(), args[8].c_str(), args[9].c_str());
		fp = popen(cmd_class_class2_del, "r");

		sprintf(cmd_class_root_del,
			"tc class del dev %s parent 1: classid 1:1 htb rate %skbit",
			args[1].c_str(), args[2].c_str());
		fp = popen(cmd_class_root_del, "r");

//		sprintf(cmd_class_show, "tc class show dev %s", args[1].c_str());
//		fp = popen(cmd_class_show, "r");
		
	}

	/*
	if(fgets(commandout, 1048, fp) == 0)
	{
		fclose(fp);
		return false;
	}
	else
	
	while (fgets(commandout, 1048, fp) != 0)
	{
		line = line + " " + commandout;
	}
*/
	if(fp)
	{
		int iterator = 0;
		int ch = 0;
		while (!feof(fp))
		{
			ch = fgetc(fp);
			if (ch == EOF)
				break;
			commandout[iterator] = ch;
			iterator++;
		}
		commandout[iterator] = 0;
	}

	ptree root;
	ptree classinfo;
	ptree::iterator ptit = root.push_back(make_pair("tcinfo", classinfo ));

	line = commandout;
    ptree temp;

	if(line.empty())
	{
		if(args[0] != "del")
		{
			line = "The value of key is wrong";
		}
		else
		{
			line = "There is no class";
		}
	}

    ptree::iterator pit = ptit->second.push_back(make_pair("classinfo", temp));
    pit->second.push_back(make_pair("classdetals", line));
    fclose(fp);

	_generateOutput(&root, type, response);
    std::cout << response << std::endl;

    return true;

}

bool Executor::filterconfig(vector<string>& args, outputType type,  string& response)
{
	char cmd_filter_filter1_del[1024];
	char cmd_filter_filter2_del[1024];
	char cmd_filter_filter1_add[1024];
	char cmd_filter_filter2_add[1024];
	char cmd_filter_show[1024];
	char commandout[10240];
	FILE *fp;

	if(args[0] == "add")
	{

	/*///////////////////////////////
		sprintf(cmd_class_del,
			"tc class del dev %s parent 1: classid 1:1 htb",
			args[1].c_str());
		fp = popen(cmd_qdisc_del, "r");
	*////////////////////////////////////

		sprintf(cmd_filter_filter1_add, 
			"tc filter %s dev %s protocol %s parent 1:0 pref %s u32 match ip src %s flowid 1:%s", 
			args[0].c_str(), args[1].c_str(), args[2].c_str(), args[3].c_str(), args[4].c_str(), args[5].c_str());
		fp = popen(cmd_filter_filter1_add, "r");

		sprintf(cmd_filter_filter2_add, 
			"tc filter %s dev %s protocol %s parent 1:0 pref %s u32 match ip src %s flowid 1:%s", 
			args[0].c_str(), args[1].c_str(), args[2].c_str(), args[6].c_str(), args[7].c_str(), args[8].c_str());
		fp = popen(cmd_filter_filter2_add, "r");

		sprintf(cmd_filter_show, "tc filter show dev %s", args[1].c_str());
		fp = popen(cmd_filter_show, "r");
	}
	else
	{
		sprintf(cmd_filter_filter1_del,
			"tc filter %s dev %s protocol %s parent 1:0 pref %s",
			args[0].c_str(), args[1].c_str(), args[2].c_str(), args[3].c_str());
		fp = popen(cmd_filter_filter1_del, "r");
		
		sprintf(cmd_filter_filter2_del,
			"tc filter %s dev %s protocol %s parent 1:0 pref %s",
			args[0].c_str(), args[1].c_str(), args[2].c_str(), args[6].c_str());
		fp = popen(cmd_filter_filter2_del, "r");
		
		sprintf(cmd_filter_show, "tc filter show dev %s", args[1].c_str());
		fp = popen(cmd_filter_show, "r");
	}

	string line;
	/*
	if(fgets(commandout, 1048, fp) == 0)
	{
		fclose(fp);
		return false;
	}
	else
	
	while (fgets(commandout, 1048, fp) != 0)
	{
		line = line + " " + commandout;
	}
*/
	if(fp)
	{
		int iterator = 0;
		int ch = 0;
		while (!feof(fp))
		{
			ch = fgetc(fp);
			if (ch == EOF)
				break;
			commandout[iterator] = ch;
			iterator++;
		}
		commandout[iterator] = 0;
	}

	ptree root;
	ptree filterinfo;
	ptree::iterator ptit = root.push_back(make_pair("tcinfo", filterinfo ));

	line = commandout;
    ptree temp;

	if(line.empty())
	{
		line = "There is no filter";
	}

    ptree::iterator pit = ptit->second.push_back(make_pair("filterinfo", temp));
    pit->second.push_back(make_pair("filterdetals", line));
    fclose(fp);

	_generateOutput(&root, type, response);
    std::cout << response << std::endl;

    return true;

}


void Executor::_generateOutput(void *data, outputType type, string& output)
{
    /*ostringstream类通常用于执行C风格的串流的输出操作，格式化字符串，避免申请大量的缓冲区，替代sprintf
	*/
	std::ostringstream ostr;
    ptree *pt = (ptree *) data;
    if (TYPE_JSON == type)
        write_json(ostr, *pt);			//把pt指向的ptree的内容，写成json格式存入到ostr中
    else if (TYPE_XML == type)
        write_xml(ostr, *pt);

    output = ostr.str();				//把ostr中内容转换中string，存入到output,外部可以访问。
}

