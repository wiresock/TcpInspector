# TcpInspector

TcpInspector is a utility that provides functionality similar to the TCPView utility developed by Mark Russinovich. It allows you to list and drop TCP sessions for a target application based on a regular expression pattern. A notable enhancement includes its capability to manage IPv6 TCP sessions, even allowing for the termination of these sessions - an option that is not available in TCPView.

## Usage

```
TcpInspector <target_application_regex> <list_or_drop>
```

- `target_application_regex`: A regular expression pattern to match the target application.
- `list_or_drop`: Specifies whether to list or drop the TCP sessions. Use `list` to list the sessions and `drop` to drop the sessions.

## How it works

1. TcpInspector takes two command-line arguments: the target application regular expression and the command (list or drop).
2. If the required arguments are not provided, an error message is displayed, and the program exits.
3. The utility uses the `process_lookup` class from the `iphelper` library to retrieve information about TCP sessions for both IPv4 and IPv6 addresses.
4. If the command is `list`, TcpInspector lists the TCP sessions for both IPv4 and IPv6 addresses that match the target application regular expression.
5. If the command is `drop`, TcpInspector drops the TCP sessions for both IPv4 and IPv6 addresses that match the target application regular expression.
6. For each TCP session, the utility displays the local and remote IP addresses and ports.
7. If an error occurs during the session drop process, an error message is displayed.

## Demo Run

This section provides examples of using TcpInspector to list and drop TCP sessions.

### Listing TCP Sessions

Command to list TCP sessions for Firefox:
```
PS D:\projects\github.com\TcpInspector\x64\Release> ./tcpinspector f.* list
```

Output:
```
TCP IPv4 sessions for regex pattern: f.*:
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60783 <---> 172.64.154.86 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60633 <---> 80.68.253.2 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 127.0.0.1 : 60572 <---> 127.0.0.1 : 60571
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 127.0.0.1 : 60571 <---> 127.0.0.1 : 60572
C:\Program Files\Mozilla Firefox\firefox.exe :[4528]: 127.0.0.1 : 60574 <---> 127.0.0.1 : 60573
C:\Program Files\Mozilla Firefox\firefox.exe :[4528]: 127.0.0.1 : 60573 <---> 127.0.0.1 : 60574
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60580 <---> 140.82.113.26 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60593 <---> 20.88.154.143 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60602 <---> 34.107.243.93 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60603 <---> 34.107.243.93 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60632 <---> 80.68.253.2 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60634 <---> 34.120.208.123 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60680 <---> 185.199.110.154 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60688 <---> 185.199.110.133 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60773 <---> 188.114.99.229 : 443
C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60784 <---> 172.64.154.86 : 443
[...other session details...]
```

### Dropping TCP Sessions

Command to drop TCP sessions for Firefox:
```
PS D:\projects\github.com\TcpInspector\x64\Release> ./tcpinspector f.* drop
```

Output:
```
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60783 <---> 172.64.154.86 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60633 <---> 80.68.253.2 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 127.0.0.1 : 60572 <---> 127.0.0.1 : 60571
Failed to drop TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 127.0.0.1 : 60571 <---> 127.0.0.1 : 60572 Error code: 317
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[4528]: 127.0.0.1 : 60574 <---> 127.0.0.1 : 60573
Failed to drop TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[4528]: 127.0.0.1 : 60573 <---> 127.0.0.1 : 60574 Error code: 317
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60580 <---> 140.82.113.26 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60593 <---> 20.88.154.143 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60602 <---> 34.107.243.93 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60603 <---> 34.107.243.93 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60632 <---> 80.68.253.2 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60634 <---> 34.120.208.123 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60680 <---> 185.199.110.154 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60688 <---> 185.199.110.133 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60773 <---> 188.114.99.229 : 443
Dropped TCP session: C:\Program Files\Mozilla Firefox\firefox.exe :[7900]: 10.9.0.2 : 60784 <---> 172.64.154.86 : 443
[...other session details...]
```

This demonstration shows how TcpInspector effectively lists and drops TCP sessions based on the specified regular expression, providing clear insights into its operation.

## Credits

The approach used in TcpInspector is inspired by Mark Russinovich's TCPView utility.

## License

TcpInspector is released under the [MIT License](LICENSE).