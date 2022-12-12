Create node_exporter user with no login

`$ sudo useradd -M node_exporter`

`$ sudo usermod -L node_exporter`

Create folder

`$ sudo mkdir /var/lib/node_exporter/textfile_collector`

`$ sudo chown -R node_exporter:node_exporter /var/lib/node_exporter`

Create config

create `/etc/sysconfig/node_exporter`
