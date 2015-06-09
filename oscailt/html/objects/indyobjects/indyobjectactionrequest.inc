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

require_once "objects/editlock.inc";

/************************************************************************/
/* Class to represent a management request about an object              */
/************************************************************************/
class indyObjectActionRequest extends indyObject
{
   var $edit_entry_url;
   var $site_id;
   var $target_objects;
   var $obj_type;
   var $obj_id;
   var $action;
   var $obj_language;
   var $button;
   var $selflink;
   var $valid_actions = array("edit", "view", "xmlview", "translate", "list", "create", "delete", "recache", "import", "export", "site-recache", "site-files");

   //notification policy stuff
   var $low_level_notifications = array("delete", "site-recache", "import");
   var $medium_level_notifications = array("recache", "edit", "create");
   var $high_level_notifications = array("translate");

   function copy()
   {
      $other = new indyObjectActionRequest($this->selflink);
      $other->edit_entry_url = $this->edit_entry_url;
      $other->site_id = $this->site_id;
      $other->target_objects = $this->target_objects;
      $other->obj_type = $this->obj_type;
      $other->obj_id = $this->obj_id;
      $other->action = $this->action;
      $other->obj_language = $this->obj_language;
      $other->button = $this->button;
      return $other;
   }


   function indyObjectActionRequest($url=false)
   {
      global $OSCAILT_SCRIPT;
      if($url === false) $url = $OSCAILT_SCRIPT;
      $this->selflink = $url;
   }

   function isIndividualObjectUpdateAction()
   {
      if(isset($this->action) && ($this->action == 'delete' or $this->action == 'edit' or $this->action == 'translate' or $this->action == 'recache' or $this->action == 'export'))
      {
         return true;
      }
      return false;
   }

   function isUpdateAction()
   {
         if(isset($this->action) && ($this->action == 'delete' or $this->action == 'create' or $this->action == 'edit' or $this->action == 'translate' or $this->action == 'recache' or $this->action == 'export' or $this->action == 'site-export' or $this->action == 'site-recache' or $this->action == 'import'))
         {
            return true;
         }
         return false;
   }

   function checkPermissions($type)
   {
      global $editor_session;
      if(($type=='read' && $editor_session->editor->allowedReadAccessTo('editdataobjects', 'admin'))
         or ($type=='write' && $editor_session->editor->allowedWriteAccessTo('editdataobjects', 'admin')))
      {
         return true;
      }
      if($this->getAction() == false)
      {
         if($editor_session->editor->containsRoleForSite($this->getSiteId()))
         {
            return true;
         }
         return false;
      }
      if(strtolower($this->getObjectType()) == 'site')
      {
         if($this->getAction() == 'create')
         {
            if(($type=='read' && $editor_session->editor->allowedReadAccessTo('createsiteobjects', 'admin')) or ($type=='write' && $editor_session->editor->allowedWriteAccessTo('createsiteobjects', 'admin')))
            {
               return true;
            }
            return false;
         }
         elseif($this->action == 'delete')
         {
            if(($type=='read' && $editor_session->editor->allowedReadAccessTo('deletesiteobjects', 'admin')) or ($type=='write' && $editor_session->editor->allowedWriteAccessTo('deletesiteobjects', 'admin')))
            {
               return true;
            }
            return false;
         }
         else
         {
            if(($type=='read' && $editor_session->editor->allowedReadAccessTo('editsiteobjects', 'admin')) or ($type=='write' && $editor_session->editor->allowedWriteAccessTo('editsiteobjects', 'admin')))
            {
               return true;
            }
            return false;
         }

      }
      elseif($this->action == 'delete' or $this->action=='create')
      {
         if(($type == 'write' && $editor_session->editor->allowedWriteAccessTo($this->action.'obj'.$this->getObjectType(), $this->getSiteId())) or ($type=='read' && $editor_session->editor->allowedReadAccessTo($this->action.'obj'.$this->getObjectType(), $this->getSiteId())))
         {
            return true;
         }
         return false;

      }
      elseif($this->action == 'recache' or $this->action=='site-recache')
      {
         if(($type == 'write' && $editor_session->editor->allowedWriteAccessTo('publishsite', $this->getSiteId())) or ($type=='read' && $editor_session->editor->allowedReadAccessTo('publishsite', $this->getSiteId())))
         {
            return true;
         }
         return false;
      }
      elseif($this->action == 'list' or $this->action == 'view' or $this->action == 'xmlview')
      {
         if($editor_session->editor->allowedReadAccessTo("viewprotected", $this->getSiteId()))
         {
            return true;
         }
         return false;
      }
      elseif($this->action == 'import')
      {
         if(($type=='read' && $editor_session->editor->allowedReadAccessTo('importdataobjects', 'admin')) or ($type=='write' && $editor_session->editor->allowedWriteAccessTo('importdataobjects', 'admin')))
         {
            return true;
         }
      }
      elseif($this->action == 'export')
      {
         if(($type == 'write' && $editor_session->editor->allowedWriteAccessTo('exportdataobjects', 'admin')) or ($type=='read' && $editor_session->editor->allowedReadAccessTo('exportdataobjects', 'admin')))
         {
            return true;
         }
         return false;
      }
      elseif($this->action == 'site-export')
      {
         if(($type == 'write' && $editor_session->editor->allowedWriteAccessTo('exportsite', $this->getSiteId())) or ($type=='read' && $editor_session->editor->allowedReadAccessTo('exportsite', $this->getSiteId())))
         {
            return true;
         }
         return false;
      }
      elseif($this->action == 'site-files')
      {
         if($editor_session->editor->allowedWriteAccessTo('editsitefiles', $this->getSiteId()))
         {
            return true;
         }
         return false;
      }
      elseif($this->action == 'translate' or $this->action=='edit')
      {
         if(($type == 'write' && $editor_session->editor->allowedWriteAccessTo('edit'.'obj'.$this->getObjectType(), $this->getSiteId())) or ($type=='read' && $editor_session->editor->allowedReadAccessTo('edit'.'obj'.$this->getObjectType(), $this->getSiteId())))
         {
            return true;
         }
         return false;
      }
      return false;
   }

   function requiresLock()
   {
      if($this->action == "edit" or $this->action == "delete" or $this->action == "translate") return true;
      else return false;
   }

   function getRequiredLocks()
   {
      if(!$this->requiresLock()) return true;
      $lock = new EditLock();
      $id = $this->getTargetObject();
      if(!$id) return true;
      $type = 'indyobject';
      if($lock->lockItem($type, $id))
      {
         return true;
      }
      elseif($lock->lock_id == 0)
      {
         $this->setUserMessage(Array( USER_ERROR, $lock->result_message, "lock" ) );
         return false;
      }
      else
      {
         $result = $lock->isUsable();
         $this->setUserMessage(Array( USER_WARNING, $lock->result_message, "lock" ) );
         return $result;
      }
   }

   function checkLocks()
   {
      if(!$this->requiresLock()) return true;
      $lock = new EditLock();
      $id = $this->getTargetObject();
      if(!$id) return true;
      $type = 'indyobject';
      $result = $lock->canUpdate($type, $id);
      if(isset($lock->result_message))
      {
         $this->setUserMessage(Array( USER_WARNING, $lock->result_message, "lock" ) );
      }
      return $result;
   }

   function relinquishLocks()
   {
      if(!$this->requiresLock()) return true;
      $lock = new EditLock();
      $id = $this->getTargetObject();
      if(!$id) return false;
      $type = 'indyobject';
      $lock->lockItem($type, $id);
      $lock->delete();
      $lock->cleanUp();
   }

   function getQueryString()
   {
      $fields = array();
      $q_str = "";
      if(isset($this->obj_id))
      {
         $fields[] = "obj_id=" . $this->obj_id;
      }
      if(isset($this->action))
      {
         $fields[] = "action=" . $this->action;
      }
      if(isset($this->site_id))
      {
         $fields[] = "site_id=" . implode(",", $this->site_id);
      }
      if(isset($this->target_objects))
      {
         $fields[] = "target_id=" . implode(",", $this->target_objects);
      }
      if(isset($this->obj_type))
      {
         $fields[] = "obj_type=" . implode(",", $this->obj_type);
      }
      if(isset($this->obj_language))
      {
         $fields[] = "obj_language=" . implode(",", $this->obj_language);
      }
      if(isset($this->edit_entry_url))
      {
         $fields[] = "edit_entry_url=" . $this->edit_entry_url;
      }
      if(count($fields) > 0)
      {
         $q_str = implode("&", $fields);
      }
      if(strstr($this->selflink, '?'))
      {
         $q_str = '&'.$q_str;
      }
      else
      {
         $q_str = '?'.$q_str;
      }
      return $q_str;
   }

   function getObjPage()
   {
      $str = $this->getPage();
      if(isset($this->obj_id))
      {
         $str .= '?obj_id='.$this->obj_id;
      }
      return $str;
   }

   function getPage()
   {
      return $this->selflink;
   }

   function getLink()
   {
      return $this->selflink.$this->getQueryString();
   }

   function getEntryURL()
   {
      if(isset($this->edit_entry_url))
      {
         return $this->edit_entry_url;
      }
      return "";
   }

   function getAsEntryURL()
   {
      //format[id][action][language][type][target]

      if(isset($this->obj_id)) $str = "[$this->obj_id]";
      else $str = "[]";
      if(isset($this->action)) $str .= "[$this->action]";
      else $str .= "[]";
      if(isset($this->site_id)) $str .= "[".implode(",", $this->site_id)."]";
      else $str .= "[]";
      if(isset($this->obj_language)) $str .= "[".implode(",", $this->obj_language)."]";
      else $str .= "[]";
      if(isset($this->obj_type)) $str .= "[". implode(",", $this->obj_type) ."]";
      else $str .= "[]";
      if(isset($this->target_objects)) $str .= "[". implode(",", $this->target_objects) ."]";
      else $str .= "[]";
      return $str;
   }

   function setEntryURL($url)
   {
      $this->edit_entry_url = $url;
   }



   function populateFromEntry()
   {
      $entries = array();
      if(!isset($this->edit_entry_url) or $this->edit_entry_url == "")
      {
         unset($this->obj_id);
         unset($this->action);
         unset($this->site_id);
         unset($this->obj_language);
         unset($this->obj_type);
         unset($this->target_objects);
         return;
      }
      preg_match_all('/\[([\w\*]*)\]/', $this->edit_entry_url, $entries);
      if($entries[1][0] === "") unset($this->obj_id);
      else $this->obj_id = $entries[1][0];
      if($entries[1][1] === "") unset($this->action);
      else $this->action = $entries[1][1];
      if($entries[1][2] === "") unset($this->site_id);
      else $this->site_id = explode(",", $entries[1][2]);
      if($entries[1][3] === "") unset($this->obj_language);
      else $this->obj_language = explode(",", $entries[1][3]);
      if($entries[1][4] === "") unset($this->obj_type);
      else $this->obj_type = explode(",", $entries[1][4]);
      if($entries[1][5] === "") unset($this->target_objects);
      else $this->target_objects = explode(",", $entries[1][5]);
      unset($this->edit_entry_url);
   }

   function getHiddenQuery()
   {
      $str = "";
      if(isset($this->obj_id))
      {
          $str .= "<input type='hidden' name='obj_id' value='" . $this->obj_id."' />\n";
      }
      if(isset($this->action))
      {
          $str .= "<input type='hidden' name='action' value='" . $this->action."' />\n";
      }
      if(isset($this->site_id))
      {
          $str .= "<input type='hidden' name='site_id' value='" . implode(",", $this->site_id)."' />\n";
      }
      if(isset($this->target_objects))
      {
         $str .= "<input type='hidden' name='target_id' value='" . implode(",", $this->target_objects)."' />\n";
      }
      if(isset($this->obj_type))
      {
          $str .= "<input type='hidden' name='obj_type' value='" . implode(",", $this->obj_type)."' />\n";
      }
      if(isset($this->obj_language))
      {
         $str .= "<input type='hidden' name='obj_language' value='" . implode(",", $this->obj_language)."' />\n";
      }
      if(isset($this->edit_entry_url))
      {
          $str .= "<input type='hidden' name='edit_entry_url' value='" . $this->edit_entry_url."' />\n";
      }
      return $str;
   }

   function readRequest()
   {
      if(isset($_REQUEST['site_id']))
      {
         $this->site_id = explode(",", $_REQUEST['site_id']);
      }
      if(isset($_REQUEST['action']))
      {
         $this->action = $_REQUEST['action'];
      }
      else
      {
         foreach($this->valid_actions as $va)
         {
            if(isset($_REQUEST[$va.'_x']))
            {
               $this->action = $va;
               break;
            }

         }
      }
      if(isset($_REQUEST['obj_id']))
      {
         $this->obj_id = $_REQUEST['obj_id'];
      }
      if(isset($_REQUEST['target_id']))
      {
         $this->target_objects = explode(",", $_REQUEST['target_id']);
         $t_o = $this->getTargetObject();
         if(($t_o === false or $t_o == 0) && $this->isIndividualObjectUpdateAction())
         {
            $this->action = 'list';
         }
      }
      if(isset($_REQUEST['obj_type']))
      {
         $this->obj_type = explode(",", $_REQUEST['obj_type']);
      }
      if(isset($_REQUEST['obj_language']))
      {
         $this->obj_language = explode(",", $_REQUEST['obj_language']);
      }
      else
      {
         global $userLanguage;
         $this->obj_language = array($userLanguage->getLanguageCodePrefix());
      }
      if(isset($_REQUEST['edit_entry_url']))
      {
          $this->edit_entry_url = $_REQUEST['edit_entry_url'];
      }

      if(isset($_REQUEST['preview']))
      {
         $this->button = 'preview';
      }
      elseif(isset($_REQUEST['save']))
      {
         $this->button = 'confirm';
      }
      elseif(isset($_REQUEST['confirm']))
      {
         $this->button = 'confirm';
      }
      elseif(isset($_REQUEST['cancel']))
      {
         $this->button = 'cancel';
      }
   }

   function setTargetObjects($obj_ids)
   {
      $this->target_objects = array();

      foreach($obj_ids as $obj_id)
      {
         $this->target_objects[] = $obj_id;
      }
   }

   function getTargetObjects()
   {
      if(isset($this->target_objects))
      {
         return $this->target_objects;
      }
      return false;
   }

   function getTargetObject()
   {
      if(isset($this->target_objects))
      {
         if(is_array($this->target_objects))
         {
            return $this->target_objects[0];
         }
         return $this->target_objects;
      }
      return false;
   }

   function getSiteId()
   {
      if(isset($this->site_id))
      {
         if(is_array($this->site_id))
         {
            return $this->site_id[0];
         }
         return $this->site_id;
      }
      return 0;
   }

   function getSiteIds()
   {
      if(isset($this->site_id))
      {
         return $this->site_id;
      }
      return false;

   }



   function getObjectType()
   {
      if(isset($this->obj_type))
      {
         if(is_array($this->obj_type))
         {
            return $this->obj_type[0];
         }
         return $this->obj_type;
      }
      return false;
   }

   function getObjectTypes()
   {
      if(isset($this->obj_type))
      {
         return $this->obj_type;
      }
      return false;
   }

   function getObjectLanguage()
   {
      if(isset($this->obj_language))
      {
         if(is_array($this->obj_language))
         {
            return $this->obj_language[0];
         }
         return $this->obj_language;
      }
      return false;
   }

   function getObjectLanguages()
   {
      if(isset($this->obj_language))
      {
         return $this->obj_language;
      }
      return false;
   }


   function getFetchObjectParameters()
   {
      $obj_id = $this->getTargetObject();
      if($obj_id === false)
      {
         $this->setUserMessage( array( INDY_ERROR,                        "User has not selected a type of object to create","byw" ) );
         return false;
      }

      $obj_language = $this->getObjectLanguage();
      if($obj_language === false)
      {
         $this->setUserMessage( array( INDY_ERROR,                        "Not specified: language in which to fetch this object","1sa2" ) );
         return false;
      }
      return array($obj_id, $obj_language);
   }


   function getNewObjectParameters()
   {
      $site_id = $this->getSiteId();
      if($site_id === false)
      {
         $this->setUserMessage( array( INDY_ERROR,                        "User has not specified a site in which to create this object","zla2" ) );
         return false;
      }

      $obj_type = $this->getObjectType();
      if($obj_type === false)
      {
         $this->setUserMessage( array( INDY_ERROR,                        "User has not selected a type of object to create","bwa2" ) );
         return false;
      }

      $obj_language = $this->getObjectLanguage();
      if($obj_language === false)
      {
         $this->setUserMessage( array( INDY_ERROR,                        "User has not specified a language in which to create this object","bsa2" ) );
         return false;
      }

      return array($site_id, $obj_type, $obj_language);
   }

   function requiresApproval()
   {
      global $editor_session;
   }


   function isCancel()
   {
      if(isset($this->button) && ($this->button == 'cancel'))
      {
         return true;
      }
      return false;
   }

   function isCommitUpdateAction()
   {
      if(isset($this->button) && $this->button == 'confirm' && $this->isUpdateAction())
      {
         return true;
      }
      return false;
   }

   function isUnconfirmedUpdateAction()
   {
      if($this->action=='delete' or (isset($this->button) && ($this->button == 'preview' or $this->button == 'save') && $this->isUpdateAction()))
      {
         return true;
      }
      return false;
   }

   function getString()
   {
      $str = $this->action;
      if($this->getObjectType() !== false)
      {
         $str .= " ".$this->getObjectType();
      }
      if($this->getTargetObject() !== false)
      {
         $str .= " ".$this->getTargetObject()." [".$this->getObjectLanguage() . "] ";
      }
      $str .= " Site:".$this->getSiteId();
      return $str;

   }

   function getAction()
   {
      if(isset($this->action) && in_array($this->action, $this->valid_actions))
      {
         return $this->action;
      }
      else return false;
   }

   function getSiteFilters()
   {
      if(isset($this->site_id))
      {
         return $this->site_id;
      }
      return array("*");
   }

   function getSiteFilter()
   {
      if(isset($this->site_id))
      {
         if(is_array($this->site_id))
         {
            return $this->site_id[0];
         }
         return $this->site_id;
      }
      return array("*");
   }

   function getTypeFilters()
   {
      if(isset($this->obj_type))
      {
         return $this->obj_type;
      }
      return array("*");
   }

   function sendNotification($to, $from, $reply, $msg_body, $subj_prefix)
   {
      global $system_config,$editor_session;

      if(isset($_REQUEST['action_reason']) && trim($_REQUEST['action_reason']) != "")
      {
         $reason = trim($_REQUEST['action_reason']);
      }
      else
      {
         $reason = "<not specified>";
      }
      $subject = $subj_prefix." ".$this->getString()." performed by ".$editor_session->editor->editor_name;
      $message = $system_config->software_name." ".$system_config->software_version." Automatic Notification"."\r\n";
      $message = $message."Date   : ".strftime($system_config->default_strftime_format,time()+$system_config->timezone_offset)."\r\n";
      $message = $message."Action   : ".$this->getString()." performed by ".$editor_session->editor->editor_name."\r\n";
      $message = $message."Reason   : ".$reason."\r\n";
      $message .= $msg_body;
      $message = $message."\r\n";
      if( $system_config->debug_level >= DEBUG_LEVEL_1 )
      {
         echo("<BR>To: $to<BR>");
         echo("<BR>From: $from<BR>");
         echo("<BR>Replyto: $reply<BR>");
         echo("<BR>Subject: $subject<BR>");
         echo("<BR><pre>".htmlspecialchars($message)."</pre><BR>");
      }
      mail($to, $subject, $message, "From: ".$from."\r\n"."Reply-To: ".$editor_session->editor->editor_email.",".$reply."\r\n"."X-Mailer: ".$system_config->software_name."/".$system_config->software_version." using PHP/".phpversion());
   }

   function requiresNotify($lvl)
   {
      $lvl = strtolower($lvl);
      if($lvl == 'none' or !$this->isUpdateAction()) return false;
      if(in_array(strtolower($this->action), $this->low_level_notifications))
      {
         return true;
      }
      if($lvl == 'low') return false;
      if(in_array(strtolower($this->action), $this->medium_level_notifications))
      {
         return true;
      }
      if($lvl == 'medium') return false;
      if(in_array(strtolower($this->action), $this->high_level_notifications))
      {
         return true;
      }
      if($lvl == 'high') return false;
      return true;
   }


   function logAction($result)
   {
      $item_type = $this->getObjectType();
      $action = $this->getAction();
      $obj_lang = $this->getObjectLanguage();
      $obj_id = $this->getTargetObject();
      if($item_type === false) $item_type = 'unknown type';
      if($action === false) $item_type = 'unknown action';
      if($obj_id === false) $obj_id = 'unknown target id';
      if($obj_lang === false) $obj_lang = 'unknown';
      $item_type = "data object $item_type [$obj_lang]";
      logAction(null, $obj_id, $item_type, $action);
   }

}


?>