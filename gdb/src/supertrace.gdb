set pagination off

b main
r

python
sys.path.insert(0, './')
import supertrace 
end

breakall
set $x=$lastbp()
commands 1-$x
    silent
    supertrace
    cont
end
