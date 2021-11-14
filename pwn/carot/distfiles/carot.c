/*
    clang carot.c -Wl,-z,relro,-z,now -o carot -fno-stack-protector
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void http_send_reply_it_works() {
  printf("HTTP/1.0 200 OK\r\n");
  printf("Content-Type: text/html\r\n\r\n");
  printf("<html><head></head><body>It works!</body></html>\n");
}

void http_send_reply_bad_request() {
  printf("HTTP/1.0 400 Bad Request\r\n");
  printf("Content-Type: text/html\r\n\r\n");
  printf("<html><head></head><body>400 Bad Request</body></html>\n");
}

void http_send_reply_not_found() {
  printf("HTTP/1.0 404 Not Found\r\n");
  printf("Content-Type: text/html\r\n");
  printf("Connection: close\r\n\r\n");

  printf("<html>\n");
  printf("<head><title>404 Not Found</title></head>\n");
  printf("<body bgcolor=\"white\">\n");
  printf("<center><h1>404 Not Found</h1></center>\n");
  printf("</body>\n");
  printf("</html>\n");
}

char *lookup_content_type(char *ext) {
  if (strcasecmp(ext, "html") == 0) return "text/html";
  if (strcasecmp(ext, "txt") == 0) return "text/plain";
  if (strcasecmp(ext, "text") == 0) return "text/plain";
  if (strcasecmp(ext, "gif") == 0) return "image/gif";
  if (strcasecmp(ext, "jpeg") == 0) return "image/jpeg";
  if (strcasecmp(ext, "jpg") == 0) return "image/jpeg";
  if (strcasecmp(ext, "png") == 0) return "image/png";
  return NULL;
}

char gif[14] = {
  0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x00, 0x3b
};

void try_http_send_reply_with_file(char *fname) {
  char *dotp;
  char *content_type;

  if (*fname != '/') {
    http_send_reply_bad_request();
    return;
  }

  dotp = strrchr(fname, '.');
  if (dotp == NULL) { // No file extension
    http_send_reply_not_found();
    return;
  }

  content_type = lookup_content_type(dotp+1);
  if (content_type == NULL) {
    http_send_reply_not_found();
    return;
  }
 
  if (strcmp(fname, "/index.html") == 0) {
    http_send_reply_it_works();
    return;
  } else if (strcmp(fname, "/small.gif") == 0) {
     printf("HTTP/1.0 200 OK\r\n");
     printf("Content-Type: %s\r\n", content_type);
     printf("Content-Length: %ld\r\n\r\n", sizeof(gif));
     fwrite(gif, sizeof(char), sizeof(gif), stdout);
  } else {
    http_send_reply_not_found();
    return;
  }
}

const int KEEP_ALIVE = 0;
const int CLOSE = 1;

int connect_mode;

#define BUFFERSIZE 512

char* http_receive_request() {
  long long int read_limit = 4096;

  connect_mode = -1;

  char buffer[BUFFERSIZE] = {};
  scanf("%[^\n]", buffer);
  getchar();
  
  if (memcmp(buffer, "GET ", 4) != 0) return NULL;
  
  int n = strlen(buffer);
  read_limit -= n;

  if (n < 9) return NULL;

  char* tail = buffer + n-9;
  if (memcmp(tail, " HTTP/1.0", 9) != 0 && 
      memcmp(tail, " HTTP/1.1", 9) != 0) return NULL;

  *tail = '\0';
  char* ret = strdup(buffer+4);
  *tail = ' ';

  while (1) {
    buffer[0] = '\0';
    scanf("%[^\n]", buffer);
    getchar();
    
    int n = strlen(buffer);
    if (n == 0) break;

    read_limit -= n;
    if (read_limit < 0) {
      free(ret);
      return NULL;
    }

    if (n < 12) continue;
    if (memcmp(buffer, "Connection: ", 12) != 0) continue;

    if (connect_mode != -1) {
      free(ret);
      return NULL;
    }

    if (strcmp(buffer+12, "keep-alive") == 0) {
      connect_mode = KEEP_ALIVE;
    } else if (strcmp(buffer+12, "close") == 0) {
      connect_mode = CLOSE;
    } else {
      free(ret);
      return NULL;
    }
  }

  return ret;
}

int main() {
  setbuf(stdout, NULL);
  while (1) {
    char* fname = http_receive_request();
    if (fname == NULL) {
      http_send_reply_bad_request();
    } else {
      try_http_send_reply_with_file(fname);
      free(fname); 
    }

    if (connect_mode != KEEP_ALIVE) break;
  }
}

