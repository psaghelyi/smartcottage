# rpibridge network config

The `*.network` / `*.netdev` files in this directory are copies of what's deployed
under `/etc/systemd/network/` on the live box — kept in sync with `git diff` against
the live files before editing either side. See the top-level `CLAUDE.md` for the full
topology (VLANs, bridge, ZeroTier, dnsmasq) and how these files fit together.

To deploy a change: edit the file here, copy it to `/etc/systemd/network/`, then
`sudo systemctl restart systemd-networkd` (or reboot) and verify with `networkctl`.

## `etc-backup/` and `secrets/` — OS upgrade / SD card migration

`etc-backup/` holds plain-text copies of live `/etc` files that aren't part of a
package install and would otherwise only exist on this SD card: `dnsmasq.conf`,
`iptables-rules.v4`, `resolved.conf`, `fstab`, `crontab-root.txt`. These are tracked
in git like everything else here — refresh them (`sudo cp /etc/... rpibridge/etc-backup/...`)
whenever you change the live equivalent, same as the `*.network` files above.

`secrets/` holds the one piece of state that's genuinely irreplaceable and can't go
in git: the ZeroTier node identity. See `secrets/README.md` for what's there and how
to restore it on a freshly imaged box — **do this before first-booting ZeroTier on
the new install**, or the box gets a new ZeroTier address and has to be re-authorized
on the network.

Other stateful data that migrates by plain filesystem copy rather than git (already
gitignored, already living inside this repo tree): `services/portainer/.portainer-data`,
`services/nginx-reverse-proxy/.config/` (Let's Encrypt certs + Cloudflare API token),
`services/homeassistant/token`, and any other `.*-data/`/`.*-config/` directories under
`services/`. Shinobi's data does *not* need migrating — it lives on `/mnt/sda1`, a
separate physical disk untouched by an SD card swap (see `etc-backup/fstab` for the
mount entry, keyed to the disk's own PARTUUID so it carries over unchanged).

# Setup router

## Enable ip forwarding:

`$ sudo sysctl -w net.ipv4.ip_forward=1`

### Bonus: Disable IPv6

```
$ sudo sysctl -w net.ipv6.conf.all.disable_ipv6=1
$ sudo sysctl -w net.ipv6.conf.default.disable_ipv6=1
$ sudo sysctl -w net.ipv6.conf.lo.disable_ipv6=1
```

## Setup iptables

Allow everything comming from internal:

`$ sudo iptables -A FORWARD -i br0 -j ACCEPT`

NAT everything going out:

`$ sudo iptables -t nat -A POSTROUTING -o eth0.111 -j MASQUERADE`
