#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>



#define DEBUG

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



using namespace std;
using namespace libconfig;


//TODO Add another directory to backup to
string backupDir = "/home/florian/data/Bilder";
string backupSaveDir = "/home/florian/data/tmp";
string outputFileForFpart = backupSaveDir + "/" + "fpart_test";
string tar_file = "Archive";
string mkisofs_outputFile = backupSaveDir + "/" + "Backup";


int numberOfFpartFiles;

int readConfigFile()
{
    Config cfg;

    // Read the file. If there is an error, report it and exit.
  try
  {
    cfg.readFile("example.cfg");
  }
  catch(const FileIOException &fioex)
  {
    cerr << "I/O error while reading file." << endl;
    return(EXIT_FAILURE);
  }
  //catch(const ParseException &pex)
  //{
  //  std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
  //            << " - " << pex.getError() << std::endl;
  //  return(EXIT_FAILURE);
  //}

  // Get the store name.
  try
  {
    string name = cfg.lookup("name");
    cout << "Store name: " << name << endl << endl;
  }
  catch(const SettingNotFoundException &nfex)
  {
    cerr << "No 'name' setting in configuration file." << endl;
  }

  const Setting& root = cfg.getRoot();

  // Output a list of all books in the inventory.
  try
  {
    const Setting &books = root["inventory"]["books"];
    int count = books.getLength();

    cout << setw(30) << left << "TITLE" << "  "
         << setw(30) << left << "AUTHOR" << "   "
         << setw(6) << left << "PRICE" << "  "
         << "QTY"
         << endl;

    for(int i = 0; i < count; ++i)
    {
      const Setting &book = books[i];

      // Only output the record if all of the expected fields are present.
      string title, author;
      double price;
      int qty;

      if(!(book.lookupValue("title", title)
           && book.lookupValue("author", author)
           && book.lookupValue("price", price)
           && book.lookupValue("qty", qty)))
        continue;

      cout << setw(30) << left << title << "  "
           << setw(30) << left << author << "  "
           << '$' << setw(6) << right << price << "  "
           << qty
           << endl;
    }
    cout << endl;
  }
  catch(const SettingNotFoundException &nfex)
  {
    // Ignore.
  }

  // Output a list of all books in the inventory.
  try
  {
    const Setting &movies = root["inventory"]["movies"];
    int count = movies.getLength();

    cout << setw(30) << left << "TITLE" << "  "
         << setw(10) << left << "MEDIA" << "   "
         << setw(6) << left << "PRICE" << "  "
         << "QTY"
         << endl;

    for(int i = 0; i < count; ++i)
    {
      const Setting &movie = movies[i];

      // Only output the record if all of the expected fields are present.
      string title, media;
      double price;
      int qty;

      if(!(movie.lookupValue("title", title)
           && movie.lookupValue("media", media)
           && movie.lookupValue("price", price)
           && movie.lookupValue("qty", qty)))
        continue;

      cout << setw(30) << left << title << "  "
           << setw(10) << left << media << "  "
           << '$' << setw(6) << right << price << "  "
           << qty
           << endl;
    }
    cout << endl;
  }
  catch(const SettingNotFoundException &nfex)
  {
    // Ignore.
  }

  return(EXIT_SUCCESS);


}

int fpart()
{
    //fPart configuration

    string size = "644245094";


    //TODO Figure out what to do with partition 0 and check for it

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






    for (i=1; i<numberOfFpartFiles; i++) // starting at 1, since 0 for fpart ist larger than the rest
    {
        stringstream out;
        out << i;
        string number_i = out.str();
        string tar_filename = backupSaveDir + "/" + tar_file + number_i + ".tar.gz";
        string tar_fileList = "-T " + outputFileForFpart + "." + number_i;

        string tar = tar_command + " " + tar_par + " " + tar_filename + " " + tar_fileList;

        system(tar.c_str());
        cout << tar << endl;
    }


    return 0;
}

int mkisofs()
{
    //Define basic strings
    string mkisofs_command = "mkisofs";
    string mkisofs_Volume = "-V Backup";
    string mkisofs_volset = "-volset";
    string mkisofs_par = "-r";


    int i,j,k,l;
    //Base file Name
    string mkisofs_input = ""; // = backupSaveDir + "/" + tar_file + "1" + ".tar.gz";
    string mkisofs_temp;
    //5 tar archives a 600 MB for one DVD.
    //How many DVDs are needed?
    l = numberOfFpartFiles / 5;
    //m = numberOfFpartFiles % 5;

    #ifdef DEBUG
    numberOfFpartFiles = 70;
    l = 14; // in case fpart() doesnt run
    cout << "l=" << l << endl;

    #endif



    for (j=0; j<l; j++)
    {
        k = (j * 5);

            // This is for the mkisofs command for the volume iteration
            stringstream out1;
            out1 << j + 1;
            string number_j = out1.str();

        for (i=1; i<6; i++) // split in 5 parts
        {

            stringstream out;
            out << i + k;
            string number_i = out.str();
            #ifdef DEBUG
            cout << "i+k=" << i+k << endl;
            #endif
            if ((i + k) < numberOfFpartFiles)
            {
                mkisofs_input = mkisofs_input + " " + backupSaveDir + "/" + tar_file + number_i + ".tar.gz" ;
            }

            //mkisofs_temp = mkisofs_input;
            //mkisofs_input = mkisofs_temp + " " + backupSaveDir + "/" + tar_file + number_i + ".tar.gz" ;
        }

        string mkisofs = mkisofs_command + " " + mkisofs_Volume + " " + mkisofs_volset + " " + number_j + " " + mkisofs_par + " " + "-o " + mkisofs_outputFile + number_j + ".iso " + mkisofs_input;

        #ifdef DEBUG
        cout << mkisofs << endl;
        #endif
        system(mkisofs.c_str());


        //this is for the reset of the mkisofs_input string, otherwise it contains everything of the loop
       /* stringstream out2;
        out2 << 6 + k;
        string number_i = out2.str();
        mkisofs_input = backupSaveDir + "/" + tar_file + number_i + ".tar.gz" ; // reset */
        mkisofs_input = "";
    }


    return 0;
}


int dvdisaster()
{
    //dvdisaster -mRS02 -n dvd -c -i fparttest.iso -v
    #ifdef DEBUG

    numberOfFpartFiles = 70;



    #endif
    string dvdisaster_command = "dvdisaster";
    string dvdisaster_par = "-mRS02 -n dvd -c -i";

    int i,j;
    j = numberOfFpartFiles / 5;

    for (i=0; i<j; i++)
    {
        stringstream out;
        out << i + 1; //so that numbers of dvds start at 1
        string number_i = out.str();
        string dvdisaster = dvdisaster_command + " " + dvdisaster_par + " " + mkisofs_outputFile + number_i + ".iso" + " -v" ;

        system(dvdisaster.c_str());
    }

    return 0;
}


int main()
{
    //fpart();
    //tar();
    //mkisofs();
    //dvdisaster();
    readConfigFile();

    return 0;
}
