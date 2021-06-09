#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TCPPort.h"

#define BACKLOG 5			//Maximum number of connection waiting to be accepted
#define TIMEOUT_SEC 30		//Timeout parameter for select() - in seconds
#define TIMEOUT_USEC 0		//Timeout parameter for select() - in micro seconds
#define MAX_BUFFER_SIZE 1024//Incoming data buffer size (maximum bytes per incoming data)
#define MAX_QUEUE_SIZE 100 //Incoming data buffer size (maximum bytes per incoming data)


clib::TCPPort::TCPPort()
{
	// Init variabes by default
    _data = new std::queue<incom_data>();
    _is_server = false;
    _timeout = 100;

    _port = 0;

    memset(&_addr, 0, sizeof(sockaddr_in));
    _addr.sin_addr.s_addr = inet_addr("127.0.0.1");
}


clib::TCPPort::~TCPPort()
{
	// Close socket
	Close();
}


bool clib::TCPPort::Open(uint16_t portNumber, uint16_t timeout, bool serverSocket)
{
    // Init params in Windows OS.
#if defined(linux) || defined(__linux) || defined(__linux__)
#else
    WSADATA wsaData = { 0 };
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
        return false;
#endif

    _port = portNumber;
    _running = true;
    _is_server = serverSocket;
    _timeout = timeout;
    start();
    return true;
}



void clib::TCPPort::SetIPAddr(const char* ip_addr)
{
    _addr.sin_addr.s_addr = inet_addr(ip_addr);
}


void clib::TCPPort::SetIPAddr(sockaddr_in & addr)
{
    _addr.sin_addr.s_addr = addr.sin_addr.s_addr;
}


int clib::TCPPort::ReadData(uint8_t * buf, uint32_t size)
{
    if (_is_server)
    {
        if (!_data->empty()){
            incom_data packet = _data->front();
            _data->pop();
            memcpy(buf, packet.data, packet.data_size);
            delete[] packet.data;
            return packet.data_size;
        }
        else{
            return -1;
        }
    }else
    {
       return socket_wrapper.receive_data(-1, size, (char*)buf);
    }
}


int clib::TCPPort::SendData(uint8_t * buf, uint32_t size)
{
    if (_is_server)
    {
        return socket_wrapper.respond_data((char*)buf,size);
    }else
    {
        return socket_wrapper.send_data(-1, size, (char*)buf);
    }
    return -1;
}


void clib::TCPPort::Close()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void clib::TCPPort::start()
{
    if (_is_server)
    {
        _running = true;
        _thread = std::thread(&TCPPort::run, this);
    }else
    {
        socket_wrapper.create_socket(_port, _addr, _timeout);
    }
}

void clib::TCPPort::stop()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

void clib::TCPPort::run()
{
    int listener_fd;		//Listener socket
    int max_fd;				//Hold the maximum file descriptor number from the active sockets

    //Wraps the sockets detailed implementation. See SocketWrapper for details.
    // 1.
    listener_fd = socket_wrapper.socket_bind(_port, _addr);

    // 2.
    socket_wrapper.start_listen(listener_fd, BACKLOG);
    std::cout << "Listening on port " << _port << "..." << std::endl;

    // 3.
    socket_wrapper.set_nonblock(listener_fd);

    // 4.
    socket_wrapper.create_sets(listener_fd);
    max_fd = listener_fd;

    // 5.
    while(_running){

        //5.1
        socket_wrapper.start_select(max_fd, TIMEOUT_SEC, TIMEOUT_USEC);
        for (int i=0; i < max_fd+1; i++){

            // 5.2
            if (i == listener_fd){
                max_fd = socket_wrapper.check_new_connection(listener_fd, max_fd);
            }

            //5.3
            else {
                incom_data packet;
                packet.data = new char[MAX_BUFFER_SIZE];
                int bytes = socket_wrapper.receive_data(i, MAX_BUFFER_SIZE, packet.data);
                if (bytes > 0) {
                    packet.data_size = bytes;
                    //Add data to the internal data queue
                    _data->push(packet);
                    if (_data->size() > MAX_QUEUE_SIZE)
                    {
                        incom_data old =_data->front();
                        _data->pop();
                        delete[] old.data;
                    }
                }else
                {
                    delete[] packet.data;
                }
            }
        }
    }

    // 6.
    socket_wrapper.close_all(max_fd);
}


bool clib::TCPPort::isOpen()
{
    return _running;
}

clib::TCPPort::SocketWrapper::SocketWrapper()
{
    //Clearing master and read sets
    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);
}

clib::TCPPort::SocketWrapper::~SocketWrapper()
{

}

int clib::TCPPort::SocketWrapper::create_socket(int port, sockaddr_in addr, uint16_t _timeout)
{
    if ((client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    int retVal = 0;
#if defined(linux) || defined(__linux) || defined(__linux__)
    // Init timeouts
    timeval timeparams;
    timeparams.tv_sec = _timeout / 1000;
    timeparams.tv_usec = _timeout * 1000; // Timeout in microseconds for read data from socket.
    if (timeout != 0) {
        retVal = setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeparams, sizeof(timeval));
        // Close socket in case error
        if (retVal < 0) {
            close(sock);
#else
    // Init timeouts
    DWORD timeout = _timeout; // Timeout in milliseconds for read data from socket.
    if (timeout != 0) {
        retVal = setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        // Close socket in case error
        if (retVal < 0) {
            closesocket(client_fd);
            WSACleanup();
#endif
            return false;
        }
    }

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr = addr.sin_addr;

    // Convert IPv4 and IPv6 addresses from text to binary form
//    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
//    {
//        printf("\nInvalid address/ Address not supported \n");
//        return -1;
//    }

    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    return true;
}

int clib::TCPPort::SocketWrapper::socket_bind(int port, sockaddr_in addr)
{
    int listener_fd;						//Listener socket descriptor
    struct addrinfo hints;					//Holds wanted settings for the listener socket
    struct addrinfo *server_info_list;		//A list of possible information to create socket

    //All the other fields in the addrinfo struct (hints) must contain 0
    memset(&hints, 0, sizeof hints);

    //Initialize connection information
    hints.ai_family = AF_UNSPEC;			//Supports IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;		//Reliable Stream (TCP)
    hints.ai_flags = AI_PASSIVE;			//Assign local host address to socket

    //Get address information
    int err;
    char *ip = inet_ntoa(addr.sin_addr);
    err = getaddrinfo(ip, std::to_string(port).c_str(), &hints, &server_info_list);
    if (err != 0){
        std::cerr << "getaddrinfo: " << gai_strerror(err) << std::endl;
#if defined(linux) || defined(__linux) || defined(__linux__)
            close(listener_fd);
#else
            closesocket(listener_fd);
            WSACleanup();
#endif
        return false;
    }

    //Go over list and try to create socket and bind
    addrinfo* p;
    for(p = server_info_list;p != NULL; p = p->ai_next) {

        //Create the socket - system call that returns the file descriptor of the socket
        listener_fd = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
        if (listener_fd == -1) {
            continue; //try next
        }

        //Make sure the port is not in use. Allows reuse of local address (and port)
        int retVal;
#if defined(linux) || defined(__linux) || defined(__linux__)
        int trueflag = 1;
        retVal = setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &trueflag, sizeof trueflag);
        if (retVal < 0)
        {
            close(listener_fd);
            return false;
        }
#else
        const char trueflag = 1;
        retVal = setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &trueflag, sizeof(trueflag));
        // Close socket in case error
        if (retVal < 0) {
            perror("setsockopt");
            closesocket(listener_fd);
            WSACleanup();
            return false;
        }
#endif
        //Bind socket to specific port (p->ai_addr holds the address and port information)
        if (bind(listener_fd, p->ai_addr, p->ai_addrlen) == -1) {
#if defined(linux) || defined(__linux) || defined(__linux__)
            close(listener_fd);
#else
            closesocket(listener_fd);
#endif
            continue; //try next
        }

        break; //success
    }

    //No one from the list succeeded - failed to bind
    if (p == NULL)  {
            std::cerr << "failed to bind" << std::endl;
#if defined(linux) || defined(__linux) || defined(__linux__)
            close(listener_fd);
#else
            closesocket(listener_fd);
            WSACleanup();
#endif
        return false;
    }

    //If we got here we successfully created a socket to listen on
    //Free list as we no longer need it
    freeaddrinfo(server_info_list);

    //return the listener socket descriptor
    return listener_fd;
}

void clib::TCPPort::SocketWrapper::start_listen(int listener_fd, int backlog)
{
    //Listen on the given port for incoming connections (Maximum BACKLOG waiting connections in queue)
    if (listen(listener_fd, backlog) == -1){
        perror("listen");
#if defined(linux) || defined(__linux) || defined(__linux__)
            close(listener_fd);
#else
            closesocket(listener_fd);
            WSACleanup();
#endif
    }
}

void clib::TCPPort::SocketWrapper::set_nonblock(int socket)
{
#if defined(linux) || defined(__linux) || defined(__linux__)
    int flags;
    // save current flags
    flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1)
        perror("fcntl");
    // set socket to be non-blocking
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1)
        perror("fcntl");
#else
    u_long mode = 1;  // 1 to enable non-blocking socket
    ioctlsocket(socket, FIONBIO, &mode);
#endif

}

void clib::TCPPort::SocketWrapper::create_sets(int listener_fd)
{
    //clear master and read sets
    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);
    // add the listener socket descriptor to the master set
#if defined(linux) || defined(__linux) || defined(__linux__)
    FD_SET(listener_fd, &master_fds);
#else
    FD_SET((SOCKET)listener_fd, &master_fds);
#endif

}

void clib::TCPPort::SocketWrapper::start_select(int max_fd, int timeout_sec, int timeout_usec)
{
    //Set timeout values (for waiting on select())
    struct timeval timeout = {timeout_sec, timeout_usec};
    //Copy all available (open) sockets to the read set
    read_fds = master_fds;

    //Select - 	modifies read_fds set to show which sockets are ready for reading
    //			if none are ready, it will timeout after the given timeout values
    int sel = select(max_fd+1, &read_fds, NULL, NULL, &timeout);
    if (sel == -1) {
        perror("select");
    }
}

int clib::TCPPort::SocketWrapper::check_new_connection(int listener_fd, int max_fd)
{
    //Check if listener socket is in read set (has changed and has an incoming connection to accept)
    if (FD_ISSET(listener_fd,&read_fds))
    {
            struct sockaddr_storage their_addr;
            socklen_t addr_size = sizeof their_addr;

            //Accept the incoming connection, save the socket descriptor (client_fd)
            client_fd = accept(listener_fd, (struct sockaddr *)&their_addr, &addr_size);
            if (client_fd == -1){
                perror("accept");
            }
            else{ //If connection accepted
                //Set this socket to be non-blocking
                set_nonblock(client_fd);
                //Add socket to the master set
#if defined(linux) || defined(__linux) || defined(__linux__)
                FD_SET(client_fd, &master_fds);
#else
                FD_SET((SOCKET)client_fd, &master_fds);
#endif
                //Update max_fd
                if (client_fd > max_fd)
                    max_fd = client_fd;
                //Print incoming connection
                if (their_addr.ss_family == AF_INET){
                    //IPv4
                    char ip_as_string[INET_ADDRSTRLEN];
                    inet_ntop(their_addr.ss_family,&((struct sockaddr_in *)&their_addr)->sin_addr,ip_as_string, INET_ADDRSTRLEN);
                    std::cout << "New connection from " << ip_as_string << " on socket " << client_fd << std::endl;
                } else if(their_addr.ss_family == AF_INET6){
                    //IPv6
                    char ip_as_string[INET6_ADDRSTRLEN];
                    inet_ntop(their_addr.ss_family,&((struct sockaddr_in6 *)&their_addr)->sin6_addr,ip_as_string, INET6_ADDRSTRLEN);
                    std::cout << "New connection from " << ip_as_string << " on socket " << client_fd << std::endl;
                }
            }
    }
    return max_fd;
}

int clib::TCPPort::SocketWrapper::receive_data(int client, int buffer_size, char* data)
{
    if (client >= 0)
    {
        //Check if socket is in read set (has data or has closed the connection)
        if (FD_ISSET(client, &read_fds))
        {
            int bytes;

            //Receive data
            bytes = recv(client, data, buffer_size, 0);

            //Connection has been closed by client
            if (bytes <= 0)
            {
                if (bytes == -1)
                    perror("recv");
                //close socket
#if defined(linux) || defined(__linux) || defined(__linux__)
                close(client);
#else
                closesocket(client);
#endif
                //remove from master set
#if defined(linux) || defined(__linux) || defined(__linux__)
                FD_CLR(client_fd, &master_fds);
#else
                FD_CLR((SOCKET)client, &master_fds);
#endif
                return 0;
            }
            //Some data received - bytes > 0
            return bytes;
        }
    }else
    {
        int bytes;
        bytes = recv(client_fd, data, buffer_size, 0);
        return bytes;
    }
    return -1;
}

int clib::TCPPort::SocketWrapper::send_data(int client, int data_size, char *data)
{
    if (client < 0)
        return send(client_fd , data , data_size , 0 );
    else
        return send(client , data , data_size , 0 );
}

int clib::TCPPort::SocketWrapper::respond_data(char* data, int data_size)
{
    //Send "data" to given client
    int ret = send(client_fd, data, data_size, 0);
    if (ret == -1) {
        perror("send");
    }
    return ret;
}

void clib::TCPPort::SocketWrapper::close_all(int max_fd)
{
    //Close all socket descriptors, this will terminate all connections
    for (int i=0; i < max_fd+1; i++){
        //If socket is in the master set it means it is still open - so close it
        if (FD_ISSET(i, &master_fds))
#if defined(linux) || defined(__linux) || defined(__linux__)
            close(i);
#else
            closesocket(i);
#endif
    }

#if defined(linux) || defined(__linux) || defined(__linux__)
#else
        WSACleanup();
#endif
}

