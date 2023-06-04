#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <WinSock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <Netiodef.h>
#include <winternl.h>
#include <gsl/gsl>
#include "netlib/net/ip_address.h"
#include "netlib/net/ip_endpoint.h"
#include "netlib/iphelper/process_lookup.h"
#include "netlib/iphelper/set_tcp_entry.h"

int wmain(const int argc, wchar_t* argv[]) {
    // Check if the required arguments are provided
    if (argc < 3) {
        std::wcerr << L"Usage: TcpDrop <target_application_regex> <list_or_drop>\n";
        return 1;
    }

    iphelper::process_lookup<net::ip_address_v4>::get_process_helper().actualize(true, false);
	iphelper::process_lookup<net::ip_address_v6>::get_process_helper().actualize(true, false);

    // Get the target application regex from the first argument
    const std::wstring target_app_regex_str(argv[1]);
    const std::wregex target_app_regex(target_app_regex_str, std::regex_constants::icase);

    auto tcp_sessions_v4 = iphelper::process_lookup<net::ip_address_v4>::get_process_helper().get_tcp_sessions_for_process(target_app_regex);
	auto tcp_sessions_v6 = iphelper::process_lookup<net::ip_address_v6>::get_process_helper().get_tcp_sessions_for_process(target_app_regex);

    // Get the command (list or drop) from the second argument
    if(const std::wstring command(argv[2]); command == L"list")
    {
        std::wstringstream oss;
        std::ranges::for_each(tcp_sessions_v4, [&oss](auto&& entry)
        {
            oss << std::wstring(entry.local.ip) << L" : " << entry.local.port <<
                L" <---> " << std::wstring(entry.remote.ip) << L" : " << entry.remote.port << std::endl;
        });
        std::wcout << L"TCP IPv4 sessions for regex pattern: " << target_app_regex_str << L":\n" << oss.str();

		oss.str(L"");

		std::ranges::for_each(tcp_sessions_v6, [&oss](auto&& entry)
			{
				oss << std::wstring(entry.local.ip) << L" : " << entry.local.port <<
					L" <---> " << std::wstring(entry.remote.ip) << L" : " << entry.remote.port << std::endl;
			});
		std::wcout << L"TCP IPv6 sessions for regex pattern: " << target_app_regex_str << L":\n" << oss.str();
    }
	else if (command == L"drop")
	{
		try {
			iphelper::set_tcp_entry drop_tcp_entry;

			// Process IPv4 sessions
			MIB_TCPROW row_v4;
			std::ranges::for_each(tcp_sessions_v4, [&row_v4, &drop_tcp_entry](auto&& entry) {
				row_v4.dwLocalAddr = entry.local.ip.S_un.S_addr;
				row_v4.dwLocalPort = htons(entry.local.port);
				row_v4.dwRemoteAddr = entry.remote.ip.S_un.S_addr;
				row_v4.dwRemotePort = htons(entry.remote.port);
				row_v4.dwState = MIB_TCP_STATE_DELETE_TCB;
				if (const auto result = drop_tcp_entry(&row_v4); result != NO_ERROR) {
					std::wcerr << L"Failed to drop TCP session: " << std::wstring(entry.local.ip) << L" : " << entry.local.port <<
						L" <---> " << std::wstring(entry.remote.ip) << L" : " << entry.remote.port << L" Error code: " << result << std::endl;
				}
				else {
					std::wcout << L"Dropped TCP session: " << std::wstring(entry.local.ip) << L" : " << entry.local.port <<
						L" <---> " << std::wstring(entry.remote.ip) << " : " << entry.remote.port << std::endl;
				}
				});

			// Process IPv6 sessions
			MIB_TCP6ROW row_v6;
			std::ranges::for_each(tcp_sessions_v6, [&row_v6, &drop_tcp_entry](auto&& entry) {
				row_v6.LocalAddr = entry.local.ip;
				row_v6.dwLocalScopeId = entry.local.scope_id.value_or(0);
				row_v6.dwLocalPort = htons(entry.local.port);
				row_v6.RemoteAddr = entry.remote.ip;
				row_v6.dwRemoteScopeId = entry.remote.scope_id.value_or(0);
				row_v6.dwRemotePort = htons(entry.remote.port);
				row_v6.State = MIB_TCP_STATE_DELETE_TCB;
				if (const auto result = drop_tcp_entry(&row_v6); result != NO_ERROR) {
					std::wcerr << L"Failed to drop TCP session: " << std::wstring(entry.local.ip) << L" : " << entry.local.port <<
						L" <---> " << std::wstring(entry.remote.ip) << L" : " << entry.remote.port << L" Error code: " << result << std::endl;
				}
				else {
					std::wcout << L"Dropped TCP session: " << std::wstring(entry.local.ip) << L" : " << entry.local.port <<
						L" <---> " << std::wstring(entry.remote.ip) << L" : " << entry.remote.port << std::endl;
				}
				});
		}
		catch (const std::exception& e) {
			std::wcerr << L"Failed to drop TCP sessions: " << e.what() << std::endl;
		}
	}
	else
	{
		std::wcerr << L"Usage: TcpDrop <target_application_regex> <list_or_drop>\n";
		return 1;
	}

    return 0;
}


