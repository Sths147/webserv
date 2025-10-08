#include "ClientCgi.hpp"


ClientCgi::~ClientCgi( void ) {

}


#include <unistd.h>
#include <sys/epoll.h>


void				ClientCgi::del_epoll_and_close( int epoll_fd ){
	if (this->_fd_in != -1) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd_in, NULL);
		close(this->_fd_in);
	}
	if (this->_fd_out != -1) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, this->_fd_out, NULL);
		close(this->_fd_out);
	}
}
