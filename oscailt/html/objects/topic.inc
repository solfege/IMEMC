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
      var $language_id;

      function Topic()
      {
         $this->excluded = 0;
      }

      function getName()
      {
         return $this->topic_name;
      }

      function setDefaultLanguage()
      {
         global $system_config;
	 // Need to convert to an id still
         $this->language_id = $system_config->default_language_code;
	 // Should set the above or the min which may not be 1.
         $this->language_id = 1;
      }

      function save()
      {
         global $prefix, $dbconn;
         if($this->topic_id == NULL)
         {
            checkForError(sql_query("INSERT INTO ".$prefix."_topics values (null,'$this->topic_name',".$this->active.", ".$this->excluded.",".$this->language_id.")", $dbconn, 1));
	    $this->topic_id = mysql_insert_id($dbconn);
         }
         else
         {
            $result = sql_query("SELECT topic_name from ".$prefix."_topics where topic_id=".$this->topic_id." AND language_id=".$this->language_id, $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
	    {
               checkForError(sql_query("UPDATE ".$prefix."_topics SET topic_name = '$this->topic_name', active =".$this->active.", excluded =".$this->excluded." WHERE topic_id =".$this->topic_id." AND language_id=".$this->language_id, $dbconn, 1));
	    } else {
               checkForError(sql_query("INSERT INTO ".$prefix."_topics values (".$this->topic_id.", '$this->topic_name', ".$this->active.",".$this->excluded.",".$this->language_id.")", $dbconn, 1));
	       $this->topic_id = mysql_insert_id($dbconn);
	    }
         }
      }

      function load($language_id=NULL)
      {
         if ($this->topic_id != NULL)
         {
            if ($language_id != NULL) $this->language_id = $language_id;

            global $prefix, $dbconn;
            $result = sql_query("SELECT topic_name, active, excluded, language_id from ".$prefix."_topics where topic_id=".$this->topic_id." AND language_id=".$this->language_id, $dbconn, 1);
            checkForError($result);
            if(sql_num_rows( $result ) > 0)
            {
               list($this->topic_name, $this->active, $this->excluded, $this->language_id) = sql_fetch_row($result, $dbconn);
            }
            else
            {
               reportError("No Topic with ID $this->topic_id and Lang Id $this->language_id exists");
            }
         }
         else
         {
            reportError("The topic id is null");
         }
      }

      // Deletes topic for all languages.
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
      var $max_topic;

      function load()
      {
         global $prefix, $dbconn;
         $result = sql_query("SELECT topic_id, topic_name, active, excluded, language_id from ".$prefix."_topics order by topic_id", $dbconn, 1);
         checkForError($result);
         if(sql_num_rows( $result ) > 0)
         {
            $this->topics = array();
            $this->excluded = array();
            $this->max_topic = 0;
            for ($i=0; $i < sql_num_rows( $result ); $i++)
            {
               $t = new Topic();
               list($t->topic_id, $t->topic_name, $t->active, $t->excluded, $t->language_id) = sql_fetch_row($result, $dbconn);
               $this->topics[$t->language_id][$t->topic_id] = $t;
               if($t->excluded) $this->excluded[] = $t->topic_id;
               if($t->topic_id > $this->max_topic) $this->max_topic = $t->topic_id;
            }
         }
         else
         {
            reportError("No Topics present in database");
         }
      }

      function getMaxTopicId()
      {
         if($this->topics==NULL) $this->load();
         return $this->max_topic;
      }

      function getTopics($lang_id)
      {
         if($this->topics==NULL) $this->load();

	 foreach ($this->topics as $language_id => $topics_array)
	 {
             if ($language_id == $lang_id) return array_values($topics_array);
	 }
	 return null;
      }

      //filters the topics according to permissions
      function getfTopics($lang_id)
      {
         global $editor_session;
         if($editor_session->isSessionOpen() && $editor_session->editor->allowedReadAccessTo("viewprotectedcategories")) return $this->getTopics($lang_id);
         return $this->getNonExcludedTopics($lang_id);
      }

      function getNonExcludedTopics($lang_id)
      {
         if($this->topics==NULL) $this->load();
         $non_excluded = array();

         foreach($this->topics as $language_id => $sub_array)
	 {
             if ($language_id == $lang_id)
             {
		 foreach (array_keys($sub_array) as $t_id) {
                    if(!in_array($t_id, $this->excluded))
                    {
                        $non_excluded[] = $this->topics[$language_id][$t_id];
                    }
                 }
             }
         }
         return $non_excluded;
      }

      function getExcludedTopics($lang_id=1)
      {
         if($this->topics==NULL) $this->load();
         $excluded = array();

         foreach($this->excluded as $e)
         {
            $excluded[] = $this->topics[$lang_id][$e];
         }
         return $excluded;
      }

      // A topic may not be active
      function getActiveTopics($lang_id)
      {
         if($this->topics==NULL) $this->load();
         $activeArray = array();

         foreach($this->topics as $language_id => $sub_array)
	 {
             if ($language_id == $lang_id ) 
	     {
                foreach($subarray as $topic_id => $t)
                {
                    if ($t->active ) $activeArray[] = $t->topic_id;
                }
             }
         }
         return $activeArray;
      }

      function getTopicByName($name,$lang_id=1)
      {
         if($this->topics==NULL) $this->load();

         foreach($this->topics as $language_id => $sub_array)
	 {
             if ($language_id == $lang_id ) 
	     {
                 foreach($sub_array as $topic_id => $t)
                 {
                     if (!strcasecmp($t->topic_name,$name)) return $t;
                 }
             }
         }

         $name=strtolower(str_replace(array("\\","/"," ","-","_"),"",$name));

         foreach($this->topics as $language_id => $sub_array)
	 {
             if ($language_id == $lang_id ) 
	     {
                 foreach($sub_array as $topic_id => $t)
                 {
                     if (!strcasecmp($t->getStrippedName(),$name)) return $t;
                 }
             }
         }
         return NULL;
      }

      function getTopicByAnyLangName($name)
      {
         if($this->topics==NULL) $this->load();

         foreach($this->topics as $language_id => $sub_array)
	 {
            foreach($sub_array as $topic_id => $t)
            {
                if (!strcasecmp($t->topic_name,$name)) return $t;
            }
         }

         $name=strtolower(str_replace(array("\\","/"," ","-","_"),"",$name));

         foreach($this->topics as $language_id => $sub_array)
	 {
             foreach($sub_array as $topic_id => $t)
             {
                 if (!strcasecmp($t->getStrippedName(),$name)) return $t;
             }
         }
         return NULL;
      }

      // Get all the language versions of a given topic id.
      function getAllTopicLanguagesByID($topic_id)
      {
         if($this->topics==NULL) $this->load();

         $topic_languages = array();

         foreach($this->topics as $language_id => $sub_array)
	 {
             foreach($sub_array as $t_topic_id => $t)
             {
                 if ($t_topic_id != $topic_id ) continue;

                 $topic_languages[] = $this->topics[$language_id][$topic_id];
             }
         }
         return $topic_languages;
      }

      function getTopicByID($topic_id,$lang_id=1)
      {
         if(is_array($topic_id)) $topic_id = array_shift($topic_id);

         if($this->topics==NULL) $this->load();

         if(isset($this->topics[$lang_id][$topic_id])) return $this->topics[$lang_id][$topic_id];
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

      function dump($dir, $t_ids_to_dump = null, $lang_ids_to_dump=null)
      {
          foreach($this->topics as $language_id => $topics_array)
	  {
            foreach($topics_array as $t_key => $t)
            {
               if($lang_ids_to_dump === null or in_array($language_id, $lang_ids_to_dump))
               {
                  if($t_ids_to_dump === null or in_array($t->topic_id, $t_ids_to_dump))
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
            }
          }
          return true;
      }

   }
?>
