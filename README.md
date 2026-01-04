# Waydroid Multi-Distro Installer

A simple, clean CLI installer for Waydroid that supports multiple Linux distributions.

## Features

- **Multi-distro support**: Install Waydroid on Arch, Void, Debian/Ubuntu, Fedora, openSUSE, and NixOS
- **Automatic detection**: Detects your distribution and package manager
- **Unattended mode**: Install without prompts for scripting
- **Complete uninstallation**: Remove Waydroid and all configurations
- **NixOS support**: Step-by-step guide for declarative configuration

## Supported Distributions

| Distribution | Package Manager | Installation Type |
|--------------|-----------------|-------------------|
| Arch Linux | pacman/AUR | Automatic |
| Void Linux | xbps | Automatic |
| Debian/Ubuntu | apt | Automatic |
| Fedora/RHEL | dnf/rpm-ostree | Automatic |
| openSUSE | zypper | Automatic |
| NixOS | nix | Instructive (step-by-step guide) |

## Quick Start

```bash
# Clone the repository
git clone https://github.com/franckey02/Waydroid-Cli-setup.git
cd  Waydroid-Cli-setup

# Install Waydroid (requires sudo)
cd sh
sudo ./waydroid-installer.sh

# Install without prompts
sudo ./waydroid-installer.sh --unattended

# Uninstall Waydroid completely
sudo ./waydroid-uninstall.sh
```

## Usage

### Installation

```bash
sudo ./waydroid-installer.sh [OPTIONS]
```

**Options:**
- `-h, --help` - Show help message
- `-u, --unattended` - Unattended mode (no questions)

**Examples:**
```bash
# Interactive installation
sudo ./waydroid-installer.sh

# Unattended installation (for scripts)
sudo ./waydroid-installer.sh --unattended
```

### Uninstallation

```bash
sudo ./waydroid-uninstall.sh
```

The uninstaller will:
1. Stop all Waydroid services
2. Remove the Waydroid package
3. Delete all configuration files
4. Remove kernel modules (if installed via DKMS)
5. Clean up user data

## Project Structure

```
waydroid-installer/
├── waydroid-installer.sh          # Main installer script
├── waydroid-uninstall.sh          # Complete uninstaller
└── modules/installers/            # Distribution-specific installers
    ├── arch.sh                   # Arch Linux
    ├── void.sh                   # Void Linux
    ├── debian.sh                 # Debian/Ubuntu
    ├── fedora.sh                 # Fedora/RHEL
    ├── opensuse.sh               # openSUSE
    └── nixos.sh                  # NixOS (instructive guide)
```

## What Each Installer Does

### Arch Linux
- Updates system packages
- Installs Waydroid from official repos or AUR
- Initializes with or without GAPPS
- Enables systemd service

### Debian/Ubuntu
- Updates system packages
- Adds official Waydroid repository
- Installs Waydroid
- Initializes with or without GAPPS
- Enables systemd service

### Fedora
- Detects if immutable variant (Silverblue/Kinoite)
- Installs via dnf or rpm-ostree
- Initializes with or without GAPPS
- Provides OTA URLs

### openSUSE
- Updates system
- Adds Waydroid repository
- Configures kernel parameters (psi=1)
- Configures AppArmor for dnsmasq
- Requires reboot for kernel changes

### Void Linux
- Updates system
- Installs Waydroid from official repos
- Initializes with or without GAPPS
- Enables runit service

### NixOS
- Provides step-by-step guide for declarative configuration
- Shows required kernel modules and parameters
- Explains how to rebuild system
- Includes troubleshooting tips

## Requirements

- Linux distribution (see supported list above)
- Root privileges (sudo)
- Internet connection
- Wayland session (recommended, X11 may work with nested compositors)

## Troubleshooting

### Common Issues

1. **No internet in Waydroid:**
   ```bash
   sudo sysctl -w net.ipv4.ip_forward=1
   sudo iptables -A FORWARD -i waydroid0 -j ACCEPT
   ```

2. **Wayland session required:**
   - Ensure you're running a Wayland session
   - For X11, consider using cage or weston as nested compositors

3. **Kernel modules missing:**
   - Arch: Use linux, linux-lts, or linux-zen kernels
   - Other distros: May need DKMS modules or kernel recompilation

### Manual Startup

If services don't start automatically:
```bash
# Start container
sudo waydroid container start

# In another terminal, start session
waydroid session start

# Launch full UI
waydroid show-full-ui
```


## Author
JavierC

## Acknowledgments

- Waydroid team for the amazing Android-on-Linux solution
- Multiple Linux distribution maintainers
- The open source community

---

**Note:** This installer is designed to be simple and straightforward. It follows the official installation methods for each distribution.
