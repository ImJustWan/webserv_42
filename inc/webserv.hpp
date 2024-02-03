#pragma once

# include <map>
# include <string>			/* Components for manipulating sequences of characters  */
# include <fcntl.h>			/* Manipulate file descriptor*/
# include <vector>
# include <iostream>		/* I/O streams library  */
# include <unistd.h>
# include <stdlib.h>
# include <exception>		/* Error handling library  */
# include <sys/stat.h>
# include <sys/epoll.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <bits/stdc++.h>


# include "colors.hpp"
# include "ServerHandler.hpp"
# include "IEvent.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Get.hpp"
# include "Server.hpp"
# include "Post.hpp"
# include "Delete.hpp"

/*  Listening port  */
# define PORT 8081
