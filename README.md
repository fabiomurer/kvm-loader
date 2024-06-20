# Toy KVM-based VM manager

This project represents a simple KVM-based VM manager (VMM). It can load statically linked ELF64 executables
and start them in the virtual machines.

## Current features list

The following features are already implemented:
- Loading of ELF64 executables into VM memory
- VM starts in the Long mode, so paging is already set up
- GDT and one (NULL) entry of IDT are set

## How to build
The VM manager can be compiled with the following command.

```bash
make
```

This produces the `lvm` binary.

To generate the test application which can be loaded into the VM, run
```bash
make testprog
```

It produces the `tprog` binary.

## How to use

The statically linked ELF64 application can be loaded into the VM with the following command:

```bash
./lvm <app>
```

For instance, to run the test application provided with this repo you could simply run
```bash
./lvm tprog
```

Please note, that the application should be compiled with `-fno-pie -no-pie -nostdlib` flags,
since the currect version of the ELF loader doesn't support load-time relocation.
