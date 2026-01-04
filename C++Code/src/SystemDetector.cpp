#include "SystemDetector.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/utsname.h>

using namespace std;

SystemDetector::SystemDetector() {}

SystemInfo SystemDetector::detect() {
    system_info.distribution = detect_distribution();
    system_info.package_manager = detect_package_manager();
    system_info.architecture = detect_architecture();
    system_info.session_type = detect_session();
    system_info.is_root = check_root();
    
    // Detectar versión
    if (system_info.distribution == Distro::VOID) {
        if (utils::file_exists("/etc/void-release")) {
            string content = utils::exec_command("cat /etc/void-release");
            auto parts = utils::split(content, ' ');
            if (parts.size() > 1) {
                system_info.version = parts[1];
            }
        }
    } else {
        // Para otras distribuciones, intentar desde os-release
        if (utils::file_exists("/etc/os-release")) {
            ifstream file("/etc/os-release");
            string line;
            while (getline(file, line)) {
                if (line.find("VERSION_ID=") == 0) {
                    system_info.version = line.substr(11);
                    // Eliminar comillas
                    system_info.version.erase(
                        remove(system_info.version.begin(), 
                               system_info.version.end(), '"'),
                        system_info.version.end()
                    );
                    break;
                }
            }
        }
    }
    
    return system_info;
}

Distro SystemDetector::detect_distribution() {
    Distro distro = detect_from_os_release();
    if (distro == Distro::UNKNOWN) {
        distro = detect_from_etc_release();
    }
    return distro;
}

Distro SystemDetector::detect_from_os_release() {
    if (!utils::file_exists("/etc/os-release")) {
        return Distro::UNKNOWN;
    }
    
    ifstream file("/etc/os-release");
    string line;
    string id;
    
    while (getline(file, line)) {
        if (line.find("ID=") == 0) {
            id = line.substr(3);
            // Eliminar comillas
            id.erase(remove(id.begin(), id.end(), '"'), id.end());
            
            // Mapear a enum Distro
            if (id == "arch" || id == "manjaro" || id == "endeavouros") {
                return Distro::ARCH;
            } else if (id == "debian") {
                return Distro::DEBIAN;
            } else if (id == "ubuntu" || id == "pop" || id == "linuxmint" || 
                       id == "elementary" || id == "zorin") {
                return Distro::UBUNTU;
            } else if (id == "fedora" || id == "rhel" || id == "centos") {
                return Distro::FEDORA;
            } else if (id == "opensuse-tumbleweed" || id == "opensuse-leap") {
                return Distro::OPENSUSE;
            } else if (id == "nixos") {
                return Distro::NIXOS;
            } else if (id == "void") {
                return Distro::VOID;
            }
        }
    }
    
    return Distro::UNKNOWN;
}

Distro SystemDetector::detect_from_etc_release() {
    if (utils::file_exists("/etc/void-release")) {
        return Distro::VOID;
    } else if (utils::file_exists("/etc/arch-release")) {
        return Distro::ARCH;
    } else if (utils::file_exists("/etc/debian_version")) {
        return Distro::DEBIAN;
    } else if (utils::file_exists("/etc/fedora-release")) {
        return Distro::FEDORA;
    }
    
    return Distro::UNKNOWN;
}

PackageManager SystemDetector::detect_package_manager() {
    return detect_pm_by_command();
}

PackageManager SystemDetector::detect_pm_by_command() {
    // Verificar comandos en orden de preferencia
    vector<pair<string, PackageManager>> managers = {
        {"apt", PackageManager::APT},
        {"pacman", PackageManager::PACMAN},
        {"dnf", PackageManager::DNF},
        {"yum", PackageManager::YUM},
        {"zypper", PackageManager::ZYPPER},
        {"xbps-install", PackageManager::XBPS},
        {"nix-env", PackageManager::NIX}
    };
    
    for (const auto& [cmd, pm] : managers) {
        string test_cmd = "command -v " + cmd + " >/dev/null 2>&1";
        if (system(test_cmd.c_str()) == 0) {
            return pm;
        }
    }
    
    return PackageManager::UNKNOWN;
}

string SystemDetector::detect_architecture() {
    struct utsname buf;
    if (uname(&buf) == 0) {
        return string(buf.machine);
    }
    return "unknown";
}

string SystemDetector::detect_session() {
    char* session = getenv("XDG_SESSION_TYPE");
    if (session != nullptr) {
        return string(session);
    }
    
    char* wayland = getenv("WAYLAND_DISPLAY");
    if (wayland != nullptr) {
        return "wayland";
    }
    
    char* display = getenv("DISPLAY");
    if (display != nullptr) {
        return "x11";
    }
    
    return "unknown";
}

bool SystemDetector::check_root() {
    return geteuid() == 0;
}
