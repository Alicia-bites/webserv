#include "location.hpp"

Location::Location()
: path_("/")
, maxBodySize_(8388608)
, directoryIndex_(true)
, directoryIndexWasSet_(false)
, allowUpload_(false)
{
	#if DEBUG
		std::cout << CHARTREUSE1 << "Calling Location default constructor" << RESET << std::endl;
	#endif

	methods_.push_back("GET");
	methods_.push_back("POST");
	methods_.push_back("DELETE");
}

// WARNING : if no method specified, will set methods to GET, POST, DELETE.
Location::Location(std::string path)
: path_(path)
, maxBodySize_(8388608)
, directoryIndex_(true)
, directoryIndexWasSet_(false)
, allowUpload_(false)
{
	#if DEBUG
		std::cout << CHARTREUSE1 << "Calling Location #1 constructor" << RESET << std::endl;
	#endif

	methods_.push_back("GET");
	methods_.push_back("POST");
	methods_.push_back("DELETE");
}

// WARNING : a deactivated method is equal to ""
Location::Location(std::string path, std::vector<std::string> methods)
: path_(path)
, methods_(methods)
, maxBodySize_(8388608)
, directoryIndex_(true)
, directoryIndexWasSet_(false)
, allowUpload_(false)
{
	#if DEBUG
		std::cout << CHARTREUSE1 << "Calling Location #2 constructor" << RESET << std::endl;
	#endif

	if (methods_.size() > 3)
		throw webserv::exceptions::config::InvalidMethods();
	
	while (methods_.size() < 3)
		methods_.push_back("");
}

Location::Location(const Location & src)
: path_(src.path_)
, root_(src.root_)
, methods_(src.methods_)
, index_(src.index_)
// , uploadDir_(src.uploadDir_)
, maxBodySize_(src.maxBodySize_)
, directoryIndex_(src.directoryIndex_)
, directoryIndexWasSet_(src.directoryIndexWasSet_)
, allowUpload_(src.allowUpload_)
{
	#if DEBUG
		std::cout << CHARTREUSE1 << "Calling Location copy constructor" << RESET << std::endl;
	#endif
}

Location::~Location()
{
	#if DEBUG
		std::cout << CHARTREUSE1 << "Calling Location destructor" << RESET << std::endl;
	#endif
}

Location & Location::operator=(const Location & src)
{
	#if DEBUG
		std::cout << CHARTREUSE1 << "Calling Location assignement operator" << RESET << std::endl;
	#endif

	if (this != &src)
	{
		path_ = src.path_;
		root_ = src.root_;
		methods_ = src.methods_;
		index_ = src.index_;
		// uploadDir_ = src.uploadDir_;
		maxBodySize_ = src.maxBodySize_;
		directoryIndex_ = src.directoryIndex_;
		directoryIndexWasSet_ = src.directoryIndexWasSet_;
		allowUpload_ = src.allowUpload_;
	}

	return *this;
}

std::string Location::getPath() const
{
	return path_;
}

std::string Location::getRoot() const
{
	return root_;
}

std::vector<std::string>	Location::getMethods() const
{
	return methods_;
}

Location::vector_type	Location::getIndex() const
{
	return index_;
}

// std::vector<std::string>	Location::getUploadDir() const
// {
// 	return uploadDir_;
// }

unsigned long	Location::getMaxBodySize() const
{
	return maxBodySize_;
}

bool	Location::getDirectoryIndex() const
{
	return directoryIndex_;
}

bool	Location::getDirectoryIndexWasSet() const
{
	// std::cout << "  DIRECTORY_INDEX_WAS_SET = " << directoryIndexWasSet_ << std::endl;
	return directoryIndexWasSet_;
}

bool	Location::getAllowUpload() const
{
	return allowUpload_;
}

void	Location::setPath(const std::string & path)
{
	path_ = path;
}

void	Location::setRoot(const std::string & root)
{
	root_ = root;
}

void	Location::setMethods(const Location::vector_type & methods)
{
	methods_ = methods;
	
	// be default all three methods are activated
	if (methods_.size() == 0)
	{
		methods_.push_back("GET");
		methods_.push_back("POST");
		methods_.push_back("DELETE");
	}
}

void	Location::setIndex(const Location::vector_type & index)
{
	index_ = index;
}

// void	Location::setUploadDir(const std::vector<std::string> & uploadDir)
// {
	// uploadDir_ = uploadDir;
// }

void	Location::setMaxBodySize(const unsigned long & maxBodySize)
{
	maxBodySize_ = maxBodySize;
}

void	Location::setDirectoryIndex(const bool & directoryIndex)
{
	directoryIndexWasSet_ = 1;
	directoryIndex_= directoryIndex;
}

void	Location::setAllowUpload(const bool & allowUpload)
{
	allowUpload_ = allowUpload;
}

std::ostream & operator<<(std::ostream & o, Location const & location)
{
	o 	<< DEEPSKYBLUE1
		<< "path : "
		<< location.getPath() << std::endl
		<< DARKTURQUOISE
		<< "root : "
		<< location.getRoot() << std::endl
		<< LIGHTSEAGREEN 
		<< "directoryIndex = " 
		<< location.getDirectoryIndex() << std::endl
		<< DEEPSKYBLUE2
		<< "allowUpload = " 
		<< location.getAllowUpload();
		// if (location.getDirectoryIndexWasSet() == 0)
			// o << " | DEFAULT";
		o << std::endl << DEEPSKYBLUE2
		<< "methods = ";
		std::vector<std::string> methods = location.getMethods();
		for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
			o << *it << " | ";

		o << std::endl << DEEPSKYBLUE3 << "index = ";
		std::vector<std::string> index = location.getIndex();
		for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); it++)
			o << *it << " | ";

		// o << std::endl << STEELBLUE3 << "uploadDir = ";
		// std::vector<std::string> uploadDir = location.getUploadDir();
		// for (std::vector<std::string>::iterator it = uploadDir.begin(); it != uploadDir.end(); it++)
			// o << *it << " | ";

		o << std::endl << DEEPSKYBLUE4 << "MaxBodySize = "
		<< location.getMaxBodySize() << RESET << std::endl;

		return o;
}
