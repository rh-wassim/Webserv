#include "Config.hpp"

int      check_is_digit(char *n)
{
    int i = 0;
    int num;
    while (n[i])
    {
        if (!isdigit(n[i]))
        {
            std::cout << "This is Not a Number\n";
            exit(1);
        }
        i++;
    }
    num = atoi(n);
    return (num);
}

void    confi::split_string(std::string &str, location &lc, char c)
{
    char *word = strtok(const_cast<char *>(str.c_str()), " ");

    word = strtok(NULL, " ");
    while (word)
    {
        if (c == 'm' && (!strcmp(word, "GET") || !strcmp(word, "POST") || !strcmp(word, "DELETE")))
            lc.allow_methods.push_back(word);
        else if (c == 'i')
            lc.index.push_back(word);
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
        num = check_is_digit(n);
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
        std::cout << "Error: This is Not IPv4\n";
        exit(1);
    }
}

int     confi::setup_configuration(std::string file)
{
    std::fstream congi_file;
    std::string line, key, value;
    struct server sr;
    struct location lc;
    int i = -1;
    int j = -1;
    congi_file.open(file);
    if (congi_file.is_open())
    {
        congi_file.seekg(0, std::ios::end);
        if (congi_file.tellg() == 0)
        {
            std::cout << "Error : Config File is Empty\n";
            exit(1);
        }
        congi_file.seekg(0, std::ios::beg);
        while (getline(congi_file, line))
        {
            i = -1;
            if (!line.compare("server"))
            {
                i = 0;
                j = -1;
                sr.mx_cl_bd_size = 1000000;
                while (getline(congi_file, line))
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
                        lc.path_location = value;
                        lc.auto_index = "on";
                        lc.auto_upload = "on";
                        while (getline(congi_file, line))
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
                                sr.locations.push_back(lc);
                                lc.allow_methods.clear();
                                lc.index.clear();
                                lc.path_location.clear();
                                lc.root.clear();
                                lc.auto_upload.clear();
                                lc.auto_index.clear();
                                lc.redirection.clear();
                                break;
                            }
                            if (!key_local.compare("allow_methods"))
                                split_string(line, lc, 'm');
                            else if (!key_local.compare("index"))
                                split_string(line, lc, 'i');
                            else if (!key_local.compare("root") && strm_local.eof()  && !value_local.empty())
                                lc.root = value_local;
                            else if (!key_local.compare("auto_upload") && strm_local.eof() && !value_local.empty())
                                lc.auto_upload = value_local;
                            else if (!key_local.compare("upload_path") && strm_local.eof() && !value_local.empty())
                                lc.upload_path = value_local;
                            else if (!key_local.compare("auto_index") && strm_local.eof() && !value_local.empty())
                                lc.auto_index = value_local;
                            else if (!key_local.compare("return") && strm_local.eof() && !value_local.empty())
                                lc.redirection = value_local;
                            else if (!key_local.compare("cgi_path") && strm_local.eof() && !value_local.empty())
                                lc.cgi_path = value_local;
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
                        if ((sr.host.empty() && sr.server_name.empty()) || sr.port.empty() || sr.locations.empty())
                        {
                            std::cerr << "You must Follow Our Rules" << std::endl;
                            exit(1);
                        }
                        servers.push_back(sr);
                        sr.err_pages.clear();
                        sr.locations.clear();
                        i--;
                        break;
                    }
                    else if (!key.compare("host") && strm.eof() && !value.empty())
                    {
                        parse_host(value);
                        sr.host = value;
                    }
                    else if (!key.compare("port") && strm.eof() && !value.empty())
                    {
                        int num_port;
                        num_port = check_is_digit(const_cast<char *>(value.c_str()));
                        if (num_port < 0 || num_port > 65535)
                        {
                            std::cout << "Error in Port\n";
                            exit(1);
                        }
                        sr.port = value;
                    }
                    else if (!key.compare("max_client_body_size") && strm.eof() && !value.empty())
                        sr.mx_cl_bd_size = std::atoi(value.c_str());
                    else if (!key.compare("server_name"))
                        sr.server_name = value;
                    else if (!key.compare("error_page"))
                    {
                        if (strm.eof())
                            std::cerr << "You must Follow Our Rules" << std::endl;
                        strm >> key;
                        if (!strm.eof()) {
                            std::cerr << "You must Follow Our Rules" << std::endl;
                        }
                            sr.err_pages[value] = key;
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

        //adding

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
    cfg.setup_configuration(av[1]);
    std::cout << "---OK---\n";
}
