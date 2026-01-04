#!/bin/bash
# openSUSE installer for Waydroid

install_opensuse() {
    echo "Installing Waydroid on openSUSE..."
    
    # Update system
    zypper dup -y
    
    # Detect version
    local version="tumbleweed"
    if [[ "$VERSION" =~ "15" ]]; then
        version="leap"
    fi
    
    # Add repository
    echo "Adding Waydroid repository..."
    if [ "$version" = "tumbleweed" ]; then
        zypper addrepo https://download.opensuse.org/repositories/home:runa-chin:Waydroid/openSUSE_Tumbleweed/home:runa-chin:Waydroid.repo
    else
        zypper addrepo https://download.opensuse.org/repositories/home:runa-chin:Waydroid/openSUSE_Leap_$VERSION/home:runa-chin:Waydroid.repo
    fi
    
    zypper refresh
    
    # Install grubby for kernel parameters
    zypper install -y grubby
    
    # Add PSI kernel parameter
    current_kernel="/boot/vmlinuz-$(uname -r)"
    if [ -f "$current_kernel" ]; then
        grubby --update-kernel="$current_kernel" --args="psi=1"
    fi
    
    # Install Waydroid
    zypper install -y waydroid
    
    # Configure AppArmor for dnsmasq
    echo "Configuring AppArmor..."
    cat > /etc/apparmor.d/usr.sbin.dnsmasq.waydroid << EOF
@{run}/waydroid-lxc/ r,
@{run}/waydroid-lxc/* rw,
EOF
    
    systemctl reload apparmor
    
    # Initialize
    echo "Initializing Waydroid..."
    waydroid init
    
    # Enable service
    systemctl enable --now waydroid-container.service
    
    echo "Waydroid installation completed on openSUSE"
    echo "Reboot required for kernel parameter changes"
}
