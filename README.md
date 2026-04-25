# 🌐 Webserv — From Educational HTTP Server to Modern System

Webserv started as a school project: a from-scratch implementation of an HTTP/1.0 server in C++98, designed to deepen understanding of one of the core protocols of the web.

What began as a low-level exercise in socket programming and protocol handling is now evolving into a modern, production-inspired engineering project focused on:

- Low-level systems expertise
- Clean and modern C++ design
- Robust testing practices
- Real-world DevOps workflows

## 📌 Overview

Webserv is a fully functional HTTP/1.0 server in C++20, designed to handle real browser requests with a strict non-blocking architecture.

It implements core web server features from scratch:

- HTTP request parsing and response generation
- Static file serving and uploads
- CGI execution (PHP, Python)
- Configurable routing and virtual servers

The project emphasizes correctness, performance, and architectural clarity, without relying on external frameworks.

### ⚡ Highlights

- 🔌 Fully non-blocking, event-driven server
  → single poll() loop handling all client and socket I/O
- 🌐 HTTP/1.0 support with `GET`, `POST`, `DELETE`
- 📁 Static file serving + upload handling
- 🔄 CGI support (PHP & Python)
- ⚙️ Fully configurable (routes, ports, autoindex, error pages)
- 🍪 Basic cookies & session handling
- 🔀 Multi-port / virtual server support

### 🚧 Ongoing Refactor

The project is currently being heavily refactored and modernized:

- 🆕 Core Refactor
  - 🚧 Migration from C++98 → C++20
  - 🚧 Cleaner architecture & separation of concerns
- 🧪 Testing
  - 🚧 Python functional test suite
  - 📌 Full unit test coverage (planned)
- 🔁 DevOps
  - 🚧 Docker containerization
  - 📌 Jenkins CI/CD pipeline with Docker Hub deployment (planned)

### 🎯 Direction

The goal is to turn this project into a clean, reliable, and maintainable system, combining:

- Systems-level programming
- Modern C++ practices
- Automated testing
- Continuous integration & deployment

## 🛠️ Build & Deployment

### 🔧 Build

The project uses a Makefile with multiple targets depending on your workflow:

- `make confpath`
  → set the PATH for config files to the actual working directory
- `make` or `make webserv`
  → Build the server locally
- `make docker`
  → Build the project inside a Docker container (recommended for reproducibility)
- `make test`
  → Build inside Docker and run the Python functional test suite

### 🧹 Cleanup

- `make clean`
  → Remove object files
- `make fclean`
  → Remove object files and the executable

### ▶️ Run the Server

After building and setting the root path in the default config file (`conf/webserv_default.conf`), start the server with:

```sh
./webserv
```

You can optionally provide a configuration file:

```sh
./webserv myconfig.conf
```

## 🧩 Configuration Overview

The configuration system is inspired by web servers like Nginx and allows fine-grained control over server behavior.

### 🖥️ Server Block

Each server block defines a virtual server:

- listen → Port to bind
- root → Base directory for file serving
- index → Default file for directories
- server_name → Hostname
- client_max_body_size → Max request size
- error_page → Custom error pages

You can define multiple servers on different ports.

### 📍 Location Block

Each location defines behavior for a specific route:

- allow_methods → Allowed HTTP methods
- root → Override root directory
- index → Override default file
- autoindex → Enable/disable directory listing
- upload_path → Enable file uploads and define storage
- return → HTTP redirection
- extension + cgi_path → Enable CGI execution

### 🔄 CGI Example

```sh
location /php {
	allow_methods GET POST;
	extension .php;
	cgi_path /usr/bin/php-cgi;
}
```

This route:

- Executes `.php` files using php-cgi
- Passes full request data to the CGI process

### 📤 File Upload Example

```sh
location /upload {
	allow_methods GET POST DELETE;
	autoindex on;
}
```

- Enables file uploads via POST
- Allows browsing uploaded files (autoindex on)
- Supports deletion (DELETE)

### 🪶 Minimal Configuration Example

```
server {
	listen 8080;
	root /path/to/webserv/www;
	index index.html;

	error_page 400 /html/wsstatus/ws_400.html;
	error_page 403 /html/wsstatus/ws_403.html;
	error_page 404 /html/wsstatus/ws_404.html;
	error_page 405 /html/wsstatus/ws_405.html;
	error_page 413 /html/wsstatus/ws_413.html;
	error_page 500 /html/wsstatus/ws_500.html;
	error_page 501 /html/wsstatus/ws_501.html;
	error_page 505 /html/wsstatus/ws_505.html;

	location / {
		allow_methods GET;
	}
}
```

### 🏗️ Full Configuration Example

```
server {
	listen 8080;
	client_max_body_size 100000;
	root /paht/to/webserv/www;
	index /html/page/index.html;
	server_name localhost;

	error_page 400 /html/wsstatus/ws_400.html;
	error_page 403 /html/wsstatus/ws_403.html;
	error_page 404 /html/wsstatus/ws_404.html;
	error_page 405 /html/wsstatus/ws_405.html;
	error_page 413 /html/wsstatus/ws_413.html;
	error_page 500 /html/wsstatus/ws_500.html;
	error_page 501 /html/wsstatus/ws_501.html;
	error_page 505 /html/wsstatus/ws_505.html;

	location / {
		allow_methods GET POST DELETE;
		index /html/page/index.html;
		autoindex off;
	}

	location /css {
		allow_methods GET;
		autoindex off;
	}

	location /php {
		allow_methods GET POST;
		extension .php;
		cgi_path /usr/bin/php-cgi;
		autoindex off;
	}

	location /python {
		allow_methods GET POST;
		extension .py;
		cgi_path /usr/bin/python3;
		autoindex off;
	}

	location /html/kitty {
		allow_methods GET POST;
		upload_path SERVER_PATH/www/upload/;
		autoindex off;
	}

	location /upload {
		allow_methods GET POST DELETE;
		autoindex on;
	}
}
```

## Credits

Created by [pjay](https://github.com/Pierrickjay) and [rertzer](https://github.com/rertzer)
