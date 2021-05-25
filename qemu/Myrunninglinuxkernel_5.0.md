# Myrunninglinuxkernel_5



```bash
git clone https://github.com/shihyu/Myrunninglinuxkernel_5.0
cd Myrunninglinuxkernel_5.0
./run_debian_arm64.sh build_kernel
sudo ./run_debian_arm64.sh build_rootfs

sudo ./run_debian_arm64_ssh.sh run
```



```
root
123

apt-get update
apt install openssh-server vim  haveged silversearcher-ag  htop


systemctl status ssh
```

```bash
benshushu:~# systemctl status ssh
● ssh.service - OpenBSD Secure Shell server
   Loaded: loaded (/lib/systemd/system/ssh.service; enabled; vendor preset: enab
   Active: active (running) since Tue 2021-05-25 08:29:10 UTC; 1h 54min ago
     Docs: man:sshd(8)
           man:sshd_config(5)
 Main PID: 305 (sshd)
    Tasks: 1 (limit: 4577)
   Memory: 6.1M
   CGroup: /system.slice/ssh.service
           └─305 sshd: /usr/sbin/sshd -D [listener] 0 of 10-100 startups

May 25 08:29:10 benshushu sshd[305]: Server listening on 0.0.0.0 port 22.
May 25 08:29:10 benshushu sshd[305]: Server listening on :: port 22.
May 25 08:29:10 benshushu systemd[1]: Started OpenBSD Secure Shell server.
May 25 09:19:53 benshushu sshd[629]: Accepted password for benshushu from 10.0.2
May 25 09:19:53 benshushu sshd[629]: pam_unix(sshd:session): session opened for 
May 25 09:54:57 benshushu sshd[717]: Accepted password for benshushu from 10.0.2
May 25 09:54:57 benshushu sshd[717]: pam_unix(sshd:session): session opened for 
May 25 09:54:58 benshushu sshd[717]: pam_env(sshd:session): Unable to open env f
May 25 10:04:20 benshushu sshd[1881]: Accepted password for benshushu from 10.0.
May 25 10:04:20 benshushu sshd[1881]: pam_unix(sshd:session): session opened for

```



```bash
在 benshushu 帳號 mkdir ~/.ssh
ssh-keygen
host 跟 qemu 互加  authorized_keys

ssh benshushu@localhost -p 6666 -v
```





## run_debian_arm64_ssh.sh

```bash
run_qemu_debian(){
		qemu-system-aarch64 -m 8192 -cpu cortex-a57 -M virt,gic-version=3,its=on,iommu=smmuv3\
			-nographic $SMP -kernel arch/arm64/boot/Image \
			-append "noinintrd sched_debug root=/dev/vda rootfstype=ext4 rw crashkernel=256M loglevel=8" \
			-drive if=none,file=$rootfs_image,id=hd0 \
			-device virtio-blk-device,drive=hd0 \
			--fsdev local,id=kmod_dev,path=./kmodules,security_model=none \
			-device virtio-9p-pci,fsdev=kmod_dev,mount_tag=kmod_mount\
            -nic user,hostfwd=tcp::6666-:22\
            -net nic\
			$DBG

}
```

