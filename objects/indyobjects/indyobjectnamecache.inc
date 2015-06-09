<?php

class indyObjectNameCache extends indyObject
{
   var $namecache = array();

   function load($types, $force=false)
   {
      global $system_config, $userLanguage;

      foreach($types as $t)
      {
         if(!isset($this->namecache[$t]) or $force)
         {
            if(file_exists($system_config->object_name_store."$t.cache"))
            {
               $file = file($system_config->object_name_store."$t.cache");
               $txt = implode("", $file);
               $this->namecache[$t] = unserialize($txt);
            }
         }
      }
   }

   function loadType($t)
   {
      $this->load(array($t), true);
      if(isset($this->namecache[$t])) return true;
      return false;
   }

   function save()
   {
      global $system_config;
      //print_r($this->namecache);
      foreach(array_keys($this->namecache) as $type)
      {
         $txt = serialize($this->namecache[$type]);
         $fp = fopen($system_config->object_name_store."$type.cache","w");
         // locking file
         if (flock($fp, LOCK_EX))
         {
            fputs($fp, $txt);
            // unlocking file
            flock($fp, LOCK_UN);
         }
         fclose($fp);
      }
   }

   function getObjectName($t, $obj_id, $langCode, $def_lang_code)
   {
      $t = strtoupper($t);
      if(!isset($this->namecache[$t]))
      {
         if($this->loadType($t) == false)
         {
            return false;
         }
      }
      if(!isset($this->namecache[$t][$obj_id]))
      {
         return false;
      }
      if(isset($this->namecache[$t][$obj_id][$langCode]))
      {
         return $this->namecache[$t][$obj_id][$langCode];
      }
      if(isset($this->namecache[$t][$obj_id][$def_lang_code]))
      {
         return $this->namecache[$t][$obj_id][$def_lang_code];
      }
   }

   function setObjectName($t, $obj_id, $langCode, $name)
   {
      if(!isset($this->namecache[$t]))
      {
         if($this->loadType($t) == false)
         {
            $this->namecache[$t] = array();
         }
      }
      if(!isset($this->namecache[$t][$obj_id]))
      {
         $this->namecache[$t][$obj_id] = array();
      }
      $this->namecache[$t][$obj_id][$langCode] = $name;
   }

}

?>