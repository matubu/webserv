/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bledda <bledda@student.42nice.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 02:35:05 by bledda            #+#    #+#             */
/*   Updated: 2022/02/12 00:05:56 by bledda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "utils.hpp"
# include <stdexcept>
# include <map>

using namespace std;

class config
{
	private:
		typedef	string						URL;
		typedef string						EXTENSION;
		typedef vector<string>				METHOD;
		typedef int							CODE;
		typedef map <EXTENSION, string>		CGI;
		typedef pair<CODE, string>			REDIRECTION;
	private:
		int									_port;
		string								_host;
		int									_body_size;
		map<int, string>					_error;
		vector<string>						_server_name;

		map <URL, bool>						_autoindex;
		map <URL, string>					_root;
		map <URL, string>					_index_file;
		map <URL, METHOD>					_method;
		map <URL, REDIRECTION>				_redirect;
		map <URL, CGI>						_cgi;
	public:
		class NotDigit: public exception {
			public :
			const char * what() const throw() {return ("Not digit");}
		};
		class NegDigit: public exception {
			public :
			const char *what() const throw() {return ("Negatif digit");}
		};
		class NotBool: public exception {
			public :
			const char *what() const throw() {return ("Not Boolean");}
		};
		class NotMethod: public exception {
			public :
			const char *what() const throw() {return ("Not Method");}
		};
		class NotRedictCode: public exception {
			public :
			const char *what() const throw() {return ("Not Code Redirection");}
		};
		class NotDefine: public exception {
			public :
			const char *what() const throw() {return ("Not Define");}
		};
	public:
		config()
		{
			set_port(8080);
			set_host("127.0.0.1");
			set_body_size(1024);
		}
		~config() {};
		void set_port(int port) {
			(port < 0) ? throw NegDigit() : _port = port;
		}
		void set_port(string port) {
			(strisdigit(port)) ? set_port(stoi(port)) : throw NotDigit();
		}
		void set_host(string host) {
			_host = host;
		}
		void set_body_size(int body_size) {
			(body_size < 0) ? throw NegDigit() : _body_size = body_size;
		}
		void set_body_size(string body_size) {
			(strisdigit(body_size)) ? set_port(stoi(body_size)) : throw NotDigit();
		}
		void set_error(int code_error, string url) {
			(code_error < 0) ? throw NegDigit() : set_error(code_error, url);
		}
		void set_error(string code_error, string url) {
			if (!strisdigit(code_error))
				throw NotDigit();
			set_error(stoi(code_error), url);
		}
		void set_server_name(string name) {
			_server_name.push_back(name);
		}
		void set_autoindex(string url, bool val) {
			_autoindex[url] = val;
		}
		void set_autoindex(string url, string val) {
			if (strtolower(val) == "true"
			|| strtolower(val) == "1"
			|| strtolower(val) == "false"
			|| strtolower(val) == "0")
				set_autoindex(url, ((strtolower(val) == "true"
									|| strtolower(val) == "1") ? true : false));
			else
				throw NotBool();
		}
		void set_root(string url, string root) {
			_root[url] = root;
		}
		void set_index_file(string url, string file) {
			_index_file[url] = file;
		}
		void set_method(string url, string method) {
			if (strtolower(method) == "get"
				|| strtolower(method) == "post"
				|| strtolower(method) == "delete")
			{
				vector<string>::iterator it;
				for (it = _method[url].begin(); it != _method[url].end(); it++)
					if (strtolower(method) == strtolower(*it))
						return ;
				_method[url].push_back(method);
			}
			else
				throw NotMethod();
		}
		void set_redirect(string url, int code, string redirect) {
			if (code < 0)
				throw NegDigit();
			else if (code < 300 || code > 307)
				throw NotRedictCode();
			_redirect[url] = make_pair(code, redirect);
		}
		void set_redirect(string url, string code, string redirect) {
			if (!strisdigit(code))
				throw NotDigit();
			set_redirect(url, stoi(code), redirect);
		}
		void set_cgi(string url, string extension, string bin_cgi) {
			_cgi[url][extension] = bin_cgi;
		}
	public :
		string get_port() const {return (itos(_port));}
		string get_host() const {return (_host);}
		string get_body_size() const {return (itos(_body_size));}
		string get_error(int error) {
			if (_error.count(error))
				return (_error[error]);
			throw NotDefine();
		}
		vector<string> get_server_name() const {return (_server_name);}
		bool get_autoindex(string url) {
			if (_autoindex.count(url))
				return (_autoindex[url]);
			throw NotDefine();
		}
		string get_root(string url) {
			if (_root.count(url))
				return (_root[url]);
			throw NotDefine();
		}
		string get_index_file(string url) {
			if (_index_file.count(url))
				return (_index_file[url]);
			throw NotDefine();
		}
		vector<string> get_method(string url) {
			if (_method.count(url))
				return (_method[url]);
			throw NotDefine();
		}
		REDIRECTION get_redirect(string url) {
			if (_redirect.count(url))
				return (_redirect[url]);
			throw NotDefine();
		}
		CGI get_cgi(string url) {
			if (_cgi.count(url))
				return (_cgi[url]);
			throw NotDefine();
		}
};