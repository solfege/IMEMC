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
/* This class is a collection of indymedia data objects                 */
/* It can be stored either in a Database Table, or in a xml file        */
/* It does the hard work of maintaining object relations                */
/* It also manages the loading of the object xml files from storage     */
/* It firstly loads stubs of objects and caches of their details (names)*/
/* and only loads objects from storage when they are fetched            */
/************************************************************************/
class indyObjectSet extends indyObject
{
    //internal storage of contents
    var $itemCollection = array();
    //internal state registers
    var $openObjects = array();
    var $updatedObjects = array();
    var $deletedObjects = array();
    var $needRecachingObjects = array();
    var $newMasterObjects = array();
    var $newObject;
    var $createdObjects = array();
    var $updatedMasterObjects = array();
    var $deletedMasterObjects = array();
    var $available_languages_cache = array();//object languages cache.

    var $supportedTypes; //typeset object
    var $object_name_cache; //name cache object

    //Directory that contain the type definition xml files
    var $typeDirectory;

    //things for the parser
    var $parser;
    var $inObjSet = FALSE;
    var $currentObject;
    var $currentObjectIndex;

    //how the set is stored
    var $storage;
    var $default_storage = 'sql'; //can be sql or a file path
    var $sites;
    var $index_file_map = array();
    //object representing the request -> to allow me to build links on management pages
    var $calling_req;

   /************************************************************************/
   /* Constructor          */
   /* Input: filename of type set, storage
   /* Output: none          */
   /************************************************************************/
   function indyObjectSet( $typedir, $storage="sql")
   {
      global $system_config;
      $this->typeDirectory = $typedir;
      $this->storage = $storage;
      $this->object_name_cache = new indyObjectNameCache();//speed up cache of object names
   }

   function getAllSiteIds()
   {
      $all_sites = array();
      if($this->storage == 'sql')
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT DISTINCT object_id FROM ".$prefix."_master_data_objects WHERE object_type = 'SITE'", $dbconn, 4);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {
               list($site_id) = sql_fetch_row($result, $dbconn);
               $all_sites[] = $site_id;
            }
         }
         else
         {
            return false;
         }
      }
      else
      {
         $this->loadSiteIndices($this->storage);
         $all_sites = array_keys($this->index_file_map);
         if(count($all_sites) <= 0)
         {
            $this->setUserMessage( array ( INDY_ERROR, "Failed to read and sites from site index file in $this->storage","" ) );
            return false;
         }
      }
      $this->sites = $all_sites;
      return true;
   }

   function getAllObjectIDs()
   {
      return array_keys($this->itemCollection);
   }

   function containsObjectID($id)
   {
      if(isset($this->itemCollection[$id])) return true;
      return false;
   }

   function loadSiteIndices($storage)
   {
      if(!file_exists($storage))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to open non-existent site index file stored in $storage","" ) );
         return false;
      }
      $x = pathinfo($storage);
      $indices = file( $storage );
      foreach($indices as $index)
      {
         $bits = explode(" ", trim($index));
         $k = array_shift($bits);
         $v = implode(" ", $bits);
         if($v != "" and $v != "")
         {
            $this->index_file_map[$k] = $x['dirname'].'/'.$v;
         }
      }
   }

   function saveSiteIndices()
   {
      $x = pathinfo($this->storage);
      $str = "";
      foreach(array_keys($this->index_file_map) as $key)
      {
         $f = $this->index_file_map[$key];
         $f = preg_replace("#^".$x['dirname']."/#", "", $f);
         $str .= $key." $f\n";
      }
      if (!$handle = fopen($this->storage, 'w'))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to open output data object file $objfile", "" ));
         return false;
      }
      if (!flock($handle, LOCK_EX))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to get lock for file $objfile", "" ) );
         fclose( $handle );
         return false;
      }
      if ( !fwrite( $handle, $str )  )
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to write to data file $objfile", "" ) );
         flock($handle, LOCK_UN);
         fclose( $handle );
         return false;
      }
      flock($handle, LOCK_UN);
      fclose( $handle );
      //chmod($this->storage, 0644);
      return true;
   }

   /************************************************************************/
   /* Verifies that the sites and types given are supported by this server */
   /************************************************************************/
   function _getVerifiedInputs(&$sites, &$types)
   {
      $this->_getVerifiedSites($sites);
      $this->_getVerifiedTypes($types);
      if(count($sites) == 0 or count($types) == 0) return false;
      return true;
   }

   function _getVerifiedTypes(&$types)
   {
      $n_types = array();
      if(in_array("*", $types))
      {
         $n_types = array("*");
      }
      else
      {
         foreach($types as $type)
         {
            if($this->supportedTypes->supportsType($type))
            {
               $n_types[] = $type;
            }
            else
            {
               $this->setUserMessage( array ( INDY_WARNING, "[$type] is not the name of a type supported by this server.", "3fs") );
            }
         }
      }
      $types = $n_types;
   }

   function _getVerifiedSites(&$sites)
   {
      $n_sites = array();
      if(in_array("*", $sites))
      {
         $n_sites = array("*");
      }
      else
      {
         foreach($sites as $site)
         {
            if(in_array($site, $this->sites))
            {
               $n_sites[] = $site;
            }
            else
            {
               $this->setUserMessage( array ( INDY_WARNING, "[$site] is not the id of a site managed by this server.", "") );
            }
         }
      }
      $sites = $n_sites;
   }


    /************************************************************************/
    /* Loads the basic data for the object set
    /* Loads type data from an xml type file and loads object stubs
    /* Object stubs are storied in an xml file or DB table depending on storage setting  */
    /* Input: arrays of site ids, array type names to load objects for      */
    /* Output:success/failure t/f */
    /************************************************************************/
    function load($sites, $types, &$act_req)
    {
      $this->calling_req = $act_req;
      if($this->_loadTypeSet() === false)
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to load types for this site!", "xcv") );
         return false;
      }
      if($this->getAllSiteIds() === false)
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to load site id's for this oscailt server!", "xcv") );
         return false;
      }
      //input checking
      if($this->_getVerifiedInputs($sites, $types) === false)
      {
         $this->setUserMessage( array ( INDY_ERROR, "Input Problem, no sites/types supported!", "xce") );
         return false;
      }

      if($this->storage == 'sql')
      {
         $result = $this->_loadObjectStubsFromDB($sites, $types);
      }
      else
      {
         $result = $this->_loadObjectStubsFromFile($sites, $types);
      }
      if($result === false)
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to load objects for sites: ".implode(",", $sites)." and types: ".implode(",", $sites), "xcv") );
      }
      return $result;
   }

   /************************************************************************/
    /* Loads the data from the type directory                               */
    /* Input: none                                                          */
    /* Output:success/failure t/f                                           */
    /************************************************************************/
   function _loadTypeSet()
   {
      global $system_config;
      //first load the data type description file and parse it
      $typeSet = new indyDataTypeSet( $this->typeDirectory );
      if( !$typeSet->load())
      {
         $this->setUserMessages ( $typeSet->getUserMessages() );
         $this->setUserMessage ( array( INDY_ERROR,
            "Failed to load the set of data types $this->typeDirectory","AAs" ) );
         return false;
      }
      if( $system_config->debug_level >= DEBUG_LEVEL_1 )
         $this->setUserMessage(array( DEBUG_LEVEL_1,
         "Read the set of datatypes from the file: ".$this->typeDirectory,"" ));
      $this->setUserMessages ( $typeSet->getUserMessages() );
      $this->supportedTypes =& $typeSet;
      return true;
   }

   /************************************************************************/
   /* Loads the data from the database   */
   /* Input: array of site ids and array of site types to load objects for
   /* Output:success/failure t/f */
   /************************************************************************/
   function _loadObjectStubsFromDB(&$sites, &$types)
   {
      global $prefix, $dbconn;
      $where_clauses = array();
      if(!in_array("*", $sites))
      {
         $sub_clauses = count($sites);
         if($sub_clauses > 0)
         {
            $site_where_part = "(site_id = ".$sites[0]." OR object_id =".$sites[0];
            $i = 1;
            while($i < $sub_clauses)
            {
               $site_where_part .= " OR site_id = ".$sites[$i];
               $site_where_part .= " OR object_id =".$sites[$i];
               $i++;
            }
            $site_where_part .= ")";
            array_push($where_clauses, $site_where_part);
         }
      }

      if(!in_array("*", $types))
      {
         if(!in_array("site", $types)) $types[] = 'site';
         $sub_clauses = count($types);
         if($sub_clauses > 0)
         {
            $type_where_part = "(object_type = '".$types[0]."'";
            $i = 1;
            while($i < $sub_clauses)
            {
               $type_where_part .= " OR object_type='".$types[$i]."'";
               $i++;
            }
            $type_where_part .= ")";
            array_push($where_clauses, $type_where_part);
         }
      }

      if(count($where_clauses) > 0) $where_part = " WHERE ".implode(" AND ", $where_clauses);
      else $where_part = "";
	$sql = "SELECT object_id, site_id, object_type, primary_language_code, storage FROM ".$prefix."_master_data_objects $where_part";
      $result = sql_query($sql, $dbconn, 4);
      checkForError($result);
      if(sql_num_rows( $result ) > 0)
      {
         for ($i=0; $i<sql_num_rows( $result ); $i++)
         {

           list($obj_id, $site_id, $object_type, $primary_language_code, $storage) = sql_fetch_row($result, $dbconn);
           $this->itemCollection[$obj_id] = $this->createDataObjectStub($obj_id, $site_id, $object_type, $primary_language_code, $storage);
         }
            return true;
      $this->setUserMessage(array(INDY_WARNING, "No objects found in data base with appropriate site id", ""));
      }
      return true;
   }

   /************************************************************************/
   /* Loads the data from xml file   */
   /* Input: array of site ids and array of site types to load objects for
   /* Output:success/failure t/f */
   /* Note there is an assumption that each site has its own stub file     */
   /************************************************************************/
   function _loadObjectStubsFromFile(&$sites, &$types)
   {
      $loaded_some = false;
      if(in_array("*", $sites))
      {
         $sites = $this->sites;
      }
      foreach($sites as $site)
      {
         if($this->_loadSiteObjectStubsFromFile($site, $types))
         {
            $loaded_some = true;
         }
         else
         {
            $this->setUserMessage( array ( INDY_WARNING, "Failed to load Data File for object stubs with site id $site", "hws") );
         }
      }
      return $loaded_some;
   }

   /************************************************************************/
   /* Loads the data from a single xml file   */
   /* Input: site id and array of types to load objects for
   /* Output:success/failure t/f */
   /************************************************************************/
   function _loadSiteObjectStubsFromFile($site, $types)
   {
      global $system_config;
      $storage = $this->index_file_map[$site];
      if(!in_array($site, $this->sites))
      {
         $this->setUserMessage( array ( INDY_WARNING, "The set of data objects for site $site does not exist", "") );
         return false;
      }
      $item_file = $storage;
      if( !file_exists( $item_file ) )
      {
         $this->setUserMessage( array ( INDY_ERROR, "The set of data objects does not exist.  No file: ".$item_file. "") );
         return false;
      }

      $xmlData = implode('', file($item_file));
      if(($xmlData !== false) && ($this->readXML($xmlData) !== false))
      {
         if( $system_config->debug_level >= DEBUG_LEVEL_1 ) $this->setUserMessage( array ( DEBUG_LEVEL_1, "Read the set of data objects from the file: ".$item_file, "" ) );
         if(!in_array("*", $types))
         {
            $this->_filterTypesOut($types);
         }
         //a hack to support relative paths to storage of object data files...
         $stubs = $this->getObjectsBySite($site);
         foreach($stubs as $k => $item)
         {
            $p = $item->storage;
            if(isRelativePath($p))
            {
               $x = pathinfo($storage);
               $item->real_storage = $x['dirname'].'/'.$p;
            }
            $item->storage = $p;//$st;
            $this->itemCollection[$item->obj_id] = $item;
            //}
         }

         return true;
      }
      else
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to read the set of data objects from the file: ".$item_file, "" ) );
         return false;
      }
   }

   /************************************************************************/
   /* Filters any object stubs out which were loaded from file without type checking
   /* Input: array of good type names
   /* Note: this is carried out as the xml is gulped up in a whole file w/o type discrimination
   /************************************************************************/
   function _filterTypesOut(&$ok_types)
   {
      foreach(array_keys($this->itemCollection) as $item_id)
      {
         if(!in_array(strtolower($this->itemCollection[$item_id]->obj_type), $ok_types))
         {
            unset($this->itemCollection[$item_id]);
         }
      }
   }

   /************************************************************************/
   /* Return Object Stubs filtered by various things                       */
   /* Input: none */
   /* Output: array of indyObjects, indexed by id numbers */
   /************************************************************************/
   function getObjectStubs ()
   {
      return $this->itemCollection;
   }

   function containsObject($id)
   {
       if(isset($this->itemCollection[$id]))
       {
           return true;
       }
       return false;
   }

   function getObjectStub ($id)
   {
       if(isset($this->itemCollection[$id]))
       {
           return $this->itemCollection[$id];
       }
       return false;
   }

   function getObjTypeName($objID)
   {
      $tname = "";
      if(isset($this->itemCollection[$objID]))
      {
         $deObj = $this->itemCollection[$objID];
         $tname = $deObj->obj_type;
      }
      return $tname;
   }


   function getObjectsBySite($site)
   {
      if(!in_array($site, $this->sites))
      {
         $this->setUserMessage( array( INDY_ERROR,
                     "The site $site is not managed by this module",
                      "da9" ) );
                 return false;
      }
      $ret = array();
      foreach ($this->itemCollection as $item)
      {
         if($item->site_id == $site)
         {
            $ret[] = $item;
         }
      }
      return $ret;
   }

   function getObjectStubsByTypename($t)
   {
      if($t == "" or !$this->supportedTypes->supportsType($t))
      {
         $this->setUserMessage( array( INDY_ERROR,
               "The type $t is not supported by this installation",
                "da3" ) );
      }
      $ret = array();
      foreach ($this->itemCollection as $item)
      {
         if(strtolower($item->obj_type) == strtolower($t))
         {
            $ret[] = $item;
         }
      }
      return $ret;
   }

   function getObjectInfoByTypename($t, $langCode, $site_id = null)
   {
      if($t == "" or !$this->supportedTypes->supportsType($t))
      {
         $this->setUserMessage( array( INDY_ERROR,
               "The type $t is not supported by this installation",
                "da3" ) );
      }
      $ret = array();
      foreach ($this->itemCollection as $item)
      {
         if($site_id !== null and $item->site_id != $site_id) continue;
         if(strtolower($item->obj_type) == strtolower($t))
         {
            $this->object_name_cache->load(array(strtoupper($t)));
            $nm = $this->object_name_cache->getObjectName($t, $item->obj_id, $langCode, $item->primary_language_code);
            if($nm === false)
            {
               //echo "<P>did not fetch object name from cache - fetching object $item->obj_id</p>";
               $obj = $this->fetchObject($item->obj_id, $langCode);
               if($obj == false)
               {
                  $this->setUserMessage( array( INDY_ERROR,
                  "Error Loading object $item->obj_id of type $t", "d23" ) );
               }
               else
               {
                  $ret[$obj->id()] = $obj->name();
               }
            }
            else
            {
               $ret[$item->obj_id] = $nm;
            }
         }
      }
      return $ret;
   }

   function getObjectsByTypename ($t, $langCode)
   {
      if($t == "" or !$this->supportedTypes->supportsType($t))
      {
         $this->setUserMessage( array( INDY_ERROR,
               "The type $t is not supported by this installation",
                "da3" ) );
      }
      $ret = array();
      foreach ($this->itemCollection as $item)
      {
         if(strtolower($item->obj_type) == strtolower($t))
         {

            $obj = $this->fetchObject($item->obj_id, $langCode);
            if($obj == false)
            {
               $this->setUserMessage( array( INDY_ERROR,
               "Error Loading object $item->obj_id of type $t", "d23" ) );
               }
            else
            {
               $ret[] = $obj;
            }
         }
      }
      return $ret;
   }

   /************************************************************************/
   /* Finds out which languages an object currently appears in..
   /* Input: object id
   /* Output: array of 2 letter language codes.
   /************************************************************************/
   function getObjectAvailableLanguages($obj_id)
   {
      global $languageList, $prefix, $dbconn;
      $avlangs = array();
      if(!isset($this->itemCollection[$obj_id]))
      {
         $this->setUserMessage( array ( INDY_ERROR,    "Request for languages of non-existant object stub with id[$obj_id]", "w7e"));
         return $avlangs;
      }
      $objStub = $this->itemCollection[$obj_id];
      $primary_language = $objStub->primary_language_code;
      $avlangs[0] = $primary_language;
      if($objStub->storage == 'sql')
      {
         $result = sql_query("SELECT DISTINCT language_code FROM ".$prefix."_data_objects where object_id=$obj_id", $dbconn, 4);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {
                 list($lang_code) = sql_fetch_row($result, $dbconn);
                 if(!in_array($lang_code, $avlangs)) $avlangs[] = $lang_code;
            }
         }
      }
      else
      {
         $languages = $languageList->getLanguages();
         foreach($languages as $lang)
         {
            $lprefix = $lang->getLanguageCodePrefix();
            $lfname = $objStub->real_storage.'_'.$lprefix.'.xml';
            if(file_exists($lfname) && !in_array($lprefix, $avlangs))
            {
               $avlangs[] = $lprefix;
            }            
         }
      }
      return $avlangs;
   }

   function fetchAllObjects(&$all_objs, $langCode)
   {
      foreach($this->itemCollection as $item)
      {
         $obj = $this->fetchObject($item->obj_id, $langCode);
         if(!$obj)
         {
            $this->setUserMessage( array( INDY_ERROR,
            "Failed to fetch Object $item->obj_id","w2x" ));
         }
         else
         {
            $all_objs[$item->obj_id] = $obj;
         }
      }
      return $all_objs;
   }

    /************************************************************************/
    /* Return a full data Object from the xml file or database              */
    /* Input: id number of object                                           */
    /* Output: false | indyObject                                           */
    /************************************************************************/
    function fetchObject($obj_id, $langCode, $force_unmasked = false)
    {
      global $system_config, $legalDataTypes;
      if(!isset($this->itemCollection[$obj_id]))
      {
         if(strpos($obj_id, 'content') !== 0)
         {
            $this->setUserMessage( array( INDY_ERROR,
            "Object $obj_id does not exist in this object set","w2x" ));
         }
         return false;
      }

      //if it's already loaded, return it..
      $objStub = $this->itemCollection[$obj_id];
      if(isset($this->openObjects[$obj_id]) && isset($this->openObjects[$obj_id][$langCode]))
      {
         return $this->openObjects[$obj_id][$langCode];
      }

      //it's not loaded so create a new object...
      $obj_type = $objStub->obj_type;
      if(!in_array(strtolower($obj_type), array_keys($legalDataTypes)))
      {
         $this->setUserMessage( array( INDY_ERROR,
            "Object type $obj_type not supported by this object set","w2x" ));
         return false;
      }

      require_once $system_config->indyobject_code_dir."indy".strtolower($obj_type).".inc";
      $objToCreate = "indy".ucfirst(strtolower($obj_type))."Object";
      $newobj = eval(sprintf("return new %s( \$obj_id, \$obj_type);", $objToCreate));
      //if the object is not brand spanking new, load it from storage..
      if($obj_id == -1)
      {
         $newobj->language = $langCode;
         $newobj->setCaller($this->calling_req);
      }
      else
      {
         //check to see if the master object is loaded...
         if(isset($this->openObjects[$obj_id]) && isset($this->openObjects[$obj_id]["master"]))
         {
            $master = $this->openObjects[$obj_id]["master"];
         }
         else //load master copy of object..
         {
            if($langCode == 'master')
            {
               $master =& $newobj;
            }
            else
            {
               $master = eval(sprintf("return new %s( \$obj_id, \$obj_type);", $objToCreate));
            }
            $result = $master->loadFromStorage('master', $objStub);
            if(!$result)
            {
               $this->setUserMessages( $master->getUserMessages() );
               $this->setUserMessage( array( INDY_ERROR, "Failed to load master data object for object $obj_id",   "sxs"));
               return false;
            }
            $this->openObjects[$obj_id]["master"] = $master;

         }
         if($langCode == 'master')
         {
            $newobj->setCaller($this->calling_req);
         }
         else
         {
            $result = $newobj->loadFromStorage($langCode, $objStub);
            if(!$result)
            {
               if($objStub->primary_language_code != $langCode)
               {
                  if( $system_config->debug_level >= DEBUG_LEVEL_1 )
                  {
                     $this->setUserMessage(array(DEBUG_LEVEL_1,"Failed to load data object [$obj_id] in language [$langCode] trying $objStub->primary_language_code","wsa"));
                  }
                  $newobj = $this->fetchObject($obj_id, $objStub->primary_language_code);
                  if($newobj != false)
                  {
                     $result = true;
                     $newobj->not_translated = true;
                  }
               }
               if(!$result)
               {
                  if($newobj) $this->setUserMessages( $newobj->getUserMessages() );
                  $this->setUserMessage( array( INDY_ERROR,
                     "Failed to load data object for object [$obj_id] in language $langCode","wxs"));
                  return false;
               }
            }
            if(!$force_unmasked && $newobj->not_translated == false)
            {
               $newobj->applyMasterSettings($master);
            }
            $newobj->setCaller($this->calling_req);
            $this->openObjects[$obj_id][$langCode] = $newobj;
         }
         if( $system_config->debug_level >= DEBUG_LEVEL_1 )
         {
            $this->setUserMessage(array(DEBUG_LEVEL_1,"Loaded the data object [$obj_id] language[$langCode] from storage","wsa"));
         }
      }
      return $newobj;
   }

   function unload($id)
   {
      unset($this->openObjects[$id]);
   }

   /*function import(&$other_set, $force_ids)
   {
      //not currently used for some reason - delete.
   }
   */

   /************************************************************************/
   /* Exports an object set to directory specified by $direc               */
   /* Site id is the site section which will be exported                   */
   /* choices is an array of user choices about what to export             */
   /* meta is the item set in which the details of the collection will be  */
   /* stored.
   /************************************************************************/
   function export($direc, $site_id, &$choices, &$meta)
   {
      global $system_config, $languageList;
      $export_links = $choices['links'];
      $update_urls = $choices['update urls'];
      $exported_langs = array();
      $exported_files = 0;
      foreach($choices['language'] as $c)
      {
         $l = $languageList->getLanguageById($c);
         $exported_langs[] = $l->getLanguageCodePrefix();
      }
      $site_path = $direc."site_section_".$site_id."/";
      $sundry_path = $site_path."sundries/";
      foreach(array($site_path, $sundry_path) as $s)
      {
         if(!is_dir($s))
         {
            if(!mkdir($s, $system_config->default_writable_directory_permissions))
            {
               $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $s for export", "ssf5"));
               return false;
            }
            else
            {
               if($system_config->debug_level >= DEBUG_LEVEL_1)
               {
                  $this->setUserMessage(array(
                  DEBUG_LEVEL_1, "Created Directory $s",   "a3dw5"));
               }
            }
         }
      }
      $this->storage = $direc.$meta->name.".index";
      $this->loadSiteIndices($this->storage);
      $this->index_file_map[$site_id] = $site_path."site_section_".$site_id.".xml";
      $export_count = 0;
      foreach(array_keys($this->itemCollection) as $obj_id)
      {
         if($this->itemCollection[$obj_id]->site_id != $site_id)
         {
            continue;
         }
         if(!in_array($obj_id, $choices['obj']))
         {
            //echo "<P>$obj_id removed - not chosen</p>";
            unset($this->itemCollection[$obj_id]);
            continue;
         }
         $obj_storage = $obj_id;
         $av_langs = $this->getObjectAvailableLanguages($obj_id);
         $langs = array();
         foreach($av_langs as $lang)
         {
            if(in_array($lang, $exported_langs)) $langs[] = $lang;
         }
         if(count($langs) == 0)
         {
            unset($this->itemCollection[$obj_id]);
            continue;
         }
         $export_count++;
         if(!in_array($this->itemCollection[$obj_id]->primary_language_code, $langs))
         {
            $this->itemCollection[$obj_id]->primary_language_code = $langs[0];
         }
         array_unshift($langs, 'master');
         foreach($langs as $lang)
         {
            $obj = $this->fetchObject($obj_id, $lang);
            if(!$obj)
            {
               $this->setUserMessage( array( INDY_ERROR,
                  "Failed to load data object for object [$obj_id] in language $lang","wsxs"));
            }
            else
            {
               if($export_links)
               {
               	  $t = $this->supportedTypes->getType($obj->type);
                  $file_attachments = $obj->getLocalFileLinks($t);
                  foreach($file_attachments as $f => $v)
                  {
                     //copy the file to its new home
                     $x = pathinfo($v);
                     $newfile = 'sundries/'.$x['basename'];
                     if(copy($v,$site_path.$newfile))
                     {
                        $exported_files++;
                        $obj->setMetaData($f, $newfile);
                     }
                  }
                  $lnks = $obj->getEmbeddedImageLinks($t);
                  foreach($lnks as $field => $links)
                  {
                     $remapped_urls = array();
                     foreach($links as $l)
                     {
                        $x = pathinfo($l);
                        $newfile = 'sundries/'.$x['basename'];
                        if(!file_exists($newfile) and copy($l,$site_path.$newfile))
                        {
                           $exported_files++;
                           $remapped_urls[$l] = $newfile;
                        }
                     }
                     $data = $obj->getMeta($field);
                     foreach($remapped_urls as $orig => $n)
                     {
                        $data = str_replace($orig, $n, $data);
                     }
                     if(count($remapped_urls) > 0)
                     {
                        $obj->setMetaData($field, $data);
                     }
                  }
               }
               $obj->writeToFile($obj_storage, $site_path);
            }
         }
         $this->itemCollection[$obj_id]->storage = $obj_storage;
      }
      $result = $this->writeSiteIndexFile($site_id);
      if($result)
      {
         $result = $this->saveSiteIndices();
      }
      global $userLanguage;
      $site_obj = $this->fetchObject($site_id, $userLanguage->getLanguageCodePrefix());
      if($site_obj)
      {
         $i = new indyDataItem("objects_".$site_id, "objectlist");
         $i->setMeta("title", $site_obj->name());
         $i->setMeta("objects", $export_count);
         if($export_links)
         {
            $i->setMeta("attached_files", $exported_files);
         }
         $meta->addItem($i);
      }
      return $result;
   }


    /************************************************************************/
    /* Return an array of all supported Object types                        */
    /* Input: none                                                          */
    /* Output: array of indydataTypes, indexed by type names                */
    /************************************************************************/
    function getSupportedTypes ()
    {
        return $this->supportedTypes->getSupportedTypes();
    }

    /************************************************************************/
    /* Return a html string with all the possible page structures as        */
    /* options and the correct one selected                                 */
    /* Input: indyObject                                                    */
    /* Output: html string                                                  */
    /************************************************************************/
    function getLayoutOptions($indyObj)
    {
       $retstr = '<option value="">None</option>';
       $obj_info = $this->getObjectInfoByTypename($t, $indyObj->getLangCode());
       foreach(array_keys($obj_info) as $o)
       {
          $retstr .= '<option value="'.$o.'"';
          if($indyObj->pagelayoutref == $o) $retstr .= ' selected';
          $retstr.='>'.$obj_info[$o].'</option>';
       }
       return $retstr;
    }


    /************************************************************************/
    /* Deletes an object from the object set and from storage               */
    /* Input: object ID, language code                                      */
    /* Output:none                                                          */
    /************************************************************************/
    function deleteObject( $objID, $langCode )
    {
       global $system_config, $languageList;
       $remove_master = false;
       if(!isset($this->itemCollection[$objID])) return false;
       $objStub = $this->itemCollection[$objID];
       $available_langs = $this->getObjectAvailableLanguages($objID);
       if($langCode == 'master')
       {
          $remove_master = true;
       }
       elseif($langCode == $objStub->primary_language_code)
       {
          if(count($available_langs) <= 1)
          {
             $remove_master = true;
          }
          else
          {
             $this->itemCollection[$objID]->primary_language_code = $available_langs[1];
             $this->updatedMasterObjects[] = $this->itemCollection[$objID];
          }
      }
      if($remove_master)
      {
         $this->_scheduleDeletion($objID, 'master');
         foreach($available_langs as $lang)
         {
            $this->_scheduleDeletion($objID, $lang);
         }
      }
      else
      {
         $this->_scheduleDeletion($objID, $langCode);
      }
      return true;
    }

    /************************************************************************/
    /* Deletes an object's data from storage                                */
    /* Input: object ID, language code                                      */
    /* Output:none                                                          */
    /************************************************************************/
    function deleteObjectData($obj_id, $langCode)
    {
      //problem is that the stub might have been deleted => fetch it from register if so
      if(!isset($this->itemCollection[$obj_id]))
      {
         foreach($this->deletedMasterObjects as $one_del)
         {
            if($one_del->obj_id == $obj_id)
            {
               $objStub = $one_del;
               break;
            }
         }
      }
      else
      {
         $objStub = $this->itemCollection[$obj_id];
      }
      if(!isset($objStub))
      {
         $this->setUserMessage( array ( INDY_ERROR,    "Cant find object stub for deleted data object $obj_id, can't determine storage!", "w2e"));
         return false;
      }

      if($objStub->storage == 'sql')
      {
          global $prefix, $dbconn;
         $result = sql_query("DELETE FROM ".$prefix."_data_objects WHERE object_id=".$obj_id." AND language_code=\"".$langCode."\"", $dbconn, 4);
         if(!$result)
         {
            $this->setSQLError($result);
            $this->setUserMessage( array ( INDY_ERROR,    "Failed to delete object data language $langCode and id[$obj_id]", "w3e") );
         }
         return $result;
      }
      else
      {
         if(isset($objStub->real_storage))
         {
            $storage = $objStub->real_storage;
         }
         else
         {
            $storage = $objStub->storage;
         }
         if($langCode == 'master') $objfile = $storage.".xml";
         else $objfile = $storage."_".$langCode.".xml";
         if(!file_exists($objfile))
         {
            $this->setUserMessage( array ( INDY_ERROR, "Failed to delete non-existent file with id $objStub->id stored in $storage","" ) );
            return false;
         }
         $worked = unlink($objfile);
         if( !$worked )
         {
            $this->setUserMessage( array( INDY_WARNING,
               "Failed to delete the data object file $objfile", "waah" ) );
             return false;
         }
         else
         {
            $this->setUserMessage( array( INDY_MSG,
               "Deleted the data object file $objfile", "wah45" ) );
            return true;
         }
       }
    }


    /************************************************************************/
    /* Object creation functions
    /************************************************************************/
    function createNewObjectFromScratch($site_id, $obj_type, $lang, $storage)
    {
      list($new_stub, $newobj) = $this->createNewDataObject($site_id, $obj_type, $lang, $storage);
      $this->newMasterObjects[] = $new_stub;
      return $newobj;
    }

    function createNewObjectFromTemplate(&$tmpl_obj, $site_id, $lang, $storage)
    {
      $new_stub = $this->createDataObjectStub(-1, $site_id, $tmpl_obj->type, $lang, $storage);
      $this->itemCollection[-1] = $new_stub;
      $this->newMasterObjects[] = $new_stub;
      $tmpl_obj->language = $lang;
      $tmpl_obj->setCaller($this->calling_req);
      $tmpl_obj->objref = -1;
      $this->newObject =& $tmpl_obj;
      return $tmpl_obj;
    }

    function createNewObjectFromForm($site_id, $obj_type, $lang, $storage)
    {
      list($new_stub, $newobj) = $this->createNewDataObject($site_id, $obj_type, $lang, $storage);
      $this->newMasterObjects[] = $new_stub;
      return $this->loadObjectFromForm($newobj, "create");
    }


    /************************************************************************/
    /* Creates a new data object in the set*/
    /* Input: integer - object id, type, is active - t/f,name, language*/
    /* Output: Data Object */
    /************************************************************************/
    function createNewDataObject($site_id, $obj_type, $lang, $storage="sql")
    {
      global $system_config;
      $new_stub = $this->createDataObjectStub(-1, $site_id, $obj_type, $lang, $storage);
      $this->itemCollection[-1] = $new_stub;
      $newobj = $this->fetchObject(-1, $lang);
      if($system_config->debug_level >= DEBUG_LEVEL_2)
      {
         $this->setUserMessage( array( DEBUG_LEVEL_2,
         "Created New Object of type $obj_type, language $lang","w3a"));
      }
       return array($new_stub, $newobj);
    }

    function createDataObjectStub($obj_id, $site_id, $obj_type, $primary_language_code, $storage)
    {
       $obj_type = strtoupper( $obj_type );
       $newobj = new indyObjectStub($obj_id, $site_id, $obj_type, $primary_language_code, $storage);
       return $newobj;
    }


    /************************************************************************/
    /* Updates an object in the set with user-input from the HTML form  */
    /* Input: none */
    /* Output:success/failure t/f */
    /************************************************************************/
    function loadObjectFromForm($obj, $action)
    {
        if(!$obj->loadFromForm($this))
        {
            $this->setUserMessages ($obj->getUserMessages());
            $this->setUserMessage( array( INDY_ERROR, "Failed to load the data object ".$obj->id()." from the form", "wwh"));
            return false;
        }
        if($action == 'create')
        {
           $this->newObject =& $obj;
        }
        else
        {
            $this->registerObjectUpdate($obj);
            if($obj->getLangCode() != 'master')
            {
               $master_obj = $this->fetchObject($obj->id(), 'master');
               if(isset($_REQUEST['translate_mode']) && $_REQUEST['translate_mode'] == 'true')
               {
                  $master_obj->clearMetaData();
                  foreach($obj->lockedMetaData as $m)
                  {
                     $master_obj->setMetaData($m, $obj->getMeta($m));
                  }
                  foreach (array_keys($obj->containedItems) as $container)
                  {
                  	if(in_array($container, $obj->lockedMetaData))
                  	{
                  		$master_obj->containedItems[$container] = $obj->containedItems[$container];
                  	}
                  }
                  $this->registerObjectUpdate($master_obj);
               }
               else
               {
                  $obj->applyMasterSettings($master_obj);
               }
            }
        }
        return $obj;
    }

   /************************************************************************/
   /* Functions to insert objects into the collection - used by import     */
   /************************************************************************/
   function forceInsertObject($stub, $obj, $recache=true)
   {
      if(isset($this->itemCollection[$stub->obj_id]))
      {
         $this->setUserMessage( array( INDY_ERROR,
         "Failed to Insert Object - object id already exists in collection", "wa2" ) );
         return false;
      }
      $this->itemCollection[$stub->obj_id] = $stub;
      $this->newMasterObjects[] = $stub;
      $this->newObject = $obj;
      $result = $this->commitChanges($recache);
      if($result === false)
      {
         $this->setUserMessage( array( INDY_ERROR,
            "Failed to Commit Changes with force insert object $stub->obj_id", "wa22" ) );
            return false;
      }
      $new_id = $this->newObject->objref;
      unset($this->newObject);
      return $new_id;
   }

   function forceUpdateObject(&$stub, &$obj, &$master_obj)
   {
      if($stub->obj_id != -1 && isset($this->itemCollection[$stub->obj_id]))
      {
         //$old_stub = $this->itemCollection[$stub->obj_id];
         $old_langs = $this->getObjectAvailableLanguages($stub->obj_id);
         foreach($old_langs as $lang)
         {
            $this->deleteObjectData($stub->obj_id, $lang);
         }
         $this->registerObjectStubUpdate($stub);
         $this->registerObjectUpdate($master_obj);
         $this->registerObjectUpdate($obj);
         $result = $this->commitChanges(false);
         if($result === false)
         {
            $this->setUserMessage( array( INDY_ERROR,
               "Failed to Commit Changes with updated object $stub->obj_id", "wa22" ) );
            return false;
         }
         return $stub->obj_id;
      }
      else
      {
         return $this->forceInsertObject($stub, $obj, false);
      }
   }

   function updateObjectData(&$stub, &$obj)
   {
      if(!isset($this->itemCollection[$stub->obj_id]))
      {
         $this->setUserMessage( array( INDY_ERROR,
         "Failed to Update Object Data - object does not exists in collection", "wa2" ) );
         return false;
      }
      $this->itemCollection[$stub->obj_id] = $stub;
      $this->registerObjectUpdate($obj);
   }

   function loadObjectFromTemplate($newobj)
   {
      return $newobj;
   }

   /************************************************************************/
   /* Functions to set the internal state of the object set                */
   /************************************************************************/
   function _scheduleRecaching($obj_id)
   {
      if(!in_array($obj_id, $this->needRecachingObjects))
      {
         $this->needRecachingObjects[] = $obj_id ;
      }
   }


   function _scheduleDeletion($obj_id, $langCode)
   {
      if($langCode == 'master')
      {
         $this->deletedMasterObjects[] = $this->itemCollection[$obj_id];
         unset($this->itemCollection[$obj_id]);
         if(isset($this->openObjects[$obj_id]))
         {
            unset($this->openObjects[$obj_id]);
         }
      }
      else
      {
         if(!isset($this->deletedObjects[$obj_id])) $this->deletedObjects[$obj_id] = array();
         $this->deletedObjects[$obj_id][] = $langCode;
         if(isset($this->openObjects[$obj_id][$langCode]))
         {
            unset($this->openObjects[$obj_id][$langCode]);
         }
      }
   }

   function _scheduleCreation($obj_id, $lang)
   {
      if(!isset($this->createdObjects[$obj_id])) $this->createdObjects[$obj_id] = array();
      $this->createdObjects[$obj_id][] = $lang;
      $this->_scheduleRecaching($obj_id);

   }

   function registerObjectStubUpdate(&$obj_stub)
   {
      $this->itemCollection[$obj_stub->obj_id] = $obj_stub;
      $this->updatedMasterObjects[] = $this->itemCollection[$obj_stub->obj_id];
   }


   function registerObjectUpdate(&$obj)
   {
      $langCode = $obj->getLangCode();
      if(!isset($this->openObjects[$obj->id()])) $this->openObjects[$obj->id()] = array();
      $this->openObjects[$obj->id()][$langCode] = $obj;
      if(!isset($this->updatedObjects[$obj->id()])) $this->updatedObjects[$obj->id()] = array();
      $this->updatedObjects[$obj->id()][] = $langCode;
      $this->_scheduleRecaching($obj->id());
   }

   function generateNewObjectID()
   {
      $max_id = max(array_keys($this->itemCollection));
      $max_id++;
      return $max_id;
   }

    /************************************************************************/
    /* Saves any changes to the object set and updates all affected objects */
    /* Output:success/failure t/f                                           */
    /************************************************************************/
    function commitChanges($recache = true, $force_recache=false)
    {
      global $system_config;
      $this->commitMasterUpdates();
      $this->pruneDeletedIncludedObjects();
      $this->commitUpdates();
      if(($recache && $system_config->auto_cache_objects) or $force_recache)
      {
         $this->findCascadedRecaches();
         $this->updateObjectCaches();
      }
      $this->resetInternalState();
      return true;
    }

   /************************************************************************/
   /* Saves any changes to the master object set                           */
   /* Input: none                                                          */
   /* Output:success/failure t/f                                           */
   /************************************************************************/
   function commitMasterUpdates()
   {
      global $system_config;
      $sites_to_update = array();
      foreach($this->deletedMasterObjects as $objStub)
      {
         if($this->storage == 'sql')
         {
            if(!$objStub->deleteFromDB())
            {
               $this->setUserMessage( array( INDY_ERROR,
               "Failed to Delete master copy of the data object $objStub->obj_id", "wa4" ) );
            }
         }
         else
         {
            $site_id = $objStub->site_id;
            if(!in_array($site_id, $sites_to_update))
            {
               $sites_to_update[] = $site_id;
            }
         }
      }
      foreach($this->updatedMasterObjects as $objStub)
      {
         if($this->storage == 'sql')
         {
            if($objStub->writeToDB() === false)
            {
               $this->setUserMessage( array( INDY_ERROR,
               "Failed to Update master copy of the data object", "wa4" ) );
            }
         }
         else
         {
            $site_id = $objStub->site_id;
            if(!in_array($site_id, $sites_to_update))
            {
               $sites_to_update[] = $site_id;
            }
         }
      }
      foreach($this->newMasterObjects as $objStub)
      {
         if($objStub->storage == 'sql')
         {
            list($m_stub, $new_master) = $this->createNewDataObject($objStub->site_id, $objStub->obj_type, 'master');
            if($new_master === false)
            {
               $this->setUserMessage(array (INDY_ERROR, "Failed to create new master object for site $objStub->site_id, type $objStub->obj_type","sds"));
               continue;
            }
            unset($this->itemCollection[-1]);
            $result = $objStub->insertIntoDB($new_master);
            if($result !== false)
            {
               $this->newObject->objref = $objStub->obj_id;
               $this->itemCollection[$objStub->obj_id] = $objStub;
               $this->_scheduleCreation($objStub->obj_id,$this->newObject->getLangCode());
               if(count($this->newObject->lockedMetaData) > 0)
               {
                  $master = $this->fetchObject($objStub->obj_id, 'master');
                  foreach($this->newObject->lockedMetaData as $metakey)
                  {
                     $master->metadata[$metakey] = $this->newObject->metadata[$metakey];
                     $this->registerObjectUpdate($master);
                  }
               }
            }
            else
            {
            	
            	echo "<P>failed to insert New object".$objStub->obj_id .mysql_errno()." - ".mysql_error();
            }
         }
         else
         {
            list($m_stub, $master) = $this->createNewDataObject($objStub->site_id, $objStub->obj_type, 'master', $objStub->storage);
            unset($this->itemCollection[-1]);
            if($objStub->obj_id == -1)
            {
               $new_id = $this->generateNewObjectID();
               $objStub->obj_id = $new_id;
               $this->newObject->objref = $objStub->obj_id;
               $this->itemCollection[$objStub->obj_id] = $objStub;
            }
            if(isRelativePath($objStub->storage) and isset($this->index_file_map[$objStub->site_id]))
            {
               $objStub->storage = $objStub->obj_id;
               $site_index_storage = $this->index_file_map[$objStub->site_id];
               $x = pathinfo($site_index_storage);
               $objStub->real_storage = $x['dirname']."/".$objStub->storage;
               $storage = $objStub->real_storage;
            }
            else
            {
               $objStub->storage = $objStub->storage.$objStub->obj_id;
               $storage = $objStub->storage;
            }
            $master->objref = $objStub->obj_id;
            $master->storage = $objStub->storage;
            $master->writeToFile($storage);
            if(!in_array($objStub->site_id, $sites_to_update))
            {
               $sites_to_update[] = $objStub->site_id;
            }
            if(strtolower($objStub->obj_type) == 'site')
            {
               if($this->storage != 'sql')
               {
                  $this->index_file_map[$objStub->obj_id] = $system_config->new_objects_storage.$system_config->new_indices_name.$objStub->obj_id.'.xml';
                  $sites_to_update[] = $objStub->obj_id;
                  $this->saveSiteIndices();
               }
            }
            $this->_scheduleCreation($objStub->obj_id,$this->newObject->getLangCode());
            if(count($this->newObject->lockedMetaData) > 0)
            {
               $master = $this->fetchObject($objStub->obj_id, 'master');
               foreach($this->newObject->lockedMetaData as $metakey)
               {
                  $master->metadata[$metakey] = $this->newObject->metadata[$metakey];
                  $this->registerObjectUpdate($master);
               }
            }
         }
      }
      foreach($sites_to_update as $site)
      {
         if(!$this->writeSiteIndexFile($site))
         {
            $this->setUserMessage( array( INDY_ERROR,
             "Failed to Write New Site Index File", "wa4" ) );
         }
      }
   }

   function writeSiteIndexFile($site)
   {
      global $system_config;
      $item_file = $this->index_file_map[$site];

      if (file_exists($item_file) && $system_config->back_up_data_object_index && !copy( $item_file, $item_file.'.bak' ) )
      {
         $this->setUserMessage(
         array (INDY_ERROR,
            "Failed to back up configuration changes to object set file $item_file",""));
         return false;
      }
      if (file_exists($item_file) && !is_writable($item_file))
      {
         $this->setUserMessage(array(INDY_ERROR,"Output object set file $item_file is not writable - check the file permissions",   "ads"));
         return false;
      }
      $ip = $this->getXML($site);
      if ( !$handle = fopen( $item_file, 'w' ) )
      {
         $this->setUserMessage(array(
            INDY_ERROR,   "Failed to open output object set file: $item_file",""));
         return false;
      }
      if (!flock($handle, LOCK_EX))
      {
         $this->setUserMessage( array (
            INDY_ERROR,
            "Failed to get lock for file: $item_file",""));
         if ( $system_config->back_up_data_object_index ) copy( $item_file.'.bak', $item_file);

         fclose( $handle );
         return false;
      }
      if ( !fwrite( $handle, $ip) )
      {
         $this->setUserMessage( array (
            INDY_ERROR,
            "Failed to write object set output to file: $item_file",""));
         if ( $system_config->back_up_data_object_index ) copy( $item_file.'.bak', $item_file);

         fclose( $handle );
         return false;
      }
      fclose( $handle );
      $this->setUserMessage( array (
         INDY_MSG,
         "Saved updates to object set file: ".$item_file,
         "er3" ) );
      return true;
   }

   /************************************************************************/
   /* Identifies deleted master objects contained by other objects         */
   /* and schedules them for updating in turn                              */
   /* Input: none                                                          */
   /* Output: none                                                         */
   /************************************************************************/
   function pruneDeletedIncludedObjects()
   {
      global $system_config;
      //remove any deleted objects from lists and menus
      foreach($this->deletedMasterObjects as $objStub)
      {
         $deletedID = $objStub->obj_id;
         foreach($this->itemCollection as $item)
         {
            $objtype = strtoupper($item->obj_type);
            if(!$this->supportedTypes->isComposite($objtype) or $item->obj_id == $deletedID)
            {
               continue;
            }
            $available_langs = $this->getObjectAvailableLanguages($item->obj_id);
            foreach($available_langs as $lang)
            {
               $dataObj = $this->fetchObject( $item->obj_id, $lang );
               if( !$dataObj )
               {
                  $this->setUserMessage( array (
                     INDY_WARNING,
                     "Failed: to open data file for object: [".$item->obj_id. "] language [$lang] to check for cascaded deletion..",
                     "4gs"));
                     continue;
               }

               if( $system_config->debug_level >= DEBUG_LEVEL_2 ) $this->setUserMessage( array ( DEBUG_LEVEL_2, "Trying object $item->obj_id to check if $deletedID is included", "8ty" ));

               if($dataObj->containsObject($deletedID, $this))
               {
                  if( $system_config->debug_level >= DEBUG_LEVEL_1 ) $this->setUserMessage( array ( DEBUG_LEVEL_1, "Found deleted object $deletedID  in $item->obj_id, pruning", "1ty" ));
                  $dataObj->removeObjectFromContainers($deletedID);
                  $this->setUserMessage( array (INDY_MSG, "Successfully updated composite object:". $dataObj->id() . " removed reference to deleted object $deletedID.","aec"));
                  $this->registerObjectUpdate($dataObj);
               }
            }
         }
      }
   }

   /************************************************************************/
   /* Identifies deleted objects contained by other objects                */
   /* and schedules containers for recaching                               */
   /* Input: none                                                          */
   /* Output: none                                                         */
   /************************************************************************/
   function scheduleRecachesForContainedDeletes()
   {
      foreach(array_keys($this->deletedObjects) as $deletedID)
      {
         $deletedLangs = $this->deletedObjects[$deletedID];
         foreach ( $this->itemCollection as $item )
         {
            $objtype = strtoupper ( $item->obj_type );
            if(!$this->supportedTypes->isComposite($objtype))
            {
               continue;
            }
            $available_langs = $this->getObjectAvailableLanguages($item->obj_id);
            foreach($available_langs as $lang)
            {
               $dataObj = $this->fetchObject( $item->obj_id, $lang );
               if( !$dataObj )
               {
                  $this->setUserMessage( array (
                     INDY_WARNING,
                     "Failed: to open data file for object: [".$item->obj_id. "] language [$lang] to check for cascaded deletion..","ad2"));
               }
               if($dataObj->containsObject($deletedID, $this))
               {
                  $this->_scheduleRecaching($item->obj_id);
               }
            }
         }
      }
    }

    /************************************************************************/
    /* Carries out changes to an objects data                               */
    /************************************************************************/
    function saveObject($obj_id, $obj_langs, &$obj_stub, $action)
    {
      $result = true;
      foreach($obj_langs as $lang)
      {
         if($action == 'delete')
         {
            $worked = $this->deleteObjectData($obj_id, $lang);
            if(!$worked)
            {
               $this->setUserMessage( array( INDY_ERROR,
               "Failed to delete the object data for object $obj_id in language $lang", "wa8" ));
               $result = false;
            }
         }
         else
         {
            if($action == 'create')
            {
               $indy_obj = $this->newObject;
            }
            else
            {
               $indy_obj = $this->fetchObject($obj_id, $lang);
            }
            if($action == 'translate')
            {
               $master_obj = $this->fetchObject($obj_id, 'master');
            }
            if(!$indy_obj)
            {
               $this->setUserMessage( array( INDY_ERROR,"Failed to $load the object data for object $obj_id in language $lang", "wih" ));
               return false;
            }
            if($obj_stub->storage == 'sql')
            {
               if($lang == 'master')
               {
                  $worked = $obj_stub->updateMasterContent($indy_obj->getXMLHeader()."\n".$indy_obj->getXML());
               }
               else
               {
                  $worked = $indy_obj->writeToDB($lang, $action);
               }
            }
            else
            {
               if(isRelativePath($obj_stub->storage))
               {
                  $site_index_storage = $this->index_file_map[$obj_stub->site_id];
                  $x = pathinfo($site_index_storage);
                  $storage = $x['dirname'].'/'.$obj_stub->storage;
               }
               else
               {
                  $storage = $obj_stub->storage;
               }
               $worked = $indy_obj->writeToFile($storage);
            }
            if(!$worked)
            {
               $this->setUserMessage( array( INDY_ERROR,
               "Failed to $action the object data for object $obj_id in language $lang", "wah" ));
               $result = false;
            }
         }
      }
      return $result;
    }

   /************************************************************************/
   /* Commits updates, deletes, etc to data objects to storage             */
   /* Input: none                                                          */
   /* Output: true/false (false for error)                                 */
   /************************************************************************/
   function commitUpdates()
   {
      global $system_config;
      $result = false;
      foreach(array_keys($this->createdObjects) as $obj_id)
      {
          $result = $this->saveObject($obj_id, $this->createdObjects[$obj_id], $this->itemCollection[$obj_id], 'create');
          if( $system_config->debug_level >= DEBUG_LEVEL_2 ) $this->setUserMessage( array ( DEBUG_LEVEL_2, "[$result] result for creation of $obj_id", "8ty" ));
      }
      foreach(array_keys($this->updatedObjects) as $obj_id)
      {
          $result = $this->saveObject($obj_id, $this->updatedObjects[$obj_id], $this->itemCollection[$obj_id], 'edit');
      }
      foreach(array_keys($this->deletedObjects) as $obj_id)
      {
          $dummy = "";
          $result = $this->saveObject($obj_id, $this->deletedObjects[$obj_id], $dummy, 'delete');
      }
      if(!$result)
      {
          $this->setUserMessage( array( INDY_ERROR,
          "Failed to commit updates to the object data for updated objects", "1ah" ));
      }
      return $result;
   }

   /************************************************************************/
   /* Finds which objects need to be recached as a result of updates       */
   /* Input: none                                                          */
   /* Output: none (schedules any that need recaching)                     */
   /************************************************************************/
   function findCascadedRecaches()
   {
      //find which objects need recaching and which objects need updating.
      //now work out which objects need recaching as a result of this update...
      //have to load every single bloody item
      $relevant_ids = array_merge(array_keys($this->updatedObjects), array_keys($this->deletedObjects));
      foreach ( $this->itemCollection as $item )
      {
         $item_scheduled = false;
         if(strtolower($item->obj_type) != 'list' and !$item->isScreenElement())
         {
            continue;
         }
         $available_langs = $this->getObjectAvailableLanguages($item->obj_id);
         foreach($available_langs as $lang)
         {
            $objtype = strtoupper ( $item->obj_type );
            $dataObj = $this->fetchObject( $item->obj_id, $lang );
            if( !$dataObj )
            {
               $this->setUserMessage(array(INDY_WARNING,         "Failed: to open data file for object: [".$item->obj_id. "] language [$lang] to check for cascaded deletion..","d23"));
               continue;
            }
            foreach($relevant_ids as $objID)
            {
               $o_type = $this->supportedTypes->getType($dataObj->getType());
               if(($this->supportedTypes->isComposite($objtype) and $dataObj->containsObject($objID, $this)) or $dataObj->containsReference($objID,
               $o_type))
               {
                  $this->_scheduleRecaching($item->obj_id);
                  $item_scheduled = true;
                  break;
               }
            }
            if($item_scheduled) break;
         }
      }
   }

   /************************************************************************/
    /* Just resets all the internal registers / object should be consistent with stored version..
    /************************************************************************/
   function resetInternalState()
   {
      $this->updatedObjects = array();
      $this->deletedObjects = array();
      $this->createdObjects = array();
      $this->deletedMasterObjects = array();
      $this->updatedMasterObjects = array();
      $this->newMasterObjects = array();
      $this->needRecachingObjects = array();
   }

   /************************************************************************/
   /* Debugging Function - writes internal state to html
   /************************************************************************/
   function getInternalStateAsHTML()
   {
      if(count($this->newMasterObjects) > 0)
      {
         $str = "New Master Objects (".count($this->newMasterObjects)."): ";
         foreach($this->newMasterObjects as $new_stub)
         {
            $str .= "ID: $new_stub->obj_id, SITE: $new_stub->site_id, TYPE: $new_stub->obj_type, PRIMARY LANGUAGE: $new_stub->primary_language_code, STORAGE: $new_stub->storage <br />\n";
         }
      }
      else $str = "No New Master Objects <br />\n";
      if(count($this->updatedMasterObjects) > 0)
      {
         $str .= "Updated Master Objects (".count($this->updatedMasterObjects)."): ";
         foreach($this->updatedMasterObjects as $new_stub)
         {
            $str .= "ID: $new_stub->obj_id, SITE: $new_stub->site_id, TYPE: $new_stub->obj_type, PRIMARY LANGUAGE: $new_stub->primary_language_code, STORAGE: $new_stub->storage <br />\n";
         }
      }
      else $str .= "No updated Master Objects <br />\n";
      if(count($this->deletedMasterObjects) > 0)
      {
         $str .= "Deleted Master Objects (".count($this->deletedMasterObjects)."): ";
         foreach($this->deletedMasterObjects as $new_stub)
         {
            $str .= "ID: $new_stub->obj_id, SITE: $new_stub->site_id, TYPE: $new_stub->obj_type, PRIMARY LANGUAGE: $new_stub->primary_language_code, STORAGE: $new_stub->storage <br />\n";
         }
      }
      else $str .= "No deleted Master Objects <br />\n";
      if(count($this->createdObjects) > 0)
      {
         $str .= "New Data Objects (".count($this->createdObjects)."): ";
         foreach(array_keys($this->createdObjects) as $obj_id)
         {
            $str .= "ID: $obj_id, LANGUAGES: ".implode(", ", $this->createdObjects[$obj_id])." <br />\n";
         }
      }
      else $str .= "No New Data Objects <br />\n";
      if(count($this->updatedObjects) > 0)
      {
         $str .= "Updated Data Objects (".count($this->updatedObjects)."): ";
         foreach(array_keys($this->updatedObjects) as $obj_id)
         {
            $str .= "ID: $obj_id, LANGUAGES: ".implode(", ", $this->updatedObjects[$obj_id])." <br />\n";
         }
      }
      else $str .= "No Updated Data Objects <br />\n";
      if(count($this->deletedObjects) > 0)
      {
         $str .= "deleted Data Objects (".count($this->deletedObjects)."): ";
         foreach(array_keys($this->deletedObjects) as $obj_id)
         {
            $str .= "ID: $obj_id, LANGUAGES: ".implode(", ", $this->deletedObjects[$obj_id])." <br />\n";
         }
      }
      else $str .= "No Deleted Data Objects <br />\n";

      if(isset($this->newObject))
      {
         $str .= "New Object Awaiting Creation: ".$this->newObject->objref."<br />";
      }
      return $str;

   }

   /************************************************************************/
    /* Functions concerned with caching...
    /************************************************************************/
   function updateObjectCaches()
   {
      foreach ( $this->needRecachingObjects as $obj_id )
      {
         $this->buildObjectCacheFiles($obj_id);
      }
      $this->object_name_cache->save();
      return true;
   }

   /************************************************************************/
   /* Rebuilds the HTML cache files for the objects of the passed sites
   /* Input: array of site ids
   /************************************************************************/
   function rebuildCaches($sites)
   {
      $this->_getVerifiedSites($sites);
      if(count($sites) <= 0)
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to rebuild caches: no valid sites selected.", "scv") );
         return false;
      }
      elseif(in_array("*", $sites))
      {
         $all_sites = true;
      }
      else
      {
         $all_sites = false;
      }

      $num_worked = 0;
      // Let them know which sites
      foreach($sites as $each_site)
      {
         $this->setUserMessage( array ( INDY_MSG, "Rebuilding cache for site " .$each_site, "scv") );
      }

      foreach($this->itemCollection as $item)
      {
         if($all_sites or in_array($item->site_id, $sites))
         {
            $success = $this->buildObjectCacheFiles($item->obj_id);
            if($success === false)
            {
               $this->setUserMessage( array ( INDY_ERROR, "Failed to rebuild cache for object: $item->obj_id", "sfv") );
            }
            else
            {
               $num_worked++;
            }
         }
      }
      $this->object_name_cache->save();
      return $num_worked;
   }

   //return a reference to my cache file stem...
   function getIncludeFileRef($id)
   {
      $cache_dir = $this->getObjectCacheDir($id);
      return $cache_dir.$id.'.inc';
   }
   // This is only used by view objects...
   function getIncludeFileRefByLang($id,$lang_code)
   {
      $cache_dir = $this->getObjectCacheDir($id);
      return $cache_dir. $id ."_".$lang_code.'.inc';
   }

   function getObjectCacheDir($id)
   {
      global $system_config;
      $cacheroot = $system_config->html_cache_store;
      $directory_prefix = "objectstore";
      $directory_num = (int)($id / $system_config->objects_per_cache_directory);
      $cache_dir = $cacheroot.$directory_prefix."_".$directory_num."/";
      return $cache_dir;
   }


   /************************************************************************/
   /* Rebuilds the HTML cache files for the object
   /* Input: object id
   /* Output: none
   /************************************************************************/
   function buildObjectCacheFiles($obj_id)
   {
      //build the cache files for the object...
      //first get the page layout object...
      global $system_config;
      $cache_dir = $this->getObjectCacheDir($obj_id);
      if(!is_dir($cache_dir))
      {
         if(!mkdir($cache_dir, $system_config->default_writable_directory_permissions))
         {
            $this->setUserMessage( array ( INDY_ERROR, "Failed to Make Directory $cache_dir to cache object: $obj_id", "sf5"));
            return false;
         }
         else
         {
            if($system_config->debug_level >= DEBUG_LEVEL_1)
            {
               $this->setUserMessage(array(
                  DEBUG_LEVEL_1, "Created Directory $cache_dir for object $obj_id cache",   "a35"));
            }
         }
      }
      $objStub = $this->itemCollection[$obj_id];
      $site_id = $objStub->site_id;
      if(!isset($this->itemCollection[$site_id]) or strtolower($this->itemCollection[$site_id]->obj_type) != 'site')
      {
         return false;
      }
      $available_langs = $this->getObjectAvailableLanguages($obj_id);
      //make sure that we load a fresh version from storage to ensure it is masked.
      unset($this->openObjects[$obj_id]);
      foreach($available_langs as $lang)
      {
         $rec_obj = $this->fetchObject($obj_id, $lang);
         if(!$rec_obj)
         {
            continue;
         }
         $this->object_name_cache->setObjectName($rec_obj->type, $obj_id, $lang, $rec_obj->name());
         $layoutRef = $rec_obj->getPageLayoutRef();
         $forDisplay = true;
         if($layoutRef === false or strtolower($this->itemCollection[$layoutRef]->obj_type) != 'page')
         {
            $forDisplay = false;
         }
         else
         {
            $layoutObj = $this->fetchObject($layoutRef, $lang);
            if($layoutObj === false or $layoutObj->getType() != "PAGE")
            {
               $forDisplay = false;
            }
         }
         $cachefname = $cache_dir.$obj_id."_".$lang.".inc";
         if($forDisplay)
         {
            $cachecontents = $layoutObj->fill($rec_obj, $this, $site_id);
         }
         else
         {
            $cachecontents = "";
            $c_s = array();
            if($rec_obj->isProtected())
            {
               $cachecontents .= $rec_obj->getProtectionHTMLHead($site_id);
            }
            $cachecontents .= $rec_obj->getHTML('<?php echo $page_style; ?>', FULL_DEPTH, $this, $c_s);
            if($rec_obj->isProtected())
            {
               $cachecontents .= $rec_obj->getProtectionHTMLFoot();
            }
         }
         if( $system_config->debug_level >= DEBUG_LEVEL_1 )
         {
            $this->setUserMessage( array (
               DEBUG_LEVEL_1,
               "Rebuilding cache file for object  " . $obj_id . " language $lang","67j"));
         }
         if (!$handle = fopen($cachefname, 'w'))
         {
            $this->setUserMessage( array(INDY_ERROR,"Failed to open cache file ".$cachefname,"8jg"));
            return false;
         }
         if (flock($handle, LOCK_EX))
         {
            $bytes = fwrite( $handle, $cachecontents );
            if($bytes === false)
            {
               $this->setUserMessage( array(INDY_ERROR,
               "Failed to write to object cache file ".$cachefname,""));
               fclose($handle);
               return false;
            }
            elseif ( $bytes === 0 )
            {
               $this->setUserMessage( array(INDY_WARNING,"cache file ".$cachefname." is empty",""));
            }
            flock($handle, LOCK_UN);
         }
         else
         {
            $this->setUserMessage( array(
            INDY_ERROR,
            "Failed to get Lock to write cache file ".$cachefname,
            ""));
            fclose($handle);
            return false;
         }
         fclose($handle);
      }

      //save the index string to the file for it...
      $index_string = $this->getCacheIndexString($obj_id, $cache_dir, $available_langs);
      $cacheindexfile = $cache_dir.$obj_id.'.inc';
      if($system_config->debug_level >= DEBUG_LEVEL_1 )
      {
         $this->setUserMessage(array(
            DEBUG_LEVEL_1,
            "Rebuilding cache file for object  " . $obj_id,
            ""));
      }
      if (!$handle = fopen($cacheindexfile, 'w'))
      {
         $this->setUserMessage(array(
            INDY_ERROR,
            "Failed to open cache file ".$cacheindexfile,
            ""));
         return false;
      }
      if (flock($handle, LOCK_EX))
      {
         $bytes = fwrite( $handle, $index_string );
         if ( $bytes === false )
         {
            $this->setUserMessage( array(
               INDY_ERROR,
               "Failed to write to object cache file ".$cacheindexfile,
               ""));
            fclose($handle);
            return false;
         }
         elseif ( $bytes === 0 )
         {
            $this->setUserMessage( array(
               INDY_WARNING,
               "cache file ".$cacheindexfile.
               " is empty",
               "" ) );
         }
         else
         {
            $this->setUserMessage( array (
               INDY_MSG,
               "Succesfully rebuilt cache file " .
               $cacheindexfile,
               ""));
         }
         flock($handle, LOCK_UN);
      }
      else
      {
         $this->setUserMessage( array( INDY_ERROR,
            "Failed to get Lock to write index file ".$cacheindexfile, ""));
            fclose($handle);
            return false;
      }
      fclose ($handle);
      return true;
   }

   /************************************************************************/
   /* Returns a php string that directs the language choice                */
   /* Input: object id, directory of cache, array of available languages   */
   /* Output: php string                                                   */
   /************************************************************************/
   function getCacheIndexString($obj_id, $cache_dir, $available_languages)
   {
      $cachebase = $cache_dir.$obj_id;
      $defaultLang = array_shift($available_languages);
      $phpstr = '<?php'."\n";
      if(count($available_languages) == 0)
      {
         $phpstr .= 'include("'.$cachebase.'_'.$defaultLang.'.inc");';
      }
      else
      {
         $phpstr .= 'global $userLanguage;'."\n";
         $phpstr .= '$langCode = $userLanguage->getLanguageCodePrefix();'."\n";
         $first = true;
         foreach($available_languages as $av)
         {
            if($first===false) $phpstr .= "else";
            $phpstr .= 'if($langCode=="'.$av.'") include("'.$cachebase.'_'.$av.'.inc");'."\n";
            $first = false;
         }
         $phpstr .= 'else include("'.$cachebase.'_'.$defaultLang.'.inc");';
      }
      $phpstr .= "\n". '?>'."\n";
      return $phpstr;
   }

    /************************************************************************/
    /* Loads a Data Object from xml
    /* Input: string - XML contents
    /* Output: success / failure t/f */
    /************************************************************************/
    function readXML($source)
    {
        //open the file and read it in and create the various bits..
        if (!function_exists('xml_parser_create'))
        {
                $this->setUserMessage( array(
                    INDY_ERROR,
                    "Failed to load PHP's XML Extension. " .                              "http://www.php.net/manual/en/ref.xml.php",
                    ""));
                return false;
        }

        $parser = @xml_parser_create();

        if ( !is_resource( $parser ) )
        {
            $this->setUserMessage( array(
                INDY_ERROR,
                "Failed to create an instance of PHP's XML parser. " .
                "http://www.php.net/manual/en/ref.xml.php",
                ""));
            return false;
        }

        $this->parser = $parser;
        xml_set_object( $this->parser, $this );
        xml_set_element_handler( $this->parser, 'start_element', 'end_element' );
        xml_set_character_data_handler( $this->parser, 'cdata' );

        $status = xml_parse( $this->parser, $source );
        if (! $status )
        {
            $errorcode = xml_get_error_code( $this->parser );
            if ( $errorcode != XML_ERROR_NONE )
            {
                $xml_error = xml_error_string( $errorcode );
                $error_line = xml_get_current_line_number($this->parser);
                $error_col = xml_get_current_column_number($this->parser);
                $errormsg = "$xml_error at line $error_line, column $error_col";
                $this->setUserMessage( array( INDY_ERROR, $errormsg,"" ) );
                return false;
            }
        }
        xml_parser_free( $this->parser );
        return true;
    }

    /************************************************************************/
    /* Called when XML parser encounters a new element
    /* Input: parser, element, attributes (array)
    /* Output: none */
    /************************************************************************/
    function start_element ($p, $element, &$attrs)
    {
        $element = strtolower( $element );
        if ( $element == 'indyobjectset' )
        {
            $this->inMenuSet = TRUE;
        }
        elseif (! $this->inMenuSet )
        {
            return;
        }
        elseif ( $element == 'indyobjectstub' )
        {
            $this->currentObjectIndex = $attrs['OBJECTID'];
            $this->currentObject = $this->createDataObjectStub(   $attrs['OBJECTID'], $attrs['SITE_ID'], $attrs['TYPE'], $attrs['PRIMARY_LANGUAGE_CODE'], $attrs['STORAGE']);
        }
    }

    /************************************************************************/
    /* Called when XML parser encounters an element end
    /* Input: parser, element
    /* Output: none */
    /************************************************************************/
    function end_element ($p, $element)
    {
        $element = strtolower($element);
        if ( $element == 'indyobjectset' )
        {
            $this->inMenuSet = FALSE;
        }
        elseif ( ($this->inMenuSet == TRUE) && ($element == 'indyobjectstub'))
        {
            $this->itemCollection[$this->currentObjectIndex] = $this->currentObject;
            $this->currentObject = null;
        }
    }

    /************************************************************************/
    /* Called when XML parser encounters character data outside tags
    /* Input: parser, text
    /* Output: none */
    /************************************************************************/
    function cdata ($p, $text){}


    /************************************************************************/
    /* Functions to get the object set as an xml file
    /************************************************************************/
   function getXMLHeader(){
      $xmlStr = '<?xml version="1.0" encoding="ISO-8859-1" ?>';
      return $xmlStr;
   }

    /************************************************************************/
    /* Returns the data object set as an XML string  */
    /* Input: none */
    /* Output:string */
    /************************************************************************/
    function getXML ($site)
    {
        $menustr = $this->getXMLHeader();
        $menustr .= "\n<indyObjectSet>\n";
        foreach($this->itemCollection as $item)
        {
            if($item->site_id == $site)
            {
               $menustr .= $item->getIndexXML();
           }
        }
        $menustr .= "</indyObjectSet>\n";
        return $menustr;
    }



    /************************************************************************/
    /* Called to Display a list of objects of a particular type
    /* Input: typename - blank is all types..
    /* Output: none */
    /************************************************************************/
    function drawAllObjectsAsLists($langCode, &$template_set)
    {
      $orig_type = $this->calling_req->obj_type;
      foreach($this->supportedTypes->getTypes() as $one_type)
      {
         $this->calling_req->obj_type = array(strtolower($one_type->type));
         $this->drawObjectListByType($one_type->type, $langCode, $template_set);
      }
      $this->calling_req->obj_type = $orig_type;
    }

    function drawObjectListByType($t, $langCode, &$template_set)
    {
      if(!$this->supportedTypes->supportsType($t))
      {
         $this->writeUserMessageBox();
         return;
      }
      $obj_type = $this->supportedTypes->getType($t);
      $objs = $this->getObjectsByTypename ($t, $langCode);
      $template_objs = $template_set->getObjectsByTypename($t, $langCode);
      if($objs === false)
      {
         $this->writeUserMessageBox();
         $objs = array();
      }
      if($template_objs === false)
      {
         $template_objs = array();
      }

      if(count($objs) == 0){
         list($m_stub, $new_obj) = $this->createNewDataObject(0, $t, $langCode);
         $new_obj->setCaller($this->calling_req);
         echo $new_obj->getEmptyListing($obj_type, $template_objs);
         return;
      }
      $objs[0]->setCaller($this->calling_req);
      echo($objs[0]->getListingHeader($obj_type, count($objs), $template_objs));
      foreach($objs as $obj)
      {
         $obj = $this->fetchObject($obj->id(), $langCode);
         if($obj !== false)
         {
            $obj->setCaller($this->calling_req);
            $obj->writeOptionsJS();
            echo $obj->getListingEntry($obj_type, $this);
         }
      }
      echo($objs[0]->getListingFooter($obj_type));
   }
}

?>
