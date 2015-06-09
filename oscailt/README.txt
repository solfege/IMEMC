Last Update: Sept 2011

This README file lists all the releases of Oscailt from 3.0 onwards. For details of installation or
upgrade instructions, see below.

For details of the changes in each release, please read the releasenotes.txt file. This lists any new features
added or bugfixes made.

For a list of the features available in Oscailt see the HTML file oscailt_3x_features.html in the documentation
directory of the Oscailt cvs and now included with each release.

If you already have an Oscailt installation, you will probably want to follow the upgrade instructions to migrate
from each release to the next. Otherwise if you are new to Oscailt or just want to do a new install, then you 
should follow the 'Full Installation Instructions' below.


Oscailt 3.5 version.
--------------------
See the upgrade instructions in the file oscailt/upgrades/upgrade-3-05/UpgradeGuide_3-5.html

Oscailt 3.4 version.
--------------------
See the upgrade instructions in the file oscailt/upgrades/upgrade-3-04/UpgradeGuide_3-4.html

Oscailt 3.3 version.
--------------------
See the upgrade instructions in the file oscailt/upgrades/upgrade-3-03/UpgradeGuide_3-3.html

Oscailt 3.2 version.
--------------------
See the upgrade instructions in the file oscailt/upgrades/upgrade-3-02/UpgradeGuide_3-2.html

Oscailt 3.1 version.
--------------------
See the upgrade instructions in the file oscailt/upgrades/upgrade-3-01/UpgradeGuide_3-1.html

Oscailt 3.0 version.
--------------------
See the full install instructions below as there is no upgrade path from Oscailt 2.x


Installation Requirements:
--------------------------
Before carrying out a full install, ideally you should have an Apache web server on some variant of Linux or Unix, 
MySQL Database and PHP 5.x or later. However Oscailt will still work with PHP 4.x versions.

Summary:
1. Apache Web Server installed
2. PHP installed 
3. MySql installed 
4. You must create an (empty) database schema in MySQL that Oscailt will use to create its tables.
5. If you are making use of email notifications, then your server should have some sort of email software preinstalled.
   

Full Install Instructions
-------------------------
Summary:
1. Copy the contents of the html directory in the Oscailt release zip file to the destinaton on the web server.
2. Create an empty database schema in the database. For MySQL the SQL command is: CREATE DATABASE database_name;
3. Edit config/dbconfig.php to set the database host address, database name, username / password etc.
   Make sure no extra blanks appear in this file after the PHP terminating tag on the last line of the file.
4. Edit the Apache file .htaccess to allow access to the install.php script. 
5. Load install.php in your web browser -> follow the instructions.
   The install will create additional directories and create the necessary database tables. It provides the option 
   to import an existing site template. It will offer an option to load a basic site template into the database, 
   create a default editor called 'admin' and build the site cache files. 
6. When finished installing, be sure to remove the file install.php or remove its entry from .htaccess file.
7. Go to admin.php to do admin stuff like setting up other users and configuring the site name and site URL.
8. index.php is the site itself.  


Note 1: For first time install, you SHOULD always use one of the template sites offered.
Note 2: During the installation on the last step, you may see a warning about failing to parse OML. You can ignore
        this warning. It may be due to an incorrect URL used to gather the Indymedia cities listing.

Note for Windows Users.
-----------------------
If you are installing under Windows, the PHP function 'is_executable' which is called at line 197 in the file
html/objects/magpie/snoopy.inc is not available in Windows and will cause execution to terminate at that point.
This will cause step 5 of the full install which builds the object caches to fail. To get around this problem
just add the line:

        return false;

before the if statment (line 197) which contains the above call. This should solve the problem. This is partially
fixed in Oscailt 3.2 by trying to avoid calling this function.  

For shared memory, the file objects/memorymgmt.inc dummy functions have been created for the following:
     sem_get()
     sem_release()
     sem_remove()
     sem_acquire()

These are NOT available under Windows and therefore these dummy versions of these at the start of the above file
need to be commented out to allow the code to work since it is assumed they are available and most working sites
are likely to be hosted on some Linux/Unix varient.


Oscailt Documentation
---------------------
1. This README file
2. The upgrade guides
3. The release notes (releasenotes.txt).
4. Info about Oscailt at: http://docs.indymedia.org/view/Devel/Oscailt -This site is not always available.
5. Design documentation release (below) on sourceforge at: http://sourceforge.net/forum/forum.php?forum_id=832687
6. Oscailt features (oscailt_3x_features.html) in the documentation release or in cvs on sourceforge.

Documentation Releases
----------------------

Design Documentation release Doc v1.0 -June 2008
------------------------------------------------
This documentation release is the first attempt at explaining the internals of Oscailt and how it works.


Oscailt Modified Version of XSPF Player
---------------------------------------
The Fabricio Zuardi XSPF embedded flash audio player has been modified for use with Oscailt. This is available as
a separate release to the main Oscailt codebase.

Embedded Flash Audio Player v1.0 -Aug 2010
------------------------------------------------
This is the first release of the modified Fabricio Zuardi XSPF embedded flash audio player. To install this, just follow
the instructions in the release file.


Known Problems
--------------

Known Problems with Retrieval of Indymedia City Listing
-------------------------------------------------------
If you specify the use of HTTPS in CityListing object for retrieval of the Indymedia IMC list, it will expect
to receive a SSL certificate that it recognises and it will fail otherwise. To get around this problem, use
HTTP instead. The URL for the IMC city list is: http://contact.indymedia.org/oml.php

Known Problems with Oscailt 3.0
-------------------------------

1. When you have installed your site, you may get the following error: 
    database error : 1054 - Unknown column 's.story_id' in 'on clause

   This is caused by a compliance problem with SQL and some earlier versions of MySql. The solution is to download
   version 1.9 of the file storyquery.inc from the Oscailt sourceforge CVS directory which can be found here:
   http://oscailt.cvs.sourceforge.net/oscailt/oscailt/

2. Sometimes when editing a story or comment it fails because the story or comment id somehow gets set to zero.


Known Problems with Oscailt 3.1
-------------------------------

1. When you install the software and the cache is being rebuilt it may fail especially on Windows machines. This
   is because the function: is_executable() which is called in snoopy.inc is not supported by Windows, and PHP
   will terminate at that point. The solution is to take the latest version of this file from the Oscailt CVS.

2. Problems staying logged in and accessing the administraiton screen. On servers with PHP 4.x there is a problem
   where sessions are not being set. This is caused by low level errors and output being sent before the session
   is started. This can be caused by extra characters appearing in any of the include file after the final closing
   PHP tag. This has been found to be the case in dbconfig.php. To solve this remove any characters including
   blanks and linefeeds after the closing ?> tag.


Known Problems with Oscailt 3.2
-------------------------------

1. If you have friendly URLS for document objects, they may not convert properly to the new types when you try
   to edit them. This is fixed in the next release.

Known Problems with Oscailt 3.3
-------------------------------

1. Installation problem creating the translations table. 

Known Problems with Oscailt 3.4
-------------------------------

1. Blocking of embedded cover images not working correctly with configuration
2. Public edits introduced in 3.2 breaks publish form.
