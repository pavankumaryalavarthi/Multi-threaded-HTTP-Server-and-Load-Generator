#include "http_server.hh"
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <time.h>
using namespace std;
vector<string> split1(const string &s, char delim) {
  vector<string> elems;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim)) {
    if (!item.empty()) elems.push_back(item);
  }
  return elems;
}

HTTP_Request::HTTP_Request(string r) {
  vector<string> lines = split1(r, '\n');
  vector<string> first_line = split1(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request

  /*
   TODO : extract the request method and URL from first_line here
  */
  this->method = first_line[0];
  this->url = first_line[1];
  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req) {
  HTTP_Request *request = new HTTP_Request(req);
  HTTP_Response *response = new HTTP_Response();
  string url = string("html_files") + request->url;
  response->HTTP_version = "1.1";
  response->content_type = "text/html";
  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";

    string body;

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
      if(url.back() != '/') url += '/';
      url += "index.html";
    }

    /*
    TODO : open the file and read its contents
    */
    ifstream file(url);
    string line;
    int size = 0;
    while(!file.eof()){
        getline(file, line);
        response->body += line;
        size += line.length();
    }
    file.close();
    /*
    TODO : set the remaining fields of response appropriately
    */
    response->content_length = to_string(size);
  }
  else {
   
  
    response->status_code = "404";

    /*
    TODO : set the remaining fields of response appropriately
    */
    response->status_text = "File Not Found";
    response->body = "<html><body><p>404 : File Not Found</p></body></html>";
    response->content_length = to_string(response->body.length());
  }
  delete request;
  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
  time_t time1 = time(NULL);
  return "HTTP/" + this->HTTP_version +" "+ this->status_code+ " "+this->status_text + "\r\nDate : " + ctime(&time1) + "Content-Length: "  + this->content_length + "\r\nContent-Type: "+ this->content_type+"\r\n\n"+this->body;
}