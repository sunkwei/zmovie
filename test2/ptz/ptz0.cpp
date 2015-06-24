#include <stdio.h>
#include <stdlib.h>
#include <cc++/thread.h>
#include <cc++/url.h>
#include <map>
#include <string>
#include <vector>
#include "../kvconfig2.h"
#include "ptz0.h"
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#include "../cJSON.h"
#include <assert.h>
#include "../utils.h"
#include <cc++/socket.h>
#ifdef WIN32
#   include <process.h>
#else
#   include <unistd.h>
#   include <errno.h>
#   include <fcntl.h>
#   define closesocket close
#   define WSAGetLastError() errno
#   define WSAEWOULDBLOCK EAGAIN
#endif //
namespace {
	struct Ptz
	{
		char *serial_name;	// 
		int addr;

		char *wj_url;	// 链接老刘的云天服务.
		char *tcp_url;	// 链接d3100的arm
		char who[64];	// 用于 tcp

		ost::URLStream us;	// 用于 wj_url ...

		PtzParams params;

		Ptz()
		{
			serial_name = 0;
			addr = 0;

			wj_url = 0;
			tcp_url = 0;

			params.f = -1.0;
		}
	};
}

// ---------------- webservice + json 模式 ---------------------
static int wj_req(ost::URLStream &s, const char *url)
{
	TimeUsed tu(url);

	ost::URLStream::Error err = s.get(url);
	if (err)
		return err;

	std::stringstream ss;

	while (!s.eof()) {
		std::string x;
		s >> x;
		ss << x;
	}

	if (tu.is_timeout()) {
		fprintf(stderr, "\t:%s\n", ss.str().c_str());
	}

	// 根据 { "result":"ok",  } 进行判断.
	util_TypedObject to = util_build_object_from_json_string(ss.str().c_str());
	if (to.type == 4 && to.data.ov["result"].type == 2 && to.data.ov["result"].data.sv == "ok") {
		return 0;
	}
	else {
		fprintf(stderr, "<libptz> ERROR: 云台失败:'%s', str=%s\n", url, ss.str().c_str());
		return -1;
	}
}

static int wj_req_with_res(ost::URLStream &s, const char *url, std::ostream &os)
{
	TimeUsed tu(url);

	ost::URLStream::Error err = s.get(url);
	if (err)
		return err;

	std::stringstream ss;
	while (!s.eof()) {
		std::string x;
		s >> x;
		os << x;
		ss << x;
	}

	if (tu.is_timeout()) {
		fprintf(stderr, "\t:%s\n", ss.str().c_str());
	}

	// 根据 { "result":"ok",  } 进行判断.
	util_TypedObject to = util_build_object_from_json_string(ss.str().c_str());
	if (to.type == 4 && to.data.ov["result"].type == 2 && to.data.ov["result"].data.sv == "ok") {
		return 0;
	}
	else {
		fprintf(stderr, "<libptz> ERROR: 云台失败:'%s', str=%s\n", url, ss.str().c_str());
		return -1;
	}
}

// -------------- 构造更方便些的 json 解析树.
struct TypedObject
{
	int type;	// 0: bv
	// 1: nv
	// 2: sv
	// 3: av
	// 4: ob
	// 5: null
	struct {
		bool bv;	// True, False
		double nv;	// number
		std::string sv;	// string
		std::vector<TypedObject> av;	// array
		std::map<std::string, TypedObject> ov; // object
	} data;
};

typedef std::map<std::string, TypedObject> JSONDATA;

static void build_array(const cJSON *json, std::vector<TypedObject> &array);
static void build_object(const cJSON *json, std::map<std::string, TypedObject> &obj);

static std::string build_one(const cJSON *json, TypedObject &to)
{
	switch (json->type) {
	case 0:	// cJSON_False
	case 1:	// cJSON_True
		to.type = 0;
		to.data.bv = (json->type == cJSON_True);
		break;

	case 2: // cJSON_NULL
		to.type = 5;
		break;

	case 3: // cJSON_Number
		to.type = 1;
		to.data.nv = json->valuedouble;
		break;

	case 4: // cJSON_String
		to.type = 2;
		to.data.sv = json->valuestring;
		break;

	case 5: // cJSON_array
		to.type = 3;
		build_array(json->child, to.data.av);
		break;

	case 6: // cJSON_object
		to.type = 4;
		build_object(json->child, to.data.ov);
		break;
	}

	if (json->string)
		return json->string;
	else
		return "";	// 无名对象 ..
}

static void build_array(const cJSON *json, std::vector<TypedObject> &array)
{
	while (json) {
		TypedObject to;
		build_one(json, to);
		array.push_back(to);
		json = json->next;
	}
}

static void build_object(const cJSON *json, std::map<std::string, TypedObject> &objs)
{
	while (json) {
		TypedObject to;
		std::string name = build_one(json, to);
		objs[name] = to;
		json = json->next;
	}
}

static int connect_t(int sock, const sockaddr *addr, int len, int timeout = 3000)
{
	fd_set w;
	FD_ZERO(&w);
	FD_SET(sock, &w);

	int rc = connect(sock, addr, len);
	if (rc == 0)  {
		return 0;
	}
	else {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			timeval tv;
			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout % 1000) * 1000;
			rc = select(sock+1, 0, &w, 0, &tv);
			if (rc == -1) {
				fprintf(stderr, "[libptz] ERR: select fault ???\n");
				return -1;
			}
			if (rc == 0) {
				return -2;
			}
			if (FD_ISSET(sock, &w)) {
				return 0;
			}
		}
		else {
			return -1;
		}
	}
}

static bool SetSocketBlockingEnabled(int fd, bool blocking)
{
   if (fd < 0) return false;

#ifdef WIN32
   unsigned long mode = blocking ? 0 : 1;
   return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags < 0) return false;
   flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
}

/** 对 url 发出 cmd，接收保存到 res 中.

	 FIXME: 这种接收是有问题的，因为不知道何时接受完成 ...
 */
static int tcp_req(const char *url, const char *cmd, int cmdlen, char *res, size_t res_buf_len, int timeout)
{
	char host[128], who[64];
	int port;

	if (sscanf(url, "tcp://%[^:]:%d/%s", host, &port, who) != 3) {
		fprintf(stderr, "[libptz] ERR: invalid tcp url format %s\n", url);
		fprintf(stderr, "\tusing: tcp://<host>:<port>/<who>\n\n");
		return -1;
	}

    struct addrinfo ai, *info, *p;
	memset(&ai, 0, sizeof(ai));
	ai.ai_family = AF_INET;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_protocol = IPPROTO_TCP;

	char service[16];
	snprintf(service, sizeof(service), "%d", port);
	if (getaddrinfo(host, service, &ai, &info) != 0) {
		fprintf(stderr, "[libptz] ERR: getaddrinfo err for %s\n", url);
		return -2;
	}

	bool completed = false;
	char *recvbuf = res;
	int data_len = 0;

	for (p = info; p && !completed; p = p->ai_next) {
		int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock < 0) {
			fprintf(stderr, "[libptz] ERR: can't create socket ???\n");
			continue;
		}

        SetSocketBlockingEnabled(sock, false);

		if (connect_t(sock, p->ai_addr, p->ai_addrlen, timeout) < 0) {
			fprintf(stderr, "[libptz] ERR: can't connect to %s\n", url);
			closesocket(sock);
			continue;
		}

		send(sock, cmd, cmdlen, 0); // FIXME: 这一句基本不会出错了 :)

		fd_set r;
		FD_ZERO(&r);
		FD_SET(sock, &r);
		timeval tv = { timeout/1000, timeout % 1000 * 1000 };
		int rc = select(sock+1, &r, 0, 0, &tv);
		if (rc == -1) {
			fprintf(stderr, "[libptz] ERR: select falut??\n");
			closesocket(sock);
			continue;
		}
		else if (rc == 0) {
			fprintf(stderr, "[libptz] ERR: recv timeout\n");
			closesocket(sock);
			continue;
		}
		
		rc = recv(sock, recvbuf, res_buf_len, 0);
		if (rc > 0) {
			completed = true;
			data_len = rc;
			closesocket(sock);
		}
		else if (rc == 0) {
			fprintf(stderr, "[libptz] ERR: peer closed???\n");
			closesocket(sock);
			continue;
		}
		else {
			// FIXME: 应该检查 lasterr ...
			fprintf(stderr, "[libptz] ERR: recv fault!\n");
			closesocket(sock);
			continue;
		}
	}

	freeaddrinfo(info);

	return data_len;
}

static int tcp_req_without_res(const char *url, const char *cmd)
{
	char res[128];
	int rc;
	if ((rc = tcp_req(url, cmd, strlen(cmd), res, sizeof(res)-1, 2000)) < 0) {
		fprintf(stderr, "[libptz] ERR: tcp req err of %s\n", url);
		return -100;
	}

	res[rc] = 0;

	if (strstr(res, "error")) {
		return -1;
	}
	else {
		return 0;
	}
}

static int tcp_get_pos(const char *url, const char *who, int &x, int &y)
{
	/*
		req: PtzCmd=GetPos&Who=teacher
		res: ###X=-378&Y=27
	 */

	char res[128], cmd[64];
	int rc;
	
	snprintf(cmd, sizeof(cmd), "PtzCmd=GetPos&Who=%s", who);
	if ((rc = tcp_req(url, cmd, strlen(cmd), res, sizeof(res)-1, 2000)) < 0) {
		fprintf(stderr, "[libptz] ERR: tcp req err of %s\n", url);
		return -100;
	}

	res[rc] = 0;

	if (strstr(res, "error")) {
		return -1;
	}
	else {
		if (sscanf(res, "###X=%d&Y=%d", &x, &y) == 2) {
			return 0;
		}
		else {
			fprintf(stderr, "[libptz] ERR: get_pos ret '%s' with invalid format\n", res);
			return -2;
		}
	}
}

static int tcp_get_zoom(const char *url, const char *who, int &z)
{
	/**
		req: PtzCmd=GetZoom&Who=teacher
		res: ###Zoom=0
	 */

	char res[128], cmd[64];
	int rc;
	
	snprintf(cmd, sizeof(cmd), "PtzCmd=GetZoom&Who=%s", who);
	if ((rc = tcp_req(url, cmd, strlen(cmd), res, sizeof(res)-1, 2000)) < 0) {
		fprintf(stderr, "[libptz] ERR: tcp req err of %s\n", url);
		return -100;
	}

	res[rc] = 0;

	if (strstr(res, "error")) {
		return -1;
	}
	else {
		if (sscanf(res, "###Zoom=%d", &z) == 1) {
			return 0;
		}
		else {
			fprintf(stderr, "[libptz] ERR: get_zoom ret '%s' with invalid format\n", res);
			return -2;
		}
	}
}

ptz_t *ptz_open(const char *name, int addr)
{
	if (strlen(name) > 7 && strncmp("http://", name, 7) == 0) {
		// json 格式 ...
		Ptz *ptz = new Ptz;
        ptz->wj_url = ::strdup(name);
		return (ptz_t*)ptz;
	}
	if (strlen(name) > 6 && strncmp("tcp://", name, 6) == 0) {
		// d3100 模式.
		Ptz *ptz = new Ptz;
        ptz->tcp_url = ::strdup(name);
		char host[128];
		int port;
		if (sscanf(name, "tcp://%[^:]:%d/%s", host, &port, ptz->who) != 3) {
			log("ERR: tcp schema using: tcp://<host>:<port>/<who>\n");
			return 0;
		}
		return (ptz_t*)ptz;
	}
    else {
        log("ERR: only support http/tcp schema!!!!\n");
        return 0;
	}

	return 0;
}

void ptz_close(ptz_t *p)
{
	if (!p) return;
	Ptz *ptz = (Ptz*)p;

	if (ptz->wj_url) {
		free(ptz->wj_url);
		delete ptz;
	}
	else if (ptz->tcp_url) {
		free(ptz->tcp_url);
		delete ptz;
	}
}

int ptz_reset(ptz_t *p)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	ptz_zoom_stop(p);
	ptz_stop(p);
	ptz_set_absolute_position(p, 0, 0, 30);
	ptz_zoom_set(p, 0);
	return 0;
}

int ptz_reset2(ptz_t *p, int h, int v, int move_speed, int zoom_speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	ptz_zoom_stop(p);
	ptz_stop(p);
	ptz_zoom_set(p, 0);
	ptz_set_absolute_position(p, h, v, move_speed);
	return 0;
}

int ptz_left(ptz_t *p, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/left?speed=%d", ptz->wj_url, speed);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=Turn&Who=%s&Direction=left&Speed=%d", ptz->who, speed);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
	}
}

int ptz_right(ptz_t *p, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/right?speed=%d", ptz->wj_url, speed);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=Turn&Who=%s&Direction=right&Speed=%d", ptz->who, speed);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_up(ptz_t *p, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/up?speed=%d\n", ptz->wj_url, speed);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=Turn&Who=%s&Direction=up&Speed=%d", ptz->who, speed);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_down(ptz_t *p, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/down?speed=%d\n", ptz->wj_url, speed);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=Turn&Who=%s&Direction=down&Speed=%d", ptz->who, speed);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_upleft(ptz_t *p, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		return -1;
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_upright(ptz_t *p, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		return -1;
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_downleft(ptz_t *p, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		return -1;
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_downright(ptz_t *p, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		return -1;
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_stop(ptz_t *p)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/stop", ptz->wj_url);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=StopTurn&Who=%s", ptz->who);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_preset_call(ptz_t *p, int idx)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/preset_call?id=%d", ptz->wj_url, idx);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=PresetCall&Who=%s&ID=%d", ptz->who, idx);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
		return -1;
	}
}

int ptz_preset_save(ptz_t *p, int idx)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/preset_save?id=%d", ptz->wj_url, idx);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=PresetSave&Who=%s&ID=%d", ptz->who, idx);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
		return -1;
	}
}

int ptz_preset_clr(ptz_t *p, int idx)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/preset_clear?id=%d", ptz->wj_url, idx);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=PresetDel&Who=%s&ID=%d", ptz->who, idx);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
		return -1;
	}
}

int ptz_zoom_get(ptz_t *p)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		std::stringstream ss;
		char buf[128];
		snprintf(buf, sizeof(buf), "%s/get_zoom", ptz->wj_url);
		if (wj_req_with_res(ptz->us, buf, ss) == 0) {
			/** 解析 json ...
					{
						"result": "ok",
						"info": "...",
						"value":
						{
							"type": "position",
							"data": 
							{
								"z": 0
							}
						}
					}
			 */
			cJSON *json = cJSON_Parse(ss.str().c_str());
			if (!json) return -1;

			TypedObject to;
			build_one(json, to);
			cJSON_Delete(json);

			assert(to.type == 4);

			if (to.data.ov["result"].data.sv == "ok") {
				return (int)(to.data.ov["value"].data.ov["data"].data.ov["z"].data.nv);
			}
			else {
				return -1;	// 
			}
		}
		else
			return -1;
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=GetZoom&Who=%s", ptz->who);
		int z;
		if (tcp_get_zoom(ptz->tcp_url, ptz->who, z) == 0) {
			return z;
		}
		else {
			return -1;
		}
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_get_current_position_and_zoom(ptz_t *p, int *h, int *v, int *z)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		std::stringstream ss;
		char buf[128];
		snprintf(buf, sizeof(buf), "%s/get_pos_zoom", ptz->wj_url);
		if (wj_req_with_res(ptz->us, buf, ss) == 0) {
			/** 
			{
				"result": "ok",
				"info": "...",
				"value":
				{
					"type": "position",
					"data": 
					{
						"x": x,
						"y": y,
						"z": 0
					}
				}
			}
			 */
			cJSON *json = cJSON_Parse(ss.str().c_str());
			if (!json) return -1;

			TypedObject to;
			build_one(json, to);
			cJSON_Delete(json);

			assert(to.type == 4);
			if (to.data.ov["result"].data.sv == "ok") {
				*h = (int)(to.data.ov["value"].data.ov["data"].data.ov["x"].data.nv);
				*v = (int)(to.data.ov["value"].data.ov["data"].data.ov["y"].data.nv);
				*z = (int)(to.data.ov["value"].data.ov["data"].data.ov["z"].data.nv);

				return 0;
			}
			else {
				return -1;
			}
		}
		else
			return -1;
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_zoom_set(ptz_t *p, int z)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/set_zoom?z=%d", ptz->wj_url, z);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=SetZoom&Who=%s&Zoom=%d", ptz->who, z);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_zoom_set_blocked(ptz_t *p, int z)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/set_zoom_blocked?z=%d", ptz->wj_url, z);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=SetZoom&Who=%s&Zoom=%d", ptz->who, z);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_zoom_set2(ptz_t *p, int z)
{
	return ptz_zoom_set(p, z);
}

int ptz_zoom_tele(ptz_t *p, int s)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/zoom_tele?speed=%d", ptz->wj_url, s);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=ZoomTele&Who=%s&Speed=%d", ptz->who, s);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_zoom_wide(ptz_t *p, int s)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/zoom_wide?speed=%d", ptz->wj_url, s);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=ZoomWide&Who=%s&Speed=%d", ptz->who, s);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_zoom_stop(ptz_t *p)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/zoom_stop", ptz->wj_url);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=ZoomStop&Who=%s", ptz->who);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_get_current_position(ptz_t *p, int *h, int *v)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		std::stringstream ss;
		snprintf(url, sizeof(url), "%s/get_pos", ptz->wj_url);
		if (wj_req_with_res(ptz->us, url, ss) == 0) {
			/** 解析 json ...
					{
						"result": "ok",
						"info": "...",
						"value":
						{
							"type": "position",
							"data": 
							{
								"x": 0,
								"y": 0
							}
						}
					}
			 */
			cJSON *json = cJSON_Parse(ss.str().c_str());
			if (!json) return -1;

			TypedObject to;
			build_one(json, to);
			cJSON_Delete(json);

			assert(to.type == 4);
			if (to.data.ov["result"].data.sv == "ok") {
				*h = (int)(to.data.ov["value"].data.ov["data"].data.ov["x"].data.nv);
				*v = (int)(to.data.ov["value"].data.ov["data"].data.ov["y"].data.nv);
				return 0;
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=GetPos&Who=%s", ptz->who);
		if (tcp_get_pos(ptz->tcp_url, ptz->who, *h, *v) < 0) {
			return -1;
		}
		else {
			return 0;
		}
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_set_relative_position(ptz_t *p, int h, int v, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		return -1;
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_set_absolute_position(ptz_t *p, int h, int v, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/set_pos?x=%d&y=%d&sx=%d&sy=%d", ptz->wj_url, h, v, speed, speed);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=TurnToPos&Who=%s&X=%d&Y=%d&SX=%d&SY=%d", ptz->who, h, v, speed, speed);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

int ptz_set_absolute_position_blocked(ptz_t *p, int h, int v, int speed)
{
	if (!p) return -1;
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		char url[128];
		snprintf(url, sizeof(url), "%s/set_pos_blocked?x=%d&y=%d&sx=%d&sy=%d", ptz->wj_url, h, v, speed, speed);
		return wj_req(ptz->us, url);
	}
	else if (ptz->tcp_url) {
		char cmd[64];
		snprintf(cmd, sizeof(cmd), "PtzCmd=TurnToPos&Who=%s&X=%d&Y=%d&SX=%d&SY=%d", ptz->who, h, v, speed, speed);
		return tcp_req_without_res(ptz->tcp_url, cmd);
	}
	else {
        assert(0);
        return -1;
    }
}

/** 使用 octive 拟合 sony d70 云台，得到的多项式.
	v =
       0    5638    8529   10336   11445   12384   13011   13637   14119   14505   14914   15179   15493   15733   15950   16119   16288   16384
	z = 
	   1 	 2      3		4		5		6		7		8		9		10		11		12		13		14		15		16		17		18
 */
 double ptz_zoom_ratio_of_value(double v)
{
	double zx = v/10000.0;
	double focal = 58.682*zx*zx*zx*zx*zx*zx-257.08*zx*zx*zx*zx*zx
		+445.88*zx*zx*zx*zx-369.8*zx*zx*zx+150.84*zx*zx-18.239*zx+4.2017;
	return focal/4.2017;
// 	return 1.5516e-15 * pow(v, 4) - 4.2558e-11 * pow(v, 3) +
// 		4.0745e-7 * pow(v, 2) - 1.0751e-3 * pow(v, 1) + 1.013;
#if 0
	return 1.0389e-29 * pow(v, 8) - 8.4558e-25 * pow(v, 7) + 2.9101e-20 * pow(v, 6) -
		5.4805e-16 * pow(v, 5) + 6.0893e-12 * pow(v, 4) - 3.9826e-8 * pow(v, 3) +
		1.4161e-4 * pow(v, 2) - 2.1026e-1 * v + 1.0;
#endif
}

#define M_PI       3.14159265358979323846
#define F (4.2017)

/** 此款云台，1x 倍率时，视角为 63°，焦距为 4.7mm，因此可以推断ccd半宽为:
		ccd_width/2 = tan(63/2*Pi/180) * 4.7 = 2.88mm
		
		相应的半高为:
		ccd_height/2 = ccd_width/2 * 9 / 16 = 1.62mm

		倍率可以认为与 f 成反比关系.
		
		则计算视角:
			h = 2 * atan(ccd_width/2 / f);
			v = 2 * atan(ccd_height/2 / f);
 */

int ptz_get_view_angle(ptz_t *p, int *h, int *v)
{
	Ptz *ptz = (Ptz*)p;
	int zv = ptz_zoom_get(p);
	double zr = ptz_zoom_ratio_of_value(zv);
	double f = zr * F;

	double hh = atan(2.88 / f) * 180 / M_PI * 2.0;
	double vv = atan(1.62 / f) * 180 / M_PI * 2.0;

	*h = hh;
	*v = vv;

	return 0;
}

int ptz_ext_location_center(ptz_t *p, double hr, double vr, int speed, AnglesF *af)
{
	Ptz *ptz = (Ptz*)p;
	int zv = ptz_zoom_get(p);
		
	double zr = ptz_zoom_ratio_of_value(zv);
	double f = zr * F;
	if (af)
		af->f = f;

	//fprintf(stderr, "hr=%.3f, vr=%.3f, zr=%.3f(%d), f=%.3f\n", hr, vr, zr, zv, f);

	double hh = atan(2.4 * hr / f) ;
	if (af)
		af->als.u_angle = hh;
	double vv = atan(1.35 * vr /sqrt(2.4 * hr * 2.4 * hr + f *f)) ;
	if (af)
		af->als.v_angle = vv;


	//fprintf(stderr, "hh=%.3f, vv=%.3f\n", hh, vv);

// 		int h = hh / 0.075;
// 		int v = vv / 0.075;
	int h = (int)(hh>0 ? (hh*790+0.5) : (hh*790-0.5));
	int v = (int)(vv>0 ? (vv*955+0.5) : (vv*955-0.5));
	//fprintf(stderr, "h=%d, v=%d\n", h, v);

	int ret = ptz_set_relative_position(p, h, v, speed);
	return ret;
}

double ptz_get_f( ptz_t *p)
{
	int zv = ptz_zoom_get( p);
	double zr = ptz_zoom_ratio_of_value( zv);
	double f = zr * F;
	return f;
}

double ptz_get_picture_scale( ptz_t *p)
{
	int zv = ptz_zoom_get( p);
	return ptz_zoom_ratio_of_value( zv);
}

int ptz_ext_location_center_offset(ptz_t *p, double hr_src, double vr_src, double hr_dst, double vr_dst, int speed, TwoAnglesF *taf)
{
	Ptz *ptz = (Ptz*)p;
	int zv = ptz_zoom_get(p);
	double zr = ptz_zoom_ratio_of_value(zv);
	double f = zr * F;
	taf->f = f;

	double hh_s = atan(2.88 * hr_src / f) * 180 / M_PI;		double hh_d = atan(2.88 * hr_dst / f) * 180 / M_PI;
	double vv_s = atan(1.62 * vr_src / f) * 180 / M_PI;		double vv_d = atan(1.62 * vr_dst / f) * 180 / M_PI;

	taf->first_als.u_angle = hh_s;
	taf->first_als.v_angle = vv_s;
	taf->second_als.u_angle = hh_d;
	taf->second_als.v_angle = vv_d;

	int h_s = hh_s / 0.075;		int h_d = hh_d / 0.075;
	int v_s = vv_s / 0.075;		int v_d = vv_d / 0.075;

	return ptz_set_relative_position(p, h_s + h_d, v_s + v_d, speed);
}

double ptz_get_scales(ptz_t *p)
{
	Ptz *ptz = (Ptz*)p;
	if (ptz->wj_url) {
		// TODO: 
		return 1.0;
	}
	else {
		return ptz_get_picture_scale(p);
	}
}
