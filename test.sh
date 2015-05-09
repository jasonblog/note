for doc in `find . -name '*.md'`
do
    doc=${doc#./}
    doc1="$doc""~"
    echo $doc
    echo "$doc1"
    opencc -i $doc -o $doc1 
    mv "$doc1" "$doc"
done
