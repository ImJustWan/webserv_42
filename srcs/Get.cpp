#include "Get.hpp"

Get::Get() {

}

Get::~Get() {

}

Get::Get ( const Get& cpy ) {
	*this = cpy;
}

Get& Get::operator= ( const Get& cpy ) {
	(void)cpy;
	return *this;
}
