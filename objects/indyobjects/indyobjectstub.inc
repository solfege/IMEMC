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
/* indyObjectstub class -> Stubs of objects loaded by object set       */
/************************************************************************/

class indyObjectStub extends indyObject
{
    //properties common to all objects (xml attributes)
   var $obj_id;
   var $site_id;
   var $obj_type;
   var $primary_language_code;
   var $storage;
   var $real_storage; //real path when object set uses offset



   /************************************************************************/
   /* Constructor */
   /* input: id, type, name, active (t/f, language
   /************************************************************************/
   function indyObjectStub($obj_id, $site_id, $obj_type, $primary_language_code, $storage)
   {
        $this->obj_id = $obj_id;
        $this->site_id = $site_id;
        $this->obj_type = $obj_type;
        $this->primary_language_code = $primary_language_code;
        $this->storage = $storage;
   }

   function getIndexXML()
   {
      $xmlStr = "<indyObjectStub objectID=\"".$this->obj_id."\" type=\"".$this->obj_type.
      "\" site_id=\"".$this->site_id."\" storage=\"$this->storage\" primary_language_code=\"$this->primary_language_code\" />\n";
      return $xmlStr;
   }

   function copy()
   {
      $other = new indyObjectStub($this->obj_id, $this->site_id, $this->obj_type, $this->primary_language_code, $this->storage);
      $other->real_storage = $this->real_storage;
      return $other;
   }

   function _getEmptyMasterContentXML()
   {

   }

   function deleteFromDB()
   {
      global $prefix, $dbconn;
      $result = sql_query("DELETE FROM ".$prefix.   "_master_data_objects WHERE object_id ='$this->obj_id'", $dbconn, 4);
      if(!$result)
      {
         return false;
      }
      return true;
   }

   function writeToDB()
   {
      global $prefix, $dbconn;
      $result = sql_query("UPDATE ".$prefix.   "_master_data_objects SET object_type = '$this->obj_type', primary_language_code = '$this->primary_language_code', site_id = '$this->site_id' WHERE object_id ='$this->obj_id'", $dbconn, 4);
      if(!$result)
      {
         return false;
      }
      return true;
   }


   function insertIntoDB(&$master)
   {
      global $prefix, $dbconn;
      $sql = "INSERT INTO ".$prefix. "_master_data_objects values (null, '$this->site_id', '$this->obj_type', '$this->storage', '$this->primary_language_code', '')";
      $result = sql_query($sql, $dbconn, 4);
      if(!$result)
      {
         if(($this->obj_id != -1) && (is_numeric($this->obj_id)))
         {
         	$sql = "INSERT INTO ".$prefix. "_master_data_objects values ($this->obj_id, '$this->site_id', '$this->obj_type', '$this->storage', '$this->primary_language_code', '')";
	        $result = sql_query($sql, $dbconn, 4);         	
	        if(!$result)
	        {
	            return false;	        
	        }
         }

      }
      $insert_id = mysql_insert_id($dbconn);
      if(($this->obj_id != -1) && (is_numeric($this->obj_id)) && $insert_id != $this->obj_id)
      {
         $result = sql_query("UPDATE ".$prefix.   "_master_data_objects SET object_id = $this->obj_id WHERE object_id = $insert_id", $dbconn, 4);
         if($result)
         {
            $insert_id = $this->obj_id;
         }
      }
      $this->obj_id = $insert_id;
      $master->objref = $this->obj_id;
      if($this->updateMasterContent($master->getXMLHeader()."\n".$master->getXML()))
      {
         return $this->obj_id;
      }
      return false;
   }

   function updateMasterContent($content)
   {
      global $prefix, $dbconn;
      
      $content = addslashes($content);
      $result = sql_query("UPDATE ".$prefix.   "_master_data_objects SET master_content = '$content' WHERE object_id ='$this->obj_id'", $dbconn, 4);
      if(!$result)
      {
         return false;
      }
      return true;
   }

   function loadMasterDBContent()
   {
      global $prefix, $dbconn;
      $result = sql_query("SELECT master_content FROM ".$prefix."_master_data_objects WHERE object_id=$this->obj_id", $dbconn, 4);
      if(!$result)
      {
         return false;
      }
      if(sql_num_rows( $result ) > 0)
      {
         $rsl = sql_fetch_row($result, $dbconn);
         $content = $rsl[0];
      }
      else
      {
      	 $content = "";
      }
      return stripslashes($content);
   }

   function isScreenElement()
   {
      $t = strtoupper($this->obj_type);
      if($t == "BAR" || $t == "BANNER" || $t == "MENU" || $t == "BOX") return true;
      return false;
   }

   function isStructure()
   {
      if(strtoupper($this->obj_type) == "PAGE" or strtoupper($this->obj_type) == "SITE") return true;
      return false;
   }
}

?>
