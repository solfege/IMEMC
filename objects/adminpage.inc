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

   function processRequest()
   {
      global $editor_session, $system_config;

      SharedMemoryUpdate("admin");
      if(isset($_REQUEST['silent']) && $_REQUEST['silent'] == true) 
      {
	 $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
         ?> <a href="<?=$http_mode?>://<?=$system_config->site_url?>/admin.php">Admin</a>
	 <?
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
      if(!isset($_REQUEST['task']) && !isset($_REQUEST['site_id']))
      {
         // This displays the normal admin page of various options
         $this->writeAdminPage();
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
      $css_1 = 'common.css';
      $css_2 = 'style.css';
      $css_3 = 'contentstyle.css';
      $common_lnk = "<a href='".$lnk_base."&amp;editstyle=$css_1'>";
      $style_lnk = "<a href='".$lnk_base."&amp;editstyle=$css_2'>";
      $contentstyle_lnk = "<a href='".$lnk_base."&amp;editstyle=$css_3'>";
      $edit = $this->site_options->getString("edit");
      echo "<P>$common_lnk"."$edit $css_1</a> | $style_lnk"."$edit $css_2</a> | $contentstyle_lnk"."$edit $css_3</a></P>";
      
      echo "<P><BR> <b>Note</b>: Style Sheet files directory is: ".$st_store."</P>";

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
              echo "<tr class='admin'><td class='admin'>Site: <b>".$new_obj->name()."</b> (".$each_obj->obj_id .")</td><td> is using ".$t_msg. " Style Sheet (CSS) Files.</td>";
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


   function writeStyleEditSection($target)
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
      echo "<table class='admin'>";
      echo "<tr class='admin'><th class='admin'>";
      $edit = $this->site_options->getString("edit");
      echo $edit." ".$target."</th></tr>";
      echo "<tr class='admin'><td class='admin'>";
      echo "<textarea rows='50' cols='400' name='style-contents'>".htmlspecialchars($contents, ENT_QUOTES)."</textarea>";
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

      echo "<tr class='admin'><td class='admin'>Filesize: ".$number_bytes." Kb. Last update: ".$time_msg."</td></tr>";

      echo "<tr class='admin'><th class='admin'>";
      echo "<div class='editorconfirmbuttons'><input type=submit name='cancelstylee' value='<< $cancel_text'>";
      echo "<input type=submit name='confirm' value='$confirm_text >>'>";
      echo "</div></th></tr></table></div>";
   }

   function writeAdminPage()
   {
      global $editor_session, $system_config, $userLanguage;
      $name = $editor_session->editor->editor_name;
      $welcome = $this->site_options->getString("welcome");
      $title = $this->uni_options->getMeta("title");
      $intro = $welcome." ".$name."! ".$this->uni_options->getMeta("description");
      $personal_options = $this->uni_options->getItemsByType("personalaction");
      $options_str = "";
      foreach($personal_options as $opt)
      {
         $name = $opt->getMeta("title");
         $lnk = $opt->getMeta("link");
         $descr = $opt->getMeta("description");
         $icon = $opt->getMeta("icon");
         if($icon != "")
         {
            $icon = "<img class='editoroption' src='$icon' alt='$name'>";
         }
         $html = " <a class='sitebuilder-filter' href='$lnk' title='$descr'>$name $icon</a>";
         $options_str .= $html . " |";
      }
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
      $site_choices = array();
      $obj_set = new indyObjectSet($system_config->xmltypedef_dir, $system_config->object_index_storage);
      $act_req = new indyObjectActionRequest();
      $obj_set->load(array("*"), array("site"), $act_req);
      //$site_choices = $this->getSiteObjects($site_choices);
      $site_info = $obj_set->getObjectInfoByTypename("site", $userLanguage->getLanguageCodePrefix());
      if(count($site_info) > 1)
      {
         $input_str = "<P align='center'>".$this->uni_options->getString("choose_site");
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
      $sitechoice_opt->setMeta('input_field', $input_str);
      $filter_opts[] = $sitechoice_opt;
      $this->writeMenuItems($filter_opts, $cols);
      $this->writeMenuFooter($cols);
      writeAdminPageFooter();
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
      <table align=center>
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
            <td>
               <input name=editorname>
            </td>
         </tr>
         <tr>
            <td><b>Password</b></td>
            <td>
               <input type=password name=editorpassword>
            </td>
         </tr>
         <tr>
            <td colspan=2 align=center>
               <input type=submit value="login">
            </td>
         </tr>
      </table>
      <input type='hidden' name='originator' value='site_id'>
      </form>
      <?
   }
}
?>
