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
include_once("objects/attachment.inc");
$oscailt_page_setting = array();
/************************************************************************/
/* This class Manages the displaying of the various object management   */
/* screens                                                              */
/************************************************************************/
class indyObjectManager extends indyObject
{
   var $obj_set;
   var $template_set;
   var $site_obj;
   var $action_req;
   var $type_dir;
   var $storage;
   var $notification_msg = "";
   var $newobjects_storage;
   var $header_prefix = "";


   function indyObjectManager($type_dir, $url)
   {
      global $system_config;
      $this->action_req = new indyObjectActionRequest($url);
      $this->type_dir = $type_dir;
      $this->newobjects_storage = $system_config->new_objects_storage;
      $this->storage = $system_config->object_index_storage;
   }




   /************************************************************************/
   /* Processes the request figures out what to do and generally routes    */
   /* Input: site options array of settings (from xml file)                */
   /************************************************************************/
   function processRequest(&$site_options)
   {
      global $editor_session, $performance_test;
      if($performance_test > 1)  markTime("Starting object management processing");
      //safe to assume that editor_session is open
      $this->action_req->readRequest();
      $result = $this->action_req->checkPermissions("read");
      if(!$result)
      {
         $editor_session->writeNoReadPermissionError();
         return false;
      }
      if(!$this->loadSiteObject($this->action_req->getSiteFilter()))
      {
         return false;
      }


      $this->header_prefix = $this->site_obj->name()." - ".$site_options->getString("site_administration");
      addToPageTitle($site_options->getString("site_administration"));
      // Save changes
      if($this->action_req->isCommitUpdateAction())
      {
         $result = $this->action_req->checkPermissions("write");
         if(!$result)
         {
            $editor_session->writeNoWritePermissionError();
            return false;
         }
         $result = $this->action_req->checkLocks();
         $this->action_req->writeUserMessageBox();
         if(!$result)
         {
            return false;
         }
         $result = $this->commitUpdates($site_options);
         $this->performNotificationAndLogging($result);
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->action_req->relinquishLocks();
         return $this->writeResultPage($result, $site_options);
      }
      if(!$this->action_req->getRequiredLocks())
      {
         $this->action_req->writeUserMessageBox();
         return;
      }
      $this->action_req->writeUserMessageBox();
      if($this->action_req->isCancel())
      {
         $this->action_req->populateFromEntry();
      }


      $action = $this->action_req->getAction();
      $this->obj_set = new indyObjectSet($this->type_dir, $this->storage);
      if($action === false)
      {
         return $this->writeIntroPage($site_options);
      }


      $type_filters = $this->action_req->getTypeFilters();
      if($action == 'edit' or $action == 'translate' or $action == 'create' or $action == 'view')
      {
         $type_filters = array("*");
      }
      if($action == 'create' or $action== 'edit' or $action == 'translate' or $action=='delete')
      {
         $site_filters = array("*");
      }
      else
      {
         $site_filters = $this->action_req->getSiteFilters();
      }
      if(!$this->obj_set->load($site_filters, $type_filters, $this->action_req))
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessage(array(INDY_ERROR, "Failed to load object set", "32"));
         return false;
      }
      $this->setUserMessages($this->obj_set->getUserMessages());
      if($this->action_req->isUnconfirmedUpdateAction())
      {
         $result = $this->processUnconfirmedAction($action, $site_options);
      }
      elseif($action == 'view' || $action == 'xmlview')
      {


         $indy_obj = $this->loadObjectForEditing();
         if(!$indy_obj)
         {
            $result = false;
         }
         else
         {
            $result = $this->writeViewPage($indy_obj, $action, $site_options);
         }
      }
      elseif($action == 'create' or $action== 'edit' or $action == 'translate')
      {
         $indy_obj = $this->loadObjectForEditing();
         if(!$indy_obj)
         {
            $result = false;
         }
         else
         {
            $tr_mode = false;
            if($action == 'translate' && !$indy_obj->hasLockedFields())
            {
               $langs = $this->obj_set->getObjectAvailableLanguages($indy_obj->objref);
               if(count($langs) == 1)
               {
                  $tr_mode = true;
               }
            }
            $result = $this->writeEditPage($indy_obj, $site_options, $tr_mode);
         }
      }
      elseif($action == 'list')
      {
         $result = $this->writeObjectListPage($site_options);
      }
      elseif($action == 'recache')
      {
         $extra_html = "<input type='hidden' name='confirm-publish' value='on' />";
         $result = $this->writeConfirmActionPage($site_options, $extra_html);
      }
      //below here the functions could do with their own page
      elseif($action == 'import')
      {
         $result = $this->writeConfirmImportPage($site_options);
         //$result = $this->writeImportPage($site_options);
      }
      elseif($action == 'site-recache')
      {
         //$result = $this->writeExportPage($site_options);
         $result = $this->writeConfirmActionPage($site_options);
      }
      elseif($action == 'site-export')
      {
         require_once("objects/indyobjects/indysiteexporter.inc");
         $exporter = new indySiteExporter($this->obj_set, $this->action_req);
         $result = $this->writeSiteExportPage($site_options, $exporter);
         //$result = $this->writeConfirmActionPage($site_options);
      }
      elseif($action == 'export')
      {
         //$result = $this->writeExportPage($site_options);
         $result = $this->writeConfirmExportPage($site_options);
      }
      elseif($action == 'site-files')
      {
         //$result = $this->writeExportPage($site_options);
         $result = $this->writeSiteFilesPage($site_options);
      }
      if($result === false)
      {
         $this->setUserMessage(array(USER_ERROR, "Error Processing Object Management Request", "32"));
      }
      return $result;


   }


   function performNotificationAndLogging($result)
   {
      global $system_config;
      $msg = $this->notification_msg;
      if($msg == "")
      {
         $msg = "<no notification content specified>";
      }
      $notify_level = $this->site_obj->getMeta("notify_editobject_level");
      if($result !== false && ($this->action_req->requiresNotify($notify_level) or (isset($_REQUEST['send_mail']) && $_REQUEST['send_mail'] == 'on')))
      {
         if(isValidEmailAddress(trim($this->site_obj->getMeta("notify_object_toemail"))))
         {
            $to = trim($this->site_obj->getMeta("notify_object_toemail"));
         }
         else
         {
            $to = $system_config->notification_to_email_address;
         }
         if(isValidEmailAddress(trim($this->site_obj->getMeta("notify_object_replyemail"))))
         {
            $reply = trim($this->site_obj->getMeta("notify_object_replyemail"));
         }
         elseif(isValidEmailAddress($system_config->notification_replyto_email_address))
         {
            $reply = $system_config->notification_replyto_email_address;
         }
         else
         {
            $reply = $to;
         }
         $from = $system_config->notification_from_email_address;
         $subj_prefix = $this->site_obj->getMeta("notify_object_subject_prefix");
         $this->action_req->sendNotification($to, $from, $reply, $msg, $subj_prefix);
      }
      $this->action_req->logAction($result);
   }


   /************************************************************************/
   /* Carries out any database updates that are needed                     */
   /* Input: none                                                          */
   /* output: t/f                                                          */
   /************************************************************************/
   function commitUpdates(&$site_options)
   {
      global $system_config, $OSCAILT_SCRIPT;
      $this->obj_set = new indyObjectSet($this->type_dir, $this->storage);
      $action = $this->action_req->getAction();

      if($action == 'site-export' or $action == 'site-recache')
      {
         $sites = $this->action_req->site_id;
      }
      else
      {
         $sites = array("*");
      }
      $types = array("*");
      if(!$this->obj_set->load($sites, $types, $this->action_req))
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Set of Managed Objects: Updates Not Saved","42s" ) );
         return false;
      }
      $sites_to_recache = array();


      if($action == 'recache')
      {
         $targets = $this->action_req->getTargetObjects();
         foreach($targets as $target_id)
         {
            $result = $this->obj_set->buildObjectCacheFiles($target_id);
         }
         $this->notification_msg .= "\r\nData Objects Re-cached:".implode(", ", $targets);
         return $result;
      }
      elseif($action == 'export')
      {
         $result = $this->exportToTemplate();
         return $result;
      }
      elseif($action == 'import')
      {
         $result = $this->importObjectSet();
         return $result;
      }
      elseif($action == 'site-export')
      {
         $this->notification_msg .= "\r\nData Objects Exported to :".$system_config->object_export_dir. "for site ".$this->action_req->getSiteId();
         require_once("objects/indyobjects/indysiteexporter.inc");
         $exporter = new indySiteExporter($this->obj_set, $this->action_req);
         $exporter->readChoices();
         if($exporter->checkChoices($site_options))
            return $exporter->export();
         else
         {
            $this->setUserMessage ( array( USER_ERROR, "Programme Error: Bad input into export function","42e") );
            return false;
         }
      }
      elseif($action == 'site-recache')
      {
         $this->notification_msg .= "\r\nSite ".$this->action_req->getSiteId()." republished.\r\n";
         $sites_to_recache[] = $this->action_req->getSiteId();
         $system_config->auto_cache_objects = true;
      }
      elseif($action == 'create')
      {
         $storage = $this->newobjects_storage;
         list($site_id, $obj_type, $target_lang) = $this->action_req->getNewObjectParameters();
         $indy_obj = $this->obj_set->createNewObjectFromForm($site_id, $obj_type, $target_lang, $storage);
      }
      else
      {
         $targets = $this->action_req->getTargetObjects();
         $langs = $this->action_req->getObjectLanguages();

         foreach($targets as $target_id)
         {
            foreach($langs as $target_lang)
            {
               if($action == 'delete')
               {
                  $indy_obj = $this->obj_set->fetchObject($target_id, $target_lang);
                  $this->notification_msg .= "\r\nDeleted Object $target_id XML:\r\n".$indy_obj->getXML();
                  $this->obj_set->deleteObject( $target_id, $target_lang );
               }
               elseif($action == 'edit' or $action == 'translate')
               {
                  $indy_obj = $this->obj_set->fetchObject($target_id, $target_lang);
                  $this->notification_msg .= "\r\nOriginal Object $target_id XML:\r\n".$indy_obj->getXML();
                  if(strtoupper($indy_obj->type) == 'SITE')
                  {
                     $sites_to_recache[] = $indy_obj->objref;
                  }
                  if(!$indy_obj)
                  {
                     $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Stored Copy of Managed Object $target_id ($target_lang): Updates Not Saved","42g"    ) );
                     return false;
                  }
                  $updated_obj = $this->obj_set->LoadObjectFromForm($indy_obj, $action);
                  if($updated_obj === false)
                  {
                     $this->setUserMessages($this->obj_set->getUserMessages());
                     $this->setUserMessages($indy_obj->getUserMessages());
                     $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Updates of Managed Object $target_id ($target_lang) From Form: Updates Not Saved","42s"    ) );
                     return false;
                  }
                  $this->notification_msg .= "\r\nUpdated Object $target_id XML:\r\n".$updated_obj->getXML();
               }
            }
            $obj_type = $indy_obj->getType();
         }
      }
      $force_publish = false;
      if(isset($_REQUEST['confirm-publish']) && $_REQUEST['confirm-publish'] == 'on')
      {
         $force_publish = true;
      }
      $result = $this->obj_set->commitChanges(true, $force_publish);
      if($action == 'create')
      {
         $this->action_req->target_objects = array($this->obj_set->newObject->objref);
         $this->notification_msg = $this->obj_set->newObject->getXML();
         if(strtolower($obj_type) == 'site')
         {
            $this->createSiteSundries($this->obj_set->newObject);
         }
      }
      elseif(strtolower($obj_type) == 'site')
      {
         if($action != 'delete')
         {
            $params = $this->action_req->getFetchObjectParameters();
            $site_obj = $this->obj_set->fetchObject($params[0], $params[1]);
            if($site_obj)
            {
               $this->checkSiteSundries($site_obj);
            }
         }
      }
      if(count($sites_to_recache) > 0  && ($system_config->auto_cache_objects or $force_publish))
      {
         $this->obj_set->load(array("*"),array("*"), $this->action_req);
         $this->obj_set->rebuildCaches($sites_to_recache);
      }
      return $result;
   }


   /************************************************************************/
   /* Exports the object to the template set                               */
   /************************************************************************/
   function exportToTemplate()
   {
      global $system_config, $OSCAILT_SCRIPT;
      if(!isset($_REQUEST['export_name']) or $_REQUEST['export_name'] == "")
      {
         $this->setUserMessage ( array( USER_ERROR, "You Must specify a name to give to the template what you are exporting.","sdd" ) );
         return false;
      }
      $template_base = cleanseTitleField($_REQUEST['export_name']);
      $targets = $this->action_req->getTargetObjects();
      foreach($targets as $target_id)
      {
         $tname = strtolower($this->obj_set->getObjTypeName($target_id));
         $orig_obj_stub = $this->obj_set->getObjectStub($target_id);
         if($orig_obj_stub === false)
         {
            $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Object Stub for $target_id: Can't Export","42s"    ) );
            return false;
         }
         $obj_stub = $orig_obj_stub->copy();
         $obj_stub->obj_id = $template_base;
         $obj_stub->storage = $template_base;
         $obj_stub->site_id = "templates";
         if(!$this->loadTemplateSet())
         {
            return false;
         }
         if($this->template_set->containsObject($template_base))
         {
      	    $this->template_set->registerObjectStubUpdate($obj_stub);
            $obj = $this->template_set->fetchObject($template_base, $obj_stub->primary_language_code);
         }
         else
         {
            $insert_obj = $this->obj_set->fetchObject($target_id, $obj_stub->primary_language_code);
            if($insert_obj)
            {
               $insert_obj->objref = $obj_stub->obj_id;
               $insert_obj->lockedMetaData = array();
               $result = $this->template_set->forceInsertObject($obj_stub, $insert_obj, false);
               if($result === false)
               {
                  $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Insert Object Into Object Template Set","22g"    ) );
                  return false;
               }
            }
         }
         $langs = $this->obj_set->getObjectAvailableLanguages($target_id);
         array_unshift($langs, 'master');
         foreach($langs as $lang)
         {
            $indy_obj = $this->obj_set->fetchObject($target_id, $lang);
            if(!$indy_obj)
            {
               $this->setUserMessage ( array( USER_ERROR,"Programme Error: Failed to Load Stored Copy of Managed Object $target_id ($target_lang): Export Not Produced","42g"    ) );
               return false;
            }
            $indy_obj->objref=$obj_stub->obj_id;
            $this->template_set->registerObjectUpdate($indy_obj);
            $this->notification_msg .= "\r\nObject $target_id $lang added to template set as $template_base.\r\nXML:\r\n".$indy_obj->getXML();
         }
         if(!$this->template_set->commitChanges(false))
         {
            $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Update Data for Template Object $indy_obj->objref: Export Not Produced","42g"    ) );
            return false;
         }
         $this->setUserMessages($this->template_set->getUserMessages());
      }
      return true;
   }


   /************************************************************************/
   /* imports a set of objects from an object index file                   */
   /************************************************************************/
   function importObjectSet()
   {
      global $system_config, $OSCAILT_SCRIPT;


      if(!isset($_REQUEST['import_name']) or $_REQUEST['import_name'] == "")
      {
         $this->setUserMessage ( array( USER_ERROR, "You Must specify a name of the object index file that you want to import.","sdd" ) );
         return false;
      }
      if(isset($_REQUEST['force_import_ids']) && $_REQUEST['force_import_ids'] == 'on')
      {
         $force_ids = true;
      }
      else
      {
         $force_ids = false;
      }
      $import_id_map = array();
      $inserted_ids = array();
      $import_index = cleanseTitleField($_REQUEST['import_name']);
      $import_set = new indyObjectSet($this->type_dir, $import_index);
      $import_set->load(array("*"), array("*"), $this->action_req);
      foreach(array_keys($import_set->itemCollection) as $obj_id)
      {
         $import_stub = $import_set->itemCollection[$obj_id]->copy();
         if(!$force_ids)
         {
            $import_stub->obj_id = -1;
         }


         $insert_obj = $import_set->fetchObject($obj_id, $import_stub->primary_language_code);
         $master_obj = $import_set->fetchObject($obj_id, 'master');
         $import_stub->storage = $system_config->object_index_storage;
         $insert_id = $this->obj_set->forceUpdateObject($import_stub, $insert_obj, $master_obj);
         if($insert_id === false)
         {
            $this->setUserMessage ( array( INDY_WARNING, "Failed to Force Object ID Import of Managed Object $obj_id ($lang): Import Continuing","x"));
            continue;
         }
         if($insert_id != $obj_id)
         {
            $import_id_map[$obj_id] = $insert_id;//old -> new
         }
         $inserted_ids[] = $insert_id;
         $langs = $import_set->getObjectAvailableLanguages($obj_id);
         array_unshift($langs, 'master');
         foreach($langs as $lang)
         {
            $indy_obj = $import_set->fetchObject($obj_id, $lang);
            if(!$indy_obj)
            {
               $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Import Copy of Managed Object $obj_id ($lang): Import Aborted","42z"    ) );
               return false;
            }
            $indy_obj->objref=$insert_id;
            $this->obj_set->registerObjectUpdate($indy_obj);
         }
      }
      if(!$this->obj_set->commitChanges(false))
      {
         $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Update Data for Template Object $indy_obj->objref: Export Not Produced","42g"    ) );
         return false;
      }
      $this->setUserMessages($import_set->getUserMessages());
      //finally tidy up the inserted objects to ensure that their links and so on remain consistent.
      foreach($inserted_ids as $insert_id)
      {
         $obj_stub = $this->obj_set->itemCollection[$insert_id];
         if(strtolower($obj_stub->obj_type) == 'site')
         {
            $site_obj = $this->obj_set->fetchObject($obj_stub->obj_id, $obj_stub->primary_language_code);
            //$this->createSiteSundries($obj_stub->obj_id, false);
            $this->createSiteSundries($site_obj, false);
         }
         if(in_array($obj_stub->site_id, array_keys($import_id_map)))
         {
            $obj_stub->site_id = $import_id_map[$obj_stub->site_id];
            $this->obj_set->registerObjectStubUpdate($obj_stub);
         }
         elseif(!isset($this->obj_set->itemCollection[$obj_stub->site_id ]) or strtolower($this->obj_set->itemCollection[$obj_stub->site_id]->obj_type) != 'site')
         {
            $obj_stub->site_id = 1;
            $this->obj_set->registerObjectStubUpdate($obj_stub);
         }
         $langs = $this->obj_set->getObjectAvailableLanguages($insert_id);
         array_unshift($langs, 'master');
         foreach($langs as $lang)
         {
            $obj = $this->obj_set->fetchObject($insert_id, $lang);
            $obj->remapObjectReferences($this->obj_set, $import_id_map);
            $this->obj_set->registerObjectUpdate($obj);
            $this->notification_msg .= "\r\nObject $insert_id imported into data object set XML:\r\n".$obj->getXML();
         }
      }
      if(!$this->obj_set->commitChanges(true))
      {
         $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Update Data for import operation remapping","42g"    ) );
         return false;
      }
      return true;
   }


   function loadTemplateSet()
   {
      global $system_config, $OSCAILT_SCRIPT;
      if(isset($this->template_set))
      {
         return true;
      }
      else
      {
         $sites = array("*");
         $template_types = array("*");
         $this->template_set = new indyObjectSet($this->type_dir, $system_config->object_template_store."templates.index");
         $template_req = new indyObjectActionRequest($OSCAILT_SCRIPT);
         if(!$this->template_set->load($sites, $template_types, $template_req))
         {
            $this->setUserMessages($this->template_set->getUserMessages());
            $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Set of Template Objects","42s" ) );
            return false;
         }
         //op_r($this->template_set);
      }
      return true;
   }


   /************************************************************************/
   /* Create the various other things that a site structure needs          */
   /* Like File Storage areas,           */
   /************************************************************************/
   function createSiteSundries(&$site_obj, $force_new=true)
   {
      global $system_config, $pseudo_directory_offset;
      $st_store = $pseudo_directory_offset.$system_config->site_file_store_base.'site_'.$site_obj->objref.'/';
      if(!mkdir($st_store, $system_config->default_writable_directory_permissions))
      {
         $this->setUserMessage( array ( USER_WARNING, "Failed to Make Directory $st_store to store files associated with the site: ".$site_obj->objref, "sr5"));
         if(!file_exists($st_store)) return false;
      }
      if($system_config->debug_level >= DEBUG_LEVEL_1)
      {
         $this->setUserMessage(array(
            DEBUG_LEVEL_1, "Created Directory $st_store for site object ".$site_obj->objref." file store",   "a3y"));
      }
      return $this->checkSiteSundries($site_obj, $force_new);
   }




   function checkSiteSundries(&$site_obj, $force_new=false)
   {
      global $system_config, $pseudo_directory_offset;
      if($site_obj->getMeta('different_css') == 'on')
      {
         $default_store = $pseudo_directory_offset.$system_config->site_file_store_base ."default/";
         $st_store = $pseudo_directory_offset.$system_config->site_file_store_base.'site_'.$site_obj->objref.'/';
         $css_1 = 'style.css';
         $css_2 = 'common.css';
         $css_3 = 'contentstyle.css';
         if(!file_exists($st_store.$css_1) or $force_new)
         {
            copy( $default_store.$css_1, $st_store.$css_1);
            chmod($st_store.$css_1, $system_config->default_writable_directory_permissions);
         }
         if(!file_exists($st_store.$css_2) or $force_new)
         {
            copy( $default_store.$css_2, $st_store.$css_2);
            chmod($st_store.$css_2, $system_config->default_writable_directory_permissions);
         }
         if(!file_exists($st_store.$css_3) or $force_new)
         {
            copy( $default_store.$css_3, $st_store.$css_3);
            chmod($st_store.$css_3, $system_config->default_writable_directory_permissions);
         }
      }
      return true;
   }




   function checkExportInput()
   {
      echo "<P class=error>NOT IMPLEMENTED EITHER!!!</p>";
   }


   /************************************************************************/
   /* Loads the appropriate object that is requested                     */
   /************************************************************************/
   function loadObjectForEditing()
   {
      global $editor_session;
      if($this->action_req->getAction() == 'create')
      {
         $storage = $this->newobjects_storage;
         $params = $this->action_req->getNewObjectParameters();
         if($params === false)
         {
            $this->setUserMessages($this->action_req->getUserMessages());
            $this->setUserMessage ( array( USER_ERROR, "Programme Error: Insufficent Data Supplied to Create new Object","43s" ) );
            return false;
         }
         if($this->action_req->isUnconfirmedUpdateAction())
         {
            $indy_obj = $this->obj_set->createNewObjectFromForm($params[0], $params[1], $params[2], $storage);
         }
         else
         {
            $this->loadTemplateSet();
            if(isset($_REQUEST['obj_template']) && $_REQUEST['obj_template'] != "")
            {
               $tmpl_obj = $this->template_set->fetchObject($_REQUEST['obj_template'], $params[2]);
               $tmpl_obj->lockedMetaData = array();
               $tmpl_obj->lockedContainers = array();               
	       $tmpl_obj->lockedAttributes = array();                              
               if($tmpl_obj)
               {
                  $indy_obj = $this->obj_set->createNewObjectFromTemplate($tmpl_obj, $params[0], $params[2], $storage);
               }
            }
            if(!isset($tmpl_obj))
            {
               $indy_obj = $this->obj_set->createNewObjectFromScratch($params[0], $params[1], $params[2], $storage);
            }
         }
      }
      else
      {
         $params = $this->action_req->getFetchObjectParameters();
         if($params === false)
         {
            $this->setUserMessages($this->action_req->getUserMessages());
            $this->setUserMessage ( array( USER_ERROR, "Programme Error: Insufficent Data Supplied to Load Object","4as" ) );
            return false;
         }
	 // echo("parms 0 ".$params[0]. " 1 ".$params[1]."<br>");
         $indy_obj = $this->obj_set->fetchObject($params[0], $params[1]);
         if(!$indy_obj)
         {
            $this->setUserMessages($this->obj_set->getUserMessages());
            $this->setUserMessage( array(USER_ERROR, "Failed to Load Specified Object", "x3f" ));
            return false;
         }
         if($this->action_req->isUnconfirmedUpdateAction())
         {
            $indy_obj = $this->obj_set->loadObjectFromForm($indy_obj, $this->action_req->getAction());
         }
      }
      return $indy_obj;
   }




   /************************************************************************/
   /* Checks object and if it is valid, presents confirm screen         */
   /* Input: $action (string) $site options (array)                  */
   /* output: t/f
   /************************************************************************/
   function processUnconfirmedAction($action, &$site_options)
   {
      if($action == 'edit' or $action == 'create' or $action == 'translate')
      {
         $indy_obj = $this->loadObjectForEditing();
         if(!$indy_obj)
         {
            $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Stored Copy of Managed Object for $action): Abandoning ship","sos"    ) );
            return false;
         }
         $x = "";
         $tr_mode = false;
         if($action == 'translate')
         {
            if($action == 'translate')
            {
               $langs = $this->obj_set->getObjectAvailableLanguages($indy_obj->objref);
               if(count($langs) == 1)
               {
                  $tr_mode = true;
               }
            }
         }
         if($indy_obj->valid)
         {
            $result = $this->writePreviewPage($indy_obj, $site_options, $tr_mode);
         }
         else
         {
            $result = $this->writeEditPage($indy_obj, $site_options, $tr_mode);
         }
      }
      elseif($action == 'export')
      {
         if($this->checkExportInput($site_options))
         {
            $result = $this->writeConfirmExportPage($site_options);
         }
         else
         {
            $this->writeExportPage($site_options);
            return false;
         }
      }
      elseif($action == 'site-export')
      {
         require_once("objects/indyobjects/indysiteexporter.inc");
         $exporter = new indySiteExporter($this->obj_set, $this->action_req);
         $exporter->readChoices();
         if($exporter->checkChoices($site_options))
         {
            $result = $this->writeConfirmSiteExportPage($site_options, $exporter);
         }
         else
         {
            $this->writeSiteExportPage($site_options, $exporter);
            return false;
         }
      }
      else
      {
         $result = $this->writeConfirmActionPage($site_options);
      }
      return $result;
   }


   /************************************************************************/
   /* loads all of the site objects managed by this installation         */
   /* Input: reference to array for return values, language code (string)   */
   /* output: reference to array filled with objects                       */
   /************************************************************************/
   function getAllManagedSiteObjects(&$all_objs, $langCode)
   {
      $this->obj_set = new indyObjectSet($this->type_dir, $this->storage);
      $site_filters = array("*");
      $type_filters = array("site");
      if(!$this->obj_set->load($site_filters, $type_filters, $this->action_req))
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessage ( array( USER_ERROR, "Programme Error: Failed to Load Set of Managed Sites","45s" ) );
         return false;
      }


      $all_objs = $this->obj_set->fetchAllObjects($all_objs, $langCode);
      $this->setUserMessages($this->obj_set->getUserMessages());
      return $all_objs;
   }


   function loadSiteObject($obj_id)
   {
      global $system_config, $site_title;
      if(isset($this->site_obj)) return true;
      $obj_set = new indyObjectSet($this->type_dir, $this->storage);
      if(!$obj_set->load(array("*"), array("site"), $this->action_req))
      {
         $this->setUserMessages($obj_set->getUserMessages());
         $this->setUserMessage(array(INDY_ERROR, "Failed to load object set", "32"));
         return false;
      }
      if(!isset($obj_set->itemCollection[$obj_id]))
      {
         $this->setUserMessages($obj_set->getUserMessages());
         $this->setUserMessage(array(INDY_ERROR, "Failed to load object set", "32"));
         return false;
      }
      $obj_stub = $obj_set->itemCollection[$obj_id];
      $lang = $obj_stub->primary_language_code;
      $site_obj = $obj_set->fetchObject($obj_id, $lang);
      if(!$site_obj)
      {
         $this->setUserMessages($obj_set->getUserMessages());
         $this->setUserMessage(array(INDY_ERROR, "Failed to load site object $obj_id", "232"));
         return false;
      }
      $this->site_obj = $site_obj;
      return true;
   }


   /************************************************************************/
   /* Normally objects are viewed in their cached versions.  This will    */
   /* deliver the live version of the object, constructed from the metadata*/
   /* used by test.php                                                     */
   /************************************************************************/
   function getLiveObjectHTML($id, $lang_code)
   {
      $this->obj_set = new indyObjectSet($this->type_dir, $this->storage);
      if(!$this->obj_set->load(array("*"), array("*"), $this->action_req))
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessage(array(USER_ERROR, "Failed to load object set", "32"));
         return false;
      }


      if(!$this->obj_set->containsObject($id))
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessage(array(USER_ERROR, "Attempt to load non-existant object with ID $id", "32"));
         return false;
      }
      $indy_obj = $this->obj_set->fetchObject($id, $lang_code);
      if(!$indy_obj)
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessage(array(USER_ERROR, "Failed to load requested object", "32"));
         return false;
      }
      $obj_style = "";
      $layout_ref = $indy_obj->getPageLayoutRef();
      if($layout_ref !== false and strtolower($this->obj_set->itemCollection[$layout_ref]->obj_type) == 'page')
      {
         $layout_obj = $this->obj_set->fetchObject($layout_ref, $lang_code);
         if($layout_obj)
         {
            $obj_stub = $this->obj_set->itemCollection[$id];
            $site_id = $obj_stub->site_id;
            $object_html = $layout_obj->fill($indy_obj, $this->obj_set, $site_id, true);
            return $object_html;
         }
      }
      $obj_style = '<?php echo $page_style; ?>';
      $c_s = array();
      if($indy_obj->isProtected())
      {
         if(!$indy_obj->checkObjectAccess($this->obj_set->itemCollection[$indy_obj->objref]->site_id))
         {
            writeFailureMessage("Access Denied", "You do not have permission to access this page");
            return "";
         }
      }
      $object_html = $indy_obj->getHTML($obj_style, FULL_DEPTH, $this->obj_set, $c_s, true);
      if($object_html === false)
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessage(array(USER_ERROR, "Failed to load requested object", "32"));
         return false;
      }
      return $object_html;
   }


   /************************************************************************/
   /* Functions to draw the various filters on the management pages      */
   /************************************************************************/
   function getSiteFilter($sites)
   {
         $ret = "<INPUT type='hidden' name='site_id' value='".implode(",", $sites)."'>\n";
         return $ret;
   }


   function getListFilters($stra)
   {


      $onward_req = $this->action_req->copy();
      unset($onward_req->obj_type);
      unset($onward_req->obj_language);
      unset($onward_req->site_id);
      $sites = $this->action_req->site_id;
      $types = $this->action_req->obj_type;
      $langs = $this->action_req->obj_language;
      $link = $onward_req->getLink();


      $str = "<form style='display: inline;' method='post' action='$link' name='typefilterform'>\n";
      $str .= "<span class='sitebuilder-filter'>$stra</span> ";
      $str .= $this->getSiteFilter($sites);
      $str .= $this->getTypeFilter($types);
      $str .= $this->getLanguageFilter($langs);
      $str .= "<input type=submit name='go' value='View'></form>\n";
      return $str;
   }


   function getTypeFilter($types)
   {
      /*$all_selected = "";
      if(is_array($types) and count($types) > 0)
      {
         if(in_array('*', $types))
         {
            $all_selected = "selected";
            $types=array();
         }
      }
      else
      {
         $types = array();


      }
      $str = "<select name='obj_type'>";
      $str .= "<option $all_selected value='*'>All Types</option>\n";
      $str .= $this->obj_set->supportedTypes->getTypesAsOptions($types);
      $str .= "</select>";
      return $str;*/
      if(is_array($types) and count($types) > 0)
      {
         return "<input type='hidden' name='obj_type' value='".implode(",",$types)."'>";
      }
      else
      {
         return "";
      }
   }


   function getLanguageFilter($langs)
   {
      global $languageList;
      $str = "";
      if(count($languageList->getfLanguages())>1)
      {
         $str .= "<select name='obj_language'>";
         $str .= $languageList->getAllLanguagesAsPrefixNameOptions($langs[0]);
         $str .= "</select>";
      }
      return $str;
   }


   function getSiteActionsHTML(&$site_options)
   {
      $ret_str = "";
      $onward_req = $this->action_req->copy();
      unset($onward_req->obj_type);
      unset($onward_req->obj_language);


      $actions = $site_options->getItemsByType('siteaction');
      foreach($actions as $act)
      {
         $onward_req->action = $act->name;
         if($onward_req->checkPermissions("read"))
         {
            $icon = $act->getIcon();
            $text = htmlspecialchars($act->getMeta("title"), ENT_QUOTES);
            $title = htmlspecialchars($act->getMeta("description"), ENT_QUOTES);
            $lnk = htmlspecialchars($onward_req->getLink(), ENT_QUOTES);
            $ret_str .= "<A class='sitebuilder-filter' HREF='$lnk'>$text <IMG class='itemoption' SRC='$icon' ALT='$text' TITLE='$title'></A> |\n";
         }
      }
      return $ret_str;
   }




   /************************************************************************/
   /* Functions to draw the contextual navigation for object management    */
   /************************************************************************/
   function writeSpecificObjectNavig(&$indy_obj, &$site_options)
   {
      $type_id = $indy_obj->getType();
      $type_obj = $this->obj_set->supportedTypes->getType($type_id);
      $type_name = $type_obj->name;
      $navs = array();


      $navs = $this->getNavigArray($site_options, $navs, $type_id, $type_name);
      $filter_str = "";
      $indy_obj->writeOptionsJS();
      echo $this->getNavigation($navs);
      echo $filter_str;
   }




   function getSpecificObjectNavig(&$indy_obj, &$site_options)
   {
      $type_id = $indy_obj->getType();
      $type_obj = $this->obj_set->supportedTypes->getType($type_id);
      $type_name = $type_obj->name;
      $navs = array();
      $navs = $this->getNavigArray($site_options, $navs, $type_id, $type_name);
      $indy_obj->writeOptionsJS();
      return $this->getNavigation($navs);
   }




   function getNavigation(&$contexts)
   {
      $ctxt_str = "";
      if(count($contexts > 0))
      {
         $ctxt_str = "<UL class='context'>\n";
         foreach($contexts as $ctxt)
         {
            $ctxt_str .= "<LI class='context'>$ctxt</LI>\n";
         }
         $ctxt_str .= "</UL>\n";
      }
      return $ctxt_str;
   }


   function getNavigArray(&$site_options, &$arr, $type_id=0, $type_name="")
   {
      global $OSCAILT_SCRIPT, $editor_session;
      if($OSCAILT_SCRIPT == 'admin.php')
      {
         $site_id = $this->site_obj->objref;
         $oscailt_admin_str = $site_options->getString("oscailt_administration");
         $arr[] = "<a class='admin-context' href='".$OSCAILT_SCRIPT."'>$oscailt_admin_str</a>\n";

         if ($editor_session->editor->isDashboardOn() == false ) {
             $oscailt_admin_str = $site_options->getString("oscailt_dashboard");
             $arr[] = "<a class='admin-context' href='".$OSCAILT_SCRIPT."?dashboard=1'>$oscailt_admin_str</a>\n"; 
         }
         $site_admin_str = $this->header_prefix;
         $arr[]= "<a class='admin-context' href='".$OSCAILT_SCRIPT."?site_id=$site_id'>$site_admin_str</a>\n";
      }
      elseif($this->action_req->getAction() !== false)
      {
         $site_admin_str = $this->header_prefix;
         $arr[]= "<a class='admin-context' href='".$this->action_req->getObjPage()."'>$site_admin_str</a>\n";
      }
      if(isset($this->action_req->target_objects) or isset($this->action_req->obj_type))
      {
         $mg_str = $site_options->getString("management");
         $list_req = $this->action_req->copy();
         unset($list_req->target_objects);
         unset($list_req->edit_entry_url);
         $list_req->action = 'list';
         $list_req->obj_type = array(strtolower($type_id));
         $arr[] = "<a class='admin-context' href='".$list_req->getLink()."'>$type_name $mg_str</a>\n";
      }
      if(isset($this->action_req->target_objects))
      {
         $obj_req = $this->action_req->copy();
         unset($obj_req->edit_entry_url);
         $obj_req->action = 'view';
         $id = $obj_req->getTargetObject();
         $obj_str = $site_options->getString("indy_object").": $id (".$obj_req->getObjectLanguage().")";
         $arr[] = "<a class='admin-context' href='".$obj_req->getLink()."'>$obj_str</a>\n";
      }
      return $arr;
   }




   /************************************************************************/
   /* Write the initial listing of object types page
   /* Input: data object or none
   /* Output: true/false
   /************************************************************************/
   function writeIntroPage(&$s_o)
   {
      $site_filters = $this->action_req->getSiteFilters();
      $this->obj_set->load($site_filters,array("*"), $this->action_req);
      $objTypeSet = new indyDataTypeSet ($this->type_dir);
      //if( !($objTypeSet->load() && $objTypeSet->loadAllTypes()) )
      if( !($objTypeSet->load() && $objTypeSet->loadAllTypeDetails()) )
      {
         $this->setUserMessages( $objTypeSet->getUserMessages() );
         $this->setUserMessage( array( USER_ERROR, "Failed to load the list of Data Types", "" ) );
         $this->writeUserMessageBox();
         return false;
      }
      $navs = array();
      $navs = $this->getNavigArray($s_o, $navs, "", "");
      $nav_str = $this->getNavigation($navs);
      $this->setUserMessages( $objTypeSet->getUserMessages() );
      //$types = $objTypeSet->getTypes();
      $types = $objTypeSet->getAllTypeDetails();
      $title_text = $this->header_prefix;
      $icon = $s_o->getIcon("small");
      $filters = $this->getSiteActionsHTML($s_o);
      $intro_text = $s_o->getString("site_builder_intro");
      writeAdminPageHeader($title_text, $nav_str, $intro_text, $filters);
      $this->writeUserMessageBox();
      $this->writeSiteObjectsMenuSection($s_o, $types);
      writeAdminPageFooter();
   }






   /************************************************************************/
   /* Functions to draw the Page Garnishing                         */
   /************************************************************************/
   function writeMenuHeader()
   {
   ?>
      <table class="admin">
   <?
   }


   function writeMenuFooter($cols)
   {
   ?>
      <tr class="admin"><th colspan=<?=$cols?> class="admin"><img class="right" src="graphics/adminicon.gif">&nbsp;</th></tr>
      </table>
   <?
   }


   function writeMenuSectionHeader($title, $msg, $cols)
   {
      echo "<TR><TH class=admin colspan=$cols><big>$title</big><br /><small>$msg</small></TH></TR>\n";
   }


   function writeSiteObjectsMenuSection(&$s_o, &$types)
   {
      global $legalDataTypes;
      $type_sets = array();
      foreach(array_keys($legalDataTypes) as $type_name)
      {
         if(!isset($type_sets[$legalDataTypes[$type_name]])) $type_sets[$legalDataTypes[$type_name]] = array();
         foreach($types as $type)
         {
            if($type->type == $type_name)
            {
               $type_sets[$legalDataTypes[$type_name]][] = $type;
               break;
            }
         }
      }
      $cols = 3;
      $this->writeMenuHeader();
      if(isset($type_sets['element']))
      {
         $title = $s_o->getString("simple_objects");
         $msg = $s_o->getString("simple_objects_intro");
         writeMenuSectionHeader($title, $msg, $cols);
         $this->writeTypeSet($type_sets['element'], $cols, $s_o);
      }
      if(isset($type_sets['module']))
      {
         $title = $s_o->getString("module_objects");
         $msg = $s_o->getString("module_objects_intro");
         writeMenuSectionHeader($title, $msg, $cols);
         $this->writeTypeSet($type_sets['module'], $cols, $s_o);
      }
      if(isset($type_sets['layout']))
      {
         $title = $s_o->getString("layout_objects");
         $msg = $s_o->getString("layout_objects_intro");
         writeMenuSectionHeader($title, $msg, $cols);
         $this->writeTypeSet($type_sets['layout'], $cols, $s_o);
      }
      /*if(isset($type_sets['site']))
      {
         $title = $s_o->getString("site_objects");
         $msg = $s_o->getString("site_objects_intro");
         writeMenuSectionHeader($title, $msg, $cols);
         $this->writeTypeSet($type_sets['site'], $cols);
      }*/
      $this->writeMenuFooter($cols);
   }


   function writeTypeSet(&$types, $cols, &$s_o)
   {
      $onwards = $this->action_req->copy();
      unset($onwards->target_objects);
      $current_col = 0;
      $col_width = (int)100 / $cols;
      foreach($types as $atype)
      {
         if(is_array($atype))
         {
            $onwards->target_objects = array($atype[0]);
            $onwards->action = 'edit';
            $type = $atype[1];
         }
         else
         {
            $onwards->action = 'list';
            $onwards->obj_type = array($atype->type);
            unset($onwards->target_objects);
            $type = $atype;
         }
         if($current_col % $cols == 0) echo '<tr class="admin">';
         $current_col++;
         echo "<td width='$col_width"."%' class='admin'>";
         echo $this->getObjectTypeMenuEntry($type, $onwards, $s_o);
         echo '</td>';
         if($current_col % $cols == 0) echo '</tr>';
      }
      if($current_col % $cols != 0)
      {
         while($current_col % $cols != 0)
         {
            echo '<td class="admin2">&nbsp;</td>';
            $current_col++;
         }
         echo '</tr>';
      }
   }


   //function which draws an entry for a type in the index page of a site section.
   //


   function getObjectTypeMenuEntry(&$type, &$onwards, &$s_o)
   {
      global $performance_test;
      $objs = $this->obj_set->getObjectInfoByTypename($type->type, $this->action_req->getObjectLanguage());
      if($performance_test > 5)  markTime("Fetched Objects Info for type: $type->type");
      echo "<div style='float: right;  text-align: center; margin: 6px 4px 10px 4px;'>";
      echo "<a class='admin' href='".$onwards->getLink()."'>";
      $x = $s_o->getString("manage_all_objects_of_type")." ".$type->name;
      echo "<img alt='$x' title='$x' border=0 src='";
      echo $type->getTypeIcon('medium') . "'></a><br>";
      //echo "<a class='admin' href='".$onwards->getLink()."'>";
      //echo $s_o->getString("view_all");
      //echo '</a> ';
      echo "</div>";
      echo '<h4 class="admin">';
      echo '<a class="sitebuilder-filter" href="'.$onwards->getLink().'">';
      echo $type->name;
      echo "</a></h4>\n";
      echo '<p class="admin" style="font-size: 0.8em">'.$type->getMeta('description')."</P>";
      if(count($objs) > 0)
      {
         echo "<P style='vertical-align: bottom; clear: both; text-align: center;'>";
         echo $this->getObjectTypeSelect($type->type, $objs, $onwards, $s_o);
         echo "</P>";
      }
      else
      {
         echo "<p class='object-listing-count'>0 data objects configured</p>";
      }
      if($type->not_translated){
         echo '<p class="editornote" style=""><small><i>'.$s_o->getString("type_not_translated").'</i></small></p>';
      }
   }


   function getObjectTypeSelect($type_id, &$obj_info, &$onwards, &$s_o)
   {
      global $OSCAILT_SCRIPT;
      $save_act = $onwards->action;
      $onwards->action = "edit";
      $formname = "obj_choose_form_".$type_id;
      ?>
      <form name="<?=$formname?>" onChange="JavaScript:document.<?=$formname?>.submit()" method='GET' action='<?=$OSCAILT_SCRIPT?>'>
      <?
      echo $onwards->getHiddenQuery();
      if($onwards->obj_id != "" )
      {
         echo "<input type='hidden' name='obj_id' value='$onwards->obj_id'>\n";
      }
      echo "<select name='target_id' class='object-name-list'>\n";
      $x = "Object";
      if(count($obj_info) > 1 or count($obj_info) == 0) $x .= "s";
      echo "<option value=''>".count($obj_info)." Data $x configured</option>";
      foreach($obj_info as $id => $info)
      {
         echo "<option value='$id'>$info</option>";
      }
      echo "</select>";
      if(count($obj_info) > 0)
      {
         echo "<input type='submit' name='edito' class='object-name-list' value='edit'>";
      }
      echo "</form>";
      $onwards->action = $save_act;
      //<input class='object-name-list' type='submit' name='button' value='Edit &gt;&gt;'>
   }


   /************************************************************************/
   /* Functions for writing the object listing page                        */   
   /************************************************************************/
   function writeObjectListPage(&$site_options)
   {
      $lang_code = $this->action_req->getObjectLanguage();
      $this->loadTemplateSet();
      if(!isset($this->action_req->obj_type) or !is_array($this->action_req->obj_type) or count($this->action_req->obj_type) <= 0)
      {
         $this->action_req->obj_type = array("*");
         $type_id = "*";
      }
      if(in_array("*", $this->action_req->obj_type))
      {
         $type_name = $site_options->getString("all")." ".$site_options->getString("indy_objects");
         $intro = $site_options->getString("object_listing_intro");
         $type_id = "*";
      }
      else
      {
         $type_id = $this->action_req->getObjectType();
         $type_obj = $this->obj_set->supportedTypes->getType($type_id);
         $type_name = $type_obj->name;
      }
      $navs = array();
      $navs = $this->getNavigArray($site_options, $navs, $type_id, $type_name);
      $filter_str = $this->getListFilters($site_options->getString("object_filter"));
      $nav_str = $this->getNavigation($navs);


      $title_text = $type_name." ".$site_options->getString("object_listing");
      $intro = $site_options->getString("object_listing_intro");
      addToPageTitle($site_options->getString("object_listing"));
      addToPageDescription($intro);
      writeAdminPageHeader($title_text, $nav_str, $intro, $filter_str);
      if(in_array("*", $this->action_req->obj_type))
      {
         $this->obj_set->drawAllObjectsAsLists($lang_code, $this->template_set);
      }
      else
      {
         foreach($this->action_req->obj_type as $t)
         {
            $this->obj_set->drawObjectListByType($t, $lang_code, $this->template_set);
         }
      }
   }


/*
   function writeSiteExportPage(&$site_options, $exporter)
   {
      $navs = $this->getNavigArray($site_options, $navs, "", "");
      $item = $site_options->getItemByName("site-export");
      $navs[] = $item->getMeta("title");
      $filter_str = "";
      $nav_str = $this->getNavigation($navs);
      $title_text = $item->getMeta("title");
      $intro = $item->getMeta("description");
      addToPageTitle($title_text);
      addToPageDescription($intro);
      writeAdminPageHeader($title_text, $nav_str, $intro, $filter_str);
      $cols = 2;
      $this->writeMenuHeader();
      $linkbase = $this->action_req->getPage();
      $hidden = $this->action_req->getHiddenQuery();
      echo "<form name='editobjectform' action='".$linkbase."' method=post>\n";
      echo "$hidden\n";
      $exporter->writeChoiceForm($cols);
      $this->writeCategorySection($cols);
      $this->writeObjectSection($cols);
      $this->writeChooseCollectionSection($cols);
      $this->writeExportSubmit($cols);
      $this->writeMenuFooter($cols);
      echo "</form>";
   }


   function writeConfirmSiteExportPage(&$site_options, $exporter)
   {
      $this->writeConfirmPageHeader();
      $exporter->writeHiddenChoices();
      $warning = $site_options->getString("are_you_sure");
      $buttons = "<p>".$exporter->getExportChoicesInfo();
      $act = ucfirst($this->action_req->getAction());
      $title = $site_options->getString("confirm")." $act";
      $buttons .= $this->getConfirmButtons($act, $site_options);
      $type = $this->action_req->getObjectType();
      $id = $this->action_req->getTargetObject();
      if(!$id) $id = "";
      $notify_level = $this->site_obj->getMeta("notify_editobject_level");
      $reqd = $this->action_req->requiresNotify($notify_level);
      echo $this->getNotifyForm($title, $warning, $act, $type, $id, $reqd, $buttons);
      $this->writeConfirmPageFooter();
   }
//functionality moved to exporter / importer objects
*/


   /************************************************************************/
   /* Functions for writing the object editing page                        */
   /************************************************************************/
   function writeEditPage(&$indy_obj, &$site_options, $tr_mode)
   {
      if(!$indy_obj)
      {
         $indy_obj = $this->loadObjectForEditing();
      }
      if(!$indy_obj)
      {
         $this->setUserMessage( array(USER_ERROR, "Failed to Load Required Data, No Editing Possible", "x3f" ));
         return false;
      }
      $action = $this->action_req->getAction();
      if($action == 'edit')
      {
         $this->writeSpecificObjectHeader($indy_obj, $site_options, 'edit', "object_edit_intro");
      }
      elseif($action == 'translate')
      {
         $indy_obj->language = $this->action_req->getObjectLanguage();
         $this->writeSpecificObjectHeader($indy_obj, $site_options, 'translate', "object_translate_intro");
      }
      elseif($action == 'create')
      {
         $indy_obj->language = $this->action_req->getObjectLanguage();
         $this->writeSpecificObjectHeader($indy_obj, $site_options, 'create', "object_create_intro");
      }
      $this->setUserMessages($indy_obj->getUserMessages());
      $this->writeEditFormHeader();
      $this->writeUserMessageBox();
      if(!$indy_obj->drawAsForm($this->obj_set, $tr_mode))
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessages($indy_obj->getUserMessages());
         $this->setUserMessage( array(USER_ERROR, "Encountered Internal Error while drawing edit form for object $indy_obj->objref", "x3z" ));
         return false;
      }
      $this->writeEditFormFooter();
      return true;
   }


   function writeSpecificObjectHeader(&$indy_obj, &$site_options, $action_string, $intro_string, $get_actions = true)
   {
      global $languageList;
      if($indy_obj->getLangCode() == 'master')
      {
         $lang_str = " - Master Copy";
      }
      elseif(count($languageList->languages) > 1)
      {
         $lang = $languageList->getLanguageByPrefix($indy_obj->getLangCode());
         $lang_str = " - ".$lang->language_name."";
      }
      else
      {
         $lang_str = "";
      }
      $type_id = $indy_obj->type;
      $type_obj = $this->obj_set->supportedTypes->getType($type_id);
      $type_name = $type_obj->name;
      $filter_str = "";
      if($get_actions && $this->action_req->getAction() != 'create')
      {
         $filter_str = $indy_obj->getActionOptions($this->obj_set);
      }


      $nav_str = $this->getSpecificObjectNavig($indy_obj, $site_options);
      $title_text = $site_options->getString($action_string). " $type_name ";
      if($this->action_req->getAction() != 'create')
      {
         $title_text .= $indy_obj->id();
      }
      $title_text .= $lang_str;
      $intro = $site_options->getString($intro_string);


      addToPageTitle($title_text);
      addToPageDescription($indy_obj->getMeta("shortdescription"));
      writeAdminPageHeader($title_text, $nav_str, $intro, $filter_str);


   }


   function writeEditFormHeader()
   {
      $linkbase = $this->action_req->getPage();
      $hidden = $this->action_req->getHiddenQuery();
      echo "<form name='editobjectform' action='".$linkbase."' method=post>\n";
      echo "$hidden\n";
   }


   function writeEditFormFooter()
   {
      echo "<div class='submit'><input type=submit name='cancel' value='<< Cancel'>";
      echo "<input type=submit name='preview' value='Preview >>'>";
      echo "</div></form>\n";
   }




   /************************************************************************/
   /* Functions for writing the object view page                           */
   /************************************************************************/
   function writeViewPage(&$indy_obj, $context, &$site_options)
   {
      if($context == 'xmlview')
      {
         $this->writeSpecificObjectHeader($indy_obj, $site_options, "view_xml", "view_xml_intro");
      }
      else
      {
         $this->writeSpecificObjectHeader($indy_obj, $site_options, "view_displays", "view_displays_intro");
      }
      $this->writeUserMessageBox();
      if(!$indy_obj->writeObjectViews($this->obj_set, $context, "menuobject"))
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessages($indy_obj->getUserMessages());
         $this->setUserMessage( array(USER_ERROR, "Encountered Internal Error while drawing view of object $indy_obj->objref", "x3z" ));
         return false;
      }
      writeAdminPageFooter();
      return true;
   }




   /************************************************************************/
   /* Functions for writing the import page                                */
   /************************************************************************/
   function writeImportPage(&$site_options)
   {
      //not implemented - just get a confirm page
   }


   /************************************************************************/
   /* Functions for writing the preview page                               */
   /************************************************************************/
    function writePreviewPage(&$indy_obj, &$site_options, $tr_mode)
    {
      $this->writeSpecificObjectHeader($indy_obj, $site_options, "preview", "preview_intro", false);


      if(!$indy_obj->writeObjectViews($this->obj_set, 'preview', "menuobject"))
      {
         $this->setUserMessages($this->obj_set->getUserMessages());
         $this->setUserMessages($indy_obj->getUserMessages());
         $this->setUserMessage( array(USER_ERROR, "Encountered Internal Error while drawing preview of object $indy_obj->objref", "x14" ));
         return false;
      }
      $this->writePreviewFormHeader();
      $indy_obj->drawAsHidden($this->obj_set, $tr_mode);
      $warning = $site_options->getString("are_you_sure");
      $title = $site_options->getString("save_changes");
      $notify_level = $this->site_obj->getMeta("notify_editobject_level");
      $reqd = $this->action_req->requiresNotify($notify_level);
      echo $this->getNotifyForm($title, $warning, $this->action_req->getAction(), strtolower($indy_obj->getType()), $this->action_req->getTargetObject(), $reqd, $this->getPreviewFormButtons($site_options, $tr_mode));


      echo "</form>";
      if($tr_mode == false && $indy_obj->getLangCode() != 'master')
      {
         $this->writePreviewFormHeader();
         echo "<div class='edit-again'><a name='edit-again'></a>\n";
         if(!$indy_obj->drawAsForm($this->obj_set, $tr_mode))
         {
            $this->setUserMessages($this->obj_set->getUserMessages());
            $this->setUserMessages($indy_obj->getUserMessages());
            $this->setUserMessage( array(USER_ERROR, "Encountered Internal Error while drawing edit form for object $indy_obj->objref", "x3z" ));
            return false;
         }
      }
      $this->writePreviewPageFooter($tr_mode or $indy_obj->getLangCode() == 'master');
      return true;
    }


    function writePreviewFormHeader()
    {
      $this->writeEditFormHeader();
    }


   function getPreviewFormButtons(&$site_options, $tr_mode=false)
   {
      global $system_config;
      $cancel_text = $site_options->getString("cancel_button");
      $save_text = $site_options->getString("save_changes_button");


      $edit_again_text = $site_options->getString("edit_again");
      $edit_again_lnk = "<a href='#edit-again'>$edit_again_text</a>";
      $str = "<div class='editorconfirmbuttons'><input type=submit name='cancel' value='<< $cancel_text'>";
      $str .= "<input type=submit name='confirm' value='$save_text >>'>";
      if($tr_mode == false) $str .= " $edit_again_lnk\n";
      $str .= $this->writeConfirmAndRecacheOption($site_options);
      $str .= "</div>\n";
      return $str;
   }


   function writePreviewPageFooter($tr_mode=false)
   {
      if($tr_mode) echo "</form>\n";
      else $this->writeEditFormFooter();
      writeAdminPageFooter();
   }




   function writeExportPage(&$site_options)
   {
      echo "<p class=error>NOT IMPLEMENTED!!!</p>";
   }


   /************************************************************************/
   /* Functions to draw the confirm page                           */
   /************************************************************************/
   function writeConfirmPageHeader()
   {
      $linkbase = $this->action_req->getPage();
      $hidden = $this->action_req->getHiddenQuery();
      echo "<form name='confirmactionform' style='display: inline; margin: 0;' action='".$linkbase."' method=post>\n";
      echo "$hidden\n";
   }


   function writeConfirmPageFooter()
   {
      echo "</form>\n";
      writeAdminPageFooter();
   }


   function getConfirmButtons($action, &$site_options)
   {
      $cancel_text = $site_options->getString("cancel_button");
      $confirm_text = $site_options->getString("confirm")." $action";
      $str = "<div class='editorconfirmbuttons'><input type=submit name='cancel' value='<< $cancel_text'>";
      $str .= "<input type=submit name='confirm' value='$confirm_text >>'>";
      return $str;
   }


   function writeConfirmExportPage(&$site_options)
   {
      $this->writeConfirmPageHeader();
      $warning = $site_options->getString("are_you_sure");
      $buttons = "<p>".$site_options->getString("enter_template_name");
      $buttons .= "<input type='text' name='export_name' value=''></p>";
      $act = ucfirst($this->action_req->getAction());
      $title = $site_options->getString("confirm")." $act";
      $buttons .= $this->getConfirmButtons($act, $site_options);
      $type = $this->action_req->getObjectType();
      $id = $this->action_req->getTargetObject();
      if(!$id) $id = "";
      $notify_level = $this->site_obj->getMeta("notify_editobject_level");
      $reqd = $this->action_req->requiresNotify($notify_level);
      echo $this->getNotifyForm($title, $warning, $act, $type, $id, $reqd, $buttons);
      $this->writeConfirmPageFooter();
   }


   function writeConfirmActionPage(&$site_options, $extra_html="")
   {
      $this->writeConfirmPageHeader();
      $warning = $site_options->getString("are_you_sure");
      $act = ucfirst($this->action_req->getAction());
      $title = $site_options->getString("confirm")." $act";
      $buttons = $this->getConfirmButtons($act, $site_options);
      $buttons .= $extra_html;
      $type = $this->action_req->getObjectType();
      $id = $this->action_req->getTargetObject();
      if(!$id) $id = "";
      $notify_level = $this->site_obj->getMeta("notify_editobject_level");
      $reqd = $this->action_req->requiresNotify($notify_level);
      echo $this->getNotifyForm($title, $warning, $act, $type, $id, $reqd, $buttons);
      $this->writeConfirmPageFooter();
   }


   function writeConfirmImportPage(&$site_options)
   {
      global $system_config;
      $this->writeConfirmPageHeader();
      $buttons = "<p>".$site_options->getString("enter_import_name");
      $buttons .= "<input type='text' name='import_name' value=''><br />";
      $buttons .= $site_options->getString("force_import_ids");
      $buttons .= " <input type='checkbox' name='force_import_ids'></p>";
      $act = ucfirst($this->action_req->getAction());
      $warning = $act.": ". $site_options->getString("are_you_sure")."?";
      $title = $site_options->getString("confirm")." $act";
      $buttons .= $this->getConfirmButtons($act, $site_options);
      $buttons .= $this->writeConfirmAndRecacheOption($site_options);
      $type = $this->action_req->getObjectType();
      $id = $this->action_req->getTargetObject();
      if(!$id) $id = "";
      $notify_level = $this->site_obj->getMeta("notify_editobject_level");
      $reqd = $this->action_req->requiresNotify($notify_level);
      echo $this->getNotifyForm($title, $warning, $act, $type, $id, $reqd, $buttons);
      $this->writeConfirmPageFooter();
   }


   function writeConfirmAndRecacheOption(&$site_options)
   {
      $ret = "";
      $publish_req = $this->action_req->copy();
      $publish_req->action = 'recache';
      global $system_config;
      if(!$system_config->auto_cache_objects && !$system_config->use_live_objects && $publish_req->checkPermissions("write"))
      {
         $publish_intro = $site_options->getString("publish_changes_intro");
         $ret = "<p class='publish-changes'>$publish_intro <input type='checkbox' name='confirm-publish' /></p>\n";
      }
      return $ret;
   }


   function writeSiteFilesPage(&$s_o)
   {
      $navs = $this->getNavigArray($s_o, $navs, "", "");
      $nav_str = $this->getNavigation($navs);
      $title_text = $this->header_prefix . " - ".$s_o->getString("file_management");
      $icon = $s_o->getIcon("small");
      $filters = $this->getSiteActionsHTML($s_o);
      $intro_text = $s_o->getString("site_builder_intro");
      addToPageTitle($s_o->getString("file_management"));
      writeAdminPageHeader($title_text, $nav_str, $intro_text, $filters);
      $this->writeSubPageHeader();
      if(isset($_REQUEST['editstyle']) && ($_REQUEST['editstyle'] == 'common.css'  or $_REQUEST['editstyle'] == 'style.css' or $_REQUEST['editstyle'] == 'contentstyle.css') && !isset($_REQUEST['cancelstylee']))
      {
         $result = $this->writeStyleEditSection($s_o, $_REQUEST['editstyle']);
         $this->writeUserMessageBox();
      }
      elseif(isset($_REQUEST['file_upload_button']))
      {
         $this->uploadFiles();
         $this->writeUserMessageBox();
      }
      $this->writeStyleSheetSection($s_o);
      $this->writeGraphicsSection($s_o);
      $this->writeFileSummarySection($s_o);
      writeAdminPageFooter();
   }

   function writeSubPageHeader()
   {
       // Basically display the path to the site files.
       global $system_config, $pseudo_directory_offset;
       $t_site_id = $this->action_req->getSiteFilter();
       $st_store = $pseudo_directory_offset.$system_config->site_file_store_base.'site_'.$t_site_id.'/';
       ?>
       <div class="sitebuilder-header"> <b> <?=$st_store?> </b>
       <?
       if ($this->site_obj->getMeta("different_css") != "on")
       {
         echo "<br />";
         echo 'Default CSS files in use.'; 
       }
       ?>
       </div>
       <?
   }


   function writeStyleEditSection(&$site_options, $target)
   {
      global $system_config, $pseudo_directory_offset, $OSCAILT_SCRIPT;
      $obj_id = $this->action_req->getSiteFilter();
      $file_target = $pseudo_directory_offset.$system_config->site_file_store_base.'site_'.$obj_id.'/'.$target;
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
      $lnk = $this->action_req->getLink()."&amp;editstyle=$target";
      echo "<br /><div align = 'center'>";
      echo "<form name='editstyle' action='$lnk' method='POST'>";
      echo "<textarea rows='50' cols='400' name='style-contents'>".htmlspecialchars($contents, ENT_QUOTES)."</textarea>";
      $cancel_text = $site_options->getString("cancel_button");
      $confirm_text = $site_options->getString("save_changes");
      echo "<div class='editorconfirmbuttons'><input type=submit name='cancelstylee' value='<< $cancel_text'>";
      echo "<input type=submit name='confirm' value='$confirm_text >>'>";
      echo "</div></div>";
   }


   function writeFileSummarySection(&$s_o)
   {
      global $system_config, $pseudo_directory_offset, $OSCAILT_SCRIPT;
      $obj_id = $this->action_req->getSiteFilter();
      $st_store = $pseudo_directory_offset.$system_config->site_file_store_base.'site_'.$obj_id.'/';
      $section_title = $s_o->getString("file_summary");
      $section_intro = $s_o->getString("file_summary_intro");
      echo "<br />";
      echo '<div class="object-summary-details">';
      echo "<h4 class='admin'>$section_title</h4>";
      echo "<p>$section_intro</p>";


      if ($handle = opendir($st_store))
      {
         $myfiles = array();
         while (false !== ($file = readdir($handle)))
         {
             if ($file != "." && $file != ".." && $file != 'contentstyle.css' && $file != 'common.css' && $file != 'style.css')
             {
                   $attachment = new Attachment();
                   $attachment->attachment_file = $st_store."/". $file;
                   $attachment->determineTypeFromFileName();
                   $icon = $attachment->getIconTag();

		   if (isset($_REQUEST['show_file_size']) && $_REQUEST['show_file_size'] == 'true') {
                       $fsize = filesize($st_store.$file);
		       if ($fsize > 1048576 ) $fsize = round(100 * $fsize /1048576 ) / 100 . " Mb";
		       else if ($fsize > 1024 ) $fsize = round(100 * $fsize /1024 ) / 100 ." kb";
		       else $fsize .= " bytes";

                       $myfiles[] = $icon." <a href='$st_store".$file."'>$file</a> &nbsp;". $fsize;
		   } else {
                       $myfiles[] = $icon." <a href='$st_store".$file."'>$file</a>";
		   }
             }
         }
         closedir($handle);
         if(count($myfiles) > 0)
         {
               echo "<UL class='file-list'>";
               foreach($myfiles as $f)
               {
                  echo "<LI class='file-list'>$f</LI>";
               }
               echo "</UL>";
         }
	 echo "<br />";

         $lnk = $this->action_req->getLink();
	 echo "<FORM name='show_files' action='".$lnk."' method='POST'>";

	 if (isset($_REQUEST['show_file_size']) && $_REQUEST['show_file_size'] == 'true') {
	     echo "<INPUT class='publish' type='submit' value='Show Filenames Only' >";
	     echo "<INPUT type='hidden' name='show_file_size' value='false' >";
	 } else {
	     echo "<INPUT class='publish' type='submit' value='Show File Sizes' >";
	     echo "<INPUT type='hidden' name='show_file_size' value='true' >";
	 }
	 echo "</FORM>";


      }
      else
      {
         echo "<P>Failed to read $st_store directory</P>";
      }
      echo "</div>";
   }


   function writeStyleSheetSection(&$s_o)
   {
      global $system_config, $pseudo_directory_offset, $OSCAILT_SCRIPT;
      $obj_id = $this->action_req->getSiteFilter();
      $st_store = $pseudo_directory_offset.$system_config->site_file_store_base.'site_'.$obj_id.'/';
      $css_1 = 'common.css';
      $css_2 = 'style.css';
      $css_3 = 'contentstyle.css';
      if(file_exists($st_store.$css_1) or file_exists($st_store.$css_2) and file_exists($st_store.$css_3))
      {
         $section_title = $s_o->getString("style_management");
         $section_intro = $s_o->getString("style_intro");
         $common_lnk = "<a href='".$this->action_req->getLink()."&amp;editstyle=$css_1'>";
         $style_lnk = "<a href='".$this->action_req->getLink()."&amp;editstyle=$css_2'>";
         $contentstyle_lnk = "<a href='".$this->action_req->getLink()."&amp;editstyle=$css_3'>";
         $edit = $s_o->getString("edit");
         echo "<br />";
         echo '<div class="object-summary-details">';
         echo "<h4 class='admin'>$section_title</h4>";
         echo "<p>$section_intro</p>";
         echo "<P>$common_lnk"."$edit $css_1</a> | $style_lnk"."$edit $css_2</a> | $contentstyle_lnk"."$edit $css_3</a></P>";
         echo '</div>';
      }
   }


   function writeGraphicsSection(&$s_o)
   {
      echo "<br />";
      $max_files = 5;
      $section_title = $s_o->getString("file_uploads");
      $section_intro = $s_o->getString("uploads_intro");
      echo "<br />";
      echo '<div class="object-summary-details">';
      echo "<h4 class='admin'>$section_title</h4>";
      echo "<p>$section_intro</p>";
      $lnk = $this->action_req->getLink();
      if(!isset($_REQUEST['file_number_button']))
      {
         echo "<form name='choose_files' action='$lnk' method='POST'>";
         echo $s_o->getString('choose_file_number');
         echo " <SELECT class='publish' name='number_of_files'>";
         for($i=1;$i<=$max_files;$i++)
         {
            echo "<OPTION>$i</OPTION>\n";
         }
         echo "</SELECT>\n";
         echo "<INPUT class='publish' type='submit' value='".$s_o->getString('file_number_button_text')."'";
         echo "name='file_number_button'>";
         echo "</form>";
      }
      else
      {
         if($_REQUEST['number_of_files'] > 0 && $_REQUEST['number_of_files'] <= $max_files)
         {
            $file_num = $_REQUEST['number_of_files'];
         }
         else $file_num = 1;
         echo "<form enctype='multipart/form-data' name='choose_files' action='$lnk' method='POST'>";
         echo "<input type='hidden' name='number_of_files' value='$file_num'>";
         echo $s_o->getString('choose_files');
         for($i=1;$i<=$file_num;$i++)
         {
            echo "<P align='center'><b>".$s_o->getString("choose_upload");
            echo "</b> <input type='file' name='file_".$i."' size='30'></p>\n";
         }
         $cancel_text = $s_o->getString("cancel_button");
         $confirm_text = $s_o->getString("file_upload_button_text");
         echo "<div class='editorconfirmbuttons'><input type=submit name='cancelstylee' value='<< $cancel_text'>";
         echo "<input type=submit name='file_upload_button' value='$confirm_text'>";
         echo "</div>";
         echo "</form>";
      }
      echo '</div>';
   }


   function uploadFiles()
   {
      global $system_config, $pseudo_directory_offset, $OSCAILT_SCRIPT;
      $obj_id = $this->action_req->getSiteFilter();
      $st_store = $pseudo_directory_offset.$system_config->site_file_store_base.'site_'.$obj_id.'/';
      $max_files = 5;
      $attachments = array();
      if(isset($_REQUEST['number_of_files']) && $_REQUEST['number_of_files']>0 && $_REQUEST['number_of_files'] <= $max_files)
      {
         $num_files = $_REQUEST['number_of_files'];
         $fine_n_dandy = true;
         for($i=1;$i<=$num_files;$i++)
         {
            $attachment = new Attachment();
            $result = $attachment->createFromUpload('file_'.$i,true, $st_store);
            if($result!="OK")
            {
               $this->setUserMessage( array ( USER_ERROR, $result, "" ));
               $fine_n_dandy = false;
            }
         }
      }
      else
      {
         $this->setUserMessage( array ( USER_ERROR, "Invalid Number of Files $num_files", "" ));
         return false;
      }
      if($fine_n_dandy)
      {
         $this->setUserMessage( array ( USER_MSG, "$num_files file(s) uploaded successfully.", "" ));
      }
      return true;
   }


   /************************************************************************/
   /* Functions for writing the results page                               */
   /************************************************************************/
   function writeResultPage($result, &$site_options)
   {
      global $OSCAILT_SCRIPT, $system_config;
      if(!$result)
      {
         $this->writeUserMessageBox();
         return;
      }
      if($system_config->auto_cache_objects or (isset($_REQUEST['confirm-publish']) && $_REQUEST['confirm-publish'] == 'on') or $system_config->use_live_objects)
      {
         $published = true;
      }
      else
      {
         $published = false;
      }
      $show_publish_options = false;


      $action = $this->action_req->getAction();
      $options = array();
      if($OSCAILT_SCRIPT == 'admin.php')
      {
         $options[] = "<a href='$OSCAILT_SCRIPT'>".$site_options->getString("oscailt_administration")."</a>";
      }
      else
      {
         $options[] = "<a href='$OSCAILT_SCRIPT"."?obj_id=".$this->action_req->getSiteFilter()."'>".$site_options->getString("site_administration")."</a>";
      }


      if($action == 'delete')
      {
           $msg = $site_options->getString("delete_success");
           $show_publish_options = true;
      }
      elseif($action == 'site-recache')
      {
           $msg = $site_options->getString("site_recache_success");
      }
      elseif($action == 'site-export')
      {
           $msg = $site_options->getString("site_export_success");
      }
      elseif($action == 'import')
      {
           $msg = $site_options->getString("import_success");
           if(!$published)
           {
            $show_publish_options = true;
           }
      }
      else
      {
         if(!$published)
         {
            $show_publish_options = true;
           }
           if($action == 'create')
           {
              $indy_obj = $this->obj_set->fetchObject($this->action_req->getTargetObject(), $this->action_req->getObjectLanguage());
           }
           else
           {
            $indy_obj = $this->loadObjectForEditing();
         }
         $back_link = $this->action_req->copy();
         $back_link->populateFromEntry();
         $href = "href='$OSCAILT_SCRIPT".$back_link->getQueryString()."'";
         $options[] = "<a $href>".$site_options->getString("leave_sitebuilder")."</a>";


         $edit_again_link = $this->action_req->copy();
         $edit_again_link->action = "edit";
         $href = "href='$OSCAILT_SCRIPT".$edit_again_link->getQueryString()."'";
         $options[] = "<a $href>".$site_options->getString("edit_again")."</a>";


         $msgbase = $indy_obj->name()." <small>(ID: $indy_obj->objref | TYPE: ".ucfirst(strtolower($indy_obj->type)).")</small>";




         if($indy_obj->getPageLayoutRef() !== false)
         {
            if($show_publish_options == false)
            {
               $href = "href='index.php"."?obj_id=$indy_obj->objref'";
               $str = $site_options->getString("view_main_page");
               $options[] = "<a $href>$str</a>";
            }
            else
            {
               $str = $site_options->getString("view_main_page");
               $href = "href='index.php"."?obj_id=$indy_obj->objref'";
               $href2 = "href='test.php"."?obj_id=$indy_obj->objref'";
               $str_live = $site_options->getString("view_live_object");
               $str_cache = $site_options->getString("view_cache_object");
               $options[] = "$str: <a $href2>$str_live</a> | <a $href>$str_cache</a>";
            }
         }
         else
         {
            if($indy_obj->isStructure())
            {
               $options[] = $site_options->getString("noview_structural");
            }
            elseif($indy_obj->isScreenElement())
            {
               $options[] = $site_options->getString("noview_menu");
               $options = $this->getPageLink($options);
            }
            else
            {
               $options[] = $site_options->getString("noview_element");
               $options = $this->getScreenElementLinks($options);
            }
         }
         if($action == 'create')
         {
            $msg = $msgbase." ".$site_options->getString("create_success");
            if($indy_obj->isStructure())
            {
            }
            elseif($indy_obj->isScreenElement())
            {
               $msg .= $site_options->getString("create_nopageview");
            }
            else
            {
               $msg .= $site_options->getString("create_nomenuview");
            }
         }
         elseif($action == 'edit')
         {
            $msg = $msgbase." ".$site_options->getString("update_success");
         }
         elseif($action == 'translate')
         {
            $msg = $msgbase." ".$site_options->getString("translate_success");
         }
         elseif($action == 'recache')
         {
              $msg = $site_options->getString("cache_success");
              $show_publish_options = false;
         }
         elseif($action == 'export')
         {
             $msg = $site_options->getString("export_success");
              $show_publish_options = false;
         }
      }


      echo "<div class='user-message'><h3 align=center>$msg</h3>";
      if($show_publish_options)
      {
         echo "<P>".$site_options->getString("notpublished_intro");
         echo "</P>";
         $options[] = "<a href='index.php'>".$site_options->getString("browse_cache_site")."</a>";
         $options[] = "<a href='test.php'>".$site_options->getString("browse_live_site")."</a>";
      }


      if(count($options) > 0)
      {
         echo "<ul>";
         foreach($options as $opt)
         {
            echo "<li>$opt</li>";
         }
         echo "</ul>";
      }
      echo "</div>\n";
      $this->writeUserMessageBox();


      return true;
   }


   function getPageLink(&$options)
   {
      $onwards = $this->action_req->copy();
      $onwards->action = 'list';
      $onwards->obj_type = array('page');
      $lnk = "<a href='".htmlspecialchars($onwards->getQueryString())."'>View Configured Page Layouts</a>";
      $options[] = $lnk;
      return $options;
   }


   function getScreenElementLinks(&$options)
   {
      $onwards = $this->action_req->copy();
      $onwards->action = 'list';
      $onwards->obj_type = array('menu');
      $lnk = "<a href='".htmlspecialchars($onwards->getQueryString())."'>View Configured Menus</a>";
      $str = $lnk;
      $onwards->obj_type = array('bar');
      $lnk = "<a href='".htmlspecialchars($onwards->getQueryString())."'>View Configured Horizontal Bars</a>";
      $str .= " | $lnk";
      $onwards->obj_type = array('box');
      $lnk = "<a href='".htmlspecialchars($onwards->getQueryString())."'>View Configured Inset Boxes</a>";
      $str .= " | $lnk";
      $onwards->obj_type = array('list');
      $lnk = "<a href='".htmlspecialchars($onwards->getQueryString())."'>View Configured Lists</a>";
      $str .= " | $lnk";
      $options[] = $str;
      return $options;
   }

}
?>
