# Open Chinese Convert 開放中文轉換

```
for doc in `find . -name '*.md'`
do
    doc=${doc#./}
    doc1="$doc""~"
    echo $doc
    echo "$doc1"
    opencc -i $doc -o $doc1 -c s2tw.json
    mv "$doc1" "$doc"
done
```
