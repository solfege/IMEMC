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

/************************************************************************/
/* This class is the abstract base class of all objects that can manage */
/* content objects. (ie can allow editors to edit content through them  */
/************************************************************************/
require_once("objects/editlock.inc");
require_once("objects/itemaction.inc");
require_once("objects/storyquery.inc");
require_once("sitefiltereddisplay.inc");

class managingDisplay extends siteFilteredDisplay
{
   var $stories;
   var $ignore_hidden=false;
   var $valid_item_actions = array();//what actions this object can process
   var $valid_item_options = array();//what options are valid to display for this object
   var $item_action;
   var $editlock_obj = null;

   function managingDisplay($obj_id)
   {
      $this->siteFilteredDisplay($obj_id);
   }

   function initiallise()
   {
      parent::initiallise();
      $this->setValidActions();
      $this->setValidOptions();
   }

   //defines what actions this object can deal with for each type of target
   function setValidActions ()
   {
      global $system_config;
      $this->valid_item_actions = array(
         "attachment" => array('clip', 'unclip', 'copy', "copyandunhide", "create",
                     "move", "edit", "hide", "unhide", "delete", "featurize", "unfeaturize", "swap"),
         "comment" => array('clip', 'unclip', 'upgrade', 'copy', "edit", "pending", "hide", "unhide", "delete", "release", "annotate"),
         "story" => array('clip', 'unclip', 'downgrade', "upgrade", "edit", "pending", "hide", "unhide", "lock", "unlock", "delete", "stick", "unstick", "release", "promote", "demote", "translate", "public_edit", "remove_edit", "annotate"),
         "feature" => array('clip', 'unclip', "edit", "pending", "hide", "unhide", "lock", "unlock", "delete", "stick", "unstick", "release", "translate", "annotate", "promote", "demote")
      );

      if ($system_config->comment_vote_hide_block == true) {
          array_push($this->valid_item_actions["comment"], "promote", "demote");
      }
   }

   //defines what actions this object should display when it draws an item of the relevant type
   function setValidOptions()
   {
      global $system_config;
      $this->valid_item_options = array(
         "attachment" => array('clip', 'unclip', "edit", "hide", "unhide", "delete", "featurize", "unfeaturize","swap"),
         "story" => array("multi", 'clip', "unclip", "upgrade", "edit", "hide", "unhide", "pending", "lock", "unlock", "delete", "stick", "unstick", "promote", "demote"),
         "feature" => array("multi", 'clip', "edit", "hide", "unhide", "pending", "lock", "unlock","delete", "stick", "unstick", "promote", "demote"),
         "comment" => array("multi", 'clip', "unclip", 'upgrade', "edit", "hide", "unhide", "pending", "delete", "release", "annotate")
      );

      if ($system_config->comment_vote_hide_block == true) {
          array_push($this->valid_item_options["comment"], "promote", "demote");
      }
   }


   /*******************************************
   //Main Function to display the object...
   /******************************************/
   function processRequest()
   {
      global $editor_session, $user_prefs;
      parent::processRequest();

      if($editor_session->isSessionOpen() && $this->isEditAction())
      {
         if($this->item_action->isCancel())
         {
            $this->display();
         }
         elseif(!$this->item_action->requiresPermission())
         {
            $action_result = $this->item_action->execute($this->query);
            $this->display($action_result);
         }
         elseif($this->item_action->isPermitted())
         {
            if($this->item_action->isConfirm() or !$this->item_action->requiresConfirm())
            {
               if($this->item_action->requiresApproval())
               {
                  $action_result = $this->item_action->addToApprovalQueue();
                  $this->displayApprovalPending($action_result);
                  $this->display($action_result);
               }
               else
               {
                  $action_result = $this->item_action->execute($this->query);
		  if ($action_result == false) {
                      $act_txt = $this->item_action->toString()." - Not Permitted";
		      if ($this->item_action == "hide") 
                          $msg = "General failure: Possibly a hide was attempted on a comment already voted on";
		      else
                          $msg = "General failure: You cannot carry out this action";
                      writeFailureMessage($act_txt, $msg);
                  }
                  $this->display($action_result);
               }
            }
            elseif($this->item_action->requiresConfirm())
            {
               if($this->item_action->item_action == "translate")
                   $this->displayTranslateLink();
               else
                   $this->displayConfirm();
            }
         }
         else
         {
            $this->displayNotPermitted();
         }
      }
      else
      {
         $this->display();
      }

   }

   /*********************************************************************/
   /* Result Page drawing functions                              */
   /*********************************************************************/
   function writeFailureMessage($act_txt, $msg)
   {
      global $graphics_store;
      $icon = "<p align='center'><img src='$graphics_store"."warning.gif' alt='warning' style='border: #000 1pt solid'></p>";
      echo "<div class='user-message'>\n";
      echo $icon;
      echo "<h3 align='center' class='publish-header'>$act_txt</h3>\n";
      echo "<p>$msg</p>\n";
      echo "</div>\n<br>";
   }

   //confirms always point to themselves
   function getConfirmHeader($use_translate_hdr = false)
   {
      $lnk = $this->getFilterLink();
      if ($use_translate_hdr == true )
          $str = '<form name="action_confirm" action="'.$lnk.'" method=post>'.'<div class="translateconfirm">';
      else
          $str = '<form name="action_confirm" action="'.$lnk.'" method=post>'.'<div class="editorconfirm">';

      $str .= $this->item_action->getAsHiddenHTML();
      return $str;
   }

   function getConfirmFooter()
   {
      $str = '<div class="editorconfirmbuttons">';
      $str .= '<input class="editorconfirmbuttons" type=submit name=cancel value="&lt;&lt; cancel"></input>';
      $str .='<input class="editorconfirmbuttons" type=submit name=confirm value="';
      $str .= $this->item_action->item_action.' &gt;&gt;"></input></div>';
      $str .= "</div></div></form>";
      return $str;
   }


   function displayNotPermitted()
   {
      $act_txt = $this->item_action->toString()." - Not Permitted";
      $msg = "You do not have sufficent permissions to carry out this action";
      writeFailureMessage($act_txt, $msg);
   }

   function displayApprovalPending($action_result)
   {
      $act_txt = $this->item_action->toString()." - Approval Required";
      $msg = "You do not have sufficent permissions to carry out this action. It must be approved before it is saved to the database.  An email has been sent to seek approval for your action";
      writeFailureMessage($act_txt, $msg);
   }

   function getNotifyForm()
   {
      global $graphics_store;
      $str = '<div class="notifybox">';
      $str .= '<img src="'.$graphics_store.'mailicon.gif"> ';
      $str .= '<b>Send Mail Notification </b>';
      $str .= '<input type=checkbox name=send_mail ';
      if($this->item_action->requiresNotify()) $str .= 'checked DISABLED>';
      else $str .= ">";

      /*
      if ($this->item_action->item_action == "hide" && $this->item_action->item_type == "comment") {
          $trans_id=1;
	  echo "Rate this comment. 1 = borderline okay .... 5 (very bad) <BR>";
	  ?> <input type=radio name="trans_<?=$trans_id?>" value="rate_1"> 1
	     <input type=radio name="trans_<?=$trans_id?>" value="rate_2"> 2 
	     <input type=radio name="trans_<?=$trans_id?>" value="rate_3"> 3 
	     <input type=radio name="trans_<?=$trans_id?>" value="rate_4"> 4 
	     <input type=radio name="trans_<?=$trans_id?>" value="rate_5"> 5 
          <?

      }
      */

      $str .= '<br /><br />';
      $str .= '<B>Reason for taking action "'.$this->item_action->item_action.'" on '.$this->item_action->item_type.'</B><br />';
      $str .= '<TEXTAREA name=action_reason rows=5 cols=30></TEXTAREA>';
      return $str;
   }


   function displayConfirm()
   {
      global $editor_session,$system_config,$path_prefix, $suppress_page_insets;
      $suppress_page_insets = true;
      $this->item_action->loadItem();
      $item_id=$this->item_action->item_id;
      echo $this->getConfirmHeader();
      echo $this->item_action->getConfirmMessage();
      if ($this->item_action->item_action == "unhide" && $this->item_action->item_type == "feature")
      {
          echo $this->generateDelayUnhide();
      }
      echo $this->getNotifyForm();
      echo $this->getConfirmFooter();
   }

   function generateDelayUnhide()
   {
       $sch_choice= "<p align='center'> 
	   <input type=radio name=feature_unhide_mode value='unhide_now' checked onclick='javascript:updateSchDateMode()'>Unhide Now  &nbsp; &nbsp;
	   <input type=radio name=feature_unhide_mode value='unhide_later' onclick='javascript:updateSchDateMode()'>Schedule Unhide Later <br><br>
       ".getReminderDateSelect(). "</p>
       <script type='text/javascript' language='JavaScript'>
       function updateSchDateMode()
       {
          for (var i=0; i < document.action_confirm.feature_unhide_mode.length; i++)
          {
               if (document.action_confirm.feature_unhide_mode[i].checked)
               {
                   var checked_value = document.action_confirm.feature_unhide_mode[i].value;
               }
          } 
	  if (checked_value == 'unhide_now')
	  {
	      document.action_confirm.event_time_day.disabled = true;
	      document.action_confirm.event_time_month.disabled = true;
	      document.action_confirm.event_time_year.disabled = true;
	      document.action_confirm.event_time_hr.disabled = true;
	      document.action_confirm.event_time_min.disabled = true;
	  } else {
	      document.action_confirm.event_time_day.disabled = false;
	      document.action_confirm.event_time_month.disabled = false;
	      document.action_confirm.event_time_year.disabled = false;
	      document.action_confirm.event_time_hr.disabled = false;
	      document.action_confirm.event_time_min.disabled = false;
	  }
       }
       updateSchDateMode();
       </script>
       ";

       return $sch_choice;
   }

   function getTranslatedLinksArray($story_id_array)
   {
      if (count($story_id_array) == 0 ) return array();

      global $prefix, $dbconn, $languageList, $system_config;
      $t_base_article = $this->getArticleBaseLink();
      $ret_str = array();
      $found_array = array();
      $found_trans_array = array();

      $in_part = " IN (";
      foreach ($story_id_array as $s) {
          $in_part .= $s->story_id.",";
      }
      $in_part = substr($in_part,0,-1) .")";
      // echo "Doing 1st query  for ".$in_part . "<BR>";
      
      $result = sql_query("SELECT ct.content_type, ct.translated_content_id, ct.original_content_id, ot.language_id, ot.story_title from ".$prefix."_content_translations as ct RIGHT JOIN ".$prefix."_stories as ot ON ct.translated_content_id=ot.story_id WHERE ct.original_content_id $in_part", $dbconn, 2);
      checkForError($result);
      if(sql_num_rows( $result ) > 0)
      {
          for ($irow=0; $irow < sql_num_rows( $result ); $irow++)
          {
             list($content_type, $translated_story_id, $original_story_id, $translated_language_id,$translated_title) = sql_fetch_row($result, $dbconn);
             $lingoObj = $languageList->getLanguageByID($translated_language_id);
             $found_array[] = $original_story_id;
	     $found_trans_array[] = $translated_story_id;

             $ret_str[$original_story_id][$translated_story_id] = "[".$t_base_article .$translated_story_id.'">'.$lingoObj->getName().'</a>] ';
	     // echo "1st Return org story ". $original_story_id. " trans story ".$translated_story_id. "<br>";
          }
      }

      $t_atleast_1 = false;
      $in_part = " IN (";
      foreach ($story_id_array as $s) {
	  if (in_array($s->story_id, $found_array) ) continue;

          $in_part .= $s->story_id.",";
          $t_atleast_1 = true;
      }
      foreach ($found_trans_array as $t_id) {
          $in_part .= $t_id.",";
          $t_atleast_1 = true;
      }
      $in_part = substr($in_part,0,-1) .")";

      if ($t_atleast_1 == true) {
	  //  echo "Doing next query  for ".$in_part . "<BR>";
          // No look up via translated story id.
	  // Now we can do the reverse lookup but remember to exclude the original story id.
          $second_result = sql_query("SELECT ct.content_type, ct.translated_content_id, ct.original_content_id, ot.language_id, ot.story_title from ".$prefix."_content_translations as ct LEFT JOIN ".$prefix."_stories as ot ON ot.story_id=ct.original_content_id WHERE ct.translated_content_id $in_part", $dbconn, 2);
          if(sql_num_rows( $second_result ) > 0)
	  {
                $reverse_story_ids = array();
                for ($irow=0; $irow < sql_num_rows( $second_result ); $irow++)
                {
                    list($content_type, $translated_story_id, $original_story_id, $translated_language_id, $translated_title) = sql_fetch_row($second_result, $dbconn);
                    $lingoObj = $languageList->getLanguageByID($translated_language_id);
		    if (in_array($original_story_id, $found_array) ) {
                        $ret_str[$translated_story_id][$original_story_id] = "";
                    } else {
                        $reverse_story_ids[] = $original_story_id;
			if (!isset($ret_str[$translated_story_id][$original_story_id])) {
                            $t_link = "[".$t_base_article . $original_story_id.'">'.$lingoObj->getName().'</a>] ';
                            $ret_str[$translated_story_id][$original_story_id] = $t_link;
			}
		    }
	            // echo "2nd Return trans story ".$translated_story_id. " org story ".$original_story_id."<br>";
                }
		// Now do the reverse lookup but only for those story ids that are not in the first list
		$rev_search_set = array();
                $in_part = " IN (";
                foreach ($reverse_story_ids as $s_id) {
	            if (in_array($s_id, $found_array) ) continue;
		    $rev_search_set[] = $s_id;
                    $in_part .= $s_id.",";
                }
                $in_part = substr($in_part,0,-1) .")";

		if (count($rev_search_set) > 0 ) { 
                    $reverse_result = sql_query("SELECT ct.content_type, ct.translated_content_id, ct.original_content_id, ot.language_id, ot.story_title from ".$prefix."_content_translations as ct RIGHT JOIN ".$prefix."_stories as ot ON ct.translated_content_id=ot.story_id WHERE ct.original_content_id $in_part", $dbconn, 2);
                    if(sql_num_rows( $reverse_result ) > 0)
	            {
                        for ($irow=0; $irow < sql_num_rows( $reverse_result ); $irow++)
                        {
                            list($content_type, $translated_story_id, $original_story_id, $translated_language_id, $translated_title) = sql_fetch_row($reverse_result, $dbconn);
                            $lingoObj = $languageList->getLanguageByID($translated_language_id);
                            $ret_str[$original_story_id][$translated_story_id] = "[".$t_base_article . $translated_story_id.'">'.$lingoObj->getName().'</a>] ';
			    /*
		            if (in_array($original_story_id, $reverse_story_ids) ) {
			        echo "duplicate ";
                                $ret_str[$translated_story_id] = $original_story_id;
                            } else {
                                $ret_str[$translated_story_id][$original_story_id] = "y[".$t_base_article . $translated_story_id.'">'.$lingoObj->getName().'</a>] ';
		            }
                            */
	                    // echo "3rd Return org story ". $original_story_id. " trans story ".$translated_story_id. "<br>";
		        }
		    }
		}
          }
          return $ret_str;
      }

      return $ret_str;
   }

   function getTranslatedLinks($story_id, $generate_links = false, $list_mode=false, $add_text = false)
   {
      if ($story_id == null && $generate_links == true) return "";
      if ($story_id == null) return "No valid story id given ";

      global $prefix, $dbconn, $languageList, $system_config;
      $t_base_article = $this->getArticleBaseLink();
      $ret_str = array();

      $result = sql_query("SELECT ct.content_type, ct.translated_content_id, ot.language_id, ot.story_title from ".$prefix."_content_translations as ct RIGHT JOIN ".$prefix."_stories as ot ON ct.translated_content_id=ot.story_id WHERE ct.original_content_id = $story_id", $dbconn, 2);
      checkForError($result);
      if(sql_num_rows( $result ) > 0)
      {
          if ($generate_links == true && $add_text == true ) $ret_str[] = "Translated version in ";
          for ($irow=0; $irow < sql_num_rows( $result ); $irow++)
          {
             list($content_type, $translated_story_id, $translated_language_id,$translated_title) = sql_fetch_row($result, $dbconn);
             $lingoObj = $languageList->getLanguageByID($translated_language_id);
             if ($generate_links == true) 
	     {
                 $ret_str[] = "[".$t_base_article .$translated_story_id.'">'.$lingoObj->getName().'</a>] ';
             }
	     else
	     {
                 //$ret_str[$translated_story_id] = "Story ". $translated_story_id . " in language " .$translated_language_id." ".$lingoObj->getName();
                 $ret_str[$translated_story_id] = "Story ". $translated_story_id . " in ".$lingoObj->getName();
             }
          }
      }
      else
      {
          // Now look up via translated story id.
          $result = sql_query("SELECT original_content_id from ".$prefix."_content_translations WHERE translated_content_id = $story_id", $dbconn, 2);
          if(sql_num_rows( $result ) > 0)
          {
             // Assume there is only 1 row in this reverse situation and then use it to redo the original lookup.
             // but first do a query to get the language id of it.
             list($translated_story_id) = sql_fetch_row($result, $dbconn);

             $lang_result = sql_query("SELECT language_id, story_title from ".$prefix."_stories WHERE story_id = $translated_story_id", $dbconn, 2);
             list($translated_language_id,$translated_title) = sql_fetch_row($lang_result, $dbconn);
             $lingoObj = $languageList->getLanguageByID($translated_language_id);
             if ($generate_links == true) 
	     {
                 $ret_str[] = "[".$t_base_article .$translated_story_id.'">'.$lingoObj->getName().'</a>] ';
             }
	     else
	     {
                 //$ret_str[$translated_story_id] = "Story ". $translated_story_id . " in language " .$translated_language_id." ".$lingoObj->getName();
                 $ret_str[$translated_story_id] = "Story ". $translated_story_id . " in ".$lingoObj->getName();
             }

	     // Now we can do the reverse lookup but remember to exclude the original story id.
             $second_result = sql_query("SELECT ct.content_type, ct.translated_content_id, ot.language_id, ot.story_title from ".$prefix."_content_translations as ct LEFT JOIN ".$prefix."_stories as ot ON ot.story_id=ct.translated_content_id WHERE ct.original_content_id = $translated_story_id AND ct.translated_content_id !=$story_id", $dbconn, 2);
             if(sql_num_rows( $second_result ) > 0)
             {
                for ($irow=0; $irow < sql_num_rows( $second_result ); $irow++)
                {
                    list($content_type, $translated_story_id, $translated_language_id,$translated_title) = sql_fetch_row($second_result, $dbconn);
                    $lingoObj = $languageList->getLanguageByID($translated_language_id);
                    if ($generate_links == true) 
   	            {
                        $ret_str[] = "[".$t_base_article . $translated_story_id.'">'.$lingoObj->getName().'</a>] ';
                    }
   	            else
   	            {
                        // $ret_str[$translated_story_id] = "Story ". $translated_story_id . " in language " .$translated_language_id." ".$lingoObj->getName();
                        $ret_str[$translated_story_id] = "Story ". $translated_story_id . " in ".$lingoObj->getName();
                    }
                }
             }
             return $ret_str;
          }
          if ($generate_links == true) return "";
          return "No translated versions found";
      }
      return $ret_str;
   }

   function getArticleBaseLink()
   {
       global $system_config, $redirectList;
       // This is a lot of work but Italians use artico instead of article, so have to do the lookup just in case.
       // Note the redirects should already be loaded.
       $article_primary = "article";
       $redirectList->load();
       foreach($redirectList->redirects as $redirect)
       {
           if ($redirect->type  == "ArticleRedirector") {
              $article_primary = $redirect->getPrimaryRedirect();
	      break;
           }
       }
       $http_mode = isset($_SERVER['HTTPS']) ? "https" : "http";
       // Basically http://www.site.domain/article/
       $t_art_base = '<a href="'.$http_mode.'://'.$system_config->site_url.'/'.$article_primary.'/';

       return $t_art_base;
   }

   function displayTranslateLink()
   {
      global $editor_session,$system_config,$path_prefix, $suppress_page_insets;
      $suppress_page_insets = true;
      $this->item_action->loadItem();
      $item_id=$this->item_action->item_id;
      echo $this->getConfirmHeader(true);
      echo $this->item_action->getConfirmMessage();

      $str = '<div class="translatebox">';
      $str .= 'Translated Story Id: ';
      $str .= '<input type=text name=translated_story_id size=10>';
      $str .= '<input type=radio name="trans_add" value="add" checked> Add';
      $str .= '<input type=radio name="trans_add" value="no change"> No Action <BR>';
      $str .= '<br /><br />';
      $str .= '<B>Existing linked translated stories </B><br />';
      echo $str;
      $trans_list = $this->getTranslatedLinks($this->item_action->item->story_id);
      $count = 1;

      if (is_array($trans_list)) {
      ?> <Table border=0 width=100%> <?
      foreach($trans_list as $trans_id => $trans_item) {
          ?> <TR><TD> <?
          echo $trans_item;
	  ?> </td><td align=left><input type=text name="trans_story_id_<?=$count?>" value="" size=10>
	     <input type=hidden name="trans_id_<?=$count?>" value="<?=$trans_id?>" >
	     <input type=radio name="trans_<?=$trans_id?>" value="update"> Update
	     <input type=radio name="trans_<?=$trans_id?>" value="remove"> Remove 
	     <input type=radio name="trans_<?=$trans_id?>" value="no change" checked> No Update 
	     </td></tr>
	  <?
          $count++;
      }
      ?> </Table> <?
      } else {
          echo $trans_list;
      }
      echo $this->getConfirmFooter();
   }


   /*******************************************
   /*function to determine whether the object has been */
   /*called to perform an editorial action on content */
   /******************************************/
   function isEditAction()
   {
      foreach(array_keys($this->valid_item_actions) as $item_type)
      {
         foreach($this->valid_item_actions[$item_type] as $item_action)
         {
            if(isset($_REQUEST[$item_action.$item_type]))
            {
               $this->item_action = new itemAction($item_type, $item_action, $_REQUEST[$item_action.$item_type], "write");
               return true;
            }
            elseif(isset($_REQUEST["multi".$item_action.$item_type]))
            {
               $this->item_action = new itemAction($item_type, "multi".$item_action, $_REQUEST["multi".$item_action.$item_type], "write");
               return true;
            }
         }
      }
      return false;
   }

   //returns an array of viable editor options for the item
   function getItemOptions(&$item,$item_type,$summary_mode, $opt_num=0, $attachIds=NULL)
   {
      global $editor_session, $system_config;
      $all_options = array();
      if(isset($_REQUEST['print_page']) && $_REQUEST['print_page']=="true") return "";

      if($editor_session->canEditContent())
      {
         // This should get load only once per newswire or article request. This is used to decide whether
         // to draw the 'release' option. If another user locks an item in meantime and display is not refresh
         // then we will not display the option until the next refresh.
         if ($this->editlock_obj == null)
	 {
             $this->editlock_obj = new EditLock();
             $this->editlock_obj->load("both");
	 }
         //if((!isset($editor_session->editor->editor_state['edit_mode'])) or ($editor_session->editor->editor_state['edit_mode'] != 'content' and $editor_session->editor->editor_state['edit_mode'] != 'all')) return "";
         if ($item_type=="story") $item_id=$item->story_id;
         else if ($item_type=="comment") $item_id=$item->comment_id;
         else if ($item_type=="attachment") $item_id=$item->attachment_id;
         if ($item_type=="attachment")
         {
            $cls = 'editor-attachment-option';
         }
         else
         {
            $cls = 'editor-option';
         }
         if($item_type=="story" && $item->type_id==1) $item_type="feature";
         $t_public_edit = false;
	 if($item_type=="story" && $system_config->enable_public_editing == true ) {
             require_once("objects/publiclist.inc");
             $t_editList = new EditEntriesList();
             $t_public_edit = $t_editList->isStoryInList($item->story_id);
         }
         $story_id_backup = $this->query->story_id;
         if($summary_mode) $this->query->story_id = null;
         $display_options = array();
         $item_options = $this->valid_item_options[$item_type];
         $proposed_action = new itemAction($item_type, "", $item_id, "");
         $proposed_action->setItem($item);
         $target_obj = $this->getTargetObject($proposed_action);
         $f_lnk = $this->getFilterLink($target_obj);
         foreach($item_options as $item_action)
         {
            //have to find the ones that are relevant...
            if(($item_action == 'hide' && $item->hidden==1) or
               ($item_action == 'pending' && $item->hidden==2) or
               ($item_action == 'unhide' && $item->hidden==false) or
               ($item_action == 'lock' && $item->locked==true) or
               ($item_action == 'unlock' && $item->locked==false) or
               ($item_action == 'stick' && $item->sticky_time>0) or
               ($item_action == 'unstick' && $item->sticky_time==0) or
               ($item_action == 'release' && $this->editlock_obj->lockExists($item_type, $item_id) == false) or
               ($item_action == 'public_edit' && ($t_public_edit == true or $system_config->enable_public_editing == false)) or
               ($item_action == 'remove_edit' && ($t_public_edit == false or $system_config->enable_public_editing == false)) or
               //($item_action == 'featurize' && !($item->featurized==false)) or
               ($item_action == 'featurize' && !(($item->isImage() || $item->isAudio() || $item->isEmbeddedAudio()) && $item->featurized==false)) or
               ($item_action == 'unfeaturize' && !(($item->isImage() || $item->isAudio() || $item->isEmbeddedAudio()) && $item->featurized==true)))
               continue;

            $proposed_action->setItemAction($item_action);
            $action_link = $this->getActionLink($proposed_action, $f_lnk, $summary_mode);
	    if ($item_action == "swap") {
	        // This will be the case when there is only one image.
	        if ($attachIds == NULL) continue;
	        if (count($attachIds) <= 1) continue;

                $oneaction = $proposed_action->getOptionHTML($action_link, $opt_num, $cls, $attachIds);
            }
	    else
                $oneaction = $proposed_action->getOptionHTML($action_link, $opt_num, $cls);

            $this->query->obj_id = $this->obj_id; //reset query to this
            if($oneaction !== false) $display_options[] = $oneaction;
         }
         if($summary_mode) $this->query->story_id = $story_id_backup;
         $retstr = "";
         if(count($display_options) > 0)
         {
            $retstr   = "<div class='$cls'>".implode(" ", $display_options)."</div>";
            return $retstr;
         }
      }
      return "";
   }

   function getTargetObject(&$item_action)
   {
      if($item_action->item_action == "edit" and $item_action->item_type != 'attachment')
      {
         return $this->site_settings["publishobject"];
      }
      else
      {
         return $this->obj_id;
      }
   }

   function getActionLink(&$proposed_action, $lnk = null, $summary_mode=false)
   {
      if($lnk === null or $proposed_action->item_action == "edit")
      {
         $target_obj = $this->getTargetObject($proposed_action);
         if($summary_mode) $this->query->story_id = $proposed_action->item->story_id;
         $lnk = $this->getFilterLink($target_obj);
         if($summary_mode) $this->query->story_id = null;
      }
      $lnk = addLinkParameters($lnk, array($proposed_action->item_action.$proposed_action->item_type."=".$proposed_action->item_id));
      return $lnk;
   }

   function writeMultiEditOptions($type, $actions, $num)
   {
      global $editor_session, $oscailt_page_setting;

      $site_id = $oscailt_page_setting['site_id'];
      if($num <= 1 or $editor_session->canEditContent() == false) return "";
      $allowed_actions = array();

      foreach($actions as $action)
      {
         if($editor_session->editor->allowedReadAccessTo("multi".$action.$type, $site_id))
         {
            $allowed_actions[] = $action;
         }
      }

      if(count($allowed_actions) > 0)
      {

         ?>
         <script language="Javascript">
         <!--
            function processCheckBoxes(num, urlbase, action, type)
            {
               var i = 0;
               var formname = "multiitem_";
               var allitems = new Array(num-1);
               var currentitem = 0;
               while(i < num)
               {
                  i = i + 1;
                  var thisform = formname + i;
                  if(document[thisform].itemselect.checked)
                  {
                     allitems[currentitem] = document[thisform].item_id.value;
                     currentitem++;
                  }
               }
               //build up the string to load...
               var actionstring;
               if(currentitem > 0)
               {
                  actionstring = urlbase + "&" + action + type + "=";
                  for (x=0; x<currentitem; x++)
                  {
                     if(x > 0){
                        actionstring = actionstring + ",";
                     }
                     actionstring = actionstring + allitems[x];
                  }
                  window.location = actionstring;
               }
               else
               {
                  actionstring = urlbase;
                  alert("No Items Selected");
               }
            }
         //-->
         </script>
         <?
         echo "<p class='multioptions'>";
         echo " All Selected: <input type='checkbox' checked disabled>";//just for show
         $lnk = $this->getFilterLink();
         foreach($allowed_actions as $act)
         {
            $tit = "$act every selected $type";
            $tmp_ia = new ItemAction($type, $act, 0, "");
            $tmp_ia->loadActionData();
            $name = $tmp_ia->actionData['name'];
            $icon = $tmp_ia->actionData['icon'];
            $alt = $tmp_ia->actionData['description'];
            $cls = 'editor-option';
            echo "<a class='$cls' title='".htmlspecialchars($tit, ENT_QUOTES)."' ". 'href="javascript:processCheckBoxes('.$num;
            echo ", '$lnk', '$act', '$type')\"><img alt='".htmlspecialchars($alt, ENT_QUOTES)."' src='$icon' class='$cls'>$name</a> ";
         }
         echo "<p>";
      }
   }


}

