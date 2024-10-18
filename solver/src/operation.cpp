#include "common.hpp"
#include "operation.hpp"

using namespace std;

Operation::Operation(Type type, int size, int x, int y, Direction direction) :
    type(type),
    size(size),
    x(x),
    y(y),
    direction(direction)    
{

}

Operation::Operation(int type, int size, int x, int y, int direction) :
    type(static_cast<Type>(type)),
    size(size),
    x(x),
    y(y),
    direction(static_cast<Direction>(direction))    
{

}