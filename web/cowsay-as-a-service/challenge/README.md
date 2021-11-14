# Cowsay as a Service

```bash
docker build . -t acsc-cowsay
docker run -p 3000:3000 -e FLAG=ACSC{} acsc-cowsay
```