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

require_once($path_prefix."objects/indyobjects/indydataobjects.inc");
require_once($path_prefix."objects/adminutilities.inc");
error_reporting(E_ALL);
class AdminPageObject extends indyObject
{
   var $my_url;
   var $uni_options;
   var $site_options;

   function AdminPageObject()
   {
      global $OSCAILT_SCRIPT;
      $this->my_url = $OSCAILT_SCRIPT;
   }

   function load()
   {
      global $system_config, $userLanguage;

      $this->uni_options = new indyItemSet();
      if($this->uni_options->load($system_config->xml_store, "universal_config_options") === false)
      {
         $this->setUserMessages( $this->uni_options->getUserMessages() );
         $this->setUserMessage( array( USER_ERROR, "Failed to load the list of Editor Options", "4tx" ) );
         return false;
      }
      $this->setUserMessages( $this->uni_options->getUserMessages() );
      if($this->uni_options->not_translated)
      {
	 $this->setUserMessage(array (USER_WARNING, "Note: this page has not been translated into your language (".$userLanguage->getName().") yet!", "xre") ); 
      }

      $this->site_options = new indyItemSet();
      if($this->site_options->load($system_config->xml_store, "site_management_options") === false)
      {
         $this->setUserMessages( $this->site_options->getUserMessages() );
         $this->setUserMessage( array( USER_ERROR, "Failed to load the list of Editor Options", "" ) );
         return false;
      }
      $this->setUserMessages( $this->site_options->getUserMessages() );
      if($this->site_options->not_translated)
      {
	 $this->setUserMessage(array (USER_WARNING, "This Page has not been translated into your language (".$userLanguage->getName().") yet!", "") );
      }
      return true;
   }

   /************************************************************************/
   /* Write a set of administrator options
   /* Input: set of items
   /* Output: none;
   /************************************************************************/
   function writeMenuItems($allItems, $cols)
   {
      $current_col = 0;
      $col_width = (int)(100 / $cols);
      foreach($allItems as $item)
      {
         if(!isset($item->type)) continue;
         if($current_col % $cols == 0) echo '<tr class="admin">';
         $current_col++;
         if($item->type == 'sitechoiceaction')
         {
            if(($current_col % $cols) != 0)
            {
               echo '<td colspan="2" width="'.($col_width*2).'%" class="admin" valign="top">';
               $current_col++;
            }
            else
            {
               echo '<td width="'.$col_width.'%" class="admin" valign="top">';
            }
            echo '<form style="margin: 0; display: inline" name="sitechooser" action="'.$this->my_url.'" method="GET">';
            echo '<div style="margin: 10px 4px 4px 4px; float: right; vertical-align: middle;">';
            echo '<input type="image" name="site_builder" value="go" border=0 src="'.$item->getIcon().'" alt="'.$item->getMeta("title").'">';
            echo "</div>";
            echo '<h4 class="admin">';
            echo '<a class="sitebuilder-filter" href=\'Javascript:document.sitechooser.submit()\'">';
            echo $item->getMeta("title");
            echo "</a></h4>\n";
            echo '<p class="admin"><small>';
            echo $item->getMeta('description');
            echo $item->getMeta('input_field');
            echo "</small></p>\n";
         }
         else
         {
            echo '<td width="'.$col_width.'%" class="admin" valign="top">';
            echo '<div style="margin: 10px 4px 4px 4px; float: right; vertical-align: middle;">';
            echo '<a href="'.$item->getMeta("link").'">';
            echo '<img border=0 src="'.$item->getIcon().'" alt="'.$item->getMeta("title").'">';
            echo "</a></div>";
            echo '<h4 class="admin">';
            echo '<a class="sitebuilder-filter" href="'.$item->getMeta("link").'">';
            echo $item->getMeta("title");
            echo "</a></h4>\n";
            echo '<p class="admin"><small>';
            echo $item->getMeta('description');
            echo "</small></p>\n";
         }
         echo '</td>';
         if($current_col % $cols == 0) echo '</tr>';
      }
      if($current_col % $cols != 0)
      {
         while($current_col % $cols != 0)
         {
            echo '<td class="admin">&nbsp;</td>';
            $current_col++;
         }
         echo '</tr>';
      }

   }

   function writeSideMenuItems($allItems, $title, $section_msg)
   {
      echo '<tr class="admin"><th class="admin" bgcolor="yellow">';
      //echo '<div style="margin: 5px 4px 4px 4px; float: left; vertical-align: top; border 5;">';
      // echo '<h4 class="object-name-list">'.$title . "</h4><ul class='sub-menu'>";
      // echo $title ;
      echo '<div class="admin-nav">'.$title . '</div>';
      echo '</th></tr>';
      echo '<tr class="admin"><td class="admin" >';
      echo '<div style="margin: 0px 4px 4px 4px; float: left; vertical-align: top;">';
      echo '<ul class="sub-menu">';
      foreach($allItems as $item)
      {
         if($item->type == 'sitechoiceaction') {
            echo '<form style="margin: 0; display: inline" name="sitechooser" action="'.$this->my_url.'" method="GET">';
            echo '<div style="margin: 10px 4px 4px 4px; float: right; vertical-align: middle;">';
            echo "</div>";
            echo '<li><a href=\'Javascript:document.sitechooser.submit()\'">';
            echo $item->getMeta("title");
            echo "</a>\n";
            echo '<br><small>';
            echo $item->getMeta('input_field');
            echo "</small></li>\n"; 
	 } else {
             echo '<li> <a href="'.$item->getMeta("link").'">'.$item->getMeta("title").'</a>';
             echo "</li>";
	 }
      }
      echo "</ul></div>";
      echo '</td></tr>';

   }


   function processRequest()
   {
      global $editor_session, $system_config;

      SharedMemoryUpdate("admin");
      if(isset($_REQUEST['silent']) && $_REQUEST['silent'] == true) 
      {
	 $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
         ?> <a href="<?=$http_mode?>://<?=$system_config->site_url?>/admin.php?main=true">Admin</a>
         &nbsp; <a href="<?=$http_mode?>://<?=$system_config->site_url?>/admin.php?dashboard=1">Dashboard</a>
	 <?
	 setPageTitle("");
         return;
      }

      if(!$this->load())
      {
         $this->setUserMessage( array( USER_ERROR, "Failed to Process Administration Request - Internal Error", "dsg" ) );
         $this->writeUserMessageBox();
         return false;
      }
      $this->setUserMessage( array( USER_MSG, "Welcome ".$editor_session->editor->editor_name, "" ) );
      $this->writeUserMessageBox();

      if(!isset($_REQUEST['task']) && !isset($_REQUEST['site_id']) && !isset($_REQUEST['dashboard']))
      {
         // Check the editor settings on whether to display dashboard by default or not.
         if(isset($_REQUEST['main']) && $_REQUEST['main'] == 'true' ) {
             $this->writeAdminPage();
	 }
         elseif ($editor_session->editor->isDashboardOn()) {
	     // Display Dash board 
             global $admin_table_width;
             $admin_table_width = "95%";
	      
             $this->writeAdminDashboardPage();
	 }
         else $this->writeAdminPage();
      }
      elseif(isset($_REQUEST['dashboard']))
      {
	 // Display Dash board 
         global $admin_table_width;
         $admin_table_width = "95%";
	      
         $this->writeAdminDashboardPage();
      }
      elseif(isset($_REQUEST['site_id']))
      {
         // This displays the admin page version for site building and all sub options
         $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $this->my_url);
         $res = $obj_man->processRequest($this->site_options);
         if ($res === false)
         {
            $obj_man->writeUserMessageBox();
         }
      }
      elseif(isset($_REQUEST['task']))
      {
         $tsk = $_REQUEST['task'];
         if($tsk = "editstylesheets")
         {
	    $available_sites = null;
            $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $this->my_url);
            $obj_man->obj_set = new indyObjectSet($obj_man->type_dir, $obj_man->storage);
            $sites = array("*");
            $types = array("site");
            if(!$obj_man->obj_set->load($sites, $types, $obj_man->action_req))
            {
                $obj_man->writeUserMessageBox();
            }
	    else
	    {
	       $available_sites = $obj_man->obj_set->getObjectStubs();
	    }
            $this->writeStyleSheetSection($obj_man, $available_sites);
         }
         else
         {
         writeFailureMessage("Coming Soon", "This functionality is part of the experimental Oscailt-Community System");
         }
      }
   }

   // Handle the Admin page Style sheeting viewing and editing.

   function writeStyleSheetSection($obj_man, $available_sites)
   {
      global $system_config, $pseudo_directory_offset, $OSCAILT_SCRIPT;
      $section_title = $this->site_options->getString("style_management");
      $section_intro = $this->site_options->getString("style_intro");
      $ctxt_str = "<UL class='context'>\n";
      $title = $this->uni_options->getMeta("title");
      $ctxt_str .= "<LI class='context'><a class='admin-context' href='$OSCAILT_SCRIPT'>$title</a></LI>\n";
      $ctxt_str .= "<LI class='context'><a class='admin-context' href='".$OSCAILT_SCRIPT."?task=editstylesheets'>$section_title</a></LI>\n";
      $ctxt_str .= "</UL>\n";
      writeAdminPageHeader($section_title, $ctxt_str, $section_intro, "");
      $st_store = $pseudo_directory_offset.$system_config->site_file_store_base.'default/';
      $lnk_base = $OSCAILT_SCRIPT."?task=editstylesheets";
      if(isset($_REQUEST['editstyle']) && ($_REQUEST['editstyle'] == 'common.css'  or $_REQUEST['editstyle'] == 'style.css' or $_REQUEST['editstyle'] == 'contentstyle.css') && !isset($_REQUEST['cancelstylee']))
      {
         $result = $this->writeStyleEditSection($_REQUEST['editstyle']);
         $this->writeUserMessageBox();
      }

      if(isset($_REQUEST['viewstyle']) && ($_REQUEST['viewstyle'] == 'common.css'  or $_REQUEST['viewstyle'] == 'style.css' or $_REQUEST['viewstyle'] == 'contentstyle.css') && !isset($_REQUEST['cancelstylee']))
      {
         $result = $this->writeStyleEditSection($_REQUEST['viewstyle'], true);
         $this->writeUserMessageBox();
      }

      $css_1 = 'common.css';
      $css_2 = 'style.css';
      $css_3 = 'contentstyle.css';
      $edit_common_lnk = "<a href='".$lnk_base."&amp;editstyle=$css_1'>";
      $edit_style_lnk = "<a href='".$lnk_base."&amp;editstyle=$css_2'>";
      $edit_contentstyle_lnk = "<a href='".$lnk_base."&amp;editstyle=$css_3'>";
      $edit = $this->site_options->getString("edit");

      $view = $this->site_options->getString("view");
      $view_common_lnk = "<a href='".$lnk_base."&amp;viewstyle=$css_1'>";
      $view_style_lnk = "<a href='".$lnk_base."&amp;viewstyle=$css_2'>";
      $view_contentstyle_lnk = "<a href='".$lnk_base."&amp;viewstyle=$css_3'>";
      if ($view == null) $view = "View";

      echo "<P><table width='100%' cellspacing=4 cellpadding=0>";
      echo "<tr><td><b>".$css_1."</b></td><td> Contains style classes for administration and layout pages </td><td align='right'>".$edit_common_lnk.$edit."</a> | ". $view_common_lnk.$view."</a> </td></tr>";
      echo "<tr><td><b>".$css_2."</b></td><td> Contains style classes for menus, boxes, page layouts, links, headings etc used by public site </td><td align='right'>".$edit_style_lnk.$edit."</a> | ".$view_style_lnk.$view."</a> </td></tr>";
      echo "<tr><td><b>".$css_3."</b></td><td> Contains style classes used by features </td><td align='right'>".$edit_contentstyle_lnk.$edit."</a> | ". $view_contentstyle_lnk.$view."</a> </td></tr>";
      echo "</table>";

      // echo "<P><div align='left'>$edit_common_lnk"."$edit $css_1</a> | $edit_style_lnk"."$edit $css_2</a> | $edit_contentstyle_lnk"."$edit $css_3</a></div>";
      //echo " <div='right'>$view_common_lnk"."$view $css_1</a> | $view_style_lnk"."$view $css_2</a> | $view_contentstyle_lnk"."$view $css_3</a></div></P>";
      
      echo "<P><BR> <b>Note</b>: Style Sheet files directory is: <b>".$st_store."</b></P>";

      if (count($available_sites) > 0) 
      {
          echo '<table width="80%" border=0 class="admin">';
          echo '<tr><th class="admin" colspan=3>Style Sheet (CSS) File Usage by Site </th></tr>';
          foreach ($available_sites as $each_obj) 
          {
              $new_obj = $obj_man->obj_set->fetchObject($each_obj->obj_id, "en");
              $t_mode = $new_obj->getMeta('different_css');
              if ($t_mode == "on") $t_msg = "<b>non-default</b>";
              else $t_msg = "<b>these default</b>";
              echo "<tr class='admin'><td class='admin'>Site: <b>".$new_obj->name()."</b> (".$each_obj->obj_id .")</td><td> is using ".$t_msg. " Style Sheet (CSS) Files.<BR /> </td>";
              if ($t_mode == "on") 
	      {
		  echo "<td class='admin'><a href='admin.php?action=site-files&site_id=".$each_obj->obj_id."'>Edit Page &gt;&gt;</a></td>";
	      } else {
		  echo "<td class='admin'>&nbsp;</td>";
	      }
              echo "</tr>";
          }
          echo '</tr></table>';
      }
      echo '</div>';

   }

   function writeStyleEditSection($target, $view_only=false)
   {
      global $system_config, $pseudo_directory_offset, $OSCAILT_SCRIPT;
      $file_target = $pseudo_directory_offset.$system_config->site_file_store_base.'default/'.$target;
      $lnk = $OSCAILT_SCRIPT."?task=editstylesheets";

      if(!file_exists($file_target) or !is_writable($file_target))
      {
         $this->setUserMessage( array(USER_ERROR, "Style sheet $file_target does not exist or is not writable!", "x3z" ));
         return false;
      }
      if(isset($_REQUEST['confirm']))
      {
         $contents = $_REQUEST['style-contents'];
         if (!$handle = fopen($file_target, 'w'))
         {
            $this->setUserMessage( array ( USER_ERROR, "Failed to open output style sheet file $file_target", "" ));
            return false;
         }
         if ( !fwrite( $handle, $contents )  )
         {
            $this->setUserMessage( array ( USER_ERROR, "Failed to write style sheet file $file_target", "" ) );
            fclose( $handle );
            return false;
         }
         $this->setUserMessage( array ( USER_MSG, "Updated style sheet file $file_target", "" ) );
         fclose( $handle );
         return true;
      }
      $contents = implode("", file($file_target));
      $lnk .= "&amp;editstyle=$target";

      echo "<br /><div align = 'center'>";
      echo "<form name='editstyle' action='$lnk' method='POST'>";
      echo "<table class='admin' border=0 ><tr class='admin'><th class='admin'>";

      if ($view_only == true) {
          echo $target."</th></tr>";
      } else {
          $edit = $this->site_options->getString("edit");
          echo $edit." ".$target."</th></tr>";
      }

      //echo "<tr class='admin'><td class='admin' style='background:yellow'>";
      echo "<tr class='admin'><td class='admin'>";
      if ($view_only == true) {
          //echo "<pre>".htmlspecialchars($contents, ENT_QUOTES) . "</pre>";
          // echo '<font size="+1">';
          if (isset($_REQUEST['font']) && ($_REQUEST['font'] == "1" or $_REQUEST['font'] == "2" or $_REQUEST['font'] == "3"))
              echo '<font size="'.$_REQUEST['font'].'">';
	  else
              echo '<font size="3">';
          echo "<pre class='style_view'>".$contents."</pre>" ;
          echo '</font>';
      } else {
          echo "<textarea rows='44' cols='400' name='style-contents'>".htmlspecialchars($contents, ENT_QUOTES)."</textarea>";
      }
      $cancel_text = $this->site_options->getString("cancel_button");
      $confirm_text = $this->site_options->getString("save_changes");
      echo "</td></tr>";

      // Report details on the file size and when it was last updated.
      $number_bytes = filesize($file_target);
      // Report it in Kb
      $number_bytes = round(100 * $number_bytes /1024 ) / 100 ;
      // Note: stat returns an array and item 10 is the modified time.
      $file_modt = filemtime($file_target);
      $time_msg = strftime("%H:%M:%S, %a %d %b", $file_modt);

      $t_link = $OSCAILT_SCRIPT."?".$_SERVER['QUERY_STRING'];
      echo "<tr class='admin'><td class='admin'><div align='left'>Filesize: ".$number_bytes." Kb. Last update: ".$time_msg."</div><div align='right'>";
      echo "<a href='".$t_link."&font=1'><font size='2'>A</font></a> <a href='".$t_link."&font=2'><font size='3'>A</font></a> <a href='".$t_link."&font=3'><font size='4'>A</font></a> </div> </td></tr>";

      echo "<tr class='admin'><th class='admin'>";
      echo "<div class='editorconfirmbuttons'><input type=submit name='cancelstylee' value='<< $cancel_text'>";
      if ($view_only == false) {
          echo "<input type=submit name='confirm' value='$confirm_text >>'>";
      }
      echo "</div></th></tr></table></div>";
   }

   // Dashboard display function to display summary info
   // Server Uptime Publish mode + Monitor enabled, Spam mode
   // users msgs 
   // language setting momo lang
   // Totals for stories, comments and attachments in last 24 hrs
   // links to admin site logout msgs
   function writeAdminDashboardPage()
   {
      #require_once("oscailt_init.inc");
      addToPageTitle('Dashboard ');
      require_once("oscailt_msg.inc");
      require_once("objects/memorymgmt.inc");
      require_once("objects/redirector.inc");
      require_once("objects/publishstate.inc");
      require_once("objects/reminderlist.inc");
      global $editor_session, $system_config, $userLanguage, $oscailt_basic_config, $prefix;
      global $redirectList;

      // Code should be cleaned up later. 
      if (isset($_REQUEST['disable_open_publish']) && $_REQUEST['disable_open_publish'] != "") {
          // echo("Key pressed ". $_REQUEST['disable_open_publish']);
          if ($editor_session->editor->allowedWriteAccessTo("viewstatus")) {
		  if ($_REQUEST['disable_open_publish'] == "Disable Publishing Now") {
		  // echo  " off ";
                  //$system_config->disable_open_publish = null;
                  checkForError(sql_query("UPDATE ".$prefix."_configuration SET value='1' WHERE name='disable_open_publish'", $dbconn, 1));
                  logAction("", "N/A", "Open-Publish", "Turned off"); 
                  $editorStatusList = new PublishState();
                  $editorStatusList->load();
                  $editorStatusList->add("system", "post", time(),"Open Publishing disabled dashboard by user: ".$editor_session->editor->editor_name);
                  $editorStatusList->save();

              } else {
		  // echo  " on ";
                  //$system_config->disable_open_publish = true;
                  checkForError(sql_query("UPDATE ".$prefix."_configuration SET value=NULL WHERE name='disable_open_publish'", $dbconn, 1));
                  logAction("", "N/A", "Open-Publish", "Turned on"); 
                  $editorStatusList = new PublishState();
                  $editorStatusList->load();
                  $editorStatusList->add("system", "post", time(),"Open Publishing re-enabled from dashboard by user: ".$editor_session->editor->editor_name);
                  $editorStatusList->save();
		  
              }
              $system_config->load();
          }
      }

      $current_time = time() + $system_config->timezone_offset; 
      $timeMsg = strftime($system_config->default_strftime_format, $current_time); 
      $cols=2;

      $options_str = $this->getAdminLinks("Dashboard", "<a class='sitebuilder-filter' href='admin.php?main=true' title='Admin'>Main Admin</a> ");
      writeAdminPageHeader("Oscailt Dashboard", "", "", $options_str);

      $relative_time = false;
      if ($relative_time == true ) {
          $time_diff = ($current_time - $editor_session->editor->editor_lastlogin);
          $txt_editor_lastlogin = getTimeAgoString($time_diff);
      } else {
          $txt_editor_lastlogin = strftime($system_config->default_strftime_format, $editor_session->editor->editor_lastlogin + $system_config->timezone_offset);
     }
  
      # Using two tables for the dasbboard. One for the links on the LHS and the rest for the dashboard.
      echo "<table class='admin' border=1><tr class='admin'><td>";
      $this->writeSideAdminPage();
      echo "</td><td valign='top' >";
      echo "<table class='admin' border=0>";
      ?>
        <tr class=admin valign=top>
	    <th class="admin" colspan=3><?=$timeMsg?> <BR> &nbsp; You are user <u><?=$editor_session->editor->editor_name?></u> </th>
        </tr>
      <?  

      $this->writeInfoItem("You last logged in on ". $txt_editor_lastlogin,"",2);

      $t_up = serverUpTime(true);
      if ($t_up != null) {
           $time_ago_str = getTimeAgoString($t_up);
	   $this->writeInfoItem("Server Uptime ", $time_ago_str,2);
      } else {
          $t_up = serverUpTime(false);
          if ($t_up != null) {
              $time_ago_str = getTimeAgoString($t_up);
	      $this->writeInfoItem("MySQL Database Server Uptime ", $time_ago_str,2);
          }
      }

      $schTaskList = new schTaskList();
      $schTaskList->load();
      if ($schTaskList->count() > 0 ) 
          echo "<tr class=admin valign=top><td class=admin><b>There are ".$schTaskList->count()." scheduled task(s) pending. Click <a href='editorstatus.php?schedule=true'>here</a> to view</b></td><td colspan=2> </td> </tr>";

      $t_sys = checkForOscailtMsg(true);
      if ($t_sys != "")
          echo "<tr class=admin valign=top><td class=admin><b>".$t_sys."</b></td><td colspan=2> </td> </tr>";

      $this->writeConfigBooleanItem("Open Publishing disabled?","disable_open_publish",$system_config->disable_open_publish,"Turn off ability of public to publish. Useful for emergencies caused by spammers.", true ,2);

      $this->writeInfoItem("Comments Publish Delay Duration ",$system_config->publish_comment_delay." Minutes",2);
      
      $this->writeConfigBooleanItem("IP monitoring enabled?","publish_monitor_enabled",$system_config->publish_monitor_enabled,"",false,2);

      $this->writeConfigBooleanItem("Spam words checking for publish stories enabled?","check_spamwords",$system_config->check_spamwords,"Spam words are entered in a separate screen and are checked for in the story content during publish and rejected if they are present.",false, 2);
      $this->writeInfoItem("Default Language Code ", $oscailt_basic_config['default_language_code'] ,2);
      if ( isset($userLanguage) && $userLanguage->getLanguageCodePrefix() != "en")
      {
          $this->writeInfoItem("Your Language Setting is ", $userLanguage->getName(). " (".$userLanguage->getLanguageCodePrefix().")", 2);
      }

      // Some site links. Rather arbitary because not all sites will set up these friendly redirect URLs.
      // Therefore this should be edited accordingly.
      $check_array = array("features","openwire", "newswire", "comments", "events", "gallery", "search"); 
      $lnk_set = "";
      foreach ($check_array as $site_link) {
	  if ($redirectList->containsRedirectString($site_link) == true) {
	      $t_lnk = '<a href="'.$site_link .'">'. $site_link.'</a> &nbsp;';
              $lnk_set .= $t_lnk;
          }
      }

      if ($lnk_set != "") {
          $this->writeConfigHeader("Site Links","Links to common redirects",3);
          ?>
            <tr class=admin>
	        <td class=admin align=center colspan=3><?=$lnk_set?></td>
            </tr>
          <?
      }

      // echo "</table>";
      // echo "<table class='admin'>";
      $this->writeConfigHeader("Publish Activity so far today ","Number of stories, comments and attachements",3);

      $t_most_recent = $this->getPublishTime("stories");
      $t_most_recent = $t_most_recent + $system_config->timezone_offset;
      $time_str = getTimeAgoString(($current_time - $t_most_recent));
      //$time_str = strftime($system_config->default_strftime_format, $t_most_recent);
      $this->writeInfoItem("Last story published ",$time_str,3);

      $t_most_recent = $this->getPublishTime("comments");
      $t_most_recent = $t_most_recent + $system_config->timezone_offset;
      $time_str = getTimeAgoString(($current_time - $t_most_recent));
      //$time_str = strftime($system_config->default_strftime_format, $t_most_recent);
      $this->writeInfoItem("Last comment published ",$time_str,3);

      $this->writeInfoItemPair("","Total", "Hidden");

      $current_hr = date("H",$current_time);
      if ($current_hr > 7 && $current_hr < 23 ) {
          $t_count = $this->getTableCount("stories", $current_hr);
          $t_count_hide = $this->getTableCount("stories", $current_hr,true);
          if ($t_count > 0 ) $t_percent = floor($t_count_hide*100/$t_count);
          else $t_percent = "0";
    
          $t_str = $t_count_hide . " &nbsp; ". $t_percent. "%";
          $t_lnk = "<a href='viewarticles.php' title='recent stories'>&gt;&gt;</a>";
          $this->writeInfoItemPair("Number of stories published in the last ".$current_hr." hours ".$t_lnk,$t_count, $t_str );
    
          $t_count = $this->getTableCount("comments", $current_hr);
          $t_count_hide = $this->getTableCount("comments", $current_hr, true);
          if ($t_count > 0 ) $t_percent = floor($t_count_hide*100/$t_count);
          else $t_percent = "0";
    
          $t_str = $t_count_hide . " &nbsp; ". $t_percent. "%";
          $t_lnk = "<a href='stats.php?comments=true' title='by hour'>(by hour)</a> ";
          $t_lnk .= "<a href='viewarticles.php?comments=true' title='recent comments'>&gt;&gt;</a>";
          $this->writeInfoItemPair("Number of comments published in the last ".$current_hr." hours ".$t_lnk,$t_count, $t_str);
    
          $t_count = $this->getTableCount("attachments", $current_hr);
          $t_count_hide = $this->getTableCount("attachments", $current_hr, true);
          if ($t_count > 0 ) $t_percent = floor($t_count_hide*100/$t_count);
          else $t_percent = "0";
    
          $t_str = $t_count_hide . " &nbsp; ". $t_percent. "%";
          $t_lnk = "<a href='viewarticles.php?attachments=true' title='recent attachments'>&gt;&gt;</a>";
          $this->writeInfoItemPair("Number of attachments published in the last ".$current_hr." hours ".$t_lnk,$t_count, $t_str );
      }

      $t_count = $this->getTableCount("stories", 24);
      $t_count_hide = $this->getTableCount("stories", 24,true);
      if ($t_count > 0 ) $t_percent = floor($t_count_hide*100/$t_count);
      else $t_percent = "0";

      $t_str = $t_count_hide . " &nbsp; ". $t_percent. "%";
      $t_lnk = "<a href='viewarticles.php' title='recent stories'>&gt;&gt;</a>";
      $this->writeInfoItemPair("Number of stories published in the last 24 hours ".$t_lnk,$t_count, $t_str );

      $t_count = $this->getTableCount("comments", 24);
      $t_count_hide = $this->getTableCount("comments", 24, true);
      if ($t_count > 0 ) $t_percent = floor($t_count_hide*100/$t_count);
      else $t_percent = "0";

      $t_str = $t_count_hide . " &nbsp; ". $t_percent. "%";
      $t_lnk = "<a href='viewarticles.php?comments=true' title='recent comments'>&gt;&gt;</a>";
      $this->writeInfoItemPair("Number of comments published in the last 24 hours ".$t_lnk,$t_count, $t_str);

      $t_count = $this->getTableCount("attachments", 24);
      $t_count_hide = $this->getTableCount("attachments", 24, true);
      if ($t_count > 0 ) $t_percent = floor($t_count_hide*100/$t_count);
      else $t_percent = "0";

      $t_str = $t_count_hide . " &nbsp; ". $t_percent. "%";
      $t_lnk = "<a href='viewarticles.php?attachments=true' title='recent attachments'>&gt;&gt;</a>";
      $this->writeInfoItemPair("Number of attachments published in the last 24 hours ".$t_lnk,$t_count, $t_str );

      $this->writeMenuFooter(($cols+1));

      echo "</td></tr></table>";
      writeAdminPageFooter();
   }
   function getPublishTime($item_to_get )
   {
       global $dbconn, $prefix;

       if ($item_to_get == "stories") $table_name = "_stories";
       else if ($item_to_get == "attachments") $table_name = "_attachments";
       else if ($item_to_get == "comments") $table_name = "_comments";
       else $table_name = "_comments";

       $result = sql_query("select UNIX_TIMESTAMP(time_posted) from ".$prefix.$table_name." ORDER BY time_posted DESC LIMIT 0,1", $dbconn,0);
       checkForError($result);
       $t_time = 0;

       if(sql_num_rows( $result ) > 0)
       {
           list($t_time) = sql_fetch_row($result, $dbconn);
       }
       return $t_time;
   }

   function getTableCount($item_to_count, $for_last_hours,$count_hidden=false)
   {
       global $dbconn, $prefix, $system_config;
       // To force a cache avoid use of the unqiue value from time() so round it to nearest 10 secs.
       // $since_time = 10 * floor(time()/10) - ($for_last_hours * 3600);
       $since_time = 10 * floor((time()-$system_config->timezone_offset)/10) - ($for_last_hours * 3600);

       if ($item_to_count == "stories") $table_name = "_stories";
       else if ($item_to_count == "attachments") $table_name = "_attachments";
       else if ($item_to_count == "comments") $table_name = "_comments";
       else $table_name = "_comments";

       if ($count_hidden == true)
           $result = sql_query("select count(1) from ".$prefix.$table_name." WHERE hidden=1 AND unix_timestamp(time_posted) > ".$since_time, $dbconn, 2);
       else
           $result = sql_query("select count(1) from ".$prefix.$table_name." WHERE unix_timestamp(time_posted) > ".$since_time, $dbconn, 2);
       checkForError($result);

       $t_count = 0;

       if(sql_num_rows( $result ) > 0)
       {
           list($t_count) = sql_fetch_row($result, $dbconn);
       }
       return $t_count;
   }

   function writeConfigBooleanItem($displayName,$name,$value,$details, $editable=false, $cols_to_span=1)
   {
       global $editor_session;

       if ($editable == false OR !$editor_session->editor->allowedWriteAccessTo("viewstatus") OR $name !="disable_open_publish") {
           ?>
           <tr class=admin valign=top>
              <td class=admin><?=$displayName?>    &nbsp;<BR><small><?=$details?></small></td>
	      <td class=admin align="right" colspan=<?=$cols_to_span?>> <? if($value==true) echo("Yes");else echo("No"); ?></td>
           </tr>
           <?
       } else {
           $btn_str = "Disable Publishing Now";
	   $btn_color="#EF6668";
           if($value == 'true' OR $value == '1') $btn_str = "Re-enable Publishing Now"; 
           if($value == 'true' OR $value == '1') $btn_color="#58CA58";

           ?>
           <tr class=admin valign=top>
              <td class=admin><?=$displayName?>    &nbsp;<BR><small><?=$details?></small></td>
	      <td class=admin align="right" colspan=<?=$cols_to_span?>> 
	      <form name="oscailt_dashboard" action="<?=$this->my_url?>?dashboard=1" method=POST>
		 <input type=hidden  name="publish_action" value="1" >
		 <input type=submit  name="<?=$name?>" value="<?=$btn_str?>" style='color:<?=$btn_color?>;'>
                 </form>
              </td> 
           </tr>
           <?
       }
   }

   function writeInfoItem($header,$details,$cols_to_span=1)
   {
       ?>
       <tr class=admin>
	  <td class=admin><?=$header?></td><td class=admin align="right" colspan=<?=$cols_to_span?>> <?=$details?></td>
       </tr>
       <?
   }

   function writeInfoItemPair($header,$details1,$details2)
   {
       ?>
       <tr class=admin>
          <td class=admin>&nbsp;<?=$header?></td><td class=admin align="right"> <?=$details1?></td>
          <td class=admin align="right"> <?=$details2?></td>
       </tr>
       <?
   }
   function writeConfigHeader($header,$details, $cols_to_span=2)
   {
       ?>
       <tr class=admin valign=top>
	  <th class=admin colspan=<?=$cols_to_span?>>&nbsp;<?=$header?>&nbsp;<BR> <small><?=$details?></small></th>
       </tr>
       <?
   }

   function writeSideAdminPage()
   {
      global $editor_session, $system_config, $userLanguage;

      echo "<table class='admin'>";
      
      $title = $this->uni_options->getString("monitor_title");
      $msg = $this->uni_options->getString("monitor_intro");
      $filter_opts = $this->uni_options->getItemsByType("monitoraction");
      $this->writeSideMenuItems($filter_opts, $title, $msg);


      $title = $this->uni_options->getString("main_title");
      $msg = $this->uni_options->getString("main_intro");
      $filter_opts = $this->uni_options->getItemsByType("mainaction");
      $this->writeSideMenuItems($filter_opts, $title, $msg);

      $title = $this->uni_options->getString("filter_title");
      $msg = $this->uni_options->getString("filter_intro");
      $filter_opts = $this->uni_options->getItemsByType("filteraction");
      $this->writeSideMenuItems($filter_opts, $title, $msg);

      $title = $this->uni_options->getString("role_title");
      $msg = $this->uni_options->getString("role_intro");
      $filter_opts = $this->uni_options->getItemsByType("roleaction");
      $this->writeSideMenuItems($filter_opts, $title, $msg);

      $title = $this->uni_options->getString("admin_title");
      $msg = $this->uni_options->getString("admin_intro");
      $filter_opts = $this->uni_options->getItemsByType("adminaction");
      $this->writeSideMenuItems($filter_opts, $title, $msg);
      
      $title = $this->uni_options->getString("sites_title");
      $msg = $this->uni_options->getString("sites_intro");
      $filter1_opts = $this->uni_options->getItemsByType("siteaction");
      $filter2_opts =$this->uni_options->getItemsByType("sitechoiceaction");
      $sitechoice_opt = $filter2_opts[0];
      $input_str = $this->getSiteChoiceList(false);
      $sitechoice_opt->setMeta('input_field', $input_str);
      //$filter_opts = array_merge($filter1_opts, $filter2_opts);
      array_push($filter1_opts, $sitechoice_opt);

      $this->writeSideMenuItems($filter1_opts, $title, $msg);

      echo "</table>";
   }

   // Common function for displaying links for main admin screen and dashboard with option
   // to skip a link and swap in another.
   function getAdminLinks($skip_item="", $swap_item="")
   {
      $personal_options = $this->uni_options->getItemsByType("personalaction");
      $options_str = "";
      foreach($personal_options as $opt)
      {
         $name = $opt->getMeta("title");
	 if ($skip_item == $name) {
	     if ($swap_item != "") {
                 $options_str .= " ".$swap_item . " |";
             }
             continue;
         }

         $lnk = $opt->getMeta("link");
         $descr = $opt->getMeta("description");
         $icon = $opt->getMeta("icon");
         if($icon != "")
         {
            if(file_exists($icon)) $icon = "<img class='editoroption' src='$icon' alt='$name'>";
	    else $icon = "";
         }
         $html = " <a class='sitebuilder-filter' href='$lnk' title='$descr'>$name $icon</a>";
         $options_str .= $html . " |";
      }
      return $options_str;

   }
   function writeAdminPage()
   {
      global $editor_session, $system_config, $userLanguage;
      $name = $editor_session->editor->editor_name;
      $welcome = $this->site_options->getString("welcome");
      $title = $this->uni_options->getMeta("title");
      $intro = $welcome." ".$name."! ".$this->uni_options->getMeta("description");

      // This reads the other parts of the data read from the XML file to build up the list of links.
      $options_str = $this->getAdminLinks();
      writeAdminPageHeader($title, "", $intro, $options_str);
      $cols = 4;
      echo "<table class='admin'>";
      
      $title = $this->uni_options->getString("monitor_title");
      $msg = $this->uni_options->getString("monitor_intro");
      writeAdminSectionHeader($title, $msg, $cols);
      $filter_opts = $this->uni_options->getItemsByType("monitoraction");
      $this->writeMenuItems($filter_opts, $cols);


      $title = $this->uni_options->getString("main_title");
      $msg = $this->uni_options->getString("main_intro");
      writeAdminSectionHeader($title, $msg, $cols);
      $opts = $this->uni_options->getItemsByType("mainaction");
      $this->writeMenuItems($opts, $cols);

      $title = $this->uni_options->getString("filter_title");
      $msg = $this->uni_options->getString("filter_intro");
      writeAdminSectionHeader($title, $msg, $cols);
      $filter_opts = $this->uni_options->getItemsByType("filteraction");
      $this->writeMenuItems($filter_opts, $cols);

      $title = $this->uni_options->getString("role_title");
      $msg = $this->uni_options->getString("role_intro");
      writeAdminSectionHeader($title, $msg, $cols);
      $filter_opts = $this->uni_options->getItemsByType("roleaction");
      $this->writeMenuItems($filter_opts, $cols);

      //$title = $this->uni_options->getString("monitor_title");
      //$msg = $this->uni_options->getString("monitor_intro");
      //writeAdminSectionHeader($title, $msg, $cols);
      //$filter_opts = $this->uni_options->getItemsByType("monitoraction");
      //$this->writeMenuItems($filter_opts, $cols);

      $title = $this->uni_options->getString("admin_title");
      $msg = $this->uni_options->getString("admin_intro");
      writeAdminSectionHeader($title, $msg, $cols);
      $filter_opts = $this->uni_options->getItemsByType("adminaction");
      $this->writeMenuItems($filter_opts, $cols);

      $title = $this->uni_options->getString("sites_title");
      $msg = $this->uni_options->getString("sites_intro");
      writeAdminSectionHeader($title, $msg, $cols);
      $filter_opts = $this->uni_options->getItemsByType("siteaction");
      $sitechoices = $this->uni_options->getItemsByType("sitechoiceaction");
      $sitechoice_opt = $sitechoices[0];
      $input_str = $this->getSiteChoiceList();
      $sitechoice_opt->setMeta('input_field', $input_str);
      $filter_opts[] = $sitechoice_opt;
      $this->writeMenuItems($filter_opts, $cols);
      $this->writeMenuFooter($cols);
      writeAdminPageFooter();
   }

   function getSiteChoiceList($add_title=true)
   {
      global $editor_session, $system_config, $userLanguage;

      $site_choices = array();
      $obj_set = new indyObjectSet($system_config->xmltypedef_dir, $system_config->object_index_storage);
      $act_req = new indyObjectActionRequest();
      $obj_set->load(array("*"), array("site"), $act_req);
      //$site_choices = $this->getSiteObjects($site_choices);
      $site_info = $obj_set->getObjectInfoByTypename("site", $userLanguage->getLanguageCodePrefix());
      if(count($site_info) > 1)
      {
         if ($add_title == true) $input_str = "<P align='center'>".$this->uni_options->getString("choose_site");
	 else $input_str = "<P>";
         $input_str .= " <select name='site_id'>\n";
         foreach($site_info as $id => $name)
         {
            $input_str .= "<option value='$id'>$name</option>\n";
         }
         $input_str .= "</select></p>\n";
      }
      else
      {
         if(count($site_info) == 1)
         {
            $ids = array_keys($site_info);
            $site_id = $ids[0];
         }
         else
         {
            $site_id = 1;
         }
         $input_str = "<input type='hidden' name='site_id' value='$site_id'>";
      }

      return $input_str;
   }

   function writeMenuFooter($cols)
   {
      echo '<tr class="admin"><th colspan="'.$cols.'" class="admin"><img class="right" src="graphics/adminicon.gif">&nbsp;</th></tr></table>';
   }

   function getSiteObjects(&$all_sites)
   {
      global $system_config, $userLanguage;
      $obj_man = new indyObjectManager($system_config->xmltypedef_dir, $this->my_url);
      $all_sites = array();
      $all_sites = $obj_man->getAllManagedSiteObjects($all_sites, $userLanguage->getLanguageCodePrefix());
      return $all_sites;
   }

   function writeLoginBox($to_index = false)
   {
      SharedMemoryUpdate("admin");
      $hide_logo = false;
      if (isset($_REQUEST['silent']) && $_REQUEST['silent'] == 'true') {
          $hide_logo = true;
	  setPageTitle("");
      }

      if($to_index == true)
      {
         if (isset($_REQUEST['loginpage']) ) {
            // Allow an alternative page set after login.
            $target = $_REQUEST['loginpage'];
         }
         else
         {
            $target = "index.php";
         }
      }
      else
      {
         $target = $this->my_url;
      }
      global $editor_session;
      ?>
      <form name="oscailt_login" action="<?=$target?>" method=POST>
      <table align=center >
         <tr>
            <td colspan=2 align=center>
      <?
      if ($hide_logo == false) {
            ?> <a href="admin.php?silent=true&loginpage=admin.php"><img src="graphics/oscailtlogo250.gif"></a> <?
      } else {
            ?> &nbsp; <?
      }
      ?>
         </td>
         </tr>
         <tr>
            <td colspan=2 align=center><font class=error><b><?=$editor_session->login_failure_message?></b></font></td>
         </tr>
         <tr>
            <td><b>Editor</b></td>
            <td> <input name=editorname> </td>
         </tr>
         <tr>
            <td><b>Password</b></td>
            <td> <input type=password name=editorpassword> </td>
         </tr>
         <tr>
            <td colspan=2 align=center> <input type=submit value="login"> </td>
         </tr>
      </table>
      <input type='hidden' name='originator' value='site_id'>
      </form>
      <?
   }
}
?>
