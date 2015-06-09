<?

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



//********************************************************

//class to capture user preference information - contains

//anything user preferences (condense comments etc....

//will be expanded before too long to support the preference data object

//********************************************************/

 class PreferenceQuery

 {

   var $valid_saved_preferences = array();

   var $explicit_preferences = array();

   var $valid_user_preferences = array();

   var $editor_session_settings = array();

   var $user_session_settings = array();

   var $non_default_preferences= array();

   var $overrides = array();







   function setValidSavedPreferences()

   {

      $this->valid_saved_preferences = array("condense_comments", "comment_order", "comment_limit", "show_all_comments", "show_comment_titles", "event_display", "fontsizeinc", "userlanguage", "user_frontpage", "mono_lingual_filter", "video_mode");

      $this->user_session_settings = array("show_preferences");

      $this->editor_session_settings = array("edit_mode", "show_editor_preferences");

   }



   function updateSavedPreferences()

   {

      global $system_config, $editor_session;

      $this->setValidSavedPreferences();

      foreach($this->valid_saved_preferences as $pref)

      {

         if($pref == 'comment_limit' or $pref == 'user_frontpage') $numeric_value = true;

         else $numeric_value = false;

         //first check the get -> overrides any cookie based ones...

         if(isset($_GET[$pref]) && $this->containsValidPreferenceData($_GET[$pref]))

         {

            $_REQUEST[$pref] = $this->cleansePreferenceData($_GET[$pref], $numeric_value);

            $this->explicit_preferences[$pref] = $_REQUEST[$pref];

         }

         elseif(isset($_POST[$pref]) && $this->containsValidPreferenceData($_POST[$pref]))

         {

            $_REQUEST[$pref] = $this->cleansePreferenceData($_POST[$pref], $numeric_value);

            $this->explicit_preferences[$pref] = $_REQUEST[$pref];

         }

         if($pref == 'fontsizeinc')

         {

            if(isset($_GET[$pref]) && $this->containsValidPreferenceData($_GET[$pref]))

            {

               $val = $_GET[$pref];

            }

            elseif(isset($_POST[$pref]) && $this->containsValidPreferenceData($_POST[$pref]))

            {

               $val = $_POST[$pref];

            }

            else

            {

               $val = false;

            }

            if($val !== false)

            {

               $cleansed = $this->cleansePreferenceData($val, true);

               if($cleansed > 20) $cleansed == 0;

               if($val{0} == '-')

               {

                  $cleansed = '-'.$cleansed;

               }

               $dom_info = parse_url("http://".$system_config->site_url);
	       if(!isset($dom_info['path'])) $dom_info['path'] = "";
               setCookie($pref,$cleansed,time()+60*60*24*$system_config->cookie_life, $dom_info['path']."/");

            }

         }

         else

         {

            $dom_info = parse_url("http://".$system_config->site_url);
            if(!isset($dom_info['path'])) $dom_info['path'] = "";
            if(isset($_REQUEST["save_prefs"]) && $_REQUEST["save_prefs"]=="true" && isset($_REQUEST[$pref]) && $this->containsValidPreferenceData($_REQUEST[$pref]))

            {

               if(($pref != 'comment_limit' && $_REQUEST[$pref] === "0") or $_REQUEST[$pref]=="" or $_REQUEST[$pref] == 'delete')

               {

                  setCookie ($pref, "", time() - 3600, $dom_info['path']."/");

               }

               else

               {

                  setCookie($pref,$_REQUEST[$pref],time()+60*60*24*$system_config->cookie_life,$dom_info ['path']."/");

               }

            }

         }

      }

      if($editor_session->isSessionOpen())

      {

         $update = false;

         foreach($this->editor_session_settings as $e_session)

         {

            if(isset($_REQUEST[$e_session]))

            {

               $editor_session->editor->editor_state[$e_session] = $_REQUEST[$e_session];

               //echo "<P>set ".$e_session." to ".$_REQUEST[$e_session];

               $update = true;

            }

         }

         if($update)

         {

            $_SESSION['current_editor'] = $editor_session->editor;

         }

      }

      foreach($this->user_session_settings as $u_session)

      {

         if(isset($_REQUEST[$u_session]))

         {

            $editor_session->setUserSessionValue($u_session, $_REQUEST[$u_session]);

         }

      }

   }



   function readPrefsFromForm(&$user_options, &$display_options)

   {

      foreach(array_keys($user_options) as $user_key)

      {

         if(isset($_REQUEST[$user_key]) && $this->containsValidPreferenceData($_REQUEST[$user_key]))

         {

            if(is_array($user_options[$user_key]))

            {

               //implode around comma and populate array

               $_REQUEST[$user_key] = implode(",", $this->cleansePreferenceData($_REQUEST[$user_key]));

            }

            $val = $this->cleansePreferenceData($_REQUEST[$user_key]);

            if($val == 'on') $val = 'true';

            if(($display_options[$user_key] != $val) || in_array($user_key, array_keys($this->explicit_preferences)))

            {

               if(isset($this->explicit_preferences[$user_key]) && $this->explicit_preferences[$user_key] == $val)

               {

                  $this->non_default_preferences[$user_key] = $val;

               }

               $display_options[$user_key] = $val;

            }

         }

      }



   }



   //preference information from the user (including filter settings) can only consist of word characters

   //makes it safe from sql injection attacks.

   function cleansePreferenceData($data, $is_numeric=false)

   {

      if($is_numeric) $pattern = '/\D/';

      else $pattern = '/\W/';

      $data = preg_replace($pattern, "", $data);

      return $data;

   }



   function containsValidPreferenceData($data)

   {

      if(preg_match('/\w/', $data)) return true;

      return false;

   }



   function setQueryOption($key, $val)

   {

      $this->overrides[$key] = $val;

   }



   function getQueryOption($key)

   {

      if(isset($this->overrides[$key]))

      {

         return $this->overrides[$key];

      }

      return false;

   }



   function unsetQueryOption($k)

   {

      unset($this->overrides[$k]);

   }





   function unsetQueryOptions()

   {

      $this->overrides = array();

   }



   function getQueryString()

   {

      $qstr = "";

      foreach(array_keys($this->non_default_preferences) as $pref)

      {

         if(!in_array($pref, array_keys($this->overrides)))

         {

            $qstr .= '&'.$pref.'='.$this->non_default_preferences[$pref];

         }

      }

      foreach(array_keys($this->overrides) as $override)

      {

         $qstr .= '&'.$override.'='.$this->overrides[$override];

      }

      return $qstr;

   }

}



?>
