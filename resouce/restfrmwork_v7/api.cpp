#include <string.h>

#include <boost/foreach.hpp>

#include <api.hpp>
#include <strutil.hpp>			//字符串加强的头文件

using namespace ourapi;

//=======================加入tc的请求=========================
//============================================================
struct validate_key
{
    string api;
    set <string>* keys; 
};

api::api()
{
//    set<string> keys;
	string qdisckeys[] = {"act", "dev", "default", "type"};
	string classkeys[] = {"act", "dev", "rate", "ceil", "class1", "rate1", "ceil1", "prio1", 
							"class2", "rate2", "ceil2", "prio2", "delay","type"};	//可以针对更多的分类，添加key
	string filterkeys[] = {"act", "dev", "protocol", "pref1", "ipsrc1", "flowid1",
							"pref2", "ipsrc2", "flowid2", "type"};			//可针对更多的分类，添加相应filter的key

	_apikeys["/tcconfig/htb/qdisc"] = set<string>(qdisckeys, qdisckeys + 4);
	_apikeys["/tcconfig/htb/class"] = set<string>(classkeys, classkeys + 14);
	_apikeys["/tcconfig/htb/filter"] = set<string>(filterkeys, filterkeys + 10);
}

bool api::executeAPI(const string& url, const map<string, string>& argvals, string& response)
{
    // Ignore all the args except the "fields" param 
    /*验证keys是否正确*/
	validate_key vkey ;
    vkey.api = url;
	set<string> extractkey;
	map<string, string>::const_iterator it_key = argvals.begin();
	while(it_key != argvals.end())
	{
		extractkey.insert(it_key -> first);
		++it_key;
	}
	vkey.keys = &extractkey;		//vkey对象中的keys已经存有了argvals中的key
	if ( !_validate_key(&vkey))
	{
        _getInvalidResponse_keys(response);
		return false;
	}
	
    Executor::outputType type = Executor::TYPE_JSON;

    vector<string> params;

	/*在argvals的map容器中，查找key索引的元素,并返回指向该元素的迭代器
	*/

	//------------------针对url=/tcconfig/htb/qdisc的设计-------------------//
	if (url == "/tcconfig/htb/qdisc")
	{
		map<string,string>::const_iterator it_act = argvals.find("act");
		string prms = it_act->second;
		StrUtil::eraseWhiteSpace(prms);
		//StrUtil::splitString(prms, ",", params);
		params.push_back(prms);

		map<string,string>::const_iterator it_dev = argvals.find("dev");
		prms = it_dev->second;
		StrUtil::eraseWhiteSpace(prms);
		//StrUtil::splitString(prms, ",", params);
		params.push_back(prms);

		map<string,string>::const_iterator it_default = argvals.find("default");
		prms = it_default->second;
		StrUtil::eraseWhiteSpace(prms);
		//StrUtil::splitString(prms, ",", params);
		params.push_back(prms);

	}
	//---------------------------------------------------------------------//

	//----------------针对url=/tcconfig/htb/class的设计--------------------//
	if (url == "/tcconfig/htb/class")
	{
		map<string,string>::const_iterator it_act = argvals.find("act");
		string prms = it_act->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_dev = argvals.find("dev");
		prms = it_dev->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);
		
		//主分类的信息
		map<string,string>::const_iterator it_rate = argvals.find("rate");
		prms = it_rate->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_ceil = argvals.find("ceil");
		prms = it_ceil->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		//分类1的信息
		map<string,string>::const_iterator it_class1 = argvals.find("class1");
		prms = it_class1->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_rate1 = argvals.find("rate1");
		prms = it_rate1->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_ceil1 = argvals.find("ceil1");
		prms = it_ceil1->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_prio1 = argvals.find("prio1");
		prms = it_prio1->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);
		
		//分类2的信息
		map<string,string>::const_iterator it_class2 = argvals.find("class2");
		prms = it_class2->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_rate2 = argvals.find("rate2");
		prms = it_rate2->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_ceil2 = argvals.find("ceil2");
		prms = it_ceil2->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_prio2 = argvals.find("prio2");
		prms = it_prio2->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		//delay等信息提取
		map<string,string>::const_iterator it_delay = argvals.find("delay");
		prms = it_delay->second;
		StrUtil::eraseWhiteSpace(prms);
		//StrUtil::splitString(prms, ",", params);
		params.push_back(prms);
	}
    
	//------------------针对url=/tcconfig/htb/filter的设计-------------------//
	if (url == "/tcconfig/htb/filter")
	{
		map<string,string>::const_iterator it_act = argvals.find("act");
		string prms = it_act->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_dev = argvals.find("dev");
		prms = it_dev->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_protocal = argvals.find("protocol");
		prms = it_protocal->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_filter_prio1 = argvals.find("pref1");
		prms = it_filter_prio1->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_ipsrc1 = argvals.find("ipsrc1");
		prms = it_ipsrc1->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_flowid1 = argvals.find("flowid1");
		prms = it_flowid1->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_filter_prio2 = argvals.find("pref2");
		prms = it_filter_prio2->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_ipsrc2 = argvals.find("ipsrc2");
		prms = it_ipsrc2->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_flowid2 = argvals.find("flowid2");
		prms = it_flowid2->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);
	}
	//---------------------------------------------------------------------//

	/* 系统原有函数，已分别对应不同的url进行了实现
	map<string,string>::const_iterator it1 = argvals.find("value"); 

	//如果在url中存在fields索引的元素，把其值赋给prms，//
    if (it1 != argvals.end()) {
        string prms = it1->second;
        StrUtil::eraseWhiteSpace(prms);				//调用strutil.hpp中定义的函数，删除空白
        StrUtil::splitString(prms, ",", params);	//字符串切割函数，以逗号为分割，将可能多个参数存入params的vector容器   
    }
	else{
	//-----------------如果"value"字符串（传入的参数的key）错了---------------//
	_getInvalidResponse(response);
	return false;
	//------------------------------------------------------------------------//
	}
	*/
	
	/*-----不再将params中的值转换到uniqueparams中，直接传递params的vector容器，同时也不判断参数有效性------//
	/*BOOST_FOREACH用于遍历STL容器(params)，把每次遍历的值都赋给字符串pr，并把其插入uniqueparams的set容器中//
    BOOST_FOREACH( string pr, params ) {
        uniqueparams.insert(pr);
    }
    vdata.params = &uniqueparams;   //将uniqueparams容器（内存参数）的地址赋给vdata中同样指向一个set容器地址的指针

	//判断参数是否有效，如果无效，则调用_getInvalidResponse//
    if ( !_validate(&vdata)) {
        _getInvalidResponse(response);
        return false;
    }
	*/

	/*在argvals的map容器中，查找type键值的参数*/
    map<string,string>::const_iterator it_type = argvals.find("type");
	//如果url中有type字段，且其参数存在
    if (it_type != argvals.end()){
        const string outputtype = it_type->second;
        if (strcasecmp(outputtype.c_str(), "xml") == 0 ) { //忽略大小写比较两个字符串，相等则返回0
            type = Executor::TYPE_XML;						//调用excutor.hpp中的函数，返回xml格式的文档
        }
    }

    return _executeAPI(url, params, type, response);
}

/*-----------------重写以下函数----------------------------------//
bool api::_executeAPI(const string& url, const set<string>& argvals, 
        Executor::outputType type, string& response)
{
    bool ret = false;
	/*如果url==/sysinfo,则调用Executor工具的sysinfo（linux下的系统命令）返回系统信息/
    if (url == "/sysinfo") 
        ret = _executor.sysinfo(argvals, type,  response);
    if (url == "/diskinfo")
        ret = _executor.diskinfo(argvals, type, response);
    if (url == "/procinfo")
        ret = _executor.procinfo(argvals, type, response);

	//=======================加入tc的请求=========================
	if (url == "/tcinfo/show")
		ret = _executor.tcinfo(argvals, type, response);
	//============================================================
    
	return ret;
}
*/

bool api::_executeAPI(const string& url, vector<string>& argvals, 
        Executor::outputType type, string& response)
{
    bool ret = false;
	if (url == "/tcconfig/htb/qdisc")
		ret = _executor.qdiscconfig(argvals, type, response);
	if(url == "/tcconfig/htb/class")
		ret = _executor.classconfig(argvals, type, response);
	if(url == "/tcconfig/htb/filter")
		ret = _executor.filterconfig(argvals, type, response);
	//============================================================
    
	return ret;
}

/*
/*用于判断/syinfo?fields=cpus中 cpus字段是否为有效字段。因为在_apiparams中定义了该字段的具体值
bool api::_validate(const void *data)
{
    const validate_data *vdata = static_cast<const validate_data *>(data );
    map<string, set<string> > ::iterator it =  _apiparams.find(vdata->api);		//vdata->api 等于 url;

    it = _apiparams.find(vdata->api);

	/*如果vdata中不存在api：如/sysinfo，/procinfo等，那么返回超过末端迭代器，返回false
    if ( it == _apiparams.end()){
        return false;
    }
    set<string>::iterator it2 = vdata->params->begin(); //vdata中存有参数的容器迭代器赋值给it2

	//遍历vdata中的存有参数的迭代器，判断参数个数是否存在问题
    while (it2 != vdata->params->end()) {
        if (it->second.find(*it2) == it->second.end()) 
            return false;
        ++it2;
    }

    return true;
}
*/
/*
void api::_getInvalidResponse(string& response)
{
    response = "Some error in your keys";
}
*/
void api::_getInvalidResponse_keys(string& response)
{
	response = "Some error in your validate_key";
}

bool api::_validate_key(const void *key)
{
	const validate_key *vkey = static_cast<const validate_key *>(key);
	
	//在_apikeys中寻找vkey->api，是否存在
    map<string, set<string> > ::iterator it =  _apikeys.find(vkey->api);		//vkey->api 等于 url;

	/*如果_apikeys中不存在该api：如/tcconfig/htb/qdisc等，那么返回超过末端迭代器，返回false*/
    if ( it == _apikeys.end()){
        return false;
    }
    set<string>::iterator it2 = vkey->keys->begin(); //vkey中存有参数的容器迭代器赋值给it2

	//遍历vkey中的存有参数的迭代器，判断参数个数是否存在问题
    while (it2 != vkey->keys->end()) {
        if (it->second.find(*it2) == it->second.end()) 
            return false;
        ++it2;
    }

    return true;
}