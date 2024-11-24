#!/bin/bash

# Function
open_and_wpa_psk() {

	# Enter network password
	read -p "Enter '$NETWORK_SSID' password: " PSK

	cat <<-EOF > "$MOUNT_POINT_ROOT/etc/NetworkManager/system-connections/$NETWORK_SSID.nmconnection"
	[connection]
	id=$NETWORK_SSID
	uuid=$(uuidgen)
	type=wifi
	interface-name=wlan0

	[wifi]
	mode=infrastructure
	ssid=$NETWORK_SSID

	[wifi-security]
	auth-alg=open
	key-mgmt=wpa-psk
	psk=$PSK

	[ipv4]
	method=auto

	[ipv6]
	addr-gen-mode=default
	method=auto

	[proxy]
	EOF

	chmod 600 "$MOUNT_POINT_ROOT/etc/NetworkManager/system-connections/$NETWORK_SSID.nmconnection"

}

# Check if user has maximum root privileges
if [ "$EUID" -ne 0 ]; then
	echo "You must be root to execute this script!"
	exit 1
fi

echo "Starting headless setup."

MOUNT_POINT_ROOT="/mnt/rpi-root"
MOUNT_POINT_FIRMWARE="$MOUNT_POINT_ROOT/boot/firmware"
DEFAULT_USERNAME="user"

# Device file
echo ""
lsblk
echo ""
while true; do
	read -p "Choose the device file where the image has been written: " DEVICE_FILE
	if [[ ! -e "$DEVICE_FILE" ]]; then
		echo -e "File '$DEVICE_FILE' doesn't exist!"
	else
		echo -e "Chosen $DEVICE_FILE!"
		break
	fi
done

# Unmount all partitions
umount $DEVICE_FILE*
if [[ -e "$MOUNT_POINT_ROOT" ]]; then
	umount -R $MOUNT_POINT_ROOT
fi

# Set the firmware partition device file
if echo "$DEVICE_FILE" | grep -q "nvme"; then
	FIRMWARE_PARTITION="$DEVICE_FILE"p1
else
	FIRMWARE_PARTITION="$DEVICE_FILE"1
fi

# Set the root partition device file
if echo "$DEVICE_FILE" | grep -q "nvme"; then
	ROOT_PARTITION="$DEVICE_FILE"p2
else
	ROOT_PARTITION="$DEVICE_FILE"2
fi

# Create a mount point if it doesn't exist
if [ ! -d "$MOUNT_POINT_ROOT" ]; then
	mkdir -p "$MOUNT_POINT_ROOT"
fi

# Mount the firmware partition on the mount point
mount $ROOT_PARTITION $MOUNT_POINT_ROOT

# Mount the firmware partition on the mount point
mount $FIRMWARE_PARTITION $MOUNT_POINT_FIRMWARE

# Enter username
read -p "Enter username for the Raspberry Pi [default 'user']: " USERNAME

if [[ -z "$USERNAME" ]]; then
	USERNAME=$DEFAULT_USERNAME
fi

# Enter password
while true; do
	read -s -p "Enter '$USERNAME' password: " PASSWORD
	if [[ -z "$PASSWORD" ]]; then
		echo -e "\nPassword cannot be empty!"
	else
		echo ""
		read -s -p "Reenter '$USERNAME' password: " PASSWORD_CHECK
		if [[ "$PASSWORD" == "$PASSWORD_CHECK" ]]; then
			echo -e "\nPassword verfied!"
			break
		else
			echo -e "\nPasswords do not match!"
		fi
	fi
done

# Calculate the password SHA512 and network password SHA512
PASSWORD_PSK=$(echo "$PASSWORD" | openssl passwd -6 -stdin)

# Create user
echo "$USERNAME:$PASSWORD_PSK" | tee $MOUNT_POINT_FIRMWARE/userconf.txt > /dev/null
echo "User info configured."

# Enable ssh
touch $MOUNT_POINT_FIRMWARE/ssh
echo "Enabled SSH."

# Enter network information
read -p "Connect Raspberry Pi to a wireless network when booting [y/n (default)]: " AUTO_WIRELESS

if [[ "$AUTO_WIRELESS" == "y" || "$AUTO_WIRELESS" == "Y" ]]; then

	read -p "Enter SSID for the network the Raspberry Pi will connect to: " NETWORK_SSID

	# Choose the authentication algorithm
	echo "Network authentication algorithms:"
	echo "open -> 1 (default)"
	read -p "Choose the authentication algorithm: " AUTH_ALG

	# Choose the key management setting
	echo "Network key management setting:"
	echo "wpa-psk -> 1 (default)"
	read -p "Choose the key management setting: " KEY_MGMT

	if [[ "$AUTH_ALG" == "1" && "$KEY_MGMT" == "1" ]]; then
		open_and_wpa_psk
	else
		open_and_wpa_psk
	fi

fi

# Unmount the firmware partition
umount $FIRMWARE_PARTITION

# Unmount the root partition
umount $ROOT_PARTITION

# Delete the mount point
rmdir $MOUNT_POINT_ROOT

echo "Headless setup complete."
