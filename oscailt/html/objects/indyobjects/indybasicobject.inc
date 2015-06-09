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
include_once "objects/contentcleansing.inc";

/************************************************************************/
/* indyBasicObject -> generic class of dataobject that encompasses      */
/* much core functionality       this is one of the major work-horses   */
/* This class is inherited by all modules and data objects.           */
/* It is also annotated with comments for developpers                   */
/************************************************************************/
class indyBasicObject extends indyObject
{
   var $type = "";
   var $objref;
   var $page;
   var $language;

   //state flags
   var $not_translated;
   var $protected;
   var $active;
   var $valid = true;
   var $loaded_from_storage;

   //object content holders
   var $metadata = array ();
   var $containedItems = array ();
   var $fields_loaded_from_storage = array();
   var $invalid_user_fields = array();

   //placeholders for versioning -> v_3.1
   var $last_update;
   var $revision;
   var $active_version;

   //the site object to which this object is attached
   var $site_object;

   //state variable for drawing forms
   var $display_master_options;

   //state containers for parsing xml.
   var $inObjXML = false;
   var $currentMetaElement;
   var $currentMetaContents;
   var $CurrentMetaType;
   var $inListingXML;
   var $currentListing;

    //attributes to store information about which fields are set in the master object
   var $lockedAttributes = array();
   var $lockedMetaData = array();
   var $lockedContainers = array();

   //the management request object that called this object
   var $calling_req;

   //for drawing itself as a form/table
   var $container_settables = array(
      "title" => 'text',
      "hsize" => 'select',
   );


   function indyBasicObject($a, $b)
    {
       $this->objref = $a;
       $this->type=$b;
       $this->page = '<?php echo $pseudo_directory_offset.$OSCAILT_SCRIPT ?>';
    }

   /************************************************************************/
   /* functions to generate links to this and other objects.               */
   /* if you call these functions, the urls returned will be safe from xss */
   /* and will support the friendly url system                             */
   /************************************************************************/
   function getSelfPage()
   {
      return $this->page;
   }

   function getObjLink($obj_id, $escape=true)
   {
      global $redirectList, $system_config, $OSCAILT_SCRIPT;
      $red = $redirectList->getRedirect($obj_id);
      if($system_config->use_friendly_urls)
      {
         if($red !== false)
         {
            $prim = $red->getPrimaryRedirect();
            if($prim !== false)
            {
               if($escape)
               {
                  return '<?php echo $pseudo_directory_offset ?>'.$prim;
               }
               else
               {
                  return $prim;
               }
            }
         }
      }
      if($escape)
      {
         return $this->page."?obj_id=$obj_id";
      }
      else
      {
         return $OSCAILT_SCRIPT."?obj_id=$obj_id";
      }
   }

   function getSelfLink()
   {
      return $this->getObjLink($this->objref);
   }

   /************************************************************************/
   /* gets and sets and capability querying of the object                  */
   /************************************************************************/
   function name()
   {
      return $this->getMeta('name');
   }

   function id()
   {
      return $this->objref;
   }

   function isActive()
   {
      return $this->active;
   }

   function isProtected()
   {
      return $this->protected;
   }

   function getType()
   {
       return $this->type;
   }

   function getLanguage()
   {
       return $this->language;
   }

   function isScreenElement()
   {
      $t = strtoupper($this->type);
      if($t == "BAR" || $t == "MENU" || $t == "BOX") return true;
      return false;
   }

   function isStructure()
   {
      if(strtoupper($this->type) == "PAGE" or strtoupper($this->type) == "SITE") return true;
      return false;
   }
   /*function substituteObjectLinks($str)
   {
      $rpl_tmpl = '<a href="'.$this->getSelfPage().'?obj_id=';
      $patterns = array('/<indyobject[^>]*id=[\'\"]?(\d+)[\'\"]?[^>]*>/ie', '/<\/indyobject[^>]>/i');
      $replacements = array('hello', '</a>');
      $str = preg_replace($patterns, $replacements, $str);
      return $str;
   }*/
   //returning fields that might have pointers to local material
   function getURLFields()
   {
      return array("linkdestination");
   }
   function getFileFields()
   {
      return array("image");
   }
   function getLocalFileLinks(&$type_obj, $file_offset="")
   {
      global $system_config;
      $lnks = array();
      $fields = $this->getFileFields();
      foreach($fields as $f)
      {
         $data = $this->getMeta($f);
         if($data != "")
         {
            if(isRelativeUrl($data) and file_exists($file_offset.$data))
            {
               $lnks[$f] = $data;
            }
            elseif(preg_match('#^(http|https)://'.$system_config->site_url.'/(.*)$#i', $data, $matches))
            {
               if(file_exists($matches[2]))
               {
                  $lnks[$f] = $data;
               }
            }
         }
      }
      return $lnks;
   }

   function getEmbeddedImageLinks(&$type_obj, $file_offset="")
   {
      $lnks = array();
      foreach($this->metadata as $k => $v)
      {
         if(is_string($v))
         {
            $res = findLocalLinkedImages($v, $file_offset);
            if(count($res) > 0)
            {
               $lnks[$k] = $res;
            }
         }
      }
      return $lnks;
   }

   /************************************************************************/
   /* Get and set of contents in the data object
   /************************************************************************/
   function getMeta ($key)
   {
       if (isset( $this->metadata[ strtolower( $key ) ]))
       {
          $info = $this->metadata[ strtolower( $key ) ];
          return $info;
       }
       else return false;
   }

   function setMetaData ($key, $value)
   {
       $this->metadata[strtolower($key)] = $value;
   }

   function clearMetaData ()
   {
       $this->metadata = array();
       $this->fields_loaded_from_storage = array();
   }

   function getShortTitle ()
   {
       return $this->getMeta( 'shortlinktext' );
   }

   function setShortTitle ( $nt )
   {
       $this->setMetaData( 'shortlinktext', $nt );
   }

   function getLongTitle ()
   {
       return $this->getMeta( 'longlinktext' );
   }

   function getLink ()
   {
       return $this->getMeta( 'linkdestination' );
   }

   function getLangCode()
   {
      global $languageList;
      if($this->language == 'master') return 'master';
      else
      {
         $lang = $languageList->getLanguageByCode($this->language);
         if(!$lang)
         {
            $lang = $languageList->getLanguageByName($this->language);
         }
         if(!$lang) return "ot";
         return $lang->getLanguageCodePrefix();
      }
   }


   /************************************************************************/
   /* Functions concerned with contained and referenced objects
   /************************************************************************/



   /************************************************************************/
   /* Checks to see if this object contains an object with a certain id    */
   /* input: object id, object set, recursion level                        */
   /* called recursively on containers within containers                   */
   /************************************************************************/
   function containsObject($obj_id, &$obj_set, $level = 0)
   {
      if($level > 10)
      {
         $this->setUserMessage( array ( INDY_WARNING, "Reached maximum recursion level $level in object $this->objref looking for $obj_id","3be"));
         return false;
      }
      foreach (array_keys($this->containedItems) as $container)
      {
         foreach ( $this->containedItems[$container] as $itemref )
           {
            if($itemref->getId() == $obj_id)
            {
               return true;
            }
            $next_obj = $obj_set->fetchObject($itemref->getId(), $this->getLangCode());
            if(!$next_obj)
            {
               $this->setUserMessage( array ( INDY_ERROR, "Failed to fetch Data Object with id $obj_id for language $this->language", "xfa" ));
               continue;
            }
            if($next_obj->containsObject($obj_id, $obj_set, ++$level))
            {
               return true;
            }
         }
      }
      return false;
   }

   function updateContainedObjectID($old_id, $new_id, &$obj_set)
   {
      foreach (array_keys($this->containedItems) as $container)
      {
         foreach ( array_keys($this->containedItems[$container]) as $itemref_index )
         {
            $itemref = $this->containedItems[$container][$itemref_index];
            if($itemref->getId() == $old_id)
            {
               $this->containedItems[$container][$itemref_index]->id = $new_id;
            }
         }
      }
   }

   function updateContainedObjectIDs($ref_map, &$obj_set)
   {
      foreach (array_keys($this->containedItems) as $container)
      {
         foreach ( array_keys($this->containedItems[$container]) as $itemref_index )
         {
            $itemref = $this->containedItems[$container][$itemref_index];
            $old_id = $itemref->getId();
            if(in_array($old_id, array_keys($ref_map)) and $ref_map[$old_id] != $old_id)
            {
            	$this->containedItems[$container][$itemref_index]->id = $ref_map[$old_id];
            }
         }
      }
   }

   /************************************************************************/
   /* Checks to see if the object contains a reference to another object.
   /************************************************************************/
   function containsReference ($obj_id, &$type_obj)
   {
      $ref_fields = $type_obj->getReferenceFields ();
      foreach($ref_fields as $ref_field)
      {
         if($this->getMeta($ref_field) === $obj_id)
         {
            return true;
         }
      }
      return false;
   }

   function updateReference ($old_id, $new_id, &$type_obj)
   {
      $ref_fields = $type_obj->getReferenceFields ();
      foreach($ref_fields as $ref_field)
      {
         if($this->getMeta($ref_field) == $old_id)
         {
            $this->setMetaData($ref_field, $new_id);
         }
      }
   }
   
   function updateReferences ($ref_map, &$type_obj)
   {
      $ref_fields = $type_obj->getReferenceFields ();
      foreach($ref_fields as $ref_field)
      {
         $old_id = $this->getMeta($ref_field);
         if(in_array($old_id, array_keys($ref_map)) and $ref_map[$old_id] != $old_id)
         {
            $this->setMetaData($ref_field, $ref_map[$old_id]);
         }
      } 	
   }

   /************************************************************************/
   /* Add/remove an object from an objects' containers
   /************************************************************************/
   function addContainedObject ( $objref, $container, $settings)
   {
       $newobj = new indyObjectRef( $objref, $settings);
       $this->containedItems[$container][] = $newobj;
   }

   function removeObjectFromContainers($objref)
   {
      foreach (array_keys($this->containedItems) as $container)
      {
         $this->removeContainedObject($objref, $container);
      }
   }

    function removeContainedObject ( $objref, $container='contained' )
    {
        $gotIt = false;
        $newbie = array();
        foreach ( $this->containedItems[$container] as $item )
        {
            if( $item->getId() == $objref )
            {
                $gotIt = true;
            }
            else $newbie[] = $item;
        }
        if($gotIt) $this->containedItems[$container] = $newbie;
        return $gotIt;
    }

   /************************************************************************/
   /* Checks to see if an object contains a reference to a page layout     */
   /************************************************************************/
   function getPageLayoutRef()
   {
      if($this->getMeta('pagelayout') == "") return false;
      return $this->getMeta('pagelayout');
   }

   /************************************************************************/
   /* Called for imported objects - changes their object references when   */
   /* the imported objects are given new ids in the process                */
   /************************************************************************/
   function remapObjectReferences(&$obj_set, $id_map)
   {
      $type_obj = $obj_set->supportedTypes->getType($this->getType());
      $this->updateReferences($id_map, $type_obj);
      $this->updateContainedObjectIDs($id_map, $obj_set);      
   }


   function remapLinkedObjectReferences(&$obj_set, &$import_source_map, $file_offset="")
   {
      $t = $obj_set->supportedTypes->getType($this->getType());
      $file_attachments = $this->getLocalFileLinks($t, $file_offset);
      foreach($file_attachments as $f => $v)
      {
         if(in_array($v, array_keys($import_source_map)))
         {
            //echo "<P>Remapping $f from $v to ".$import_source_map[$v];
            $this->setMetaData($f, $import_source_map[$v]);
         }
         else
         {
            //echo "<P>not remapping $f to $v</P>";
         }
      }
      $lnks = $this->getEmbeddedImageLinks($t, $file_offset);
      foreach($lnks as $field => $links)
      {
         $remapped_urls = array();
         foreach($links as $l)
         {
            //copy the file to its new home
            if(in_array($l, array_keys($import_source_map)))
            {
               //echo "<P>Remapping embedded image in field $field from $l to ".$import_source_map[$l];
               $remapped_urls[$l] = $import_source_map[$l];
            }
         }
         $data = $this->getMeta($field);
         foreach($remapped_urls as $orig => $n)
         {
            $data = str_replace($orig, $n, $data);
         }
         if(count($remapped_urls) > 0)
         {
            $this->setMetaData($field, $data);
         }
      }
   }


   /************************************************************************/
   /* functions concerned with storage in db/file
   /************************************************************************/

   /************************************************************************/
   /* reads the object from a storage
   /* Input: 2 character language code, reference to the object stub
   /* output: success/failure t/f
   /************************************************************************/
   function loadFromStorage($langCode, &$objStub)
   {
      if($objStub->storage == 'sql')
      {
         if($langCode == 'master')
         {
            $xml_content = $objStub->loadMasterDBContent();
         }
         else
         {
            $xml_content = $this->loadFromDB($langCode);
         }
         if(!$xml_content)
         {
            $this->setUserMessage( array ( INDY_WARNING, "Failed to read Data Object $this->objref, for language $langCode from database",""));
            return false;
         }
      }
      else
      {
         if($objStub->real_storage == null)
         {
            $xml_content = $this->readFromLangFile($objStub->storage, $langCode);
         }
         else
         {
            $xml_content = $this->readFromLangFile($objStub->real_storage, $langCode);
         }
         if($xml_content === false)
         {
            $this->setUserMessage( array ( INDY_ERROR, "Failed to read Data Object $this->objref for language $langCode from location: ".$objStub->storage, "" ) );
            return false;
         }
      }
      if(strlen($xml_content) > 0)
      {
         $result = $this->readXML($xml_content);
      }
      if(!$result)
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to Parse XML Data Object ".$this->objref, "" ) );
           return false;
      }
      $this->loaded_from_storage = true;
      return true;

    }

    /************************************************************************/
    /* reads the object from a language file
    /* Input: file path (minus language extension) to write to, language code
    /* output: success/failure t/f
    /************************************************************************/
    function readFromLangFile($storage, $langCode)
    {
       $objfile = $storage."_".$langCode.".xml";
       return $this->readFromFile($objfile);
    }

    /************************************************************************/
    /* reads the object from a file
    /* Input: file path to write to
    /* output: success/failure t/f
    /************************************************************************/
    function readFromFile($objfile)
    {
      if(!file_exists($objfile))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to open non-existent file with id $this->objref stored in $objfile","" ) );
         return false;
      }

      $xmlData = implode( '', file( $objfile ) );
      if( ( $xmlData !== FALSE ))
      {
         return $xmlData;
      }
      $this->setUserMessage( array ( INDY_ERROR, "Failed to read Data Object  id $this->objref stored in $objfile","" ) );
        return false;
    }

   /************************************************************************/
   /* reads the object from db entry
   /* Input: language code
   /* output: success/failure t/f
   /************************************************************************/
   function loadFromDB($langCode)
   {
      global $prefix, $dbconn;
      $result = sql_query("SELECT content FROM ".$prefix."_data_objects where object_id=".$this->objref." AND language_code=\"".$langCode."\"", $dbconn, 4);

      if(!$result)
      {
         $this->setSQLError($result);
         $this->setUserMessage( array ( INDY_ERROR,    "Failed to load object with language $langCode and id[$this->objref]", "w3e") );
         return false;
      }

      if(sql_num_rows( $result ) > 0)
      {
         list($content) = sql_fetch_row($result, $dbconn);
         return stripslashes($content);
      }
      $this->setUserMessage( array ( INDY_WARNING, "No objects found in data base with language $langCode for object $this->objref", "xfe") );
      return false;//if it doesn't exist -> load the default language.
   }

    /************************************************************************/
    /* writes the object to a particular file
    /* Input: file path (minus language code extension)
    /* output: success/failure t/f
    /************************************************************************/
    function writeToFile($objfile_base, $path="")
    {
      $objfile = $path.$objfile_base."_".$this->getlangCode().".xml";
      if (!$handle = fopen($objfile, 'w'))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to open output data object file $objfile", "" ));
         return false;
      }
      if (!flock($handle, LOCK_EX))
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to lock output data object file $objfile", "" ));
         fclose($handle);
         return false;
      }
      $str = $this->getXMLHeader();
      $str .= $this->getXML();
      if ( !fwrite( $handle, $str )  )
      {
         $this->setUserMessage( array ( INDY_ERROR, "Failed to write to data file $objfile", "" ) );
         fclose($handle);
         return false;
      }
      flock($handle, LOCK_UN);
      fclose( $handle );
      return true;
    }



    /************************************************************************/
    /* writes the object to a database
    /* Input: Language Code to write in table index, the action string (eg create)
    /* output: success/failure t/f
    /************************************************************************/
    function writeToDB($langCode, $action)
    {
       global $prefix, $dbconn;
       if ( $langCode != $this->getLangCode() )
       {
          $this->setUserMessage( array ( INDY_ERROR, "Mismatch between object language and db index ($langCode) for object $this->objref", "a32" ) );
       }
       $content = addslashes($this->getXMLHeader().$this->getXML());
       $last_update = "FROM_UNIXTIME(".time().")";
       $revision = isset($this->revision) ? $this->revision : 1;
       $active_version = ($this->active_version) ? 1 : 0;
       $where_clause = "language_code='$langCode' AND object_id =$this->objref";

       //need to figure out if it is a new (insert) or existing (update) object.
       //we could also use the action to assume this (check for create) but might as well be safe.
       $result = sql_query("SELECT object_id, revision FROM ".$prefix."_data_objects WHERE $where_clause", $dbconn, 4);
       if(!$result)
       {
          $this->setSQLError($result);
          $this->setUserMessage( array ( INDY_ERROR, "Failed database check to see if object $this->objref existed with $langCode", "x3e") );
          return false;
       }
       if(sql_num_rows( $result ) > 0)
       {
          $sql_statement =  "UPDATE ".$prefix."_data_objects SET content='$content', last_update=$last_update, revision=$revision, active_version=$active_version WHERE $where_clause";
       }
       else
       {
          $sql_statement = "INSERT INTO ".$prefix."_data_objects VALUES(null, $this->objref, '$langCode', '$content', $last_update, $revision, $active_version)";
       }
       $result = sql_query($sql_statement, $dbconn, 4);
       if(!$result)
       {
          $this->setSQLError($result);
          $this->setUserMessage( array ( INDY_ERROR,    "Failed database update with language $langCode for object $this->objref", "x3e") );
          return false;
       }
       return true;
   }


   /************************************************************************/
   /* function to carry out the masking of the object by a master object
   /* Input: master object reference.
   /* output: none
   /************************************************************************/
   function applyMasterSettings(&$master)
   {
      if(isset($master->active) && $master->active != null)
      {
         $this->active = $master->active;
         $this->lockedAttributes[] = 'active';
      }
      if($master->protected != null)
      {
         $this->protected = $master->protected;
         $this->lockedAttributes[] = 'protected';
      }
      foreach(array_keys($master->metadata) as $metakey)
      {
         $this->metadata[$metakey] = $master->metadata[$metakey];
         $this->lockedMetaData[] = $metakey;
      }
      foreach(array_keys($master->containedItems) as $container)
      {
         $this->containedItems[$container] = $master->containedItems[$container];
         $this->lockedContainers[] = $container;
      }
   }
   
   function hasLockedFields()
   {
      if(count($this->lockedMetaData) > 0 or count($this->lockedContainers) > 0 or count($this->lockedAttributes) > 0)
      {
         return true;
      }
      return false;
   }

   function setCaller(&$calling_req)
   {
      $this->calling_req = $calling_req;
   }

    /************************************************************************/
    /* loads data into the object from a html form and validates it
    /* Input: http request object, data object set object.
    /* output: success/failure (t/f)
    /************************************************************************/
    function loadFromForm( &$objset )
    {
        // echo "loadFromForm ".$this->getType()." ";
        $old_meta = $this->metadata;
        $this->clearMetaData();
        $meta =  $objset->supportedTypes->getInputFields( $this->getType() ) ;
        $type_obj = $objset->supportedTypes->getType($this->getType());
        $this->language = $_REQUEST['obj_language'];
        $fields_to_validate = array();
        foreach ($meta as $m)
        {
           if($this->language=='master' && isset($_REQUEST['enable_master_checkboxes']))
           {
              if(isset($_REQUEST['enable_master_'.$m]) && $_REQUEST['enable_master_'.$m] == 'on')
              {
                 $fields_to_validate = $this->loadCleansedInputData($m, $old_meta, $fields_to_validate);
              }
           }
           else
           {
               if(isset($_REQUEST[$m]))
               {
                  $fields_to_validate = $this->loadCleansedInputData($m, $old_meta, $fields_to_validate);
               }
           }
           if(isset($_REQUEST['enable_master_checkboxes']))
           {
              if(isset($_REQUEST['enable_master_'.$m]) && $_REQUEST['enable_master_'.$m] == 'on')
              {
                  $this->lockedMetaData[] = $m;
              }
           }
        }
        //op_r($fields_to_validate);
        if( isset($_REQUEST["active"]) && ($_REQUEST["active"]=="yes" || $_REQUEST["active"]=="on"  || $_REQUEST["active"]=="true")) $this->active = true;
        else $this->active=false;
        if( isset($_REQUEST["protected"]) && ($_REQUEST["protected"]=="yes" || $_REQUEST["protected"]=="on"  || $_REQUEST["protected"]=="true")) $this->protected = true;
        else $this->protected=false;

        $this->validate($objset, $fields_to_validate);
        $this->loaded_from_storage = false;
        return true;
    }

   /************************************************************************/
   /* Reads the form data from the object/module screens                   */
   /* strips out any input that might break the XML parser                 */
   /************************************************************************/
   function loadCleansedInputData($m, &$old_meta, &$form_loaded_fields)
   {
      global $editor_session;
      // echo "loadCleansedInputData ";
      if(isset($old_meta[$m]) && is_string($old_meta[$m]) && containsForbiddenTags($old_meta[$m]))
      {
         $this->setMetaData($m, $old_meta[$m]);
         $this->fields_loaded_from_storage[] = $m;
      }
      else
      {
         $val = $_REQUEST[$m];
         $form_loaded_fields[] = $m;
         $strs_to_strip = array(']]>');
         $replacements = array("]]&gt;");
         $val = str_replace($strs_to_strip, $replacements, $val);
	 if ($val =="is_date") {
	    // echo " ******** is date ********* ";
	    echo "<BR>";

	    if (isset($_REQUEST[$m."_day"])) $this->setMetaData($m."_day", trim($_REQUEST[$m."_day"]));
	    if (isset($_REQUEST[$m."_month"])) $this->setMetaData($m."_month", trim($_REQUEST[$m."_month"]));
	    if (isset($_REQUEST[$m."_year"])) $this->setMetaData($m."_year", trim($_REQUEST[$m."_year"]));
	    if (isset($_REQUEST[$m."_hour"])) $this->setMetaData($m."_hour", trim($_REQUEST[$m."_hour"]));
	    if (isset($_REQUEST[$m."_min"])) $this->setMetaData($m."_min", trim($_REQUEST[$m."_min"]));
            $this->setMetaData($m, trim($val));
	 } 
	 elseif(is_string($val))
         {
            $this->setMetaData($m, trim($val));
         }
         else
         {
            $this->setMetaData($m, $val);
         }
      }
      return $form_loaded_fields;
   }

   /************************************************************************/
   /* Validate User input according to rules set in type definition files
   /* output: none - sets valid flag to false if there is invalid data
   /************************************************************************/
   function validate(&$objset, &$field_list)
   {
      global $system_config, $public_tags;
      $rules =  $objset->supportedTypes->getValidationRules( $this->getType() ) ;
      $all_rule_data = array();
      foreach(array_keys($rules) as $rk)
      {
         if((isset($_REQUEST['enable_master_checkboxes']) and isset($_REQUEST['enable_master_'.$rk]) and $_REQUEST['enable_master_'.$rk] == 'on' and (!isset($_REQUEST['translate_mode']) or $_REQUEST['translate_mode'] != 'true')) xor $this->language == 'master')
         {
            continue;
         }
         if(isset($_REQUEST[$rk."_disabled"]) && $_REQUEST[$rk."_disabled"] == 'true')
         {
            continue;
         }
         $input_val = $this->getMeta($rk);
         $rule_set = $rules[$rk];
         $rn = $rule_set['title'];
         if(isset($rule_set['list']))
         {
            $rule_values = $rule_set['list'];
            if($input_val === false or !in_array($input_val, $rule_values))
            {
               $this->valid = false;
               $this->setUserMessage(array(USER_ERROR, "You have not selected a valid value for the $rn field. Please select a value from the available options", "dsaw"));
            }
         }
         elseif(isset($rule_set['rules']) && in_array($rk, $field_list))
         {
            $rule_values = $rule_set['rules'];
            if(in_array('mandatory', $rule_values) and ($input_val === false or $input_val === ""))
            {
               $this->valid = false;
               $this->setUserMessage(array(USER_ERROR, "<b>$rn: Missing required field</b>. You have not input a value for the ".$this->getLinkToAnchor($rn, 'edit-object-error')." field. This field is required. Please input a value into the field below", "dw"));
               $this->invalid_user_fields[] = $rn;
            }
            else
            {
               if(in_array('html', $rule_values))
               {
                  $use_html = true;
                  $to_html = true;
               }
               else
               {
                  $use_html = false;
                  $to_html = false;
               }
               if($system_config->force_correct_html_object_input)
               {
                  $problems = findContentProblems($input_val, $use_html, $to_html);
                  if(count($problems) > 0)
                  {
                     $this->valid = false;
                     foreach($problems as $problem)
                     {
                        $this->setUserMessage(array(USER_ERROR, "<b>Field ".$this->getLinkToAnchor($rn, 'edit-object-error')."</b> ".$problem, ""));
                        $this->invalid_user_fields[] = $rn;
                     }
                     continue;
                  }
               }
               $this->setMetaData($rk, processRichContentField($input_val, $use_html, $to_html));
            }
         }
         foreach($rule_values as $r)
         {
            if(strpos($r, "numeric") === 0)
            {
               $input_val = cleanseNumericalQueryField($input_val);
               $this->setMetaData($rk, $input_val);
               $limit_start = strpos($r, "(");
               $limit_end = strpos($r, ")");
               if($limit_start !== false and $limit_end !== false and $limit_end > $limit_start+1)
               {
                  $num_str = substr($r, $limit_start+1, $limit_end-$limit_start-1);
                  $vals = explode(" ", $num_str);
                  if(isset($vals[0]) && $vals[0] !== "")
                  {
                     if($input_val < $vals[0])
                     {
                        $this->valid = false;
                        $this->setUserMessage(array(USER_ERROR, "<b>Field ".$this->getLinkToAnchor($rn, 'edit-object-error')."</b> "."Input $input_val is less than the allowed minimum value for field: ".$vals[0], ""));
                        $this->invalid_user_fields[] = $rn;
                        continue;
                     }
                  }
                  if(isset($vals[1]) && $vals[1] !== "")
                  {
                     if($input_val > $vals[1])
                     {
                        $this->valid = false;
                        $this->setUserMessage(array(USER_ERROR, "<b>Field ".$this->getLinkToAnchor($rn, 'edit-object-error')."</b> "."Input $input_val is greater than the allowed maximum value for field: ".$vals[1], ""));
                        $this->invalid_user_fields[] = $rn;
                        continue;
                     }
                  }
               }
            }
         }
      }
   }


    /************************************************************************/
    /*XML functions
    /************************************************************************/


    /************************************************************************/
    /* gets the object as an xml string
    /* Input: none
    /* output: xml string
    /************************************************************************/
    function getXML()
    {

         $xmlStr = "\n<indyObject objectID=\"".$this->objref."\" type=\"".$this->type."\" ";

         if(isset($this->active))
         {
            $xmlStr .= "active=";
            $xmlStr .= ($this->active) ? '"Y" ' : '"N" ';
         }
         if(isset($this->protected))
         {
           $xmlStr .= 'protected=';
           $xmlStr .= ($this->protected) ? '"Y" ' : '"N" ';
         }
         if(isset($this->last_update))
         {
            $xmlStr .= 'lastupdate="'.$this->last_update.'" ';
         }
         if(isset($this->revision))
         {
            $xmlStr .= 'revision="'.$this->revision.'" ';
         }
         if(isset($this->active_version))
         {
             $xmlStr .= 'active_version=';
             $xmlStr .= ($this->active_version) ? '"Y" ' : '"N" ';
         }
         $xmlStr .= (isset($this->language)) ? 'language="'.$this->language."\">\n" :  ">\n";
         $metaKeys = array_keys ($this->metadata);
         foreach ($metaKeys as $key)
         {
            if( is_array( $this->metadata[$key] ) )
            {
                $xmlStr .= "<".$key." type=\"csv\">\n<![CDATA[".implode(",",$this->metadata[$key])."]]>\n</".$key.">\n";
            }
            else $xmlStr .= "<".$key.">\n<![CDATA[".$this->metadata[$key]."]]>\n</".$key.">\n";
         }
         foreach (array_keys($this->containedItems) as $container)
         {
            $xmlStr .= '<indyobject-summary name="'.$container.'">'."\n";
            foreach ($this->containedItems[$container] as $item)
            {
               $xmlStr .= $item->getXML()."\n";
            }
            $xmlStr .= '</indyobject-summary>'."\n";
         }
         $xmlStr .= "</indyObject>\n";
         return $xmlStr;
    }

    function getXMLHeader()
    {
       $xmlStr = '<?xml version="1.0" encoding="ISO-8859-1" ?>';
       return $xmlStr;
    }


    /************************************************************************/
    /* functions for parsing xml - takes string, returns true/false
    /************************************************************************/
    function readXML( $source )
    {
        if (!function_exists('xml_parser_create' ) )
        {
                $this->setUserMessage( array ( INDY_ERROR, "Failed to load PHP's XML Extension http://www.php.net/manual/en/ref.xml.php","" ) );
                return false;
        }

        $parser = @xml_parser_create();

        if (!is_resource($parser))
        {
            $this->setUserMessage( array ( INDY_ERROR, "Failed to create an instance of PHP's XML parser http://www.php.net/manual/en/ref.xml.php", "" ) );
          return false;
        }

        $this->parser = $parser;
        xml_set_object( $this->parser, $this );
	// In versions higher than PHP 5.0 (e.g. 5.1), it defaults to UTF-8 which surprisenly doesn't handle
	// ISO-8859-1 character data very well.
        xml_parser_set_option($this->parser, XML_OPTION_TARGET_ENCODING, 'ISO-8859-1');

        xml_set_element_handler($this->parser, 'start_element', 'end_element');
        xml_set_character_data_handler( $this->parser, 'cdata' );

        $status = xml_parse( $this->parser, $source );
        if (! $status ) {
            $errorcode = xml_get_error_code( $this->parser );
            if ( $errorcode != XML_ERROR_NONE ) {
                $xml_error = xml_error_string( $errorcode );
                $error_line = xml_get_current_line_number($this->parser);
                $error_col = xml_get_current_column_number($this->parser);
                $errormsg = "$xml_error at line $error_line, column $error_col";
                $this->setUserMessage( array ( INDY_ERROR, $errormsg, "" ) );
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
        global $system_config;
        $element = strtolower( $element );
        if ( $element == 'indyobject' )
        {
            $this->inObjXML = TRUE;
            if(isset($attrs['LANGUAGE'])) $this->language = $attrs['LANGUAGE'];
            if(isset($attrs['PROTECTED'])) $this->protected = ($attrs['PROTECTED'] == "N") ? false : true;
            if(isset($attrs['ACTIVE'])) $this->active = ($attrs['ACTIVE'] == "N") ? false : true;
            if(isset($attrs['TYPE'])) $this->type = $attrs['TYPE'];
            if(isset($attrs['REVISION'])) $this->revision = $attrs['REVISION'];
            if(isset($attrs['ACTIVEVERSION'])) $this->active_version = $attrs['ACTIVEVERSION'];
            if(isset($attrs['LASTUPDATE'])) $this->last_update = $attrs['LASTUPDATE'];
        }
        elseif (! $this->inObjXML )
        {
            return;
        }
        elseif ( $this->inObjXML && ($element == 'indyobject-summary') ){
           $this->inListingXML = true;
           $this->currentListing = $attrs['NAME'];
        }
        elseif ( $this->inObjXML && $this->inListingXML && $element == 'indyobjectref' )
        {
           $idref = false;
           $settings = array();
           foreach(array_keys($attrs) as $akey)
           {
                 if($akey == 'IDREF') $idref = $attrs['IDREF'];
                 else $settings[strtolower($akey)] = $attrs[$akey];
           }
           if($idref !== false)
           {
              $this->addContainedObject($idref, $this->currentListing, $settings);
          }
        }
        else
        {
            if ( isset( $attrs['TYPE'] ) ) {
                $this->currentMetaType = $attrs['TYPE'];
            }
            $this->currentMetaElement = $element;
            $this->currentMetaContents = "";
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
        if ( $element == 'indyobject' )
        {
            $this->inObjXML = false;
        }
        elseif ($element == 'indyobject-summary'){
           $this->inListingXML = false;
           unset($this->currentListing);
        }
        elseif ($this->inObjXML && isset($this->currentMetaElement) && $this->currentMetaElement != null)
        {
            if( isset( $this->currentMetaType ) && ( $this->currentMetaType == "csv" ) )
            {
                $this->setMetaData ($this->currentMetaElement, explode( ",", trim( $this->currentMetaContents ) ) );
            }
            else
            {
                $this->setMetaData( $this->currentMetaElement, trim($this->currentMetaContents) );
            }
            unset ($this->currentMetaElement);
            unset ($this->currentMetaType);
            unset ($this->currentMetaContents);
        }
    }

    /************************************************************************/
    /* Called when XML parser encounters character data outside tags
    /* Input: parser, text
    /* Output: none */
    /************************************************************************/
    function cdata ($p, $text)
    {
        if(($this->inObjXML) && (isset($this->currentMetaElement)) && ($this->currentMetaElement != null))
        {
            $this->currentMetaContents .= $text;
        }
      }

    /************************************************************************/
    /* to display the xml structure on a html page
    /************************************************************************/
   function writeXMLtoHTML($cls, &$obj_set)
   {
      $t = $obj_set->supportedTypes->getType($this->getType());
      if($t === false)
      {
         $this->setUserMessages($obj_set->supportedTypes->getUserMessages);
         $this->setUserMessage( array ( INDY_ERROR, "Failed to load type data for ". $this->getType(),"a3c"));
         return false;
      }
      $str = "<table class='admin'>\n";
        $str .= "<tr class='admin'>\n";
      $str .= "<th class='admin' colSpan=2>".$t->getMeta('name')." [".$this->id()."] - $this->language</th></tr>\n";
      $str .="<tr class='admin'>\n<td class='admin'>XML</td>\n";
      $str .="<td class='admin'><pre>".htmlspecialchars($this->getXML())."</pre></td></tr>\n";
      $str .= "</table>\n";
      echo $str;
      return true;
   }

    /************************************************************************/
    /* General HTML Producing and Object Drawing Functions
    /************************************************************************/


   /************************************************************************/
   /* wraps object in div and other tags - input is css class string,
   /* wrap -> whether to wrap in div tags, outputs html string
   /************************************************************************/
   function getHeadHTML($cls_str, $wrap = true)
   {
      //$wrap = true;
      $ret = "\n\n<!-- <".$this->name()." obj id ".$this->id()."> -->\n";
      if($wrap) $ret .= "<div $cls_str>\n";
      return $ret;
   }

   function getFootHTML($cls, $wrap = true)
   {
      $ret = "";
      if($wrap) $ret = "\n</div>";
      $ret .= "\n<!-- </".$this->name()." obj id ".$this->id()."> -->\n\n";
      return $ret;
   }

   /************************************************************************/
   /* HTML to produce the little coloured boxes linking to the object edit screen
   /************************************************************************/
   function getObjectEditingHTML(&$obj_set, $wrap_start, $wrap_end, $is_page=false)
   {
      global $system_config;
      $page_id = '<?=$_REQUEST["obj_id"]?>';
      if(strtolower($this->getType()) == 'site' or !$this->loadSiteObject($obj_set) )
      {
         return "";
      }
      if($is_page)
      {
         $t = $obj_set->supportedTypes->getType($this->type);
         $icon = $t->getTypeIcon("small");
         if(strcasecmp($this->type, 'page') === 0) $align = 'right';
         else $align = 'left';
      }
      elseif($this->isStructure())
      {
         $icon = "editpage.gif";
      }
      elseif($this->isScreenElement())
      {
         $icon = "editmenu.gif";
      }
      else
      {
         $icon = "editobject.gif";
      }
      $site_id = $this->site_object->objref;
      $obj_id = $this->objref;
      $t = $obj_set->supportedTypes->getType( $this->type );
      $lnk = $this->getObjLink($site_id).htmlspecialchars("&obj_type=".strtolower($this->getType())."&target_id=$obj_id&action=edit&site_id=$site_id&obj_language=".$this->getLangCode()."&edit_entry_url=[");
      $lnk .= $page_id.htmlspecialchars("][][][][][]");
      $alt = "Edit $t->name [".$this->name()."] Configuration";

      if($is_page && isRelativePath($icon))
      {
         $t_type = strtolower($this->getType());
         if($t_type == "document" || $t_type == "comments" || $t_type == "search" || $t_type == "contact" || $t_type == "events" || $t_type == "archive" || $t_type == "gallery" || $t_type == "preferences" || $t_type == "publish") {
             $ret = '
         <?php
          global $editor_session, $pseudo_directory_offset;
          if(($editor_session->canEditLayout()|| $editor_session->canEditContent()) && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobj'.strtolower($this->getType()).'", '.$site_id.')))
          {
          ?>
             '.$wrap_start.'
            <a href="'.$lnk.'"><img hspace=1 border=0  align="'.$align.'" title="'.$alt.'" alt="'.$alt.'" src="<?php echo $pseudo_directory_offset?>'.$icon.'" /></a>
             '.$wrap_end.'
          <?
          }
          ?>
          ';
          } else {
             $ret = '
         <?php
          global $editor_session, $pseudo_directory_offset;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobj'.strtolower($this->getType()).'", '.$site_id.')))
          {
          ?>
             '.$wrap_start.'
            <a href="'.$lnk.'"><img hspace=1 border=0  align="'.$align.'" title="'.$alt.'" alt="'.$alt.'" src="<?php echo $pseudo_directory_offset?>'.$icon.'" /></a>
             '.$wrap_end.'
          <?
          }
          ?>
          ';
          }
      }
      elseif($is_page)
      {
          $ret = '
         <?php
          global $editor_session;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobj'.strtolower($this->getType()).'", '.$site_id.')))
          {
          ?>
             '.$wrap_start.'
            <a href="'.$lnk.'"><img hspace=1 align="'.$align.'" border=0 title="'.$alt.'" alt="'.$alt.'" src="'.$icon.'" /></a>
             '.$wrap_end.'
          <?
          }
          ?>
          ';
      }
      else
      {
         $ret = '
         <?php
          global $editor_session, $graphics_store;
          if($editor_session->canEditLayout() && ($editor_session->editor->allowedReadAccessTo("editdataobjects") || $editor_session->editor->allowedReadAccessTo("editobj'.strtolower($this->getType()).'", '.$site_id.')))
          {
          ?>
             '.$wrap_start.'
            <a href="'.$lnk.'"><img hspace=1 border=0 title="'.$alt.'" alt="'.$alt.'" src="<?php echo $graphics_store?>'.$icon.'" /></a>
             '.$wrap_end.'
          <?
          }
          ?>
       ';
      }
      return $ret;
   }

   /************************************************************************/
   /* Spits out the php to protect objects
   /************************************************************************/
   function getProtectionHTMLHead($site_id)
   {
       //$rethtml = "";
       $rethtml = '<?php'."\n";
       $rethtml .= 'global $editor_session;'."\n";
       $rethtml .= 'if($editor_session->isSessionOpen() and ($editor_session->editor->allowedReadAccessTo("viewprotected", '.$site_id.'))){'."\n".'?>'."\n";
        return $rethtml;
   }

   function getProtectionHTMLFoot($show_error=false)
   {
      $rethtml = "";
      //error message when the object is the primary target
      if($show_error)
      {
         $rethtml .= '<?'."\n".'}'."\n";
         $rethtml .= 'else{ writeFailureMessage("Access Denied", "You do not Have Permission to Access this page"); ?>';
      }
      $rethtml .= "\n".'<?'."\n".'}'."\n".'?>'."\n";
      return $rethtml;
   }

   function checkObjectAccess($site_id)
   {
      global $editor_session;
      if($editor_session->isSessionOpen() and ($editor_session->editor->allowedReadAccessTo("viewprotected", $site_id)))
      {
         return true;
      }
      return false;

   }

   /************************************************************************/
   /* For preview page
   /************************************************************************/
   function drawAsHidden(&$ios, $translate_mode)
   {
      $fieldsToDraw = $ios->supportedTypes->getTypeInfo($this->getType());
      if(isset($fieldsToDraw['description'])){
         $descr = $fieldsToDraw['description'];
         unset($fieldsToDraw['description']);
      }
      if($translate_mode)
      {
         echo "<input type='hidden' name='translate_mode' value='true'>\n";
      }
      if($this->calling_req->getObjectLanguage() == 'master' or $translate_mode)
      {
         $this->display_master_options = true;
         echo "<input type='hidden' name='enable_master_checkboxes' value='true'>\n";
      }
      $rules =  $ios->supportedTypes->getValidationRules( $this->getType() ) ;

      foreach ($fieldsToDraw as $field)
      {
         if(!isset($field['type']) and !isset($field['specialtype'])) continue;

         $ipType = (isset($field['specialtype'])) ? $field['specialtype']: $field['type'];
         if($ipType == 'divider')
         {
            continue;
         }
         elseif($ipType == 'object-listing')
         {
            $ipName = $field['name'];
            $this->writeHiddenObjectListing($ipName, $ios);
         }
         elseif($ipType == 'multifilterselect' or $ipType == 'filterselect')
         {
            $this->writeHiddenFilterFields($field, $ipType, $ios);
         }
         elseif($ipType == 'date')
         {
            $this->writeHiddenDateFields($field, $ipType, $ios);
         }
         else
         {
            $saved_data = false;
            if($ipType == 'textarea' or $ipType == 'text')
            {
               if(isset($rules[$field['name']])  && isset($rules[$field['name']]['rules']))
               {
                  global $public_tags;
                  $public_tags_save = $public_tags;
                  $public_tags = array();
                  if(!in_array("html", $rules[$field['name']]['rules']))
                  {
                     $allow_html = false;
                  }
                  else
                  {
                     $allow_html = true;
                  }
                  $saved_data = $this->getMeta($field['name']);
                  $this->setMetaData($field['name'], unprocessRichContentField($this->getMeta($field['name']), $allow_html, $allow_html));
                  $public_tags = $public_tags_save;
               }
            }
            $this->writeBasicHiddenField($ipType, $field);
            if($saved_data !== false)
            {
               $this->setMetaData($field['name'], $saved_data);
            }
         }
      }
      return true;
   }


   function writeHiddenDateFields(&$field, $ipType, &$obj_set)
   {
      $t_day_field = $field['name']."_day";
      $t_month_field = $field['name']."_month";
      $t_year_field = $field['name']."_year";
      $t_hour_field = $field['name']."_hour";
      $t_min_field = $field['name']."_min";

      echo "<input type='hidden' name='".$field['name']."' value='".htmlspecialchars($this->getMeta($field['name']), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='".$t_day_field."' value='".htmlspecialchars($this->getMeta($t_day_field), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='".$t_month_field."' value='".htmlspecialchars($this->getMeta($t_month_field), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='".$t_year_field."' value='".htmlspecialchars($this->getMeta($t_year_field), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='".$t_hour_field."' value='".htmlspecialchars($this->getMeta($t_hour_field), ENT_QUOTES)."'>\n";
      echo "<input type='hidden' name='".$t_min_field."' value='".htmlspecialchars($this->getMeta($t_min_field), ENT_QUOTES)."'>\n";
   }

   function writeBasicHiddenField($ipType, &$field)
   {
      echo "<input type='hidden' name='".$field['name']."' value='".htmlspecialchars($this->getMeta($field['name']), ENT_QUOTES)."'>\n";
      if($this->display_master_options)
      {
         $val = (isset($_REQUEST['enable_master_'.$field['name']])) ? $_REQUEST['enable_master_'.$field['name']] : "";
         echo "<input type='hidden' name='enable_master_".$field['name']."' value='$val'>\n";
      }
   }

    /************************************************************************/
    /* For preview page - write out the normal filters with display none
    /************************************************************************/
   function writeHiddenFilterFields(&$field, $ipType, &$obj_set)
   {
      $ipName = $field['name'];
      unset($field['name']);
      unset($field['specialtype']);
      //need to load up our site object
      $lang_code = $this->calling_req->getObjectLanguage();
      $obj_stub = $obj_set->itemCollection[$this->objref];
      $site_id = $obj_stub->site_id;
      if($lang_code == 'master') $lang_code = $obj_stub->primary_language_code;
      $site_obj = $obj_set->fetchObject($site_id, $lang_code);
      $site_filters = $site_obj->getSiteFilters("sitebasicfilters");

      foreach(array_keys($field) as $subfieldname)
      {
         $title = isset( $field[$subfieldname]['title'] ) ? $field[$subfieldname]['title'] : "" ;
         $description = isset( $field[$subfieldname]['description'] ) ? $field[$subfieldname]['description'] : "" ;
         $help = isset( $field[$subfieldname]['help'] ) ? $field[$subfieldname]['help'] : "";
         if($ipType == 'multifilterselect')
         {
            $contents ='<select style="display: none" name="'.$ipName.$subfieldname.'[]" multiple>'."\n";
         }
         else
         {
            $contents = '<select style="display: none" name="'.$ipName.$subfieldname.'">'."\n";
         }
         $contents .= $this->getFilterOptionsHTML($field[$subfieldname]['options'], $ipName, $subfieldname, $site_filters[$subfieldname]);
         $contents .= "</select>";
         if($this->display_master_options)
         {
            $val = (isset($_REQUEST['enable_master_'.$ipName.$subfieldname])) ? $_REQUEST['enable_master_'.$ipName.$subfieldname] : "";
            echo "<input type='hidden' name='enable_master_".$ipName.$subfieldname."' value='".$val."'>\n";
         }
         echo $contents;
      }
   }

   /************************************************************************/
   /* function to write the object out to a html form for editing
   /* Input: object set
   /* output: none
   /************************************************************************/
   function drawAsForm(&$ios, $translate_mode)
   {
      writeHelpJS();
      $this->calling_req = $ios->calling_req;
      if($this->calling_req->getObjectLanguage() == 'master' or $translate_mode)
      {
         $this->display_master_options = true;
      }
      else
      {
         $this->display_master_options = false;
      }
      if($translate_mode)
      {
         echo "<input type='hidden' name='translate_mode' value='true'>\n";
      }
      $fieldsToDraw = $ios->supportedTypes->getTypeInfo($this->getType());
      if($fieldsToDraw === false)
      {

         $this->setUserMessages($ios->supportedTypes->getUserMessages());
         $this->setUserMessage( array ( INDY_ERROR,    "Failed to Retrieve Type Info for type ".$this->getType(), "x3e") );
         return false;
      }

      if(isset($fieldsToDraw['description'])){
         $descr = $fieldsToDraw['description'];
         unset($fieldsToDraw['description']);
      }
      $status_html = "";
      $cols = 4;
      $type_obj = $ios->supportedTypes->getType($this->getType());
      $title = $type_obj->name;
      $divs = $type_obj->getItemsByType('divider');
      $this->writeTableHead($title, $descr, $cols, $divs);
      $rules =  $ios->supportedTypes->getValidationRules( $this->getType() ) ;
      foreach ($fieldsToDraw as $field)
      {
         if(!isset($field['type']) and !isset($field['specialtype'])) continue;
         $ipType = (isset($field['specialtype'])) ? $field['specialtype']: $field['type'];
         if($ipType == 'reference')
         {
            $this->preProcessReferenceField($field, $ios);
            $ipType = 'select';
         }
         if($ipType == 'divider')
         {
            if(isset($field['description'])) $d = $field['description'];
            else $d = "";
            $this->writeDividerHTML($field['title'], $cols, $d);
         }
         elseif($ipType == 'object-listing')
         {
            $ipName = $field['name'];
            $this->writeObjectListing($ipName, $ios);
         }
         elseif($ipType == 'multifilterselect' or $ipType == 'filterselect')
         {
            $this->writeFilterFields($field, $ipType, $ios);
         }
         else
         {
            if($ipType == 'textarea' or $ipType == 'text')
            {
               if(in_array($field['name'], $this->fields_loaded_from_storage) or ($this->loaded_from_storage and containsForbiddenTags($this->getMeta($field['name']))))
               {
                  $field['forbidden_content'] = true;
               }
               elseif(isset($rules[$field['name']])  && isset($rules[$field['name']]['rules']))
               {
                  global $public_tags;
                  $public_tags_save = $public_tags;
                  $public_tags = array();

                  if(!in_array("html", $rules[$field['name']]['rules']))
                  {
                     $allow_html = false;
                  }
                  else
                  {
                     $allow_html = true;
                  }
                  if(isset($this->metadata[$field['name']]))
                  {
                     $this->setMetaData($field['name'], unprocessRichContentField($this->getMeta($field['name']), $allow_html, $allow_html));
                  }
                  $public_tags = $public_tags_save;
               }
            }
            $this->writeBasicField($ipType, $field);
         }
      }
      $this->writeTableFooter();
      return true;
   }

   /************************************************************************/
   /* function to turn the filters and multifilters items into html
   /************************************************************************/
   function writeFilterFields(&$field, $ipType, &$obj_set)
   {
      $ipName = $field['name'];
      unset($field['name']);
      unset($field['specialtype']);
      $lang_code = $this->calling_req->getObjectLanguage();
      $obj_stub = $obj_set->itemCollection[$this->objref];
      $site_id = $obj_stub->site_id;
      if($lang_code == 'master') $lang_code = $obj_stub->primary_language_code;
      $site_obj = $obj_set->fetchObject($site_id, $lang_code);
      $site_filters = $site_obj->getSiteFilters("sitebasicfilters");

      foreach(array_keys($field) as $subfieldname)
      {
         list($lockdirective, $is_locked) = $this->getMasterLockingInformation($ipName.$subfieldname);
         $title = isset( $field[$subfieldname]['title'] ) ? $field[$subfieldname]['title'] : "" ;
         $description = isset( $field[$subfieldname]['help'] ) ? $field[$subfieldname]['help'] : "";
         $help = "";
         if($ipType == 'multifilterselect')
         {
            $contents ='<select name="'.$ipName.$subfieldname.'[]" multiple '.$lockdirective.'>'."\n";
         }
         else
         {
            $contents = '<select name="'.$ipName.$subfieldname.'" '.$lockdirective.'>'."\n";
         }
         $contents .= $this->getFilterOptionsHTML($field[$subfieldname]['options'], $ipName, $subfieldname, $site_filters[$subfieldname]);
         $contents .= "</select>";
         echo $this->getFormFieldHTML($title, $description, $help, $contents, $is_locked, "", $ipName.$subfieldname);
      }
   }


   function getFilterOptionsHTML(&$options, $ipName, $subfieldname, &$site_filters)
   {
      $str = "";
      foreach ( array_keys( $options ) as $opt )
      {
         if(in_array("None", $site_filters) or in_array($opt, $site_filters) or strtolower($this->type) == 'site')
         {
            $str .= "<option value=\"".$opt."\"";
            if( is_array( $this->getMeta( $ipName.$subfieldname ) ) )
            {
               if ( in_array( $opt, $this->getMeta( $ipName.$subfieldname ) ) ) $str .= " selected";
            }
            elseif($opt == $this->getMeta($ipName.$subfieldname)) $str .= " selected";
            $str .= ">\n".$options[$opt]."</option>\n";
         }
      }
      return $str;
   }
   /************************************************************************/
   /* function to populate a reference type field with relevant data
   /* Input: field, reference to object set
   /* output: none
   /************************************************************************/
   function preProcessReferenceField(&$field, &$ios)
   {
      $types = explode(',', $field['referencedtypes']);
      foreach($types as $type)
      {
         $field['options'][""] = "None";
         $obj_info = $ios->getObjectInfoByTypename($type, $this->calling_req->getObjectLanguage());
         foreach(array_keys($obj_info) as $o)
         {
            $field['options'][$o] = $obj_info[$o];
         }
      }
   }

   /************************************************************************/
   /* Is a field locked by the master object?
   /************************************************************************/
   function getMasterLockingInformation($name)
   {
      //need to check that field name isn't locked ..
      if(!$this->display_master_options)
      {
         if(in_array($name, $this->lockedMetaData))
         {
            $lockdirective = "disabled";
            $is_locked = true;
         }
         else
         {
            $lockdirective = "";
            $is_locked = false;
         }
      }
      else
      {
         $is_locked = false;
         $lockdirective = "";
         if($this->calling_req->getObjectLanguage() == 'master' and isset($this->metadata[$name]))
         {
            $is_locked = true;
         }
         elseif($this->calling_req->getObjectLanguage() != 'master')
         {
            if(in_array($name, $this->lockedMetaData))
            {
               $lockdirective = "disabled";
               $is_locked = true;
            }
         }
      }
      return array($lockdirective, $is_locked);
   }

   /************************************************************************/
   /* function to write out a html field of one of the basic types (text,etc)
   /* Input: input type, field (data array)
   /* output: none
   /************************************************************************/
   function writeBasicField($ipType, &$field)
   {
      if(!isset($field['name'])) return false;
      $name = $field['name'];
      list($lockdirective, $is_locked) = $this->getMasterLockingInformation($name);
      if($lockdirective == 'disabled')
      {
         echo "<input type='hidden' name='$name"."_disabled' value='true'>\n";
      }
      $title = isset( $field['title'] ) ? $field['title'] : "" ;
      $description = isset( $field['description'] ) ? $field['description'] : "" ;
      $help = isset( $field['help'] ) ? $field['help'] : "";

      $s1 = isset( $field['dimension1'] ) ? $field['dimension1'] : "";
      $s2 = isset( $field['dimension2'] ) ? $field['dimension2'] : "";
      $contents = $this->getAnchor($title);
      $status_html = "";
      if($ipType == 'text' or $ipType == 'textarea')
      {
         if(isset($field['forbidden_content']))
         {
            $description = "This field contains Code or HTML that you do not have the authority to use.  You can not edit this field as the HTML would be deleted. Only an editor with authority to create the code in this field can edit it.";
         }
         if($ipType == "text")
         {
            if(isset($field['forbidden_content']))
            {
               $contents .= "<input type = 'hidden' name='$name' value='void'>";
               $contents .= "<input size='$s1' maxLength='$s2' name='$name"."_dummy' value='";
               $contents .= str_replace(array("'","\"","& ", "<", ">"),array("&#039;","&quot;","&amp; ", "&lt;", "&gt;"), $this->getMeta($name));
               $contents .= "' disabled='disabled'>\n";

            }
            else
            {
               $contents .= "<input size='$s1' maxLength='$s2' name='$name' value='";
               $contents .= str_replace(array("'","\"","& ", "<", ">"),array("&#039;","&quot;","&amp; ", "&lt;", "&gt;"), $this->getMeta($name));
               $contents .= "' $lockdirective>\n";
            }
            if(isset($field['rules'])) $status_html = $this->getRuleStatusHTML($field['rules']);
         }
         elseif ($ipType == "textarea")
         {
            if(isset($field['forbidden_content']))
            {
               $contents .= "<input type = 'hidden' name='$name' value='void'>";
               $contents .= "<textarea rows='$s1' cols='$s2' name='$name"."_dummy' disabled='disabled'>";
	       $contents .= str_replace(array("'","\"","& ", "<", ">"),array("&#039;","&quot;","&amp; ", "&lt;", "&gt;"), $this->getMeta($name));               
               //$contents .= $this->getMeta($name);//$contents .= htmlspecialchars($this->getMeta($name), ENT_QUOTES);
               $contents .= "</textarea>\n";

            }
            else
            {
               $contents .= "<textarea rows='$s1' cols='$s2' name='$name' $lockdirective>";
	       if(isset($field['rules']) && strstr($field['rules'], "literal") !== false) {
 	           $contents .= $this->getMeta($name);               
	       } else {
 	           $contents .= str_replace(array("'","\"","& ", "<", ">"),array("&#039;","&quot;","&amp; ", "&lt;", "&gt;"), $this->getMeta($name));               
	       }
               $contents .= "</textarea>\n";
            }
            if(isset($field['rules'])) $status_html = $this->getRuleStatusHTML($field['rules']);
         }
      }
      elseif ($ipType == "radio")
      {
         $options = explode(",", $field['options']);
         foreach ($options as $pos)
         {
            $pos = trim($pos);
            $contents .= "&nbsp;<input class='menuobject' type='radio' name='$name' value='$pos'";
            if($this->getMeta($name) == $pos) $contents .= " checked";
            $contents .= " $lockdirective> $pos\n&nbsp;&nbsp;";
         }
      }
      elseif ( $ipType == "select" )
      {
         $contents .= "<select name='$name' $lockdirective>\n";;
         $options = $field['options'];
         foreach (array_keys($options) as $opt)
         {
            $contents .= "<option value='$opt'";
            if($opt == $this->getMeta($name)) $contents .= " selected";
            $contents .= ">".$options[$opt]."</option>\n";
         }
         $contents .= "</select>";
      }
      elseif ( $ipType == "date" )
      {
         // The date type is special because you have to generate 3 further sub-names of day, month and year.
         global $system_config;
         $default_date = time()+$system_config->timezone_offset;
         $selected_day = date("j",$default_date);
         $selected_month = date("n",$default_date);
         $selected_year = date("Y",$default_date);
         $selected_hour = date("H",$default_date);
         $selected_min = date("i",$default_date);
         $current_year = $selected_year;

         if ($this->getMeta($name.'_day') != "") $selected_day = $this->getMeta($name.'_day');
         if ($this->getMeta($name.'_month') != "") $selected_month = $this->getMeta($name.'_month');
         if ($this->getMeta($name.'_year') != "") $selected_year = $this->getMeta($name.'_year');

         // $contents .= "meta day [".$this->getMeta($name.'_day') . "] ";
         // $contents .= "meta mon [".$this->getMeta($name.'_month') . "] ";
         // $contents .= "meta year [".$this->getMeta($name.'_year') . "] ";

         if($this->getMeta($name."_day") != "") $selected_day = $this->getMeta($name."_day");
         $contents .= "<input type='hidden' name='".$name."' value='is_date' >\n";
         $contents .= "<select name='".$name."_day' $lockdirective>\n";
         for ($t_i =1; $t_i <=31; $t_i++)
         {
            $contents .= '<option';
            if($t_i == $selected_day) $contents .= " selected ";
            $contents.= ' value="'.$t_i.'">'.$t_i.'</option>\n';
         }
         $contents .= "</select> ";

         $month_list = array("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec");

         if($this->getMeta($name."_month") != "") $selected_month = $this->getMeta($name."_month");

         $contents .= "<select name='".$name."_month' $lockdirective>\n";
         for ($t_i =1; $t_i <= 12; $t_i++)
         {
            $contents .= '<option';
            if ($t_i == $selected_month) 
                $contents.= ' value="'.$t_i.'" selected="yes" >'.$month_list[($t_i-1)].'</option>\n';
	    else
                $contents.= ' value="'.$t_i.'">'.$month_list[($t_i-1)].'</option>\n';
         }
         $contents .= "</select>";

         $contents .= "<select name='".$name."_year' $lockdirective>\n";

         if($this->getMeta($name."_year") != "") $selected_year = $this->getMeta($name."_year");

         for ($t_i=$current_year; $t_i <=$current_year+2; $t_i++)
         {
            $contents .= '<option';
            if($t_i == $selected_year) $contents .= " selected ";
            $contents.= ' value="'.$t_i.'">'.$t_i.'</option>\n';
         }
         $contents .= "</select>";

         if($this->getMeta($name."_hour") != "") $selected_hour = $this->getMeta($name."_hour");
         if($this->getMeta($name."_min") != "") $selected_min = $this->getMeta($name."_min");

         $contents .= " at <select name='".$name."_hour' $lockdirective>\n";
         for ($t_i= 0; $t_i <=23; $t_i++)
         {
            $contents .= '<option';
            if($t_i == $selected_hour) $contents .= " selected ";
            if($t_i < 10) $contents.= ' value="'.$t_i.'">0'.$t_i.'</option>\n';
	    else $contents.= ' value="'.$t_i.'">'.$t_i.'</option>\n';
         }
         $contents .= "</select>";
         $contents .= "  <select name='".$name."_min' $lockdirective>\n";
         for ($t_i= 0; $t_i <=59; $t_i++)
         {
            $contents .= '<option';
            if($t_i == $selected_min) $contents .= " selected ";
            if($t_i < 10) $contents.= ' value="'.$t_i.'">0'.$t_i.'</option>\n';
	    else $contents.= ' value="'.$t_i.'">'.$t_i.'</option>\n';
         }
         $contents .= "</select>";
      }
      elseif ( $ipType == "checkbox" )
      {
         $contents .= "<input type=checkbox name='$name'";
         if($this->getMeta($name) == "on") $contents .= " checked";
         $contents .= " $lockdirective>\n";
      }
      echo $this->getFormFieldHTML($title, $description, $help, $contents, $is_locked, $status_html, $name);
   }

   /************************************************************************/
   /* function to wrap the basic form output in html
   /* Input: title of field, description, help text, contents (all strings)
   /*        boolean locked,
   /* output: html string
   /************************************************************************/
   function getFormFieldHTML($title, $description, $help, $contents, $is_locked, $status_html, $name="")
   {
      $translate_contents = "";
      if($this->calling_req->getObjectLanguage() == 'master' xor $is_locked )
      {
         $cls_str = 'class="edit-object-locked"';
         $entry_str = 'class="edit-object-locked-contents"';
         $cls_spacer = 'class="edit-object-locked-spacer"';
      }
      elseif(in_array($title, $this->invalid_user_fields))
      {
         $cls_str = 'class="edit-object-error"';
         $entry_str = 'class="edit-object-contents-error"';
         $cls_spacer = 'class="edit-object-error-spacer"';
      }
      else
      {
         $cls_str = 'class="edit-object"';
         $entry_str = 'class="edit-object-contents"';
         $cls_spacer = 'class="edit-object-spacer"';
      }
      if($this->display_master_options)
      {
         $master_html = $this->getMasterLockingHTML($is_locked, $name);
      }
      else
      {
         $master_html = $this->getMasterLockedHTML($is_locked);
      }

      $str = "<tr $entry_str>\n";
      $str .= "<td $entry_str rowspan='2'>$master_html</td>\n";
      $str .= "<td $entry_str>&nbsp;</td>\n";
      $str .= "<td $entry_str>";
      $str .= "<small>$description ".getHelpHTML($help, true, true)."</small></td>\n";
      $str .= "</tr>\n";
      $str .= "<tr $cls_str><td $cls_str width='20%' align='right'><strong>$title</strong><br />$status_html</td>\n";
      $str .= "<td $cls_str width='80%'>$contents";
      $str .= "</td>\n";
      $str .= "</tr>\n";
      $str .= "<tr $cls_spacer>\n";
      $str .= "<td $cls_spacer colspan='3'>\n";
      $str .= "</td></tr>\n";
      return $str;
   }

   /************************************************************************/
   /* functions to generate the html allowing and indicating that a field locked
   /* Input:  boolean locked,
   /* output: html string
   /************************************************************************/
   function getMasterLockedHTML($is_locked)
   {
      if($is_locked)
      {
         $arr = array();
         $this->loadMasterLockedOptions($arr);
         $icon = $arr['locked']['icon'];
         $text = $arr['locked']['text'];
         $help = $arr['locked']['help'];
         $str = "<img src='$icon' alt='$text' title='$help'>";
         $str .= getHelpHTML($help, true, true);
      }
      else $str = "&nbsp;";
      return $str;
   }

   function getMasterLockingHTML($is_locked, $nm)
   {
      $arr = array();
      $this->loadMasterLockedOptions($arr);
      $icon = $arr['locking']['icon'];
      $text = $arr['locking']['text'];
      $help = $arr['locking']['help'];
      $str = "<img class='master-locking-icon' src='$icon' alt='$text' title='$help'>";
      $str .= "<input type='checkbox' name='enable_master_".$nm."'";
      if($is_locked or (isset($_REQUEST['enable_master_'.$nm]) && $_REQUEST['enable_master_'.$nm] == 'on'))
      {
         $str .= " checked";
      }
      $str .= "></input>";
      $str .= getHelpHTML($help, true, true);
      return $str;
   }




   /************************************************************************/
   /* Bits and pieces to draw the object as a form
   /************************************************************************/

   function writeTableHead($title, $descr, $cols, &$sections)
   {
        echo "<div class='object-listing-spacer'></div>";
        if($this->display_master_options)
        {
           echo "<input type='hidden' name='enable_master_checkboxes' value='true'>\n";
        }
        echo "<table class='edit-object'>\n";
        echo "<tr class='admin'><th class='admin' colspan='$cols'>".$this->getAnchor("top").$this->name()." $title</th></tr>";
        echo "<tr class='admin'><td class='admin' colspan='$cols'>";
        if(count($sections) > 0)
        {
            echo "<div class='edit-object-section-navig'><a name='#top;'></a>";
            echo "<select class='edit-object-section-navig' name='section_navigation' onChange='Javascript:location = this.options[this.selectedIndex].value;'>";
            echo "<option value='#top'>Jump to Form Section</option>\n";
            foreach($sections as $s)
            {
               $tit = makeStringOkForFilenames($s->getMeta("title"));
               echo "<option value='#".$tit."'>".$s->getMeta("title")."</option>\n";
            }
            echo "</select></div>";
        }
        echo "$descr</td></tr>";

   }

   function getAnchor($txt)
   {
      $fl_txt = makeStringOkForFilenames($txt);
      return "<a name='$fl_txt'></a>";
   }

   function getLinkToAnchor($txt, $cls = "")
   {
      $fl_txt = makeStringOkForFilenames($txt);
      return "<a href='#$fl_txt' class='$cls'>$txt</a>";
   }

   function writeTableFooter()
   {
      echo "</table>\n";
   }

   function writeDividerHTML($txt, $cols, $d="")
   {
      echo "<tr class=admin><th colspan=$cols class='admin'>";
      echo $this->getAnchor($txt);
      echo "$txt</th></tr>";
      if($d != "")
      {
         echo "<tr class=admin><td colspan=$cols class='admin'><span class='edit-object-section-intro'>";
         echo $d;
         echo "</span></td></tr>";
      }
   }

   function getRuleStatusHTML($rulestr)
   {
      $rule_data = array();
      $this->loadRulesData($rule_data);
      $str = "";
      $rules = explode(",", $rulestr);
      $written = false;
      if(in_array("mandatory", $rules))
      {
         $written = true;
         $icon = $rule_data['mandatory']['icon'];
         $text = $rule_data['mandatory']['text'];
         $help = $rule_data['mandatory']['help'];
         $str = "<span class='publish-require'>$text</span>";
      }
      if(in_array("html", $rules))
      {
         if($written) $str .= "<br>";
         $icon = $rule_data["html"]['icon'];
         $text = $rule_data["html"]['text'];
         $help = $rule_data["html"]['help'];
         $str .= " ".$this->wrapRuleHTML($icon, $text, $help);
      }
      if(in_array("literal", $rules))
      {
         if($written) $str .= "<br>";
         $icon = $rule_data["literal"]['icon'];
         $text = $rule_data["literal"]['text'];
         $help = $rule_data["literal"]['help'];
         $str .= " ".$this->wrapRuleHTML($icon, $text, $help);
      }
      return $str;
/*      if(in_array("php", $rules))
      {
         $icon = $rule_data["php"]['icon'];
         $text = $rule_data["php"]['text'];
         $help = $rule_data["php"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      }

      if(in_array("html", $rules))
      {
         $icon = $rule_data["html"]['icon'];
         $text = $rule_data["html"]['text'];
         $help = $rule_data["html"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      }
      if(in_array("phpbb", $rules))
      {
         $icon = $rule_data["phpbb"]['icon'];
         $text = $rule_data["phpbb"]['text'];
         $help = $rule_data["phpbb"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      }
      if(in_array("nospace", $rules))
      {
         $icon = $rule_data["nospace"]['icon'];
         $text = $rule_data["nospace"]['text'];
         $help = $rule_data["nospace"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      }
      if(in_array("lowercase", $rules))
      {
         $icon = $rule_data["lowercase"]['icon'];
         $text = $rule_data["lowercase"]['text'];
         $help = $rule_data["lowercase"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      }
      if(in_array("uppercase", $rules))
      {
         $icon = $rule_data["uppercase"]['icon'];
         $text = $rule_data["uppercase"]['text'];
         $help = $rule_data["uppercase"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      }
      if(in_array("file", $rules))
      {
         $icon = $rule_data["file"]['icon'];
         $text = $rule_data["file"]['text'];
         $help = $rule_data["file"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      }
      if(in_array("filteredfile", $rules))
      {
         $icon = $rule_data["file"]['icon'];
         $text = $rule_data["file"]['text'];
         $help = $rule_data["file"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      }
      if(in_array("url", $rules))
      {
         $icon = $rule_data["url"]['icon'];
         $text = $rule_data["url"]['text'];
         $help = $rule_data["url"]['help'];
         $str .= $this->wrapRuleHTML($icon, $text, $help);
      } */
      return $str;
   }

   function wrapRuleHTML($i, $t, $h)
   {
      return "<small><span class='publish-content-status'>$t</span></small>".getHelpHTML($h, true, true);
   }


   /************************************************************************/
   /* Bits and pieces to draw listings of objects
   /************************************************************************/

   function getListingHeader($obj_type, $num, &$template_objs)
   {
      global $editor_session;
      $retstr = '<br /><table class="object-summary">';
      $retstr .= '<tr class="object-summary">';
      $retstr .= '<td class="object-summary">';
      $onward_req = $this->calling_req;
      $onward_req->action = 'create';
      if($onward_req->checkPermissions("read"))
      {
         $retstr .= $this->getCreateForm($obj_type, $template_objs);
      }
      $retstr .= '<img align="left" style="vertical-align: middle; margin-right: 6px;" src="'.$obj_type->getTypeIcon("medium").'"> ';
      $retstr .= '<strong>'.$obj_type->name." Listing. <br />($num configured)</strong><br />";
      $retstr .= $obj_type->getMeta("description");
      $retstr .= "</td>\n</tr>\n";
      return $retstr;
   }

   function getListingEntry(&$obj_type, &$obj_set)
   {
      $retstr ="\n".'<tr class="object-summary"><td class="object-summary">';
      $retstr .= $this->getActionOptions($obj_set);
      $retstr .= '<div class="object-summary-details">';
      $retstr .= '<p><strong>'.$this->getMeta('name')."</strong> - ".$this->getListSummary()."</p>";
      $retstr .= "<p>".$this->getStatusHTML()."</p>";
      $retstr .= "</div>\n</td>\n</tr>\n";
      return $retstr;
   }

   function getEmptyListing(&$obj_type, &$template_objs)
   {
      $retstr = $this->getListingHeader($obj_type, 0, $template_objs);
      $retstr .= '<tr class="object-summary"><td class="object-summary">';
      $retstr .= ' [No '.$obj_type->name.' Objects]</td></tr>';
      return $retstr.$this->getListingFooter($obj_type);
   }

   function getListingFooter(&$obj_type)
   {
      $retstr = "</table>\n";
      return $retstr;
   }


   function getActionOptions(&$obj_set)
   {
      $retstr = "<table class='object-options'>";
      $retstr .= '<tr class="object-options"><td class="object-options">';
      $retstr .= $this->getActionOptionsHTML($obj_set);
      $retstr .= '</td><td class="object-options">';
      $retstr .= '<nobr>'.$this->getTranslationOptions($obj_set).'</nobr>';
      $retstr .= "</td></tr></table>\n";
      return $retstr;
   }

   function writeOptionsJS()
   {
      ?>
      <script language="Javascript">
      <!--
         function processAction(urlbase, form_element)
         {

            //build up the string to load...
            var actionstring;
            actionstring = urlbase + "&obj_language=" + document[form_element].obj_language.value;
            //alert(actionstring);
            window.location = actionstring;
         }

      //-->
      </script>
      <?
   }

   function getActionOptionsHTML(&$obj_set)
   {
      $act_opts = array();
      $onwards = $this->calling_req->copy();
      $onwards->setTargetObjects(array($this->objref));
      $langCode = $onwards->getObjectLanguage();
      $entry_url = $onwards->getEntryURL();
      if($entry_url == "")
      {
         $url = $onwards->getAsEntryURL();
         $onwards->setEntryURL($url);
      }
      unset($onwards->action);
      unset($onwards->obj_language);

      $this->loadActionOptions($act_opts);
      $cls = 'itemoption';
      $htmlstr = "<form name='objectaction_".$this->objref."' style='display: inline; float: left; margin: 0'";
      $htmlstr .= "action='".$onwards->getPage()."' method='post'>\n";
      $htmlstr .= $onwards->getHiddenQuery();
      list($edit_option_langs, $trans_option_langs) = $this->getObjectLanguagesOptions($obj_set, $langCode);
      if($edit_option_langs != "")
      {
         $htmlstr .= "<select name='obj_language'>$edit_option_langs</select>\n";
      }
      else
      {
         $htmlstr .= "<input type='hidden' name='obj_language' value='".$this->getLangCode()."'>\n";
      }
      foreach(array_keys($act_opts) as $opt)
      {
         $icon = $act_opts[$opt]['icon'];
         $help = $act_opts[$opt]['help'];
         $txt = $act_opts[$opt]['text'];
         $onwards->action = $opt;
         if($onwards->checkPermissions("read"))
         {
            $href = 'JavaScript:processAction("'.$onwards->getLink().'", "objectaction_'.$this->objref.'")';
            $lnk = "<a href='".$href."'>$txt</a>";
            $htmlstr .= "<input alt='$txt' type='image' name='$opt' src='$icon' title='$help'>$lnk ";
         }
      }
      $htmlstr .= "</form>\n";
      return $htmlstr;
   }



   function getTranslationOptions(&$obj_set)
   {
      //need to find the available languages and not-translated languages
      $act_opts = array();
      $onwards = $this->calling_req->copy();
      $onwards->setTargetObjects(array($this->objref));
      $langCode = $onwards->getObjectLanguage();
      unset($onwards->action);
      unset($onwards->obj_language);
      list($edit_option_langs, $trans_option_langs) = $this->getObjectLanguagesOptions($obj_set, $langCode);
      $htmlstr = "";
      if($trans_option_langs != "")
      {
         $trans_option_lhtml = "<select name='obj_language'>$trans_option_langs</select>\n";

         $this->loadTranslateOptions($act_opts);
         $icon = $act_opts['translate']['icon'];
         $help = $act_opts['translate']['help'];
         $txt = $act_opts['translate']['text'];

         $cls = 'itemoption';
         $htmlstr .= "<form name='objecttranslate_".$this->objref.   "' style='display: inline; float: right; margin: 0'";
         $htmlstr .= "action='".$onwards->getPage()."' method='post'>\n";
         $htmlstr .= $onwards->getHiddenQuery();

         $onwards->action = "translate";
         if(!$onwards->checkPermissions("read")) return "";
         $href = 'JavaScript:processAction("'.$onwards->getLink().'", "objecttranslate_'.$this->objref.'")';
         $lnk = "<a href='".$href."'>$txt</a>";
         $htmlstr .= "<input alt='$txt' type='image' name='translate' src='$icon' title='$help'>$lnk ";
         $htmlstr .= $trans_option_lhtml;
         $htmlstr .= "</form>\n";
      }
      $htmlstr .= "\n";
      return $htmlstr;
   }

   function getStatusHTML()
   {
      $fields = $this->getStatusFields();
      $html = "ID: $this->objref ";
      $i = 0;
      foreach(array_keys($fields) as $fk)
      {
         if($i != 0) $html .= " ";
         $i++;
         $html .= "<span class='object-status'>".$fk .": ".$fields[$fk]."</span>";
      }
      return $html;
   }

   function getStatusFields()
   {
      $sta = array();
      $fields = array();
      $this->loadStatusData($sta);
      $cls = 'itemdetails';
      $b = "";//\n<span class='$cls'>";
      $e = "";//</span>\n";
      $s = $sta['Access Control'];
      if($this->isProtected()==true)   $prefix = 'on';
      else $prefix = 'off';
      $alt = $s[$prefix.'help'];
      $icon = $s[$prefix.'icon'];
      $txt = $s[$prefix.'text'];
      $fields['Access Control'] = $txt.' <img alt="'.$alt.'" src="'.$icon.'" class="'.$cls.'">';

      $s = $sta['Limited to Section'];
      if($this->isActive()==true)   $prefix = 'on';
      else $prefix = 'off';

      $alt = $s[$prefix.'help'];
      $icon = $s[$prefix.'icon'];
      $txt = $s[$prefix.'text'];
      $fields['Limited to Section'] = $txt .' <img alt="'.$alt.'" src="'.$icon.'" class="'.$cls.'">';

      $s = $sta['Language Status'];
      if($this->not_translated == false)   $prefix = 'on';
      else $prefix = 'off';

      $alt = $s[$prefix.'help'];
      $icon = $s[$prefix.'icon'];
      $txt = $s[$prefix.'text'];
      $fields['Language Status'] = $txt.'<img alt="'.$alt.'" src="'.$icon.'" class="'.$cls.'">';

      return $fields;
   }



   function getObjectLanguageHTML(&$obj_set)
   {
      $available_langs = $obj_set->getObjectAvailableLanguages($this->objref);
      if(count($available_langs) <= 1)
      {
         return "";
      }
      $my_lang = $this->getLangCode();
      $php_str = '<?php getPageLanguageOptions("'.$my_lang.'", '.serialiseArrayParams($available_langs).', "'.$this->objref.'"); ?>';
      return $php_str;
   }

   /************************************************************************/
   /* Creates the strings of translated and not translated languages
   /************************************************************************/
   function getObjectLanguagesOptions(&$obj_set, $langCode)
   {
      global $languageList;
      $trans = array();
      $not_trans = array();
      $my_langs = $obj_set->getObjectAvailableLanguages($this->objref);
      $all_langs=$languageList->getLanguages();
      foreach($all_langs as $one_lang)
      {
         $lpref = $one_lang->getLanguageCodePrefix();
         $lname = $one_lang->getName();
         if(in_array($lpref, $my_langs) && !in_array($lpref, $trans))
         {
            $trans[$lpref] = $lname;
         }
         elseif ((!in_array($lpref, $trans)) && (!in_array($lpref, $not_trans)))
         {
            $not_trans[$lpref] = $lname;
         }
      }
      $transtr = "";
      if(count($trans) > 1 or $this->hasLockedFields() or $langCode=='master')
      {
         foreach(array_keys($trans) as $tran)
         {
            $transtr .= "<option name='obj_language' value='$tran'";
            if($tran == $langCode)
            {
               $transtr .= ' selected';
            }
            $transtr .= ">".$trans[$tran]."\n";
         }
         $transtr .= "<option name='obj_language' value='master'";
         if($langCode == 'master') $transtr .= " selected";
         $transtr .= ">Master\n";
      }

      $untranstr = "";
      if(count($not_trans) < 1)
      {
         $untranstr = "";
      }
      else
      {
         foreach(array_keys($not_trans) as $untran)
         {
            $untranstr .= "<option name='obj_language' value='$untran'";
            if($untran == $langCode)
            {
               $untranstr .= ' selected';
            }
            $untranstr .= ">".$not_trans[$untran]."\n";
         }
      }
      return array($transtr, $untranstr);
   }



   function getListSummary()
   {
      return $this->getMeta('shortdescription');
   }

   function getExtraDetails()
   {
      return "";
   }



   function getCreateForm(&$obj_type, &$templ_objs)
   {
      global $languageList;

      $onwards = $this->calling_req->copy();
      $onwards->setEntryURL($this->calling_req->getAsEntryURL());
      $onwards->action = 'create';
      $onwards->obj_type = array(strtolower($obj_type->type));
      $lang = $onwards->getObjectLanguage();
      unset($onwards->target_objects);
      unset($onwards->object_language);
      $hidden = $onwards->getHiddenQuery();
      $page = $onwards->getPage();
      $langhtml = "";
      $tmplhtml = "";
      if(count($languageList->getfLanguages())>1)
      {
         $langhtml = "<span class='object-create'>Language: ";
         $langhtml .= '<select class="object-navbar" name="obj_language">'."\n";
         $langhtml .= $languageList->getAllLanguagesAsPrefixNameOptions($lang);
         $langhtml .= '</select></span> ';
      }
      if(count($templ_objs) > 0)
      {
         $tmplhtml = "<span class='object-create'>Use Template: ";
         $tmplhtml .= '<select class="object-navbar" name="obj_template">'."\n";
         $tmplhtml .= '<option value=""> None </option>'."\n";
         foreach($templ_objs as $one_tmpl)
         {
            $tmplhtml .= "<option value='$one_tmpl->objref'> $one_tmpl->objref </option>";
         }
         $tmplhtml .= '</select></span>';
      }
      $formname = "create_".$obj_type->type."_form";
      $href = "JavaScript:document.$formname.submit()";
      $retstr = "<div class='object-create'>\n";
      $retstr .= '<form style="display: inline; margin: 0;" action="'.$page;
      $retstr .= '" name="'.$formname.'" method="post">'."\n";
      $retstr .= $hidden;
      $retstr .= "<P>";
      $retstr .= $langhtml;
      $retstr .= $tmplhtml;
      $retstr .= "</p>";
      $retstr .= '<span class="object-create"><input type="submit" name="create" value="Create New '.$obj_type->name.' >>"></span>';
      $retstr .= '</form>'."\n";
      $retstr .= '</div>'."\n";
      return $retstr;
   }




   function writeObjectViews(&$obj_set, $context, $cls)
   {
      global $page_style, $system_config, $oscailt_page_setting;
      if(!isset($oscailt_page_setting['site_id'])) $oscailt_page_setting['site_id'] = 1;
      $page_style = 'object-views';

      if($context == 'xmlview')
      {
         $result = $this->writeXMLtoHTML($cls, $obj_set);
         return $result;
      }
      else
      {
         if(strtoupper($this->type) == 'SITE') return "Cant view this object as it would lead to an infinite loop";
         $t = strtoupper($this->type);
         if($t == 'ARTICLE')
         {
            $this->setArticlePreviewDetails();
         }
         elseif($t == 'FILTERBOX' or $t == 'BAR' or $t == 'BOX' or $t == 'MENU' or $t == 'LIST' or $t == 'PAGE')
         {
            $oscailt_page_setting['page_filters'] = array('topic' => array("*"), 'region' => array("*"), 'language' => array("*"), 'type' => array("*"));
         }

         //set up our settings
         $result = false;
         $disp_settings = array();
         $disps = $this->getDisplayOptions(FULL_DEPTH);
         $i = BAR_DEPTH;

         $num_displays = count($disps);
         if($num_displays < 1)
         {
            return "Not Displayable";
         }
         $str = "<table class='object-views'>\n";
         $str .= "<th class='object-views-header'>Display Mode</th>\n";
         $str .= "<th class='object-views-header'>Appearance</th>\n";
         for($j=0; $j<=(count($disps)-1); $j++)
         {
            $str .= "<tr class='object-views'>\n";
            $str .= "<th class='object-views'>";
            $str .= $disps[$j];
            $str .= "</th>\n";
            $str .= "<td class='object-views'>";
            $disp_settings['depth'] = $disps[$j];
            $entry_str = $this->getHTML($cls,$i,$obj_set,$disp_settings,false);
            if($entry_str == false)
            {
               $entry_str = "Not Applicable";
            }
            $str .= $this->evaluateStringWithPHP(true, $entry_str);
            $str .= "</td>";
            $str .= "</tr>\n";
            if($i > FULL_DEPTH)
            {
               $i--;
            }
         }
         $str .= "</table>\n";
      }
      echo $str;
      return true;
   }


   function setArticlePreviewDetails()
   {
      global $system_config, $oscailt_story_query, $prefix, $dbconn;
      $got_one = false;
      $test_story_id = $system_config->articleview_preview_story;
      if($test_story_id > 0)
      {
         //try to select it...
         $sql = "select story_id from ".$prefix."_stories where story_id = $test_story_id";
         $result = sql_query($sql, $dbconn, 2);
         $res = checkForError($result);
         if(!$res) return;
         if(sql_num_rows( $result ) > 0)
         {
            $got_one = $test_story_id;
         }
      }
      if($got_one == false)
      {
         $sql = "select story_id from ".$prefix."_stories limit 1";//first story
         $result = sql_query($sql, $dbconn, 2);
         $res = checkForError($result);
         if(!$res) return;
         if(sql_num_rows( $result ) > 0)
         {
            list($got_one) = sql_fetch_row($result);
         }
      }
      if(!isset($oscailt_story_query) or $oscailt_story_query == null)
      {
         $oscailt_story_query = new StoryQuery();
      }
      $oscailt_story_query->story_id = $got_one;
   }

   //checks to see if disp1 is a 'fuller' display mode than disp2
   function isFullerDisplayThan($disp1, $disp2)
   {
      $trans = array_flip($this->display_options);
      if($trans[$disp1] > $trans[$disp2]) return true;
      return false;
   }


   function getDisplayOptions($depth)
   {
      return $this->display_options;
   }

   function getHsizeSelect($sel, $nm, $enabled=true)
   {
      if(!$enabled) $en="disabled";
      else $en = "";
      $str = "<select name='$nm' $en>\n";
      for($i=0; $i<=5; $i++)
      {
         if($sel == $i) $sel_str='selected';
         else $sel_str = "";
         $str .= "<option value='$i'$sel_str>$i</option>\n";
      }
      $str .= "</select>";
      return $str;
   }

   function getDisplayOptionsSelect($depth, $sel, $nm, $enabled=true)
   {
      $en = "";
      if(!$enabled or count($this->display_options) <= 1) $en="disabled";

      $str = $this->getDisplayOptionsHTML($depth, $sel);
      if($str == false) return false;
      return "<select name='$nm' $en>$str</select>\n";
   }

   function getDisplayOptionsHTML($depth, $sel)
   {
      $opts = $this->getDisplayOptions($depth);
      if(!$opts) return false;
      $str = "";
      if(count($opts) == 0) $opts = array("n/a");
      foreach($opts as $opt)
      {
         $sel_str = "";
         if($sel == $opt)
         {
            $sel_str = " selected";
         }
         $str .= "<option value='$opt'$sel_str>$opt</option>\n";
      }
      return $str;
   }

   function getDisplayMode(&$caller_settings, $depth)
   {
      $all_pos = $this->getDisplayOptions($depth);
      $have_disp = false;
      $wrap = false;
      //figure out who is calling us and what they want us to display...
      if($caller_settings && is_array($caller_settings))
      {
         if(isset($caller_settings['depth']))
         {
            if(in_array($caller_settings['depth'], $all_pos))
            {
               $display = $caller_settings['depth'];
               $have_disp = true;
            }
         }
         if(isset($caller_settings['surround']) && $caller_settings['surround'] == 'on')
         {
            $wrap = true;
         }
      }
      if(!$have_disp)
      {
         $display = $this->getDefaultDisplayMode($depth);
      }
      return array($wrap, $display);
   }

   function getCallerOverrideProtection(&$cs)
   {
      if($cs && is_array($cs) && isset($cs['override_protected']) && $cs['override_protected'] == 'on')
      {
         return true;
      }
      return false;
   }

   function getCallerHeaderSize(&$c_s)
   {
      if($c_s && is_array($c_s) && isset($c_s['hsize']) and $c_s['hsize'] > 0 and $c_s['hsize'] <= 5)
      {
         return $c_s['hsize'];
      }
      else return 0;
   }

   function getAppropriateHeaderSize(&$c_s, $depth)
   {
      $x = $this->getCallerHeaderSize($c_s);
      if($x == 0)
      {
         if($depth == FULL_DEPTH) $x = 1;
         elseif($depth == SUMMARY_DEPTH) $x = 3;
         elseif($depth == LONG_MENU_DEPTH) $x = 4;
         elseif($depth == SHORT_MENU_DEPTH) $x = 5;
         elseif($depth == LONG_LIST_DEPTH) $x = 0;
      }
      return $x;
   }

   function getHeaderTags(&$c_s, $depth, $cls_str, $use_default)
   {
      if($use_default)
      {
         $hs = $this->getAppropriateHeaderSize($c_s, $depth);
      }
      else
      {
         $hs = $this->getCallerHeaderSize($c_s);
      }
      if($hs > 0)
      {
         $h = array("<h$hs $cls_str>", "</h$hs>");
      }
      else
      {
         $h = array("", "");
      }
      return $h;
   }

   function getPageTitleSettingPHP(&$obj_set)
   {
      global $pageTitle, $pageDescription;
      if(!$this->loadSiteObject($obj_set))
      {
         return "";
      }
      $php_str =
      '<?
         addToPageDescription("'.addslashes($this->site_object->getMeta("shortdescription")).'");
      ';
      if($this->site_object->getMeta("show_site_name_in_title") == 'on')
      {
         $php_str .=
         'addToPageTitle("'.addslashes(ucwords($this->site_object->name())).'");
         ';
      }
      if(strtolower($this->type) != 'site' && strtolower($this->type) != 'article')
      {
         $php_str .= 'addToPageTitle("'.addslashes(ucwords($this->name())).'");
         addToPageDescription("'.addslashes($this->getMeta("shortdescription")).'");';
      }
      $php_str .= ' ?>';
      return $php_str;
   }

   function getPageSettingArray(&$obj_set)
   {
      $pg_settings = array();
      if(!$this->loadSiteObject($obj_set))
      {
         return $pg_settings;
      }
      $site_filters = $this->site_object->getBasicSiteFilterArray();
      $site_mail_settings = $this->site_object->getBasicEmailSettings();
      $site_module_settings = $this->site_object->getSiteSettings();

      $pg_settings['site_module_settings'] = $site_module_settings;
      $pg_settings['site_mail_settings'] = $site_mail_settings;
      $pg_settings['site_filters'] = $site_filters;
      $pg_settings['obj_id'] = $this->objref;
      $pg_settings['obj_type'] = $this->type;
      $pg_settings['site_id'] = $this->site_object->objref;
      return $pg_settings;
   }

   function getPageSettingString(&$obj_set)
   {
      $arr = $this->getPageSettingArray($obj_set);
      $php_str = serialiseArrayParams($arr);

      return '$oscailt_page_setting = '.$php_str.';';
   }


    function getHTML($cls, $depth, &$obj_set, $caller_settings, $live=false)
    {
         return false;
    }

   function getObjectIconHTML($img, $alt, $title)
   {
      if($img == "") return "";
      $img = '<?php global $pseudo_directory_offset; echo $pseudo_directory_offset ?>'.htmlspecialchars($img);
      $alt = htmlspecialchars($alt, ENT_QUOTES);
      $title = htmlspecialchars($title, ENT_QUOTES);
      if($img == "") return "";
      return "<img class='object-icon' alt='$alt' title='$title' src='$img'>";
   }

   function populatePHPTemplateString($str, $vals)
   {
      //search for the appropriate array entry in vals
      foreach(array_keys($vals) as $a_key)
      {
         $pattern = '/\[\@\[\@'.$a_key.'\]\@\]/';
         $replacement = $vals[$a_key];
         $str = preg_replace($pattern, $replacement, $str);
      }
      return $str;
   }

   //function that executes any php containing strings in live operation
     function EvaluateStringWithPHP($live, $str)
     {
        global $page_style, $pseudo_directory_offset, $graphics_store, $OSCAILT_SCRIPT;
        if($live)
        {
           ob_start();
           eval(' ?>'.$str.'<?php ');
           $str_contents = ob_get_contents();
           ob_end_clean();
           return $str_contents;
        }
        return $str;
     }

    function loadSiteObject(&$obj_set)
    {
       if($this->site_object)
       {
          return true;
       }
       if(!isset($obj_set->itemCollection[$this->id()]))
       {
          $this->setUserMessage(array(INDY_ERROR,"Failed to load object stub for object id ". $this->id(),"238"));
          return false;
       }
       $obj_stub = $obj_set->itemCollection[$this->id()];
       $site_id = $obj_stub->site_id;
       $s_obj = $obj_set->fetchObject($site_id, $this->getLangCode());
       if(!$s_obj or strtolower($s_obj->type) != 'site')
       {
          $this->setUserMessage(array(INDY_ERROR,"Failed to load site object $site_id for object id ". $this->id(),"238"));
          return false;
       }
       $this->site_object = $s_obj;
       return true;
    }


   /************************************************************************/
   /* just a place to put data...
   /************************************************************************/
   function loadMasterLockedOptions(&$act_opts)
   {
      $act_opts['locked'] = array("icon" => "graphics/lock.gif", "text" => "Locked",  "help" =>"This entry has been set in the master document and cant be changed in individual languages.");
      $act_opts['locking'] = array("icon" => "graphics/lock.gif", "text" => "Lock",  "help" =>"Check this box if you want to lock this field for all languages.");
   }

   function loadRulesData(&$rule_arr)
   {
      $rule_arr['optional'] = array("icon" => "graphics/search2.gif", "text" => "optional", "help"=>"This field is optional");
      $rule_arr['mandatory'] = array("icon" => "graphics/search.gif", "text" => "required", "help"=>"This field is required");
      $rule_arr['php'] = array("icon" => "graphics/search2.gif", "text" => "PHP Allowed", "help"=>"You can enter PHP code in this field");
      $rule_arr['html'] = array("icon" => "graphics/search2.gif", "text" => "Code", "help"=>"You can enter HTML code in this field");
      $rule_arr['phpbb'] = array("icon" => "graphics/search2.gif", "text" => "PHPbb Code allowed", "help"=>"You can enter PHPBB code in this field");
      $rule_arr['nospace'] = array("icon" => "graphics/search2.gif", "text" => "No Spaces Allowed", "help"=>"This Field should not include spaces");
      $rule_arr['lowercase'] = array("icon" => "graphics/search2.gif", "text" => "Use Lowercase Only", "help"=>"This Field should only use lower case (small) letters");
      $rule_arr['uppercase'] = array("icon" => "graphics/search2.gif", "text" => "Use Uppercase Only", "help"=>"This Field should only use upper case (capital) letters");
      $rule_arr['file'] = array("icon" => "graphics/search2.gif", "text" => "Valid Local Filename", "help"=>"This Field should provide the path to a valid local file");
      $rule_arr['filteredfile'] = array("icon" => "graphics/search2.gif", "text" => "Valid Local Filename", "help"=>"This Field should provide the path to a valid local file without the file extension eg: attachments/june2005/banner oscailt will add the extension for the particular topic.");
      $rule_arr['url'] = array("icon" => "graphics/search2.gif", "text" => "URL", "help"=>"This Field should contain a valid URL");
      $rule_arr['literal'] = array("icon" => "graphics/search2.gif", "text" => "Literal", "help"=>"This Field indiicates to keep literals and not convert greater than and less than symbols");
   }



   function loadStatusData(&$status)
   {
      $status['Access Control'] = array("onicon" => "graphics/hide.gif", "ontext" => "protected", "onhelp"=>"this is hidden to the public", "officon" => "graphics/active.gif", "offtext" => "public", "offhelp" => "this is viewable by all");
      $status['Limited to Section'] = array("onicon" => "graphics/active.gif", "ontext" => "all sections", "onhelp"=>"this can be added to site pages", "officon" => "graphics/inactive.gif", "offtext" => "this section only", "offhelp" => "this is not public and can't be added to pages on the site");
      $status['Language Status'] = array("onicon" => "graphics/active.gif", "ontext" => "translated ok", "onhelp"=>"this has been translated to your language", "officon" => "graphics/inactive.gif", "offtext" => "not translated", "offhelp" => "this has not been translated");
   }

   function loadActionOptions(&$act_opts)
   {
      $act_opts['view'] = array("icon" => "graphics/search2.gif", "text" => "View", "help" =>"view this object");
      $act_opts['xmlview'] = array("icon" => "graphics/search.gif", "text" => "View xml", "help" => "xml view");
      $act_opts['edit'] = array("icon" => "graphics/edit.gif", "text" => "Edit", "help" =>"edit this object");
      $act_opts['delete'] = array("icon" => "graphics/delete.gif", "text" => "Delete", "help" =>"delete this object");
      $act_opts['recache'] = array("icon" => "graphics/copy.gif", "text" => "Re-publish", "help" =>"Re-cache this object");
      $act_opts['export'] = array("icon" => "graphics/upgrade.gif", "text" => "Save Template", "help" =>"Re-cache this object");
   }


   function loadTranslateOptions(&$act_opts)
   {
      $act_opts['translate'] = array("icon" => "graphics/edit.gif", "text" => "Translate",  "help" =>"translate this thingamabob");
   }

}


?>
