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



if(!isset($OSCAILT_SCRIPT))

{

   $OSCAILT_SCRIPT = 'index.php';

}

require_once("oscailt_init.inc");

require_once("oscailt_msg.inc");

SharedMemoryUpdate("index");

$user_prefs->updateSavedPreferences();

$redirectList->load();



$target_indyobject_id = getRequestTargetObjectID();

if($target_indyobject_id > 10200)

{

   require_once "header.inc";

   ?>Invalid Request<?

   require_once("oscailt_destroy.inc");

   require_once("footer.inc");

   exit;

}



if($OSCAILT_SCRIPT == 'test.php')

{

   $system_config->use_live_objects = true;

   $system_config->use_friendly_urls = false;

}

$use_live = $system_config->use_live_objects;

$allow_live = $system_config->allow_live_objects;

$suppress_page_insets = false;



//first execute the code for the requested object

//the main pane is run first and buffered to allow it to set things like the page title and so on.

ob_start();



if(!$use_live)

{

   $cachefile = getObjectCacheIndexFile($target_indyobject_id);

   if(file_exists($cachefile))

   {

      include_once($cachefile);

   }

   elseif($allow_live)

   {

      $use_live = true;

   }

}

if($use_live)

{

   require_once("objects/indyobjects/indydataobjects.inc");

   $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $OSCAILT_SCRIPT);

   $lang_code = $userLanguage->getLanguageCodePrefix();

   $result = $obj_man->getLiveObjectHTML($target_indyobject_id, $lang_code);

   if($result)

   {

      eval(' ?> '.$result.' <?');

      if(isHTMLPage() && !$system_config->use_live_objects)

      echo "<div class='error'>Failed to load cached object - using live object</div>";

   }

   else

   {

      $obj_man->writeUserMessageBox();

   }

}





if(!isHTMLPage())

{

   ob_end_flush();

   disconnectFromDatabase();

   exit;

}





if(isset($_GET['print_page']))

{

   require_once "headerprint.inc";

   ob_end_flush();//   echo $PAGE_CONTENTS;

   require_once "footerprint.inc";

}

else

{

   $PAGE_CONTENTS = ob_get_contents();

   $file_size = strlen($PAGE_CONTENTS);

   ob_end_clean();

   if($performance_test > 2) markTime("Processed Main Page Code");



   $got_layout = false;

   $use_live = $system_config->use_live_objects;

   if(!$use_live && isset($PAGE_LAYOUT_ID))

   {

      $page_cachefile = getObjectCacheIndexFile($PAGE_LAYOUT_ID);

      if(file_exists($page_cachefile))

      {

         require_once "header.inc";

	 checkForOscailtMsg();

         include_once($page_cachefile);

         $got_layout = true;

      }

      elseif($allow_live)

      {

         $use_live = true;

      }

   }

   if($use_live && isset($PAGE_LAYOUT_ID))

   {

      if(!isset($obj_man))

      {

         require_once("objects/indyobjects/indydataobjects.inc");

         $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $OSCAILT_SCRIPT);

         $lang_code = $userLanguage->getLanguageCodePrefix();

      }

      $result = $obj_man->getLiveObjectHTML($PAGE_LAYOUT_ID, $lang_code);

      if($result)

      {

         require_once "header.inc";

         eval(' ?> '.$result.' <?');

         if(!$system_config->use_live_objects)

             echo "<div class='error'>Failed to load cached object - using live object</div>";



         $got_layout = true;

      }

   }

   if(!$got_layout)

   {

      require_once "rescueheader.inc";

      require_once "rescuepage.inc";

   }

   unset($PAGE_CONTENTS);



   require_once("oscailt_destroy.inc");

   require_once("footer.inc");

}

?>

