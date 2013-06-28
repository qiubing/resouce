#include <signal.h>
#include <pthread.h>
#include <platform.h>
#include <microhttpd.h>
#include <iostream>
#include <map>
#include <string>

#include <api.hpp>

using std::map;
using std::string;

#define PAGE "<html><head><title>Error</title></head><body>Bad data</body></html>"

static int shouldNotExit = 1;

static int send_bad_response( struct MHD_Connection *connection)
{
    static char *bad_response = (char *)PAGE;
    int bad_response_len = strlen(bad_response);
    int ret;
    struct MHD_Response *response;

    response = MHD_create_response_from_buffer ( bad_response_len,
                bad_response,MHD_RESPMEM_PERSISTENT);
    if (response == 0){
        return MHD_NO;
    }
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);
    return ret;
}


static int get_url_args(void *cls, MHD_ValueKind kind,
                    const char *key , const char* value)
{
    map<string, string> * url_args = static_cast<map<string, string> *>(cls);

    if (url_args->find(key) == url_args->end()) {
         if (!value)
             (*url_args)[key] = "";
         else
		 {
            (*url_args)[key] = value;
		////////////输出url的key值/////////////////////////	
	//		std::cout << "The key is " << key << std::endl;
	//		std::cout << "The value of key is" << (*url_args)[key] << std::endl; 
		//	printf("%s: %s\n", key, (*url_args)[key]);
		////////////////////////////////////////////////
		 }
    }
    return MHD_YES;

}
                
static int url_handler (
//void *cls由用户设置
void *cls,
//struct MHD_Connection *connection用来描述一个连接
    struct MHD_Connection *connection,
    //const char *url字符串，请求的文件，注意只包含路径，不包含?以后的东西
    const char *url,
    //const char *method 字符串，请求方法比如"GET"之类的。
    const char *method,
   // const char *version 字符串，HTTP协议版本
    const char *version,
    //const char *upload_data 不是字符串，提交数据，Lib提供了函数，一般不直接处理。
    const char *upload_data,
    //size_t *upload_data_size 提交数据的长度
    size_t *upload_data_size, void **ptr)
{
    static int aptr;
    const char *fmt = (const char *)cls;
    const char *val;
    char *me;
    const char *typexml = "xml";
    const char *typejson = "json";
    const char *type = typejson;

    //这是一个响应结构，包含了HTTP头，消息体结构。
    struct MHD_Response *response;
    int ret;
    map<string, string> url_args;
    map<string, string>:: iterator  it;
    ourapi::api callapi;
    string respdata;

    // Support only GET for demonstration
    if (0 != strcmp (method, "GET")) //比较两个字符串是否相同，看是否为get方式，相等返回0
        return MHD_NO; 


    if (&aptr != *ptr) {
        *ptr = &aptr;
        return MHD_YES;
    }


    if (MHD_get_connection_values (connection, MHD_GET_ARGUMENT_KIND, 
                           get_url_args, &url_args) < 0) {
        return send_bad_response(connection);

    }

	////////////////////////输出url///////////////////
	//std::cout << "The url is " <<  url << std::endl;
	//////////////////////////////////////////////////
	
	callapi.executeAPI(url, url_args, respdata);		//调用api.cpp中操作函数，执行操作

    *ptr = 0;                  /* reset when done */
    val = MHD_lookup_connection_value (connection, MHD_GET_ARGUMENT_KIND, "q");
    me = (char *)malloc (respdata.size() + 1);
    if (me == 0)
        return MHD_NO;
    strncpy(me, respdata.c_str(), respdata.size() + 1);
    /**
	 * 这个函数从一个buffer中创建了响应。并且一个响应结构可以用多次。
	 * 你可以从buffer中创建，也可以和（真实文件的）文件描述符fd关联。
	 * 销毁时会释放内存，关闭文件描述符
	 */
	response = MHD_create_response_from_buffer (strlen (me), me,
					      MHD_RESPMEM_MUST_FREE);
    if (response == 0){
        free (me);
        return MHD_NO;
    }

    it = url_args.find("type");
    if (it != url_args.end() && strcasecmp(it->second.c_str(), "xml") == 0)
        type = typexml;

    MHD_add_response_header(response, "Content-Type", "text");
    MHD_add_response_header(response, "OurHeader", type);

	//把响应加入发送队列，返回是否成功（MHD_YES,MHD_NO）
    ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
    MHD_destroy_response (response);//销毁响应结构
    return ret;
}

void handle_term(int signo)
{
    shouldNotExit = 0;
}

void* http(void *arg)
{
    int *port = (int *)arg;
    struct MHD_Daemon *d;

    /*建一个服务器，lib会新开一个线程处理请求。
	 * 根据传入的参数，下面函数有如下意义：
	 * 线程模式：只使用一个线程
	 * 端口port
	 *入站连接鉴权回调函数:设置为NULL就允许所有连接。
	 上一个函数的cls参数:NULL
	 接下来还可以可选地设置很多参数。
	 以MHD_OPTION_END结束。
	 * */
	d = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG | MHD_USE_POLL,
                       *port,
                        0, 0, &url_handler, (void *)PAGE, MHD_OPTION_END);
    if (d == 0){
        return 0;
    }
    while(shouldNotExit) {
        sleep(1);
    }
    MHD_stop_daemon (d);
    return 0;
}

int main (int argc, char *const *argv)
{

    if (argc != 2){
        printf ("%s PORT\n", argv[0]);
        exit(1);
    }
    
	/*创建守护进程，即希望脱离控制台，以守护进程形式在后台运行
	函数原型：int daemon(int nochdir, int noclose);
	当nochdir为0时，daemon将更改进程的根目录为root(“/”)。
	当noclose为0是，daemon将进程的STDIN, STDOUT, STDERR都重定向到/dev/null
	成功返回0，失败返回-1
	*/

	daemon(0,0);			

	/*设置某一信号的对应动作,
	第一个参数signum指明了所要处理的信号类型,SIGTERM请求中止进程，kill命令缺省发送
	第二个参数handler描述了与信号关联的动作,调用handle_term（Line 117）
	*/
    signal(SIGTERM, handle_term);

    int port = atoi(argv[1]);			//把服务器端口值转换成整形（atoi）之后，赋给port

    pthread_t  thread;					//把thread定义成为一个表示线程id类型的对象

	/*Linux环境创建线程函数
	第一个参数为指向线程标识符的指针;
	第二个参数用来设置线程属性;(空指针表示默认属性)
	第三个参数是线程运行函数的起始地址;（另一个线程运行自定义的http函数）
	最后一个参数是运行函数的参数。
	当创建线程成功时，函数返回0，若不为0则说明创建线程失败，常见的错误返回代码为EAGAIN和EINVAL。
	前者表示系统限制创建新的线程，例如线程数目过多了；后者表示第二个参数代表的线程属性值非法。
	创建线程成功后，新创建的线程则运行参数三和参数四确定的函数，原来的线程则继续运行下一行代码。
	*/
    if ( 0 != pthread_create(&thread, 0 , http, &port)){
        exit(1); //线程没有创建成功，非正常退出
    }

    /*以阻塞的方式等待thread指定的线程结束。
	主线程会一直等待直到等待的线程结束自己才结束。
	当函数返回时，被等待线程的资源被收回。如果进程已经结束，那么该函数会立即返回。
	返回值 ： 0代表成功。 失败，返回的则是错误号。
	*/
	pthread_join(thread, 0);
    return 0;
}
