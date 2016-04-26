# dbt
DVD Backup Tool

1) What is dbt (going to be?)?

dbt is going to be a backup tool to backup diretories to DVDs.

2) What does it need?

Right now it relies on dvdisaster, fpart and tar to do its bidding.

See:

Fpart: https://sourceforge.net/projects/fpart

Dvdisaster: http://dvdisaster.net

3) Will it work on my computer?

Under certain conditions. You'll need Code::Blocks IDE to open the project file and to compile it.
For compilation you'll also need libconfig installed.

4) I have questions or maybe a patch/idea for your project. How can I contact you?

Just send an email to my account. See my GitHub Profile for the email adress.

5) How does it work?

dbt uses fpart to crawl through a directory, sorts the files acording to size and packs them into 600 MB (or whatever you prefer) stacks.
These stacks of files are TAR'ed into archives. Default settings will put 5 TAR archives into 1 .iso file (so we get 3000 MB .iso files).
Error correction codes will be added to the images using dvdisaster, so even when the optical medium fails on certain parts, recovery will be possible.
These "enhanced" images are then written to DVD's.

Recovery is easy and can be done without the need of any additional software. Just untar the archives.

It is supposed to be an easy backup solution for static data.


