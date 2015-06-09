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

require_once('displayobject.inc');


class preferencesDisplay extends displayObject
{
   var $query;

   function preferencesDisplay ($obj_id)
   {
      $this->displayObject($obj_id);
   }

   function processRequest()
   {
      parent::processRequest();
      if(isset($_REQUEST['save']))
      {
         //print message to say 'preferences updated'
         $msg = $this->getDisplayOption('preferences_updated');
         $this->writeUserMessage($msg);
      }
      $this->display();

   }

   function loadQuery()
   {
      global $oscailt_story_query;
      if(!isset($oscailt_story_query))
      {
         $oscailt_story_query = new StoryQuery();
         $oscailt_story_query->loadFormValues();
      }
      $this->query = $oscailt_story_query->copy();
   }

   function retrieveHeadlines()
   {
      global $editor_session, $page_style, $graphics_store;
      $this->setDisplayOptions();
      $this->loadQuery();
      $hsize = $this->getDisplayOption('hsize');
      $title = $this->getDisplayOption('title');
      $cls = $this->getDisplayOption('class');
      // echo("getOptDis class = ".$cls ."<BR>");
      if($cls == "") $cls = $page_style;

      $selflink = "<a class='$cls' href='".$this->getSimpleLink()."'>";
      $link = $this->getPreferenceLink();
      if(stristr($link,"?")!=false) $link=$link."&";
      else $link=$link."?";
      $show_prefs = true;
      if($this->getDisplayOption("display") == "Expandable Menu")
      {
         echo "<a name='user_preferences'></a>";
         if(isset($editor_session->user_settings['show_preferences']) && ($editor_session->user_settings['show_preferences'] == 'true'))
         {
            $lnk = $link."show_preferences=false#user_preferences";
            $lnk_bit = " <a class='$cls' href='$lnk'>";
            $lnk_end = " <img class='icon' src='$graphics_store"."downgrade.gif' alt='hide preferences'></a>";
            if($hsize > 0 and $hsize < 6)
            {
               echo "<h".$hsize." class='$cls'>".$lnk_bit.$title.$lnk_end."</h".$hsize.">";
            }
            else
            {
                echo $lnk_bit.$title.$lnk_end;
            }
         }
         else
         {
            $lnk = $link."show_preferences=true#user_preferences";
            $lnk_bit = " <a class='$cls' href='$lnk'>";
            $lnk_end = " <img class='icon' src='$graphics_store"."upgrade.gif' alt='show preferences'>";
            if($hsize > 0 and $hsize < 6)
            {
               echo "<h".$hsize." class='$cls'>".$lnk_bit.$title.$lnk_end."</h".$hsize.">";
            }
            else
            {
                echo $lnk_bit.$title.$lnk_end;
            }
            $show_prefs = false;
         }
      }
      else
      {
         if($editor_session->isSessionOpen())
         {
            $title = $editor_session->editor->editor_name. " ".$title;
         }
         if($hsize > 0 and $hsize < 6)
         {
            echo "<h".$hsize." class='$cls'>".$selflink.$title."</a></h".$hsize.">";
         }
         else
         {
            echo $selflink.$title."</a>";
         }
      }
      if($show_prefs)
      {
         echo "<div style='margin: 0; padding: 0; font-size: 0.85em;'>";
         $this->showUserOptions($cls);
         echo "</div>";
      }
      if($editor_session->isSessionOpen())
      {
         $show_ed_prefs = true;
         $title = $editor_session->editor->editor_name." options";
         if($this->getDisplayOption("display") == "Expandable Menu")
         {
            echo "<a name='editor_preferences'></a>";
            if(isset($editor_session->editor->editor_state['show_editor_preferences']) and $editor_session->editor->editor_state['show_editor_preferences'] == 'true')
            {
               $lnk = $link."show_editor_preferences=false#editor_preferences";
               $lnk_bit = " <a class='$cls' href='$lnk'>";
               $lnk_end = " <img class='icon' src='$graphics_store"."downgrade.gif' alt='hide editor preferences'></a>";
               if($hsize > 0 and $hsize < 6)
               {
                  echo "<h".$hsize." class='$cls'>".$lnk_bit.$title.$lnk_end."</h".$hsize.">";
               }
               else
               {
                  echo $lnk_bit.$title.$lnk_end;
               }
            }
            else
            {
               $lnk = $link."&show_editor_preferences=true#editor_preferences";
               $lnk_bit = " <a class='$cls' href='$lnk'>";
               $lnk_end = " <img class='icon' src='$graphics_store"."upgrade.gif' alt='show editor preferences'>";
               if($hsize > 0 and $hsize < 6)
               {
                  echo "<h".$hsize." class='$cls'>".$lnk_bit.$title.$lnk_end."</h".$hsize.">";
               }
               else
               {
                   echo $lnk_bit.$title.$lnk_end;
               }
               $show_ed_prefs = false;
            }
         }
      }
      else
      {
         $show_ed_prefs = false;
      }
      if($show_ed_prefs)
      {
         $this->showEditorOptions($cls);
      }
   }

   function showUserOptions($cls)
   {
      global $oscailt_page_setting, $graphics_store, $pageTitle;
      echo "<ul class='$cls'>";
      if($this->getDisplayOption("showlanguage") == 'true')
      {
         echo "<li class='$cls'>";
         $this->showLanguageOptions($cls);
         echo "</li>";
      }
      if($this->getDisplayOption("showfontsize") == 'true')
      {
         echo "<li class='$cls'>";
         $this->showFontSizeOptions($cls);
         echo "</li>";
      }
      if($this->getDisplayOption("showoscailthome") == 'true' && isset($oscailt_page_setting['obj_id']))
      {
         echo "<li class='$cls'>";
         $page_id = $oscailt_page_setting['obj_id'];
         $this->showHomePageOptions($page_id, $cls);
         echo "</li>";
      }
      if($this->getDisplayOption("showallprefs") == 'true' && $this->getDisplayOption("display") == "Expandable Menu")
      {
         echo "<li class='$cls'>";
         echo "<a class='$cls' href='".$this->getSimpleLink()."'>".$this->getDisplayOption("allprefstext")."</a>";
         echo "</li>";
      }

      if($this->getDisplayOption("showaddbookmark") == 'true')
      {
         ?>
         <script type="text/javascript">
             function bookmarksite(title, url) {
             if (document.all) window.external.AddFavorite(url, title);
             else if (window.sidebar) window.sidebar.addPanel(title, url, "")
         }
         </script>
         <?
         $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
         $t_url = $_SERVER['REQUEST_URI'];
         if (substr($t_url,0,1) == "/") $t_url = substr($t_url,1);
         $t_url = $http_mode . "://". $t_url;
         echo "<li class='$cls'>";
	 ?>
		 <div id="sidebar_bookmarkbuttons"><a href="javascript:bookmarksite('<?=$pageTitle?>','<?=$t_url?>')"><img src="<?=$graphics_store?>/addtofavs.gif" border="0"></a></div>
         <?
         echo "</li>";
      }
      echo "</ul>";
   }

   function showLanguageOptions($cls)
   {
      global $languageList, $userLanguage;
      echo $this->getDisplayOption("languagetext");
      $cls_str = "class='$cls'";
      $link = $this->getPreferenceLink("fontsizeinc");
      if(stristr($link,"?")!=false) $link=$link."&";
      else $link=$link."?";
      $langs = $languageList->getfLanguages();
      if(count($langs) < 1) echo "error";
      echo " - ";
      $first = false;
      foreach($langs as $l)
      {
         $my_link = $link;
         if(!$first)   $first = true;
         else echo " | ";
         if(compareToUserLanguage($l->getLanguageCodePrefix()))
         {
            echo "<u $cls_str>".$l->getLanguageCodePrefix()."</u>";
         }
         else
         {
            $my_link.="userlanguage=".$l->getLanguageCodePrefix()."&amp;save_prefs=true";
            echo "<a $cls_str href='$my_link'>".$l->getLanguageCodePrefix()."</a>";
         }
      }
   }

   function showHomePageOptions($page_id, $cls)
   {
      global $graphics_store;
      $lnk = $this->getPreferenceLink("user_frontpage");
      $lnk .= "&amp;user_frontpage=". $page_id;
      echo "<a class='$cls' href='$lnk"."&amp;save_prefs=true' rel='nofollow'>";
      echo "<img src='$graphics_store"."address.gif' alt='".htmlspecialchars($this->getDisplayOption("oscailthometext"), ENT_QUOTES)."'> ";
      echo $this->getDisplayOption("oscailthometext");
      echo "</a>";
   }

   function showFontSizeOptions($cls)
   {
      $cls_str = "class='$cls'";
      $link = $this->getPreferenceLink("fontsizeinc");
      if(stristr($link,"?")!=false) $link=$link."&";
      else $link=$link."?";
      echo $this->getDisplayOption("fontsizetext")." ";
      $a_start = "<a $cls_str href='";
      if(isset( $_REQUEST['fontsizeinc'] ) && $_REQUEST['fontsizeinc']>2)
      {
         echo "<A $cls_str href='$link"."fontsizeinc=0'>normal</A>&nbsp;|&nbsp;";
         echo "<A $cls_str href='$link"."fontsizeinc=".($_REQUEST['fontsizeinc']-2)."'>&lt;&lt;</A>&nbsp;|&nbsp;";
      }
      else if(isset( $_REQUEST['fontsizeinc'] ) && $_REQUEST['fontsizeinc']>0)
      {
         echo "<A $cls_str href='$link"."fontsizeinc=".($_REQUEST['fontsizeinc']-2)."'>normal</A>&nbsp;|&nbsp;";
      }
      if(isset($_REQUEST['fontsizeinc'])) $bigger = $_REQUEST['fontsizeinc'] + 2;
      else $bigger = 2;
      // Change to add re=nofollow for the bots. Note the double and single quotes interchanged
      // echo "<A $cls_str href='$link"."fontsizeinc=".$bigger."'>&gt;&gt;</A>";
      echo '<A '.$cls_str.' href="'.$link.'fontsizeinc='.$bigger.'" rel="nofollow">&gt;&gt;</A>';
   }

   function showEditorOptions($cls)
   {
      global $editor_session;
      echo "<small>";
      echo "<ul class='$cls'>";
      if($this->getDisplayOption("showeditmode") == 'true')
      {
         echo "<li class='$cls'>";
         $this->showEditModeOptions($cls);
         echo "</li>";
      }

      if($this->getDisplayOption("showuserstatus") == 'true')
      {
         echo "<li class='$cls'>";
         $this->showUsersStatusOptions($cls);
         echo "</li>";
      }

      if($this->getDisplayOption("showprofile") == 'true')
      {
         echo "<li class='$cls'>";
         $this->showProfileOptions($cls);
         echo "</li>";
         echo "<li class='$cls'>";
         $this->showAdminOption($cls);
         echo "</li>";
      }
      if($this->getDisplayOption("showlogout") == 'true')
      {
         echo "<li class='$cls'>";
         $this->showLogoutOptions($cls);
         echo "</li>";
      }
      echo "</ul>";
      echo "</small>";
   }

   function showEditModeOptions($cls)
   {
      global $pseudo_directory_offset,$editor_session;
      $poss = array("normal" => $this->getDisplayOption("normaltext"),
                     "content" => $this->getDisplayOption("contenttext"),
                     "sitebuilder" => $this->getDisplayOption("sitebuildertext"),
                     "all" => $this->getDisplayOption("alltext"));
      $current_mode = (isset($editor_session->editor->editor_state['edit_mode'])) ? $editor_session->editor->editor_state['edit_mode'] : "normal";
      $lnk = $this->getPreferenceLink("edit_mode");

      echo "<form name='sidebar_prefs' style='display:inline; margin: 0px;' action='$lnk' method='POST'>";
      echo "<select class='$cls' name='edit_mode'>";
      foreach($poss as $k => $v)
      {
         if($k == $current_mode) echo "<option value='$k' selected>$v</option>";
         else echo "<option value='$k'>$v</option>";
      }
      echo "</select>";
      echo "<input type='submit' class='$cls' name='set_edit_mode' value='".$this->getDisplayOption("editmodetext")."'>";
      echo "</form>";
   }

   function showLogoutOptions($cls)
   {
      global $pseudo_directory_offset;
      $a = "<a class='$cls' href='";
      $link = $this->getPreferenceLink();
      $link = addLinkParameters($link, array("logout=true"));
      $a .= $link."'>";
      echo $a.$this->getDisplayOption("logouttext")."</a>";
   }

   function showProfileOptions($cls)
   {
      global $pseudo_directory_offset;
      $lnk = "<a class='$cls' href='";
      $lnk .= $pseudo_directory_offset."editeditors.php?editself=true'>";
      echo $lnk.$this->getDisplayOption("profiletext")."</a>";
   }

   function showUsersStatusOptions($cls)
   {
      global $pseudo_directory_offset;
      $lnk = "<a class='$cls' href='";
      $lnk .= $pseudo_directory_offset."editorstatus.php'>";
      echo $lnk.$this->getDisplayOption("userstatustext")."</a>";
   }

   function showAdminOption($cls)
   {
      global $pseudo_directory_offset;
      $admin_lnk = "<a class='$cls' href='";
      $admin_lnk .= $pseudo_directory_offset."admin.php'>";
      echo $admin_lnk.$this->getDisplayOption("admintext")."</a>";
   }

   function getPreferenceLink($key=false)
   {
      global $pseudo_directory_offset, $user_prefs, $OSCAILT_SCRIPT;
      if($key != false)
      {
         if(isset($user_prefs->non_default_preferences[$key]))
         {
            $orig = $user_prefs->non_default_preferences[$key];
            unset($user_prefs->non_default_preferences[$key]);
         }
         else $orig = false;
      }

      global $oscailt_page_setting;
      if(isset($_REQUEST['site_id']) && isset($_REQUEST['action']))
      {
         require_once("objects/indyobjects/indyobject.inc");
         require_once("objects/indyobjects/indyobjectactionrequest.inc");
         $ac = new indyObjectActionRequest();
         $ac->readRequest();
         $str = $ac->getLink();
      }
      elseif(isset($oscailt_page_setting['obj_id']))
      {
         $str = $this->getFullLink($oscailt_page_setting['obj_id']);
      }
      else $str = "";
      if($key !== false)
      {
         if($orig !== false) $user_prefs->non_default_preferences[$key] = $orig;
      }
      return $str;
   }

   function display()
   {
      $this->writeDisplayHeader();
      $this->writeOptionTableHeader();
      $this->writeUniversalOptions();
      writeMenuFooter(3, false);
      writeMenuSpacer();
      writeMenuHeader();
      $this->writeArticleOptions();
      writeMenuFooter(3, false);
      writeMenuSpacer();
      writeMenuHeader();
      $this->writeEventsOptions();
      writeMenuFooter(3, false);
      writeMenuSpacer();
      writeMenuHeader();
      $this->writeCommentsOptions();
      writeMenuFooter(3, false);
      writeMenuSpacer();
      writeMenuHeader();
      $this->writeOptionTableFooter();
      $this->writeDisplayFooter();
   }

   function writeOptionTableHeader()
   {
      $title = $this->getDisplayOption("title");
      $option = $this->getDisplayOption("optiontext");
      $val = $this->getDisplayOption("valuetext");
      $targ = $this->getSimpleLink();
      ?>
      <form name='userprefs_settings' action='<?=$targ?>' method='POST'>
      <input type="hidden" name="save_prefs" value="true">
      <table class="admin">
      <?
   }

   function writeOptionTableFooter()
   {
      $option = $this->getDisplayOption("saveoptionstext");
      ?>
         <tr class="admin">
            <th class="admin" colspan="3" style='text-align: center;'><input type='submit' name="save" value="<?=$option?>"></big></th>
         </tr>
         </table>
         </form>
      <?
   }


   function writeUniversalOptions()
   {
      global $languageList;
      $option = $this->getDisplayOption("optiontext");
      $val = $this->getDisplayOption("valuetext");
      $d = $this->getDisplayOption("descriptiontext");
      $art = $this->getDisplayOption("universaloptionstext");
      $lang = $this->getDisplayOption("languageoptiontitle");
      $langdesc = $this->getDisplayOption("languageoptiondescription");
      $langfilter = $this->getDisplayOption("languagefiltertitle");
      $langfilteroff = $this->getDisplayOption("languagefilteroff");
      $langfilteron = $this->getDisplayOption("languagefilteron");
      $langfilterdesc = $this->getDisplayOption("languagefilterdescription");
      $fsize = $this->getDisplayOption("fontsizeoptiontitle");
      $fsizedesc = $this->getDisplayOption("fontsizeoptiondescription");
      $langs = $languageList->getfLanguages();
      if(count($langs) < 1) $lang_html = "error no languages available";
      elseif(count($langs) == 1)
      {
         $lang_html = $langs[0]->language_name;
      }
      else
      {
         $lang_html = "<select name='userlanguage'>";
         foreach($langs as $l)
         {
            if(compareToUserLanguage($l->getLanguageCodePrefix()))
            {
               $lang_html .= "<option selected value='".$l->getLanguageCodePrefix()."'>".$l->language_name."</option>";
            }
            else
            {
               $lang_html .= "<option value='".$l->getLanguageCodePrefix()."'>".$l->language_name."</option>";
            }
         }
         $lang_html .= "</select>";
      }
      $opts = array(2, 4, 6, 8, 10);
      $fsize_html = "<select name='fontsizeinc'>";
      if(!isset( $_REQUEST['fontsizeinc'] ) or  $_REQUEST['fontsizeinc'] <= 0)
      {
         $fsize_html .= "<option value='0' selected>normal</option>";
         $fs = 0;
      }
      else
      {
         $fsize_html .= "<option value='0'>normal</option>";
         $fs = $_REQUEST['fontsizeinc'];
      }
      foreach($opts as $o)
      {
         if($o == $fs)
            $fsize_html .= "<option value='$o' selected> +$o </option>";
         else
            $fsize_html .= "<option value='$o'> +$o </option>";
      }
      $fsize_html .= "</select>";

      if(isset($_REQUEST['mono_lingual_filter']) && $_REQUEST['mono_lingual_filter'] == 'on')
      {
        $langfilter_html = "<input name='mono_lingual_filter' value='delete' type='radio'> $langfilteroff<br>";
        $langfilter_html .= "<input name='mono_lingual_filter' value='on' type='radio' checked> $langfilteron";
      }
      else
      {
        $langfilter_html = "<input name='mono_lingual_filter' value='delete' type='radio' checked> $langfilteroff<br>";
        $langfilter_html .= "<input name='mono_lingual_filter' value='on' type='radio'> $langfilteron";
      }
      ?>
      <tr class="admin">
         <th class="admin" colspan="3"><big><?=$art?></big></th>
      </tr>

      <tr class="admin">
         <th class="admin"><?=$option?></th>
         <th class="admin"><?=$d?></th>
         <th class="admin"><?=$val?></th>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$lang?></strong></td>
         <td class="admin"><?=$langdesc?></td>
         <td class="admin"><?=$lang_html?></td>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$langfilter?></strong></td>
         <td class="admin"><?=$langfilterdesc?></td>
         <td class="admin"><?=$langfilter_html?></td>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$fsize?></strong></td>
         <td class="admin"><?=$fsizedesc?></td>
         <td class="admin"><?=$fsize_html?></td>
      </tr>

      <?
   }

   function writeArticleOptions()
   {
      $option = $this->getDisplayOption("optiontext");
      $val = $this->getDisplayOption("valuetext");
      $art = $this->getDisplayOption("articleoptionstext");
      $d = $this->getDisplayOption("descriptiontext");

      $cc = $this->getDisplayOption("condense_commentstitle");
      $cc_desc = $this->getDisplayOption("condense_commentsdescription");
      $c_order = $this->getDisplayOption("comment_ordertitle");
      $c_order_desc = $this->getDisplayOption("comment_orderdescription");
      $c_limit = $this->getDisplayOption("comment_limittitle");
      $c_limit_desc = $this->getDisplayOption("comment_limitdescription");

      $video_mode = $this->getDisplayOption("video_mode_title");
      $video_mode_desc = $this->getDisplayOption("video_mode_description");

      $cc_html = "<select name='condense_comments'>";
      if(!isset($_REQUEST['condense_comments']) or $_REQUEST['condense_comments'] === 0)
      {
         $cc_html .= "<option value='delete' selected>default</option>";
         $c_set = 0;
      }
      else
      {
         $cc_html .= "<option value='delete'>default</option>";
         $c_set = $_REQUEST['condense_comments'];
      }

      $cc_html .= $this->getFormOptionText($c_set);
      $cc_html .= "</select>";


      $c_order_html = "<select name='comment_order'>";
      if(!isset($_REQUEST['comment_order']) or $_REQUEST['comment_order'] === 0)
      {
         $c_order_html .= "<option value='0' selected>default</option>";
         $c_set = 0;
      }
      else
      {
         $c_order_html .= "<option value='0'>default</option>";
         $c_set = $_REQUEST['comment_order'];
      }
      if($c_set === 'desc')
      {
         $c_order_html .= "<option selected value='desc'>Oldest Comments First</option>";
      }
      else
      {
         $c_order_html .= "<option value='desc'>Oldest Comments First</option>";
      }
      if($c_set === 'asc')
      {
         $c_order_html .= "<option selected value='asc'>Newest Comments First</option>";
      }
      else
      {
        $c_order_html .= "<option value='asc'>Newest Comments First</option>";
      }
      $c_order_html .= "</select>";

      $c_limit_html = "<select name='comment_limit'>";
      if(!isset($_REQUEST['comment_limit']) or $_REQUEST['comment_limit'] === "")
      {
         $c_limit_html .= "<option value='delete' selected>default</option>";
         $c_set = "";
      }
      else
      {
         $c_limit_html .= "<option value='delete'>default</option>";
         $c_set = $_REQUEST['comment_limit'];
      }
      $vals = array(5,10,15,20,25,30,40,50,60,70,80,90,100,200);
      foreach($vals as $v)
      {
         if($v == $c_set) $s = "selected";
         else $s = "";
         $c_limit_html .= "<option $s value='$v'>$v</option>";
      }
      if($c_set === "0")
      {
         $c_limit_html .= "<option selected value='0'>All</option>";
      }
      else
      {
         $c_limit_html .= "<option value='0'>All</option>";
      }
      $c_limit_html .= "</select>";

      // Generate the HTML for the Video Mode selection
      $video_mode_html = "<select name='video_mode'>";
      if(!isset($_REQUEST['video_mode']) or $_REQUEST['video_mode'] === "")
      {
         $video_mode_html .= "<option value='delete' selected>default</option>";
         $video_mode_set = "";
      }
      else
      {
         $video_mode_html .= "<option value='delete'>default</option>";
         $video_mode_set = $_REQUEST['video_mode'];
      }
      $vals = array('hidden','image');
      foreach($vals as $v)
      {
         if($v == $video_mode_set) $s = "selected";
         else $s = "";
         $video_mode_html .= "<option $s value='$v'>$v</option>";
      }
      $video_mode_html .= "</select>";

      ?>
      <tr class="admin">
         <th class="admin" colspan="3"><big><?=$art?></big></th>
      </tr>
      <tr class="admin">
         <th class="admin"><?=$option?></th>
         <th class="admin"><?=$d?></th>
         <th class="admin"><?=$val?></th>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$cc?></strong></td>
         <td class="admin"><?=$cc_desc?></td>
         <td class="admin"><?=$cc_html?></td>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$c_order?></strong></td>
         <td class="admin"><?=$c_order_desc?></td>
         <td class="admin"><?=$c_order_html?></td>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$c_limit?></strong></td>
         <td class="admin"><?=$c_limit_desc?></td>
         <td class="admin"><?=$c_limit_html?></td>
      </tr>

      <tr class="admin">
         <td class="admin"><strong><?=$video_mode?></strong></td>
         <td class="admin"><?=$video_mode_desc?></td>
         <td class="admin"><?=$video_mode_html?></td>
      </tr>
      <?
   }


   function writeEventsOptions()
   {
      $option = $this->getDisplayOption("optiontext");
      $val = $this->getDisplayOption("valuetext");
      $art = $this->getDisplayOption("eventsoptionstext");
      $d = $this->getDisplayOption("descriptiontext");

      $ev = $this->getDisplayOption("event_displaytitle");
      $ev_desc = $this->getDisplayOption("event_displaydescription");
      $ev_html = "<select name='event_display'>";
      if(!isset($_REQUEST['event_display']) or $_REQUEST['event_display'] === "")
      {
         $ev_html .= "<option value='delete' selected>default</option>";
         $c_set = "";
      }
      else
      {
         $ev_html .= "<option value='delete'>default</option>";
         $c_set = $_REQUEST['event_display'];
      }
      $vals = array("weekly", "biweekly", "monthly");
      foreach($vals as $v)
      {
         if($v == $c_set) $s = "selected";
         else $s = "";
         $ev_html .= "<option $s value='$v'>$v</option>";
      }
      $ev_html .= "</select>";
      ?>
      <tr class="admin">
         <th class="admin" colspan="3"><big><?=$art?></big></th>
      </tr>
      <tr class="admin">
         <th class="admin"><?=$option?></th>
         <th class="admin"><?=$d?></th>
         <th class="admin"><?=$val?></th>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$ev?></strong></td>
         <td class="admin"><?=$ev_desc?></td>
         <td class="admin"><?=$ev_html?></td>
      </tr>
      <?
   }

   // This checks the input form setting for true, false and nothing and builds form option field
   function getFormOptionText($form_setting)
   {
      $opt_html = "";
      if($form_setting === 'true')
      {
         $opt_html .= "<option selected value='true'>Yes</option>";
      }
      else
      {
         $opt_html .= "<option value='true'>Yes</option>";
      }
      if($form_setting === 'false')
      {
         $opt_html .= "<option selected value='false'>No</option>";
      }
      else
      {
        $opt_html .= "<option value='false'>No</option>";
      }

      return $opt_html;

   }
   function writeCommentsOptions()
   {
      $option = $this->getDisplayOption("optiontext");
      $val = $this->getDisplayOption("valuetext");
      $art = $this->getDisplayOption("commentsoptionstext");
      $d = $this->getDisplayOption("descriptiontext");

      $sc_title = $this->getDisplayOption("comment_titles_title");
      $sc_title_desc = $this->getDisplayOption("comment_titles_description");
      $sa_comments = $this->getDisplayOption("all_comments_title");
      $sa_comments_desc = $this->getDisplayOption("all_comments_description");
      $sc_text = $this->getDisplayOption("comments_text_title");
      $sc_text_desc = $this->getDisplayOption("comments_text_description");

      $sc_title_html = "<select name='show_comment_titles'>";

      if(!isset($_REQUEST['show_comment_titles']) or $_REQUEST['show_comment_titles'] === 0)
      {
         $sc_title_html .= "<option value='delete' selected>default</option>";
         $c_set = 0;
      }
      else
      {
         $sc_title_html .= "<option value='delete'>default</option>";
         $c_set = $_REQUEST['show_comment_titles'];
      }

      $sc_title_html .= $this->getFormOptionText($c_set);
      $sc_title_html .= "</select>";

      $sa_comments_html = "<select name='show_all_comments'>";
      if(!isset($_REQUEST['show_all_comments']) or $_REQUEST['show_all_comments'] === 0)
      {
         $sa_comments_html .= "<option value='delete' selected>default</option>";
         $c_set = 0;
      }
      else
      {
         $sa_comments_html .= "<option value='delete'>default</option>";
         $c_set = $_REQUEST['show_all_comments'];
      }

      $sa_comments_html .= $this->getFormOptionText($c_set);
      $sa_comments_html .= "</select>";

      
      $sc_text_html = "<select name='show_comment_text'>";
      if(!isset($_REQUEST['show_comment_text']) or $_REQUEST['show_comment_text'] === 0)
      {
         $sc_text_html .= "<option value='delete' selected>default</option>";
         $c_set = 0;
      }
      else
      {
         $sc_text_html .= "<option value='delete'>default</option>";
         $c_set = $_REQUEST['show_comment_text'];
      }

      $sc_text_html .= $this->getFormOptionText($c_set);
      $sc_text_html .= "</select>";

      ?>
      <tr class="admin">
         <th class="admin" colspan="3"><big><?=$art?></big></th>
      </tr>
      <tr class="admin">
         <th class="admin"><?=$option?></th>
         <th class="admin"><?=$d?></th>
         <th class="admin"><?=$val?></th>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$sc_title?></strong></td>
         <td class="admin"><?=$sc_title_desc?></td>
         <td class="admin"><?=$sc_title_html?></td>
      </tr>
      <tr class="admin">
         <td class="admin"><strong><?=$sa_comments?></strong></td>
         <td class="admin"><?=$sa_comments_desc?></td>
         <td class="admin"><?=$sa_comments_html?></td>
      </tr>

      <tr class="admin">
         <td class="admin"><strong><?=$sc_text?></strong></td>
         <td class="admin"><?=$sc_text_desc?></td>
         <td class="admin"><?=$sc_text_html?></td>
      </tr>
      <?
   }
}

?>
