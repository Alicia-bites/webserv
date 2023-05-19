namespace webserv {
	namespace utils {
		unsigned int atoul(const char *s) {
			unsigned int n = 0;
			while (*s) {
				n = n * 10 + (*s - '0');
				s++;
			}
			return n;
		}
	}
}
