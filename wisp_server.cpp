
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "debug.h"
#include "tinyxml.h"

#include "microhttpd.h"
#include "wisp_server.h"
#include "util.h"


#define NBSP(str)	(str) == NULL || (*str) == '\0' ? "&nbsp;" : (str)
#define BLANK(str)	(str) == NULL || (*str) == '\0' ? "" : (str)
#define OK "<html><head><title>upload</title></head><body>File upload</body></html>"
#define FNF "<html><head><title>File not found</title></head><body>File not found: %s</body></html>"
#define UNKNOWN "<html><head><title>Nothingness</title></head><body>There is nothing here. Sorry.</body></html>\n"
#define DEFAULT "<script type=\"text/javascript\"> document.location.href='/';</script>"
#define EMPTY "<html></html>"

typedef struct _conninfo {
	conntype_t conn_type;
	char const *conn_url;
	void *conn_arg1;
	void *conn_arg2;
	void *conn_arg3;
	void *conn_arg4;
	void *conn_res;
	struct MHD_PostProcessor *conn_pp;
} conninfo_t;


static Webserver *wserver = NULL;
unsigned short Webserver::port = 0;
bool Webserver::ready = false;
MyData *mydata = new MyData("abc");

bool done;
int debug;


/*
 * web_send_data
 * Send internal HTML string
 */
static int web_send_data (struct MHD_Connection *connection, const char *data,
		const int code, bool free, bool copy, const char *ct)
{
	struct MHD_Response *response;
	int ret;

	response = MHD_create_response_from_data(strlen(data), (void *)data, free ? MHD_YES : MHD_NO, copy ? MHD_YES : MHD_NO);
	if (response == NULL)
		return MHD_NO;
	if (ct != NULL)
		MHD_add_response_header(response, "Content-type", ct);
	ret = MHD_queue_response(connection, code, response);
	MHD_destroy_response(response);
	return ret;
}

/*
 * web_read_file
 * Read files and send them out
 */
ssize_t web_read_file (void *cls, uint64_t pos, char *buf, size_t max)
{
	FILE *file = (FILE *)cls;

	fseek(file, pos, SEEK_SET);
	return fread(buf, 1, max, file);
}

/*
 * web_close_file
 */
void web_close_file (void *cls)
{
	FILE *fp = (FILE *)cls;

	fclose(fp);
}

/*
 * web_send_file
 * Read files and send them out
 */
int web_send_file (struct MHD_Connection *conn, const char *filename, const int code, const bool unl)
{
	struct stat buf;
	FILE *fp;
	struct MHD_Response *response;
	const char *p;
	const char *ct = NULL;
	int ret;

	if ((p = strchr(filename, '.')) != NULL) {
		p++;
		if (strcmp(p, "xml") == 0)
			ct = "text/xml";
		else if (strcmp(p, "js") == 0)
			ct = "text/javascript";
	}
	if (stat(filename, &buf) == -1 ||
			((fp = fopen(filename, "r")) == NULL)) {
		if (strcmp(p, "xml") == 0)
			response = MHD_create_response_from_data(0, (void *)"", MHD_NO, MHD_NO);
		else {
			int len = strlen(FNF) + strlen(filename) - 1; // len(%s) + 1 for \0
			char *s = (char *)malloc(len);
			if (s == NULL) {
				dbg("sjil_webserver:Out of memory FNF\n");
				exit(1);
			}
			snprintf(s, len, FNF, filename);
			response = MHD_create_response_from_data(len, (void *)s, MHD_YES, MHD_NO); // free
		}
	} else
		response = MHD_create_response_from_callback(buf.st_size, 32 * 1024, &web_read_file, fp,
				&web_close_file);
	if (response == NULL)
		return MHD_YES;
	if (ct != NULL)
		MHD_add_response_header(response, "Content-type", ct);
	ret = MHD_queue_response(conn, code, response);
	MHD_destroy_response(response);
	if (unl)
		unlink(filename);
	return ret;
}

#if 1
 int Webserver::SendPollResponse(struct MHD_Connection *conn)
 {
    char fntemp[128];
    int fd;
    int ret;
    TiXmlDocument doc;


    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
    doc.LinkEndChild(decl);
    TiXmlElement* pollElement = new TiXmlElement("poll");
    doc.LinkEndChild(pollElement);
 
    pollElement->SetAttribute("mydata",mydata->get_data().c_str());
    
    strncpy(fntemp, "/data/misc/sms/ozwcp.poll.XXXXXX", sizeof(fntemp));
    fd = mkstemp(fntemp);
    if (fd == -1){
    		dbg("mkstemp fail");
        return MHD_YES;
    }
    close(fd);
    if(unlink(fntemp)){
    	  dbg("unlink fail");
        return MHD_YES;
    }    
    strncat(fntemp, ".xml", sizeof(fntemp));
    doc.SaveFile(fntemp);
    ret = web_send_file(conn, fntemp, MHD_HTTP_OK, true);
    dbg("send  tmp xml file ok\n");
    return ret;
 }
#endif


/*
 * web_config_post
 * Handle the post of the updated data
 */

int web_config_post (void *cls, enum MHD_ValueKind kind, const char *key, const char *filename,
		const char *content_type, const char *transfer_encoding, const char *data, uint64_t off, size_t size)
{
	conninfo_t *cp = (conninfo_t *)cls;

	dbg("func:%s, post: key=%s, filename=%s, content_type=%s, transfer_encoding=%s, off=%d, size=%d, datalen=%d, data=%s \n", __func__, key, filename,content_type, transfer_encoding,  off, (int)size, strlen(data), data );

	if ( 0 == strcmp(cp->conn_url, "/index") ) {
        if( 0 == strcmp(key,"upload_file") )
        {
            char local_filename[128];
            int namelen = strlen(filename);
            FILE *f = NULL;
            // fix
            if( 0 == namelen )
                ;

            strcpy(local_filename,"upload/");
            strcat(local_filename,filename);
            // 文件类型
           //if( 0 == strcmp(transfer_encoding,"test/plain") ){
           //     strcat(local_filename,".txt");
           //}

            dbg("filename:%s\n",local_filename);

            // 文件是否存在
            //if( !access(local_filename,F_OK) ){
            //    dbg("sjil_webserver:uploas file's name exitsts\n");     
            //}
            //else{
                f = fopen(local_filename,"a+");
                fseek(f,off,SEEK_SET);
                //fseek(f,-4,SEEK_END);
                fwrite(data,1,size,f);
                fclose(f);
            //}
        }
    }
    else if( 0 == strcmp(cp->conn_url, "/startapp") ) {
        cp->conn_arg1 = (void *)strdup(data);
    }
    else if( 0 == strcmp(cp->conn_url, "/poll") ) {
        cp->conn_arg1 = (void *)strdup(data);
    }

    /***
	if (strcmp(cp->conn_url, "/devpost.html") == 0) {
		if (strcmp(key, "fn") == 0)
			cp->conn_arg1 = (void *)strdup(data);
		else if (strcmp(key, "dev") == 0)
			cp->conn_arg2 = (void *)strdup(data);
		else if (strcmp(key, "usb") == 0)
			cp->conn_arg3 = (void *)strdup(data);
	}
    ***/
	return MHD_YES;
}



/*
 * Process web requests
 */
int Webserver::HandlerEP (void *cls, struct MHD_Connection *conn, const char *url,
		const char *method, const char *version, const char *up_data,
		size_t *up_data_size, void **ptr)
{
	Webserver *ws = (Webserver *)cls;

	return ws->Handler(conn, url, method, version, up_data, up_data_size, ptr);
}


//code here
static int
handle_key (void *cls, enum MHD_ValueKind kind, const char *key,
               const char *value)
{
  dbg("%s : %s\n", key, value);

  return MHD_YES;
}


int Webserver::Handler (struct MHD_Connection *conn, const char *url,
		const char *method, const char *version, const char *up_data,
		size_t *up_data_size, void **ptr)
{
	int ret;
	conninfo_t *cp;
     


	if (debug)
		dbg("%x: %s: \"%s\" conn=%x size=%d *ptr=%x data=%s\n", pthread_self(), method, url, conn, *up_data_size, *ptr,up_data);

    if (*ptr == NULL) {	/* do never respond on first call */
			cp = (conninfo_t *)malloc(sizeof(conninfo_t));
		if (cp == NULL)
			return MHD_NO;
		cp->conn_url = url;
		cp->conn_arg1 = NULL;
		cp->conn_arg2 = NULL;
		cp->conn_arg3 = NULL;
		cp->conn_arg4 = NULL;
		cp->conn_res = NULL;		

		if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
			cp->conn_pp = MHD_create_post_processor(conn, 1024, web_config_post, (void *)cp);
        dbg("cp->conn_pp is %x\n",cp->conn_pp);
			if (cp->conn_pp == NULL) {
				free(cp);
				return MHD_NO;
			}
			cp->conn_type = CON_POST;
		} else if (strcmp(method, MHD_HTTP_METHOD_GET) == 0) {
			cp->conn_type = CON_GET;
		} else {
			free(cp);
			return MHD_NO;
		}
		*ptr = (void *)cp;
		return MHD_YES;
	}
	if (strcmp(method, MHD_HTTP_METHOD_GET) == 0) {
        //const char* body = MHD_get_connection_values  (conn, MHD_GET_ARGUMENT_KIND, NULL); 

//        dbg("----------Get url:%s\n",url);
        if (strcmp(url, "/") == 0){

			dbg("----------goto url %s \n",url);
			// MHD_get_connection_values (conn, MHD_GET_ARGUMENT_KIND, handle_key,NULL);
			const char *shell_cmd;
			const char *shell_key="shell";
			shell_cmd=MHD_lookup_connection_value(conn, MHD_GET_ARGUMENT_KIND,shell_key);
			dbg("shell cmd is %s \n",shell_cmd);
        	  char *test_data="OK";
        	//code here
        	char *shell_ret=(char*)malloc(102400);
            if(shell_ret == NULL){
                dbg("canot malloc in wisp\n");
            }
        	if(_System(shell_cmd,shell_ret,102400)<0){
                sprintf(shell_ret,"shell exec error");
            }
            ret=web_send_data(conn,shell_ret,MHD_HTTP_OK,true,true,NULL);
            if(shell_ret != NULL){
                dbg("free shell_ret\n");
                free(shell_ret);
            }
            shell_ret = NULL;
        }

		return ret;
	}else if (strcmp(method, MHD_HTTP_METHOD_POST) == 0) {
        dbg("----------Post url:%s \n",url);
		cp = (conninfo_t *)*ptr;
		ret = web_send_data(conn, UNKNOWN, MHD_HTTP_NOT_FOUND, false, false, NULL); // no free, no copy
		return ret;
	}else
    return MHD_NO;

    
}

/*
 * web_free
 * Free up any allocated data after connection closed
 */

void Webserver::FreeEP (void *cls, struct MHD_Connection *conn,
		void **ptr, enum MHD_RequestTerminationCode code)
{
	Webserver *ws = (Webserver *)cls;

	ws->Free(conn, ptr, code);
}

void Webserver::Free (struct MHD_Connection *conn, void **ptr, enum MHD_RequestTerminationCode code)
{
	conninfo_t *cp = (conninfo_t *)*ptr;

	if (cp != NULL) {
		if (cp->conn_arg1 != NULL)
			free(cp->conn_arg1);
		if (cp->conn_arg2 != NULL)
			free(cp->conn_arg2);
		if (cp->conn_arg3 != NULL)
			free(cp->conn_arg3);
		if (cp->conn_arg4 != NULL)
			free(cp->conn_arg4);
		if (cp->conn_type == CON_POST) {
			MHD_destroy_post_processor(cp->conn_pp);
		}
		free(cp);
		*ptr = NULL;
	}
}

bool Webserver::exitserver()
{
	done = true;						 // let main exit
	return true;
}


/*
 * Constructor
 * Start up the web server
 */

Webserver::Webserver (int const wport) : sortcol(COL_NODE), logbytes(0), adminstate(false)
{
	dbg("webserver starting port %d\n", wport);
	port = wport;
	wdata = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG, port,
			NULL, NULL, &Webserver::HandlerEP, this,
			MHD_OPTION_NOTIFY_COMPLETED, Webserver::FreeEP, this, MHD_OPTION_END);

	if (wdata != NULL) {
		ready = true;
	}
}

/*
 * Destructor
 * Stop the web server
 */

Webserver::~Webserver ()
{
	if (wdata != NULL) {
		MHD_stop_daemon((MHD_Daemon *)wdata);
		wdata = NULL;
		ready = false;
	}
}

//-----------------------------------------------------------------------------
// <main>
// Create the driver and then wait
//-----------------------------------------------------------------------------
int main()
{
  wserver = new Webserver(8888);
  while (!wserver->isReady()) {
    delete wserver;
    sleep(2);
    wserver = new Webserver(8888);
  }

  while (!done) {	// now wait until we are done
    sleep(1);
  }

  delete wserver;
  exit(0);
}
