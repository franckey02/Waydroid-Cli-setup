# Compile with CMake
mkdir build && cd build
cmake ..
make

# Install
sudo ./waydroid-installer --install

# Uninstall
sudo ./waydroid-installer --uninstall

# install unattended
sudo ./waydroid-installer --install --unattended
