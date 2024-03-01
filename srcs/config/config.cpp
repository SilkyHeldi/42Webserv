/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llepiney <llepiney@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/27 18:06:58 by ayblin            #+#    #+#             */
/*   Updated: 2023/07/25 20:17:09 by llepiney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

std::vector<std::string> Config_List::getDistinctPorts() const {
    std::vector<std::string> distinctPorts;
    std::set<std::string> portSet; // Utiliser un set pour stocker les ports distincts

    // Parcourir les configurations dans _ConfigList
    for (std::vector<Config>::const_iterator it = _ConfigList.begin(); it != _ConfigList.end(); ++it) {
        const Config& config = *it;
        portSet.insert(config._port); // Ajouter le port dans le set
    }

    // Copier les ports distincts du set vers le vecteur distinctPorts
    for (std::set<std::string>::const_iterator it = portSet.begin(); it != portSet.end(); ++it) {
        const std::string& port = *it;
        distinctPorts.push_back(port);
    }

    return distinctPorts;
}

// void Location::display() const {
//     std::cout << "Location Properties:" << std::endl;
//     std::cout << "_path: " << _path << std::endl;
//     std::cout << "_root: " << _root << std::endl;
//     std::cout << "_auth: " << _auth << std::endl;
//     std::cout << "_index: " << _index << std::endl;
//     std::cout << "_client_max_body_size: " << _client_max_body_size << std::endl;

//     std::cout << "_limit_except: ";
//     for (std::vector<std::string>::const_iterator it = _limit_except.begin(); it != _limit_except.end(); ++it) {
//         std::cout << *it << " ";
//     }
//     std::cout << std::endl;

//     std::cout << "_error_page: " << std::endl;
//     for (std::map<std::string, std::string>::const_iterator it = _error_page.begin(); it != _error_page.end(); ++it) {
//         std::cout << "    " << it->first << ": " << it->second << std::endl;
//     }

//     std::cout << "_cgi: " << std::endl;
//     for (std::map<std::string, std::string>::const_iterator it = _cgi.begin(); it != _cgi.end(); ++it) {
//         std::cout << "    " << it->first << ": " << it->second << std::endl;
//     }
// }

// void Config::display() const {
//     // Afficher les variables de configuration
//     std::cout << "\033[1;32m=== Configuration ===\033[0m" << std::endl;
//     std::cout << "\033[1;33m_port:\033[0m " << _port << std::endl;
//     std::cout << "\033[1;33m_error_page:\033[0m ";
//     printMap(_error_page);
//     std::cout<< std::endl;
//     std::cout << "\033[1;33m_root:\033[0m " << _root << std::endl;
//     std::cout << "\033[1;33m_servername:\033[0m ";
//     for (const std::string& serverName : _servername) {
//         std::cout << serverName << " ";
//     }
//     std::cout << "\n\033[1;33m_limit_except:\033[0m ";
//     for (const auto& method : _limit_except) {
//         std::cout << method << " ";
//     }
//     std::cout << std::endl;
//     std::cout << "\033[1;33m_cgi:\033[0m" << std::endl;
//     printMap(_cgi);
//     for (const Location& location : _locations) {
//         std::cout << "\033[1;32m=== Locations ===\033[0m" << std::endl;
//         std::cout << "\033[1;33mContent:\033[0m " << location.content << std::endl;
//         std::cout << "\033[1;33m_path:\033[0m " << location._path << std::endl;
//         std::cout << "\033[1;33m_root:\033[0m " << location._root << std::endl;
//         std::cout << "\033[1;33m_auth:\033[0m " << location._auth << std::endl;
//         std::cout << "\033[1;33m_index:\033[0m " << location._index << std::endl;
//         std::cout << "\033[1;33m_limit_except:\033[0m ";
//         for (const auto& method : location._limit_except) {
//             std::cout << method << " ";
//         }
//         std::cout << std::endl;
//         std::cout << "\033[1;33m_cgi:\033[0m" << std::endl;
//         printMap(location._cgi);
//     }
// }

// void Config_List::display() const {
//     // Afficher les configurations pour chaque serveur
//     std::cout << "\033[1;32m=== Config_List ===\033[0m" << std::endl;
//     for (const Config& conf : _ConfigList) {
//         conf.display(); // Call the display function of Config to print individual configurations

//         // Put a big separator between servers
//         std::cout << std::string(50, '=') << std::endl;
//     }
// }

template<typename K, typename V>
void printMap(const std::map<K, V>& map) {
    typename std::map<K, V>::const_iterator it;
    for (it = map.begin(); it != map.end(); ++it) {
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
    }
}


std::map<std::string, std::string> stringToMap(const std::string& input) {
    std::map<std::string, std::string> cgiMap;

    size_t startPos = 0;
    size_t endPos = input.find(";");
    while (endPos != std::string::npos) {
        std::string pair = input.substr(startPos, endPos - startPos);
        size_t spacePos = pair.find(" ");
        if (spacePos != std::string::npos) {
            std::string key = pair.substr(0, spacePos);
            std::string value = pair.substr(spacePos + 1);
            cgiMap[key] = value;
        }
        startPos = endPos + 1;
        endPos = input.find(";", startPos);
    }
    //printMap(cgiMap);
    return cgiMap;
}

std::string removeLeadingSlash(const std::string& str) {
    if (!str.empty() && str[0] == '/')
        return str.substr(1);
    else
        return str;
}

void ft_trim(std::string& str) {
    // Remove leading whitespace
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));

    // Remove trailing whitespace and semicolon
    size_t endPos = str.find_last_not_of(" \t\n\r\f\v;");
    if (endPos != std::string::npos)
        str.erase(endPos + 1);
    else
        str.clear();
}

void removeTrailingSemicolon(std::string& str) {
    if (!str.empty() && str[str.size() - 1] == ';') {
        str.erase(str.size() - 1);
    }
}

std::string extract_text_between_braces(const std::string& input, const std::string& keyword) {
    size_t start_pos = input.find(keyword + " {");
    if (start_pos == std::string::npos) {
        std::cerr << "Error: Could not find keyword \"" << keyword << "\" followed by opening brace.\n";
        return "";
    }

    start_pos += keyword.size() + 2;  // Move past keyword and opening brace

    size_t end_pos = start_pos;
    int brace_count = 1;  // Track nested braces
    while (brace_count > 0 && end_pos < input.size()) {
        if (input[end_pos] == '{') {
            brace_count++;
        } else if (input[end_pos] == '}') {
            brace_count--;
        }
        end_pos++;
    }

    if (brace_count > 0) {
        std::cerr << "Error: Could not find matching closing brace for keyword \"" << keyword << "\".\n";
        return "";
    }

    return input.substr(start_pos, end_pos - start_pos - 1);
}

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string removeLocationBlocks(const std::string& nginxConfig) {
    std::string cleanedConfig;
    std::string::size_type pos = 0;
    std::string::size_type start, end;
    const std::string locationToken = "location";
    const std::string openingBrace = "{";
    const std::string closingBrace = "}";
    int braceCount = 0;

    while (pos < nginxConfig.length()) {
        // Trouver le prochain occurence de "location"
        start = nginxConfig.find(locationToken, pos);
        if (start == std::string::npos) {
            cleanedConfig += nginxConfig.substr(pos);
            break;
        }

        // Ajouter le contenu avant "location" au résultat
        cleanedConfig += nginxConfig.substr(pos, start - pos);

        // Trouver la première accolade ouvrante après "location"
        start = nginxConfig.find(openingBrace, start);
        if (start == std::string::npos) {
            break;  // La configuration est mal formée, sortie prématurée
        }

        // Trouver l'accolade fermante correspondante
        braceCount = 1;
        end = start + 1;
        while (end < nginxConfig.length() && braceCount > 0) {
            if (nginxConfig.compare(end, closingBrace.length(), closingBrace) == 0) {
                --braceCount;
            } else if (nginxConfig.compare(end, openingBrace.length(), openingBrace) == 0) {
                ++braceCount;
            }
            ++end;
        }

        // Ajouter le contenu après "location" au résultat
        pos = end;
    }

    return cleanedConfig;
}

void mergeMaps(std::map<std::string, std::string>& destination, const std::map<std::string, std::string>& source) {
    typedef std::map<std::string, std::string>::const_iterator MapIterator;

    for (MapIterator it = source.begin(); it != source.end(); ++it) {
        destination[it->first] = it->second;
    }
}

std::vector<Location> parseLocations(const std::string& blockserv) {
    std::vector<Location> locations;
    std::istringstream iss(blockserv);
    std::string line;
    Location loc;
    bool insideLocationBlock = false;

    while (std::getline(iss, line)) {
        if (line.find("  location /") == 0) {
            if (insideLocationBlock) {
                locations.push_back(loc);
            }
            loc = Location();
            loc._path = line.substr(11);
            loc._path = loc._path.substr(0, loc._path.find(" {"));
            loc._path = removeLeadingSlash(loc._path);
            insideLocationBlock = true;
        } else if (line.find("  }") == 0) {
            insideLocationBlock = false; 
            locations.push_back(loc);
        } else if (insideLocationBlock) {
            loc.content += line;
            if (line.find("root") != std::string::npos) {
                loc._root = trim(line.substr(line.find("root") + 4));
                ft_trim(loc._root);
            }
            if (line.find("index") != std::string::npos) {
                loc._index = trim(line.substr(line.find("index") + 5));
                ft_trim(loc._index);
            }
            if (line.find("limit_except") != std::string::npos) {
                std::string limitExceptTokens = trim(line.substr(line.find("limit_except") + 12));
    
                // Split the tokens by whitespace and add them to the vector
                std::istringstream iss(limitExceptTokens);
                std::string token;
                while (iss >> token) {
                    if (!token.empty() && token[token.size() - 1] == ';') {
                        token.erase(token.size() - 1);
                    }
                    loc._limit_except.push_back(token);
                }
            }
            if (line.find("auth") != std::string::npos) {
                loc._auth = trim(line.substr(line.find("auth") + 4));
                ft_trim(loc._auth);
            }
            if (line.find("cgi") != std::string::npos) {
                std::string tmp = trim(line.substr(line.find("cgi") + 3));
                //std::cout<<BLUE_TEXT<<"cgi"<<tmp<<DEFAULT<<std::endl;
                std::map<std::string, std::string> tmpMap = stringToMap(tmp);
                mergeMaps(loc._cgi, tmpMap);
            }
            if (line.find("error_page") != std::string::npos) {
                std::string tmp = trim(line.substr(line.find("error_page") + 10));
                loc._error_page = stringToMap(tmp);
            }
            if (line.find("client_max_body_size") != std::string::npos) {
                loc._client_max_body_size = trim(line.substr(line.find("client_max_body_size") + 20));
                ft_trim(loc._client_max_body_size);
            }
        }
    }

    if (insideLocationBlock) {
        locations.push_back(loc);
    }

    return locations;
}

bool compareByPathLength(const Location& loc1, const Location& loc2) {
    return loc1._path.length() > loc2._path.length();
}

void trierLocationsParLongueur(std::vector<Location>& locations) {
    std::sort(locations.begin(), locations.end(), compareByPathLength);
}

Config::Config(std::string conf) : _port("0"), _root("") ,_client_max_body_size(""){
    std::string confrmv = removeLocationBlocks(conf);
    std::stringstream ss(confrmv);
    std::string line;
    while (std::getline(ss, line)) {
        std::stringstream ls(line);
        std::string first_token;
        ls >> first_token;
        if (first_token == "listen") {
            std::string listen;
            ls >> listen;
            _port = listen;
            ft_trim(_port);
        } if (first_token == "cgi") {
            std::string key, value;
            std::map<std::string, std::string> tmpMap;
            ls >> key >> value;
            tmpMap[key] = value;
            mergeMaps(_cgi, tmpMap);
        } else if (first_token == "error_page") {
            std::string key, value;
            ls >> key;
            ls >> value;
            _error_page[key] = value.substr(0, value.size() - 1);
        } else if (first_token == "root") {
            ls >> _root;
            ft_trim(_root);
        }
        else if (first_token == "servername")
        {
            std::string token;
            while (ls >> token) {
                if (!token.empty() && token[token.size() - 1] == ';') {
                    token.erase(token.size() - 1);
                }
                _servername.push_back(token);
            }
        }
        else if (first_token == "client_max_body_size")
        {
            ls >> _client_max_body_size;
            ft_trim(_client_max_body_size);
        }
        else if (first_token == "limit_except") {
            std::string token;
            while (ls >> token) {
                if (!token.empty() && token[token.size() - 1] == ';') {
                    token.erase(token.size() - 1);
                }
                _limit_except.push_back(token);
            }
        }
    }
    std::string blockserv = extract_text_between_braces(conf, "server");
    _locations = parseLocations(blockserv);
    trierLocationsParLongueur(_locations);
}


std::vector<std::string> parseServers(const std::string& filePath) {
    std::vector<std::string> servers;
    std::string line;
    std::string serverBlock;
    bool insideServerBlock = false;
    std::stack<char> bracketStack;
    std::ifstream configFile(filePath.c_str());
    if (!configFile)
        throw std::runtime_error("..");
    while (std::getline(configFile, line)) {
        // Check if the line starts with "server {"
        if (line.find("server {") == 0) {
            // If we were already inside a server block, save its contents as a string
            if (insideServerBlock) {
                servers.push_back(serverBlock);
                serverBlock.clear();
                bracketStack = std::stack<char>();
            }
            insideServerBlock = true;
        }
        // If we're inside a server block, add the current line to its contents
        if (insideServerBlock) {
            serverBlock += line + '\n';
            // Keep track of nested brackets
            for (size_t i = 0; i < line.size(); ++i) {
                char c = line[i];
                if (c == '{') {
                    bracketStack.push(c);
                } else if (c == '}') {
                    bracketStack.pop();
                    // If we've closed all the brackets and are no longer inside the server block, save its contents as a string
                    if (bracketStack.empty()) {
                        servers.push_back(serverBlock);
                        serverBlock.clear();
                        insideServerBlock = false;
                    }
                }
            }
        }
    }
    // If we were still inside a server block when we finished reading the file, save its contents as a string
    if (insideServerBlock) {
        servers.push_back(serverBlock);
    }
    return servers;
}


Config Config_List::getConfigByPortAndServerName(const std::string& port, const std::map<std::string, std::string>& hostMap) const {
    std::map<std::string, std::string>::const_iterator it = hostMap.find("Host");
    std::string serverNameValue = (it != hostMap.end()) ? it->second : "";
    size_t colonPos = serverNameValue.find(':');
    if (colonPos != std::string::npos) {
        serverNameValue = serverNameValue.substr(0, colonPos);
    }
    Config emptyConfig;

    // Recherche de correspondances de port et de servername
    for (size_t i = 0; i < _ConfigList.size(); ++i) {
        const Config& config = _ConfigList[i];
        if (config._port == port) {
            for (size_t j = 0; j < config._servername.size(); ++j) {
                if (serverNameValue == config._servername[j]) {
                    return config; // Retourner la configuration avec le bon port et le bon servername
                }
            }
        }
    }

    // Recherche de correspondances de port uniquement
    for (size_t i = 0; i < _ConfigList.size(); ++i) {
        const Config& config = _ConfigList[i];
        if (config._port == port) {
            return config; // Retourner la configuration avec le bon port
        }
    }

    // Aucune configuration trouvée pour le port spécifié ou le servername spécifié
    // Vous pouvez retourner une configuration par défaut ou effectuer une autre action appropriée.
    // Dans cet exemple, nous renvoyons une configuration vide.
    return emptyConfig;
}


Location Config::findMatchingLocation(const std::string& uri) {
    std::string cleanedUri = uri;

    // Supprimer le '/' initial s'il existe
    if (!cleanedUri.empty() && cleanedUri[0] == '/') {
        cleanedUri = cleanedUri.substr(1);
    }

    // Parcourir le vecteur de locations
    for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
        const Location& loc = *it;
        if (cleanedUri.compare(0, loc._path.length(), loc._path) == 0) {
            return loc; // Renvoyer une copie de l'objet Location trouvé
        }
    }
    static Location emptyLocation;
    return emptyLocation;
}


Config_List::Config_List(std::string conf_path){
    std::vector<std::string> servers = parseServers(conf_path);

    for (std::vector<std::string>::iterator it = servers.begin(); it != servers.end(); ++it) {
        Config config(*it);
        _ConfigList.push_back(config);
    }
    if (!checkForPortAndServerNameConflict())
        throw ConfigListException("Port and servername conflict detected in the configuration.");
}

int Config_List::checkForPortAndServerNameConflict() const {
    for (size_t i = 0; i < _ConfigList.size(); ++i) {
        const Config& config1 = _ConfigList[i];

        for (size_t j = i + 1; j < _ConfigList.size(); ++j) {
            const Config& config2 = _ConfigList[j];

            // Check if ports are the same and server names overlap or are both empty
            if (config1._port == config2._port) {
                if ((config1._servername.empty() && config2._servername.empty()) ||
                    std::find_first_of(config1._servername.begin(), config1._servername.end(),
                                       config2._servername.begin(), config2._servername.end()) != config1._servername.end()) {
                    return 0;  // Conflict found
                }
            }
        }
    }

    return 1;  // No conflicts found
}

std::string Config_List::getLocationRoot(std::string& locationPath, std::string port) {
    for (std::vector<Config>::const_iterator configIt = _ConfigList.begin(); configIt != _ConfigList.end(); ++configIt) {
        const Config& config = *configIt;
        if (config._port == port) {
            for (std::vector<Location>::const_iterator locationIt = config._locations.begin(); locationIt != config._locations.end(); ++locationIt) {
                const Location& location = *locationIt;
                if (location._path == locationPath) {
                    if (!location._root.empty()) {
                        return location._root;
                    } else {
                        return config._root;
                    }
                }
            }
            return config._root;
        }
    }
    // No matching port found, return default root
    return "";
}

std::string Config::getErrorPage(const std::string& locPath, const std::string& error) const {
    // Recherche dans les locations
    for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
        const Location& location = *it;
        if (location._path == locPath) {
            std::map<std::string, std::string>::const_iterator errorPageIt = location._error_page.find(error);
            if (errorPageIt != location._error_page.end()) {
                std::string errorPage = errorPageIt->second;
                removeTrailingSemicolon(errorPage);
                return errorPage;
            }
        }
    }

    // Recherche dans la map _error_page de Config
    std::map<std::string, std::string>::const_iterator errorPageIt = _error_page.find(error);
    if (errorPageIt != _error_page.end()) {
        std::string errorPage = errorPageIt->second;
        removeTrailingSemicolon(errorPage);
        return errorPage;
    }

    // Si aucune correspondance n'est trouvée, retourner une valeur par défaut ou une chaîne vide selon vos besoins
    return "";
}

std::string Config::getRoot(const std::string& locPath) const {
    // Recherche dans les locations
    for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
        const Location& location = *it;
        if (location._path == locPath) {
            return location._root;
        }
    }

    // Si aucune correspondance n'est trouvée, retourne la valeur de _root de Config
    return _root;
}
std::string Config::getBodyLimitForLocation(const std::string& locPath) const {
    // Recherche dans les locations
    for (std::vector<Location>::const_iterator it = _locations.begin(); it != _locations.end(); ++it) {
        const Location& location = *it;
        if (location._path == locPath && !location._client_max_body_size.empty()) {
            return location._client_max_body_size;
        }
    }

    // Si aucune correspondance avec une valeur non vide n'est trouvée, retourne la valeur de _client_max_body_size de Config (peut être vide également)
    return _client_max_body_size;
}

Location::Location(const Location& other) {
    content = other.content;
    _path = other._path;
    _root = other._root;
    _auth = other._auth;
    _index = other._index;
    _client_max_body_size = other._client_max_body_size;
    _limit_except = other._limit_except;
    _error_page = other._error_page;
    _cgi = other._cgi;
}

Location& Location::operator=(const Location& other) {
    if (this == &other) {
        return *this;
    }

    content = other.content;
    _path = other._path;
    _root = other._root;
    _auth = other._auth;
    _index = other._index;
    _client_max_body_size = other._client_max_body_size;
    _limit_except = other._limit_except;
    _error_page = other._error_page;
    _cgi = other._cgi;

    return *this;
}

Config::Config(const Config& other) {
    _servername = other._servername;
    _locations = other._locations;
    _port = other._port;
    _cgi = other._cgi;
    _error_page = other._error_page;
    _root = other._root;
    _client_max_body_size = other._client_max_body_size;
    _limit_except = other._limit_except;
}

Config& Config::operator=(const Config& other) {
    if (this == &other) {
        return *this;
    }

    _servername = other._servername;
    _locations = other._locations;
    _port = other._port;
    _cgi = other._cgi;
    _error_page = other._error_page;
    _root = other._root;
    _client_max_body_size = other._client_max_body_size;
    _limit_except = other._limit_except;
    return *this;
}