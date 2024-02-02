#include "Post.hpp"

Post::Post() {

}

Post::~Post() {

}

Post::Post ( const Post& cpy ) {
	*this = cpy;
}

Post& Post::operator= ( const Post& cpy ) {
	(void)cpy;
	return *this;
}
