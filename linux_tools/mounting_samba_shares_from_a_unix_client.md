# Mounting samba shares from a unix client


```sh
sudo mkdir /mnt/smb
```

```sh
sudo mount.cifs //10.46.8.21/gpd/Project/VR /mnt/smb/ -osec=ntlmv2,domain=HTCTAIPEI,username=jason_yao,password=xxxxx
```

```sh
sudo umount /mnt/smb/
```