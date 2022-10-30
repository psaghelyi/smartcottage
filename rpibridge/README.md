# Switch to networkd

## Remove non-systemd networking packages.

```
$ sudo apt remove --purge --auto-remove dhcpcd5 fake-hwclock ifupdown isc-dhcp-client isc-dhcp-common openresolv
$ sudo killall wpa_supplicant
$ sudo killall dhcpcd
```

## Configure systemd networking.

### Create config file for the usb0 interface:

`sudo nano /etc/systemd/network/10-usb0.network`

and enter the following in it:

> [Match]
> Name=usb0
>
> [Network]
> LinkLocalAddressing=ipv4

This will enable Link Local Address, aka APIPA (Automatic Private IP Addressing) on usb0.

### Create config file for the wlan0 interface, e.g.:

`sudo nano /etc/systemd/network/10-wlan0.network`

with the following contents:

> [Match]
> Name=wlan0
> 
> [Network]
> DHCP=ipv4

This will enable DHCP on wlan0.

### Create config file for the eth0 interface:

`sudo nano /etc/systemd/network/10-eth0.network`

with the following contents:

> [Match]
> Name=eth0
> 
> [Network]
> DHCP=ipv4

This will enable DHCP address on eth0. If you want to use a static IP address instead, replace the last line with:

> Address=192.168.1.100/24
> Gateway=192.168.1.1

or whatever your address, netmask and gateway are. 


## Enable systemd networking.

### Enable network management service:

`$ sudo systemctl enable systemd-networkd`

### Enable network name resolution (DNS) service:

```
$ sudo ln -sf /run/systemd/resolve/resolv.conf /etc/resolv.conf
$ sudo systemctl enable systemd-resolved
```

### Enable network time synchronization (NTP) service:

`$ sudo systemctl enable systemd-timesyncd`

### Reboot:

`$ sudo reboot`

### Check if everything is working:

```
$ systemctl status systemd-networkd
$ systemctl status systemd-resolved
$ systemctl status systemd-timesyncd
```





# Setup router

## Enable ip forwarding:

`$ sudo sysctl -w net.ipv4.ip_forward=1`

## Setup iptables

Allow everything comming from internal:

`$ sudo iptables -A FORWARD -i br0 -j ACCEPT`

NAT everything going out:

`$ sudo iptables -t nat -A POSTROUTING -o eth0.111 -j MASQUERADE`
