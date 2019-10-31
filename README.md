# fty-config

fty-config is an agent who has in charge Save, Restore, and Reset the system.

## How to build

To build fty-config project run:

```bash
./autogen.sh
./configure
make
make check # to run self-test
```

## How to run

To run fty-config project:

* from within the source tree, run:

```bash
./src/fty-config
```

For the other options available, refer to the manual page of fty-info.

* from an installed base, using systemd, run:

```bash
systemctl start fty-config
```

### Configuration file

Agent has a configuration file: fty-config.cfg.
Except standard server and malamute options, there are two other options:
* server/check_interval for how often to publish Linux system metrics
* parameters/path for REST API root used by IPM Infra software
Agent reads environment variable BIOS_LOG_LEVEL, which sets verbosity level of the agent.

## Architecture

### Overview