#include "Delete.hpp"

Delete::Delete() {

}

Delete::~Delete() {

}

Delete::Delete ( const Delete& cpy ) {
	*this = cpy;
}

Delete& Delete::operator= ( const Delete& cpy ) {
	(void)cpy;
	return *this;
}
