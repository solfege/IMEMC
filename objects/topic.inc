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
   Class Topic
   {
      var $topic_id;
      var $topic_name;
      var $active;
      var $excluded;

      function Topic()
      {
      }

      function getName()
      {
         return $this->topic_name;
      }

      function save()
      {
         global $prefix, $dbconn;
         if($this->topic_id == NULL)
         {
            checkForError(sql_query("INSERT INTO ".$prefix."_topics values ('$this->topic_id', '$this->topic_name', '$this->active', '$this->excluded')", $dbconn, 1));
	    $this->topic_id = mysql_insert_id($dbconn);
         }
         else
         {
            checkForError(sql_query("UPDATE ".$prefix."_topics SET topic_name = '$this->topic_name', active = '$this->active', excluded = '$this->excluded' WHERE topic_id = $this->topic_id", $dbconn, 1));
         }
      }

      function load()
      {
         if($this->topic_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("SELECT topic_name, active, excluded from ".$prefix."_topics where topic_id=$this->topic_id", $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($this->topic_name, $this->active, $this->excluded) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Topic with ID $this->topic_id exists");
            }
         }
         else
         {
            reportError("The topic id is null");
         }
      }

      function delete()
      {
         global $topicList;
         if($this->topic_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("DELETE FROM ".$prefix."_topics where topic_id=$this->topic_id", $dbconn, 1);
            checkForError($result);
            $topicList->reset();
         }
         else
         {
            reportError("The topic id is null");
         }
      }

      function getStoryCount()
      {
         $count=0;
         if($this->topic_id != NULL)
         {
            global $prefix, $dbconn;
            $result = sql_query("SELECT count(*) from ".$prefix."_stories where topic_id=$this->topic_id", $dbconn, 2);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($count) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Topic with ID $this->topic_id exists");
            }
         }
         else
         {
            reportError("The topic id is null");
         }
         return $count;
      }

      function getStrippedName()
      {
         return strtolower(str_replace(array("\\","/"," ","-","_"),"",$this->topic_name));
      }
   }

   Class TopicList
   {
      var $topics;

      var $excluded;

      function load()
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT topic_id, topic_name, active, excluded from ".$prefix."_topics", $dbconn, 1);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            $this->topics = array();

            $this->excluded = array();
            for ($i=0; $i<sql_num_rows( $result ); $i++)
            {

               $t = new Topic();
               list($t->topic_id, $t->topic_name, $t->active, $t->excluded) = sql_fetch_row($result, $dbconn);
               $this->topics[$t->topic_id] = $t;
               if($t->excluded) $this->excluded[] = $t->topic_id;
            }
         }
         else
         {
            reportError("No Topics present in database");
         }
      }

      function getTopics()
      {
         if($this->topics==NULL) $this->load();

         return array_values($this->topics);
      }

      //filters the topics according to permissions
      function getfTopics()
      {
         global $editor_session;
         if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewprotectedcategories")) return $this->getTopics();
         return $this->getNonExcludedTopics();
      }

      function getNonExcludedTopics()
      {
         if($this->topics==NULL) $this->load();
         $non_excluded = array();

         foreach(array_keys($this->topics) as $t_id)
         {
             if(!in_array($t_id, $this->excluded))
             {
                 $non_excluded[] = $this->topics[$t_id];
             }
         }
         return $non_excluded;
      }

      function getExcludedTopics()
      {
         if($this->topics==NULL) $this->load();
         $excluded = array();

         foreach($this->excluded as $e)
         {
            $excluded[] = $this->topics[$e];
         }
         return $excluded;
      }

      // A topic may not be active
      function getActiveTopics()
      {
         if($this->topics==NULL) $this->load();
         $activeArray = array();

         foreach($this->topics as $t)
         {
            if ($t->active ) $activeArray[] = $t->topic_id;
         }
         return $activeArray;
      }

      function getTopicByName($name)
      {
         if($this->topics==NULL) $this->load();

         foreach($this->topics as $t)
         {
            if (!strcasecmp($t->topic_name,$name)) return $t;
         }

         $name=strtolower(str_replace(array("\\","/"," ","-","_"),"",$name));

         foreach($this->topics as $t)
         {
            if (!strcasecmp($t->getStrippedName(),$name)) return $t;
         }
         return NULL;
      }

      function getTopicByID($topic_id)
      {
         if(is_array($topic_id)) $topic_id = array_shift($topic_id);

         if($this->topics==NULL) $this->load();

         if(isset($this->topics[$topic_id])) return $this->topics[$topic_id];
         return NULL;
      }

      function reset()
      {
         $this->topics = NULL;
      }
      function getcount()
      {
         return count($this->topics);
      }

      function dump($dir, $ids = null)
      {
          foreach($this->topics as $t)
          {
             if($ids === null or in_array($t->topic_id, $ids))
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
