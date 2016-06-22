#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"

int main(void) {
    _csp t = file_reader("csp50.txt");
    print_to_graphviz(&t);
    return 0;
}
