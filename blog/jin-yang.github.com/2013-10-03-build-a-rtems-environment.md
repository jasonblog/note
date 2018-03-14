---
title: How to run RTEMS on QEMU
layout: post
comments: true
category: rtems
keywords: rtems,qemu
description: The Real-Time Executive for Multiprocessor Systems or [RTEMS](http://www.rtems.org/ "Go to offical RTEMS site") is a open source fully featured Real Time Operating System. More features you can get from the offical web-site. Here, we try to build a simulation environments for RTEMS based on QEMU and platform x86 the most common one we used. This will be done form scratch, hope you can enjoy that.
---

The Real-Time Executive for Multiprocessor Systems or [RTEMS](http://www.rtems.org/ "Go to offical RTEMS site") is a open source fully featured Real Time Operating System. More features you can get from the offical web-site.

Here, we try to build a simulation environments for RTEMS based on QEMU and platform x86 the most common one we used. This will be done form scratch, hope you can enjoy that.

<!-- more -->

## Prepare

This is test on the Ubuntu 13.10, maybe there are some bugs on other platforms, but I think most of Linux distributions will work.

The environment is based on QEMU, when start RTEMS we use the Grub2, so you should make sure that both have been installed. On most platform the Grub2 has installed, maybe only QEMU should to be intalled manually. You can install QEMU on Ubuntu through command `sudo apt-get install qemu`. Maybe what you install is not the lastest version, you could install this through source code. That's a little complicated we will not discuss that here.

Actually, I build the QEMU from source code and intsall it to /opt/qemu directory. You can test QEMU through `qemu-system-i386 -version`, if QEMU works, the version informatin will be returned.   

## Make a bootable image

We will build a bootable image based on grub2, in fact I highly recommand you use [this image](http://www.rtems.org/ftp/pub/rtems/people/chrisj/grub/rtems-boot.img) which is based on Grub-0.97 to start the RTEMS. Grub2 has many fascinating functions, those make it become slower when start. The purpose of this article is to show you how a bootable image build, since when we try to use grub-0.97 to do this bugs occured.

1. Create an actual empty disk image about 15MB through the following command, you could create bigger one, but I think this is big enough.   
	`dd if=/dev/zero of=rtems-boot.img bs=512 count=32130`
2. Most of the following steps should use root account, so change to root user. And then associate the disk image we created before with a loop devices which is not used.   
	`losetup /dev/loop0 rtems-boot.img`
3. Make the partition table, partition and set it bootable.   
	`fdisk /dev/loop0`   
	Within the fdisk prompt, type "o" to create a partition table, "n" to create a new table ("p" to create a primary partition and make it the first partition with "1"), "a" to toggle a bootable flag and "w" to save the changes.
4. Notify the kernel about the new partion. Map the partition form the image file.   
	`kpartx -av /dev/loop0`   
   Then we should find the first unused loop device, assume this is /dev/loop1.   
	`losetup -f`
5. And create a loop device for this partition, just like /dev/sda and /dev/sda0.   
	`losetup /dev/loop1 /dev/mapper/loop0p1`
6. OK, now we can make an ext2 filesystem on the first partition and mount.   
	`mkfs.ext2 /dev/loop1 && mount /dev/loop1 /mnt`
7. Create a directory and a file.  
	`mkdir -p /mnt/boot/grub && echo 'source (hd1,msdos1)/grub.cfg' > /mnt/boot/grub/grub.cfg`
8. Finally, we can install the grub2 to the disk image.  
	`grub-install --allow-floppy --boot-directory=/mnt/boot --disk-module=biosdisk --modules="ext2 part_msdos" --force /dev/loop0`
9. Do some other thing.  
	`umount /mnt && losetup -d /dev/loop1 && kpartx -dv /dev/loop0 && losetup -d /dev/loop0`

Until now, I think this hard disk image can work, try it now through command `qemu-system-i386 -m 128 -boot b -hda rtems-boot.img -hdb fat:. -serial stdio -no-reboot`. It may take some time to start Grub2, that's why Grub0.97 is recommended.

![If the picture doesn't exist]({{ site.url }}/images/rtems/grub2/rtems_grub2_prepared.png "A bootable hard disk image has prepared"){: .pull-center}

## Start RTEMS
Assume you have built the compile environment for RTEMS. Copy the executable binary examples file, such as ticker.exe to examples folder, just like the following.

![If the picture doesn't exist]({{ site.url }}/images/rtems/grub2/rtems_grub2_files.png "The files in our folder"){: .pull-center}

The 'grub.cfg' file is a configuration file for grub2, you can add the following contents to it.

{% highlight text %}
set default="0"
set timeout=10
 
menuentry "RTEMS Ticker" {
     set root='hd1,msdos1'
         multiboot (hd1,msdos1)/examples/ticker.exe
}
{% endhighlight %}

Then start RTEMS with the following command.

`qemu-system-i386 -m 128 -boot b -hda rtems-boot.img -hdb fat:. -serial stdio -no-reboot`

and you will get the screen like

![If the picture doesn't exist]({{ site.url }}/images/rtems/grub2/rtems_grub2_startup.png "Start the grub2, you will get a startup screen"){: .pull-center}

press enter to execute the ticker.exe, and you get that. Congratulations ^_^

![If the picture doesn't exist]({{ site.url }}/images/rtems/grub2/rtems_grub2_execute.png "Execute the RTEMS binary file"){: .pull-center}

## Reference
1. [Building Gurb](http://www.rtems.org/wiki/index.php/Building_Grub)   
	How to build a bootable image.
2. [QEMU](http://www.rtems.org/wiki/index.php/QEMU)   
	Run RTESM on QEMU, including TCP/IP stack, debugging, USB, and also a bootable image based on Grub-legacy or Grub-0.97.
3. [Quick Start](http://www.rtems.org/wiki/index.php/Quick_Start)   
	How run RTEMS on sparc platform.
4. [Boot Image](/reference/rtems/rtems-boot.img "This is a copy")   
    This is just a copy from [*http://www.rtems.org/ftp/pub/rtems/people/chrisj/grub/rtems-boot.img*](http://www.rtems.org/ftp/pub/rtems/people/chrisj/grub/rtems-boot.img).

