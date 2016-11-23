#include <iostream>

int main(void)
{
    int a, b, c;

    while ( std::cin >> a >> b >> c ) {
        if ( a <= 20 && b <= 20 ) {
            std::cout << a << " " << b << " " << c << std::endl;
        }
    }

    return 0;
}
