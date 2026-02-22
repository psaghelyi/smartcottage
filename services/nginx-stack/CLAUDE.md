# nginx-stack – Reverse proxy and Let's Encrypt for psaghelyi.ddns.net

This folder runs **nginx in Docker** on **192.168.1.100**, with **Let's Encrypt (certbot)** for `psaghelyi.ddns.net`. Plain HTTP is split by an **external nginx** (e.g. on the router); only `psaghelyi.ddns.net` is sent here.

---

## 1. Traffic flow: external nginx → this host

An **external nginx** (e.g. on the router) receives all plain HTTP (port 80) and splits it:

- **`Host: psaghelyi.ddns.net`** → proxied to **192.168.1.100:80** (this host).
- All other hostnames → sent elsewhere (e.g. another host like 192.168.1.200).

So only traffic for `psaghelyi.ddns.net` reaches this machine. That is enough for:

- **ACME HTTP-01 challenges** (Let's Encrypt) for `psaghelyi.ddns.net`.
- Any other HTTP for that host (we intentionally reject everything except ACME; see below).

---

## 2. What runs on 192.168.1.100 (this host)

- **Nginx** in a Docker container (e.g. container name `nginx`).
- **Certbot** is *not* a long-running service; it is run manually or by cron to **renew** certificates.

Nginx is configured to:

- **Port 80, `server_name psaghelyi.ddns.net`:**
  - **`/.well-known/acme-challenge/<token>`** → served from the certbot webroot (`/var/www/certbot` in the container). This is the only path we need for Let's Encrypt.
  - **Any other request** → **403 Forbidden** (we do not serve normal HTTP content for this host).
- **Other ports (e.g. 443, 8006, …)** → your HTTPS services (Proxmox, etc.), using the same TLS certificate for `psaghelyi.ddns.net`.

So: **only ACME challenge requests are accepted on port 80**; everything else is rejected.

---

## 3. Certificates and paths

- **Certificates** are stored under **`.config/certbot/`** in this folder:
  - **`.config/certbot/conf`** – Let's Encrypt config and **live** certs (e.g. `live/psaghelyi.ddns.net/fullchain.pem`, `privkey.pem`). Nginx mounts this read-only as its SSL cert source.
  - **`.config/certbot/www`** – Webroot for HTTP-01. Certbot writes challenge files here; nginx serves them from `/var/www/certbot` (same dir, mounted from this path).
  - **`.config/certbot/lib`** and **`.config/certbot/log`** – certbot state and logs.

Nginx is started with **bind mounts** from this directory (exact path depends on where the repo lives on the server; see below). It reads TLS certs from the **conf** dir and serves ACME challenges from the **www** dir.

---

## 4. Certificate renewal (critical: run from the same path as nginx)

Renewal is done by **`renewcert.sh`** in this folder. It:

1. Runs **certbot renew** in a one-off Docker container.
2. Mounts **this folder’s** `.config/certbot/{www,conf,log,lib}` into the container.
3. Certbot writes challenge files into **www**; Let's Encrypt fetches them at `http://psaghelyi.ddns.net/.well-known/acme-challenge/<token>`.
4. The external nginx sends that request to 192.168.1.100:80; nginx here serves from the **same** `www` directory.
5. After a successful renewal, the script runs a **deploy hook**: `docker exec <container> nginx -s reload` so nginx picks up the new cert.

**You must run `renewcert.sh` from the same directory path that the nginx container uses for its mounts.**  
If nginx is started with e.g. `/home/pi/Projects/github/psaghelyi/smartcottage/services/nginx-stack`, then run the script from that path, not from another clone (e.g. not from `/mnt/sda1/Project/...`). Otherwise certbot writes challenges into a different filesystem location than the one nginx serves, and renewal will fail with HTTP-01 404.

Example (adjust path to match your server):

```bash
cd /home/pi/Projects/github/psaghelyi/smartcottage/services/nginx-stack
NGINX_CONTAINER=nginx ./renewcert.sh
```

- **`NGINX_CONTAINER`** – name of the nginx container (default in script: `nginx-reverse-proxy`; on this server it is `nginx`). Set it if your container has a different name.

---

## 5. Automatic renewal (cron)

**`certbot-renew.cron`** is a cron snippet that runs `renewcert.sh` twice daily (e.g. 03:00 and 15:00).

- **Install (as root):**  
  `sudo cp certbot-renew.cron /etc/cron.d/ && sudo chmod 644 /etc/cron.d/certbot-renew.cron`
- **Path inside the cron file:**  
  Must point to the **same path from which nginx is run** (see above). If the repo is at `/home/pi/Projects/.../nginx-stack`, edit the cron line to use that path instead of `/mnt/sda1/...`.

Example line (path must match your server):

```
0 3,15 * * * root /home/pi/Projects/github/psaghelyi/smartcottage/services/nginx-stack/renewcert.sh >> /var/log/certbot-renew.log 2>&1
```

Ensure **`NGINX_CONTAINER`** is correct for your setup. If your nginx container is named `nginx`, you can set it in cron, e.g.:

```
0 3,15 * * * root NGINX_CONTAINER=nginx /home/pi/Projects/.../nginx-stack/renewcert.sh >> /var/log/certbot-renew.log 2>&1
```

---

## 6. First-time certificate (optional)

If you need to **create** the certificate for `psaghelyi.ddns.net` for the first time (instead of renewing), use the same webroot and host. On the server you may have a script like **`gencert_psaghelyi.ddns.net.sh`** that runs:

```bash
certbot certonly --webroot --webroot-path /var/www/certbot/ -d psaghelyi.ddns.net --email your@email.com
```

Run it from the **same directory** that nginx uses (so the webroot is the same). After that, normal renewal is done by **`renewcert.sh`** and cron.

---

## 7. Summary

| Item | Description |
|------|-------------|
| **External nginx** | Splits HTTP; sends only `psaghelyi.ddns.net` to **192.168.1.100:80**. |
| **This host** | Nginx in Docker; serves only `/.well-known/acme-challenge/` for that host; everything else on 80 → 403. |
| **HTTPS (e.g. :8006)** | Same nginx, same cert for `psaghelyi.ddns.net`. |
| **Cert storage** | `.config/certbot/` in this folder; nginx mounts `conf` (certs) and `www` (ACME webroot). |
| **Renewal** | Run **`renewcert.sh`** from the **same path** nginx uses; set **`NGINX_CONTAINER`** if needed. |
| **Cron** | Use **`certbot-renew.cron`** with the correct path and container name so renewal stays automatic. |
