#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "colors.hpp"
#include "exceptions.hpp"

// location <path> <method>
// if <path> is not specified, its default value will be /
// if <method> is not specified, its default value will GET POST DELETE
class Location
{
	public :
		typedef	std::vector<std::string>	vector_type;

	private :
		std::string 				path_;
		std::string 				root_;
		vector_type					methods_;
		vector_type					index_;
		// std::vector<std::string>	uploadDir_;
        unsigned long 				maxBodySize_;
		bool						directoryIndex_;
		bool						directoryIndexWasSet_;
		bool						allowUpload_;

	public:
		Location();
		Location(std::string path);
		Location(std::string path, std::vector<std::string> methods);
		Location(const Location & src);
		~Location();

		Location & operator=(const Location & src);
		
		std::string 				getPath() const;
		std::string 				getRoot() const;
		std::vector<std::string>	getMethods() const;
		vector_type					getIndex() const;
		// std::vector<std::string>	getUploadDir() const;
		unsigned long				getMaxBodySize() const;
		bool						getDirectoryIndex() const;
		bool						getDirectoryIndexWasSet() const;
		bool						getAllowUpload() const;


		void	setPath(const std::string & path);
		void	setRoot(const std::string & root);
		void	setMethods(const std::vector<std::string> & methods);
		void	setIndex(const vector_type & index);
		// void	setUploadDir(const std::vector<std::string> & upload_dir);
		void	setMaxBodySize(const unsigned long & maxBodySize);
		void	setDirectoryIndex(const bool & directoryIndex);
		void	setAllowUpload(const bool & allowUpload);

};

std::ostream & operator<<(std::ostream & o, Location const & location);
