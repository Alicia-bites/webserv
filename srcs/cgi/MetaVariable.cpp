#include "MetaVariable.hpp"
#include "Request.hpp"
#include "exceptions.hpp"
#include "utils.hpp"
#include <cstring>

using namespace webserv;
using namespace webserv::io;

/* NOTE:
 * META-VARIABLE remote_ident IS NOT MANDATORY
 * SEE https://www.rfc-editor.org/rfc/rfc3875#section-4.1.10
 *
 * SOURCE:
 * - https://www.rfc-editor.org/rfc/rfc3875#section-4.1
 */

MetaVariable::MetaVariable( webserv::http::Request const &req ):
	_nbVars(0),
	_isInitialized(false),
	_vars(NULL)
{
	std::string	tmpHost;

	setDefaultVars();
	// var[PATH_TRANSLATED] = webserv::utils::path_join("./front", req.uri);
	std::string relPath = webserv::utils::path_join("./front", req.uri);
	if (relPath.at(0) != '/') // not an absolute path
	{
		char *cwd = getcwd(NULL, 0);
		if (cwd == NULL)
			throw exceptions::cgi::AllocationError();
		if (relPath.substr(0, 2) == "./")
			relPath = webserv::utils::path_join(cwd, relPath.substr(2));
		else
			relPath = webserv::utils::path_join(cwd, relPath);
		free(cwd);
	}
	var[PATH_TRANSLATED] = relPath;

	var[QUERY_STRING] = req.query;
	var[REQUEST_METHOD] = req.method;
	var[SERVER_PROTOCOL] = req.protocol;
	var[SCRIPT_NAME] = req.uri;
	var[CONTENT_TYPE] = req.getHeader("Content-Type"); // ONLY TRUE FOR POST METHOD
	var[CONTENT_LENGTH] = req.getHeader("Content-Length"); // ONLY TRUE FOR POST METHOD
	var[REMOTE_ADDR] = req.address;

	tmpHost = req.getHeader("Host");
	var[SERVER_NAME] =  tmpHost.substr(0, tmpHost.find(":"));
	var[SERVER_PORT] = utils::itoa(req.port);
	this->setHttpVars(req);

	size_t varCount = 0; // Number of all variables (envp + meta vars)
	size_t envpCount = 0; // Number of envp variables

	// Count the number of variables
	while (req.envp[varCount] != NULL)
	{
		varCount++;
		envpCount++;
	}
	varCount += var.size();
	_vars = (char **)malloc(sizeof(char *) * (varCount + 1));
	if (_vars == NULL)
		throw exceptions::cgi::AllocationError();
	bzero(_vars, sizeof(char *) * (varCount + 1));
	_isInitialized = true; // Set the flag to true to dealloc the array in the destructor
	// Copy the envp variables
	for (size_t i = 0; i < envpCount; i++) {
		_vars[i] = strdup(req.envp[i]);
		if (_vars[i] == NULL)
			throw exceptions::cgi::AllocationError();
		_nbVars++;
	}

	// Copy the meta variables
	for (std::map<std::string, std::string>::iterator it = var.begin(); it != var.end(); it++) {
		_vars[_nbVars] = strdup((it->first + "=" + it->second).c_str());
		if (_vars[_nbVars] == NULL)
			throw exceptions::cgi::AllocationError();
		_nbVars++;
	}
	// Add the NULL pointer to the end of the array
	_vars[_nbVars] = NULL;
}

void MetaVariable::setDefaultVars( void )
{
	var[AUTH_TYPE] = DFT_VAL_AUTH_TYPE;
	var[CONTENT_LENGTH] = DFT_VAL_CONT_LEN;
	var[CONTENT_TYPE] = DFT_VAL_CONT_TYPE;
	var[GATEWAY_INTERFACE] = DFT_VAL_CGI;
	var[PATH_INFO] = DFT_VAL_PATH_INFO;
	var[PATH_TRANSLATED] = DFT_VAL_PATH_TRANS;
	var[QUERY_STRING] = DFT_VAL_QUERY_STR;
	var[REDIRECT_STATUS] = DFT_VAL_REDIRECT_STATUS;
	var[REMOTE_ADDR] = DFT_VAL_REMOTE_ADDR;
	var[REMOTE_HOST] = DFT_VAL_REMOTE_HOST;
	var[REMOTE_USER] = DFT_VAL_REMOTE_USER;
	var[REQUEST_METHOD] = DFT_VAL_REQ_METH;
	var[SCRIPT_NAME] = DFT_VAL_SCRIPT_NAME;
	var[SERVER_NAME] = DFT_VAL_SRV_NAME;
	var[SERVER_PORT] = DFT_VAL_SRV_PORT;
	var[SERVER_PROTOCOL] = DFT_VAL_SRV_PROTO;
	var[SERVER_SOFTWARE] = DFT_VAL_SRV_SOFT;
}

void	MetaVariable::setHttpVars( webserv::http::Request const &req)
{
	std::string	key;
	std::string	value;
	std::vector<std::string>	sensitiveKey;
	bool		isSensitiveKey = false;
	
	sensitiveKey.push_back("Authorization");
	sensitiveKey.push_back("Content-Length");
	sensitiveKey.push_back("Content-Type");
	sensitiveKey.push_back("Connection");

	if (req.protocol.find("HTTP") == std::string::npos)
		return ;
	for (std::map<std::string, std::string>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
	{
		key = it->first;
		value = it->second;
		isSensitiveKey = false;
		for (std::vector<std::string>::iterator itSensKey = sensitiveKey.begin(); itSensKey != sensitiveKey.end(); ++itSensKey)
		{
			if (key.compare(*itSensKey) == 0)
			{
				isSensitiveKey = true;
				break ;
			}
		}
		if (isSensitiveKey == true)
			continue ;
		std::transform(key.begin(), key.end(), key.begin(), ::toupper);
		utils::replaceChr(key, HTTP_KEY_SEP, META_KEY_SEP);
		key = META_HTTP_PREFIX + key;
		var[key] = value;
	}
}

// DESTRUCTOR
MetaVariable::~MetaVariable( void )
{
	if (!_isInitialized)
		return ; // Main array allocation failed, no need to free anything

	// Free initialized variables
	for (size_t i = 0; i < _nbVars; i++)
		free(_vars[i]);

	// Free the array
	free(_vars);
}

char** MetaVariable::getVars( void )
{
	// In theory an exception should have been thrown before, it's just a fail-safe
	return (_isInitialized ? _vars : NULL);
}

// OUTSIDE OF THE CLASS
std::ostream	&operator << ( std::ostream &out, MetaVariable const &ref  )
{
	for (std::map<std::string, std::string>::const_iterator it = ref.var.begin(); it != ref.var.end(); ++it)
	{
		out << "[" << it->first << "] : " << it->second << '\n'; // TODO: REMOVE THE \n FOR THE LAST IT
	}
	return (out);
}
