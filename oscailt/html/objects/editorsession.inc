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

require_once("objects/publishstate.inc");

class EditorSession
{
   var $login_failure_message = "";
   var $editor;
   var $clipboard;
   var $token = null;
   var $newstoken = null;
   var $user_settings = array();

   function isSessionOpen()
   {
      if($this->editor==null) return false;
      else return true;
   }

   function canEditContent()
   {
      if($this->isSessionOpen() && isset($this->editor->editor_state['edit_mode']) and ($this->editor->editor_state['edit_mode'] == 'content' or $this->editor->editor_state['edit_mode'] == 'all')) return true;
      return false;
   }

   function canEditLayout()
   {
      if($this->isSessionOpen() && isset($this->editor->editor_state['edit_mode']) and ($this->editor->editor_state['edit_mode'] == 'sitebuilder' or $this->editor->editor_state['edit_mode'] == 'all')) return true;
      return false;
   }

   function canIgnoreLimits()
   {
      if($this->isSessionOpen() && $this->editor->allowedWriteAccessTo("ignoreattachmentlimits"))
      {
         return true;
      }
      return false;
   }
   function redirectToHome()
   {
      global $system_config, $pseudo_directory_offset;

      if(isset($_REQUEST['silent']) && $_REQUEST['silent'] == true) {
          //$home_url = "https://".$system_config->site_url. "/admin.php?silent=true";
          $home_url = "admin.php?silent=true";
          if(isset($_REQUEST['loginpage']) && $_REQUEST['loginpage'] == true) {
              $home_url .= "&loginpage=admin.php?silent=".$_REQUEST['loginpage'];
          }
      } else {
          $home_url = "http://".$system_config->site_url;
      }
      disconnectFromDatabase();
      RemoveSemaphoreId();

      Header("Location: ".$home_url);
      exit;
   }

   function start()
   {
      global $system_config, $pseudo_directory_offset;
      ini_set("session.gc_maxlifetime", $system_config->session_life);//sets auto-logout time

      if($system_config->site_url != "")
      {
         $dom_info = parse_url("http://".$system_config->site_url);
         $path = isset($dom_info['path']) ? $dom_info['path']."/" : "/";
         $fullpath = $_SERVER['DOCUMENT_ROOT'].$path;
         if(!file_exists($fullpath) or !is_dir($fullpath))
         {
            logMessage("Your Site URL is incorrectly set.  http://$system_config->site_url ($fullpath) does not exist or is not a directory","sitelog.txt");
            $path = "";
         }
      }
      else
      {
         $path = "";
      }
      session_set_cookie_params(0, $path);
      session_start();
      header("Cache-control: private"); //this fixes bug in IE6
      if(isset($_REQUEST['logout']))
      {
	 // Track the status of the editor
         if(isset($_SESSION['current_editor']))
         {
             // Gets the session from the cookie. Hmm, how?
             $this->editor=&$_SESSION['current_editor'];
	     $editorStatusList = new PublishState();
             $editorStatusList->load();
             $editorStatusList->add($this->editor->editor_name, "Logged out", time());
             $editorStatusList->save();
             $this->editor=null;
         }

      	 // logMessage("Logout for: editor unknown " ,"sitelog.txt");
         session_destroy();
         $this->redirectToHome();
      }
      else if(isset($_REQUEST['editorname']) && isset($_REQUEST['editorpassword']) && isset($_REQUEST['originator']) && $_REQUEST['originator'] == "site_id")
      {
         // credentials supplied so attempt to validate them
         $editor = new Editor();
         $editor->editor_name=addslashes($_REQUEST['editorname']);
         if($editor->validatePassword(addslashes($_REQUEST['editorpassword'])))
         {
            // If session already exists then this is just a refresh done with login details.
            if(isset($_SESSION['current_editor'])) {
               // session in progress so load editor from session
               $this->editor=&$_SESSION['current_editor'];
               $this->clipboard=&$_SESSION['current_clipboard'];
               return;
            }

            $editor->load();
            logMessage("Login success for editor: ".$_REQUEST['editorname'].". Last login was ".date("D, d M Y H:i:s",($editor->editor_lastlogin+$system_config->timezone_offset)),"sitelog.txt");
            $editor->updateLoginTime();
            $editor->editor_state['edit_mode'] = 'content';
            $_SESSION['current_editor'] = $editor;
            $_SESSION['current_clipboard'] = new Clipboard();
            $this->editor=&$_SESSION['current_editor'];
            $this->clipboard=&$_SESSION['current_clipboard'];

	    // Track the status of the editor
	    $editorStatusList = new PublishState();
            $editorStatusList->load();
            $editorStatusList->add($editor->editor_name, "Login", time());
            $editorStatusList->save();
	    SharedMemoryUpdate("EditorLogin");
         }
         else
         {
            // If the login failed then maybe the user does not exist. Check and log it.
            $extra_msg = ")";
            if($editor->validateUsername(addslashes($_REQUEST['editorname'])) == false )
            {
               $extra_msg = "). This username does not exist.";
            }
            // logMessage("Login failure for editor: (".$_REQUEST['editorname'] .$extra_msg,"sitelog.txt");
            $msg = "Login failure for editor: (". $_REQUEST['editorname'] . $extra_msg;
            $msg .= " Password length was " . strlen($_REQUEST['editorpassword']);
            logSpammerMessage($msg);
	    SharedMemoryUpdate("LoginFail");

            session_destroy();
            // credentials supplied invalid so redirect to login page with error
            $this->login_failure_message="ACCESS DENIED";
         }
      }
      else if(isset($_SESSION['current_editor']))
      {
         // session in progress so load editor from session
         $this->editor=&$_SESSION['current_editor'];
         $this->clipboard=&$_SESSION['current_clipboard'];
      }
      if(isset($_SESSION['token']))
      {
         $this->token = $_SESSION['token'];
      }
      if(isset($_SESSION['newstoken']))
      {
         $this->newstoken = $_SESSION['newstoken'];
      }
      if(isset($_SESSION['user_settings']))
      {
         $this->user_settings =& $_SESSION['user_settings'];
         //global $user_prefs;
         //foreach($user_prefs->user_preference_settings as $u_p)
         //{
         //   if(isset($_SESSION['user_settings'][$u_p]))
         //   {

         //   }
         //}
      }
   }

   function destroy()
   {
      session_destroy();
      $this->editor=null;
      $this->clipboard=null;
   }

   function setToken( $new_value)
   {
      if ($new_value != null)
      {
         $this->token = $new_value;
         $_SESSION['token'] = $new_value;
      }
      else
      {
         $this->token = null;
         $_SESSION['token'] = null;
      }
   }

   function setUserSessionValue($k, $v)
   {
      $_SESSION['user_settings'][$k] = $v;
      $this->user_settings =& $_SESSION['user_settings'];
   }

   function writeNoSessionError()
   {
      writeFailureMessage("Security Alert", "<b class='error'>You must be logged in to access this page</b>");
   }
   function writeNoReadPermissionError()
   {
      writeFailureMessage("Security Alert", "<b class='error'>Sorry ".$this->editor->editor_name.", but you not have permission to access this page</b>");
   }
   function writeNoWritePermissionError()
   {
      writeFailureMessage("Security Alert", "<b class='error'>Sorry ".$this->editor->editor_name.", but you not have permission to carry out this action.</b>");
   }
}
?>
