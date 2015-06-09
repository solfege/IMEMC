<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/

//set this to false if you want the install script to ignore any errors and
//to continue on with the install process when it meets them...
$quit_on_fail = true;
$logging_error = false;
error_reporting(E_ALL);
$OSCAILT_SCRIPT = "install.php";
include_once("objects/systemconfig.inc");
include_once("objects/memorymgmt.inc");
include_once("objects/sqllayer.inc");
include_once("objects/querycache.inc");
include_once("objects/utilities.inc");
include_once "objects/installdatabase.inc";

// We should now have the version number at this stage
$pageTitle = "Oscailt ".$oscailt_basic_config['software_version'] ." Full Installation Script";

ob_start();
$msg = "";
function getMySQLVersion()
{
    global $dbconn;
    // Zero means no db cache
   connectToDatabase();
   if ($dbconn == null) return "";

    $result = sql_query("SELECT VERSION()", $dbconn, 0);
    checkForError($result);

    $db_version = "";

    if(sql_num_rows( $result ) > 0)
    {
        list($db_version) = sql_fetch_row($result, $dbconn);
    }

    return $db_version;
}
function writeIntroBox()
{
   global $system_config, $OSCAILT_SCRIPT, $oscailt_basic_config;

   logInstallMsg("-------- Installing Oscailt version ".$oscailt_basic_config['software_version']. " on " .strftime("%a %d %b %H:%M:%S",time()) ." -------- ");
   
   if (isset($_SERVER['SERVER_SOFTWARE'])) {
      logInstallMsg("Server has ".$_SERVER['SERVER_SOFTWARE']. " webserver running CGI protocol ".$_SERVER['SERVER_PROTOCOL']);
   }

   logInstallMsg("Server Operating System is ".PHP_OS);

   logInstallMsg("Server has version PHP ".phpversion(). " installed.");
   // Add OS info too.

   if (!extension_loaded('gd') ) {
      logInstallMsg("PHP extension GD (graphics library) is NOT installed.");
   } else {
      logInstallMsg("PHP extension GD (graphics library) installed.");
   }

   if (!extension_loaded('mbstring') ) {
      logInstallMsg("PHP extension mbstring (multibyte strings) is NOT installed.");
   } else {
      logInstallMsg("PHP extension mbstring (multibyte strings) installed.");
   }

   if (!extension_loaded('shmem') ) {
      logInstallMsg("PHP extension shmem (Shared Memory) is NOT installed.");
   } else {
      logInstallMsg("PHP extension shmem (Shared Memory) installed.");
   }

   if (!extension_loaded('sysvsem') ) {
      logInstallMsg("PHP extension sysvsem (Semaphores) is NOT installed.");
   } else {
      logInstallMsg("PHP extension sysvsem (Semaphores) installed.");
   }

   ?>
   <div class='install-logo'><img src='graphics/oscailtlogoanim.gif' alt='Oscailt' border=0></div>
   <div class="install-page">
   <form action="<?=$OSCAILT_SCRIPT?>">
   <?
      writeInstallationInstruction("Welcome to the oscailt Installation System", "This page allows you to install the oscailt independent media software content management system in a matter of minutes. Full Installation Instructions can be found on the <a href='http://docs.indymedia.org/view/Devel/OscailtAdminInstall'>Online Documentation Wiki</a>
      <P>Before you can use this program, you need to have done 2 simple things.<ol class='install-requirement'>
      <li class='install-requirement'><b>Set Directory Permissions</b>
      You need to ensure that your <i>attachments</i>, <i>logs</i>, <i>object templates</i>, <i>exports</i> and <i>cache</i> directory are writable by your webserver. To do this on Unix, go to the directory where you installed oscailt and type:<br>
      <br><tt>chmod 0777 attachments</tt>
      <br><tt>chmod 0777 cache</tt>
      <br><tt>chmod 0777 logs</tt>
      <br><tt>chmod 0777 xmldata/templates</tt>
      <br><tt>chmod 0777 xmldata/exports</tt>
      <li class='install-requirement'><b>Setup Database Account</b>You need to set up a mysql database to store stories in. Edit the file <i>config/dbconfig.php</i> and input the appropriate database name, username, password and table prefix setting</li>
      </ol>");
      writeInstallationInstruction("Continue to Stage 1: Check Directories and Files", "Click the button below for oscailt so that Oscailt check your filesystem to make sure that everything is okay and to install any extra directories that are needed");
   ?>
   <p align='center'><input type='submit' name="file_stage_done" value="Go >>>"></p>
   </form>
   </div>
   <?
   logInstallMsg("End of stage 1 reached.");
}


function checkRequiredFiles()
{
   global $OSCAILT_SCRIPT, $query_cache, $system_config;
   logInstallMsg("Checking required paths and files.");
   ?>
      <div class='install-logo'><img src='graphics/oscailtlogoanim.gif' alt='Oscailt' border=0></div>
      <div class='install-page'>
      <form action="<?=$OSCAILT_SCRIPT?>">
   <?

   $msg = "";
   $default_site = $system_config->site_file_store_base."default/";
   $paths_to_check = array($system_config->attachment_store, $system_config->attachment_store."/xspf_player/",$system_config->private_cache, $system_config->log_store, $system_config->image_cache, $system_config->image_cache."/video_images/", $system_config->site_file_store_base, $system_config->query_cache_store, $system_config->html_cache_store, $system_config->rss_cache, $default_site, $system_config->object_name_store, $system_config->type_details_store, $system_config->object_template_store, $system_config->object_export_dir);

   if($system_config->object_index_storage != 'sql') $paths_to_check[] = $system_config->object_index_storage;

   if($system_config->new_objects_storage != 'sql') $paths_to_check[] = $system_config->new_objects_storage;

   $met_error = false;
   foreach($paths_to_check as $path)
   {
      if(!file_exists($path))
      {
         logInstallMsg("Checking path ".$path ."... path does not exist");
         if(!mkdir($path, $system_config->default_writable_directory_permissions))
         {
            logInstallMsg("Cannot create path ".$path);
            writeInstallationError("Error Creating Directory", "Oscailt encountered an error while creating <i>$path</i> directory. Check to make sure that the directory exists manually and ensure that the webserver has permission to write into that directory (type <i>chmod $path $system_config->default_writable_directory_permissions</i>");
            return;
         }
         logInstallMsg("Path ".$path ." created.");
         $msg .= "<li class='install-result'>$path directory created okay</li>";
	 if ($path == $system_config->query_cache_store) {
             $created_path_list = $query_cache->checkCacheDirs(false,true,true);
	     foreach ($created_path_list as $sub_path) {
                 logInstallMsg("Path ".$sub_path ." created.");
                 $msg .= "<li class='install-result'>$sub_path directory created okay</li>";
	     }
         }
      }
      else
      {
         if(!is_writable($path) && !chmod($path, $system_config->default_writable_directory_permissions))
         {
            logInstallMsg("Checking path ".$path ."... path exists and is NOT writeable.");
            writeInstallationError("Error Changing Directory Permissions", "Oscailt encountered an error while setting the permissions of <i>$path</i> directory. The directory exists but is not writable by the web server user. Check to make sure that the directory exists manually and ensure that the webserver has permission to write into that directory (type <i>chmod $path $system_config->default_writable_directory_permissions</i>");
            return;
         }
         logInstallMsg("Checking path ".$path ."... path exists and is writeable.");
         $msg .= "<li class='install-result'>$path permissions okay</li>";
      }
   }
   logInstallMsg("Creating Directories and Checking Permissions Complete.");
   writeInstallationResult("1. Creating Directories and Checking Permissions", $msg, "Directories OK!");

   $db_ver = getMySQLVersion();
   $schema_present = true;
   if ($db_ver == "") {
       global $dbname, $dbuname, $dbhost;
       $schema_present = false;
       writeInstallationError("Error connecting to database", "Database schema " .$dbname ." is not present. Stop the install, check your database configuration and schema exists and start the install again.");
       logInstallMsg("Error connecting to database. Schema ".$dbname. " on host ".$dbhost." appears to be missing.");
   } else {
      logInstallMsg("Server has MySQL database version ".$db_ver. " installed.");
   }

   if ($schema_present == true) 
   {
      $installed_version = detectExistingInstallation();
      if($installed_version == 0)
      {
         global $dbname, $dbuname, $dbhost;
         logInstallMsg("Empty installed database detected with database name ".$dbname ." connected via user ".$dbuname. " to host ".$dbhost);
         writeInstallationInstruction("Continue to Stage 2: Install the database", "Remember that you must have already setup the appropriate user and database in your mysql database and set the correct username and password in the <i>config/dbconfig.php</i> file");
      }
      else
      {
         global $dbname, $dbuname, $dbhost;
         logInstallMsg("An existing installed database is already present with database name ".$dbname ." connected via user ".$dbuname. " to host ".$dbhost);
   
         writeInstallationInstruction("Continue to Stage 2: Upgrade the database", "Oscailt Has detected a pre-existing version already installed (version $installed_version) and will attempt to upgrade this installation to this version ($system_config->software_version)<p>Note: if you have a large number of stories in your database, this operation could take a number of minutes.");
         echo "<input type='hidden' name='upgrade' value='$installed_version'>\n";
      }
      ?>
      <p align='center'><input type='hidden' name="file_stage_done" value="confirmed"><input type='submit' name="database_stage_done" value="Go >>>"></p>
      <?
   }
   ?>
   </form>
   </div>
   <?
   logInstallMsg("End of stage 2 reached.");
}

function importDatabaseRecords()
{
   global $system_config, $prefix, $graphics_store, $OSCAILT_SCRIPT, $msg;
   $upgrade = false;
   if(isset($_REQUEST['upgrade']))
   {
      $upgrade = $_REQUEST['upgrade'];
   }
   $graphics_store = "graphics/";
   ?>
      <div class='install-logo'><img src='graphics/oscailtlogoanim.gif' alt='Oscailt' border=0></div>
      <div class='install-page'>
      <form action="<?=$OSCAILT_SCRIPT?>">
   <?

   logInstallMsg("Importing database records.... connecting to database.");

   connectToDatabase();
   global $dbconn;
   if(!$dbconn)
   {
      logInstallMsg("Error Connecting to database ". $msg. " Oscailt Failed to connect to your database. Check the values in config/dbconfig.php to ensure that they are correct.");
      writeInstallationError("Error Connecting to database", "<ol>$msg</ol><p>Oscailt Failed to connect to your database.  Check the values in <i>config/dbconfig.php</i> to ensure that they are correct</p>");
   }
   $sql_res = (installTables($upgrade) && installStarterData($upgrade));
   if($sql_res !== false)
   {
      logInstallMsg("Database Installation ".$msg." Database Installation OK");
      writeInstallationResult("2. Database Installation", $msg,"Database Installation OK");
      writeInstallationInstruction("Continue to Stage 3: Choose Initial Site Template", "Ensure that there are no errors above and, when you are happy that your database has been properly installed, click on the button to choose the template which will form the basis of your website!");
      //this is where we want a name - value thing...
      ?>
      <p align='center'>
      <input type='hidden' name="file_stage_done" value="confirmed">
      <input type='hidden' name="database_stage_done" value="confirmed">
      <input type='submit' name="dataobjects_choices_done"  value="Go >>>"></p>
      </form>
      </div>
      <?
   }
   else
   {
      logInstallMsg("Database Installation Failed: ". $msg. " Operation failed.");
      writeInstallationResult("2. Database Installation Failed", "$msg <li class='install-error'>Operation failed</li>", "");
      writeInstallationInstruction("Fix the above problems and try again", "If the tables that you are trying to install already exist in your database, you will need to either remove them using the wipedb.php tool (copy it to the html directory from the tools directory and then open it in your browser)<p> or you can edit the install.php file and set<br><tt>\$quit_on_fail = false;</tt><br>to keep on going and install any new tables even when others exist already.");

   }
}

function writeDataObjectsChoices()
{
   global $system_config, $prefix, $graphics_store, $OSCAILT_SCRIPT, $msg;
   $graphics_store = "graphics/";
   $system_config->sql_query_caching_enabled = false;//for install
   $system_config->load();
   $system_config->sql_query_caching_enabled = false;//for install

   logInstallMsg("Writing Data Object Choices from " .$system_config->xmltypedef_dir);
   $dummy = new indyObjectActionRequest();
   $obj_set = new indyObjectSet($system_config->xmltypedef_dir, $system_config->object_index_storage);
   $obj_set->load(array("*"), array("*"), $dummy);
   $txt_strs = new indyItemSet();
   if($txt_strs->load($system_config->xml_store, "universal_config_options") === false)
   {
      logInstallMsg("Error Loading Text From XML. Failed to load the list of XML Options from $system_config->xml_store");
      writeInstallationError("Error Loading Text From XML", "Failed to load the list of XML Options from $system_config->xml_store");
      $e = true;
   }
   $importer = new indySiteImporter($obj_set, $dummy);
   ?>
      <div class='install-logo'><img src='graphics/oscailtlogoanim.gif' alt='Oscailt' border=0></div>
      <div class='install-page'>
   <?
   if(!isset($_REQUEST['collection_choice']) or trim($_REQUEST['collection_choice']) == "")
   {
      writeInstallationInstruction("Stage 3: Choose Initial Site Template", "Oscailt allows you to choose an initial template to install on your site so that you can base your site on pre-configured sites and save yourself a lot of work</div>");
      $importer->writeChooseCollectionSection(2, $txt_strs, "install");
   }
   else
   {
      $collection_id = $_REQUEST['collection_choice'];
      writeInstallationInstruction("Stage 4: Install $collection_id Template",  "View the description below and click confirm to install this site template.  If you have changed your mind, click back on the browser.</div>");
      $importer->writeChooseCollectionSection(4, $txt_strs, "install", $collection_id);
   }
   ?>
   </div>
   <?
   logInstallMsg("End of stage 3 reached.");
}


function importInitialDataObjects()
{
   global $OSCAILT_SCRIPT, $object_import_store, $system_config, $query_cache;
   ?>
      <div class='install-logo'><img src='graphics/oscailtlogoanim.gif' alt='Oscailt' border=0></div>
      <form action="<?=$OSCAILT_SCRIPT?>">
   <?
   $system_config->sql_query_caching_enabled = false;//for install
   $system_config->load();
   $system_config->auto_cache_objects = true;
   $system_config->sql_query_caching_enabled = false;//for install
   logInstallMsg("Import of initial data objects from " .$system_config->xmltypedef_dir);

   $dummy = new indyObjectActionRequest();
   $obj_set = new indyObjectSet($system_config->xmltypedef_dir, $system_config->object_index_storage);
   if(!$obj_set->load(array("*"), array("*"), $dummy))
   {
      //writeError("Failed to load the object set with data types from $system_config->xmltypedef_dir");
      $e = true;
   }
   if(!isset($_REQUEST['collection_choice']) or trim($_REQUEST['collection_choice']) == "")
   {
      logInstallMsg("No Collection Selected To Install!. You have not selected any collection to install on your site!");
      writeInstallationError("No Collection Selected To Install!", "You have not selected any collection to install on your site!");
      return false;
   }
   $c = $_REQUEST['collection_choice'];
   $importer = new indySiteImporter($obj_set, $dummy);
   //set up choices here
   $importer->import_id = $c;
   logInstallMsg("Reading choices from collection for collection choice ".$c);
   $importer->readChoicesFromCollection($c);
   $importer->basic_choices['renumber'] = false;
   $importer->is_new_install = true;
   $importer->import("import");
   $features = $importer->data_collection->obj_set->getObjectStubsByTypename('FEATURE');
   if(count($features) > 0)
   {
      $system_config->front_page_id = $features[0]->obj_id;
   }
   else
   {
      $features = $importer->data_collection->obj_set->getObjectStubsByTypename('NEWSWIRE');
      if(count($features) > 0)
      {
         $system_config->front_page_id = $features[0]->obj_id;
      }
      else
      $system_config->front_page_id = 1;
   }
   logInstallMsg("front page object id set to: ".$system_config->front_page_id);
   $system_config->sql_query_caching_enabled = true;//empty out any cache
   $system_config->auto_cache_objects = false;
   $system_config->save();
   logInstallMsg("clearing sql cache.");
   $query_cache->clearCache("sql");
   echo '<div class="install-page">';
   writeInstallationResult("Stage 4: Importing Initial Templates", "<li class='install-result'>".implode("<li class='install-result'>", $importer->import_record), "");
   writeInstallationInstruction("Continue to Stage 5: Create Object Caches", "Ensure that there are no errors above and, when you are happy that your site template has been properly installed, click on the button to cache your data objects and create your site!");
   ?>
      <p align='center'>
      <input type='hidden' name="file_stage_done" value="confirmed">
      <input type='hidden' name="database_stage_done" value="confirmed">
      <input type='hidden' name="dataobjects_choices_done" value="confirmed">
      <input type='hidden' name="dataobjects_stage_done" value="confirmed">
      <input type='submit' name="dataobjects_caching_done"  value="Cache Objects &gt;&gt;"></p>
      </form>
      </div>
   <?
   logInstallMsg("End of stage 4 reached.");
}

function createObjectCaches()
{
   global $OSCAILT_SCRIPT, $object_import_store, $system_config, $query_cache, $redirectList;
   ?>
      <div class='install-logo'><img src='graphics/oscailtlogoanim.gif' alt='Oscailt' border=0></div>
      <form action="<?=$OSCAILT_SCRIPT?>">
   <?
   $system_config->sql_query_caching_enabled = false;//for install
   $system_config->load();
   $redirectList->load();
   $system_config->auto_cache_objects = true;
   $system_config->sql_query_caching_enabled = false;//for install

   logInstallMsg("Creating object caches.");
   $dummy = new indyObjectActionRequest();
   $obj_set = new indyObjectSet($system_config->xmltypedef_dir, $system_config->object_index_storage);
   if(!$obj_set->load(array("*"), array("*"), $dummy))
   {
      logInstallMsg("Error in creating Cache. Failed to load the object set with data types from $system_config->xmltypedef_dir");
      writeInstallationError("Error in creating Cache", "Failed to load the object set with data types from $system_config->xmltypedef_dir");
      $e = true;
   }
   $msg = "";
   foreach(array_keys($obj_set->itemCollection) as $obj_id)
   {
      logInstallMsg("Building object cache files for object id " .$obj_id);
      $obj_set->buildObjectCacheFiles($obj_id);
      $msg .= "<li class='install-result'>Cached Object $obj_id</li>";
      $obj_set->unload($obj_id);
   }
   $obj_set->object_name_cache->save();
   logInstallMsg("Cached object names.");
   $msg .= "<li class='install-result'>Cached Object Names</li>";

   $obj_set->supportedTypes->saveTypeDetailsCache();
   logInstallMsg("Cached type details.");
   $msg .= "<li class='install-result'>Cached Type Details</li>";

   echo '<div class="install-page">';
   logInstallMsg("Installation complete.");
   logInstallMsg("--------------------------------------------");

   writeInstallationResult("Stage 5: Creating Page Caches", $msg, "");
   writeInstallationInstruction("Installation Complete!", "<P>Now you should go to the <a href='admin.php'>administration page</a> logging in as admin/admin and configure some editors, roles, regions and topics as well as updating the basic site configuration!  You can also view the site as it is currently set up through the site <a href='index.php'>index page</a></p>");
   echo "</div>";
   return true;
}
function createWelcomeMsg()
{
   global $oscailt_basic_config;
   // First create the empty reminder file too.
   require_once "oscailt_init.inc";
   require_once("objects/reminderlist.inc");
   require_once("objects/publishstate.inc");
   

   $reminderMsgList = new ReminderList();
   $reminderMsgList->save();

   $editorStatusList = new PublishState();
   $editorStatusList->load();
   $editorStatusList->add("system", "post", time(),"Welcome to Oscailt ".$oscailt_basic_config['software_version'] );
   $editorStatusList->add("system", "post", time(),"Be sure to set the site URL in the Admin Configuration screen"); 
   $editorStatusList->save();
} 

function writeInstallationResult($header, $txt, $result)
{
   ?>
   <h2 align='center' class='install-header'><?=htmlspecialchars($header)?></h2>
   <div class='install-body'>
   <ul class='install-result'>
   <?=$txt?>
   </ul>
   </div>
   <?
}


function writeInstallationInstruction($header, $txt)
{
   ?><h2 align='center' class='install-header'><?=htmlspecialchars($header)?></h2>
   <div class='install-body'><?=$txt?></div><?
}

function writeInstallationError($header, $txt)
{
   ?><h3 align='center' style="color: #f00"><?=htmlspecialchars($header)?></h3>
   <p><?=$txt?></p><?
}
function logInstallMsg($message)
{
   global $system_config, $path_prefix, $logging_error;
   $file = $path_prefix."oscailt_install.log";

   $OUTPUT = $message."\r\n";
   $fp = fopen($file,"a"); // open file with append permission
   // If the file open fails report this.
   if ($fp) {
      fputs($fp, $OUTPUT);
      fclose($fp);
   } else {
      ?><p align='left' style="color: #f00000">Cannot write to install file: <?=$file?><br>
        <font style="color: #f000F0"><?=$message?></font>
	</p>
      <?

      if ($logging_error == false) {
        ?><p align='left'> Running as user: <?=get_current_user()?> Script owner UID <?=getmyuid()?><br>
	  </p>
        <?
      }
      $logging_error = true;
   }
}


$query_cache = new QueryCache();
// Had to move here so as to be in scope for any other functions.
$system_config = new SystemConfig();

if(isset($_REQUEST['file_stage_done']))
{
   if($_REQUEST['file_stage_done'] == 'confirmed')
   {
      if(isset($_REQUEST['database_stage_done']))
      {
         if($_REQUEST['database_stage_done'] == 'confirmed')
         {
            require_once "oscailt_init.inc";
            require_once "objects/indyobjects/indydataobjects.inc";
            require_once "objects/indyobjects/indysiteexporter.inc";
            require_once "objects/indyobjects/indysiteimporter.inc";
            if(isset($_REQUEST['dataobjects_choices_done']))
            {
               if(isset($_REQUEST['dataobjects_stage_done']))
               {
                  if(isset($_REQUEST['dataobjects_caching_done']))
                  {
			  if (createObjectCaches() == true) 
		          {
                              createWelcomeMsg();
			  }
                  }
                  else
                  {
                     importInitialDataObjects();
                  }
               }
               else
               {
                  writeDataObjectsChoices();
               }
            }
         }
         else
         {
            if(importDatabaseRecords())
            {
               writeImportObjectsIntro();
            }
         }
      }
   }
   else
   {
      if(checkRequiredFiles())
      {
         writeFileCheckingIntro();
      }
   }
}
else
{
   writeIntroBox();
}

$PAGE_CONTENTS = ob_get_contents();
ob_end_clean();
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd">
   <HTML>
   <HEAD>
      <meta http-equiv=Content-Type content="text/html; charset=iso-8859-1">
      <link href="graphics/install/style.css" type="text/css" rel="stylesheet">
      <TITLE> Oscailt Installation Script - <?=$pageTitle?> </TITLE>
   </HEAD>
<BODY>

<?
$got_page = false;
require_once("rescuepage.inc");
require_once("oscailt_destroy.inc");
require_once("footer.inc");
?>
