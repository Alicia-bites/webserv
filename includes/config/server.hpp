#pragma once

#include <iostream>
#include <string>
#include <map>

#include "location.hpp"
#include "colors.hpp"


namespace webserv
{
	namespace config
	{
		class Server
		{
			public :
				typedef std::map<std::string, Location>									map_type;
				typedef std::map<std::string, std::pair<std::string, unsigned int> >	cgi_type;
				typedef std::map<std::string, std::pair<int, std::string> >			redirect_type;

			private :
				std::string					serverName_;
				std::string 				root_;
				std::string					IP_;
				int							port_;
				cgi_type					cgi_;
				unsigned long				maxBodySize_;
				std::vector<std::string>	index_;
				// std::vector<std::string>	uploadDir_;
				bool						directoryIndex_;
				map_type 					locations_;
				std::map<int, std::string>	errorPage_;
				redirect_type				redirect_;

				
			public:
				Server();
				Server(const Server & src);
				~Server();

				Server & operator=(const Server & src);
				
				std::string 						getServerName() const;
				std::string 						getRoot() const;
				int									getPort() const;
				std::string							getIP() const;
				cgi_type const &					getCgi() const;
				unsigned long						getMaxBodySize() const;
				std::vector<std::string>			getIndex() const;
				// std::vector<std::string>			getUploadDir() const;
				bool								getDirectoryIndex() const;
				map_type const &					getLocations() const;
				Location const	 					getDefaultLocation() const;
				redirect_type const &				getRedirect() const;
				std::map<int, std::string> const &	getErrorPage() const;


				void	setServerName(const std::string & serverName);
				void	setRoot(const std::string & root);
				void	setPort(const int & port);
				void	setIP(const std::string & IP);
				void	setCgi(cgi_type & cgi);
				void	setMaxBodySize(unsigned long & maxBodySize);
				void	setIndex(std::vector<std::string> & index);
				// void	setUploadDir(const std::vector<std::string> & upload_dir);
				void	setDirectoryIndex(bool & directoryIndex);
				void	setLocations(map_type & locations);
				void	setErrorPage(std::map<int, std::string> & errorPage);
				void	setRedirect(redirect_type & redirect);

		};
		std::ostream & operator<<(std::ostream & o, Server const & server);
	}
}
