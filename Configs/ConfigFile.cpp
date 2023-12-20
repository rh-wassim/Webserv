#include "Config.hpp"

int      ft_isdigit(char *n)
{
    int i = 0;
    int num;
    while (n[i])
    {
        if (!isdigit(n[i]))
        {
            std::cout << "Not a number\n";
            exit(1);
        }
        i++;
    }
    num = atoi(n);
    return (num);
}

void    confi::split_string(std::string &str, location &loc, char c)
{
    char *word = strtok(const_cast<char *>(str.c_str()), " ");

    word = strtok(NULL, " ");
    while (word)
    {
        if (c == 'm' && (!strcmp(word, "GET") || !strcmp(word, "POST") || !strcmp(word, "DELETE")))
            loc.allow_methods.push_back(word);
        else if (c == 'i')
            loc.index.push_back(word);
        else
        {
            std::cerr << "You must Follow Our Rules in Methods\n";
            exit(1);
        }
        word = strtok(NULL, " ");
    }
}

void     parse_host(std::string host)
{
    char *n;
    int i = 0;
    int num;

    n = strtok(const_cast<char *>(host.c_str()), ".");
    while (n != NULL)
    {
        num = ft_isdigit(n);
        if (num < 0 || num > 255)
        {
            std::cout << "Out of Range\n";
            exit(1);
        }
        n = strtok(NULL, ".");
        i++;
    }
    if (i != 4)
    {
        std::cout << "Error: Not IPv4\n";
        exit(1);
    }
}

int     confi::parse_configuration(std::string file)
{
    std::fstream config_file;
    std::string line, key, value;
    struct server serv;
    struct location loc;
    int i = -1;
    int j = -1;
    config_file.open(file);
    if (config_file.is_open())
    {
        config_file.seekg(0, std::ios::end);
        if (config_file.tellg() == 0)
        {
            std::cout << "Error : Config File is Empty\n";
            exit(1);
        }
        config_file.seekg(0, std::ios::beg);
        while (getline(config_file, line))
        {
            i = -1;
            if (!line.compare("server"))
            {
                i = 0;
                j = -1;
                serv.mx_cl_bd_size = 1000000;
                while (getline(config_file, line))
                {
                    std::stringstream strm(line);
                    strm >> key;
                    strm >> value;
                    if (!line.compare("{"))
                        i++;
                    else if (!key.compare("location"))
                    {
                        j = 0;
                        if (!strm.eof())
                        {
                            std::cerr << "Error : location Path\n";
                            exit(1);
                        }
                        loc.path_location = value;
                        loc.auto_index = "on";
                        loc.auto_upload = "on";
                        while (getline(config_file, line))
                        {
                            std::string key_local, value_local;
                            std::stringstream strm_local(line);
                            strm_local >> key_local;
                            strm_local >> value_local;
                            if (!line.compare("{"))
                            {
                                j++;
                                continue;
                            }
                            else if (!line.compare("}"))
                            {
                                j--;
                                serv.locations.push_back(loc);
                                loc.allow_methods.clear();
                                loc.index.clear();
                                loc.path_location.clear();
                                loc.root.clear();
                                loc.auto_upload.clear();
                                loc.auto_index.clear();
                                loc.redirection.clear();
                                break;
                            }
                            if (!key_local.compare("allow_methods"))
                                split_string(line, loc, 'm');
                            else if (!key_local.compare("index"))
                                split_string(line, loc, 'i');
                            else if (!key_local.compare("root") && strm_local.eof()  && !value_local.empty())
                                loc.root = value_local;
                            else if (!key_local.compare("auto_upload") && strm_local.eof() && !value_local.empty())
                                loc.auto_upload = value_local;
                            else if (!key_local.compare("upload_path") && strm_local.eof() && !value_local.empty())
                                loc.upload_path = value_local;
                            else if (!key_local.compare("auto_index") && strm_local.eof() && !value_local.empty())
                                loc.auto_index = value_local;
                            else if (!key_local.compare("return") && strm_local.eof() && !value_local.empty())
                                loc.redirection = value_local;
                            else if (!key_local.compare("cgi_path") && strm_local.eof() && !value_local.empty())
                                loc.cgi_path = value_local;
                            else
                            {
                                std::cerr << "You must Follow Our Rules in Location" << std::endl;
                                exit(1);
                            }

                        }
                        if (j != 0)
                        {
                            std::cerr << "You must Follow Our Rules" << std::endl;
                            exit(1);
                        }
                    }
                    else if (!line.compare("}"))
                    {
                        if ((serv.host.empty() && serv.server_name.empty()) || serv.port.empty() || serv.locations.empty())
                        {
                            std::cerr << "You must Follow Our Rules" << std::endl;
                            exit(1);
                        }
                        servers.push_back(serv);
                        serv.err_pages.clear();
                        serv.locations.clear();
                        i--;
                        break;
                    }
                    else if (!key.compare("host") && strm.eof() && !value.empty())
                    {
                        parse_host(value);
                        serv.host = value;
                    }
                    else if (!key.compare("port") && strm.eof() && !value.empty())
                    {
                        int port_num;
                        port_num = ft_isdigit(const_cast<char *>(value.c_str()));
                        if (port_num < 0 || port_num > 65535)
                        {
                            std::cout << "Error in Port\n";
                            exit(1);
                        }
                        serv.port = value;
                    }
                    else if (!key.compare("max_client_body_size") && strm.eof() && !value.empty())
                        serv.mx_cl_bd_size = std::atoi(value.c_str());
                    else if (!key.compare("server_name"))
                        serv.server_name = value;
                    else if (!key.compare("error_page"))
                    {
                        if (strm.eof())
                            std::cerr << "You must Follow Our Rules" << std::endl;
                        strm >> key;
                        if (!strm.eof()) {
                            std::cerr << "You must Follow Our Rules" << std::endl;
                        }
                            serv.err_pages[value] = key;
                    }
                    else
                    {
                        std::cerr << "You must Follow Our Rules" << std::endl;
                        exit(1);
                    }
                }
            }
            if (i != 0)
            {
                std::cerr << "You must Follow Our Rules" << std::endl;
                exit(1);
            }
        }

    }
    else
    {
        std::cerr << "Error: can't open config file\n";
        exit(1);
    }
    return (0);
}


//////////main//////////

int main(int ac , char **av)
{
    if (ac != 2){
        std::cout << "----->  ./webserv [configuration file]\n";
        exit (1);
    }
    confi cfg;
    cfg.parse_configuration(av[1]);
    std::cout << "---OK---\n";
}
