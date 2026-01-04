#!/bin/bash
# waydroid-installer.sh - Multi-distro CLI installer for Waydroid
# GitHub: https://github.com/franckey02/Waydroid-Cli-setup

set -e  # Exit on error

# Script base directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODULES_DIR="$SCRIPT_DIR/modules"

# Load essential modules
source "$MODULES_DIR/colors.sh"
source "$MODULES_DIR/logging.sh"
source "$MODULES_DIR/common.sh"
source "$MODULES_DIR/detection.sh"

# Global variables
DISTRO=""
VERSION=""
PKG_MANAGER=""
INIT_SYSTEM=""
SESSION_TYPE=""
ARCH=""

# Configuration
LOG_FILE="/tmp/waydroid-installer-$(date +%Y%m%d-%H%M%S).log"
VERBOSE=false
UNATTENDED=false
CONFIGURE_ONLY=false

# Main function
main() {
    # Parse arguments
    parse_args "$@"
    
    # Initialize logging
    init_logging "$LOG_FILE"
    
    # Show banner
    show_banner
    
    # Check dependencies
    check_dependencies
    
    # Detect system
    log_info "Detecting system..."
    detect_system
    
    # Show system information
    show_system_info
    
    # Execute based on mode
    if [ "$UNATTENDED" = true ]; then
        install_unattended
    elif [ "$CONFIGURE_ONLY" = true ]; then
        configure_only
    else
        interactive_menu
    fi
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -u|--unattended)
                UNATTENDED=true
                shift
                ;;
            -c|--configure-only)
                CONFIGURE_ONLY=true
                shift
                ;;
            -i|--info)
                detect_system
                show_system_info
                exit 0
                ;;
            --version)
                show_version
                exit 0
                ;;
            *)
                echo "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Interactive menu
interactive_menu() {
    while true; do
        clear
        show_banner
        show_system_info
        
        echo "Main Menu:"
        echo "1) Install Waydroid (automatic)"
        echo "2) Install Waydroid (custom)"
        echo "3) Configure network/firewall"
        echo "4) Troubleshooting"
        echo "5) Waydroid utilities"
        echo "6) Uninstall Waydroid"
        echo "7) Information/Diagnostics"
        echo "8) Exit"
        
        read -p "Select an option [1-8]: " choice
        
        case $choice in
            1)
                install_auto
                ;;
            2)
                install_custom
                ;;
            3)
                configure_network_menu
                ;;
            4)
                troubleshooting_menu
                ;;
            5)
                waydroid_utils_menu
                ;;
            6)
                uninstall_menu
                ;;
            7)
                show_diagnostics
                ;;
            8)
                echo "Goodbye!"
                exit 0
                ;;
            *)
                echo "Invalid option"
                ;;
        esac
        
        if [ "$choice" != "8" ]; then
            echo ""
            read -p "Press Enter to continue..."
        fi
    done
}

# Automatic installation
install_auto() {
    log_info "Starting automatic installation..."
    
    # Load installer based on distro
    case $DISTRO in
        nixos)
            source "$MODULES_DIR/installers/nixos.sh"
            install_nixos
            ;;
        *)
            # For other distros, run automatic installation
            source "$MODULES_DIR/installers/$DISTRO.sh" 2>/dev/null || {
                log_error "Installer not found for $DISTRO"
                return 1
            }
            
            # Confirmation (except in unattended mode)
            if [ "$UNATTENDED" = false ]; then
                echo "Waydroid will be installed on $DISTRO $VERSION"
                echo "Commands will be executed as superuser."
                read -p "Continue? [Y/n]: " confirm
                [[ $confirm =~ ^[Nn] ]] && return 1
            fi
            
            # Execute installation
            "install_${DISTRO}"
            ;;
    esac
    
    log_success "Installation completed"
    show_post_install_info
}

# Show help
show_help() {
    cat << EOF
Waydroid Installer - Multi-distro CLI installer

Usage: $0 [OPTIONS]

Options:
  -h, --help           Show this help message
  -v, --verbose        Verbose mode (show more details)
  -u, --unattended     Unattended mode (no questions)
  -c, --configure-only Configure only (assume packages installed)
  -i, --info           Show system information
  --version            Show version

Examples:
  $0                    # Interactive menu
  $0 --unattended       # Automatic installation without questions
  $0 --info            # Show system information
  $0 --configure-only  # Configuration only

Supports: Arch, Debian/Ubuntu, Fedora, openSUSE, Void, Gentoo, Alpine, NixOS
EOF
}

# Show version
show_version() {
    echo "Waydroid Installer v1.0.0"
    echo "Multi-distro support for Waydroid"
}

# Entry point
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
