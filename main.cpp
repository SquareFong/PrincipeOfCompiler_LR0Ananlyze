#include <iostream>
#include "LR0.h"

int main() {
    LR0 l;
    l.readFile("g.txt");
    l.show();

    l.buildSheet();
	cout << endl << "LR(0) Sheet: " << endl;
    l.sheetShow();
	cout << endl;
    vector<char> test1;
    test1.push_back('v');
    test1.push_back('i');
    test1.push_back(',');
    test1.push_back('i');
    test1.push_back(':');
    test1.push_back('r');
    test1.push_back('$');
	cout << "vi,i:r$: ";
	if (l.LR0analyze(test1))
		cout << "Accept" << endl;
	else
		cout << "Error" << endl;
	test1[2] = 'i';
	cout << "viii:r$: ";
	if (l.LR0analyze(test1))
		cout << "Accept" << endl;
	else
		cout << "Error" << endl;

    //std::cout << "Hello, World!" << std::endl;
    return 0;
}