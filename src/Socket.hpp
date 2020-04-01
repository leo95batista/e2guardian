// Socket class - implements BaseSocket for INET domain sockets

// For all support, instructions and copyright go to:
// http://e2guardian.org/
// Released under the GPL v2, with the OpenSSL exception described in the README file.

#ifndef __HPP_SOCKET
#define __HPP_SOCKET

// INCLUDES

#include "BaseSocket.hpp"
#include "String.hpp"
#include <sstream>
#include <iomanip>

#include "openssl/ssl.h"
#include "String.hpp"

// DECLARATIONS

class Socket : public BaseSocket
{
    friend class FDTunnel;

    public:
    // create INET socket & clear address structs
    Socket();
    // create socket using pre-existing FD (address structs will be empty!)
    Socket(int fd);
    // create socket from pre-existing FD, storing given local & remote IPs
    Socket(int newfd, struct sockaddr_in myip, struct sockaddr_in peerip);
    ~Socket();

    // connect to given IP & port (following default constructor)
    int connect(const std::string &ip, int port);

    // bind to given port
    int bind(int port);
    // bind to given IP & port, for machines with multiple NICs
    int bind(const std::string &ip, int port);

    // accept incoming connections & return new Socket
    Socket *accept();

    // close socket & clear address structs
    void reset();

    // get remote IP/port
    std::string getPeerIP();
    int getPeerSourcePort();
    int getPort();
    void setPort(int port);
    unsigned long int getPeerSourceAddr();

    // get local IP
    std::string getLocalIP();
    int getLocalPort();

    // Chunked i/o
    String chunked_trailer;
    int chunk_to_read;
    bool writeChunk( char *buffout, int len, int timeout);
    bool writeChunkTrailer( String &trailer);
    int readChunk( char *buffout, int maxlen, int timeout);
    int loopChunk(int timeout);
    int drainChunk(int timeout);
    void resetChunk();

    bool chunkError;

    //use this socket as an ssl server
    int startSslClient(const std::string &certPath, String hostname);

    //is this a SSL connection
    bool isSsl();

    bool isSslServer();

    //shuts down the current ssl connection
    void stopSsl();

    //cleans up ssl
    void cleanSsl();

    //check that everything in this certificate is correct appart from the hostname
    long checkCertValid(String &hostname);

    //check the common name and altnames of a certificate against hostname
    int checkCertHostname(const std::string &hostame);

    void close();

    //use this socket as an ssl server
    int startSslServer(X509 *x, EVP_PKEY *privKey, std::string &set_cipher);

    // blocking check, can break on config reloads
    void readyForOutput(int timeout, bool honour_reloadconfig = false);

    // non-blocking check for input data
    bool checkForInput();
    bool bcheckForInput(int timeout);

    // get a line from the socket - can break on config reloads
    int getLine(char *buff, int size, int timeout, bool honour_reloadconfig = false, bool *chopped = NULL, bool *truncated = NULL) ;

    // write buffer to string
    bool writeString(const char *line);
    bool writeString(std::string line);

    // write buffer to string - can be told not to do an initial readyForOutput, and told to break on -r
    bool writeToSocket(const char *buff, int len, unsigned int flags, int timeout, bool check_first = true, bool honour_reloadconfig = false);
    // read from socket, returning number of bytes read
    int readFromSocketn(char *buff, int len, unsigned int flags, int timeout);
    // read from socket, returning error status - can be told to skip initial checkForInput, and to break on -r
    int readFromSocket(char *buff, int len, unsigned int flags, int timeout, bool check_first = true, bool honour_reloadconfig = false);
    // write to socket, throwing std::exception on error - can be told to break on -r
  //  void writeToSockete(const char *buff, int len, unsigned int flags, int timeout, bool honour_reloadconfig = false) throw(std::exception);
    bool getIeof();

    private:
    SSL *ssl = NULL;
    SSL_CTX *ctx = NULL;
    bool isssl = false;
    bool issslserver = false;

    // local & remote addresses
    struct sockaddr_in my_adr;
    struct sockaddr_in peer_adr;
    int my_port;
    bool ieof = false;
};

#endif
