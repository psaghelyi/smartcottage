# SmartCottage — rpibridge

This repo configures and documents **rpibridge**, a Raspberry Pi 4 that acts as the
network bridge/router between a cottage's local LAN and the owner's home network
(via ZeroTier), and also hosts a Docker-based homelab (CCTV/Shinobi, media, monitoring,
dev tooling). This file reflects the **live state of the device** as discovered on
2026-08-01, not just what's checked into git — see "Repo vs. live drift" below.

## Hardware / OS

- Raspberry Pi 4 Model B Rev 1.4, hostname `rpibridge`.
- Debian 11 (bullseye), kernel `6.1.21-v8+` (Raspberry Pi Foundation kernel, Apr 2023 — dated).
- Root fs on SD card (`/dev/root`, 29G, 75% used). Bulk storage is a separate 916G disk
  mounted at `/mnt/sda1` (43% used) — this is where Shinobi's camera footage lives
  (`/home/pi/Shinobi` is a symlink to `/mnt/sda1/Shinobi`), so an OS reinstall is
  low-risk to recorded video.
- Timezone Europe/Budapest, NTP-synced. No RTC battery — cold boots start at the
  epoch until NTP catches up (visible in `last` as `Thu Jan 1 01:00` boot entries).

## Network architecture

The Pi is the gateway between the cottage LAN and the outside world/home network.
`rpibridge/README.md` documents the original setup steps; `rpibridge/*.network` and
`*.netdev` are systemd-networkd unit files meant to be copied to `/etc/systemd/network/`.

```
eth0 (VLAN trunk, no IP)
 ├─ eth0.111  → 192.168.10.0/24  "WAN-side" — static IP .111, gateway .254, NAT'd out (uplink)
 └─ eth0.222  → enslaved into br0 (no IP of its own)

br0 (bridge, STP on)
 ├─ eth0.222        (cottage LAN physical side)
 └─ ztugawpd7r       (ZeroTier network "CCTV" interface)
 → br0 address 192.168.192.1/24 — this is the cottage's internal LAN
```

- **eth0.111** is the uplink into the home network / internet (masqueraded via
  `iptables -t nat -A POSTROUTING -o eth0.111 -j MASQUERADE`, `ip_forward=1`,
  rules persisted via `iptables-persistent`).
- **br0** is the cottage-side LAN (`192.168.192.0/24`), bridging the physical VLAN 222
  segment together with a ZeroTier network — so ZeroTier peers ("home network" side)
  appear as if they're on the same local LAN as cottage devices (cameras, smart plugs,
  Deco mesh nodes, etc.). This is the trick that lets the home network and cottage
  network behave as one flat LAN over ZeroTier.
- **ZeroTier**: network `CCTV` (`d3ecf5726d92fd9c`), status OK, assigned
  `192.168.192.1/23` on interface `ztugawpd7r`, bridged into `br0`.
- **DHCP + DNS for the cottage LAN (192.168.192.0/24) is handled by dnsmasq**, not
  systemd-networkd — `/etc/systemd/network/50-bridge-br0.network` has
  `DHCPServer=no` on the live box (networkd's own DHCP server block is present but
  disabled/commented out). `/etc/dnsmasq.conf` (live-only, not in this repo — see
  drift note) has `dhcp-range=192.168.192.150,192.168.192.205,1h` plus static
  `dhcp-host=` reservations for the switch, Wi-Fi repeater, iMac, Deco mesh nodes,
  6 CCTV cameras, and smart plugs.
- systemd-resolved runs with `DNSStubListener=no` and forwards to `192.168.192.1`
  (dnsmasq) — no port-53 conflict between the two.

### Repo vs. live drift

✅ **Resolved 2026-08-01**: `rpibridge/25-eth0.network`, `30-eth0.111.network`, and
`50-bridge-br0.network` were out of sync with `/etc/systemd/network/` (dnsmasq had
taken over DHCP/DNS duties and the route-to-home-LAN gateway had moved from `.48` to
`.10`, neither reflected in the repo). Repo copies are now byte-for-byte synced from
live. `rpibridge/README.md` was also trimmed — it previously carried a one-time
dhcpcd→networkd migration guide (long since completed, generic usb0/wlan0 examples
that didn't match this box's actual VLAN+bridge setup) and now just points here plus a
short "how to deploy a change" note. The root `README.md`'s stale homeassistant
port-forwarding snippet was removed too — it referenced a NAT rule and container that
no longer exist.

Going forward: **diff `rpibridge/*.network` against `/etc/systemd/network/*` before
editing either side**, so this doesn't silently drift again. `/etc/dnsmasq.conf` still
isn't checked into the repo — consider adding it if further dnsmasq changes are made.

## CCTV — Shinobi

- `services/shinobi/docker-compose.yml`: single privileged container
  `registry.gitlab.com/shinobi-systems/shinobi:dev-arm64v8`, port `8088→8080`,
  hardware video decode via `/dev/dri` + `/dev/video10-12` + `/dev/vchiq`.
- Storage: `/home/pi/Shinobi` → `/mnt/sda1/Shinobi` (config, MySQL DB, video, plugins).
- Start/stop via the repo's `services/Makefile`: `make -C services shinobi`
  (runs `docker compose pull && docker compose up -d` in `services/shinobi/`).
- There is an **uncommitted** change to this compose file adding
  `pull_policy: always` (`git diff` shows it staged as a working-tree edit only).

## Services inventory

`services/` holds one subdirectory per homelab service, each with either a
`docker-compose.yml` or `start.sh`/`stop.sh` pair, wired into `services/Makefile`
(`make <service>` = stop then start). Notable ones beyond Shinobi: Portainer
(container management, port 9000), Prometheus + node-exporter, ELK, Grafana,
Home Assistant, Jellyfin, Immich, nginx-reverse-proxy (+ Let's Encrypt cert scripts
for `askalot.io` / `psaghelyi.ddns.net`), ddclient (dynamic DNS), Bitwarden,
code-server, zeronsd (ZeroTier DNS — image only, not currently running),
Kubernetes helper scripts (k3d/k3s), NFS server, Postgres, Squid cache, Deluge,
Filebrowser, sirway, registry.

A separate `smartcottage.service` (root `scripts/`, Python + venv) is a leftover/legacy
piece — it's **not installed** on the box (`systemctl is-enabled` fails: no unit file
present) despite the top-level README describing how to install it. Treat as inactive
unless revived deliberately.

## Known issues found during live diagnosis (2026-08-01)

1. ✅ **FIXED — dnsmasq needed periodic restarting for name resolution.**
   Root's crontab has a standing band-aid: `0 */2 * * * systemctl restart dnsmasq.service`
   (left in place intentionally, see below). Root cause: `/etc/dnsmasq.conf` had its
   `interface=br0` / `interface=lo` lines **commented out**, so dnsmasq bound to the
   wildcard address (`0.0.0.0:53`) across *all* interfaces instead of pinning to
   `br0`+`lo`. This host also runs Docker, which constantly creates/tears down bridge
   and veth interfaces for compose projects — during this diagnosis session alone, two
   container networks (`shinobi_default`, `exporters_default`) flapped their veths
   within a 10-minute window. A wildcard-bound dnsmasq is known to lose track of its
   listening sockets when the interface set churns like this, which lines up with why
   a scheduled restart was needed as a workaround.
   **Applied**: uncommented `interface=br0` and `interface=lo` in `/etc/dnsmasq.conf`
   (backed up as `/etc/dnsmasq.conf.bak-pre-fix`), validated with `dnsmasq --test`,
   restarted, confirmed resolution works (`camera1` → `192.168.192.201`).
   **Left in place on purpose**: the 2-hourly cron restart, as a safety net —
   remove it (`sudo crontab -e`) after a few days of confirmed stable resolution
   with no need for manual restarts.

2. ✅ **FIXED — Shinobi (CCTV recording) was stopped.** Container `shinobi` had cleanly
   exited (`SIGINT`/PM2 shutdown) around 09:12 local, most likely mid-test of the
   uncommitted `pull_policy: always` edit to `services/shinobi/docker-compose.yml`.
   Restart policy is `unless-stopped`, so Docker would not auto-restart it since it was
   an intentional stop, not a crash.
   **Applied**: `cd services/shinobi && docker compose up -d` — container is back up,
   PM2/camera process online, hardware acceleration detected (vdpau/vaapi/drm).

3. **Docker engine/tooling is inconsistent and stale — not yet fixed.**
   `/etc/apt/sources.list.d/docker.list` is configured to pull from Docker's official
   repo, but the package actually installed is Debian's own old `docker.io
   20.10.5+dfsg1` (from 2021) — `docker-ce` was never actually switched to. Both the
   legacy Python `docker-compose` (v1.25.0, unmaintained/EOL) and the current
   `docker-compose-plugin` (v2, `docker compose`) are installed side by side.
   → **Fix**: purge `docker.io` + `docker-compose`, install `docker-ce docker-ce-cli
   containerd.io docker-buildx-plugin docker-compose-plugin` from the already-configured
   Docker repo, verify every `services/Makefile` target still starts cleanly afterward.
   Deliberately deferred — this swaps the container runtime under every running
   service (including Shinobi), so it should happen in a planned window, not as a
   quick live fix.

4. **Prometheus exporters stack (`services/prometheus/exporters/`) is intentionally
   not run.** node-exporter/cadvisor/nginx-exporter were briefly started during this
   diagnosis then torn down again at the owner's request — this isn't something they
   want running. Ignore its stopped state; it's not a bug.

5. **Repo network config is stale** — see "Repo vs. live drift" above. Any future edit
   to `rpibridge/*.network` should first be diffed against `/etc/systemd/network/*`
   on the live box, or it'll silently regress the working config. Not yet synced.

6. ✅ **FIXED — pending patch-level updates applied.** `docker-buildx-plugin` (→0.36.0),
   `docker-compose-plugin` (→5.3.1), `zerotier-one` (1.16.1→1.16.2) upgraded via apt;
   `zerotier-one.service` restarted afterward and confirmed back `ONLINE` with the
   `CCTV` network still bridged into `br0`. `systemd-networkd`/`systemd-resolved` also
   picked up minor package updates per `needrestart` but were **not** restarted live —
   restarting the network stack on this box's only path to the cottage/home network
   is deferred to a planned reboot/maintenance window rather than done ad hoc.

## OS/config upgrade plan

This box is the *only* path between the cottage LAN and the outside/home network, so
treat networking changes as high blast-radius — validate before committing, keep a
rollback path.

**Phase 0 — safety net**
- Commit the current live `/etc/systemd/network/*` and `/etc/dnsmasq.conf` into this
  repo (fixing the drift noted above) before changing anything else, so there's a
  known-good reference to restore from.
- `/mnt/sda1` (camera footage) is a separate disk from the OS SD card — note this
  explicitly before any OS-level work so it isn't accidentally wiped.

**Phase 1 — low-risk fixes, no reboot required**
- ✅ Fix dnsmasq interface binding (item 1) — done; 2-hourly cron restart kept as a
  safety net, remove it once resolution stays stable for a few days.
- ✅ Bring Shinobi back up (item 2) — done.
- ✅ Apply pending apt package upgrades (item 6) — done; `zerotier-one` restarted and
  reconnected cleanly.
- Still open: restart `systemd-networkd`/`systemd-resolved` to pick up their minor
  package updates — do this at next planned reboot rather than live, given this box's
  role as the only path into the cottage network.
- Still open: sync repo's `rpibridge/*.network` files from the live
  `/etc/systemd/network/*` and commit (item 5).

**Phase 2 — Docker engine consolidation**
- Purge `docker.io` + legacy `docker-compose`, install the real `docker-ce` stack
  from the already-configured Docker apt repo (item 3).
- Re-verify every service in `services/Makefile` restarts cleanly under the new engine
  before considering this done — this touches the container runtime for every
  service on the box, including Shinobi.

**Phase 3 — OS major upgrade (Debian 11 bullseye → Raspberry Pi OS Lite 64-bit, Debian 13 "trixie")**
- As of 2026-08, Trixie is the current Raspberry Pi OS release (Bookworm is now
  "Legacy"); since this is a fresh-flash, not an in-place `apt dist-upgrade`, there's
  no reason to stop at the intermediate Bookworm release — go straight to current.
  Docker CE, ZeroTier, and InfluxDB's apt repos all already have trixie builds.
  Worth double-checking during the migration: the NodeSource repo is currently pinned
  to Node 16.x/bullseye (Node 16 itself is long EOL regardless) and needs revisiting.
- Given this device is a live router/bridge with a hand-built VLAN + bridge +
  ZeroTier-bridging network stack, prefer **flashing a fresh image on a
  spare SD card** over an in-place `apt dist-upgrade`. Re-apply the (now-synced)
  `rpibridge/*.network` configs, dnsmasq config, Docker, and ZeroTier from this repo,
  and validate the full network path (VLAN trunk, bridge, ZeroTier bridging, DHCP/DNS)
  on the bench before swapping the SD card into production. An in-place upgrade risks
  leaving the cottage unreachable if something breaks mid-upgrade, with no easy way to
  intervene remotely.
- If an in-place upgrade is preferred anyway (e.g. physical access is inconvenient),
  do it only with local console access available as a fallback, and snapshot
  `/etc/systemd/network/`, `/etc/dnsmasq.conf`, and `/etc/iptables/rules.v4` beforehand.

**Phase 4 — post-upgrade validation checklist**
- VLAN trunk up: `eth0.111` and `eth0.222` both `configured` in `networkctl`.
- `br0` has the correct `192.168.192.1/24` address and is routable.
- ZeroTier online (`zerotier-cli info` → `ONLINE`) and the `CCTV` network still bridges
  into `br0`.
- dnsmasq resolves cottage-LAN names without needing the periodic restart — safe to
  remove the cron band-aid at this point.
- Every `services/Makefile` target starts cleanly.
- Shinobi recording and reachable through the nginx reverse proxy.
- `npx playwright install --with-deps chromium` succeeds (Playwright MCP's browser
  install) — blocked on current bullseye: Playwright 1.62 hard-refuses to install any
  browser (chromium, headless-shell, firefox) on `debian11-arm64`, not a missing-deps
  issue but a platform-support gate. `@playwright/mcp` itself is already installed
  globally via npm and just needs the browser once the OS is current.
