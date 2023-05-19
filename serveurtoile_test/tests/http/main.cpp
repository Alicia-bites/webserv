#include "encoding.hpp"
#include "exceptions.hpp"
#include "Request.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>

#define RED2 "\033[0;31m"
#define GREEN2 "\033[0;32m"
#define MAGENTA2 "\033[0;35m"
#define RESET "\033[0m"
#define DOT "●"

std::vector<int> failedTests;

void printVerdict(bool success) {
    static int testNumber = 0;
    testNumber++;
    std::cout << (success ? GREEN2 : RED2) << DOT << RESET;
    if (!success)
        failedTests.push_back(testNumber);
}

// entities.cpp tests
int tryDecode(std::string str, std::string const &expected, bool throws = false)
{
	static int testNumber = 0;
	testNumber++;
	try {
		std::string decoded = webserv::encoding::decodeURI(str);
		if (throws)
		{
			printVerdict(false);
			return 1;
		}
		printVerdict(decoded == expected);
        return ((decoded == expected) ? 0 : 1);
	} catch (std::exception &e) {
		printVerdict(throws);
        return (throws ? 0 : 1);
	}
}

// mime.cpp
int tryResolveExtension(std::string const &extension, std::string const &expected) {
	std::string mime = webserv::encoding::mimeFromExtension(extension);
	printVerdict(mime == expected);
    return ((mime == expected) ? 0 : 1);
} 

int tryResolvePath(std::string const &path, std::string const &expected) {
	std::string mime = webserv::encoding::mimeFromPath(path);
	printVerdict(mime == expected);
    return ((mime == expected) ? 0 : 1);
}

int tryResolveMimeType(std::string const &mimeType, std::string const &expected) {
	std::string extension = webserv::encoding::extensionFromMime(mimeType);
	printVerdict(extension == expected);
    return ((extension == expected) ? 0 : 1);
}

// request.cpp
std::string getContent(std::string path) {
    std::ifstream f;
    std::string content;

    f.open(path.c_str());
    if (!f.is_open()) {
        return "";
    }

    std::string line;
    while (std::getline(f, line))
        content += line + "\n";

    return content;
}

int tryParse(std::string path, bool throws, int headers_length) {
    std::string content = getContent(path);
    // Create a request, check if it throws an exception
    try {
        webserv::http::Request r(content);
        if (throws)
            return 1;
        return (r.headers.size() == static_cast<size_t>(headers_length)) ? 0 : 1;
    } catch (std::exception &e) {
        return (throws ? 0 : 1);
    }
}

int tryCompare(std::string path, webserv::http::Request &ref) {
    try {
        std::string content = getContent(path);
        webserv::http::Request parsed = webserv::http::Request(content);

        // Compare both requests
        if (parsed.method != ref.method) {
            return 1;
        }
        if (parsed.path != ref.path) {
            return 1;
        }
        if (parsed.protocol != ref.protocol) {
            return 1;
        }
        if (parsed.headers.size() != ref.headers.size()) {
            return 1;
        }
        if (parsed.headers != ref.headers) {
            return 1;
        }
        return 0;
    } catch (std::exception &e) {
        return 1;
    }
}

int main(int argc, char **argv) {
    if (argc == 1)
        return (0);
    int index = std::atoi(argv[1]);
    switch (index) {
        case 1: return tryDecode("%01", "\1");
        case 2: return tryDecode("%20", " ");
        case 3: return tryDecode("%25", "%");
        case 4: return tryDecode("%3A", ":");
        case 5: return tryDecode("%3D", "=");
        case 6: return tryDecode("%3F", "?");
        case 7: return tryDecode("%40", "@");
        case 8: return tryDecode("%5B", "[");
        case 9: return tryDecode("%5D", "]");
        case 10: return tryDecode("%7B", "{");
        case 11: return tryDecode("A%01A", "A\1A");
        case 12: return tryDecode("A%20A", "A A");
        case 13: return tryDecode("A%25A", "A%A");
        case 14: return tryDecode("A%3AA", "A:A");
        case 15: return tryDecode("A%3DA", "A=A");
        case 16: return tryDecode("A%3FA", "A?A");
        case 17: return tryDecode("A%40A", "A@A");
        case 18: return tryDecode("A%5BA", "A[A");
        case 19: return tryDecode("A%5DA", "A]A");
        case 20: return tryDecode("A%7BA", "A{A");
        case 21: return tryDecode("%C3%A9", "é");
        case 22: return tryDecode("%C3%AF", "ï");
        case 23: return tryDecode("%C3%BC", "ü");
        case 24: return tryDecode("%C3%9F", "ß");
        case 25: return tryDecode("%C3%80", "À");
        case 26: return tryDecode("A%C3%A9A", "AéA");
        case 27: return tryDecode("A%C3%AFA", "AïA");
        case 28: return tryDecode("A%C3%BCA", "AüA");
        case 29: return tryDecode("A%C3%9FA", "AßA");
        case 30: return tryDecode("A%C3%80A", "AÀA");
        case 31: return tryDecode("%E2%82%AC", "€");
        case 32: return tryDecode("%E2%80%9C", "“");
        case 33: return tryDecode("%E2%80%9D", "”");
        case 34: return tryDecode("%E2%80%98", "‘");
        case 35: return tryDecode("%E2%80%99", "’");
        case 36: return tryDecode("A%E2%82%ACA", "A€A");
        case 37: return tryDecode("A%E2%80%9CA", "A“A");
        case 38: return tryDecode("A%E2%80%9DA", "A”A");
        case 39: return tryDecode("A%E2%80%98A", "A‘A");
        case 40: return tryDecode("A%E2%80%99A", "A’A");
        case 41: return tryDecode("%F0%9F%98%80", "😀");
        case 42: return tryDecode("%F0%9F%98%81", "😁");
        case 43: return tryDecode("%F0%9F%98%82", "😂");
        case 44: return tryDecode("%F0%9F%98%83", "😃");
        case 45: return tryDecode("%F0%9F%98%84", "😄");
        case 46: return tryDecode("%F0%9F%98%85", "😅");
        case 47: return tryDecode("A%F0%9F%98%80A", "A😀A");
        case 48: return tryDecode("A%F0%9F%98%81A", "A😁A");
        case 49: return tryDecode("A%F0%9F%98%82A", "A😂A");
        case 50: return tryDecode("A%F0%9F%98%83A", "A😃A");
        case 51: return tryDecode("A%F0%9F%98%84A", "A😄A");
        case 52: return tryDecode("A%F0%9F%98%85A", "A😅A");
        case 53: return tryDecode("a", "a");
        case 54: return tryDecode("A", "A");
        case 55: return tryDecode("0", "0");
        case 56: return tryDecode(" ", " ");
        case 57: return tryDecode(":", ":");
        case 58: return tryDecode("=", "=");
        case 59: return tryDecode("?", "?");
        case 60: return tryDecode("@", "@");
        case 61: return tryDecode("é", "é");
        case 62: return tryDecode("ï", "ï");
        case 63: return tryDecode("ü", "ü");
        case 64: return tryDecode("ß", "ß");
        case 65: return tryDecode("À", "À");
        case 66: return tryDecode("€", "€");
        case 67: return tryDecode("“", "“");
        case 68: return tryDecode("”", "”");
        case 69: return tryDecode("‘", "‘");
        case 70: return tryDecode("’", "’");
        case 71: return tryDecode("😀", "😀");
        case 72: return tryDecode("😁", "😁");
        case 73: return tryDecode("😂", "😂");
        case 74: return tryDecode("😃", "😃");
        case 75: return tryDecode("😄", "😄");
        case 76: return tryDecode("aéA€0", "aéA€0");
        case 77: return tryDecode("AaaaBbbbCccc", "AaaaBbbbCccc");
        case 78: return tryDecode("Aaaa😀Bbbb😂Cccc", "Aaaa😀Bbbb😂Cccc");
        case 79: return tryDecode("%", "", true);   // Missing hex digits
        case 80: return tryDecode("%0", "", true);  // Missing 1 hex digit
        case 81: return tryDecode("%0G", "", true); // Invalid base16 string
        case 82: return tryDecode("%G0", "", true); // Invalid base16 string
        case 83: return tryDecode("%GG", "", true); // Invalid base16 string
        case 84: return tryDecode("%0g", "", true); // Invalid base16 string
        case 85: return tryDecode("%g0", "", true); // Invalid base16 string
        case 86: return tryDecode("%gg", "", true); // Invalid base16 string
        case 87: return tryDecode("%g/", "", true); // Invalid base16 string
        case 88: return tryDecode("%0/", "", true); // Invalid base16 string
        case 89: return tryDecode("%/", "", true); // Invalid base16 string
        case 90: return tryResolveExtension("html", "text/html");
        case 91: return tryResolveExtension("htm", "text/html");
        case 92: return tryResolveExtension("css", "text/css");
        case 93: return tryResolveExtension("js", "application/javascript");
        case 94: return tryResolveExtension("json", "application/json");
        case 95: return tryResolveExtension("png", "image/png");
        case 96: return tryResolveExtension("jpg", "image/jpeg");
        case 97: return tryResolveExtension("jpeg", "image/jpeg");
        case 98: return tryResolveExtension("gif", "image/gif");
        case 99: return tryResolveExtension("svg", "image/svg+xml");
        case 100: return tryResolveExtension("ico", "image/x-icon");
        case 101: return tryResolveExtension("txt", "text/plain");
        case 102: return tryResolveExtension("pdf", "application/pdf");
        case 103: return tryResolveExtension(".html", "text/html");
        case 104: return tryResolveExtension(".htm", "text/html");
        case 105: return tryResolveExtension(".css", "text/css");
        case 106: return tryResolveExtension(".js", "application/javascript");
        case 107: return tryResolveExtension(".json", "application/json");
        case 108: return tryResolveExtension(".png", "image/png");
        case 109: return tryResolveExtension(".jpg", "image/jpeg");
        case 110: return tryResolveExtension(".jpeg", "image/jpeg");
        case 111: return tryResolveExtension(".gif", "image/gif");
        case 112: return tryResolveExtension(".svg", "image/svg+xml");
        case 113: return tryResolveExtension(".ico", "image/x-icon");
        case 114: return tryResolveExtension(".txt", "text/plain");
        case 115: return tryResolveExtension(".pdf", "application/pdf");
        case 116: return tryResolveExtension("HTML", "text/html");
        case 117: return tryResolveExtension("HTM", "text/html");
        case 118: return tryResolveExtension("CSS", "text/css");
        case 119: return tryResolveExtension("JS", "application/javascript");
        case 120: return tryResolveExtension("JSON", "application/json");
        case 121: return tryResolveExtension("PNG", "image/png");
        case 122: return tryResolveExtension("JPG", "image/jpeg");
        case 123: return tryResolveExtension("JPEG", "image/jpeg");
        case 124: return tryResolveExtension("GIF", "image/gif");
        case 125: return tryResolveExtension("SVG", "image/svg+xml");
        case 126: return tryResolveExtension("ICO", "image/x-icon");
        case 127: return tryResolveExtension("TXT", "text/plain");
        case 128: return tryResolveExtension("PDF", "application/pdf");
        case 129: return tryResolveExtension(".HTML", "text/html");
        case 130: return tryResolveExtension(".HTM", "text/html");
        case 131: return tryResolveExtension(".CSS", "text/css");
        case 132: return tryResolveExtension(".JS", "application/javascript");
        case 133: return tryResolveExtension(".JSON", "application/json");
        case 134: return tryResolveExtension(".PNG", "image/png");
        case 135: return tryResolveExtension(".JPG", "image/jpeg");
        case 136: return tryResolveExtension(".JPEG", "image/jpeg");
        case 137: return tryResolveExtension(".GIF", "image/gif");
        case 138: return tryResolveExtension(".SVG", "image/svg+xml");
        case 139: return tryResolveExtension(".ICO", "image/x-icon");
        case 140: return tryResolveExtension(".TXT", "text/plain");
        case 141: return tryResolveExtension(".PDF", "application/pdf");
        case 142: return tryResolvePath("index.html", "text/html");
        case 143: return tryResolvePath("index.htm", "text/html");
        case 144: return tryResolvePath("index.css", "text/css");
        case 145: return tryResolvePath("index.js", "application/javascript");
        case 146: return tryResolvePath("index.json", "application/json");
        case 147: return tryResolvePath("index.png", "image/png");
        case 148: return tryResolvePath("index.jpg", "image/jpeg");
        case 149: return tryResolvePath("index.jpeg", "image/jpeg");
        case 150: return tryResolvePath("index.gif", "image/gif");
        case 151: return tryResolvePath("index.svg", "image/svg+xml");
        case 152: return tryResolvePath("index.ico", "image/x-icon");
        case 153: return tryResolvePath("index.txt", "text/plain");
        case 154: return tryResolvePath("index.pdf", "application/pdf");
        case 155: return tryResolvePath("assets/index.html", "text/html");
        case 156: return tryResolvePath("assets/index.htm", "text/html");
        case 157: return tryResolvePath("assets/index.css", "text/css");
        case 158: return tryResolvePath("assets/index.js", "application/javascript");
        case 159: return tryResolvePath("assets/index.json", "application/json");
        case 160: return tryResolvePath("assets/index.png", "image/png");
        case 161: return tryResolvePath("assets/index.jpg", "image/jpeg");
        case 162: return tryResolvePath("assets/index.jpeg", "image/jpeg");
        case 163: return tryResolvePath("assets/index.gif", "image/gif");
        case 164: return tryResolvePath("assets/index.svg", "image/svg+xml");
        case 165: return tryResolvePath("assets/index.ico", "image/x-icon");
        case 166: return tryResolvePath("assets/index.txt", "text/plain");
        case 167: return tryResolvePath("assets/index.pdf", "application/pdf");
        case 168: return tryResolvePath("INDEX.HTML", "text/html");
        case 169: return tryResolvePath("INDEX.HTM", "text/html");
        case 170: return tryResolvePath("INDEX.CSS", "text/css");
        case 171: return tryResolvePath("INDEX.JS", "application/javascript");
        case 172: return tryResolvePath("INDEX.JSON", "application/json");
        case 173: return tryResolvePath("INDEX.PNG", "image/png");
        case 174: return tryResolvePath("INDEX.JPG", "image/jpeg");
        case 175: return tryResolvePath("INDEX.JPEG", "image/jpeg");
        case 176: return tryResolvePath("INDEX.GIF", "image/gif");
        case 177: return tryResolvePath("INDEX.SVG", "image/svg+xml");
        case 178: return tryResolvePath("INDEX.ICO", "image/x-icon");
        case 179: return tryResolvePath("INDEX.TXT", "text/plain");
        case 180: return tryResolvePath("INDEX.PDF", "application/pdf");
        case 181: return tryResolvePath("ASSETS/INDEX.HTML", "text/html");
        case 182: return tryResolvePath("ASSETS/INDEX.HTM", "text/html");
        case 183: return tryResolvePath("ASSETS/INDEX.CSS", "text/css");
        case 184: return tryResolvePath("ASSETS/INDEX.JS", "application/javascript");
        case 185: return tryResolvePath("ASSETS/INDEX.JSON", "application/json");
        case 186: return tryResolvePath("ASSETS/INDEX.PNG", "image/png");
        case 187: return tryResolvePath("ASSETS/INDEX.JPG", "image/jpeg");
        case 188: return tryResolvePath("ASSETS/INDEX.JPEG", "image/jpeg");
        case 189: return tryResolvePath("ASSETS/INDEX.GIF", "image/gif");
        case 190: return tryResolvePath("ASSETS/INDEX.SVG", "image/svg+xml");
        case 191: return tryResolvePath("ASSETS/INDEX.ICO", "image/x-icon");
        case 192: return tryResolvePath("ASSETS/INDEX.TXT", "text/plain");
        case 193: return tryResolvePath("ASSETS/INDEX.PDF", "application/pdf");
        case 194: return tryResolveMimeType("text/html", "html");
        case 195: return tryResolveMimeType("text/css", "css");
        case 196: return tryResolveMimeType("application/javascript", "js");
        case 197: return tryResolveMimeType("application/json", "json");
        case 198: return tryResolveMimeType("image/png", "png");
        case 199: return tryResolveMimeType("image/jpeg", "jpg");
        case 200: return tryResolveMimeType("image/gif", "gif");
        case 201: return tryResolveMimeType("image/svg+xml", "svg");
        case 202: return tryResolveMimeType("image/x-icon", "ico");
        case 203: return tryResolveMimeType("text/plain", "txt");
        case 204: return tryResolveMimeType("application/pdf", "pdf");
        case 205: return tryResolveMimeType("TEXT/HTML", "html");
        case 206: return tryResolveMimeType("TEXT/CSS", "css");
        case 207: return tryResolveMimeType("APPLICATION/JAVASCRIPT", "js");
        case 208: return tryResolveMimeType("APPLICATION/JSON", "json");
        case 209: return tryResolveMimeType("IMAGE/PNG", "png");
        case 210: return tryResolveMimeType("IMAGE/JPEG", "jpg");
        case 211: return tryResolveMimeType("IMAGE/GIF", "gif");
        case 212: return tryResolveMimeType("IMAGE/SVG+xml", "svg");
        case 213: return tryResolveMimeType("IMAGE/X-ICON", "ico");
        case 214: return tryResolveMimeType("TEXT/PLAIN", "txt");
        case 215: return tryResolveMimeType("APPLICATION/PDF", "pdf");
        case 216: return tryResolveExtension("invalidExtension", "application/octet-stream");
        case 217: return tryResolveExtension(".invalidExtension", "application/octet-stream");
        case 218: return tryResolveExtension("invalidExtension.", "application/octet-stream");
        case 219: return tryResolveExtension("INVALIDEXTENSION", "application/octet-stream");
        case 220: return tryResolveExtension(".INVALIDEXTENSION", "application/octet-stream");
        case 221: return tryResolveExtension("INVALIDEXTENSION.", "application/octet-stream");
        case 222: return tryResolvePath("invalidPath/invalidFile", "application/octet-stream");
        case 223: return tryResolvePath("invalidPath/.invalidFile", "application/octet-stream");
        case 224: return tryResolvePath("invalidPath/invalidFile.", "application/octet-stream");
        case 225: return tryResolvePath("INVALIDPATH/INVALIDFILE", "application/octet-stream");
        case 226: return tryResolvePath("INVALIDPATH/.INVALIDFILE", "application/octet-stream");
        case 227: return tryResolvePath("INVALIDPATH/INVALIDFILE.", "application/octet-stream");
        case 228: return tryResolveMimeType("invalid/mime", "bin");
        case 229: return tryResolveMimeType("INVALID/MIME", "bin");
        case 230: return tryParse("data/lf_new_ko_headers_0", true, 3);
        case 231: return tryParse("data/lf_new_ko_headers_1", true, 3);
        case 232: return tryParse("data/lf_new_ko_headers_2", true, 3);
        case 233: return tryParse("data/lf_new_ko_headers_3", true, 0);
        case 234: return tryParse("data/lf_new_ko_headers_4", true, 3);
        case 235: return tryParse("data/lf_ko_headers_0", true, 0);
        case 236: return tryParse("data/lf_ko_headers_1", true, 0);
        case 237: return tryParse("data/crlf_ok_headers_0", false, 3);
        case 238: return tryParse("data/crlf_ok_headers_1", false, 3);
        case 239: return tryParse("data/crlf_ok_headers_2", false, 3);
        case 240: return tryParse("data/crlf_ok_headers_3", false, 0);
        case 241: return tryParse("data/crlf_ok_headers_4", false, 3);
        case 242: return tryParse("data/crlf_ko_headers_0", false, 3);
        case 243: return tryParse("data/crlf_ko_headers_1", true, 0);
        default:
            return (2);
    }
}
