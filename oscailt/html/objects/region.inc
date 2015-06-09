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
   Class Region
   {
      var $region_id;
      var $region_name;
      var $active;
      var $excluded;
      var $language_id;

      function Region()
      {
         $this->excluded = 0;
      }

      function getName()
      {
         return $this->region_name;
      }

      function save()
      {
         global $prefix, $dbconn;
         if($this->region_id == NULL)
         {
            checkForError(sql_query("INSERT INTO ".$prefix."_regions values ('null', '$this->region_name', ".$this->active.", ".$this->excluded.",".$this->language_id.")", $dbconn, 1));
            $this->region_id = mysql_insert_id($dbconn);
         }
         else
         {
            $result = sql_query("SELECT region_name from ".$prefix."_regions where region_id=".$this->region_id." AND language_id=".$this->language_id, $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
	    {
                checkForError(sql_query("UPDATE ".$prefix."_regions SET region_name = '$this->region_name', active =".$this->active.", excluded =".$this->excluded." WHERE region_id=".$this->region_id." AND language_id=".$this->language_id, $dbconn, 1));
            } else {
                checkForError(sql_query("INSERT INTO ".$prefix."_regions values (".$this->region_id.", '$this->region_name', ".$this->active.", ".$this->excluded.",".$this->language_id.")", $dbconn, 1));
                $this->region_id = mysql_insert_id($dbconn);
            }
         }
      }

      function load($language_id = NULL)
      {
         if($this->region_id != NULL)
         {
            if ($language_id != NULL) $this->language_id = $language_id;

            global $prefix, $dbconn;
            $result = sql_query("SELECT region_name, active, excluded, language_id from ".$prefix."_regions where region_id=".$this->region_id." AND language_id=".$this->language_id, $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($this->region_name, $this->active, $this->excluded, $this->language_id) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Region with ID $this->region_id and Lang Id $this->language_id  exists");
            }
         }
         else
         {
            reportError("The region id is null");
         }
      }

      function delete()
      {
         global $regionList;
         if($this->region_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("DELETE FROM ".$prefix."_regions where region_id=$this->region_id", $dbconn, 1);
            checkForError($result);
            $regionList->reset();
         }
         else
         {
            reportError("The region id is null");
         }
      }

      function getStoryCount()
      {
         $count=0;
         if($this->region_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("SELECT count(*) from ".$prefix."_stories where region_id=$this->region_id", $dbconn, 2);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($count) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Region with ID $this->region_id exists");
            }
         }
         else
         {
            reportError("The region id is null");
         }
         return $count;
      }

      function getStrippedName()
      {
         return strtolower(str_replace(array("\\","/"," ","-","_"),"",$this->region_name));
      }
   }


   Class RegionList
   {
      var $regions;
      var $excluded;
      var $max_region;

      function load()
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT region_id, region_name, active, excluded, language_id from ".$prefix."_regions order by region_id", $dbconn, 1);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            $this->regions = array();
            $this->excluded = array();
            $this->max_region = 0;
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {
               $r = new Region();
               list($r->region_id, $r->region_name, $r->active, $r->excluded, $r->language_id) = sql_fetch_row($result, $dbconn);
               $this->regions[$r->language_id][$r->region_id] = $r;
               if($r->excluded) $this->excluded[] = $r->region_id;
               if($r->region_id > $this->max_region) $this->max_region = $r->region_id;
            }
         }
         else
         {
            reportError("No Regions present in database");
         }
      }

      function getMaxRegionId()
      {
         if($this->regions == NULL) $this->load();
         return $this->max_region;
      }

      function getRegions($lang_id)
      {
         if($this->regions==NULL) $this->load();

 	 foreach ($this->regions as $language_id => $regions_array)
	 {
             if ($language_id == $lang_id) return array_values($regions_array);
	 }
	 return null; 
      }


      function getExcludedRegions($lang_id=1)
      {
         if($this->regions==NULL) $this->load();
         $excluded = array();
         foreach($this->excluded as $e)
         {
            $excluded[] = $this->regions[$lang_id][$e];
         }
         return $excluded;
      }

      //screening protected regions

      function getfRegions($lang_id)
      {
         global $editor_session;
         if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewprotectedcategories")) return $this->getRegions($lang_id);
         return $this->getNonExcludedRegions($lang_id);
      }


      function getNonExcludedRegions($lang_id)
      {
         if($this->regions==NULL) $this->load();
         $non_excluded = array();

         foreach($this->regions as $language_id => $sub_array)
	 {
             if ($language_id == $lang_id)
             {
		 foreach (array_keys($sub_array) as $r_id) {
                    if(!in_array($r_id, $this->excluded))
                    {
                        $non_excluded[] = $this->regions[$language_id][$r_id];
                    }
                 }
             }
         }
         return $non_excluded;
      }


      function getRegionByName($name, $lang_id=1)
      {
         if($this->regions==NULL) $this->load();

         foreach($this->regions as $language_id => $sub_array)
	 {
             if ($language_id == $lang_id ) 
	     {
                 foreach($sub_array as $region_id => $r)
                 {
                     if (!strcasecmp($r->region_name,$name)) return $r;
                 }
             }
         }

         $name=strtolower(str_replace(array("\\","/"," ","-","_"),"",$name));

         foreach($this->regions as $language_id => $sub_array)
	 {
             if ($language_id == $lang_id ) 
	     {
                 foreach($sub_array as $region_id => $r)
                 {
                     if (!strcasecmp($r->getStrippedName(),$name)) return $r;
                 }
             }
         }
         return NULL;
      }

      function getRegionByAnyLangName($name)
      {
         if($this->regions ==NULL) $this->load();

         foreach($this->regions as $language_id => $sub_array)
	 {
            foreach($sub_array as $region_id => $r)
            {
                if (!strcasecmp($r->region_name,$name)) return $r;
            }
         }

         $name=strtolower(str_replace(array("\\","/"," ","-","_"),"",$name));

         foreach($this->regions as $language_id => $sub_array)
	 {
             foreach($sub_array as $region_id => $r)
             {
                 if (!strcasecmp($r->getStrippedName(),$name)) return $r;
             }
         }
         return NULL;
      }

      // Get all the language versions of a given region id.
      function getAllRegionLanguagesByID($region_id)
      {
         if($this->regions == NULL) $this->load();

         $region_languages = array();

         foreach($this->regions as $language_id => $sub_array)
	 {
             foreach($sub_array as $t_region_id => $r)
             {
                 if ($t_region_id != $region_id ) continue;

                 $region_languages[] = $this->regions[$language_id][$region_id];
             }
         }
         return $region_languages;
      }


      function getRegionByID($region_id, $lang_id=1)
      {
         if(is_array($region_id)) $region_id = array_shift($region_id);

         if($this->regions==NULL) $this->load();

         if(isset($this->regions[$lang_id][$region_id])) return $this->regions[$lang_id][$region_id]; 
         return NULL;

      }

      function reset()
      {
         $this->regions = NULL;
      }

      function getCount()
      {
         return count($this->regions);
      }

      function dump($dir, $r_ids_to_dump = null, $lang_ids_to_dump=null)
      {
          foreach($this->regions as $language_id => $regions_array)
	  {
            foreach($regions_array as $t_key => $r)
            {
               if($lang_ids_to_dump === null or in_array($language_id, $lang_ids_to_dump))
               {
                  if($r_ids_to_dump === null or in_array($r->region_id, $r_ids_to_dump))
                  {
                     $file = $dir."/".$r->getStrippedName().".dump";
                     $fp = fopen($file,"w");
                     if (flock($fp, LOCK_EX))
                     {
                        fputs($fp, serialize($r));
                        flock($fp, LOCK_UN);
                        fclose($fp);
                     }
                     else
                     {
                        reportError("<P>failed to get lock on export dump file $file</p>");
                        return false;
                     }
                  }
               }
            }
          }
          return true;
      } 
   }
?>
