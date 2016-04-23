#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>

/*
fpart -s 644245094 -o fpart_test /home/florian/data/Bilder/
tar -cvzf fparttest1.tar.gz -T fpart_test.1
tar -cvzf fparttest2.tar.gz -T fpart_test.2
tar -cvzf fparttest3.tar.gz -T fpart_test.3
tar -cvzf fparttest4.tar.gz -T fpart_test.4
tar -cvzf fparttest5.tar.gz -T fpart_test.5
mkisofs -V Backup -volset 1 -r -o fparttest.iso fparttest*
dvdisaster -mRS02 -n dvd -c -i fparttest.iso -v
*/

/* TODO
-- Figure out how to handle partition 0 with fparts


*/

using namespace std;

string backupDir = "/home/florian/data/Bilder";
string outputFileForFpart = "/home/florian/data/Bilder/fpart_test";
int numberOfFpartFiles;

int fpart()
{
    //fPart configuration

    string size = "644245094";



    string fpart_command = "fpart";
    string fpart_size = "-s " + size;
    string fpart_outputFile = "-o " + outputFileForFpart;
    string fpart = fpart_command + " " + fpart_size + " " + fpart_outputFile + " " + backupDir;


    // Run fpart
    system(fpart.c_str());

    //How many files have been created?

    string ls_command = string("ls -la ") + outputFileForFpart + "* " + "|wc -l" + "> size.txt"; // Result will be the number of fpart Files
    system(ls_command.c_str()); // Run the command

    string line;
    ifstream file;
    file.open("size.txt"); // Open file

    getline(file, line); //File consists of only 1 line, which is a number aka the number of fpart Files
    numberOfFpartFiles = atoi(line.c_str()); //Save said number to int

    return 0;
}

int tar()
{
    int i;
    string tar_command = "tar";
    string tar_par = "-cvzf"; //compress verbose gzip and use the following filename
    string tar_file = "Archive";





    for (i=1; i<numberOfFpartFiles; i++) // starting at 1, since 0 for fpart ist larger than the rest
    {
        stringstream out;
        out << i;
        string number_i = out.str();
        string tar_filename = backupDir + "/" + tar_file + number_i + ".tar.gz";
        string tar_fileList = "-T " + outputFileForFpart + "." + number_i;

        string tar = tar_command + " " + tar_par + " " + tar_filename + " " + tar_fileList;

        system(tar.c_str());
        cout << tar << endl;
    }


    return 0;
}




int main()
{
    fpart();
    tar();

    return 0;
}
