#pragma once

namespace iphelper
{
	typedef enum tag_NSI_STORE {
		NsiPersistent,
		// Persists as long as module exists.
		NsiActive,
		NsiBoth,
		NsiCurrent,
		NsiBootFirmwareTable
	} NSI_STORE;

	typedef enum tag_NSI_SET_ACTION {
		NsiSetDefault,
		NsiSetCreateOnly,
		NsiSetCreateOrSet,
		NsiSetDelete,
		NsiSetReset,
		NsiSetClear,
		NsiSetCreateOrSetWithReference,
		NsiSetDeleteWithReference,
	} NSI_SET_ACTION;

	typedef enum tag_NDIS_NSI_OBJECT_INDEX
	{
		NdisNsiObjectInterfaceInformation,
		NdisNsiObjectInterfaceEnum,
		NdisNsiObjectInterfaceLookUp,
		NdisNsiObjectIfRcvAddress,
		NdisNsiObjectStackIfEntry,
		NdisNsiObjectInvertedIfStackEntry,
		NdisNsiObjectNetwork,
		NdisNsiObjectCompartment,
		NdisNsiObjectThread,
		NdisNsiObjectSession,
		NdisNsiObjectInterfacePersist,
		NdisNsiObjectCompartmentLookup,
		NdisNsiObjectInterfaceInformationRaw,
		NdisNsiObjectInterfaceEnumRaw,
		NdisNsiObjectStackIfEnum,
		NdisNsiObjectInterfaceIsolationInfo,
		NdisNsiObjectJob,
		NdisNsiObjectMaximum
	} NDIS_NSI_OBJECT_INDEX, * PNDIS_NSI_OBJECT_INDEX;

	NTSTATUS
		NsiSetAllParameters(
			__in NSI_STORE      Store,
			__in NSI_SET_ACTION Action,
			__in PNPI_MODULEID  ModuleId,
			__in ULONG          ObjectIndex,
			__in_bcount_opt(KeyStructLength) PVOID KeyStruct,
			__in ULONG          KeyStructLength,
			__in_bcount_opt(RwParameterStructLength) PVOID RwParameterStruct,
			__in ULONG          RwParameterStructLength
		);

	class set_tcp_entry
	{
		/**
		 * @brief struct to hold data for closing a TCP socket.
		 */
		struct delete_tcb_data {
			/**
			 * @brief union for local address, supporting IPv4 and IPv6.
			 */
			union
			{
				SOCKADDR_IN v4; /**< IPv4 local address. */
				SOCKADDR_IN6 v6; /**< IPv6 local address. */
			} local_address;

			/**
			 * @brief union for remote address, supporting IPv4 and IPv6.
			 */
			union
			{
				SOCKADDR_IN v4; /**< IPv4 remote address. */
				SOCKADDR_IN6 v6; /**< IPv6 remote address. */
			} remote_address;
		};


		// For details on NPI_MODULEID, refer to 
		// https://learn.microsoft.com/en-us/previous-versions/windows/hardware/device-stage/drivers/ff568813(v=vs.85)
		static constexpr BYTE npi_ms_tcp_moduleid[sizeof(NPI_MODULEID)] = {
			0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x4A, 0x00, 0xEB, 0x1A, 0x9B, 0xD4, 0x11, 0x91, 0x23,
			0x00, 0x50, 0x04, 0x77, 0x59, 0xBC
		};

		// A pointer to NsiSetAllParameters function
		NTSTATUS(WINAPI* nsi_set_all_parameters_)(DWORD store, DWORD action, LPVOID module_id, DWORD object_index,
			LPVOID key_struct, DWORD key_struct_length, LPVOID rw_parameter_struct,
			DWORD rw_parameter_struct_length) = nullptr;

		// Handle to the loaded NSI module
		HMODULE nsi_module_ = nullptr;

	public:
		set_tcp_entry()
		{
			// Load the NSI module
			nsi_module_ = LoadLibraryA("nsi.dll");
			if (nsi_module_ == nullptr)
			{
				throw std::runtime_error("Failed to load nsi.dll");
			}

			// Load the NsiSetAllParameters function
			nsi_set_all_parameters_ = reinterpret_cast<decltype(nsi_set_all_parameters_)>(reinterpret_cast<VOID*>(
				GetProcAddress(nsi_module_, "NsiSetAllParameters")));
			if (nsi_set_all_parameters_ == nullptr)
			{
				throw std::runtime_error("Failed to load NsiSetAllParameters");
			}
		}

		~set_tcp_entry()
		{
			if (nsi_module_ != nullptr)
			{
				FreeLibrary(nsi_module_);
				nsi_module_ = nullptr;
			}
		}

		set_tcp_entry(const set_tcp_entry& other) = delete;
		set_tcp_entry(set_tcp_entry&& other) = delete;
		set_tcp_entry& operator=(const set_tcp_entry& other) = delete;
		set_tcp_entry& operator=(set_tcp_entry&& other) = delete;

		/**
		 * @brief Sets the specified TCP connection entry in the TCP connection table.
		 *
		 * @param tcp_row A pointer to a MIB_TCPROW structure that specifies the TCP connection to set.
		 * @return DWORD - Returns ERROR_INVALID_PARAMETER if the state of the TCP connection is not 12 (TCP_STATE_ESTAB),
		 * otherwise returns the status of the nsi_set_all_parameters_ function call after setting the TCP connection entry.
		 */
		DWORD operator() (PMIB_TCPROW tcp_row) const
		{
			NTSTATUS status;
			delete_tcb_data data{};

			if (tcp_row->dwState != MIB_TCP_STATE_DELETE_TCB)
				return ERROR_INVALID_PARAMETER;

			data.local_address.v4.sin_family = AF_INET;
			data.local_address.v4.sin_addr.S_un.S_addr = tcp_row->dwLocalAddr;
			data.local_address.v4.sin_port = static_cast<USHORT>(tcp_row->dwLocalPort);

			data.remote_address.v4.sin_family = AF_INET;
			data.remote_address.v4.sin_addr.S_un.S_addr = tcp_row->dwRemoteAddr;
			data.remote_address.v4.sin_port = static_cast<USHORT>(tcp_row->dwRemotePort);

			status = nsi_set_all_parameters_(
				NsiActive,
				NsiSetCreateOrSet,
				const_cast<LPVOID>(reinterpret_cast<const void*>(&npi_ms_tcp_moduleid[0])),
				NdisNsiObjectJob,
				&data,
				sizeof(data),
				nullptr,
				0);

			return RtlNtStatusToDosError(status);
		}

		/**
		 * @brief Sets the specified TCP connection entry in the TCP connection table for IPv6 addresses.
		 *
		 * @param tcp_row A pointer to a MIB_TCP6ROW structure that specifies the TCP connection to set.
		 * @return DWORD - Returns ERROR_INVALID_PARAMETER if the state of the TCP connection is not 12 (TCP_STATE_ESTAB),
		 * otherwise returns the status of the nsi_set_all_parameters_ function call after setting the TCP connection entry.
		 */
		DWORD operator() (PMIB_TCP6ROW tcp_row) const
		{
			NTSTATUS status;
			delete_tcb_data data{};

			if (tcp_row->State != MIB_TCP_STATE_DELETE_TCB)
				return ERROR_INVALID_PARAMETER;

			data.local_address.v6.sin6_family = AF_INET6;
			data.local_address.v6.sin6_addr = tcp_row->LocalAddr;
			data.local_address.v6.sin6_port = static_cast<USHORT>(tcp_row->dwLocalPort);
			data.local_address.v6.sin6_scope_id = tcp_row->dwLocalScopeId;

			data.remote_address.v6.sin6_family = AF_INET6;
			data.remote_address.v6.sin6_addr = tcp_row->RemoteAddr;
			data.remote_address.v6.sin6_port = static_cast<USHORT>(tcp_row->dwRemotePort);
			data.remote_address.v6.sin6_scope_id = tcp_row->dwRemoteScopeId;

			status = nsi_set_all_parameters_(
				NsiActive,
				NsiSetCreateOrSet,
				const_cast<LPVOID>(reinterpret_cast<const void*>(&npi_ms_tcp_moduleid[0])),
				NdisNsiObjectJob,
				&data,
				sizeof(data),
				nullptr,
				0);

			return RtlNtStatusToDosError(status);
		}
	};
}
