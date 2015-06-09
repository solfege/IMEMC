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
      function Region()
      {
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
            checkForError(sql_query("INSERT INTO ".$prefix."_regions values ('$this->region_id', '$this->region_name', '$this->active', '$this->excluded')", $dbconn, 1));
            $this->region_id = mysql_insert_id($dbconn);
         }
         else
         {
            checkForError(sql_query("UPDATE ".$prefix."_regions SET region_name = '$this->region_name', active = '$this->active', excluded = '$this->excluded' WHERE region_id = $this->region_id", $dbconn, 1));
         }
      }

      function load()
      {
         if($this->region_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("SELECT region_name, active, excluded from ".$prefix."_regions where region_id=$this->region_id", $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($this->region_name, $this->active, $this->excluded) = sql_fetch_row($result, $dbconn);
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

      function load()
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT region_id, region_name, active, excluded from ".$prefix."_regions", $dbconn, 1);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            $this->regions = array();
            $this->excluded = array();
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {
               $r = new Region();
               list($r->region_id, $r->region_name, $r->active, $r->excluded) = sql_fetch_row($result, $dbconn);
               $this->regions[$r->region_id] = $r;
               if($r->excluded) $this->excluded[] = $r->region_id;
            }
         }
         else
         {
            reportError("No Regions present in database");
         }
      }

      function getRegions()
      {
         if($this->regions==NULL) $this->load();
         return array_values($this->regions);
      }


      function getExcludedRegions()
      {
         if($this->regions==NULL) $this->load();
         $excluded = array();
         foreach($this->excluded as $e)
         {
            $excluded[] = $this->regions[$e];
         }
         return $excluded;
      }

      //screening protected regions

      function getfRegions()
      {
         global $editor_session;
         if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewprotectedcategories")) return $this->getRegions();
         return $this->getNonExcludedRegions();
      }


      function getNonExcludedRegions()
      {
         if($this->regions==NULL) $this->load();
         $non_excluded = array();
         foreach(array_keys($this->regions) as $r_id)
         {
            if(!in_array($r_id, $this->excluded))
            {
               $non_excluded[] = $this->regions[$r_id];
            }
         }
         return $non_excluded;
      }


      function getRegionByName($name)
      {
         if($this->regions==NULL) $this->load();
         foreach($this->regions as $r)
         {
            if (!strcasecmp($r->region_name,$name)) return $r;
         }
         $name=strtolower(str_replace(array("\\","/"," ","-","_"),"",$name));
         foreach($this->regions as $r)
         {
            if (!strcasecmp($r->getStrippedName(),$name)) return $r;
         }
         return NULL;
      }


      function getRegionByID($region_id)
      {
         if(is_array($region_id)) $region_id = array_shift($region_id);
         if($this->regions==NULL) $this->load();
         if(isset($this->regions[$region_id])) return $this->regions[$region_id];
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

      function dump($dir, $ids = null)
      {
          foreach($this->regions as $r)
          {
             if($ids === null or in_array($r->region_id, $ids))
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
          return true;
      }


   }
?>
