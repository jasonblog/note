gitbook build
cp -a _book /tmp/
git add .
git commit -m 'update'
git push
git checkout gh-pages
cp /tmp/_book/* .
git checkout master
