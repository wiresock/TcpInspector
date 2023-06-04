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

## Example

To list TCP sessions for an application named `firefox.exe` using TcpInspector, run the following command:

```
TcpInspector firefox.exe list
```

To drop TCP sessions for the same application, use the following command:

```
TcpInspector firefox.exe drop
```

## Credits

The approach used in TcpInspector is inspired by Mark Russinovich's TCPView utility.

## License

TcpInspector is released under the [MIT License](LICENSE).