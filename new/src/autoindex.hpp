#pragma once

#include "utils.hpp"
#include "Request.hpp"
#include "Response.hpp"

void    autoindex(int fd, const Request &req, const Response &res)
{
    DIR				*dir;
    struct dirent	*diread;
    std::string		file = ftos(AUTOINDEX_TEMPLATE_FILE);
    size_t			start = file.find("{{"), end = file.find("}}");
    std::string		s = file.substr(0, start);
    std::string		pattern = file.substr(start + 2, end - start - 2);

    if ((dir = opendir(res.path.c_str())) == NULL)
        errorpage("403", "Forbidden", fd);
    while ((diread = readdir(dir)))
    {
        if (!strcmp(diread->d_name, ".")
            || (!strcmp(diread->d_name, "..") && req.url == "/")) continue ;

        struct stat	stats;
        char		date[128];

        stat((res.path + "/" + diread->d_name).c_str(), &stats);
        strftime(date, 128, "%d %h %Y", localtime(&stats.st_ctime));

        s += replaceAll(replaceAll(replaceAll(replaceAll(replaceAll(pattern,
                    "$NAME", diread->d_name),
                    "$URL", req.url + diread->d_name),
                    "$DATE", std::string(date)),
                    "$SIZE", readable_fsize(stats.st_size)),
                    "$ISDIR", stats.st_mode & S_IFDIR ? "1" : "");
    }
    s += file.substr(end + 2);
    s = headers("200 OK", s.size(), "text/html") + "\r\n" + s;
    send(fd, s.c_str(), s.size(), 0);
}