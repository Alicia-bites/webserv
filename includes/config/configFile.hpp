#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <utility>
#include <algorithm>
#include <cctype>

#include <sstream>

#include <cstring>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "colors.hpp"

#include "configFile.hpp"
#include "location.hpp"
#include "server.hpp"
#include "exceptions.hpp"

// this class will take a string that is the name of a config file.
// It will open it.
// Parse it.
// If it encounters an error, it will add it to a container (still 
// undefined and return it to the user for him to to do corrections)

namespace webserv
{
	namespace config
	{
		class ConfigFile
		{
			public :
				typedef std::map<std::string, Server>	map_type;

			private :
				std::string					fileName_;
				std::ifstream				input_;
				std::vector<std::string>	lines_;
				std::vector<std::string>	errors_;
				std::map<std::string, int>	counterServer_;
				std::map<std::string, int>	counterLocation_;


			public :
				map_type					servers;

				ConfigFile();
				ConfigFile(const std::string& input);
				ConfigFile(const ConfigFile & src);
				~ConfigFile();

				ConfigFile &	operator=(const ConfigFile & src);

				std::vector<std::string>	getErrors() const;
				Server const &				getServer(std::string const & host, int & port);
				// Server const &				getServer(std::string const & host);
				// getLocation(chemin d'acces) --> si pas trouvé, retourne une Location par défaut
				Location const  			getLocation(std::string const & serverName, std::string const & filePath);
			
			private :
				void			isValid();
				void			readFile();
				bool			parseFile();
				void			produceConfigLogs();
			
				void			initReqServer();
				void    		initReqLocation();

				std::string		firstWord(std::string & line);
				void			trimSpaces(std::string& line);
				void			replaceTabs(std::string & line);
				bool			reachedEndOfServerBlock(std::vector<std::string>::const_iterator it);


				void			trimInlineComment(std::string & line);
				unsigned long	convertSize(unsigned long & size, char unit);
				
				template <typename T>
				bool 			hasDuplicates(const std::vector<T> & vec);
				
				bool			isValidSize(const std::string & size);
				bool			isNumAndDots(const std::string & line);
				bool			isValidIpAddress(const char *);
				bool			isLastWord(const std::string & line, const std::string & word);
				bool			foundListenDuplicate(std::vector<std::string> & ServerNamesAddons);
				bool			onlyWhiteSpaces(std::string const & line);
				int				getNextLineIndent(std::vector<std::string>::const_iterator it);


				bool					getListen(std::string & line, std::pair<std::string, int> listen);

				void							parseServerName(std::string & line, std::vector<std::string> & server_name);
				std::string						parseRoot(std::string & line);
				std::pair<std::string, int>		parseListen(std::string & line);
				unsigned long					parseMaxBodySize(std::string & line);
				std::string						parseLocation(std::string & line, Server::map_type & locations);
				Location::vector_type			parseIndex(std::string & line);
				void    						parseCgi(std::string & line, Server::cgi_type & cgi);
				bool							parseDirectoryIndex(std::string & line, bool & defined);
				Server::redirect_type			parseRedirect(std::string & line, Server::redirect_type & redirect);
				std::map<int, std::string>		parseErrorPage(std::string & line, std::map<int, std::string> & error_page);
				std::vector<std::string>		parseUploadDir(std::string & line);
				bool    						parseAllowUpload(std::string & line);
				void							verifyOccurence(std::string block);
				map_type::iterator const		samePortAs(std::string name, int port, std::string IP);

		};
	}
}
