# Linux NFS Root under QEMU ARM emulator


Developing a root filesystem for embedded Linux architectures can present some inconveniences: for example if the filesystem should be loaded into the embedded system every time and it’s not always an easy/quick task, or if the disk space is very low. The Linux kernel has the ability to read the root filesystem from the network, specifically from a NFS share, and this can be exploited to solve these problems. A common setup is having an embedded board running a Linux kernel, connected via Ethernet to a server with a directory exported through NFS and a cross-compiler installed. Testing a new root filesystem becomes as easy as resetting the board. When using QEMU as a target instead of a real board, the inconvenience is similar: the root filesystem image should be created every time. Fortunately, QEMU offers a network model that allows to access the host as thought it was connected with an Ethernet network. The details of the default connection can be found in the documentation; briefly, a virtual LAN is created where the guest virtual system is given address 10.0.2.15 while the host workstation can be accessed using address 10.0.2.2.

#Prerequisites
- qemu-system-arm: can be installed on Ubuntu with “sudo apt-get install qemu-kvm-extras“, on Debian with “aptitude install qemu” as root
- NFS server: can be installed on Ubuntu or Debian with the nfs-kernel-server package
- curses developer libraries: can be installed on Ubuntu or Debian with the libncurses-dev package
- arm-none-linux-gnueabi toolchain:  can be downloaded from the the CodeSourcery ARM GNU/Linux toolchain page
- zImage: the Linux kernel created in my previous post here
- Busybox _install: the Busybox-based file system created in my previous post here

#Exporting root

We will create an exported folder with our user permissions: in this way the user is free to compile ARM software and copy it inside the shared folder. For QEMU to be able to read/write the folder, it should gain the same access permissions as our user.  This can be done using NFS functionality that maps the anonymous access to a custom user. Find your uid and gid numeric values with the id command; for example, mine are both 1000. Then from the command line, as root, run:
```sh
# mkdir -p /srv/nfs/
# chown 1000:1000 /srv/nfs
```

Then edit the file /etc/exports adding the following line:
```
/srv/nfs 127.0.0.1(rw,sync,no_subtree_check,all_squash,insecure,anonuid=1000,anongid=1000)
```
A reader pointed out that this method does not work for more complex filesystems. For things to work properly, the /etc/exports should contain instead:
```
/srv/nfs 127.0.0.1(rw,sync,no_subtree_check,no_root_squash,insecure)
```
The no_root_squash option is needed to access these files as root from the NFS client. Then the exported root filesystem should have normal ownership (uid=0 and gid=0 for example) so the root privileges are needed also to manipulate the files in /srv/nfs/.
Run the command as root:
```
# exportfs -av
```

You can check that the folder is effectively exported by running as root:
```sh
# mkdir ~/nfs_test
# mount localhost:/srv/nfs ~/nfs_test
# umount ~/nfs_test```

#Populating the root filesystem
Now we can populate the folder with an ARM root filesystem; we will use a Busybox root tree, that can be built by following my previous post here, and add some initialization files. Copy the _install directory that is generated with Busybox build into the NFS share, in order to have the /srv/nfs/_install folder containing the roof filesystem. Then create some missing folders as your user:

```
$ cd /srv/nfs/_install
$ mkdir -p dev proc sys etc/init.d
```
As said before, in order to make things work with more complex filesystems the files should have the correct ownership (uid=0 and gid=0 for example). To set these permissions a normal user would need root privileges.

Another possibility is to:

- run a fakeroot shell (fakeroot /bin/bash)
- populate the root filesystem with Busybox and other files as fake “root”
- create an archive with tar
- the real root user extracts the tar file into /srv/nfs/

The first program that the Linux kernel executes is /sbin/init; this program reads a setting file (if available) to know what to do. In our case we want the initialization simply to run a shell script and open a console. Create the file /srv/nfs/_install/etc/inittab with the following contents:

```
::sysinit:/etc/init.d/rcS
::respawn:/sbin/getty -n -l /bin/sh -L 115200 tty1 vt100
::restart:/sbin/init
```

The rcS file, as seen in my previous post about Busybox, can be used to populate the system directories. In our case, since the root tree is owned by a user in the host workstation, we cannot create device nodes; a possible workaround is to mount a temporary filesystem into the /dev directory. Create a new file called /srv/nfs/_install/etc/init.d/rcS with execute permissions (using “chmod +x“), that includes the following content:

```
mount -t proc none /proc
mount -t sysfs none /sys
mount -t ramfs ramfs /dev
/sbin/mdev -s
```

#Booting Linux
Now we are ready to test the boot process. Run the following command from the directory where the Linux kernel image is located:
```
$ qemu-system-arm -M versatilepb -m 128M -kernel zImage -append "root=/dev/nfs
nfsroot=10.0.2.2:/srv/nfs/_install rw ip=10.0.2.15::10.0.2.1:255.255.255.0
init=/sbin/init"
```

The kernel should perform the boot and then a shell should appear. You can try to create a file inside QEMU and see that it appears also in the host system; note that the file’s owner is the user set in the exports file. At the same time, you can write files into the exported folders and they can be accessed by the virtual guest ARM system as well.

To troubleshoot the NFS connection, the log file /var/log/syslog in the host system can be examined.
