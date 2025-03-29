```
  C HTTP server that serves basic HTML files on request
```


TODO:
- Add authentication with tokens


# a-very-simple-http-server

serves .html files from `./html/`

example usage:

`http://localhost:PORT/` -> returns `html/index.html`

`http://localhost:PORT/test.html` -> returns `html/test.html`


if file is not found, or error reading and sending file, the server serves the appropriate error:

```
  const char *http_404_response =
      "HTTP/1.1 404 Not Found\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 48\r\n"
      "\r\n"
      "<html><body><h1>404 Not Found</h1></body></html>";
  
  const char *http_500_response =
      "HTTP/1.1 500 Internal Server Error\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 54\r\n"
      "\r\n"
      "<html><body><h1>500 Server Error</h1></body></html>";
```
