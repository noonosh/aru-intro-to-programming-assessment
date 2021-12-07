// This file is subject to copyright.
// All contents of this file were inspired by web-blog at ...


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>

struct curl_fetch_st {
	char *payload;
	size_t size;
};

/* callback for curl fetch */
size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;                             /* calculate buffer size */
	struct curl_fetch_st *p = (struct curl_fetch_st *) userp;   /* cast pointer to fetch struct */

	p->payload = (char *) realloc(p->payload, p->size + realsize + 1);
	if (p->payload == NULL) {
		fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
		free(p->payload);
		return -1;
	}

	/* copy contents to buffer */
	memcpy(&(p->payload[p->size]), contents, realsize);
	p->size += realsize;
	p->payload[p->size] = 0;
	return realsize;
}

/* fetch and return url body via curl */
CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch) {
	CURLcode rcode;                   /* curl result code */

	fetch->payload = (char *) calloc(1, sizeof(fetch->payload));
	if (fetch->payload == NULL) {
		fprintf(stderr, "ERROR: Failed to allocate payload in curl_fetch_url");
		return CURLE_FAILED_INIT;
	}

	fetch->size = 0;
	curl_easy_setopt(ch, CURLOPT_URL, url);
	curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);
	curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);
	curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(ch, CURLOPT_TIMEOUT, 5);
	curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

	/* fetch the url */
	rcode = curl_easy_perform(ch);
	return rcode;
}

struct curl_fetch_st curl_fetch;                        /* curl fetch struct */

char *curl_receive_reply_json_to_buffer(char *url, char *userpwd) {
	CURL *ch;                                               /* curl handle */
	CURLcode rcode;                                         /* curl result code */
	struct curl_fetch_st *cf = &curl_fetch;                 /* pointer to fetch struct */
	struct curl_slist *headers = NULL;                      /* http headers to send with request */

	/*TODO: when to free above malloced url*/

	/* init curl handle */
	if ((ch = curl_easy_init()) == NULL) {
		/* log error */
		fprintf(stderr, "ERROR: Failed to create curl handle in fetch_session");
		/* return error */
		return NULL;
	}

	/* set content type */
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");

	curl_easy_setopt(ch, CURLOPT_HTTPGET, 1L);

	if (userpwd != NULL) { 
		curl_easy_setopt(ch, CURLOPT_USERPWD, userpwd);
		curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 0);
	}
    
	/* fetch page and capture return code */
	rcode = curl_fetch_url(ch, url, cf);

	/* cleanup curl handle */
	curl_easy_cleanup(ch);

	/* free headers */
	curl_slist_free_all(headers);
	/* check return code */
	if (rcode != CURLE_OK || cf->size < 1) {
		/* log error */
		fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s",
			url, curl_easy_strerror(rcode));
		/* return error */
		return NULL; // TODO: how to handle "curl said: Timeout was reached"
	}
    
	/* check payload */
	if (cf->payload != NULL) {
		/* print result */
		return cf->payload;
	}

	/* error */
	fprintf(stderr, "ERROR: Failed to populate payload");
	/* free payload */
	free(cf->payload);
	/* return */
	return NULL;
}

char *curl_download_json_to_buffer(char *remote_url) {
	char *reply_json_received;
	int wait_count = 0;

	while(wait_count < 10) {
		reply_json_received = curl_receive_reply_json_to_buffer(remote_url, NULL);
		if (reply_json_received == NULL) {
			wait_count++;
			printf("failed for %d th time, retrying after 3 sec...\n", wait_count);
			sleep(5); //sleep for 5 sec and retry
			continue;
		} else
			break;
	}

	return reply_json_received;
}