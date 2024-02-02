// #ifndef __POST__
#define __POST__

#pragma once

class Post {

public:

	Post( void );
	~Post();
	Post ( const Post& cpy );
	Post& operator= ( const Post& cpy );

private:
};

// #endif /* __POST__ */