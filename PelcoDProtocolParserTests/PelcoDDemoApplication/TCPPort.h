#pragma once
#include <stddef.h>
#include <stdint.h>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <queue>
#include <string>
#include <iostream>
#include <vector>
#if defined(linux) || defined(__linux) || defined(__linux__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h>
#include "arpa/inet.h"
#else
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#pragma comment(lib, "Ws2_32.lib")
#endif


namespace clib
{

	/**
     * @brief TCP socket class.
	 */
    class TCPPort
	{
	public:

		/**
		 * @brief Class constructor.
		 */
        TCPPort();

		/**
		 * @brief Class destructor.
		 */
        ~TCPPort();

		/**
         * @brief Open - Method to open TCP socket.
         * @param portNumber TCP port number for TCP socket.
		 * @param timeout Wait data timeout in milliseconds.
         * @param isServer If set to TRUE - the server socket type will be opened,
         * otherwise - will be opened the client socket type.
		 * @return TRUE in case success or FALSE in case any errors.
		 */
        bool Open(uint16_t portNumber, uint16_t timeout, bool isServer);

		/**
         * @brief SetIPAddr - Method to set host IP address for server socet
         * type (default 127.0.0.1 Host IP) and destination IP address for
         * client socket type (default 255.255.255.255 destination IP).
		 * @param dstIP IP address string.
		 */
        void SetIPAddr(const char *ip_addr);

		/**
         * @brief SetIPAddr - Method to set host IP address for server socet
         * type (default 127.0.0.1 Host IP) and destination IP address for
         * client socket type (default 255.255.255.255 destination IP).
		 * @param dstAddr IP address structure.
		 */
        void SetIPAddr(sockaddr_in &addr);

		/**
		 * @brief Method to read data.
		 * @param buf pointer to data buffer to copy data.
		 * @param size size of data buffer.
         * @param conn_id Сonnection number from which the data was received
		 * @return Number of read bytes or return -1 in case error.
		 */
        int ReadData(uint8_t *buf, uint32_t size, int& conn_id);

		/**
		 * @brief Method to send data.
		 * @param buf pointer to data to send.
		 * @param size size of data to send.
         * @param conn_id Сonnection number where data needs to be sent. Only
         * for server socket. For the client type, conn_id should be set to -1.
		 * @return Number of bytes sent or return -1 if UDP socket not open.
		 */
        int SendData(uint8_t *buf, uint32_t size, int conn_id = -1);

		/**
		 * @brief Method to check if UDP socket open.
		 * @return TRUE if socke open or FALSE.
		 */
		bool isOpen(); 

		/**
		 * \brief Methos to close UDP socket.
		 */
		void Close();

	private:
        class SocketWrapper {
        public:
            /**
             * Constructor.
             * Initializes relevant fd_sets
             */
            SocketWrapper();
            /**
             * Destructor. Nothing to destruct.
             */
            virtual ~SocketWrapper();
            /**
             * This method get local address information,
             * creates a listener sockets and binds it to the given port.
             * Returns the listener socket file descriptor.
             */
            int create_socket(int, sockaddr_in, uint16_t);
            /**
             * This method get local address information,
             * creates a listener sockets and binds it to the given port.
             * Returns the listener socket file descriptor.
             */
            int socket_bind(int, sockaddr_in);
            /**
             * This method receives listener socket and backlog number.
             * It sets the socket to start listening for incoming connections
             * with maximum backlog number of incoming connections waiting to be accepted.
             */
            void start_listen(int,int);
            /**
             * This method receives a socket and sets it to be non-blocking.
             */
            void set_nonblock(int);
            /**
             * This method initializes the relevant sets and adds the given socket to the master set.
             */
            void create_sets(int);
            /**
             * This method receives the maximum socket descriptor number (max_fd) and timeout values.
             * It checks if sockets descriptors (up to max_fd) are ready for read / write / accept connections,
             * and sets the read_fds set accordingly.
             * If none are ready it will timeout after the given timeout values.
             */
            void start_select(int,int,int);
            /**
             * This method a listener socket and the maximum socket descriptor number (max_fd).
             * It checks if the socket is in the read_fds set (meaning that it has a incoming connection waiting to be accepted).
             * If it is, it accepts the incoming connection and adding its socket descriptor to the master set.
             * Updates and returns the new max_fd (if changed).
             */
            int check_new_connection(int,int);
            /**
             * This method receives a socket, buffer size, and a reference to data string.
             * It checks if the socket is in the read_fds set (meaning it has incoming data to receive or it has ended the connection).
             * If the socket has ended the connection, it closes the file descriptor and delete it from the master set.
             * If the socket has incoming data, it reads it (up to buffer_size) and assign it to the given string.
             * It returns the number of bytes received or -1 if the socket is not in the read_fds set.
             */
            int receive_data(int buffer_size, char* data, int& conn_id);
            /**
             * This method receives a socket, buffer size, and a reference to data string.
             * It checks if the socket is in the read_fds set (meaning it has incoming data to receive or it has ended the connection).
             * If the socket has ended the connection, it closes the file descriptor and delete it from the master set.
             * If the socket has incoming data, it reads it (up to buffer_size) and assign it to the given string.
             * It returns the number of bytes received or -1 if the socket is not in the read_fds set.
             */
            int send_data(int data_size, char* data, int conn_id = -1);

            /**
             * This method send "OK" to the given socket.
             */
            int respond_data(char* data, int data_size, int conn_id);
            /**
             * This method closes all active sockets.
             * The active sockets are the one that are in the master_fd set.
             */
            void close_all(int);
        private:
            int client_fd;
            fd_set read_fds;		//Socket descriptor set that holds the sockets that are ready for read
            fd_set master_fds;		//Socket descriptor set that hold all the available (open) sockets
        };

        SocketWrapper socket_wrapper;

        ///< TCP port
        uint16_t _port;
        struct sockaddr_in _addr;
        bool _is_server;
        uint16_t _timeout;
        std::thread _thread;				//Internal thread, this is in order to start and stop the thread from different class methods
        struct incom_data
        {
            char* data;
            uint16_t data_size;
            uint16_t cliend_id;
        };

        std::queue<incom_data>* _data;		//Queue for saving incoming data
        // Flag for starting and terminating the main loop
        bool _running;


        /**
         * Starts the internal thread that executes the main routine (run()).
         */
        bool start();
        /**
         * Stops the main routine and the internal thread.
         */
        void stop();
        /**
         * This is the main routine of this class.
         * It accepts incoming connection and receives incoming data from these connections.
         * It is private because it is only executed in a different thread by start() method.
         */
        void run();


    };

}

