/*

    COPYRIGHT AND PERMISSION NOTICE
    Copyright (c) 2015 miVersa Inc
    All rights reserved.
    Permission to use, copy, modify, and distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
    USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall
    not be used in advertising or otherwise to promote the sale, use or other
    dealings in this Software without prior written authorization of the
    copyright holder.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include "ov_jsmn.h"
#include "ov_api.h"
#include <curl/curl.h>

#define NO_OF_TOKENS 1000

#define OV_MAGIC_NO 0x20150302
typedef struct ov {
	unsigned long magic_no;             //sanity check
	const char *app_id;
	const char *app_secret;
	const char *username;
	const char *url;
	char *get_params;
	char get_params_buf[512];
} ov_t;

#define OV_MSG_MAGIC_NO 0x19980824
typedef struct ov_msg {
	unsigned long magic_no;             //sanity check
	struct curl_slist *headers;
	struct curl_httppost *formpost;
	struct curl_httppost *lastptr;
	CURL *curl;
	void (*json_cleanup)(void*);
	void *data;
	ov_t *ov;
} ov_msg_t;

char *jsmn_get_string(jsmntok_t*, char*, const char*);
int jsmn_get_int(jsmntok_t*, char*, const char*);

/* Global Initialization */
ov_handle_t *ov_init(const char *app_id, const char *app_secret, const char *user_name, const char *url)
{
	ov_t *ov = (ov_t*) calloc(1, sizeof(ov_t));

	if (ov) {
		ov->magic_no = OV_MAGIC_NO;
		ov->app_id = app_id;
		ov->app_secret = app_secret;
		ov->username = user_name;
		ov->url = url;
		curl_global_init(CURL_GLOBAL_ALL);
	}
	return (ov_handle_t*) ov;
}

/* Global Shutdown */
void ov_shutdown(ov_handle_t *ov_handle)
{
	ov_t *ov = (ov_t*) ov_handle;
	assert(ov->magic_no == OV_MAGIC_NO);

	curl_global_cleanup();
	ov->magic_no = 0;
	free(ov);
}

/* CURL Add in a string parameter */
int ov_curl_set_string(void *ov_msg_handle, const char *name, char *val)
{
	ov_msg_t *ov_msg = (ov_msg_t*) ov_msg_handle;
	assert(ov_msg->magic_no == OV_MSG_MAGIC_NO);

	printf("SET MULTIPART ENTRY: NAME = '%s' VAL = '%s'\n", name, val);

	curl_formadd(&ov_msg->formpost, &ov_msg->lastptr, CURLFORM_COPYNAME, name, CURLFORM_COPYCONTENTS, val, CURLFORM_END);
	return 0;
}

/* CURL Add in an integer parameter */
int ov_curl_set_int(void *ov_msg_handle, const char *name, int val)
{
	char int_str[32];
	snprintf(int_str, sizeof(int_str), "%d", val);
	return ov_curl_set_string(ov_msg_handle, name, int_str);
}

/* CURL Read data from a file */
int ov_curl_set_filedata(void *ov_msg_handle, const char *name, char *path)
{
	ov_msg_t *ov_msg = (ov_msg_t*) ov_msg_handle;
	assert(ov_msg->magic_no == OV_MSG_MAGIC_NO);

	curl_formadd(&ov_msg->formpost, &ov_msg->lastptr, CURLFORM_COPYNAME, name, CURLFORM_FILE, path, CURLFORM_END);
	return 0;
}

/* CURL file download */
static size_t ov_download_write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int ov_curl_download(char *local_path, char *url)
{
    CURL *curl = curl_easy_init();
    CURLcode res;

    if (curl) {
        FILE *fp = fopen(local_path,"wb");
        if (fp) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ov_download_write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);            /* always cleanup */
            fclose(fp);
            return res;
        }
        /* couldn't open file for writing */
        curl_easy_cleanup(curl);
    }
    return 1;
}

/* CURL Message Initialization */
void *ov_curl_msg_init(ov_handle_t *ov_handle, const char *command, void (*json_cleanup)(void*), void *data)
{
	ov_t *ov = (ov_t*) ov_handle;
	ov_msg_t *ov_msg = (ov_msg_t*) calloc(1, sizeof(ov_msg_t));

	assert(ov->magic_no == OV_MAGIC_NO);

	if (ov_msg) {
		struct curl_slist *headers = NULL;
		CURL *curl = curl_easy_init();

		ov_msg->magic_no = OV_MSG_MAGIC_NO;

		if (!curl) {
			free(ov_msg);
			return NULL;
		}
#if NOT_YET /* Not required yet, do we use appID/secret or just secret? */
		/* Set up Authentication */

		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD, ov->app_secret);
#endif // NOT_YET

		/* add in the headers */
		headers = curl_slist_append(headers, "Accept: */*");
		headers = curl_slist_append(headers, "Expect: 100-continue");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		ov_msg->headers = headers;

        ov_curl_set_string(ov_msg, "appID", (char*) ov->app_id);
        ov_curl_set_string(ov_msg, "appSecret", (char*) ov->app_secret);
        ov_curl_set_string(ov_msg, "command", (char*) command);
        ov_curl_set_string(ov_msg, "username", (char*) ov->username);
        curl_easy_setopt(curl, CURLOPT_URL, ov->url);

		/* set up other common stuff */
		ov->get_params = NULL;
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
//		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		/* fill in common form fields */
		/* store curl handle */
		ov_msg->curl = curl;
		ov_msg->json_cleanup = json_cleanup;
		ov_msg->data = data;
		ov_msg->ov = ov;
	}
	return (void*) ov_msg;
}

void ov_curl_msg_cleanup(ov_handle_t *ov_handle)
{
	ov_msg_t *ov_msg = (ov_msg_t*) ov_handle;

	assert(ov_msg->magic_no == OV_MSG_MAGIC_NO);

	if (ov_msg->json_cleanup) {
		ov_msg->json_cleanup(ov_msg->data);
	}
	ov_msg->magic_no = 0;
	free(ov_msg);
}

typedef struct curl_string {
	char *ptr;
	size_t len;
} curl_string_t;

static void ov_curl_string_init(curl_string_t *s)
{
	s->len = 0;
	s->ptr = malloc(s->len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

/* CURL Write Callback */
static size_t ov_curl_callback(char *ptr, size_t size, size_t nmemb, void *user_data)
{
	curl_string_t *s = (curl_string_t*)user_data;
	size_t new_len = s->len + size * nmemb;
	//printf("ov_url_callback\n");
	s->ptr = realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr + s->len, ptr, size * nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size * nmemb;
}

/* CURL Make the Request */
static int ov_curl(void *ov_msg_handle, int (*parse_result)(void*, char*, void*), void *parse_data)
{
	ov_msg_t *ov_msg = (ov_msg_t*) ov_msg_handle;
	CURL *curl = ov_msg->curl;
	CURLcode res = CURLE_OK;
	curl_string_t s;

	assert(ov_msg->magic_no == OV_MSG_MAGIC_NO);

	ov_curl_string_init(&s);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, ov_msg->formpost);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ov_curl_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &s);

	/* Perform the request, res will get the return code */
	switch ((res = curl_easy_perform(curl))) {
	case CURLE_OK:
		if (parse_result) {
			parse_result(ov_msg_handle, s.ptr, parse_data);
		}
		free(s.ptr);
		break;
	default:
		fprintf(stderr, "curl_easy_perform() failed <%d>: %s\n", res, curl_easy_strerror(res));
		res = 1;
	}
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return res;
}

typedef struct ov_json_data {
	void (*json_cb)(void*, jsmntok_t*, char*, void*);
	void *json_data;
} ov_json_data_t;

static int ov_json_parse(void *ov_msg_handle, char *data, void *jd_cookie)
{
	ov_json_data_t *jd = jd_cookie;
	jsmn_parser parser;
	jsmnerr_t rc;
	jsmntok_t tokens[NO_OF_TOKENS] = {};

	if (!jd->json_cb)
	 {
		printf("No JSON Callback - Unparsed data:\n\n%s\n", data);
		return 0;
	}

    printf("\nRaw Unparsed data:\n\n%s\n", data);

	jsmn_init(&parser);
	rc = jsmn_parse(&parser, data, tokens, NO_OF_TOKENS);

	if (rc == 0) {
		jsmntok_t *t = tokens;
		/* null-terminate text to make printing easier */
		while (t->end) {
			if (t->end > 0) {
				data[t->end] = '\0';
			}
			t++;
		}
		if (jd->json_cb) {
			jd->json_cb(ov_msg_handle, tokens, data, jd->json_data);
		}
	}
	return rc;
}

int ov_curl_json(void *ov_msg_handle, void (*ov_json_parse_cb)(void*, jsmntok_t *, char*, void*), void *data)
{
	ov_json_data_t jd = {
		.json_data = data,
		.json_cb = ov_json_parse_cb
	};
	return ov_curl(ov_msg_handle, ov_json_parse, &jd);
}
