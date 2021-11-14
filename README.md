# ACSC 2021 Challenge Repository
**The format is based on [zer0pts CTF repository](https://gitlab.com/zer0pts/zer0pts-ctf-2021/). This might be changed according to the design of the scoreboard.**

## Directory Structure
The directory for each challenge is `/<category>/<challenge_name>`.
The challenge directory has 3 folders: `challenge`, `distfiles`, and `solution`.

- `challenge`: Files necessary for building the task (source code, flag, etc)
- `distfiles`: Files to be distributed to the competitors (if any)
- `solution`: Scripts of the intended solution

Along with the 3 folders, you must put `task.yml`. (See the next section.)

If your task runs on docker, you can put `docker-compose.yml` and `Dockerfile` right under the challenge directory.

Example:
```
.
|-- pwn/
|    |-- histogram/
.    .    |-- challenge/
.    .    |-- distfiles/
.    .    |-- solution/
          |-- Dockerfile
          |-- docker-compose.yml
          +-- task.yml
```

## Challenge Information
You need to provide the information of your task in YAML format in `task.yml`.
The file must contain the following information:

- `name`: Challenge name
- `description`: Challenge description (HTML)
- `flag`: FLAG
- `author`: Challenge author
- `tags`: Choose tags from `pwn`, `web`, `crypto`, `rev`, `forensics`, `hardware`, `ppc`, `warmup`

Additionally, you can put the following information:

- `port`: Port number

The tokens `{host}` and `{port}` in `task.yml` are converted into the host name and port number respectively.

Example 1:
```yaml
---
name: "welcome"
description: >
  <p>The flag is written in Discord</p>
flag: "ACSC{W3lc0m3W3lc0m3}"
author: "ACSC organization"
tags:
- welcome
```

Example 2:
```yaml
---
name: "just nc"
description: >
  <p>Just nc</p><br>
  <code>nc {host} {port}</code>
flag: "ACSC{hogehogehugahuga}"
author: ptr-yudai
port: 1337
tags:
- pwn
- warmup
```
