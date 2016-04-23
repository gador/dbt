#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

int main()
{
    cout << "Hello world!" << endl;
    system("ls -l >test.txt");
    cout << ifstream("test.txt").rdbuf();
    return 0;
}
