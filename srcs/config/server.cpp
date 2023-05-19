#include "server.hpp"

namespace webserv
{
	namespace config
	{
		// CONSTRUCTORS ---------------------------------------------------------------------------------------------------
		Server::Server()
		: serverName_("none")
		, root_("none")
		, IP_("")
		, port_(0)
		, maxBodySize_(8388608)
		, directoryIndex_(true)
		, locations_(std::map<std::string, Location>())
		, errorPage_(std::map<int, std::string>())
		{
			#if DEBUG
				std::cout << AQUAMARINE1 << "Calling Server default constructor" << RESET << std::endl;
			#endif
		}

		Server::Server(const Server & src)
		: serverName_(src.serverName_)
		, root_(src.root_)
		, IP_(src.IP_)
		, port_(src.port_)
		, cgi_(src.cgi_)
		, maxBodySize_(src.maxBodySize_)
		, index_(src.index_)
		// , uploadDir_(src.uploadDir_)
		, directoryIndex_(src.directoryIndex_)
		, locations_(src.locations_)
		, errorPage_(src.errorPage_)
		, redirect_(src.redirect_)
		{
			#if DEBUG
				std::cout << AQUAMARINE1 << "Calling Server copy constructor" << RESET << std::endl;
			#endif
		}

		// DESTRUCTORS ---------------------------------------------------------------------------------------------------

		Server::~Server()
		{
			#if DEBUG
				std::cout << AQUAMARINE1 << "Calling Server destructor" << RESET << std::endl;
			#endif
		}

		// ASSIGNEMENT ---------------------------------------------------------------------------------------------------

		Server & Server::operator=(const Server & src)
		{
			#if DEBUG
				std::cout << AQUAMARINE1 << "Calling Server assignement operator" << RESET << std::endl;
			#endif

			if (this != &src)
			{
				serverName_ = src.serverName_;
				root_ = src.root_;
				IP_ = src.IP_;
				port_ = src.port_;
				cgi_ = src.cgi_;
				maxBodySize_ = src.maxBodySize_;
				index_ = src.index_;
				// uploadDir_ = src.uploadDir_;
				redirect_ = src.redirect_;
				directoryIndex_ = src.directoryIndex_;
				locations_ = src.locations_;
				errorPage_ = src.errorPage_;
			}

			return *this;
		}

		// GETTERS ---------------------------------------------------------------------------------------------------

		std::string Server::getServerName() const
		{
			return serverName_;
		}

		std::string Server::getRoot() const
		{
			return root_;
		}

		std::string Server::getIP() const
		{
			return IP_;
		}

		int	Server::getPort() const
		{
			return port_;
		}

		Server::cgi_type const &	Server::getCgi() const
		{
			return cgi_;
		}

		unsigned long	Server::getMaxBodySize() const
		{
			return maxBodySize_;
		}

		Server::map_type const &	Server::getLocations() const
		{
			return locations_;
		}

		Location const	Server::getDefaultLocation() const
		{
			Location defaultLocation;
			defaultLocation.setDirectoryIndex(getDirectoryIndex());
			defaultLocation.setIndex(getIndex());
			defaultLocation.setMaxBodySize(getMaxBodySize());
			defaultLocation.setPath("");
			// defaultLocation.setUploadDir(getUploadDir());
			std::cout << defaultLocation << std::endl;
			return defaultLocation;
		}

		std::map<int, std::string> const &	Server::getErrorPage() const
		{
			return errorPage_;
		}

		std::vector<std::string>	Server::getIndex() const
		{
			return index_;
		}

		// std::vector<std::string>	Server::getUploadDir() const
		// {
			// return uploadDir_;
		// }

		bool	Server::getDirectoryIndex() const
		{
			return directoryIndex_;
		}

		Server::redirect_type const & 	Server::getRedirect() const
		{
			return redirect_;
		}

		// SETTERS ---------------------------------------------------------------------------------------------------

		void	Server::setServerName(const std::string & serverName)
		{
			serverName_ = serverName;
		}

		void	Server::setRoot(const std::string & root)
		{
			root_ = root;
		}

		void	Server::setPort(const int & port)
		{
			port_ = port;
		}

		void	Server::setIP(const std::string & IP)
		{
			IP_ = IP;
		}

		void	Server::setCgi(cgi_type & cgi)
		{
			cgi_ = cgi;
		}

		void	Server::setMaxBodySize(unsigned long & maxBodySize)
		{
			if (maxBodySize == 0)
				maxBodySize_ = 8388608;
			else
				maxBodySize_ = maxBodySize;
		}

		void	Server::setIndex(std::vector<std::string> & index)
		{
			index_ = index;
		}

		// void	Server::setUploadDir(const std::vector<std::string> & uploadDir)
		// {
		// 	uploadDir_ = uploadDir;
		// }

		void	Server::setDirectoryIndex(bool & directoryIndex)
		{
			directoryIndex_ = directoryIndex;
		}

		void	Server::setLocations(map_type & locations)
		{
			locations_ = locations;
		}

		void	Server::setErrorPage(std::map<int, std::string> & errorPage)
		{
			errorPage_ = errorPage;
		}

		void	Server::setRedirect(redirect_type & redirect)
		{
			redirect_ = redirect;
		}

		std::ostream & operator<<(std::ostream & o, Server const & server)
		{
			o	<< DEEPPINK1
				<< "Server name = "
				<< server.getServerName() << std::endl
				<< DEEPPINK3
				<< "Root = "
				<< server.getRoot() << std::endl
				<< DEEPPINK5
				<< "IP = "
				<< server.getIP() << std::endl
				<< PLUM1
				<< "Port = "
				<< server.getPort() << std::endl
				<< DEEPPINK6
				<< "maxBodySize = "
				<< server.getMaxBodySize() << std::endl
				<< MAGENTA1
				<< "directoryIndex = "
				<< server.getDirectoryIndex() << std::endl
				<< DEEPPINK8
				<< "redirect = ";
				for (Server::redirect_type::const_iterator it = server.getRedirect().begin(); it != server.getRedirect().end(); it++)
				{
					o << "key --> " << it->first << std::endl << "error code : " 
					<< it->second.first << " | url to redirect on : " 
					<< it->second.second << RESET << std::endl;
				}

				o << std::endl << MAGENTA4 << "index = ";
				std::vector<std::string> index = server.getIndex();
				for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); it++)
					o << *it << " | ";

				o << std::endl << PLUM3 << "cgi = ";
				Server::cgi_type cgi = server.getCgi();
				for (Server::cgi_type::iterator it = cgi.begin(); it != cgi.end(); it++)
					o << it->first << " | " << it->second.first << ", timeout (opt) = " << it->second.second << std::endl;

				o << std::endl << MAGENTA5 << "locations = ";
				Server::map_type locations = server.getLocations();
				for (Server::map_type::iterator it = locations.begin(); it != locations.end(); it++)
					o << "key --> " << it->first << std::endl << it->second << RESET << std::endl;

				o << std::endl << VIOLET << "errorPage = ";
				std::map<int, std::string> errorPage = server.getErrorPage();
				for (std::map<int, std::string>::iterator it = errorPage.begin(); it != errorPage.end(); it++)
					o << VIOLET << "key --> " << it->first << std::endl << it->second << RESET << std::endl;

				// o << std::endl << MEDIUMORCHID3 << "uploadDir = ";
				// std::vector<std::string> uploadDir = server.getUploadDir();
				// for (std::vector<std::string>::iterator it = uploadDir.begin(); it != uploadDir.end(); it++)
				// 	o << *it << " | ";

				return o;
		}
	}
}
