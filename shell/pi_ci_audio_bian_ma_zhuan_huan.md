# 批次Audio編碼轉換


```sh
if [ "$1" != "" ]; then                                                                                                                                                        
	file="$1"
else
	file="*.wav"
fi

echo  "$file"

for filename in `find . -name "$file"` 
do  
    extension="${filename##*.}"
    tmp_file="tmp_file.""$extension"
	echo $filename
	echo $tmp_file 
    ffmpeg -i $filename  $tmp_file
	mv "$tmp_file" "$filename"
done 

```