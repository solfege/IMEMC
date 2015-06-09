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

   Class Type
   {
      var $type_id;
      var $type_name;
      var $active;
      var $excluded;

      function Type()
      {
      }
      function getName()
      {
         return $this->type_name;
      }

      function save()
      {
         global $prefix, $dbconn;
         if($this->type_id == NULL)
         {
            checkForError(sql_query("INSERT INTO ".$prefix."_types values ('$this->type_id', '$this->type_name', '$this->active', '$this->excluded')", $dbconn, 1));
            $this->type_id = mysql_insert_id($dbconn);
         }
         else
         {
            checkForError(sql_query("UPDATE ".$prefix."_types SET type_name = '$this->type_name', active = '$this->active', excluded = '$this->excluded' WHERE type_id = $this->type_id", $dbconn, 1));
         }
      }

      function load()
      {
         if($this->type_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("SELECT type_name, active, excluded from ".$prefix."_types where type_id=$this->type_id", $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($this->type_name, $this->active, $this->excluded) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Type with ID $this->type_id exists");
            }
         }
         else
         {
            reportError("The type id is null");
         }
      }

      function delete()
      {
         global $typeList;
         if($this->type_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("DELETE FROM ".$prefix."_types where type_id=$this->type_id", $dbconn, 1);
            checkForError($result);
            $typeList->reset();
         }
         else
         {
            reportError("The type id is null");
         }
      }


      function getStoryCount()
      {
         $count=0;
         if($this->type_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("SELECT count(*) from ".$prefix."_stories where type_id=$this->type_id", $dbconn, 2);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
              list($count) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Type with ID $this->type_id exists");
            }
        }
        else
      {
         reportError("The type id is null");
        }
        return $count;
     }

     function getStrippedName()
     {
        return strtolower(str_replace(array("\\","/"," ","-","_"),"",$this->type_name));
     }
   }

   Class TypeList
   {
      var $types;
      var $excluded;

      function load()
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT type_id, type_name, active, excluded from ".$prefix."_types", $dbconn, 1);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
           $this->types = array();
            $this->excluded = array();
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {
               $t = new Type();
               list($t->type_id, $t->type_name, $t->active, $t->excluded) = sql_fetch_row($result, $dbconn);
               $this->types[$t->type_id] = $t;
            }
         }
         else
         {
            reportError("No Types present in database");
         }
      }

      function getTypes()
      {
         if($this->types==NULL) $this->load();
         return array_values($this->types);
      }

      function getExcludedTypes()
      {
         if($this->types==NULL) $this->load();
         $excluded = array();
         foreach($this->excluded as $e)
         {
             $excluded[] = $this->types[$e];
         }
         return $excluded;
      }

      function getfTypes()
      {
         global $editor_session;
         if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewprotectedcategories")) return $this->getTypes();
         return $this->getNonExcludedTypes();
      }

      function getNonExcludedTypes()
      {
         if($this->types==NULL) $this->load();
         $non_excluded = array();

         foreach(array_keys($this->types) as $t_id)
         {

            if(!in_array($t_id, $this->excluded))
            {
               $non_excluded[] = $this->types[$t_id];
            }
         }
         return $non_excluded;
      }

      function getTypeByName($name)
      {
         if($this->types==NULL) $this->load();
         foreach($this->types as $t)
         {
            if (!strcasecmp($t->type_name,$name)) return $t;
         }
         // exact match failed so try inexact
         $name=strtolower(str_replace(array("\\","/"," ","-","_"),"",$name));
         foreach($this->types as $t)
         {
            if (!strcasecmp($t->getStrippedName(),$name)) return $t;
         }
         return NULL;
      }

      function getTypeByID($type_id)
      {
         if($this->types==NULL) $this->load();
         if(is_array($type_id)) $type_id = array_shift($type_id);
         if(isset($this->types[$type_id])) return $this->types[$type_id];
         return NULL;
      }

      function reset()
      {
         $this->types = NULL;
      }

      function dump($dir, $ids = null)
      {
          foreach($this->types as $t)
          {
             if($ids === null or in_array($t->type_id, $ids))
             {
                $file = $dir."/".$t->getStrippedName().".dump";
                $fp = fopen($file,"w");
                if (flock($fp, LOCK_EX))
                {
                   fputs($fp, serialize($t));
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
