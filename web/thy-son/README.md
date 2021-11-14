# Thy Son

## Deployment

Prevent containers to access metadata API
ref: <https://docs.aws.amazon.com/AmazonECS/latest/developerguide/task-iam-roles.html>

```sh
$ sudo iptables --insert FORWARD 1 --in-interface docker+ --destination 169.254.169.254/32 --jump DROP
$ sudo apt install iptables-persistent
$ sudo netfilter-persistent save
# when the server rebooted, docker service automatically re-order iptables. so we need reloading iptables manually.
# maybe we need startup script...
$ sudo netfilter-persistent reload
```

Before deployment, edit `RECAPTCHA_PUBLIC_KEY` and `RECAPTCHA_PRIVATE_KEY` in `docker-compose.yml`.
Also, don't forget to modify challenge/flag

```
$ docker build -t acsc-thyson ./challenge
$ docker compose up
```

Open localhost:5000
