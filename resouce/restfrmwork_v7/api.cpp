#include <string.h>

#include <boost/foreach.hpp>

#include <api.hpp>
#include <strutil.hpp>			//�ַ�����ǿ��ͷ�ļ�

using namespace ourapi;

//=======================����tc������=========================
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
							"class2", "rate2", "ceil2", "prio2", "delay","type"};	//������Ը���ķ��࣬���key
	string filterkeys[] = {"act", "dev", "protocol", "pref1", "ipsrc1", "flowid1",
							"pref2", "ipsrc2", "flowid2", "type"};			//����Ը���ķ��࣬�����Ӧfilter��key

	_apikeys["/tcconfig/htb/qdisc"] = set<string>(qdisckeys, qdisckeys + 4);
	_apikeys["/tcconfig/htb/class"] = set<string>(classkeys, classkeys + 14);
	_apikeys["/tcconfig/htb/filter"] = set<string>(filterkeys, filterkeys + 10);
}

bool api::executeAPI(const string& url, const map<string, string>& argvals, string& response)
{
    // Ignore all the args except the "fields" param 
    /*��֤keys�Ƿ���ȷ*/
	validate_key vkey ;
    vkey.api = url;
	set<string> extractkey;
	map<string, string>::const_iterator it_key = argvals.begin();
	while(it_key != argvals.end())
	{
		extractkey.insert(it_key -> first);
		++it_key;
	}
	vkey.keys = &extractkey;		//vkey�����е�keys�Ѿ�������argvals�е�key
	if ( !_validate_key(&vkey))
	{
        _getInvalidResponse_keys(response);
		return false;
	}
	
    Executor::outputType type = Executor::TYPE_JSON;

    vector<string> params;

	/*��argvals��map�����У�����key������Ԫ��,������ָ���Ԫ�صĵ�����
	*/

	//------------------���url=/tcconfig/htb/qdisc�����-------------------//
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

	//----------------���url=/tcconfig/htb/class�����--------------------//
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
		
		//���������Ϣ
		map<string,string>::const_iterator it_rate = argvals.find("rate");
		prms = it_rate->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		map<string,string>::const_iterator it_ceil = argvals.find("ceil");
		prms = it_ceil->second;
		StrUtil::eraseWhiteSpace(prms);
		params.push_back(prms);

		//����1����Ϣ
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
		
		//����2����Ϣ
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

		//delay����Ϣ��ȡ
		map<string,string>::const_iterator it_delay = argvals.find("delay");
		prms = it_delay->second;
		StrUtil::eraseWhiteSpace(prms);
		//StrUtil::splitString(prms, ",", params);
		params.push_back(prms);
	}
    
	//------------------���url=/tcconfig/htb/filter�����-------------------//
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

	/* ϵͳԭ�к������ѷֱ��Ӧ��ͬ��url������ʵ��
	map<string,string>::const_iterator it1 = argvals.find("value"); 

	//�����url�д���fields������Ԫ�أ�����ֵ����prms��//
    if (it1 != argvals.end()) {
        string prms = it1->second;
        StrUtil::eraseWhiteSpace(prms);				//����strutil.hpp�ж���ĺ�����ɾ���հ�
        StrUtil::splitString(prms, ",", params);	//�ַ����и�����Զ���Ϊ�ָ�����ܶ����������params��vector����   
    }
	else{
	//-----------------���"value"�ַ���������Ĳ�����key������---------------//
	_getInvalidResponse(response);
	return false;
	//------------------------------------------------------------------------//
	}
	*/
	
	/*-----���ٽ�params�е�ֵת����uniqueparams�У�ֱ�Ӵ���params��vector������ͬʱҲ���жϲ�����Ч��------//
	/*BOOST_FOREACH���ڱ���STL����(params)����ÿ�α�����ֵ�������ַ���pr�����������uniqueparams��set������//
    BOOST_FOREACH( string pr, params ) {
        uniqueparams.insert(pr);
    }
    vdata.params = &uniqueparams;   //��uniqueparams�������ڴ�������ĵ�ַ����vdata��ͬ��ָ��һ��set������ַ��ָ��

	//�жϲ����Ƿ���Ч�������Ч�������_getInvalidResponse//
    if ( !_validate(&vdata)) {
        _getInvalidResponse(response);
        return false;
    }
	*/

	/*��argvals��map�����У�����type��ֵ�Ĳ���*/
    map<string,string>::const_iterator it_type = argvals.find("type");
	//���url����type�ֶΣ������������
    if (it_type != argvals.end()){
        const string outputtype = it_type->second;
        if (strcasecmp(outputtype.c_str(), "xml") == 0 ) { //���Դ�Сд�Ƚ������ַ���������򷵻�0
            type = Executor::TYPE_XML;						//����excutor.hpp�еĺ���������xml��ʽ���ĵ�
        }
    }

    return _executeAPI(url, params, type, response);
}

/*-----------------��д���º���----------------------------------//
bool api::_executeAPI(const string& url, const set<string>& argvals, 
        Executor::outputType type, string& response)
{
    bool ret = false;
	/*���url==/sysinfo,�����Executor���ߵ�sysinfo��linux�µ�ϵͳ�������ϵͳ��Ϣ/
    if (url == "/sysinfo") 
        ret = _executor.sysinfo(argvals, type,  response);
    if (url == "/diskinfo")
        ret = _executor.diskinfo(argvals, type, response);
    if (url == "/procinfo")
        ret = _executor.procinfo(argvals, type, response);

	//=======================����tc������=========================
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
/*�����ж�/syinfo?fields=cpus�� cpus�ֶ��Ƿ�Ϊ��Ч�ֶΡ���Ϊ��_apiparams�ж����˸��ֶεľ���ֵ
bool api::_validate(const void *data)
{
    const validate_data *vdata = static_cast<const validate_data *>(data );
    map<string, set<string> > ::iterator it =  _apiparams.find(vdata->api);		//vdata->api ���� url;

    it = _apiparams.find(vdata->api);

	/*���vdata�в�����api����/sysinfo��/procinfo�ȣ���ô���س���ĩ�˵�����������false
    if ( it == _apiparams.end()){
        return false;
    }
    set<string>::iterator it2 = vdata->params->begin(); //vdata�д��в�����������������ֵ��it2

	//����vdata�еĴ��в����ĵ��������жϲ��������Ƿ��������
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
	
	//��_apikeys��Ѱ��vkey->api���Ƿ����
    map<string, set<string> > ::iterator it =  _apikeys.find(vkey->api);		//vkey->api ���� url;

	/*���_apikeys�в����ڸ�api����/tcconfig/htb/qdisc�ȣ���ô���س���ĩ�˵�����������false*/
    if ( it == _apikeys.end()){
        return false;
    }
    set<string>::iterator it2 = vkey->keys->begin(); //vkey�д��в�����������������ֵ��it2

	//����vkey�еĴ��в����ĵ��������жϲ��������Ƿ��������
    while (it2 != vkey->keys->end()) {
        if (it->second.find(*it2) == it->second.end()) 
            return false;
        ++it2;
    }

    return true;
}