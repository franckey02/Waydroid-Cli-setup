#include <iostream>
#include <string>
#include "SystemDetector.h"
#include "Installer.h"
#include "Uninstaller.h"

using namespace std;

void show_help() {
    cout << "Waydroid C++ Installer - Multi-distro CLI installer\n";
    cout << "Copyright (c) 2024, javierc\n";
    cout << "License: BSD 3-Clause\n\n";
    cout << "Usage: waydroid-installer [OPTIONS]\n\n";
    cout << "Options:\n";
    cout << "  -h, --help       Show this help message\n";
    cout << "  -i, --install    Install Waydroid\n";
    cout << "  -u, --uninstall  Uninstall Waydroid completely\n";
    cout << "  --unattended     Run in unattended mode (no prompts)\n";
    cout << "  --info           Show system information\n\n";
    cout << "Supported distributions:\n";
    cout << "  Arch Linux, Void Linux, Debian/Ubuntu,\n";
    cout << "  Fedora/RHEL/CentOS, openSUSE, NixOS\n\n";
    cout << "Examples:\n";
    cout << "  sudo waydroid-installer --install\n";
    cout << "  sudo waydroid-installer --uninstall\n";
    cout << "  sudo waydroid-installer --install --unattended\n";
}

void show_system_info(const SystemInfo& info) {
    cout << "=== System Information ===\n";
    
    cout << "Distribution: ";
    auto it = DISTRO_NAMES.find(info.distribution);
    if (it != DISTRO_NAMES.end()) {
        cout << it->second;
    } else {
        cout << "Unknown";
    }
    
    if (!info.version.empty()) {
        cout << " " << info.version;
    }
    cout << "\n";
    
    cout << "Architecture: " << info.architecture << "\n";
    cout << "Session: " << info.session_type << "\n";
    cout << "Running as root: " << (info.is_root ? "Yes" : "No") << "\n";
    cout << "===========================\n";
}

int main(int argc, char* argv[]) {
    // Parsear argumentos
    bool do_install = false;
    bool do_uninstall = false;
    bool unattended = false;
    bool show_info = false;
    
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            show_help();
            return 0;
        } else if (arg == "-i" || arg == "--install") {
            do_install = true;
        } else if (arg == "-u" || arg == "--uninstall") {
            do_uninstall = true;
        } else if (arg == "--unattended") {
            unattended = true;
        } else if (arg == "--info") {
            show_info = true;
        } else {
            cerr << "Error: Unknown option '" << arg << "'\n";
            show_help();
            return 1;
        }
    }
    
    // Detectar sistema
    SystemDetector detector;
    SystemInfo system_info = detector.detect();
    
    if (show_info) {
        show_system_info(system_info);
        return 0;
    }
    
    if (!do_install && !do_uninstall) {
        // Modo interactivo
        string choice;
        cout << "Waydroid C++ Installer\n";
        cout << "1. Install Waydroid\n";
        cout << "2. Uninstall Waydroid\n";
        cout << "3. Exit\n";
        cout << "Choose option (1-3): ";
        getline(cin, choice);
        
        if (choice == "1") {
            do_install = true;
        } else if (choice == "2") {
            do_uninstall = true;
        } else {
            return 0;
        }
    }
    
    if (do_install) {
        Installer installer(system_info);
        if (!installer.install(unattended)) {
            return 1;
        }
    } else if (do_uninstall) {
        Uninstaller uninstaller(system_info);
        if (!uninstaller.uninstall(unattended)) {
            return 1;
        }
    }
    
    return 0;
}
