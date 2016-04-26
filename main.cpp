#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>



#define DEBUG
//#define DEBUG_VALUES



using namespace std;
using namespace libconfig;



string backupDir ; //= "/home/florian/data/Bilder";
string backupSaveDir ; //= "/home/florian/data/tmp";
string outputFileForFpart = backupSaveDir + "/" + "fpart_test";
string tar_file = "Archive";
string mkisofs_outputFile = backupSaveDir + "/" + "Backup";

string fpart_size = "644245094"; //default value. Equals 600 MB

string dvd_device = "/dev/dvd"; //default value


int numberOfFpartFiles;


// Switch from system() calls to e.g. fork exec

int readConfigFile()
{
    Config cfg;
    bool failure = false;

    // Read the file. If there is an error, report it and exit.
  try
  {
    cfg.readFile("dbt.cfg");
  }
  catch(const FileIOException &fioex)
  {
    cerr << "I/O error while reading file." << endl;
    return(EXIT_FAILURE);
  }

  try
  {
    backupDir = (const char *)cfg.lookup("backup_dir"); // const char * needs to be used to force an explicit conversion
    #ifdef DEBUG
    cout << "Backup Directory: " << backupDir << endl << endl;
    #endif
  }
  catch(const SettingNotFoundException &nfex)
  {
    cerr << "No 'backup_dir' setting in configuration file." << endl;
    failure = true;
  }
    try
  {
    backupSaveDir = (const char *)cfg.lookup("backup_save_dir");
    outputFileForFpart = backupSaveDir + "/" + "fpart_test";
    mkisofs_outputFile = backupSaveDir + "/" + "Backup";
    #ifdef DEBUG
    cout << "fPart FileForFpart " << outputFileForFpart << endl << endl;
    cout << "Backup to Directory: " << backupSaveDir << endl << endl;
    #endif
  }
  catch(const SettingNotFoundException &nfex)
  {
    cerr << "No 'backup_save_dir' setting in configuration file." << endl;
    failure = true;
  }

    try
  {
    fpart_size = (const char *)cfg.lookup("part_size");
    #ifdef DEBUG
    cout << "Split into parts of aprox.  " << fpart_size << " B size" << endl << endl;
    #endif
  }
  catch(const SettingNotFoundException &nfex)
  {
    cerr << "No 'part_size' setting in configuration file." << endl;
    failure = true;
  }
     try
  {
    dvd_device = (const char *)cfg.lookup("dvd_device");

    #ifdef DEBUG
    cout << "DVD Device " << dvd_device << endl << endl;
    #endif
  }
  catch(const SettingNotFoundException &nfex)
  {
    cerr << "No 'backup_save_dir' setting in configuration file." << endl;
    failure = true;
  }



    if (failure)
    {
        return(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);


}

int fpart()
{
    //fPart configuration

    //string fpart_size = "644245094";


    //TODO Figure out what to do with partition 0 and check for it

    string fpart_command = "fpart";
    string fpart_size_command = "-s " + fpart_size;
    string fpart_outputFile = "-o " + outputFileForFpart;
    string fpart = fpart_command + " " + fpart_size_command + " " + fpart_outputFile + " " + backupDir;

    #ifdef DEBUG
    cout << "Command fpart: " << fpart << endl;
    #endif
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

    #ifdef DEBUG_VALUES
    numberOfFpartFiles = 70;
    l = 14; // in case fpart() doesnt run
    cout << "l=" << l << endl;

    #endif



    for (j=0; j<l; j++)
    {
        k = (j * 5); //Add 5 every turn of the outer loop

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
            if ((i + k) < numberOfFpartFiles) //so that we don't try to add files that don't exist
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

int writeDVD()
{
    //growisofs -Z %device -A BAR -V Backup -volset %number -dry-run -r %directory
    string growisofs_command = "growisofs";
    string growisofs_device = "-Z " + dvd_device;
    string growisofs_name = "-A BAR";
    string growisofs_volume = "-V Backup";
    string growisofs_volset = "-volset";
    string growisofs_par = " -dry-run -r";

    int i,j;
    j = numberOfFpartFiles / 5;
    for (i=0; i< j; i++)
    {
        stringstream out;
        out << i + 1; //so that numbers of dvds start at 1
        string number_i = out.str();
        string growisofs = growisofs_command + " " + growisofs_device + " " + growisofs_name + " " + growisofs_volume + " " + growisofs_volset + " " + number_i + " " + growisofs_par + " " + mkisofs_outputFile + number_i + ".iso";
        #ifdef DEBUG
        cout << growisofs << endl;
        #endif
    }


    return 0;
}


int main()
{
    //At first, read configuration file
    //TODO Add introduction text
    //TODO add CLI parameters for configuration file location, backupDir and backupSaveDir
    if (readConfigFile() == EXIT_FAILURE)
    {
        cout << "Something went wrong with reading the configuration file. Aborting."<< endl;
        return 1;
    }



    fpart();
    tar();
    mkisofs();
    dvdisaster();
    writeDVD();


    return 0;
}
