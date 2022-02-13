#pragma once

#include "utils.hpp"

typedef std::map<std::string, const char *> mime_map_t;

class MimeMap {
	public:
	mime_map_t map;

	MimeMap() {
		map["aac"] = "audio/aac";
		map["abw"] = "application/x-abiword";
		map["arc"] = "application/x-freearc";
		map["avif"] = "image/avif";
		map["avi"] = "video/x-msvideo";
		map["azw"] = "application/vnd.amazon.ebook";
		map["bin"] = "application/octet-stream";
		map["bmp"] = "image/bmp";
		map["bz"] = "application/x-bzip";
		map["bz2"] = "application/x-bzip2";
		map["cda"] = "application/x-cdf";
		map["csh"] = "application/x-csh";
		map["css"] = "text/css";
		map["csv"] = "text/csv";
		map["doc"] = "application/msword";
		map["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
		map["eot"] = "application/vnd.ms-fontobject";
		map["epub"] = "application/epub+zip";
		map["gz"] = "application/gzip";
		map["gif"] = "image/gif";
		map["htm"] = "text/html";
		map["html"] = "text/html";
		map["ico"] = "image/vnd.microsoft.icon";
		map["ics"] = "text/calendar";
		map["jar"] = "application/java-archive";
		map["jpeg"] = "image/jpeg";
		map["jpg"] = "image/jpeg";
		map["js"] = "text/javascript";
		map["json"] = "application/json";
		map["jsonld"] = "application/ld+json";
		map["mid"] = "audio/midi";
		map["midi"] = "audio/midi";
		map["mjs"] = "text/javascript";
		map["mp3"] = "audio/mpeg";
		map["mp4"] = "video/mp4";
		map["mpeg"] = "video/mpeg";
		map["mpkg"] = "application/vnd.apple.installer+xml";
		map["odp"] = "application/vnd.oasis.opendocument.presentation";
		map["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
		map["odt"] = "application/vnd.oasis.opendocument.text";
		map["oga"] = "audio/ogg";
		map["ogv"] = "video/ogg";
		map["ogx"] = "application/ogg";
		map["opus"] = "audio/opus";
		map["otf"] = "font/otf";
		map["png"] = "image/png";
		map["pdf"] = "application/pdf";
		map["php"] = "application/x-httpd-php";
		map["ppt"] = "application/vnd.ms-powerpoint";
		map["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
		map["rar"] = "application/vnd.rar";
		map["rtf"] = "application/rtf";
		map["sh"] = "application/x-sh";
		map["svg"] = "image/svg+xml";
		map["swf"] = "application/x-shockwave-flash";
		map["tar"] = "application/x-tar";
		map["tiff"] = "image/tiff";
		map["tif"] = "image/tiff";
		map["ts"] = "video/mp2t";
		map["ttf"] = "font/ttf";
		map["txt"] = "text/plain";
		map["vsd"] = "application/vnd.visio";
		map["wav"] = "audio/wav";
		map["weba"] = "audio/webm";
		map["webm"] = "video/webm";
		map["webp"] = "image/webp";
		map["woff"] = "font/woff";
		map["woff2"] = "font/woff2";
		map["xhtml"] = "application/xhtml+xml";
		map["xls"] = "application/vnd.ms-excel";
		map["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
		map["xml"] = "application/xml";
		map["xul"] = "application/vnd.mozilla.xul+xml";
		map["zip"] = "application/zip";
		map["3gp"] = "video/3gpp";
		map["3g2"] = "video/3gpp2";
		map["7z"] = "application/x-7z-compressed";
		map["wasm"] = "application/wasm";
	}
	~MimeMap() {}
};

const MimeMap	mime_map;

const char	*mime(const std::string &path)
{
	std::string ext = path.substr(path.find_last_of('.') + 1);
	mime_map_t::const_iterator it = mime_map.map.find(ext);
	if (it == mime_map.map.end())
		return ("text/plain");
	return (it->second);
}