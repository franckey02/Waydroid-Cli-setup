// Agregar estas funciones al namespace utils en common.h
namespace utils {
    std::string exec_command(const std::string& cmd) {
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return "";
        
        try {
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
        } catch (...) {
            pclose(pipe);
            return "";
        }
        
        pclose(pipe);
        return result;
    }
    
    bool file_exists(const std::string& path) {
        std::ifstream f(path.c_str());
        return f.good();
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    bool contains(const std::string& str, const std::string& substr) {
        return str.find(substr) != std::string::npos;
    }
}
