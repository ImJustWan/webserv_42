# pragma once

# include <iostream>
# include <stdbool.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <unistd.h>
# include <fstream>
# include <cstdlib>
# include <sys/types.h>
# include <sys/socket.h>

# include "colors.hpp"

bool	isCGI( std::string const & ressource );
void	execCGIScript( std::string ressource, int event_socket );

