#include "encoding.hpp"
#include "exceptions.hpp"
#include "Request.hpp"
#include "configFile.hpp"
#include "server.hpp"
#include "location.hpp"
#include "colors.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

#define DOT "●"

std::vector<int> failedTests;

void printVerdict(bool success) {
    static int testNumber = 0;
    testNumber++;
    std::cout << (success ? GREEN2 : RED2) << DOT << RESET;
    if (!success)
        failedTests.push_back(testNumber);
}

int tryConfig(std::string path, bool fails = false) {
    try {
        webserv::config::ConfigFile config(path);
        return (fails ? 1 : 0);
    } catch (std::exception &e) {
        return (fails ? 0 : 1);
    }
}

// bool compareLocations(const Location &parsed, const Location &ref) {
//     if (parsed.getMethods().size() != ref.getMethods().size())
//         return false;
//     Location::vector_type methods = parsed.getMethods();
//     Location::vector_type::const_iterator it = methods.begin();
//     while (it != parsed.getMethods().end()) {
//         if (std::find(ref.getMethods().begin(), ref.getMethods().end(), *it) == ref.getMethods().end())
//             return false;
//         it++;
//     }
//     if (parsed.getDirectoryIndex() != ref.getDirectoryIndex())
//         return false;
//     if (parsed.getDirectoryIndexWasSet() != ref.getDirectoryIndexWasSet())
//         return false;
//     if (parsed.getIndex() != ref.getIndex())
//         return false;
//     if (parsed.getMaxBodySize() != ref.getMaxBodySize())
//         return false;
//     if (parsed.getMethods() != ref.getMethods())
//         return false;
//     if (parsed.getPath() != ref.getPath())
//         return false;
//     return true;
// }

// bool compareServers(Server &parsed, Server &ref) {
//     if (parsed.getServerName() != ref.getServerName())
//         return false;
//     if (parsed.getRoot() != ref.getRoot())
//         return false;
//     if (parsed.getIndex() != ref.getIndex())
//         return false;
//     if (parsed.getIP() != ref.getIP())
//         return false;
//     if (parsed.getPort() != ref.getPort())
//         return false;
//     if (parsed.getMaxBodySize() != ref.getMaxBodySize())
//         return false;
//     if (parsed.getDirectoryIndex() != ref.getDirectoryIndex())
//         return false;
//     if (parsed.getCgi() != ref.getCgi())
//         return false;
//     if (parsed.getLocations().size() != ref.getLocations().size())
//         return false;
//     Server::map_type::const_iterator it = parsed.getLocations().begin();
//     Server::map_type::const_iterator it2 = ref.getLocations().begin();
//     while (it != parsed.getLocations().end()) {
//         if (ref.getLocations().find(it->first) == ref.getLocations().end())
//             return false;
//         if (!compareLocations(it->second, it2->second))
//             return false;
//         it++;
//         it2++;
//     }
//     return true;
// }

int tryCompare(std::string path, webserv::config::ConfigFile &ref) {
    try {
        webserv::config::ConfigFile config(path);
        if (config.servers.size() != ref.servers.size())
		{
			std::cerr << "config size = " << config.servers.size() << std::endl;
			std::cerr << "ref size = " << ref.servers.size() << std::endl;
			std::cerr << "servers mismatch" << std::endl;
            return (1);
		}

        // Dirty way of comparing
        std::ofstream parsed("tmp_parsed");
        std::ofstream reference("tmp_reference");
        
        // Loop through all servers
        webserv::config::ConfigFile::map_type::iterator it = config.servers.begin();
        webserv::config::ConfigFile::map_type::iterator it2 = ref.servers.begin();
        while (it != config.servers.end() && it2 != ref.servers.end()) {
            parsed << it->second << std::endl;
            reference << it2->second << std::endl;
            it++;
            it2++;
        }
        if (it != config.servers.end() || it2 != ref.servers.end()) {
            return (1);

		}

        parsed.close();
        reference.close();

        // Compare files
        std::ifstream parsedFile("tmp_parsed", std::ios::binary);
        std::ifstream referenceFile("tmp_reference", std::ios::binary);

        std::string parsedLine;
        std::string referenceLine;

        while (std::getline(parsedFile, parsedLine) && std::getline(referenceFile, referenceLine)) {
            if (parsedLine != referenceLine)
            {
                std::cerr << RESET << "--- PARSING ERROR --- " << std::endl;
                std::cerr << RESET << "Got: " << parsedLine << std::endl;
                std::cerr << RESET << "Expected: " << referenceLine << std::endl;
                std::cerr << RESET << "--------------------- " << std::endl;
                parsedFile.close();
                referenceFile.close();
                std::remove("tmp_parsed");
                std::remove("tmp_reference");
				
                return (1);
            }
        }

        parsedFile.close();
        referenceFile.close();

        // delete tmp files
        std::remove("tmp_parsed");
        std::remove("tmp_reference");
        return (0);
    } catch (std::exception &e) {
		std::cout << e.what() << std::endl;
        return (1); // files passed to tryCompare must be valid
    }
}

webserv::config::ConfigFile & getValidMinOrder() {
    static webserv::config::ConfigFile conf;
    static bool initialized = false;
    if (initialized)
        return conf;
    webserv::config::Server server;
    server.setServerName("amarchan.42.fr;");
    server.setRoot("/var/www/wordpress");
    Location::vector_type indexes;
    indexes.push_back("index.php");
    indexes.push_back("index.html");
    indexes.push_back("index.htm");
    server.setIndex(indexes);
    server.setIP("0.0.0.0");
    server.setPort(80);

	// std::vector<std::string> upload_dir;
	// upload_dir.push_back("/upload");
	// upload_dir.push_back(".");
	// server.setUploadDir(upload_dir);

	// Location location;
	// location.setPath(".");
    // location.setRoot("/var/www/wordpress");

	// std::vector<std::string> tmp;
	// tmp.push_back("DELETE");
	// tmp.push_back("POST");
	// location.setMethods(tmp);
	// webserv::config::Server::map_type locations;
	// locations["."] = location;
	// server.setLocations(locations);
    
	conf.servers["amarchan.42.fr;"] = server;
    return conf;
}

webserv::config::ConfigFile & getValidMinRelativeOrder() {
    static webserv::config::ConfigFile conf;
    static bool initialized = false;
    if (initialized)
        return conf;
    webserv::config::Server server;
    server.setServerName("amarchan.42.fr;");
    server.setRoot("./relative_path_dot_preceding");
    Location::vector_type indexes;
    indexes.push_back("index.php");
    indexes.push_back("index.html");
    indexes.push_back("index.htm");
    server.setIndex(indexes);
    server.setIP("0.0.0.0");
    server.setPort(80);

	// std::vector<std::string> upload_dir;
	// upload_dir.push_back("/upload");
	// upload_dir.push_back(".");
	// server.setUploadDir(upload_dir);

	// Location location;
	// location.setPath(".");
	// std::vector<std::string> tmp;
	// tmp.push_back("DELETE");
	// tmp.push_back("POST");
	// location.setMethods(tmp);
    // location.setRoot("./relative_path_dot_preceding");

	// webserv::config::Server::map_type locations;
	// locations["."] = location;
	// server.setLocations(locations);

    conf.servers["amarchan.42.fr;"] = server;
    return conf;
}

webserv::config::ConfigFile & getValidSemiColonRoot() {
    static webserv::config::ConfigFile conf;
    static bool initialized = false;
    if (initialized)
        return conf;
    webserv::config::Server server;
    server.setServerName("amarchan.42.fr;");
    server.setRoot("./semi;colon;in;path/");
    Location::vector_type indexes;
    indexes.push_back("index.php");
    indexes.push_back("index.html");
    indexes.push_back("index.htm");
    server.setIndex(indexes);
    server.setIP("0.0.0.0");
    server.setPort(80);
	
	// std::vector<std::string> upload_dir;
	// upload_dir.push_back("/upload");
	// upload_dir.push_back(".");
	// server.setUploadDir(upload_dir);


	// Location location;
	// location.setPath(".");
	// std::vector<std::string> tmp;
	// tmp.push_back("DELETE");
	// tmp.push_back("POST");
	// location.setMethods(tmp);
    // location.setRoot("./semi;colon;in;path/");

	// webserv::config::Server::map_type locations;
	// locations["."] = location;
	// server.setLocations(locations);
	
    conf.servers["amarchan.42.fr;"] = server;
    return conf;
}

webserv::config::ConfigFile & getValid2ServerBlocks() {
    static webserv::config::ConfigFile conf;
    static bool initialized = false;
    if (initialized)
        return conf;
    webserv::config::Server server;
    server.setServerName("amarchan.42.fr;");
    server.setRoot("/var/www/wordpress");
    Location::vector_type indexes;
    indexes.push_back("index.php");
    indexes.push_back("index.html");
    indexes.push_back("index.htm");
    server.setIndex(indexes);
    server.setIP("0.0.0.0");
    server.setPort(80);

	// std::vector<std::string> upload_dir;
	// upload_dir.push_back("/upload");
	// upload_dir.push_back(".");
	// server.setUploadDir(upload_dir);


	// Location location2;
	// location2.setPath(".");
    // location2.setRoot("/var/www/wordpress");

	// std::vector<std::string> tmp;
	// tmp.push_back("DELETE");
	// tmp.push_back("POST");
	// location2.setMethods(tmp);
	// webserv::config::Server::map_type location2s;
	// location2s["."] = location2;
	// server.setLocations(location2s);
	

    conf.servers["amarchan.42.fr;"] = server;

	indexes.clear();

	// webserv::config::Server server;
    server.setServerName("pinkFloyd;");
    server.setRoot("/var/www/maggie");
    indexes.push_back("what.php");
    indexes.push_back("have.html");
    indexes.push_back("we.htm");
    indexes.push_back("done.htm");

    server.setIndex(indexes);
    server.setIP("0.94.5.20");
    server.setPort(8080);

	// std::vector<std::string> upload_dir2;
	// upload_dir2.push_back("/upload");
	// upload_dir2.push_back(".");
	// server.setUploadDir(upload_dir2);


	// Location location;
	// location.setPath(".");
    // location.setRoot("/var/www/maggie");

	// std::vector<std::string> tmp1;
	// tmp1.push_back("DELETE");
	// tmp1.push_back("POST");
	// location.setMethods(tmp1);
	// location2s["."] = location;
	// server.setLocations(location2s);
	

    conf.servers["pinkFloyd;"] = server;
    return conf;
}

webserv::config::ConfigFile & getValid2ServerBlocksLocation() {
    static webserv::config::ConfigFile conf;
    static bool initialized = false;
    if (initialized)
        return conf;

    webserv::config::Server server;
    server.setServerName("amarchan.42.fr;");
    server.setRoot("/var/www/wordpress");

    Location::vector_type indexes;
    indexes.push_back("index.php");
    indexes.push_back("index.html");
    indexes.push_back("index.htm");
    server.setIndex(indexes);

    server.setIP("0.0.0.0");
    server.setPort(80);

	Location location;
	location.setPath("/toto/tata");
	location.setRoot("/var/www/wordpress");
	std::vector<std::string> tmp;
	tmp.push_back("GET");
	tmp.push_back("POST");
	location.setMethods(tmp);

	Location::vector_type location_indexes;
	location_indexes.push_back("YOUPLABOOM.html");
	location_indexes.push_back("FABULOSO.php");
	location.setIndex(location_indexes);

	webserv::config::Server::map_type locations;
	locations["/toto/tata"] = location;
	// server.setLocations(locations);

	// std::vector<std::string> upload_dir;
	// upload_dir.push_back("/upload");
	// upload_dir.push_back(".");
	// server.setUploadDir(upload_dir);


	// Location location1;
	// location1.setPath(".");
	// std::vector<std::string> tmp2;
	// tmp2.push_back("DELETE");
	// tmp2.push_back("POST");
	// location1.setMethods(tmp2);
	// location1.setRoot("/var/www/wordpress");
	// locations["."] = location1;
	server.setLocations(locations);
	
	// std::cout << "root = " << 
    conf.servers["amarchan.42.fr;"] = server;

	// indexes.clear();
	tmp.clear();
	locations.clear();

// server # 2 ---------------------------------------------------------

	Location::vector_type indexes_2;
    server.setServerName("pinkFloyd;");
    server.setRoot("/var/www/maggie");
    indexes_2.push_back("what.php");
    indexes_2.push_back("have.html");
    indexes_2.push_back("we.htm");
    indexes_2.push_back("done.htm");
    server.setIndex(indexes_2);	
    server.setIP("0.94.5.20");
    server.setPort(8080);
	
	location.setPath("/secret");
	tmp.push_back("POST");
	location.setMethods(tmp);
	location.setDirectoryIndex(false);
	location.setIndex(indexes_2);
	location.setRoot("/var/www/maggie");
	locations["/secret"] = location;
	server.setLocations(locations);

	// std::vector<std::string> upload_dir2;
	// upload_dir2.push_back("/upload");
	// upload_dir2.push_back(".");
	// server.setUploadDir(upload_dir2);


	// Location location2;
	// location2.setPath(".");
	// std::vector<std::string> tmp1;
	// tmp1.push_back("DELETE");
	// tmp1.push_back("POST");
	// location2.setMethods(tmp1);
	// location2.setRoot("/var/www/maggie");
	// locations["."] = location2;
	server.setLocations(locations);
	
	conf.servers["pinkFloyd;"] = server;
	// std::cout << "root = " << conf.servers["pinkFloyd;"].getRoot() << std::endl;
	// std::cout << "root = " << conf.servers["amarchan.42.fr;"].getRoot() << std::endl;

	
	// for(webserv::config::ConfigFile::map_type::iterator it = conf.servers.begin(); it != conf.servers.end(); it++)
	// {
	// 	std::cout << SALMON1 << ">>>> Printing server with key : " << it->first << std::endl;
	// 	std::cout << it->second << std::endl;
	// 	std::cout << "-------------------------------------" << std::endl;
	// }
    return conf;
}

webserv::config::ConfigFile & getValidCheckLocationOverride_0() {
	static webserv::config::ConfigFile conf;
    static bool initialized = false;
    if (initialized)
        return conf;
    webserv::config::Server server;
    server.setServerName("amarchan.42.fr;");
    server.setRoot("/var/www/wordpress");

    server.setIP("0.0.0.0");
    server.setPort(80);

	Location location;
	location.setPath("/");

	std::vector<std::string> tmp;
	tmp.push_back("GET");
	tmp.push_back("POST");
	tmp.push_back("DELETE");
	location.setMethods(tmp);
    location.setRoot("/var/www/wordpress");

	Location::vector_type location_indexes;
	location_indexes.push_back("bugs.html");
	location_indexes.push_back("bunny.php");
	location.setIndex(location_indexes);

	webserv::config::Server::map_type locations;
	locations["/"] = location;
	server.setLocations(locations);

	// std::vector<std::string> upload_dir;
	// upload_dir.push_back("/upload");
	// upload_dir.push_back(".");
	// server.setUploadDir(upload_dir);


	// Location location1;
	// location1.setPath(".");
	// std::vector<std::string> tmp1;
	// tmp1.push_back("DELETE");
	// tmp1.push_back("POST");
	// location1.setMethods(tmp1);
    // location1.setRoot("/var/www/wordpress");

	// locations["."] = location1;
	// server.setLocations(locations);
	

    conf.servers["amarchan.42.fr;"] = server;
	
    return conf;
}

webserv::config::ConfigFile & getValidCheckLocationOverride_1() {
	static webserv::config::ConfigFile conf;
    static bool initialized = false;
    if (initialized)
        return conf;
    webserv::config::Server server;
    server.setServerName("amarchan.42.fr;");
    server.setRoot("/var/www/wordpress");

    server.setIP("0.0.0.0");
    server.setPort(80);

	Location location;
	location.setPath("/");
    location.setRoot("/var/www/wordpress");

	std::vector<std::string> tmp;
	tmp.push_back("GET");
	tmp.push_back("POST");
	tmp.push_back("DELETE");
	location.setMethods(tmp);

	Location::vector_type location_indexes;
	location_indexes.push_back("bugs.html");
	location_indexes.push_back("bunny.php");
	location.setIndex(location_indexes);

	webserv::config::Server::map_type locations;
	locations["/"] = location;
	server.setLocations(locations);

	// std::vector<std::string> upload_dir;
	// upload_dir.push_back("/upload");
	// upload_dir.push_back(".");
	// server.setUploadDir(upload_dir);


	// Location location1;
	// location1.setPath(".");
    // location1.setRoot("/var/www/wordpress");

	// std::vector<std::string> tmp1;
	// tmp1.push_back("DELETE");
	// tmp1.push_back("POST");
	// location1.setMethods(tmp1);
	// locations["."] = location1;
	server.setLocations(locations);
	

    conf.servers["amarchan.42.fr;"] = server;
	
    return conf;
}

webserv::config::ConfigFile & getValidCheckLocationOverride_2() {
	static webserv::config::ConfigFile conf;
    static bool initialized = false;
    if (initialized)
        return conf;
    webserv::config::Server server;
    server.setServerName("amarchan.42.fr;");
    server.setRoot("/var/www/wordpress");

    server.setIP("0.0.0.0");
    server.setPort(80);

	Location location;
	location.setPath("/");
    location.setRoot("/var/www/wordpress");

	std::vector<std::string> tmp;
	tmp.push_back("POST");
	location.setMethods(tmp);

	Location::vector_type location_indexes;
	location_indexes.push_back("bugs.html");
	location_indexes.push_back("bunny.php");
	location.setIndex(location_indexes);

	webserv::config::Server::map_type locations;
	locations["/"] = location;
	server.setLocations(locations);

	// std::vector<std::string> upload_dir;
	// upload_dir.push_back("/upload");
	// upload_dir.push_back(".");
	// server.setUploadDir(upload_dir);

	// Location location2;
	// location2.setPath(".");
    // location2.setRoot("/var/www/wordpress");

	// std::vector<std::string> tmp1;
	// tmp1.push_back("DELETE");
	// tmp1.push_back("POST");
	// location2.setMethods(tmp1);
	// locations["."] = location2;
	// server.setLocations(locations);
	

    conf.servers["amarchan.42.fr;"] = server;
	
    return conf;
}


int main(int argc, char **argv) {
    // webserv::config::ConfigFile ref("test_files/valid_relative_root");
    // return (0);
    if (argc == 1)
        return (0);
    int index = std::atoi(argv[1]);
    switch (index) {
        case 1: return tryConfig(argv[0], true);
        case 2: return tryConfig(".", true);
        case 3: return tryConfig("", true);
        case 4: return tryConfig("\00", true);
        case 5: return tryConfig("test_file", true);
        case 6: return tryConfig("/dev/null", true);
        case 7: return tryConfig("test_files/legit", false);
        case 8: return tryConfig("test_files/error_duplicate_cgi", true);
        case 9: return tryConfig("test_files/error_too_many_max_body_size_entries", true);
        case 10: return tryConfig("test_files/error_invalid_IP_address", true);
        case 11: return tryConfig("test_files/error_max_body_size_invalid", true);
        case 12: return tryConfig("test_files/error_too_many_max_body_size_per_location", true);
        case 13: return tryConfig("test_files/error_no_root", true);
        case 14: return tryConfig("test_files/error_empty", true);
        case 15: return tryConfig("test_files/error_too_many_server_name_entries", true);
        case 16: return tryConfig("test_files/error_too_many_index_entries", true);
        case 17: return tryConfig("test_files/error_invalid_port", true);
        case 18: return tryConfig("test_files/error_too_many_max_body_size", true);
        case 19: return tryConfig("test_files/error_indent_level", true);
        case 20: return tryConfig("test_files/error_noRightToRead", true);
        case 21: return tryConfig("test_files/error_too_many_cgi_ext_entries", true);
        case 22: return tryConfig("test_files/error_no_ip_address", true);
        case 23: return tryConfig("test_files/error_too_many_directory_index_entries", true);
        case 24: return tryConfig("test_files/error_too_many_listen_entries", true);
        case 25: return tryConfig("test_files/error_server_name", true);
        case 26: return tryConfig("test_files/error_indent_level2", true);
        case 27: return tryConfig("test_files/error_too_many_root", true);
        case 28: return tryConfig("test_files/error_invalid_method", true);
        case 29: return tryConfig("test_files/crash_new_lines", false);
        case 30: return tryConfig("test_files/error_negative_port", true);
        case 31: return tryConfig("test_files/error_duplicate_index", true);
        case 32: return tryConfig("test_files/error_duplicate_location_method", true);
        case 33: return tryConfig("test_files/crash_weird", true);
        case 34: return tryConfig("test_files/error_duplicate_server", true);
        case 35: return tryConfig("test_files/error_port_overflow_negative", true);
        case 36: return tryConfig("test_files/valid_min_order_0", false);
        case 37: return tryConfig("test_files/valid_min_order_1", false);
        case 38: return tryConfig("test_files/valid_min_order_2", false);
        case 39: return tryConfig("test_files/valid_min_order_3", false);
        case 40: return tryConfig("test_files/valid_min_order_4", false);
        case 41: return tryConfig("test_files/valid_min_order_5", false);
        case 42: return tryConfig("test_files/valid_min_order_6", false);
        case 43: return tryConfig("test_files/valid_min_order_7", false);
        case 44: return tryConfig("test_files/valid_min_order_8", false);
        case 45: return tryConfig("test_files/valid_min_order_9", false);
        case 46: return tryConfig("test_files/valid_min_order_10", false);
        case 47: return tryConfig("test_files/valid_min_order_11", false);
        case 48: return tryConfig("test_files/valid_min_order_12", false);
        case 49: return tryConfig("test_files/valid_min_order_13", false);
        case 50: return tryConfig("test_files/valid_min_order_14", false);
        case 51: return tryConfig("test_files/valid_min_order_15", false);
        case 52: return tryConfig("test_files/valid_min_order_16", false);
        case 53: return tryConfig("test_files/valid_min_order_17", false);
        case 54: return tryConfig("test_files/valid_min_order_18", false);
        case 55: return tryConfig("test_files/valid_min_order_19", false);
        case 56: return tryConfig("test_files/valid_min_order_20", false);
        case 57: return tryConfig("test_files/valid_min_order_21", false);
        case 58: return tryConfig("test_files/valid_min_order_22", false);
        case 59: return tryConfig("test_files/valid_min_order_23", false);
        case 60: return tryConfig("test_files/valid_min_relative_order_0", false);
        case 61: return tryConfig("test_files/valid_min_relative_order_1", false);
        case 62: return tryConfig("test_files/valid_min_relative_order_2", false);
        case 63: return tryConfig("test_files/valid_min_relative_order_3", false);
        case 64: return tryConfig("test_files/valid_min_relative_order_4", false);
        case 65: return tryConfig("test_files/valid_min_relative_order_5", false);
        case 66: return tryConfig("test_files/valid_min_relative_order_6", false);
        case 67: return tryConfig("test_files/valid_min_relative_order_7", false);
        case 68: return tryConfig("test_files/valid_min_relative_order_8", false);
        case 69: return tryConfig("test_files/valid_min_relative_order_9", false);
        case 70: return tryConfig("test_files/valid_min_relative_order_10", false);
        case 71: return tryConfig("test_files/valid_min_relative_order_11", false);
        case 72: return tryConfig("test_files/valid_min_relative_order_12", false);
        case 73: return tryConfig("test_files/valid_min_relative_order_13", false);
        case 74: return tryConfig("test_files/valid_min_relative_order_14", false);
        case 75: return tryConfig("test_files/valid_min_relative_order_15", false);
        case 76: return tryConfig("test_files/valid_min_relative_order_16", false);
        case 77: return tryConfig("test_files/valid_min_relative_order_17", false);
        case 78: return tryConfig("test_files/valid_min_relative_order_18", false);
        case 79: return tryConfig("test_files/valid_min_relative_order_19", false);
        case 80: return tryConfig("test_files/valid_min_relative_order_20", false);
        case 81: return tryConfig("test_files/valid_min_relative_order_21", false);
        case 82: return tryConfig("test_files/valid_min_relative_order_22", false);
        case 83: return tryConfig("test_files/valid_min_relative_order_23", false);
        case 84: return tryConfig("test_files/valid_semicolon_root_0", false);
        case 85: return tryConfig("test_files/valid_semicolon_root_1", false);
        case 86: return tryConfig("test_files/valid_semicolon_root_2", false);
        case 87: return tryConfig("test_files/valid_semicolon_root_3", false);
        case 88: return tryConfig("test_files/valid_semicolon_root_4", false);
        case 89: return tryConfig("test_files/valid_semicolon_root_5", false);
        case 90: return tryConfig("test_files/valid_semicolon_root_6", false);
        case 91: return tryConfig("test_files/valid_semicolon_root_7", false);
        case 92: return tryConfig("test_files/valid_semicolon_root_8", false);
        case 93: return tryConfig("test_files/valid_semicolon_root_9", false);
        case 94: return tryConfig("test_files/valid_semicolon_root_10", false);
        case 95: return tryConfig("test_files/valid_semicolon_root_11", false);
        case 96: return tryConfig("test_files/valid_semicolon_root_12", false);
        case 97: return tryConfig("test_files/valid_semicolon_root_13", false);
        case 98: return tryConfig("test_files/valid_semicolon_root_14", false);
        case 99: return tryConfig("test_files/valid_semicolon_root_15", false);
        case 100: return tryConfig("test_files/valid_semicolon_root_16", false);
        case 101: return tryConfig("test_files/valid_semicolon_root_17", false);
        case 102: return tryConfig("test_files/valid_semicolon_root_18", false);
        case 103: return tryConfig("test_files/valid_semicolon_root_19", false);
        case 104: return tryConfig("test_files/valid_semicolon_root_20", false);
        case 105: return tryConfig("test_files/valid_semicolon_root_21", false);
        case 106: return tryConfig("test_files/valid_semicolon_root_22", false);
        case 107: return tryConfig("test_files/valid_semicolon_root_23", false);
        case 108: return tryConfig("test_files/valid_semicolon_index", false);

        // compare tests
        case 109: return tryCompare("test_files/valid_min_order_0", getValidMinOrder());
        case 110: return tryCompare("test_files/valid_min_order_1", getValidMinOrder());
        case 111: return tryCompare("test_files/valid_min_order_2", getValidMinOrder());
        case 112: return tryCompare("test_files/valid_min_order_3", getValidMinOrder());
        case 113: return tryCompare("test_files/valid_min_order_4", getValidMinOrder());
        case 114: return tryCompare("test_files/valid_min_order_5", getValidMinOrder());
        case 115: return tryCompare("test_files/valid_min_order_6", getValidMinOrder());
        case 116: return tryCompare("test_files/valid_min_order_7", getValidMinOrder());
        case 117: return tryCompare("test_files/valid_min_order_8", getValidMinOrder());
        case 118: return tryCompare("test_files/valid_min_order_9", getValidMinOrder());
        case 119: return tryCompare("test_files/valid_min_order_10", getValidMinOrder());
        case 120: return tryCompare("test_files/valid_min_order_11", getValidMinOrder());
        case 121: return tryCompare("test_files/valid_min_order_12", getValidMinOrder());
        case 122: return tryCompare("test_files/valid_min_order_13", getValidMinOrder());
        case 123: return tryCompare("test_files/valid_min_order_14", getValidMinOrder());
        case 124: return tryCompare("test_files/valid_min_order_15", getValidMinOrder());
        case 125: return tryCompare("test_files/valid_min_order_16", getValidMinOrder());
        case 126: return tryCompare("test_files/valid_min_order_17", getValidMinOrder());
        case 127: return tryCompare("test_files/valid_min_order_18", getValidMinOrder());
        case 128: return tryCompare("test_files/valid_min_order_19", getValidMinOrder());
        case 129: return tryCompare("test_files/valid_min_order_20", getValidMinOrder());
        case 130: return tryCompare("test_files/valid_min_order_21", getValidMinOrder());
        case 131: return tryCompare("test_files/valid_min_order_22", getValidMinOrder());
        case 132: return tryCompare("test_files/valid_min_order_23", getValidMinOrder());

        case 133: return tryCompare("test_files/valid_min_relative_order_0", getValidMinRelativeOrder());
        case 134: return tryCompare("test_files/valid_min_relative_order_1", getValidMinRelativeOrder());
        case 135: return tryCompare("test_files/valid_min_relative_order_2", getValidMinRelativeOrder());
        case 136: return tryCompare("test_files/valid_min_relative_order_3", getValidMinRelativeOrder());
        case 137: return tryCompare("test_files/valid_min_relative_order_4", getValidMinRelativeOrder());
        case 138: return tryCompare("test_files/valid_min_relative_order_5", getValidMinRelativeOrder());
        case 139: return tryCompare("test_files/valid_min_relative_order_6", getValidMinRelativeOrder());
        case 140: return tryCompare("test_files/valid_min_relative_order_7", getValidMinRelativeOrder());
        case 141: return tryCompare("test_files/valid_min_relative_order_8", getValidMinRelativeOrder());
        case 142: return tryCompare("test_files/valid_min_relative_order_9", getValidMinRelativeOrder());
        case 143: return tryCompare("test_files/valid_min_relative_order_10", getValidMinRelativeOrder());
        case 144: return tryCompare("test_files/valid_min_relative_order_11", getValidMinRelativeOrder());
        case 145: return tryCompare("test_files/valid_min_relative_order_12", getValidMinRelativeOrder());
        case 146: return tryCompare("test_files/valid_min_relative_order_13", getValidMinRelativeOrder());
        case 147: return tryCompare("test_files/valid_min_relative_order_14", getValidMinRelativeOrder());
        case 148: return tryCompare("test_files/valid_min_relative_order_15", getValidMinRelativeOrder());
        case 149: return tryCompare("test_files/valid_min_relative_order_16", getValidMinRelativeOrder());
        case 150: return tryCompare("test_files/valid_min_relative_order_17", getValidMinRelativeOrder());
        case 151: return tryCompare("test_files/valid_min_relative_order_18", getValidMinRelativeOrder());
        case 152: return tryCompare("test_files/valid_min_relative_order_19", getValidMinRelativeOrder());
        case 153: return tryCompare("test_files/valid_min_relative_order_20", getValidMinRelativeOrder());
        case 154: return tryCompare("test_files/valid_min_relative_order_21", getValidMinRelativeOrder());
        case 155: return tryCompare("test_files/valid_min_relative_order_22", getValidMinRelativeOrder());
        case 156: return tryCompare("test_files/valid_min_relative_order_23", getValidMinRelativeOrder());

        case 157:
        {
            webserv::config::ConfigFile conf;
            webserv::config::Server server;
            server.setServerName("amarchan.42.fr;");
            server.setRoot("relative/path/to/your/frontend");
            Location::vector_type indexes;
            indexes.push_back("in;dex.php");
            indexes.push_back("ind;ex.html");
            indexes.push_back("i;ndex.htm");
            server.setIndex(indexes);
            server.setIP("0.0.0.0");
            server.setPort(80);

			// std::vector<std::string> upload_dir;
			// upload_dir.push_back("/upload");
			// upload_dir.push_back(".");
			// server.setUploadDir(upload_dir);
		
			// Location location;
			// location.setPath(".");
            // location.setRoot("relative/path/to/your/frontend");

			// std::vector<std::string> tmp;
			// tmp.push_back("DELETE");
			// tmp.push_back("POST");
			// location.setMethods(tmp);
			// webserv::config::Server::map_type locations;
			// locations["."] = location;
			// server.setLocations(locations);

            conf.servers["amarchan.42.fr;"] = server;
            return tryCompare("test_files/valid_semicolon_index", conf);
        }

        case 158: return tryCompare("test_files/valid_semicolon_root_0", getValidSemiColonRoot());
        case 159: return tryCompare("test_files/valid_semicolon_root_1", getValidSemiColonRoot());
        case 160: return tryCompare("test_files/valid_semicolon_root_2", getValidSemiColonRoot());
        case 161: return tryCompare("test_files/valid_semicolon_root_3", getValidSemiColonRoot());
        case 162: return tryCompare("test_files/valid_semicolon_root_4", getValidSemiColonRoot());
        case 163: return tryCompare("test_files/valid_semicolon_root_5", getValidSemiColonRoot());
        case 164: return tryCompare("test_files/valid_semicolon_root_6", getValidSemiColonRoot());
        case 165: return tryCompare("test_files/valid_semicolon_root_7", getValidSemiColonRoot());
        case 166: return tryCompare("test_files/valid_semicolon_root_8", getValidSemiColonRoot());
        case 167: return tryCompare("test_files/valid_semicolon_root_9", getValidSemiColonRoot());
        case 168: return tryCompare("test_files/valid_semicolon_root_10", getValidSemiColonRoot());
        case 169: return tryCompare("test_files/valid_semicolon_root_11", getValidSemiColonRoot());
        case 170: return tryCompare("test_files/valid_semicolon_root_12", getValidSemiColonRoot());
        case 171: return tryCompare("test_files/valid_semicolon_root_13", getValidSemiColonRoot());
        case 172: return tryCompare("test_files/valid_semicolon_root_14", getValidSemiColonRoot());
        case 173: return tryCompare("test_files/valid_semicolon_root_15", getValidSemiColonRoot());
        case 174: return tryCompare("test_files/valid_semicolon_root_16", getValidSemiColonRoot());
        case 175: return tryCompare("test_files/valid_semicolon_root_17", getValidSemiColonRoot());
        case 176: return tryCompare("test_files/valid_semicolon_root_18", getValidSemiColonRoot());
        case 177: return tryCompare("test_files/valid_semicolon_root_19", getValidSemiColonRoot());
        case 178: return tryCompare("test_files/valid_semicolon_root_20", getValidSemiColonRoot());
        case 179: return tryCompare("test_files/valid_semicolon_root_21", getValidSemiColonRoot());
        case 180: return tryCompare("test_files/valid_semicolon_root_22", getValidSemiColonRoot());
        case 181: return tryCompare("test_files/valid_semicolon_root_23", getValidSemiColonRoot());
		case 182: return tryCompare("test_files/valid_2_server_blocks_0", getValid2ServerBlocks());
		case 183: return tryCompare("test_files/valid_2_server_blocks_1", getValid2ServerBlocks());
		case 184: return tryCompare("test_files/valid_2_server_blocks_location_0", getValid2ServerBlocksLocation());
		case 185: return tryCompare("test_files/valid_2_server_blocks_location_1", getValid2ServerBlocksLocation());
		case 186: return tryCompare("test_files/valid_2_server_blocks_location_2", getValid2ServerBlocksLocation());
		case 187: return tryCompare("test_files/valid_2_server_blocks_location_3", getValid2ServerBlocksLocation());
		case 188: return tryCompare("test_files/valid_2_server_blocks_location_4", getValid2ServerBlocksLocation());
		case 189: return tryCompare("test_files/valid_2_server_blocks_location_5", getValid2ServerBlocksLocation());
		
		// back to tryconfig
		// case 190: return tryConfig("test_files/error_too_many_upload_dir_location", true);
		// case 191: return tryConfig("test_files/error_too_many_upload_dir_server", true);
		case 192: return tryConfig("test_files/error_duplicate_error_page", true);
		case 193: return tryConfig("test_files/error_too_many_redirect", false);
		case 194: return tryConfig("test_files/error_no_listen", true);
		case 195: return tryConfig("test_files/valid_no_server_name", false);
		// case 196: return tryConfig("test_files/error_too_many_upload_dir_server", true);
		// case 197: return tryConfig("test_files/error_too_many_upload_dir_server", true);
		// case 198: return tryConfig("test_files/error_too_many_upload_dir_server", true);
		case 199: return tryConfig("test_files/error_duplicate_listen", true);
		case 200: return tryConfig("test_files/error_duplicate_server_name", true);
		case 201: return tryConfig("test_files/error_indent_level_2", true);
		case 202: return tryConfig("test_files/error_indent_level_1", true);
		case 203: return tryConfig("test_files/error_too_many_max_body_size_1", true);
		case 204: return tryConfig("test_files/error_unknown_property_1", true);
		case 205: return tryConfig("test_files/error_too_many_max_body_size_0", true);
		case 206: return tryConfig("test_files/error_empty_location_block", true);

		// back to compare tests
		case 207: return tryCompare("test_files/check_location_override_0", getValidCheckLocationOverride_0());
		case 208: return tryCompare("test_files/check_location_override_1", getValidCheckLocationOverride_1());
		case 209: return tryCompare("test_files/check_location_override_2", getValidCheckLocationOverride_2());

		// back to tryconfif
		case 210: return tryConfig("test_files/error_timeout_0", true);
		case 211: return tryConfig("test_files/error_timeout_1", true);
		// case 212: return tryConfig("test_files/error_upload_dir_0", true);
		// case 213: return tryConfig("test_files/error_upload_dir_1", true);
		// case 214: return tryConfig("test_files/error_upload_dir_2", true);



        default: return (2);
    }
}
