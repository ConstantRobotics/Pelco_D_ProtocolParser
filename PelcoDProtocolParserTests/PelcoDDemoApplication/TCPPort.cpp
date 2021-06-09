#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TCPPort.h"

#define BACKLOG 5			//Maximum number of connection waiting to be accepted
#define TIMEOUT_SEC 30		//Timeout parameter for select() - in seconds
#define TIMEOUT_USEC 0		//Timeout parameter for select() - in micro seconds
#define MAX_BUFFER_SIZE 1024//Incoming data buffer size (maximum bytes per incoming data)
#define MAX_QUEUE_SIZE 100 //Incoming data buffer size (maximum bytes per incoming data)


#if defined(linux) || defined(__linux) || defined(__linux__)

#else
#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

static char	*inet_ntop4(const u_char *src, char *dst, socklen_t size);
static char	*inet_ntop6(const u_char *src, char *dst, socklen_t size);

/* char *
 * inet_ntop(af, src, dst, size)
 *	convert a network format address to presentation format.
 * return:
 *	pointer to presentation format address (`dst'), or NULL (see errno).
 * author:
 *	Paul Vixie, 1996.
 */
char *
inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    switch (af) {
    case AF_INET:
        return (inet_ntop4( (unsigned char*)src, (char*)dst, size)); // ****
    case AF_INET6:
        return (char*)(inet_ntop6( (unsigned char*)src, (char*)dst, size)); // ****
    default:
        // return (NULL); // ****
        return 0 ; // ****
    }
    /* NOTREACHED */
}

/* const char *
 * inet_ntop4(src, dst, size)
 *	format an IPv4 address
 * return:
 *	`dst' (as a const)
 * notes:
 *	(1) uses no statics
 *	(2) takes a u_char* not an in_addr as input
 * author:
 *	Paul Vixie, 1996.
 */
static char *
inet_ntop4(const u_char *src, char *dst, socklen_t size)
{
    static const char fmt[128] = "%u.%u.%u.%u";
    char tmp[sizeof "255.255.255.255"];
    int l;

    // l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]); // ****
    l = std::sprintf( tmp, fmt, src[0], src[1], src[2], src[3] ); // **** vc++ does not have snprintf
    if (l <= 0 || (socklen_t) l >= size) {
        return (NULL);
    }
    memcpy(dst, tmp, size);
    return (dst);
}

/* const char *
 * inet_ntop6(src, dst, size)
 *	convert IPv6 binary address into presentation (printable) format
 * author:
 *	Paul Vixie, 1996.
 */
static char *
inet_ntop6(const u_char *src, char *dst, socklen_t size)
{
    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a value of the specified size.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
    struct { int base, len; } best, cur;
#define NS_IN6ADDRSZ	16
#define NS_INT16SZ	2
    u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
    int i;

    /*
     * Preprocess:
     *	Copy the input (bytewise) array into a wordwise array.
     *	Find the longest run of 0x00's in src[] for :: shorthanding.
     */
    memset(words, '\0', sizeof words);
    for (i = 0; i < NS_IN6ADDRSZ; i++)
        words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
    best.base = -1;
    best.len = 0;
    cur.base = -1;
    cur.len = 0;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;

    /*
     * Format the result.
     */
    tp = tmp;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        /* Are we inside the best run of 0x00's? */
        if (best.base != -1 && i >= best.base &&
            i < (best.base + best.len)) {
            if (i == best.base)
                *tp++ = ':';
            continue;
        }
        /* Are we following an initial run of 0x00s or any real hex? */
        if (i != 0)
            *tp++ = ':';
        /* Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 && (best.len == 6 ||
            (best.len == 7 && words[7] != 0x0001) ||
            (best.len == 5 && words[5] == 0xffff))) {
            if (!inet_ntop4(src+12, tp, sizeof tmp - (tp - tmp)))
                return (NULL);
            tp += strlen(tp);
            break;
        }
        tp += std::sprintf(tp, "%x", words[i]); // ****
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) ==
        (NS_IN6ADDRSZ / NS_INT16SZ))
        *tp++ = ':';
    *tp++ = '\0';

    /*
     * Check for overflow, copy, and we're done.
     */
    if ((socklen_t)(tp - tmp) > size) {
        return (NULL);
    }
    strcpy(dst, tmp);
    return (dst);
}
#endif

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
    return start();
}



void clib::TCPPort::SetIPAddr(const char* ip_addr)
{
    _addr.sin_addr.s_addr = inet_addr(ip_addr);
}


void clib::TCPPort::SetIPAddr(sockaddr_in & addr)
{
    _addr.sin_addr.s_addr = addr.sin_addr.s_addr;
}


int clib::TCPPort::ReadData(uint8_t *buf, uint32_t size, int& conn_id)
{
    if (_is_server)
    {
        if (!_data->empty()){
            incom_data packet = _data->front();
            conn_id = packet.cliend_id;
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
       return socket_wrapper.receive_data(size, (char*)buf, conn_id);
    }
}


int clib::TCPPort::SendData(uint8_t * buf, uint32_t size, int conn_id)
{
    if (_is_server)
    {
        return socket_wrapper.respond_data((char*)buf,size, conn_id);
    }else
    {
        return socket_wrapper.send_data(size, (char*)buf, conn_id);
    }
    return -1;
}


void clib::TCPPort::Close()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();
}

bool clib::TCPPort::start()
{
    if (_is_server)
    {
        _running = true;
        _thread = std::thread(&TCPPort::run, this);
        return true;
    }else
    {
        return socket_wrapper.create_socket(_port, _addr, _timeout);
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
                int bytes = socket_wrapper.receive_data(MAX_BUFFER_SIZE, packet.data, i);
                if (bytes > 0) {
                    packet.data_size = bytes;
                    packet.cliend_id = i;
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
        return false;
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
        return false;
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

int clib::TCPPort::SocketWrapper::receive_data(int buffer_size, char* data, int& client)
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
        client = client_fd;
        return bytes;
    }
    return -1;
}

int clib::TCPPort::SocketWrapper::send_data(int data_size, char *data, int client)
{
    if (client < 0)
        return send(client_fd , data , data_size , 0 );
    else
        return send(client , data , data_size , 0 );
}

int clib::TCPPort::SocketWrapper::respond_data(char* data, int data_size, int conn_id)
{
    //Send "data" to given client
    int ret = send(conn_id, data, data_size, 0);
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

