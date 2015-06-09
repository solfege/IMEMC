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
$allActionData = array();
include_once("config/notification_levels.php");

//class to represent an action on a particular content item
class itemAction
{
   var $item_type;
   var $item_action;
   var $item_id;
   var $mapped_item_ids = array();
   var $actionData;
   var $action_type;
   var $saved_items = array();
   var $item;  //the object to be acted upon...
   var $logEditorHacks = true;
   var $logActionHacks = true;
   var $user_messages = array();
   var $query;

   function itemAction($item_type, $item_action, $item_id, $action_type)
   {
      $this->item_type = $item_type;
      $this->item_action = $item_action;
      $this->item_id = $item_id;
      $this->action_type = $action_type;
   }

   function toString($include_body = false)
   {
      $str = $this->item_action." ".$this->item_type." ";
      $item_ids = explode(",", $this->item_id);
      $str .= implode(", ", $item_ids);
      return $str;
   }

   function loadDataFile($fname)
   {
      global $graphics_store, $allActionData;
      if(isset($allActionData[$fname]))
      {
         $this->actionData = $allActionData[$fname];
         if($allActionData[$fname] === false) return false;
         return true;
      }
      if (!file_exists($fname))
      {
         $allActionData[$fname] = false;
         return false;
      }
      $old = ini_get('track_errors');
      ini_set('track_errors', false);
      include($fname);
      ini_set('track_errors', $old);
      $allActionData[$fname] = $actionData;
      $this->actionData = $actionData;
      if (isset($php_errormsg) or !isset($actionData))
      {
         if(isset($php_errormsg)) $error_message = $php_errormsg;
         else $error_message = "Failed to load Action Data from $fname";
         logMessage($error_message,"sitelog.txt");
         return false;
      }
      //$this->meta = $actionData;
      return true;
   }

   //tries files in the following order:
   //1. action_item_lang.inc
   //2. action_lang.inc
   //3. action_item.inc
   //4. action.inc
   //5. defaultaction_lang.inc
   //6. defaultaction.inc
   function loadActionData()
   {
      global $userLanguage, $system_config;
      $old_id = $this->item_id;
      $this->item_id = str_replace(',', ", ", $this->item_id);
      $docbase = $system_config->item_action_cache_store;
      $langCode = $userLanguage->getLanguageCodePrefix();
      $onetry = $docbase.$this->item_action.'_'.$this->item_type.'_'.$langCode.'.inc';
      if($this->loadDataFile($onetry)) return true;
      $onetry = $docbase.$this->item_action.'_'.$this->item_type.'.inc';
      if($this->loadDataFile($onetry)) return true;
      $onetry = $docbase.$this->item_action.'_'.$langCode.'.inc';
      if($this->loadDataFile($onetry)) return true;
      $onetry = $docbase.$this->item_action.'.inc';
      if($this->loadDataFile($onetry)) return true;
      $onetry = $docbase.'defaultaction_'.$langCode.'.inc';
      if($this->loadDataFile($onetry)) return true;
      $onetry = $docbase.'defaultaction.inc';
      if($this->loadDataFile($onetry)) return true;
      $this->item_id = $old_id;
      return false;
   }

   function getOptionHTML($lnk, $item_number, $cls, $attachIds = NULL)
   {
      // item_number means different things in different modes.
      if($this->requiresPermission() && !$this->isPermitted() && $this->item_action!='multi')
      {
         return false;
      }

      if($this->item_action != "multi")
      {
         $this->loadActionData();
	 // print_r ($this->actionData) ;
         $name = $this->actionData['name'];
         $icon = $this->actionData['icon'];

         $alt = htmlspecialchars($this->actionData['description'], ENT_QUOTES);
         if($this->requiresPermission() && $this->requiresApproval())
         {
            $name = 'propose '.$name;
         }
	 // You would add code here to check for swap and add the javascript for the dropdown list of image
	 // numbers to swap with. This would require extension of this class
         if($this->item_action == 'swap' && $this->item_type == 'attachment' && $attachIds !=NULL)
         {
            $htmlstr = '<form style="display: inline; margin: 0;" method="post" action="'.$lnk.'" name="swap_'.$this->item_type.'_'.$this->item_id.'"><a class="'.$cls.'" href="javascript:document.swap_'. $this->item_type.'_'. $this->item_id.'.submit()"><img class="'.$cls.'" alt="$alt" src="'.$icon.'" border=0>'.$name.'</a>';
	    
            $htmlstr .= '<select name="'.$this->item_type.'_'.$this->item_id.'_with">';
	    // item number is zero based.
            for($orderIndex = 0; $orderIndex < count($attachIds); $orderIndex++)
	    {
		if ($orderIndex == $item_number) continue;
                $htmlstr .= '<option value="'.$attachIds[$orderIndex].'">'. ($orderIndex+1) .'</option>';
            }
            $htmlstr .= '</select></form>';
         }
	 elseif($this->item_action == 'edit' && $this->item_type == 'attachment')
         {
            $htmlstr = '<form style="display: inline; margin: 0;" method="post" action="'.$lnk.'" name="edit_'.$this->item_type.'_'.$this->item_id.'"><a class="'.$cls.'" href="javascript:document.edit_'. $this->item_type.'_'. $this->item_id.'.submit()"><img class="'.$cls.'" alt="$alt" src="'.$icon.'" border=0>'.$name.'</a><input name="'.$this->item_type.'_'.$this->item_id.'_description" value="'.htmlspecialchars($this->item->description).'" size=70></span></form>';
         }
	 elseif($this->item_action == 'promote' && $this->item_type == 'story')
         {
            $votecount = $this->item->vote;
            $htmlstr = "<a class='$cls' href='$lnk'><img alt='$alt' src='$icon' class='$cls'>$name ($votecount)</a>";
         }
         else
         {
            $htmlstr = "<a class='$cls' href='$lnk'><img alt='$alt' src='$icon' class='$cls'>$name</a>";
         }
      }
      elseif($this->item_action == "multi")
      {
         $type = ($this->item_type == "feature") ? "story" : $this->item_type;
         $htmlstr = '<form style="display: inline; margin: 0;" name="multiitem_'.$item_number.'" method=post action=""><input type="hidden" name="item_id" value="'.$this->item_id.'" /><input class="itemselect" name="itemselect" type=checkbox /></form> ';
      }
      return $htmlstr;
   }



   function addUserMsg($msg)
   {
      $this->user_messages[] = $msg;
   }

   function getUserMsgs($msg)
   {
      return implode("\n", $this->user_messages);
   }

   function setItemType($item_type)
   {
      $this->item_type = $item_type;
   }

   function setItemAction($item_action)
   {
      $this->item_action = $item_action;
   }

   function setActionType($action_type)
   {
      $this->action_type = $action_type;
   }

   function setItemID($item_id)
   {
      $this->item_id = $item_id;
   }

   function setItem(&$item)
   {
      $this->item = $item;
   }

   function setItemContext($context)
   {}

   function getAsHiddenHTML()
   {
      $retstr='<input type="hidden" name="'.$this->item_action.$this->item_type.'" value="'.$this->item_id.'"'.">\n";
      if ($this->item_action == "swap")
      {
          $swapval = $_REQUEST[$this->item_type.'_'.$this->item_id.'_with'];
          $retstr .='<input type="hidden" name="'.$this->item_action.'_with'.'" value="'.$swapval.'"'.">\n";
      }
      return $retstr;
   }

   //determines if a particular action requires a notify
   function requiresNotify()
   {
      global $oscailt_page_setting, $lowlevel_item_actions,$mediumlevel_item_actions, $highlevel_item_actions ;

      if($this->item_action == 'clip' or $this->item_action=='unclip' or $this->item_action == 'release') return false;
      if($this->item_action == 'promote' ) return false;
      if($this->item_action == 'demote' ) return false;
      if($this->item_action == 'translate' ) return false;

      if(!isset($oscailt_page_setting["site_mail_settings"]) or !isset($oscailt_page_setting["site_mail_settings"]["level"])) return true;
      $lvl = strtolower($oscailt_page_setting["site_mail_settings"]['level']);
      if($lvl == 'none') return false;
      if($lvl == 'all') return true;

      if($this->itemActionContained($lowlevel_item_actions))
      {
         return true;
      }
      if($lvl == 'low') return false;
      if($this->itemActionContained($mediumlevel_item_actions))
      {
         return true;
      }
      if($lvl == 'medium') return false;
      if($this->itemActionContained($highlevel_item_actions))
      {
         return true;
      }
      if($lvl == 'high') return false;
      return true; //shouldnt get here - just in case y'know.
   }

   function itemActionContained(&$action_array)
   {
      if(isset($action_array[strtolower($this->item_type)]) && in_array($this->item_action, $action_array[strtolower($this->item_type)])) return true;
      return false;
   }

   //determines whether a particular action requires an approval before being carried out..
   function requiresApproval()
   {
      global $oscailt_page_setting, $editor_session;
      if($editor_session->editor->allowedWriteAccessTo($this->item_action.$this->item_type, $oscailt_page_setting['site_id']))
      {
         return false;
      }
      elseif($this->item_action == 'edit' and $editor_session->editor->allowedWriteAccessTo("editowncontent", $oscailt_page_setting['site_id']) && (($this->item_type == 'story' or $this->item_type == 'comment') && strcasecmp($this->item->author_name, $editor_session->editor->editor_name) == 0))
      {
         return false;
      }
      return true;
   }

   function requiresPermission()
   {
      //check the various permissions that don't require permission (clip) and promote
      if($this->item_action == 'clip' or $this->item_action == 'multiclip' or $this->item_action == 'unclip' or $this->item_action == 'release' or $this->item_action == 'promote' or $this->item_action == 'demote')  return false;
      return true;
   }

   //determines if a particular action requires a confirm page
   function requiresConfirm()
   {
      //all actions currently require confirms
      return true;
   }

   function isConfirm()
   {
      if(isset($_REQUEST['confirm']))   return true;
      else return false;
   }

   function isCancel()
   {
      if(isset($_REQUEST['cancel']))   return true;
      else return false;
   }

   //function that actually loads the item to be acted upon
   function loadItem()
   {
      if($this->item_type=="story" || $this->item_type=="feature")
      {
         $this->item = new Story();
         $this->item->story_id=$this->item_id;
         $this->item->load();
      }
      else if($this->item_type=="comment")
      {
         $this->item = new Comment();
         $this->item->comment_id=$this->item_id;
         $this->item->load();
      }
      else if($this->item_type=="attachment")
      {
         if($this->item_action=="create")
         {
            $this->item = new Attachment();
            $this->item->attachment_file=$this->item_id;
            $this->item->loadByFileName();
         }
         else
         {
            $this->item = new Attachment();
            $this->item->attachment_id=$this->item_id;
            $this->item->load();
         }
      }
   }

   function logAction($item_id_override ="")
   {
      // The override allows one to have two ids for comments hides. It will be: story id, comment id
      if ($item_id_override == "") logAction($this->item, $this->item_id, $this->item_type, $this->item_action);
      else logAction($this->item, $item_id_override, $this->item_type, $this->item_action);
   }

   function execute(&$query)
   {
      global $system_config;
      $item_ids = explode(",", $this->item_id);
      if(substr($this->item_action, 0, 5) == "multi")
      {
         $this->item_action= substr($this->item_action, 5, strlen($this->item_action)-5);
      }
      $execute_result = false;
      foreach($item_ids as $item_id)
      {
         $this->item_id = $item_id;
         $this->loadItem();
	 // echo("Calling ExecuteOne for itemId = " .$this->item_id . "attch id " . $this->item->attachment_id ."<BR>");
         $execute_result = $this->executeOne($query);
      }
      if($this->requiresNotify() or (isset($_REQUEST['send_mail']) and $_REQUEST['send_mail'] == 'on'))
      {
         $action = $this->item_action;
         $message = "";
         if(count($item_ids) == 1)
         {
            $subject_str = $this->item_action." ".$this->item_type." ".$this->item_id;
         }
         else
         {
            // Most multiple hides are of comments rather than stories. So in the subject give the story id
            // and not the comment id of the first comment hidden which is probably confusing.
            if($this->item_type == 'comment')
	    {
                $subject_str = "Multiple ".$this->item_action." ".$this->item_type." [".count($item_ids) . " from ".$this->item->story_id."]";
            }
	    else
            {
                $subject_str = "Multiple ".$this->item_action." ".$this->item_type." [".count($item_ids) . " from ".$item_ids[0]."]";
            }
         }

         if($action == 'delete' or $action == 'downgrade'or ($action == 'upgrade' && $this->item_type == 'comment'))
         {
            for($i = 0; $i<count($this->saved_items); $i++)
            {
               $this->saved_items[$i]->stripslashes();
               if($this->item_type=="story" || $this->item_type=="feature") $message .= $this->saved_items[$i]->toString()."\r\n";
               else $message .= $this->saved_items[$i]->toString(true)."\r\n";
               $message .= "----------- End of $this->item_type $item_id Content-----------\r\n";
               $message .= "\r\n\r\n";
            }
         }
         else
         {
            $item_counter = 1;
            $total_items = count($item_ids);
            $index_info  = "";
            $hide_info  = "";
            foreach($item_ids as $item_id)
            {
               if(isset($this->mapped_item_ids[$item_id])) $this->item_id = $this->mapped_item_ids[$item_id];
               else $this->item_id = $item_id;
               $this->loadItem();
               $this->item->stripslashes();
               if($this->item_type=="story" || $this->item_type=="feature") $message .= $this->item->toString()."\r\n";
               else $message .= $this->item->toString(true)."\r\n";
               $message .= "----------- End of $this->item_type $item_id Content-----------\r\n";
               $message .= "\r\n\r\n";

	       $id_override = "";
               if($this->item_action=="hide" ) {
                   $hide_info = "hide: ". date("Y-m-d-H:i",time()+$system_config->timezone_offset);
                   if($this->item_type=="story" || $this->item_type=="feature") {
                       $index_info = "hide story " . $this->item->story_id . " : " . $this->item->story_title . " by " . $this->item->author_name;
                       $hide_info .= " story: " . $this->item->story_id . " : " . $this->item->story_title . " reason: ";
                   }
                   if($this->item_type=="comment")
                   {
    		       // The item id will be the comment id, but we must go inside the comment obj to get story id
                       if ($total_items > 1 ) $index_info  = "multiple hide comment (refer to ".$this->item->story_id.") ";
                       else $index_info  = "hide comment ";
                       $index_info .= $this->item_id." of ". $this->item->story_id ." : ".$this->item->comment_title. " by " . $this->item->author_name;
                       $hide_info .= " comment: " . $this->item_id . " : " . $this->item->comment_title . " reason: ";
                   }
                   if ($index_info != "" ) {
                       //echo("INFO: " . $index_info . " <BR>");
                       logMessage($index_info, "notification_index.log", false);
		       if (strlen($_REQUEST['action_reason']) > 60) 
		       {
                           $hide_info .= substr($_REQUEST['action_reason'],0, 59) . "...";
                       }
		       else
		       {
                           $hide_info .= $_REQUEST['action_reason'];
                       }
                       logMessage($hide_info, "hides_list.log", false);
                   }
               }
               if($this->item_type=="comment")
               {
    	           $id_override = $this->item->story_id . ", " . $this->item_id;
               }

               if ($total_items > 1 ) {
                   // When it is multiple hides we want to say 1 of n in the reason. So save off reason.
                   // When coding this starting a string with a number caused problems. Thats why it is like this.
                   $save_reason = $_REQUEST['action_reason'];
                   $modified_reason = "$item_counter of $total_items " . $save_reason;
                   $_REQUEST['action_reason'] = $modified_reason;
	           $this->logAction($id_override);
                   $_REQUEST['action_reason'] = $save_reason;
                   $item_counter++;
               }
            }
         }
         $this->sendNotification($message, $subject_str);
         $this->flushNotificationIndex("notification_index.log");
      }

      // If there was only one item then we can write the entry here.
      if (count($item_ids) == 1 ) 
      {
          if($this->item_type=="comment")
          {
	      $id_override = $this->item->story_id . ", " . $this->item_id;
	      $this->logAction($id_override);
          } else if($this->item_action=="promote" || $this->item_action=="demote") {
	      // Do not log it if the vote was rejected because editor already voted.
              if ($execute_result == true) $this->logAction();
          } else if($this->item_action=="translate") {
	      // Do not log it if no change was made.
              if ($execute_result == true) $this->logAction();
          } else {
              $this->logAction();
          }
      }
   }

   function sendNotification(&$content, $subject_str)
   {
      global $oscailt_page_setting, $system_config,$editor_session;
      $mail_set = $oscailt_page_setting["site_mail_settings"];
      $to = $mail_set['to'];
      $from = $mail_set['from'];
      $reply = $mail_set['replyto'];
      $subj_prefix = $mail_set['subject_prefix'];
      $subject = $subj_prefix." ".$subject_str." performed by ".$editor_session->editor->editor_name;
      $message = $system_config->software_name." ".$system_config->software_version." Automatic Notification"."\r\n";
      $message .= "Date   : ".date("l, M j Y, g:ia",time()+$system_config->timezone_offset)."\r\n";
      $message .= "Action   : ".$subject_str." performed by ".$editor_session->editor->editor_name."\r\n";
      if(!isset($_REQUEST['action_reason']) || trim($_REQUEST['action_reason']) == "") $reason = "<not specified>";
      else $reason = $_REQUEST['action_reason'];
      $message .= "Reason   : ".$reason."\r\n";
      $message .= "\r\n";
      $message .= $content."\r\n";
      if( $system_config->debug_level >= DEBUG_LEVEL_1 )
      {
         echo("<BR>To: $to<BR>");
         echo("<BR>From: $from<BR>");
         echo("<BR>Replyto: $reply<BR>");
         echo("<BR>Subject: $subject<BR>");
         echo("<BR><pre>".htmlspecialchars($message)."</pre><BR>");
      }
      else
      {
         mail($to, $subject, $message, "From: ".$from."\r\n"."Reply-To: ".$editor_session->editor->editor_email.",".$reply."\r\n"."X-Mailer: ".$system_config->software_name."/".$system_config->software_version." using PHP/".phpversion());
      }
   }


   function executeOne(&$query)
   {
      global $editor_session, $system_config;
      $priorHiddenStatus=$this->item->hidden;

      //logAction($this->item, $this->item_id, $this->item_type, $this->item_action);
      if($this->item_action=="clip")
      {
         $editor_session->clipboard->clip($this->item);
      }
      else if($this->item_action=="unclip")
      {
         $editor_session->clipboard->unclip($this->item);
      }
      else if($this->item_action=='release')
      {
         $this->releaseLock();
      }
      else if($this->item_action=="downgrade")
      {
         $editor_session->clipboard->unclip($this->item);
         if($this->item_type=="story" && $query->story_id!=null)
         {
            $this->saved_items[] = $this->item;
            $this->saved_items[count($this->saved_items) -1]->load();
            $comment=$this->item->downgradeToComment($query->story_id);
            $editor_session->clipboard->clip($comment);
         }
      }
      else if($this->item_action=="upgrade")
      {
         $editor_session->clipboard->unclip($this->item);
         if($this->item_type=="comment")
         {
            $this->saved_items[] = $this->item;
            $this->saved_items[count($this->saved_items) -1]->load();
            $story=$this->item->upgradeToStory();
            $editor_session->clipboard->clip($story);
            $query->comment_id=null;
         }
         else if($this->item_type=="story")
         {
            $this->item->makeTemporaryFeature();
            $query->story_id=$this->item->story_id;
            $this->item_id=$this->item->story_id;
         }
      }
      else if($this->item_action=="copy")
      {
         if($this->item_type=="comment" && $query->story_id!=null) $this->item->copyToStory($query->story_id);
         else if($this->item_type=="attachment" && $query->comment_id!=null) $this->item->copyToComment($query->comment_id);
         else if($this->item_type=="attachment" && $query->story_id!=null) $this->item->copyToStory($query->story_id);
      }
      else if($this->item_action=="copyandunhide") //used by feature link scanner not the clipboard
      {
         $this->item->hidden=false;
         if($this->item_type=="comment" && $query->story_id!=null) $this->item->copyToStory($query->story_id);
         else if($this->item_type=="attachment" && $query->comment_id!=null) $this->item->copyToComment($query->comment_id);
         else if($this->item_type=="attachment" && $query->story_id!=null) $this->item->copyToStory($query->story_id);
      }
      else if($this->item_action=="create") //used by feature link scanner not the clipboard
      {
         if($this->item_type=="attachment" && $query->story_id!=null)
         {
            if($this->item->fileExists())
            {
               $parentStory = new Story();
               $parentStory->story_id=$query->story_id;
               $parentStory->load();
               $this->item->story_id=$query->story_id;
               $this->item->time_posted=$parentStory->time_posted;
               $this->item->hidden=false;
               $this->item->determineTypeFromFileName();
               $this->item->save();
            }
         }
      }
      else if($this->item_action=="move")
      {
         $editor_session->clipboard->unclip($this->item); //take item off clipboard as its id might be changing
         if($this->item_type=="comment" && $query->story_id!=null) $this->item->moveToStory($query->story_id);
         else if($this->item_type=="attachment" && $query->comment_id!=null) $this->item->moveToComment($query->comment_id);
         else if($this->item_type=="attachment" && $query->story_id!=null) $this->item->moveToStory($query->story_id);
         $editor_session->clipboard->clip($this->item);
      }
      else if($this->item_action=="lock")
      {
         $this->item->lock();
      }
      else if($this->item_action=="unlock")
      {
         $this->item->unlock();
      }
      else if($this->item_action=="hide")
      {
         $this->item->hide();
      }
      else if($this->item_action=="unhide")
      {
         if($this->item_type=="feature" && isset($_REQUEST['merge_feature']) && $_REQUEST['merge_feature'] == 'on')
         {
            if(isset($_REQUEST['reset_feature_date']) && $_REQUEST['reset_feature_date'] == 'on') $this->item->time_posted = time();
            $this->item->mergeTemporaryFeatureIntoOriginal();
            $query->story_id=$this->item->story_id;
            $this->mapped_item_ids[$this->item_id] = $this->item->story_id;
            $this->item_id=$this->item->story_id;
         }
         else if($this->item_type=="feature" && $_REQUEST['reset_feature_date']!=null)
         {
            $this->item->time_posted = time();
            $this->item->addslashes();
            $this->item->save();
         }
         $this->item->unhide();
      }
      else if($this->item_action=="multihide")
      {
         $all_ids = explode(',', $this->item_id);
         foreach($all_ids as $one_id)
         {
            $this->item = new Comment();
            $this->item->comment_id=$one_id;
            $this->item->load();
            $this->item->hide();
         }
      }
      else if($this->item_action=="delete")
      {
         $this->saved_items[] = $this->item;
         $this->saved_items[count($this->saved_items) -1]->load();
         if($this->item_type=="story" || $this->item_type=="feature" || $this->item_type=="comment") $this->item->deleteAttachments();
         if($this->item_type=="story" || $this->item_type=="feature") $this->item->deleteComments();
         if($this->item_type=="attachment") $this->item->deleteFileIfSingleReference();
         $this->item->delete();
         if($this->item_type=="story" || $this->item_type=="feature") $query->story_id=null;
      }
      else if($this->item_action=="featurize")
      {
         if($this->item_type=="attachment" && $this->item->isImage())
         {
            $system_config->featured_image=$this->item->attachment_id;
            $system_config->save();
            $this->item->featurize();
         }
      }
      else if($this->item_action=="unfeaturize")
      {
         if($this->item_type=="attachment" && $this->item->isImage())
         {
            $system_config->featured_image=null;
            $system_config->save();
            $this->item->unfeaturize();
         }
      }
      else if($this->item_action=="stick" && $_REQUEST['sticky_days']>0)
      {
         $sticky_time = ($_REQUEST['sticky_days']*24*60*60)+time();
         if($this->item_type=="story" || $this->item_type=="feature") $this->item->stick($sticky_time);
      }
      else if($this->item_action=="unstick")
      {
         if($this->item_type=="story" || $this->item_type=="feature") $this->item->unstick();
      }
      else if($this->item_action=="edit")
      {
         if($this->item_type=="attachment")
         {
            $this->item->description=$_REQUEST[$this->item_type."_".$this->item->attachment_id."_description"];
            $this->item->addslashes();
            $this->item->save();
         }
      }
      else if($this->item_action=="promote")
      {
         // For this story id, inc the vote in the story table and check for a false result.
	 if ($this->item->promote() == false)
	 {
             //$this->writeUserMsg("You already voted for story ".$this->item_id."!");
             $this->writeUserMsg("You already voted for this story!");
             // $this->addUserMsg("Yoy already voted for this story jimmy");
             return false;
         }
      }
      else if($this->item_action=="demote")
      {
         // For this story id, dec the vote in the story table and check for a false result.
	 if ($this->item->promote(-1) == false)
	 {
             //$this->writeUserMsg("You already voted for story ".$this->item_id."!");
             $this->writeUserMsg("You already voted for this story!");
             return false;
         }
      }
      else if($this->item_action=="swap")
      {
         if($this->item_type=="attachment")
         {
            // Need to modify the setting of image order -that's all. item is an object of type attachment.
            // See that class for details of the save operation.
            $swap_target = $_REQUEST[$this->item_action.$this->item_type];

	    $d_msg = $this->item_type."_".$this->item->attachment_id."_with";
	    $d_msg .= "[" . $_REQUEST[$this->item_action."_with"] ."]";
	    $swap_dest = $_REQUEST[$this->item_action."_with"];

	    if ($swap_target == $swap_dest) {
                $this->writeUserMsg("Target and Dest attachment ids the same for swap operation. Aborting.");
                logMessage("Target and Dest attachment ids the same for swap operation. Aborting.","sitelog.txt");
	    } else {
	        $this->item->swap_order($swap_target,$swap_dest);
                // logAction($this->item, $this->item_id, $this->item_type, $d_msg );
            }
         }
      }
      else if($this->item_action=="translate")
      {
         $_REQUEST['action_reason'] = "";
         if($this->item_type=="story")
         {
            // Can only mark stories for translation. The idea is to link the story with another story id
            // which is the translated version.
	    $ret_code = false;
	    echo("This story id = ".  $this->item->story_id . " lang ". $this->item->language_id ."<BR>");
	    echo("select story id = ".  $_REQUEST['translated_story_id']. "<BR>");
	    global $prefix, $dbconn;
            if(isset($_REQUEST['translated_story_id']) && $_REQUEST['translated_story_id'] != '' && isset($_REQUEST['trans_add']) && $_REQUEST['trans_add'] == 'add')
            {
                // First check that the target translated story id actually exists.
                $check_id = sql_query("SELECT 1 FROM ".$prefix."_stories WHERE story_id =".$_REQUEST['translated_story_id'], $dbconn, 2);
                checkForError($check_id);
		if(sql_num_rows($check_id) > 0 ) {
                   checkForError(sql_query("INSERT INTO ".$prefix."_content_translations values (null,'story',".$this->item->story_id.",".$_REQUEST['translated_story_id'].")", $dbconn, 2));
                   $t_insert_id = mysql_insert_id($dbconn);
                   $_REQUEST['action_reason'] = "Linked to story_id ".$_REQUEST['translated_story_id'];
	           $ret_code = true;
		}
		else
		{
                    $this->writeUserMsg("Target translated story id ".$_REQUEST['translated_story_id']. " does not exist. Cannot do update!");
                    return false;
                }
            }
	    // Now get and do the other updates from the form.
	    for ($iField=1; $iField < 10; $iField++)
            {
                if(isset($_REQUEST['trans_id_'.$iField]) && $_REQUEST['trans_id_'.$iField] != '') {
                    echo("iField ".$iField. " Trans Id:".$_REQUEST['trans_id_'.$iField] ."<BR>");
                    $trans_id = $_REQUEST['trans_id_'.$iField];
                    echo("Action: ".$_REQUEST['trans_'.$trans_id] ."<BR>");
                    if(isset($_REQUEST['trans_'.$trans_id]) && $_REQUEST['trans_'.$trans_id] == 'update') 
		    {
                        if(isset($_REQUEST['trans_story_id_'.$iField]) && $_REQUEST['trans_story_id_'.$iField] != '')
		       	{
                             // Confirm the updated story id exists.
                             $check_id = sql_query("SELECT 1 FROM ".$prefix."_stories WHERE story_id =".$_REQUEST['trans_story_id_'.$iField], $dbconn, 2);
                             checkForError($check_id);
			     if(sql_num_rows($check_id) > 0 ) {
                                 checkForError(sql_query("UPDATE ".$prefix."_content_translations SET translated_content_id =".$_REQUEST['trans_story_id_'.$iField]. " WHERE translated_content_id=".$trans_id, $dbconn, 2));
                                 $_REQUEST['action_reason'] .= " Updated story link ".$trans_id. " to ".$_REQUEST['trans_story_id_'.$iField];
	                         $ret_code = true;
		             }
		       	}
                    }
		    elseif(isset($_REQUEST['trans_'.$trans_id]) && $_REQUEST['trans_'.$trans_id] == 'remove') 
		    {
                        // Confirm the story id exists.
                        $check_id = sql_query("SELECT 1 FROM ".$prefix."_content_translations WHERE translated_content_id =".$trans_id, $dbconn, 2);
                        checkForError($check_id);
			if(sql_num_rows($check_id) > 0 ) {
                            checkForError(sql_query("DELETE FROM ".$prefix."_content_translations WHERE translated_content_id=".$trans_id, $dbconn, 2));
                            $_REQUEST['action_reason'] .= " Removed story link to ".$trans_id;
	                    $ret_code = true;
		       	}
                    }
                }
            }
	    return $ret_code;
         }
      }
      return true;
   }

   function writeUserMsg($msg)
   {
      echo "<div class='user-message'>\n";
      echo "<p>$msg</p>\n";
      echo "</div>\n<br>";
   }

   function releaseLock()
   {
      require_once('objects/editlock.inc');
      $lock = new EditLock();
      if($this->item_type == 'comment')
      {
         $id = $this->item->comment_id;
      }
      elseif($this->item_type == 'story')
      {
         $id = $this->item->story_id;
      }
      $lock->lockItem($this->item_type, $id);
      $lock->delete();
      $lock->cleanUp();
   }

   function notify()
   {
      global $system_config,$editor_session;
      $item->stripslashes();
      $subject = $item_action." ".$item_type." ".$item_id." performed by ".$editor_session->editor->editor_name;
      $message = $message.$system_config->software_name." ".$system_config->software_version." Automatic Notification"."\r\n";
      $message = $message."Date   : ".date("l, M j Y, g:ia",time()+$system_config->timezone_offset)."\r\n";
      $message = $message."Action   : ".$item_action." ".$item_type." ".$item_id." performed by ".$editor_session->editor->editor_name."\r\n";
      if($reason==null || strlen($reason)==0) $reason = "<not specified>";
      $message = $message."Reason   : ".$reason."\r\n";
      $message = $message."\r\n";
      if($item_type=="story" || $item_type=="feature") $message = $message.$item->toString()."\r\n";
      else if($item_type=="comment") $message = $message.$item->toString(true)."\r\n";
      else if($item_type=="attachment") $message = $message.$item->toString(true)."\r\n";
      mail($system_config->notification_to_email_address, $subject, $message, "From: ".$system_config->notification_from_email_address."\r\n"."Reply-To: ".$editor_session->editor->editor_email.",".$system_config->notification_replyto_email_address."\r\n"."X-Mailer: ".$system_config->software_name."/".$system_config->software_version." using PHP/".phpversion());
   }

   function addToApprovalQueue()
   {
      $item_ids = explode(",", $this->item_id);
      $action = $this->item_action;
      $message = "";
      $subject_str = "Propose Update ";
      if(count($item_ids) == 1)
      {
         $subject_str .= $this->item_action." ".$this->item_type." ".$this->item_id;
      }
      else
      {
         $subject_str .= "Multiple ".$this->item_action." ".$this->item_type." [".count($item_ids) . " from ".$item_ids[0]."]";
      }
      foreach($item_ids as $item_id)
      {
         $this->item_id = $item_id;
         $this->loadItem();
         $this->item->stripslashes();
         if($this->item_type=="story" || $this->item_type=="feature") $message .= $this->item->toString()."\r\n";
         else $message .= $this->item->toString(true)."\r\n";
         $message .= "----------- End of $this->item_type $item_id Content-----------\r\n";
         $message .= "\r\n\r\n";
      }
      $this->sendNotification($message, $subject_str);
   }

   function securityAlert($isloggedIn)
   {
      if($isloggedIn && $this->logEditorHacks)
      {
         $msg = 'Editor attempt to hack '.
            $this->item_action.' '.$this->item_type.' '.$this->item_id.' ('.$this->action_type.')';
      }
      elseif($this->logActionHacks)
      {
         $msg = 'User attempt to hack '.
            $this->item_action.' '.$this->item_type.' '.$this->item_id.' ('.$this->action_type.')';
      }
      logSecurityMessage($msg);
   }

   function getActionNotPermittedMsg()
   {
      return 'No Permission for'.
            $this->item_action.' '.$this->item_type.' '.$this->item_id.' ('.$this->action_type.')';
   }

   function getNoEditorSessionError()
   {
      return 'No Editor Permission for'.
            $this->item_action.' '.$this->item_type.' '.$this->item_id.' ('.$this->action_type.')';
   }

   function isPermitted()
   {
      global $editor_session, $oscailt_page_setting;
      if($editor_session->isSessionOpen())
      {
         if($editor_session->editor->allowedReadAccessTo($this->item_action.$this->item_type, $oscailt_page_setting['site_id']))
         {
            return true;
         }
         elseif($this->item_action == 'edit' and $editor_session->editor->allowedReadAccessTo("editowncontent", $oscailt_page_setting['site_id']) && (($this->item_type == 'story' or $this->item_type == 'comment') && strcasecmp($this->item->author_name, $editor_session->editor->editor_name) == 0))
         {
            return true;
         }
         else
         {
            return false;
         }
      }
      else
      {
         $this->securityAlert(false);
         $this->setUserMsg($this->getNoEditorSessionError());
      }
      return false;
   }

   function getConfirmMessage()
   {
      global $graphics_store;
      $this->loadActionData();
      $icon = $this->actionData['icon'];
      $alt = $this->actionData['description'];
      $extra = isset($this->actionData['extra']) ? $this->actionData['extra']: "";
      $str = "<p class='confirmheader'>$alt</p>\n";
      $str .= '<div align=center class="confirmbox">';
      $str .= "<p class='confirmbox'>";
      $str.='<img src="'.$graphics_store.'caution.gif" align=center><br>';
      $str .= "<img class='actionicon' alt='".$alt."' src='$icon' border=0>"." Are you sure you wish to ". $this->item_action.' '. $this->item_type.' '.$this->item_id;
      $str .= "?</P>";
      if($extra != "")
      {
         $str .= "<P class='confirm-extra'>$extra</P>";
      }
      $str .= "</div>\n";
      return $str;
   }

   // This is used for flushing the entries in the index_notification.log file to the notification email
   // list. It generates a regular index file to match the comment and story ids for hides against their titles.
   // Send if the time is due, send the full file and then empty it again.
   function flushNotificationIndex($filename)
   {
      global $oscailt_page_setting, $system_config, $path_prefix;

      //$tmp_msg = "time: " . time() . " last gen ". $system_config->lastindex_generated ;
      // logMessage($tmp_msg,"sitelog2.txt");
      // Just noticed if there is a delay and time goes over then the new timestamp starts
      // later, so it will not be every 24hrs and it will then to drift forward.

      // If it is less than 70 secs since last generation, then don't bother
      // 1 day = 86400 secs. 12hrs = 43200
      $the_time = time();
      if ( ($the_time - $system_config->lastindex_generated ) < 86400 ) return;

      // Gets the remainder but accounts for if say 2 days out..
      $drift_correction = ($the_time - $system_config->lastindex_generated ) % 86400;

      $system_config->lastindex_generated = ($the_time - $drift_correction); 
  
      $system_config->save();

      $mail_set = $oscailt_page_setting["site_mail_settings"];
      $to = $mail_set['to'];
      $from = $mail_set['from'];
      $reply = $mail_set['replyto'];
      $subj_prefix = $mail_set['subject_prefix'];
      $subject = "Mappings of titles to Ids of hidden stories and comments. ";
      $subject .= date("D, M j Y, g:ia",time()+$system_config->timezone_offset);

   
      // Now read the contents of the file.
      $file_path = $path_prefix.$system_config->log_store.$filename;
      if(file_exists($file_path))
      {
         $fcontents = file($file_path);
         $message = "Mappings of titles to ids of hidden stories and comments "."\r\n";
         $message .= "Comments the second id is of the parent story."."\r\n";
         $message .= "\r\n";
         foreach ($fcontents as $each_line ) {
             // $message .= $each_line ."\r\n";
             $message .= $each_line ;
         }

         mail($to, $subject, $message, "From: ".$from."\r\n"."X-Mailer: ".$system_config->software_name."/".$system_config->software_version." using PHP/".phpversion());

	 //echo("MAIL: Subj: " .$subject ."<BR>Msg: " .$message ."<BR>");
         // Now that the file has been read and sent, empty it.
         $fp = fopen($file_path,"w");
         if (flock($fp, LOCK_EX))
         {
             fputs($fp, '');
             // unlocking file
             flock($fp, LOCK_UN);
             fclose($fp);
         } else {
	     echo "<P>failed to get lock on notification index file ". $file_path. "</p>";
         }
      } else {
          logMessage("Could not find " .$file_path ." during file flush for index generation ","sitelog.txt");
      }
   }
}
?>
