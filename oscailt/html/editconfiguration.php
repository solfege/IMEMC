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
require_once("oscailt_init.inc");
require_once("objects/videos.inc");

addToPageTitle("Main Oscailt Server Configuration");
$OSCAILT_SCRIPT = "editconfiguration.php";

// This is only called for the PHP info option
function writeLocalAdminHeader()
{
   global $OSCAILT_SCRIPT;

   ?>
     <TABLE class='admin'>
        <TR class='admin'><TD class='admin'><a href="<?=$OSCAILT_SCRIPT?>?phpinfo=false">Edit Configuration</a> | <a href="<?=$OSCAILT_SCRIPT?>?shmem=true">Shared Memory</a> | <a href="<?=$OSCAILT_SCRIPT?>?phpinfo=true">Installation Info</a> </TD></TR>
     </TABLE>
   <?
}
// This function displays some basic information about the PHP installation itself on the server.
function writeSharedMemoryConfig()
{
   global $system_config, $counterNames;

   ?>
   <table align=center class=admin width="65%">
   <tr class=admin>
      <th class=admin colspan=2><font size=+1>Shared Memory Configuration and Control Panel</font></th>
   </tr>
   <form name="memory_config" action="editconfiguration.php" method=post>
   <?
   if (extension_loaded('shmop') ) {
      if ($system_config->memory_mgmt_installed == true ) {
          writeInfoItem("<b>Shared Memory Features</b>","Installed");
      } else {
          writeInfoItem("Shared Memory Features","Not Installed");
      }
   } else {
          writeInfoItem("<P class='error'>Warning: PHP Shared Memory extension <b>'shmop'</b> is not loaded. This means that shared memory features cannot be used. Contact system administrator for details on how to install this.</P>","");
   }
   writeConfigBooleanItem("Shared Memory Activation","memory_mgmt_activated",$system_config->memory_mgmt_activated,"Tick box if you want to activate the shared memory. It must be activated to use it. <br>Activating the memory creates the memory counters and deactivating deletes them again from memory.");

   // Indicate when it was activated.
   if ($system_config->memory_mgmt_installed == true && $system_config->memory_mgmt_activated == true && $system_config->memory_mgmt_activate_time > 0) {
       $begin_txt = strftime("%a %d %b %H:%M:%S",$system_config->memory_mgmt_activate_time+$system_config->timezone_offset);
       $time_diff = time() - $system_config->memory_mgmt_activate_time;
       $time_diff_str = getTimeAgoString($time_diff, true);
       writeInfoItem("Shared Memory was Activated (".$time_diff_str.") at:", $begin_txt);
       ?>
       <tr class=admin>
       <td colspan=2 align=left>
           <input type=submit name=reset_sh_mem_btn value="Reset Shared Memory Counters and Date">
           <input type=hidden name=reset_sh_memory value="1" >
       </td>
       </tr>
       <?

   }

   if ($system_config->memory_mgmt_installed == true && $system_config->memory_mgmt_activated == false ) {
       ?>
       <tr class=admin>
       <td colspan=2 align=center>
           <input type=submit name=run_sh_mem_chk_btn value="Check Shared Memory Exists Already">
           <input type=hidden name=chk_sh_memory value="1" >
       </td>
       </tr>
       <?
   }

   ?>
   <tr class=admin>
      <th class=admin colspan=2>Shared memory counter names </th>
   </tr>
   <?

   global $counterNames;

   $memory_state = "in-active";
   if ($system_config->memory_mgmt_activated == true ) $memory_state = "active";

   foreach (array_keys($counterNames) as $key ) {
      // $value = SharedMemoryRead($key);
      if (is_array($key)) $t_key = $key[0];
      else $t_key = $key;
      writeInfoItem("Memory Counter ".$t_key, $memory_state );
   }
   // Save state of activation to detect if it was really changed later
   if ($system_config->memory_mgmt_installed == true ) {
       ?>
       <tr>
         <td colspan=2 align=center><input type=submit name=save_shared_memory value="Save configuration">
         <input type=hidden name=shmem value=true>
         <input type=hidden name=previous_activation value=<?=$system_config->memory_mgmt_activated?>>
         </td>
       </tr>
       <?
   }
   ?>
   </form>
   </table>
   <?


}
// This function displays the information your browser is returning.
function writeYourIdInfo()
{
   $server_only = array("SystemRoot", "COMSPEC", "SERVER_SIGNATURE","SERVER_SOFTWARE","PATH_TRANSLATED", "DOCUMENT_ROOT");

   ?>
   <table align=center class=admin width="65%">
   <tr class=admin>
      <th class=admin colspan=2><font size=+1>Your Browser is Providing the Following Information</font><br><small>Note: Some data is filtered out.</small></th>
   </tr>
   <tr class=admin>
      <th class=admin colspan=2>Contents of $_REQUEST </th>
   </tr>
   <?
      foreach ($_REQUEST as $key => $value)
      {
         writeInfoItem($key, $value );
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=2>Contents of $_SERVER  </th>
   </tr>
   <?
      foreach ($_SERVER  as $key => $value)
      {
         // Don't show stuff belonging to the server itself
	 if (in_array($key, $server_only)) continue;
         writeInfoItem($key, $value );
	 if ($key == "REMOTE_ADDR") writeInfoItem("Your Host Name", gethostbyaddr($_SERVER['REMOTE_ADDR']));
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=2>Contents of $_GET </th>
   </tr>
   <?
      foreach ($_GET  as $key => $value)
      {
         // Don't show stuff belonging to the server itself
	 if (in_array($key, $server_only)) continue;
         writeInfoItem($key, $value );
      }
   ?>
   <tr class=admin>
      <th class=admin colspan=2>Contents of $_SESSION  </th>
   </tr>
   <?
      foreach ($_SESSION  as $key => $value)
      {
         // Don't show stuff belonging to the server itself
	 if (in_array($key, $server_only)) continue;
         writeInfoItem($key, $value );
      }

   ?>
   </table>
   <?
}
function tidyUpValue($t_value)
{
    if ($t_value == null) return "NULL";
    if ($t_value == false) return "FALSE";
    return $t_value;

}
function displayMagpieSettings()
{
   ?>
   <tr class=admin>
      <th class=admin colspan=2><small> Magpie RSS settings </small> </th>
   </tr>
   <?

   // Have to include magpie stuff and then call init to set the settings so as to read them.
   require_once('objects/magpie/rss_fetch.inc');
   init();

   if (defined('MAGPIE_CACHE_ON'))
       writeInfoItem("MAGPIE_CACHE_ON:", tidyUpValue(MAGPIE_CACHE_ON) );
   else
       writeInfoItem("MAGPIE_CACHE_ON:", "Not Defined" );

   if (defined('MAGPIE_CACHE_DIR'))
       writeInfoItem("MAGPIE_CACHE_DIR:", tidyUpValue(MAGPIE_CACHE_DIR) );
   else
       writeInfoItem("MAGPIE_CACHE_DIR:", "Not Defined" );

   if (defined('MAGPIE_CACHE_AGE'))
       writeInfoItem("MAGPIE_CACHE_AGE:", tidyUpValue(MAGPIE_CACHE_AGE) );
   else
       writeInfoItem("MAGPIE_CACHE_AGE:", "Not Defined" );

   if (defined('MAGPIE_CACHE_FRESH_ONLY'))
       writeInfoItem("MAGPIE_CACHE_FRESH_ONLY:", tidyUpValue(MAGPIE_CACHE_FRESH_ONLY) );
   else
       writeInfoItem("MAGPIE_CACHE_FRESH_ONLY:", "Not Defined" );

   if (defined('MAGPIE_OUTPUT_ENCODING'))
       writeInfoItem("MAGPIE_OUTPUT_ENCODING:", tidyUpValue(MAGPIE_OUTPUT_ENCODING) );
   else
       writeInfoItem("MAGPIE_OUTPUT_ENCODING:", "Not Defined" );

   if (defined('MAGPIE_INPUT_ENCODING'))
       writeInfoItem("MAGPIE_INPUT_ENCODING:", tidyUpValue(MAGPIE_INPUT_ENCODING) );
   else
       writeInfoItem("MAGPIE_INPUT_ENCODING:", "Not Defined" );

   if (defined('MAGPIE_DETECT_ENCODING'))
       writeInfoItem("MAGPIE_DETECT_ENCODING:", tidyUpValue(MAGPIE_DETECT_ENCODING) );
   else
       writeInfoItem("MAGPIE_DETECT_ENCODING:", "Not Defined" );

   if (defined('MAGPIE_DEBUG'))
       writeInfoItem("MAGPIE_DEBUG:", tidyUpValue(MAGPIE_DEBUG) );
   else
       writeInfoItem("MAGPIE_DEBUG:", "Not Defined" );

}
function displaySuhosinSettings()
{
   ?>
   <tr class=admin>
      <th class=admin colspan=2> Suhosin (Hardened PHP) settings </th>
   </tr>
   <?

   if (!extension_loaded('suhosin')) {
       if (!dl('suhosin.so')) {
           // Extension not loaded.
           writeInfoItem("Suhoisin not loaded", "" );
           return ;
       }
   }
	
   // Have to include magpie stuff and then call init to set the settings so as to read them.

   $suhosin_groups = array("Logging Configuration" => array("suhosin.log.syslog",
                              "suhosin.log.syslog.facility",
                              "suhosin.log.syslog.priority",
                              "suhosin.log.sapi",
                              "suhosin.log.script",
                              "suhosin.log.phpscript",
                              "suhosin.log.script.name",
                              "suhosin.log.phpscript.name",
                              "suhosin.log.use-x-forwarded-for"),
                             "Executor Options" => array("suhosin.executor.max_depth",
                              "suhosin.executor.include.max_traversal",
                              "suhosin.executor.include.whitelist",
                              "suhosin.executor.include.blacklist",
                              "suhosin.executor.func.whitelist",
                              "suhosin.executor.func.blacklist",
                              "suhosin.executor.eval.whitelist",
                              "suhosin.executor.eval.blacklist",
                              "suhosin.executor.disable_eval",
                              "suhosin.executor.disable_emodifier",
                              "suhosin.executor.allow_symlink"),
                             "Misc Options" => array("suhosin.simulation",
                              "suhosin.apc_bug_workaround",
                              "suhosin.sql.bailout_on_error",
                              "suhosin.sql.user_prefix",
                              "suhosin.sql.user_postfix",
                              "suhosin.multiheader",
                              "suhosin.mail.protect",
                              "suhosin.memory_limit"),
                             "Transparent Encryption Options" => array("suhosin.session.encrypt",
                              "suhosin.session.cryptkey",
                              "suhosin.session.cryptua",
                              "suhosin.session.cryptdocroot",
                              "suhosin.session.cryptraddr",
                              "suhosin.session.checkraddr",
                              "suhosin.cookie.encrypt",
                              "suhosin.cookie.cryptkey",
                              "suhosin.cookie.cryptua",
                              "suhosin.cookie.cryptdocroot",
                              "suhosin.cookie.cryptraddr",
                              "suhosin.cookie.checkraddr",
                              "suhosin.cookie.cryptlist",
                              "suhosin.cookie.plainlist"),
                             "Filtering Options" => array("suhosin.filter.action",
                              "suhosin.cookie.max_array_depth",
                              "suhosin.cookie.max_array_index_length",
                              "suhosin.cookie.max_name_length",
                              "suhosin.cookie.max_totalname_length",
                              "suhosin.cookie.max_value_length",
                              "suhosin.cookie.max_vars",
                              "suhosin.cookie.disallow_nul",
                              "suhosin.get.max_array_depth",
                              "suhosin.get.max_array_index_length",
                              "suhosin.get.max_name_length",
                              "suhosin.get.max_totalname_length",
                              "suhosin.get.max_value_length",
                              "suhosin.get.max_vars",
                              "suhosin.get.disallow_nul",
                              "suhosin.post.max_array_depth",
                              "suhosin.post.max_array_index_length",
                              "suhosin.post.max_name_length",
                              "suhosin.post.max_totalname_length",
                              "suhosin.post.max_value_length",
                              "suhosin.post.max_vars",
                              "suhosin.post.disallow_nul",
                              "suhosin.request.max_array_depth",
                              "suhosin.request.max_array_index_length",
                              "suhosin.request.max_totalname_length",
                              "suhosin.request.max_value_length",
                              "suhosin.request.max_vars",
                              "suhosin.request.max_varname_length",
                              "suhosin.request.disallow_nul",
                              "suhosin.upload.max_uploads",
                              "suhosin.upload.disallow_elf",
                              "suhosin.upload.disallow_binary",
                              "suhosin.upload.remove_binary",
                              "suhosin.upload.verification_script",
                              "suhosin.session.max_id_length") );
                                                
                                                
    foreach ( $suhosin_groups as $group_name => $suhosin_options) {
        ?>
        <tr class=admin>
	<th class=admin colspan=2><small> <?=$group_name?> settings</small> </th>
        </tr>
        <?
        foreach ( $suhosin_options as $suhosin_keys) {
            writeInfoItem($suhosin_keys, ini_get($suhosin_keys) );
        }
    }
}
// This function displays some basic information about the PHP installation itself on the server.
function writePHPConfigInfo()
{
   global $system_config, $oscailt_basic_config;

   ?>
   <table align=center class=admin width="65%">
   <tr class=admin>
      <th class=admin colspan=2><font size=+1>Oscailt, Webserver, MySQL and PHP Installation Information</font></th>
   </tr>
   <tr class=admin>
      <th class=admin colspan=2>Version of Oscailt Installed on Server </th>
   </tr>
   <?
          writeInfoItem("Software version", $oscailt_basic_config['software_version'] );
          ?>
          <tr class=admin>
              <th class=admin colspan=2><small> Settings in config/systemconfig.php - changes by file only </small></th>
          </tr>
          <?
          writeInfoItem("Type Id for Features", $oscailt_basic_config['feature_type_id'] );
          writeInfoItem("Type Id for Events", $oscailt_basic_config['event_type_id'] );
          writeInfoItem("Default Language Code ", $oscailt_basic_config['default_language_code'] );
          writeInfoItem("Show page translations box", $oscailt_basic_config['show_page_translations_box'] );

          if ($oscailt_basic_config['enable_wysiwyg_editor'] == "")
              writeInfoItem("WYISWYG editor for publish form", "None");
	  else if ($oscailt_basic_config['enable_wysiwyg_editor'] == "TinyMCE" OR $oscailt_basic_config['enable_wysiwyg_editor'] == "OpenWYSIWYG" )
              writeInfoItem("WYISWYG editor for publish form", $oscailt_basic_config['enable_wysiwyg_editor']);
	  else 
              writeInfoItem("WYISWYG editor for publish form", $oscailt_basic_config['enable_wysiwyg_editor']." <font color='red'>- Warning: Invalid setting</font>" );

          if ($oscailt_basic_config['comment_vote_hide_block'] == true)
              writeInfoItem("Comment hides blockable by one or more votes", "Enabled");
	  else 
              writeInfoItem("Comment hides blockable by one or more votes", "Disabled");

          writeInfoItem("Debug level", $oscailt_basic_config['debug_level'] );

          ?>
          <tr class=admin>
              <th class=admin colspan=2><small> Settings for embedded video controlling whether video is displayed by default or not. </small></th>
          </tr>
          <?
          writeInfoItem("Default for whether to show embedded video cover images or not", $oscailt_basic_config['embedded_video_default_mode'] );
	  $vid_list="";
          foreach ($oscailt_basic_config['safe_video_list'] as $safe_video )
          {
              $vid_list .= getEmbeddedVideoTypes($safe_video) . "<BR> ";
          }
          writeInfoItem("Safe Video List ", $vid_list );
	  $vid_list="";
          foreach ($oscailt_basic_config['video_cover_image_retrieval'] as $safe_video )
          {
              $vid_list .= getEmbeddedVideoTypes($safe_video) . " <BR> ";
          }
          writeInfoItem("Video Cover Image Retrieval List ", $vid_list );

	  displayMagpieSettings();
          displaySuhosinSettings();
   ?>
   <tr class=admin>
      <th class=admin colspan=2>Webserver and CGI Installed on Server </th>
   </tr>
   <?
      if (isset($_SERVER['SERVER_SOFTWARE'])) {
          writeInfoItem("Webserver Software", $_SERVER['SERVER_SOFTWARE'] );
          writeInfoItem("Server Protocol", $_SERVER['SERVER_PROTOCOL'] );

	  // Only display if var is set
	  if (stristr($_SERVER['SERVER_SOFTWARE'], "Apache") && isset($_REQUEST['apache']) ) 
          {
              $apache_mods_info = apache_get_modules();
	      if (count($apache_mods_info) > 0)
              {
	          $mod_list = "";
                  foreach ($apache_mods_info as $module_entry )
                  {
	              $mod_list .= $module_entry . "<BR>";
                  }
                  writeInfoItem("Apache Modules Loaded:", $mod_list );
              }
          }
      }
      if (isset($_ENV['OS'])) {
          writeInfoItem("Operating System (via ENV['OS'] ) ", $_ENV['OS'] );
      }
      // Maybe too much info 
      // writeInfoItem("Operating System (via php_uname ) ", php_uname() );
      writeInfoItem("Operating System (via PHP_OS ) ", PHP_OS );

      // This function is in memorymgmt.inc. It really should be utilities at this stage....
      $t_up = serverUpTime(true);
      if ($t_up != null) {
           $time_ago_str = getTimeAgoString($t_up);
	   writeInfoItem("Server Uptime ", $time_ago_str);
       }

   ?>
   <tr class=admin>
      <th class=admin colspan=2>Version of MySQL Database Installed on Server </th>
   </tr>
   <?
      $mysql_version_info = getMySQLVersion();
      if ($mysql_version_info == "" )
          writeInfoItem("MySQL Version:"," No version information available.");
      else
          writeInfoItem("MySQL Version:", $mysql_version_info );
	  
      $mysql_status_info = getMySQLUptime();
      if ($mysql_status_info == null )
          writeInfoItem("Uptime"," No status information available.");
      else
      {
          foreach ($mysql_status_info as $stat_entry )
          {
             writeInfoItem($stat_entry[0], $stat_entry[1] );
          }
      }

      $mysql_status_info = getMySQLStatus("collation_database");
      if ($mysql_status_info == null )
          writeInfoItem("collation_database"," No status information available.");
      else
      {
          foreach ($mysql_status_info as $stat_entry )
          {
             writeInfoItem($stat_entry[0], $stat_entry[1] );
          }
      }

      $mysql_status_info = getMySQLStatus("character_set_database");
      if ($mysql_status_info == null )
          writeInfoItem("character_set_database"," No status information available.");
      else
      {
          foreach ($mysql_status_info as $stat_entry )
          {
             writeInfoItem($stat_entry[0], $stat_entry[1] );
          }
      }
      $mysql_status_info = getMySQLStatus("character_set", true);
      if ($mysql_status_info == null )
          writeInfoItem("character_set"," No status information available.");
      else
      {
          foreach ($mysql_status_info as $stat_entry )
          {
             writeInfoItem($stat_entry[0], $stat_entry[1] );
          }
      }


   ?>
   <tr class=admin>
      <th class=admin colspan=2>Version of PHP Installed on Server </th>
   </tr>
   <?
      $php_version_info = phpversion();
      if ($php_version_info == false )
          writeInfoItem("PHP Version:"," No version information available.");
      else
          writeInfoItem("PHP Version:", $php_version_info );
   ?>
   <tr class=admin>
      <th class=admin colspan=2>Some Basic php.ini File Settings </th>
   </tr>
   <?
      // Get some key ini settings.
      // Some sites may set LimitRequestBody in apache config file under tag: Files *.php
      writeInfoItem("error_reporting ", ini_get('error_reporting') );
      writeInfoItem("track_errors ", ini_get('track_errors') );
      writeInfoItem("display_errors ", ini_get('display_errors') );
      writeInfoItem("html_errors ",    ini_get('html_errors') );
      writeInfoItem("log_errors",    ini_get('log_errors') );
      writeInfoItem("error_log",    ini_get('error_log') );

      writeInfoItem("register_globals ", ini_get('register_globals') );
      writeInfoItem("magic_quotes_gpc ", ini_get('magic_quotes_gpc') );

      // These control upload limits
      writeInfoItem("memory_limit ", ini_get('memory_limit') );
      writeInfoItem("post_max_size ",ini_get('post_max_size') );
      /*
      $php_array = ini_get_all();
      foreach ($php_array as $key => $val) {
	  if (is_array($val) ) {
		  $list_str = "";
		 foreach ($val as $k => $v) {
		  $list_str .= $k . " ".$v ."<BR>";
		}
          writeInfoItem($key ,$list_str );
	  } else 
          writeInfoItem($key ,$val );
      }
      */

      // Values returned as XX M or XX Mb so have to remove the letters.
      $t_max_post  = trim(preg_replace("/[A-Za-z]/","",ini_get('post_max_size')));
      $t_max_upload= trim(preg_replace("/[A-Za-z]/","",ini_get('upload_max_filesize')));
      if ( $t_max_post < $t_max_upload ) {
          // Breaking message out here so it will be easier to translate in the future.
          $t_warning = "<br><div class='error'>Warning: post_max_size should be greater than or equal to upload_max_filesize</div>";
          writeWarningItem($t_warning);
      }
      writeInfoItem("upload_max_filesize ", ini_get('upload_max_filesize') );

      writeInfoItem("upload_tmp_dir", ini_get('upload_tmp_dir') );
      writeInfoItem("file_uploads  ", ini_get('file_uploads') );
      writeInfoItem("max_execution_time  ", ini_get('max_execution_time') );

      writeInfoItem("allow_url_fopen", ini_get('allow_url_fopen') );
      writeInfoItem("safe_mode", ini_get('safe_mode') );

      writeInfoItem("session.auto_start",   ini_get('session.auto_start') );
      writeInfoItem("session.cache_expire", ini_get('session.cache_expire') );
      writeInfoItem("session.gc_divisor",   ini_get('session.gc_divisor') );
      writeInfoItem("session.gc_maxlifetime", ini_get('session.gc_maxlifetime') );
      writeInfoItem("session.use_cookies",  ini_get('session.use_cookies') );
      writeInfoItem("session.use_trans_sid",ini_get('session.use_trans_sid') );


      /*
      foreach ( $php_ini_array as $ini_key => $ini_values) 
      {
         $detail = "";
         foreach ( $ini_values as $ini_subkey => $ini_subvalue) 
         {
             $detail .= $ini_subkey . " = " . $ini_subvalue . "<BR>";
         }
         writeInfoItem($ini_key , $detail );
      }
      */

      $show_functions = false;
      if ( isset($_REQUEST['showfuncs']) && $_REQUEST['showfuncs'] == 'true') $show_functions = true;
   ?>
   <tr class=admin>
      <th class=admin colspan=2>Installed PHP Extensions on Server</th>
   </tr>
   <?

   if ($show_functions == false) {
   ?>
   <tr class=admin>
       <td class="admin" colspan=2 align=center>
           <form name="main_config_sys" action="editconfiguration.php" method="POST">
           <input type=submit name=show_ext_funcs_btn value="Show PHP Extension Functions">
           <input type=hidden name=phpinfo  value="true" >
           <input type=hidden name=showfuncs value="true" >
           <input type=hidden name=shmem value="" >
           </form>
       </td>
   </tr>
   <?
   }

      $php_ext_array = get_loaded_extensions();
      foreach ( $php_ext_array as $ext_key => $ext_name) 
      {
          $func_list = "";
          if ($show_functions == true) {
	      $func_array = get_extension_funcs($ext_name);
	      foreach ($func_array as $func_name) {
                  if ($func_list == "") $func_list = "<br><i> &nbsp; Extension Functions</i> <BR> &nbsp; " .$func_name;
		  else $func_list .= "<BR> &nbsp; ". $func_name;
	      }
          }
          if ($ext_name == "shmop") $ext_name .= " &nbsp; (Shared Memory)";
          $bold_ext_name = "<b>".$ext_name. "</b>";
          writeInfoItem("PHP Extension ", $bold_ext_name . $func_list );

          if ($ext_name == "gd") {
              $gd_info_array = gd_info();
              foreach ( $gd_info_array as $gd_key => $gd_value) 
              {
                  if ($gd_value == null || $gd_value == false) $gd_val_info = "No";
		  else if ($gd_value == 1 ) $gd_val_info = "Yes";
		  else $gd_val_info = $gd_value;
                  writeInfoItem(" &nbsp; GD Support Info ". $gd_key, $gd_val_info );
              }
          }
      }
   ?>
   </table>
   <?
}
// Return the uptime for mysql and convert it to days, hrs, mins
function getMySQLUptime()
{
    global $dbconn;
    // Zero means no db cache
    $result = sql_query("SHOW STATUS LIKE 'uptime'", $dbconn, 0);
    checkForError($result);

    $db_uptime = array();
    $db_uptime_name = "";
    $db_uptime_value = "";

    if(sql_num_rows( $result ) > 0)
    {
        list($db_uptime_name, $db_uptime_value) = sql_fetch_row($result, $dbconn);
        $db_uptime_string = getTimeAgoString($db_uptime_value );
        $db_uptime[] = array($db_uptime_name, $db_uptime_string);
    }

    return $db_uptime;
}

function getMySQLStatus($variable_str, $exact_match = false)
{
    global $dbconn;
    // Zero means no db cache
    if ($exact_match == true) {
       $result = sql_query("SHOW VARIABLES LIKE '". $variable_str."'", $dbconn, 0);
    } else {
       $result = sql_query("SHOW VARIABLES LIKE '". $variable_str."%'", $dbconn, 0);
    }
    checkForError($result);

    $db_status = array();
    $db_status_name = "";
    $db_status_value = "";

    if(sql_num_rows( $result ) > 0)
    {
        for ($iresult =0; $iresult < sql_num_rows( $result );$iresult++)
        {
            list($db_status_name,$db_status_value) = sql_fetch_row($result, $dbconn);
            $db_status[] = array($db_status_name, $db_status_value);
        }
    }

    return $db_status;
}

function getMySQLVersion()
{
    global $dbconn;
    // Zero means no db cache
    $result = sql_query("SELECT VERSION()", $dbconn, 0);
    checkForError($result);

    $db_version = "";

    if(sql_num_rows( $result ) > 0)
    {
        list($db_version) = sql_fetch_row($result, $dbconn);
    }

    return $db_version;
}

function writeConfigForm()
{
   global $system_config;
   ?>
   <table align=center class=admin>
   <tr class=admin>
      <th class=admin colspan=4><font size=+1>Site Configuration</font></th>
   </tr>
   <form name="main_config" action="editconfiguration.php" method=post>
   <input type=hidden name=memory_mgmt_activated value=<?=$system_config->memory_mgmt_activated?> >
   <input type=hidden name=memory_mgmt_activated value=<?=$system_config->memory_mgmt_activated?> >
   <input type=hidden name=check_spamwords value=<?=$system_config->check_spamwords?> >
   <?
      writeConfigHeader("Site Identification","The basic data used to identify your site. This is used in RSS news syndication and throughout the site to generate links and references to this site as well as to create the page titles");
      writeConfigItem("Short Name","site_short_name",$system_config->site_short_name,"The name of your site");
      writeConfigItem("Long Name","site_long_name",$system_config->site_long_name,"The full (longer) name of your site");


      writeConfigBooleanItem("Use Friendly URL's","use_friendly_urls",$system_config->use_friendly_urls,"Do you want the site to use search engine friendly URLs for each page. For example this means that you can use urls like <i>article/23432</i> instead of <i>?obj_id=234&story_id=23432</i>.  Note, this requires you to install the .htaccess file");



      writeConfigTextAreaItem("Description","site_description",$system_config->site_description,"Details of your site - is used in the page description meta tag indexed by search engines");
      writeConfigTextAreaItem("Keywords","site_keywords",$system_config->site_keywords,"Comma separated keywords of the site used in the page keywords meta tag. This is usually indexed by search engines");

      writeConfigItem("URL","site_url",$system_config->site_url,"The url with no http:// or trailing /");
      writeConfigHeader("Writable File Storage for Attachments, Images and Style-Sheets","The directories that attachments, style-sheets and cached images are stored in<BR>Ensure that these directories are writable by the user the apache process is running under and is accessible from the web-server root.");

      writeConfigItem("Attachment Store File System Path","attachment_store",$system_config->attachment_store,"The path of the attachment store - either the absolute path from the system root or the path relative to the oscailt installation directory (must have trailing slash)");


      writeConfigItem("URL Path","attachment_store_url",$system_config->attachment_store_url,"The path of the attachment store (must have trailing slash). If you want to serve your attachments for a different server, you can enter its URL here, otherwise, just use the path of the attachments directory relative to the oscailt web root or its url");


      writeConfigItem("Image Cache File System Path","image_cache",$system_config->image_cache,"The path of the image cache relative to the oscailt installation directory (no trailing slash)");


      writeConfigItem("Site File Store File System Path","site_file_store_base",$system_config->site_file_store_base,"The path to the site file-storage directory relative to the oscailt installation directory (must have trailing slash)");




      writeConfigHeader("Private File Storage for caches and data object storage","The directories that oscailt will use to store private internal data in.<BR>Ensure that these directories are writable by the user the apache process is running under, but they should not be publically accessible.");


      writeConfigItem("Private Cache Directory File System Path","private_cache",$system_config->private_cache,"The path of the cache store relative to the web root (must have trailing slash)");


      writeConfigItem("Object Index Storage","object_index_storage",$system_config->object_index_storage,"Either enter <b>sql</b> if you want to store data object indices in the database or enter the path of the object index file relative to the web root.");


      writeConfigItem("Data Objects Storage Location","new_objects_storage",$system_config->new_objects_storage,"Either enter <b>sql</b> if you want to store data objects in the database, or enter the path of the object storage directory relative to the web root (must have trailing slash)");


      writeConfigItem("Exported Objects File System Path","object_export_dir",$system_config->object_export_dir,"The path to the directory where exported objects will be saved.  This can be visible from your web-server if you want to publically share your data object configurations.(must have trailing slash)");





      writeConfigHeader("Maximum Attachment Sizes","The max size of file uploads (may not apply to editors depending on permissions),<BR>Values may not exceed max file size set in php.ini. Values may be set to lower values for individual sites, but these limits are are the maximum allowed for all the sites configured as part of this oscailt installation");

      writeConfigNumericItem("Image Files","image_attachment_max_size",$system_config->image_attachment_max_size,"","Kilobytes",10,2048,10);
      writeConfigNumericItem("Image Files (width)","image_attachment_max_width",$system_config->image_attachment_max_width,"","Pixels",10,2400,10);

      $t_max_filesize = ini_get('upload_max_filesize');
      $t_max_filesize = trim(preg_replace("/[A-Za-z]/","",$t_max_filesize));
      $t_one_mb = 1024 * 1024;

      writeConfigNumericItem("Video Files","video_attachment_max_size",$system_config->video_attachment_max_size,"","Megabytes",1,100,1);
      if (floor($system_config->video_attachment_max_size/$t_one_mb) > $t_max_filesize) {
          writeWarningItem("Warning: Max Video File Size is greater than PHP upload_max_filesize setting!");
      }

      writeConfigNumericItem("Audio Files","audio_attachment_max_size",$system_config->audio_attachment_max_size,"","Megabytes",1,100,1);
      if (floor($system_config->audio_attachment_max_size/$t_one_mb) > $t_max_filesize) {
          writeWarningItem("Warning: Max Audio File Size is greater than PHP upload_max_filesize setting!");
      }

      writeConfigNumericItem("Miscellaneous Files","miscellaneous_attachment_max_size",$system_config->miscellaneous_attachment_max_size,"","Megabytes",1,100,1);
      if (floor($system_config->miscellaneous_attachment_max_size/$t_one_mb) > $t_max_filesize) {
          writeWarningItem("Warning: Max Miscellaneous File Size is greater than PHP upload_max_filesize setting!");
      }



      // Story Attachments
      writeConfigHeader("Story Attachments","These restrictions do not apply to editors with the ignoreattachmentlimits permission");

      writeConfigBooleanItem("Allow Image Files","allow_story_image_attachments",$system_config->allow_story_image_attachments,"");
      writeConfigBooleanItem("Allow Video Files","allow_story_video_attachments",$system_config->allow_story_video_attachments,"");
      writeConfigBooleanItem("Allow Audio Files","allow_story_audio_attachments",$system_config->allow_story_audio_attachments,"");
      writeConfigBooleanItem("Allow Miscellaneous Files","allow_story_miscellaneous_attachments",$system_config->allow_story_miscellaneous_attachments,"");
      writeConfigNumericItem("Max Attachments Per Story","story_max_attachments",$system_config->story_max_attachments,"This Maximum number of attachments that a user may attach to a story","Files",1,100,1);
      writeConfigNumericItem("Editor Max Attachments","editor_max_attachments",$system_config->editor_max_attachments,"This only applies to intial publication, editors may add more files when editing stories","Files",1,100,1);


      // Comment Attachments
      writeConfigHeader("Comment Attachments","These restrictions do not apply to editors with the ignoreattachmentlimits permission");
      writeConfigBooleanItem("Allow Image Files","allow_comment_image_attachments",$system_config->allow_comment_image_attachments,"");
      writeConfigBooleanItem("Allow Video Files","allow_comment_video_attachments",$system_config->allow_comment_video_attachments,"");
      writeConfigBooleanItem("Allow Audio Files","allow_comment_audio_attachments",$system_config->allow_comment_audio_attachments,"");
      writeConfigBooleanItem("Allow Miscellaneous Files","allow_comment_miscellaneous_attachments",$system_config->allow_comment_miscellaneous_attachments,"");
      writeConfigNumericItem("Max Attachments Per Comment","comment_max_attachments",$system_config->comment_max_attachments,"This only applies to intial publication, editors may add more files","Files",1,100,1);



      writeConfigHeader("Content Length Restrictions","Maximum Limits for content fields.");

      writeConfigNumericItem("Max User Comment Length","user_max_comment",$system_config->user_max_comment,"","Kilobytes",1,64,1);
      writeConfigNumericItem("Max User Story Summary Length","user_max_summary",$system_config->user_max_summary,"","Kilobytes",1,64,1);
      writeConfigNumericItem("Max User Story Contents Length","user_max_contents",$system_config->user_max_contents,"","Kilobytes",1,500,5);
      writeConfigNumericItem("Editor Max Comment Length","editor_max_comment",$system_config->editor_max_comment,"","Kilobytes",1,64,1);
      writeConfigNumericItem("Editor Max Story Summary Length","editor_max_summary",$system_config->editor_max_summary,"","Kilobytes",1,64,1);
      writeConfigNumericItem("Editor Max Story Contents Length","editor_max_contents",$system_config->editor_max_contents,"","Kilobytes",1,500,5);



      writeConfigHeader("Email Settings","Settings relating mainly to the automatic mails sent when editors modify content on the website. These settings may be over-written for individual site sections in the installation.");

      writeConfigItem("Notification From Address","notification_from_email_address",$system_config->notification_from_email_address,"The address from which all mail generated by this site will be sent. You must ensure that this address is allowed to send from your configured mailserver, and that if sending to a mailing list it is allowed post to it");

      writeConfigItem("Notification To Address","notification_to_email_address",$system_config->notification_to_email_address,"The address to which all automatic email notifications will be sent, usually a list set up for this purpose");

      writeConfigItem("Notification Reply To Address","notification_replyto_email_address",$system_config->notification_replyto_email_address,"The address (or addresses separated by comma) which will be added to the replyto section of the auto notifications. Useful if you wish discussion of hidden posts to occur on separate list to the one notifications are sent to");

      writeConfigItem("Contact Address","contact_email_address",$system_config->contact_email_address,"The address that submissions to the contact form will be sent");



      // HTML / Code Input Settings 
      writeConfigHeader("HTML / Code Input Settings","You can choose whether HTML tags will be allowed in publish forms and how it will be processed");

      writeConfigBooleanItem("Allow HTML in story summaries?","allow_rich_content_in_summary",$system_config->allow_rich_content_in_summary,"Should users and editors be allowed to use (limited and defined in markupconfig.inc) HTML in story summaries?");

      writeConfigBooleanItem("Allow HTML in story contents?","allow_rich_content_in_story",$system_config->allow_rich_content_in_story,"Should users and editors be allowed to use (limited and defined in markupconfig.inc) HTML in story contents?");

      writeConfigBooleanItem("Allow HTML in comments?","allow_rich_content_in_comment",$system_config->allow_rich_content_in_comment,"Should users and editors be allowed to use (limited and defined in markupconfig.inc) HTML in comments?");

      //needs a lot more work!
      //writeConfigBooleanItem("Support Limited BB code?","allow_bb_code",$system_config->allow_bb_code,"Should users and editors be allowed to use (limited and defined in markupconfig.inc) BB Code (eg [B]bold text[/B] as well as HTML?");


      writeConfigNumericItem("Maximum BBcoded text","maximum_allowed_bbed_text",$system_config->maximum_allowed_bbed_text,"The maximum number of caracters that can be enclosed in BB tags - allows you to stop enormous quotes or all-bold comments.", "",20,5000,10);


      writeConfigBooleanItem("Force Correct HTML Input?","force_correct_html_input",$system_config->force_correct_html_input,"Should users and editors be notified of errors in their HTML and forced to fix them before they publish? If you do not check this box, disallowed tags will still be stripped from the content, but the user will not be told of it.");

      writeConfigBooleanItem("Force Correct HTML Input for data objects?","force_correct_html_object_input",$system_config->force_correct_html_object_input,"Should users and editors be notified of errors in their HTML and forced to fix them before they publish updates to data objects? If you do not check this box, disallowed tags will still be stripped from the content, but the user will not be told of it.");

      writeConfigBooleanItem("Enforce strict HTML Parsing?","enforce_strict_tag_closing",$system_config->enforce_strict_tag_closing,"Should Oscailt forbid unclosed tags, even in cases where it does not affect display (such as p, li, etc)  This will help you to ensure that your pages are standards compliant.");



      // Data Object Page Settings 
      writeConfigHeader("Data Object Page Settings","You can choose which page will be the default front page on your site as well as some other basic choices of page layouts");

      writeConfigItem("Front Page Object","front_page_id",$system_config->front_page_id,"The Data object module that will be the default (index.php) entry point to the site.");


      writeConfigItem("Administration Page Layout Object","adminpage_obj_id",$system_config->adminpage_obj_id,"The Page Layout setting that the administration page (admin.php) will be view through.");

      writeConfigItem("Article For Previews","articleview_preview_story",$system_config->articleview_preview_story,"In order to properly preview changes to article module settings, you need to specify a valid ID of any story in the database to use in preview mode.");




      // Object Caching Settings
      writeConfigHeader("Object Caching Settings","You can choose to cache objects (strongly recommended to use caching and leave these fields unchecked) as html files to speed up operation");

      writeConfigBooleanItem("Inactivate Object Caching?","use_live_objects",$system_config->use_live_objects,"Use Live Objects instead of cached ones (not recommended)?");

      writeConfigBooleanItem("Automatically Cache Object Updates?.","auto_cache_objects",$system_config->auto_cache_objects,"Immediately re-cache updated objects (not recommended).");

      writeConfigBooleanItem("Allow Live Access?.","allow_live_objects",$system_config->allow_live_objects,"Allow Users To view live objects rather than cached ones? (not recommended).");
      writeConfigBooleanItem("Activate HTML Caching?","use_html_cache",$system_config->use_html_cache,"Enable HTML caching. This is used for only a few of the main pages.");




      writeConfigHeader("IP Blocks and Bans","Block and ban ip addresses and referers from accessing the site.");

      writeConfigTextAreaItem("Banned IP addresses","banned_ips",$system_config->banned_ips,"Enter IP addresses, one per line.  Any users whose address matches these addresses will not be allowed to access the site at all.");

      writeConfigTextAreaItem("Banned Referers","banned_referers",$system_config->banned_referers,"Enter Referers (sites that link to this site) that you want to block. Blocking nazi sites such as shitefront.org is a good idea.");

      writeConfigItem("Redirect URL","redirect_banned_url",$system_config->redirect_banned_url,"A URL to redirect banned posters to.  Whatever you think, gay inter-racial porn or the UNDRM - it's your pick.");

      writeConfigItem("Banned SpamBot IP List","spam_ip_list",$system_config->spam_ip_list,"Enter what you think are Spam Bot IP addresses. Separate each one by spaces. These will be blocked from accessing the site.");

      writeConfigItem("Banned SpamBot IP List by POST","spam_post_ip_list",$system_config->spam_post_ip_list,"Enter what you think are Spam Bot IP addresses that you want to ban from doing POST requests to the site. Separate each one by spaces.");
      // writeConfigBooleanItem("Turn on spam words checking for publish stories","check_spamwords",$system_config->check_spamwords,"Spam words are entered in a separate screen and are checked for in the story content during publish and rejected if they are present.");



      // Logging and Security
      writeConfigHeader("Logging and Security","Settings that govern system security and logging");

      writeConfigBooleanItem("Enable IP monitoring?.","publish_monitor_enabled",$system_config->publish_monitor_enabled,"Enable Temporary recording of users who publish stories and comments on the site.  This allows you to block abusers immediately.");

      writeConfigNumericItem("IP Monitor Cache Size","monitor_size_limit",$system_config->monitor_size_limit,"The number of ip addresses that will be stored.  So, if you set this value to 20, oscailt will never store more than the last 20 ip addresses of publishers in the cache", "",1,80,1);

      writeConfigNumericItem("User Status Cache Size","status_monitor_size_limit",$system_config->status_monitor_size_limit,"The number of User status and messages that will be stored. So, if you set this value to 20, oscailt will never store more than the last 20 user status and messages in the cache", "",1,120,1);

      writeConfigNumericItem("Security Logging Level","security_recording_level",$system_config->security_recording_level,"The level of security logging that you want.  Set to 0 for no security logging, 1 for logging of malicious attempts to inject scripts into forms and 2 for logging of all tags and bad data.", "",0,2,1);

      writeConfigNumericItem("Security IP Recording","security_ip_recording",$system_config->security_ip_recording,"Do you want to log IP addresses to the security log.  Set to 0 for no IP logging, 1 for logging of IP addresses with malicious attempts to inject scripts into forms and 2 for logging of all IP addresses with bad tags and bad data.  This will only take effect (obviously) if the above setting includes logging of the events", "",0,2,1);



      if ($system_config->system_file_override == true )
      {
          writeInfoItem("<span class='error'>Warning: systemconfig.inc file value override is in effect for <b>user_error reporting</b>!</span>","<span class='error'>".$system_config->user_error_reporting."</span>");
      }
      writeConfigNumericItem("Enable User Error Reporting","user_error_reporting",$system_config->user_error_reporting,"The level of error reporting about internal errors that you want to be visible to users.  Set to 0 for no error messages, 1 for a standard 'try back later message' and 2 for reporting of php and database error messages.", "",0,2,1);


      // Miscellaneous Section
      writeConfigHeader("Miscellaneous","Various Settings that don't fit anywhere else!");

      writeConfigBooleanItem("Embedded Audio Player","audio_player_installed",$system_config->audio_player_installed,"Tick box if the XSPF Embedded Audio Player has been installed. This must be installed to allow embedded audio publishing. Then configure options in Article and Newswire modules");
      if (extension_loaded('shmop') ) {
          writeConfigBooleanItem("Shared Memory Features","memory_mgmt_installed",$system_config->memory_mgmt_installed,"Tick box if you want to make use of the features implemented through the PHP shared memory extension. This covers article counters and other counter types.");
      } else {
          // If lib not installed but setting on, turn it off and warn user.
          if ($system_config->memory_mgmt_installed == true ) {
              writeInfoItem("<P class='error'>Note: Value of memory management installation flag was previously set to true. This will now be set to false if the data is saved on this screen.<BR><BR>Warning: PHP Shared Memory extension <b>'shmop'</b> is not loaded. This means that shared memory features cannot be used. Contact system administrator for details on how to install this.</P>","");
          } else {
              writeInfoItem("<P class='error'>Warning: PHP Shared Memory extension <b>'shmop'</b> is not loaded. This means that shared memory features cannot be used. Contact system administrator for details on how to install this.</P>","");
          }
      }

      writeConfigBooleanItem("Disable Open Publishing","disable_open_publish",$system_config->disable_open_publish,"Turn off ability of public to publish. Useful for emergencies caused by spammers.<br><i>Some sites may not have open publishing enabled at the publish object level in which case this has no effect </i>");

      writeConfigBooleanItem("Enable Public Editing","enable_public_editing",$system_config->enable_public_editing,"Turn on ability of public to edit stories they have published or which have passwords set for a set period. This is useful for correcting and other edits.<br><i>A similar option must be enabled in the public module in the admin layout section for this to have an effect</i>");

      writeConfigNumericItem("Comments Publish Delay Duration","publish_comment_delay",$system_config->publish_comment_delay,"The amount of time in minutes to delay publish of comments. Selecting zero disables it.","Minutes", 0,80,10, true);

      writeConfigBooleanItem("Enable Newswire Thumbnails","newswire_thumbnails_enabled",$system_config->newswire_thumbnails_enabled,"Controls whether thumbnails of the first image in a story are shown on the newswire page along with the story summary");


      writeHelpJS();
      $help_str = getDateFormatHelp();
      writeConfigItem("Default Date Format","default_strftime_format",$system_config->default_strftime_format,"The Default Date String Format (strftime format eg %A %B %d, %Y %H:%M).".$help_str);


      writeConfigNumericItem("Edit Lock Duration","edit_locking_time",$system_config->edit_locking_time,"The maximum amount of time in minutes that an editor can keep a story locked from other editors while they edit it.","Minutes", 0,90,1);

      writeConfigBooleanItem("Pre-fill Editor Details","prepopulate_editor_details",$system_config->prepopulate_editor_details,"You can choose to pre-fill the editor's name and other details into publish forms when editors wish to publish stories.");


      writeConfigBooleanItem("Mono Lingual Filter","mono_lingual_filter",$system_config->mono_lingual_filter,"Controls whether the default view of the site includes all languages or just one. If enabled the default language filter used is the one requested by the clients browser preferences");

      writeConfigNumericItem("Timezone Offset","timezone_offset",$system_config->timezone_offset,"The offset to shift all times displayed on the site by, to cater for timezones other than the one this site is hosted in. This offset is relative to the host site's local time not GMT.","Hours",-23,23,1);


      writeConfigNumericItem("Refresh Rate","rss_query_cache_expiry",$system_config->rss_query_cache_expiry,"How often the local cache of syndicated news feeds should be updated","Hours",1,72,1);

   ?>
   <tr>
      <td colspan=2 align=center><input type=submit name=save value="Save configuration"></td>
      </form>
   </tr>
   </table>
   <?
}

function getDateFormatHelp()
{
     $help_text = "The following conversion specifiers are recognized in the format string:<br>
     <ul>
     <li> %a - abbreviated weekday name according to the current locale
     <li> %A - full weekday name according to the current locale
     <li> %b - abbreviated month name according to the current locale
     <li> %B - full month name according to the current locale
     <li> %c - preferred date and time representation for the current locale
     <li> %C - century number (the year divided by 100 and truncated to an integer, range 00 to 99)
     <li> %d - day of the month as a decimal number (range 01 to 31)
     <li> %D - same as %m/%d/%y
     <li> %e - day of the month as a decimal number, a single digit is preceded by a space (range ' 1' to '31')
     <li> %g - like %G, but without the century.
     <li> %G - The 4-digit year corresponding to the ISO week number (see %V). This has the same format and value as %Y, except that if the ISO week number belongs to the previous or next year, that year is used instead.
     <li> %h - same as %b
     <li> %H - hour as a decimal number using a 24-hour clock (range 00 to 23)
     <li> %I - hour as a decimal number using a 12-hour clock (range 01 to 12)
     <li> %j - day of the year as a decimal number (range 001 to 366)
     <li> %m - month as a decimal number (range 01 to 12)
     <li> %M - minute as a decimal number
     <li> %n - newline character
     <li> %p - either `am' or `pm' according to the given time value, or the corresponding strings for the current locale
     <li> %r - time in a.m. and p.m. notation
     <li> %R - time in 24 hour notation
     <li> %S - second as a decimal number
     <li> %t - tab character
     <li> %T - current time, equal to %H:%M:%S
     <li> %u - weekday as a decimal number [1,7], with 1 representing Monday 
         <BR> Warning: Sun Solaris seems to start with Sunday as 1 although ISO 9889:1999 (the current C standard) clearly specifies that it should be Monday.
     <li> %U - week number of the current year as a decimal number, starting with the first Sunday as the first day of the first week
     <li> %V - The ISO 8601:1988 week number of the current year as a decimal number, range 01 to 53, where week 1 is the first week that has at least 4 days in the current year, and with Monday as the first day of the week. (Use %G or %g for the year component that corresponds to the week number for the specified timestamp.)
     <li> %W - week number of the current year as a decimal number, starting with the first Monday as the first day of the first week
     <li> %w - day of the week as a decimal, Sunday being 0
     <li> %x - preferred date representation for the current locale without the time
     <li> %X - preferred time representation for the current locale without the date
     <li> %y - year as a decimal number without a century (range 00 to 99)
     <li> %Y - year as a decimal number including the century
     <li> %Z - time zone or name or abbreviation
     <li> %% - a literal `%' character 
      </ul>";
      return getHelpHTML($help_text, false, true, true);
}
function writeConfigItem($displayName,$name,$value,$details)
{
   ?>
   <tr class=admin valign=top>
      <td class=admin>&nbsp;<B><?=$displayName?></B>&nbsp;<BR><small><?=$details?></small></td>
      <td class=admin><input size=40 name="<?=$name?>" value="<?=$value?>"></td>
   </tr>
   <?
}


function writeConfigTextAreaItem($displayName,$name,$value,$details)
{
   ?>
   <tr class=admin valign=top>
      <td class=admin>&nbsp;<B><?=$displayName?></B>&nbsp;<BR><small><?=$details?></small></td>
      <td class=admin><textarea rows=5 cols=36 name="<?=$name?>"><?=$value?></textarea></td>
   </tr>
   <?
}


function writeConfigBooleanItem($displayName,$name,$value,$details)
{
   ?>
   <tr class=admin valign=top>
      <td class=admin>&nbsp;<B><?=$displayName?></B>&nbsp;<BR><small><?=$details?></small></td>
      <td class=admin><input type=checkbox name="<?=$name?>" <? if($value==true) echo("checked"); ?>></td>
   </tr>
   <?
}


function writeConfigNumericItem($displayName,$name,$value,$details,$units,$min,$max,$increment,$multiplier_off=false)
{
   if ($multiplier_off == true) {
       $multiplier = 1;
   } else {
       if($units=="Kilobytes") $multiplier = 1024;
       else if($units=="Megabytes") $multiplier = 1024*1024;
       else if($units=="Minutes") $multiplier = 60;
       else if($units=="Hours") $multiplier = 60*60;
       else if($units=="Days") $multiplier = 60*60*24;
       else if($units=="Weeks") $multiplier = 60*60*24*7;
       else $multiplier = 1;
   }
   $display_value=$value/$multiplier;
   ?>
   <tr class=admin valign=top>
      <td class=admin>&nbsp;<B><?=$displayName?></B>&nbsp;<BR><small><?=$details?></small></td>
      <td class=admin>
      <select name="<?=$name?>">
      <?
      for($i=$min;$i<=$max;$i=$i+$increment)
      {
         ?><option <? if($display_value==$i) echo("selected ");?>value=<?=$i*$multiplier?>>
         <? if($units=="Hours" && $i>0 && $min<0) echo("+");?> <?=$i?><?
      }
      ?>
      </select>
      <?=$units?></td>
   </tr>
   <?
}


function writeInfoItem($header,$details)
{
   ?>
   <tr class=admin>
      <td class=admin>&nbsp;<?=$header?></td><td class=admin> <?=$details?></td>
   </tr>
   <?
}

function writeWarningItem($warn_msg)
{
   ?>
     <tr class=admin>
        <td class=admin colspan=2><div class='error'>&nbsp;<?=$warn_msg?></div></td>
     </tr>
   <?
}
function writeConfigHeader($header,$details)
{
   ?>
   <tr class=admin>
      <th class=admin colspan=2>&nbsp;<?=$header?>&nbsp;</th>
   </tr>
   <tr class=admin>
      <td class=admin colspan=2><small><?=$details?></small></td>
   </tr>
   <?
}

function reportPublishMonitorChanges()
{
    global $system_config, $editor_session;

    require_once("objects/publishstate.inc");
    $editorStatusList = new PublishState();
    $editorStatusList->load();

    if ($system_config->publish_monitor_enabled) {
        $system_config->publish_monitor_due_off = 3600 * 24;
        $system_config->publish_monitor_began = time();
        $_REQUEST['action_reason'] = "Turned off in Admin Configuration screen. Defaulting to 24 hrs.";
        logAction("", "N/A", "IP-Monitor", "Turned on");

        // Now generate a message to the Oscailt messaging system
        $sysmsg = "User: " . $editor_session->editor->editor_name . " turned on monitor by configuration change in admin configuration screen. Default period for 24 hrs";

    } else {
        // Flush the list since it is being turned off.
        require_once('objects/publishmonitor.inc');
        $monitorList = new PublishMonitor();
        $monitorList->recent_publishes = array();
        $monitorList->save();

        $_REQUEST['action_reason'] = "Turned off in Admin Configuration screen.";
        logAction("", "N/A", "IP-Monitor", "Turned off");
        $sysmsg = "User: " . $editor_session->editor->editor_name . " turned off monitor by configuration change in admin configuration screen.";
    }
    $editorStatusList->add("system", "post",time(), $sysmsg);
    $editorStatusList->save();

}
function reportPublishDelayChanges($last_value)
{
    global $system_config;

    if ($system_config->publish_comment_delay) {
        if ($last_value == 0)
            $_REQUEST['action_reason'] = "Publish comment delay switched on to ".$system_config->publish_comment_delay." mins in Admin Configuration screen.";
	else
            $_REQUEST['action_reason'] = "Publish comment delay changed from ".$last_value." mins to ".$system_config->publish_comment_delay." mins in Admin Configuration screen.";

        logAction("", "N/A", "config", "Turned on");

    } else {
        $_REQUEST['action_reason'] = "Publish comment delay switched off in Admin Configuration screen.";
        logAction("", "N/A", "config", "Turned off");
    }

}




ob_start();
if($editor_session->isSessionOpen())
{
   global $system_config;

   if ($system_config->memory_mgmt_installed == true ) {
       $link_array = array();
       if ($system_config->memory_mgmt_installed == true )
	   $link_array["editconfiguration.php?shmem=true"] = "Shared Memory Activation";

       $link_array["editconfiguration.php?showid=true"] = "Show Idenity";

       //writeAdminHeader("editconfiguration.php?phpinfo=true","System Installation Info", array("editconfiguration.php?shmem=true" =>"Shared Memory Activation","editconfiguration.php?showid=true" =>"Show Idenity"));
       writeAdminHeader("editconfiguration.php?phpinfo=true","System Installation Info", $link_array);
   } else {
       writeAdminHeader("editconfiguration.php?phpinfo=true","System Installation Info");
   }

   if($editor_session->editor->allowedReadAccessTo("editconfiguration"))
   {
      $display_config = true;
      if ( isset($_REQUEST['shmem']) && $_REQUEST['shmem'] == 'true')
      {
          // writeLocalAdminHeader();
          if(isset($_REQUEST['save_shared_memory']) && $_REQUEST['save_shared_memory'] !=null)
          {
              if($editor_session->editor->allowedWriteAccessTo("editconfiguration"))
              {
                 // When activating or deactivating shared memory we need to create or destroy it.
                 $system_config->loadFormValues("memory_mgmt_activated"); 
                 // This form var previous_activation is used to detect if value really changed.
                 if(isset($_REQUEST['previous_activation'])) {
                     if( $_REQUEST['previous_activation'] != $system_config->memory_mgmt_activated) {
                         // At this point activation state is only changed in this thread
                         // If you are turning off, save state first, then delete
                         // If you are turning on, create memory first then save state
                         if( $system_config->memory_mgmt_activated == false) {
                             $system_config->memory_mgmt_activate_time = 0;
                             $system_config->saveMemoryState();
                             $system_config->load();
                             SharedMemoryStateChange(); 
                             echo "<P class='error'>Configuration Saved!</P>";
                             logAction("", "0", "shared-memory", "deactivated");
		         } else {
                             // Switching on...
                             if (SharedMemoryStateChange() == true) {
                                 $system_config->memory_mgmt_activate_time = time();
                                 $system_config->saveMemoryState();
                                 $system_config->load();
                                 echo "<P class='error'>Configuration Saved!</P>";
                                 logAction("", "0", "shared-memory", "activated");
		             } else {
                                 $system_config->load();
                                 echo "<P class='error'>Shared Memory Activation Error. No Configuration Update.</p>";
		             }
		         }
                     } else {
                         echo "<P class='error'>No Net Configuration Change.</P>";
                     }

                 }
                 // echo "shared memory: ".$system_config->memory_mgmt_activated ."<BR>";
              }
              else $editor_session->writeNoWritePermissionError();
          }
	  else if(isset($_REQUEST['reset_sh_memory']) && $_REQUEST['reset_sh_memory'] !=null)
          {
              $system_config->memory_mgmt_activate_time = time();
              $system_config->updateMemoryDate();
              $system_config->load();
              $error_msg = SharedMemoryReset();
	      if ($error_msg == "") {
                  echo "<P class='error'>Shared Memory reset successfully!</P>";
                  logAction("", "0", "shared-memory", "reset counters");
	      } else {
                  echo $error_msg;
                  echo "<P class='error'>Shared Memory reset with errors!</P>";
	      }
          }
	  else if(isset($_REQUEST['chk_sh_memory']) && $_REQUEST['chk_sh_memory'] !=null)
          {
              SharedMemoryExistCheck();
          }

          writeSharedMemoryConfig();
          $display_config = false;
      } else if ( isset($_REQUEST['phpinfo']) && $_REQUEST['phpinfo'] == 'true')
      {
          // writeLocalAdminHeader();
          writePHPConfigInfo();
          $display_config = false;
      } else if ( isset($_REQUEST['showid']) && $_REQUEST['showid'] == 'true')
      {
          writeYourIdInfo();
          $display_config = false;
      }
      else if(isset($_REQUEST['save']) && $_REQUEST['save'] !=null)
      {
         if($editor_session->editor->allowedWriteAccessTo("editconfiguration"))
         {
            // Checking to see if publish_monitor_enabled changed because it would need to
            // be reported to the action logs. This block of code should really be in a function.
	    $prev_publish_monitor_enabled = $system_config->publish_monitor_enabled;
	    $prev_publish_comment_delay   = $system_config->publish_comment_delay;
            $system_config->loadFormValues();

	    if ($prev_publish_monitor_enabled != $system_config->publish_monitor_enabled) {
                // This handles any logging for changes.
                reportPublishMonitorChanges();
            }
	    if ($prev_publish_comment_delay != $system_config->publish_comment_delay) {
                // This handles any logging for changes.
                reportPublishDelayChanges($prev_publish_comment_delay);
            }
            $system_config->save();
            $system_config->load();
            echo "<P class='error'>Configuration Saved!</P>";

         }
         else $editor_session->writeNoWritePermissionError();
      }
      if ($display_config == true)
         writeConfigForm();
   }
   else $editor_session->writeNoReadPermissionError();
}
else $editor_session->writeNoSessionError();

include_once("adminfooter.inc");
?>
