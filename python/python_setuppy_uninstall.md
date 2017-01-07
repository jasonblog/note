# python setup.py uninstall


```sh
To record list of installed files, you can use:

python setup.py install --record files.txt
Once you want to uninstall you can use xargs to do the removal:

cat files.txt | xargs rm -rf
```