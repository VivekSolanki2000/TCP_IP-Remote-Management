# TCP_IP-Remote-Management
 TCP_IP-Remote-Management

get-process     return : {pid,resp}
get-mem <process-name> || get-mem <process-id>   VSZ + RSS  using  ps -p 275535 -o %mem,vsz,rss return : {pid,resp}
get-cpu-usage <process-name> || get-cpu-usage <process-id> using pipe & .sh  (https://www.baeldung.com/linux/total-process-cpu-usage) return : {pid,resp}
get-ports-used <process-name> || get-ports-used <process-id>  ls /proc/pid/fd cmp with cat /proc/net/tcp && cat /proc/net/udp return : {pid,resp}
kill <process-name> || kill <process-id> -9


help
history

class MessageHeader
{
    selfInfo Struct
    int messagesize
    cmd
    flg For name and pid
    process id /process name
    
};

class resp
{
    {pid,resp}
    
};

EOFMSG